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
