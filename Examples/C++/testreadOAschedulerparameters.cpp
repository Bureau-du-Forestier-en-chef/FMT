#include "FMTAreaParser.h"
#include "FMTModelParser.h"
#include "FMTModel.h"
#include "FMTDefaultLogger.h"
#ifdef FMTWITHOSI
    #include "FMTOperatingAreaScheme.h"
#endif

int main(int argc, char *argv[])
    {   
#ifdef FMTWITHOSI
    Logging::FMTDefaultLogger().logStamp();
    const std::string primarylocation = std::string(argv[2]);
    const std::string scenario = std::string(argv[3]);
    const std::string fichierParam = std::string(argv[1]);
    Parser::FMTModelParser modelparser;
    modelparser.setDefaultExceptionHandler();
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
    modelparser.setErrorsToWarnings(errors);
    std::vector<std::string>scenarios(1, scenario);
    const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
    Parser::FMTAreaParser areaParser;
    const std::vector<Heuristics::FMTOperatingAreaScheme> opeareas = areaParser.readOaSchedulerParameters(fichierParam, models.at(0).getThemes(), 13, 1);
#endif
    return 0;
	}

