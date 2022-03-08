/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTOSR_H_INCLUDED
#define FMTOSR_H_INCLUDED

#include "FMTbounds.hpp"
#include "FMTmask.hpp"
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <string>
#include "FMTutility.hpp"
#include <unordered_set>
#include "FMTdevelopment.hpp"


namespace Graph
{
	class FMTgraphvertextoyield;
}

namespace Parser
{
	class FMToutputparser;
}


namespace Core
{
	class FMTaction;
	class FMTyields;
	class FMTactualdevelopment;
	class FMTdevelopmentpath;
	class FMToperator;
class FMTEXPORT FMToutputsource : public FMTspec
    {
	friend class boost::serialization::access;
	friend class Parser::FMToutputparser;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
			ar & boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTspec>(*this));
			ar & BOOST_SERIALIZATION_NVP(mask);
			ar & BOOST_SERIALIZATION_NVP(target);
			ar & BOOST_SERIALIZATION_NVP(action);
			ar & BOOST_SERIALIZATION_NVP(yield);
			ar & BOOST_SERIALIZATION_NVP(values);
			ar & BOOST_SERIALIZATION_NVP(themetarget);
	}
    FMTmask mask;
    FMTotar target;
	std::string action,yield;
	std::vector<double>values;
	bool average;
	int outputorigin;
	int themetarget;
	//target =-1 or themetarget constructor
	void pushvalues(const std::vector<double>& newvalues);
    public:
		FMToutputsource();
		~FMToutputsource() = default;
        FMToutputsource(const FMTotar ltarget,double lvalue = 0,
			std::string lyield= "", std::string laction = "",int origin=-1, int ttarget=-1);
        FMToutputsource(const FMTotar ltarget,
			std::vector<double>lvalues, int origin = -1, int ttarget=-1);
        FMToutputsource(const FMTspec& spec,const FMTmask& lmask,
               const FMTotar ltarget, std::string lyield= "",
               std::string laction = "", int origin = -1, int ttarget=-1);
        FMToutputsource(const FMToutputsource& rhs);
        FMToutputsource& operator = (const FMToutputsource& rhs);
        operator std::string() const override;
		bool operator == (const FMToutputsource& rhs) const;
		bool operator != (const FMToutputsource& rhs) const;
		bool operator < (const FMToutputsource& rhs) const;
		inline int getoutputorigin() const
			{
			return outputorigin;
			}
		inline int getthemetarget() const
			{
			return themetarget;
			}
		bool isinaggregate(const FMToutputsource& rhs, const std::vector<Core::FMTaction>& actions) const;
		bool issubsetof(const FMToutputsource& rhs, const std::vector<Core::FMTaction>& actions) const;
		bool issubsetof(const FMToutputsource& rhs) const;
		bool issamebutdifferentaction(const FMToutputsource& rhs) const;
		inline const FMTmask& getmask() const
			{
			return mask;
			}
		void setmask(const FMTmask& newmask);
		void settarget(const FMTotar& newtype);
		void setthemetarget(const int& newttarget);
		void setoutputorigin(const int& neworigin);
		void resetvalues(const FMToperator& op,const FMToutputsource& other);
		inline const std::string& getaction() const
			{
			return action;
			}
		inline const std::string& getyield() const
			{
			return yield;
			}
		inline const FMTotar& gettarget() const
			{
			return target;
			}
		double getvalue(int period = 0) const;
		inline bool isvariable() const
			{
			return bool(mask);
			}
		inline bool islevel() const
			{
			return (target == FMTotar::level);
			}
		inline bool isconstant() const
			{
			return (target == FMTotar::val);
			}
		inline bool isvariablelevel() const
			{
			return (!action.empty() && islevel());
			}
		inline const std::string& getlevel() const
			{
			return yield;
			}
		bool isnull(const FMTyields& ylds) const;
		inline bool istimeyield() const
			{
			return (target == FMTotar::timeyld);
			}
		std::vector<const FMTaction*>targets(const std::vector<FMTaction>& actions) const;
		std::unordered_set<int>targetsset(const std::vector<FMTaction>& actions) const;
		inline bool isinventory() const
			{
			return (target == FMTotar::inventory);
			}
		inline bool useinedges() const
			{
			return (target == FMTotar::inventory);
			}
		inline bool isnextperiod() const
			{
			return (target == FMTotar::inventory && action.empty());
			}
		inline bool useoutedges() const
			{
			return (target == FMTotar::actual);
			}
		double getcoef(const FMTdevelopment& development,
			const FMTyields& yields,
			const Graph::FMTgraphvertextoyield* graphinfo =nullptr,
			std::vector<FMTdevelopmentpath> const * paths=nullptr,
			 FMTaction const * action=nullptr) const;
		inline bool use(const FMTdevelopment& development, const FMTyields& ylds,
			const Graph::FMTgraphvertextoyield* graphinfo=nullptr) const
			{
			return (development.getmask().issubsetof(mask) && development.is(*this, ylds, graphinfo));
			}
		FMToutputsource presolve(const FMTmaskfilter& filter,const std::vector<FMTtheme>& newthemes) const;
		void setaverage();
		inline bool isaverage() const
			{
			return average;
			}
		bool canbededucedtoconstant() const;
		double getconstantvalue(const std::vector<Core::FMTactualdevelopment>& area,
								const std::vector<Core::FMTaction>&actions, const FMTyields& yields) const;
		size_t hash(int period = -1,bool withyield = false) const;
		void fillhashmask(Core::FMTmask& basemask) const;
		void fillhashspec(Core::FMTmask& basemask) const;
		bool isequalbyvalue(const FMToutputsource& rhs) const;
		inline bool isaction() const
			{
			return !action.empty();
			}
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
