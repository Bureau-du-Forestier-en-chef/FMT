#ifdef FMTWITHOSI
#include <vector>
#include "FMTLpModel.h"
#include "FMTModelParser.h"
#include "FMTVersion.h"
#include "FMTDefaultLogger.h"
#include "FMTConstraint.h"
#include "FMTScheduleParser.h"
#include "FMTMask.h"
#include "FMTOutputNode.h"
#include "FMTTheme.h"
#include "FMTFreeExceptionHandler.h"
#include <string>
#endif


int main()
{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder + "TWD_land.pri";
		Parser::FMTModelParser modelparser;
		const std::vector<std::string>scenarios(1, "LP2");
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTSolverInterface::CLP);
		std::vector<std::string>outputnames;
		outputnames.push_back("OVOLREC");
		std::vector<Core::FMTOutput>outputtotest;
		for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
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
		std::vector<Core::FMTConstraint>constraints = optimizationmodel.getConstraints();
		const Core::FMTConstraint objective = constraints.at(0);
		constraints.erase(constraints.begin());
		for (const Core::FMTConstraint& constraint : constraints)
		{
			optimizationmodel.setConstraint(constraint);
		}
		const Graph::FMTGraphStats BASE_STATS = optimizationmodel.setObjective(objective);
		if (optimizationmodel.initialSolve())
		{
			for (const Core::FMTOutput& output : outputtotest)
			{
			const double returnedvalue = optimizationmodel.getOutput(output,1,Core::FMToutputlevel::totalonly).at("Total");
			Logging::FMTDefaultLogger() << "Base value of " + output.getName() << " " << returnedvalue << " ";
			}
		Logging::FMTDefaultLogger() << "\n";
		Logging::FMTDefaultLogger() << "BASE ROWS OF " << BASE_STATS.rows << "\n";
		for (const Core::FMTConstraint& constraint : constraints)
			{
			const Graph::FMTGraphStats STATS = optimizationmodel.eraseConstraint(constraint);
			Logging::FMTDefaultLogger() << "ROWS Of "<< STATS.rows <<" "<< std::string(constraint) << "\n";
			if (!optimizationmodel.resolve())
				{
				Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Cannot resolve when erasing "+ constraint.getName(),
					"testoutputsshadowcost", __LINE__, primarylocation);
				}
			for (const Core::FMTOutput& output : outputtotest)
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

