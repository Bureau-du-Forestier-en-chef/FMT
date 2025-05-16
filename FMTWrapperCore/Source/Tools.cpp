#include  "Tools.h"
#include "FMTmodel.h"
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

