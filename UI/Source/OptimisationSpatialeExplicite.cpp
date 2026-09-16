#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"
#include "Conversions.h"

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

            // Path conversion. scenarioName stays empty: the Core logs the name
            // of the scenario the controller resolved.
            params.rastersPath = Conversions::toStdString(cheminRasters);
            params.outputPath = Conversions::toStdString(cheminSorties);
            params.gdalProvider = Conversions::toStdString(providerGdal);

            // Numeric parameters
            params.numberOfPeriods = periodes;
            params.maxMoves = p_MaxMoves;
            params.maxAcceptedMoves = p_MaxAcceptedMoves;
            params.maxCycleMoves = p_MaxCycleMoves;
            params.outputLevel = outputLevel;
            params.outputMinPeriod = etanduSortiesMin;
            params.outputMaxPeriod = etanduSortiesMax;

            // Boolean options
            params.useStanlock = indicateurStanlock;
            params.generateEvents = indGenererEvents;
            params.generateSpatialOutputs = indSortiesSpatiales;

            // C# to C++ list conversions
            params.constraintNames = Conversions::toStdVector(contraintes);
            params.outputNames = Conversions::toStdVector(outputs);

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
