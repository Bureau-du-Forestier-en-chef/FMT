#include <vector>
#ifdef FMTWITHOSI
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTconstraint.h"
#include "FMTscheduleparser.h"
#endif


int main()
{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTversion().hasFeature("OSI"))
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
			optimizationmodel.buildPeriod();
		}
		std::vector<Core::FMTConstraint>constraints = optimizationmodel.getconstraints();
		const Core::FMTConstraint objective = constraints.at(0);
		constraints.erase(constraints.begin());
		for (const Core::FMTConstraint& constraint : constraints)
		{
			optimizationmodel.setConstraint(constraint);
		}
		optimizationmodel.setObjective(objective);
		if (optimizationmodel.initialSolve())
		{
			Parser::FMTscheduleparser scheduleparser;
			std::vector<Core::FMTSchedule>optimalschedules;
			for (size_t period = 1; period <= 10; ++period)
			{
				optimalschedules.push_back(optimizationmodel.getSolution(period));
			}
			scheduleparser.write(optimalschedules, outdir + optimizationmodel.getName() + "._seq");
		}
	}
	else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}
