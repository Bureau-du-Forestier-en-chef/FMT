#include "TransformationCore.h"
#include "FMTModel.h"
#include "FMTSchedule.h"
#include "FMTModelParser.h"
#include "FMTScheduleParser.h"
#include "FMTError.h"
#include "FMTExceptionHandler.h"
#include <boost/filesystem.hpp>



Models::FMTModel FMTWrapperCore::Transformation::aggregateAllActions(
	const Models::FMTModel& p_model, 
	const std::vector<std::string>& p_aggregates, 
	const std::vector<std::string>& p_order, 
	const std::string& p_primary_path, 
	const std::string& p_output_scenario_name)
{
	Models::FMTModel aggregatedModel;
	try
	{
		Parser::FMTModelParser ModelParser;
		const std::vector<Core::FMTSchedule>SCHEDULES = ModelParser.readSchedules(p_primary_path, { p_model }).at(0);
		std::vector<std::string> scheduleOrder = p_model.getSchedulesPriorities(SCHEDULES);

		aggregatedModel = p_model.aggregateAllActions(p_aggregates, scheduleOrder);
		aggregatedModel.setName(p_output_scenario_name);
		std::string outputPath;
		
		//ATTENTION il est important d'�crire le model avant la schedule car modelParser.write va �craser le fichier de schedule
		ModelParser.writeToProject(p_primary_path, aggregatedModel);
		
		if (!SCHEDULES.empty())
		{
			
			Parser::FMTScheduleParser SCHEDULE_PARSER;
			const std::vector<Core::FMTSchedule>NEWSCHEDULE = aggregatedModel.aggregateSchedules(SCHEDULES);
			std::string schedule_path = SCHEDULE_PARSER.getSchedulePath(p_primary_path, p_output_scenario_name);
			SCHEDULE_PARSER.write(NEWSCHEDULE, schedule_path);
		}

	}
	catch (...)
	{
		Exception::FMTExceptionHandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raiseFromCatch("", "FMTWrapperCore::TransformationCore::aggregateAllActions", __LINE__, __FILE__);
	}
	return aggregatedModel;

}

Models::FMTModel FMTWrapperCore::Transformation::splitActions(const Models::FMTModel& p_model, const std::string& p_primary_path, const std::vector<std::string>& p_splitted, const std::vector<std::string>& p_splitted_mask, const std::string& p_scenario_name)
{
	Models::FMTModel SPLITTED_MODEL;
	try
	{
		if (p_splitted.empty() || p_splitted_mask.empty()) {
			throw Exception::FMTexc::FMTempty_action;
		}
		Parser::FMTModelParser ModelParser;

		SPLITTED_MODEL = p_model.splitActions(p_splitted, p_splitted_mask);
		SPLITTED_MODEL.setName(p_scenario_name);

		std::string outputPath;

		const std::vector<Core::FMTSchedule>SCHEDULES = ModelParser.readSchedules(p_primary_path, {p_model}).at(0);
		//ATTENTION il est important d'ecrire le model avant la schedule car modelParser.write va ecraser le fichier de schedule

		ModelParser.writeToProject(p_primary_path, SPLITTED_MODEL);
		if (!SCHEDULES.empty())
		{
			Parser::FMTScheduleParser SCHEDULE_PARSER;
			const std::vector<Core::FMTSchedule>NEWSCHEDULE = SPLITTED_MODEL.splitSchedules(SCHEDULES);
			std::string schedule_path = SCHEDULE_PARSER.getSchedulePath(p_primary_path, p_scenario_name);
			SCHEDULE_PARSER.write(NEWSCHEDULE, schedule_path);
		}

	}
	catch (...)
	{
		Exception::FMTExceptionHandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raiseFromCatch("", "FMTWrapperCore::TransformationCore::splitActions", __LINE__, __FILE__);
	}
	return SPLITTED_MODEL;
}

Models::FMTModel FMTWrapperCore::Transformation::buildAction(const Models::FMTModel& p_model, const std::string& p_actionName, const std::string& p_targetYield, const std::string& p_primaryPath, const std::string& p_scenario_name)
{
	Models::FMTModel BUILDED_MODEL;
	try
	{
		Parser::FMTModelParser ModelParser;
		std::string outputPath;
		const std::vector<Core::FMTSchedule>SCHEDULES = ModelParser.readSchedules(p_primaryPath, { p_model }).at(0);
		BUILDED_MODEL = p_model.buildAction(p_actionName, p_targetYield);
		BUILDED_MODEL.setName(p_scenario_name);

		//ATTENTION il est important d'�crire le model avant la schedule car modelParser.write va �craser le fichier de schedule

		ModelParser.writeToProject(p_primaryPath, BUILDED_MODEL);

		if (!SCHEDULES.empty())
		{
			Parser::FMTScheduleParser SCHEDULE_PARSER;
			const std::vector<Core::FMTSchedule> NEW_SCHEDULE = BUILDED_MODEL.buildSchedule(*BUILDED_MODEL.getActions().begin(), 
				p_model,p_targetYield, SCHEDULES);
			std::string schedule_path = SCHEDULE_PARSER.getSchedulePath(p_primaryPath, p_scenario_name);
			SCHEDULE_PARSER.write(NEW_SCHEDULE, schedule_path);
		}
	}
	catch (...) {
		Exception::FMTExceptionHandler* modelExceptionHandler = p_model.getExceptionHandler();
		modelExceptionHandler->raiseFromCatch("", "FMTWrapperCore::TransformationCore::buildAction", __LINE__, __FILE__);
	}
	return BUILDED_MODEL;
}
