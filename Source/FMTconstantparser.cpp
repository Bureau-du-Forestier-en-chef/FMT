/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTconstantparser.hpp"
#include "FMTconstants.hpp"
#include "FMTtheme.hpp"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

namespace Parser{

FMTconstantparser::FMTconstantparser(): FMTparser()
    {
	setsection(Core::FMTsection::Constants);
    }

FMTconstantparser::FMTconstantparser(const FMTconstantparser& rhs):FMTparser(rhs)
{
	setsection(Core::FMTsection::Constants);
}


FMTconstantparser& FMTconstantparser::operator = (const FMTconstantparser& rhs)
    {
        if (this!=&rhs)
        {
			setsection(Core::FMTsection::Constants);
            FMTparser::operator=(rhs);
        }
    return *this;
    }

Core::FMTconstants FMTconstantparser::read(const std::string& location)
    {
	Core::FMTconstants constants;
	try {
		boost::filesystem::path pathtoconstants(location);
		if (boost::filesystem::is_regular_file(pathtoconstants))//Constants is not a needed component
		{
			std::ifstream CONstream(location);
			std::vector<Core::FMTtheme>themes;
			if (FMTparser::tryopening(CONstream, location))
			{
				while (CONstream.is_open())
				{
					const std::string line = FMTparser::getcleanlinewfor(CONstream, themes, constants);
					if (!line.empty())
					{
						const std::vector<std::string>splited = FMTparser::spliter(line, FMTparser::rxseparator);
						const std::string key = splited[0];
						std::vector<double>values;
						for (size_t id = 1; id < splited.size(); ++id)
						{
							const int period = static_cast<int>((id - 1));
							if (splited[id].find("#") != std::string::npos)
							{
								std::string strid = splited[id];
								strid.erase(0, 1);
								values.push_back(constants.get<double>(strid, period));
							}
							else if (isnum(splited[id]))
							{
								values.push_back(getnum<double>(splited[id]));
							}
						}
						if (!values.empty())
						{
							constants.set(key, values);
						}

					}
				}
			}
		}
		}catch (...)
			{
			_exhandler->raisefromcatch("In " + _location + " at line " + std::to_string(_line),"FMTconstantparser::read", __LINE__, __FILE__, _section);
			}
	constants.passinobject(*this);
    return constants;
    }

void FMTconstantparser::write(const Core::FMTconstants& constants,const std::string& location) const
    {
	try {
		std::ofstream constantstream;
		constantstream.open(location);
		if (tryopening(constantstream, location))
		{
			constantstream << std::string(constants);
			constantstream.close();
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("at "+location,"FMTconstantparser::write", __LINE__, __FILE__, _section);
		}
    }
}
