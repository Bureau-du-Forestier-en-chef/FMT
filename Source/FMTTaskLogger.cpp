/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTaskLogger.h"
#include <boost/thread.hpp>
#include "FMTSolverLogger.h"

namespace Logging
{
	std::unique_ptr <FMTLogger> FMTTaskLogger::Clone() const
	{
		return std::unique_ptr <FMTLogger>(new FMTTaskLogger(*this));
	}


	FMTTaskLogger::FMTTaskLogger()
	{
#ifdef FMTWITHOSI
		solverref->setLogLevel(0);
#endif
	}
#ifdef FMTWITHOSI
	int FMTTaskLogger::print()
	{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
		//return FMTLogger::print();
		return 0;
	}

	void FMTTaskLogger::checkSeverity()
	{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
		//FMTLogger::checkSeverity();
	}

	FMTLogger* FMTTaskLogger::clone() const
	{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		return new FMTTaskLogger(*this);
	}
#endif
}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTTaskLogger)

