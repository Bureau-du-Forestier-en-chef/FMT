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
#include "FMTfreeexceptionhandler.h"
#include <string>
#endif


int main()
{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder + "TWD_land.pri";
		Parser::FMTmodelparser modelparser;
		const std::vector<std::string>scenarios(1, "LP2");
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		std::vector<std::string>outputnames;
		outputnames.push_back("OVOLREC");
		std::vector<Core::FMToutput>outputtotest;
		for (const Core::FMToutput& output : optimizationmodel.getOutputs())
		{
			if (std::find(outputnames.begin(), outputnames.end(), output.getName())!=outputnames.end())
			{
				outputtotest.push_back(output);
			}
		}
		for (size_t period = 0; period < 10; ++period)
		{
			optimizationmodel.buildPeriod();
		}
		std::vector<Core::FMTconstraint>constraints = optimizationmodel.getconstraints();
		const Core::FMTconstraint objective = constraints.at(0);
		constraints.erase(constraints.begin());
		for (const Core::FMTconstraint& constraint : constraints)
		{
			optimizationmodel.setConstraint(constraint);
		}
		const Graph::FMTgraphstats BASE_STATS = optimizationmodel.setObjective(objective);
		if (optimizationmodel.initialSolve())
		{
			for (const Core::FMToutput& output : outputtotest)
			{
			const double returnedvalue = optimizationmodel.getOutput(output,1,Core::FMToutputlevel::totalonly).at("Total");
			Logging::FMTDefaultLogger() << "Base value of " + output.getName() << " " << returnedvalue << " ";
			}
		Logging::FMTDefaultLogger() << "\n";
		Logging::FMTDefaultLogger() << "BASE ROWS OF " << BASE_STATS.rows << "\n";
		for (const Core::FMTconstraint& constraint : constraints)
			{
			const Graph::FMTgraphstats STATS = optimizationmodel.eraseConstraint(constraint);
			Logging::FMTDefaultLogger() << "ROWS Of "<< STATS.rows <<" "<< std::string(constraint) << "\n";
			if (!optimizationmodel.resolve())
				{
				Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Cannot resolve when erasing "+ constraint.getName(),
					"testoutputsshadowcost", __LINE__, primarylocation);
				}
			for (const Core::FMToutput& output : outputtotest)
				{
				const double returnedvalue = optimizationmodel.getOutput(output, 1, Core::FMToutputlevel::totalonly).at("Total");
				Logging::FMTDefaultLogger() << std::string(constraint) + " value of " + output.getName() << " " << returnedvalue << " ";
				}
			Logging::FMTDefaultLogger() << "\n";
			optimizationmodel.setConstraint(constraint);
			if (!optimizationmodel.resolve())
				{
				Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Cannot resolve when setting " + constraint.getName(),
					"testoutputsshadowcost", __LINE__, primarylocation);
				}
			}


		}
	
#endif 
	return 0;
}

