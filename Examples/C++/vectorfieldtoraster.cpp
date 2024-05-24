
#ifdef FMTWITHGDAL
#include "FMTareaparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTareaparser.h"
#endif

int main()
{
    #ifdef FMTWITHGDAL
        Logging::FMTdefaultlogger().logstamp();
        const std::string modellocation = "../../../../Examples/Models/TWD_land/";
        const std::string	maplocation = modellocation + "/Carte/TWD_land.shp";
        const std::string	writelocation = "../../tests/vectorfieldtoraster/";
        Parser::FMTareaparser areaparser;
        areaparser.vectorfieldtoraster(maplocation,writelocation+"AGE.tif",1420,"AGE",true);
        areaparser.vectorfieldtoraster(maplocation,writelocation+"THEME1.tif",1420,"THEME1",true);
    #endif
	return 0;
}
