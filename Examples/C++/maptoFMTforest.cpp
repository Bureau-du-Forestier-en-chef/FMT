#include <vector>
#ifdef FMTWITHGDAL
#include "FMTforest.h"
#include "FMTmodelparser.h"
#include "FMTareaparser.h"
#include "FMTversion.h"
#include "FMTlogger.h"
#include "FMTareaparser.h"
#include "FMTforest.h"
#include "FMTmodel.h"
#include "FMTtheme.h"
#endif

int main(int argc, char* argv[])
{
#ifdef FMTWITHGDAL
    Logging::FMTlogger().logstamp();
    const std::string vals = argv[1];
    std::vector<std::string>results;
    boost::split(results, vals, boost::is_any_of("|"));
    const std::string primarylocation = results.at(0);
    const std::string scenario = results.at(1);
    const int side = std::stoi(results.at(2));
    const std::string	maplocation = argv[2];// modellocation + "/Carte/TWD_land.shp";
    const std::string	writelocation = argv[3];// "../../tests/maptoFMTforest/";
    std::string lockfield;
    if (primarylocation.find("TWD_land")==std::string::npos)
    {
        lockfield = "STANLOCK";
    }
    Parser::FMTmodelparser mparser;
    std::vector<Exception::FMTexc>errors;
    errors.push_back(Exception::FMTexc::FMTmissingyield);
    errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
    errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
    errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
    errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
    errors.push_back(Exception::FMTexc::FMToveridedyield);
    errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
    errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
    errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
    mparser.seterrorstowarnings(errors);
    const std::vector<std::string>scenarios(1, scenario);
    const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
    const std::vector<Core::FMTtheme> themes = models.at(0).getthemes();
    Parser::FMTareaparser areaparser;
    //areaparser.setdefaultexceptionhandler();
    Spatial::FMTforest forest = areaparser.vectormaptoFMTforest(maplocation,side,themes,"AGE","SUPERFICIE",1,0.0001, lockfield,0.0,writelocation,false);
#endif
	return 0;
}
