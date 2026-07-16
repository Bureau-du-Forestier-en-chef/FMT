#ifdef FMTWITHOSI
#include <vector>
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTconstraint.h"
#include "FMTscheduleparser.h"
#include "FMTmask.h"
#include "FMToutputnode.h"
#include "FMTtheme.h"
#include <string>
#endif


int main()
{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder + "TWD_land.pri";
		Parser::FMTModelParser modelparser;
		const std::vector<std::string>scenarios(1, "LP");
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
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
			std::vector<Core::FMTOutput>outputtotest;
			for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
				{
				if (output.getName()== "OSUPREC")
					{
					outputtotest.push_back(output);
					break;
					}
				}
			optimizationmodel.getVariabilities(outputtotest,1,10);
		}
	
#endif 
	return 0;
}

