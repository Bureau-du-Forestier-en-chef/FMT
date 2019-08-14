
#include "FMTexception.h"

namespace Exception
{

    FMTexception::FMTexception():holdup(false), _msg(), exceptiontype(), section(){}

    FMTexception::FMTexception(const FMTexc lexception,const string message): holdup(false),_msg(), exceptiontype(lexception), section()
        {
        _msg = "FMTexc("+to_string(lexception) +")" + message;
        }
     FMTexception::FMTexception(const FMTexc lexception,FMTwssect lsection,const string message): holdup(false),_msg(), exceptiontype(lexception),section(lsection)
        {
        _msg = "FMTexc("+to_string(lexception) +")"+message+" FMTwssect("+to_string(lsection) +")"+FMTwssect_str(lsection);
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
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Error) << _msg << "\n";
        }

	FMTexc FMTexception::gettype() const
		{
		return exceptiontype;
		}

	FMTwssect FMTexception::getsection() const
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
