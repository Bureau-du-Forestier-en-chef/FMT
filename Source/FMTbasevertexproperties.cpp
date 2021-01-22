/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTbasevertexproperties.h"

namespace Graph
{

	bool FMTbasevertexproperties::operator < (const FMTbasevertexproperties& rhs) const
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

	FMTbasevertexproperties::FMTbasevertexproperties(const FMTbasevertexproperties& rhs) :
		development(rhs.development->Clone())
		{
		
		}

	void FMTbasevertexproperties::passinobject(const Core::FMTobject& rhs)
		{
		development->passinobject(rhs);
		}
	FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTfuturdevelopment& ldevelopment) : development(new Core::FMTfuturdevelopment(ldevelopment))
		{

		}
	FMTbasevertexproperties& FMTbasevertexproperties::operator = (const FMTbasevertexproperties& rhs)
	{
		if (this != &rhs)
			{
			development = rhs.development->Clone();
			}
		return *this;
	}
	FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTactualdevelopment& ldevelopment) : development(new Core::FMTactualdevelopment(ldevelopment))
	{

	}
	FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTdevelopment& ldevelopment) :development(new Core::FMTdevelopment(ldevelopment))
	{

	}

	void FMTbasevertexproperties::setdevlopementmask(const Core::FMTmask& newmask)
	{
		development->mask = newmask;
	}

	double FMTbasevertexproperties::getbaseRHS() const
	{
		return 0;
	}
	bool FMTbasevertexproperties::operator == (const FMTbasevertexproperties& rhs) const
	{
		return (*development == *rhs.development);
	}
	bool FMTbasevertexproperties::operator != (const FMTbasevertexproperties& rhs) const
	{
		return (!(*this == rhs));
	}

	size_t FMTbasevertexproperties::hash() const
	{
		return boost::hash<Core::FMTdevelopment>()(*development);
	}

	FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTfuturdevelopment& ldevelopment,
		const int& lconstraintID) : development(new Core::FMTfuturdevelopment(ldevelopment))
	{
	}
	FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTactualdevelopment& ldevelopment,
		const int& lconstraintID) : development(new Core::FMTactualdevelopment(ldevelopment))
	{

	}
	FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTdevelopment& ldevelopment, const int& lconstraintID) :
		development(new Core::FMTdevelopment(ldevelopment))
	{

	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTbasevertexproperties)
