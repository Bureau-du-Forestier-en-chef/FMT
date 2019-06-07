#include "FMTerror.h"



namespace Exception
{
	FMTerror::FMTerror() : FMTexception() {}
	FMTerror::FMTerror(const FMTexception& rhs) : FMTexception(rhs) {}
	FMTerror::FMTerror(const FMTexc lexception, const string message) : FMTexception(lexception, message) {}
	FMTerror::FMTerror(const FMTexc lexception, const FMTwssect lsection, const string message) : FMTexception(lexception, lsection, message) {}

}
