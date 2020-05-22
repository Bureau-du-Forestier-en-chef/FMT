/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTvertexproperties.h"

namespace Graph
{
	FMTvertexproperties::FMTvertexproperties() :development(), constraintID() {}

	bool FMTvertexproperties::operator < (const FMTvertexproperties& rhs) const
		{
		if (this->development->period < rhs.development->period)
			{
			return true;
			}
		if (*(this->development) < *(rhs.development))
			{
			return true;
			}
		return false;
		}

	FMTvertexproperties::FMTvertexproperties(const FMTvertexproperties& rhs):
		development(), constraintID(rhs.constraintID)
			{
			development = rhs.development->Clone();
			}
	FMTvertexproperties::FMTvertexproperties(const Core::FMTfuturdevelopment& ldevelopment,
		const int& lconstraintID):development(new Core::FMTfuturdevelopment(ldevelopment)),constraintID(lconstraintID)
		{
		}
	FMTvertexproperties& FMTvertexproperties::operator = (const FMTvertexproperties& rhs)
		{
		if (this!=&rhs)
			{
			constraintID = rhs.constraintID;
			development = rhs.development->Clone();
			}
		return *this;
		}
	FMTvertexproperties::FMTvertexproperties(const Core::FMTactualdevelopment& ldevelopment,
		const int& lconstraintID): development(new Core::FMTactualdevelopment(ldevelopment)), constraintID(lconstraintID)
		{

		}
    FMTvertexproperties::FMTvertexproperties(const Core::FMTdevelopment& ldevelopment, const int& lconstraintID):
        development(new Core::FMTdevelopment(ldevelopment)), constraintID(lconstraintID)
		{

		}

	void FMTvertexproperties::setdevlopementmask(const Core::FMTmask& newmask)
		{
		development->mask = newmask;
		}

	double FMTvertexproperties::getbaseRHS() const
		{
		return development->getarea();
		}
	bool FMTvertexproperties::operator == (const FMTvertexproperties& rhs) const
		{
		return (*development == *rhs.development);
		}
	bool FMTvertexproperties::operator != (const FMTvertexproperties& rhs) const
		{
		return (!(*this==rhs));
		}

	void FMTvertexproperties::setconstraintID(const int& ID)
		{
		constraintID = ID;
		}

	size_t FMTvertexproperties::hash() const
		{
		return boost::hash<Core::FMTdevelopment>()(*development);
		}

	std::string FMTvertexproperties::constraintname() const
		{
		return "D" + std::to_string(constraintID);
		}
}
