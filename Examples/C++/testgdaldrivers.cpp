#include <vector>
#include <algorithm>
#ifdef FMTWITHGDAL
	#include "FMTareaparser.hpp"
	#include "FMTfreeexceptionhandler.hpp"
#endif

int main(int argc, char* argv[])
{
	#ifdef FMTWITHGDAL
		Logging::FMTlogger().logstamp();
		const std::string DriverType =  argv[1];
		const std::string DriverName =  argv[2];
		const std::string DriverExtension =  argv[3];
		Parser::FMTareaparser AreaParser;
		std::vector<std::string>Drivers;
		std::vector<std::string>Extensions;
		if (DriverType == "vector")
			{
				Drivers = AreaParser.getGDALvectordrivernames();
				std::vector<std::vector<std::string>> LocalExtensions = AreaParser.getGDALvectordriverextensions();
				for (const std::vector<std::string>& ExtV : LocalExtensions)
				{
					for (const std::string& Ext : ExtV)
					{
						Extensions.push_back(Ext);
					}
				}
			}else{
				Drivers = AreaParser.getGDALrasterdrivernames();
				std::vector<std::vector<std::string>> LocalExtensions = AreaParser.getGDALrasterdriverextensions();
				for (const std::vector<std::string>& ExtV : LocalExtensions)
				{
					for (const std::string& Ext : ExtV)
					{
						Extensions.push_back(Ext);
					}
				}
			}
		if (std::find(Drivers.begin(),Drivers.end(),DriverName)==Drivers.end())
			{
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Cant find driver "+DriverName,
				"testgdaldrivers", __LINE__,__FILE__);
			}
		if (std::find(Extensions.begin(), Extensions.end(), DriverExtension) == Extensions.end())
			{
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Cant find driver extension " + DriverExtension,
				"testgdaldrivers", __LINE__, __FILE__);
			}

	#endif
	return 0;
}
