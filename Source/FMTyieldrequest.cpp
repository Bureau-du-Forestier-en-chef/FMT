/*
Copyright (c) 2019 Gouvernement du Qu�bec

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
	FMTyieldrequest::FMTyieldrequest(const Core::FMTdevelopment& ldevelopment,
		const Graph::FMTgraphvertextoyield& lgraphvertex) :
		datas(),
		resume_mask(),
		m_yields(),
		development(&ldevelopment),
		graphvertex(&lgraphvertex)
	{

	}

	FMTyieldrequest::FMTyieldrequest(const FMTdevelopment& ldevelopment,
		const FMTyieldrequest& oldrequest):
		datas(oldrequest.datas),
		resume_mask(oldrequest.resume_mask),
		m_yields(oldrequest.m_yields),
		development(&ldevelopment),
		graphvertex()
	{
		if (ldevelopment.getmask()!= oldrequest.development->getmask())
			{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot create a yield request with a different developement mask ",
				"FMTyieldrequest::FMTyieldrequest()", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
	}

	FMTyieldrequest::FMTyieldrequest(const Core::FMTdevelopment& ldevelopment):
		datas(),
		resume_mask(),
		m_yields(),
		development(&ldevelopment),
		graphvertex(nullptr)
	{

	}

	const FMTdevelopment& FMTyieldrequest::getdevelopment() const
	{
		return *development;
	}
	const std::vector<FMTyieldrequest::const_iterator>&FMTyieldrequest::getdatas() const
	{
		return datas;
	}
	const Core::FMTmask& FMTyieldrequest::getresumemask() const
	{
		return resume_mask;
	}
	const Graph::FMTgraphvertextoyield* FMTyieldrequest::getvertexgraphinfo() const
	{
		return graphvertex;
	}

	FMTyieldrequest::const_iterator FMTyieldrequest::getFirstSeen(const std::string& p_yield) const
		{
		return  m_yields->_getFirstSeen(p_yield);
		}


void FMTyieldrequest::_updateData(const FMTyields& yields) const
{
	try {
		if (resume_mask.empty())
		{
			m_yields = &yields;
			resume_mask = yields.filtermask(development->getmask());
			datas = yields.findsetswithfiltered(resume_mask);
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyieldrequest::_updateData", __LINE__, __FILE__);
	}

}




}