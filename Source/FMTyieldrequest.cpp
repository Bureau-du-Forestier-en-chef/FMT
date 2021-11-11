/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldrequest.hpp"
#include "FMTyields.hpp"
#include "FMTdevelopment.hpp"
#include "FMTyieldhandler.hpp"
#include "FMTgraphvertextoyield.hpp"
#include "FMTdevelopment.hpp"

namespace Core
{ 
	FMTyieldrequest::FMTyieldrequest(const Core::FMTdevelopment& ldevelopment,
		const Graph::FMTgraphvertextoyield& lgraphvertex) :
		datas(),
		resume_mask(),
		development(&ldevelopment),
		graphvertex(&lgraphvertex)
	{

	}

	FMTyieldrequest::FMTyieldrequest(const FMTdevelopment& ldevelopment,
		const FMTyieldrequest& oldrequest):
		datas(oldrequest.datas),
		resume_mask(oldrequest.resume_mask),
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
		development(&ldevelopment),
		graphvertex(nullptr)
	{

	}

	const FMTdevelopment& FMTyieldrequest::getdevelopment() const
	{
		return *development;
	}
	const std::vector<const std::unique_ptr<FMTyieldhandler>*>&FMTyieldrequest::getdatas() const
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


void FMTyieldrequest::updatedata(const FMTyields& yields) const
{
	try {
		if (resume_mask.empty())
		{
			resume_mask = yields.filtermask(development->getmask());
			datas = yields.findsetswithfiltered(resume_mask);
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyieldrequest::updatedata", __LINE__, __FILE__);
	}

}

}