/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldRequest.h"
#include "FMTYields.h"
#include "FMTDevelopment.h"
#include "FMTYieldHandler.h"
#include "FMTGraphVertexToYield.h"
#include "FMTDevelopment.h"

namespace Core
{ 
	FMTYieldRequest::FMTYieldRequest(const Core::FMTDevelopment& ldevelopment,
		const Graph::FMTGraphVertexToYield& lgraphvertex) :
		m_datas(),
		m_resumeMask(),
		m_yields(),
		m_development(&ldevelopment),
		m_graphvertex(&lgraphvertex)
	{

	}

	FMTYieldRequest::FMTYieldRequest(const FMTDevelopment& ldevelopment,
		const FMTYieldRequest& oldrequest):
		m_datas(oldrequest.m_datas),
		m_resumeMask(oldrequest.m_resumeMask),
		m_yields(oldrequest.m_yields),
		m_development(&ldevelopment),
		m_graphvertex()
	{
		if (ldevelopment.getMask()!= oldrequest.m_development->getMask())
			{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot create a yield request with a different developement mask ",
				"FMTYieldRequest::FMTYieldRequest()", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
	}

	FMTYieldRequest::FMTYieldRequest(const Core::FMTDevelopment& ldevelopment):
		m_datas(),
		m_resumeMask(),
		m_yields(),
		m_development(&ldevelopment),
		m_graphvertex(nullptr)
	{

	}

	const FMTDevelopment& FMTYieldRequest::getDevelopment() const
	{
		return *m_development;
	}
	const std::vector<FMTYieldRequest::const_iterator>&FMTYieldRequest::getDatas() const
	{
		return m_datas;
	}
	const Core::FMTMask& FMTYieldRequest::getResumeMask() const
	{
		return m_resumeMask;
	}
	const Graph::FMTGraphVertexToYield* FMTYieldRequest::getVertexGraphInfo() const
	{
		return m_graphvertex;
	}

	FMTYieldRequest::const_iterator FMTYieldRequest::getFirstSeen(const std::string& p_yield) const
		{
		return  m_yields->_getFirstSeen(p_yield);
		}


void FMTYieldRequest::_updateData(const FMTYields& yields) const
{
	try {
		if (m_resumeMask.empty())
		{
			m_yields = &yields;
			m_resumeMask = yields.filterMask(m_development->getMask());
			m_datas = yields.findSetsWithFiltered(m_resumeMask);
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTYieldRequest::_updateData", __LINE__, __FILE__);
	}

}




}