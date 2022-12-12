#ifdef FMTWITHOSI
    #include "FMTareaparser.hpp"
	#include "FMTlpmodel.hpp"
	#include "FMTmodelparser.hpp"
	#include "FMTversion.hpp"
	#include "FMTlogger.hpp"
	#include "FMTconstraint.hpp"
    #include "FMToutputnode.hpp"
    #include "FMTmask.hpp"
	#include "FMTopareaschedulertask.hpp"
	#include "FMTtaskhandler.hpp"
    #include "FMToperatingareascheduler.hpp"
    #include "FMTtimeyieldhandler.hpp"
    #include "FMTfreeexceptionhandler.hpp"
#endif
#ifdef FMTWITHOSI
std::vector<Heuristics::FMToperatingareascheme> ObtenirOperatingArea(   const std::string& fichierShp,const std::vector<Core::FMTtheme>& themes, const int& numeroTheme,const int& startingperiod,
                                                                        const std::string& nomChampAge,const std::string& nomChampSuperficie,const std::string& nomChampStanlock
                                                                    )
    {
	
	    std::vector<Heuristics::FMToperatingareascheme> opeareas;
	    bool spatialconstraints = false;
        std::vector<std::string>operatingareasname = themes.at(numeroTheme-1).getattributes("?",false);
        for (const auto& OA : operatingareasname)
        {
            if (OA != "NA")
            {
                const int OPT = 2;
                const int RET = 6;//4;
                const int REP = 6;
                const float NPE = 0;
                const float GUP = 0;
                std::string mask;
                for (int theme = 1; theme <= themes.size(); ++theme)
                {
                    if (theme == numeroTheme)
                    {
                        mask += OA + " ";
                    }
                    else
                    {
                        mask += "? ";
                    }
                }
                const Core::FMTmask FMTmask = Core::FMTmask(mask, themes);
                opeareas.push_back(Heuristics::FMToperatingareascheme(
                    Heuristics::FMToperatingarea(FMTmask, NPE),
                    OPT,
                    RET,
                    RET,
                    REP,
                    GUP,
                    startingperiod,0.0));

                if (NPE > 0 || GUP > 0)
                {
                    spatialconstraints = true;
                }
            }
        }
        if (spatialconstraints)
        {
            Parser::FMTareaparser areaParser;
            Logging::FMTlogger()<<"Lecture des blocs voisins."<<"\n";
            opeareas = areaParser.getschemeneighbors(opeareas, themes, fichierShp, nomChampAge, nomChampSuperficie, 1.0, 1,nomChampStanlock);
        }
        return opeareas;
}

Core::FMToutputnode createBFECoptaggregate(Models::FMTmodel& model)   
    {
        std::string Agg_name = "~BFECOPTOUTPUTYOUVERT~";
            std::vector<Core::FMTaction> newactions;
            int youvert = 0;

            for (Core::FMTaction& action : model.getactions())
            {
                if (action.useyield("YOUVERT"))
                {
                    youvert += 1;
                    std::vector<std::string> agg = action.getaggregates();
                    if (std::count(agg.begin(), agg.end(), Agg_name))
                    {
                        Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "L'utilisateur à utiliser le nom ~BFECOPTOUTPUTYOUVERT~ dans ses outputs",
                        "testOAschedulerBFEC", __LINE__,model.getname());  
                    }

                    action.push_aggregate(Agg_name);
                }

                newactions.push_back(action);
            }

            if (youvert < 1)
            {
                Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Aucune action dans le modèle n'a de youvert",
                        "testOAschedulerBFEC", __LINE__,model.getname()) ;
            }

            model.setactions(newactions);
            const std::vector<Core::FMTtheme> themes = model.getthemes();	
            std::string stringMask = "";
            for (int i = 1; i <= themes.size(); i++)
            {
                if (stringMask == "")
                {
                    stringMask += "?";
                }
                else
                {
                    stringMask += " ?";
                }
            }

            Core::FMTmask fmtMask = Core::FMTmask(stringMask, themes);
            return Core::FMToutputnode(fmtMask, Agg_name);
    }
#endif
int main(int argc, char *argv[])
    {   
       
        #ifdef FMTWITHOSI
            Logging::FMTlogger().logstamp();
            const std::string primarylocation = std::string(argv[1]);
            const std::vector<std::string>scenarios(1,std::string(argv[2]));
            const std::string fichierShp = std::string(argv[3]);
            const std::string out("../../tests/testOAschedulertask/" + scenarios.at(0));
            Parser::FMTmodelparser modelparser;
            modelparser.setdefaultexceptionhandler();
            const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
            Models::FMTmodel model = models.at(0);
            Models::FMTlpmodel optimizationmodel(model, Models::FMTsolverinterface::MOSEK);
            optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH,5);
	        optimizationmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
            const int startingperiod = optimizationmodel.getconstraints().at(0).getperiodlowerbound();
            const Core::FMToutputnode nodeofoutput =  createBFECoptaggregate(optimizationmodel);
            const std::vector<Heuristics::FMToperatingareascheme> opeareas = ObtenirOperatingArea(fichierShp,optimizationmodel.getthemes(),14, startingperiod, "AGE", "SUPERFICIE", "STANLOCK");
            std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTopareaschedulertask(optimizationmodel, opeareas, nodeofoutput,out, "YOUVERT",10,120));
			Parallel::FMTtaskhandler handler(maintaskptr,4);
            handler.settasklogger();
			handler.conccurentrun();
		#endif 
        return 0;
	}

