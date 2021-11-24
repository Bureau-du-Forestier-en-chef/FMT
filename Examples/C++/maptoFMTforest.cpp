#include <vector>
#ifdef FMTWITHGDAL
#include "FMTforest.hpp"
#include "FMTmodelparser.hpp"
#include "FMTareaparser.hpp"
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTareaparser.hpp"
#include "FMTforest.hpp"
#include "FMTmodel.hpp"
#include "FMTtheme.hpp"
#endif

int main()
{
#ifdef FMTWITHGDAL
    Logging::FMTlogger().logstamp();
    const std::string modellocation = "../../../../Examples/Models/TWD_land/";
    const std::string	primarylocation = modellocation + "TWD_land.pri";
    const std::string	maplocation = modellocation + "/Carte/TWD_land.shp";
    const std::string	writelocation = "tests/";
    Parser::FMTmodelparser mparser;
    const std::vector<std::string>scenarios(1, "ROOT");
    const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
    const std::vector<Core::FMTtheme> themes = models.at(0).getthemes();
    Parser::FMTareaparser areaparser;
    areaparser.setdefaultexceptionhandler();
    Spatial::FMTforest forest = areaparser.vectormaptoFMTforest(maplocation,1420,themes,"AGE","SUPERFICIE",1,0.0001,"",0.0,writelocation,false);
#endif
	return 0;
}
