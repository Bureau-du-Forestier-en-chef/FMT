#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "FMTException.h"
#include "FMTModelParser.h"
#include "FMTModel.h"
#include "TestTools.h"

// Checks that reading an invalid scenario raises the expected FMT exception: a model error
// that is no longer reported, or reported with another code, makes the test fail.
//
// argv[1]: <primary file>|<scenario>
// argv[2]: number of the expected Exception::FMTexc, as FMT prints it in FMTexc(<number>).
//          It is searched in the whole chain of nested exceptions raised by
//          FMTModelParser::readproject: the outer ones only say where the reading failed.
namespace
{
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
}

int main(int argc, char* argv[])
{
	return Testing::runTest([&]()
		{
		if (argc < 3)
		{
			throw std::invalid_argument("Usage: testReadingErrors <primary file>|<scenario> <expected FMTexc number>");
		}
		const std::string FIRST_ARGUMENT(argv[1]);
		std::vector<std::string> arguments;
		boost::split(arguments, FIRST_ARGUMENT, boost::is_any_of("|"));
		if (arguments.size() != 2)
		{
			throw std::invalid_argument("The first argument needs a primary file and a scenario");
		}
		const std::string SCENARIO = arguments.at(1);
		const int EXPECTED = std::stoi(argv[2]);

		Parser::FMTModelParser parser;
		parser.setDefaultExceptionHandler();
		bool raised = false;
		std::vector<int> codes;
		try
		{
			parser.readproject(arguments.at(0), std::vector<std::string>(1, SCENARIO));
		}
		catch (const std::exception& error)
		{
			raised = true;
			std::cout << "Exception raised while reading " << SCENARIO << ":" << std::endl;
			Testing::printException(error);
			codes = collectCodes(error);
		}

		std::vector<std::string> codeNames;
		for (const int CODE : codes)
		{
			codeNames.push_back("FMTexc(" + std::to_string(CODE) + ")");
		}
		Testing::Checker checker;
		checker.check(raised, "reading " + SCENARIO + " raises an exception");
		checker.check(std::find(codes.begin(), codes.end(), EXPECTED) != codes.end(),
			"FMTexc(" + std::to_string(EXPECTED) + ") in the raised chain: " + boost::join(codeNames, " "));
		return checker.exitCode();
		});
}
