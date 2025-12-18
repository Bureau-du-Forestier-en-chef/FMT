/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/



#include "FMTGraphInfo.h"
#include "FMTGraphValues.h"



namespace Spatial
	{

	FMTGraphInfo::FMTGraphInfo(size_t Id):
		m_GraphId(Id), m_Values()
	{

	}

	const std::vector<double>& FMTGraphInfo::GetValues(size_t p_ConstraintId) const
	{
		return m_Values.at(p_ConstraintId).get().GetValues();
	}
	size_t FMTGraphInfo::GetGraphId() const
	{
		return m_GraphId;
	}
	void FMTGraphInfo::SetValues(size_t p_ConstraintId,const std::vector<double>& p_Values)
	{
		m_Values.at(p_ConstraintId) = FMTGraphValues(p_Values);
	}

	void FMTGraphInfo::SetValuesSize(size_t p_ValuesSize)
	{
		m_Values.resize(p_ValuesSize);
	}

	bool FMTGraphInfo::IsEmpty() const
		{
		return m_Values.empty();
		}

}


