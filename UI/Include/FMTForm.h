#ifndef FMTFORM_HEADER
#define FMTFORM_HEADER
#include "stdafx.h"
#include <vector>
#include <string>
//#include "FMTconstraint.h"

namespace Core { 
	class FMTschedule;
	class FMTconstraint;
	class FMToutput;
}

namespace Models{
	class FMTsemodel;
}
namespace Wrapper
{
	class FMTformCache;

	public ref class FMTForm
	{
	private:
		FMTformCache* Cache;
		void InscrireLigneFichierTexte(System::String^ nomFichier, System::String^ message, bool indicateurFeedback, bool nouveaufichier);
		std::vector<Core::FMTconstraint> ObtenirArrayContraintes(int indexScenario);
		std::vector<Core::FMTconstraint> ObtenirArrayContraintesSelectionnees(std::vector<Core::FMTconstraint> contraitesBase, System::Collections::Generic::List<System::String^>^ contraintesSelection);
		Core::FMToutput ObtenirOutputSelectionnee(std::vector<Core::FMToutput> outputsBase, System::String^ outputSelection);
		std::vector<Core::FMToutput> ObtenirArrayOutputsSelectionnees(std::vector<Core::FMToutput> outputsBase, System::Collections::Generic::List<System::String^>^ outputsSelection);
		//std::vector<Heuristics::FMToperatingareascheme> ObtenirOperatingArea(System::Collections::Generic::List<System::Collections::Generic::List<System::String^>^>^ listeParametres, System::String^ fichierShp, Core::FMTmask selectedmask, std::vector<Core::FMTtheme> presolvedthemes, std::vector<Core::FMTtheme> postsolvedthemes, int numeroTheme, int startingperiod, System::String^ nomChampAge, System::String^ nomChampSuperficie, System::String^ nomChampStanlock, System::Collections::Generic::List<int>^ listeWarnings);
		delegate void managedFeed(const char*);
		managedFeed^ managed;
		System::IntPtr unmanaged;
		void ToFeedBack(const char* message);
		void raisefromcatch(std::string text,
			const std::string& method, const int& line, const std::string& fil);
		std::vector<Core::FMTschedule> ObtenirSEQ(System::String^ nomFichierPri, int indexScenario);
		void RapportdeBris(const Models::FMTsemodel& semodel);
		void RapportdeCarboneSpatial(const Models::FMTsemodel& semodel, const int& nombredeperiodes, const std::vector<Core::FMTschedule>& schedules);
		void EcrituredesPerturbations(const Models::FMTsemodel& semodel, System::String^ cheminsorties, const int& nombredeperiodes, System::Collections::Generic::List<int>^ growththemes, const bool& incarbon);
		void EcritureDesEvenements(const Models::FMTsemodel& semodel, System::String^ cheminsorties, const int& nombredeperiodes, const bool& incarbon);
		std::vector<Core::FMToutput>  EcritureDesOutputs(const Models::FMTsemodel& semodel, System::Collections::Generic::List<System::String^>^ outputs, const int& nombredeperiodes, const bool& incarbon);
		void EcrituredesOutputsSpatiaux(const Models::FMTsemodel& semodel, const std::vector<Core::FMToutput>& outputs, const int& sortiemin, const int& sortiemax, System::String^ localisation);
		void EcritureDesPredicteurs(const Models::FMTsemodel& semodel, const std::string& rastpath, const int& periodes, System::Collections::Generic::List<System::String^>^ predictoryields);
	public:
		event System::EventHandler^ FeedBack;
		event System::EventHandler^ RetourJson;
		FMTForm();
		~FMTForm();
		bool Cache_AjouterScenarios(System::String^ fichierPriSystem, System::String^ scenarioSystem);
		void Cache_Vider();
		void Cache_InitialiserModelParser(System::Collections::Generic::List<int>^ listeWarnings, int maxWarnings);
		void Cache_AssignerNomLogger(System::String^ nomFichierLogger);
		bool Cache_EnleverModel(int indexScenario);
		System::Collections::Generic::List<System::String^>^ ObtenirListeContraintes(int indexScenario);
		System::Collections::Generic::List<System::String^>^ ObtenirListeOutputs(int indexScenario);
		System::Collections::Generic::List<System::String^>^ ObtenirListeExtentionsSorties();
		System::Collections::Generic::List<int>^ ObtenirListeSolvers();
		int ObtenirNombreThemes(int indexScenario);
		int ObtenirNombrePeriodes(System::String^ nomFichierPri, int indexScenario);
		bool Plannification(System::String^ fichierPri, System::Collections::Generic::List<int>^ scenarios, int solver, int period, int nbreProcessus, System::Collections::Generic::List<System::String^>^ outputs, int outputLevel, int etanduSortiesMin, int etanduSortiesMax, System::String^ cheminSorties, System::String^ providerGdal, System::Collections::Generic::List<bool>^ playback);
		bool Replanification(int indexScenStrategique, int indexScenStochastique, int indexScenTactique, int solver, int period, int periodReplannif, double variabilite, int nbreProcessus, int nombreReplicasMin, int nombreReplicasMax, System::Collections::Generic::List<System::String^>^ outputs, int outputLevel, System::String^ cheminSorties, System::String^ providerGdal, int taskLogLevel, bool indProduireSolution, bool p_writeSchedule);
		bool SimulationSpatialeExplicite(System::String^ fichierPri, System::String^ cheminRasters, int scenario, System::Collections::Generic::List<System::String^>^ contraintes, int periodes, int greedySearch, System::Collections::Generic::List<System::String^>^ outputs, bool indicateurStanlock, int outputLevel, int etanduSortiesMin, int etanduSortiesMax, System::String^ cheminSorties, bool indGenererEvents, bool indSortiesSpatiales, System::String^ providerGdal, bool indCarbon, System::Collections::Generic::List<System::String^>^ predictoryields, System::Collections::Generic::List<int>^ growththemes);
		bool InitialAreaVariability(System::String^ fichierPri, int scenario, int solver, System::Collections::Generic::List<System::String^>^ contraintes, int period, System::Collections::Generic::List<System::String^>^ outputs, int outputLevel, int etanduSortiesMin, int etanduSortiesMax, System::String^ cheminSorties, System::String^ providerGdal, System::Collections::Generic::List<System::Collections::Generic::List<System::String^>^>^ ListeInformations);
		bool Raterisation(System::String^ fichierPri, int scenario, System::String^ fichierShp, System::String^ repertoireSortie, int resolution, System::String^ nomChampAge, System::String^ nomChampSuperficie, System::String^ nomChampStanlock);
		bool OperatingAreaScheduling(System::String^ fichierPri, int scenario, System::String^ fichierShp, int solver, int nombrePeriodes, int nombreThread, int numeroTheme, int tempsMaximum, int nombreIteration, System::String^ nomChampAge, System::String^ nomChampSuperficie, System::String^ nomChampStanlock, System::String^ cheminParametres, System::String^ nomFichierResultat, int periodeMiseAjour, System::String^ returnTimeOutput);
		//std::vector<std::string> getAttributes(const Models::FMTmodel& p_model, int p_theme);
		/**
		* @brief Agrège toutes les actions en utilisant le modèle spécifié.
		*
		* Cette fonction agrège toutes les actions basées sur l'index du modèle et les paramètres fournis.
		*
		* @param p_modelIndex Index du modèle à utiliser pour l'agrégation.
		* @param p_aggregates Liste des actions agrégées.
		* @param p_schedulePri Le path du fichier .pri.
		* @param p_outputDirPath Le path du dossier de sortie.
		* @param p_scenario_name Nom du scénario.
		* @return `true` si l'agrégation a réussi, `false` sinon.
		*/
		bool aggregateAllActions(const int p_modelIndex, System::Collections::Generic::List<System::String^>^ p_aggregates, System::Collections::Generic::List<System::String^>^ p_order,  System::String^ p_schedulePri, System::String^ p_outputDirPath, System::String^ p_scenario_name, System::String^ p_pri_name);

		/**
		* @brief Sépare les actions en utilisant le modèle spécifié.
		*
		* Cette fonction divise les actions basées sur l'index du modèle et les paramètres fournis.
		*
		* @param p_modelIndex Index du modèle à utiliser pour la séparation.
		* @param p_schedulePri Le path du fichier .pri.
		* @param p_splitted Liste sur lequel les splits sont effectué ex: {"ACT","AEC"}.
		* @param p_splitted_mask Le mask a splitter.
		* @param p_outputDirPath Le path du dossier sortie.
		* @param p_scenario_name Nom du scénario.
		* @return `true` si la séparation a réussi, `false` sinon.
		*/
		bool splitActions(const int p_modelIndex, System::String^ p_schedulePri, System::Collections::Generic::List<System::String^>^ p_splitted, System::Collections::Generic::List<System::String^>^ p_splitted_mask, System::String^ p_outputDirPath, System::String^ p_scenario_name, System::String^ p_pri_name);

		/**
		* @brief retourne une listes des noms d'actions du modèle.
		*
		*@param p_modelIndex Index du modèle à utiliser.
		* @return une listes des noms d'actions du modèle sinon nullptr
		*/
		System::Collections::Generic::List<System::String^>^ getActionsNames(int p_modelIndex);

		/**
		* @brief retourne une listes des noms d'actions du modèle.
		*
		*@param p_modelIndex Index du modèle à utiliser.
		* @return une listes des noms d'actions du modèle sinon nullptr
		*/
		System::Collections::Generic::List<System::String^>^ getAggregates(int p_modelIndex);

		/**
		* @brief retourne une listes des noms de yields du modèle.
		*
		*@param p_modelIndex Index du modèle à utiliser.
		* @return une listes des noms des yields du modèle sinon nullptr
		*/
		System::Collections::Generic::List<System::String^>^ getYields(int p_modelIndex);

		/**
		* @brief retourne un double du yield.
		*
		*@param p_modelIndex Index du modèle à utiliser.
		*@param p_yield
		*@param p_age
		* @return Un double du yield
		*/
		double getYield(int p_modelIndex, System::String^ p_mask, System::String^ p_yield, int p_age);

		/**
		* @brief retourne l'age max du model.
		*
		*@param p_modelIndex Index du modèle à utiliser.
		* @return double
		*/
		double getMaxAge(int p_modelIndex);

		/**
		* @brief Build l'action.
		*
		*@param p_modelIndex Index du modèle à utiliser.
		*@param p_actionName Le nom de l'action
		*@param p_targetYield Le targetYield
		*@param p_schedulePri Le path du fichier .pri
		*@param p_outputDirPath Le path du dossier de sortie
		*@param p_scenario_name Le nom du scénario
		*@return vrai ou faux
		*/
		bool buildAction(const int p_modelIndex, System::String^ p_actionName, System::String^ p_targetYield, System::String^ p_schedulePri, System::String^ p_outputDirPath, System::String^ p_scenario_name, System::String^ p_pri_name);
		
		/**
		* @brief Donne tous les attriduts du modèle selon le thème.
		*
		*@param p_modelIndex Index du modèle à utiliser.
		*@param p_themeIndex Index du thème souhaité dans la liste des thème du modèle.
		*@return Retourne une liste d'attributs du thème.
		*/
		System::Collections::Generic::List<System::String^>^ getAttributes(const int p_modelIndex, const int p_themeIndex);

		/**
		* @brief Dit si le mask est valide.
		*
		*@param p_modelIndex Index du modèle à utiliser.
		*@param p_mask Le mask à valider.
		*@return Retourne true ou false.
		*/
		bool validateMask(const int p_modelIndex, System::String^ p_mask);

		/**
		* @brief Retourne tous les masks du modèle utilisé selon les thèmes.
		*
		*@param p_modelIndex Index du modèle à utiliser.
		*@param p_themes Liste des thèmes à vérifier.
		*@return Retourne une liste de masks.
		*/
		System::Collections::Generic::List<System::String^>^ getAllMasks(const int p_modelIndex, const int p_periods, System::Collections::Generic::List<int>^ p_themesNumber);
	};
}
#endif
