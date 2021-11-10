/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTexception.hpp"

namespace Exception
{

    FMTexception::FMTexception():holdup(false), _msg(), exceptiontype(), section(), method(), file(), line() {}

    FMTexception::FMTexception(const FMTexc lexception,const std::string message): holdup(false),_msg(), exceptiontype(lexception), section()
		, method(), file(), line()
        {
        _msg = "FMTexc("+ std::to_string(lexception) +")" + message;
        }
     FMTexception::FMTexception(const FMTexc lexception, Core::FMTsection lsection,const std::string message): holdup(false),_msg(),
		 exceptiontype(lexception),section(lsection),method(), file(), line()
        {
        _msg = "FMTexc("+ std::to_string(lexception) +")"+message+" FMTsection("+ std::to_string(lsection) +")"+ Core::FMTsection_str(lsection);
        }
    FMTexception::FMTexception(const FMTexception& rhs):
		holdup(rhs.holdup),_msg(rhs._msg), exceptiontype(rhs.exceptiontype),section(rhs.section),
		method(rhs.method), file(rhs.file), line(rhs.line)
        {

        }

	FMTexception::FMTexception(const FMTexc lexception, Core::FMTsection lsection, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline):
		holdup(false), _msg(), exceptiontype(lexception), section(lsection),method(lmethod),file(lfile),line(lline)
		{
		_msg = "FMTexc(" + std::to_string(lexception) + ")" + message +
			" FMTsection(" + std::to_string(lsection) + ")" + Core::FMTsection_str(lsection) + "\n" + FMTexception::getsrcinfo();
		}

	FMTexception::FMTexception(const FMTexc lexception, Core::FMTsection lsection, const std::string message,
		const std::string& lmethod) :
		holdup(false), _msg(), exceptiontype(lexception), section(lsection), method(lmethod), file(), line()
	{
		_msg = "FMTexc(" + std::to_string(lexception) + ")" + message +
			" FMTsection(" + std::to_string(lsection) + ")" + Core::FMTsection_str(lsection)+ " " + lmethod;
	}

	FMTexception::FMTexception(const FMTexc lexception, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline) : holdup(false), _msg(), exceptiontype(lexception), section()
		, method(lmethod), file(lfile), line(lline)
	{
		_msg = "FMTexc(" + std::to_string(lexception) + ")" + message+"\n"+ FMTexception::getsrcinfo();
	}

	FMTexception::FMTexception(const FMTexc lexception, const std::string message,
		const std::string& lmethod) : holdup(false), _msg(), exceptiontype(lexception), section()
		, method(lmethod), file(), line()
	{
		_msg = "FMTexc(" + std::to_string(lexception) + ")"+ message +" "+ lmethod;
	}



    FMTexception& FMTexception::operator = (const FMTexception& rhs)
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
    const char* FMTexception::what() const throw()
        {
        return _msg.c_str();
        }

	FMTexc FMTexception::gettype() const
		{
		return exceptiontype;
		}

	Core::FMTsection FMTexception::getsection() const
		{
		return section;
		}

	bool FMTexception::hold() const
		{
		return holdup;
		}

	void FMTexception::sethold(bool side)
		{
		holdup = side;
		}

	std::string FMTexception::getsrcinfo() const
		{
		return "In Method("+ method +") In File(" + file + ") At Line(" + std::to_string(line) + ")";
		}

}
