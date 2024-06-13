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

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
	/*const std::string primlocation = "D:/CC_modele_feu/WS_CC/Feux_2023_ouest_V01.pri";
	const int length = 10;
	const int replicate = 5;
	std::vector<std::string>allscenarios;
	allscenarios.push_back("strategique");
	allscenarios.push_back("stochastique");//"stochastique_Histo");
	allscenarios.push_back("tactique");*/
	const std::string primlocation = argv[1];
	const int length = std::stoi(argv[2]);
	const int replicate = std::stoi(argv[3]);
	std::vector<std::string>allscenarios;
	allscenarios.push_back("Globalreplanning");
	allscenarios.push_back("Globalfire");
	allscenarios.push_back("Localreplanning");
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	modelparser.seterrorstowarnings(errors);
	std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
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
	global.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,1);
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
	std::vector<std::string>OutputtoLookFor = { "OVOLTOTREC" ,/*"OSUPTOT",*/"OVOL_UA_TOTREC" ,"OSUPBRULER_CORRIGER" ,
												"SUPERFICIE_RECUP_FEU" ,/*"OSUPPL_FEU_POSTRECUP",*/
													"OSUPTBE" , "SUPERFICIE_RECUP_TBE",
												/*"OCATTBE_C1" ,"OCATTBE_C2" ,"OCATTBE_C3",
												"OCATTBE_C4","OCATTBE_C5","OCATTBE_C6","OTBECOMP","OSUPADMATBE"*/};
	std::vector<Core::FMToutput>selectedoutputs;
	for (const Core::FMToutput& output : global.getoutputs())
	{
		if (std::find(OutputtoLookFor.begin(), OutputtoLookFor.end(), output.getname())!= OutputtoLookFor.end())
		{
			selectedoutputs.push_back(output);
		}
	}
	std::size_t lastslash = primlocation.find_last_of("/\\");
	const std::string locname = primlocation.substr(lastslash+1,(primlocation.size()-lastslash)-4);
	const std::string outputlocation = "../../tests/replanner/"+ locname;
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTreplanningtask(global, stochastic, local, selectedoutputs, outputlocation, "CSV", layersoptions, replicate,length,0.5, Core::FMToutputlevel::standard));
	Parallel::FMTtaskhandler handler(maintaskptr,5);
	//handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	#endif
	return 0;
	}

