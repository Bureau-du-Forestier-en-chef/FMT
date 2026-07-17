/*
Example to get FMTpredictors on a FMTSesModel
*/

#if defined FMTWITHGDAL && defined FMTWITHOSI
	#include <vector>
	#include "FMTlpmodel.h"
	#include "FMTsesmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTexception.h"
	#include "FMTpredictor.h"
	#include "FMTconstraint.h"
	#include "FMTtransition.h"
	#include "FMTspatialschedule.h"
	#include "FMTareaparser.h"
	#include "FMTforest.h"
	//#include "ogr_srs_api.h"
	//#include "gdal.h"
	//#include "gdal_priv.h"
	//#include "ogrsf_frmts.h"
#endif 




int main()
	{
#if defined FMTWITHGDAL && defined FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTVersion().hasFeature("OSI"))
		{
		#ifdef FMTWITHONNXR
			

		#endif
		const std::string folder = "../../../../Examples/Models/TWD_land/";
		const std::string primarylocation = folder+"TWD_land.pri";
		const std::string maplocation = folder+"Carte/TWD_land.shp";
		const std::string agefield = "AGE";
		const std::string areafield = "SUPERFICIE";
		const std::string lockfield = "";
		const std::string scenario_name = "LP";
		const std::string outdir = "../../tests/GetCarbonpredictors/";
		const std::vector<std::string> yieldsforpredictors(1,"VOLUMETOTAL");
		Parser::FMTModelParser modelparser;
        std::vector<Exception::FMTexc> errors;
        errors.push_back(Exception::FMTexc::FMTmissingyield);
        errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
        modelparser.setErrorsToWarnings(errors);
		const std::vector<std::string>scenarios(1, scenario_name);
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		for (size_t period = 0; period < 5; ++period)
		{
			optimizationmodel.buildPeriod();
		}
		std::vector<Core::FMTConstraint>constraints = optimizationmodel.getconstraints();
		const Core::FMTConstraint objective = constraints.at(0);
		constraints.erase(constraints.begin());
		for (const Core::FMTConstraint& constraint : constraints)
		{
			optimizationmodel.setConstraint(constraint);
		}
		optimizationmodel.setObjective(objective);
		if (optimizationmodel.initialSolve())
			{
				Models::FMTSesModel simulationmodel(optimizationmodel);
				std::vector<Core::FMTTransition> strans;
				for (const auto& tran : simulationmodel.getTransitions())
					{
						strans.push_back(tran.single());
					}
				simulationmodel.setTransitions(strans);
				Parser::FMTAreaParser areaparser;
				//areaparser.passinobject(modelparser);
				Spatial::FMTForest initialforestmap=areaparser.vectormaptoFMTforest(maplocation,380,optimizationmodel.getThemes(),agefield,areafield,1,0.0001,lockfield,0.0,"",false);
				simulationmodel.setInitialMapping(initialforestmap);
				for (size_t period = 1; period <= 5; ++period)
				{
					for (const auto& t : simulationmodel.greedyReferenceBuild(optimizationmodel.getSolution(period),10))
					{
						Logging::FMTDefaultLogger() << t.first << " " << t.second << " ";
					}
					Logging::FMTDefaultLogger() << "\n";
				}
				Spatial::FMTSpatialSchedule spatialschedule = simulationmodel.getSpSchedule();	
				std::vector<std::vector<std::vector<std::pair<std::string,double>>>> allpredictors;
				std::set<std::string> allprednames;
				for (size_t period = 1; period <= 5; ++period)
				{
					std::vector<std::vector<std::pair<std::string,double>>> periodpredictors;
					std::vector<std::vector<Graph::FMTPredictor>> predictors = areaparser.writePredictors(outdir,spatialschedule,yieldsforpredictors,simulationmodel,period);
					for (const auto& predictorslist : predictors)
					{
						for (const auto& predict : predictorslist)
						{
							std::vector<std::pair<std::string,double>> graphpred;
							size_t nameid = 0;
							const std::vector<double> predvals = predict.getPredictors();
							for (const std::string& predname : predict.getPredictorNames(yieldsforpredictors))
							{
								graphpred.push_back(std::pair<std::string,double>(predname,predvals.at(nameid)));
								allprednames.insert(predname);
								++nameid;
							}
							periodpredictors.push_back(graphpred);
						}
					} 
					allpredictors.push_back(periodpredictors);
				}
				int period=1;
				for (const auto& periodpred : allpredictors)
				{
					int id=0;
					for (const auto& graphpred:periodpred)
					{
						for(const auto& pred : graphpred)
						{
							Logging::FMTDefaultLogger() << "ID: " << id << " Period: " << period << " " << pred.first.c_str() << " " << pred.second << "\n";
						}
						++id;
					}
					++period;
				}
			}
	}else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
		}
#endif 
	return 0;
	}