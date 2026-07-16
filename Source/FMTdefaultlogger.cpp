/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTdefaultlogger.h"
#if defined FMTWITHPYTHON
#include <boost/python.h>
#endif // defined FMTWITHPYTHON
#include <iostream>
#ifdef FMTWITHOSI
#include "FMTsolverlogger.h"
#endif

namespace Logging
	{

	std::unique_ptr <FMTLogger> FMTDefaultLogger::Clone() const
	{
		return std::unique_ptr <FMTLogger>(new FMTDefaultLogger(*this));
	}


	FMTDefaultLogger::FMTDefaultLogger():
		FMTLogger()
		{
		#ifdef FMTWITHOSI
			solverref->setLogLevel(1);
		#endif
		}

#ifdef FMTWITHOSI
	int FMTDefaultLogger::print()
		{
		return FMTLogger::print();
		}

	void FMTDefaultLogger::checkSeverity()
		{
		FMTLogger::checkSeverity();
		}

	FMTLogger* FMTDefaultLogger::clone() const
		{
		return new FMTDefaultLogger(*this);
		}
#endif

	}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTDefaultLogger)