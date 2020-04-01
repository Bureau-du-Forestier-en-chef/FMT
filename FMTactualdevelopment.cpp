/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
		if (!selectedmask.empty())
			{
			newdev.mask = mask.presolve(selectedmask, presolvedthemes);
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

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTactualdevelopment);