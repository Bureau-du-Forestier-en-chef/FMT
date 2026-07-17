/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTPATCHRULES_m_included
#define FMTPATCHRULES_m_included

#include <vector>
#include <set>
#include "FMTObject.h"

namespace Core {
	class FMTConstraint;
	class FMTAction;
}


namespace Spatial

{
	class FMTEventContainer;
	class FMTEvent;
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
		double evaluate(const FMTEventContainer& p_events, 
				const FMTSpatialGraphs& p_SpatialGraphs) const;
		void fillTooSmallEvents(
			std::vector<std::set<FMTEvent>::iterator>& p_SmallEvents,
			FMTEventContainer& p_events) const;
		void fillTooBigEvents(std::vector<std::set<FMTEvent>::iterator>& p_BigEvents,
			FMTEventContainer& p_events) const;
		void fillDispersionEvents(std::vector<std::set<FMTEvent>::iterator>& p_Dispersion,
					FMTEventContainer& p_events, const FMTSpatialGraphs& p_SpatialGraphs) const;
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
		bool _tooSmall(const FMTEvent& p_event, size_t& p_cost) const;
		bool _tooBig(const FMTEvent& p_event, size_t& p_cost) const;
		bool _isTooClose(const FMTEvent& p_event, const FMTEventContainer& p_events, size_t& p_cost) const;
		bool _isSizeUsed() const;
		bool _isAdjacencyUsed() const;
		bool _hasMinimalAdjacency() const;
		bool _hasMaximalAdjacency() const;
		bool _isGroupUsed() const;
		double _evaluateSize(const FMTEventContainer& p_events) const;
		double _evaluateAdjacency(const FMTEventContainer& p_events) const;
		double _evaluateGroup(const FMTEventContainer& p_events,
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
