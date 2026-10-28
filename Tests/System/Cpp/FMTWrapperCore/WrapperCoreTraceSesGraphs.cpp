/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "FMTException.h"
#include "FMTModel.h"
#include "FMTSesModel.h"
#include "FMTModelParser.h"
#include "FMTAreaParser.h"
#include "FMTForest.h"
#include "FMTSpatialSchedule.h"
#include "FMTVirtualLineGraph.h"
#include "FMTLineGraph.h"
#include "FMTOutput.h"
#include "Selection.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

/*
DIAGNOSTIC vector too long: replays the steps of SES::RunSES one by one (greedyReferenceBuild per period, then
postSolve, then the outputs of SES::calculateOutputs) and writes to std::cout the line graphs of the cells that
break the invariants of their layout (FMTGraph::getIncoherences) after each step. A failure gives the details
of the graphs in its message. Nothing is written in the folder of the rasters.
Takes the JSON of parameters of WrapperCoreRunSes, or uses the model 08451 of the issue.
*/

namespace
{
	//Writes the number of line graphs used by the cells and the details of those breaking the invariants of their layout.
	size_t traceGraphs(const Models::FMTSesModel& p_model, const std::string& p_step)
	{
		const Spatial::FMTSpatialSchedule SCHEDULE = p_model.getSpSchedule();
		std::map<const Graph::FMTLineGraph*, size_t> cellsByGraph;
		for (const auto& CELL : SCHEDULE)
		{
			++cellsByGraph[&CELL.second.getLineGraph()];
		}
		size_t graphsInError = 0;
		for (const auto& GRAPH : cellsByGraph)
		{
			const std::string INCOHERENCES = GRAPH.first->getIncoherences();
			if (!INCOHERENCES.empty())
			{
				if (graphsInError < 20)
				{
					std::cout << "TRACE " << p_step << " graph of " << GRAPH.second << " cells, base "
						<< std::string(GRAPH.first->getBaseDevelopment()) << " incoherences:" << INCOHERENCES
						<< " " << GRAPH.first->getDiagnosticString() << "\n";
				}
				++graphsInError;
			}
		}
		std::cout << "TRACE " << p_step << " : " << SCHEDULE.size() << " cells, " << cellsByGraph.size()
			<< " graphs, " << graphsInError << " in error\n" << std::flush;
		return graphsInError;
	}
}

