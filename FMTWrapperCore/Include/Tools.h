#include <string>
#include <vector>
#include <set>
#include "FMTModel.h"

namespace Core { 
    class FMTSchedule;
    class FMTConstraint;
    class FMTOutput;
	class FMTTheme;
	class FMTMask;
}

namespace Models
{
	class FMTModel;
}

namespace FMTWrapperCore
{
    class __declspec(dllexport) Tools
    {
    public:
		/**
		* @brief Retourne l'age max du model selon les yields basé sur l'age.
		*
		*@param p_model Le modèle à utiliser.
		* @return int l'age max du model.
		*/
		static int getMaxAge(const Models::FMTModel& p_model);

		/**
		* @brief retourne une listes des noms d'actions du mod�le.
		*
		*@param p_modelIndex Index du mod�le � utiliser.
		* @return une listes des noms d'actions du mod�le sinon nullptr
		*/
		//std::vector<std::string> getActionsNames(int p_modelIndex);

		/**
		* @brief retourne une listes des noms d'actions du mod�le.
		*
		*@param p_modelIndex Index du mod�le � utiliser.
		* @return une listes des noms d'actions du mod�le sinon nullptr
		*/
		//std::vector<std::string> getAggregates(int p_modelIndex);

		/**
		* @brief retourne une listes des noms de yields du mod�le.
		*
		*@param p_modelIndex Index du mod�le � utiliser.
		* @return une listes des noms des yields du mod�le sinon nullptr
		*/
		//std::vector<std::string> getYields(int p_modelIndex);

		/**
		* @brief retourne un double du yield.
		*
		*@param p_model Modele a utiliser.
		*@param p_yield
		*@param p_age
		* @return Un double du yield
		*/
		static double getYield(const Models::FMTModel& p_model, const std::string& p_mask,const std::string& p_yield, int p_age);

		/**
		* @brief retourne un set de tous les masks utiles du modèle selon les thèmes sélectionné.
		*
		*@param p_model le lpModele a utiliser.
		*@param un vector de themes a vérifier.
		*@return Un set des masks.
		*/
		static std::set<std::string> getAllMasks(const Models::FMTModel& p_model, const int p_periods, const std::vector<int>& p_themesNumbers, const std::string& p_rasterPath);

		/**
		* @brief écrit un projet en cache (base + scénarios) dans un dossier.
		*
		* Le nom du fichier .pri est dérivé automatiquement du nom du modèle de base
		* (p_models.front().getname())
		*
		*@param p_models Les modèles du cache; le premier sert de base (ROOT).
		*@param p_destinationDirectory Le dossier de destination où écrire le projet.
		*/
		static void writeToProject(const std::vector<Models::FMTModel>& p_models, const std::string& p_destinationDirectory);
		/**
		* @brief Get change log
		*@return the change log
		*/
		static std::string getChangeLog();
		/**
		* @brief Get exception description
		*@return the exception description
		*/
		static std::string getExceptionDescription(int p_exceptionId);
	private:
		/**
		* @brief Retourne un vecteur de FMTActualDevelopment selon le raster d'aire.
		*
		*@param p_model le Modele a utiliser.
		*@param p_rasterPath Le path du raster d'aire.
		*@return Un vecteur de FMTActualDevelopment.
		*/
		static std::vector<Core::FMTActualDevelopment> getRasterArea(const Models::FMTModel& p_model, const std::string& p_rasterPath);
		static const size_t m_GET_ALL_MASKS_THRESHOLD = 1000000;
		static Core::FMTMask _GetFullMask(const std::vector<Core::FMTTheme>& p_themes);
		static std::set<std::string> _GetThemesDecomposition(
					const Core::FMTMask& p_mask,
					const std::vector<Core::FMTTheme>& p_themes);
		
	
	};
}
