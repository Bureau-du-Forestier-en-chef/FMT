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

#include "FMTvertexproperties.h"

namespace Graph
{
	FMTvertexproperties::FMTvertexproperties() :development(), constraintID() {}

	bool FMTvertexproperties::operator < (const FMTvertexproperties& rhs) const
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

	FMTvertexproperties::FMTvertexproperties(const FMTvertexproperties& rhs):
		development(), constraintID(rhs.constraintID)
			{
			development = rhs.development->Clone();
			}
	FMTvertexproperties::FMTvertexproperties(const Core::FMTfuturdevelopment& ldevelopment,
		const int& lconstraintID):development(new Core::FMTfuturdevelopment(ldevelopment)),constraintID(lconstraintID)
		{
		}
	FMTvertexproperties& FMTvertexproperties::operator = (const FMTvertexproperties& rhs)
		{
		if (this!=&rhs)
			{
			constraintID = rhs.constraintID;
			development = rhs.development->Clone();
			}
		return *this;
		}
	FMTvertexproperties::FMTvertexproperties(const Core::FMTactualdevelopment& ldevelopment,
		const int& lconstraintID): development(new Core::FMTactualdevelopment(ldevelopment)), constraintID(lconstraintID)
		{

		}
    FMTvertexproperties::FMTvertexproperties(const Core::FMTdevelopment& ldevelopment, const int& lconstraintID):
        development(new Core::FMTdevelopment(ldevelopment)), constraintID(lconstraintID)
		{

		}

	void FMTvertexproperties::setdevlopementmask(const Core::FMTmask& newmask)
		{
		development->mask = newmask;
		}

	double FMTvertexproperties::getbaseRHS() const
		{
		return development->getarea();
		}
	bool FMTvertexproperties::operator == (const FMTvertexproperties& rhs) const
		{
		return (*development == *rhs.development);
		}
	bool FMTvertexproperties::operator != (const FMTvertexproperties& rhs) const
		{
		return (!(*this==rhs));
		}

	void FMTvertexproperties::setconstraintID(const int& ID)
		{
		constraintID = ID;
		}

	size_t FMTvertexproperties::hash() const
		{
		return boost::hash<Core::FMTdevelopment>()(*development);
		}

	std::string FMTvertexproperties::constraintname() const
		{
		return "D" + std::to_string(constraintID);
		}
}
