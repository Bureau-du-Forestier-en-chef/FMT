#include  "Tools.h"


int FMTWrapperCore::Tools::getMaxAge(const Models::FMTmodel& p_model)
{
	double result = 0;
	try
	{
		const Core::FMTyields YIELDS = p_model.getyields();
		const Core::FMTyields* YIELDSptr = &YIELDS;
		std::vector<const Core::FMTyieldhandler*> handler;
		for (const auto& DATA : YIELDS)
		{
			handler.push_back(DATA.second.get());
		}
		result = YIELDS.getmaxbase(handler);

	}
	catch (...)
	{
		throw std::exception("Error in FMTWrapperCore::Tools::getMaxAge");
	}
	return result;
}

double FMTWrapperCore::Tools::getYield(const Models::FMTmodel& p_model, const std::string& p_mask, const std::string& p_yield, int p_age)
{
	double result = NULL;
	try
	{
		const std::vector<Core::FMTtheme> THEMES = p_model.getthemes();
		const Core::FMTmask MASK = Core::FMTmask(p_mask, THEMES);
		const Core::FMTdevelopment DEVELOPMENT = Core::FMTdevelopment(MASK, p_age, 0);
		const Core::FMTyieldrequest REQUEST = DEVELOPMENT.getyieldrequest();
		const Core::FMTyields YIELDS = p_model.getyields();
		result = YIELDS.get(REQUEST, p_yield);
	}
	catch (...)
	{
		throw std::exception("Error in FMTWrapperCore::Tools::getYield");
	}
	return result;
}

