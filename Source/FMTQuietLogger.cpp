/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTQuietLogger.h"
#ifdef FMTWITHOSI
#include "FMTSolverLogger.h"
#endif
#include <boost/thread.hpp>

namespace Logging
{

std::unique_ptr <FMTLogger> FMTQuietLogger::Clone() const
	{
	return std::unique_ptr <FMTLogger>(new FMTQuietLogger(*this));
	}

void FMTQuietLogger::cout(const char* message) const
	{

	}

FMTQuietLogger::FMTQuietLogger():
	FMTLogger()
	{
	#ifdef FMTWITHOSI
		solverref->setLogLevel(0);
	#endif
	}
#ifdef FMTWITHOSI
int FMTQuietLogger::print()
	{
	//boost::lock_guard<boost::recursive_mutex> guard(mtx);
	//return FMTLogger::print();
	return 0;
	}

void FMTQuietLogger::checkSeverity()
	{
	//boost::lock_guard<boost::recursive_mutex> guard(mtx);
	FMTLogger::checkSeverity();
	}

FMTLogger * FMTQuietLogger::clone() const
	{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	return new FMTQuietLogger(*this);
	}
#endif
}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTQuietLogger)

