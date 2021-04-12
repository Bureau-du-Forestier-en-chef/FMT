/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTquietlogger.h"

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
	std::lock_guard<std::recursive_mutex> guard(mtx);
	//return FMTlogger::print();
	return 0;
	}

void FMTquietlogger::checkSeverity()
	{
	std::lock_guard<std::recursive_mutex> guard(mtx);
	FMTlogger::checkSeverity();
	}

CoinMessageHandler * FMTquietlogger::clone() const
	{
	std::lock_guard<std::recursive_mutex> guard(mtx);
	return new FMTquietlogger(*this);
	}
#endif
}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTquietlogger)

