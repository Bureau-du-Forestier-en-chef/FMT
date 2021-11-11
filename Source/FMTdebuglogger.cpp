/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdebuglogger.hpp"

namespace Logging
{
	FMTdebuglogger::FMTdebuglogger()
		{
		#ifdef FMTWITHOSI
			this->setLogLevel(4);
		#endif
		}
	#ifdef FMTWITHOSI
	int FMTdebuglogger::print()
		{
		return FMTlogger::print();
		}
	void FMTdebuglogger::checkSeverity()
		{
		FMTlogger::checkSeverity();
		}
	CoinMessageHandler* FMTdebuglogger::clone() const
		{
		return new FMTdebuglogger(*this);
		}
	#endif

}


BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTdebuglogger)