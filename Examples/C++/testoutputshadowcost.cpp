﻿#ifdef FMTWITHOSI
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
	Logging::FMTdefaultlogger().logstamp();
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder + "TWD_land.pri";
		Parser::FMTmodelparser modelparser;
		const std::vector<std::string>scenarios(1, "LP2");
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		std::vector<std::string>outputnames;
		outputnames.push_back("OVOLREC");
		std::vector<Core::FMToutput>outputtotest;
		for (const Core::FMToutput& output : optimizationmodel.getoutputs())
		{
			if (std::find(outputnames.begin(), outputnames.end(), output.getname())!=outputnames.end())
			{
				outputtotest.push_back(output);
			}
		}
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
		const Graph::FMTgraphstats BASE_STATS = optimizationmodel.setobjective(objective);
		if (optimizationmodel.initialsolve())
		{
			for (const Core::FMToutput& output : outputtotest)
			{
			const double returnedvalue = optimizationmodel.getoutput(output,1,Core::FMToutputlevel::totalonly).at("Total");
			Logging::FMTdefaultlogger() << "Base value of " + output.getname() << " " << returnedvalue << " ";
			}
		Logging::FMTdefaultlogger() << "\n";
		Logging::FMTdefaultlogger() << "BASE ROWS OF " << BASE_STATS.rows << "\n";
		for (const Core::FMTconstraint& constraint : constraints)
			{
			const Graph::FMTgraphstats STATS = optimizationmodel.eraseconstraint(constraint);
			Logging::FMTdefaultlogger() << "ROWS Of "<< STATS.rows <<" "<< std::string(constraint) << "\n";
			if (!optimizationmodel.resolve())
				{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Cannot resolve when erasing "+ constraint.getname(),
					"testoutputsshadowcost", __LINE__, primarylocation);
				}
			for (const Core::FMToutput& output : outputtotest)
				{
				const double returnedvalue = optimizationmodel.getoutput(output, 1, Core::FMToutputlevel::totalonly).at("Total");
				Logging::FMTdefaultlogger() << std::string(constraint) + " value of " + output.getname() << " " << returnedvalue << " ";
				}
			Logging::FMTdefaultlogger() << "\n";
			optimizationmodel.setconstraint(constraint);
			if (!optimizationmodel.resolve())
				{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Cannot resolve when setting " + constraint.getname(),
					"testoutputsshadowcost", __LINE__, primarylocation);
				}
			}


		}
	
#endif 
	return 0;
}

