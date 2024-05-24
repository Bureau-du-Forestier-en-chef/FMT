/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdebuglogger.h"
#ifdef FMTWITHOSI
	#include "FMTsolverlogger.h"
#endif

namespace Logging
{
	std::unique_ptr <FMTlogger> FMTdebuglogger::Clone() const
	{
		return std::unique_ptr <FMTlogger>(new FMTdebuglogger(*this));
	}

	FMTdebuglogger::FMTdebuglogger():
		FMTlogger()
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