#include "FMTactualdevelopment.h"



namespace Core

{

	FMTactualdevelopment::FMTactualdevelopment() :FMTdevelopment(), area() {}
	FMTactualdevelopment::FMTactualdevelopment(FMTmask mask, int age, int lock, double area) : FMTdevelopment(mask, age, lock), area(area) {}
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

	FMTactualdevelopment::operator string() const
	{
		string line;
		if (lock > 0)
		{
			line = "*A " + string(mask) + " " + to_string(age) + " " + to_string(area) + " _lock " + to_string(lock);
		}
		else {
			line = "*A " + string(mask) + " " + to_string(age) + " " + to_string(area);
		}
		return line;
	}

	FMTactualdevelopment::FMTactualdevelopment(const FMTdevelopment& rhs, double larea) : FMTdevelopment(rhs), area(larea)
	{



	}
	bool FMTactualdevelopment::operator != (const FMTactualdevelopment& rhs) const
		{
		return !(*this == rhs);
		}
	double FMTactualdevelopment::getarea() const
		{
		return area;
		}

	unique_ptr<FMTdevelopment> FMTactualdevelopment::Clone() const
		{
		return unique_ptr<FMTdevelopment>(new FMTactualdevelopment(*this));
		}

	bool FMTactualdevelopment::operator < (const FMTactualdevelopment& rhs) const
		{
		return (FMTdevelopment::operator < (rhs) && area < rhs.area);
		}

}
