#include <string>
#include <vector>
#include "FMTexception.h"
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"
#include "SES.h"
#include "FMTfreeexceptionhandler.h"
#include <filesystem>
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
		params.primaryFilePath = "C:\\Users\\Admlocal\\Documents\\issues\\ses\\PC_7001892_U03772_SSP02_2022_DET\\PC_7001892_U03772_SSP02.pri";
		params.rastersPath = "C:\\Users\\Admlocal\\Documents\\issues\\ses\\PC_7001892_U03772_SSP02_2022_DET\\rasters\\";
		params.outputPath = "C:\\Users\\Admlocal\\Documents\\SCRAP";
		params.scenarioName = "15_Sc5_Determin_apsp_carbone";
		params.numberOfPeriods = 5;
		params.greedySearchIterations = 5;
		params.useStanlock = false;
		//outputLevel: STRATE = 3, TH�MATIQUE = 1, TOTALE = 2
		params.outputLevel = 2;
		params.outputMinPeriod = 1;
		params.outputMaxPeriod = 5;
		params.gdalProvider = "CSV";
		params.carbonMode = false;
		params.generateEvents = false;
		params.generateSpatialOutputs = false;
		params.constraintNames = {"_MAX OVOLTOTREC_YP23 2.._LENGTH"};
		params.outputNames = {"OVOLTOTREC"};
		params.predictorYields = {};
		params.growthThemes = {};
	}

	Parser::FMTmodelparser modelparser;
	modelparser.setDefaultExceptionHandler();
	modelparser.setDefaultLogger();
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
	modelparser.setErrorsToWarnings(errors);

    std::vector<std::string> scenarioName;
	scenarioName.push_back(params.scenarioName);
    std::vector<Models::FMTmodel> models = modelparser.readproject(params.primaryFilePath, scenarioName);

    Models::FMTmodel& selectedModel = models[0];

	std::vector<Core::FMTSchedule> schedules = modelparser.readschedules(params.primaryFilePath, models).at(0);

	if (!std::filesystem::is_directory(params.outputPath))
	{
		std::filesystem::create_directories(params.outputPath);
	}
	if (!std::filesystem::is_directory(params.rastersPath))
	{
		Exception::FMTFreeExceptionHandler().raise(
			Exception::FMTexc::FMTfunctionfailed,
			"Not a valid raster path",
			"testWrapperCoreSES", __LINE__, params.primaryFilePath);
	}

	FMTWrapperCore::SESResults results;
	try
	{
		results = FMTWrapperCore::SES::RunSES(
			params,        
			selectedModel,    
			schedules);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		try { std::rethrow_if_nested(e); }
		catch (const std::exception& nested) { std::cerr << nested.what() << std::endl; }
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown error" << std::endl;
		return 1;
	}

	if (argc > 1)
	{
		try {
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
					if ((std::abs(it->periodValues.at(itemNum) - item.second.get_value<double>())) * 100 / item.second.get_value<double>() > 5)
					{
						std::cout << ("Warning: " + std::to_string(it->periodValues.at(itemNum))
							+ "!=" + std::to_string(item.second.get_value<double>())
							+ " (> 5%) at period " + std::to_string(itemNum) + "\n");
						//Exception::FMTFreeExceptionHandler().raise(
						//	Exception::FMTexc::FMTfunctionfailed,
						//	results.errorMessage,
						//	"testWrapperCoreSES", __LINE__, params.primaryFilePath);
					}
					++itemNum;
				}
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return 1;
		}
	}

	return 0;
}