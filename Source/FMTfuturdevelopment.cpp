/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTfuturdevelopment.h"

namespace Core
{

	FMTFuturDevelopment::FMTFuturDevelopment(FMTFuturDevelopment&& rhs) :
		FMTDevelopment(std::move(rhs))
	{

	}

	FMTFuturDevelopment& FMTFuturDevelopment::operator=(FMTFuturDevelopment&& rhs)
	{
		FMTDevelopment::operator=(std::move(rhs));
		return *this;
	}

	FMTFuturDevelopment::FMTFuturDevelopment() :FMTDevelopment() {}

	FMTFuturDevelopment::FMTFuturDevelopment(const FMTFuturDevelopment& rhs) : FMTDevelopment(rhs)
	{

	}
	FMTFuturDevelopment::FMTFuturDevelopment(const FMTDevelopment& dev) : FMTDevelopment(dev) {}

	FMTFuturDevelopment& FMTFuturDevelopment::operator = (const FMTFuturDevelopment& rhs)
	{
		if (this != &rhs)
		{
			FMTDevelopment::operator=(rhs);
		}
		return *this;
	}
	bool FMTFuturDevelopment::operator == (const FMTFuturDevelopment& rhs) const
	{
		return FMTDevelopment::operator==(rhs);
	}

	std::unique_ptr<FMTDevelopment> FMTFuturDevelopment::Clone() const
		{
		return std::unique_ptr<FMTDevelopment>(new FMTFuturDevelopment(*this));
		}

	double FMTFuturDevelopment::getArea() const
		{
		return 0;
		}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTFuturDevelopment)