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
				const bool BASE_CASE = (inlookat(*p_names[YldId]) || (p_original == *p_names[YldId]));
				size_t dataId = 0;
				bool foundValue = false;
				FMTyieldrequest::const_iterator FIRST_SEEN = p_request.getFirstSeen(*p_names[YldId]);
				while (!foundValue && dataId < FULL_DATA.size())
						{
						const std::unique_ptr<FMTyieldhandler>* YIELD = &FULL_DATA[dataId]->second;
						if (!NEED_TO_TEST_OVERRIDE ||
							overridetabou.find((*YIELD)->getoverrideindex()) == overridetabou.end())
							{
							if (FULL_DATA[dataId] >= FIRST_SEEN &&
								(*YIELD)->containsyield(*p_names[YldId]) &&
								!(this == &(**YIELD) && BASE_CASE))
								{
									data[YldId] = YIELD;
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
			const std::unique_ptr<FMTyieldhandler>* DATA = p_data[Id];
			const std::string* YIELD_NAME = p_names[Id];
			std::pair<std::map<std::string, double>::iterator,bool> newValue = result.insert(std::pair<std::string, double>(*YIELD_NAME, 0.0));
			if (DATA != nullptr)
			{
				const double VALUE = (*DATA)->get(newValue.first->first, p_request);
				newValue.first->second = VALUE;
			}
		}
		return result;
	}

	

	double FMTcomplexyieldhandler::get(const std::string& yld, const FMTyieldrequest& request) const
	{
		double value = 0;
		try {
			if (_cache.inCache(request,yld))
				{
				return _cache.get(request, yld);
				}
			const FMTdata* cdata = &m_elements.at(yld);
			std::chrono::time_point<std::chrono::high_resolution_clock>calculationStart;
			if (lookat.empty())
				{
				calculationStart = getclock();
				}
				//bool age_only = true;
				const std::vector<const std::string*> SOURCES = cdata->getSources();
				const std::vector<const std::unique_ptr<FMTyieldhandler>*> SOURCES_DATA = _getData(request, SOURCES, yld);
				//const std::map<std::string, const std::unique_ptr<FMTyieldhandler>*> srcsdata = this->getdata(request, sources, yld);
				
				if (lookat.find(yld) == lookat.end())
					{
						lookat.insert(yld);
					}
					else {
						_exhandler->raise(Exception::FMTexc::FMTinvalid_yield, "Recursivity detected for complexe yield " + yld,
							"FMTcomplexyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
					}
					switch (cdata->getop())
					{
					case FMTyieldparserop::FMTrange:
					{
						value = 1;
						size_t SourceId = 0;
						for (size_t Id = 0; Id < SOURCES.size(); ++Id)
						{
							const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA[Id];
							double yieldValue = 0.0;
							if (DATA!=nullptr)
								{
								const std::string* YIELD_RANGE = SOURCES[Id];
								yieldValue = (*DATA)->get(*YIELD_RANGE, request);
								}
							if (yieldValue < cdata->data.at(SourceId) ||
								yieldValue > cdata->data.at(SourceId + 1))
							{
								value = 0;
								break;
							}
							SourceId += 2;
						}


						break;
					}
					case FMTyieldparserop::FMTmultiply:
					{
						value = 1.0;
						for (size_t Id = 0; Id < SOURCES.size(); ++Id)
						{
							const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA[Id];
							double yieldValue = 0.0;
							if (DATA != nullptr)
							{
								const std::string* YIELD_NAME = SOURCES[Id];
								yieldValue = (*DATA)->get(*YIELD_NAME, request);
							}
							value *= yieldValue;
						}
						for (const double& vecValue : cdata->data)
						{
							value *= vecValue;
						}
						break;
					}
					case FMTyieldparserop::FMTsum:
					{
						for (size_t Id = 0; Id < SOURCES.size(); ++Id)
						{
							const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA[Id];
							double yieldValue = 0.0;
							if (DATA != nullptr)
							{
								const std::string* YIELD_NAME = SOURCES[Id];
								yieldValue = (*DATA)->get(*YIELD_NAME, request);
							}
							value += yieldValue;
						}
						for (const double& vecvalue : cdata->data)
							{
							value += vecvalue;
							}
						break;
					}
					case FMTyieldparserop::FMTsubstract:
					{
						std::vector<const double*>VALUES = cdata->getValues();
						size_t Id = 0;
						for (size_t valueId = 0; valueId < VALUES.size(); ++valueId)
							{
								double theValue = 0;
								if (VALUES[valueId] != nullptr)
								{
									theValue = *VALUES[valueId];
								}
								else {
									const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA[Id];
									if (DATA != nullptr)
									{
										const std::string* YIELD_NAME = SOURCES[Id];
										theValue = (*DATA)->get(*YIELD_NAME, request);
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
						break;
					}
					case FMTyieldparserop::FMTdivide:
					{
						std::vector<const double*>VALUES = cdata->getValues();
						size_t Id = 0;
						for (size_t valueId = 0; valueId < VALUES.size(); ++valueId)
						{
							double theValue = 0;
							if (VALUES[valueId] != nullptr)
							{
								theValue = *VALUES[valueId];
							}
							else {
								const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA[Id];
								if (DATA != nullptr)
								{
									const std::string* YIELD_NAME = SOURCES[Id];
									theValue = (*DATA)->get(*YIELD_NAME, request);
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
						break;
					}
					case FMTyieldparserop::FMTytp:
					{
						const int AGE = request.getdevelopment().getage();
						const std::unique_ptr<FMTyieldhandler>* ddata = SOURCES_DATA[0];
						value = (*ddata)->getpeak(request, *SOURCES[0], AGE);
						break;
					}
					case FMTyieldparserop::FMTmai:
					{
						double year = 1;
						if (cdata->data.begin() != cdata->data.end())
						{
							year = *cdata->data.begin();
						}
						const std::unique_ptr<FMTyieldhandler>* ddata = SOURCES_DATA[0];
						const int AGE = request.getdevelopment().getage();
						value = ((*ddata)->getyieldlinearvalue(*SOURCES.at(0), request,false) / (year * AGE));
						break;
					}
					case FMTyieldparserop::FMTcai:
					{
						double year = 1;
						if (cdata->data.begin() != cdata->data.end())
						{
							year = *cdata->data.begin();
						}
						const std::unique_ptr<FMTyieldhandler>* ddata = SOURCES_DATA[0];
						//const double upval = (*ddata)->getyieldlinearvalue(sources.at(0), age);
						const double upval = (*ddata)->getyieldlinearvalue(*SOURCES.at(0), request);
						const int AGE = request.getdevelopment().getage();
						const int newage = AGE - 1;
						Core::FMTdevelopment newdevelopement(request.getdevelopment());
						newdevelopement.setage(newage);
						const FMTyieldrequest newrequest(newdevelopement, request);
						//const double dwval = (*ddata)->getyieldlinearvalue(sources.at(0), newage);
						const double dwval = (*ddata)->getyieldlinearvalue(*SOURCES.at(0), newrequest);
						value = ((upval - dwval) / (year));
						break;
					}
					case FMTyieldparserop::FMTequation:
					{
						const FMTexpression expression = cdata->toexpression();
						try {
							const std::map<std::string, double>source_values = _toMap(request,SOURCES, SOURCES_DATA);
							value = expression.shuntingyard(source_values);
						}
						catch (...)
						{
							std::string baseeq;
							for (const std::string& opstr : expression.getinfix())
							{
								baseeq += opstr + " ";
							}
							_exhandler->raisefromcatch("For complex equation " + baseeq, "FMTcomplexyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
						}
						break;
					}
					case FMTyieldparserop::FMTendpoint:
					{
						value = 0;
						const std::map<std::string, double>source_values = _toMap(request, SOURCES, SOURCES_DATA);
						const double lowerbound = cdata->data.at(0);
						const double upperbound = cdata->data.at(1);
						const std::vector<std::string const*> ylds = cdata->getSources();
						int peak = -1;
						int lowerpeak = -1;
						const std::unique_ptr<FMTyieldhandler>* ddata;
						const int AGE = request.getdevelopment().getage();
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
						break;
					}
					case FMTyieldparserop::FMTdelta:
					{
						const std::map<std::string, double>source_values = _toMap(request, SOURCES, SOURCES_DATA);
						const int PERIOD = request.getdevelopment().getperiod();
						const int periodtolookat = std::max(0, PERIOD + static_cast<int>(cdata->data.back()));
						Core::FMTdevelopment newdevelopement(request.getdevelopment());
						newdevelopement.setperiod(periodtolookat);
						const FMTyieldrequest newrequest(newdevelopement,request);
						const std::map<std::string, double>periodic_source_values = _toMap(newrequest, SOURCES, SOURCES_DATA);
						value = std::abs(source_values.begin()->second - periodic_source_values.begin()->second);
						break;
					}
					case FMTyieldparserop::FMTdistance:
					{
						/*The distance function calculate the distance in age between a minimal yield value and a maximal yield value when you start from age 0,
						if the peak of the curve is reached before finding the maximal yield it will return infinity,so it only works with the yield value
						before the yield peak*/
						const double lower_bound = cdata->data.at(0);
						const double upper_bound = cdata->data.at(1);
						double localvalue = 0;
						int localage = 0;
						double minage = 0.0;
						double maxage = 0.0;
						double dblage = 0.0;
						Core::FMTdevelopment newdevelopement(request.getdevelopment());
						newdevelopement.setage(1);
						FMTyieldrequest newrequest(newdevelopement, request);
						const std::unique_ptr<FMTyieldhandler>* ddata = SOURCES_DATA[0];
						const double peakage = (*ddata)->getpeak(newrequest, *SOURCES[0], 0);
						bool gotminage = false;
						bool gotmaxage = false;
						while (localvalue < upper_bound && dblage <= peakage)
							{
							newdevelopement.setage(localage);
							dblage = static_cast<double>(localage);
							const FMTyieldrequest localrequest(newdevelopement, request);
							//const std::vector<double>values = getsourcesarray(srcsdata, localrequest, age_only);
							//localvalue = values.at(0);
							localvalue = (*SOURCES_DATA[0])->get(*SOURCES[0], localrequest);
							if ((minage< dblage) && (localvalue >= lower_bound) && !gotminage)
								{
								minage = dblage;
								gotminage = true;
								}else if ((localvalue >= upper_bound) && !gotmaxage)
									{
									maxage = dblage;
									gotmaxage = true;
									}
							++localage;
							}
						value = maxage - minage;
						if (value < 0 ||!(gotmaxage && gotminage))
							{
							value = 0;
							_exhandler->raise(Exception::FMTexc::FMTignore,
								"Cannot reach distance bounds for " +yld +" on development "+std::string(request.getdevelopment()), "FMTyieldhandler::get", __LINE__, __FILE__);
							}
						break;
					}
					case FMTyieldparserop::FMTmax:
					{
						double maxValue = 0;
						for (size_t Id = 0; Id < SOURCES.size(); ++Id)
							{
								const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA[Id];
								const std::string* YIELD_NAME = SOURCES[Id];
								if (DATA != nullptr)
								{
									const double VALUE = (*DATA)->get(*YIELD_NAME, request);
									if (VALUE > maxValue)
										{
										maxValue = VALUE;
										}
								}
							}
						value = maxValue;
						break;
					}
					case FMTyieldparserop::FMTmin:
					{
						double minValue = std::numeric_limits<double>::max();
						for (size_t Id = 0; Id < SOURCES.size(); ++Id)
						{
							const std::unique_ptr<FMTyieldhandler>* DATA = SOURCES_DATA[Id];
							const std::string* YIELD_NAME = SOURCES[Id];
							if (DATA != nullptr)
							{
								const double VALUE = (*DATA)->get(*YIELD_NAME, request);
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
								"Cannot find minimum for " + yld + " on development " + std::string(request.getdevelopment()), "FMTyieldhandler::get", __LINE__, __FILE__);
						}
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

					
					//_cache.reserve(request);
					
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

