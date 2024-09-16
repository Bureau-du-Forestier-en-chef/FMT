#include <string>
#include <vector>
#include "FMTmodel.h"

namespace Core { 
    class FMTschedule;
    class FMTconstraint;
    class FMToutput;
}

namespace FMTWrapperCore
{
    class __declspec(dllexport) Tools
    {
    public:
		/**
		* @brief retourne l'age max du model.
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
		*@param p_modelIndex Index du mod�le � utiliser.
		*@param p_yield
		*@param p_age
		* @return Un double du yield
		*/
		//double getYield(int p_modelIndex, std::string p_mask, std::string p_yield, int p_age);
    };
}
