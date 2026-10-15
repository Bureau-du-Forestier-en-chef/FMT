#include <vector>
#ifdef FMTWITHOSI
#include "FMTLpModel.h"
#include "FMTModelParser.h"
#include "FMTVersion.h"
#include "FMTDefaultLogger.h"
#include "FMTConstraint.h"
#include "FMTScheduleParser.h"
#endif


int main()
{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTVersion().hasFeature("OSI"))
	{
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder + "TWD_land.pri";
		const std::string outdir = "../../tests/Simpleplanning/";
		Parser::FMTModelParser modelparser;
		const std::vector<std::string>scenarios(1, "LP");
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTSolverInterface::CLP);
		for (size_t period = 0; period < 10; ++period)
		{
			optimizationmodel.buildPeriod();
		}
		std::vector<Core::FMTConstraint>constraints = optimizationmodel.getConstraints();
		const Core::FMTConstraint objective = constraints.at(0);
		constraints.erase(constraints.begin());
		for (const Core::FMTConstraint& constraint : constraints)
		{
			optimizationmodel.setConstraint(constraint);
		}
		optimizationmodel.setObjective(objective);
		if (optimizationmodel.initialSolve())
		{
			Parser::FMTScheduleParser scheduleparser;
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
