/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTvertexproperties.h"

namespace Graph
{


	FMTvertexproperties::FMTvertexproperties(const Core::FMTfuturdevelopment& ldevelopment,
		const int& lconstraintID):FMTbasevertexproperties(ldevelopment),constraintID(lconstraintID)
		{
		}
	FMTvertexproperties::FMTvertexproperties(const Core::FMTactualdevelopment& ldevelopment,
		const int& lconstraintID): FMTbasevertexproperties(ldevelopment), constraintID(lconstraintID)
		{

		}
    FMTvertexproperties::FMTvertexproperties(const Core::FMTdevelopment& ldevelopment, const int& lconstraintID):
		FMTbasevertexproperties(ldevelopment), constraintID(lconstraintID)
		{

		}

	double FMTvertexproperties::getbaseRHS() const
		{
		return development->getarea();
		}

	void FMTvertexproperties::setconstraintID(const int& ID)
		{
		constraintID = ID;
		}

	size_t FMTvertexproperties::hash() const
		{
		return FMTbasevertexproperties::hash();
		}

	std::string FMTvertexproperties::constraintname() const
		{
		return "D" + std::to_string(constraintID);
		}
}
