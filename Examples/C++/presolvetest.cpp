#include <vector>
#ifdef FMTWITHOSI
	#include "FMTversion.hpp"
	#include "FMTlogger.hpp"
	#include "FMTmodelparser.hpp"
    #include "FMTlpmodel.hpp"
    #include "FMTconstraint.hpp"
    #include "FMTfreeexceptionhandler.hpp"
    #include "FMTexception.hpp"
#endif

int main(int argc, char *argv[])
	{
	argc = 4;
	argv = new char*[4];
	argv[1] = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/FM/PC_7001892_U03772_SSP02.pri";
	argv[2] = "03_sc1a_bfecopt";
	argv[3] = "5";
	#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	const std::string primarylocation = argv[1];
	const std::string scenario = argv[2];
	const int scenario_length = std::stoi(argv[3]);
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	modelparser.seterrorstowarnings(errors);
	const std::vector<std::string>scenarios(1, scenario);
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
	Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
	optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH,scenario_length);
	optimizationmodel.FMTmodel::setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	optimizationmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 0);
	optimizationmodel.doplanning(true);
	Models::FMTlpmodel presolvedoptimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
	presolvedoptimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, scenario_length);
	presolvedoptimizationmodel.FMTmodel::setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	presolvedoptimizationmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS,10);
	presolvedoptimizationmodel.doplanning(true);
	const double nopresolve = optimizationmodel.getObjValue();
	const double presolve = presolvedoptimizationmodel.getObjValue();
	if (std::abs(nopresolve - presolve) >= 0.1)
	{
		
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
			"presolvetest", __LINE__, primarylocation);
	}
	#endif
	return 0;
	}

