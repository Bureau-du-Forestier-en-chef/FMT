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
		const std::vector<std::string>scenarios(1, "level");
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
		testmaps["TESTC"] = 1613.12;
		testmaps["TEST1"] = 72590.4;
		testmaps["TEST2"] = 1583.12;
		testmaps["TEST3"] = -1583.12;
		testmaps["TEST4"] = 1631.12;
		testmaps["TEST5"] = -19327.4;
		testmaps["TEST6"] = 19339.4;
		testmaps["TEST7"] = 402;
		testmaps["TEST8"] = -96757.2;
		testmaps["TEST9"] = (2.60216e+06 - 3.8656);
		testmaps["TEST10"] = 1;
		testmaps["TEST11"] = 30;
		testmaps["TEST12"] = 1684097.28;
		testmaps["TEST13"] = 871.0848;
		testmaps["TEST14"] = 3226.24;
		testmaps["TEST15"] = 193574.4;
		if (optimizationmodel.initialsolve())
			{
			for (const Core::FMToutput& output : optimizationmodel.getoutputs())
				{
				if (output.getname().find("TEST")!=std::string::npos)
					{
					const double value = optimizationmodel.getoutput(output, 1, Core::FMToutputlevel::totalonly).at("Total");
					if (1<std::abs(value-testmaps.at(output.getname())))
						{
						std::cout<< output.getname()<<" "<< std::abs(value - testmaps.at(output.getname())) <<"\n";
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

