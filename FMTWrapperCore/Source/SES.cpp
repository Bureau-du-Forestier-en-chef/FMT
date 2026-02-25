#include "SES.h"
#include "FMTsesmodel.h"
#include "FMTsamodel.h"
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTareaparser.h"
#include "FMTscheduleparser.h"
#include "FMTtransitionparser.h"
#include "FMTGCBMtransition.h"
#include "FMToutputnode.h"
#include "FMTSpatialSchedule.h"
#include "FMTforest.h"
#include "FMTschedule.h"
#include "FMTconstraint.h"
#include "FMToutput.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTmodel.h"
#include <sstream>
#include <fstream>

namespace FMTWrapperCore
{
    std::vector<Core::FMToutput> SES::FilterOutputs(
        const std::vector<Core::FMToutput>& allOutputs,
        const std::vector<std::string>& selectedNames)
    {
        std::vector<Core::FMToutput> filtered;

        for (const std::string& name : selectedNames)
        {
            for (const Core::FMToutput& output : allOutputs)
            {
                if (output.getname() == name)
                {
                    filtered.push_back(output);
                    break;
                }
            }
        }

        return filtered;
    }

    std::vector<Core::FMTtheme> SES::BuildGrowthThemes(
        const std::vector<Core::FMTtheme>& allThemes,
        const std::vector<int>& themeIndices)
    {
        std::vector<Core::FMTtheme> growthThemes;

        if (!themeIndices.empty())
        {
            for (int themeID : themeIndices)
            {
                // Les indices sont 1-based dans l'interface
                if (themeID > 0 && static_cast<size_t>(themeID) <= allThemes.size())
                {
                    growthThemes.push_back(allThemes.at(themeID - 1));
                }
            }
        }

        return growthThemes;
    }

    SESResults SES::RunSES(
        const SESParameters& params,
        const Models::FMTmodel& baseModel,
        const std::vector<Core::FMTschedule>& schedules)
    {
        SESResults results;

        try
        {
            Models::FMTsesmodel simulationModel(baseModel);

            if (!params.constraintNames.empty())
            {
                std::vector<Core::FMTconstraint> allConstraints = simulationModel.getconstraints();
                std::vector<Core::FMTconstraint> selectedConstraints;

                for (const std::string& name : params.constraintNames)
                {
                    for (const Core::FMTconstraint& constraint : allConstraints)
                    {
                        if (constraint.getname() == name)
                        {
                            selectedConstraints.push_back(constraint);
                            break;
                        }
                    }
                }

                simulationModel.setconstraints(selectedConstraints);
            }

            std::vector<Core::FMTtransition> singleTransitions;
            for (const auto& transition : simulationModel.gettransitions())
            {
                singleTransitions.push_back(transition.single());
            }
            simulationModel.settransitions(singleTransitions);

            Parser::FMTareaparser areaparser;
            const std::string ageRasterPath = params.rastersPath + "AGE.tif";

            std::vector<std::string> themeRasterPaths;
            for (size_t i = 1; i <= simulationModel.getthemes().size(); ++i)
            {
                themeRasterPaths.push_back(params.rastersPath + "THEME" + std::to_string(i) + ".tif");
            }

            Spatial::FMTforest initialForest;
            if (!params.useStanlock)
            {
                initialForest = areaparser.readrasters(
                    simulationModel.getthemes(),
                    themeRasterPaths,
                    ageRasterPath,
                    1,
                    0.0001);
            }
            else
            {
                const std::string stanlockPath = params.rastersPath + "STANLOCK.tif";
                initialForest = areaparser.readrasters(
                    simulationModel.getthemes(),
                    themeRasterPaths,
                    ageRasterPath,
                    1,
                    0.0001,
                    stanlockPath);
            }

            simulationModel.setinitialmapping(initialForest);

            if (schedules.empty())
            {
                results.errorMessage = "No schedules provided";
                return results;
            }

            if (schedules.back().getperiod() < params.numberOfPeriods)
            {
                results.errorMessage = "Dépassement de la période : size " +
                    std::to_string(schedules.size()) + " periode " +
                    std::to_string((schedules.back().getperiod() + 1));
                return results;
            }

            simulationModel.setparameter(Models::FMTintmodelparameters::LENGTH, params.numberOfPeriods);
            simulationModel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, params.greedySearchIterations);
            simulationModel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
            simulationModel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, true);

            simulationModel.doplanning(false, schedules);

            std::string outputDirectory = params.carbonMode ? params.rastersPath : params.outputPath;

            results.infeasibilityMessages = GenerateInfeasibilityReport(simulationModel);

            results.carbonReport = GenerateCarbonReport(
                simulationModel,
                params.numberOfPeriods,
                schedules);

            results.disturbanceFiles = WriteDisturbances(
                simulationModel,
                outputDirectory,
                params.numberOfPeriods,
                params.growthThemes);

            if (params.generateEvents || params.carbonMode)
            {
                results.eventsData = GenerateEventsData(simulationModel);

                // Écrire le fichier d'événements
                results.eventsFilePath = outputDirectory + "/events.txt";
                std::ofstream eventsFile(results.eventsFilePath);
                if (eventsFile.is_open())
                {
                    eventsFile << results.eventsData.statistics;
                    eventsFile.close();
                }
            }

            if (!params.outputNames.empty())
            {
                results.outputsData = CalculateOutputs(
                    simulationModel,
                    params.outputNames,
                    params.numberOfPeriods);

                results.scheduleFilePath = WriteSchedule(simulationModel, outputDirectory);

                if (!params.carbonMode)
                {
                    ExportResults(
                        simulationModel,
                        results.outputsData.outputObjects,
                        params.outputMinPeriod,
                        params.outputMaxPeriod,
                        params.outputPath,
                        params.outputLevel,
                        params.gdalProvider);
                }
                else
                {
                    WriteUpdatedForest(
                        simulationModel,
                        params.rastersPath,
                        themeRasterPaths,
                        ageRasterPath,
                        params.rastersPath + "STANLOCK.tif");

                    if (!params.predictorYields.empty())
                    {
                        results.predictorsData = CalculatePredictors(
                            simulationModel,
                            params.rastersPath,
                            params.numberOfPeriods,
                            params.predictorYields);
                    }
                }

                if (params.generateSpatialOutputs)
                {
                    results.spatialOutputFiles = WriteSpatialOutputs(
                        simulationModel,
                        results.outputsData.outputObjects,
                        params.outputMinPeriod,
                        params.outputMaxPeriod,
                        outputDirectory);
                }
            }

            results.success = true;
        }
        catch (const std::exception& e)
        {
            results.success = false;
            results.errorMessage = std::string("Exception: ") + e.what();
        }
        catch (...)
        {
            results.success = false;
            results.errorMessage = "Unknown error occurred during simulation";
        }

        return results;
    }

    SESResults SES::RunSES(
        const SESParameters& params,
        const std::vector<Core::FMTschedule>& schedules)
    {
        SESResults results;

        try
        {
            Parser::FMTmodelparser parser;
            std::vector<Models::FMTmodel> models = parser.readproject(params.primaryFilePath);

            if (params.scenarioIndex < 0 || params.scenarioIndex >= static_cast<int>(models.size()))
            {
                results.success = false;
                results.errorMessage = "Invalid scenario index: " + std::to_string(params.scenarioIndex);
                return results;
            }

            return RunSES(params, models.at(params.scenarioIndex), schedules);
        }
        catch (const std::exception& e)
        {
            results.success = false;
            results.errorMessage = std::string("Error loading model: ") + e.what();
            return results;
        }
        catch (...)
        {
            results.success = false;
            results.errorMessage = "Unknown error while loading model";
            return results;
        }
    }

    std::vector<std::string> SES::GenerateInfeasibilityReport(const Models::FMTsemodel& semodel)
    {
        std::vector<std::string> messages;

        try
        {
            // La méthode LogConstraintsInfeasibilities écrit directement dans le logger
            // On la laisse comme elle est pour ne rien casser
            semodel.LogConstraintsInfeasibilities();
        }
        catch (...)
        {
            messages.push_back("Error generating infeasibility report");
        }

        return messages;
    }

    SAResults SES::RunOptimization(
        const SAParameters& params,
        const Models::FMTmodel& baseModel)
    {
        SAResults results;

        try
        {
            Models::FMTsamodel optimizationModel(baseModel);

            if (!params.constraintNames.empty())
            {
                std::vector<Core::FMTconstraint> allConstraints = optimizationModel.getconstraints();
                std::vector<Core::FMTconstraint> selectedConstraints;

                for (const std::string& name : params.constraintNames)
                {
                    for (const Core::FMTconstraint& constraint : allConstraints)
                    {
                        if (constraint.getname() == name)
                        {
                            selectedConstraints.push_back(constraint);
                            break;
                        }
                    }
                }

                optimizationModel.setconstraints(selectedConstraints);
            }

            std::vector<Core::FMTtransition> modifiedTransitions;
            for (const Core::FMTtransition& transition : optimizationModel.gettransitions())
            {
                modifiedTransitions.push_back(transition.single());
            }
            optimizationModel.settransitions(modifiedTransitions);

            Parser::FMTareaparser areaparser;
            const std::string ageRasterPath = params.rastersPath + "AGE.tif";

            std::vector<std::string> themeRasterPaths;
            for (size_t i = 1; i <= optimizationModel.getthemes().size(); ++i)
            {
                themeRasterPaths.push_back(params.rastersPath + "THEME" + std::to_string(i) + ".tif");
            }

            Spatial::FMTforest initialForest;
            if (!params.useStanlock)
            {
                initialForest = areaparser.readrasters(
                    optimizationModel.getthemes(),
                    themeRasterPaths,
                    ageRasterPath,
                    1,
                    0.0001);
            }
            else
            {
                const std::string stanlockPath = params.rastersPath + "STANLOCK.tif";
                initialForest = areaparser.readrasters(
                    optimizationModel.getthemes(),
                    themeRasterPaths,
                    ageRasterPath,
                    1,
                    0.0001,
                    stanlockPath);
            }

            optimizationModel.setinitialmapping(initialForest);

            optimizationModel.setparameter(Models::FMTintmodelparameters::LENGTH, params.numberOfPeriods);
            optimizationModel.setparameter(Models::FMTintmodelparameters::MAX_MOVES, params.maxMoves);
            optimizationModel.setparameter(Models::FMTintmodelparameters::MAX_ACCEPTED_CYCLE_MOVES, params.maxAcceptedMoves);
            optimizationModel.setparameter(Models::FMTintmodelparameters::MAX_CYCLE_MOVES, params.maxCycleMoves);

            optimizationModel.doplanning(true);

            std::string outputDirectory = params.outputPath;

            results.infeasibilityMessages = GenerateInfeasibilityReport(optimizationModel);

            std::vector<int> emptyGrowthThemes;
            results.disturbanceFiles = WriteDisturbances(
                optimizationModel,
                outputDirectory,
                params.numberOfPeriods,
                emptyGrowthThemes);

            if (params.generateEvents)
            {
                results.eventsData = GenerateEventsData(optimizationModel);

                results.eventsFilePath = outputDirectory + "/events.txt";
                std::ofstream eventsFile(results.eventsFilePath);
                if (eventsFile.is_open())
                {
                    eventsFile << results.eventsData.statistics;
                    eventsFile.close();
                }
            }

            if (!params.outputNames.empty())
            {
                results.outputsData = CalculateOutputs(
                    optimizationModel,
                    params.outputNames,
                    params.numberOfPeriods);

                results.scheduleFilePath = WriteSchedule(optimizationModel, outputDirectory);

                ExportResults(
                    optimizationModel,
                    results.outputsData.outputObjects,
                    params.outputMinPeriod,
                    params.outputMaxPeriod,
                    params.outputPath,
                    params.outputLevel,
                    params.gdalProvider);

                if (params.generateSpatialOutputs)
                {
                    results.spatialOutputFiles = WriteSpatialOutputs(
                        optimizationModel,
                        results.outputsData.outputObjects,
                        params.outputMinPeriod,
                        params.outputMaxPeriod,
                        outputDirectory);
                }
            }

            results.success = true;
        }
        catch (const std::exception& e)
        {
            results.success = false;
            results.errorMessage = std::string("Exception: ") + e.what();
        }
        catch (...)
        {
            results.success = false;
            results.errorMessage = "Unknown error occurred during optimization";
        }

        return results;
    }

    CarbonReportData SES::GenerateCarbonReport(
        const Models::FMTsemodel& semodel,
        const int numberOfPeriods,
        const std::vector<Core::FMTschedule>& schedules)
    {
        CarbonReportData reportData;

        try
        {
            Models::FMTsemodel localmodel(semodel);
            const Spatial::FMTSpatialSchedule& schedule = semodel.getspschedule();
            const std::vector<Core::FMTschedule> newschedule = semodel.GetSchedules(schedule, false);

            size_t scid = 0;

            for (int period = 1; period <= numberOfPeriods; ++period)
            {
                CarbonReportData::PeriodData periodData;
                periodData.period = period;

                std::vector<Core::FMTconstraint> periodicconstraints = semodel.getconstraints();
                for (Core::FMTconstraint& periodconstraint : periodicconstraints)
                {
                    const int lowerperiod = periodconstraint.getperiodlowerbound();
                    const int upperperiod = std::min(period, periodconstraint.getperiodupperbound());
                    periodconstraint.setlength(lowerperiod, upperperiod);
                }
                localmodel.setconstraints(periodicconstraints);

                double primalinf = 0;
                double objectivevalue = 0;
                localmodel.GetSolutionStatus(schedule, objectivevalue, primalinf, true, false);

                periodData.objectiveValue = objectivevalue;
                periodData.primalInfeasibility = primalinf;

                double oldtotal = 0;
                double newtotal = 0;

                size_t oriloc = 0;
                for (const Core::FMTschedule& schedule : schedules)
                {
                    if (schedule.getperiod() == period)
                    {
                        break;
                    }
                    ++oriloc;
                }

                size_t newloc = 0;
                for (const Core::FMTschedule& schedule : newschedule)
                {
                    if (schedule.getperiod() == period)
                    {
                        break;
                    }
                    ++newloc;
                }

                if (scid < newschedule.size() && scid < schedules.size())
                {
                    for (const auto& data : schedules.at(oriloc))
                    {
                        const double basearea = schedules.at(oriloc).actionarea(data.first);
                        double newarea = 0;

                        if (newschedule.at(newloc).find(data.first) != newschedule.at(newloc).end())
                        {
                            newarea = newschedule.at(newloc).actionarea(data.first);
                        }

                        oldtotal += basearea;
                        newtotal += newarea;

                        double ratio = (basearea > 0) ? (newarea / basearea) : 0.0;
                        periodData.actionRatios[data.first.getname()] = ratio;
                    }

                    periodData.totalRatio = (oldtotal > 0) ? (newtotal / oldtotal) : 0.0;
                }

                reportData.periods.push_back(periodData);
                ++scid;
            }
        }
        catch (...)
        {
            // En cas d'erreur, retourner les données partielles
        }

        return reportData;
    }

    std::vector<std::string> SES::WriteDisturbances(
        const Models::FMTsemodel& semodel,
        const std::string& outputBasePath,
        const int numberOfPeriods,
        const std::vector<int>& growthThemeIndices)
    {
        std::vector<std::string> transitionFiles;

        try
        {
            const std::vector<Core::FMTtheme> growthThemes = BuildGrowthThemes(
                semodel.getthemes(),
                growthThemeIndices);

            const Spatial::FMTSpatialSchedule& schedule = semodel.getspschedule();
            const std::vector<Core::FMTaction> actions = semodel.getactions();

            Parser::FMTtransitionparser transitionparser;
            Parser::FMTareaparser areaparser;

            for (int period = 1; period <= numberOfPeriods; ++period)
            {
                const std::vector<Core::FMTGCBMtransition> transitions =
                    areaparser.writedisturbances(
                        outputBasePath,
                        schedule,
                        actions,
                        growthThemes,
                        period);

                std::string fichier = outputBasePath + "transition" + std::to_string(period) + ".txt";
                transitionparser.writeGCBM(transitions, fichier);

                transitionFiles.push_back(fichier);
            }
        }
        catch (...)
        {
            // En cas d'erreur, retourner les fichiers partiellement créés
        }

        return transitionFiles;
    }

    EventsData SES::GenerateEventsData(const Models::FMTsemodel& semodel)
    {
        EventsData eventsData;

        try
        {
            const Spatial::FMTSpatialSchedule& schedule = semodel.getspschedule();
            const std::vector<Core::FMTaction> actions = semodel.getactions();

            eventsData.statistics = schedule.getpatchstats(actions);
        }
        catch (...)
        {
            eventsData.statistics = "";
        }

        return eventsData;
    }

    OutputsData SES::CalculateOutputs(
        const Models::FMTsemodel& semodel,
        const std::vector<std::string>& outputNames,
        const int numberOfPeriods)
    {
        OutputsData outputsData;

        try
        {
            const std::vector<Core::FMToutput> allOutputs = semodel.getoutputs();
            std::vector<Core::FMToutput> selectedOutputs = FilterOutputs(allOutputs, outputNames);

            outputsData.outputObjects = selectedOutputs;

            for (const Core::FMToutput& output : selectedOutputs)
            {
                OutputsData::OutputResult result;
                result.outputName = output.getname();

                for (int period = 1; period <= numberOfPeriods; ++period)
                {
                    const std::map<std::string, double> OUTS = semodel.getoutput(
                        output,
                        period,
                        Core::FMToutputlevel::totalonly);

                    if (OUTS.find("Total") != OUTS.end())
                    {
                        result.periodValues[period] = OUTS.at("Total");
                    }
                }

                outputsData.results.push_back(result);
            }
        }
        catch (...)
        {
            // En cas d'erreur, retourner les données partielles
        }

        return outputsData;
    }

    std::vector<std::string> SES::WriteSpatialOutputs(
        const Models::FMTsemodel& semodel,
        const std::vector<Core::FMToutput>& outputs,
        const int minPeriod,
        const int maxPeriod,
        const std::string& outputPath)
    {
        std::vector<std::string> rasterFiles;

        try
        {
            Parser::FMTareaparser areaparser;

            for (int period = minPeriod; period <= maxPeriod; ++period)
            {
                for (const Core::FMToutput& output : outputs)
                {
                    const std::string outputname = output.getname() + "_" + std::to_string(period) + ".tif";
                    const std::string outputrasterpath = outputPath + "/" + outputname;

                    const Spatial::FMTlayer<double> outputlayer = semodel.getspatialoutput(output, period);
                    areaparser.writelayer(outputlayer, outputrasterpath);

                    rasterFiles.push_back(outputrasterpath);
                }
            }
        }
        catch (...)
        {
            // En cas d'erreur, retourner les fichiers partiellement créés
        }

        return rasterFiles;
    }

    PredictorsData SES::CalculatePredictors(
        const Models::FMTsemodel& semodel,
        const std::string& rasterPath,
        const int numberOfPeriods,
        const std::vector<std::string>& predictorYields)
    {
        PredictorsData predictorsData;

        try
        {
            Parser::FMTareaparser areaparser;
            const Spatial::FMTSpatialSchedule& schedule = semodel.getspschedule();

            for (size_t period = 1; period <= static_cast<size_t>(numberOfPeriods); ++period)
            {
                std::vector<std::vector<Graph::FMTpredictor>> predictors =
                    areaparser.writepredictors(
                        rasterPath,
                        schedule,
                        predictorYields,
                        semodel,
                        period);

                // Extraire les noms des prédicteurs (seulement une fois)
                if (predictorsData.predictorNames.empty() && !predictors.empty() && !predictors.back().empty())
                {
                    predictorsData.predictorNames = predictors.back().back().getpredictornames(predictorYields);
                }

                // Extraire les valeurs pour chaque nœud
                size_t indexPredictors = 0;
                for (const auto& predictorslist : predictors)
                {
                    if (!predictorslist.empty())
                    {
                        PredictorsData::PredictorNode node;
                        node.period = period;
                        node.nodeIndex = indexPredictors;
                        node.values = predictorslist.back().getpredictors();

                        predictorsData.nodes.push_back(node);
                    }
                    ++indexPredictors;
                }
            }
        }
        catch (...)
        {
            // En cas d'erreur, retourner les données partielles
        }

        return predictorsData;
    }

    std::string SES::WriteSchedule(
        const Models::FMTsemodel& semodel,
        const std::string& outputPath)
    {
        std::string schedulePath;

        try
        {
            Parser::FMTscheduleparser scheduparser;
            const Spatial::FMTSpatialSchedule& schedule = semodel.getspschedule();
            const std::vector<Core::FMTschedule> schedules = semodel.GetSchedules(schedule);

            schedulePath = outputPath + "/" + semodel.getname() + "_.seq";
            scheduparser.write(schedules, schedulePath);
        }
        catch (...)
        {
            schedulePath = "";
        }

        return schedulePath;
    }

    void SES::WriteUpdatedForest(
        const Models::FMTsemodel& semodel,
        const std::string& rasterPath,
        const std::vector<std::string>& themeRasterPaths,
        const std::string& ageRasterPath,
        const std::string& stanlockRasterPath)
    {
        try
        {
            Parser::FMTareaparser areaparser;
            const Spatial::FMTSpatialSchedule& schedule = semodel.getspschedule();

            areaparser.writeforest(
                schedule.getforestperiod(0),
                semodel.getthemes(),
                themeRasterPaths,
                ageRasterPath,
                stanlockRasterPath);
        }
        catch (...)
        {
            // L'erreur sera gérée par le wrapper
        }
    }

    void SES::ExportResults(
        const Models::FMTsemodel& semodel,
        const std::vector<Core::FMToutput>& outputs,
        const int minPeriod,
        const int maxPeriod,
        const std::string& outputPath,
        const int outputLevel,
        const std::string& gdalProvider)
    {
        try
        {
            Parser::FMTmodelparser Modelparser;

            Modelparser.writeresults(
                semodel,
                outputs,
                minPeriod,
                maxPeriod,
                outputPath,
                static_cast<Core::FMToutputlevel>(outputLevel),
                gdalProvider);
        }
        catch (...)
        {
            // L'erreur sera gérée par le wrapper
        }
    }

} // namespace FMTWrapperCore