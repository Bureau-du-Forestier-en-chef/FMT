/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldhandler.hpp"
#include "FMTyieldrequest.hpp"
#include "FMTdevelopment.hpp"
#include "FMTbounds.hpp"
#include "FMTdata.hpp"
#include "FMTtheme.hpp"
#include "FMTexceptionhandler.hpp"

namespace Core{

FMTyieldhandler::operator std::string() const
        {
		std::string value = "";
		try{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTyieldhandler::std::string()", __LINE__, __FILE__,Core::FMTsection::Yield);
			}
        return value;
        }


    FMTyieldhandler::FMTyieldhandler(const FMTmask& lmask) :
		FMTobject(),
		mask(lmask),
		bases(),
		lookat()
	{
	
	}


	std::unique_ptr<FMTyieldhandler>FMTyieldhandler::clone() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldhandler>(nullptr);
	}



    bool FMTyieldhandler::push_base(const int& base)
        {
		try {
			bases.push_back(base);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::push_base", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return false;
        }
    bool FMTyieldhandler::push_data(const std::string& yld,const double& value)
        {
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::push_data", __LINE__, __FILE__, Core::FMTsection::Yield);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTyieldhandler::push_data", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return false;
        }

	bool FMTyieldhandler::basepush_data(std::map<std::string, FMTdata>& elements, const std::string& yld, const double& value)
	{
		if (elements.find(yld) == elements.end())
		{
			elements[yld] = FMTdata();
		}
		elements[yld].data.push_back(value);
		return true;
	}
	bool FMTyieldhandler::basepush_data(std::map<std::string, FMTdata>& elements, const std::string& yld, const FMTdata& data)
	{
		elements[yld] = FMTdata(data);
		return true;
	}

	void FMTyieldhandler::setbase(const std::vector<int>& allvalues)
		{
		bases = allvalues;
		}

	bool FMTyieldhandler::inlookat(const std::string& yld) const
	{
		return (lookat.find(yld) != lookat.end());
	}

	std::vector<std::string> FMTyieldhandler::indexes(const std::vector<std::string>& names) const
        {
		try {
			return std::vector<std::string>();
			//_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			//	"FMTyieldhandler::indexes", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::indexes", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<std::string>();
        }

    bool FMTyieldhandler::push_data(const std::string& yld,const FMTdata& data)
        {
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::push_data", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::push_data", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return true;
        }
    FMTyldtype FMTyieldhandler::gettype() const
        {
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::gettype", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::gettype", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return FMTyldtype::FMTageyld;
        }

	FMTmask FMTyieldhandler::getmask() const
		{
		return mask;
		}
	std::vector<std::string> FMTyieldhandler::compare(const std::vector<std::string>& keys) const
        {
		try {
			std::vector<std::string>same;
			try {
				for (const std::string& key : keys)
				{
					if (containsyield(key))
					{
						same.push_back(key);
					}
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTageyieldhandler::compare", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
			return same;
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::compare", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return std::vector<std::string>();
        }


	 bool FMTyieldhandler::operator == (const FMTyieldhandler& rhs) const
	 {
		 return (mask == rhs.mask &&
			 bases == rhs.bases);
	 }


    double FMTyieldhandler::get(const std::string& yld,const FMTyieldrequest& request) const
        {
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
        }

	

	double FMTyieldhandler::getlinearvalue(const std::vector<double>& dls, const int& agetarget,bool allowoutofrange) const
		{
		double value = 0;
		try{
		if (agetarget >= bases.back())
			{
			value = dls.back();
			if (!allowoutofrange && agetarget > bases.back())
			{
				_exhandler->raise(Exception::FMTexc::FMToutofrangeyield, " at age "+std::to_string(agetarget)+" for max age of "+ std::to_string(bases.back()),
					"FMTyieldhandler::getlinearvalue", __LINE__, __FILE__, Core::FMTsection::Yield);
				return 0;
			}
		}else if(agetarget < bases.front())
			{
			value = (agetarget * (dls[0] / static_cast<double>(bases[0])));
			}else{
			int id = 0;
			int highindex = -1;
			int lowindex = -1;
			for (const int& base : bases)
				{
				if (base <= agetarget)
					{
					lowindex = id;
					}
				if (base >= agetarget)
					{
					highindex = id;
					break;
					}
				++id;
				}
			if (lowindex != highindex)
				{
				const double factor = ((dls[highindex] - dls[lowindex]) / (static_cast<double>(bases[highindex]) - static_cast<double>(bases[lowindex])));
				const double lastvalue = dls[lowindex];
				value = lastvalue + ((agetarget - bases[lowindex]) * factor);
				}else{
				value = dls[highindex];
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getlinearvalue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
		}

	int FMTyieldhandler::getlastbase() const
		{
		int basevalue = 0;
		if (!bases.empty())
		{
			basevalue= bases.back();
		}
		return basevalue;
		}

	size_t FMTyieldhandler::getoverrideindex() const
		{
		return 0;
		}

	std::vector<size_t> FMTyieldhandler::gettabous() const
	{
		return std::vector<size_t>();
	}

	void FMTyieldhandler::setoverrideindex(const size_t& newindex)
		{
		
		}

	const std::vector<int>& FMTyieldhandler::getbases() const
		{
		return bases;
		}

	double FMTyieldhandler::getlastvalue(const std::string yld) const
		{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::getlastvalue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getlastvalue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
		}

double FMTyieldhandler::getyieldlinearvalue(const std::string&yldname, const FMTyieldrequest& request,bool allowoutofrange) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::getyieldlinearvalue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getyieldlinearvalue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}

	double FMTyieldhandler::getchangesfrom(const int& targetage, const int& peakstep) const
		{
		double value = 0;
		try{
		if (peakstep > 0)
			{
			const size_t agesize = static_cast<size_t>(targetage);
			std::vector<double>peakvalues(std::max(agesize, bases.size()) + 1, 0.0);
			const int peakage = bases[peakstep];
			int id = 0;
			for (double& pvalue : peakvalues)
			{
				pvalue = (peakage - id);
				++id;
			}
			value = peakvalues.at(targetage);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getchangesfrom", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
		}


	int FMTyieldhandler::getendpoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const
		{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::getendpoint", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getendpoint", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
		}

	int FMTyieldhandler::getage(const FMTyieldrequest& request, const FMTspec& spec) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::getage", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getage", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}

	int FMTyieldhandler::getmaxbase(const FMTyieldrequest& request) const
	{
		int maxage = 0;
		try {
			for (const auto& data : request.getdatas())
			{
				const int maxbase = (*data)->getlastbase();
				if (maxbase> maxage)
				{
					maxage = maxbase;
				}
			}

		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getmaxbase", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return maxage;
	}


    double FMTyieldhandler::getpeak(const FMTyieldrequest& request,const std::string& yld, const int& targetage) const
        {
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::getpeak", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getpeak", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
        }


	std::unique_ptr<FMTyieldhandler> FMTyieldhandler::presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>& newthemes) const
		{
		std::unique_ptr<FMTyieldhandler> newhandler = this->clone();
		try {
			newhandler->mask = this->mask.presolve(filter, newthemes);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return newhandler;
		}

	std::unique_ptr<FMTyieldhandler> FMTyieldhandler::postsolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>& basethemes) const
			{
			std::unique_ptr<FMTyieldhandler> newhandler = this->clone();
			try {
				std::vector<std::string> values;
				const std::string maskname(std::string(newhandler->mask));
				boost::split(values, maskname,boost::is_any_of(FMT_STR_SEPARATOR),boost::token_compress_on);
				for (const std::string& value : values)
				{
					for (const FMTtheme& bt : basethemes)
					{
						if(bt.isaggregate(value))
						{
							_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot postsolve mask with aggregates " + std::string(newhandler->mask),
														"FMTyieldhandler::postsolve", __LINE__, __FILE__);
						}
					}
				}
				newhandler->mask = newhandler->mask.postsolve(filter,basethemes);
			}catch (...)
			{
				_exhandler->printexceptions("", "FMTyieldhandler::postsolve", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
			return newhandler;
			}

	std::unique_ptr<FMTyieldhandler> FMTyieldhandler::getfromfactor(const double& factor,
		std::vector<std::string>yieldnames) const
		{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::getfromfactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getfromfactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldhandler>(nullptr);
		}

	bool FMTyieldhandler::empty() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::empty", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::empty", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return true;
	}
	size_t FMTyieldhandler::size() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::size", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::size", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}
	FMTdata& FMTyieldhandler::operator[](const std::string& yldname)
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::operator[]", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::operator[]", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		FMTdata nulldata;
		return nulldata;
	}
	const FMTdata& FMTyieldhandler::at(const std::string& yldname) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::at", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::at", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		FMTdata nulldata;
		return nulldata;
	}

	bool FMTyieldhandler::isnullyield(const std::string& yldname) const
	{
		try {
			return at(yldname).nulldata();
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::isnullyield", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return false;
	}

	bool FMTyieldhandler::containsyield(const std::string& yldname) const
		{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::containsyield", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::containsyield", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return false;
		}

	std::vector<std::string>FMTyieldhandler::getyieldnames() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::getyieldnames", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getyieldnames", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<std::string>();
	}

	void FMTyieldhandler::clearcache()
	{
		
	}


	std::map<std::string, std::vector<double>>FMTyieldhandler::getallyieldsdata(const int& maxbase)const
	{
		try{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldhandler::getallyieldsdata", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
			catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getallyieldsdata", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::map<std::string, std::vector<double>>();
	}


}
