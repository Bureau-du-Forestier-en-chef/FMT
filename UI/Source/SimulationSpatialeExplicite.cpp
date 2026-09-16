#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "FMTForm.h"
#include "Controller.h"
#include "Conversions.h"

namespace Wrapper
{
    namespace {

        FMTWrapperCore::SESParameters ConvertirParametres(
            System::String^ fichierPri,
            System::String^ cheminRasters,
            System::Collections::Generic::List<System::String^>^ contraintes,
            int periodes,
            int greedySearch,
            System::Collections::Generic::List<System::String^>^ outputs,
            bool indicateurStanlock,
            int outputLevel,
            int etanduSortiesMin,
            int etanduSortiesMax,
            System::String^ cheminSorties,
            bool indGenererEvents,
            bool indSortiesSpatiales,
            System::String^ providerGdal,
            bool indCarbon,
            System::Collections::Generic::List<System::String^>^ predictoryields,
            System::Collections::Generic::List<int>^ growththemes)
        {
            FMTWrapperCore::SESParameters params;

            // Path conversion. scenarioName stays empty: the Core logs the name
            // of the scenario the controller resolved.
            params.primaryFilePath = Conversions::toStdString(fichierPri);
            params.rastersPath = Conversions::toStdString(cheminRasters);
            params.outputPath = Conversions::toStdString(cheminSorties);
            params.gdalProvider = Conversions::toStdString(providerGdal);

            // Numeric parameters
            params.numberOfPeriods = periodes;
            params.greedySearchIterations = greedySearch;
            params.outputLevel = outputLevel;
            params.outputMinPeriod = etanduSortiesMin;
            params.outputMaxPeriod = etanduSortiesMax;

            // Boolean options
            params.useStanlock = indicateurStanlock;
            params.generateEvents = indGenererEvents;
            params.generateSpatialOutputs = indSortiesSpatiales;
            params.carbonMode = indCarbon;

            // C# to C++ list conversions
            params.constraintNames = Conversions::toStdVector(contraintes);
            params.outputNames = Conversions::toStdVector(outputs);
            params.predictorYields = Conversions::toStdVector(predictoryields);

            for each (int theme in growththemes)
            {
                params.growthThemes.push_back(theme);
            }

            return params;
        }

    }

    void FMTForm::_EnvoyerResultatsInterface(
        const FMTWrapperCore::SESResults& results,
        bool indCarbon)
    {
        for (const auto& periodData : results.carbonReport.periods)
        {
            int jsonloc = periodData.period - 1;

            RetourJson("objectives;" + jsonloc + ";Objective;" + periodData.objectiveValue,
                gcnew System::EventArgs());
            RetourJson("objectives;" + jsonloc + ";Primalinfeasibility;" + periodData.primalInfeasibility,
                gcnew System::EventArgs());

            for (const auto& actionRatio : periodData.actionRatios)
            {
                RetourJson("objectives;" + jsonloc + ";" +
                    gcnew System::String(actionRatio.first.c_str()) + ";" + actionRatio.second,
                    gcnew System::EventArgs());
            }

            RetourJson("objectives;" + jsonloc + ";Total;" + periodData.totalRatio,
                gcnew System::EventArgs());
        }

        // Disturbance files
        if (indCarbon)
        {
            for (const std::string& fichier : results.disturbanceFiles)
            {
                RetourJson(Conversions::fromUtf8("GCBMtransitionlocations;" + fichier), gcnew System::EventArgs());
            }
        }

        // Events file
        if (!results.eventsFilePath.empty() && indCarbon)
        {
            RetourJson(gcnew System::String(("eventslocation;" + results.eventsFilePath).c_str()),
                gcnew System::EventArgs());
        }

        // Outputs
        if (indCarbon && !results.outputsData.results.empty())
        {
            for (const auto& result : results.outputsData.results)
            {
                for (const auto& periodValue : result.periodValues)
                {
                    RetourJson("outputs;" + gcnew System::String(result.outputName.c_str()) +
                        ";" + periodValue.second, gcnew System::EventArgs());
                }
            }
        }

        // Schedule
        if (!results.scheduleFilePath.empty() && indCarbon)
        {
            RetourJson("schedules;" + gcnew System::String(results.scheduleFilePath.c_str()),
                gcnew System::EventArgs());
        }

        // Predictors
        if (indCarbon && !results.predictorsData.nodes.empty())
        {
            for (const auto& node : results.predictorsData.nodes)
            {
                int indexPeriode = node.period - 1;
                std::string outof = std::to_string(indexPeriode) + ";" + std::to_string(node.nodeIndex) + ";";

                for (const double& predval : node.values)
                {
                    outof += std::to_string(predval);
                    outof += ";";
                }

                outof.pop_back();
                RetourJson("allpredictionsnodes;" + gcnew System::String(outof.c_str()),
                    gcnew System::EventArgs());
            }

            for (const std::string& name : results.predictorsData.predictorNames)
            {
                RetourJson("allpredictornames;" + gcnew System::String(name.c_str()),
                    gcnew System::EventArgs());
            }
        }
    }

    bool FMTForm::SimulationSpatialeExplicite(
        System::String^ fichierPri,
        System::String^ cheminRasters,
        int scenario,
        System::Collections::Generic::List<System::String^>^ contraintes,
        int periodes,
        int greedySearch,
        System::Collections::Generic::List<System::String^>^ outputs,
        bool indicateurStanlock,
        int outputLevel,
        int etanduSortiesMin,
        int etanduSortiesMax,
        System::String^ cheminSorties,
        bool indGenererEvents,
        bool indSortiesSpatiales,
        System::String^ providerGdal,
        bool indCarbon,
        System::Collections::Generic::List<System::String^>^ predictoryields,
        System::Collections::Generic::List<int>^ growththemes)
    {
        try
        {
            const FMTWrapperCore::SESParameters PARAMS = ConvertirParametres(
                fichierPri, cheminRasters, contraintes, periodes,
                greedySearch, outputs, indicateurStanlock, outputLevel,
                etanduSortiesMin, etanduSortiesMax, cheminSorties,
                indGenererEvents, indSortiesSpatiales, providerGdal,
                indCarbon, predictoryields, growththemes);

            const FMTWrapperCore::SESResults RESULTS =
                FMTWrapperCore::Controller::runSpatialSimulation(PARAMS, scenario);

            _EnvoyerResultatsInterface(RESULTS, indCarbon);

            return true;
        }
        catch (...)
        {
            _raiseFromCatch("", "FMTForm::SimulationSpatialeExplicite", __LINE__, __FILE__);
            return false;
        }
    }
}
