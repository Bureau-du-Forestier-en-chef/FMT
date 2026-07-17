/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldHandler.h"
#include "FMTYieldRequest.h"
#include "FMTDevelopment.h"
#include "FMTBounds.hpp"
#include "FMTData.h"
#include "FMTTheme.h"
#include "FMTExceptionHandler.h"

namespace Core{

FMTYieldHandler::operator std::string() const
        {
		std::string value = "";
		try{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::std::string()", __LINE__, __FILE__,Core::FMTsection::Yield);
			}
        return value;
        }


    FMTYieldHandler::FMTYieldHandler(const FMTMask& lmask) :
		FMTObject(),
		mask(lmask),
		bases(),
		lookat()
	{
	
	}


	std::unique_ptr<FMTYieldHandler>FMTYieldHandler::clone() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldHandler>(nullptr);
	}



    bool FMTYieldHandler::pushBase(const int& base)
        {
		try {
			bases.push_back(base);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::pushBase", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return false;
        }
    bool FMTYieldHandler::pushData(const std::string& yld,const double& value)
        {
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::pushData", __LINE__, __FILE__, Core::FMTsection::Yield);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::pushData", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return false;
        }

	bool FMTYieldHandler::basePushData(std::map<std::string, FMTData, cmpYieldString>& elements, const std::string& yld, const double& value)
	{
		if (elements.find(yld) == elements.end())
		{
			elements[yld] = FMTData();
		}
		elements[yld].data.push_back(value);
		return true;
	}
	bool FMTYieldHandler::basePushData(std::map<std::string, FMTData, cmpYieldString>& elements, const std::string& yld, const FMTData& data)
	{
		elements[yld] = FMTData(data);
		return true;
	}

	void FMTYieldHandler::setBase(const std::vector<int>& allvalues)
		{
		bases = allvalues;
		}

	bool FMTYieldHandler::inLookAt(const std::string& yld) const
	{
		return (lookat.find(yld) != lookat.end());
	}

	std::vector<std::string> FMTYieldHandler::indexes(const std::vector<std::string>& names) const
        {
		try {
			return std::vector<std::string>();
			//_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			//	"FMTYieldHandler::indexes", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::indexes", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<std::string>();
        }

    bool FMTYieldHandler::pushData(const std::string& yld,const FMTData& data)
        {
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::pushData", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::pushData", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return true;
        }
    FMTyldtype FMTYieldHandler::getType() const
        {
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::getType", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getType", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return FMTyldtype::FMTageyld;
        }

	FMTMask FMTYieldHandler::getMask() const
		{
		return mask;
		}

	void FMTYieldHandler::setMask(const FMTMask& p_mask)
		{
		mask = p_mask;
		}


	std::vector<std::string> FMTYieldHandler::compare(const std::vector<std::string>& keys) const
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
				_exhandler->raiseFromCatch("", "FMTAgeYieldHandler::compare", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
			return same;
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::compare", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return std::vector<std::string>();
        }


	 bool FMTYieldHandler::operator == (const FMTYieldHandler& rhs) const
	 {
		 return (mask == rhs.mask &&
			 bases == rhs.bases);
	 }


    double FMTYieldHandler::get(const std::string& yld,const FMTYieldRequest& request) const
        {
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
        }

	

	double FMTYieldHandler::getLinearValue(const std::vector<double>& dls, const int& agetarget,bool allowoutofrange) const
		{
		double value = 0;
		try{
		if (agetarget >= bases.back())
			{
			value = dls.back();
			if (!allowoutofrange && agetarget > bases.back())
			{
				_exhandler->raise(Exception::FMTexc::FMToutofrangeyield, " at age "+std::to_string(agetarget)+" for max age of "+ std::to_string(bases.back())+" "+std::string(mask)+"\n",
					"FMTYieldHandler::getLinearValue", __LINE__, __FILE__, Core::FMTsection::Yield);
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
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getLinearValue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
		}

	int FMTYieldHandler::getLastBase() const
		{
		int basevalue = 0;
		if (!bases.empty())
		{
			basevalue= bases.back();
		}
		return basevalue;
		}

	size_t FMTYieldHandler::getOverrideIndex() const
		{
		return 0;
		}

	std::vector<size_t> FMTYieldHandler::getTabous() const
	{
		return std::vector<size_t>();
	}

	void FMTYieldHandler::setOverrideIndex(const size_t& newindex)
		{
		
		}

	const std::vector<int>& FMTYieldHandler::getBases() const
		{
		return bases;
		}

	double FMTYieldHandler::getLastValue(const std::string yld) const
		{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::getLastValue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getLastValue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
		}

double FMTYieldHandler::getYieldLinearValue(const std::string&yldname, const FMTYieldRequest& request,bool allowoutofrange) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::getYieldLinearValue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getYieldLinearValue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}

	double FMTYieldHandler::getChangesFrom(const int& targetage, const int& peakstep) const
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
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getChangesFrom", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
		}


	int FMTYieldHandler::getEndPoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const
		{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::getEndPoint", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getEndPoint", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
		}

	int FMTYieldHandler::getAge(const FMTYieldRequest& request, const FMTSpec& spec) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::getAge", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getAge", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}

	int FMTYieldHandler::getMaxBase(const FMTYieldRequest& request) const
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
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getMaxBase", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return maxage;
	}


    double FMTYieldHandler::getPeak(const FMTYieldRequest& request,const std::string& yld, const int& targetage) const
        {
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::getPeak", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getPeak", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
        }


	std::unique_ptr<FMTYieldHandler> FMTYieldHandler::presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>& newthemes) const
		{
		std::unique_ptr<FMTYieldHandler> newhandler = this->clone();
		try {
			newhandler->mask = this->mask.presolve(filter, newthemes);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return newhandler;
		}

	std::unique_ptr<FMTYieldHandler> FMTYieldHandler::postSolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>& basethemes) const
			{
			std::unique_ptr<FMTYieldHandler> newhandler = this->clone();
			try {
				std::vector<std::string> values;
				const std::string maskname(std::string(newhandler->mask));
				boost::split(values, maskname,boost::is_any_of(FMT_STR_SEPARATOR),boost::token_compress_on);
				for (const std::string& value : values)
				{
					for (const FMTTheme& bt : basethemes)
					{
						if(bt.isAggregate(value))
						{
							_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot postSolve mask with aggregates " + std::string(newhandler->mask),
														"FMTYieldHandler::postSolve", __LINE__, __FILE__);
						}
					}
				}
				newhandler->mask = newhandler->mask.postSolve(filter,basethemes);
			}catch (...)
			{
				_exhandler->printExceptions("", "FMTYieldHandler::postSolve", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
			return newhandler;
			}

	std::unique_ptr<FMTYieldHandler> FMTYieldHandler::getFromFactor(const double& factor,
		std::vector<std::string>yieldnames) const
		{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::getFromFactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getFromFactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldHandler>(nullptr);
		}

	bool FMTYieldHandler::empty() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::empty", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::empty", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return true;
	}
	size_t FMTYieldHandler::size() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::size", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::size", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return 0;
	}
	FMTData& FMTYieldHandler::operator[](const std::string& yldname)
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::operator[]", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::operator[]", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		FMTData nullData;
		return nullData;
	}
	const FMTData& FMTYieldHandler::at(const std::string& yldname) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::at", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::at", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		FMTData nullData;
		return nullData;
	}

	bool FMTYieldHandler::isNullYield(const std::string& yldname) const
	{
		try {
			return at(yldname).nullData();
		}
		catch (...)
		{
			_exhandler->raiseFromCatch(yldname, "FMTYieldHandler::isNullYield", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return false;
	}

	bool FMTYieldHandler::containsYield(const std::string& yldname) const
		{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::containsYield", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::containsYield", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return false;
		}

	std::vector<std::string>FMTYieldHandler::getYieldNames() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::getYieldNames", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getYieldNames", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::vector<std::string>();
	}

	void FMTYieldHandler::clearCache()
	{
		
	}


	std::map<std::string, std::vector<double>>FMTYieldHandler::getAllYieldsData(const int& maxbase)const
	{
		try{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTYieldHandler::getAllYieldsData", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
			catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldHandler::getAllYieldsData", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::map<std::string, std::vector<double>>();
	}


}
