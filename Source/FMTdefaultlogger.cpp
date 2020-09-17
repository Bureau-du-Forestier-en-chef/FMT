/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTdefaultlogger.h"
#ifdef FMTWITHOSI
#include <CoinMessageHandler.hpp>
#endif
#if defined FMTWITHPYTHON
#include <boost/python.hpp>
#endif // defined FMTWITHPYTHON
#include <iostream>

namespace Logging
	{

	FMTdefaultlogger::FMTdefaultlogger()
		{
		#ifdef FMTWITHOSI
			this->setLogLevel(1);
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

	CoinMessageHandler * FMTdefaultlogger::clone() const
		{
		return new FMTdefaultlogger(*this);
		}
#endif

	}

BOOST_CLASS_EXPORT_IMPLEMENT(Logging::FMTdefaultlogger)