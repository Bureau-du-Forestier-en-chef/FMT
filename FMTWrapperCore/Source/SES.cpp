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
#include "FMTfreeexceptionhandler.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace FMTWrapperCore
{
    std::vector<Core::FMToutput> SES::filterOutputs(
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

    std::vector<Core::FMTtheme> SES::buildGrowthThemes(
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

    std::vector<Core::FMTconstraint> SES::filterConstraints(
        const std::vector<Core::FMTconstraint>& allConstraints,
        const std::vector<std::string>& selectedNames)
    {
        std::vector<Core::FMTconstraint> selectedConstraints;

        for (const std::string& name : selectedNames)
        {
            for (const Core::FMTconstraint& constraint : allConstraints)
            {
                if (std::string(constraint) == name)
                {
                    selectedConstraints.push_back(constraint);
                    break;
                }
            }
        }

        return selectedConstraints;
    }

    void SES::applySingleTransitions(Models::FMTmodel& model)
    {
        std::vector<Core::FMTtransition> singleTransitions;
        for (const Core::FMTtransition& transition : model.getTransitions())
        {
            singleTransitions.push_back(transition.single());
        }
        model.setTransitions(singleTransitions);
    }

    void SES::prepareInitialForest(
        Models::FMTsemodel& model,
        const std::string& rastersPath,
        bool useStanlock,
        std::string& ageRasterPath,
        std::vector<std::string>& themeRasterPaths)
    {
        ageRasterPath = rastersPath + "AGE.tif";

        themeRasterPaths.clear();
        for (size_t i = 1; i <= model.getthemes().size(); ++i)
        {
            themeRasterPaths.push_back(rastersPath + "THEME" + std::to_string(i) + ".tif");
        }

        Parser::FMTareaparser areaparser;
        Spatial::FMTforest initialForest;
        if (!useStanlock)
        {
            initialForest = areaparser.readRasters(
                model.getthemes(),
                themeRasterPaths,
                ageRasterPath,
                1,
                0.0001);
        }
        else
        {
            const std::string stanlockPath = rastersPath + "STANLOCK.tif";
            initialForest = areaparser.readRasters(
                model.getthemes(),
                themeRasterPaths,
                ageRasterPath,
                1,
                0.0001,
                stanlockPath);
        }

        model.setInitialMapping(initialForest);
    }

    EventsData SES::writeEventsFile(
        const Models::FMTsemodel& semodel,
        const std::string& eventsFilePath)
    {
        EventsData eventsData = generateEventsData(semodel);

        std::ofstream eventsFile(eventsFilePath);
        if (eventsFile.is_open())
        {
            eventsFile << eventsData.statistics;
            eventsFile.close();
        }

        return eventsData;
    }

    SESResults SES::RunSES(
        const SESParameters& params,
        const Models::FMTmodel& baseModel,
        const std::vector<Core::FMTschedule>& schedules)
    {
        SESResults results;
        
        Models::FMTsesmodel simulationModel(baseModel);

        if (!params.constraintNames.empty())
        {
            simulationModel.setconstraints(
                filterConstraints(simulationModel.getconstraints(), params.constraintNames));
        }

        applySingleTransitions(simulationModel);

        std::string ageRasterPath;
        std::vector<std::string> themeRasterPaths;
        prepareInitialForest(
            simulationModel,
            params.rastersPath,
            params.useStanlock,
            ageRasterPath,
            themeRasterPaths);

        if (schedules.empty())
        {
            results.errorMessage = "No schedules provided";
            return results;
        }

        if (schedules.back().getPeriod() < params.numberOfPeriods)
        {
            results.errorMessage = "Dépassement de la période : size " +
                std::to_string(schedules.size()) + " periode " +
                std::to_string((schedules.back().getPeriod() + 1));
            return results;
        }

        simulationModel.setparameter(Models::FMTintmodelparameters::LENGTH, params.numberOfPeriods);
        simulationModel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, params.greedySearchIterations);
        simulationModel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
        simulationModel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, true);

        simulationModel.doPlanning(false, schedules);

        std::string outputDirectory = params.carbonMode ? params.rastersPath : params.outputPath + "\\";

        results.infeasibilityMessages = generateInfeasibilityReport(simulationModel);

        results.carbonReport = generateCarbonReport(
            simulationModel,
            params.numberOfPeriods,
            schedules);

        results.disturbanceFiles = writeDisturbances(
            simulationModel,
            outputDirectory,
            params.numberOfPeriods,
            params.growthThemes);

        if (params.generateEvents || params.carbonMode)
        {
            results.eventsFilePath = outputDirectory + "events.txt";
            results.eventsData = writeEventsFile(simulationModel, results.eventsFilePath);
        }

        if (!params.outputNames.empty())
        {
            results.outputsData = calculateOutputs(
                simulationModel,
                params.outputNames,
                params.numberOfPeriods);

            results.scheduleFilePath = writeSchedule(simulationModel, outputDirectory);

            if (!params.carbonMode)
            {
                exportResults(
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
                writeUpdatedForest(
                    simulationModel,
                    params.rastersPath,
                    themeRasterPaths,
                    ageRasterPath,
                    params.rastersPath + "STANLOCK.tif");

                if (!params.predictorYields.empty())
                {
                    results.predictorsData = calculatePredictors(
                        simulationModel,
                        params.rastersPath,
                        params.numberOfPeriods,
                        params.predictorYields);
                }
            }

            if (params.generateSpatialOutputs)
            {
                results.spatialOutputFiles = writeSpatialOutputs(
                    simulationModel,
                    results.outputsData.outputObjects,
                    params.outputMinPeriod,
                    params.outputMaxPeriod,
                    outputDirectory);
            }
        }

        results.success = true;
        return results;
    }

    std::vector<std::string> SES::generateInfeasibilityReport(const Models::FMTsemodel& semodel)
    {
        std::vector<std::string> messages;

        try
        {
            // Journalise le rapport (via le logger du modèle) puis reconstruit
            // les mêmes messages pour les retourner à l'appelant.
            semodel.logConstraintsInfeasibilities();

            const std::vector<Core::FMTconstraint> constraints = semodel.getconstraints();
            double brokenup = 0;
            double total = 0;

            // L'indice 0 correspond à l'objectif : on commence à 1 comme dans
            // FMTsemodel::logConstraintsInfeasibilities().
            for (size_t cid = 1; cid < constraints.size(); ++cid)
            {
                double value = semodel.getConstraintEvaluation(cid);
                if (value > 0)
                {
                    const Core::FMTconstraint& constraint = constraints.at(cid);
                    if (constraint.isGoal())
                    {
                        double goalValue = 0;
                        std::string goalName;
                        constraint.getGoal(goalName, goalValue);
                        if (goalName == "_WEIGHT")
                        {
                            value /= goalValue;
                        }
                    }

                    std::string constraintName = std::string(constraint);
                    std::replace(constraintName.begin(), constraintName.end(), '\n', ' ');
                    constraintName += ("(" + std::to_string(static_cast<int>(value)) + ")");
                    messages.push_back(constraintName);
                    ++brokenup;
                }
                ++total;
            }

            const double ratio = (brokenup > 0) ? (brokenup / total) * 100 : 0;
            messages.push_back("Percentage of infeasible constraints " +
                std::to_string(static_cast<int>(ratio)) + " %");
        }
        catch (std::exception& e)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch(e.what(), "SES::generateInfeasibilityReport", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch("Unknown error", "SES::generateInfeasibilityReport", __LINE__, __FILE__);
        }

        return messages;
    }

    SAResults SES::RunOptimization(
        const SAParameters& params,
        const Models::FMTmodel& baseModel)
    {
        SAResults results;

        Models::FMTsamodel optimizationModel(baseModel);

        if (!params.constraintNames.empty())
        {
            optimizationModel.setconstraints(
                filterConstraints(optimizationModel.getconstraints(), params.constraintNames));
        }

        applySingleTransitions(optimizationModel);

        std::string ageRasterPath;
        std::vector<std::string> themeRasterPaths;
        prepareInitialForest(
            optimizationModel,
            params.rastersPath,
            params.useStanlock,
            ageRasterPath,
            themeRasterPaths);

        optimizationModel.setparameter(Models::FMTintmodelparameters::LENGTH, params.numberOfPeriods);
        optimizationModel.setparameter(Models::FMTintmodelparameters::MAX_MOVES, params.maxMoves);
        optimizationModel.setparameter(Models::FMTintmodelparameters::MAX_ACCEPTED_CYCLE_MOVES, params.maxAcceptedMoves);
        optimizationModel.setparameter(Models::FMTintmodelparameters::MAX_CYCLE_MOVES, params.maxCycleMoves);

        optimizationModel.doPlanning(true);

        std::string outputDirectory = params.outputPath;

        results.infeasibilityMessages = generateInfeasibilityReport(optimizationModel);

        std::vector<int> emptyGrowthThemes;
        results.disturbanceFiles = writeDisturbances(
            optimizationModel,
            outputDirectory,
            params.numberOfPeriods,
            emptyGrowthThemes);

        if (params.generateEvents)
        {
            results.eventsFilePath = outputDirectory + "/events.txt";
            results.eventsData = writeEventsFile(optimizationModel, results.eventsFilePath);
        }

        if (!params.outputNames.empty())
        {
            results.outputsData = calculateOutputs(
                optimizationModel,
                params.outputNames,
                params.numberOfPeriods);

            results.scheduleFilePath = writeSchedule(optimizationModel, outputDirectory);

            exportResults(
                optimizationModel,
                results.outputsData.outputObjects,
                params.outputMinPeriod,
                params.outputMaxPeriod,
                params.outputPath,
                params.outputLevel,
                params.gdalProvider);

            if (params.generateSpatialOutputs)
            {
                results.spatialOutputFiles = writeSpatialOutputs(
                    optimizationModel,
                    results.outputsData.outputObjects,
                    params.outputMinPeriod,
                    params.outputMaxPeriod,
                    outputDirectory);
            }
        }

        results.success = true;
        return results;
    }

    CarbonReportData SES::generateCarbonReport(
        const Models::FMTsemodel& semodel,
        const int numberOfPeriods,
        const std::vector<Core::FMTschedule>& schedules)
    {
        CarbonReportData reportData;

        try
        {
            Models::FMTsemodel localmodel(semodel);
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();
            const std::vector<Core::FMTschedule> newSchedule = semodel.getSchedules(schedule, false);
            size_t scid = 0;

            for (int period = 1; period <= numberOfPeriods; ++period)
            {
                CarbonReportData::PeriodData periodData;
                periodData.period = period;

                std::vector<Core::FMTconstraint> periodicconstraints = semodel.getconstraints();
                for (Core::FMTconstraint& periodconstraint : periodicconstraints)
                {
                    const int lowerperiod = periodconstraint.getPeriodLowerBound();
                    const int upperperiod = std::min(period, periodconstraint.getPeriodUpperBound());
                    periodconstraint.setLength(lowerperiod, upperperiod);
                }
                localmodel.setconstraints(periodicconstraints);

                double primalinf = 0;
                double objectivevalue = 0;
                localmodel.getSolutionStatus(schedule, objectivevalue, primalinf, true, false);
                periodData.objectiveValue = objectivevalue;
                periodData.primalInfeasibility = primalinf;

                double oldtotal = 0;
                double newtotal = 0;

                size_t oriloc = 0;
                for (const Core::FMTschedule& schedule : schedules)
                {
                    if (schedule.getPeriod() == period)
                    {
                        break;
                    }
                    ++oriloc;
                }

                size_t newloc = 0;
                for (const Core::FMTschedule& schedule : newSchedule)
                {
                    if (schedule.getPeriod() == period)
                    {
                        break;
                    }
                    ++newloc;
                }

                if (scid < newSchedule.size() && scid < schedules.size())
                {
                    for (const auto& data : schedules.at(oriloc))
                    {
                        const double basearea = schedules.at(oriloc).actionArea(data.first);
                        double newarea = 0;

                        if (newSchedule.at(newloc).find(data.first) != newSchedule.at(newloc).end())
                        {
                            newarea = newSchedule.at(newloc).actionArea(data.first);
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
        catch (std::exception& e)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch(e.what(), "SES::generateCarbonReport", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch("Unknown error", "SES::generateCarbonReport", __LINE__, __FILE__);
        }

        return reportData;
    }

    std::vector<std::string> SES::writeDisturbances(
        const Models::FMTsemodel& semodel,
        const std::string& outputBasePath,
        const int numberOfPeriods,
        const std::vector<int>& growthThemeIndices)
    {
        std::vector<std::string> transitionFiles;

        try
        {
            const std::vector<Core::FMTtheme> growthThemes = buildGrowthThemes(
                semodel.getthemes(),
                growthThemeIndices);
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();
            const std::vector<Core::FMTaction> actions = semodel.getactions();

            Parser::FMTtransitionparser transitionparser;
            Parser::FMTareaparser areaparser;

            for (int period = 1; period <= numberOfPeriods; ++period)
            {
                const std::vector<Core::FMTGCBMtransition> transitions =
                    areaparser.writeDisturbances(
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
        catch (std::exception& e)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch(e.what(), "SES::writeDisturbances", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch("", "SES::writeDisturbances", __LINE__, __FILE__);
        }

        return transitionFiles;
    }

    EventsData SES::generateEventsData(const Models::FMTsemodel& semodel)
    {
        EventsData eventsData;

        try
        {
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();
            const std::vector<Core::FMTaction> actions = semodel.getactions();
            eventsData.statistics = schedule.getPatchStats(actions);
        }
        catch (std::exception& e)
        {
            eventsData.statistics = "";
            Exception::FMTfreeexceptionhandler().raisefromcatch(e.what(), "SES::generateEventsData", __LINE__, __FILE__);
        }
        catch (...)
        {
            eventsData.statistics = "";
            Exception::FMTfreeexceptionhandler().raisefromcatch("Unknown error", "SES::generateEventsData", __LINE__, __FILE__);
        }

        return eventsData;
    }

    OutputsData SES::calculateOutputs(
        const Models::FMTsemodel& semodel,
        const std::vector<std::string>& outputNames,
        const int numberOfPeriods)
    {
        OutputsData outputsData;

        try
        {
            const std::vector<Core::FMToutput> allOutputs = semodel.getoutputs();
            std::vector<Core::FMToutput> selectedOutputs = filterOutputs(allOutputs, outputNames);

            outputsData.outputObjects = selectedOutputs;

            for (const Core::FMToutput& output : selectedOutputs)
            {
                OutputsData::OutputResult result;
                result.outputName = output.getname();

                for (int period = 1; period <= numberOfPeriods; ++period)
                {
                    const std::map<std::string, double> OUTS = semodel.getOutput(
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
        catch (std::exception& e)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch(e.what(), "SES::calculateOutputs", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch("Unknown error", "SES::calculateOutputs", __LINE__, __FILE__);
        }

        return outputsData;
    }

    std::vector<std::string> SES::writeSpatialOutputs(
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
                    const std::string outputrasterpath = outputPath + outputname;

                    const Spatial::FMTlayer<double> outputlayer = semodel.getSpatialOutput(output, period);
                    areaparser.writeLayer(outputlayer, outputrasterpath);

                    rasterFiles.push_back(outputrasterpath);
                }
            }
        }
        catch (std::exception& e)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch(e.what(), "SES::writeSpatialOutputs", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch("Unknown error", "SES::writeSpatialOutputs", __LINE__, __FILE__);
        }

        return rasterFiles;
    }

    PredictorsData SES::calculatePredictors(
        const Models::FMTsemodel& semodel,
        const std::string& rasterPath,
        const int numberOfPeriods,
        const std::vector<std::string>& predictorYields)
    {
        PredictorsData predictorsData;

        try
        {
            Parser::FMTareaparser areaparser;
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();

            for (size_t period = 1; period <= static_cast<size_t>(numberOfPeriods); ++period)
            {
                std::vector<std::vector<Graph::FMTpredictor>> predictors =
                    areaparser.writePredictors(
                        rasterPath,
                        schedule,
                        predictorYields,
                        semodel,
                        period);

                if (predictorsData.predictorNames.empty() && !predictors.empty() && !predictors.back().empty())
                {
                    predictorsData.predictorNames = predictors.back().back().getPredictorNames(predictorYields);
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
                        node.values = predictorslist.back().getPredictors();

                        predictorsData.nodes.push_back(node);
                    }
                    ++indexPredictors;
                }
            }
        }
        catch (std::exception& e)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch(e.what(), "SES::calculatePredictors", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch("Unknown error", "SES::calculatePredictors", __LINE__, __FILE__);
        }

        return predictorsData;
    }

    std::string SES::writeSchedule(
        const Models::FMTsemodel& semodel,
        const std::string& outputPath)
    {
        std::string schedulePath;

        try
        {
            Parser::FMTscheduleparser scheduparser;
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();
            const std::vector<Core::FMTschedule> schedules = semodel.getSchedules(schedule);

            schedulePath = outputPath + semodel.getname() + "._seq";
            scheduparser.write(schedules, schedulePath);
        }
        catch (std::exception& e)
        {
            schedulePath = "";
            Exception::FMTfreeexceptionhandler().raisefromcatch(e.what(), "SES::writeSchedule", __LINE__, __FILE__);
        }
        catch (...)
        {
            schedulePath = "";
            Exception::FMTfreeexceptionhandler().raisefromcatch("Unknown error", "SES::writeSchedule", __LINE__, __FILE__);
        }

        return schedulePath;
    }

    void SES::writeUpdatedForest(
        const Models::FMTsemodel& semodel,
        const std::string& rasterPath,
        const std::vector<std::string>& themeRasterPaths,
        const std::string& ageRasterPath,
        const std::string& stanlockRasterPath)
    {
        try
        {
            Parser::FMTareaparser areaparser;
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();

            areaparser.writeForest(
                schedule.getForestPeriod(0),
                semodel.getthemes(),
                themeRasterPaths,
                ageRasterPath,
                stanlockRasterPath);
        }
        catch (std::exception& e)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch(e.what(), "SES::writeUpdatedForest", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch("Unknown error", "SES::writeUpdatedForest", __LINE__, __FILE__);
        }
    }

    void SES::exportResults(
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
                outputPath + "\\tempOutput",
                static_cast<Core::FMToutputlevel>(outputLevel),
                gdalProvider);

            std::filesystem::path csvDir = outputPath + "\\tempOutput";

            for (const auto& entry : std::filesystem::directory_iterator(csvDir)) {
                if (entry.is_regular_file()) {
                    std::filesystem::path destination = outputPath / entry.path().filename();
                    std::filesystem::rename(entry.path(), destination);
                }
            }

            std::filesystem::remove(csvDir);
        }
        catch (std::exception& e)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch(e.what(), "SES::exportResults", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTfreeexceptionhandler().raisefromcatch("Unknown error", "SES::exportResults", __LINE__, __FILE__);
        }
    }
}