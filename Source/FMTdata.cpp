/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdata.hpp"
#include <algorithm>
#include "FMTdevelopment.hpp"
#include "FMTyieldrequest.hpp"
#include "FMTmask.hpp"
#include "FMTexpression.hpp"
#include "FMTfunctioncall.hpp"


namespace Core{

FMTdata::FMTdata():ops(FMTyieldparserop::FMTnone),source(),stacking(), _cache(),_agebase(false),data()
		{
			_cache = new boost::unordered_map<FMTdevelopment,double>();
		}

FMTdata::FMTdata(const FMTdata& rhs) : ops(rhs.ops),source(rhs.source), stacking(rhs.stacking), _cache(), _agebase(rhs._agebase),data(rhs.data)
	{
	_cache = new boost::unordered_map<FMTdevelopment,double>(*rhs._cache);
	}

FMTdata& FMTdata::operator = (const FMTdata& rhs)
    {
    if(this!=&rhs)
        {
        data = rhs.data;
        ops = rhs.ops;
        source = rhs.source;
		stacking = rhs.stacking;
		if(_cache!=nullptr)
		{
			delete _cache;
		}
		_cache = new boost::unordered_map<FMTdevelopment,double>(*rhs._cache);
		_agebase = rhs._agebase;
        }
    return *this;
    }

FMTyieldparserop FMTdata::getop() const
    {
    return ops;
    }

void FMTdata::clearcache()
	{
		//_cache = std::unique_ptr<boost::unordered_map<FMTdevelopment,double>>(new boost::unordered_map<FMTdevelopment,double>());
		delete _cache;
		_cache = new boost::unordered_map<FMTdevelopment,double>();
	}

bool FMTdata::constant() const
	{
	if (ops == FMTyieldparserop::FMTnone)
		{
		std::vector<double>tempvalues(data);
		std::sort(tempvalues.begin(), tempvalues.end());
		int uniqueCount = static_cast<int>(std::unique(tempvalues.begin(), tempvalues.end()) - tempvalues.begin());
		return (uniqueCount == 1);
		}
	return false;
	}

bool FMTdata::nulldata() const
	{
	if (ops == FMTyieldparserop::FMTnone)
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

/*size_t FMTdata::hashdata(const int& period, const int& age, const FMTmask& mask) const
	{
	std::size_t seed = 0;
	boost::hash_combine(seed, boost::hash<Core::FMTmask>()(mask));
	boost::hash_combine(seed, boost::hash<int>()(age));
	if (ops != FMTyieldparserop::FMTcai && ops != FMTyieldparserop::FMTmai && ops!= FMTyieldparserop::FMTytp && ops != FMTyieldparserop::FMTrange)
		{
		if (!_agebase)
			{
			boost::hash_combine(seed, boost::hash<int>()(period));
			}
		}
	return seed;
	}*/

FMTdevelopment FMTdata::getsummarydevelopment(const FMTyieldrequest& request) const
	{
	const Core::FMTdevelopment& refdev = request.getdevelopment();
	int lperiod = refdev.getperiod();
	if (ops != FMTyieldparserop::FMTcai && ops != FMTyieldparserop::FMTmai && ops!= FMTyieldparserop::FMTytp && ops != FMTyieldparserop::FMTrange)
		{
		if (!_agebase)
			{
			lperiod=0;
			}
		}
	return FMTdevelopment(request.getresumemask(), refdev.getage(),0, refdev.getperiod());
	}

bool FMTdata::cachevalue(const FMTyieldrequest& request) const
	{
	return (_cache->find(this->getsummarydevelopment(request)) != _cache->end());
	}
double FMTdata::get(const FMTyieldrequest& request) const
	{
	return _cache->at(this->getsummarydevelopment(request));
	}
void FMTdata::set(const double& value, const FMTyieldrequest& request,const bool& age_only) const
	{
	_agebase = age_only;
	_cache->operator[](this->getsummarydevelopment(request)) = value;
	}

std::vector<std::string> FMTdata::getsource() const
    {
	if (ops == FMTyieldparserop::FMTequation)
		{
		std::vector<std::string>nsources;
		for (const std::string& val : source)
			{
			if (!FMTfunctioncall(val).valid() && !FMToperator(val).valid())
				{
				nsources.push_back(val);
				}
			}
		return nsources;
	}else if (ops==FMTyieldparserop::FMTmodelpred)
		{
		//Get sources from the model
		}
    return source;
    }

FMTdata::operator std::string() const
    {
	std::string value = "";
    switch (ops)
        {
        case FMTyieldparserop::FMTcai:
        value+="_CAI(";
        break;
        case FMTyieldparserop::FMTdivide:
        value+="_DIVIDE(";
        break;
        case FMTyieldparserop::FMTmai:
        value+="_MAI(";
        break;
        case FMTyieldparserop::FMTmultiply:
        value+="_MULTIPLY(";
        break;
        case FMTyieldparserop::FMTrange:
        value+="_RANGE(";
        break;
        case FMTyieldparserop::FMTsubstract:
        value+="_SUBTRACT(";
        break;
        case FMTyieldparserop::FMTsum:
        value+="_SUM(";
        break;
        case FMTyieldparserop::FMTytp:
        value+="_YTP(";
        break;
		case FMTyieldparserop::FMTequation:
		value += "_EQUATION(";
		break;
		case FMTyieldparserop::FMTendpoint:
		value += "_ENDPOINT(";
		break;
		case FMTyieldparserop::FMTdiscountfactor:
		value += "_DISCOUNTFACTOR(";
		break;
		case FMTyieldparserop::FMTmodelpred:
			value += "_PRED(";
		break;
        default:
        break;
        }
	if (ops!=FMTyieldparserop::FMTequation)
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
		for (const std::string& srcval : source)
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

std::vector<double>FMTdata::tovalues(const std::map<std::string, double>& sources) const
	{
	std::vector<double>values(stacking.size(),0.0);
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
	std::vector<std::string>vals(source.size());
	int loc = 0;
	for (const std::string& val : source)
		{
		if (val.empty())
			{
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

FMTdata::FMTdata(const std::vector<double>& lvalues,
	const FMTyieldparserop& lops,
	const std::vector<std::string>& lsource): ops(lops), source(lsource), stacking(), _cache(), _agebase(),data(lvalues)
	{
		//_cache = std::unique_ptr<boost::unordered_map<FMTdevelopment,double>>(new boost::unordered_map<FMTdevelopment,double>());
		_cache = new boost::unordered_map<FMTdevelopment,double>();
	}

FMTdata::FMTdata(const std::vector<double>& lvalues,
                const FMTyieldparserop& lops,
                const std::vector<std::string>& lsource,
				const std::vector<bool>& varstack):ops(lops),source(lsource),stacking(varstack), _cache(), _agebase(),data(lvalues)
	{
		//_cache = std::unique_ptr<boost::unordered_map<FMTdevelopment,double>>(new boost::unordered_map<FMTdevelopment,double>());
		_cache = new boost::unordered_map<FMTdevelopment,double>();
	}

FMTdata FMTdata::operator * (const double& factor) const
	{
	FMTdata newdata(*this);
	for (double& value : newdata.data)
		{
		value *= factor;
		}
	return newdata;
	}

FMTdata::~FMTdata()
{
	delete _cache;
}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTdata)
