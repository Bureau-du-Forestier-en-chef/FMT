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

		std::vector<std::string> aggregates = p_aggregates;
		const std::string primaryLocation = p_schedulePri;
		const std::string SCENARIO = p_scenario_name;
		const std::vector<std::string> SCENARIOS(1, SCENARIO);
		const std::vector<Models::FMTmodel> MODELS = { p_model };
		aggregatedModel = MODELS.at(0).aggregateAllActions(aggregates);
		aggregatedModel.setname(SCENARIO);

		//const std::vector<Models::FMTmodel> MODELS = { MODEL };
		const std::string OUTPUT_DIRECTORY = p_outputDirPath;
		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(primaryLocation, MODELS).at(0);
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
			SCHEDULE_PARSER.write(NEWSCHEDULE, OUTPUT_DIRECTORY + "\\" + SCENARIO + ".seq");
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
