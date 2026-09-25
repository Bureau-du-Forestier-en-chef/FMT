#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "FMTModelParser.h"
#include "FMTModel.h"
#include "FMTDevelopment.h"
#include "FMTDevelopmentPath.h"
#include "FMTFuturDevelopment.h"
#include "FMTTransition.h"
#include "FMTConstraint.h"
#include "FMTException.h"
#include "TestTools.h"

// Checks what FMTModelParser::readproject reads from one scenario: one query per CSV row.
//
// argv[1]: <primary file>|<scenario>|<query>[|<query argument>...]
// argv[2]: expected value. A list is separated by '|'; items that are numbers are compared
//          with a relative tolerance of 1e-6, the other items as exact strings. An expected
//          value written FMTexc(<code>) means the query must raise an FMT exception whose
//          chain holds this code.
//
// Queries:
//   THEMES                          number of themes
//   THEMENAME|<theme>               name of a theme (numbered from 1)
//   ATTRIBUTES|<theme>              attributes of a theme, in order
//   AGGREGATE|<theme>|<aggregate>   attributes of an aggregate of a theme
//   DEVELOPMENTS|<mask>             number of initial developments within the mask
//   AREA|<mask>                     initial area within the mask
//   ACTIONS                         action names, in order
//   ACTION|<action>                 flags of the action: RESETAGE and LOCKEXEMPT, or NONE
//   OPERABLE|<action>|<mask>|<age>[|<lock>[|<period>]]
//                                   1 when the development is operable, 0 otherwise
//   OPERATE|<action>|<mask>|<age>[|<lock>[|<period>]]
//                                   <mask>|<age>|<lock>|<proportion> of each development path
//                                   the action and its transition lead to, in order
//   GROW|<mask>|<age>[|<lock>[|<period>]]
//                                   <mask>|<age>|<lock>|<period> of the grown development
//   YIELD|<mask>|<age>|<yield>[|<period>]
//                                   yield value of the development (lock 0)
//
//   A development is written <mask>|<age>[|<lock>[|<period>]]: the lock is 0 and the period
//   is 1 when they are left out.
//   OUTPUTS                         output names, in order
//   CONSTRAINTS                     number of lines of the optimize section, objective included
//   CONSTRAINT|<index>              <type>|<output>|<lower>|<upper>|<first period>|<last period>
//                                   of a line (index 0 is the objective); infinite bounds
//                                   are -INF and INF, the _LENGTH period is LENGTH
//
// Expected values are computed by hand from the model files: running the test with a
// wrong expected value must make it fail.
namespace
{
	std::vector<std::string> split(const std::string& p_value)
	{
		std::vector<std::string> items;
		boost::split(items, p_value, boost::is_any_of("|"));
		return items;
	}

	std::string toString(double p_value)
	{
		if (std::isinf(p_value) || p_value <= std::numeric_limits<double>::lowest())
		{
			return (p_value < 0) ? "-INF" : "INF";
		}
		std::ostringstream stream;
		stream.precision(12);
		stream << p_value;
		return stream.str();
	}

