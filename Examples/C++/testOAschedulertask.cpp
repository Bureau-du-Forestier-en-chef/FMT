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

std::vector<Heuristics::FMToperatingareascheme> ObtenirOperatingArea(
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
        std::vector<Heuristics::FMToperatingareascheme> opeareas = areaParser.readOAschedulerparameters(
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

Core::FMTOutputNode createBFECoptaggregate(Models::FMTModel& model)   
    {
        std::string Agg_name = "~BFECOPTOUTPUTYOUVERT~";
            std::vector<Core::FMTAction> newactions;
            int youvert = 0;

            for (Core::FMTAction& action : model.getactions())
            {
                if (action.useYield("YOUVERT"))
                {
                    youvert += 1;
                    std::vector<std::string> agg = action.getAggregates();
                    if (std::count(agg.begin(), agg.end(), Agg_name))
                    {
                        Exception::FMTFreeExceptionHandler().raise(
                            Exception::FMTexc::FMTfunctionfailed, 
                            "L'utilisateur à utiliser le nom ~BFECOPTOUTPUTYOUVERT~ dans ses outputs",
                            "testOAschedulerBFEC", __LINE__,model.getName());  
                    }

                    action.pushAggregate(Agg_name);
                }

                newactions.push_back(action);
            }

            if (youvert < 1)
            {
                Exception::FMTFreeExceptionHandler().raise(
                    Exception::FMTexc::FMTfunctionfailed, 
                    "Aucune action dans le modèle n'a de youvert",
                    "testOAschedulerBFEC", __LINE__,model.getName()) ;
            }

            model.setActions(newactions);
            const std::vector<Core::FMTTheme> themes = model.getThemes();	
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

            Core::FMTMask fmtMask = Core::FMTMask(stringMask, themes);
            return Core::FMTOutputNode(fmtMask, Agg_name);
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
            int length;
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
                length = 5;
            }else
                {
                //primarylocation = "C:\\Users\\Admlocal\\Documents\\issues\\C2_02020265\\02_Travail_Realisme\\PC_9943_U08651_2028_MODB01.pri";
                //scenarios = std::vector<std::string> (1, "323_TYFSansPre_avsp");
                //boost::filesystem::path primpath(primarylocation);
                //const boost::filesystem::path basefolder = primpath.parent_path();
                //lfichierParam = "C:\\Users\\Admlocal\\Documents\\issues\\C2_02020265\\Parametres_Bfecopt.csv";
                //fichierShp = "C:\\Users\\Admlocal\\Documents\\issues\\C2_02020265\\02_Travail_Realisme\\Carte\\PC_9943_UA_U08651.shp";
                //results = std::vector<std::string> (1, "323_TYFSansPre_avsp");
                //length = 20;
                primarylocation = "C:\\Users\\Admlocal\\Documents\\issues\\C2_02031521\\PC_10067_U02571_2028_Corr_Vol_Phase2\\PC_10067_U02571_2028_MODB01.pri";
                scenarios = std::vector<std::string> (1, "200_StratReg_apsp");
                boost::filesystem::path primpath(primarylocation);
                const boost::filesystem::path basefolder = primpath.parent_path();
                lfichierParam = "C:\\Users\\Admlocal\\Documents\\issues\\C2_02031521\\02571_parametres_RP1_1.csv";
                fichierShp = "C:\\Users\\Admlocal\\Documents\\issues\\C2_02031521\\PC_10067_U02571_2028_Corr_Vol_Phase2\\Carte\\PC_10067_UA_U02571_TIF_UTA.shp";
                results = std::vector<std::string> (1, "200_StratReg_apsp");
                length = 20;
                }
           
            const std::string out("../../tests/testOAschedulertask/" + scenarios.at(0));
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
            Models::FMTModel model = models.at(0);
            //Models::FMTLpModel optimizationmodel(model, Models::FMTsolverinterface::CLP);
            Models::FMTLpModel optimizationmodel(model, Models::FMTsolverinterface::MOSEK);
            optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, length);
            optimizationmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
	        optimizationmodel.setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true); 
            // pour gérer les variables négatives
            //const int startingperiod = optimizationmodel.getconstraints().at(0).getPeriodLowerBound();
            const int startingperiod = optimizationmodel.getParameter(Models::FMTintmodelparameters::UPDATE);
            const Core::FMTOutputNode nodeofoutput =  createBFECoptaggregate(optimizationmodel);
            Core::FMTOutput adm7m;
            for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
                {
                if (output.getName() == "OATTEINTE7M")
                    {
                    adm7m = output;
                    break;
                    }
                }
            const std::vector<Heuristics::FMToperatingareascheme> opeareas = ObtenirOperatingArea(
                fichierShp,
                optimizationmodel.getThemes(),
                14, 
                startingperiod, 
                "AGE", 
                "SUPERFICIE", 
                "STANLOCK", 
                lfichierParam);
            {
                std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTopareaschedulertask(
                    optimizationmodel, 
                    opeareas, 
                    nodeofoutput, 
                    out, 
                    "YOUVERT", 
                    10, 
                    9000, 
                    adm7m));//120));
                Parallel::FMTtaskhandler handler(maintaskptr, 1);
                handler.setTaskLogger();
                handler.conccurentRun();
                maintaskptr->finalize(); // écrit ici le meilleur modèle sur le disque
            }
            // On relit ici le nouveau "root" qui est le meilleur modèle écrit précédement 
            const std::vector<Models::FMTModel> nmodels = modelparser.readproject(
                "../../tests/testOAschedulertask/" + results[0] + ".pri", std::vector<std::string> (1, "ROOT"));
            Models::FMTModel readmodel = nmodels.at(0);
            Models::FMTLpModel noptimizationmodel(readmodel, Models::FMTsolverinterface::CLP); // Pourquoi CLP et pas Mosek?
            noptimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, length);
            noptimizationmodel.setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
            noptimizationmodel.Models::FMTModel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
            const std::vector<Core::FMTSchedule> schedules = modelparser.readschedules(
                "../../tests/testOAschedulertask/" + results[0] + ".pri", nmodels).at(0);
            // On regarde si on est capable de relire ce qu'on vient de créer
            noptimizationmodel.doPlanning(false, schedules); // si c'est false, pas besoin de optimiser. Fait juste prendre la solution. 
		#endif 
        return 0;
	}

