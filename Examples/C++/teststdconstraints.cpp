#include <vector>
#ifdef FMTWITHOSI
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTversion.h"
	#include "FMTlogger.h"
	#include "FMTconstraint.h"
	#include "FMTscheduleparser.h"
	#include "FMTfreeexceptionhandler.h"
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
		std::vector<std::string>scenarios;
		scenarios.push_back("stdconstraints");
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel lpmodel(models.at(0),Models::FMTsolverinterface::CLP);
		lpmodel.doplanning(true);
	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
#endif 
	return 0;
	}

