#ifdef FMTWITHOSI
    #include "FMTareaparser.h"
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTconstraint.h"
    #include "FMToutputnode.h"
    #include "FMTmask.h"
	#include "FMTopareaschedulertask.h"
	#include "FMTtaskhandler.h"
    #include "FMToperatingareascheduler.h"
    #include "FMTtimeyieldhandler.h"
    #include "FMTfreeexceptionhandler.h"
    #include "boost/filesystem.hpp"
#endif
#ifdef FMTWITHOSI

std::vector<Heuristics::FMTOperatingAreaScheme> ObtenirOperatingArea(
    const std::string& fichierShp,
    const std::vector<Core::FMTTheme>& themes,
    const int& numeroTheme,
    const int& startingperiod,
    const std::string& nomChampAge,
    const std::string& nomChampSuperficie,
    const std::string& nomChampStanlock,
    const std::string& fichierParam)
    {
        Parser::FMTAreaParser areaParser;
        std::vector<Heuristics::FMTOperatingAreaScheme> opeareas = areaParser.readOAschedulerparameters(
            fichierParam,
            themes,
            numeroTheme - 1,
            startingperiod);
        for (const auto& op : opeareas) 
        {
            if (op.getNeighborsPerimeter() > 0 || op.getGreenUp() > 0)
            {
                Logging::FMTDefaultLogger() << "Lecture des blocs voisins." << "\n";
                opeareas = areaParser.getSchemeNeighbors(
                    opeareas, 
                    themes, 
                    fichierShp, 
                    nomChampAge, 
                    nomChampSuperficie, 
                    1.0, 
                    1, 
                    nomChampStanlock);
                return opeareas;
            }
        }
        return opeareas;
    }

#endif
int main(int argc, char *argv[])
    {   
     #ifdef FMTWITHOSI
     Logging::FMTDefaultLogger().logStamp();
     std::string primarylocation;
     std::vector<std::string> results;
     std::vector<std::string> scenarios;
     std::string lfichierParam;
     std::string fichierShp;
     if (argc > 1)
            {
            primarylocation = std::string(argv[1]);
            const std::string vals = argv[2];
            boost::split(results, vals, boost::is_any_of("|"));
            scenarios = std::vector<std::string> (1, results.at(0));
            boost::filesystem::path primpath(primarylocation);
            const boost::filesystem::path basefolder = primpath.parent_path();
            lfichierParam = basefolder.string() + "/Scenarios/" + results.at(0) + "/" + results.at(1);
            fichierShp = std::string(argv[3]);
      }else{
        primarylocation = "D:/02_Travail_Realisme/02_Travail_Realisme/PC_9943_U08651_2028_MODB01.pri";
        scenarios = std::vector<std::string>(1, "323_TYFSansPre_avsp");
        boost::filesystem::path primpath(primarylocation);
        const boost::filesystem::path basefolder = primpath.parent_path();
        lfichierParam = "D:/02_Travail_Realisme/02_Travail_Realisme/Parametres_Bfecopt.csv";
        fichierShp = "D:/02_Travail_Realisme/02_Travail_Realisme/Carte/PC_9943_UA_U08651.shp";
        results = std::vector<std::string>(1, "323_TYFSansPre_avsp");
        }
     Parser::FMTModelParser modelparser;
     modelparser.setDefaultExceptionHandler();
                std::vector<Exception::FMTexc> errors;
                errors.push_back(Exception::FMTexc::FMTmissingyield);
                errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
                errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
                errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
                errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
                errors.push_back(Exception::FMTexc::FMToveridedyield);
                errors.push_back(Exception::FMTexc::FMToutofrangeyield);
                errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
                errors.push_back(Exception::FMTexc::FMTmissingyield);
                errors.push_back(Exception::FMTexc::FMTEmptyOA);
                errors.push_back(Exception::FMTexc::FMTdeathwithlock);
                modelparser.setErrorsToWarnings(errors);
                const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
                Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
                const int startingperiod = optimizationmodel.getParameter(Models::FMTintmodelparameters::UPDATE);
               
                const std::vector<Heuristics::FMTOperatingAreaScheme> opeareas = ObtenirOperatingArea(
                    fichierShp,
                    optimizationmodel.getThemes(),
                    14,
                    startingperiod,
                    "AGE",
                    "SUPERFICIE",
                    "STANLOCK",
                    lfichierParam);
                size_t neighborsCount = 0;
                for (const auto& OPArea : opeareas)
                    {
                    neighborsCount += OPArea.getNeighbors().size();
                    }
                const size_t TESTED_VALUE = static_cast<size_t>(std::stoi(results[2]));
                if (neighborsCount != TESTED_VALUE)
                    {
                    std::cout << neighborsCount << "!=" << TESTED_VALUE << "\n";
                    Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed,
                        "Wrong number of neighbors!",
                        "testOAAdjacency", __LINE__, __FILE__);
                    }
            #endif 
    return 0;
	}

