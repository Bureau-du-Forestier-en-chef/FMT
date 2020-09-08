#include <vector>
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTlogger.h"


int main()
	{
	Logging::FMTlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
		{
		#if defined _MSC_VER
			const std::string primarylocation = "../Examples/Models/TWD_land/TWD_land.pri";
		#else
			const std::string primarylocation = "Examples/Models/TWD_land/TWD_land.pri";
		#endif
		Parser::FMTmodelparser modelparser;
		const std::vector<std::string>scenarios(1, "LP");
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		for (size_t period = 0; period < 10; ++period)
		{
			optimizationmodel.buildperiod();
		}
		std::vector<Core::FMTconstraint>constraints = optimizationmodel.getconstraints();
		const Core::FMTconstraint objective = constraints.at(0);
		optimizationmodel.setobjective(objective);
		constraints.erase(constraints.begin());
		for (const Core::FMTconstraint& constraint : constraints)
		{
			optimizationmodel.setconstraint(constraint);
		}
		optimizationmodel.initialsolve();
	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	
	return 0;
	}

