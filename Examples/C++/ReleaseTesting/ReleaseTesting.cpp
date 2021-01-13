// Write your own code to test FMT
#include <vector>
#include "FMTsesmodel.h"
#include "FMTlpmodel.h"
#include "FMTsamodel.h"
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTlogger.h"


int main()
	{
	Logging::FMTlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
		{
		#if defined _MSC_VER
			const std::string primarylocation = "../Examples/Models/TWD_land/TWD_land.pri";
		#else
			const std::string primarylocation = "Examples/Models/TWD_land/TWD_land.pri";
		#endif
		Parser::FMTmodelparser modelparser;
		const std::vector<std::string> scenarios {"LP","LPwfire","Spatial"};
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}

	return 0;
	}
