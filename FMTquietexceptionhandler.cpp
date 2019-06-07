#include "FMTquietexceptionhandler.h"

namespace Exception
{

	FMTquietexceptionhandler::FMTquietexceptionhandler() :FMTexceptionhandler() {}

	FMTlev FMTquietexceptionhandler::raise(FMTexc lexception, FMTwssect lsection, string text,
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
		if (_level == FMTlev::FMT_Warning)
		{
			//FMTwarning(excp).warn();
		}
		else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range) {
			throw FMTerror(excp);
		}
		return _level;
	}

}
