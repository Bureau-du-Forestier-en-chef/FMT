/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlifespanparser.h"
#include "FMTlifespans.h"
#include "FMTtheme.h"
#include "FMTconstants.h"

namespace Parser{

FMTlifespanparser::FMTlifespanparser():FMTparser()
    {
	setSection(Core::FMTsection::Lifespan);
    }

FMTlifespanparser::FMTlifespanparser(const FMTlifespanparser& rhs):FMTparser(rhs)
    {
	setSection(Core::FMTsection::Lifespan);
    }
FMTlifespanparser& FMTlifespanparser::operator = (const FMTlifespanparser& rhs)
    {
    if (this!=&rhs)
        {
        FMTparser::operator=(rhs);
		setSection(Core::FMTsection::Lifespan);
        }
    return *this;
    }

Core::FMTlifespans FMTlifespanparser::read(const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& constants,const std::string& location)
    {
    Core::FMTlifespans lifespan;
	try {
		std::ifstream LIFstream(location);
		if (FMTparser::tryOpening(LIFstream, location))
		{
			std::queue<std::pair<std::string, int>>Lines = FMTparser::GetCleanLinewfor(LIFstream, themes, constants);
			while (!Lines.empty())
			{
				const std::string line = GetLine(Lines);
				if (!line.empty())
				{
					std::vector<std::string>splited = FMTparser::spliter(line, FMTparser::m_SEPARATOR);
					std::string page = splited[splited.size() - 1];
					const int age = getNum<int>(page, constants);
					splited.pop_back();
					std::string mask = boost::algorithm::join(splited, " ");
					if (!Core::FMTtheme::validate(themes, mask, " at line " + std::to_string(m_line))) continue;
					lifespan.push_back(Core::FMTmask(mask, themes), age);
				}
			}
		}
		lifespan.shrink();
		//lifespan.passinobject(*this);
	}catch (...)
		{
		_exhandler->raisefromcatch("In " + m_location + " at line " + std::to_string(m_line),"FMTlifespanparser::read", __LINE__, __FILE__, m_section);
		}
    return lifespan;
    }

void FMTlifespanparser::write(const Core::FMTlifespans& lifespan,const std::string& location) const
    {
	try {
		std::ofstream lifespanstream;
		lifespanstream.open(location);
		if (tryOpening(lifespanstream, location))
		{
			lifespanstream << std::string(lifespan);
			lifespanstream.close();
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("at " + location,"FMTlifespanparser::write", __LINE__, __FILE__, m_section);
		}
    }

}
