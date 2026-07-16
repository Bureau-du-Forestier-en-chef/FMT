#include <vector>
#ifdef FMTWITHOSI
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTconstraint.h"
	#include "FMTscheduleparser.h"
	#include "FMTfreeexceptionhandler.h"
#endif


int main()
	{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTversion().hasFeature("OSI"))
		{
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder+"TWD_land.pri";
		Parser::FMTModelParser modelparser;
		const std::vector<std::string>scenarios(1, "timeyieldoutput");
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		for (size_t period = 0; period < 1; ++period)
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
		std::map<std::string, double>testmaps;
		testmaps["TEST1"] = 3226.24;
		testmaps["TEST2"] = 64524.8;
		if (optimizationmodel.initialSolve())
			{
			for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
				{
				if (output.getName().find("TEST")!=std::string::npos)
					{
					const double value = optimizationmodel.getOutput(output, 1, Core::FMToutputlevel::totalonly).at("Total");
					if (1<std::abs(value-testmaps.at(output.getName())))
						{
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

