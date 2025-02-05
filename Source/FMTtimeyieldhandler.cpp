/*
Copyright (c) 2019 Gouvernement du Québecvalue = (*ddata)->getpeak(srcsdata.begin()->first, age)

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtimeyieldhandler.h"
#include "FMTyieldrequest.h"
#include "FMTdevelopment.h"
#include "FMTdata.h"
#include "FMTexceptionhandler.h"

namespace Core {

	FMTtimeyieldhandler::operator std::string() const
	{
		std::string value;
		try {
			value += "*YT " + std::string(mask) + "\n";
			size_t baseid = 0;
			for (std::map<std::string, FMTdata, cmpYieldString>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
			{
				value += it->first + " ";
				/*for (const int& base : bases)
				{
					value += std::to_string(base) + " ";
				}*/
				value += std::to_string(bases.at(std::min(baseid, bases.size()-1))) + " ";
				const std::vector<double>* data = &it->second.data;
				for (const double & val : *data)
				{
					std::string singlevalue = std::to_string(val);
					singlevalue.erase(singlevalue.find_last_not_of('0') + 1, std::string::npos);
					singlevalue.erase(singlevalue.find_last_not_of('.') + 1, std::string::npos);
					value += singlevalue + " ";
				}
				value += "\n";
				++baseid;
			}

		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtimeyieldhandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}


	bool FMTtimeyieldhandler::push_data(const std::string& yld, const double& value)
	{
		try {
			if (m_elements.find(yld) == m_elements.end())
			{
				m_elements[yld] = FMTdata();
				if (m_elements.size() > 1  && bases.size() >= m_elements.size())//presume that the last base pushed is the base of the push data...
				{
					const size_t newlocation = std::distance(std::begin(m_elements), m_elements.find(yld));
					const int lastbase = getlastbase();
					bases.insert(bases.begin() + newlocation, lastbase);
					bases.pop_back();
				}
			}
			m_elements[yld].data.push_back(value);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtimeyieldhandler::push_data", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return true;
		//return (basepush_data(elements, yld, value));
	}

	bool FMTtimeyieldhandler::push_data(const std::string& yld, const FMTdata& data)
	{
		return (basepush_data(m_elements, yld, data));
	}
	FMTyldtype FMTtimeyieldhandler::gettype() const
	{
		return FMTyldtype::FMTtimeyld;
	}


	bool FMTtimeyieldhandler::operator == (const FMTtimeyieldhandler& rhs) const
	{
		return (FMTyieldhandler::operator==(rhs) &&
			m_elements == rhs.m_elements);
	}

	double FMTtimeyieldhandler::get(const std::string& yld, const FMTyieldrequest& request) const
	{
		try {
				std::map<std::string, Core::FMTdata, cmpYieldString>::const_iterator data0it = m_elements.find(yld);
				if (data0it != m_elements.end())
				{
					const FMTdata* lvalues = &data0it->second;
					const int target = request.getdevelopment().getperiod();
					if (lvalues->getop() == FMTyieldparserop::FMTdiscountfactor)
					{
						const double perioddbl = static_cast<double>(target);
						const double pertio = lvalues->data.at(1);
						const std::string discounttype = *lvalues->getSources().at(0);
						const double rateofreturn = lvalues->data.at(1);
						double exponant = perioddbl; //full
						if (discounttype == "NONE")
						{
							exponant = perioddbl - 1;
						}
						else if (discounttype == "HALF")
						{
							exponant = perioddbl * 0.5;
						}
						return (1 / std::pow((1 + rateofreturn), pertio * exponant));
					}else {
						double value = lvalues->data.back();
						const int BASE = getbases().at(0);
						const int INDEX = target - BASE; 
						if (INDEX < 0)
						{
							value = 0;
						}
						else if (INDEX < static_cast<int>(lvalues->data.size()))
						{
							value = lvalues->data.at(INDEX);
						}
						return value;
					}
				}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("at yield " + yld, "FMTtimeyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}

	double FMTtimeyieldhandler::getlastvalue(const std::string yld) const
	{
		std::map<std::string, FMTdata, cmpYieldString>::const_iterator it = m_elements.find(yld);
		return it->second.data.back();
	}


	std::unique_ptr<FMTyieldhandler> FMTtimeyieldhandler::getfromfactor(const double& factor,
		std::vector<std::string>yieldnames) const
	{
		FMTtimeyieldhandler newhandler(*this);
		try {
			std::vector<std::map<std::string, FMTdata,cmpYieldString>::iterator>iterators;
			if (yieldnames.empty())
			{
				for (std::map<std::string, FMTdata>::iterator it = newhandler.m_elements.begin();
					it != newhandler.m_elements.end(); it++)
				{
					iterators.push_back(it);
				}
			}
			else {
				for (const std::string& yldname : yieldnames)
				{
					std::map<std::string, FMTdata, cmpYieldString>::iterator it = newhandler.m_elements.find(yldname);
					if (it != newhandler.m_elements.end())
					{
						iterators.push_back(it);
					}
				}
			}
			for (std::map<std::string, FMTdata, cmpYieldString>::iterator datait : iterators)
			{
				datait->second = ((datait->second)*factor);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtimeyieldhandler::getfromfactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return newhandler.clone();
	}

	bool FMTtimeyieldhandler::empty() const
	{
		return m_elements.empty();
	}
	size_t FMTtimeyieldhandler::size() const
	{
		return m_elements.size();
	}
	FMTdata& FMTtimeyieldhandler::operator[](const std::string& yldname)
	{
		return m_elements[yldname];
	}
	const FMTdata& FMTtimeyieldhandler::at(const std::string& yldname) const
	{
		return m_elements.at(yldname);
	}
	bool FMTtimeyieldhandler::containsyield(const std::string& yldname) const
	{
		return (m_elements.find(yldname) != m_elements.end());
	}

	std::vector<std::string>FMTtimeyieldhandler::getyieldnames() const
	{
		std::vector<std::string>results;
		results.reserve(m_elements.size());
		for (const auto& data : m_elements)
		{
			results.push_back(data.first);
		}
		return results;
	}

	void FMTtimeyieldhandler::clearcache()
	{

	}

	FMTtimeyieldhandler::FMTtimeyieldhandler(const FMTmask& mask):
		FMTyieldhandler(mask)
	{

	}

	std::unique_ptr<FMTyieldhandler>FMTtimeyieldhandler::clone() const
	{
		return std::unique_ptr<FMTyieldhandler>(new FMTtimeyieldhandler(*this));
	}

	std::map<std::string, std::vector<double>>FMTtimeyieldhandler::getallyieldsdata(const int& maxbase)const
	{
		std::map<std::string, std::vector<double>>localstuff;
		try {
			for (std::map<std::string, FMTdata, cmpYieldString>::const_iterator cit = m_elements.begin(); cit != m_elements.end(); cit++)
			{
				localstuff[cit->first] = cit->second.data;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtimeyieldhandler::getallyieldsdata", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return localstuff;
	}

	void FMTtimeyieldhandler::setyieldvalues(const std::string& yldname,const int& startingperiod,const std::vector<double>& values)
	{
		try{
			if(this->getbases().empty())
			{
				this->push_base(startingperiod);
			}else if(this->getbases().size()>1)
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Yieldhandler have multiple starting period ...",
				"FMTtimeyieldhandler::setyieldvalues", __LINE__, __FILE__, Core::FMTsection::Yield);
			}else if(this->getbases()[0]!=startingperiod)
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Starting period is different from this FMTtimeyieldhandler. \nYou must create a new FMTtimeyieldhandler for those values",
				"FMTtimeyieldhandler::setyieldvalues", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
			for(const double& value : values)
			{
				this->push_data(yldname,value);
			}
		}catch(...){
			_exhandler->raisefromcatch("", "FMTtimeyieldhandler::setyieldvalues", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}


}