	bool toNumber(const std::string& p_value, double& p_number)
	{
		try
		{
			size_t used = 0;
			p_number = std::stod(p_value, &used);
			return used == p_value.size();
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	bool itemsMatch(const std::vector<std::string>& p_actual, const std::vector<std::string>& p_expected)
	{
		if (p_actual.size() != p_expected.size())
		{
			return false;
		}
		for (size_t item = 0; item < p_actual.size(); ++item)
		{
			double actual = 0;
			double expected = 0;
			if (toNumber(p_actual.at(item), actual) && toNumber(p_expected.at(item), expected))
			{
				if (std::abs(actual - expected) > 1e-6 * (std::max)(1.0, std::abs(expected)))
				{
					return false;
				}
			}
			else if (p_actual.at(item) != p_expected.at(item))
			{
				return false;
			}
		}
		return true;
	}

	const Core::FMTTheme& getTheme(const Models::FMTModel& p_model, const std::vector<Core::FMTTheme>& p_themes,
		const std::string& p_number)
	{
		const size_t NUMBER = static_cast<size_t>(std::stoi(p_number));
		if (NUMBER < 1 || NUMBER > p_themes.size())
		{
			throw std::invalid_argument("No theme " + p_number + " in " + p_model.getName());
		}
		return p_themes.at(NUMBER - 1);
	}

	// Core::FMTActionComparator is not exported by FMTlib: the actions are searched by name here.
	Core::FMTAction getAction(const Models::FMTModel& p_model, const std::string& p_name)
	{
		for (const Core::FMTAction& action : p_model.getActions())
		{
			if (action.getName() == p_name)
			{
				return action;
			}
		}
		throw std::invalid_argument("No action " + p_name + " in " + p_model.getName());
	}

	// Core::FMTTransitionComparator is not exported by FMTlib either: same search by name.
	Core::FMTTransition getTransition(const Models::FMTModel& p_model, const std::string& p_name)
	{
		for (const Core::FMTTransition& transition : p_model.getTransitions())
		{
			if (transition.getName() == p_name)
			{
				return transition;
			}
		}
		throw std::invalid_argument("No transition " + p_name + " in " + p_model.getName());
	}

	// Builds the development of a query written <mask>|<age>[|<lock>[|<period>]].
	Core::FMTDevelopment getDevelopment(const std::vector<Core::FMTTheme>& p_themes,
		const std::vector<std::string>& p_query, size_t p_first)
	{
		const int LOCK = (p_query.size() > p_first + 2) ? std::stoi(p_query.at(p_first + 2)) : 0;
		const int PERIOD = (p_query.size() > p_first + 3) ? std::stoi(p_query.at(p_first + 3)) : 1;
		return Core::FMTDevelopment(Core::FMTMask(p_query.at(p_first), p_themes),
			std::stoi(p_query.at(p_first + 1)), LOCK, PERIOD);
	}

	std::string getTypeName(Core::FMTconstrainttype p_type)
	{
		switch (p_type)
		{
		case Core::FMTconstrainttype::FMTMAXobjective:
			return "MAX";
		case Core::FMTconstrainttype::FMTMINobjective:
			return "MIN";
		case Core::FMTconstrainttype::FMTMAXMINobjective:
			return "MAXMIN";
		case Core::FMTconstrainttype::FMTMINMAXobjective:
			return "MINMAX";
		case Core::FMTconstrainttype::FMTevenflow:
			return "EVEN";
		case Core::FMTconstrainttype::FMTnondeclining:
			return "NDY";
		case Core::FMTconstrainttype::FMTsequence:
			return "SEQ";
		case Core::FMTconstrainttype::FMTstandard:
			return "STANDARD";
		default:
			return "OTHER";
		}
	}

	std::string getPeriod(int p_period)
	{
		return (p_period == (std::numeric_limits<int>::max)()) ? "LENGTH" : std::to_string(p_period);
	}

	// Returns the items answering a query on the model.
	std::vector<std::string> answer(const Models::FMTModel& p_model, const std::vector<std::string>& p_query)
	{
		const std::string& QUERY = p_query.at(0);
		const std::vector<Core::FMTTheme> THEMES = p_model.getThemes();
		std::vector<std::string> items;
		if (QUERY == "THEMES" && p_query.size() == 1)
		{
			items.push_back(std::to_string(THEMES.size()));
		}
		else if (QUERY == "THEMENAME" && p_query.size() == 2)
		{
			items.push_back(getTheme(p_model, THEMES, p_query.at(1)).getName());
		}
		else if (QUERY == "ATTRIBUTES" && p_query.size() == 2)
		{
			items = getTheme(p_model, THEMES, p_query.at(1)).getBaseAttributes();
		}
		else if (QUERY == "AGGREGATE" && p_query.size() == 3)
		{
			items = getTheme(p_model, THEMES, p_query.at(1)).getAttributes(p_query.at(2));
		}
		else if ((QUERY == "DEVELOPMENTS" || QUERY == "AREA") && p_query.size() == 2)
		{
			const Core::FMTMask MASK(p_query.at(1), THEMES);
			size_t developments = 0;
			double area = 0;
			for (const Core::FMTActualDevelopment& development : p_model.getArea())
			{
				if (development.getMask().isSubsetOf(MASK))
				{
					++developments;
					area += development.getArea();
				}
			}
			items.push_back((QUERY == "AREA") ? toString(area) : std::to_string(developments));
		}
		else if (QUERY == "ACTIONS" && p_query.size() == 1)
		{
			for (const Core::FMTAction& action : p_model.getActions())
			{
				items.push_back(action.getName());
			}
		}
		else if (QUERY == "ACTION" && p_query.size() == 2)
		{
			const Core::FMTAction ACTION = getAction(p_model, p_query.at(1));
			if (ACTION.isResetAge())
			{
				items.push_back("RESETAGE");
			}
			if (!ACTION.doRespectLock())
			{
				items.push_back("LOCKEXEMPT");
			}
			if (items.empty())
			{
				items.push_back("NONE");
			}
		}
		else if (QUERY == "OPERABLE" && p_query.size() >= 4 && p_query.size() <= 6)
		{
			const Core::FMTAction ACTION = getAction(p_model, p_query.at(1));
			const Core::FMTDevelopment DEVELOPMENT = getDevelopment(THEMES, p_query, 2);
			items.push_back(DEVELOPMENT.operable(ACTION, p_model.getYields()) ? "1" : "0");
		}
		else if (QUERY == "OPERATE" && p_query.size() >= 4 && p_query.size() <= 6)
		{
			const Core::FMTAction ACTION = getAction(p_model, p_query.at(1));
			const Core::FMTTransition TRANSITION = getTransition(p_model, p_query.at(1));
			const Core::FMTDevelopment DEVELOPMENT = getDevelopment(THEMES, p_query, 2);
			for (const Core::FMTDevelopmentPath& path :
				DEVELOPMENT.operate(ACTION, TRANSITION, p_model.getYields(), THEMES))
			{
				const Core::FMTDevelopment& RESULT = path.getDevelopment();
				items.push_back(std::string(RESULT.getMask()));
				items.push_back(std::to_string(RESULT.getAge()));
				items.push_back(std::to_string(RESULT.getLock()));
				items.push_back(toString(path.getProportion()));
			}
		}
		else if (QUERY == "GROW" && p_query.size() >= 3 && p_query.size() <= 5)
		{
			const Core::FMTFuturDevelopment GROWN = getDevelopment(THEMES, p_query, 1).grow();
			items = { std::string(GROWN.getMask()), std::to_string(GROWN.getAge()),
				std::to_string(GROWN.getLock()), std::to_string(GROWN.getPeriod()) };
		}
		else if (QUERY == "YIELD" && (p_query.size() == 4 || p_query.size() == 5))
		{
			const int PERIOD = (p_query.size() == 5) ? std::stoi(p_query.at(4)) : 1;
			const Core::FMTDevelopment DEVELOPMENT(Core::FMTMask(p_query.at(1), THEMES), std::stoi(p_query.at(2)), 0, PERIOD);
			const Core::FMTYields YIELDS = p_model.getYields();
			items.push_back(toString(YIELDS.get(DEVELOPMENT.getYieldRequest(), p_query.at(3))));
		}
		else if (QUERY == "OUTPUTS" && p_query.size() == 1)
		{
			for (const Core::FMTOutput& output : p_model.getOutputs())
			{
				items.push_back(output.getName());
			}
		}
		else if (QUERY == "CONSTRAINTS" && p_query.size() == 1)
		{
			items.push_back(std::to_string(p_model.getConstraints().size()));
		}
		else if (QUERY == "CONSTRAINT" && p_query.size() == 2)
		{
			const std::vector<Core::FMTConstraint> CONSTRAINTS = p_model.getConstraints();
			const size_t INDEX = static_cast<size_t>(std::stoi(p_query.at(1)));
			if (INDEX >= CONSTRAINTS.size())
			{
				throw std::invalid_argument("No constraint " + p_query.at(1) + " in " + p_model.getName());
			}
			const Core::FMTConstraint& CONSTRAINT = CONSTRAINTS.at(INDEX);
			double lower = 0;
			double upper = 0;
			CONSTRAINT.getBounds(lower, upper);
			items = { getTypeName(CONSTRAINT.getConstraintType()), CONSTRAINT.getName(), toString(lower), toString(upper),
				getPeriod(CONSTRAINT.getPeriodLowerBound()), getPeriod(CONSTRAINT.getPeriodUpperBound()) };
		}
		else
		{
			throw std::invalid_argument("Unknown query or wrong number of arguments: " + boost::join(p_query, "|"));
		}
		return items;
	}

	// Reads an expected value written FMTexc(<code>), the form used for a query that must raise.
	bool getExpectedException(const std::string& p_expected, int& p_code)
	{
		const std::string PREFIX = "FMTexc(";
		if (p_expected.size() <= PREFIX.size() || p_expected.compare(0, PREFIX.size(), PREFIX) != 0
			|| p_expected.at(p_expected.size() - 1) != ')')
		{
			return false;
		}
		p_code = std::stoi(p_expected.substr(PREFIX.size(), p_expected.size() - PREFIX.size() - 1));
		return true;
	}

	// Codes of the FMT exceptions of a chain, outermost first.
	std::vector<int> collectCodes(const std::exception& p_exception)
	{
		std::vector<int> codes;
		Testing::visitNested(p_exception, [&codes](const std::exception& p_nested, std::size_t)
			{
				const Exception::FMTException* FMT_EXCEPTION = dynamic_cast<const Exception::FMTException*>(&p_nested);
				if (FMT_EXCEPTION != nullptr)
				{
					codes.push_back(static_cast<int>(FMT_EXCEPTION->getType()));
				}
			});
		return codes;
	}

	// Checks that the query raises the expected FMT exception code.
	int checkRaised(const Models::FMTModel& p_model, const std::vector<std::string>& p_query,
		const std::string& p_scenario, int p_code)
	{
		std::vector<int> codes;
		try
		{
			answer(p_model, p_query);
		}
		catch (const std::exception& error)
		{
			Testing::printException(error);
			codes = collectCodes(error);
		}
		std::string chain;
		for (const int code : codes)
		{
			chain += " FMTexc(" + std::to_string(code) + ")";
		}
		Testing::Checker checker;
		checker.check(std::find(codes.begin(), codes.end(), p_code) != codes.end(),
			p_scenario + " " + boost::join(p_query, "|") + ": expected FMTexc(" + std::to_string(p_code)
			+ ") in the raised chain, got" + (chain.empty() ? " nothing" : chain));
		return checker.exitCode();
	}
}

int main(int argc, char* argv[])
{
	return Testing::runTest([&]()
		{
		if (argc < 3)
		{
			throw std::invalid_argument("Usage: testScenarioReading <primary file>|<scenario>|<query>... <expected value>");
		}
		std::vector<std::string> arguments = split(argv[1]);
		if (arguments.size() < 3)
		{
			throw std::invalid_argument("The first argument needs a primary file, a scenario and a query");
		}
		const std::string PRIMARY_FILE = arguments.at(0);
		const std::string SCENARIO = arguments.at(1);
		const std::vector<std::string> QUERY(arguments.begin() + 2, arguments.end());
		const std::vector<std::string> EXPECTED = split(argv[2]);

		Parser::FMTModelParser parser;
		parser.setDefaultExceptionHandler();
		const std::vector<Models::FMTModel> MODELS = parser.readproject(PRIMARY_FILE, std::vector<std::string>(1, SCENARIO));

		int expectedCode = 0;
		if (EXPECTED.size() == 1 && getExpectedException(EXPECTED.at(0), expectedCode))
		{
			return checkRaised(MODELS.at(0), QUERY, SCENARIO, expectedCode);
		}

		const std::vector<std::string> ACTUAL = answer(MODELS.at(0), QUERY);
		Testing::Checker checker;
		checker.check(itemsMatch(ACTUAL, EXPECTED), SCENARIO + " " + boost::join(QUERY, "|")
			+ ": " + boost::join(ACTUAL, "|") + " (expected " + argv[2] + ")");
		return checker.exitCode();
		});
}
