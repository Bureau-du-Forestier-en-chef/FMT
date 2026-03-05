#include <string>
#include <vector>
#include "FMTexception.h"
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"
#include "SES.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

int main(int argc, char* argv[])
{
	FMTWrapperCore::SESParameters params;

	if (argc > 1) 
	{
		// À faire
	}
	else
	{
		params.primaryFilePath = "C:\\Users\\Admlocal\\Documents\\FMT\\model_test\\PC_7001892_U03772_SSP02_2022_DET\\PC_7001892_U03772_SSP02_2022_DET\\PC_7001892_U03772_SSP02.pri";
		params.rastersPath = "C:\\Users\\Admlocal\\Documents\\FMT\\model_test\\PC_7001892_U03772_SSP02_2022_DET\\PC_7001892_U03772_SSP02_2022_DET\\rasters\\";
		params.outputPath = "C:\\Users\\Admlocal\\Documents\\FMT\\model_test\\PC_7001892_U03772_SSP02_2022_DET\\output";
		params.scenarioName = "15_Sc5_Determin_apsp_carbone";
		params.numberOfPeriods = 5;
		params.greedySearchIterations = 5;
		params.useStanlock = false;
		//outputLevel: STRATE = 3, THÉMATIQUE = 1, TOTALE = 2
		params.outputLevel = 2;
		params.outputMinPeriod = 1;
		params.outputMaxPeriod = 5;
		params.gdalProvider = "CSV";
		params.carbonMode = false;
		params.generateEvents = true;
		params.generateSpatialOutputs = true;
		params.constraintNames = { "_MAX OVOLTOTREC_YP23 2.._LENGTH \n" };
		params.outputNames = { "OVOLTOTREC"}; 
		params.predictorYields = {};
		params.growthThemes = {};
	}

	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	modelparser.setdefaultlogger();
	modelparser.setTerminateStack();
	modelparser.setAbortStack();

	std::vector<Exception::FMTexc> errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
	errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
	errors.push_back(Exception::FMTexc::FMTunclosedforloop);
	errors.push_back(Exception::FMTexc::FMToutofrangeyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	errors.push_back(Exception::FMTexc::FMTempty_schedules);
	errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
	modelparser.seterrorstowarnings(errors);

    std::vector<std::string> scenarioName;
	scenarioName.push_back(params.scenarioName);
    std::vector<Models::FMTmodel> models = modelparser.readproject(params.primaryFilePath, scenarioName);

    Models::FMTmodel& selectedModel = models[0];

	std::vector<Core::FMTschedule> schedules = modelparser.readschedules(params.primaryFilePath, models).at(0);

    FMTWrapperCore::SESResults results = FMTWrapperCore::SES::RunSES(
        params,        
        selectedModel,    
        schedules);       

	return 0;
}