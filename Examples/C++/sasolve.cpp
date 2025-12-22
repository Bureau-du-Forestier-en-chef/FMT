#include <vector>
#ifdef FMTWITHOSI
#include "FMTsamodel.h"
#include "FMTmodelparser.h"
#include "FMTareaparser.h"
#include "FMTforest.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTconstraint.h"
#include "FMTfreeexceptionhandler.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "FMTGCBMtransition.h"
#endif


int main(int argc, char* argv[])
{
	Logging::FMTdefaultlogger().logstamp();

	if (Version::FMTversion().hasfeature("GDAL"))
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
			primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/Weight/PC_9964_U08451_2028_MODB01.pri";
			scenario = "120_RegProv_apsp_AGG";
			length = 3;
			objectivevalue = 1000;
			resolution = 200;
			//resolution = 1000;
			/*primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/02662/PC_9309_U02662_4_Vg2_2023_vRP1f.pri";
			scenario = "spatial";
			length = 1;
			objectivevalue = 1000;*/
			}
		const std::string outputlocation = "../../tests/sasolve";
		Parser::FMTmodelparser modelparser;
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
		modelparser.seterrorstowarnings(errors);
		const std::vector<std::string>scenarios(1, scenario);
		std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		
		boost::filesystem::path pripath(primarylocation);
		boost::filesystem::path basefolder = pripath.parent_path();
		const boost::filesystem::path RASTER_LOCATION = basefolder / boost::filesystem::path("rasters");
		Spatial::FMTforest forest;
		Parser::FMTareaparser areaparser;
		if (boost::filesystem::is_directory(RASTER_LOCATION))
			{
			std::vector<std::string>themesName;
			size_t i = 1;
			for (const auto& THEME : models.at(0).getthemes())
			{
				themesName.push_back(RASTER_LOCATION.string() + "/THEME" + std::to_string(i)+".tif");
				++i;
			}
			forest = areaparser.readrasters(models.at(0).getthemes(),
				themesName, RASTER_LOCATION.string() + "/AGE.tif", 1.0, 0.0001, RASTER_LOCATION.string() + "/STANLOCK.tif");
		}else {
			boost::filesystem::path maplocation = basefolder / boost::filesystem::path("Carte") / boost::filesystem::path(pripath.stem().string() + ".shp");
			forest = areaparser.vectormaptoFMTforest(maplocation.string(), resolution, models.at(0).getthemes(), "AGE", "SUPERFICIE", 1, 0.0001);
			}
		models[0].setparameter(Models::FMTintmodelparameters::SEED, 100);
		Models::FMTsamodel optimizationmodel(models.at(0),forest);
		//optimizationmodel.setdebuglogger();
		std::vector<Core::FMTtransition>singletransitions;
		for (const Core::FMTtransition transition : optimizationmodel.gettransitions())
			{
			singletransitions.push_back(transition.single());
			}
		optimizationmodel.settransitions(singletransitions);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, length);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::MAX_MOVES, 10000000);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::MAX_ACCEPTED_CYCLE_MOVES, 500);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::MAX_CYCLE_MOVES, 1000);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 10);
		optimizationmodel.setparameter(Models::FMTstrmodelparameters::WORKING_DIRECTORY, outputlocation);
		optimizationmodel.doplanning(true);
		std::vector<Core::FMToutput>outputs;
		for (const Core::FMToutput& out : optimizationmodel.getoutputs())
		{
			if (out.getname().find("OVOL")!=std::string::npos)
			{
				outputs.push_back(out);
			}

		}
		modelparser.writeresults(optimizationmodel, outputs, 1, length, outputlocation, Core::FMToutputlevel::totalonly);
		for (int period = 1; period <= length; ++period)
			{
			areaparser.writedisturbances(outputlocation, optimizationmodel.getspschedule(), optimizationmodel.getactions(), optimizationmodel.getthemes(), period);
			}
		
		/*if ((std::abs(optimizationmodel.getObjValue() - objectivevalue)) >= 1)
		{
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
				"presolvetest", __LINE__, primarylocation);
		}*/
	}
	else {
		Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
	return 0;
}

