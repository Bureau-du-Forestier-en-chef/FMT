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
		scenarios.push_back("stdconstraints");
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel lpmodel(models.at(0),Models::FMTSolverInterface::CLP);
		lpmodel.doPlanning(true);
	}else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
		}
#endif 
	return 0;
	}

