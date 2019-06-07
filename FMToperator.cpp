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
    vector<string>values = {"*","/","+","-",/*"(",")",*/","};
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
    switch (key)
        {
        case FMTokey::add:
            return rhs1 + rhs2;
        break;
        case FMTokey::sub:
            return rhs1 - rhs2;
        break;
        case FMTokey::divide:
            return rhs1 / rhs2;
        break;
        case FMTokey::multiply:
            return rhs1 * rhs2;
        break;
        default:
            return 0;
        break;
        };

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
	if (key==FMTokey::comma || key == FMTokey::add || key == FMTokey::multiply)
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
	if (key == FMTokey::multiply||key == FMTokey::divide)
		{
		result = 20;
		}else if (key == FMTokey::add|| key == FMTokey::sub)
			{
			result = 10;
			}
	return result;
	}

}
