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
    #include "boost/filesystem.hpp"
#endif
#ifdef FMTWITHOSI

std::vector<Heuristics::FMToperatingareascheme> ObtenirOperatingArea(   const std::string& fichierShp,const std::vector<Core::FMTtheme>& themes, const int& numeroTheme,const int& startingperiod,
                                                                        const std::string& nomChampAge,const std::string& nomChampSuperficie,const std::string& nomChampStanlock, const std::string& fichierParam
                                                                    )
    {
        Parser::FMTareaparser areaParser;
        std::vector<Heuristics::FMToperatingareascheme> opeareas = areaParser.readOAschedulerparameters(fichierParam,themes,numeroTheme-1,startingperiod);
        for (const auto& op : opeareas) 
        {
            if (op.getneihgborsperimeter() > 0 || op.getgreenup() > 0)
            {
                Logging::FMTlogger() << "Lecture des blocs voisins." << "\n";
                opeareas = areaParser.getschemeneighbors(opeareas, themes, fichierShp, nomChampAge, nomChampSuperficie, 1.0, 1, nomChampStanlock);
                return opeareas;
            }
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
            const std::string vals = argv[2];
            std::vector<std::string>results;
            boost::split(results, vals, boost::is_any_of("|"));
            const std::vector<std::string>scenarios(1, results.at(0));
            boost::filesystem::path primpath(primarylocation);
            const std::string filename = primpath.stem().string();
            const boost::filesystem::path basefolder = primpath.parent_path();
            const std::string lfichierParam = basefolder.string() + "/Scenarios/" + results.at(0) + "/" + results.at(1);
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
            const std::vector<Heuristics::FMToperatingareascheme> opeareas = ObtenirOperatingArea(fichierShp,optimizationmodel.getthemes(),14, startingperiod, "AGE", "SUPERFICIE", "STANLOCK", lfichierParam);
            {
                std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTopareaschedulertask(optimizationmodel, opeareas, nodeofoutput,out, "YOUVERT",10,120));
                Parallel::FMTtaskhandler handler(maintaskptr, 4);
                handler.settasklogger();
                handler.conccurentrun();
            }
            const std::vector<Models::FMTmodel> nmodels = modelparser.readproject("../../tests/testOAschedulertask/"+ results[0] + ".pri", std::vector<std::string>(1, "ROOT"));
            Models::FMTmodel readmodel = nmodels.at(0);
            Models::FMTlpmodel noptimizationmodel(readmodel, Models::FMTsolverinterface::CLP);
            noptimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, 5);
            noptimizationmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
            noptimizationmodel.Models::FMTmodel::setparameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
            const std::vector<Core::FMTschedule> schedules = modelparser.readschedules("../../tests/testOAschedulertask/" + results[0] + ".pri", nmodels).at(0);
            noptimizationmodel.doplanning(false, schedules);
		#endif 
        return 0;
	}

