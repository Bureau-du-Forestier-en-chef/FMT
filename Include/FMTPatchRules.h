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
	class FMTconstraint;
	class FMTaction;
}


namespace Spatial

{
	class FMTeventcontainer;
	class FMTevent;
	class FMTSpatialGraphs;
	class FMTPatchRules : public Core::FMTobject
	{
	public:
		FMTPatchRules()=default;
		FMTPatchRules(const FMTPatchRules& rhs) = default;
		FMTPatchRules& operator = (const FMTPatchRules& rhs)=default;
		static std::vector<FMTPatchRules> 
			GetRules(const std::vector<Core::FMTconstraint>& p_constraints,
					const std::vector<Core::FMTaction>& p_actions);
		double Evaluate(const FMTeventcontainer& p_events, 
				const FMTSpatialGraphs& p_SpatialGraphs) const;
		void FillTooSmallEvents(
			std::vector<std::set<FMTevent>::iterator>& p_SmallEvents,
			FMTeventcontainer& p_events) const;
		void FillTooBigEvents(std::vector<std::set<FMTevent>::iterator>& p_BigEvents,
			FMTeventcontainer& p_events) const;
		void FillDispertionEvents(std::vector<std::set<FMTevent>::iterator>& p_Dispertion,
					FMTeventcontainer& p_events, const FMTSpatialGraphs& p_SpatialGraphs) const;
	private:
		FMTPatchRules(const std::vector<Core::FMTconstraint>& p_constraints,
			const std::vector<Core::FMTaction>& p_actions, int p_Id);
		void _BuildPatchRules(const std::vector<Core::FMTconstraint>& p_constraints,
			const std::vector<Core::FMTaction>& p_actions, int p_Id);
		static std::vector<std::pair<std::vector<int>,
						std::pair<int,int>>> _GetOrderedRules(const std::vector<Core::FMTconstraint>& p_constraints,
						const std::vector<Core::FMTaction>& p_actions);
		template <typename U>
		static void _GetBounds(double p_lower, double p_upper,
			U& p_NewLower, U& p_NewUpper);
		bool _TooSmall(const FMTevent& p_event, size_t& p_cost) const;
		bool _TooBig(const FMTevent& p_event, size_t& p_cost) const;
		bool _IsSizeUsed() const;
		bool _IsAdjacencyUsed() const;
		bool _HasMinimalAdjacency() const;
		bool _HasMaximalAdjacency() const;
		bool _IsGroupUsed() const;
		double _EvaluateSize(const FMTeventcontainer& p_events,
				const FMTSpatialGraphs& p_SpatialGraphs) const;
		double _EvaluateAdjacency(const FMTeventcontainer& p_events,
			const FMTSpatialGraphs& p_SpatialGraphs) const;
		double _EvaluateGroup(const FMTeventcontainer& p_events,
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
