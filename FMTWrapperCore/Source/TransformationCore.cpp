#include "TransformationCore.h"
#include "FMTmodel.h"
#include "FMTschedule.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"
#include "FMTerror.h"
#include "FMTexceptionhandler.h"
#include <boost/filesystem.hpp>



Models::FMTmodel FMTWrapperCore::Transformation::aggregateAllActions(
	const Models::FMTmodel& p_model, 
	const std::vector<std::string>& p_aggregates, 
	const std::vector<std::string>& p_order, 
	const std::string& p_primary_path, 
	const std::string& p_output_scenario_name)
{
	Models::FMTmodel aggregatedModel;
	try
	{
		Parser::FMTmodelparser ModelParser;
		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(p_primary_path, { p_model }).at(0);
		std::vector<std::string> scheduleOrder = p_model.getSchedulesPriorities(SCHEDULES);

		aggregatedModel = p_model.aggregateAllActions(p_aggregates, scheduleOrder);
		aggregatedModel.setname(p_output_scenario_name);
		std::string outputPath;
		
		//ATTENTION il est important d'�crire le model avant la schedule car modelParser.write va �craser le fichier de schedule
		ModelParser.writetoproject(p_primary_path, aggregatedModel);
		
		if (!SCHEDULES.empty())
		{
			
			Parser::FMTscheduleparser SCHEDULE_PARSER;
			const std::vector<Core::FMTschedule>NEWSCHEDULE = aggregatedModel.aggregateSchedules(SCHEDULES);
			std::string schedule_path = SCHEDULE_PARSER.getSchedulePath(p_primary_path, p_output_scenario_name);
			SCHEDULE_PARSER.write(NEWSCHEDULE, schedule_path);
		}

	}
	catch (...)
	{
		Exception::FMTexceptionhandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raisefromcatch("", "FMTWrapperCore::TransformationCore::aggregateAllActions", __LINE__, __FILE__);
	}
	return aggregatedModel;

}

Models::FMTmodel FMTWrapperCore::Transformation::splitActions(const Models::FMTmodel& p_model, const std::string& p_primary_path, const std::vector<std::string>& p_splitted, const std::vector<std::string>& p_splitted_mask, const std::string& p_scenario_name)
{
	Models::FMTmodel SPLITTED_MODEL;
	try
	{
		if (p_splitted.empty() || p_splitted_mask.empty()) {
			throw Exception::FMTexc::FMTempty_action;
		}
		Parser::FMTmodelparser ModelParser;

		SPLITTED_MODEL = p_model.splitActions(p_splitted, p_splitted_mask);
		SPLITTED_MODEL.setname(p_scenario_name);

		std::string outputPath;

		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(p_primary_path, {p_model}).at(0);
		//ATTENTION il est important d'ecrire le model avant la schedule car modelParser.write va ecraser le fichier de schedule

		ModelParser.writetoproject(p_primary_path, SPLITTED_MODEL);
		if (!SCHEDULES.empty())
		{
			Parser::FMTscheduleparser SCHEDULE_PARSER;
			const std::vector<Core::FMTschedule>NEWSCHEDULE = SPLITTED_MODEL.splitSchedules(SCHEDULES);
			std::string schedule_path = SCHEDULE_PARSER.getSchedulePath(p_primary_path, p_scenario_name);
			SCHEDULE_PARSER.write(NEWSCHEDULE, schedule_path);
		}

	}
	catch (...)
	{
		Exception::FMTexceptionhandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raisefromcatch("", "FMTWrapperCore::TransformationCore::splitActions", __LINE__, __FILE__);
	}
	return SPLITTED_MODEL;
}

Models::FMTmodel FMTWrapperCore::Transformation::buildAction(const Models::FMTmodel& p_model, const std::string& p_actionName, const std::string& p_targetYield, const std::string& p_primaryPath, const std::string& p_scenario_name)
{
	Models::FMTmodel BUILDED_MODEL;
	try
	{
		Parser::FMTmodelparser ModelParser;
		std::string outputPath;
		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(p_primaryPath, { p_model }).at(0);
		BUILDED_MODEL = p_model.buildAction(p_actionName, p_targetYield);
		BUILDED_MODEL.setname(p_scenario_name);

		//ATTENTION il est important d'�crire le model avant la schedule car modelParser.write va �craser le fichier de schedule

		ModelParser.writetoproject(p_primaryPath, BUILDED_MODEL);

		if (!SCHEDULES.empty())
		{
			Parser::FMTscheduleparser SCHEDULE_PARSER;
			const std::vector<Core::FMTschedule> NEW_SCHEDULE = BUILDED_MODEL.buildSchedule(*BUILDED_MODEL.getactions().begin(), SCHEDULES);
			std::string schedule_path = SCHEDULE_PARSER.getSchedulePath(p_primaryPath, p_scenario_name);
			SCHEDULE_PARSER.write(NEW_SCHEDULE, schedule_path);
		}
	}
	catch (...) {
		Exception::FMTexceptionhandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raisefromcatch("", "FMTWrapperCore::TransformationCore::buildAction", __LINE__, __FILE__);
	}
	return BUILDED_MODEL;
}
