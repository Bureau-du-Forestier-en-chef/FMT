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
	

	FMTcomplexyieldhandler::operator std::string() const
	{
		std::string value;
		try {
			value += "*YC " + std::string(mask);
			if (getoverrideindex()>0)
				{
				value += " _OVERRIDE";
				}
			

			value += "\n";
			std::vector<std::string>YieldsNames;
			for (std::map<std::string, FMTdata>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
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
			_exhandler->raisefromcatch("", "FMTcomplexyieldhandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}


	bool FMTcomplexyieldhandler::push_data(const std::string& yld, const double& value)
	{
		return (basepush_data(m_elements, yld, value));
	}

	bool FMTcomplexyieldhandler::push_data(const std::string& yld, const FMTdata& data)
	{
		return (basepush_data(m_elements, yld, data));
	}

	std::vector<std::string> FMTcomplexyieldhandler::indexes(const std::vector<std::string>& names) const
	{
		std::vector<std::string>indexs;
		try {
				for (std::map<std::string, FMTdata>::const_iterator data_it = m_elements.begin(); data_it != m_elements.end(); data_it++)
				{
					if (data_it->second.getop() == FMTyieldparserop::FMTequation)
					{
						const std::vector<const std::string*>variables = data_it->second.getSources();
						for (const std::string* variable : variables)
						{
							if (!variable->empty() && std::find(names.begin(), names.end(), *variable) == names.end() &&
								!FMTfunctioncall(*variable).valid() &&
								!FMToperator(*variable).valid() &&
								(*variable != ")" && *variable != "("))
							{
								indexs.push_back(*variable);
							}
						}
					}
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTcomplexyieldhandler::indexes", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return indexs;
	}


	bool FMTcomplexyieldhandler::operator == (const FMTcomplexyieldhandler& rhs) const
	{
		return (FMTyieldhandler::operator==(rhs) &&
			m_elements == rhs.m_elements);
	}



	std::map<std::string, double> FMTcomplexyieldhandler::getsources(const std::map<std::string, const std::unique_ptr<FMTyieldhandler>*>& srcdata, const FMTyieldrequest& request, bool& age_only) const
	{
		std::map<std::string, double>alldata;
		try {
			for (std::map<std::string, const std::unique_ptr<FMTyieldhandler>*>::const_iterator datait = srcdata.begin(); datait != srcdata.end(); datait++)
			{
				alldata[datait->first] = 0;
				if (datait->second != nullptr)
				{
					const std::unique_ptr<FMTyieldhandler>* yldata = datait->second;
					if ((*yldata)->gettype() != FMTyldtype::FMTageyld)
					{
						age_only = false;
					}
					
					alldata[datait->first] = (*yldata)->get(datait->first, request);
				}

			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTcomplexyieldhandler::getsources", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return alldata;
	}

	std::unique_ptr<FMTyieldhandler>FMTcomplexyieldhandler::toageyld(const FMTyieldrequest& request,
		const std::vector<std::string>& yieldnames, const int& minage, const int& maxage) const
	{
		FMTageyieldhandler nhandler(mask);
		try {
			for (int age = minage; age <= maxage; ++age)
			{
				nhandler.push_base(age);
			}
			for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				if (containsyield(yieldnames.at(id)))
				{
					Core::FMTdevelopment newdev(request.getdevelopment());
					for (int age = minage; age <= maxage; ++age)
					{
						newdev.setage(age);
						const FMTyieldrequest newrequest(newdev, request);
						nhandler.push_data(yieldnames.at(id), get(yieldnames.at(id), newrequest));
					}
				}
			}
		}
		catch (...) {
			_exhandler->raisefromcatch("Error in converting complexyield to ageyield for yieldhandler " + std::string(*this), "FMTcomplexyieldhandler::toageyld", __LINE__, __FILE__);
		}
		return nhandler.clone();
	}


	std::vector<double>FMTcomplexyieldhandler::getsourcesarray(const std::map<std::string, const std::unique_ptr<FMTyieldhandler>*>& srcdata, const FMTyieldrequest& request, bool& age_only) const
	{
		std::vector<double>alldata(srcdata.size(), 0.0);
		try {
			size_t location = 0;
			for (std::map<std::string, const std::unique_ptr<FMTyieldhandler>*>::const_iterator datait = srcdata.begin(); datait != srcdata.end(); datait++)
			{
				if (datait->second != nullptr)
				{
					const std::unique_ptr<FMTyieldhandler>* yldata = datait->second;
					if ((*yldata)->gettype() != FMTyldtype::FMTageyld)
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
			_exhandler->raisefromcatch("", "FMTcomplexyieldhandler::getsourcesarray", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return alldata;
	}

	FMTcomplexyieldhandler::~FMTcomplexyieldhandler()
		{
		//_cache.clearHalf();
		}
	
	bool FMTcomplexyieldhandler::comparesources(const std::string& yield, const FMTcomplexyieldhandler& overridedyield) const
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
			_exhandler->raisefromcatch("", "FMTcomplexyieldhandler::comparesources", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return true;
	}

	void FMTcomplexyieldhandler::settabou(const size_t& index)
		{
		overridetabou.insert(index);
		}

	std::vector<size_t>FMTcomplexyieldhandler::gettabous() const
	{
		return std::vector<size_t>(overridetabou.begin(), overridetabou.end());
	}

	void FMTcomplexyieldhandler::settabou(const FMTcomplexyieldhandler& rhs)
		{
		overrideindex = rhs.overrideindex;
		overridetabou = rhs.overridetabou;
		}

	void FMTcomplexyieldhandler::setoverrideindex(const size_t& newindex)
		{
		overrideindex = newindex;
		}

	size_t  FMTcomplexyieldhandler::getoverrideindex() const
		{
		return overrideindex;
		}

	std::vector<const std::unique_ptr<FMTyieldhandler>*>FMTcomplexyieldhandler::_getData(const FMTyieldrequest& p_request,
		const std::vector<const std::string*>& p_names, const std::string& p_original) const
	{
		std::vector<const std::unique_ptr<FMTyieldhandler>*>data(p_names.size(), nullptr);
		try{
			const bool NEED_TO_TEST_OVERRIDE = !overridetabou.empty();
			const std::vector<FMTyieldrequest::const_iterator>& FULL_DATA = p_request.getdatas();
			for (size_t YldId = 0; YldId < p_names.size(); ++YldId)
				{
				const bool BASE_CASE = (inlookat(*p_names.at(YldId)) || (p_original == *p_names.at(YldId)));
				size_t dataId = 0;
				bool foundValue = false;
				FMTyieldrequest::const_iterator FIRST_SEEN = p_request.getFirstSeen(*p_names.at(YldId));
				while (!foundValue && dataId < FULL_DATA.size())
						{
					const std::unique_ptr<FMTyieldhandler>* YIELD = &FULL_DATA.at(dataId)->second;
						if (!NEED_TO_TEST_OVERRIDE ||
							overridetabou.find((*YIELD)->getoverrideindex()) == overridetabou.end())
							{
							if (FULL_DATA.at(dataId) >= FIRST_SEEN &&
								(*YIELD)->containsyield(*p_names.at(YldId)) &&
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
			_exhandler->raisefromcatch("", "FMTcomplexyieldhandler::_getData", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return data;
	}

	std::map<std::string, double>FMTcomplexyieldhandler::_toMap(const FMTyieldrequest& p_request, 
														const std::vector<const std::string*>& p_names,
														const std::vector<const std::unique_ptr<FMTyieldhandler>*>& p_data)
	{
		std::map<std::string, double>result;
		for (size_t Id = 0; Id < p_names.size(); ++Id)
		{
			const std::unique_ptr<FMTyieldhandler>* DATA = p_data.at(Id);
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


	double FMTcomplexyieldhandler::_getRange(const FMTdata* p_data,const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 1;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			size_t SourceId = 0;
			for (size_t Id = 0; Id < SOURCES.size(); ++Id)
			{
				const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA.at(Id);
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
			_exhandler->raisefromcatch("On yield " + p_yld , "FMTcomplexyieldhandler::_getRange", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTcomplexyieldhandler::_getMultiply(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 1;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			for (size_t Id = 0; Id < SOURCES.size(); ++Id)
			{
				const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA.at(Id);
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
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getMultiply", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	double FMTcomplexyieldhandler::_getSum(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			for (size_t Id = 0; Id < SOURCES.size(); ++Id)
			{
				const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA.at(Id);
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
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getSum", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return value;
	}

	double FMTcomplexyieldhandler::_getSubstract(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0.0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
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
					const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA.at(Id);
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
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getSubstract", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	return value;
	}

	double FMTcomplexyieldhandler::_getDivide(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0.0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
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
					const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA.at(Id);
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
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getDivide", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTcomplexyieldhandler::_getYTP(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			const int AGE = p_request.getdevelopment().getage();
			const std::unique_ptr<FMTyieldhandler>* ddata = SOURCES_DATA.at(0);
			value = (*ddata)->getpeak(p_request, *SOURCES.at(0), AGE);
		}catch (...)
			{
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getYTP", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTcomplexyieldhandler::_getMAI(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0;
		try {
			double year = 1;
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			if (p_data->data.begin() != p_data->data.end())
			{
				year = *p_data->data.begin();
			}
			const std::unique_ptr<FMTyieldhandler>* ddata = SOURCES_DATA.at(0);
			const int AGE = p_request.getdevelopment().getage();
			value = ((*ddata)->getyieldlinearvalue(*SOURCES.at(0), p_request, false) / (year * AGE));
		}catch (...)
			{
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getMAI", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTcomplexyieldhandler::_getCAI(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0;
		try {
			double year = 1;
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			if (p_data->data.begin() != p_data->data.end())
			{
				year = *p_data->data.begin();
			}
			const std::unique_ptr<FMTyieldhandler>* ddata = SOURCES_DATA.at(0);
			//const double upval = (*ddata)->getyieldlinearvalue(sources.at(0), age);
			const double upval = (*ddata)->getyieldlinearvalue(*SOURCES.at(0), p_request);
			const int AGE = p_request.getdevelopment().getage();
			const int newage = AGE - 1;
			Core::FMTdevelopment newdevelopement(p_request.getdevelopment());
			newdevelopement.setage(newage);
			const FMTyieldrequest newrequest(newdevelopement, p_request);
			//const double dwval = (*ddata)->getyieldlinearvalue(sources.at(0), newage);
			const double dwval = (*ddata)->getyieldlinearvalue(*SOURCES.at(0), newrequest);
			value = ((upval - dwval) / (year));
		}catch (...)
			{
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getCAI", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTcomplexyieldhandler::_getEquation(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			const FMTexpression EXPRESSION = p_data->toexpression();
			const std::map<std::string, double>source_values = _toMap(p_request, SOURCES, SOURCES_DATA);
			value = EXPRESSION.shuntingyard(source_values);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getEquation", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	double FMTcomplexyieldhandler::_getEndPoint(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			const std::map<std::string, double>source_values = _toMap(p_request, SOURCES, SOURCES_DATA);
			const double lowerbound = p_data->data.at(0);
			const double upperbound = p_data->data.at(1);
			const std::vector<std::string const*> ylds = p_data->getSources();
			int peak = -1;
			int lowerpeak = -1;
			const std::unique_ptr<FMTyieldhandler>* ddata;
			const int AGE = p_request.getdevelopment().getage();
			if (source_values.at(*ylds.at(0)) < lowerbound)
			{
				//ddata = srcsdata.at(*ylds.at(0));
				ddata = SOURCES_DATA.at(0);
				peak = (*ddata)->getendpoint(*ylds.at(0), lowerpeak, lowerbound, source_values.at(*ylds.at(0)));
				value = (-getchangesfrom(AGE, peak));
			}
			if (source_values.at(*ylds.at(1)) > upperbound)
			{
				//ddata = srcsdata.at(*ylds.at(0));
				ddata = SOURCES_DATA.at(0);
				lowerpeak = (*ddata)->getendpoint(*ylds.at(0), lowerpeak, lowerbound, std::numeric_limits<double>::lowest());
				//ddata = srcsdata.at(*ylds.at(1));
				ddata = SOURCES_DATA.at(1);
				peak = (*ddata)->getendpoint(*ylds.at(1), lowerpeak, upperbound, source_values.at(*ylds.at(1)));
				value = (-getchangesfrom(AGE, peak));
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getEndPoint", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTcomplexyieldhandler::_getDelta(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
		{
		double value = 0;
		try {
			const FMTdata* C_DATA = &m_elements.at(p_yld);
			const std::vector<const std::string*> SOURCES = C_DATA->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			const std::map<std::string, double>source_values = _toMap(p_request, SOURCES, SOURCES_DATA);
			const int PERIOD = p_request.getdevelopment().getperiod();
			const int periodtolookat = std::max(0, PERIOD + static_cast<int>(C_DATA->data.back()));
			Core::FMTdevelopment newdevelopement(p_request.getdevelopment());
			newdevelopement.setperiod(periodtolookat);
			const FMTyieldrequest newrequest(newdevelopement, p_request);
			const std::map<std::string, double>periodic_source_values = _toMap(newrequest, SOURCES, SOURCES_DATA);
			value = std::abs(source_values.begin()->second - periodic_source_values.begin()->second);
		}catch (...)
			{
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getDelta", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
		}

	double FMTcomplexyieldhandler::_getDistance(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			/*The distance function calculate the distance in age between a minimal yield value and a maximal yield value when you start from age 0,
						if the peak of the curve is reached before finding the maximal yield it will return infinity,so it only works with the yield value
						before the yield peak*/
			const double lower_bound = p_data->data.at(0);
			const double upper_bound = p_data->data.at(1);
			double localvalue = 0;
			int localage = 0;
			double minage = 0.0;
			double maxage = 0.0;
			double dblage = 0.0;
			Core::FMTdevelopment newdevelopement(p_request.getdevelopment());
			newdevelopement.setage(1);
			FMTyieldrequest newrequest(newdevelopement, p_request);
			const std::unique_ptr<FMTyieldhandler>* ddata = SOURCES_DATA.at(0);
			const double peakage = (*ddata)->getpeak(newrequest, *SOURCES.at(0), 0);
			bool gotminage = false;
			bool gotmaxage = false;
			while (localvalue < upper_bound && dblage <= peakage)
			{
				newdevelopement.setage(localage);
				dblage = static_cast<double>(localage);
				const FMTyieldrequest localrequest(newdevelopement, p_request);
				//const std::vector<double>values = getsourcesarray(srcsdata, localrequest, age_only);
				//localvalue = values.at(0);
				localvalue = (*SOURCES_DATA.at(0))->get(*SOURCES.at(0), localrequest);
				if ((minage < dblage) && (localvalue >= lower_bound) && !gotminage)
				{
					minage = dblage;
					gotminage = true;
				}
				else if ((localvalue >= upper_bound) && !gotmaxage)
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
					"Cannot reach distance bounds for " + p_yld + " on development " + std::string(p_request.getdevelopment()), "FMTcomplexyieldhandler::_getDistance", __LINE__, __FILE__);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getDistance", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}

	double FMTcomplexyieldhandler::_getMax(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			double maxValue = 0;
			for (size_t Id = 0; Id < SOURCES.size(); ++Id)
			{
				const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA.at(Id);
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
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getMax", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	double FMTcomplexyieldhandler::_getMin(const FMTdata* p_data, const std::string& p_yld, const FMTyieldrequest& p_request) const
	{
		double value = 0;
		try {
			const std::vector<const std::string*> SOURCES = p_data->getSources();
			const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(p_request, SOURCES, p_yld);
			double minValue = std::numeric_limits<double>::max();
			for (size_t Id = 0; Id < SOURCES.size(); ++Id)
			{
				const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA.at(Id);
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
					"Cannot find minimum for " + p_yld + " on development " + std::string(p_request.getdevelopment()), "FMTcomplexyieldhandler::_getMin", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("On yield " + p_yld, "FMTcomplexyieldhandler::_getMin", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	

	double FMTcomplexyieldhandler::get(const std::string& yld, const FMTyieldrequest& request) const
	{
		double value = 0;
		try {
			if (_cache.inCache(request, yld))
				{
				return _cache.get(request, yld);
				}
			const FMTdata* C_DATA = &m_elements.at(yld);
			std::chrono::time_point<std::chrono::high_resolution_clock>calculationStart;
			if (lookat.empty())
				{
				calculationStart = getclock();
				}
				if (lookat.find(yld) == lookat.end())
					{
						lookat.insert(yld);
					}
					else {
						_exhandler->raise(Exception::FMTexc::FMTinvalid_yield, "Recursivity detected for complexe yield " + yld,
							"FMTcomplexyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
					}
					switch (C_DATA->getop())
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
					const double TIME_TOOK = getduration<std::chrono::milliseconds::period>(calculationStart);
					if (TIME_TOOK>0.05)
						{
						_cache.set(value, request, yld);
						}
				}
				
		}
		catch (...)
		{
			_exhandler->raisefromcatch("at yield " + yld, "FMTcomplexyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}

	int FMTcomplexyieldhandler::getlastbase() const
	{
		return 0;
	}

	const std::map<std::string, FMTdata,cmpYieldString>& FMTcomplexyieldhandler::getdataelements() const
	{
		return m_elements;
	}
	bool FMTcomplexyieldhandler::empty() const
	{
		return m_elements.empty();
	}
	size_t FMTcomplexyieldhandler::size() const
	{
		return m_elements.size();
	}
	FMTdata& FMTcomplexyieldhandler::operator[](const std::string& yldname)
	{
		return m_elements[yldname];
	}
	const FMTdata& FMTcomplexyieldhandler::at(const std::string& yldname) const
	{
		return m_elements.at(yldname);
	}
	bool FMTcomplexyieldhandler::containsyield(const std::string& yldname) const
	{
		return (m_elements.find(yldname) != m_elements.end());
	}

	std::vector<std::string>FMTcomplexyieldhandler::getyieldnames() const
	{
		std::vector<std::string>results;
		results.reserve(m_elements.size());
		for (const auto& data : m_elements)
		{
			results.push_back(data.first);
		}
		return results;
	}

	void FMTcomplexyieldhandler::clearcache()
	{
		for (auto& data : m_elements)
		{
			data.second.clearcache();
		}
	}

	std::unique_ptr<FMTyieldhandler>FMTcomplexyieldhandler::clone() const
	{
		return std::unique_ptr<FMTyieldhandler>(new FMTcomplexyieldhandler(*this));
	}

	FMTyldtype FMTcomplexyieldhandler::gettype() const
	{
		return FMTyldtype::FMTcomplexyld;
	}

	FMTcomplexyieldhandler::FMTcomplexyieldhandler(const FMTmask& mask):
		FMTyieldhandler(mask), m_elements(), overridetabou(), overrideindex(0), _cache()
	{

	}

	FMTcomplexyieldhandler::FMTcomplexyieldhandler() :
		FMTyieldhandler(), m_elements(), overridetabou(), overrideindex(0), _cache()
	{

	}

	std::unique_ptr<FMTyieldhandler> FMTcomplexyieldhandler::complexyldtoageyld(const FMTyieldrequest& request, const FMTspec& lspec) const
	{
		
		try {
			return toageyld(request, lspec.getylds(), 0, request.getdevelopment().getage());
		}
		catch (...) {
			_exhandler->raisefromcatch("Error in converting complexyield to ageyield for yieldhandler " + std::string(*this), "FMTyieldrequest::complexyldtoageyld", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTyieldhandler>();
	}

	double FMTcomplexyieldhandler::getyieldlinearvalue(const std::string& yldname, const FMTyieldrequest& request, bool allowoutofrange) const //should allowoutofrange always false??? to get a 0 at the end of the curve
	{
		double returned = 0;
		try {
			std::vector<std::string>target(1, yldname);
			const std::unique_ptr<FMTyieldhandler> ageyield =  toageyld(request, target, 0, request.getdevelopment().getage());
			returned = ageyield->getyieldlinearvalue(yldname, request);
		}
		catch (...) {
			_exhandler->raisefromcatch("On yield " + yldname, "FMTcomplexyieldhandler::getyieldlinearvalue", __LINE__, __FILE__);
		}
		return returned;
	}

	double FMTcomplexyieldhandler::getpeak(const FMTyieldrequest& request, const std::string& yld, const int& targetage) const
	{
		try {
			std::vector<std::string>ylds(1, yld);
			const int maxbase = getmaxbase(request);
			return toageyld(request, ylds, 0, maxbase)->getpeak(request,yld,targetage);
		}catch (...) {
			_exhandler->raisefromcatch("", "FMTcomplexyieldhandler::getpeak", __LINE__, __FILE__);
		}
		return 0;
	}

	int FMTcomplexyieldhandler::getage(const FMTyieldrequest& request, const FMTspec& spec) const
	{
		try {
			std::unique_ptr<FMTyieldhandler>ageyield = complexyldtoageyld(request, spec);
			return ageyield->getage(request, spec);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTageyieldhandler::getage", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}


}

