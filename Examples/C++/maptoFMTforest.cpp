#include <vector>
#include "FMTforest.h"
#include "FMTmodelparser.h"
#include "FMTareaparser.h"
#include "FMTversion.h"
#include "FMTlogger.h"
#include "FMTareaparser.h"
#include "FMTforest.h"
#include "FMTmodel.h"
#include "FMTtheme.h"

int main()
{
    Logging::FMTlogger().logstamp();
    #if defined _MSC_VER
        const std::string modellocation = "../Examples/Models/TWD_land/";
    #else
        const std::string modellocation = "Examples/Models/TWD_land/";
    #endif
    const std::string	primarylocation = modellocation + "TWD_land.pri";
    const std::string	maplocation = modellocation + "/Carte/TWD_land.shp";
    const std::string	writelocation = modellocation + "rasters/";
    Parser::FMTmodelparser mparser;
    const std::vector<std::string>scenarios(1, "ROOT");
    const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
    const std::vector<Core::FMTtheme> themes = models.at(0).getthemes();
    Parser::FMTareaparser areaparser;
    areaparser.setdefaultexceptionhandler();
    Spatial::FMTforest forest = areaparser.vectormaptoFMTforest(maplocation,1420,themes,"AGE","SUPERFICIE",1,0.0001,"",0.0,writelocation,false);
    return 0;
}
