#include "OperatingArea.h"

#include <algorithm>
#include <string>
#include <vector>

#include "FMTAction.h"
#include "FMTAreaParser.h"
#include "FMTDefaultLogger.h"
#include "FMTLogger.h"
#include "FMTLpModel.h"
#include "FMTMask.h"
#include "FMTModelParameters.h"
#include "FMTOpAreaSchedulerTask.h"
#include "FMTOperatingAreaScheduler.h"
#include "FMTOutput.h"
#include "FMTOutputNode.h"
#include "FMTTaskHandler.h"
#include "FMTTheme.h"
#include "ModelQuery.h"
#include "Selection.h"

namespace FMTWrapperCore
{
    namespace
    {
        // Reserved aggregate the scheduling adds to the actions that use YOUVERT.
        const std::string YOUVERT_AGGREGATE = "~BFECOPTOUTPUTYOUVERT~";
        // Yield marking the actions that open an operating area.
        const std::string YOUVERT_YIELD = "YOUVERT";
    }

    OperatingAreaResults OperatingArea::schedule(
        const OperatingAreaParameters& p_params,
        const Models::FMTModel& p_model)
    {
        OperatingAreaResults results;

        try
        {
            // FMT static logger: in the interface, it is the CallbackLogger the wrapper used
            // directly; in a test, the default logger.
            Logging::FMTLogger& logger = *Models::FMTModel::getLogger();

            logger << Logging::FMTDefaultLogger().getLogStamp() << "\n";
            logger << "Préparation du modèle" << "\n";

            Models::FMTLpModel optimizationModel(
                p_model,
                static_cast<Models::FMTSolverInterface>(p_params.solver));

            logger << "FMT -> Traitement pour le scénario : " + optimizationModel.getName() << "\n";

            optimizationModel.setParameter(Models::FMTintmodelparameters::LENGTH, p_params.numberOfPeriods);
            optimizationModel.setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
            optimizationModel.setParameter(Models::FMTintmodelparameters::UPDATE, p_params.updatePeriod);

            const int STARTING_PERIOD =
                optimizationModel.getParameter(Models::FMTintmodelparameters::UPDATE);

            // The reserved aggregate designates the YOUVERT actions in the output node of the task.
            std::vector<Core::FMTAction> actions = optimizationModel.getActions();
            int youvertActions = 0;

            for (Core::FMTAction& action : actions)
            {
                if (action.useYield(YOUVERT_YIELD))
                {
                    ++youvertActions;

                    const std::vector<std::string> AGGREGATES = action.getAggregates();

                    if (std::count(AGGREGATES.begin(), AGGREGATES.end(), YOUVERT_AGGREGATE))
                    {
                        results.errorMessage =
                            "L'utilisateur à utiliser le nom ~BFECOPTOUTPUTYOUVERT~ dans ses outputs.";
                        logger << results.errorMessage << "\n";
                        return results;
                    }

                    action.pushAggregate(YOUVERT_AGGREGATE);
                }
            }

            if (youvertActions < 1)
            {
                results.errorMessage = "Aucune action dans le modèle n'a de yield youvert";
                logger << results.errorMessage << "\n";
                return results;
            }

            optimizationModel.setActions(actions);

            const std::vector<Core::FMTTheme> THEMES = optimizationModel.getThemes();
            const Core::FMTOutputNode OUTPUT_NODE(ModelQuery::getFullMask(THEMES), YOUVERT_AGGREGATE);

            Parser::FMTAreaParser areaParser;

            const std::vector<Heuristics::FMTOperatingAreaScheme> OPERATING_AREAS =
                areaParser.getOperatingArea(
                    p_params.vectorFilePath,
                    THEMES,
                    p_params.themeNumber,
                    STARTING_PERIOD,
                    p_params.ageField,
                    p_params.areaField,
                    p_params.lockField,
                    p_params.parametersFilePath);

            logger << "Résolution du modèle" << "\n";

            Parallel::FMTOpAreaSchedulerTask mainTask(
                optimizationModel,
                OPERATING_AREAS,
                OUTPUT_NODE,
                p_params.resultFolder + "\\Retour",
                YOUVERT_YIELD,
                p_params.numberOfIterations,
                p_params.maximumTime,
                Selection::findOutput(optimizationModel.getOutputs(), p_params.returnTimeOutputName));

            Parallel::FMTTaskHandler handler(mainTask, p_params.numberOfThreads);

            logger << "Génération du calendrier de COS" << "\n";

            handler.conccurentRun();

            results.success = true;
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "OperatingArea::schedule", __LINE__, __FILE__);
        }

        return results;
    }
}
