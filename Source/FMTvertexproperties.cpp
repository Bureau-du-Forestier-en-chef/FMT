/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTvertexproperties.h"
#include "FMTfuturdevelopment.h"
#include "FMTactualdevelopment.h"

namespace Graph
{
	FMTvertexproperties::FMTvertexproperties(const FMTvertexproperties& rhs):
		FMTbasevertexproperties(rhs), constraintID(rhs.constraintID)
	{

	}

	FMTvertexproperties::FMTvertexproperties(const Core::FMTFuturDevelopment& ldevelopment,
		const int& lconstraintID):FMTbasevertexproperties(ldevelopment),constraintID(lconstraintID)
		{
		}
	FMTvertexproperties::FMTvertexproperties(const Core::FMTActualDevelopment& ldevelopment,
		const int& lconstraintID): FMTbasevertexproperties(ldevelopment), constraintID(lconstraintID)
		{

		}
    FMTvertexproperties::FMTvertexproperties(const Core::FMTDevelopment& ldevelopment, const int& lconstraintID):
		FMTbasevertexproperties(ldevelopment), constraintID(lconstraintID)
		{

		}

	

	void FMTvertexproperties::setConstraintID(const int& ID)
		{
		constraintID = ID;
		}


	std::string FMTvertexproperties::constraintName() const
		{
		return "D" + std::to_string(constraintID);
		}
}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTvertexproperties)