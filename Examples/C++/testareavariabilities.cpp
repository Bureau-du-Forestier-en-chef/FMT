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
	Logging::FMTdefaultlogger().logstamp();
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
			std::vector<Core::FMTmask>globalmasks;
			const std::vector<Core::FMTtheme>themes = optimizationmodel.getthemes();
			globalmasks.emplace_back("? PEUPLEMENT2 ?",themes);
			globalmasks.emplace_back("? PEUPLEMENT3 ?",themes);
			std::vector<double>proportions;
			proportions.push_back(0.01);
			proportions.push_back(-0.10);
			const Models::FMTlpmodel newmodel = optimizationmodel.getmodelfromproportions(globalmasks, proportions);
			for (const Core::FMToutput& output : optimizationmodel.getoutputs())
				{
				if (output.getname()== "OVOLREC")
					{
					for (size_t period = 1; period <= 10; ++period)
						{
						Logging::FMTdefaultlogger() <<"At period "<<period<< " OLD value is " << newmodel.getoutput(output, 5, Core::FMToutputlevel::totalonly).at("Total") <<
							"NEW value is " << optimizationmodel.getoutput(output,period, Core::FMToutputlevel::totalonly).at("Total") << "\n";
						}
					break;
					}
				}
		}
	
#endif 
	return 0;
}

