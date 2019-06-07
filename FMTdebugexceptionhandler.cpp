#include "FMTdebugexceptionhandler.h"

namespace Exception
{

	string FMTdebugexceptionhandler::getsrcinfo(const int& line, const string& file) const
	{
		return "In File(" + file + ") At Line(" + to_string(line) + ")";
	}

	FMTdebugexceptionhandler::FMTdebugexceptionhandler()
	{

	}

	FMTlev FMTdebugexceptionhandler::raise(FMTexc lexception, FMTwssect lsection, string text,
		const int& line, const string& file)
	{
		FMTexception excp;
		if (lsection == FMTwssect::Empty)
		{
			excp = FMTexception(lexception, updatestatus(lexception, text));
		}
		else {
			excp = FMTexception(lexception, lsection, updatestatus(lexception, text));
		}
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << getsrcinfo(line, file) << "\n";
		if (_level == FMTlev::FMT_Warning)
		{
			FMTwarning(excp).warn();
		}
		else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range) {
			throw FMTerror(excp);
		}
		return _level;
	}

}
