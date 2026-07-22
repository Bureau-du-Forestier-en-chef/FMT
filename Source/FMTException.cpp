/*
Copyright (c) 2019 Gouvernement du Québec

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
		ar & BOOST_SERIALIZATION_NVP(holdup);
		ar & BOOST_SERIALIZATION_NVP(_msg);
		ar & BOOST_SERIALIZATION_NVP(exceptiontype);
		ar & BOOST_SERIALIZATION_NVP(section);
	}

    FMTException::FMTException():holdup(false), _msg(), exceptiontype(), section(), method(), file(), line() {}

	FMTException::FMTException(const std::exception& baseexception) : holdup(false), _msg(), exceptiontype(FMTexc::FMTunhandlederror), section()
		, method(), file(), line()
	{
		_msg = "FMTexc(" + std::to_string(FMTexc::FMTunhandlederror) + ")" + baseexception.what();

	}

    FMTException::FMTException(const FMTexc lexception,const std::string message): holdup(false),_msg(), exceptiontype(lexception), section()
		, method(), file(), line()
        {
        _msg = "FMTexc("+ std::to_string(lexception) +")" + message;
        }
     FMTException::FMTException(const FMTexc lexception, Core::FMTsection lsection,const std::string message): holdup(false),_msg(),
		 exceptiontype(lexception),section(lsection),method(), file(), line()
        {
        _msg = "FMTexc("+ std::to_string(lexception) +")"+message+" FMTsection("+ std::to_string(lsection) +")"+ Core::FMTsection_str(lsection);
        }
    FMTException::FMTException(const FMTException& rhs):
		holdup(rhs.holdup),_msg(rhs._msg), exceptiontype(rhs.exceptiontype),section(rhs.section),
		method(rhs.method), file(rhs.file), line(rhs.line)
        {

        }

	FMTException::FMTException(const FMTexc lexception, Core::FMTsection lsection, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline):
		holdup(false), _msg(), exceptiontype(lexception), section(lsection),method(lmethod),file(lfile),line(lline)
		{
		_msg = "FMTexc(" + std::to_string(lexception) + ")" + message +
			" FMTsection(" + std::to_string(lsection) + ")" + Core::FMTsection_str(lsection) + "\n" + FMTException::getSrcInfo();
		}

	FMTException::FMTException(const FMTexc lexception, Core::FMTsection lsection, const std::string message,
		const std::string& lmethod) :
		holdup(false), _msg(), exceptiontype(lexception), section(lsection), method(lmethod), file(), line()
	{
		_msg = "FMTexc(" + std::to_string(lexception) + ")" + message +
			" FMTsection(" + std::to_string(lsection) + ")" + Core::FMTsection_str(lsection)+ " " + lmethod;
	}

	FMTException::FMTException(const FMTexc lexception, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline) : holdup(false), _msg(), exceptiontype(lexception), section()
		, method(lmethod), file(lfile), line(lline)
	{
		_msg = "FMTexc(" + std::to_string(lexception) + ")" + message+"\n"+ FMTException::getSrcInfo();
	}

	FMTException::FMTException(const FMTexc lexception, const std::string message,
		const std::string& lmethod) : holdup(false), _msg(), exceptiontype(lexception), section()
		, method(lmethod), file(), line()
	{
		_msg = "FMTexc(" + std::to_string(lexception) + ")"+ message +" "+ lmethod;
	}



    FMTException& FMTException::operator = (const FMTException& rhs)
        {
        if (this!=&rhs)
            {
            _msg = rhs._msg;
			exceptiontype = rhs.exceptiontype;
			section = rhs.section;
			holdup = rhs.holdup;
			method = rhs.method;
			file = rhs.file;
			line = rhs.line;
            }
        return *this;
        }
    const char* FMTException::what() const throw()
        {
        return _msg.c_str();
        }

	FMTexc FMTException::getType() const
		{
		return exceptiontype;
		}

	Core::FMTsection FMTException::getSection() const
		{
		return section;
		}

	bool FMTException::hold() const
		{
		return holdup;
		}

	void FMTException::setHold(bool side)
		{
		holdup = side;
		}

	std::string FMTException::getSrcInfo() const
		{
		return "In Method("+ method +") In File(" + file + ") At Line(" + std::to_string(line) + ")";
		}

}
