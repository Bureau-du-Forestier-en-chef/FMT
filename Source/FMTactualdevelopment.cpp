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

	FMTActualDevelopment::FMTActualDevelopment(FMTActualDevelopment&& rhs):
		FMTDevelopment(std::move(rhs)),area(std::move(rhs.area))
	{

	}

	FMTActualDevelopment& FMTActualDevelopment::operator=(FMTActualDevelopment&& rhs)
	{
		FMTDevelopment::operator=(std::move(rhs));
		area = std::move(rhs.area);
		return *this;
	}


	FMTActualDevelopment::FMTActualDevelopment() :FMTDevelopment(), area() {}
	FMTActualDevelopment::FMTActualDevelopment(const FMTMask& mask,const int& age, const int& lock, const double& area) : FMTDevelopment(mask, age, lock), area(area) {}
	FMTActualDevelopment::FMTActualDevelopment(const FMTActualDevelopment& rhs) : FMTDevelopment(rhs), area(rhs.area) {}
	FMTActualDevelopment& FMTActualDevelopment::operator = (const FMTActualDevelopment& rhs)
	{
		if (this != &rhs)
		{
			FMTDevelopment::operator=(rhs);
			area = rhs.area;
		}
		return *this;
	}
	bool FMTActualDevelopment::operator == (const FMTActualDevelopment& rhs) const
	{
		return FMTDevelopment::operator==(rhs) && area == rhs.area;
	}

	FMTActualDevelopment::operator std::string() const
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

	FMTActualDevelopment::FMTActualDevelopment(const FMTDevelopment& rhs, double larea) : FMTDevelopment(rhs), area(larea)
		{

		}

	FMTActualDevelopment FMTActualDevelopment::presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&presolvedthemes) const
		{
		FMTActualDevelopment newDev(*this);
		try {
			if (!filter.emptyFlipped())
			{
				newDev.setMask(newDev.getMask().presolve(filter, presolvedthemes));
				//newDev.mask = mask.presolve(selectedmask, presolvedthemes);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("for "+std::string(*this),"FMTActualDevelopment::presolve", __LINE__, __FILE__);
			}
		return newDev;
		}
	
	FMTActualDevelopment FMTActualDevelopment::reduceLockToDeath(const FMTLifespans& lifespans) const
		{
			FMTActualDevelopment newDev(*this);
			try {
					const int LOCK = newDev.getLock();
					const int BASE_AGE = newDev.getAge();
					if(LOCK >0)
					{
						std::vector<FMTLifespans::const_iterator> lifespanfound = lifespans.findSets(newDev.getMask());
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
													"FMTActualDevelopment::reduceLockToDeath",
													__LINE__,
													__FILE__);
								newDev.setLock(NEW_LOCK);
							}

						}
					}
				}catch (...)
					{
					_exhandler->raiseFromCatch("for "+std::string(*this),"FMTActualDevelopment::reduceLockToDeath", __LINE__, __FILE__);
					}
			return newDev;
		}

	bool FMTActualDevelopment::operator != (const FMTActualDevelopment& rhs) const
		{
		return !(*this == rhs);
		}
	double FMTActualDevelopment::getArea() const
		{
		return area;
		}

	void FMTActualDevelopment::setArea(const double& newarea)
		{
		area = newarea;
		}

	std::unique_ptr<FMTDevelopment> FMTActualDevelopment::Clone() const
		{
		return std::unique_ptr<FMTDevelopment>(new FMTActualDevelopment(*this));
		}

	bool FMTActualDevelopment::operator < (const FMTActualDevelopment& rhs) const
		{
		return (FMTDevelopment::operator < (rhs) && area < rhs.area);
		}

	FMTActualDevelopmentComparator::FMTActualDevelopmentComparator(const FMTDevelopment* base) : basedev(base)
		{

		}
	bool FMTActualDevelopmentComparator::operator()(const FMTActualDevelopment& actualdev) const
		{
		return (*basedev == actualdev);
		}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTActualDevelopment)
