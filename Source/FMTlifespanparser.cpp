/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlifespanparser.hpp"
#include "FMTlifespans.hpp"
#include "FMTtheme.hpp"
#include "FMTconstants.hpp"

namespace Parser{

FMTlifespanparser::FMTlifespanparser():FMTparser()
    {
	setsection(Core::FMTsection::Lifespan);
    }

FMTlifespanparser::FMTlifespanparser(const FMTlifespanparser& rhs):FMTparser(rhs)
    {
	setsection(Core::FMTsection::Lifespan);
    }
FMTlifespanparser& FMTlifespanparser::operator = (const FMTlifespanparser& rhs)
    {
    if (this!=&rhs)
        {
        FMTparser::operator=(rhs);
		setsection(Core::FMTsection::Lifespan);
        }
    return *this;
    }

Core::FMTlifespans FMTlifespanparser::read(const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& constants,const std::string& location)
    {
    Core::FMTlifespans lifespan;
	try {
		std::ifstream LIFstream(location);
		if (FMTparser::tryopening(LIFstream, location))
		{
			while (LIFstream.is_open())
			{
				const std::string line = FMTparser::getcleanlinewfor(LIFstream,themes, constants);
				if (!line.empty())
				{
					std::vector<std::string>splited = FMTparser::spliter(line, FMTparser::rxseparator);
					std::string page = splited[splited.size() - 1];
					const int age = getnum<int>(page, constants);
					splited.pop_back();
					std::string mask = boost::algorithm::join(splited, " ");
					if (!Core::FMTtheme::validate(themes, mask, " at line " + std::to_string(_line))) continue;
					lifespan.push_back(Core::FMTmask(mask, themes), age);
				}
			}
		}
		lifespan.shrink();
		//lifespan.passinobject(*this);
	}catch (...)
		{
		_exhandler->raisefromcatch("In " + _location + " at line " + std::to_string(_line),"FMTlifespanparser::read", __LINE__, __FILE__, _section);
		}
    return lifespan;
    }

void FMTlifespanparser::write(const Core::FMTlifespans& lifespan,const std::string& location) const
    {
	try {
		std::ofstream lifespanstream;
		lifespanstream.open(location);
		if (tryopening(lifespanstream, location))
		{
			lifespanstream << std::string(lifespan);
			lifespanstream.close();
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("at " + location,"FMTlifespanparser::write", __LINE__, __FILE__, _section);
		}
    }

}
