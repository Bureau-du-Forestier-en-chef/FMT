/*
Copyright (c) 2019 Gouvernement du Québecvalue = (*ddata)->getPeak(srcsdata.begin()->first, age)

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTimeYieldHandler.h"
#include "FMTYieldRequest.h"
#include "FMTDevelopment.h"
#include "FMTData.h"
#include "FMTExceptionHandler.h"

namespace Core {

	FMTTimeYieldHandler::operator std::string() const
	{
		std::string value;
		try {
			value += "*YT " + std::string(mask) + "\n";
			size_t baseid = 0;
			for (std::map<std::string, FMTData, cmpYieldString>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
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
			_exhandler->raiseFromCatch("", "FMTTimeYieldHandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}


	bool FMTTimeYieldHandler::pushData(const std::string& yld, const double& value)
	{
		try {
			if (m_elements.find(yld) == m_elements.end())
			{
				m_elements[yld] = FMTData();
				if (m_elements.size() > 1  && bases.size() >= m_elements.size())//presume that the last base pushed is the base of the push data...
				{
					const size_t newlocation = std::distance(std::begin(m_elements), m_elements.find(yld));
					const int lastbase = getLastBase();
					bases.insert(bases.begin() + newlocation, lastbase);
					bases.pop_back();
				}
			}
			m_elements[yld].data.push_back(value);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTTimeYieldHandler::pushData", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return true;
		//return (basePushData(elements, yld, value));
	}

	bool FMTTimeYieldHandler::pushData(const std::string& yld, const FMTData& data)
	{
		return (basePushData(m_elements, yld, data));
	}
	FMTyldtype FMTTimeYieldHandler::getType() const
	{
		return FMTyldtype::FMTtimeyld;
	}


	bool FMTTimeYieldHandler::operator == (const FMTTimeYieldHandler& rhs) const
	{
		return (FMTYieldHandler::operator==(rhs) &&
			m_elements == rhs.m_elements);
	}

	double FMTTimeYieldHandler::get(const std::string& yld, const FMTYieldRequest& request) const
	{
		try {
				std::map<std::string, Core::FMTData, cmpYieldString>::const_iterator data0it = m_elements.find(yld);
				if (data0it != m_elements.end())
				{
					const FMTData* lvalues = &data0it->second;
					const int target = request.getDevelopment().getPeriod();
					if (lvalues->getOp() == FMTyieldparserop::FMTdiscountfactor)
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
						const int BASE = getBases().at(0);
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
			_exhandler->raiseFromCatch("at yield " + yld, "FMTTimeYieldHandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}

	double FMTTimeYieldHandler::getLastValue(const std::string yld) const
	{
		std::map<std::string, FMTData, cmpYieldString>::const_iterator it = m_elements.find(yld);
		return it->second.data.back();
	}


	std::unique_ptr<FMTYieldHandler> FMTTimeYieldHandler::getFromFactor(const double& factor,
		std::vector<std::string>yieldnames) const
	{
		FMTTimeYieldHandler newhandler(*this);
		try {
			std::vector<std::map<std::string, FMTData,cmpYieldString>::iterator>iterators;
			if (yieldnames.empty())
			{
				for (std::map<std::string, FMTData>::iterator it = newhandler.m_elements.begin();
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
			_exhandler->raiseFromCatch("", "FMTTimeYieldHandler::getFromFactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return newhandler.clone();
	}

	bool FMTTimeYieldHandler::empty() const
	{
		return m_elements.empty();
	}
	size_t FMTTimeYieldHandler::size() const
	{
		return m_elements.size();
	}
	FMTData& FMTTimeYieldHandler::operator[](const std::string& yldname)
	{
		return m_elements[yldname];
	}
	const FMTData& FMTTimeYieldHandler::at(const std::string& yldname) const
	{
		return m_elements.at(yldname);
	}
	bool FMTTimeYieldHandler::containsYield(const std::string& yldname) const
	{
		return (m_elements.find(yldname) != m_elements.end());
	}

	std::vector<std::string>FMTTimeYieldHandler::getYieldNames() const
	{
		std::vector<std::string>results;
		results.reserve(m_elements.size());
		for (const auto& data : m_elements)
		{
			results.push_back(data.first);
		}
		return results;
	}

	void FMTTimeYieldHandler::clearCache()
	{

	}

	FMTTimeYieldHandler::FMTTimeYieldHandler(const FMTMask& mask):
		FMTYieldHandler(mask)
	{

	}

	std::unique_ptr<FMTYieldHandler>FMTTimeYieldHandler::clone() const
	{
		return std::unique_ptr<FMTYieldHandler>(new FMTTimeYieldHandler(*this));
	}

	std::map<std::string, std::vector<double>>FMTTimeYieldHandler::getAllYieldsData(const int& maxbase)const
	{
		std::map<std::string, std::vector<double>>localstuff;
		try {
			for (std::map<std::string, FMTData, cmpYieldString>::const_iterator cit = m_elements.begin(); cit != m_elements.end(); cit++)
			{
				localstuff[cit->first] = cit->second.data;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTTimeYieldHandler::getAllYieldsData", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return localstuff;
	}

	void FMTTimeYieldHandler::setYieldValues(const std::string& yldname,const int& startingperiod,const std::vector<double>& values)
	{
		try{
			if(this->getBases().empty())
			{
				this->pushBase(startingperiod);
			}else if(this->getBases().size()>1)
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Yieldhandler have multiple starting period ...",
				"FMTTimeYieldHandler::setYieldValues", __LINE__, __FILE__, Core::FMTsection::Yield);
			}else if(this->getBases()[0]!=startingperiod)
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Starting period is different from this FMTtimeyieldhandler. \nYou must create a new FMTtimeyieldhandler for those values",
				"FMTTimeYieldHandler::setYieldValues", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
			for(const double& value : values)
			{
				this->pushData(yldname,value);
			}
		}catch(...){
			_exhandler->raiseFromCatch("", "FMTTimeYieldHandler::setYieldValues", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
	}


}


