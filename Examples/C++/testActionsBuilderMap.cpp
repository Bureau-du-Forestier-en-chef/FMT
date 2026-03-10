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
        std::string primarylocation;
        std::string scenario;
        int side;
        std::string	maplocation;
        std::string TARGET_YIELD;
        std::string ACTION_NAME;
        if (argc> 1)
        {
            const std::string vals = argv[1];
            std::vector<std::string>results;
            boost::split(results, vals, boost::is_any_of("|"));
            primarylocation = results.at(0);
            scenario = results.at(1);
            side = std::stoi(results.at(2));
            maplocation = results.at(3);
            TARGET_YIELD = argv[2];
            ACTION_NAME = argv[3];
        }else {
               primarylocation = "C:/Users/admlocal/Desktop/03_Carbone/03_Carbone/PC_9981_U06471_2028_MODB01.pri";
               scenario = "200_StratReg_apsp";
               side = 1420;
               maplocation = "Carte/PC_9981_UA_U06471.shp";
               TARGET_YIELD = "YV_S_MORT";
               ACTION_NAME = "TBE";
            }
        const boost::filesystem::path BASE_PATH = boost::filesystem::path(primarylocation).parent_path();
        const std::string MAPm_location = (BASE_PATH / boost::filesystem::path(maplocation)).string();
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
        Spatial::FMTforest forest = areaparser.vectormaptoFMTforest(MAPm_location,side,themes,"AGE","SUPERFICIE",1,0.0001, lockfield,0.0);
        areaparser.WriteForestExtended(forest,
            MAPm_location, themes, BUILDED_MODEL.getarea(), writelocation);

#endif
	return 0;
}
