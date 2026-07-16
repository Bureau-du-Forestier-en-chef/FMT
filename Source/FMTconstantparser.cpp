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

const boost::regex FMTConstantParser::rxconstant = boost::regex("^([\\s\\t]*)((([^\\)]*)(\\)))|([^\\s^\\t]*))([\\s\\t]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);


FMTConstantParser::FMTConstantParser():
	FMTParser()
    {
	setSection(Core::FMTsection::Constants);
    }

bool FMTConstantParser::_fillConstants(Core::FMTConstants& p_constants,
	const std::string& p_input, bool p_allowNonValid) const
	{
	try{
		if (!p_input.empty())
		{
			boost::smatch kmatch;
			if (!boost::regex_search(p_input, kmatch, rxconstant)||
				p_input.find("*")!=std::string::npos)
			{
				if (p_allowNonValid)
					{
					return false;
					}
				_exhandler->raise(Exception::FMTexc::FMTundefined_constant,
					" at line " + std::to_string(m_line),
					"FMTConstantParser::_fillConstants", __LINE__, __FILE__, m_section);
			}
			std::string key = std::string(kmatch[4]) + std::string(kmatch[6]);
			if (!std::string(kmatch[5]).empty())
			{
				key += ")";
			}
			key.erase(boost::remove_if(key, boost::is_any_of(FMT_STR_SEPARATOR)), key.end());
			const std::vector<std::string>splited = FMTParser::spliter(std::string(kmatch[8]), FMTParser::m_SEPARATOR);
			std::vector<double>values;
			for (size_t id = 0; id < splited.size(); ++id)
			{
				const int period = static_cast<int>(id);
				if (splited.at(id).find("#") != std::string::npos)
				{
					std::string strid = splited.at(id);
					strid.erase(0, 1);
					values.push_back(p_constants.get<double>(strid, period));
				}
				else if (isNum(splited.at(id)))
				{
					values.push_back(getNum<double>(splited.at(id)));
				}
			}
			if (!values.empty())
			{
				if (!p_constants.isConstant("#" + key))
				{
					p_constants.set(key, values);
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTconstants_replacement,
						"Constant redefinition ignored for " + key + " at line " + std::to_string(m_line), 
						"FMTConstantParser::_fillConstants", __LINE__, __FILE__, m_section);
				}

			}
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("In " + m_location + " at line " + std::to_string(m_line),
			"FMTConstantParser::_fillConstants", __LINE__, __FILE__, m_section);
		}
	return true;
	}

std::queue<FMTParser::FMTLineInfo> FMTConstantParser::getCleanLinewfor(std::ifstream& p_stream,
	const std::vector<Core::FMTTheme>& p_themes,
	const Core::FMTConstants& p_cons) const
{
	std::queue<FMTLineInfo> lines;
	try {
		std::queue<FMTLineInfo> TempQueue = getAllLines(p_stream);
		Core::FMTConstants constantsCopy(p_cons);
		while (!TempQueue.empty())
			{
			lines.push(TempQueue.front());
			const std::string LINE = getLine(TempQueue);
			_fillConstants(constantsCopy, LINE,true);
			}
		lines = processForLoopsNInclude(p_themes, constantsCopy, lines);
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTConstantParser::getCleanLinewfor",
			__LINE__, __FILE__, m_section);
		}
	return lines;
}



Core::FMTConstants FMTConstantParser::read(const std::string& location)
    {
	Core::FMTConstants constants;
	try {
		boost::filesystem::path pathtoconstants(location);
		if (boost::filesystem::is_regular_file(pathtoconstants))//Constants is not a needed component
		{
			std::ifstream CONstream(location);
			std::vector<Core::FMTTheme>themes;
			if (FMTParser::tryOpening(CONstream, location))
			{
				std::queue<FMTParser::FMTLineInfo>Lines = getCleanLinewfor(CONstream, 
										themes, constants);
				while (!Lines.empty())
					{
					const std::string LINE = getLine(Lines);
					_fillConstants(constants, LINE);
					}
			}
		}
		}catch (...)
			{
			_exhandler->raiseFromCatch("In " + m_location + " at line " + std::to_string(m_line),"FMTConstantParser::read", __LINE__, __FILE__, m_section);
			}
	//constants.passinobject(*this);
    return constants;
    }

void FMTConstantParser::write(const Core::FMTConstants& constants,const std::string& location) const
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
		_exhandler->raiseFromCatch("at "+location,"FMTConstantParser::write", __LINE__, __FILE__, m_section);
		}
    }
}
