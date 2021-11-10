/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTquietlogger.hpp"

namespace Logging
{

FMTquietlogger::FMTquietlogger()
	{
	#ifdef FMTWITHOSI
		this->setLogLevel(0);
	#endif
	}
#ifdef FMTWITHOSI
int FMTquietlogger::print()
	{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	//return FMTlogger::print();
	return 0;
	}

void FMTquietlogger::checkSeverity()
	{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	FMTlogger::checkSeverity();
	}

CoinMessageHandler * FMTquietlogger::clone() const
	{
	boost::lock_guard<boost::recursive_mutex> guard(mtx);
	return new FMTquietlogger(*this);
	}
#endif
}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTquietlogger)

