#include "FMTmodelparser.h"
#include "FMTfreeexceptionhandler.h"
#ifdef FMTWITHOSI
	#include "FMTlpmodel.h"
#endif

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	std::string primarylocation;
	std::vector<std::string>scenarios;
	if (argc > 1)
	{
		primarylocation = std::string(argv[1]);
		scenarios = std::vector<std::string>(1, std::string(argv[2]));
	}else {
		primarylocation = "C:\\Users\\Admlocal\\Documents\\issues\\C2_02031521\\PC_10067_U02571_2028_Corr_Vol_Phase2\\PC_10067_U02571_2028_MODB01.pri";
		scenarios = std::vector<std::string> (1, "200_StratReg_apsp");
	}

	//const std::string primarylocation = "../../../../Examples/Models/TWD_land/TWD_land.pri";
	//const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/ReplaningStrat2024_04_30/Feux_2023_ouest_V01.pri";
	//const std::string primarylocation = "D:/CC_modele_feu/WS_CC/Feux_2023_ouest_V01.pri";
	//const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/02751/PC_9509_U02751_4_Vg2_2023_vRp2.pri";
	//const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/Feux_2023_ouest/feux_2023_ouest_V01.pri";
	Parser::FMTModelParser modelparser;
	modelparser.setDefaultExceptionHandler();
	const std::string outdir = "../../tests/testmodelwriter/";

	std::vector<Exception::FMTexc> errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	//errors.push_back(Exception::FMTexc::FMToutput_too_much_operator); // Pour tester la relecture
	//errors.push_back(Exception::FMTexc::FMTinvalidyield_number); // à mettre on / off
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	//modelparser.setErrorsToWarnings(errors);

	std::vector<Exception::FMTexc> readErrors(errors);
	readErrors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	readErrors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	modelparser.setErrorsToWarnings(readErrors);

	
	//const std::vector<std::string>scenarios(1, "tactique");
	//const std::vector<std::string>scenarios(1, "strategique");
	//const std::vector<std::string>scenarios(1, "14_Sc5_Determin_apsp_02751_tmw");
	//const std::vector<std::string>scenarios(1, "201_UG107_feu");
	const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
	Models::FMTLpModel optmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
	optmodel.setParameter(Models::FMTintmodelparameters::LENGTH,  3);
	optmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 10);
	optmodel.setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	optmodel.setParameter(Models::FMTboolmodelparameters::POSTSOLVE, false);
	optmodel.doPlanning(true);
	//optmodel.writeLP("D:/FMT/build/release/tests/testmodelwriter/strategic");
	//return 0;
	modelparser.write(optmodel, outdir);
	const double initobjvalue = optmodel.getObjValue();
	const std::vector<Models::FMTModel> rereadmodels = modelparser.readproject(outdir + optmodel.getName() + ".pri", std::vector<std::string>(1, "ROOT"));
	optmodel = Models::FMTLpModel(rereadmodels.at(0), Models::FMTsolverinterface::MOSEK);
	optmodel.setParameter(Models::FMTintmodelparameters::LENGTH,3);
	optmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 3);
	optmodel.setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	optmodel.doPlanning(true);
	const double finalobjvalue = optmodel.getObjValue();
	std::cout << finalobjvalue << " " << initobjvalue << std::endl;
	//modelparser.write(optmodel, outdir+"other/");
	if ((finalobjvalue < (initobjvalue - 1)) || (finalobjvalue > (initobjvalue + 1)))
	{
		Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
			"FMTsetsolution", __LINE__, primarylocation);
	}


#endif
	return 0;
}