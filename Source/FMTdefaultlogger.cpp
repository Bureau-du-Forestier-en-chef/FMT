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

	std::unique_ptr <FMTlogger> FMTdefaultlogger::Clone() const
	{
		return std::unique_ptr <FMTlogger>(new FMTdefaultlogger(*this));
	}


	FMTdefaultlogger::FMTdefaultlogger():
		FMTlogger()
		{
		#ifdef FMTWITHOSI
			solverref->setLogLevel(1);
		#endif
		}

#ifdef FMTWITHOSI
	int FMTdefaultlogger::print()
		{
		return FMTlogger::print();
		}

	void FMTdefaultlogger::checkSeverity()
		{
		FMTlogger::checkSeverity();
		}

	FMTlogger* FMTdefaultlogger::clone() const
		{
		return new FMTdefaultlogger(*this);
		}
#endif

	}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTdefaultlogger)