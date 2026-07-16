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
    std::vector<Core::FMTOutput> SES::filterOutputs(
        const std::vector<Core::FMTOutput>& allOutputs,
        const std::vector<std::string>& selectedNames)
    {
        std::vector<Core::FMTOutput> filtered;

        for (const std::string& name : selectedNames)
        {
            for (const Core::FMTOutput& output : allOutputs)
            {
                if (output.getName() == name)
                {
                    filtered.push_back(output);
                    break;
                }
            }
        }

        return filtered;
    }

    std::vector<Core::FMTTheme> SES::buildGrowthThemes(
        const std::vector<Core::FMTTheme>& allThemes,
        const std::vector<int>& themeIndices)
    {
        std::vector<Core::FMTTheme> growthThemes;

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

    std::vector<Core::FMTConstraint> SES::filterConstraints(
        const std::vector<Core::FMTConstraint>& allConstraints,
        const std::vector<std::string>& selectedNames)
    {
        std::vector<Core::FMTConstraint> selectedConstraints;

        for (const std::string& name : selectedNames)
        {
            for (const Core::FMTConstraint& constraint : allConstraints)
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

    void SES::applySingleTransitions(Models::FMTModel& model)
    {
        std::vector<Core::FMTTransition> singleTransitions;
        for (const Core::FMTTransition& transition : model.getTransitions())
        {
            singleTransitions.push_back(transition.single());
        }
        model.setTransitions(singleTransitions);
    }

    void SES::prepareInitialForest(
        Models::FMTSeModel& model,
        const std::string& rastersPath,
        bool useStanlock,
        std::string& ageRasterPath,
        std::vector<std::string>& themeRasterPaths)
    {
        ageRasterPath = rastersPath + "AGE.tif";

        themeRasterPaths.clear();
        for (size_t i = 1; i <= model.getThemes().size(); ++i)
        {
            themeRasterPaths.push_back(rastersPath + "THEME" + std::to_string(i) + ".tif");
        }

        Parser::FMTAreaParser areaparser;
        Spatial::FMTforest initialForest;
        if (!useStanlock)
        {
            initialForest = areaparser.readRasters(
                model.getThemes(),
                themeRasterPaths,
                ageRasterPath,
                1,
                0.0001);
        }
        else
        {
            const std::string stanlockPath = rastersPath + "STANLOCK.tif";
            initialForest = areaparser.readRasters(
                model.getThemes(),
                themeRasterPaths,
                ageRasterPath,
                1,
                0.0001,
                stanlockPath);
        }

        model.setInitialMapping(initialForest);
    }

    EventsData SES::writeEventsFile(
        const Models::FMTSeModel& semodel,
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
        const Models::FMTModel& baseModel,
        const std::vector<Core::FMTSchedule>& schedules)
    {
        SESResults results;
        
        Models::FMTSesModel simulationModel(baseModel);

        if (!params.constraintNames.empty())
        {
            simulationModel.setConstraints(
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

        simulationModel.setParameter(Models::FMTintmodelparameters::LENGTH, params.numberOfPeriods);
        simulationModel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, params.greedySearchIterations);
        simulationModel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
        simulationModel.setParameter(Models::FMTboolmodelparameters::POSTSOLVE, true);

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

    std::vector<std::string> SES::generateInfeasibilityReport(const Models::FMTSeModel& semodel)
    {
        std::vector<std::string> messages;

        try
        {
            // Journalise le rapport (via le logger du modèle) puis reconstruit
            // les mêmes messages pour les retourner à l'appelant.
            semodel.logConstraintsInfeasibilities();

            const std::vector<Core::FMTConstraint> constraints = semodel.getconstraints();
            double brokenup = 0;
            double total = 0;

            // L'indice 0 correspond à l'objectif : on commence à 1 comme dans
            // FMTSeModel::logConstraintsInfeasibilities().
            for (size_t cid = 1; cid < constraints.size(); ++cid)
            {
                double value = semodel.getConstraintEvaluation(cid);
                if (value > 0)
                {
                    const Core::FMTConstraint& constraint = constraints.at(cid);
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
            Exception::FMTFreeExceptionHandler().raiseFromCatch(e.what(), "SES::generateInfeasibilityReport", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch("Unknown error", "SES::generateInfeasibilityReport", __LINE__, __FILE__);
        }

        return messages;
    }

    SAResults SES::RunOptimization(
        const SAParameters& params,
        const Models::FMTModel& baseModel)
    {
        SAResults results;

        Models::FMTSaModel optimizationModel(baseModel);

        if (!params.constraintNames.empty())
        {
            optimizationModel.setConstraints(
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

        optimizationModel.setParameter(Models::FMTintmodelparameters::LENGTH, params.numberOfPeriods);
        optimizationModel.setParameter(Models::FMTintmodelparameters::MAX_MOVES, params.maxMoves);
        optimizationModel.setParameter(Models::FMTintmodelparameters::MAX_ACCEPTED_CYCLE_MOVES, params.maxAcceptedMoves);
        optimizationModel.setParameter(Models::FMTintmodelparameters::MAX_CYCLE_MOVES, params.maxCycleMoves);

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
        const Models::FMTSeModel& semodel,
        const int numberOfPeriods,
        const std::vector<Core::FMTSchedule>& schedules)
    {
        CarbonReportData reportData;

        try
        {
            Models::FMTSeModel localmodel(semodel);
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();
            const std::vector<Core::FMTSchedule> newSchedule = semodel.getSchedules(schedule, false);
            size_t scid = 0;

            for (int period = 1; period <= numberOfPeriods; ++period)
            {
                CarbonReportData::PeriodData periodData;
                periodData.period = period;

                std::vector<Core::FMTConstraint> periodicconstraints = semodel.getconstraints();
                for (Core::FMTConstraint& periodconstraint : periodicconstraints)
                {
                    const int lowerperiod = periodconstraint.getPeriodLowerBound();
                    const int upperperiod = std::min(period, periodconstraint.getPeriodUpperBound());
                    periodconstraint.setLength(lowerperiod, upperperiod);
                }
                localmodel.setConstraints(periodicconstraints);

                double primalinf = 0;
                double objectivevalue = 0;
                localmodel.getSolutionStatus(schedule, objectivevalue, primalinf, true, false);
                periodData.objectiveValue = objectivevalue;
                periodData.primalInfeasibility = primalinf;

                double oldtotal = 0;
                double newtotal = 0;

                size_t oriloc = 0;
                for (const Core::FMTSchedule& schedule : schedules)
                {
                    if (schedule.getPeriod() == period)
                    {
                        break;
                    }
                    ++oriloc;
                }

                size_t newloc = 0;
                for (const Core::FMTSchedule& schedule : newSchedule)
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
                        periodData.actionRatios[data.first.getName()] = ratio;
                    }

                    periodData.totalRatio = (oldtotal > 0) ? (newtotal / oldtotal) : 0.0;
                }
                reportData.periods.push_back(periodData);
                ++scid;
            }
        }
        catch (std::exception& e)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(e.what(), "SES::generateCarbonReport", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch("Unknown error", "SES::generateCarbonReport", __LINE__, __FILE__);
        }

        return reportData;
    }

    std::vector<std::string> SES::writeDisturbances(
        const Models::FMTSeModel& semodel,
        const std::string& outputBasePath,
        const int numberOfPeriods,
        const std::vector<int>& growthThemeIndices)
    {
        std::vector<std::string> transitionFiles;

        try
        {
            const std::vector<Core::FMTTheme> growthThemes = buildGrowthThemes(
                semodel.getThemes(),
                growthThemeIndices);
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();
            const std::vector<Core::FMTAction> actions = semodel.getactions();

            Parser::FMTTransitionParser transitionparser;
            Parser::FMTAreaParser areaparser;

            for (int period = 1; period <= numberOfPeriods; ++period)
            {
                const std::vector<Core::FMTGCBMTransition> transitions =
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
            Exception::FMTFreeExceptionHandler().raiseFromCatch(e.what(), "SES::writeDisturbances", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch("", "SES::writeDisturbances", __LINE__, __FILE__);
        }

        return transitionFiles;
    }

    EventsData SES::generateEventsData(const Models::FMTSeModel& semodel)
    {
        EventsData eventsData;

        try
        {
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();
            const std::vector<Core::FMTAction> actions = semodel.getactions();
            eventsData.statistics = schedule.getPatchStats(actions);
        }
        catch (std::exception& e)
        {
            eventsData.statistics = "";
            Exception::FMTFreeExceptionHandler().raiseFromCatch(e.what(), "SES::generateEventsData", __LINE__, __FILE__);
        }
        catch (...)
        {
            eventsData.statistics = "";
            Exception::FMTFreeExceptionHandler().raiseFromCatch("Unknown error", "SES::generateEventsData", __LINE__, __FILE__);
        }

        return eventsData;
    }

    OutputsData SES::calculateOutputs(
        const Models::FMTSeModel& semodel,
        const std::vector<std::string>& outputNames,
        const int numberOfPeriods)
    {
        OutputsData outputsData;

        try
        {
            const std::vector<Core::FMTOutput> allOutputs = semodel.getOutputs();
            std::vector<Core::FMTOutput> selectedOutputs = filterOutputs(allOutputs, outputNames);

            outputsData.outputObjects = selectedOutputs;

            for (const Core::FMTOutput& output : selectedOutputs)
            {
                OutputsData::OutputResult result;
                result.outputName = output.getName();

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
            Exception::FMTFreeExceptionHandler().raiseFromCatch(e.what(), "SES::calculateOutputs", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch("Unknown error", "SES::calculateOutputs", __LINE__, __FILE__);
        }

        return outputsData;
    }

    std::vector<std::string> SES::writeSpatialOutputs(
        const Models::FMTSeModel& semodel,
        const std::vector<Core::FMTOutput>& outputs,
        const int minPeriod,
        const int maxPeriod,
        const std::string& outputPath)
    {
        std::vector<std::string> rasterFiles;

        try
        {
            Parser::FMTAreaParser areaparser;

            for (int period = minPeriod; period <= maxPeriod; ++period)
            {
                for (const Core::FMTOutput& output : outputs)
                {
                    const std::string outputname = output.getName() + "_" + std::to_string(period) + ".tif";
                    const std::string outputrasterpath = outputPath + outputname;

                    const Spatial::FMTlayer<double> outputlayer = semodel.getSpatialOutput(output, period);
                    areaparser.writeLayer(outputlayer, outputrasterpath);

                    rasterFiles.push_back(outputrasterpath);
                }
            }
        }
        catch (std::exception& e)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(e.what(), "SES::writeSpatialOutputs", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch("Unknown error", "SES::writeSpatialOutputs", __LINE__, __FILE__);
        }

        return rasterFiles;
    }

    PredictorsData SES::calculatePredictors(
        const Models::FMTSeModel& semodel,
        const std::string& rasterPath,
        const int numberOfPeriods,
        const std::vector<std::string>& predictorYields)
    {
        PredictorsData predictorsData;

        try
        {
            Parser::FMTAreaParser areaparser;
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
            Exception::FMTFreeExceptionHandler().raiseFromCatch(e.what(), "SES::calculatePredictors", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch("Unknown error", "SES::calculatePredictors", __LINE__, __FILE__);
        }

        return predictorsData;
    }

    std::string SES::writeSchedule(
        const Models::FMTSeModel& semodel,
        const std::string& outputPath)
    {
        std::string schedulePath;

        try
        {
            Parser::FMTScheduleParser scheduparser;
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();
            const std::vector<Core::FMTSchedule> schedules = semodel.getSchedules(schedule);

            schedulePath = outputPath + semodel.getName() + "._seq";
            scheduparser.write(schedules, schedulePath);
        }
        catch (std::exception& e)
        {
            schedulePath = "";
            Exception::FMTFreeExceptionHandler().raiseFromCatch(e.what(), "SES::writeSchedule", __LINE__, __FILE__);
        }
        catch (...)
        {
            schedulePath = "";
            Exception::FMTFreeExceptionHandler().raiseFromCatch("Unknown error", "SES::writeSchedule", __LINE__, __FILE__);
        }

        return schedulePath;
    }

    void SES::writeUpdatedForest(
        const Models::FMTSeModel& semodel,
        const std::string& rasterPath,
        const std::vector<std::string>& themeRasterPaths,
        const std::string& ageRasterPath,
        const std::string& stanlockRasterPath)
    {
        try
        {
            Parser::FMTAreaParser areaparser;
            const Spatial::FMTSpatialSchedule& schedule = semodel.getSpSchedule();

            areaparser.writeForest(
                schedule.getForestPeriod(0),
                semodel.getThemes(),
                themeRasterPaths,
                ageRasterPath,
                stanlockRasterPath);
        }
        catch (std::exception& e)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch(e.what(), "SES::writeUpdatedForest", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch("Unknown error", "SES::writeUpdatedForest", __LINE__, __FILE__);
        }
    }

    void SES::exportResults(
        const Models::FMTSeModel& semodel,
        const std::vector<Core::FMTOutput>& outputs,
        const int minPeriod,
        const int maxPeriod,
        const std::string& outputPath,
        const int outputLevel,
        const std::string& gdalProvider)
    {
        try
        {
            Parser::FMTModelParser Modelparser;

            Modelparser.writeResults(
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
            Exception::FMTFreeExceptionHandler().raiseFromCatch(e.what(), "SES::exportResults", __LINE__, __FILE__);
        }
        catch (...)
        {
            Exception::FMTFreeExceptionHandler().raiseFromCatch("Unknown error", "SES::exportResults", __LINE__, __FILE__);
        }
    }
}