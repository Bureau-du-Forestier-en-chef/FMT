/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTactualdevelopment.hpp"
#include "FMTlifespans.hpp"



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
		if (getlock() > 0)
		{
			line = "*A " + std::string(getmask()) + " " + std::to_string(getage()) + " " + std::to_string(area) + " _lock " + std::to_string(getlock());
		}
		else {
			line = "*A " + std::string(getmask()) + " " + std::to_string(getage()) + " " + std::to_string(area);
		}
		return line;
	}

	FMTactualdevelopment::FMTactualdevelopment(const FMTdevelopment& rhs, double larea) : FMTdevelopment(rhs), area(larea)
		{

		}

	FMTactualdevelopment FMTactualdevelopment::presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>&presolvedthemes) const
		{
		FMTactualdevelopment newdev(*this);
		try {
			if (!filter.emptyflipped())
			{
				newdev.setmask(newdev.getmask().presolve(filter, presolvedthemes));
				//newdev.mask = mask.presolve(selectedmask, presolvedthemes);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("for "+std::string(*this),"FMTactualdevelopment::presolve", __LINE__, __FILE__);
			}
		return newdev;
		}
	
	FMTactualdevelopment FMTactualdevelopment::reducelocktodeath(const FMTlifespans& lifespans) const
		{
			FMTactualdevelopment newdev(*this);
			try {
					const int lock = newdev.getlock();
					const int baseage = newdev.getage();
					if(lock>0)
					{
						std::vector<const int *> lifespanfound = lifespans.findsets(newdev.getmask());
						if(!lifespanfound.empty())
						{
							const int devlifespan = *lifespanfound.at(0);
							const int agelock = baseage+lock;
							if(agelock >= devlifespan)
							{
								const int newlock = std::max(lock-(agelock - devlifespan),0);
								_exhandler->raise	(Exception::FMTexc::FMTdeathwithlock,
													std::string(newdev)+" death age is "+std::to_string(devlifespan)+ ". The lock "+std::to_string(lock)+" on the age class "+std::to_string(baseage)+" will exceed the death age. If this error is set to warning, the lock will be reduce to "+std::to_string(newlock)+" to reproduce the behavior of WS.",
													"FMTactualdevelopment::reducelocktodeath",
													__LINE__,
													__FILE__);
								newdev.setlock(newlock);
							}
						}
					}
				}catch (...)
					{
					_exhandler->raisefromcatch("for "+std::string(*this),"FMTactualdevelopment::reducelocktodeath", __LINE__, __FILE__);
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
