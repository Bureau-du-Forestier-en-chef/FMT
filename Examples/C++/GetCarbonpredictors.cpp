/*
Example to get FMTcarbonpredictors on a FMTsesmodel
*/

#if defined FMTWITHGDAL && defined FMTWITHOSI
	#include <vector>
	#include "FMTlpmodel.hpp"
	#include "FMTsesmodel.hpp"
	#include "FMTmodelparser.hpp"
	#include "FMTversion.hpp"
	#include "FMTlogger.hpp"
	#include "FMTexception.hpp"
	#include "FMTcarbonpredictor.hpp"
	#include "FMTconstraint.hpp"
	#include "FMTtransition.hpp"
	#include "FMTspatialschedule.hpp"
	#include "FMTareaparser.hpp"
	#include "FMTforest.hpp"
#endif 




int main()
	{
#if defined FMTWITHGDAL && defined FMTWITHOSI
	Logging::FMTlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
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
		const std::string outdir = "tests/";
		const std::vector<std::string> yieldsforpredictors(1,"VOLUMETOTAL");
		Parser::FMTmodelparser modelparser;
        std::vector<Exception::FMTexc> errors;
        errors.push_back(Exception::FMTexc::FMTmissingyield);
        errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
        modelparser.seterrorstowarnings(errors);
		const std::vector<std::string>scenarios(1, scenario_name);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		for (size_t period = 0; period < 5; ++period)
		{
			optimizationmodel.buildperiod();
		}
		std::vector<Core::FMTconstraint>constraints = optimizationmodel.getconstraints();
		const Core::FMTconstraint objective = constraints.at(0);
		constraints.erase(constraints.begin());
		for (const Core::FMTconstraint& constraint : constraints)
		{
			optimizationmodel.setconstraint(constraint);
		}
		optimizationmodel.setobjective(objective);
		if (optimizationmodel.initialsolve())
			{
				Models::FMTsesmodel simulationmodel(optimizationmodel);
				std::vector<Core::FMTtransition> strans;
				for (const auto& tran : simulationmodel.gettransitions())
					{
						strans.push_back(tran.single());
					}
				simulationmodel.settransitions(strans);
				Parser::FMTareaparser areaparser;
				areaparser.passinobject(modelparser);
				Spatial::FMTforest initialforestmap=areaparser.vectormaptoFMTforest(maplocation,380,optimizationmodel.getthemes(),agefield,areafield,1,0.0001,lockfield,0.0,"",false);
				simulationmodel.setinitialmapping(initialforestmap);
				for (size_t period = 1; period <= 5; ++period)
				{
					for (const auto& t : simulationmodel.greedyreferencebuild(optimizationmodel.getsolution(period),10))
					{
						Logging::FMTlogger() << t.first << " " << t.second << " ";
					}
					Logging::FMTlogger() << "\n";
				}
				Spatial::FMTspatialschedule spatialschedule = simulationmodel.getspschedule();	
				std::vector<std::vector<std::vector<std::pair<std::string,double>>>> allpredictors;
				std::set<std::string> allprednames;
				for (size_t period = 1; period <= 5; ++period)
				{
					std::vector<std::vector<std::pair<std::string,double>>> periodpredictors;
					std::vector<std::vector<Graph::FMTcarbonpredictor>> predictors = areaparser.writecarbonpredictors(outdir,spatialschedule,yieldsforpredictors,simulationmodel,period);
					for (const auto& predictorslist : predictors)
					{
						for (const auto& predict : predictorslist)
						{
							std::vector<std::pair<std::string,double>> graphpred;
							size_t nameid = 0;
							const std::vector<double> predvals = predict.getpredictors();
							for (const std::string& predname : predict.getpredictornames(yieldsforpredictors))
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
				GDALDataset* newdataset = areaparser.createOGRdataset(outdir+"PREDICTORS.csv", "CSV");
				OGRLayer* newlayer = newdataset->CreateLayer(("Predictors_"+simulationmodel.getname()).c_str(), NULL, wkbNone, NULL);
				OGRFieldDefn IdField("graphID", OFTInteger);
				IdField.SetWidth(32);
				newlayer->CreateField(&IdField);
				OGRFieldDefn PeriodField("Period", OFTInteger);
				PeriodField.SetWidth(32);
				newlayer->CreateField(&PeriodField);
				for(const auto& predname : allprednames)
				{
					OGRFieldDefn yieldField(predname.c_str(), OFTReal);
					newlayer->CreateField(&yieldField);
				}
				int period=1;
				for (const auto& periodpred : allpredictors)
				{
					int id=0;
					for (const auto& graphpred:periodpred)
					{
						OGRFeature *newfeature = OGRFeature::CreateFeature(newlayer->GetLayerDefn());
						newfeature->SetField("graphID", id);
						newfeature->SetField("Period", period);
						for(const auto& pred : graphpred)
						{
							newfeature->SetField(pred.first.c_str(),pred.second);
						}
						newlayer->CreateFeature(newfeature);
						++id;
					}
					++period;
				}
				GDALClose(newdataset);
			}
	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
#endif 
	return 0;
	}