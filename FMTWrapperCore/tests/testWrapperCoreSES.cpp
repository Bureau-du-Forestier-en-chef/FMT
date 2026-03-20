#include <string>
#include <vector>
#include "FMTexception.h"
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"
#include "SES.h"
#include "FMTfreeexceptionhandler.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

int main(int argc, char* argv[])
{
	FMTWrapperCore::SESParameters params;

	if (argc > 1) 
	{
		const std::string jsonLocation = argv[1];
		boost::property_tree::ptree tree;
		boost::property_tree::read_json(jsonLocation, tree);

		params.primaryFilePath = tree.get<std::string>("primaryFilePath");
		params.rastersPath = tree.get<std::string>("rastersPath");
		params.outputPath = tree.get<std::string>("outputPath");
		params.scenarioName = tree.get<std::string>("scenarioName");
		params.numberOfPeriods = tree.get<int>("numberOfPeriods");
		params.greedySearchIterations = tree.get<int>("greedySearchIterations");
		params.useStanlock = tree.get<bool>("useStanlock");
		params.outputLevel = tree.get<int>("outputLevel");
		params.outputMinPeriod = tree.get<int>("outputMinPeriod");
		params.outputMaxPeriod = tree.get<int>("outputMaxPeriod");
		params.gdalProvider = tree.get<std::string>("gdalProvider");
		params.carbonMode = tree.get<bool>("carbonMode");
		params.generateEvents = tree.get<bool>("generateEvents");
		params.generateSpatialOutputs = tree.get<bool>("generateSpatialOutputs");
		for (const auto& item : tree.get_child("constraintNames")) {
			params.constraintNames.push_back(item.second.get_value<std::string>());
		}
		for (const auto& item : tree.get_child("outputNames")) {
			params.outputNames.push_back(item.second.get_value<std::string>());
		}
		for (const auto& item : tree.get_child("predictorYields")) {
			params.predictorYields.push_back(item.second.get_value<std::string>());
		}
		for (const auto& item : tree.get_child("growthThemes")) {
			params.growthThemes.push_back(item.second.get_value<int>());
		}
	}
	else
	{
		params.primaryFilePath = "C:\\Users\\Admlocal\\Documents\\FMT\\model_test\\PC_7001892_U03772_SSP02_2022_DET\\PC_7001892_U03772_SSP02_2022_DET\\PC_7001892_U03772_SSP02.pri";
		params.rastersPath = "C:\\Users\\Admlocal\\Documents\\FMT\\model_test\\PC_7001892_U03772_SSP02_2022_DET\\PC_7001892_U03772_SSP02_2022_DET\\rasters\\";
		params.outputPath = "C:\\Users\\Admlocal\\Documents\\FMT\\model_test\\PC_7001892_U03772_SSP02_2022_DET\\output";
		params.scenarioName = "15_Sc5_Determin_apsp_carbone";
		params.numberOfPeriods = 20;
		params.greedySearchIterations = 5;
		params.useStanlock = false;
		//outputLevel: STRATE = 3, TH�MATIQUE = 1, TOTALE = 2
		params.outputLevel = 2;
		params.outputMinPeriod = 1;
		params.outputMaxPeriod = 20;
		params.gdalProvider = "CSV";
		params.carbonMode = false;
		params.generateEvents = true;
		params.generateSpatialOutputs = true;
		params.constraintNames = {"_MAX OVOLTOTREC_YP23 2.._LENGTH"};
		params.outputNames = {"OVOLTOTREC", "OSUPREGECOCOS"};
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

	if (argc > 1)
	{
		std::string resultsJsonLocation = argv[2];
		boost::property_tree::ptree resultsTree;
		boost::property_tree::read_json(resultsJsonLocation, resultsTree);

		for (const std::string& output : params.outputNames)
		{
			auto it = std::find_if(results.outputsData.results.begin(), results.outputsData.results.end(),
				[&output](const auto& item) { return item.outputName == output; });

			if (it == results.outputsData.results.end()) continue;

			int itemNum = 1;
			for (const auto& item : resultsTree.get_child(output))
			{
				if (std::abs(it->periodValues.at(itemNum) - item.second.get_value<double>()) >= 1)
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
						"testWrapperCoreSES", __LINE__, argv[2]);
				}
				++itemNum;
			}
		}
	}

	return 0;
}