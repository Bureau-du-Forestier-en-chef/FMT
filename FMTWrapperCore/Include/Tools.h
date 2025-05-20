#include <string>
#include <vector>

namespace Core { 
    class FMTschedule;
    class FMTconstraint;
    class FMToutput;
}

namespace Models
{
	class FMTmodel;
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
		static int getMaxAge(const Models::FMTmodel& p_model);

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
		static double getYield(const Models::FMTmodel& p_model, const std::string& p_mask,const std::string& p_yield, int p_age);

		/**
		* @brief retourne un set de tous les masks utiles du modèle selon les thèmes sélectionné.
		*
		*@param p_model le lpModele a utiliser.
		*@param un vector de themes a vérifier.
		*@return Un set des masks.
		*/
		static std::set<std::string> getAllMasks(const Models::FMTmodel& p_model, const int p_periods, const std::vector<int>& p_themesNumbers);
    };
}
