#include <vector>
#ifdef FMTWITHOSI
#include "FMTlpmodel.hpp"
#include "FMTmodelparser.hpp"
#include "FMTscheduleparser.hpp"
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTconstraint.hpp"
#include "FMTfreeexceptionhandler.hpp"
#include <boost/algorithm/string.hpp>
#include "FMTmask.hpp"
#include "FMTSerie.hpp"
#endif


int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();

	if (Version::FMTversion().hasfeature("OSI"))
	{
		const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals, boost::is_any_of("|"));
		const std::string primarylocation =  results.at(0);
		const std::string scenario = results.at(1);
		const int length =  std::stoi(argv[2]);
		const int rotation_length =  std::stoi(argv[3]);
		/*const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/03_Modele_de_base/09_Prototypes_2023/08_ModWS/08152/prototype_81-52/PC_7002071_UA08152_FINAL.pri";
		const std::string scenario = "test";
		const int length = 20;
		const int rotation_length = 100;*/
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
		modelparser.seterrorstowarnings(errors);
		const std::vector<std::string>scenarios(1, scenario);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, length);
		optimizationmodel.FMTmodel::setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 10);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 2);
		optimizationmodel.doplanning(false);//No solve!
		std::string fullmask;
		for (size_t thid = 0; thid < optimizationmodel.getthemes().size(); ++thid)
		{
			fullmask += "? ";
		}
		fullmask.pop_back();
		const Core::FMTmask mainmask(fullmask, optimizationmodel.getthemes());
		const std::set<Core::FMTSerie>rotations = optimizationmodel.getRotations(mainmask, "REGAPRE");
			if (static_cast<int>(rotations.size()) != rotation_length)
			{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
					"presolvetest", __LINE__, primarylocation);
			}
		}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}

