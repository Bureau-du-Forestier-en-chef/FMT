/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTageyieldhandler.hpp"
#include "FMTyieldrequest.hpp"
#include "FMTdevelopment.hpp"
#include "FMTdata.hpp"

namespace Core {

	FMTageyieldhandler::operator std::string() const
	{
		std::string value;
		try {
				value += "*Y " + std::string(mask) + "\n";
				value += "_AGE \t";
				for (std::map<std::string, FMTdata>::const_iterator it = elements.begin(); it != elements.end(); ++it)
				{
					value += it->first + "\t";
				}
				value += "\n";
				int baseid = 0;
				for (const int& base : bases)
				{
					value += std::to_string(base) + "\t";
					for (std::map<std::string, FMTdata>::const_iterator it = elements.begin(); it != elements.end(); ++it)
					{
						const std::vector<double>* data = &it->second.data;
						value += std::to_string(data->at(baseid)) + "\t";
					}
					value += "\n";
					++baseid;
				}
		
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTageyieldhandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}


	bool FMTageyieldhandler::push_data(const std::string& yld, const double& value)
	{
		return (basepush_data(elements, yld, value));
	}

	bool FMTageyieldhandler::push_data(const std::string& yld, const FMTdata& data)
	{
		return (basepush_data(elements, yld,data));
	}

	FMTageyieldhandler::FMTageyieldhandler(const FMTmask& mask) :
		FMTyieldhandler(mask)
	{

	}

	
	bool FMTageyieldhandler::operator == (const FMTageyieldhandler& rhs) const
	{
		return (FMTyieldhandler::operator==(rhs)&&
				elements==rhs.elements);
	}

	double FMTageyieldhandler::get(const std::string& yld, const FMTyieldrequest& request) const
	{
		try {
				try {
					const int target = request.getdevelopment().getage();
					std::map<std::string, Core::FMTdata>::const_iterator data0it = elements.find(yld);
					if (data0it != elements.end())
						{
						return getlinearvalue(data0it->second.data, target);
						}
				}catch (...)
					{
					_exhandler->raisefromcatch("Getting age yield", "FMTageyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
					}
		}catch (...)
			{
			_exhandler->raisefromcatch("at yield " + yld, "FMTageyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
	return 0;
	}

	FMTageyieldhandler::FMTageyieldhandler(const FMTyieldhandler& rhs):
		FMTyieldhandler(rhs)
		{
	
		}


	double FMTageyieldhandler::getlastvalue(const std::string yld) const
	{
		std::map<std::string, FMTdata>::const_iterator it = elements.find(yld);
		return it->second.data.back();
	}
	int FMTageyieldhandler::getage(const std::string yld, const double& value, const int& starting_age) const
	{
		int age = 0;
		try {
			std::map<std::string, FMTdata>::const_iterator it = elements.find(yld);
			if (it != elements.end())
			{
				const FMTdata* ldata = &it->second;
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
			_exhandler->raisefromcatch("", "FMTageyieldhandler::getage", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return age;
	}

	double FMTageyieldhandler::getyieldlinearvalue(const std::string&yldname, const int& agetarget) const
	{
		try {
			const FMTdata& lvalues = this->at(yldname);
			return getlinearvalue(lvalues.data, agetarget);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTageyieldhandler::getyieldlinearvalue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}

	int FMTageyieldhandler::getage(const FMTyieldrequest& request, const FMTspec& spec) const
	{
		try {
			const std::vector<std::string>&yieldnames = spec.getylds();
			const std::vector<FMTyldbounds>&yieldbounds = spec.getyldbounds();
			int age = request.getdevelopment().getage();
			for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				if (containsyield(yieldnames.at(id)))
				{
					const FMTyldbounds* bound = &yieldbounds.at(id);
					const int new_age = getage(yieldnames.at(id), bound->getlower(), request.getdevelopment().getage());
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
			_exhandler->raisefromcatch("", "FMTageyieldhandler::getage", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}

	std::unique_ptr<FMTyieldhandler> FMTageyieldhandler::getfromfactor(const double& factor,
		std::vector<std::string>yieldnames) const
	{
		FMTageyieldhandler newhandler(*this);
		try {
				std::vector<std::map<std::string, FMTdata>::iterator>iterators;
				if (yieldnames.empty())
				{
					for (std::map<std::string, FMTdata>::iterator it = newhandler.elements.begin();
						it != newhandler.elements.end(); it++)
					{
						iterators.push_back(it);
					}
				}
				else {
					for (const std::string& yldname : yieldnames)
					{
						std::map<std::string, FMTdata>::iterator it = newhandler.elements.find(yldname);
						if (it != newhandler.elements.end())
						{
							iterators.push_back(it);
						}
					}
				}
				for (std::map<std::string, FMTdata>::iterator datait : iterators)
				{
					datait->second = ((datait->second)*factor);
				}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTageyieldhandler::getfromfactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return newhandler.clone();
	}

	bool FMTageyieldhandler::empty() const
	{
		return elements.empty();
	}
	size_t FMTageyieldhandler::size() const
	{
		return elements.size();
	}
	FMTdata& FMTageyieldhandler::operator[](const std::string& yldname)
	{
		return elements[yldname];
	}
	const FMTdata& FMTageyieldhandler::at(const std::string& yldname) const
	{
		return elements.at(yldname);
	}
	bool FMTageyieldhandler::containsyield(const std::string& yldname) const
	{
		return (elements.find(yldname) != elements.end());
	}


	std::vector<std::string>FMTageyieldhandler::getyieldnames() const
	{
		std::vector<std::string>results;
		results.reserve(elements.size());
		for (const auto& data : elements)
		{
			results.push_back(data.first);
		}
		return results;
	}

	void FMTageyieldhandler::clearcache()
	{
		
	}

	std::unique_ptr<FMTyieldhandler>FMTageyieldhandler::clone() const
		{
		return std::unique_ptr<FMTyieldhandler>(new FMTageyieldhandler(*this));
		}

	FMTyldtype FMTageyieldhandler::gettype() const
		{
		return FMTyldtype::FMTageyld;
		}

	std::map<std::string, std::vector<double>>FMTageyieldhandler::getallyieldsdata(const int& maxbase)const
	{
		std::map<std::string, std::vector<double>>localstuff;
		try {
			const int lastbase = getlastbase();
			std::vector<int>bases = getbases();
			for (std::map<std::string, FMTdata>::const_iterator cit = elements.begin(); cit != elements.end(); cit++)
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
			_exhandler->raisefromcatch("", "FMTageyieldhandler::getallyieldsdata", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return localstuff;
	}

	int FMTageyieldhandler::getendpoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const
	{
		size_t locid = 0;
		try {
				std::map<std::string, FMTdata>::const_iterator it = elements.find(yld);
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
			_exhandler->raisefromcatch("", "FMTageyieldhandler::getendpoint", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return static_cast<int>(locid);
	}

	double FMTageyieldhandler::getpeak(const FMTyieldrequest& request,const std::string& yld, const int& targetage) const
	{
		double value = 0;
		try {
			const int peak = static_cast<int>(getpeakfrom(yld));
			value = getchangesfrom(targetage, peak);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTageyieldhandler::getpeak", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return value;
	}



	double FMTageyieldhandler::getpeakfrom(const std::string& yld, double maxvalue) const
	{
		std::map<std::string, FMTdata>::const_iterator it = elements.find(yld);
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
			_exhandler->raisefromcatch("", "FMTageyieldhandler::getpeakfrom", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return peak;
	}

}

