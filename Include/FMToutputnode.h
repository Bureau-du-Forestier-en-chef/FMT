/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTOUTPUTNODE_Hm_included
#define FMTOUTPUTNODE_Hm_included

#include "FMToutputsource.h"
#include <map>

#include "FMTutility.h"

namespace Core
{
class FMTaction;

class FMTEXPORT FMToutputnode
	{
	public:
		FMToutputsource source;
		FMToutputsource factor;
		double constant;
		FMToutputnode();
		FMToutputnode(const FMToutputsource& lsource, const FMToutputsource& lfactor, const double& lconstant);
		FMToutputnode(const FMToutputnode& rhs);
		// DocString: FMTlpheuristic::FMToutputnode
		/**
		This constructor will create a  general node targeting an action name or actions aggregate.
		It meant to be used with the scheduler heuristic.
		*/
		FMToutputnode(const Core::FMTmask& generalmask,const std::string& actionaggregate);
		size_t hash() const;
		size_t hashForValue() const;
		bool allowCashdeduction() const;
		bool singlePeriod() const;
		bool multiPeriod() const;
		bool isPastPeriod() const;
		bool isNull() const;
		bool operator < (const FMToutputnode& rhs) const;
		bool operator == (const FMToutputnode& rhs) const;
		bool isSubsetOf(const FMToutputnode& rhs) const;
		bool isSubsetOf(const FMToutputnode& rhs,const std::vector<Core::FMTaction>& actions) const;
		bool isSameButDifferentAction(const FMToutputnode& rhs) const;
		inline int getOutputId() const
			{
			return source.getOutputOrigin();
			}
		inline bool isActionbased() const
			{
			return source.isAction();
			}
		bool isSameValues(const FMToutputnode& rhs) const;
		operator std::string() const;
		std::string getHashString() const;
		Core::FMTmask getHashMask() const;
		void fillHashMaskSpec(Core::FMTmask& baseMask) const;
		FMToutputnode setPeriod(int period) const;
		FMToutputnode& operator = (const FMToutputnode& rhs);
		int setToGraph(std::vector<int>& targetedperiods, int period,int max_period);
		~FMToutputnode()=default;
		void fillUpEquation(std::map<std::string,std::vector<std::string>>& allequations,
			const std::map<std::string, double>& graphvalues,
			const std::vector<std::string>& equation, const size_t& nodeid) const;
	};

class FMTOutputNodeValueComparator
	{
	public:
		bool operator()(const FMToutputnode& node1, const FMToutputnode& node2) const;

	};

class FMTOutputNodeOriginComparator
{
public:
	bool operator()(const FMToutputnode& node1, const FMToutputnode& node2) const;

};

class FMTOutputNodeHasher 
	{
	public:
		size_t operator()(const FMToutputnode & node) const;
	};


}

BOOST_CLASS_EXPORT_KEY(Core::FMToutputnode)

#endif
