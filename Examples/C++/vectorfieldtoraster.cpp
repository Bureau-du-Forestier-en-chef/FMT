
#ifdef FMTWITHGDAL
#include "FMTareaparser.hpp"
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTareaparser.hpp"
#endif

int main()
{
    #ifdef FMTWITHGDAL
        Logging::FMTlogger().logstamp();
        const std::string modellocation = "../../../../Examples/Models/TWD_land/";
        const std::string	maplocation = modellocation + "/Carte/TWD_land.shp";
        const std::string	writelocation = "../../tests/vectorfieldtoraster/";
        Parser::FMTareaparser areaparser;
        areaparser.vectorfieldtoraster(maplocation,writelocation+"AGE.tif",1420,"AGE",true);
        areaparser.vectorfieldtoraster(maplocation,writelocation+"THEME1.tif",1420,"THEME1",true);
    #endif
	return 0;
}
