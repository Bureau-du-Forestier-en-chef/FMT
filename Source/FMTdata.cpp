/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdata.h"
#include <algorithm>
#include "FMTdevelopment.h"
#include "FMTyieldrequest.h"
#include "FMTmask.h"
#include "FMTexpression.h"
#include "FMTfunctioncall.h"
#include "FMTexceptionhandler.h"
#include <boost/lexical_cast.hpp>
#include "FMToperator.h"



namespace Core{


void FMTData::allocateCache() const
	{
	if (ops != FMTyieldparserop::FMTnone &&
		_cache == nullptr)
		{
		_cache = std::move(std::unique_ptr<std::map<FMTDevelopment, double>>(new std::map<FMTDevelopment, double>()));
		}
	}

void FMTData::deAllocateCache() const
	{
	if (_cache != nullptr)
		{
		_cache = std::move(std::unique_ptr<std::map<FMTDevelopment, double>>(nullptr));
		}
	}

FMTData::FMTData():ops(FMTyieldparserop::FMTnone), data(),source(),stacking(), _cache(nullptr), _agebase(false)
		{
			//_cache = new boost::unordered_map<FMTDevelopment,double>();
		}

FMTData::FMTData(const FMTData& rhs) :data(rhs.data),ops(rhs.ops),source(rhs.source), stacking(rhs.stacking), _cache(nullptr), _agebase(rhs._agebase)
	{
	//_cache = new boost::unordered_map<FMTDevelopment,double>(*rhs._cache);
	}

FMTData& FMTData::operator = (const FMTData& rhs)
    {
    if(this!=&rhs)
        {
        data = rhs.data;
        ops = rhs.ops;
        source = rhs.source;
		stacking = rhs.stacking;
		deAllocateCache();
		_agebase = rhs._agebase;
        }
    return *this;
    }

FMTyieldparserop FMTData::getOp() const
    {
    return ops;
    }

void FMTData::clearCache()
	{
		//_cache = std::unique_ptr<boost::unordered_map<FMTDevelopment,double>>(new boost::unordered_map<FMTDevelopment,double>());
		//delete _cache;
		//_cache = new boost::unordered_map<FMTDevelopment,double>();
		deAllocateCache();
		allocateCache();
	}

bool FMTData::constant() const
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

bool FMTData::nullData() const
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

/*size_t FMTData::hashdata(const int& period, const int& age, const FMTMask& mask) const
	{
	std::size_t seed = 0;
	boost::hash_combine(seed, boost::hash<Core::FMTMask>()(mask));
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

FMTDevelopment FMTData::getSummaryDevelopment(const FMTYieldRequest& request) const
	{
	const Core::FMTDevelopment& refdev = request.getDevelopment();
	int lperiod = refdev.getPeriod();
	if (ops != FMTyieldparserop::FMTcai && ops != FMTyieldparserop::FMTmai && ops!= FMTyieldparserop::FMTytp && ops != FMTyieldparserop::FMTrange)
		{
		if (!_agebase)
			{
			lperiod=0;
			}
		}
	return FMTDevelopment(request.getResumeMask(), refdev.getAge(),0, refdev.getPeriod());
	}

bool FMTData::cacheValue(const FMTYieldRequest& request) const
	{
	allocateCache();
	return _cache->find(this->getSummaryDevelopment(request)) != _cache->end();
	}
double FMTData::get(const FMTYieldRequest& request) const
	{
	allocateCache();
	return _cache->at(this->getSummaryDevelopment(request));
	}
void FMTData::set(const double& value, const FMTYieldRequest& request,const bool& age_only) const
	{
	allocateCache();
	_agebase = age_only;
	_cache->operator[](this->getSummaryDevelopment(request)) = value;
	}

std::vector<const std::string*> FMTData::getSources() const
	{
	std::vector<const std::string*>outSources;
	if (ops == FMTyieldparserop::FMTequation)
	{
		outSources.reserve(source.size());
		for (const std::string& val : source)
		{
			if (!FMTFunctionCall(val).valid() && !FMTOperator(val).valid() &&
				!(val.size() == 1 && (val.at(0) == '(' || val.at(0) == ')')))
			{
				outSources.push_back(&val);
			}
		}
	}
	else if (ops == FMTyieldparserop::FMTmodelpred)
	{
		//Get sources from the model
	}else {
		outSources.reserve(source.size());
		for (const std::string& value : source)
			{
			outSources.push_back(&value);
			}
		}
	return outSources;
	}

std::vector<std::string> FMTData::getSourcesCopy() const
{
	const std::vector<const std::string*> SOURCES = getSources();
	std::vector<std::string>result(SOURCES.size());
	size_t Id = 0;
	for (std::string& data : result)
		{
		data = *SOURCES.at(Id);
		++Id;
		}
	return result;
}

/*
std::vector<std::string> FMTData::getSource() const
    {
	if (ops == FMTyieldparserop::FMTequation)
		{
		std::vector<std::string>nsources;
		for (const std::string& val : source)
			{
			if (!FMTFunctionCall(val).valid() && !FMTOperator(val).valid() && 
				!(val.size() == 1 && (val.at(0) == '(' || val.at(0) == ')')))
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
*/
FMTData::operator std::string() const
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
		case FMTyieldparserop::FMTdelta:
			value += "_DELTA(";
		break;
		case FMTyieldparserop::FMTdistance:
			value += "_DISTANCE(";
		break;
		case FMTyieldparserop::FMTmax:
			value += "_MAX(";
		break;
		case FMTyieldparserop::FMTmin:
			value += "_MIN(";
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

std::vector<const double*>FMTData::getValues() const
{
	std::vector<const double*>values(stacking.size(),nullptr);
	size_t numid = 0;
	int id = 0;
	for (const bool& isvar : stacking)
	{
		if (!isvar)
		{
			values.at(id) = &data.at(numid);
			++numid;
		}
		++id;
	}
	return values;

}

/*
std::vector<double>FMTData::tovalues(const std::map<std::string, double>& sources) const
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
*/

FMTExpression FMTData::toExpression() const
	{
	std::vector<std::string>vals(source.size());
	int loc = 0;
	for (const std::string& val : source)
		{
		if (val.empty())
			{
			vals.at(loc) = boost::lexical_cast<std::string>(data.at(loc));
		}else {
			vals.at(loc) = val;
			}
		++loc;
		}
	return FMTExpression(vals);
	}


bool FMTData::operator == (const FMTData& rhs) const
	{
	return(ops == rhs.ops &&
		source == rhs.source &&
		stacking == rhs.stacking &&
		data == rhs.data);
	}

FMTData::FMTData(const std::vector<double>& lvalues,
	const FMTyieldparserop& lops,
	const std::vector<std::string>& lsource): ops(lops), source(lsource), stacking(), _cache(nullptr), _agebase(),data(lvalues)
	{
		//_cache = std::unique_ptr<boost::unordered_map<FMTDevelopment,double>>(new boost::unordered_map<FMTDevelopment,double>());
		//_cache = new boost::unordered_map<FMTDevelopment,double>();
	}

FMTData::FMTData(const std::vector<double>& lvalues,
                const FMTyieldparserop& lops,
                const std::vector<std::string>& lsource,
				const std::vector<bool>& varstack):ops(lops),source(lsource),stacking(varstack), _cache(nullptr), _agebase(),data(lvalues)
	{
		//_cache = std::unique_ptr<boost::unordered_map<FMTDevelopment,double>>(new boost::unordered_map<FMTDevelopment,double>());
		//_cache = new boost::unordered_map<FMTDevelopment,double>();
	}

FMTData FMTData::operator * (const double& factor) const
	{
	FMTData newData(*this);
	for (double& value : newData.data)
		{
		value *= factor;
		}
	return newData;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTData)
