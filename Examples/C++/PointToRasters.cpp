#include <vector>
#ifdef FMTWITHGDAL
#include "FMTactualdevelopment.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTareaparser.h"
#include "FMTlandscapeparser.h"
#include "FMTforest.h"
#include "FMTtheme.h"
#include "FMTconstants.h"
#endif

int main(int argc, char* argv[])
{
#ifdef FMTWITHGDAL
    Logging::FMTdefaultlogger().logstamp();
    int side;//196
    std::string	themesLocation;
    std::string	pointsLocation;
    std::string	writeLocation;
    if (argc > 1)
    {
        std::string vals = argv[1];
        std::vector<std::string>results;
        boost::split(results, vals, boost::is_any_of("|"));
        pointsLocation = results.at(0);
        themesLocation = results.at(1);
        side = std::stoi(results.at(2));//196
        writeLocation = argv[2];// "../../tests/maptoFMTforest/";
    }else {
        side = 1420;
        themesLocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/PointToRasters/Sqlite.lan";
        pointsLocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/PointToRasters/V2Forest.xml";
        writeLocation= "../../tests/PointToRasters/";
    }
    const std::string LOCK_FIELD = "STANLOCK";

    Parser::FMTareaparser areaparser;
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
    areaparser.seterrorstowarnings(errors);

    Parser::FMTlandscapeparser landScapeParse;
    const std::vector<Core::FMTtheme> THEMES = landScapeParse.read(Core::FMTconstants(), themesLocation);
   
    const Spatial::FMTforest FOREST = areaparser.vectormaptoFMTforest(pointsLocation,side, THEMES,"AGE","SUPERFICIE",
                                                                    1,0.0001, LOCK_FIELD,0.0,writeLocation,false);
    areaparser.write(FOREST.getarea(), writeLocation + "area.are");
#endif
	return 0;
}
