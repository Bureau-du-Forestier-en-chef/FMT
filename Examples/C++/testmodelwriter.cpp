#include "FMTmodelparser.h"
#include "FMTfreeexceptionhandler.h"
#ifdef FMTWITHOSI
	#include "FMTlpmodel.h"
#endif

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	const std::string primarylocation =  std::string(argv[1]);
	//const std::string primarylocation = "../../../../Examples/Models/TWD_land/TWD_land.pri";
	//const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/ReplaningStrat2024_04_30/Feux_2023_ouest_V01.pri";
	//const std::string primarylocation = "D:/CC_modele_feu/WS_CC/Feux_2023_ouest_V01.pri";
	//const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/02751/PC_9509_U02751_4_Vg2_2023_vRp2.pri";
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	const std::string outdir = "../../tests/testmodelwriter/";
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	modelparser.seterrorstowarnings(errors);
	const std::vector<std::string>scenarios(1, std::string(argv[2]));
	//const std::vector<std::string>scenarios(1, "tactique");
	//const std::vector<std::string>scenarios(1, "strategique");
	//const std::vector<std::string>scenarios(1, "14_Sc5_Determin_apsp_02751_tmw");
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
	Models::FMTlpmodel optmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
	optmodel.setparameter(Models::FMTintmodelparameters::LENGTH,  3);
	optmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 10);
	optmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	optmodel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, false);
	optmodel.doplanning(true);
	//optmodel.writeLP("D:/FMT/build/release/tests/testmodelwriter/strategic");
	//return 0;
	modelparser.write(optmodel, outdir);
	const double initobjvalue = optmodel.getObjValue();
	const std::vector<Models::FMTmodel> rereadmodels = modelparser.readproject(outdir + optmodel.getname() + ".pri", std::vector<std::string>(1, "ROOT"));
	optmodel = Models::FMTlpmodel(rereadmodels.at(0), Models::FMTsolverinterface::MOSEK);
	optmodel.setparameter(Models::FMTintmodelparameters::LENGTH,3);
	optmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 3);
	optmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	optmodel.doplanning(true);
	const double finalobjvalue = optmodel.getObjValue();
	std::cout << finalobjvalue << " " << initobjvalue << std::endl;
	//modelparser.write(optmodel, outdir+"other/");
	if ((finalobjvalue < (initobjvalue - 1)) || (finalobjvalue > (initobjvalue + 1)))
	{
		std::cout << "ok" << std::endl;
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
			"FMTsetsolution", __LINE__, primarylocation);
	}
	// Create a console output say "hello"


#endif
	return 0;
}