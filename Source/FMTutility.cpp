/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTutility.h"
#include <string>

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

	bool lessThenYield(const std::string& p_first, const std::string& p_second) noexcept
		{
		const size_t FIRST_LENGTH = p_first.length();
		const size_t SECOND_LENGTH = p_second.length();
		bool lessThan = false;
		if (FIRST_LENGTH != SECOND_LENGTH)
		{
			lessThan = FIRST_LENGTH < SECOND_LENGTH;
		}
		else {
			size_t i = FIRST_LENGTH;
			bool gotValue = false;
			while (!gotValue && i > 0)
			{
				--i;
				if (p_first[i] != p_second[i])
				{
					if (p_first[i] < p_second[i])
					{
						lessThan = true;
					}
					else {
						lessThan = false;
					}
					gotValue = true;
				}
			}
		}
		return lessThan;
		}

	bool equalYield(const std::string& p_first, const std::string& p_second) noexcept
		{
		const size_t FIRST_LENGTH = p_first.length();
		const size_t SECOND_LENGTH = p_second.length();
		bool equal = false;
		if (FIRST_LENGTH == SECOND_LENGTH)
		{
			size_t i = FIRST_LENGTH;
			while (!equal && i > 0)
			{
				--i;
				equal = (p_first[i] == p_second[i]);
			}
		}
		return equal;
		}
}

