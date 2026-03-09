/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTPATCHRULES_m_included
#define FMTPATCHRULES_m_included

#include <vector>
#include "FMTobject.h"

namespace Core {
	class FMTconstraint;
	class FMTaction;
}


namespace Spatial

{
	class FMTPatchRules : public Core::FMTobject
	{
	public:
		FMTPatchRules()=default;
		FMTPatchRules(const FMTPatchRules& rhs) = default;
		FMTPatchRules& operator = (const FMTPatchRules& rhs)=default;
		bool IsBinding() const;
		bool IsSizeUsed() const;
		bool IsAdjacencyUsed() const;
		bool HasMinimalAdjacency() const;
		bool HasMaximalAdjacency() const;
		const std::vector<int>& GetActions() const;
		size_t GetMinimalGreenUp() const;
		size_t GetMaximalGreenUp() const;
		size_t GetMinimalAdjacency() const;
		size_t GetMaximalAdjacency() const;
		size_t GetMinimalSize() const;
		size_t GetMaximalSize() const;
		static std::vector<FMTPatchRules> GetRules(const std::vector<Core::FMTconstraint>& p_constraints,
											const std::vector<Core::FMTaction>& p_actions);
	private:
		FMTPatchRules(const std::vector<Core::FMTconstraint>& p_constraints,
			const std::vector<Core::FMTaction>& p_actions, int p_Id);
		void _BuildPatchRules(const std::vector<Core::FMTconstraint>& p_constraints,
			const std::vector<Core::FMTaction>& p_actions, int p_Id);
		static std:vector<std::pair<std::vector<int>,
						std::pair<int,int>>> _GetOrderedRules(const std::vector<Core::FMTconstraint>& p_constraints,
						const std::vector<Core::FMTaction>& p_actions);
		template <typename U>
		static void _GetBounds(double p_lower, double p_upper,
			U& p_NewLower, U& p_NewUpper);
		size_t m_GreenUp;
		size_t m_MinimalAdjacency;
		size_t m_MaximalAdjacency;
		size_t m_MinimalSize;
		size_t m_MaximalSize;
		size_t m_NeighborSize;
		size_t m_GroupGreenUp;
		std::vector<int>m_ActionTargets;
		int m_GroupTheme;
		int m_RulesId;
		int m_MinimalPeriod;
		int m_MaximalPeriod;
	};



}
#endif
