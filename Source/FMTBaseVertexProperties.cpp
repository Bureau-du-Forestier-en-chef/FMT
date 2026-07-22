/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTBaseVertexProperties.h"
#include "FMTFuturDevelopment.h"
#include "FMTActualDevelopment.h"
#include "FMTDevelopment.h"

namespace Graph
{
	
	int FMTBaseVertexProperties::getConstraintId() const
		{
		return 0;
		}

	const Core::FMTDevelopment& FMTBaseVertexProperties::get() const
		{
		return *m_development;
		}

	bool FMTBaseVertexProperties::operator < (const FMTBaseVertexProperties& rhs) const
	{
		if (m_development->getPeriod() < rhs.m_development->getPeriod())
		{
			return true;
		}
		if (*m_development < *rhs.m_development)
		{
			return true;
		}
		return false;
	}
	
	FMTBaseVertexProperties::FMTBaseVertexProperties(const FMTBaseVertexProperties& rhs) :
		m_development(rhs.m_development->Clone())
	{


	}
	FMTBaseVertexProperties& FMTBaseVertexProperties::operator = (const FMTBaseVertexProperties& rhs)
	{
		if (this != &rhs)
		{
			m_development = rhs.m_development->Clone();
		}
		return *this;
	}
	
	FMTBaseVertexProperties::FMTBaseVertexProperties(const Core::FMTFuturDevelopment& p_development) :
		m_development( new Core::FMTFuturDevelopment(p_development))
		{

		}
	FMTBaseVertexProperties::FMTBaseVertexProperties(const Core::FMTActualDevelopment& p_development) :
		m_development(new Core::FMTActualDevelopment(p_development))
		{

		}

	void FMTBaseVertexProperties::setDevlopementMask(const Core::FMTMask& p_newmask)
	{
		m_development->setMask(p_newmask);
	}

	double FMTBaseVertexProperties::getBaseRhs() const
	{
		return m_development->getArea();
	}

	bool FMTBaseVertexProperties::operator == (const FMTBaseVertexProperties& rhs) const
	{
		return (*m_development == *rhs.m_development);
	}
	bool FMTBaseVertexProperties::operator != (const FMTBaseVertexProperties& rhs) const
	{
		return (!(*this == rhs));
	}

	size_t FMTBaseVertexProperties::hash() const
	{
		return m_development->hash();
	}


	void FMTBaseVertexProperties::_save(double p_area)
		{
		if (p_area > 0)
			{
				m_development = std::unique_ptr<Core::FMTDevelopment>(new Core::FMTActualDevelopment(*m_development, p_area));
			}else {
				m_development = std::unique_ptr<Core::FMTDevelopment>(new Core::FMTFuturDevelopment(*m_development));
				}
		}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTBaseVertexProperties)
