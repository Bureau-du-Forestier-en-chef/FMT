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


