#include <vector>
#include <string>
#ifdef FMTWITHOSI
	#include "FMTtaskhandler.h"
	#include "FMTreplanningtask.h"
	#include "FMTlpmodel.h"
	#include "FMTnssmodel.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTmodelparser.h"
#endif
	#include "FMTdefaultlogger.h"
#include <boost/filesystem.hpp>

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
	const bool writeschedule = true;
	std::string primlocation;
	int length;
	int replicate;
	std::vector<std::string>allscenarios;
	std::string outputlocation = "../../tests/replanner/Feux_2023_ouest_V01";
	if (argc > 1) {
		primlocation = argv[1];
		length = std::stoi(argv[2]);
		replicate = std::stoi(argv[3]);
		allscenarios.push_back("Globalreplanning");
		allscenarios.push_back("Globalfire");
		allscenarios.push_back("Localreplanning");
	}
	else
	{
		primlocation = "D:/CC_modele_feu/WS_CC/Feux_2023_ouest_V01.pri";
		length = 5;
		replicate = 5;
		allscenarios.push_back("strategique"); //Pour test le lancé d'erreur
		//allscenarios.push_back("strategique_minimisation_Risque");
		allscenarios.push_back("stochastique_CC");
		allscenarios.push_back("tactique");
	}

	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	std::vector<Exception::FMTexc> errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	modelparser.seterrorstowarnings(errors);
	std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
	std::cout << models.at(0).getname() << std::endl;
	#ifdef FMTWITHMOSEK
		Models::FMTlpmodel global(models.at(0), Models::FMTsolverinterface::MOSEK);
	#else
		Models::FMTlpmodel global(models.at(0), Models::FMTsolverinterface::CLP);
	#endif
	global.setparameter(Models::FMTintmodelparameters::LENGTH, length);
	/*global.setcompresstime(10, 12, 2);
	global.setcompresstime(12, 14, 3);
	global.setcompresstime(14, 16, 4);
	global.setcompresstime(16, 30, 5);*/
	global.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
	global.setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
	Models::FMTnssmodel stochastic(models.at(1), 0);
	stochastic.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	#ifdef FMTWITHMOSEK
		Models::FMTlpmodel local(models.at(2), Models::FMTsolverinterface::MOSEK);
	#else
		Models::FMTlpmodel local(models.at(2), Models::FMTsolverinterface::CLP);
	#endif
	local.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	local.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,1);
	std::vector<std::string>OutputtoLookFor = { "OVOLTOTREC","ORISQUE", "OVOL_UA_TOTREC" ,"OSUPBRULER_CORRIGER" ,
												"SUPERFICIE_RECUP_FEU" ,/*"OSUPPL_FEU_POSTRECUP",*/
													"OSUPTBE" , "SUPERFICIE_RECUP_TBE",
												/*"OCATTBE_C1" ,"OCATTBE_C2" ,"OCATTBE_C3",
												"OCATTBE_C4","OCATTBE_C5","OCATTBE_C6","OTBECOMP","OSUPADMATBE"*/};
	std::vector<Core::FMToutput>selectedoutputs;
	for (const Core::FMToutput& output : global.getoutputs())
	{
		if (std::find(OutputtoLookFor.begin(), OutputtoLookFor.end(), output.getname()) != OutputtoLookFor.end())
		{
			selectedoutputs.push_back(output);
		}
	}
	std::size_t lastslash = primlocation.find_last_of("/\\");
	const std::string locname = primlocation.substr(lastslash + 1, (primlocation.size() - lastslash) - 5);
	//const std::string outputlocation = "../../tests/replanner/"+ locname;

	std::vector<std::string> layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTreplanningtask(
		global, stochastic, local, selectedoutputs, outputlocation, "CSV", layersoptions, 
		replicate, length, 0.5, Core::FMToutputlevel::standard, writeschedule)); //test du bool writeschedule
	Parallel::FMTtaskhandler handler(maintaskptr, 3); // FIXME diminuer 5 � 1 pour le debuggage
	//handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	#endif
	return 0;
	}

