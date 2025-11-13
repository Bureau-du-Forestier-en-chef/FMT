/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTconstantparser.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <boost/algorithm/string.hpp>
#include "FMTexceptionhandler.h"


namespace Parser{

const boost::regex FMTconstantparser::rxconstant = boost::regex("^([\\s\\t]*)((([^\\)]*)(\\)))|([^\\s^\\t]*))([\\s\\t]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);


FMTconstantparser::FMTconstantparser():
	FMTparser()
    {
	setSection(Core::FMTsection::Constants);
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
			if (FMTparser::tryOpening(CONstream, location))
			{
				std::queue<FMTparser::FMTLineInfo>Lines = FMTparser::GetCleanLinewfor(CONstream, themes, constants);
				while (!Lines.empty())
				{
					const std::string line = GetLine(Lines);
					if (!line.empty())
					{
						boost::smatch kmatch;
						if (!boost::regex_search(line,kmatch,rxconstant))
						{
							_exhandler->raise(Exception::FMTexc::FMTundefined_constant,
								" at line " + std::to_string(m_line),
								"FMTconstantparser::read", __LINE__, __FILE__, m_section);
						}
						std::string key = std::string(kmatch[4]) + std::string(kmatch[6]);
						if (!std::string(kmatch[5]).empty())
							{
							key += ")";
							}
						key.erase(boost::remove_if(key, boost::is_any_of(FMT_STR_SEPARATOR)), key.end());
						const std::vector<std::string>splited = FMTparser::spliter(std::string(kmatch[8]), FMTparser::m_SEPARATOR);
						std::vector<double>values;
						for (size_t id = 0; id < splited.size(); ++id)
						{
							const int period = static_cast<int>(id);
							if (splited.at(id).find("#") != std::string::npos)
							{
								std::string strid = splited.at(id);
								strid.erase(0, 1);
								values.push_back(constants.get<double>(strid, period));
							}
							else if (isNum(splited.at(id)))
							{
								values.push_back(getNum<double>(splited.at(id)));
							}
						}
						if (!values.empty())
						{
							if (!constants.isconstant("#" + key))
							{
								constants.set(key, values);
							}else {
								_exhandler->raise(Exception::FMTexc::FMTconstants_replacement,
									"Constant redefinition ignored for "+key + " at line " + std::to_string(m_line), "FMTconstantparser::read", __LINE__, __FILE__, m_section);
							}
							
						}

					}
				}
			}
		}
		}catch (...)
			{
			_exhandler->raisefromcatch("In " + m_location + " at line " + std::to_string(m_line),"FMTconstantparser::read", __LINE__, __FILE__, m_section);
			}
	//constants.passinobject(*this);
    return constants;
    }

void FMTconstantparser::write(const Core::FMTconstants& constants,const std::string& location) const
    {
	try {
		std::ofstream constantstream;
		constantstream.open(location);
		if (tryOpening(constantstream, location))
		{
			constantstream << std::string(constants);
			constantstream.close();
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("at "+location,"FMTconstantparser::write", __LINE__, __FILE__, m_section);
		}
    }
}
