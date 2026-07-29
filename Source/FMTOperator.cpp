/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOperator.h"
#include <algorithm>
#include "FMTLogger.h"
#include <algorithm>

namespace Core{

FMTOperator::FMTOperator() :m_key()
	{

	}

FMTOperator::FMTOperator(FMTokey lkey): m_key(lkey)
    {

    }

FMTOperator::FMTOperator(std::string strkey): m_key()
    {
	const std::vector<std::string>values = {"*","/","+","-",",","^"};
	m_key = FMTokey::notvalid;
	std::vector<std::string>::const_iterator valit = std::find(values.begin(), values.end(), strkey);
	if (valit!= values.end())
		{
		const int id = (static_cast<int>(std::find(values.begin(), values.end(), strkey) - values.begin()) + 1);
		m_key = static_cast<FMTokey>(id);
		}
    }

bool FMTOperator::operator == (const FMTOperator& rhs) const
	{
	return m_key == rhs.m_key;
	}

bool FMTOperator::operator != (const FMTOperator& rhs) const
	{
	return !(m_key == rhs.m_key);
	}

double FMTOperator::call(const double& rhs1, const double& rhs2) const
    {
	double returnval = 0;
	switch (m_key)
	{

	case FMTokey::add:
	{
		returnval = rhs1 + rhs2;
		break;
	}
	case FMTokey::sub:
	{
		returnval = rhs1 - rhs2;
		break;
	}
	case FMTokey::divide:
	{
		returnval = rhs1 / rhs2;
		break;
	}
	case FMTokey::multiply:
	{
		returnval = rhs1 * rhs2;
		break;
	}
	case FMTokey::pow:
	{
	returnval = std::pow(rhs1, rhs2);
	break;
	}
        default:
			returnval = 0;
        break;
        };
	return returnval;
    }
FMTOperator::FMTOperator(const FMTOperator& rhs) : m_key(rhs.m_key)
    {

    }
FMTOperator& FMTOperator::operator = (const FMTOperator& rhs)
    {
    if (this!=&rhs)
        {
        m_key = rhs.m_key;
        }
    return *this;
    }
FMTOperator::operator std::string() const
    {
    switch (m_key)
        {
        case FMTokey::add:
            return "+";
        break;
        case FMTokey::sub:
            return "-";
        break;
        case FMTokey::divide:
            return "/";
        break;
        case FMTokey::multiply:
            return "*";
        break;
		case FMTokey::comma:
			return ",";
		break;
		case FMTokey::pow:
			return "^";
		break;
        default:
            return "";
        break;
        };

    }
bool FMTOperator::valid() const
	{
	return (FMTokey::notvalid != m_key);
	}

size_t FMTOperator::hash() const
	{
	size_t seed = 0;
	boost::hash_combine(seed, m_key);
	return seed;
	}

bool FMTOperator::isFactor() const
	{
	return (m_key == FMTokey::divide || m_key == FMTokey::multiply);
	}

bool FMTOperator::isDivide() const
	{
	return m_key == FMTokey::divide;
	}


FMTokey FMTOperator::getKey() const
	{
	return m_key;
	}

std::string FMTOperator::associativity() const
	{
	std::string asso = "RIGHT";
	if (m_key==FMTokey::comma || m_key == FMTokey::add || m_key == FMTokey::multiply || m_key == FMTokey::pow)
		{
		asso = "LEFT";
		}
	return asso;
	}

FMTOperator FMTOperator::reverse() const
	{
	FMTOperator newop(*this);
	if (m_key ==  FMTokey::add)
		{
		newop.m_key = FMTokey::sub;
	}else if (m_key == FMTokey::sub)
		{
		newop.m_key = FMTokey::add;
		}
	return newop;
	}

int FMTOperator::precedence() const
	{
	int result = -1;
	if (m_key == FMTokey::multiply||m_key == FMTokey::divide ||m_key == FMTokey::pow)
		{
		result = 20;
		}else if (m_key == FMTokey::add|| m_key == FMTokey::sub)
			{
			result = 10;
			}
	return result;
	}

}
