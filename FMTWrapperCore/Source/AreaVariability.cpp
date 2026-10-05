#include "AreaVariability.h"

#include <cstdlib>
#include <string>
#include <vector>

#include "FMTConstraint.h"
#include "FMTDefaultLogger.h"
#include "FMTFreeExceptionHandler.h"
#include "FMTLogger.h"
#include "FMTLpModel.h"
#include "FMTMask.h"
#include "FMTModelParser.h"
#include "FMTOutput.h"
#include "FMTTheme.h"
#include "Selection.h"

namespace FMTWrapper::Backend
{
    namespace
    {
        std::string trim(const std::string& p_text)
        {
            const char* WHITESPACE = " \t\r\n\f\v";
            const size_t FIRST = p_text.find_first_not_of(WHITESPACE);

            if (FIRST == std::string::npos)
            {
                return std::string();
            }

            const size_t LAST = p_text.find_last_not_of(WHITESPACE);

            return p_text.substr(FIRST, LAST - FIRST + 1);
        }
    }

    AreaVariabilityResults AreaVariability::run(
        const AreaVariabilityParameters& p_params,
        const Models::FMTModel& p_model)
    {
        AreaVariabilityResults results;

        try
        {
            // FMT static logger: in the interface, it is the CallbackLogger the wrapper used
            // directly; in a test, the default logger.
            Logging::FMTLogger& logger = *Models::FMTModel::getLogger();

            logger << Logging::FMTDefaultLogger().getLogStamp() << "\n";

            Models::FMTLpModel optimizationModel(
                p_model,
                static_cast<Models::FMTSolverInterface>(p_params.solver));

            logger << "FMT -> Traitement pour le scénario : " + optimizationModel.getName() << "\n";
            logger << "FMT Event Spatialy Explicit Simulation c++ - > Intégration des contraintes sélectionnées" << "\n";

            optimizationModel.setConstraints(
                Selection::selectConstraints(optimizationModel.getConstraints(), p_params.constraintNames));

            for (int period = 0; period < p_params.numberOfPeriods; ++period)
            {
                optimizationModel.buildPeriod();
            }

            if (!p_params.proportionsTableProvided)
            {
                results.errorMessage =
                    "FMT - Arrêt du traitement. Le fichier de paramètres .csv est obligatoire.";
                logger << results.errorMessage << "\n";
                return results;
            }

            if (p_params.proportionsTable.size() <= 1)
            {
                results.errorMessage =
                    "FMTErreur - Arrêt du traitement. Le fichier de paramètres .csv est vide.";
                logger << results.errorMessage << "\n";
                return results;
            }

            const std::vector<Core::FMTTheme> THEMES = optimizationModel.getThemes();
            std::vector<Core::FMTMask> maskTargets;
            std::vector<double> proportions;

            // The first row of the table is a header.
            for (size_t row = 1; row < p_params.proportionsTable.size(); ++row)
            {
                const std::vector<std::string>& VALUES = p_params.proportionsTable.at(row);

                if (VALUES.empty())
                {
                    Exception::FMTFreeExceptionHandler().raise(
                        Exception::FMTexc::FMTfunctionfailed,
                        "Ligne " + std::to_string(row + 1) + " vide dans la table des proportions",
                        "AreaVariability::run", __LINE__, __FILE__);
                }

                const std::string& PROPORTION = VALUES.back();
                std::string mask;

                // Historical behavior kept as is, the bug being tracked in an issue: any column
                // whose value equals the value of the last column is dropped from the mask,
                // not only the last column.
                for (const std::string& VALUE : VALUES)
                {
                    if (VALUE != PROPORTION)
                    {
                        mask += VALUE + " ";
                    }
                }

                proportions.push_back(std::atof(PROPORTION.c_str()));
                maskTargets.push_back(Core::FMTMask(trim(mask), THEMES));
            }

            // A development can belong to a single mask of the table only.
            // The former code called exit(-1) here, which closed the whole process.
            for (const Core::FMTActualDevelopment& DEVELOPMENT : optimizationModel.getArea())
            {
                int matches = 0;

                for (const Core::FMTMask& TARGET : maskTargets)
                {
                    if (DEVELOPMENT.getMask().isSubsetOf(TARGET))
                    {
                        ++matches;
                    }
                }

                if (matches > 1)
                {
                    Exception::FMTFreeExceptionHandler().raise(
                        Exception::FMTexc::FMTfunctionfailed,
                        "Intersecting globalmask : le développement " +
                            std::string(DEVELOPMENT.getMask()) + " relève de " +
                            std::to_string(matches) + " masques de la table des proportions",
                        "AreaVariability::run", __LINE__, __FILE__);
                }
            }

            if (p_params.outputNames.empty())
            {
                results.errorMessage = "FMT - Modèle non réalisable";
                logger << results.errorMessage << "\n";
                return results;
            }

            const std::vector<Core::FMTOutput> OUTPUTS =
                Selection::selectOutputs(optimizationModel.getOutputs(), p_params.outputNames);

            logger << "FMT - Démarrage de Initial Area Variability" << "\n";

            Parser::FMTModelParser modelParser;
            modelParser.writeResults(
                optimizationModel.getModelFromProportions(maskTargets, proportions),
                OUTPUTS,
                p_params.outputMinPeriod,
                p_params.outputMaxPeriod,
                p_params.outputPath,
                static_cast<Core::FMToutputlevel>(p_params.outputLevel),
                p_params.gdalProvider);

            logger << "FMT - Initial Area Variability complété." << "\n";

            results.success = true;
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "AreaVariability::run", __LINE__, __FILE__);
        }

        return results;
    }
}
