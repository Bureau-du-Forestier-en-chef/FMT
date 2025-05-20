#include  "Tools.h"
#include "FMTmodel.h"
#include "FMTlpmodel.h"
#include "FMTerror.h"


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

std::set<std::string> FMTWrapperCore::Tools::getAllMasks(const Models::FMTmodel& p_model, const std::vector<int>& p_themesNumbers) {
	std::set<std::string> masks;
	try
	{

		const std::vector<Models::FMTmodel> MODELS = ModelParser.readproject(pathPri, { scenarioName });
		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(pathPri, MODELS).at(0);
		//const int PERIODS = SCHEDULES.back().getperiod();
		const int PERIODS = 5;
		Models::FMTlpmodel optModel(MODELS.at(0), Models::FMTsolverinterface::MOSEK);
		optModel.setparameter(Models::FMTintmodelparameters::LENGTH, PERIODS);
		optModel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		optModel.doplanning(false);
		optModel.getAllMasks(THEMES);
		masks = p_model.getAllMasks(p_themes);
	}
	catch (...)
	{
		Exception::FMTexceptionhandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raisefromcatch("", "FMTWrapperCore::Tools::getYield", __LINE__, __FILE__);
	}
	return masks;
}

