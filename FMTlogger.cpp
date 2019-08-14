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

#include "FMTlogger.h"

namespace Logging
{
	FMTlogger::FMTlogger()
		{

		}
	FMTlogger::FMTlogger(const FMTlogger& rhs) : CoinMessageHandler(rhs), _msglevel(rhs._msglevel)
		{

		}
	FMTlogger& FMTlogger::operator = (const FMTlogger& rhs)
		{
		if (this!=&rhs)
			{
			CoinMessageHandler::operator=(rhs);
			_msglevel = rhs._msglevel;
			}
		return *this;
		}

	void FMTlogger::checkSeverity()
		{

		}

	CoinMessageHandler* FMTlogger::clone() const
		{
		return new FMTlogger(*this);
		}

	int FMTlogger::print()
		{
		char buffer[COIN_MESSAGE_HANDLER_MAX_BUFFER_SIZE];
		snprintf(buffer, sizeof(buffer), "%s\n", this->messageBuffer_);
		#if defined(FMTPY)
			PySys_WriteStdout(buffer);
		#else
			cout << buffer << flush;
		#endif
		return 0;
		}

	FMTlogger::FMTlogger(FMTlogtype type)
		{
		_msglevel = type;
		}
	string FMTlogger::getlevel(FMTlogtype ltype)
		{
		string str_type = "(";
		switch (ltype)
			{
			case FMT_Debug:
				str_type += "Debug";
			break;
			case FMT_Warn:
				str_type += "Warning";
			break;
			case FMT_Error:
				str_type += "Error";
			break;
			case FMT_Info:
				str_type += "Info";
			break;
			default:
				str_type += "Debug";
			break;
			}
		str_type += ")";
		return str_type;
		}
}


