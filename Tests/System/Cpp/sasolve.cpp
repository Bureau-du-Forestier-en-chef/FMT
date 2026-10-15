#include <vector>
#ifdef FMTWITHOSI
#include "FMTSaModel.h"
#include "FMTModelParser.h"
#include "FMTAreaParser.h"
#include "FMTForest.h"
#include "FMTVersion.h"
#include "FMTDefaultLogger.h"
#include "FMTConstraint.h"
#include "FMTFreeExceptionHandler.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "FMTGCBMTransition.h"
#endif


int main(int argc, char* argv[])
{
	Logging::FMTDefaultLogger().logStamp();

	if (Version::FMTVersion().hasFeature("GDAL"))
	{
		std::string primarylocation;
		std::string scenario;
		int length;
		double objectivevalue;
		int resolution = 1420;
		if (argc>1)
		{
			const std::string vals = argv[1];
			std::vector<std::string>results;
			boost::split(results, vals, boost::is_any_of("|"));
			primarylocation = results.at(0);
			scenario = results.at(1);
			length = std::stoi(argv[2]);
			objectivevalue = std::stod(argv[3]);
		}else {
			primarylocation = "C:/Users/admlocal/Desktop/08251_SA/PC_9949_U08251_2028_MODB01.pri";
			scenario = "205_StratReg_OPE60_SANSTBE_SNAT_apsp";
			length = 3;
			objectivevalue = 1000;
			resolution = 200;
			//resolution = 1000;
			/*primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/02662/PC_9309_U02662_4_Vg2_2023_vRP1f.pri";
			scenario = "spatial";
			length = 3;
			objectivevalue = 1000;*/
			}
		const std::string outputlocation = "../../tests/sasolve";
		Parser::FMTModelParser modelparser;
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
		errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
		errors.push_back(Exception::FMTexc::FMTunclosedforloop);
		errors.push_back(Exception::FMTexc::FMToutofrangeyield);
		errors.push_back(Exception::FMTexc::FMTdeathwithlock);
		errors.push_back(Exception::FMTexc::FMTempty_schedules);
		errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
		modelparser.setErrorsToWarnings(errors);
		const std::vector<std::string>scenarios(1, scenario);
		std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		
		boost::filesystem::path pripath(primarylocation);
		boost::filesystem::path basefolder = pripath.parent_path();
		const boost::filesystem::path RASTER_LOCATION = basefolder / boost::filesystem::path("rasters");
		Spatial::FMTForest forest;
		Parser::FMTAreaParser areaparser;
		if (boost::filesystem::is_directory(RASTER_LOCATION))
			{
			std::vector<std::string>themesName;
			size_t i = 1;
			for (const auto& THEME : models.at(0).getThemes())
			{
				themesName.push_back(RASTER_LOCATION.string() + "/THEME" + std::to_string(i)+".tif");
				++i;
			}
			forest = areaparser.readRasters(models.at(0).getThemes(),
				themesName, RASTER_LOCATION.string() + "/AGE.tif", 1.0, 0.0001, RASTER_LOCATION.string() + "/STANLOCK.tif");
		}else {
			boost::filesystem::path maplocation = basefolder / boost::filesystem::path("Carte") / boost::filesystem::path(pripath.stem().string() + ".shp");
			forest = areaparser.vectormaptoFMTforest(maplocation.string(), resolution, models.at(0).getThemes(), "AGE", "SUPERFICIE", 1, 0.0001);
			}
		//models[0].setParameter(Models::FMTintmodelparameters::SEED, 100);
		Models::FMTSaModel optimizationmodel(models.at(0));
		optimizationmodel.setInitialMapping(forest);
		//optimizationmodel.redirectLogToFile(outputlocation + "/SA.log");
		std::vector<Core::FMTTransition>singletransitions;
		for (const Core::FMTTransition transition : optimizationmodel.getTransitions())
			{
			singletransitions.push_back(transition.single());
			}
		optimizationmodel.setTransitions(singletransitions);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, length);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::MAX_MOVES, 500000);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::MAX_ACCEPTED_CYCLE_MOVES, 3000);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::MAX_CYCLE_MOVES, 5000);
		//optimizationmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 10);
		//optimizationmodel.setParameter(Models::FMTstrmodelparameters::WORKING_DIRECTORY, outputlocation);
		optimizationmodel.doPlanning(true);
		//optimizationmodel.logConstraintsInfeasibilities();
		/*std::vector<Core::FMTOutput>outputs;
		for (const Core::FMTOutput& out : optimizationmodel.getoutputs())
		{
			if (out.getName().find("OVOL")!=std::string::npos)
			{
				outputs.push_back(out);
			}

		}
		modelparser.writeresults(optimizationmodel, outputs, 1, length, outputlocation, Core::FMToutputlevel::totalonly);
		for (int period = 1; period <= length; ++period)
			{
			areaparser.writeDisturbances(outputlocation, optimizationmodel.getSpSchedule(), optimizationmodel.getActions(), optimizationmodel.getthemes(), period);
			}*/
		
	}
	else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
	}
	return 0;
}

