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

#include "FMTdata.h"


namespace Core{

FMTdata::FMTdata():ops(FMTyieldparserop::FMTwsnone),source(),stacking(), _cache(),_agebase(false),data(){}

FMTdata::FMTdata(const FMTdata& rhs) : ops(rhs.ops),source(rhs.source), stacking(rhs.stacking), _cache(rhs._cache), _agebase(rhs._agebase),data(rhs.data){}

FMTdata& FMTdata::operator = (const FMTdata& rhs)
    {
    if(this!=&rhs)
        {
        data = rhs.data;
        ops = rhs.ops;
        source = rhs.source;
		stacking = rhs.stacking;
		_cache = rhs._cache;
		_agebase = rhs._agebase;
        }
    return *this;
    }

FMTyieldparserop FMTdata::getop() const
    {
    return ops;
    }

bool FMTdata::constant() const
	{
	if (ops == FMTyieldparserop::FMTwsnone)
		{
		vector<double>tempvalues(data);
		std::sort(tempvalues.begin(), tempvalues.end());
		int uniqueCount = static_cast<int>(std::unique(tempvalues.begin(), tempvalues.end()) - tempvalues.begin());
		return (uniqueCount == 1);
		}
	return false;
	}

bool FMTdata::nulldata() const
	{
	if (ops == FMTyieldparserop::FMTwsnone)
		{
		for (const double& value : data)
			{
			if (value!=0.0)
				{
				return false;
				}
			}
		return true;
		}
	return false;
	}

size_t FMTdata::hashdata(const int& period, const int& age, const FMTmask& mask) const
	{
	std::size_t seed = 0;
	boost::hash_combine(seed, boost::hash<Core::FMTmask>()(mask));
	boost::hash_combine(seed, boost::hash<int>()(age));
	if (ops != FMTyieldparserop::FMTwscai && ops != FMTyieldparserop::FMTwsmai && ops!= FMTyieldparserop::FMTwsytp && ops != FMTyieldparserop::FMTwsrange)
		{
		if (!_agebase)
			{
			boost::hash_combine(seed, boost::hash<int>()(period));
			}
		}
	return seed;
	}

bool FMTdata::cachevalue(const FMTmask& mask, const int& age, const int& period) const
	{
	return (_cache.find(this->hashdata(period, age, mask)) != _cache.end());
	}
double FMTdata::get(const FMTmask& mask, const int& age, const int& period) const
	{
	return _cache.at(this->hashdata(period, age, mask));
	}
void FMTdata::set(const double& value, const FMTmask& mask, const int& age, const int& period,const bool& age_only) const
	{
	_agebase = age_only;
	_cache[this->hashdata(period, age, mask)] = value;
	}

vector<string> FMTdata::getsource() const
    {
	if (ops == FMTyieldparserop::FMTwsequation)
		{
		vector<string>nsources;
		for (const string& val : source)
			{
			if (!FMTfunctioncall(val).valid() && !FMToperator(val).valid())
				{
				nsources.push_back(val);
				}
			}
		return nsources;
		}
    return source;
    }

FMTdata::operator string() const
    {
    string value = "";
    switch (ops)
        {
        case FMTyieldparserop::FMTwscai:
        value+="_CAI(";
        break;
        case FMTyieldparserop::FMTwsdivide:
        value+="_DIVIDE(";
        break;
        case FMTyieldparserop::FMTwsmai:
        value+="_MAI(";
        break;
        case FMTyieldparserop::FMTwsmultiply:
        value+="_MULTIPLY(";
        break;
        case FMTyieldparserop::FMTwsrange:
        value+="_RANGE(";
        break;
        case FMTyieldparserop::FMTwssubstract:
        value+="_SUBTRACT(";
        break;
        case FMTyieldparserop::FMTwssum:
        value+="_SUM(";
        break;
        case FMTyieldparserop::FMTwsytp:
        value+="_YTP(";
        break;
		case FMTyieldparserop::FMTwsequation:
		value += "_EQUATION(";
		break;
		case FMTyieldparserop::FMTwsendpoint:
		value += "_ENDPOINT(";
		break;
		case FMTyieldparserop::FMTwsdiscountfactor:
		value += "_DISCOUNTFACTOR(";
		break;
        default:
        break;
        }
	if (ops!=FMTyieldparserop::FMTwsequation)
		{
		size_t varid = 0;
		size_t numid = 0;
		for (const bool& isvar : stacking)
			{
			if (isvar)
				{
				value += source[varid]+",";
				++varid;
			}else {
				value += boost::lexical_cast<std::string>(data[numid]) + ",";
				++numid;
				}
			}
		value.pop_back();

		}else{
		int loc = 0;
		for (const string& srcval : source)
			{
			if (!srcval.empty())
				{
				value += srcval;
			}else {
				value += boost::lexical_cast<std::string>(data[loc]);
				}
			loc += 1;
			}
		}
    value+=")";
    return value;
    }

vector<double>FMTdata::tovalues(const map<string, double>& sources) const
	{
	vector<double>values(stacking.size(),0.0);
	size_t varid = 0;
	size_t numid = 0;
	int id = 0;
	for (const bool& isvar : stacking)
		{
		if (isvar)
		{
			values[id] = sources.at(source.at(varid));
			++varid;
		}else {
			values[id] = data.at(numid);
			++numid;
		}
		++id;
		}
	return values;
	}


FMTexpression FMTdata::toexpression() const
	{
	vector<string>vals(source.size());
	int loc = 0;
	for (const string& val : source)
		{
		if (val.empty())
			{
			//vals[loc]=(to_string(data[loc]));
			vals[loc] = boost::lexical_cast<std::string>(data[loc]);
		}else {
			vals[loc]=val;
			}
		++loc;
		}
	return FMTexpression(vals);
	}


bool FMTdata::operator == (const FMTdata& rhs) const
	{
	return(ops == rhs.ops &&
		source == rhs.source &&
		stacking == rhs.stacking &&
		data == rhs.data);
	}

FMTdata::FMTdata(const vector<double>& lvalues,
	const FMTyieldparserop& lops,
	const vector<string>& lsource): ops(lops), source(lsource), stacking(), _cache(), _agebase(),data(lvalues) {}

FMTdata::FMTdata(const vector<double>& lvalues,
                const FMTyieldparserop& lops,
                const vector<string>& lsource,
				const vector<bool>& varstack):ops(lops),source(lsource),stacking(varstack), _cache(), _agebase(),data(lvalues){}

}
