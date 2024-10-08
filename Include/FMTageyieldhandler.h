/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTAGEYIELDHANDLER_H_INCLUDED
#define FMTAGEYIELDHANDLER_H_INCLUDED

#include "FMTyieldhandler.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTdata.h"

namespace Core
{
	class FMTEXPORT FMTageyieldhandler : public FMTyieldhandler
	{
	public:
		virtual double get(const std::string& yld, const FMTyieldrequest& request) const;
		virtual  operator std::string() const;
		~FMTageyieldhandler() = default;
		FMTageyieldhandler() = default;
		FMTageyieldhandler(const FMTageyieldhandler& rhs) = default;
		FMTageyieldhandler& operator = (const FMTageyieldhandler& rhs) = default;
		FMTageyieldhandler(const FMTyieldhandler& rhs);
		// DocString: FMTageyieldhandler(const FMTmask&)
		/**
		Constructor for ageyieldhandler with a FMTmask
		*/		
		FMTageyieldhandler(const FMTmask& mask);
		// DocString: FMTageyieldhandler::setyieldvalues
		/**
		Set the values for given yield name. (baseages) are the base used to determine the age of the (values) like the _AGE in WS. 
		The vector of (baseages) must be the same as (values).
		*/
		virtual void setyieldvalues(const std::string& yldname,const std::vector<int>& baseages,const std::vector<double>& values);	
		virtual double getlastvalue(const std::string yld) const;
		virtual bool push_data(const std::string& yld, const double& value);
		virtual bool push_data(const std::string& yld, const FMTdata& data);
		virtual std::unique_ptr<FMTyieldhandler>clone() const;
		virtual bool operator == (const FMTageyieldhandler& rhs) const;
		virtual bool empty() const;
		virtual size_t size() const;
		virtual FMTyldtype gettype() const;
		virtual FMTdata& operator[](const std::string& yldname);
		virtual const FMTdata& at(const std::string& yldname) const;
		virtual bool containsyield(const std::string& yldname) const;
		virtual std::vector<std::string>getyieldnames() const;
		virtual void clearcache();
		virtual std::map<std::string, std::vector<double>>getallyieldsdata(const int& maxbase)const;
		virtual std::unique_ptr<FMTyieldhandler> getfromfactor(const double& factor,
			std::vector<std::string>yieldnames = std::vector<std::string>()) const;
		virtual int getendpoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const;
		virtual double getpeak(const FMTyieldrequest& request,const std::string& yld, const int& targetage) const;
		virtual double getpeakfrom(const std::string& yld, double maxvalue = std::numeric_limits<double>::lowest()) const;
		virtual int getage(const FMTyieldrequest& request, const FMTspec& spec) const;
		virtual double getyieldlinearvalue(const std::string&yldname, const FMTyieldrequest& request, bool allowoutofrange=true) const;
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTyieldhandler", boost::serialization::base_object<FMTyieldhandler>(*this));
			ar& BOOST_SERIALIZATION_NVP(m_elements);
		}
		std::map<std::string, FMTdata, cmpYieldString>m_elements;
		int getage(const std::string yld, const double& value, const int& starting_age) const;
	};

}

#endif 

