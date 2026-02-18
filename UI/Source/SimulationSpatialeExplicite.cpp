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
#include "SES.h" // Nouveau header

namespace Wrapper
{

// ============================================================================
// MÉTHODES REFACTORISÉES - N'APPELLENT QUE SES ET GÈRENT L'INTERFACE
// ============================================================================

void FMTForm::RapportdeBris(const Models::FMTsemodel& semodel)
{
    try
    {
        // Appel à la logique pure C++
        FMTWrapperCore::SES::GenerateInfeasibilityReport(semodel);
        // Note: Cette méthode écrit toujours directement dans le logger natif
        // comme avant, donc pas de changement de comportement
    }
    catch (...)
    {
        FMTFormCache::GetInstance()->GetFormHandler()->raisefromcatch("", "FMTForm::RapportdeBris", __LINE__, __FILE__);
    }
}

void FMTForm::RapportdeCarboneSpatial(
    const Models::FMTsemodel& semodel,
    const int& nombredeperiodes, 
    const std::vector<Core::FMTschedule>& schedules)
{
    try 
    {
        // Appel à la logique pure C++ pour obtenir les données
        FMTWrapperCore::CarbonReportData reportData = 
            FMTWrapperCore::SES::GenerateCarbonReport(semodel, nombredeperiodes, schedules);
        
        // Gestion de l'interface - envoi des données à C#
        for (const auto& periodData : reportData.periods)
        {
            int jsonloc = periodData.period - 1;
            
            // Envoi de l'objectif et de l'infaisabilité
            RetourJson("objectives;" + jsonloc + ";Objective;" + periodData.objectiveValue, 
                      gcnew System::EventArgs());
            RetourJson("objectives;" + jsonloc + ";Primalinfeasibility;" + periodData.primalInfeasibility, 
                      gcnew System::EventArgs());
            
            // Envoi des ratios d'actions
            for (const auto& actionRatio : periodData.actionRatios)
            {
                RetourJson("objectives;" + jsonloc + ";" + 
                          gcnew System::String(actionRatio.first.c_str()) + ";" + actionRatio.second, 
                          gcnew System::EventArgs());
            }
            
            // Envoi du ratio total
            RetourJson("objectives;" + jsonloc + ";Total;" + periodData.totalRatio, 
                      gcnew System::EventArgs());
        }
    }
    catch (...)
    {
        FMTFormCache::GetInstance()->GetFormHandler()->raisefromcatch("", "FMTForm::RapportdeCarboneSpatial", __LINE__, __FILE__);
    }
}

void FMTForm::EcrituredesPerturbations(
    const Models::FMTsemodel& semodel, 
    System::String^ cheminsorties,
    const int& nombredeperiodes, 
    System::Collections::Generic::List<int>^ growththemes, 
    const bool& incarbon)
{
    try 
    {
        // Conversion des paramètres C# → C++
        const std::string rastpath = msclr::interop::marshal_as<std::string>(cheminsorties);
        std::vector<int> growthThemeIndices;
        for each (int themeID in growththemes)
        {
            growthThemeIndices.push_back(themeID);
        }
        
        // Log dans le logger natif
        FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
        *logger << "FMT -> Écriture des perturbations" << "\n";
        
        // Appel à la logique pure C++
        std::vector<std::string> transitionFiles = 
            FMTWrapperCore::SES::WriteDisturbances(
                semodel, 
                rastpath, 
                nombredeperiodes, 
                growthThemeIndices);
        
        // Gestion de l'interface - envoi des chemins de fichiers à C#
        if (incarbon)
        {
            for (const std::string& fichier : transitionFiles)
            {
                RetourJson(gcnew System::String(("GCBMtransitionlocations;" + fichier).c_str()), 
                          gcnew System::EventArgs());
            }
        }
    }
    catch (...)
    {
        FMTFormCache::GetInstance()->GetFormHandler()->raisefromcatch("", "FMTForm::EcrituredesPerturbations", __LINE__, __FILE__);
    }
}

void FMTForm::EcritureDesEvenements(
    const Models::FMTsemodel& semodel, 
    System::String^ cheminsorties,
    const int& nombredeperiodes, 
    const bool& incarbon)
{
    try 
    {
        FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
        
        // Appel à la logique pure C++ pour obtenir les données
        FMTWrapperCore::EventsData eventsData = FMTWrapperCore::SES::GenerateEventsData(semodel);
        
        // Gestion de l'écriture dans le fichier (interface)
        System::String^ eventpath = System::IO::Path::Combine(
            cheminsorties, 
            gcnew System::String(std::string("events.txt").c_str()));
        
        *logger << "Écriture des événements ici: " + msclr::interop::marshal_as<std::string>(eventpath) << "\n";
        
        InscrireLigneFichierTexte(
            eventpath, 
            gcnew System::String(eventsData.statistics.c_str()), 
            false, 
            true);
        
        // Envoi du chemin à l'interface C#
        if (incarbon)
        {
            const std::string stdeventpath = msclr::interop::marshal_as<std::string>(eventpath);
            RetourJson(gcnew System::String(("eventslocation;" + stdeventpath).c_str()), 
                      gcnew System::EventArgs());
        }
    }
    catch (...)
    {
        FMTFormCache::GetInstance()->GetFormHandler()->raisefromcatch("", "FMTForm::EcritureDesEvenements", __LINE__, __FILE__);
    }
}

std::vector<Core::FMToutput> FMTForm::EcritureDesOutputs(
    const Models::FMTsemodel& semodel, 
    System::Collections::Generic::List<System::String^>^ outputs, 
    const int& nombredeperiodes, 
    const bool& incarbon)
{
    std::vector<Core::FMToutput> listeOutputs;
    
    try 
    {
        FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
        
        // Conversion des paramètres C# → C++
        std::vector<std::string> outputNames;
        for each (System::String^ output in outputs)
        {
            outputNames.push_back(msclr::interop::marshal_as<std::string>(output));
        }
        
        // Appel à la logique pure C++ pour obtenir les données
        FMTWrapperCore::OutputsData outputsData = 
            FMTWrapperCore::SES::CalculateOutputs(semodel, outputNames, nombredeperiodes);
        
        // Récupérer les objets outputs pour le retour
        listeOutputs = outputsData.outputObjects;
        
        // Gestion de l'interface - envoi des résultats à C#
        if (incarbon)
        {
            for (const auto& result : outputsData.results)
            {
                for (const auto& periodValue : result.periodValues)
                {
                    *logger << "outputs;" + result.outputName + ";" + std::to_string(periodValue.second) << "\n";
                    RetourJson("outputs;" + gcnew System::String(result.outputName.c_str()) + ";" + periodValue.second, 
                              gcnew System::EventArgs());
                }
            }
        }
    }
    catch (...)
    {
        FMTFormCache::GetInstance()->GetFormHandler()->raisefromcatch("", "FMTForm::EcritureDesOutputs", __LINE__, __FILE__);
    }
    
    return listeOutputs;
}

void FMTForm::EcrituredesOutputsSpatiaux(
    const Models::FMTsemodel& semodel, 
    const std::vector<Core::FMToutput>& outputs, 
    const int& sortiemin, 
    const int& sortiemax, 
    System::String^ localisation)
{
    try 
    {
        FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
        *logger << "FMT -> Écriture des outputs spatiaux" << "\n";
        
        // Conversion du paramètre C# → C++
        const std::string outputPath = msclr::interop::marshal_as<std::string>(localisation);
        
        // Appel à la logique pure C++
        std::vector<std::string> rasterFiles = 
            FMTWrapperCore::SES::WriteSpatialOutputs(
                semodel, 
                outputs, 
                sortiemin, 
                sortiemax, 
                outputPath);
        
        *logger << "FMT -> Écriture des outputs spatiaux terminée" << "\n";
    }
    catch (...)
    {
        FMTFormCache::GetInstance()->GetFormHandler()->raisefromcatch("", "FMTForm::EcrituredesOutputsSpatiaux", __LINE__, __FILE__);
    }
}

void FMTForm::EcritureDesPredicteurs(
    const Models::FMTsemodel& semodel, 
    const std::string& rastpath, 
    const int& periodes, 
    System::Collections::Generic::List<System::String^>^ predictoryields)
{
    try 
    {
        // Conversion des paramètres C# → C++
        std::vector<std::string> yieldsForPredictors;
        for each (System::String^ valeur in predictoryields)
        {
            yieldsForPredictors.push_back(msclr::interop::marshal_as<std::string>(valeur));
        }
        
        // Appel à la logique pure C++ pour obtenir les données
        FMTWrapperCore::PredictorsData predictorsData = 
            FMTWrapperCore::SES::CalculatePredictors(
                semodel, 
                rastpath, 
                periodes, 
                yieldsForPredictors);
        
        // Gestion de l'interface - envoi des données à C#
        for (const auto& node : predictorsData.nodes)
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
        
        // Envoi des noms de prédicteurs
        for (const std::string& name : predictorsData.predictorNames)
        {
            RetourJson("allpredictornames;" + gcnew System::String(name.c_str()), 
                      gcnew System::EventArgs());
        }
    }
    catch (...)
    {
        FMTFormCache::GetInstance()->GetFormHandler()->raisefromcatch("", "FMTForm::EcritureDesPredicteurs", __LINE__, __FILE__);
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
        // ====================================================================
        // PARTIE 1: PRÉPARATION DU MODÈLE (reste dans FMTForm car utilise FMTFormCache)
        // ====================================================================
        
        Models::FMTsesmodel simulationmodel(FMTFormCache::GetInstance()->getmodel(scenario));
        FMTFormLogger* logger = FMTFormCache::GetInstance()->GetFormLogger();
        *logger << Logging::FMTdefaultlogger().getlogstamp() << "\n";
        *logger << "FMT -> Traitement pour le scénario : " + simulationmodel.getname() << "\n";
        
        // Gestion des contraintes
        if (contraintes->Count > 0)
        {
            *logger << "FMT -> Intégration des contraintes sélectionnées" << "\n";
            simulationmodel.setconstraints(
                ObtenirArrayContraintesSelectionnees(
                    ObtenirArrayContraintes(scenario), 
                    contraintes));
        }
        
        // Modification des transitions
        *logger << "FMT -> Modification et intégration des transitions" << "\n";
        std::vector<Core::FMTtransition> strans;
        for (const auto& tran : simulationmodel.gettransitions())
        {
            strans.push_back(tran.single());
        }
        simulationmodel.settransitions(strans);
        
        // Lecture des rasters
        *logger << "FMT -> Lecture des rasters" << "\n";
        Parser::FMTareaparser areaparser;
        std::string rastpath = msclr::interop::marshal_as<std::string>(cheminRasters);
        const std::string agerast = rastpath + "AGE.tif";
        
        std::vector<std::string> themesrast;
        for (int i = 1; i <= simulationmodel.getthemes().size(); i++)
        {
            themesrast.push_back(rastpath + "THEME" + std::to_string(i) + ".tif");
        }
        
        Spatial::FMTforest initialforestmap;
        if (!indicateurStanlock)
        {
            initialforestmap = areaparser.readrasters(
                simulationmodel.getthemes(), 
                themesrast, 
                agerast, 
                1, 
                0.0001);
        }
        else
        {
            initialforestmap = areaparser.readrasters(
                simulationmodel.getthemes(), 
                themesrast, 
                agerast, 
                1, 
                0.0001, 
                rastpath + "STANLOCK.tif");
        }
        simulationmodel.setinitialmapping(initialforestmap);
        
        // Lecture des schedules
        const std::vector<Core::FMTschedule> schedules = ObtenirSEQ(fichierPri, scenario);
        
        if (schedules.back().getperiod() < periodes)
        {
            const std::string logout = "Dépassement de la période : size " + 
                std::to_string(schedules.size()) + " periode " + 
                std::to_string((schedules.back().getperiod() + 1));
            *logger << logout << "\n";
            return false;
        }
        
        // Configuration et exécution de la simulation
        simulationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, periodes);
        simulationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, greedySearch);
        simulationmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
        simulationmodel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
        simulationmodel.doplanning(false, schedules);
        
