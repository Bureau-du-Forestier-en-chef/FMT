/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTPATCHRULES_m_included
#define FMTPATCHRULES_m_included

#include <vector>
#include <set>
#include "FMTobject.h"

namespace Core {
	class FMTConstraint;
	class FMTAction;
}


namespace Spatial

{
	class FMTeventcontainer;
	class FMTevent;
	class FMTSpatialGraphs;
	class FMTPatchRules : public Core::FMTObject
	{
	public:
		FMTPatchRules()=default;
		FMTPatchRules(const FMTPatchRules& rhs) = default;
		FMTPatchRules& operator = (const FMTPatchRules& rhs)=default;
		static std::vector<FMTPatchRules> 
			getRules(const std::vector<Core::FMTConstraint>& p_constraints,
					const std::vector<Core::FMTAction>& p_actions);
		double evaluate(const FMTeventcontainer& p_events, 
				const FMTSpatialGraphs& p_SpatialGraphs) const;
		void fillTooSmallEvents(
			std::vector<std::set<FMTevent>::iterator>& p_SmallEvents,
			FMTeventcontainer& p_events) const;
		void fillTooBigEvents(std::vector<std::set<FMTevent>::iterator>& p_BigEvents,
			FMTeventcontainer& p_events) const;
		void fillDispersionEvents(std::vector<std::set<FMTevent>::iterator>& p_Dispersion,
					FMTeventcontainer& p_events, const FMTSpatialGraphs& p_SpatialGraphs) const;
	private:
		FMTPatchRules(const std::vector<Core::FMTConstraint>& p_constraints,
			const std::vector<Core::FMTAction>& p_actions, int p_Id);
		void _buildPatchRules(const std::vector<Core::FMTConstraint>& p_constraints,
			const std::vector<Core::FMTAction>& p_actions, int p_Id);
		static std::vector<std::pair<std::vector<int>,
						std::pair<int,int>>> _getOrderedRules(const std::vector<Core::FMTConstraint>& p_constraints,
						const std::vector<Core::FMTAction>& p_actions);
		template <typename U>
		static void _getBounds(double p_lower, double p_upper,
			U& p_NewLower, U& p_NewUpper);
		bool _tooSmall(const FMTevent& p_event, size_t& p_cost) const;
		bool _tooBig(const FMTevent& p_event, size_t& p_cost) const;
		bool _isTooClose(const FMTevent& p_event, const FMTeventcontainer& p_events, size_t& p_cost) const;
		bool _isSizeUsed() const;
		bool _isAdjacencyUsed() const;
		bool _hasMinimalAdjacency() const;
		bool _hasMaximalAdjacency() const;
		bool _isGroupUsed() const;
		double _evaluateSize(const FMTeventcontainer& p_events) const;
		double _evaluateAdjacency(const FMTeventcontainer& p_events) const;
		double _evaluateGroup(const FMTeventcontainer& p_events,
			const FMTSpatialGraphs& p_SpatialGraphs) const;
		
	
		size_t m_GreenUp;
		size_t m_MinimalAdjacency;
		size_t m_MaximalAdjacency;
		size_t m_MinimalSize;
		size_t m_MaximalSize;
		size_t m_NeighborSize;
		int m_GroupGreenUp;
		int m_MinimalGroupDistance;
		int m_MaximalGroupDistance;
		std::vector<int>m_ActionTargets;
		int m_GroupTheme;
		int m_RulesId;
		int m_MinimalPeriod;
		int m_MaximalPeriod;
	};



}
#endif
