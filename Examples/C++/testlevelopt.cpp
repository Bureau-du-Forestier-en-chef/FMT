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
		std::vector<std::string>scenarios;
		scenarios.push_back("levelopt1");
		scenarios.push_back("levelopt2");
		scenarios.push_back("levelopt3");
		scenarios.push_back("levelopt4");
		scenarios.push_back("levelopt5");
		scenarios.push_back("levelopt6");
		scenarios.push_back("levelopt7");
		scenarios.push_back("levelopt8");
		scenarios.push_back("levelopt9");
		std::vector<double>objectivevalues;
		objectivevalues.push_back(15485.952);
		objectivevalues.push_back(1000);
		objectivevalues.push_back(2580.992);
		objectivevalues.push_back(50);
		objectivevalues.push_back(907.38);
		objectivevalues.push_back(862.18);
		objectivevalues.push_back(856.88);
		objectivevalues.push_back(1814.76);
		objectivevalues.push_back(907.38);
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		for (size_t scnid = 0 ; scnid < models.size();++scnid)
		{
			Models::FMTLpModel optimizationmodel(models.at(scnid), Models::FMTsolverinterface::CLP);
			size_t scenariolength = 1;
			if (scnid>=4)
				{
				scenariolength = 2;
				}
			for (size_t period = 0; period < scenariolength; ++period)
			{
				optimizationmodel.buildPeriod();
			}
			std::vector<Core::FMTConstraint>constraints = optimizationmodel.getConstraints();
			const Core::FMTConstraint objective = constraints.at(0);
			constraints.erase(constraints.begin());
			for (const Core::FMTConstraint& constraint : constraints)
			{
				const std::string cval = std::string(constraint);
				optimizationmodel.setConstraint(constraint);
			}
			optimizationmodel.setObjective(objective);
			if (optimizationmodel.initialSolve())
			{
				const double value = optimizationmodel.getObjValue();
				if (1 < std::abs(value - objectivevalues.at(scnid)))
				{
					
					Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value "+std::to_string(std::abs(value - objectivevalues.at(scnid))),
						"FMTleveltest", __LINE__, primarylocation);
				}
			}
		}
		
	}else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
		}
#endif 
	return 0;
	}

