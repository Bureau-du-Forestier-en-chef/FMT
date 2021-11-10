/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTfuturdevelopment.hpp"

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

	std::unique_ptr<FMTdevelopment> FMTfuturdevelopment::Clone() const
		{
		return std::unique_ptr<FMTdevelopment>(new FMTfuturdevelopment(*this));
		}

	double FMTfuturdevelopment::getarea() const
		{
		return 0;
		}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTfuturdevelopment)