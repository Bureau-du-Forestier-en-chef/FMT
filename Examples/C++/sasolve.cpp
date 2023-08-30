#include <vector>
#ifdef FMTWITHOSI
#include "FMTsamodel.hpp"
#include "FMTmodelparser.hpp"
#include "FMTareaparser.hpp"
#include "FMTforest.hpp"
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTconstraint.hpp"
#include "FMTfreeexceptionhandler.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "FMTGCBMtransition.hpp"
#endif


int main(int argc, char* argv[])
{
	Logging::FMTlogger().logstamp();

	if (Version::FMTversion().hasfeature("GDAL"))
	{
		/*/const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals, boost::is_any_of("|"));
		const std::string primarylocation =  results.at(0);
		const std::string scenario =  results.at(1);
		const int length =  std::stoi(argv[2]);
		const double objectivevalue =  std::stod(argv[3]);*/
		const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/02662/PC_9309_U02662_4_Vg2_2023_vRP1f.pri";
		const std::string scenario = "spatial2";
		//const std::string outputlocation = "../../tests/sasolve";
		const std::string outputlocation = "D:/test/SA/";
		const int length = 5;
		const double objectivevalue = 1000;
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
		errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
		modelparser.seterrorstowarnings(errors);
		const std::vector<std::string>scenarios(1, scenario);
		std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		
		boost::filesystem::path pripath(primarylocation);
		boost::filesystem::path basefolder = pripath.parent_path();
		boost::filesystem::path maplocation = basefolder / boost::filesystem::path("Carte") / boost::filesystem::path(pripath.stem().string() + ".shp");
		Parser::FMTareaparser areaparser;
		const Spatial::FMTforest forest = areaparser.vectormaptoFMTforest(maplocation.string(), 1420, models.at(0).getthemes(), "AGE", "SUPERFICIE", 1, 0.0001);
		models[0].setparameter(Models::FMTintmodelparameters::SEED, 100);
		Models::FMTsamodel optimizationmodel(models.at(0),forest);
		std::vector<Core::FMTtransition>singletransitions;
		for (const Core::FMTtransition transition : optimizationmodel.gettransitions())
			{
			singletransitions.push_back(transition.single());
			}
		optimizationmodel.settransitions(singletransitions);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, length);
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
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
	return 0;
}

