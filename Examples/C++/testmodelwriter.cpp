#include "FMTmodelparser.hpp"
#include "FMTfreeexceptionhandler.hpp"
#ifdef FMTWITHOSI
	#include "FMTlpmodel.hpp"
#endif

int main(int argc, char* argv[])
{
	const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/TBEV02/tbe_R02_v01.pri";//std::string(argv[1]);
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	const std::string outdir = "tests/";
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	modelparser.seterrorstowarnings(errors);
	//const std::vector<std::string>scenarios(1, std::string(argv[2]));
	const std::vector<std::string>scenarios(1, "02_test_Recolte_ratio_series");
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
	Models::FMTlpmodel optmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
	//optmodel.setparameter(Models::FMTintmodelparameters::LENGTH,std::stoi(argv[3]);
	optmodel.setparameter(Models::FMTintmodelparameters::LENGTH, 3);
	//optmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 0);
	//optmodel.setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
	optmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	optmodel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, false);
	modelparser.write(optmodel, outdir);
	optmodel.doplanning(true);
	const double initobjvalue = optmodel.getObjValue();
	std::cout << initobjvalue << std::endl;
	//modelparser.write(optmodel, outdir);
	const std::vector<Models::FMTmodel> rereadmodels = modelparser.readproject(outdir + optmodel.getname() + ".pri", std::vector<std::string>(1, "ROOT"));
	optmodel = Models::FMTlpmodel(rereadmodels.at(0), Models::FMTsolverinterface::MOSEK);
	optmodel.setparameter(Models::FMTintmodelparameters::LENGTH, 3);
	//optmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 0);
	optmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, false);
	optmodel.doplanning(true);
	const double finalobjvalue = optmodel.getObjValue();
	std::cout << finalobjvalue << " " << initobjvalue << std::endl;
	modelparser.write(optmodel, outdir+"other/");
	if ((finalobjvalue < (initobjvalue - 1)) || (finalobjvalue > (initobjvalue + 1)))
	{
		std::cout << "ok" << std::endl;
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
			"FMTsetsolution", __LINE__, primarylocation);
	}
}