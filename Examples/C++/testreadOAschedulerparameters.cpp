#include "FMTareaparser.h"
#include "FMTmodelparser.h"
#include "FMTmodel.h"
#include "FMTdefaultlogger.h"
#ifdef FMTWITHOSI
    #include "FMToperatingareascheme.h"
#endif

int main(int argc, char *argv[])
    {   
#ifdef FMTWITHOSI
    Logging::FMTdefaultlogger().logstamp();
    const std::string primarylocation = std::string(argv[2]);
    const std::string scenario = std::string(argv[3]);
    const std::string fichierParam = std::string(argv[1]);
    Parser::FMTmodelparser modelparser;
    modelparser.setdefaultexceptionhandler();
    std::vector<Exception::FMTexc>errors;
    errors.push_back(Exception::FMTexc::FMTmissingyield);
    errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
    errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
    errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
    errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
    errors.push_back(Exception::FMTexc::FMToveridedyield);
    errors.push_back(Exception::FMTexc::FMToutofrangeyield);
    errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
    errors.push_back(Exception::FMTexc::FMTmissingyield);
    modelparser.seterrorstowarnings(errors);
    std::vector<std::string>scenarios(1, scenario);
    const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
    Parser::FMTareaparser areaParser;
    const std::vector<Heuristics::FMToperatingareascheme> opeareas = areaParser.readOAschedulerparameters(fichierParam, models.at(0).getthemes(), 13, 1);
#endif
    return 0;
	}

