/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtasklogger.h"
#include <boost/thread.hpp>
#include "FMTsolverlogger.h"

namespace Logging
{

	FMTtasklogger::FMTtasklogger()
	{
#ifdef FMTWITHOSI
		solverref->setLogLevel(0);
#endif
	}
#ifdef FMTWITHOSI
	int FMTtasklogger::print()
	{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		//return FMTlogger::print();
		return 0;
	}

	void FMTtasklogger::checkSeverity()
	{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		FMTlogger::checkSeverity();
	}

	FMTlogger* FMTtasklogger::clone() const
	{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		return new FMTtasklogger(*this);
	}
#endif
}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTtasklogger)

