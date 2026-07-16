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
class FMTAction;

class FMTEXPORT FMTOutputNode
	{
	public:
		FMTOutputSource source;
		FMTOutputSource factor;
		double constant;
		FMTOutputNode();
		FMTOutputNode(const FMTOutputSource& lsource, const FMTOutputSource& lfactor, const double& lconstant);
		FMTOutputNode(const FMTOutputNode& rhs);
		// DocString: FMTlpheuristic::FMTOutputNode
		/**
		This constructor will create a  general node targeting an action name or actions aggregate.
		It meant to be used with the scheduler heuristic.
		*/
		FMTOutputNode(const Core::FMTMask& generalmask,const std::string& actionaggregate);
		size_t hash() const;
		size_t hashForValue() const;
		bool allowCashdeduction() const;
		bool singlePeriod() const;
		bool multiPeriod() const;
		bool isPastPeriod() const;
		bool isNull() const;
		bool operator < (const FMTOutputNode& rhs) const;
		bool operator == (const FMTOutputNode& rhs) const;
		bool isSubsetOf(const FMTOutputNode& rhs) const;
		bool isSubsetOf(const FMTOutputNode& rhs,const std::vector<Core::FMTAction>& actions) const;
		bool isSameButDifferentAction(const FMTOutputNode& rhs) const;
		inline int getOutputId() const
			{
			return source.getOutputOrigin();
			}
		inline bool isActionbased() const
			{
			return source.isAction();
			}
		bool isSameValues(const FMTOutputNode& rhs) const;
		operator std::string() const;
		std::string getHashString() const;
		Core::FMTMask getHashMask() const;
		void fillHashMaskSpec(Core::FMTMask& baseMask) const;
		FMTOutputNode setPeriod(int period) const;
		FMTOutputNode& operator = (const FMTOutputNode& rhs);
		int setToGraph(std::vector<int>& targetedperiods, int period,int max_period);
		~FMTOutputNode()=default;
		void fillUpEquation(std::map<std::string,std::vector<std::string>>& allequations,
			const std::map<std::string, double>& graphvalues,
			const std::vector<std::string>& equation, const size_t& nodeid) const;
	};

class FMTOutputNodeValueComparator
	{
	public:
		bool operator()(const FMTOutputNode& node1, const FMTOutputNode& node2) const;

	};

class FMTOutputNodeOriginComparator
{
public:
	bool operator()(const FMTOutputNode& node1, const FMTOutputNode& node2) const;

};

class FMTOutputNodeHasher 
	{
	public:
		size_t operator()(const FMTOutputNode & node) const;
	};


}

BOOST_CLASS_EXPORT_KEY(Core::FMTOutputNode)

#endif
