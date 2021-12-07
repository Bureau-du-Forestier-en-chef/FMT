#ifdef FMTWITHOSI
#include <vector>
#include "FMTlpmodel.hpp"
#include "FMTmodelparser.hpp"
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTconstraint.hpp"
#include "FMTscheduleparser.hpp"
#include "FMTmask.hpp"
#include "FMToutputnode.hpp"
#include "FMTtheme.hpp"
#include <string>
#endif


int main()
{
#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder + "TWD_land.pri";
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
			std::vector<Core::FMToutput>outputtotest;
			for (const Core::FMToutput& output : optimizationmodel.getoutputs())
				{
				if (output.getname()== "OSUPREC")
					{
					outputtotest.push_back(output);
					break;
					}
				}
			optimizationmodel.getvariabilities(outputtotest);
		}
	
#endif 
	return 0;
}

