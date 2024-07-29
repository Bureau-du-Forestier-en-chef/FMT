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
			for (std::map<std::string, FMTdata>::const_iterator it = elements.begin(); it != elements.end(); ++it)
				{
				YieldsNames.push_back(it->first);
				}
			std::sort(YieldsNames.begin(), YieldsNames.end());
			for (const std::string& Name : YieldsNames)
				{
				value += Name + " " + std::string(elements.at(Name)) + "\n";
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
		return (basepush_data(elements, yld, value));
	}

	bool FMTcomplexyieldhandler::push_data(const std::string& yld, const FMTdata& data)
	{
		return (basepush_data(elements, yld, data));
	}

	std::vector<std::string> FMTcomplexyieldhandler::indexes(const std::vector<std::string>& names) const
	{
		std::vector<std::string>indexs;
		try {
				for (std::map<std::string, FMTdata>::const_iterator data_it = elements.begin(); data_it != elements.end(); data_it++)
				{
					if (data_it->second.getop() == FMTyieldparserop::FMTequation)
					{
						const std::vector<std::string>variables = data_it->second.getsource();
						for (const std::string& variable : variables)
						{
							if (!variable.empty() && std::find(names.begin(), names.end(), variable) == names.end() &&
								!FMTfunctioncall(variable).valid() &&
								!FMToperator(variable).valid() &&
								(variable != ")" && variable != "("))
							{
								indexs.push_back(variable);
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
			elements == rhs.elements);
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

	bool FMTcomplexyieldhandler::comparesources(const std::string& yield, const FMTcomplexyieldhandler& overridedyield) const
	{
		try {
				for (const auto& data : elements)
				{
				const std::vector<std::string>sources = data.second.getsource();
				if (std::find(sources.begin(), sources.end(), yield)!=sources.end())
					{
					return false;
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


	std::map<std::string, const std::unique_ptr<FMTyieldhandler>*> FMTcomplexyieldhandler::getdata(const FMTyieldrequest& request,
		const std::vector<std::string>& names, const std::string& original) const
	{
		std::map<std::string, const std::unique_ptr<FMTyieldhandler>*>alldata;
		try {
			for (const std::unique_ptr<FMTyieldhandler>* yield : request.getdatas())
			{
				if (overridetabou.find((*yield)->getoverrideindex())== overridetabou.end())
				{
					
					for (const std::string& name : names)
					{
						if ((*yield)->containsyield(name) && alldata.find(name) == alldata.end() &&
							!(this == &(**yield) && original == name) && (!inlookat(name)))
						{
							alldata[name] = yield;
						}
						if (alldata.size() == names.size())
						{
							return alldata;
						}
					}
				}
				
				
				
			}
			if (alldata.size() != names.size())
			{
				for (const std::string& name : names)
				{
					if (!name.empty() && alldata.find(name) == alldata.end())
					{
						
						alldata[name] = nullptr;
						/*_exhandler->raise(Exception::FMTexc::FMTignore,
							"Missing requested yield "+name, "FMTyieldhandler::getdata", __LINE__, __FILE__);*/
					}

				}

			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTcomplexyieldhandler::getdata", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return alldata;

	}


	double FMTcomplexyieldhandler::get(const std::string& yld, const FMTyieldrequest& request) const
	{
		double value = 0;
		try {
			const int age = request.getdevelopment().getage();
			const int period = request.getdevelopment().getperiod();

				const FMTdata* cdata = &elements.at(yld);
				if (cdata->cachevalue(request))
				{
					return cdata->get(request);
				}
				bool age_only = true;
				const std::vector<std::string> sources = cdata->getsource();
				const std::map<std::string, const std::unique_ptr<FMTyieldhandler>*> srcsdata = this->getdata(request, sources, yld);
				
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
						size_t srcid = 0;
						value = 1;
						const std::map<std::string, double>source_values = this->getsources(srcsdata, request, age_only);
						for (const std::string& yldrange : sources)
						{
							const double lower = cdata->data.at(srcid);
							const double upper = cdata->data.at(srcid + 1);
							if ((source_values.find(yldrange) == source_values.end()) || (source_values.at(yldrange) < lower || source_values.at(yldrange) > upper))
							{
								value = 0;
								break;
							}
							srcid += 2;
						}
						break;
					}
					case FMTyieldparserop::FMTmultiply:
					{
						value = 1;
						//const std::map<std::string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
						for (const double& sourcevalue : getsourcesarray(srcsdata, request, age_only))
						{
							value *= sourcevalue;
						}
						for (const double& vecvalue : cdata->data)
						{
							value *= vecvalue;
						}
						break;
					}
					case FMTyieldparserop::FMTsum:
					{
						//const std::map<std::string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
						for (const double& sourcevalue : getsourcesarray(srcsdata, request, age_only))
						{
							value += sourcevalue;
						}
						for (const double& vecvalue : cdata->data)
						{
							value += vecvalue;
						}
						break;
					}
					case FMTyieldparserop::FMTsubstract:
					{
						//ordering means something here!!!!
						const std::map<std::string, double>source_values = this->getsources(srcsdata, request, age_only);
						std::vector<double>values = cdata->tovalues(source_values);
						value = values.front();
						values.erase(values.begin());
						for (const double& yldvalue : values)
						{
							value -= yldvalue;
						}

						break;
					}
					case FMTyieldparserop::FMTdivide:
					{
						const std::map<std::string, double>source_values = this->getsources(srcsdata, request, age_only);
						std::vector<double>values = cdata->tovalues(source_values);
						value = values.front();
						values.erase(values.begin());
						for (const double& yldvalue : values)
						{
							if (yldvalue != 0)
							{
								value /= yldvalue;
							}
							else {
								value = 0;
							}
						}
						break;
					}
					case FMTyieldparserop::FMTytp:
					{
						const std::unique_ptr<FMTyieldhandler>* ddata = srcsdata.begin()->second;
						value = (*ddata)->getpeak(request,srcsdata.begin()->first, age);
						break;
					}
					case FMTyieldparserop::FMTmai:
					{
						double year = 1;
						if (cdata->data.begin() != cdata->data.end())
						{
							year = *cdata->data.begin();
						}
						const std::unique_ptr<FMTyieldhandler>* ddata = srcsdata.begin()->second;
						//value = ((*ddata)->getyieldlinearvalue(sources.at(0), age) / (year*age));
						value = ((*ddata)->getyieldlinearvalue(sources.at(0), request,false) / (year * age));
						break;
					}
					case FMTyieldparserop::FMTcai:
					{
						double year = 1;
						if (cdata->data.begin() != cdata->data.end())
						{
							year = *cdata->data.begin();
						}
						const std::unique_ptr<FMTyieldhandler>* ddata = srcsdata.begin()->second;
						//const double upval = (*ddata)->getyieldlinearvalue(sources.at(0), age);
						const double upval = (*ddata)->getyieldlinearvalue(sources.at(0), request);
						const int newage = age - 1;
						Core::FMTdevelopment newdevelopement(request.getdevelopment());
						newdevelopement.setage(newage);
						const FMTyieldrequest newrequest(newdevelopement, request);
						//const double dwval = (*ddata)->getyieldlinearvalue(sources.at(0), newage);
						const double dwval = (*ddata)->getyieldlinearvalue(sources.at(0), newrequest);
						value = ((upval - dwval) / (year));
						break;
					}
					case FMTyieldparserop::FMTequation:
					{
						const FMTexpression expression = cdata->toexpression();
						try {
							const std::map<std::string, double>source_values = this->getsources(srcsdata, request, age_only);
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
						const std::map<std::string, double>source_values = this->getsources(srcsdata, request, age_only);
						const double lowerbound = cdata->data.at(0);
						const double upperbound = cdata->data.at(1);
						const std::vector<std::string> ylds = cdata->getsource();
						int peak = -1;
						int lowerpeak = -1;
						const std::unique_ptr<FMTyieldhandler>* ddata;
						if (source_values.at(ylds.at(0)) < lowerbound)
						{
							ddata = srcsdata.at(ylds.at(0));
							peak = (*ddata)->getendpoint(ylds.at(0), lowerpeak, lowerbound, source_values.at(ylds.at(0)));
							value = (-getchangesfrom(age, peak));
						}
						if (source_values.at(ylds.at(1)) > upperbound)
						{
							ddata = srcsdata.at(ylds.at(0));
							lowerpeak = (*ddata)->getendpoint(ylds.at(0), lowerpeak, lowerbound, std::numeric_limits<double>::lowest());
							ddata = srcsdata.at(ylds.at(1));
							peak = (*ddata)->getendpoint(ylds.at(1), lowerpeak, upperbound, source_values.at(ylds.at(1)));
							value = (-getchangesfrom(age, peak));
						}
						break;
					}
					case FMTyieldparserop::FMTdelta:
					{
						const std::map<std::string, double>source_values = this->getsources(srcsdata, request, age_only);
						const int periodtolookat = std::max(0, period + static_cast<int>(cdata->data.back()));
						Core::FMTdevelopment newdevelopement(request.getdevelopment());
						newdevelopement.setperiod(periodtolookat);
						const FMTyieldrequest newrequest(newdevelopement,request);
						const std::map<std::string, double>periodic_source_values = this->getsources(srcsdata, newrequest, age_only);
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
						const std::unique_ptr<FMTyieldhandler>* ddata = srcsdata.begin()->second;
						const double peakage = (*ddata)->getpeak(newrequest, srcsdata.begin()->first, 0);
						bool gotminage = false;
						bool gotmaxage = false;
						while (localvalue < upper_bound && dblage <= peakage)
							{
							newdevelopement.setage(localage);
							dblage = static_cast<double>(localage);
							const FMTyieldrequest localrequest(newdevelopement, request);
							const std::vector<double>values = getsourcesarray(srcsdata, localrequest, age_only);
							localvalue = values.at(0);
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
						double maxvalue = 0;
						for (const double& sourcevalue : getsourcesarray(srcsdata, request, age_only))
							{
							if (sourcevalue > maxvalue)
								{
								maxvalue = sourcevalue;
								}
							}
						value = maxvalue;
						break;
					}
					case FMTyieldparserop::FMTmin:
					{
						double minvalue = std::numeric_limits<double>::max();
						for (const double& sourcevalue : getsourcesarray(srcsdata, request, age_only))
						{
							if (sourcevalue < minvalue)
							{
								minvalue = sourcevalue;
							}
						}
						if (minvalue!= std::numeric_limits<double>::max())
						{
							value = minvalue;
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
				cdata->set(value, request, age_only);
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

	const std::map<std::string, FMTdata>& FMTcomplexyieldhandler::getdataelements() const
	{
		return elements;
	}
	bool FMTcomplexyieldhandler::empty() const
	{
		return elements.empty();
	}
	size_t FMTcomplexyieldhandler::size() const
	{
		return elements.size();
	}
	FMTdata& FMTcomplexyieldhandler::operator[](const std::string& yldname)
	{
		return elements[yldname];
	}
	const FMTdata& FMTcomplexyieldhandler::at(const std::string& yldname) const
	{
		return elements.at(yldname);
	}
	bool FMTcomplexyieldhandler::containsyield(const std::string& yldname) const
	{
		return (elements.find(yldname) != elements.end());
	}

	std::vector<std::string>FMTcomplexyieldhandler::getyieldnames() const
	{
		std::vector<std::string>results;
		results.reserve(elements.size());
		for (const auto& data : elements)
		{
			results.push_back(data.first);
		}
		return results;
	}

	void FMTcomplexyieldhandler::clearcache()
	{
		for (auto& data : elements)
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
		FMTyieldhandler(mask), elements(), overridetabou(), overrideindex(0)
	{

	}

	FMTcomplexyieldhandler::FMTcomplexyieldhandler() :
		FMTyieldhandler(), elements(), overridetabou(), overrideindex(0)
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

