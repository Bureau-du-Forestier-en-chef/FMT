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

#include "FMTfreeexceptionhandler.h"

namespace Exception
{

#ifdef FMTWITHGDAL
	FMTexceptionhandler* FMTfreeexceptionhandler::getCPLdata()
		{
		return this;
		}
	void FMTfreeexceptionhandler::handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg)
		{

		}
#endif

FMTfreeexceptionhandler::FMTfreeexceptionhandler()
{

}


FMTlev FMTfreeexceptionhandler::raise(FMTexc lexception, Core::FMTwssect lsection, std::string text,
	const int& line, const std::string& file)
{
	FMTexception excp;
	if (lsection == Core::FMTwssect::Empty)
	{
		excp = FMTexception(lexception, updatestatus(lexception, text));
	}
	else {
		excp = FMTexception(lexception, lsection, updatestatus(lexception, text));
	}
	excp.sethold(true);
	if (_level == FMTlev::FMT_Warning)
	{
		
	}
	else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range) {
		//throw FMTerror(excp);
		std::throw_with_nested(FMTerror(excp));
	}
	return _level;
}

}