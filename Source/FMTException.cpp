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
	}

	FMTException::FMTException() :m_holdup(false), m_msg(), m_exceptiontype(FMTexc::None),
		m_section(Core::FMTsection::Empty), m_method(), m_file(), m_line(), m_FrenchDescription(), m_EnglishDescription(),
		m_level(FMTlev::FMT_None){}



	FMTException::FMTException(FMTexc p_exception, FMTlev p_level, Core::FMTsection p_section, const std::string& p_message,
		const std::string& p_method, const std::string& p_file, const int& p_line,
		const std::string& p_FrenchDescription, const std::string& p_EnglishDescription):
		m_holdup(false), m_msg(),
		m_exceptiontype(p_exception), m_section(p_section),
		m_method(p_method), m_file(p_file), m_line(p_line),
		m_FrenchDescription(p_FrenchDescription),
		m_EnglishDescription(p_EnglishDescription),
		m_level(p_level)
	{
		_buildMessage(p_message);
		
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
		return "In Method("+ m_method +") In File(" + m_file + ") At Line(" + std::to_string(m_line) + ")";
		}

	void FMTException::_buildMessage(const std::string& p_message)
	{
		if (m_section == Core::FMTsection::Empty)
		{
			m_msg = "FMTexc(" + std::to_string(static_cast<int>(m_exceptiontype)) + ") "+ getDescription() + p_message + "\n" + FMTException::getSrcInfo();
		}
		else {
			m_msg = "FMTexc(" + std::to_string(static_cast<int>(m_exceptiontype)) + ") " + getDescription() + p_message +
				" FMTsection(" + std::to_string(static_cast<int>(m_section)) + ")" + Core::FMTsection_str(m_section) + "\n" + FMTException::getSrcInfo();
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

	void FMTException::setIgnoreMessage()
		{
		m_msg.insert(0, "Ignoring: ");
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
