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
		ar & BOOST_SERIALIZATION_NVP(m_holdup);
		ar & BOOST_SERIALIZATION_NVP(_msg);
		ar & BOOST_SERIALIZATION_NVP(m_exceptiontype);
		ar & BOOST_SERIALIZATION_NVP(m_section);
	}

    FMTException::FMTException():m_holdup(false), _msg(), m_exceptiontype(), m_section(), m_method(), m_file(), m_line() {}

	FMTException::FMTException(const std::exception& baseexception) : m_holdup(false), _msg(), m_exceptiontype(FMTexc::FMTunhandlederror), m_section()
		, m_method(), m_file(), m_line()
	{
		_msg = "FMTexc(" + std::to_string(FMTexc::FMTunhandlederror) + ")" + baseexception.what();

	}

    FMTException::FMTException(const FMTexc lexception,const std::string message): m_holdup(false),_msg(), m_exceptiontype(lexception), m_section()
		, m_method(), m_file(), m_line()
        {
        _msg = "FMTexc("+ std::to_string(lexception) +")" + message;
        }
     FMTException::FMTException(const FMTexc lexception, Core::FMTsection lsection,const std::string message): m_holdup(false),_msg(),
		 m_exceptiontype(lexception),m_section(lsection),m_method(), m_file(), m_line()
        {
        _msg = "FMTexc("+ std::to_string(lexception) +")"+message+" FMTsection("+ std::to_string(lsection) +")"+ Core::FMTsection_str(lsection);
        }
    FMTException::FMTException(const FMTException& rhs):
		m_holdup(rhs.m_holdup),_msg(rhs._msg), m_exceptiontype(rhs.m_exceptiontype),m_section(rhs.m_section),
		m_method(rhs.m_method), m_file(rhs.m_file), m_line(rhs.m_line)
        {

        }

	FMTException::FMTException(const FMTexc lexception, Core::FMTsection lsection, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline):
		m_holdup(false), _msg(), m_exceptiontype(lexception), m_section(lsection),m_method(lmethod),m_file(lfile),m_line(lline)
		{
		_msg = "FMTexc(" + std::to_string(lexception) + ")" + message +
			" FMTsection(" + std::to_string(lsection) + ")" + Core::FMTsection_str(lsection) + "\n" + FMTException::getSrcInfo();
		}

	FMTException::FMTException(const FMTexc lexception, Core::FMTsection lsection, const std::string message,
		const std::string& lmethod) :
		m_holdup(false), _msg(), m_exceptiontype(lexception), m_section(lsection), m_method(lmethod), m_file(), m_line()
	{
		_msg = "FMTexc(" + std::to_string(lexception) + ")" + message +
			" FMTsection(" + std::to_string(lsection) + ")" + Core::FMTsection_str(lsection)+ " " + lmethod;
	}

	FMTException::FMTException(const FMTexc lexception, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline) : m_holdup(false), _msg(), m_exceptiontype(lexception), m_section()
		, m_method(lmethod), m_file(lfile), m_line(lline)
	{
		_msg = "FMTexc(" + std::to_string(lexception) + ")" + message+"\n"+ FMTException::getSrcInfo();
	}

	FMTException::FMTException(const FMTexc lexception, const std::string message,
		const std::string& lmethod) : m_holdup(false), _msg(), m_exceptiontype(lexception), m_section()
		, m_method(lmethod), m_file(), m_line()
	{
		_msg = "FMTexc(" + std::to_string(lexception) + ")"+ message +" "+ lmethod;
	}



    FMTException& FMTException::operator = (const FMTException& rhs)
        {
        if (this!=&rhs)
            {
            _msg = rhs._msg;
			m_exceptiontype = rhs.m_exceptiontype;
			m_section = rhs.m_section;
			m_holdup = rhs.m_holdup;
			m_method = rhs.m_method;
			m_file = rhs.m_file;
			m_line = rhs.m_line;
            }
        return *this;
        }
    const char* FMTException::what() const throw()
        {
        return _msg.c_str();
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

}
