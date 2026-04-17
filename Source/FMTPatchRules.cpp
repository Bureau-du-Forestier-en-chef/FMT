/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTPatchRules.h"
#include "FMTconstraint.h"
#include "FMTaction.h"
#include "FMTeventcontainer.h"
#include "FMTSpatialGraphs.h"
#include <limits>

namespace Spatial
{

	std::vector<FMTPatchRules> FMTPatchRules::GetRules(
		const std::vector<Core::FMTconstraint>& p_constraints,
		const std::vector<Core::FMTaction>& p_actions)
	{
		std::vector<FMTPatchRules> Rules;
		try {
			const std::vector<std::pair<std::vector<int>,
				std::pair<int, int>>> RULES = _GetOrderedRules(p_constraints, p_actions);
			int i = 0;
			for (const auto& RULE_ELEMENTS : RULES)
				{
				Rules.push_back(FMTPatchRules(p_constraints, p_actions, i));
				++i;
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTPatchRules::GetRules", __LINE__, __FILE__);
			}
		return Rules;
	}

	double FMTPatchRules::Evaluate(const FMTeventcontainer& p_events,
		const FMTSpatialGraphs& p_SpatialGraphs) const
	{
		double cost = 0;
		try {
			if (_IsSizeUsed())
			{
				cost += _EvaluateSize(p_events);
			}
			if (_IsAdjacencyUsed())
			{
				cost += _EvaluateAdjacency(p_events);
			}
			if (_IsGroupUsed())
			{
				cost += _EvaluateGroup(p_events, p_SpatialGraphs);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTPatchRules::Evaluate", __LINE__, __FILE__);
			}
		return cost;
	}

	bool FMTPatchRules::_TooSmall(const FMTevent& p_event, size_t& p_cost) const
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
	bool FMTPatchRules::_TooBig(const FMTevent& p_event, size_t& p_cost) const
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


	void  FMTPatchRules::FillTooSmallEvents(std::vector<std::set<FMTevent>::iterator>& p_SmallEvents,
		FMTeventcontainer& p_events) const
	{
		if (_IsSizeUsed())
			{
			for (const auto& EVENT : p_events.GetEventsOf(m_RulesId,
				m_MinimalPeriod, m_MaximalPeriod))
			{
				size_t useLess = 0;
				if (_TooSmall(*EVENT, useLess))
				{
					p_SmallEvents.push_back(EVENT);
				}
			}
			}
	}
	void  FMTPatchRules::FillTooBigEvents(std::vector<std::set<FMTevent>::iterator>& p_BigEvents,
		FMTeventcontainer& p_events) const
	{
		if (_IsSizeUsed())
		{
			for (const auto& EVENT : p_events.GetEventsOf(m_RulesId,
				m_MinimalPeriod, m_MaximalPeriod))
			{
				size_t useLess = 0;
				if (_TooBig(*EVENT, useLess))
				{
					p_BigEvents.push_back(EVENT);
				}
			}
		}
	}

	void FMTPatchRules::FillDispertionEvents(std::vector<std::set<FMTevent>::iterator>& p_Dispertion,
		FMTeventcontainer& p_events, const FMTSpatialGraphs& p_SpatialGraphs) const
	{
		if (_IsGroupUsed())
			{
			for (int period = m_MinimalPeriod; period <= m_MaximalPeriod; ++period)
				{
				const std::vector < std::set<FMTevent>::iterator > CONFLICTS =
					p_events.GetDispertionConflicts(m_RulesId, p_SpatialGraphs,
						m_GroupTheme, period,
						period + m_GroupGreenUp, m_MaximalGroupDistance);
				p_Dispertion.insert(p_Dispertion.end(), CONFLICTS.begin(), CONFLICTS.end());
				}
			}
	}


	double FMTPatchRules::_EvaluateSize(const FMTeventcontainer& p_events) const
	{
		double cost = 0;
		try {
			if (_IsSizeUsed())
				{
					for (const auto& EVENT : p_events.GetEventsOf(m_RulesId,
						m_MinimalPeriod, m_MaximalPeriod))
					{
						size_t EventCost = 0;
						if (!_TooSmall(*EVENT, EventCost))
						{
							_TooBig(*EVENT, EventCost);
						}
						cost += static_cast<double>(EventCost);
					}
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTPatchRules::_EvaluateSize", __LINE__, __FILE__);
			}
		return cost;
	}

	bool FMTPatchRules::_IsTooClose(const FMTevent& p_event, const FMTeventcontainer& p_events, size_t& p_cost) const
		{
		bool TooClose = false;
		if (_HasMinimalAdjacency())
			{
			for (int period = p_event.getperiod(); period <= p_event.getperiod() + m_GreenUp; ++period)
				{
				for (const FMTeventcontainer::const_iterator eventIt : p_events.getevents(period, m_RulesId))
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


	double FMTPatchRules::_EvaluateAdjacency(const FMTeventcontainer& p_events) const
	{
		double cost = 0;
		try {
			if (_IsAdjacencyUsed())
				{
				for (int period = m_MinimalPeriod; period <= m_MaximalPeriod; ++period)
					{
					for (const FMTeventcontainer::const_iterator eventIt : p_events.getevents(period, m_RulesId))
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


				for (const FMTeventcontainer::const_iterator eventit : m_events.getevents(p_period, p_actions))
				{
					const uint16_t containerlookup = static_cast<uint16_t>(baselookup + eventit->size());

					//0//-//1//
					//-//-//-//
					//2//-//3//
					const std::array<FMTcoordinate, 4> enveloppe = eventit->getEnveloppe();
					const uint16_t minimalx = containerlookup < enveloppe.at(0).getx() ? enveloppe.at(0).getx() - containerlookup : 0;
					const uint16_t minimaly = containerlookup < enveloppe.at(0).gety() ? enveloppe.at(0).gety() - containerlookup : 0;
					const uint16_t maximalx = enveloppe.at(3).getx() + containerlookup;
					const uint16_t maximaly = enveloppe.at(3).gety() + containerlookup;
					const FMTcoordinate minimalcoord(minimalx, minimaly);
					const FMTcoordinate maximalcoord(maximalx, maximaly);
					double totalwithincount = 0;
					for (int gupperiod = std::max(1, p_period - p_greenup); gupperiod <= p_period; ++gupperiod)
					{
						const double periodfactor = static_cast<double>((p_greenup - (p_period - gupperiod))) + 1;
						for (const FMTeventcontainer::const_iterator eventof : m_events.getevents(gupperiod, p_actions, minimalcoord, maximalcoord))
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
			_exhandler->raisefromcatch("",
				"FMTPatchRules::_EvaluateAdjacency", __LINE__, __FILE__);
			}
		return cost;
	}
	double FMTPatchRules::_EvaluateGroup(const FMTeventcontainer& p_events,
		const FMTSpatialGraphs& p_SpatialGraphs) const
	{
		double cost = 0;
		try {
			if (_IsGroupUsed())
			{
				for (int period = m_MinimalPeriod; period <= m_MaximalPeriod; ++period)
				{
					cost += p_events.GetDispertion(m_RulesId, p_SpatialGraphs,
						m_GroupTheme, period,
						period + m_GroupGreenUp, m_MaximalGroupDistance);
				}
			}
			
		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTPatchRules::_EvaluateGroup", __LINE__, __FILE__);
			}
		return cost;
	}


	FMTPatchRules::FMTPatchRules(const std::vector<Core::FMTconstraint>& p_constraints,
		const std::vector<Core::FMTaction>& p_actions, int p_Id):
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
		_BuildPatchRules(p_constraints, p_actions, p_Id);
		}

std::vector<std::pair<std::vector<int>,
	std::pair<int, int>>>  FMTPatchRules::_GetOrderedRules(
		const std::vector<Core::FMTconstraint>& p_constraints,
		const std::vector<Core::FMTaction>& p_actions)
	{
	std::vector<std::pair<std::vector<int>,
		std::pair<int, int>>> Rules;
		for (const Core::FMTconstraint& CONSTRAINT : p_constraints)
			{
				if (CONSTRAINT.isspatial())
				{
					const std::vector<int>ACTION_IDS = CONSTRAINT.getactionids(p_actions);
					const int LOWER_PERIOD = CONSTRAINT.getperiodlowerbound();
					const int UPPER_PERIOD = CONSTRAINT.getperiodupperbound();
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
	void  FMTPatchRules::_GetBounds(double p_lower, double p_upper,
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
	



	void FMTPatchRules::_BuildPatchRules(
		const std::vector<Core::FMTconstraint>& p_constraints,
		const std::vector<Core::FMTaction>& p_actions, int p_Id)
	{
		try {
			const std::vector<std::pair<std::vector<int>,
				std::pair<int, int>>> RULES = _GetOrderedRules(p_constraints, p_actions);
			for (const Core::FMTconstraint& CONSTRAINT : p_constraints)
			{
				if (CONSTRAINT.isspatial())
				{
					const std::vector<int>ACTION_IDS = CONSTRAINT.getactionids(p_actions);
					const int LOWER_PERIOD = CONSTRAINT.getperiodlowerbound();
					const int UPPER_PERIOD = CONSTRAINT.getperiodupperbound();
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
						if (CONSTRAINT.getconstrainttype() ==
							Core::FMTconstrainttype::FMTspatialsize)
							{
							const Core::FMTyldbounds& BOUNDS = CONSTRAINT.getyieldbound("NSIZE");
							m_NeighborSize = static_cast<size_t>(BOUNDS.getlower());
							double lowerSIZE = 0;
							double upperSIZE = 0;
							CONSTRAINT.getbounds(lowerSIZE, upperSIZE, 0);
							_GetBounds<size_t>(lowerSIZE, upperSIZE,
								m_MinimalSize, m_MaximalSize);
						}else if (CONSTRAINT.getconstrainttype() ==
							Core::FMTconstrainttype::FMTspatialadjacency)
							{
							const Core::FMTyldbounds& BOUNDS = CONSTRAINT.getyieldbound("GUP");
							m_GreenUp = static_cast<size_t>(BOUNDS.getlower());
							size_t minimalGreenUp = 0;
							size_t maximalGreenUp = 0;
							double lowerGreenUp = 0;
							double upperGreenUp = 0;
							CONSTRAINT.getbounds(lowerGreenUp, upperGreenUp, 0);
							_GetBounds<size_t>(lowerGreenUp, upperGreenUp,
								m_MinimalAdjacency, m_MaximalAdjacency);
						}else if (CONSTRAINT.getconstrainttype() ==
							Core::FMTconstrainttype::FMTSpatialGroup)
							{
							m_GroupGreenUp = static_cast<int>(CONSTRAINT.getyieldbound("GUP").getlower());
							const Core::FMTyldbounds& THEME_BOUNDS = CONSTRAINT.getyieldbound("THEME");
							int themeTarget = -1;
							if (THEME_BOUNDS.getlower() >= 0.0)
								{
								m_GroupTheme = (static_cast<int>(THEME_BOUNDS.getlower()) - 1);
								}
							double lowerDistance = 0;
							double upperDistance = 0;
							CONSTRAINT.getbounds(lowerDistance, upperDistance, 0);
							_GetBounds<int>(lowerDistance, upperDistance,
								m_MinimalGroupDistance, m_MaximalGroupDistance);
							}
					}
					

				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTPatchRules::_BuildPatchRules", __LINE__, __FILE__);
			}
	}


	bool FMTPatchRules::_IsSizeUsed() const
	{
		return (!m_ActionTargets.empty() && (m_MinimalSize > 0 ||
			m_MaximalSize != std::numeric_limits<size_t>::max()));
	}
	bool FMTPatchRules::_IsAdjacencyUsed() const
	{
		return (!m_ActionTargets.empty() && (m_MinimalAdjacency > 0 ||
			m_MaximalAdjacency != std::numeric_limits<size_t>::max()));
	}
	bool FMTPatchRules::_HasMinimalAdjacency() const
	{
		return (!m_ActionTargets.empty() && m_MinimalAdjacency > 0);
	}
	bool FMTPatchRules::_HasMaximalAdjacency() const
	{
		return (!m_ActionTargets.empty() &&
			m_MaximalAdjacency != std::numeric_limits<size_t>::max());
	}

	bool FMTPatchRules::_IsGroupUsed() const
	{
		return m_GroupTheme >= 0;
	}
	

}