/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTCOMPLEXYIELDHANDLER_H_INCLUDED
#define FMTCOMPLEXYIELDHANDLER_H_INCLUDED

#include "FMTyieldhandler.h"
#include "FMTdata.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTYieldsCache.h"
#include <memory>

namespace Core
{
	class FMTEXPORT FMTcomplexyieldhandler final : public FMTyieldhandler
	{
	public:
		void settabou(const size_t& index);
		void settabou(const FMTcomplexyieldhandler& rhs);
		std::vector<size_t>gettabous() const override;
		virtual void setoverrideindex(const size_t& newindex);
		virtual size_t  getoverrideindex() const;
		virtual int getlastbase() const;
		virtual double get(const std::string& yld, const FMTyieldrequest& request) const;
		bool comparesources(const std::string& yield,const FMTcomplexyieldhandler& overridedyield) const;
		virtual  operator std::string() const;
		~FMTcomplexyieldhandler();
		FMTcomplexyieldhandler();
		FMTcomplexyieldhandler(const FMTcomplexyieldhandler& rhs) = default;
		FMTcomplexyieldhandler& operator = (const FMTcomplexyieldhandler& rhs) = default;
		FMTcomplexyieldhandler(const FMTmask& mask);
		const std::map<std::string, FMTdata>& getdataelements() const;
		virtual std::vector<std::string> indexes(const std::vector<std::string>& names) const;
		virtual double getpeak(const FMTyieldrequest& request, const std::string& yld, const int& targetage) const;
		std::unique_ptr<FMTyieldhandler>complexyldtoageyld(const FMTyieldrequest& request, const FMTspec& lspec) const;
		virtual bool push_data(const std::string& yld, const double& value);
		virtual bool push_data(const std::string& yld, const FMTdata& data);
		virtual std::unique_ptr<FMTyieldhandler>clone() const;
		virtual bool operator == (const FMTcomplexyieldhandler& rhs) const;
		virtual bool empty() const;
		virtual size_t size() const;
		virtual FMTyldtype gettype() const;
		virtual FMTdata& operator[](const std::string& yldname);
		virtual const FMTdata& at(const std::string& yldname) const;
		virtual bool containsyield(const std::string& yldname) const;
		virtual std::vector<std::string>getyieldnames() const;
		virtual void clearcache();
		virtual int getage(const FMTyieldrequest& request, const FMTspec& spec) const;
		virtual double getyieldlinearvalue(const std::string& yldname, const FMTyieldrequest& request, bool allowoutofrange=true) const;
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTyieldhandler", boost::serialization::base_object<FMTyieldhandler>(*this));
			ar& BOOST_SERIALIZATION_NVP(elements);
		}
		std::map<std::string, FMTdata>elements;
		std::map<std::string, double> getsources(const std::map<std::string, const std::unique_ptr<FMTyieldhandler>*>& srcdata,
			const FMTyieldrequest& request, bool& age_only) const;
		std::map<std::string, const std::unique_ptr<FMTyieldhandler>*> getdata(const FMTyieldrequest& request,
			const std::vector<std::string>& names, const std::string& original) const;
		std::vector<double>getsourcesarray(const std::map<std::string, const std::unique_ptr<FMTyieldhandler>*>& srcdata,
			const FMTyieldrequest& request, bool& age_only) const;
		std::unique_ptr<FMTyieldhandler>toageyld(const FMTyieldrequest& request,
			const std::vector<std::string>& yieldnames, const int& minage, const int& maxage) const;
		std::unordered_set<size_t>overridetabou;
		size_t overrideindex;
		mutable FMTYieldsCache _cache;
	};

}

#endif 


