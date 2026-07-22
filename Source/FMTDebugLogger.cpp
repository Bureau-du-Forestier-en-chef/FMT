/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTDebugLogger.h"
#ifdef FMTWITHOSI
	#include "FMTSolverLogger.h"
#endif

namespace Logging
{
	std::unique_ptr <FMTLogger> FMTDebugLogger::Clone() const
	{
		return std::unique_ptr <FMTLogger>(new FMTDebugLogger(*this));
	}

	FMTDebugLogger::FMTDebugLogger():
		FMTLogger()
		{
		#ifdef FMTWITHOSI
			solverref->setLogLevel(4);
		#endif
		}
	#ifdef FMTWITHOSI
	int FMTDebugLogger::print()
		{
		return FMTLogger::print();
		}
	void FMTDebugLogger::checkSeverity()
		{
		FMTLogger::checkSeverity();
		}
	FMTLogger* FMTDebugLogger::clone() const
		{
		return new FMTDebugLogger(*this);
		}
	#endif

}


BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTDebugLogger)