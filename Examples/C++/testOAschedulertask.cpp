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
                Logging::FMTdefaultlogger() << "Lecture des blocs voisins." << "\n";
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
            Logging::FMTdefaultlogger().logstamp();
            std::string primarylocation;
            std::vector<std::string>results;
            std::vector<std::string>scenarios;
            std::string lfichierParam;
            std::string fichierShp;
            if (argc > 1)
            {
                primarylocation = std::string(argv[1]);
                const std::string vals = argv[2];
                boost::split(results, vals, boost::is_any_of("|"));
                scenarios = std::vector<std::string>(1, results.at(0));
                boost::filesystem::path primpath(primarylocation);
                const boost::filesystem::path basefolder = primpath.parent_path();
                lfichierParam = basefolder.string() + "/Scenarios/" + results.at(0) + "/" + results.at(1);
                fichierShp = std::string(argv[3]);
            }else
                {
                primarylocation = "C:\\Users\\Admlocal\\Documents\\issues\\OAScheduler\\ModWS_08251\\01_08251\\PC_9949_U08251_2028_MODB01.pri";
                scenarios = std::vector<std::string> (1, "250_Fin_Etape1_avsp");
                boost::filesystem::path primpath(primarylocation);
                const boost::filesystem::path basefolder = primpath.parent_path();
                lfichierParam = "C:\\Users\\Admlocal\\Documents\\issues\\OAScheduler\\Seuil_min_10%\\Parametres_Bfecopt.csv";
                fichierShp = "C:\\Users\\Admlocal\\Documents\\issues\\OAScheduler\\ModWS_08251\\01_08251\\Carte\\PC_9949_UA_U08251.shp";
                results = std::vector<std::string> (1, "250_Fin_Etape1_avsp");
                }
           
            const std::string out("../../tests/testOAschedulertask/" + scenarios.at(0));
            Parser::FMTmodelparser modelparser;
            modelparser.setdefaultexceptionhandler();
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
            modelparser.seterrorstowarnings(errors);
            const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
            Models::FMTmodel model = models.at(0);
            //Models::FMTlpmodel optimizationmodel(model, Models::FMTsolverinterface::CLP);
            Models::FMTlpmodel optimizationmodel(model, Models::FMTsolverinterface::MOSEK);
            optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, 5);
	        optimizationmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true); // pour gérer les variables négatives
            //const int startingperiod = optimizationmodel.getconstraints().at(0).getperiodlowerbound();
            const int startingperiod = optimizationmodel.getparameter(Models::FMTintmodelparameters::UPDATE);
            const Core::FMToutputnode nodeofoutput =  createBFECoptaggregate(optimizationmodel);
            Core::FMToutput adm7m;
            for (const Core::FMToutput& output : optimizationmodel.getoutputs())
                {
                if (output.getname() == "OATTEINTE7M")
                    {
                    adm7m = output;
                    break;
                    }
                }
            const std::vector<Heuristics::FMToperatingareascheme> opeareas = ObtenirOperatingArea(fichierShp,optimizationmodel.getthemes(),14, startingperiod, "AGE", "SUPERFICIE", "STANLOCK", lfichierParam);
            {
                std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTopareaschedulertask(optimizationmodel, opeareas, nodeofoutput, out, "YOUVERT", 10, 9000, adm7m));//120));
                Parallel::FMTtaskhandler handler(maintaskptr, 4);
                handler.settasklogger();
                handler.conccurentrun();
                maintaskptr->finalize(); // écrit ici le meilleur modèle sur le disque
            }
            // On relit ici le nouveau "root" qui est le meilleur modèle écrit précédement 
            const std::vector<Models::FMTmodel> nmodels = modelparser.readproject("../../tests/testOAschedulertask/" + results[0] + ".pri", std::vector<std::string>(1, "ROOT"));
            Models::FMTmodel readmodel = nmodels.at(0);
            Models::FMTlpmodel noptimizationmodel(readmodel, Models::FMTsolverinterface::CLP); // Pourquoi CLP et pas Mosek?
            noptimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, 20);
            noptimizationmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
            noptimizationmodel.Models::FMTmodel::setparameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
            const std::vector<Core::FMTschedule> schedules = modelparser.readschedules("../../tests/testOAschedulertask/" + results[0] + ".pri", nmodels).at(0);
            // On regarde si on est capable de relire ce qu'on vient de créer
            noptimizationmodel.doplanning(false, schedules); // si c'est false, pas besoin de optimiser. Fait juste prendre la solution. 
		#endif 
        return 0;
	}

