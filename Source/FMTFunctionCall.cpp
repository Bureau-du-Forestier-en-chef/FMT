/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTFunctionCall.h"
#include "FMTLogger.h"
#include <math.h>

namespace Core{


FMTFunctionCall::FMTFunctionCall():m_key()
{
}

FMTFunctionCall::FMTFunctionCall(const std::string& base)
	{
	m_key = FMTfkey::notvalid;
	if (base == "EXP")
		{ 
		m_key = FMTfkey::expo;
	}else if (base=="LN")
		{
		m_key = FMTfkey::ln;
		}
	}
double FMTFunctionCall::call(const double& rhs) const
	{
	double result = 0;
	if (FMTfkey::expo == m_key)
	{
		result = std::pow(2.71828182845904523536/*2.71828*/, rhs); //precision stuff...
	}
	else if (FMTfkey::ln == m_key && rhs > 0)
	{
		result = std::log(rhs);
	}
	result = std::round(result * 100000000) / 100000000;
	return result;
	}
FMTFunctionCall::FMTFunctionCall(const FMTFunctionCall& rhs) : m_key(rhs.m_key)
	{
	
	}

bool FMTFunctionCall::valid() const
	{
	return (m_key != FMTfkey::notvalid);
	}
FMTFunctionCall& FMTFunctionCall::	operator = (const FMTFunctionCall& rhs)
	{
	if (this!=&rhs)
		{
		m_key = rhs.m_key;
		}
	return *this;
	}

}