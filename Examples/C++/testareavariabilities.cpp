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
			std::vector<Core::FMTMask>globalmasks;
			const std::vector<Core::FMTTheme>themes = optimizationmodel.getThemes();
			globalmasks.emplace_back("? PEUPLEMENT2 ?",themes);
			globalmasks.emplace_back("? PEUPLEMENT3 ?",themes);
			std::vector<double>proportions;
			proportions.push_back(0.01);
			proportions.push_back(-0.10);
			const Models::FMTLpModel newmodel = optimizationmodel.getModelFromProportions(globalmasks, proportions);
			for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
				{
				if (output.getName()== "OVOLREC")
					{
					for (size_t period = 1; period <= 10; ++period)
						{
						Logging::FMTDefaultLogger() <<"At period "<<period<< " OLD value is " << newmodel.getOutput(output, 5, Core::FMToutputlevel::totalonly).at("Total") <<
							"NEW value is " << optimizationmodel.getOutput(output,period, Core::FMToutputlevel::totalonly).at("Total") << "\n";
						}
					break;
					}
				}
		}
	
#endif 
	return 0;
}

