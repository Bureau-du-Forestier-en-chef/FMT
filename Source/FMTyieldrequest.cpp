/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldrequest.h"
#include "FMTyields.h"
#include "FMTdevelopment.h"

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

	FMTyieldrequest::FMTyieldrequest(const Core::FMTdevelopment& ldevelopment):
		datas(),
		resume_mask(),
		development(&ldevelopment),
		graphvertex(nullptr)
	{

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