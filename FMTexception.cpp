/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTexception.h"

namespace Exception
{

    FMTexception::FMTexception():holdup(false), _msg(), exceptiontype(), section(){}

    FMTexception::FMTexception(const FMTexc lexception,const std::string message): holdup(false),_msg(), exceptiontype(lexception), section()
        {
        _msg = "FMTexc("+ std::to_string(lexception) +")" + message;
        }
     FMTexception::FMTexception(const FMTexc lexception, Core::FMTsection lsection,const std::string message): holdup(false),_msg(), exceptiontype(lexception),section(lsection)
        {
        _msg = "FMTexc("+ std::to_string(lexception) +")"+message+" FMTsection("+ std::to_string(lsection) +")"+ Core::FMTsection_str(lsection);
        }
    FMTexception::FMTexception(const FMTexception& rhs): holdup(rhs.holdup),_msg(rhs._msg), exceptiontype(rhs.exceptiontype),section(rhs.section)
        {

        }
    FMTexception& FMTexception::operator = (const FMTexception& rhs)
        {
        if (this!=&rhs)
            {
            _msg = rhs._msg;
			exceptiontype = rhs.exceptiontype;
			section = rhs.section;
			holdup = rhs.holdup;
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

}
