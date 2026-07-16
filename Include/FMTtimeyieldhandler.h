/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTTIMEYIELDHANDLER_Hm_included
#define FMTTIMEYIELDHANDLER_Hm_included

#include "FMTyieldhandler.h"
#include "FMTdata.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace Core
{
	
	class FMTEXPORT FMTTimeYieldHandler final : public FMTYieldHandler
	{
	public:
		virtual double get(const std::string& yld, const FMTYieldRequest& request) const;
		virtual  operator std::string() const;
		~FMTTimeYieldHandler() = default;
		FMTTimeYieldHandler() = default;
		FMTTimeYieldHandler(const FMTTimeYieldHandler& rhs) = default;
		FMTTimeYieldHandler& operator = (const FMTTimeYieldHandler& rhs) = default;
		// DocString: FMTTimeYieldHandler(const FMTMask&)
		/**
		Constructor for FMTTimeYieldHandler with a FMTMask
		*/		
		FMTTimeYieldHandler(const FMTMask& mask);
		// DocString: FMTTimeYieldHandler::setYieldValues
		/**
		Set the values for given yield name. 
		*/
		virtual void setYieldValues(const std::string& yldname,const int& startingperiod,const std::vector<double>& values);	
		virtual double getLastValue(const std::string yld) const;
		virtual bool pushData(const std::string& yld, const double& value);
		virtual bool pushData(const std::string& yld, const FMTData& data);
		virtual std::unique_ptr<FMTYieldHandler>clone() const;
		virtual bool operator == (const FMTTimeYieldHandler& rhs) const;
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
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTyieldhandler", boost::serialization::base_object<FMTYieldHandler>(*this));
			ar& BOOST_SERIALIZATION_NVP(m_elements);
		}
		std::map<std::string, FMTData, cmpYieldString>m_elements;
	};

}

#endif 
