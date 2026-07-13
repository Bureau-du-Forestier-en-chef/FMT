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

	const std::vector<double>& FMTGraphInfo::getValues(size_t p_ConstraintId) const
	{
		//A slot sized by SetValuesSize but never assigned holds a null pointer; return an
		//empty vector for it, preserving the previous flyweight behaviour (default value = empty).
		static const std::vector<double> EMPTY;
		const std::shared_ptr<const FMTGraphValues>& ptr = m_Values.at(p_ConstraintId);
		return ptr ? ptr->getValues() : EMPTY;
	}

	void FMTGraphInfo::setValues(size_t p_ConstraintId,const std::shared_ptr<const FMTGraphValues>& p_Values)
	{
		m_Values.at(p_ConstraintId) = p_Values;
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


