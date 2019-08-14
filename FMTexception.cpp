/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


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
