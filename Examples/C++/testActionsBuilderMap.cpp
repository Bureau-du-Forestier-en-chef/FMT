#include <vector>
#ifdef FMTWITHGDAL
    #include "FMTforest.h"
    #include "FMTmodelparser.h"
    #include "FMTareaparser.h"
    #include "FMTversion.h"
    #include "FMTdefaultlogger.h"
    #include "FMTareaparser.h"
    #include "FMTforest.h"
    #include "FMTmodel.h"
    #include "FMTtheme.h"
    #include <boost/filesystem.hpp>
#endif

int main(int argc, char* argv[])
{
    #ifdef FMTWITHGDAL
        Logging::FMTdefaultlogger().logstamp();
        const std::string vals = argv[1];
        std::vector<std::string>results;
        boost::split(results, vals, boost::is_any_of("|"));
        const std::string primarylocation = results.at(0);
        const std::string scenario = results.at(1);
        const int side = std::stoi(results.at(2));
        const std::string	maplocation = results.at(3);
        const std::string TARGET_YIELD = argv[2];
        const std::string ACTION_NAME = argv[3];
        /*const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/Prototype_Dec2023_TBE/PC_7002071_UA08152_FINAL.pri";
        const std::string scenario = "20_Regl_prov";
        const int side = 1420;
        const std::string	maplocation = "Carte/PC_7002071_UA_U08152.shp";
        const std::string TARGET_YIELD = "YV_S_MORT";
        const std::string ACTION_NAME = "TBE";*/
        const boost::filesystem::path BASE_PATH = boost::filesystem::path(primarylocation).parent_path();
        const std::string MAP_LOCATION = (BASE_PATH / boost::filesystem::path(maplocation)).string();
        const std::string	writelocation =  "../../tests/testActionsBuilderMap/";
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
        errors.push_back(Exception::FMTexc::FMTunclosedforloop);
        errors.push_back(Exception::FMTexc::FMToutofrangeyield);
        errors.push_back(Exception::FMTexc::FMTdeathwithlock);
        errors.push_back(Exception::FMTexc::FMTempty_schedules);
        errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
        mparser.seterrorstowarnings(errors);
        const std::vector<std::string>scenarios(1, scenario);
        const std::vector<Models::FMTmodel> MODELS = mparser.readproject(primarylocation, scenarios);
        const Models::FMTmodel BUILDED_MODEL = MODELS.at(0).buildAction(ACTION_NAME, TARGET_YIELD);
        const std::vector<Core::FMTtheme> themes = BUILDED_MODEL.getthemes();
        Parser::FMTareaparser areaparser;
        //areaparser.setdefaultexceptionhandler();
        Spatial::FMTforest forest = areaparser.vectormaptoFMTforest(MAP_LOCATION,side,themes,"AGE","SUPERFICIE",1,0.0001, lockfield,0.0,writelocation,false);
    #endif
	return 0;
}
