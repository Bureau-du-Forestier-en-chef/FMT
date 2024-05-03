/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMToperator.h"
#include <algorithm>
#include "FMTlogger.h"
#include <algorithm>

namespace Core{

FMToperator::FMToperator() :key()
	{

	}

FMToperator::FMToperator(FMTokey lkey): key(lkey)
    {

    }

FMToperator::FMToperator(std::string strkey): key()
    {
	const std::vector<std::string>values = {"*","/","+","-",",","^"};
	key = FMTokey::notvalid;
	std::vector<std::string>::const_iterator valit = std::find(values.begin(), values.end(), strkey);
	if (valit!= values.end())
		{
		const int id = (static_cast<int>(std::find(values.begin(), values.end(), strkey) - values.begin()) + 1);
		key = static_cast<FMTokey>(id);
		}
    }

bool FMToperator::operator == (const FMToperator& rhs) const
	{
	return key == rhs.key;
	}

bool FMToperator::operator != (const FMToperator& rhs) const
	{
	return !(key == rhs.key);
	}

double FMToperator::call(const double& rhs1, const double& rhs2) const
    {
	double returnval = 0;
	switch (key)
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
FMToperator::FMToperator(const FMToperator& rhs) : key(rhs.key)
    {

    }
FMToperator& FMToperator::operator = (const FMToperator& rhs)
    {
    if (this!=&rhs)
        {
        key = rhs.key;
        }
    return *this;
    }
FMToperator::operator std::string() const
    {
    switch (key)
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
bool FMToperator::valid() const
	{
	return (FMTokey::notvalid != key);
	}

size_t FMToperator::hash() const
	{
	size_t seed = 0;
	boost::hash_combine(seed, key);
	return seed;
	}

bool FMToperator::isfactor() const
	{
	return (key == FMTokey::divide || key == FMTokey::multiply);
	}

bool FMToperator::isdivide() const
	{
	return key == FMTokey::divide;
	}


FMTokey FMToperator::getkey() const
	{
	return key;
	}

std::string FMToperator::associativity() const
	{
	std::string asso = "RIGHT";
	if (key==FMTokey::comma || key == FMTokey::add || key == FMTokey::multiply || key == FMTokey::pow)
		{
		asso = "LEFT";
		}
	return asso;
	}

FMToperator FMToperator::reverse() const
	{
	FMToperator newop(*this);
	if (key ==  FMTokey::add)
		{
		newop.key = FMTokey::sub;
	}else if (key == FMTokey::sub)
		{
		newop.key = FMTokey::add;
		}
	return newop;
	}

int FMToperator::precedence() const
	{
	int result = -1;
	if (key == FMTokey::multiply||key == FMTokey::divide ||key == FMTokey::pow)
		{
		result = 20;
		}else if (key == FMTokey::add|| key == FMTokey::sub)
			{
			result = 10;
			}
	return result;
	}

}
