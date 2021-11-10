/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTfunctioncall.hpp"
#include "FMTlogger.hpp"
#include <math.h>

namespace Core{


FMTfunctioncall::FMTfunctioncall():key()
{
}

FMTfunctioncall::FMTfunctioncall(const std::string& base)
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
		result = std::pow(2.71828, rhs); //precision stuff...
	}
	else if (FMTfkey::ln == key && rhs > 0)
	{
		result = std::log(rhs);
	}
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