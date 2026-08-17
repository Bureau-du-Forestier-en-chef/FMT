/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTException.h"

#if defined FMTWITHPYTHON
#include <boost/python.h>
#endif // define FMTWITHPYTHON
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#if defined (_MSC_VER)
#define NOMINMAX
#include <comdef.h>
#include <windows.h>
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#include <boost/filesystem.hpp>
#endif

#if defined (__CYGWIN__)
#define NOMINMAX
#include <windows.h>
#include <iterator>
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#include <boost/filesystem.hpp>
#endif

namespace Exception
{

	template<class Archive>
	void FMTException::serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("object", boost::serialization::base_object<std::exception>(*this));
		ar& BOOST_SERIALIZATION_NVP(m_holdup);
		ar& BOOST_SERIALIZATION_NVP(m_msg);
		ar& BOOST_SERIALIZATION_NVP(m_exceptiontype);
		ar& BOOST_SERIALIZATION_NVP(m_section);
		ar& BOOST_SERIALIZATION_NVP(m_method);
		ar& BOOST_SERIALIZATION_NVP(m_file);
		ar& BOOST_SERIALIZATION_NVP(m_line);
		ar& BOOST_SERIALIZATION_NVP(m_FrenchDescription);
		ar& BOOST_SERIALIZATION_NVP(m_EnglishDescription);
		ar& BOOST_SERIALIZATION_NVP(m_level);
		ar& BOOST_SERIALIZATION_NVP(m_PrintLevel);
		ar& BOOST_SERIALIZATION_NVP(m_UserMessage);
	}

	FMTException::FMTException() :m_holdup(false), m_msg(), m_exceptiontype(FMTexc::None),
		m_section(Core::FMTsection::Empty), m_method(), m_file(), m_line(), m_FrenchDescription(), m_EnglishDescription(),
		m_level(FMTlev::FMT_None), m_PrintLevel(0){}



	FMTException::FMTException(FMTexc p_exception, FMTlev p_level, Core::FMTsection p_section, const std::string& p_message,
		const std::string& p_method, const std::string& p_file, const int& p_line,
		const std::string& p_FrenchDescription, const std::string& p_EnglishDescription):
		m_holdup(false), m_msg(),
		m_exceptiontype(p_exception), m_section(p_section),
		m_method(p_method), m_file(p_file), m_line(p_line),
		m_FrenchDescription(p_FrenchDescription),
		m_EnglishDescription(p_EnglishDescription),
		m_level(p_level), 
		m_PrintLevel(0), 
		m_UserMessage(p_message)
	{
		_buildMessage();
	}


    const char* FMTException::what() const noexcept
        {
        return m_msg.c_str();
        }

	FMTexc FMTException::getType() const
		{
		return m_exceptiontype;
		}

	Core::FMTsection FMTException::getSection() const
		{
		return m_section;
		}

	bool FMTException::hold() const
		{
		return m_holdup;
		}

	void FMTException::setHold(bool side)
		{
		m_holdup = side;
		}

	std::string FMTException::getSrcInfo() const
		{
		return "\nIn Method("+ m_method +") \n In File(" + m_file + ") \n At Line(" + std::to_string(m_line) + ")";
		}

	int FMTException::_getPrintLevel() const
		{
		return m_PrintLevel;
		}

	const std::string& FMTException::_getUserMessage() const
	{
		return m_UserMessage;
	}

	void FMTException::_buildMessage()
	{
		if (m_section == Core::FMTsection::Empty)
		{
			m_msg = "FMTexc(" + std::to_string(static_cast<int>(m_exceptiontype)) + ") " + getDescription() +  _getUserMessage();
		}else {
			m_msg = "FMTexc(" + std::to_string(static_cast<int>(m_exceptiontype)) + ") " + getDescription() + _getUserMessage() +
				" FMTsection(" + std::to_string(static_cast<int>(m_section)) + ") " + Core::FMTsection_str(m_section);
		}
		if (_getPrintLevel() > 0)
			{
			m_msg +=  getSrcInfo();
			}
	}


	const std::string& FMTException::getMessage() const
		{
		return m_msg;
		}

	FMTlev FMTException::getLevel() const
		{
		return m_level;
		}


	void FMTException::setIgnore()
		{
		m_UserMessage.insert(0, "Ignoring: ");
		m_level = FMTlev::FMT_Warning;
		_buildMessage();
		}

	void FMTException::setPrintLevel(int p_level)
		{
		m_PrintLevel = p_level;
		_buildMessage();
		}

	bool FMTException::isFatal() const
		{
		return (getLevel() == FMTlev::FMT_logic || getLevel() == FMTlev::FMT_range);
		}

	const std::string& FMTException::getDescription(bool p_french) const
		{
		return p_french ? m_FrenchDescription : m_EnglishDescription;
		}

}
