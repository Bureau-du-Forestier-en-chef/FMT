/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTutility.h"

namespace Core
{

	const char* FMTsection_str(FMTsection section)
	{
		switch (section)
		{
		case FMTsection::Control:   return "Control";
		case FMTsection::Landscape: return "Landscape";
		case FMTsection::Area: return "Area";
		case FMTsection::Action: return "Action";
		case FMTsection::Transition: return "Transition";
		case FMTsection::Yield: return "Yield";
		case FMTsection::Schedule: return "Schedule";
		case FMTsection::Outputs: return "Outputs";
		case FMTsection::Optimize: return "Optimize";
		case FMTsection::Constants: return "Constants";
		default:      return "[Unknown FMTsection]";
		}
	}
}

