/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTFunctionCall.h"
#include "FMTLogger.h"
#include <math.h>

namespace Core{


FMTFunctionCall::FMTFunctionCall():key()
{
}

FMTFunctionCall::FMTFunctionCall(const std::string& base)
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
double FMTFunctionCall::call(const double& rhs) const
	{
	double result = 0;
	if (FMTfkey::expo == key)
	{
		result = std::pow(2.71828182845904523536/*2.71828*/, rhs); //precision stuff...
	}
	else if (FMTfkey::ln == key && rhs > 0)
	{
		result = std::log(rhs);
	}
	result = std::round(result * 100000000) / 100000000;
	return result;
	}
FMTFunctionCall::FMTFunctionCall(const FMTFunctionCall& rhs) : key(rhs.key)
	{
	
	}

bool FMTFunctionCall::valid() const
	{
	return (key != FMTfkey::notvalid);
	}
FMTFunctionCall& FMTFunctionCall::	operator = (const FMTFunctionCall& rhs)
	{
	if (this!=&rhs)
		{
		key = rhs.key;
		}
	return *this;
	}

}