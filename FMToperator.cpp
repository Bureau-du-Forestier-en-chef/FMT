/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "FMToperator.h"

namespace Core{

FMToperator::FMToperator() :key()
	{

	}

FMToperator::FMToperator(FMTokey lkey): key(lkey)
    {

    }

FMToperator::FMToperator(string strkey): key()
    {
    vector<string>values = {"*","/","+","-",/*"(",")",*/",","^"};
	key = FMTokey::notvalid;
	vector<string>::iterator valit = std::find(values.begin(), values.end(), strkey);
	if (valit!= values.end())
		{
		int id = (int(std::find(values.begin(), values.end(), strkey) - values.begin()) + 1);
		key = static_cast<FMTokey>(id);
		}
    }

bool FMToperator::operator == (const FMToperator& rhs) const
	{
	return key == rhs.key;
	}

double FMToperator::call(const double& rhs1, const double& rhs2) const
    {
	double returnval = 0;
    switch (key)
        {
        case FMTokey::add:
			returnval = rhs1 + rhs2;
        break;
        case FMTokey::sub:
			returnval = rhs1 - rhs2;
        break;
        case FMTokey::divide:
			returnval = rhs1 / rhs2;
        break;
        case FMTokey::multiply:
			returnval = rhs1 * rhs2;
        break;
		case FMTokey::pow:
			returnval = pow(rhs1, rhs2);
        default:
			returnval = 0;
        break;
        };
	//returnval = std::round(returnval * 100000000) / 100000000;
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
FMToperator::operator string() const
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
		/*case FMTokey::leftparenthesis:
			return "(";
		break;
		case FMTokey::rightparenthesis:
			return ")";
		break;*/
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

/*bool FMToperator::isparenthesis() const
	{
	return (FMTokey::leftparenthesis == key || FMTokey::rightparenthesis == key);
	}*/

FMTokey FMToperator::getkey() const
	{
	return key;
	}

string FMToperator::associativity() const
	{
	string asso = "RIGHT";
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
