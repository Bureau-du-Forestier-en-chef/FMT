/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTbasevertexproperties.h"
#include "FMTfuturdevelopment.h"

namespace Graph
{
	int FMTbasevertexproperties::getConstraintId() const
		{
		return 0;
		}

	const Core::FMTdevelopment& FMTbasevertexproperties::get() const
		{
		return m_development;
		}

	bool FMTbasevertexproperties::operator < (const FMTbasevertexproperties& rhs) const
	{
		if (m_development.getperiod() < rhs.m_development.getperiod())
		{
			return true;
		}
		if (m_development < rhs.m_development)
		{
			return true;
		}
		return false;
	}

	FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTfuturdevelopment& p_development) :
		m_development(p_development,0.0)
		{

		}
	FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTactualdevelopment& p_development) :
		m_development(p_development)
	{

	}
	/*FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTdevelopment& p_development) :
		m_development(p_development,0.0)
	{

	}*/

	void FMTbasevertexproperties::setDevlopementMask(const Core::FMTmask& p_newmask)
	{
		m_development.setmask(p_newmask);
	}

	double FMTbasevertexproperties::getBaseRhs() const
	{
		return m_development.getarea();
	}

	bool FMTbasevertexproperties::operator == (const FMTbasevertexproperties& rhs) const
	{
		return (m_development == rhs.m_development);
	}
	bool FMTbasevertexproperties::operator != (const FMTbasevertexproperties& rhs) const
	{
		return (!(*this == rhs));
	}

	size_t FMTbasevertexproperties::hash() const
	{
		return boost::hash<Core::FMTdevelopment>()(m_development);
	}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTbasevertexproperties)
