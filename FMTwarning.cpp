#include "FMTwarning.h"


namespace Exception
{
FMTwarning::FMTwarning() : FMTexception() {}
FMTwarning::FMTwarning(const FMTexception& rhs) : FMTexception(rhs) {}
FMTwarning::FMTwarning(const FMTexc lexception, const string message) : FMTexception(lexception, message) {}
FMTwarning::FMTwarning(const FMTexc lexception, const FMTwssect lsection, const string message) : FMTexception(lexception, lsection, message) {}
void FMTwarning::warn() const
	{
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Warn) << _msg << "\n";
	}
}
