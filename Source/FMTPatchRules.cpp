/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTPatchRules.h"
#include "FMTConstraint.h"
#include "FMTAction.h"
#include "FMTEventContainer.h"
#include "FMTSpatialGraphs.h"
#include <limits>

namespace Spatial
{

	std::vector<FMTPatchRules> FMTPatchRules::getRules(
		const std::vector<Core::FMTConstraint>& p_constraints,
		const std::vector<Core::FMTAction>& p_actions)
	{
		std::vector<FMTPatchRules> Rules;
		try {
			const std::vector<std::pair<std::vector<int>,
				std::pair<int, int>>> RULES = _getOrderedRules(p_constraints, p_actions);
			int i = 0;
			for (const auto& RULE_ELEMENTS : RULES)
				{
				Rules.push_back(FMTPatchRules(p_constraints, p_actions, i));
				++i;
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTPatchRules::getRules", __LINE__, __FILE__);
			}
		return Rules;
	}

	double FMTPatchRules::evaluate(const FMTEventContainer& p_events,
		const FMTSpatialGraphs& p_SpatialGraphs) const
	{
		double cost = 0;
		try {
			if (_isSizeUsed())
			{
				cost += _evaluateSize(p_events);
			}
			if (_isAdjacencyUsed())
			{
				cost += _evaluateAdjacency(p_events);
			}
			if (_isGroupUsed())
			{
				cost += _evaluateGroup(p_events, p_SpatialGraphs);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTPatchRules::evaluate", __LINE__, __FILE__);
			}
		return cost;
	}

	bool FMTPatchRules::_tooSmall(const FMTEvent& p_event, size_t& p_cost) const
	{
		bool use = false;
		const size_t EVENT_SIZE = p_event.size();
		if (m_MinimalSize > 0 && EVENT_SIZE < m_MinimalSize)
		{
			p_cost = m_MinimalSize - EVENT_SIZE;
			use = true;
		}
		return use;
	}
	bool FMTPatchRules::_tooBig(const FMTEvent& p_event, size_t& p_cost) const
	{
		bool use = false;
		const size_t EVENT_SIZE = p_event.size();
		if (m_MaximalSize < std::numeric_limits<size_t>::max() &&
			EVENT_SIZE > m_MaximalSize)
		{
			p_cost = m_MinimalSize - EVENT_SIZE;
			use = true;
		}
		return use;
	}


	void  FMTPatchRules::fillTooSmallEvents(std::vector<std::set<FMTEvent>::iterator>& p_SmallEvents,
		FMTEventContainer& p_events) const
	{
		if (_isSizeUsed())
			{
			for (const auto& EVENT : p_events.getEventsOf(m_RulesId,
				m_MinimalPeriod, m_MaximalPeriod))
			{
				size_t useLess = 0;
				if (_tooSmall(*EVENT, useLess))
				{
					p_SmallEvents.push_back(EVENT);
				}
			}
			}
	}
	void  FMTPatchRules::fillTooBigEvents(std::vector<std::set<FMTEvent>::iterator>& p_BigEvents,
		FMTEventContainer& p_events) const
	{
		if (_isSizeUsed())
		{
			for (const auto& EVENT : p_events.getEventsOf(m_RulesId,
				m_MinimalPeriod, m_MaximalPeriod))
			{
				size_t useLess = 0;
				if (_tooBig(*EVENT, useLess))
				{
					p_BigEvents.push_back(EVENT);
				}
			}
		}
	}

	void FMTPatchRules::fillDispersionEvents(std::vector<std::set<FMTEvent>::iterator>& p_Dispersion,
		FMTEventContainer& p_events, const FMTSpatialGraphs& p_SpatialGraphs) const
	{
		if (_isGroupUsed())
			{
			for (int period = m_MinimalPeriod; period <= m_MaximalPeriod; ++period)
				{
				const std::vector < std::set<FMTEvent>::iterator > CONFLICTS =
					p_events.getDispersionConflicts(m_RulesId, p_SpatialGraphs,
						m_GroupTheme, period,
						period + m_GroupGreenUp, m_MaximalGroupDistance);
				p_Dispersion.insert(p_Dispersion.end(), CONFLICTS.begin(), CONFLICTS.end());
				}
			}
	}


	double FMTPatchRules::_evaluateSize(const FMTEventContainer& p_events) const
	{
		double cost = 0;
		try {
			if (_isSizeUsed())
				{
					for (const auto& EVENT : p_events.getEventsOf(m_RulesId,
						m_MinimalPeriod, m_MaximalPeriod))
					{
						size_t EventCost = 0;
						if (!_tooSmall(*EVENT, EventCost))
						{
							_tooBig(*EVENT, EventCost);
						}
						cost += static_cast<double>(EventCost);
					}
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTPatchRules::_evaluateSize", __LINE__, __FILE__);
			}
		return cost;
	}

	bool FMTPatchRules::_isTooClose(const FMTEvent& p_event, const FMTEventContainer& p_events, size_t& p_cost) const
		{
		bool TooClose = false;
		if (_hasMinimalAdjacency())
			{
			for (int period = p_event.getPeriod(); period <= p_event.getPeriod() + m_GreenUp; ++period)
				{
				for (const FMTEventContainer::const_iterator eventIt : p_events.getEvents(period, m_RulesId))
					{
					if (*eventIt != p_event &&
						p_event.within(m_MinimalAdjacency, *eventIt))
						{
						p_cost += m_MinimalAdjacency - p_event.distance(*eventIt);
						TooClose = true;
						}
					}
				}
			}
		return TooClose;
		}


	double FMTPatchRules::_evaluateAdjacency(const FMTEventContainer& p_events) const
	{
		double cost = 0;
		try {
			if (_isAdjacencyUsed())
				{
				for (int period = m_MinimalPeriod; period <= m_MaximalPeriod; ++period)
					{
					for (const FMTEventContainer::const_iterator eventIt : p_events.getEvents(period, m_RulesId))
						{
						
						}

					}
				}
			
			/*if (_IsAdjacencyUsed())
			{
				int baselookup = 0;
				double lower = 0.0;
				double upper = 0.0;
				if (p_testLower)
				{
					baselookup = p_lowerLookup;
					lower = static_cast<double>(p_lowerLookup);
				}
				else
				{
					baselookup = std::max(baselookup, p_upperLookup);
					upper = static_cast<double>(p_upperLookup);
				}


				for (const FMTEventContainer::const_iterator eventit : m_events.getEvents(p_period, p_actions))
				{
					const uint16_t containerlookup = static_cast<uint16_t>(baselookup + eventit->size());

					//0//-//1//
					//-//-//-//
					//2//-//3//
					const std::array<FMTCoordinate, 4> enveloppe = eventit->getEnveloppe();
					const uint16_t minimalx = containerlookup < enveloppe.at(0).getX() ? enveloppe.at(0).getX() - containerlookup : 0;
					const uint16_t minimaly = containerlookup < enveloppe.at(0).getY() ? enveloppe.at(0).getY() - containerlookup : 0;
					const uint16_t maximalx = enveloppe.at(3).getX() + containerlookup;
					const uint16_t maximaly = enveloppe.at(3).getY() + containerlookup;
					const FMTCoordinate minimalcoord(minimalx, minimaly);
					const FMTCoordinate maximalcoord(maximalx, maximaly);
					double totalwithincount = 0;
					for (int gupperiod = std::max(1, p_period - p_greenup); gupperiod <= p_period; ++gupperiod)
					{
						const double periodfactor = static_cast<double>((p_greenup - (p_period - gupperiod))) + 1;
						for (const FMTEventContainer::const_iterator eventof : m_events.getEvents(gupperiod, p_actions, minimalcoord, maximalcoord))
						{
							if (eventit != eventof)//They will have the same address if it's the same event!
							{
								if (p_testLower && eventit->within(static_cast<size_t>(p_lowerLookup), *eventof)) //too close
								{
									returnvalue += ((lower - eventit->distance(*eventof)) * periodfactor);
									//conflicts.push_back(eventof);
								}
								else if (!p_testLower && !eventit->within(static_cast<size_t>(p_upperLookup), *eventof))
								{
									returnvalue += ((eventit->distance(*eventof) - upper) * periodfactor);
									//conflicts.push_back(eventof);
								}

							}
						}
					}
				}
			}*/
			
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTPatchRules::_evaluateAdjacency", __LINE__, __FILE__);
			}
		return cost;
	}
	double FMTPatchRules::_evaluateGroup(const FMTEventContainer& p_events,
		const FMTSpatialGraphs& p_SpatialGraphs) const
	{
		double cost = 0;
		try {
			if (_isGroupUsed())
			{
				for (int period = m_MinimalPeriod; period <= m_MaximalPeriod; ++period)
				{
					cost += p_events.getDispersion(m_RulesId, p_SpatialGraphs,
						m_GroupTheme, period,
						period + m_GroupGreenUp, m_MaximalGroupDistance);
				}
			}
			
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTPatchRules::_evaluateGroup", __LINE__, __FILE__);
			}
		return cost;
	}


	FMTPatchRules::FMTPatchRules(const std::vector<Core::FMTConstraint>& p_constraints,
		const std::vector<Core::FMTAction>& p_actions, int p_Id):
		m_GreenUp(),
		m_MinimalAdjacency(),
		m_MaximalAdjacency(),
		m_MinimalSize(),
		m_MaximalSize(),
		m_NeighborSize(),
		m_GroupGreenUp(),
		m_MinimalGroupDistance(),
		m_MaximalGroupDistance(),
		m_ActionTargets(),
		m_GroupTheme(-1),
		m_RulesId(),
		m_MinimalPeriod(),
		m_MaximalPeriod()
		{
		_buildPatchRules(p_constraints, p_actions, p_Id);
		}

std::vector<std::pair<std::vector<int>,
	std::pair<int, int>>>  FMTPatchRules::_getOrderedRules(
		const std::vector<Core::FMTConstraint>& p_constraints,
		const std::vector<Core::FMTAction>& p_actions)
	{
	std::vector<std::pair<std::vector<int>,
		std::pair<int, int>>> Rules;
		for (const Core::FMTConstraint& CONSTRAINT : p_constraints)
			{
				if (CONSTRAINT.isSpatial())
				{
					const std::vector<int>ACTION_IDS = CONSTRAINT.getActionIds(p_actions);
					const int LOWER_PERIOD = CONSTRAINT.getPeriodLowerBound();
					const int UPPER_PERIOD = CONSTRAINT.getPeriodUpperBound();
					const std::pair<std::vector<int>, std::pair<int, int>> NEW_RULES =
						std::pair<std::vector<int>, std::pair<int, int>>(ACTION_IDS,
							std::pair<int, int>(LOWER_PERIOD, UPPER_PERIOD));
					if (std::find(Rules.begin(), Rules.end(), NEW_RULES) == Rules.end())
					{
						Rules.push_back(NEW_RULES);
					}
				}
			}
		return  Rules;
	}

	template <typename U>
	void  FMTPatchRules::_getBounds(double p_lower, double p_upper,
		U& p_NewLower, U& p_NewUpper)
	{
		if (p_lower == std::numeric_limits<double>::lowest())
			{
			p_NewLower = 0;
		}else {
			p_NewLower = static_cast<U>(p_lower);
			}
		if (p_upper == std::numeric_limits<double>::max())
		{
			p_NewUpper = std::numeric_limits<U>::max();
		}
		else {
			p_NewUpper = static_cast<U>(p_upper);
		}
	}
	



	void FMTPatchRules::_buildPatchRules(
		const std::vector<Core::FMTConstraint>& p_constraints,
		const std::vector<Core::FMTAction>& p_actions, int p_Id)
	{
		try {
			const std::vector<std::pair<std::vector<int>,
				std::pair<int, int>>> RULES = _getOrderedRules(p_constraints, p_actions);
			for (const Core::FMTConstraint& CONSTRAINT : p_constraints)
			{
				if (CONSTRAINT.isSpatial())
				{
					const std::vector<int>ACTION_IDS = CONSTRAINT.getActionIds(p_actions);
					const int LOWER_PERIOD = CONSTRAINT.getPeriodLowerBound();
					const int UPPER_PERIOD = CONSTRAINT.getPeriodUpperBound();
					const std::pair<std::vector<int>, std::pair<int, int>> NEW_RULES =
						std::pair<std::vector<int>, std::pair<int, int>>(ACTION_IDS,
							std::pair<int, int>(LOWER_PERIOD, UPPER_PERIOD));
					const int RULE = static_cast<int>(std::distance(RULES.begin(),
									std::find(RULES.begin(), RULES.end(), NEW_RULES)));
					if (RULE == p_Id)
					{
						m_RulesId = RULE;
						m_ActionTargets = ACTION_IDS;
						m_MinimalPeriod = LOWER_PERIOD;
						m_MaximalPeriod = UPPER_PERIOD;
						if (CONSTRAINT.getConstraintType() ==
							Core::FMTconstrainttype::FMTspatialsize)
							{
							const Core::FMTYldBounds& BOUNDS = CONSTRAINT.getYieldBound("NSIZE");
							m_NeighborSize = static_cast<size_t>(BOUNDS.getLower());
							double lowerSIZE = 0;
							double upperSIZE = 0;
							CONSTRAINT.getBounds(lowerSIZE, upperSIZE, 0);
							_getBounds<size_t>(lowerSIZE, upperSIZE,
								m_MinimalSize, m_MaximalSize);
						}else if (CONSTRAINT.getConstraintType() ==
							Core::FMTconstrainttype::FMTspatialadjacency)
							{
							const Core::FMTYldBounds& BOUNDS = CONSTRAINT.getYieldBound("GUP");
							m_GreenUp = static_cast<size_t>(BOUNDS.getLower());
							size_t minimalGreenUp = 0;
							size_t maximalGreenUp = 0;
							double lowerGreenUp = 0;
							double upperGreenUp = 0;
							CONSTRAINT.getBounds(lowerGreenUp, upperGreenUp, 0);
							_getBounds<size_t>(lowerGreenUp, upperGreenUp,
								m_MinimalAdjacency, m_MaximalAdjacency);
						}else if (CONSTRAINT.getConstraintType() ==
							Core::FMTconstrainttype::FMTSpatialGroup)
							{
							m_GroupGreenUp = static_cast<int>(CONSTRAINT.getYieldBound("GUP").getLower());
							const Core::FMTYldBounds& THEME_BOUNDS = CONSTRAINT.getYieldBound("THEME");
							int themeTarget = -1;
							if (THEME_BOUNDS.getLower() >= 0.0)
								{
								m_GroupTheme = (static_cast<int>(THEME_BOUNDS.getLower()) - 1);
								}
							double lowerDistance = 0;
							double upperDistance = 0;
							CONSTRAINT.getBounds(lowerDistance, upperDistance, 0);
							_getBounds<int>(lowerDistance, upperDistance,
								m_MinimalGroupDistance, m_MaximalGroupDistance);
							}
					}
					

				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTPatchRules::_buildPatchRules", __LINE__, __FILE__);
			}
	}


	bool FMTPatchRules::_isSizeUsed() const
	{
		return (!m_ActionTargets.empty() && (m_MinimalSize > 0 ||
			m_MaximalSize != std::numeric_limits<size_t>::max()));
	}
	bool FMTPatchRules::_isAdjacencyUsed() const
	{
		return (!m_ActionTargets.empty() && (m_MinimalAdjacency > 0 ||
			m_MaximalAdjacency != std::numeric_limits<size_t>::max()));
	}
	bool FMTPatchRules::_hasMinimalAdjacency() const
	{
		return (!m_ActionTargets.empty() && m_MinimalAdjacency > 0);
	}
	bool FMTPatchRules::_hasMaximalAdjacency() const
	{
		return (!m_ActionTargets.empty() &&
			m_MaximalAdjacency != std::numeric_limits<size_t>::max());
	}

	bool FMTPatchRules::_isGroupUsed() const
	{
		return m_GroupTheme >= 0;
	}
	

}