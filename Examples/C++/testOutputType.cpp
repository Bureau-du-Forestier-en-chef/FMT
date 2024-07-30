#include "FMTmodelparser.h"
#include "FMTfreeexceptionhandler.h"
#ifdef FMTWITHOSI
#include "FMTlpmodel.h"
#endif

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	const std::string primarylocation = std::string(argv[1]);
	const std::vector<std::string>scenarios(1, std::string(argv[2]));
	const size_t N_OUTPUT = static_cast<size_t>(16);
	//const std::vector<std::string>scenarios(1, "output_type");
	//const std::string primarylocation = "../../../../Examples/Models/TWD_land/TWD_land.pri";
	//const std::string primarylocation = "D:/CC_modele_feu/WS_CC/Feux_2023_ouest_V01.pri";
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	const std::string outdir = "../../tests/testOutputType/";
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	modelparser.seterrorstowarnings(errors);
	//const std::vector<std::string>scenarios(1, "tactique");
	//const std::vector<std::string>scenarios(1, "strategique");
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
	//optmodel.writeLP("D:/FMT/build/release/tests/testmodelwriter/strategic");
	//return 0;
	modelparser.write(models.at(0), outdir); // On va chercher le modèle
	const std::vector<Models::FMTmodel> rereadmodels = modelparser.readproject(outdir + models.at(0).getname() + ".pri", std::vector<std::string>(1, "ROOT"));
	Models::FMTlpmodel optmodel = Models::FMTlpmodel(rereadmodels.at(0), Models::FMTsolverinterface::MOSEK);
	//modelparser.write(optmodel, outdir+"other/");
	if (optmodel.getoutputs().size() != N_OUTPUT)
	{
		std::cout << "ok" << std::endl;
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Mauvais output",
			"FMTsetsolution", __LINE__, primarylocation);
	}
#endif
	return 0;
}