        // Détermination du répertoire de sortie
        System::String^ directoryFullName;
        if (indCarbon)
        {
            directoryFullName = cheminRasters;
        }
        else 
        {
            System::IO::DirectoryInfo^ parentdir = System::IO::Directory::GetParent(cheminSorties);
            directoryFullName = parentdir->FullName;
        }
        
        // ====================================================================
        // PARTIE 2: GÉNÉRATION DES RAPPORTS ET OUTPUTS (appels à SES)
        // ====================================================================
        
        // Rapport de bris
        RapportdeBris(simulationmodel);
        
        // CHANGEMENT: Rapport de carbone TOUJOURS appelé maintenant (plus de condition if)
        RapportdeCarboneSpatial(simulationmodel, periodes, schedules);
        
        // Écriture des perturbations
        EcrituredesPerturbations(simulationmodel, directoryFullName, periodes, growththemes, indcarbon);
        
        // Génération des événements
        if (indGenererEvents || indCarbon)
        {
            EcritureDesEvenements(simulationmodel, directoryFullName, periodes, indCarbon);
        }
        
        // Traitement des outputs
        if (outputs->Count > 0)
        {
            const std::vector<Core::FMToutput> listeOutputs = 
                EcritureDesOutputs(simulationmodel, outputs, periodes, indCarbon);
            
            // Écriture du schedule
            Parser::FMTscheduleparser scheduparser;
            System::String^ schedulepath = System::IO::Path::Combine(
                directoryFullName, 
                gcnew System::String(std::string(simulationmodel.getname() + "_.seq").c_str()));
            const std::string stdschedulepath = msclr::interop::marshal_as<std::string>(schedulepath);
            scheduparser.write(simulationmodel.GetSchedules(simulationmodel.getspschedule()), stdschedulepath);
            
            // Mode non-carbone: export complet des résultats
            if (!indCarbon)
            {
                Parser::FMTmodelparser Modelparser;
                *logger << "FMT -> Exportations des sorties " << "\n";
                
                Modelparser.writeresults(
                    simulationmodel,
                    listeOutputs,
                    etanduSortiesMin,
                    etanduSortiesMax,
                    msclr::interop::marshal_as<std::string>(cheminSorties),
                    static_cast<Core::FMToutputlevel>(outputLevel),
                    msclr::interop::marshal_as<std::string>(providerGdal));
            }
            // Mode carbone: exports spécifiques
            else
            {
                RetourJson("schedules;" + gcnew System::String(stdschedulepath.c_str()), 
                          gcnew System::EventArgs());
                
                // Écriture de la forêt mise à jour
                const Spatial::FMTSpatialSchedule& schedule = simulationmodel.getspschedule();
                areaparser.writeforest(
                    schedule.getforestperiod(0), 
                    simulationmodel.getthemes(), 
                    themesrast, 
                    rastpath + "AGE.tif", 
                    rastpath + "STANLOCK.tif");
                
                // Calcul des prédicteurs
                if (predictoryields->Count > 0)
                {
                    EcritureDesPredicteurs(simulationmodel, rastpath, periodes, predictoryields);
                }
            }
            
            // Outputs spatiaux
            if (indSortiesSpatiales)
            {
                EcrituredesOutputsSpatiaux(
                    simulationmodel, 
                    listeOutputs, 
                    etanduSortiesMin, 
                    etanduSortiesMax, 
                    directoryFullName);
            }
        }
    }
    catch (...)
    {
        raisefromcatch("", "FMTForm::SimulationSpatialeExplicite", __LINE__, __FILE__);
        return false;
    }
    
    return true;
}

} // namespace Wrapper