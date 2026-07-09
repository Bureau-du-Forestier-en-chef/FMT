/*
Copyright (c) 2019 Gouvernement du Qubec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldhandler.h"
#include "FMTyieldrequest.h"
#include "FMTdevelopment.h"
#include "FMTbounds.hpp"
#include "FMTdata.h"
#include "FMTtheme.h"
#include "FMTexceptionhandler.h"

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



    bool FMTyieldhandler::pushBase(const int& base)
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
    bool FMTyieldhandler::pushData(const std::string& yld,const double& value)
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

	bool FMTyieldhandler::basePushData(std::map<std::string, FMTdata, cmpYieldString>& elements, const std::string& yld, const double& value)
	{
		if (elements.find(yld) == elements.end())
		{
			elements[yld] = FMTdata();
		}
		elements[yld].data.push_back(value);
		return true;
	}
	bool FMTyieldhandler::basePushData(std::map<std::string, FMTdata, cmpYieldString>& elements, const std::string& yld, const FMTdata& data)
	{
		elements[yld] = FMTdata(data);
		return true;
	}

	void FMTyieldhandler::setBase(const std::vector<int>& allvalues)
		{
		bases = allvalues;
		}

	bool FMTyieldhandler::inLookAt(const std::string& yld) const
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

    bool FMTyieldhandler::pushData(const std::string& yld,const FMTdata& data)
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
    FMTyldtype FMTyieldhandler::getType() const
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

	void FMTyieldhandler::setMask(const FMTmask& p_mask)
		{
		mask = p_mask;
		}


	std::vector<std::string> FMTyieldhandler::compare(const std::vector<std::string>& keys) const
        {
		try {
			std::vector<std::string>same;
			try {
				for (const std::string& key : keys)
				{
					if (containsYield(key))
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

	

	double FMTyieldhandler::getLinearValue(const std::vector<double>& dls, const int& agetarget,bool allowoutofrange) const
		{
		double value = 0;
		try{
		if (agetarget >= bases.back())
			{
			value = dls.back();
			if (!allowoutofrange && agetarget > bases.back())
			{
				_exhandler->raise(Exception::FMTexc::FMToutofrangeyield, " at age "+std::to_string(agetarget)+" for max age of "+ std::to_string(bases.back())+" "+std::string(mask)+"\n",
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

	int FMTyieldhandler::getLastBase() const
		{
		int basevalue = 0;
		if (!bases.empty())
		{
			basevalue= bases.back();
		}
		return basevalue;
		}

	size_t FMTyieldhandler::getOverrideIndex() const
		{
		return 0;
		}

	std::vector<size_t> FMTyieldhandler::getTabous() const
	{
		return std::vector<size_t>();
	}

	void FMTyieldhandler::setOverrideIndex(const size_t& newindex)
		{
		
		}

	const std::vector<int>& FMTyieldhandler::getBases() const
		{
		return bases;
		}

	double FMTyieldhandler::getLastValue(const std::string yld) const
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

double FMTyieldhandler::getYieldLinearValue(const std::string&yldname, const FMTyieldrequest& request,bool allowoutofrange) const
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

	double FMTyieldhandler::getChangesFrom(const int& targetage, const int& peakstep) const
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


	int FMTyieldhandler::getEndPoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const
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

	int FMTyieldhandler::getAge(const FMTyieldrequest& request, const FMTspec& spec) const
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

	int FMTyieldhandler::getMaxBase(const FMTyieldrequest& request) const
	{
		int maxage = 0;
		try {
			for (const auto& data : request.getDatas())
			{
				const int maxbase = (data->second)->getLastBase();
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


    double FMTyieldhandler::getPeak(const FMTyieldrequest& request,const std::string& yld, const int& targetage) const
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
						if(bt.isAggregate(value))
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

	std::unique_ptr<FMTyieldhandler> FMTyieldhandler::getFromFactor(const double& factor,
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
		FMTdata nullData;
		return nullData;
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
		FMTdata nullData;
		return nullData;
	}

	bool FMTyieldhandler::isNullYield(const std::string& yldname) const
	{
		try {
			return at(yldname).nullData();
		}
		catch (...)
		{
			_exhandler->raisefromcatch(yldname, "FMTyieldhandler::isnullyield", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return false;
	}

	bool FMTyieldhandler::containsYield(const std::string& yldname) const
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

	std::vector<std::string>FMTyieldhandler::getYieldNames() const
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

	void FMTyieldhandler::clearCache()
	{
		
	}


	std::map<std::string, std::vector<double>>FMTyieldhandler::getAllYieldsData(const int& maxbase)const
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
