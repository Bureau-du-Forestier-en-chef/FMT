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
			std::vector<Core::FMTMask>globalmasks;
			const std::vector<Core::FMTTheme>themes = optimizationmodel.getThemes();
			globalmasks.emplace_back("? PEUPLEMENT2 ?",themes);
			globalmasks.emplace_back("? PEUPLEMENT3 ?",themes);
			std::vector<double>proportions;
			proportions.push_back(0.01);
			proportions.push_back(-0.10);
			const Models::FMTlpmodel newmodel = optimizationmodel.getModelFromProportions(globalmasks, proportions);
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

