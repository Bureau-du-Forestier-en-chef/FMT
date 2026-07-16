/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTactualdevelopment.h"
#include "FMTlifespans.h"
#include "FMTexceptionhandler.h"



namespace Core

{

	FMTactualdevelopment::FMTactualdevelopment(FMTactualdevelopment&& rhs):
		FMTdevelopment(std::move(rhs)),area(std::move(rhs.area))
	{

	}

	FMTactualdevelopment& FMTactualdevelopment::operator=(FMTactualdevelopment&& rhs)
	{
		FMTdevelopment::operator=(std::move(rhs));
		area = std::move(rhs.area);
		return *this;
	}


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
		if (getLock() > 0)
		{
			line = "*A " + std::string(getMask()) + " " + std::to_string(getAge()) + " " + std::to_string(getArea()) + " _lock " + std::to_string(getLock());
		}
		else {
			line = "*A " + std::string(getMask()) + " " + std::to_string(getAge()) + " " + std::to_string(getArea());
		}
		return line;
	}

	FMTactualdevelopment::FMTactualdevelopment(const FMTdevelopment& rhs, double larea) : FMTdevelopment(rhs), area(larea)
		{

		}

	FMTactualdevelopment FMTactualdevelopment::presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>&presolvedthemes) const
		{
		FMTactualdevelopment newDev(*this);
		try {
			if (!filter.emptyFlipped())
			{
				newDev.setMask(newDev.getMask().presolve(filter, presolvedthemes));
				//newDev.mask = mask.presolve(selectedmask, presolvedthemes);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("for "+std::string(*this),"FMTactualdevelopment::presolve", __LINE__, __FILE__);
			}
		return newDev;
		}
	
	FMTactualdevelopment FMTactualdevelopment::reduceLockToDeath(const FMTlifespans& lifespans) const
		{
			FMTactualdevelopment newDev(*this);
			try {
					const int LOCK = newDev.getLock();
					const int BASE_AGE = newDev.getAge();
					if(LOCK >0)
					{
						std::vector<FMTlifespans::const_iterator> lifespanfound = lifespans.findSets(newDev.getMask());
						if(!lifespanfound.empty())
						{
							const int DEV_LIFESPAN = lifespanfound.at(0)->second;
							const int AGE_LOCK = BASE_AGE + LOCK;

							if(AGE_LOCK >DEV_LIFESPAN)
							{
								int fix = 1;
								if (LOCK - (AGE_LOCK - DEV_LIFESPAN) == 0)
								{
									fix = 0;
								}
								const int NEW_LOCK = std::max(LOCK - (AGE_LOCK - DEV_LIFESPAN),0)+ fix;

								_exhandler->raise	(Exception::FMTexc::FMTdeathwithlock,
													std::string(newDev)+" death age is "+
												std::to_string(DEV_LIFESPAN)+ ". The lock "+std::to_string(LOCK)+" on the age class "+std::to_string(BASE_AGE)+
												" will exceed the death age. If this error is set to warning, the lock will be reduce to "+
												std::to_string(NEW_LOCK)+" to reproduce the behavior of WS.",
													"FMTactualdevelopment::reduceLockToDeath",
													__LINE__,
													__FILE__);
								newDev.setLock(NEW_LOCK);
							}

						}
					}
				}catch (...)
					{
					_exhandler->raiseFromCatch("for "+std::string(*this),"FMTactualdevelopment::reduceLockToDeath", __LINE__, __FILE__);
					}
			return newDev;
		}

	bool FMTactualdevelopment::operator != (const FMTactualdevelopment& rhs) const
		{
		return !(*this == rhs);
		}
	double FMTactualdevelopment::getArea() const
		{
		return area;
		}

	void FMTactualdevelopment::setArea(const double& newarea)
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

	FMTActualDevelopmentComparator::FMTActualDevelopmentComparator(const FMTdevelopment* base) : basedev(base)
		{

		}
	bool FMTActualDevelopmentComparator::operator()(const FMTactualdevelopment& actualdev) const
		{
		return (*basedev == actualdev);
		}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTactualdevelopment)
