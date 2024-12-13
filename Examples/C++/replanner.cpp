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
	unsigned int n_threads = 5;
	if (argc > 1) {
		const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals, boost::is_any_of("|"));
		primlocation = results[0];
		allscenarios.push_back(results[1]);
		allscenarios.push_back(results[2]);
		allscenarios.push_back(results[3]);
		outputlocation = results[4];
		n_threads = std::stoi(results[5]);
		length = std::stoi(argv[2]);
		replicate = std::stoi(argv[3]);
		/*allscenarios.push_back("Globalreplanning");
		allscenarios.push_back("Globalfire");
		allscenarios.push_back("Localreplanning");*/
	}
	else
	{
		primlocation = "D:/CC_modele_feu/WS_CC/Feux_2023_ouest_V01.pri";
		length = 20;
		replicate = 5;
		n_threads = 5;
		allscenarios.push_back("strategique_Maximisation_non_risqueFR30_CC"); //Pour test le lancé d'erreur
		//allscenarios.push_back("strategique");
		allscenarios.push_back("stochastique_CC_2pics");
		//allscenarios.push_back("tactique");
		allscenarios.push_back("tactique_AllEnrqc_CC");
	}

	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	//modelparser.setdebuglogger();
	modelparser.setTerminateStack();
	modelparser.setAbortStack();
	std::vector<Exception::FMTexc> errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	modelparser.seterrorstowarnings(errors);
	//std::vector<Models::FMTmodel> tests = modelparser.readproject(primlocation, allscenarios);
	//tests[0].presolve();
	std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
	std::cout << models.at(0).getname() << std::endl;
	//modelparser.write(models.at(0), "D:/test/");


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
	std::vector<std::string>OutputtoLookFor = { "OVOLTOTREC"/*,"ORISQUE","ORISQUE_NOR", "OVOL_UA_TOTREC" ,"OSUPBRULER_ORI","OSUPBRULER_CORRIGER" ,
												"SUPERFICIE_RECUP_FEU" ,"OSUPPLEP_FEU23_UA","OSUPPL_FEU_POSTRECUP",
													"OSUPTBE" , "SUPERFICIE_RECUP_TBE",
												"OCATTBE_C1" ,"OCATTBE_C2" ,"OCATTBE_C3",
												"OCATTBE_C4","OCATTBE_C5","OCATTBE_C6","OTBECOMP","OSUPADMATBE",
	"ORISQUE","ORISQUE_FEU","ORISQUE_TBE","ORISQUE_FEU_NOR","ORISQUE_TBE_NOR","ORISQUE_NOR","OVOLGFIREC","OVOLGFTREC","OVOLGRREC",
	"SS_10","SS_25","SS_50","SS_100","SS_200","SS_400","SR_10","SR_25","SR_50","SR_100","SR_200","SR_400",
	"REST_10","REST_25","REST_50","REST_100","REST_200","REST_400","ORISQUEYVS",
	"OVOLTOTREC","OVOLGSEPMREC","OVOLTOTREC_RECUP","OVOLTOTRECARECUP","OSUPREALREGAREG","OSUPREALPL_BR","OSUPREALREGAREG_TOT","OSUPREALREGAEDU_TOT","OSUPREALREGAEDU","OSUPREALREGAEDU_BR","OSUPREALREGAPAR_TOT","OSUPREALREGAPAR","OSUPREALREGAPAR_BR","OSUPBRULER_ORI","OSUPBRULER_CORRIGER","MORT_ADM","OSUPMORTFEUX","OSUPTBE","OTBECOMP","OCATTBE_SS","OCATTBE_SR","OCATTBE_SF","OCATTBE_FRS","OCATTBE_PE","OCATTBE_AU","SUPERFICIE_RECUP_FEU","SUPERFICIE_ADM_RECUP_FEU","OSUPPLPIG_FEU23_UA_POSTRECUP","OSUPPLEP_FEU23_UA_POSTRECUP","OSUPPL_FEU_POSTRECUP","OSUPPLPIG_FEU23_UA_POSTRECUP_ADM","OSUPPLEP_FEU23_UA_POSTRECUP_ADM","OSUPPL_FEU_POSTRECUP_ADM","SUP_ADM_ACPPTM_BR","OSUPADM_RECUP_FEU_M3HA_SEPM_70_99","OSUPADM_RECUP_FEU_M3HA_SEPM_100_124","OSUPADM_RECUP_FEU_M3HA_SEPM_125_999","SUPERFICIE_RECUP_TBE","SUPERFICIE_ADM_RECUP_TBE","OVOLTOTRECARECUP","OVOLTOTREC_RECUP","OSUPREALPL","OVOLSABPIEDINC","OVOLGEPXPIEDINC","OVOLGFIPIEDINC","OVOLGFTPIEDINC","ORISQUE","ORISQUE_FEU","ORISQUE_TBE","OVOLGRPIEDINC","OSUPINC","OSUPECHEC"*/};
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
	Parallel::FMTtaskhandler handler(maintaskptr, n_threads); // FIXME diminuer 5 � 1 pour le debuggage
	//handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	#endif
	return 0;
	}

