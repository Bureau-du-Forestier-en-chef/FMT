/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef TESTING_EXCEPTIONCHAIN_HEADER
#define TESTING_EXCEPTIONCHAIN_HEADER

// The types of the exceptions that a call of FMTlib raises, for the unit tests of Core. FMT raises an error, then
// each method it goes through wraps it in an FMTfunctionfailed (raiseFromCatch): the type of the error is that of
// an exception nested in the one that reaches the test.

#include "FMTException.h"
#include "TestTools.h"

#include <algorithm>
#include <cstddef>
#include <exception>
#include <functional>
#include <vector>

namespace Testing
{
	// Calls p_call and returns the type of each FMT exception it raises, outermost first: none when it raises
	// nothing.
	inline std::vector<Exception::FMTexc> getRaisedTypes(const std::function<void()>& p_call)
	{
		std::vector<Exception::FMTexc> types;
		try
		{
			p_call();
		}
		catch (const std::exception& error)
		{
			visitNested(error, [&types](const std::exception& p_nested, std::size_t)
				{
				const Exception::FMTException* FMT_EXCEPTION = dynamic_cast<const Exception::FMTException*>(&p_nested);
				if (FMT_EXCEPTION != nullptr)
				{
					types.push_back(FMT_EXCEPTION->getType());
				}
				});
		}
		return types;
	}

	// Returns true when p_types holds p_type.
	inline bool hasType(const std::vector<Exception::FMTexc>& p_types, Exception::FMTexc p_type)
	{
		return std::find(p_types.begin(), p_types.end(), p_type) != p_types.end();
	}
}

#endif
