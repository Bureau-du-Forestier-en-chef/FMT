/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTOUTPUTNODE_H_INCLUDED
#define FMTOUTPUTNODE_H_INCLUDED

#include "FMToutputsource.h"
#include "FMToperator.h"
#include "FMTaction.h"
#include <map>

namespace Core
{
class FMToutputnode
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
		size_t hashforvalue() const;
		bool allowcashdeduction() const;
		bool singleperiod() const;
		bool multiperiod() const;
		bool ispastperiod() const;
		bool isnull() const;
		bool operator < (const FMToutputnode& rhs) const;
		bool operator == (const FMToutputnode& rhs) const;
		bool issubsetof(const FMToutputnode& rhs) const;
		bool issubsetof(const FMToutputnode& rhs,const std::vector<Core::FMTaction>& actions) const;
		bool issamebutdifferentaction(const FMToutputnode& rhs) const;
		inline bool isactionbased() const
			{
			return source.isaction();
			}
		bool issamevalues(const FMToutputnode& rhs) const;
		operator std::string() const;
		std::string gethashstring() const;
		Core::FMTmask gethashmask() const;
		void fillhashmaskspec(Core::FMTmask& basemask) const;
		FMToutputnode setperiod(int period) const;
		FMToutputnode& operator = (const FMToutputnode& rhs);
		int settograph(std::vector<int>& targetedperiods, int period,int max_period);
		~FMToutputnode()=default;
	};

class FMToutputnodevaluecomparator
	{
	public:
		bool operator()(const FMToutputnode& node1, const FMToutputnode& node2) const;

	};

class FMToutputnodehasher 
	{
	public:
		size_t operator()(const FMToutputnode & node) const;
	};


}

#endif
