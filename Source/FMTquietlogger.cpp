/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTquietlogger.h"
#ifdef FMTWITHOSI
#include "FMTsolverlogger.h"
#endif
#include <boost/thread.hpp>

namespace Logging
{

std::unique_ptr <FMTlogger> FMTquietlogger::Clone() const
	{
	return std::unique_ptr <FMTlogger>(new FMTquietlogger(*this));
	}

void FMTquietlogger::cout(const char* message) const
	{

	}

FMTquietlogger::FMTquietlogger():
	FMTlogger()
	{
	#ifdef FMTWITHOSI
		solverref->setLogLevel(0);
	#endif
	}
#ifdef FMTWITHOSI
int FMTquietlogger::print()
	{
	//boost::lock_guard<boost::recursive_mutex> guard(mtx);
	//return FMTlogger::print();
	return 0;
	}

void FMTquietlogger::checkSeverity()
	{
	//boost::lock_guard<boost::recursive_mutex> guard(mtx);
	FMTlogger::checkSeverity();
	}

FMTlogger * FMTquietlogger::clone() const
	{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	return new FMTquietlogger(*this);
	}
#endif
}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTquietlogger)

