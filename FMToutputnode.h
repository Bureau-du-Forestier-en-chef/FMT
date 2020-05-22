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
		size_t hash() const;
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
		operator std::string() const;
		FMToutputnode setperiod(int period) const;
		FMToutputnode& operator = (const FMToutputnode& rhs);
		int settograph(std::vector<int>& targetedperiods, int period,int max_period);
		~FMToutputnode()=default;
	};

class FMToutputnodehashcomparator
	{
		size_t sourcehash;
	public:
		FMToutputnodehashcomparator(size_t hash);
		bool operator()(const FMToutputnode& node) const;

	};

}

#endif
