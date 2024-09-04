#include "TransformationCore.h"
#include "FMTmodel.h"
#include "FMTschedule.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"

Models::FMTmodel FMTWrapperCore::Transformation::aggregateAllActions(const Models::FMTmodel& p_model, const std::vector<std::string>& p_aggregates, const std::vector<std::string>& p_order, const std::string& p_schedulePri, const std::string& p_outputDirPath, const std::string& p_scenario_name)
{
	Models::FMTmodel aggregatedModel;
	try
	{
		Parser::FMTmodelparser ModelParser;

		const std::vector<std::string> SCENARIOS(1, p_scenario_name);
		const std::vector<Models::FMTmodel> MODELS = { p_model };
		aggregatedModel = MODELS.at(0).aggregateAllActions(p_aggregates);
		aggregatedModel.setname(p_scenario_name);

		//const std::vector<Models::FMTmodel> MODELS = { MODEL };
		const std::string OUTPUT_DIRECTORY = p_outputDirPath;
		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(p_schedulePri, MODELS).at(0);
		//ATTENTION il est important d'écrire le model avant la schedule car modelParser.write va écraser le fichier de schedule
		#ifdef _WIN32
		ModelParser.write(aggregatedModel, OUTPUT_DIRECTORY + "\\");
		#else
		ModelParser.write(aggregatedModel, OUTPUT_DIRECTORY + "/");
		#endif
		if (!SCHEDULES.empty())
		{
			Parser::FMTscheduleparser SCHEDULE_PARSER;
			const std::vector<Core::FMTschedule>NEWSCHEDULE = aggregatedModel.aggregateSchedules(SCHEDULES);
		#ifdef _WIN32
			SCHEDULE_PARSER.write(NEWSCHEDULE, OUTPUT_DIRECTORY + "\\" + p_scenario_name + ".seq");
		#else
			SCHEDULE_PARSER.write(NEWSCHEDULE, OUTPUT_DIRECTORY + "/" + SCENARIO + ".seq");
		#endif
		}

	}
	catch (...)
	{
		throw std::exception("Error in Wrapper::FMTModelWrapper::aggregateAllActions");
	}
	return aggregatedModel;

}
Models::FMTmodel FMTWrapperCore::Transformation::splitActions(Models::FMTmodel& p_model, const std::string& p_schedulePri, const std::vector<std::string>& p_splitted, const std::vector<std::string>& p_splitted_mask, const std::string& p_outputDirPath, const std::string& p_scenario_name)
{
	Models::FMTmodel SPLITTED_MODEL;
	try
	{
		std::vector<std::string>splitted = {};
		Parser::FMTmodelparser ModelParser;

		SPLITTED_MODEL = p_model.splitActions(splitted, p_splitted_mask);
		SPLITTED_MODEL.setname(p_scenario_name);

		const std::vector<Models::FMTmodel> MODELS = { p_model };

		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(p_schedulePri, {p_model}).at(0);
		//ATTENTION il est important d'ecrire le model avant la schedule car modelParser.write va ecraser le fichier de schedule
		#ifdef _WIN32
		ModelParser.write(SPLITTED_MODEL, p_outputDirPath + "\\");
		#else
		ModelParser.write(SPLITTED_MODEL, OUTPUT_DIRECTORY + "/");
		#endif
		if (!SCHEDULES.empty())
		{
			Parser::FMTscheduleparser SCHEDULE_PARSER;
			const std::vector<Core::FMTschedule>NEWSCHEDULE = SPLITTED_MODEL.splitSchedules(SCHEDULES);
		#ifdef _WIN32
			SCHEDULE_PARSER.write(NEWSCHEDULE, p_outputDirPath + "\\" + p_scenario_name + ".seq");
		#else
			SCHEDULE_PARSER.write(NEWSCHEDULE, OUTPUT_DIRECTORY + "/" + SCENARIO + ".seq");
		#endif
		}

	}
	catch (...)
	{
		throw std::exception("FMTWrapperCore::Transformation::splitActions");
	}
	return SPLITTED_MODEL;
}

Models::FMTmodel FMTWrapperCore::Transformation::buildAction(const Models::FMTmodel& p_model, const std::string& p_actionName, const std::string& p_targetYield, const std::string& p_schedulePri, const std::string& p_outputDirPath, const std::string& p_scenario_name)
{
	Models::FMTmodel BUILDED_MODEL;
	try
	{
		Parser::FMTmodelparser ModelParser;
		const std::string ACTION_NAME = p_actionName;
		const std::string TARGET_YIELD = p_targetYield;
		const std::string SCHEDULE_PRIMARY_LOCATION = p_schedulePri;
		const std::string OUTPUT_DIRECTORY = p_outputDirPath;
		const std::string SCENARIO_NAME = p_scenario_name;
		const std::vector<Models::FMTmodel> MODELS = { p_model };

		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(SCHEDULE_PRIMARY_LOCATION, MODELS).at(0);
		BUILDED_MODEL = MODELS.at(0).buildAction(ACTION_NAME, TARGET_YIELD);
		BUILDED_MODEL.setname(SCENARIO_NAME);
		//ATTENTION il est important d'écrire le model avant la schedule car modelParser.write va écraser le fichier de schedule
#ifdef _WIN32
		ModelParser.write(BUILDED_MODEL, OUTPUT_DIRECTORY + "\\");
#else
		ModelParser.write(BUILDED_MODEL, OUTPUT_DIRECTORY + "/");
#endif
		if (!SCHEDULES.empty())
		{
			Parser::FMTscheduleparser SCHEDULE_PARSER;
			const std::vector<Core::FMTschedule> NEW_SCHEDULE = BUILDED_MODEL.buildSchedule(*BUILDED_MODEL.getactions().begin(), SCHEDULES);
#ifdef _WIN32
			SCHEDULE_PARSER.write(NEW_SCHEDULE, OUTPUT_DIRECTORY + "\\" + SCENARIO_NAME + ".seq");
#else
			SCHEDULE_PARSER.write(NEW_SCHEDULE, OUTPUT_DIRECTORY + "/" + SCENARIO_NAME + ".seq");
#endif
		}

	}
	catch (...) {
		throw std::exception("Error in FMTWrapperCore::Transformation::buildAction");
	}
	return BUILDED_MODEL;
}
