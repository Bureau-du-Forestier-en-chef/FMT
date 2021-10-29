/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTTIMEYIELDHANDLER_H_INCLUDED
#define FMTTIMEYIELDHANDLER_H_INCLUDED

#include "FMTyieldhandler.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace Core
{
	class FMTdata;
	class FMTEXPORT FMTtimeyieldhandler final : public FMTyieldhandler
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("FMTyieldhandler", boost::serialization::base_object<FMTyieldhandler>(*this));
		ar & BOOST_SERIALIZATION_NVP(elements);
		}
		std::map<std::string, FMTdata>elements;
	public:
		virtual double get(const std::string& yld, const FMTyieldrequest& request) const;
		virtual  operator std::string() const;
		~FMTtimeyieldhandler() = default;
		FMTtimeyieldhandler() = default;
		FMTtimeyieldhandler(const FMTtimeyieldhandler& rhs) = default;
		FMTtimeyieldhandler& operator = (const FMTtimeyieldhandler& rhs) = default;
		FMTtimeyieldhandler(const FMTmask& mask);
		virtual double getlastvalue(const std::string yld) const;
		virtual bool push_data(const std::string& yld, const double& value);
		virtual bool push_data(const std::string& yld, const FMTdata& data);
		virtual std::unique_ptr<FMTyieldhandler>clone() const;
		virtual bool operator == (const FMTtimeyieldhandler& rhs) const;
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
	};

}

#endif 
