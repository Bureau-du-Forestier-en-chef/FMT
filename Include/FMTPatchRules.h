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
	// DocString: FMTPatchRules
	/**
	@brief Spatial patch rules used to evaluate and repair the size, adjacency and grouping of events in a spatial schedule.
	*/
	class FMTPatchRules : public Core::FMTObject
	{
	public:
		// DocString: FMTPatchRules()
		/**
		@brief Default constructor for FMTPatchRules.
		*/
		FMTPatchRules()=default;
		// DocString: FMTPatchRules(const FMTPatchRules)
		/**
		@brief Copy constructor for FMTPatchRules.
		@param[in] rhs the FMTPatchRules to copy.
		*/
		FMTPatchRules(const FMTPatchRules& rhs) = default;
		// DocString: FMTPatchRules::operator=
		/**
		@brief Copy assignment operator for FMTPatchRules.
		@param[in] rhs the FMTPatchRules to copy.
		@return a reference to this FMTPatchRules.
		*/
		FMTPatchRules& operator = (const FMTPatchRules& rhs)=default;
		// DocString: FMTPatchRules::getRules
		/**
		@brief Build the patch rules from the spatial constraints and the actions.
		@param[in] p_constraints the constraints.
		@param[in] p_actions the actions.
		@return the patch rules.
		*/
		static std::vector<FMTPatchRules> 
			getRules(const std::vector<Core::FMTConstraint>& p_constraints,
					const std::vector<Core::FMTAction>& p_actions);
		// DocString: FMTPatchRules::evaluate
		/**
		@brief Evaluate the cost of the patch rules on a container of events.
		@param[in] p_events the events.
		@param[in] p_SpatialGraphs the spatial graphs.
		@return the evaluation cost.
		*/
		double evaluate(const FMTEventContainer& p_events, 
				const FMTSpatialGraphs& p_SpatialGraphs) const;
		// DocString: FMTPatchRules::fillTooSmallEvents
		/**
		@brief Fill a vector with the events that are too small.
		@param[in,out] p_SmallEvents the too small events.
		@param[in] p_events the events.
		*/
		void fillTooSmallEvents(
			std::vector<std::set<FMTEvent>::iterator>& p_SmallEvents,
			FMTEventContainer& p_events) const;
		// DocString: FMTPatchRules::fillTooBigEvents
		/**
		@brief Fill a vector with the events that are too big.
		@param[in,out] p_BigEvents the too big events.
		@param[in] p_events the events.
		*/
		void fillTooBigEvents(std::vector<std::set<FMTEvent>::iterator>& p_BigEvents,
			FMTEventContainer& p_events) const;
		// DocString: FMTPatchRules::fillDispersionEvents
		/**
		@brief Fill a vector with the events that break the dispersion (adjacency) rules.
		@param[in,out] p_Dispersion the dispersion events.
		@param[in] p_events the events.
		@param[in] p_SpatialGraphs the spatial graphs.
		*/
		void fillDispersionEvents(std::vector<std::set<FMTEvent>::iterator>& p_Dispersion,
					FMTEventContainer& p_events, const FMTSpatialGraphs& p_SpatialGraphs) const;
	private:
		// DocString: FMTPatchRules(const std::vector<Core::FMTConstraint>&,const std::vector<Core::FMTAction>&,int)
		/**
		@brief Construct a FMTPatchRules from constraints, actions and an id.
		@param[in] p_constraints the constraints.
		@param[in] p_actions the actions.
		@param[in] p_Id the rules id.
		*/
		FMTPatchRules(const std::vector<Core::FMTConstraint>& p_constraints,
			const std::vector<Core::FMTAction>& p_actions, int p_Id);
		// DocString: FMTPatchRules::_buildPatchRules
		/**
		@brief Build the patch rules members from constraints, actions and an id.
		@param[in] p_constraints the constraints.
		@param[in] p_actions the actions.
		@param[in] p_Id the rules id.
		*/
		void _buildPatchRules(const std::vector<Core::FMTConstraint>& p_constraints,
			const std::vector<Core::FMTAction>& p_actions, int p_Id);
		// DocString: FMTPatchRules::_getOrderedRules
		/**
		@brief Return the rules ordered by action targets and period bounds.
		@param[in] p_constraints the constraints.
		@param[in] p_actions the actions.
		@return the ordered rules.
		*/
		static std::vector<std::pair<std::vector<int>,
						std::pair<int,int>>> _getOrderedRules(const std::vector<Core::FMTConstraint>& p_constraints,
						const std::vector<Core::FMTAction>& p_actions);
		// DocString: FMTPatchRules::_getBounds
		/**
		@brief Convert double bounds to the target type bounds.
		@tparam U the bound type.
		@param[in] p_lower the lower bound.
		@param[in] p_upper the upper bound.
		@param[out] p_NewLower the converted lower bound.
		@param[out] p_NewUpper the converted upper bound.
		*/
		template <typename U>
		static void _getBounds(double p_lower, double p_upper,
			U& p_NewLower, U& p_NewUpper);
		// DocString: FMTPatchRules::_tooSmall
		/**
		@brief Return true if an event is too small and set the associated cost.
		@param[in] p_event the event.
		@param[in,out] p_cost the cost.
		@return true if the event is too small else false.
		*/
		bool _tooSmall(const FMTEvent& p_event, size_t& p_cost) const;
		// DocString: FMTPatchRules::_tooBig
		/**
		@brief Return true if an event is too big and set the associated cost.
		@param[in] p_event the event.
		@param[in,out] p_cost the cost.
		@return true if the event is too big else false.
		*/
		bool _tooBig(const FMTEvent& p_event, size_t& p_cost) const;
		// DocString: FMTPatchRules::_isTooClose
		/**
		@brief Return true if an event is too close to another one and set the associated cost.
		@param[in] p_event the event.
		@param[in] p_events the events.
		@param[in,out] p_cost the cost.
		@return true if the event is too close else false.
		*/
		bool _isTooClose(const FMTEvent& p_event, const FMTEventContainer& p_events, size_t& p_cost) const;
		// DocString: FMTPatchRules::_isSizeUsed
		/**
		@brief Return true if the size rules are used.
		@return true if the size rules are used else false.
		*/
		bool _isSizeUsed() const;
		// DocString: FMTPatchRules::_isAdjacencyUsed
		/**
		@brief Return true if the adjacency rules are used.
		@return true if the adjacency rules are used else false.
		*/
		bool _isAdjacencyUsed() const;
		// DocString: FMTPatchRules::_hasMinimalAdjacency
		/**
		@brief Return true if a minimal adjacency is set.
		@return true if a minimal adjacency is set else false.
		*/
		bool _hasMinimalAdjacency() const;
		// DocString: FMTPatchRules::_hasMaximalAdjacency
		/**
		@brief Return true if a maximal adjacency is set.
		@return true if a maximal adjacency is set else false.
		*/
		bool _hasMaximalAdjacency() const;
		// DocString: FMTPatchRules::_isGroupUsed
		/**
		@brief Return true if the group rules are used.
		@return true if the group rules are used else false.
		*/
		bool _isGroupUsed() const;
		// DocString: FMTPatchRules::_evaluateSize
		/**
		@brief Evaluate the size cost of the events.
		@param[in] p_events the events.
		@return the size cost.
		*/
		double _evaluateSize(const FMTEventContainer& p_events) const;
		// DocString: FMTPatchRules::_evaluateAdjacency
		/**
		@brief Evaluate the adjacency cost of the events.
		@param[in] p_events the events.
		@return the adjacency cost.
		*/
		double _evaluateAdjacency(const FMTEventContainer& p_events) const;
		// DocString: FMTPatchRules::_evaluateGroup
		/**
		@brief Evaluate the group cost of the events.
		@param[in] p_events the events.
		@param[in] p_SpatialGraphs the spatial graphs.
		@return the group cost.
		*/
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
