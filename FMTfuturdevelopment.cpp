#include "FMTfuturdevelopment.h"

namespace Core
{

	FMTfuturdevelopment::FMTfuturdevelopment() :FMTdevelopment() {}

	FMTfuturdevelopment::FMTfuturdevelopment(const FMTfuturdevelopment& rhs) : FMTdevelopment(rhs)
	{

	}
	FMTfuturdevelopment::FMTfuturdevelopment(const FMTdevelopment& dev) : FMTdevelopment(dev) {}

	FMTfuturdevelopment& FMTfuturdevelopment::operator = (const FMTfuturdevelopment& rhs)
	{
		if (this != &rhs)
		{
			FMTdevelopment::operator=(rhs);
		}
		return *this;
	}
	bool FMTfuturdevelopment::operator == (const FMTfuturdevelopment& rhs) const
	{
		return FMTdevelopment::operator==(rhs);
	}

	unique_ptr<FMTdevelopment> FMTfuturdevelopment::Clone() const
		{
		return unique_ptr<FMTdevelopment>(new FMTfuturdevelopment(*this));
		}

	double FMTfuturdevelopment::getarea() const
		{
		return 0;
		}

}