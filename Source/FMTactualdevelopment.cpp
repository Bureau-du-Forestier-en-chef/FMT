/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTactualdevelopment.h"



namespace Core

{

	FMTactualdevelopment::FMTactualdevelopment() :FMTdevelopment(), area() {}
	FMTactualdevelopment::FMTactualdevelopment(const FMTmask& mask,const int& age, const int& lock, const double& area) : FMTdevelopment(mask, age, lock), area(area) {}
	FMTactualdevelopment::FMTactualdevelopment(const FMTactualdevelopment& rhs) : FMTdevelopment(rhs), area(rhs.area) {}
	FMTactualdevelopment& FMTactualdevelopment::operator = (const FMTactualdevelopment& rhs)
	{
		if (this != &rhs)
		{
			FMTdevelopment::operator=(rhs);
			area = rhs.area;
		}
		return *this;
	}
	bool FMTactualdevelopment::operator == (const FMTactualdevelopment& rhs) const
	{
		return FMTdevelopment::operator==(rhs) && area == rhs.area;
	}

	FMTactualdevelopment::operator std::string() const
	{
		std::string line;
		if (lock > 0)
		{
			line = "*A " + std::string(mask) + " " + std::to_string(age) + " " + std::to_string(area) + " _lock " + std::to_string(lock);
		}
		else {
			line = "*A " + std::string(mask) + " " + std::to_string(age) + " " + std::to_string(area);
		}
		return line;
	}

	FMTactualdevelopment::FMTactualdevelopment(const FMTdevelopment& rhs, double larea) : FMTdevelopment(rhs), area(larea)
		{

		}

	FMTactualdevelopment FMTactualdevelopment::presolve(const FMTmask& selectedmask, const std::vector<FMTtheme>&presolvedthemes) const
		{
		FMTactualdevelopment newdev(*this);
		try {
			if (!selectedmask.empty())
			{
				newdev.mask = mask.presolve(selectedmask, presolvedthemes);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("for "+std::string(*this),"FMTactualdevelopment::presolve", __LINE__, __FILE__, _section);
			}
		return newdev;
		}

	bool FMTactualdevelopment::operator != (const FMTactualdevelopment& rhs) const
		{
		return !(*this == rhs);
		}
	double FMTactualdevelopment::getarea() const
		{
		return area;
		}

	void FMTactualdevelopment::setarea(const double& newarea)
		{
		area = newarea;
		}

	std::unique_ptr<FMTdevelopment> FMTactualdevelopment::Clone() const
		{
		return std::unique_ptr<FMTdevelopment>(new FMTactualdevelopment(*this));
		}

	bool FMTactualdevelopment::operator < (const FMTactualdevelopment& rhs) const
		{
		return (FMTdevelopment::operator < (rhs) && area < rhs.area);
		}

	FMTactualdevelopmentcomparator::FMTactualdevelopmentcomparator(const FMTdevelopment* base) : basedev(base)
		{

		}
	bool FMTactualdevelopmentcomparator::operator()(const FMTactualdevelopment& actualdev) const
		{
		return (*basedev == actualdev);
		}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTactualdevelopment)
