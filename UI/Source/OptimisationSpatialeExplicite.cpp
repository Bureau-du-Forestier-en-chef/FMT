#include "stdafx.h"
#include <sstream>
#include "FMTforest.h"
#include "FMTmodelparser.h"
#include "FMTsamodel.h"
#include "FMTareaparser.h"
#include "FMTscheduleparser.h"
#include <msclr\marshal_cppstd.h>
#include "FMTFormLogger.h"
#include "FMTForm.h"
#include "FMTmodel.h"
#include "FMTFormCache.h"
#include "FMTdefaultlogger.h"
#include "SES.h"

namespace Wrapper
{
    namespace {

        FMTWrapperCore::SAParameters ConvertirParametresOptimisation(
            System::String^ cheminRasters,
            int scenario,
            System::Collections::Generic::List<System::String^>^ contraintes,
            int periodes,
            int p_MaxMoves,
            int p_MaxAcceptedMoves,
            int p_MaxCycleMoves,
            System::Collections::Generic::List<System::String^>^ outputs,
            bool indicateurStanlock,
            int outputLevel,
            int etanduSortiesMin,
            int etanduSortiesMax,
            System::String^ cheminSorties,
            bool indGenererEvents,
            bool indSortiesSpatiales,
            System::String^ providerGdal,
            const std::string& scenarioName) 
        {
            FMTWrapperCore::SAParameters params;

            // Conversion des chemins
            params.rastersPath = msclr::interop::marshal_as<std::string>(cheminRasters);
            params.outputPath = msclr::interop::marshal_as<std::string>(cheminSorties);
            params.gdalProvider = msclr::interop::marshal_as<std::string>(providerGdal);
            params.scenarioName = scenarioName;

            // Paramètres numériques
            params.numberOfPeriods = periodes;
            params.maxMoves = p_MaxMoves;
            params.maxAcceptedMoves = p_MaxAcceptedMoves;
            params.maxCycleMoves = p_MaxCycleMoves;
            params.outputLevel = outputLevel;
            params.outputMinPeriod = etanduSortiesMin;
            params.outputMaxPeriod = etanduSortiesMax;

            // Options booléennes
            params.useStanlock = indicateurStanlock;
            params.generateEvents = indGenererEvents;
            params.generateSpatialOutputs = indSortiesSpatiales;

            // Conversion des listes C# → C++
            for each (System::String ^ constraint in contraintes)
            {
                params.constraintNames.push_back(msclr::interop::marshal_as<std::string>(constraint));
            }

            for each (System::String ^ output in outputs)
            {
                params.outputNames.push_back(msclr::interop::marshal_as<std::string>(output));
            }

            return params;
        }

        void EnvoyerResultatsOptimisation(
            const FMTWrapperCore::SAResults& results,
            FMTFormLogger* logger)
        {
            // Logging des outputs
            for (const auto& result : results.outputsData.results)
            {
                for (const auto& periodValue : result.periodValues)
                {
                    *logger << "outputs;" + result.outputName + ";" + std::to_string(periodValue.second) << "\n";
                }
            }
        }

    }

    bool FMTForm::OptimisationSpatialeExplicite(
        System::String^ fichierPri,
        System::String^ cheminRasters,
        int scenario,
        System::Collections::Generic::List<System::String^>^ contraintes,
        int periodes,
        int p_MaxMoves,
        int p_MaxAcceptedMoves,
        int p_MaxCycleMoves,
        System::Collections::Generic::List<System::String^>^ outputs,
        bool indicateurStanlock,
        int outputLevel,
        int etanduSortiesMin,
        int etanduSortiesMax,
        System::String^ cheminSorties,
        bool indGenererEvents,
        bool indSortiesSpatiales,
        System::String^ providerGdal)
    {
        try
        {
            FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
            *logger << Logging::FMTdefaultlogger().getlogstamp() << "\n";

            Models::FMTmodel baseModel = FMTFormCache::GetInstance()->getmodel(scenario);
            const std::string scenarioName = baseModel.getname();

            FMTWrapperCore::SAParameters params = ConvertirParametresOptimisation(
                cheminRasters, scenario, contraintes, periodes,
                p_MaxMoves, p_MaxAcceptedMoves, p_MaxCycleMoves,
                outputs, indicateurStanlock, outputLevel,
                etanduSortiesMin, etanduSortiesMax, cheminSorties,
                indGenererEvents, indSortiesSpatiales, providerGdal,
                scenarioName); 

            *logger << "FMT -> Traitement pour le scénario : " + scenarioName << "\n";

            *logger << "FMT -> Démarrage de l'optimisation" << "\n";

            FMTWrapperCore::SAResults results =
                FMTWrapperCore::SES::RunOptimization(params, baseModel);

            if (!results.success)
            {
                *logger << "FMT -> Erreur d'optimisation: " + results.errorMessage << "\n";
                return false;
            }

            *logger << "FMT -> Optimisation terminée avec succès" << "\n";

            *logger << "FMT -> Exportations des sorties " << "\n";
            EnvoyerResultatsOptimisation(results, logger);

            return true;
        }
        catch (...)
        {
            raisefromcatch("", "FMTForm::OptimisationSpatialeExplicite", __LINE__, __FILE__);
            return false;
        }
    }

}