int main(int argc, char* argv[])
{
#ifdef FMTWITHGDAL
	const std::string MODEL = "C:/Users/admlocal/Documents/Dossier_de_travail/Modeles/modele_julianne_8451/";
	std::string primary = MODEL + "PC_9964_U08451_2028_MODB01.pri";
	std::string scenario = "400_Carbone_apsp";
	std::string rasters = MODEL + "rasters/";
	int length = 30;
	int iterations = 30;
	bool useStanlock = true;
	std::vector<std::string> constraintNames = {
		"_MAX OVOLTOTREC_YP23 2.._LENGTH",
		"_EVEN(OVOLTOTREC) 2..3", "_NDY(OVOLTOTREC) 3.._LENGTH",
		"_EVEN(OVOLGRREC) 2..3", "_NDY(OVOLGRREC) 3.._LENGTH",
		"_EVEN(OVOLGFIREC) 2..3", "_NDY(OVOLGFIREC) 3.._LENGTH",
		"OVOLTOTREC <= 3383200.000000 1", "OVOLTOTREC >= 3349400.000000 1",
		"OVOLGFIREC <= 751200.000000 1", "OVOLGFIREC >= 736400.000000 1",
		"OVOLGRREC <= 2677400.000000 1", "OVOLGRREC >= 2624400.000000 1" };
	std::vector<std::string> outputNames = { "OVOLTOTREC", "OVOLGFIREC", "OVOLGFTREC", "OVOLGRREC" };
	if (argc > 1)
	{
		boost::property_tree::ptree tree;
		boost::property_tree::read_json(argv[1], tree);
		primary = tree.get<std::string>("primaryFilePath");
		scenario = tree.get<std::string>("scenarioName");
		rasters = tree.get<std::string>("rastersPath");
		length = tree.get<int>("numberOfPeriods");
		iterations = tree.get<int>("greedySearchIterations");
		useStanlock = tree.get<bool>("useStanlock");
		constraintNames.clear();
		for (const auto& item : tree.get_child("constraintNames"))
		{
			constraintNames.push_back(item.second.get_value<std::string>());
		}
		outputNames.clear();
		for (const auto& item : tree.get_child("outputNames"))
		{
			outputNames.push_back(item.second.get_value<std::string>());
		}
	}

	Parser::FMTModelParser modelparser;
	modelparser.setDefaultExceptionHandler();
	modelparser.setDefaultLogger();
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
	const std::vector<Models::FMTModel> models = modelparser.readproject(primary, std::vector<std::string>(1, scenario));
	const std::vector<Core::FMTSchedule> schedules = modelparser.readSchedules(primary, models).at(0);

	//The setup of SES::RunSES
	Models::FMTSesModel model(models.at(0));
	if (!constraintNames.empty())
	{
		model.setConstraints(FMTWrapper::Backend::Selection::selectConstraints(model.getConstraints(), constraintNames));
	}
	std::cout << "TRACE " << model.getConstraints().size() << " constraints selected\n";
	std::vector<Core::FMTTransition> singleTransitions;
	for (const Core::FMTTransition& transition : model.getTransitions())
	{
		singleTransitions.push_back(transition.single());
	}
	model.setTransitions(singleTransitions);
	std::vector<std::string> themeRasters;
	for (size_t theme = 1; theme <= model.getThemes().size(); ++theme)
	{
		themeRasters.push_back(rasters + "THEME" + std::to_string(theme) + ".tif");
	}
	Parser::FMTAreaParser areaparser;
	const Spatial::FMTForest FOREST = useStanlock ?
		areaparser.readRasters(model.getThemes(), themeRasters, rasters + "AGE.tif", 1, 0.0001, rasters + "STANLOCK.tif") :
		areaparser.readRasters(model.getThemes(), themeRasters, rasters + "AGE.tif", 1, 0.0001);
	model.setInitialMapping(FOREST);
	model.setParameter(Models::FMTintmodelparameters::LENGTH, length);
	model.setParameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, iterations);
	model.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
	model.setParameter(Models::FMTboolmodelparameters::POSTSOLVE, true);

	//The steps of FMTModel::doPlanning, traced one by one
	size_t shortGraphs = 0;
	size_t failures = 0;
	try {
		//doPlanning presolves with the area member, which FMTModel::getArea returns
		std::unique_ptr<Models::FMTModel> presolved = model.preSolve(model.Models::FMTModel::getArea());
		Models::FMTSesModel& sesModel = *dynamic_cast<Models::FMTSesModel*>(presolved.get());
		shortGraphs += traceGraphs(sesModel, "preSolve");
		for (int period = 1; period <= length; ++period)
		{
			//As FMTModel::setUpSchedulesForBuild: an empty schedule when the period has none
			Core::FMTSchedule periodSchedule;
			for (const Core::FMTSchedule& schedule : schedules)
			{
				if (schedule.getPeriod() == period)
				{
					periodSchedule = sesModel.preSolveSchedule(schedule, model);
					break;
				}
			}
			sesModel.greedyReferenceBuild(periodSchedule, static_cast<size_t>(iterations));
			shortGraphs += traceGraphs(sesModel, "greedyReferenceBuild period " + std::to_string(period));
		}
		std::cout << "TRACE postSolve starts\n" << std::flush;
		sesModel.postSolve(model);
		shortGraphs += traceGraphs(sesModel, "postSolve");

		//The outputs of SES::calculateOutputs, each failure written
		for (const Core::FMTOutput& output : FMTWrapper::Backend::Selection::selectOutputs(sesModel.getOutputs(), outputNames))
		{
			for (int period = 1; period <= length; ++period)
			{
				try {
					sesModel.getOutput(output, period, Core::FMToutputlevel::totalonly);
				}
				catch (const std::exception& error)
				{
					++failures;
					std::cout << "TRACE getOutput " << output.getName() << " period " << period
						<< " failed: " << error.what() << "\n" << std::flush;
				}
			}
		}
	}
	catch (const std::exception& error)
	{
		++failures;
		std::cout << "TRACE failed: " << error.what() << "\n" << std::flush;
	}
	std::cout << "TRACE end: " << shortGraphs << " graphs in error used by cells, "
		<< failures << " failures\n";
	if (shortGraphs > 0 || failures > 0)
	{
		return 1;
	}
#endif
	return 0;
}
