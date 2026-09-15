#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"

namespace Wrapper
{
    namespace {

        FMTWrapperCore::SAParameters ConvertirParametresOptimisation(
            System::String^ cheminRasters,
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
            FMTWrapperCore::SAParameters params;

            // Conversion des chemins. scenarioName reste vide : le Core journalise le nom
            // du scénario que le contrôleur a résolu.
            params.rastersPath = msclr::interop::marshal_as<std::string>(cheminRasters);
            params.outputPath = msclr::interop::marshal_as<std::string>(cheminSorties);
            params.gdalProvider = msclr::interop::marshal_as<std::string>(providerGdal);

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
            const FMTWrapperCore::SAParameters PARAMS = ConvertirParametresOptimisation(
                cheminRasters, contraintes, periodes,
                p_MaxMoves, p_MaxAcceptedMoves, p_MaxCycleMoves,
                outputs, indicateurStanlock, outputLevel,
                etanduSortiesMin, etanduSortiesMax, cheminSorties,
                indGenererEvents, indSortiesSpatiales, providerGdal);

            const FMTWrapperCore::SAResults RESULTS =
                FMTWrapperCore::Controller::runSpatialOptimization(PARAMS, scenario);

            return RESULTS.success;
        }
        catch (...)
        {
            _raiseFromCatch("", "FMTForm::OptimisationSpatialeExplicite", __LINE__, __FILE__);
            return false;
        }
    }
}
