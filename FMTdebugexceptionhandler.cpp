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

#include "FMTdebugexceptionhandler.h"
#include "FMTlogger.h"

namespace Exception
{

	std::string FMTdebugexceptionhandler::getsrcinfo(const int& line, const std::string& file) const
	{
		return "In File(" + file + ") At Line(" + std::to_string(line) + ")";
	}

	FMTdebugexceptionhandler::FMTdebugexceptionhandler()
	{

	}

	FMTlev FMTdebugexceptionhandler::raise(FMTexc lexception, FMTwssect lsection, std::string text,
		const int& line, const std::string& file)
	{
		FMTexception excp;
		if (lsection == FMTwssect::Empty)
		{
			excp = FMTexception(lexception, updatestatus(lexception, text));
		}
		else {
			excp = FMTexception(lexception, lsection, updatestatus(lexception, text));
		}
		*_logger << getsrcinfo(line, file) << "\n";
		if (_level == FMTlev::FMT_Warning)
		{
			FMTwarning(excp).warn(_logger);
		}
		else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range) {
			throw FMTerror(excp);
		}
		return _level;
	}
#ifdef FMTWITHGDAL

	FMTexceptionhandler* FMTdebugexceptionhandler::getCPLdata()
		{
		return this;
		}
	void FMTdebugexceptionhandler::handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg)
		{
		FMTexceptionhandler::handelCPLerror(eErrClass, nError, pszErrorMsg);
		}
#endif

}
