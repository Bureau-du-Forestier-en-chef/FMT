#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "FMTException.h"
#include "FMTLpModel.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "AreaVariability.h"
#include "ModelQuery.h"

// Test manuel de FMTWrapperCore::AreaVariability, sur les données publiques TWD_land
// (scénario LP), avec les masques de l'exemple testareavariabilities.
//
// Couvre le cas nominal (le fichier de sortie est écrit), les trois refus (table absente,
// table réduite à son en-tête, aucun output demandé) et l'exception levée quand deux
// masques de la table recouvrent un même développement.
namespace
{
	int failures = 0;

	void check(bool p_condition, const std::string& p_description)
	{
		std::cout << (p_condition ? "  ok      " : "  ECHEC   ") << p_description << std::endl;

		if (!p_condition)
		{
			++failures;
		}
	}

	std::vector<std::vector<std::string>> makeTable(
		const std::vector<std::vector<std::string>>& p_rows)
	{
		std::vector<std::vector<std::string>> table;
		table.push_back({ "THEME1", "THEME2", "THEME3", "PROPORTION" });
		table.insert(table.end(), p_rows.begin(), p_rows.end());
		return table;
	}

	void report(const std::exception& p_exception)
	{
		std::cerr << p_exception.what() << std::endl;

		try
		{
			std::rethrow_if_nested(p_exception);
		}
		catch (const std::exception& nested)
		{
			std::cerr << nested.what() << std::endl;
		}
	}
}

int main()
{
	const std::string MODEL_FOLDER = "../../../../Examples/Models/TWD_land/";
	const std::filesystem::path OUTPUT_FOLDER = "../../tests/testWrapperCoreAreaVariability";
	const std::filesystem::path OUTPUT_FILE = OUTPUT_FOLDER / "areavariability.csv";

	std::filesystem::create_directories(OUTPUT_FOLDER);
	std::filesystem::remove(OUTPUT_FILE);

	Parser::FMTModelParser modelParser;
	modelParser.setDefaultExceptionHandler();
	modelParser.setDefaultLogger();

	FMTWrapperCore::AreaVariabilityParameters base;

	try
	{
		const std::vector<Models::FMTModel> MODELS = modelParser.readproject(
			MODEL_FOLDER + "TWD_land.pri", std::vector<std::string>(1, "LP"));
		const Models::FMTModel& MODEL = MODELS.at(0);

		base.solver = static_cast<int>(Models::FMTSolverInterface::CLP);
		base.constraintNames = FMTWrapperCore::ModelQuery::getConstraintsAsText(MODEL);
		base.numberOfPeriods = 10;
		base.outputNames = { "OVOLREC" };
		base.outputLevel = static_cast<int>(Core::FMToutputlevel::totalonly);
		base.outputMinPeriod = 1;
		base.outputMaxPeriod = 10;
		base.outputPath = OUTPUT_FILE.string();
		base.gdalProvider = "CSV";
		base.proportionsTableProvided = true;
		base.proportionsTable = makeTable({
			{ "?", "PEUPLEMENT2", "?", "0.01" },
			{ "?", "PEUPLEMENT3", "?", "-0.10" } });

		std::cout << "Cas nominal" << std::endl;
		const FMTWrapperCore::AreaVariabilityResults NOMINAL =
			FMTWrapperCore::AreaVariability::run(base, MODEL);
		check(NOMINAL.success, "l'operation reussit");
		check(std::filesystem::exists(OUTPUT_FILE), "le fichier de sortie est ecrit");

		std::cout << "Refus" << std::endl;

		FMTWrapperCore::AreaVariabilityParameters noTable = base;
		noTable.proportionsTableProvided = false;
		noTable.proportionsTable.clear();
		const FMTWrapperCore::AreaVariabilityResults NO_TABLE =
			FMTWrapperCore::AreaVariability::run(noTable, MODEL);
		check(!NO_TABLE.success && !NO_TABLE.errorMessage.empty(), "table absente");

		FMTWrapperCore::AreaVariabilityParameters headerOnly = base;
		headerOnly.proportionsTable = makeTable({});
		const FMTWrapperCore::AreaVariabilityResults HEADER_ONLY =
			FMTWrapperCore::AreaVariability::run(headerOnly, MODEL);
		check(!HEADER_ONLY.success && !HEADER_ONLY.errorMessage.empty(), "table reduite a son en-tete");

		FMTWrapperCore::AreaVariabilityParameters noOutput = base;
		noOutput.outputNames.clear();
		const FMTWrapperCore::AreaVariabilityResults NO_OUTPUT =
			FMTWrapperCore::AreaVariability::run(noOutput, MODEL);
		check(!NO_OUTPUT.success && !NO_OUTPUT.errorMessage.empty(), "aucun output demande");

		// Deux masques « tout accepté » recouvrent forcément chaque développement.
		std::cout << "Masques qui se recoupent" << std::endl;

		FMTWrapperCore::AreaVariabilityParameters overlapping = base;
		overlapping.proportionsTable = makeTable({
			{ "?", "?", "?", "0.01" },
			{ "?", "?", "?", "0.05" } });

		bool raised = false;

		try
		{
			FMTWrapperCore::AreaVariability::run(overlapping, MODEL);
		}
		catch (...)
		{
			raised = true;
		}

		check(raised, "une exception est levee, au lieu de l'ancien exit(-1)");
	}
	catch (const std::exception& e)
	{
		report(e);
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown error" << std::endl;
		return 1;
	}

	if (failures > 0)
	{
		std::cerr << failures << " verification(s) en echec" << std::endl;
		return 1;
	}

	std::cout << "test AreaVariability passed!" << std::endl;

	return 0;
}
