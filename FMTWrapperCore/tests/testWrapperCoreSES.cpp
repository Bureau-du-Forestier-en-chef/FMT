#include <string>
#include <vector>
#include "FMTexception.h"
#include "FMTmodel.h"
#include "FMTmodelparser.h"
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
		params.primaryFilePath = primaryFilePath;
		params.rastersPath = rastersPath;
		params.scenarioIndex = scenarioIndex;
		params.constraintNames = constraintNames;
		params.numberOfPeriods = numberOfPeriods;
		params.greedySearchIterations = greedySearchIterations;
		params.outputNames = outputNames;
		params.useStanlock = useStanlock;
		params.outputLevel = outputLevel;
		params.outputMinPeriod = outputMinPeriod;
		params.outputMaxPeriod = outputMaxPeriod;
		params.outputPath = outputPath;
		params.generateEvents = generateEvents;
		params.generateSpatialOutputs = generateSpatialOutputs;
		params.gdalProvider = gdalProvider;
		params.carbonMode = carbonMode;
		params.predictorYields = predictorYields;
		params.growthThemes = growthThemes;
	}

	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	modelparser.setdefaultlogger();
	modelparser.setTerminateStack();
	modelparser.setAbortStack();
	std::vector<Exception::FMTexc>errors;
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

	std::vector<Models::FMTmodel> models = modelparser.readproject(params.primaryFilePath, allscenarios);
	Parser::FMTscheduleparser schedule_parser;
	models.push_back(FMTFormCache::GetInstance()->getmodel(params.scenarioIndex));
	std::vector<Core::FMTschedule> schedules = Modelparser.readschedules(params.primaryFilePath), models).at(0);

	FMTWrapperCore::SES::RunSES(params, models, schedules);

	return 0;
}