/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdebuglogger.hpp"
#include "FMTsolverlogger.hpp"

namespace Logging
{
	FMTdebuglogger::FMTdebuglogger()
		{
		#ifdef FMTWITHOSI
			solverref->setLogLevel(4);
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
	FMTlogger* FMTdebuglogger::clone() const
		{
		return new FMTdebuglogger(*this);
		}
	#endif

}


BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTdebuglogger)