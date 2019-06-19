#include "FMTfunctioncall.h"

namespace Core{


FMTfunctioncall::FMTfunctioncall():key()
{
}

FMTfunctioncall::FMTfunctioncall(const string& base)
	{
	key = FMTfkey::notvalid;
	if (base == "EXP")
		{ 
		key = FMTfkey::expo;
	}else if (base=="LN")
		{
		key = FMTfkey::ln;
		}
	}
double FMTfunctioncall::call(const double& rhs) const
	{
	double result = 0;
	if (FMTfkey::expo == key)
	{
		result = exp(rhs);
	}
	else if (FMTfkey::ln == key && rhs > 0)
	{
		result = log(rhs);
	}
	return result;
	}
FMTfunctioncall::FMTfunctioncall(const FMTfunctioncall& rhs) : key(rhs.key)
	{
	
	}

bool FMTfunctioncall::valid() const
	{
	return (key != FMTfkey::notvalid);
	}
FMTfunctioncall& FMTfunctioncall::	operator = (const FMTfunctioncall& rhs)
	{
	if (this!=&rhs)
		{
		key = rhs.key;
		}
	return *this;
	}

}