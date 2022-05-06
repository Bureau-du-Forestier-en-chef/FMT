/*
Copyright (c) 2019 Gouvernement du Québecvalue = (*ddata)->getpeak(srcsdata.begin()->first, age)

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtimeyieldhandler.hpp"
#include "FMTyieldrequest.hpp"
#include "FMTdevelopment.hpp"
#include "FMTdata.hpp"
#include "FMTexceptionhandler.hpp"

namespace Core {

	FMTtimeyieldhandler::operator std::string() const
	{
		std::string value;
		try {
			value += "*YT " + std::string(mask) + "\n";
			for (std::map<std::string, FMTdata>::const_iterator it = elements.begin(); it != elements.end(); ++it)
			{
				value += it->first + " ";
				for (const int& base : bases)
				{
					value += std::to_string(base) + " ";
				}
				const std::vector<double>* data = &it->second.data;
				for (const double & val : *data)
				{
					value += std::to_string(val) + " ";
				}
				value += "\n";
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
		return (basepush_data(elements, yld, value));
	}

	bool FMTtimeyieldhandler::push_data(const std::string& yld, const FMTdata& data)
	{
		return (basepush_data(elements, yld, data));
	}
	FMTyldtype FMTtimeyieldhandler::gettype() const
	{
		return FMTyldtype::FMTtimeyld;
	}


	bool FMTtimeyieldhandler::operator == (const FMTtimeyieldhandler& rhs) const
	{
		return (FMTyieldhandler::operator==(rhs) &&
			elements == rhs.elements);
	}

	double FMTtimeyieldhandler::get(const std::string& yld, const FMTyieldrequest& request) const
	{
		try {
				std::map<std::string, Core::FMTdata>::const_iterator data0it = elements.find(yld);
				if (data0it != elements.end())
				{
					const FMTdata* lvalues = &data0it->second;
					const int target = request.getdevelopment().getperiod();
					if (lvalues->getop() == FMTyieldparserop::FMTdiscountfactor)
					{
						const double perioddbl = static_cast<double>(target);
						const double pertio = lvalues->data.at(1);
						const std::string discounttype = lvalues->getsource().at(0);
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
						if (target < static_cast<int>(lvalues->data.size()))
						{
							value = lvalues->data.at(target);
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
		std::map<std::string, FMTdata>::const_iterator it = elements.find(yld);
		return it->second.data.back();
	}


	std::unique_ptr<FMTyieldhandler> FMTtimeyieldhandler::getfromfactor(const double& factor,
		std::vector<std::string>yieldnames) const
	{
		FMTtimeyieldhandler newhandler(*this);
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
			_exhandler->raisefromcatch("", "FMTtimeyieldhandler::getfromfactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return newhandler.clone();
	}

	bool FMTtimeyieldhandler::empty() const
	{
		return elements.empty();
	}
	size_t FMTtimeyieldhandler::size() const
	{
		return elements.size();
	}
	FMTdata& FMTtimeyieldhandler::operator[](const std::string& yldname)
	{
		return elements[yldname];
	}
	const FMTdata& FMTtimeyieldhandler::at(const std::string& yldname) const
	{
		return elements.at(yldname);
	}
	bool FMTtimeyieldhandler::containsyield(const std::string& yldname) const
	{
		return (elements.find(yldname) != elements.end());
	}

	std::vector<std::string>FMTtimeyieldhandler::getyieldnames() const
	{
		std::vector<std::string>results;
		results.reserve(elements.size());
		for (const auto& data : elements)
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
			for (std::map<std::string, FMTdata>::const_iterator cit = elements.begin(); cit != elements.end(); cit++)
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


