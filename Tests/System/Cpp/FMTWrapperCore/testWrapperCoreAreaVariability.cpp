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

// Test of FMTWrapper::Backend::AreaVariability, registered in basetests.csv, on the public
// TWD_land data (LP scenario), with the masks of the testareavariabilities example.
//
// Covers the nominal case (the output file is written), the three refusals (missing
// table, table reduced to its header, no output requested) and the exception raised
// when two masks of the table cover the same development.
namespace
{
	int failures = 0;

	void check(bool p_condition, const std::string& p_description)
	{
		std::cout << (p_condition ? "  ok      " : "  FAILED  ") << p_description << std::endl;

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

	FMTWrapper::Backend::AreaVariabilityParameters base;

	try
	{
		const std::vector<Models::FMTModel> MODELS = modelParser.readproject(
			MODEL_FOLDER + "TWD_land.pri", std::vector<std::string>(1, "LP"));
		const Models::FMTModel& MODEL = MODELS.at(0);

		base.solver = static_cast<int>(Models::FMTSolverInterface::CLP);
		base.constraintNames = FMTWrapper::Backend::ModelQuery::getConstraintsAsText(MODEL);
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

		std::cout << "Nominal case" << std::endl;
		const FMTWrapper::Backend::AreaVariabilityResults NOMINAL =
			FMTWrapper::Backend::AreaVariability::run(base, MODEL);
		check(NOMINAL.success, "the operation succeeds");
		check(std::filesystem::exists(OUTPUT_FILE), "the output file is written");

		std::cout << "Refusals" << std::endl;

		FMTWrapper::Backend::AreaVariabilityParameters noTable = base;
		noTable.proportionsTableProvided = false;
		noTable.proportionsTable.clear();
		const FMTWrapper::Backend::AreaVariabilityResults NO_TABLE =
			FMTWrapper::Backend::AreaVariability::run(noTable, MODEL);
		check(!NO_TABLE.success && !NO_TABLE.errorMessage.empty(), "missing table");

		FMTWrapper::Backend::AreaVariabilityParameters headerOnly = base;
		headerOnly.proportionsTable = makeTable({});
		const FMTWrapper::Backend::AreaVariabilityResults HEADER_ONLY =
			FMTWrapper::Backend::AreaVariability::run(headerOnly, MODEL);
		check(!HEADER_ONLY.success && !HEADER_ONLY.errorMessage.empty(), "table reduced to its header");

		FMTWrapper::Backend::AreaVariabilityParameters noOutput = base;
		noOutput.outputNames.clear();
		const FMTWrapper::Backend::AreaVariabilityResults NO_OUTPUT =
			FMTWrapper::Backend::AreaVariability::run(noOutput, MODEL);
		check(!NO_OUTPUT.success && !NO_OUTPUT.errorMessage.empty(), "no output requested");

		// Two "accept everything" masks necessarily cover every development.
		std::cout << "Overlapping masks" << std::endl;

		FMTWrapper::Backend::AreaVariabilityParameters overlapping = base;
		overlapping.proportionsTable = makeTable({
			{ "?", "?", "?", "0.01" },
			{ "?", "?", "?", "0.05" } });

		bool raised = false;

		try
		{
			FMTWrapper::Backend::AreaVariability::run(overlapping, MODEL);
		}
		catch (...)
		{
			raised = true;
		}

		check(raised, "an exception is raised, instead of the former exit(-1)");
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
		std::cerr << failures << " check(s) failed" << std::endl;
		return 1;
	}

	std::cout << "test AreaVariability passed!" << std::endl;

	return 0;
}
