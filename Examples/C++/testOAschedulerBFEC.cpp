#ifdef FMTWITHOSI
    #include "FMTareaparser.h"
    #include "FMTyieldparser.h"
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTconstraint.h"
    #include "FMToutputnode.h"
    #include "FMTmask.h"
    #include "FMTlpheuristicmthandler.h"
    #include "FMToperatingareascheduler.h"
    #include "FMTtimeyieldhandler.h"
    #include "FMTfreeexceptionhandler.h"
#endif
#ifdef FMTWITHOSI
std::vector<Heuristics::FMToperatingareascheme> ObtenirOperatingArea(   const std::string& fichierShp,const std::vector<Core::FMTtheme>& themes, const int& numeroTheme,const int& startingperiod,
                                                                        const std::string& nomChampAge,const std::string& nomChampSuperficie,const std::string& nomChampStanlock
                                                                    )
    {
	
	    std::vector<Heuristics::FMToperatingareascheme> opeareas;
	    bool spatialconstraints = false;
        std::vector<std::string>operatingareasname = themes.at(numeroTheme-1).getAttributes("?",false);
        for (const auto& OA : operatingareasname)
        {
            if (OA != "NA")
            {
                const int OPT = 2;
                const int RET = 6;
                const int MAXRET = 6;
                const int REP = 3;
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
                    MAXRET,
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
            Logging::FMTDefaultLogger()<<"Lecture des blocs voisins."<<"\n";
            opeareas = areaParser.getSchemeNeighbors(opeareas, themes, fichierShp, nomChampAge, nomChampSuperficie, 1.0, 1,nomChampStanlock);
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
                if (action.useYield("YOUVERT"))
                {
                    youvert += 1;
                    std::vector<std::string> agg = action.getAggregates();
                    if (std::count(agg.begin(), agg.end(), Agg_name))
                    {
                        Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "L'utilisateur à utiliser le nom ~BFECOPTOUTPUTYOUVERT~ dans ses outputs",
                        "testOAschedulerBFEC", __LINE__,model.getName());  
                    }

                    action.pushAggregate(Agg_name);
                }

                newactions.push_back(action);
            }

            if (youvert < 1)
            {
                Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Aucune action dans le modèle n'a de youvert",
                        "testOAschedulerBFEC", __LINE__,model.getName()) ;
            }

            model.setActions(newactions);
            const std::vector<Core::FMTtheme> themes = model.getThemes();	
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
            Logging::FMTDefaultLogger().logStamp();
            std::string primarylocation;
            std::vector<std::string>scenarios;
            std::string fichierShp;
            if (argc>1)
            {
                primarylocation = std::string(argv[1]);
                scenarios = std::vector<std::string>(1, std::string(argv[2]));
                fichierShp = std::string(argv[3]);
            }
            else {
                primarylocation = "C:/Users/Admlocal/Documents/issues/280/08251_test/02_Travail/PC_9949_U08251_2028_MODB01.pri";
                scenarios = std::vector<std::string>(1, "120_RegProv_apsp");
                fichierShp = "C:/Users/Admlocal/Documents/issues/280/08251_test/02_Travail/Carte/PC_9949_UA_U08251.shp";
            }
            Parser::FMTmodelparser modelparser;
            modelparser.setDefaultExceptionHandler(); 
            modelparser.setTaskLogger();
            std::vector<Exception::FMTexc>errors;
		    errors.push_back(Exception::FMTexc::FMTmissingyield);
		    errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		    errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		    errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		    errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		    errors.push_back(Exception::FMTexc::FMToveridedyield);
		    errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
		    errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
            errors.push_back(Exception::FMTexc::FMTEmptyOA);
            errors.push_back(Exception::FMTexc::FMTdeathwithlock);
		    modelparser.setErrorsToWarnings(errors);
            const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
            Models::FMTmodel model = models.at(0);
            Models::FMTlpmodel optimizationmodel(model, Models::FMTsolverinterface::MOSEK);
            optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH,5);
	        optimizationmodel.setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	        optimizationmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 1);
            optimizationmodel.setParameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
          
            const int startingperiod = optimizationmodel.getconstraints().at(0).getPeriodLowerBound();
            optimizationmodel.doPlanning(true);
            const double initialobjectivevalue = optimizationmodel.getObjValue();
            const Core::FMToutputnode nodeofoutput =  createBFECoptaggregate(optimizationmodel);
            std::vector<Heuristics::FMToperatingareascheme> opeareas = ObtenirOperatingArea(fichierShp,optimizationmodel.getThemes(),14, startingperiod, "AGE", "SUPERFICIE", "STANLOCK");
            std::vector<Heuristics::FMToperatingareascheduler> opareaheuristics = optimizationmodel.getOperatingAreaSchedulerHeuristics(opeareas, nodeofoutput);
            //opareaheuristics[0].setProportionOfSet(0.25);
			Heuristics::FMTlpheuristicmthandler handler = Heuristics::FMTlpheuristicmthandler(opareaheuristics, initialobjectivevalue);
            const double calculatedpropotion = opareaheuristics[0].generateInitialProportionOfSet();
            std::cout<< "Initial proportion of set of : " + std::to_string(calculatedpropotion) << "\n";
            opareaheuristics[0].setProportionOfSet(calculatedpropotion);
			size_t bestpos = handler.initialSolve();
            bestpos = handler.greedySolve(5,10000000);
			const Heuristics::FMToperatingareascheduler bestsolve = opareaheuristics[bestpos];
            const std::vector<Core::FMTtimeyieldhandler> ythandler = bestsolve.getSolution("YOUVERT");
            /*for (const auto& out : bestsolve.getLevelSolution("COS", "BFECOPTtata", model.getoutputs().size()))
                {
                std::cout << std::string(out) << "\n";
                }*/
            //write solution
            Core::FMTyields yields;
            for (const Core::FMTtimeyieldhandler& tyld : ythandler)
            {
                std::unique_ptr<Core::FMTyieldhandler>newyield(new Core::FMTtimeyieldhandler(tyld));
                yields.push_back(newyield->getMask(), newyield);
            }
            yields.update();
            Parser::FMTyieldparser yldparser;
            const std::string solutionname = "../../tests/testOAschedulerBFEC/bfecoptsol.yld";
            yldparser.write(yields, solutionname);
            //
            Core::FMTyields myields = model.getYields();
            myields.unShrink(model.getThemes());
            for(const auto& yth : ythandler)
            {   
                myields.push_front( yth.getMask(),
                                    std::unique_ptr<Core::FMTyieldhandler>(new Core::FMTtimeyieldhandler(yth))
                                    );
            }
            myields.update();
            optimizationmodel = Models::FMTlpmodel(model, Models::FMTsolverinterface::MOSEK);
            optimizationmodel.setYields(myields);
            optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH,5);
            optimizationmodel.setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
            optimizationmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 1);
            if (optimizationmodel.doPlanning(true)) 
            {
                const double objectivevalue = optimizationmodel.getObjValue();
                const double bfecoptvalue = bestsolve.getObjValue();
                std::cout << objectivevalue << " " << bfecoptvalue << std::endl;
                if ((objectivevalue < (bfecoptvalue - 1)) || (objectivevalue > (bfecoptvalue + 1)))
                {
                    Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
                        "testOAschedulerBFEC", __LINE__, primarylocation);
                }
            }
            else {
                Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Cannot resolve model with solution",
                    "testOAschedulerBFEC", __LINE__, primarylocation);
            }
           
    #endif 
        return 0;
	}

