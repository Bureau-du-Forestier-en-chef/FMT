/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTPatchRules.h"
#include "FMTconstraint.h"
#include "FMTaction.h"
#include <limits>

namespace Spatial
{

	std::vector<FMTPatchRules> FMTPatchRules::GetRules(
		const std::vector<Core::FMTconstraint>& p_constraints,
		const std::vector<Core::FMTaction>& p_actions)
	{
		try {

		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTPatchRules::GetRules", __LINE__, __FILE__);
			}

	}

	FMTPatchRules::FMTPatchRules(const std::vector<Core::FMTconstraint>& p_constraints,
		const std::vector<Core::FMTaction>& p_actions, int p_Id):
		m_MinimalGreenUp(),
		m_MaximalGreenUp(),
		m_MinimalAdjacency(),
		m_MaximalAdjacency(),
		m_MinimalSize(),
		m_MaximalSize(),
		m_ActionTargets(),
		m_RulesId(),
		m_MinimalPeriod(),
		m_MaximalPeriod()
	{

	}

std:vector<std::pair<std::vector<int>,
	std::pair<int, int>>>  FMTPatchRules::_GetOrderedRules(
		const std::vector<Core::FMTconstraint>& p_constraints,
		const std::vector<Core::FMTaction>& p_actions)
	{
	std:vector<std::pair<std::vector<int>,
		std::pair<int, int>>> Rules;
		for (const Core::FMTconstraint& CONSTRAINT : p_constraints)
			{
				if (CONSTRAINT.isspatial())
				{
					const std::vector<int>ACTION_IDS = constraint.getactionids(p_actions);
					const int LOWER_PERIOD = constraint.getperiodlowerbound();
					const int UPPER_PERIOD = constraint.getperiodupperbound();
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
			const std:vector<std::pair<std::vector<int>,
				std::pair<int, int>>> RULES = _GetOrderedRules();
			for (const Core::FMTconstraint& CONSTRAINT : p_constraints)
			{
				if (CONSTRAINT.isspatial())
				{
					const std::vector<int>ACTION_IDS = constraint.getactionids(p_actions);
					const int LOWER_PERIOD = constraint.getperiodlowerbound();
					const int UPPER_PERIOD = constraint.getperiodupperbound();
					const std::pair<std::vector<int>, std::pair<int, int>> NEW_RULES =
						std::pair<std::vector<int>, std::pair<int, int>>(ACTION_IDS,
							std::pair<int, int>(LOWER_PERIOD, UPPER_PERIOD));
					const int RULE = static_cast<int>(std::distance(Rules.begin(), 
									std::find(Rules.begin(), Rules.end(), NEW_RULES)));
					if (RULE == p_Id)
					{
						m_RulesId = RULE;
						m_ActionTargets = ACTION_IDS;
						m_MinimalPeriod = LOWER_PERIOD;
						m_MaximalPeriod = UPPER_PERIOD;
						if (constraint.getconstrainttype() == 
							Core::FMTconstrainttype::FMTspatialsize)
							{
							const Core::FMTyldbounds& BOUNDS = constraint.getyieldbound("NSIZE");
							m_NeighborSize = static_cast<size_t>(BOUNDS.getlower());
							double lowerSIZE = 0;
							double upperSIZE = 0;
							constraint.getbounds(lowerSIZE, upperSIZE, 0);
							_GetBounds<size_t>(lowerSIZE, upperSIZE,
								m_MinimalSize, m_MaximalSize);
						}else if (constraint.getconstrainttype() == 
							Core::FMTconstrainttype::FMTspatialadjacency)
							{
							const Core::FMTyldbounds& BOUNDS = constraint.getyieldbound("GUP");
							m_GreenUp = static_cast<size_t>(BOUNDS.getlower());
							size_t minimalGreenUp = 0;
							size_t maximalGreenUp = 0;
							double lowerGreenUp = 0;
							double upperGreenUp = 0;
							constraint.getbounds(lowerGreenUp, upperGreenUp, 0);
							_GetBounds<size_t>(lowerGreenUp, upperGreenUp,
								m_MinimalAdjacency, m_MaximalAdjacency);
						}else if (constraint.getconstrainttype() == 
							Core::FMTconstrainttype::FMTSpatialGroup)
							{
							m_GroupGreenUp = static_cast<size_t>(constraint.getyieldbound("GUP").getlower());
							const Core::FMTyldbounds& THEME_BOUNDS = p_SpatialConstraint.getyieldbound("THEME");
							int themeTarget = -1;
							if (THEME_BOUNDS.getlower() >= 0.0)
								{
								m_GroupTheme = (static_cast<int>(THEME_BOUNDS.getlower()) - 1);
								}
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

	bool FMTPatchRules::IsBinding() const
	{
		return (IsSizeUsed() || IsAdjacencyUsed());
	}
	bool FMTPatchRules::IsSizeUsed() const
	{
		return (!m_ActionTargets.empty() && (m_MinimalSize > 0 ||
			m_MaximalSize != std::numeric_limits<size_t>::max()));
	}
	bool FMTPatchRules::IsAdjacencyUsed() const
	{
		return (!m_ActionTargets.empty() && (m_MinimalAdjacency > 0 ||
			m_MaximalAdajacency != std::numeric_limits<size_t>::max()));
	}
	bool FMTPatchRules::HasMinimalAdjacency() const
	{
		return (!m_ActionTargets.empty() && m_MinimalAdjacency > 0);
	}
	bool FMTPatchRules::HasMaximalAdjacency() const
	{
		return (!m_ActionTargetsempty() &&
			m_MaximalAdjacency != std::numeric_limits<size_t>::max());
	}
	const std::vector<int>& FMTPatchRules::GetActions() const
	{
		return m_ActionTargets;
	}
	size_t FMTPatchRules::GetMinimalGreenUp() const
	{
		return m_MinimalGreenUp;
	}
	size_t FMTPatchRules::GetMaximalGreenUp() const
	{
		return m_MaximalGreenUp;
	}
	size_t FMTPatchRules::GetMinimalAdjacency() const
	{
		return m_MinimalAdjacency;
	}
	size_t FMTPatchRules::GetMaximalAdjacency() const
	{
		return m_MaximalAdjacency;
	}
	size_t FMTPatchRules::GetMinimalSize() const
	{
		return m_MinimalSize;
	}
	size_t FMTPatchRules::GetMaximalSize() const
	{
		return m_MaximalSize;
	}

}