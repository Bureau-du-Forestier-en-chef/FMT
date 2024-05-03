/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTMODELYIELDHANDLER_H_INCLUDED
#define FMTMODELYIELDHANDLER_H_INCLUDED

#include "FMTyieldhandler.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <memory>

namespace Models
{
	class FMTmodel;
};

namespace Core
{
	class FMTdata;
	class FMTyieldmodel;
	class FMTtimeyieldhandler;
	class FMTEXPORT FMTmodelyieldhandler final : public FMTyieldhandler
	{
	public:
		virtual double get(const std::string& yld, const FMTyieldrequest& request) const;
		virtual  operator std::string() const;
		~FMTmodelyieldhandler() = default;
		FMTmodelyieldhandler();
		FMTmodelyieldhandler(const FMTmodelyieldhandler& rhs);
		FMTmodelyieldhandler& operator = (const FMTmodelyieldhandler& rhs);
		FMTmodelyieldhandler(const FMTmask& mask);
		std::map<std::string, size_t>getmodelsnamebyindex() const;
		void push_backmodel(const std::unique_ptr<FMTyieldmodel>& model);
		void setyield(const size_t& modelid,const size_t& yieldid,const std::string& yldname);
		virtual std::unique_ptr<FMTyieldhandler>clone() const;
		virtual bool operator == (const FMTmodelyieldhandler& rhs) const;
		virtual bool empty() const;
		virtual size_t size() const;
		virtual FMTyldtype gettype() const;
		virtual bool containsyield(const std::string& yldname) const;
		virtual bool isnullyield(const std::string& yldname) const;
		virtual std::vector<std::string>getyieldnames() const;
		virtual void clearcache();
		virtual std::unique_ptr<FMTyieldhandler> presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>& newthemes) const;
		virtual std::unique_ptr<FMTyieldhandler> postsolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>& basethemes) const;
		// DocString: FMTmodelyieldhandler::totimehandler
		/**
		If you can turn all your yieldmodel to time then you can return a timeyieldhandler.
		*/
		FMTtimeyieldhandler totimehandler() const;
		// DocString: FMTmodelyieldhandler::setModel
		/**
		@brief set The model to the yielmodel.
		@param[in] p_modelPtr the pointer to the actual model. This can be cast to different type of model...
		*/
		void setModel(Models::FMTmodel* p_modelPtr);
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTyieldhandler", boost::serialization::base_object<FMTyieldhandler>(*this));
			ar& yldnames;
		}
		std::vector<std::unique_ptr<FMTyieldmodel>>models;
		std::map<std::string, std::pair<size_t, size_t>>yldnames;
	};

}

#endif 
