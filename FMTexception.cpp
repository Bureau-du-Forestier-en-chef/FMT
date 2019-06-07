
#include "FMTexception.h"

namespace Exception
{

    FMTexception::FMTexception():_msg(), holdup(false){}

    FMTexception::FMTexception(const FMTexc lexception,const string message):_msg(), exceptiontype(lexception), section(), holdup(false)
        {
        _msg = "FMTexc("+to_string(lexception) +")" + message;
        }
     FMTexception::FMTexception(const FMTexc lexception,FMTwssect lsection,const string message):_msg(), exceptiontype(lexception),section(lsection), holdup(false)
        {
        _msg = "FMTexc("+to_string(lexception) +")"+message+" FMTwssect("+to_string(lsection) +")"+FMTwssect_str(lsection);
        }
    FMTexception::FMTexception(const FMTexception& rhs):_msg(rhs._msg), exceptiontype(rhs.exceptiontype),section(rhs.section), holdup(rhs.holdup)
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
