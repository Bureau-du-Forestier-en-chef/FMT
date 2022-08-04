#include <vector>
#ifdef FMTWITHOSI
	#include "FMTlpmodel.hpp"
	#include "FMTmodelparser.hpp"
	#include "FMTversion.hpp"
	#include "FMTlogger.hpp"
	#include "FMTconstraint.hpp"
	#include "FMTscheduleparser.hpp"
	#include "FMTfreeexceptionhandler.hpp"
#endif


int main()
	{
#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
		{
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder+"TWD_land.pri";
		Parser::FMTmodelparser modelparser;
		const std::vector<std::string>scenarios(1, "timeyieldoutput");
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		for (size_t period = 0; period < 1; ++period)
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
		std::map<std::string, double>testmaps;
		testmaps["TEST1"] = 3226.24;
		testmaps["TEST2"] = 64524.8;
		if (optimizationmodel.initialsolve())
			{
			for (const Core::FMToutput& output : optimizationmodel.getoutputs())
				{
				if (output.getname().find("TEST")!=std::string::npos)
					{
					const double value = optimizationmodel.getoutput(output, 1, Core::FMToutputlevel::totalonly).at("Total");
					if (1<std::abs(value-testmaps.at(output.getname())))
						{
						Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
							"FMTleveltest", __LINE__, primarylocation);
						}

					}

				}
			

			}
	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
#endif 
	return 0;
	}

