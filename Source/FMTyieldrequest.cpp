/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldrequest.h"
#include "FMTyields.h"
#include "FMTdevelopment.h"
#include "FMTyieldhandler.h"
#include "FMTgraphvertextoyield.h"
#include "FMTdevelopment.h"

namespace Core
{ 
	FMTYieldRequest::FMTYieldRequest(const Core::FMTDevelopment& ldevelopment,
		const Graph::FMTgraphvertextoyield& lgraphvertex) :
		datas(),
		resume_mask(),
		m_yields(),
		development(&ldevelopment),
		graphvertex(&lgraphvertex)
	{

	}

	FMTYieldRequest::FMTYieldRequest(const FMTDevelopment& ldevelopment,
		const FMTYieldRequest& oldrequest):
		datas(oldrequest.datas),
		resume_mask(oldrequest.resume_mask),
		m_yields(oldrequest.m_yields),
		development(&ldevelopment),
		graphvertex()
	{
		if (ldevelopment.getMask()!= oldrequest.development->getMask())
			{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot create a yield request with a different developement mask ",
				"FMTYieldRequest::FMTYieldRequest()", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
	}

	FMTYieldRequest::FMTYieldRequest(const Core::FMTDevelopment& ldevelopment):
		datas(),
		resume_mask(),
		m_yields(),
		development(&ldevelopment),
		graphvertex(nullptr)
	{

	}

	const FMTDevelopment& FMTYieldRequest::getDevelopment() const
	{
		return *development;
	}
	const std::vector<FMTYieldRequest::const_iterator>&FMTYieldRequest::getDatas() const
	{
		return datas;
	}
	const Core::FMTMask& FMTYieldRequest::getResumeMask() const
	{
		return resume_mask;
	}
	const Graph::FMTgraphvertextoyield* FMTYieldRequest::getVertexGraphInfo() const
	{
		return graphvertex;
	}

	FMTYieldRequest::const_iterator FMTYieldRequest::getFirstSeen(const std::string& p_yield) const
		{
		return  m_yields->_getFirstSeen(p_yield);
		}


void FMTYieldRequest::_updateData(const FMTYields& yields) const
{
	try {
		if (resume_mask.empty())
		{
			m_yields = &yields;
			resume_mask = yields.filterMask(development->getMask());
			datas = yields.findSetsWithFiltered(resume_mask);
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTYieldRequest::_updateData", __LINE__, __FILE__);
	}

}




}