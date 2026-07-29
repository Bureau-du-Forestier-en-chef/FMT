#include <vector>
#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
	#include "FMTModelParser.h"
	#include "FMTVersion.h"
	#include "FMTDefaultLogger.h"
	#include "FMTConstraint.h"
	#include "FMTScheduleParser.h"
	#include "FMTFreeExceptionHandler.h"
#endif


int main()
	{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTVersion().hasFeature("OSI"))
		{
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder+"TWD_land.pri";
		Parser::FMTModelParser modelparser;
		const std::vector<std::string>scenarios(1, "level");
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		for (size_t period = 0; period < 1; ++period)
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
		testmaps["TEST81"] = -12868.96;
		testmaps["TEST82"] = 16167.2;
		testmaps["TEST83"] = -4719.36;
		testmaps["TEST16"] = 167.75;
		testmaps["TEST17"] = 590.73;
		testmaps["TEST18"] = 48099.16;
		testmaps["TEST19"] = 1182.09;
		testmaps["TEST20"] = 16778423.81;
		testmaps["TEST21"] = 62;
		//testmaps["TEST22"] = 0;
		testmaps["TEST23"] = 838.750372;
		//testmaps["TEST23"] = 62;
		//testmaps["TEST22"] = 0;
		//modelparser.write(optimizationmodel,"D:/test/");
		if (optimizationmodel.initialSolve())
			{
			for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
				{
				if (testmaps.find(output.getName())!= testmaps.end())
					{
					const double value = optimizationmodel.getOutput(output, 1, Core::FMToutputlevel::totalonly).at("Total");
					if (1<std::abs(value-testmaps.at(output.getName())))
						{
						std::cout<< output.getName()<<" "<< std::abs(value - testmaps.at(output.getName())) <<"\n";
						Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
							"FMTleveltest", __LINE__, primarylocation);
						}

					}

				}
			

			}
	}else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
		}
#endif 
	return 0;
	}

