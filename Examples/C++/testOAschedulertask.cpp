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
            const std::string primarylocation = std::string(argv[1]);
            const std::string vals =  argv[2];
            std::vector<std::string>results;
            boost::split(results, vals, boost::is_any_of("|"));
            const std::vector<std::string>scenarios(1, results.at(0));
            boost::filesystem::path primpath(primarylocation);
            const std::string filename = primpath.stem().string();
            const boost::filesystem::path basefolder = primpath.parent_path();
            const std::string lfichierParam =  basefolder.string() + "/Scenarios/" + results.at(0) + "/" + results.at(1);
            const std::string fichierShp = std::string(argv[3]);
           
            /*
            const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/08551_det/PC_9424_U08551_4_Vg1_2023_vSSP03.pri";
            const std::vector<std::string>scenarios(1, "13_Sc5a_Determin_avsp_CLE_PESSIERE");
            boost::filesystem::path primpath(primarylocation);
            const std::string filename = primpath.stem().string();
            const boost::filesystem::path basefolder = primpath.parent_path();
            const std::string lfichierParam = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/08551_det/Scenarios/13_Sc5a_Determin_avsp_CLE_PESSIERE/parameters8551_rigide.csv";
            const std::string fichierShp = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/08551_det/Carte/PC_9424_UA_U08551.shp";
            std::vector<std::string>results(1,"13_Sc5a_Determin_avsp_CLE_PESSIERE");
            */

            const std::string out("../../tests/testOAschedulertask/" + scenarios.at(0));
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
            errors.push_back(Exception::FMTexc::FMTEmptyOA);
            modelparser.seterrorstowarnings(errors);
            const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
            Models::FMTmodel model = models.at(0);
            //Models::FMTlpmodel optimizationmodel(model, Models::FMTsolverinterface::CLP);
            Models::FMTlpmodel optimizationmodel(model, Models::FMTsolverinterface::MOSEK);
            optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, 5);
	        optimizationmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
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
                std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTopareaschedulertask(optimizationmodel, opeareas, nodeofoutput, out, "YOUVERT", 10, 120, adm7m));//120));
                Parallel::FMTtaskhandler handler(maintaskptr, 4);
                handler.settasklogger();
                handler.conccurentrun();
            }
            const std::vector<Models::FMTmodel> nmodels = modelparser.readproject("../../tests/testOAschedulertask/" + results[0] + ".pri", std::vector<std::string>(1, "ROOT"));
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

