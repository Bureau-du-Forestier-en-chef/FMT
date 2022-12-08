#include "FMTmodelparser.hpp"
#include "FMTfreeexceptionhandler.hpp"
#ifdef FMTWITHOSI
	#include "FMTlpmodel.hpp"
#endif

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	const std::string primarylocation = std::string(argv[1]);
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	const std::string outdir = "../../tests/testmodelwriter/";
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	modelparser.seterrorstowarnings(errors);
	const std::vector<std::string>scenarios(1,std::string(argv[2]));
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
	Models::FMTlpmodel optmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
	optmodel.setparameter(Models::FMTintmodelparameters::LENGTH, 3);
	optmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 3);
	optmodel.setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
	optmodel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, false);
	optmodel.doplanning(true);
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
#endif
	return 0;
}