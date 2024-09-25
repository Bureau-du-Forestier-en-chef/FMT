#include "TransformationCore.h"
#include "FMTmodel.h"
#include "FMTschedule.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"
#include "FMTerror.h"

Models::FMTmodel FMTWrapperCore::Transformation::aggregateAllActions(const Models::FMTmodel& p_model, const std::vector<std::string>& p_aggregates, const std::vector<std::string>& p_order, const std::string& p_schedulePri, const std::string& p_outputDirPath, const std::string& p_scenario_name)
{
	Models::FMTmodel aggregatedModel;
	try
	{
		Parser::FMTmodelparser ModelParser;

		aggregatedModel = p_model.aggregateAllActions(p_aggregates, p_order);
		aggregatedModel.setname(p_scenario_name);
		std::string outputPath;
		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(p_schedulePri, { p_model }).at(0);
		//ATTENTION il est important d'�crire le model avant la schedule car modelParser.write va �craser le fichier de schedule
		#ifdef _WIN32
		outputPath = p_outputDirPath + "\\";
		#else
		outputPath = p_outputDirPath + "/";
		#endif
		ModelParser.write(aggregatedModel, outputPath);
		if (!SCHEDULES.empty())
		{
			Parser::FMTscheduleparser SCHEDULE_PARSER;
			const std::vector<Core::FMTschedule>NEWSCHEDULE = aggregatedModel.aggregateSchedules(SCHEDULES);
			SCHEDULE_PARSER.write(NEWSCHEDULE, outputPath + p_scenario_name + ".seq");
		}

	}
	catch (...)
	{
		throw Exception::FMTerror(Exception::FMTexc::FMTfunctionfailed, "",
			"FMTWrapperCore::Transformation::aggregateAllActions",__FILE__,__LINE__);
	}
	return aggregatedModel;

}

Models::FMTmodel FMTWrapperCore::Transformation::splitActions(const Models::FMTmodel& p_model, const std::string& p_schedulePri, const std::vector<std::string>& p_splitted, const std::vector<std::string>& p_splitted_mask, const std::string& p_outputDirPath, const std::string& p_scenario_name)
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

		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(p_schedulePri, {p_model}).at(0);
		//ATTENTION il est important d'ecrire le model avant la schedule car modelParser.write va ecraser le fichier de schedule
		#ifdef _WIN32
		outputPath = p_outputDirPath + "\\";
		#else
		outputPath = p_outputDirPath + "/";
		#endif
		ModelParser.write(SPLITTED_MODEL, outputPath);
		if (!SCHEDULES.empty())
		{
			Parser::FMTscheduleparser SCHEDULE_PARSER;
			const std::vector<Core::FMTschedule>NEWSCHEDULE = SPLITTED_MODEL.splitSchedules(SCHEDULES);
			SCHEDULE_PARSER.write(NEWSCHEDULE, outputPath + p_scenario_name + ".seq");
		}

	}
	catch (...)
	{
		throw Exception::FMTerror(Exception::FMTexc::FMTfunctionfailed, "",
			"FMTWrapperCore::Transformation::splitActions", __FILE__, __LINE__);
	}
	return SPLITTED_MODEL;
}

Models::FMTmodel FMTWrapperCore::Transformation::buildAction(const Models::FMTmodel& p_model, const std::string& p_actionName, const std::string& p_targetYield, const std::string& p_schedulePri, const std::string& p_outputDirPath, const std::string& p_scenario_name)
{
	Models::FMTmodel BUILDED_MODEL;
	try
	{
		Parser::FMTmodelparser ModelParser;
		std::string outputPath;
		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(p_schedulePri, { p_model }).at(0);
		BUILDED_MODEL = p_model.buildAction(p_actionName, p_targetYield);
		BUILDED_MODEL.setname(p_scenario_name);
		//ATTENTION il est important d'�crire le model avant la schedule car modelParser.write va �craser le fichier de schedule
		#ifdef _WIN32
		outputPath = p_outputDirPath + "\\";
		#else
		outputPath = p_outputDirPath + "/";
		#endif
		ModelParser.write(BUILDED_MODEL, outputPath);
		if (!SCHEDULES.empty())
		{
			Parser::FMTscheduleparser SCHEDULE_PARSER;
			const std::vector<Core::FMTschedule> NEW_SCHEDULE = BUILDED_MODEL.buildSchedule(*BUILDED_MODEL.getactions().begin(), SCHEDULES);
			SCHEDULE_PARSER.write(NEW_SCHEDULE, outputPath + p_scenario_name + ".seq");
		}
	}
	catch (...) {
		throw Exception::FMTerror(Exception::FMTexc::FMTfunctionfailed, "",
			"FMTWrapperCore::Transformation::buildAction", __FILE__, __LINE__);
	}
	return BUILDED_MODEL;
}
