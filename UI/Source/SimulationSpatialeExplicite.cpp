#include "stdafx.h"
#include <sstream>
#include "FMTforest.h"
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTsesmodel.h"
#include "FMTareaparser.h"
#include "FMTscheduleparser.h"
#include "FMTtransitionparser.h"
#include "FMTGCBMtransition.h"
#include "FMToutputnode.h"
#include <msclr\marshal_cppstd.h>
#include "FMTFormLogger.h"
#include "FMTForm.h"
#include "FMTmodel.h"
#include "FMTFormCache.h"
#include "FMTexceptionhandlerwarning.h"
#include "FMTdefaultlogger.h"
#include "SES.h"

namespace Wrapper
{
    namespace {

        FMTWrapperCore::SESParameters ConvertirParametres(
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
            System::Collections::Generic::List<int>^ growththemes,
            const std::string& scenarioName)
        {
            FMTWrapperCore::SESParameters params;

            // Conversion des chemins
            params.primaryFilePath = msclr::interop::marshal_as<std::string>(fichierPri);
            params.rastersPath = msclr::interop::marshal_as<std::string>(cheminRasters);
            params.outputPath = msclr::interop::marshal_as<std::string>(cheminSorties);
            params.gdalProvider = msclr::interop::marshal_as<std::string>(providerGdal);
            params.scenarioName = scenarioName;

            // Paramètres numériques
            params.numberOfPeriods = periodes;
            params.greedySearchIterations = greedySearch;
            params.outputLevel = outputLevel;
            params.outputMinPeriod = etanduSortiesMin;
            params.outputMaxPeriod = etanduSortiesMax;

            // Options booléennes
            params.useStanlock = indicateurStanlock;
            params.generateEvents = indGenererEvents;
            params.generateSpatialOutputs = indSortiesSpatiales;
            params.carbonMode = indCarbon;

            // Conversion des listes C# → C++
            for each (System::String ^ constraint in contraintes)
            {
                params.constraintNames.push_back(msclr::interop::marshal_as<std::string>(constraint));
            }

            for each (System::String ^ output in outputs)
            {
                params.outputNames.push_back(msclr::interop::marshal_as<std::string>(output));
            }

            for each (System::String ^ yield in predictoryields)
            {
                params.predictorYields.push_back(msclr::interop::marshal_as<std::string>(yield));
            }

            for each (int theme in growththemes)
            {
                params.growthThemes.push_back(theme);
            }

            return params;
        }

    } 

    void FMTForm::EnvoyerResultatsInterface(
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

        // Fichiers de perturbations
        if (indCarbon)
        {
            for (const std::string& fichier : results.disturbanceFiles)
            {
                RetourJson(gcnew System::String(("GCBMtransitionlocations;" + fichier).c_str()),
                    gcnew System::EventArgs());
            }
        }

        // Fichier d'événements
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

        // Prédicteurs
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
            FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
            *logger << Logging::FMTdefaultlogger().getlogstamp() << "\n";

            Models::FMTmodel baseModel = FMTFormCache::GetInstance()->getmodel(scenario);
            const std::string scenarioName = baseModel.getname();

            FMTWrapperCore::SESParameters params = ConvertirParametres(
                fichierPri, cheminRasters, scenario, contraintes, periodes,
                greedySearch, outputs, indicateurStanlock, outputLevel,
                etanduSortiesMin, etanduSortiesMax, cheminSorties,
                indGenererEvents, indSortiesSpatiales, providerGdal,
                indCarbon, predictoryields, growththemes,
                scenarioName); 

            Parser::FMTmodelparser modelparser;
            std::vector<std::string> scenariosName;
            scenariosName.push_back(params.scenarioName);
            std::vector<Models::FMTmodel> models = modelparser.readproject(params.primaryFilePath, scenariosName);
            Models::FMTmodel& selectedModel = models[0];

            /*
			*logger << "primaryFilePath: " + params.primaryFilePath << "\n";
			*logger << "rastersPath: " + params.rastersPath << "\n";
			*logger << "outputPath: " + params.outputPath << "\n";
			*logger << "scenarioName: " + params.scenarioName << "\n";
			*logger << "numberOfPeriods: " + std::to_string(params.numberOfPeriods) << "\n";
			*logger << "greedySearchIterations: " + std::to_string(params.greedySearchIterations) << "\n";
			*logger << "useStanlock: " + std::string(params.useStanlock ? "true" : "false") << "\n";
			*logger << "outputLevel: " + std::to_string(params.outputLevel) << "\n";
			*logger << "outputMinPeriod: " + std::to_string(params.outputMinPeriod) << "\n";
			*logger << "outputMaxPeriod: " + std::to_string(params.outputMaxPeriod) << "\n";
			*logger << "gdalProvider: " + params.gdalProvider << "\n";
			*logger << "carbonMode: " + std::string(params.carbonMode ? "true" : "false") << "\n";
			*logger << "generateEvents: " + std::string(params.generateEvents ? "true" : "false") << "\n";
			*logger << "generateSpatialOutputs: " + std::string(params.generateSpatialOutputs ? "true" : "false") << "\n";
			*logger << "constraintNames: " << "\n";
			for (const std::string& constraint : params.constraintNames)
			{
				*logger << "  - " + constraint << "\n";
			}
			*logger << "outputNames: " << "\n";
			for (const std::string& output : params.outputNames)
			{
				*logger << "  - " + output << "\n";
			}
			*logger << "predictorYields: " << "\n";
			for (const std::string& yield : params.predictorYields)
			{
				*logger << "  - " + yield << "\n";
			}
			*logger << "growthThemes: " << "\n";
			for (const int& theme : params.growthThemes)
			{
				*logger << "  - " + std::to_string(theme) << "\n";
			}

            */


            *logger << "FMT -> Traitement pour le scénario : " + scenarioName << "\n";

            const std::vector<Core::FMTschedule> schedules = ObtenirSEQ(fichierPri, scenario);

            *logger << "FMT -> Démarrage de la simulation" << "\n";

            FMTWrapperCore::SESResults results =
                FMTWrapperCore::SES::RunSES(params, selectedModel, schedules);

            if (!results.success)
            {
                *logger << "FMT -> Erreur de simulation: " + results.errorMessage << "\n";
                return false;
            }

            *logger << "FMT -> Simulation terminée avec succès" << "\n";

            if (indCarbon && !results.outputsData.results.empty())
            {
                for (const auto& result : results.outputsData.results)
                {
                    for (const auto& periodValue : result.periodValues)
                    {
                        *logger << "outputs;" + result.outputName + ";" + std::to_string(periodValue.second) << "\n";
                    }
                }
            }

            EnvoyerResultatsInterface(results, indCarbon);

            *logger << "FMT -> Envoi des résultats à l'interface terminé" << "\n";

            return true;
        }
        catch (...)
        {
            raisefromcatch("", "FMTForm::SimulationSpatialeExplicite", __LINE__, __FILE__);
            return false;
        }
    }

    // ============================================================================
    // ANCIENNES MÉTHODES - SUPPRIMÉES
    // ============================================================================
    // Toute la logique métier a été déplacée dans SES.cpp
    // Les méthodes suivantes n'existent plus dans FMTForm :
    // - RapportdeBris
    // - RapportdeCarboneSpatial
    // - EcrituredesPerturbations
    // - EcritureDesEvenements
    // - EcritureDesOutputs
    // - EcrituredesOutputsSpatiaux
    // - EcritureDesPredicteurs
    //
    // Toute la logique est maintenant orchestrée par SES::RunSES()
    // ============================================================================
} 