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
#endif


int main(int argc, char* argv[])
{
	Logging::FMTlogger().logstamp();

	if (Version::FMTversion().hasfeature("GDAL"))
	{
		const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals, boost::is_any_of("|"));
		const std::string primarylocation =  results.at(0);
		const std::string scenario =  results.at(1);
		const int length =  std::stoi(argv[2]);
		const double objectivevalue =  std::stod(argv[3]);
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
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		
		boost::filesystem::path pripath(primarylocation);
		boost::filesystem::path basefolder = pripath.parent_path();
		boost::filesystem::path maplocation = basefolder / boost::filesystem::path("Carte") / boost::filesystem::path(pripath.stem().string() + ".shp");
		Parser::FMTareaparser areaparser;
		const Spatial::FMTforest forest = areaparser.vectormaptoFMTforest(maplocation.string(), 1420, models.at(0).getthemes(), "AGE", "SUPERFICIE", 1, 0.0001);
		Models::FMTsamodel optimizationmodel(models.at(0),forest);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, length);
		//optimizationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS,10000000);
		optimizationmodel.doplanning(true);
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
