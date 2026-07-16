/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTAGEYIELDHANDLER_Hm_included
#define FMTAGEYIELDHANDLER_Hm_included

#include "FMTyieldhandler.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTdata.h"

namespace Core
{
	class FMTEXPORT FMTAgeYieldHandler : public FMTYieldHandler
	{
	public:
		virtual double get(const std::string& yld, const FMTYieldRequest& request) const;
		virtual  operator std::string() const;
		~FMTAgeYieldHandler() = default;
		FMTAgeYieldHandler() = default;
		FMTAgeYieldHandler(const FMTAgeYieldHandler& rhs) = default;
		FMTAgeYieldHandler& operator = (const FMTAgeYieldHandler& rhs) = default;
		FMTAgeYieldHandler(const FMTYieldHandler& rhs);
		// DocString: FMTAgeYieldHandler(const FMTMask&)
		/**
		Constructor for ageyieldhandler with a FMTMask
		*/		
		FMTAgeYieldHandler(const FMTMask& mask);
		// DocString: FMTAgeYieldHandler::setYieldValues
		/**
		Set the values for given yield name. (baseages) are the base used to determine the age of the (values) like the _AGE in WS. 
		The vector of (baseages) must be the same as (values).
		*/
		virtual void setYieldValues(const std::string& yldname,const std::vector<int>& baseages,const std::vector<double>& values);	
		virtual double getLastValue(const std::string yld) const;
		virtual bool pushData(const std::string& yld, const double& value);
		virtual bool pushData(const std::string& yld, const FMTData& data);
		virtual std::unique_ptr<FMTYieldHandler>clone() const;
		virtual bool operator == (const FMTAgeYieldHandler& rhs) const;
		virtual bool empty() const;
		virtual size_t size() const;
		virtual FMTyldtype getType() const;
		virtual FMTData& operator[](const std::string& yldname);
		virtual const FMTData& at(const std::string& yldname) const;
		virtual bool containsYield(const std::string& yldname) const;
		virtual std::vector<std::string>getYieldNames() const;
		virtual void clearCache();
		virtual std::map<std::string, std::vector<double>>getAllYieldsData(const int& maxbase)const;
		virtual std::unique_ptr<FMTYieldHandler> getFromFactor(const double& factor,
			std::vector<std::string>yieldnames = std::vector<std::string>()) const;
		virtual int getEndPoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const;
		virtual double getPeak(const FMTYieldRequest& request,const std::string& yld, const int& targetage) const;
		virtual double getPeakfrom(const std::string& yld, double maxvalue = std::numeric_limits<double>::lowest()) const;
		virtual int getAge(const FMTYieldRequest& request, const FMTSpec& spec) const;
		virtual double getYieldLinearValue(const std::string&yldname, const FMTYieldRequest& request, bool allowoutofrange=true) const;
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTyieldhandler", boost::serialization::base_object<FMTYieldHandler>(*this));
			ar& BOOST_SERIALIZATION_NVP(m_elements);
		}
		std::map<std::string, FMTData, cmpYieldString>m_elements;
		int getAge(const std::string yld, const double& value, const int& starting_age) const;
	};

}

#endif 

