#include "FMTlogger.h"

namespace Logging
{
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


