#include <vector>
#ifdef FMTWITHOSI
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTmodelparser.h"
    #include "FMTlpmodel.h"
    #include "FMTconstraint.h"
    #include "FMTfreeexceptionhandler.h"
    #include "FMTexception.h"
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
	std::string primarylocation;
	std::string scenario;
	int scenario_length;
	if (argc>1)
	{
		primarylocation = argv[1];
		scenario = argv[2];
		scenario_length = std::stoi(argv[3]);
	}else {
		primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/TBEV02/tbe_R02_v01.pri";
		scenario = "02_test_Recolte_ratio_series";
		scenario_length = 2;
	}

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

