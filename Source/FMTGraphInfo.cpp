/*
Copyright (c) 2019 Gouvernement du Qubec

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

	const std::vector<double>& FMTGraphInfo::getValues(size_t p_ConstraintId) const
	{
		return m_Values.at(p_ConstraintId).get().getValues();
	}

	void FMTGraphInfo::setValues(size_t p_ConstraintId,const std::vector<double>& p_Values)
	{
		m_Values.at(p_ConstraintId) = FMTGraphValues(p_Values);
	}

	void FMTGraphInfo::setValuesSize(size_t p_ValuesSize)
	{
		m_Values.resize(p_ValuesSize);
	}

	bool FMTGraphInfo::isEmpty() const
		{
		return m_Values.empty();
		}

}


