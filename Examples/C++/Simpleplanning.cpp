#include <vector>
#ifdef FMTWITHOSI
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTlogger.h"
#include "FMTconstraint.h"
#include "FMTscheduleparser.h"
#endif


int main()
{
#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
	{
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder + "TWD_land.pri";
		const std::string outdir = "../../tests/Simpleplanning/";
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
		constraints.erase(constraints.begin());
		for (const Core::FMTconstraint& constraint : constraints)
		{
			optimizationmodel.setconstraint(constraint);
		}
		optimizationmodel.setobjective(objective);
		if (optimizationmodel.initialsolve())
		{
			Parser::FMTscheduleparser scheduleparser;
			std::vector<Core::FMTschedule>optimalschedules;
			for (size_t period = 1; period <= 10; ++period)
			{
				optimalschedules.push_back(optimizationmodel.getsolution(period));
			}
			scheduleparser.write(optimalschedules, outdir + optimizationmodel.getname() + "._seq");
		}
	}
	else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}
