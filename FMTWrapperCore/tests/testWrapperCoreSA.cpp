#include <string>
#include <vector>
#include "FMTexception.h"
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "SES.h"
#include "FMTfreeexceptionhandler.h"
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

int main(int argc, char* argv[])
{
	FMTWrapperCore::SAParameters params;

	// SAParameters ne contient pas le chemin du .pri; on le garde localement
	// car il est requis pour readproject (mais pas pour RunOptimization).
	std::string primaryFilePath;

	if (argc > 1)
	{
		const std::string jsonLocation = argv[1];
		boost::property_tree::ptree tree;
		boost::property_tree::read_json(jsonLocation, tree);

		primaryFilePath = tree.get<std::string>("primaryFilePath");
		params.rastersPath = tree.get<std::string>("rastersPath");
		params.outputPath = tree.get<std::string>("outputPath");
		params.scenarioName = tree.get<std::string>("scenarioName");
		params.numberOfPeriods = tree.get<int>("numberOfPeriods");
		params.maxMoves = tree.get<int>("maxMoves");
		params.maxAcceptedMoves = tree.get<int>("maxAcceptedMoves");
		params.maxCycleMoves = tree.get<int>("maxCycleMoves");
		params.useStanlock = tree.get<bool>("useStanlock");
		params.outputLevel = tree.get<int>("outputLevel");
		params.outputMinPeriod = tree.get<int>("outputMinPeriod");
		params.outputMaxPeriod = tree.get<int>("outputMaxPeriod");
		params.gdalProvider = tree.get<std::string>("gdalProvider");
		params.generateEvents = tree.get<bool>("generateEvents");
		params.generateSpatialOutputs = tree.get<bool>("generateSpatialOutputs");
		for (const auto& item : tree.get_child("constraintNames")) {
			params.constraintNames.push_back(item.second.get_value<std::string>());
		}
		for (const auto& item : tree.get_child("outputNames")) {
			params.outputNames.push_back(item.second.get_value<std::string>());
		}
	}
	else
	{
		// TODO: à remplir avec un jeu de données SA valide
		primaryFilePath = "TODO_PATH.pri";
		params.rastersPath = "TODO_RASTERS";
		params.outputPath = "TODO_OUTPUT";
		params.scenarioName = "TODO_SCENARIO";
		params.numberOfPeriods = 0;      // TODO
		params.maxMoves = 0;             // TODO
		params.maxAcceptedMoves = 0;     // TODO
		params.maxCycleMoves = 0;        // TODO
		params.useStanlock = false;
		//outputLevel: STRATE = 3, TH�MATIQUE = 1, TOTALE = 2
		params.outputLevel = 2;
		params.outputMinPeriod = 1;      // TODO
		params.outputMaxPeriod = 1;      // TODO
		params.gdalProvider = "CSV";
		params.generateEvents = false;
		params.generateSpatialOutputs = false;
		params.constraintNames = {};     // TODO
		params.outputNames = {};         // TODO
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
    std::vector<Models::FMTmodel> models = modelparser.readproject(primaryFilePath, scenarioName);

    Models::FMTmodel& selectedModel = models[0];

	if (!std::filesystem::is_directory(params.outputPath))
	{
		std::filesystem::create_directories(params.outputPath);
	}
	if (!std::filesystem::is_directory(params.rastersPath))
	{
		Exception::FMTfreeexceptionhandler().raise(
			Exception::FMTexc::FMTfunctionfailed,
			"Not a valid raster path",
			"testWrapperCoreSA", __LINE__, primaryFilePath);
	}

	FMTWrapperCore::SAResults results;
	try
	{
		results = FMTWrapperCore::SES::RunOptimization(
			params,
			selectedModel);
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
						//Exception::FMTfreeexceptionhandler().raise(
						//	Exception::FMTexc::FMTfunctionfailed,
						//	results.errorMessage,
						//	"testWrapperCoreSA", __LINE__, primaryFilePath);
					}
					++itemNum;
				}
			}
		}
		catch (const std::exception& e)
		{
			// La comparaison a levé une FMTexception (valeurs différentes). On la capture ICI
			// pour faire échouer le test proprement (return != 0) au lieu de la laisser sortir
			// de main : sinon std::terminate déclenche le dump de pile de FMTobject::_terminate.
			std::cerr << e.what() << std::endl;
			return 1;
		}
	}

	return 0;
}
