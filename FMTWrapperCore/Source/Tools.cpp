#include  "Tools.h"
#include "FMTmodel.h"
#include "FMTlpmodel.h"
#include "FMTerror.h"
#include "FMTmodelparser.h"


int FMTWrapperCore::Tools::getMaxAge(const Models::FMTmodel& p_model)
{
	double result = 0;
	try
	{
		const Core::FMTyields YIELDS = p_model.getyields();
		std::vector<const Core::FMTyieldhandler*> handler;
		for (const auto& DATA : YIELDS)
		{
            if (DATA.second->gettype() == Core::FMTyldtype::FMTageyld){

                handler.push_back(DATA.second.get());
            }
		}
		result = YIELDS.getmaxbase(handler);

	}
	catch (...)
	{
		Exception::FMTexceptionhandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raisefromcatch("", "FMTWrapperCore::Tools::getMaxAge", __LINE__, __FILE__);
	}
	return result;
}

double FMTWrapperCore::Tools::getYield(const Models::FMTmodel& p_model, const std::string& p_mask, const std::string& p_yield, int p_age)
{
	double result = 0;
	try
	{
        result = p_model.getYieldValue(p_mask, p_yield, p_age, 0);
	}
	catch (...)
	{
		Exception::FMTexceptionhandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raisefromcatch("", "FMTWrapperCore::Tools::getYield", __LINE__, __FILE__);
	}
	return result;
}

std::set<std::string> FMTWrapperCore::Tools::getAllMasks(const Models::FMTmodel& p_model, const int p_periods, const std::vector<int>& p_themesNumbers) {
	std::set<std::string> masks;
	try
	{
		Parser::FMTmodelparser ModelParser;
		// A vérifier avec Guillaume si on peut faire autrement que de fournir le path
		const std::vector<Models::FMTmodel> MODELS(1, p_model);

		// On va chercher tous les thèmes dans le modèle
		std::vector<Core::FMTtheme> themes;
		for (const int themeNumber : p_themesNumbers)
		{
			themes.push_back(p_model.getthemes().at(themeNumber));
		}

		// On transforme notre modèle en lpModel qui lui peut faire un getAllMasks et on ajoute les paramètres qu'on a besoin.
		Models::FMTlpmodel optModel(p_model, Models::FMTsolverinterface::MOSEK);
		optModel.setparameter(Models::FMTintmodelparameters::LENGTH, p_periods);
		optModel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		optModel.doplanning(false);

		masks = optModel.getAllMasks(themes);
		
	}
	catch (...)
	{
		Exception::FMTexceptionhandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raisefromcatch("", "FMTWrapperCore::Tools::getAllMasks", __LINE__, __FILE__);
	}
	return masks;
}

