/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTcomplexyieldhandler.h"
#include "FMTyieldrequest.h"
#include "FMTdevelopment.h"
#include "FMTageyieldhandler.h"
#include "FMTdata.h"
#include "FMTfunctioncall.h"
#include "FMTexceptionhandler.h"
#include "FMToperator.h"
#include "FMTexception.h"
#include "FMTexpression.h"
#include <unordered_set>


namespace Core {
	

	FMTComplexYieldHandler::operator std::string() const
	{
		std::string value;
		try {
			value += "*YC " + std::string(mask);
			if (getOverrideIndex()>0)
				{
				value += " _OVERRIDE";
				}
			

			value += "\n";
			std::vector<std::string>YieldsNames;
			for (std::map<std::string, FMTData>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
				{
				YieldsNames.push_back(it->first);
				}
			std::sort(YieldsNames.begin(), YieldsNames.end());
			for (const std::string& Name : YieldsNames)
				{
				value += Name + " " + std::string(m_elements.at(Name)) + "\n";
				}

		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTComplexYieldHandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}


	bool FMTComplexYieldHandler::pushData(const std::string& yld, const double& value)
	{
		return (basePushData(m_elements, yld, value));
	}

	bool FMTComplexYieldHandler::pushData(const std::string& yld, const FMTData& data)
	{
		return (basePushData(m_elements, yld, data));
	}

	std::vector<std::string> FMTComplexYieldHandler::indexes(const std::vector<std::string>& names) const
	{
		std::vector<std::string>indexs;
		try {
				for (std::map<std::string, FMTData>::const_iterator data_it = m_elements.begin(); data_it != m_elements.end(); data_it++)
				{
					if (data_it->second.getOp() == FMTyieldparserop::FMTequation)
					{
						const std::vector<const std::string*>variables = data_it->second.getSources();
						for (const std::string* variable : variables)
						{
							if (!variable->empty() && std::find(names.begin(), names.end(), *variable) == names.end() &&
								!FMTFunctionCall(*variable).valid() &&
								!FMTOperator(*variable).valid() &&
								(*variable != ")" && *variable != "("))
							{
								indexs.push_back(*variable);
							}
						}
					}
				}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTComplexYieldHandler::indexes", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return indexs;
	}


	bool FMTComplexYieldHandler::operator == (const FMTComplexYieldHandler& rhs) const
	{
		return (FMTYieldHandler::operator==(rhs) &&
			m_elements == rhs.m_elements);
	}



	std::map<std::string, double> FMTComplexYieldHandler::getSources(const std::map<std::string, const std::unique_ptr<FMTYieldHandler>*>& srcdata, const FMTYieldRequest& request, bool& age_only) const
	{
		std::map<std::string, double>alldata;
		try {
			for (std::map<std::string, const std::unique_ptr<FMTYieldHandler>*>::const_iterator datait = srcdata.begin(); datait != srcdata.end(); datait++)
			{
				alldata[datait->first] = 0;
				if (datait->second != nullptr)
				{
					const std::unique_ptr<FMTYieldHandler>* yldata = datait->second;
					if ((*yldata)->getType() != FMTyldtype::FMTageyld)
					{
						age_only = false;
					}
					
					alldata[datait->first] = (*yldata)->get(datait->first, request);
				}

			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTComplexYieldHandler::getSources", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return alldata;
	}

	std::unique_ptr<FMTYieldHandler>FMTComplexYieldHandler::toAgeYld(const FMTYieldRequest& request,
		const std::vector<std::string>& yieldnames, const int& minage, const int& maxage) const
	{
		FMTAgeYieldHandler nhandler(mask);
		try {
			for (int age = minage; age <= maxage; ++age)
			{
				nhandler.pushBase(age);
			}
			for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				if (containsYield(yieldnames.at(id)))
				{
					Core::FMTDevelopment newDev(request.getDevelopment());
					for (int age = minage; age <= maxage; ++age)
					{
						newDev.setAge(age);
						const FMTYieldRequest newrequest(newDev, request);
						nhandler.pushData(yieldnames.at(id), get(yieldnames.at(id), newrequest));
					}
				}
			}
		}
		catch (...) {
			_exhandler->raiseFromCatch("Error in converting complexyield to ageyield for yieldhandler " + std::string(*this), "FMTComplexYieldHandler::toAgeYld", __LINE__, __FILE__);
		}
		return nhandler.clone();
	}


	std::vector<double>FMTComplexYieldHandler::getSourcesArray(const std::map<std::string, const std::unique_ptr<FMTYieldHandler>*>& srcdata, const FMTYieldRequest& request, bool& age_only) const
	{
		std::vector<double>alldata(srcdata.size(), 0.0);
		try {
			size_t location = 0;
			for (std::map<std::string, const std::unique_ptr<FMTYieldHandler>*>::const_iterator datait = srcdata.begin(); datait != srcdata.end(); datait++)
			{
				if (datait->second != nullptr)
				{
					const std::unique_ptr<FMTYieldHandler>* yldata = datait->second;
					if ((*yldata)->getType() != FMTyldtype::FMTageyld)
					{
						age_only = false;
					}
					alldata[location] = (*yldata)->get(datait->first, request);
				}
				++location;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTComplexYieldHandler::getSourcesArray", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return alldata;
	}

	FMTComplexYieldHandler::~FMTComplexYieldHandler()
		{
		//_cache.clearHalf();
		}
	
	bool FMTComplexYieldHandler::compareSources(const std::string& yield, const FMTComplexYieldHandler& overridedyield) const
	{
		try {
				for (const auto& data : m_elements)
					{
					const std::vector<std::string const*>sources = data.second.getSources();
					size_t location = 0;
					while (location < sources.size())
						{
						if (*sources[location]==yield)
							{
							return false;
							}
						++location;
						}
					}
			
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTComplexYieldHandler::compareSources", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return true;
	}

	void FMTComplexYieldHandler::setTabou(const size_t& index)
		{
		overridetabou.insert(index);
		}

	std::vector<size_t>FMTComplexYieldHandler::getTabous() const
	{
		return std::vector<size_t>(overridetabou.begin(), overridetabou.end());
	}

	void FMTComplexYieldHandler::setTabou(const FMTComplexYieldHandler& rhs)
		{
		overrideindex = rhs.overrideindex;
		overridetabou = rhs.overridetabou;
		}

	void FMTComplexYieldHandler::setOverrideIndex(const size_t& newindex)
		{
		overrideindex = newindex;
		}

	size_t  FMTComplexYieldHandler::getOverrideIndex() const
		{
		return overrideindex;
		}

	std::vector<const std::unique_ptr<FMTYieldHandler>*>FMTComplexYieldHandler::_getData(const FMTYieldRequest& p_request,
		const std::vector<const std::string*>& p_names, const std::string& p_original) const
	{
		std::vector<const std::unique_ptr<FMTYieldHandler>*>data(p_names.size(), nullptr);
		try{
			const bool NEED_TO_TEST_OVERRIDE = !overridetabou.empty();
			const std::vector<FMTYieldRequest::const_iterator>& FULL_DATA = p_request.getDatas();
			for (size_t YldId = 0; YldId < p_names.size(); ++YldId)
				{
				const bool BASE_CASE = (inLookAt(*p_names.at(YldId)) || (p_original == *p_names.at(YldId)));
				size_t dataId = 0;
				bool foundValue = false;
				FMTYieldRequest::const_iterator FIRST_SEEN = p_request.getFirstSeen(*p_names.at(YldId));
				while (!foundValue && dataId < FULL_DATA.size())
						{
					const std::unique_ptr<FMTYieldHandler>* YIELD = &FULL_DATA.at(dataId)->second;
						if (!NEED_TO_TEST_OVERRIDE ||
							overridetabou.find((*YIELD)->getOverrideIndex()) == overridetabou.end())
							{
							if (FULL_DATA.at(dataId) >= FIRST_SEEN &&
								(*YIELD)->containsYield(*p_names.at(YldId)) &&
								!(this == &(**YIELD) && BASE_CASE))
								{
									data.at(YldId) = YIELD;
									foundValue = true;
								}
							}
						++dataId;
						}
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTComplexYieldHandler::_getData", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return data;
	}

	std::map<std::string, double>FMTComplexYieldHandler::_toMap(const FMTYieldRequest& p_request, 
														const std::vector<const std::string*>& p_names,
														const std::vector<const std::unique_ptr<FMTYieldHandler>*>& p_data)
	{
		std::map<std::string, double>result;
		for (size_t Id = 0; Id < p_names.size(); ++Id)
		{
			const std::unique_ptr<FMTYieldHandler>* DATA = p_data.at(Id);
			const std::string* YIELD_NAME = p_names.at(Id);
			std::pair<std::map<std::string, double>::iterator,bool> newValue = result.insert(std::pair<std::string, double>(*YIELD_NAME, 0.0));
			if (DATA != nullptr)
			{
				const double VALUE = (*DATA)->get(newValue.first->first, p_request);
				newValue.first->second = VALUE;
			}
		}
		return result;
	}


	double FMTComplexYieldHandler::_getRange(const FMTData* p_data,const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 1;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			size_t SourceId = 0;
			for (size_t Id = 0; Id < SOURCES.size(); ++Id)
			{
				const std::unique_ptr<FMTYieldHandler>* DATA = SOURCES_DATA.at(Id);
				double yieldValue = 0.0;
				if (DATA != nullptr)
				{
					const std::string* YIELD_RANGE = SOURCES.at(Id);
					yieldValue = (*DATA)->get(*YIELD_RANGE, p_request);
				}
				if (yieldValue < p_data->data.at(SourceId) ||
					yieldValue > p_data->data.at(SourceId + 1))
				{
					value = 0;
					break;
				}
				SourceId += 2;
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("On yield " + p_yld , "FMTComplexYieldHandler::_getRange", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTComplexYieldHandler::_getMultiply(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 1;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			for (size_t Id = 0; Id < SOURCES.size(); ++Id)
			{
				const std::unique_ptr<FMTYieldHandler>* DATA = SOURCES_DATA.at(Id);
				double yieldValue = 0.0;
				if (DATA != nullptr)
				{
					const std::string* YIELD_NAME = SOURCES.at(Id);
					yieldValue = (*DATA)->get(*YIELD_NAME, p_request);
				}
				value *= yieldValue;
			}
			for (const double& vecValue : p_data->data)
			{
				value *= vecValue;
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getMultiply", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	double FMTComplexYieldHandler::_getSum(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			for (size_t Id = 0; Id < SOURCES.size(); ++Id)
			{
				const std::unique_ptr<FMTYieldHandler>* DATA = SOURCES_DATA.at(Id);
				double yieldValue = 0.0;
				if (DATA != nullptr)
				{
					const std::string* YIELD_NAME = SOURCES.at(Id);
					yieldValue = (*DATA)->get(*YIELD_NAME, p_request);
				}
				value += yieldValue;
			}
			for (const double& vecValue : p_data->data)
			{
				value += vecValue;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getSum", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return value;
	}

	double FMTComplexYieldHandler::_getSubstract(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0.0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			std::vector<const double*>VALUES = p_data->getValues();
			size_t Id = 0;
			for (size_t valueId = 0; valueId < VALUES.size(); ++valueId)
			{
				double theValue = 0;
				if (VALUES.at(valueId) != nullptr)
				{
					theValue = *VALUES.at(valueId);
				}
				else {
					const std::unique_ptr<FMTYieldHandler>* DATA = SOURCES_DATA.at(Id);
					if (DATA != nullptr)
					{
						const std::string* YIELD_NAME = SOURCES.at(Id);
						theValue = (*DATA)->get(*YIELD_NAME, p_request);
					}
					++Id;
				}
				if (valueId == 0)
				{
					value = theValue;
				}
				else {
					value -= theValue;
				}
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getSubstract", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return value;
	}

	double FMTComplexYieldHandler::_getDivide(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0.0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			std::vector<const double*>VALUES = p_data->getValues();
			size_t Id = 0;
			for (size_t valueId = 0; valueId < VALUES.size(); ++valueId)
			{
				double theValue = 0;
				if (VALUES.at(valueId) != nullptr)
				{
					theValue = *VALUES.at(valueId);
				}
				else {
					const std::unique_ptr<FMTYieldHandler>* DATA = SOURCES_DATA.at(Id);
					if (DATA != nullptr)
					{
						const std::string* YIELD_NAME = SOURCES.at(Id);
						theValue = (*DATA)->get(*YIELD_NAME, p_request);
					}
					++Id;
				}
				if (valueId == 0)
				{
					value = theValue;
				}
				else {
					if (theValue == 0.0)
					{
						value = 0.0;
					}
					else {
						value /= theValue;
					}
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getDivide", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTComplexYieldHandler::_getYTP(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			const int AGE = p_request.getDevelopment().getAge();
			const std::unique_ptr<FMTYieldHandler>* ddata = SOURCES_DATA.at(0);
			value = (*ddata)->getPeak(p_request, *SOURCES.at(0), AGE);
		}catch (...)
			{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getYTP", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTComplexYieldHandler::_getMAI(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0;
		try {
			double year = 1;
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			if (p_data->data.begin() != p_data->data.end())
			{
				year = *p_data->data.begin();
			}
			const std::unique_ptr<FMTYieldHandler>* ddata = SOURCES_DATA.at(0);
			const int AGE = p_request.getDevelopment().getAge();
			value = ((*ddata)->getYieldLinearValue(*SOURCES.at(0), p_request, false) / (year * AGE));
		}catch (...)
			{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getMAI", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTComplexYieldHandler::_getCAI(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0;
		try {
			double year = 1;
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			if (p_data->data.begin() != p_data->data.end())
			{
				year = *p_data->data.begin();
			}
			const std::unique_ptr<FMTYieldHandler>* ddata = SOURCES_DATA.at(0);
			//const double upval = (*ddata)->getYieldLinearValue(sources.at(0), age);
			const double upval = (*ddata)->getYieldLinearValue(*SOURCES.at(0), p_request);
			const int AGE = p_request.getDevelopment().getAge();
			const int newage = AGE - 1;
			Core::FMTDevelopment newdevelopment(p_request.getDevelopment());
			newdevelopment.setAge(newage);
			const FMTYieldRequest newrequest(newdevelopment, p_request);
			//const double dwval = (*ddata)->getYieldLinearValue(sources.at(0), newage);
			const double dwval = (*ddata)->getYieldLinearValue(*SOURCES.at(0), newrequest);
			value = ((upval - dwval) / (year));
		}catch (...)
			{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getCAI", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTComplexYieldHandler::_getEquation(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			const FMTExpression EXPRESSION = p_data->toExpression();
			const std::map<std::string, double>source_values = _toMap(p_request, SOURCES, SOURCES_DATA);
			value = EXPRESSION.shuntingYard(source_values);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getEquation", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	double FMTComplexYieldHandler::_getEndPoint(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			const std::map<std::string, double>source_values = _toMap(p_request, SOURCES, SOURCES_DATA);
			const double lowerbound = p_data->data.at(0);
			const double upperbound = p_data->data.at(1);
			const std::vector<std::string const*> ylds = p_data->getSources();
			int peak = -1;
			int lowerpeak = -1;
			const std::unique_ptr<FMTYieldHandler>* ddata;
			const int AGE = p_request.getDevelopment().getAge();
			if (source_values.at(*ylds.at(0)) < lowerbound)
			{
				//ddata = srcsdata.at(*ylds.at(0));
				ddata = SOURCES_DATA.at(0);
				peak = (*ddata)->getEndPoint(*ylds.at(0), lowerpeak, lowerbound, source_values.at(*ylds.at(0)));
				value = (-getChangesFrom(AGE, peak));
			}
			if (source_values.at(*ylds.at(1)) > upperbound)
			{
				//ddata = srcsdata.at(*ylds.at(0));
				ddata = SOURCES_DATA.at(0);
				lowerpeak = (*ddata)->getEndPoint(*ylds.at(0), lowerpeak, lowerbound, std::numeric_limits<double>::lowest());
				//ddata = srcsdata.at(*ylds.at(1));
				ddata = SOURCES_DATA.at(1);
				peak = (*ddata)->getEndPoint(*ylds.at(1), lowerpeak, upperbound, source_values.at(*ylds.at(1)));
				value = (-getChangesFrom(AGE, peak));
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getEndPoint", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTComplexYieldHandler::_getDelta(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
		{
		double value = 0;
		try {
			const FMTData* C_DATA = &m_elements.at(p_yld);
			const std::vector<const std::string*> SOURCES = C_DATA->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			const std::map<std::string, double>source_values = _toMap(p_request, SOURCES, SOURCES_DATA);
			const int PERIOD = p_request.getDevelopment().getPeriod();
			const int periodtolookat = std::max(0, PERIOD + static_cast<int>(C_DATA->data.back()));
			Core::FMTDevelopment newdevelopment(p_request.getDevelopment());
			newdevelopment.setPeriod(periodtolookat);
			const FMTYieldRequest newrequest(newdevelopment, p_request);
			const std::map<std::string, double>periodic_source_values = _toMap(newrequest, SOURCES, SOURCES_DATA);
			value = std::abs(source_values.begin()->second - periodic_source_values.begin()->second);
		}catch (...)
			{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getDelta", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
		}

	double FMTComplexYieldHandler::_getDistance(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			/*The distance function calculate the distance in age between a minimal yield value and a maximal yield value when you start from age 0,
						if the peak of the curve is reached before finding the maximal yield it will return infinity,so it only works with the yield value
						before the yield peak*/
			const double lowerBound = p_data->data.at(0);
			const double upperBound = p_data->data.at(1);
			double localvalue = 0;
			int localage = 0;
			double minage = 0.0;
			double maxage = 0.0;
			double dblage = 0.0;
			Core::FMTDevelopment newdevelopment(p_request.getDevelopment());
			newdevelopment.setAge(1);
			FMTYieldRequest newrequest(newdevelopment, p_request);
			const std::unique_ptr<FMTYieldHandler>* ddata = SOURCES_DATA.at(0);
			const double peakage = (*ddata)->getPeak(newrequest, *SOURCES.at(0), 0);
			bool gotminage = false;
			bool gotmaxage = false;
			while (localvalue < upperBound && dblage <= peakage)
			{
				newdevelopment.setAge(localage);
				dblage = static_cast<double>(localage);
				const FMTYieldRequest localrequest(newdevelopment, p_request);
				//const std::vector<double>values = getSourcesArray(srcsdata, localrequest, age_only);
				//localvalue = values.at(0);
				localvalue = (*SOURCES_DATA.at(0))->get(*SOURCES.at(0), localrequest);
				if ((minage < dblage) && (localvalue >= lowerBound) && !gotminage)
				{
					minage = dblage;
					gotminage = true;
				}
				else if ((localvalue >= upperBound) && !gotmaxage)
				{
					maxage = dblage;
					gotmaxage = true;
				}
				++localage;
			}
			value = maxage - minage;
			if (value < 0 || !(gotmaxage && gotminage))
			{
				value = 0;
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"Cannot reach distance bounds for " + p_yld + " on development " + std::string(p_request.getDevelopment()), "FMTComplexYieldHandler::_getDistance", __LINE__, __FILE__);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getDistance", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTComplexYieldHandler::_getMax(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			double maxValue = 0;
			for (size_t Id = 0; Id < SOURCES.size(); ++Id)
			{
				const std::unique_ptr<FMTYieldHandler>* DATA = SOURCES_DATA.at(Id);
				const std::string* YIELD_NAME = SOURCES.at(Id);
				if (DATA != nullptr)
				{
					const double VALUE = (*DATA)->get(*YIELD_NAME, p_request);
					if (VALUE > maxValue)
					{
						maxValue = VALUE;
					}
				}
			}
			value = maxValue;
		}catch (...)
		{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getMax", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	double FMTComplexYieldHandler::_getMin(const FMTData* p_data, const std::string& p_yld, const FMTYieldRequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTYieldHandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			double minValue = std::numeric_limits<double>::max();
			for (size_t Id = 0; Id < SOURCES.size(); ++Id)
			{
				const std::unique_ptr<FMTYieldHandler>* DATA = SOURCES_DATA.at(Id);
				const std::string* YIELD_NAME = SOURCES.at(Id);
				if (DATA != nullptr)
				{
					const double VALUE = (*DATA)->get(*YIELD_NAME, p_request);
					if (VALUE < minValue)
					{
						minValue = VALUE;
					}
				}
			}
			if (minValue != std::numeric_limits<double>::max())
			{
				value = minValue;
			}
			else {
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Cannot find minimum for " + p_yld + " on development " + std::string(p_request.getDevelopment()), "FMTComplexYieldHandler::_getMin", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("On yield " + p_yld, "FMTComplexYieldHandler::_getMin", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	

	double FMTComplexYieldHandler::get(const std::string& yld, const FMTYieldRequest& request) const
	{
		double value = 0;
		try {
			if (_cache.inCache(request, yld))
				{
				return _cache.get(request, yld);
				}
			const FMTData* C_DATA = &m_elements.at(yld);
			std::chrono::time_point<std::chrono::high_resolution_clock>calculationStart;
			if (lookat.empty())
				{
				calculationStart = getClock();
				}
				if (lookat.find(yld) == lookat.end())
					{
						lookat.insert(yld);
					}
					else {
						_exhandler->raise(Exception::FMTexc::FMTinvalid_yield, "Recursivity detected for complexe yield " + yld,
							"FMTComplexYieldHandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
					}
					switch (C_DATA->getOp())
					{
					case FMTyieldparserop::FMTrange:
					{
						value = _getRange(C_DATA,yld, request);
						break;
					}
					case FMTyieldparserop::FMTmultiply:
					{
						value = _getMultiply(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTsum:
					{
						value = _getSum(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTsubstract:
					{
						value = _getSubstract(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTdivide:
					{
						value = _getDivide(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTytp:
					{
						value = _getYTP(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTmai:
					{
						value = _getMAI(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTcai:
					{
						value = _getCAI(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTequation:
					{
						value = _getEquation(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTendpoint:
					{
						value = _getEndPoint(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTdelta:
					{
						value = _getDelta(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTdistance:
					{
						value = _getDistance(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTmax:
					{
						value = _getMax(C_DATA, yld, request);
						break;
					}
					case FMTyieldparserop::FMTmin:
					{
						value = _getMin(C_DATA, yld, request);
						break;
					}

					default:
						break;
					}
					lookat.erase(yld);
				value = std::round(value * 100000000) / 100000000;
				
				if (lookat.empty())//Cache only first cally
				{
					const double TIME_TOOK = getDuration<std::chrono::milliseconds::period>(calculationStart);
					if (TIME_TOOK>0.05)
						{
						_cache.set(value, request, yld);
						}
				}
				
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("at yield " + yld, "FMTComplexYieldHandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	int FMTComplexYieldHandler::getLastBase() const
	{
		return 0;
	}

	const std::map<std::string, FMTData,cmpYieldString>& FMTComplexYieldHandler::getDataElements() const
	{
		return m_elements;
	}
	bool FMTComplexYieldHandler::empty() const
	{
		return m_elements.empty();
	}
	size_t FMTComplexYieldHandler::size() const
	{
		return m_elements.size();
	}
	FMTData& FMTComplexYieldHandler::operator[](const std::string& yldname)
	{
		return m_elements[yldname];
	}
	const FMTData& FMTComplexYieldHandler::at(const std::string& yldname) const
	{
		return m_elements.at(yldname);
	}
	bool FMTComplexYieldHandler::containsYield(const std::string& yldname) const
	{
		return (m_elements.find(yldname) != m_elements.end());
	}

	std::vector<std::string>FMTComplexYieldHandler::getYieldNames() const
	{
		std::vector<std::string>results;
		results.reserve(m_elements.size());
		for (const auto& data : m_elements)
		{
			results.push_back(data.first);
		}
		return results;
	}

	void FMTComplexYieldHandler::clearCache()
	{
		for (auto& data : m_elements)
		{
			data.second.clearCache();
		}
	}

	std::unique_ptr<FMTYieldHandler>FMTComplexYieldHandler::clone() const
	{
		return std::unique_ptr<FMTYieldHandler>(new FMTComplexYieldHandler(*this));
	}

	FMTyldtype FMTComplexYieldHandler::getType() const
	{
		return FMTyldtype::FMTcomplexyld;
	}

	FMTComplexYieldHandler::FMTComplexYieldHandler(const FMTMask& mask):
		FMTYieldHandler(mask), m_elements(), overridetabou(), overrideindex(0), _cache()
	{

	}

	FMTComplexYieldHandler::FMTComplexYieldHandler() :
		FMTYieldHandler(), m_elements(), overridetabou(), overrideindex(0), _cache()
	{

	}

	std::unique_ptr<FMTYieldHandler> FMTComplexYieldHandler::complexYldToAgeYld(const FMTYieldRequest& request, const FMTSpec& lspec) const
	{
		
		try {
			return toAgeYld(request, lspec.getYlds(), 0, request.getDevelopment().getAge());
		}
		catch (...) {
			_exhandler->raiseFromCatch("Error in converting complexyield to ageyield for yieldhandler " + std::string(*this), "FMTYieldRequest::complexYldToAgeYld", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTYieldHandler>();
	}

	double FMTComplexYieldHandler::getYieldLinearValue(const std::string& yldname, const FMTYieldRequest& request, bool allowoutofrange) const //should allowoutofrange always false??? to get a 0 at the end of the curve
	{
		double returned = 0;
		try {
			std::vector<std::string>target(1, yldname);
			const std::unique_ptr<FMTYieldHandler> ageyield =  toAgeYld(request, target, 0, request.getDevelopment().getAge());
			returned = ageyield->getYieldLinearValue(yldname, request);
		}
		catch (...) {
			_exhandler->raiseFromCatch("On yield " + yldname, "FMTComplexYieldHandler::getYieldLinearValue", __LINE__, __FILE__);
		}
		return returned;
	}

	double FMTComplexYieldHandler::getPeak(const FMTYieldRequest& request, const std::string& yld, const int& targetage) const
	{
		try {
			std::vector<std::string>ylds(1, yld);
			const int maxbase = getMaxBase(request);
			return toAgeYld(request, ylds, 0, maxbase)->getPeak(request,yld,targetage);
		}catch (...) {
			_exhandler->raiseFromCatch("", "FMTComplexYieldHandler::getPeak", __LINE__, __FILE__);
		}
		return 0;
	}

	int FMTComplexYieldHandler::getAge(const FMTYieldRequest& request, const FMTSpec& spec) const
	{
		try {
			std::unique_ptr<FMTYieldHandler>ageyield = complexYldToAgeYld(request, spec);
			return ageyield->getAge(request, spec);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::getAge", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}


}

