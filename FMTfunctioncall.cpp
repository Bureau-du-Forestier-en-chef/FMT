#include "FMTfunctioncall.h"
#include "FMTlogger.h"

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
		//result = exp(rhs);
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "log of  " << rhs << "\n";
		result =  pow(2.71828, rhs); //precision stuff...
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got  " << result << "\n";
	}
	else if (FMTfkey::ln == key && rhs > 0)
	{
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "log of  " << rhs << "\n";
		result = log(rhs);
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got  " << result << "\n";
	}
	//result = std::floor(result * 100000) / 100000; //precision stuff...
	result = std::round(result * 100000000) / 100000000;
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