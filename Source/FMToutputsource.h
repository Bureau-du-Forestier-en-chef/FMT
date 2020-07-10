/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTOSR_H_INCLUDED
#define FMTOSR_H_INCLUDED

#include "FMTbounds.h"
#include "FMTmask.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include <vector>
#include "FMTdevelopment.h"
#include "FMTactualdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <string>
#include "FMTaction.h"
#include "FMTutility.h"

namespace Core
{

class FMToutputsource : public FMTspec
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTspec>(*this));
		ar & BOOST_SERIALIZATION_NVP(mask);
		ar & BOOST_SERIALIZATION_NVP(target);
		ar & BOOST_SERIALIZATION_NVP(action);
		ar & BOOST_SERIALIZATION_NVP(yield);
		ar & BOOST_SERIALIZATION_NVP(values);
	}
    FMTmask mask;
    FMTotar target;
	std::string action,yield;
	std::vector<double>values;
	bool average;
    public:
		FMToutputsource();
		~FMToutputsource() = default;
        FMToutputsource(const FMTotar ltarget,double lvalue = 0, std::string lyield= "", std::string laction = "");
        FMToutputsource(const FMTotar ltarget, std::vector<double>lvalues);
        FMToutputsource(const FMTspec& spec,const FMTmask& lmask,
               const FMTotar ltarget, std::string lyield= "",
               std::string laction = "");
        FMToutputsource(const FMToutputsource& rhs);
        FMToutputsource& operator = (const FMToutputsource& rhs);
        operator std::string() const override;
		bool operator == (const FMToutputsource& rhs) const;
		bool operator != (const FMToutputsource& rhs) const;
		bool operator < (const FMToutputsource& rhs) const;
		bool isinaggregate(const FMToutputsource& rhs, const std::vector<Core::FMTaction>& actions) const;
		bool issubsetof(const FMToutputsource& rhs, const std::vector<Core::FMTaction>& actions) const;
		bool issubsetof(const FMToutputsource& rhs) const;
		bool issamebutdifferentaction(const FMToutputsource& rhs) const;
		const FMTmask& getmask() const;
		void setmask(const FMTmask& newmask);
		std::string getaction() const;
		std::string getyield() const;
		FMTotar gettarget() const;
		double getvalue(int period = 0) const;
		bool isvariable() const;
		bool islevel() const;
		bool isconstant() const;
		bool isvariablelevel() const;
		std::string getlevel() const;
		bool isnull(const FMTyields& ylds) const;
		bool istimeyield() const;
		std::vector<const FMTaction*>targets(const std::vector<FMTaction>& actions) const;
		bool isinventory() const;
		bool useinedges() const;
		bool isnextperiod() const;
		bool useoutedges() const;
		double getcoef(const FMTdevelopment& development,
			const FMTyields& yields, const FMTaction& action,
			const std::vector<FMTdevelopmentpath>& paths) const;
		inline bool use(const FMTdevelopment& development, const FMTyields& ylds) const
			{
			return (mask && development.mask.issubsetof(mask) && development.is(*this, ylds));
			}
		FMToutputsource presolve(const FMTmask& presolvedmask,const std::vector<FMTtheme>& newthemes) const;
		void setaverage();
		bool isaverage() const;
		bool canbededucedtoconstant() const;
		double getconstantvalue(const std::vector<Core::FMTactualdevelopment>& area,
								const std::vector<Core::FMTaction>&actions, const FMTyields& yields) const;
		size_t hash(int period = -1) const;
    };

class FMToutputsourcecomparator
	{
		bool variable;
	public:
		FMToutputsourcecomparator(bool lvariable);
		bool operator()(const FMToutputsource& source) const;

	};



}



#endif // FMTOSR_H_INCLUDED
