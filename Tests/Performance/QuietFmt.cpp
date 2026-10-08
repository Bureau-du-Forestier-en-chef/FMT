/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "QuietFmt.h"

#include "FMTException.h"
#include "FMTModelParser.h"

#include <vector>

namespace Performance
{
	void quietFmt()
	{
		static const bool QUIET = []()
			{
				Parser::FMTModelParser parser;
				parser.setQuietLogger();
				parser.setErrorsToWarnings(std::vector<Exception::FMTexc>{ Exception::FMTexc::FMTmissingyield,
					Exception::FMTexc::FMToutput_missing_operator, Exception::FMTexc::FMToutput_too_much_operator,
					Exception::FMTexc::FMTinvalidyield_number, Exception::FMTexc::FMTundefinedoutput_attribute,
					Exception::FMTexc::FMToveridedyield, Exception::FMTexc::FMTsourcetotarget_transition,
					Exception::FMTexc::FMTsame_transitiontargets, Exception::FMTexc::FMTunclosedforloop,
					Exception::FMTexc::FMToutofrangeyield, Exception::FMTexc::FMTdeathwithlock });
				return true;
			}();
		static_cast<void>(QUIET);
	}
}
