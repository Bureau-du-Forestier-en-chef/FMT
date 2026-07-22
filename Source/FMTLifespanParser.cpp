/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTLifespanParser.h"
#include "FMTLifespans.h"
#include "FMTTheme.h"
#include "FMTConstants.h"

namespace Parser{

FMTLifespanParser::FMTLifespanParser():FMTParser()
    {
	setSection(Core::FMTsection::Lifespan);
    }

FMTLifespanParser::FMTLifespanParser(const FMTLifespanParser& rhs):FMTParser(rhs)
    {
	setSection(Core::FMTsection::Lifespan);
    }
FMTLifespanParser& FMTLifespanParser::operator = (const FMTLifespanParser& rhs)
    {
    if (this!=&rhs)
        {
        FMTParser::operator=(rhs);
		setSection(Core::FMTsection::Lifespan);
        }
    return *this;
    }

Core::FMTLifespans FMTLifespanParser::read(const std::vector<Core::FMTTheme>& themes,const Core::FMTConstants& constants,const std::string& location)
    {
    Core::FMTLifespans lifespan;
	try {
		std::ifstream LIFstream(location);
		if (FMTParser::tryOpening(LIFstream, location))
		{
			std::queue<FMTParser::FMTLineInfo>Lines = FMTParser::getCleanLinewfor(LIFstream, themes, constants);
			while (!Lines.empty())
			{
				const std::string line = getLine(Lines);
				if (!line.empty())
				{
					std::vector<std::string>splited = FMTParser::spliter(line, FMTParser::m_SEPARATOR);
					std::string page = splited[splited.size() - 1];
					const int age = getNum<int>(page, constants);
					splited.pop_back();
					std::string mask = boost::algorithm::join(splited, " ");
					if (!Core::FMTTheme::validate(themes, mask, " at line " + std::to_string(m_line))) continue;
					lifespan.push_back(Core::FMTMask(mask, themes), age);
				}
			}
		}
		lifespan.shrink();
		//lifespan.passinobject(*this);
	}catch (...)
		{
		_exhandler->raiseFromCatch("In " + m_location + " at line " + std::to_string(m_line),"FMTLifespanParser::read", __LINE__, __FILE__, m_section);
		}
    return lifespan;
    }

void FMTLifespanParser::write(const Core::FMTLifespans& lifespan,const std::string& location) const
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
		_exhandler->raiseFromCatch("at " + location,"FMTLifespanParser::write", __LINE__, __FILE__, m_section);
		}
    }

}
