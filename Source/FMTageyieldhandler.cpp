/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTageyieldhandler.h"
#include "FMTyieldrequest.h"
#include "FMTdevelopment.h"
#include "FMTdata.h"
#include "FMTexceptionhandler.h"

namespace Core {


	FMTAgeYieldHandler::operator std::string() const
	{
		std::string value;
		try {
				value += "*Y " + std::string(mask) + "\n";
				value += "_AGE \t";
				const std::vector<std::string> yieldNames = getYieldNames();
				for (const std::string& NAME : yieldNames)
				{
					value += NAME + "\t";
				}
				value += "\n";
				int baseid = 0;
				for (const int& base : bases)
				{
					value += std::to_string(base) + "\t";
					for (const std::string& NAME : yieldNames)
					{
						const std::vector<double>* data = &m_elements.at(NAME).data;
						value += std::to_string(data->at(baseid)) + "\t";
					}
					value += "\n";
					++baseid;
				}
		
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}


	bool FMTAgeYieldHandler::pushData(const std::string& yld, const double& value)
	{
		return (basePushData(m_elements, yld, value));
	}

	bool FMTAgeYieldHandler::pushData(const std::string& yld, const FMTData& data)
	{
		return (basePushData(m_elements, yld,data));
	}

	FMTAgeYieldHandler::FMTAgeYieldHandler(const FMTMask& mask) :
		FMTYieldHandler(mask)
	{

	}

	
	bool FMTAgeYieldHandler::operator == (const FMTAgeYieldHandler& rhs) const
	{
		return (FMTYieldHandler::operator==(rhs)&&
			m_elements==rhs.m_elements);
	}

	double FMTAgeYieldHandler::get(const std::string& yld, const FMTYieldRequest& request) const
	{
		try {
				try {
					const int target = request.getDevelopment().getAge();
					std::map<std::string, Core::FMTData, cmpYieldString>::const_iterator data0it = m_elements.find(yld);
					if (data0it != m_elements.end())
						{
						return getLinearValue(data0it->second.data, target,true);
						}
				}catch (...)
					{
					_exhandler->raiseFromCatch("Getting age yield", "FMTAgeYieldHandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
					}
		}catch (...)
			{
			_exhandler->raiseFromCatch("at yield " + yld, "FMTAgeYieldHandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
	return 0;
	}

	FMTAgeYieldHandler::FMTAgeYieldHandler(const FMTYieldHandler& rhs):
		FMTYieldHandler(rhs)
		{
	
		}


	double FMTAgeYieldHandler::getLastValue(const std::string yld) const
	{
		std::map<std::string, FMTData, cmpYieldString>::const_iterator it = m_elements.find(yld);
		return it->second.data.back();
	}
	int FMTAgeYieldHandler::getAge(const std::string yld, const double& value, const int& starting_age) const
	{
		int age = 0;
		try {
			std::map<std::string, FMTData, cmpYieldString>::const_iterator it = m_elements.find(yld);
			if (it != m_elements.end())
			{
				const FMTData* ldata = &it->second;
				//std::vector<double>::const_iterator dit = ldata->data.begin();
				double minimal_gap = std::numeric_limits<double>::infinity();
				size_t minimal_gap_index = 0;
				size_t index = 0;
				for (const double& vdata : ldata->data)
				{
					if (abs(vdata - value) <= minimal_gap && index > 0 && bases.at(index) <= starting_age)
					{
						minimal_gap = abs(vdata - value);
						minimal_gap_index = index;
					}
					++index;
				}
				age = std::max(bases.at(minimal_gap_index), 1);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::getAge", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return age;
	}

	double FMTAgeYieldHandler::getYieldLinearValue(const std::string&yldname, const FMTYieldRequest& request, bool allowoutofrange) const
	{
		try {
			const FMTData& lvalues = this->at(yldname);
			return getLinearValue(lvalues.data, request.getDevelopment().getAge(),allowoutofrange);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::getYieldLinearValue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}

	int FMTAgeYieldHandler::getAge(const FMTYieldRequest& request, const FMTSpec& spec) const
	{
		try {
			const std::vector<std::string>&yieldnames = spec.getYlds();
			const std::vector<FMTYldBounds>&yieldbounds = spec.getYldBounds();
			int age = request.getDevelopment().getAge();
			for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				if (containsYield(yieldnames.at(id)))
				{
					const FMTYldBounds* bound = &yieldbounds.at(id);
					const int new_age = getAge(yieldnames.at(id), bound->getLower(), request.getDevelopment().getAge());
					if (new_age < age)
					{
						age = new_age;
					}
					return age;
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::getAge", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}

	std::unique_ptr<FMTYieldHandler> FMTAgeYieldHandler::getFromFactor(const double& factor,
		std::vector<std::string>yieldnames) const
	{
		FMTAgeYieldHandler newhandler(*this);
		try {
				std::vector<std::map<std::string, FMTData, cmpYieldString>::iterator>iterators;
				if (yieldnames.empty())
				{
					for (std::map<std::string, FMTData, cmpYieldString>::iterator it = newhandler.m_elements.begin();
						it != newhandler.m_elements.end(); it++)
					{
						iterators.push_back(it);
					}
				}
				else {
					for (const std::string& yldname : yieldnames)
					{
						std::map<std::string, FMTData, cmpYieldString>::iterator it = newhandler.m_elements.find(yldname);
						if (it != newhandler.m_elements.end())
						{
							iterators.push_back(it);
						}
					}
				}
				for (std::map<std::string, FMTData, cmpYieldString>::iterator datait : iterators)
				{
					datait->second = ((datait->second)*factor);
				}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::getFromFactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return newhandler.clone();
	}

	bool FMTAgeYieldHandler::empty() const
	{
		return m_elements.empty();
	}
	size_t FMTAgeYieldHandler::size() const
	{
		return m_elements.size();
	}
	FMTData& FMTAgeYieldHandler::operator[](const std::string& yldname)
	{
		return m_elements[yldname];
	}
	const FMTData& FMTAgeYieldHandler::at(const std::string& yldname) const
	{
		return m_elements.at(yldname);
	}
	bool FMTAgeYieldHandler::containsYield(const std::string& yldname) const
	{
		return (m_elements.find(yldname) != m_elements.end());
	}


	std::vector<std::string>FMTAgeYieldHandler::getYieldNames() const
	{
		std::vector<std::string>results;
		results.reserve(m_elements.size());
		for (const auto& data : m_elements)
		{
			results.push_back(data.first);
		}
		std::sort(results.begin(), results.end());
		return results;
	}

	void FMTAgeYieldHandler::clearCache()
	{
		
	}

	std::unique_ptr<FMTYieldHandler>FMTAgeYieldHandler::clone() const
		{
		return std::unique_ptr<FMTYieldHandler>(new FMTAgeYieldHandler(*this));
		}

	FMTyldtype FMTAgeYieldHandler::getType() const
		{
		return FMTyldtype::FMTageyld;
		}

	std::map<std::string, std::vector<double>>FMTAgeYieldHandler::getAllYieldsData(const int& maxbase)const
	{
		std::map<std::string, std::vector<double>>localstuff;
		try {
			const int lastbase = getLastBase();
			std::vector<int>bases = getBases();
			for (std::map<std::string, FMTData, cmpYieldString>::const_iterator cit = m_elements.begin(); cit != m_elements.end(); cit++)
			{
				localstuff[cit->first] = std::vector<double>();
				for (int base = 0; base <= maxbase; ++base)
				{
					std::vector<int>::const_iterator baseit = std::find(bases.begin(), bases.end(), base);
					if (baseit != bases.end())
					{
						size_t index = std::distance<std::vector<int>::const_iterator>(bases.begin(), baseit);
						localstuff[cit->first].push_back(cit->second.data.at(index));
					}
					else if (base < lastbase)
					{
						localstuff[cit->first].push_back(0);
					}
					else {
						localstuff[cit->first].push_back(cit->second.data.back());
					}
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::getAllYieldsData", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return localstuff;
	}

	int FMTAgeYieldHandler::getEndPoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const
	{
		size_t locid = 0;
		try {
				std::map<std::string, FMTData, cmpYieldString>::const_iterator it = m_elements.find(yld);
				std::vector<double>::const_iterator location;
				if (value < bound)
				{
					location = std::lower_bound(it->second.data.begin(), it->second.data.end(), bound);
				}
				else if (value > bound)
				{
					std::vector<double>::const_iterator startinglocation = it->second.data.begin() + lowerstep;
					location = std::upper_bound(startinglocation, it->second.data.end(), bound);
				}
				locid = std::distance(it->second.data.begin(), location);
				locid = std::min(locid, (it->second.data.size() - 1));
				locid = std::max(size_t(0), locid);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::getEndPoint", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return static_cast<int>(locid);
	}

	double FMTAgeYieldHandler::getPeak(const FMTYieldRequest& request,const std::string& yld, const int& targetage) const
	{
		double value = 0;
		try {
			const int peak = static_cast<int>(getPeakfrom(yld));
			value = getChangesFrom(targetage, peak);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::getPeak", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}



	double FMTAgeYieldHandler::getPeakfrom(const std::string& yld, double maxvalue) const
	{
		std::map<std::string, FMTData, cmpYieldString>::const_iterator it = m_elements.find(yld);
		int location = 0;
		int peak = -1;
		try {
			std::vector<double>::const_iterator dblit = it->second.data.begin();
			while (dblit != it->second.data.end())
			{
				if (*dblit > maxvalue)
				{
					maxvalue = *dblit;
					peak = location;
				}
				++location;
				++dblit;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::getPeakfrom", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return peak;
	}

	void FMTAgeYieldHandler::setYieldValues(const std::string& yldname,const std::vector<int>& baseages,const std::vector<double>& values)
	{
		try{
			if(values.size()!=baseages.size())
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Vector of baseages and values are not the same size.",
				"FMTAgeYieldHandler::setValues", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
			if(this->getBases().empty())
			{
				this->setBase(baseages);
			}else if(this->getBases() != baseages)
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Vector of baseages and values of bases already set are different. \nYou must create a new FMTageyieldhandler for those values",
				"FMTAgeYieldHandler::setYieldValues", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
			for(const double& value : values)
			{
				this->pushData(yldname,value);
			}
		}catch(...){
			_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::setYieldValues", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}

}

