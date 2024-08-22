#ifndef FMTFORM_CORE_HEADER
#define FMTFORM_CORE_HEADER

#include <string>
#include <vector>

namespace Core { 
    class FMTschedule;
    class FMTconstraint;
    class FMToutput;
}

namespace FMTWrapperCore
{
    class Tools
    {
    public:

		/**
		* @brief retourne une listes des noms d'actions du mod�le.
		*
		*@param p_modelIndex Index du mod�le � utiliser.
		* @return une listes des noms d'actions du mod�le sinon nullptr
		*/
		std::vector<std::string> getActionsNames(int p_modelIndex);

		/**
		* @brief retourne une listes des noms d'actions du mod�le.
		*
		*@param p_modelIndex Index du mod�le � utiliser.
		* @return une listes des noms d'actions du mod�le sinon nullptr
		*/
		std::vector<std::string> getAggregates(int p_modelIndex);

		/**
		* @brief retourne une listes des noms de yields du mod�le.
		*
		*@param p_modelIndex Index du mod�le � utiliser.
		* @return une listes des noms des yields du mod�le sinon nullptr
		*/
		std::vector<std::string> getYields(int p_modelIndex);

		/**
		* @brief retourne un double du yield.
		*
		*@param p_modelIndex Index du mod�le � utiliser.
		*@param p_yield
		*@param p_age
		* @return Un double du yield
		*/
		double getYield(int p_modelIndex, std::string p_mask, std::string p_yield, int p_age);
    };
}

#endif