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

#include "FMTedgeproperties.h"

namespace Graph
{
	FMTedgeproperties::FMTedgeproperties() :action(), variableID(),proportion()
		{
	
		}
	FMTedgeproperties::FMTedgeproperties(const FMTedgeproperties& rhs):
		action(rhs.action),
		variableID(rhs.variableID),
		proportion(rhs.proportion)
		{


		}
	FMTedgeproperties& FMTedgeproperties::operator = (const FMTedgeproperties& rhs)
		{
		if (this != &rhs)
			{
			action = rhs.action;
			variableID = rhs.variableID;
			proportion = rhs.proportion;
			}
		return *this;
		}
	bool FMTedgeproperties::isaction(const std::vector<Core::FMTaction>& actions,
								const Core::FMTaction& rhsaction) const
		{
		if (actions[action] == rhsaction)
			{
			return true;
			}
		return false;
		}

	void FMTedgeproperties::setvariableID(const int& newvariableID)
		{
		variableID = newvariableID;
		}

	void FMTedgeproperties::setactionID(const int& newid)
		{
		action = newid;
		}

	bool FMTedgeproperties::operator < (const FMTedgeproperties& rhs) const
		{
		return (action < rhs.action);
		}

	bool FMTedgeproperties::operator == (const FMTedgeproperties& rhs) const
		{
		return (action == rhs.action &&
			variableID == rhs.variableID &&
			proportion == rhs.proportion);
		}
	bool FMTedgeproperties::operator != (const FMTedgeproperties& rhs) const
		{
		return (!(*this==rhs));
		}

	FMTedgeproperties::FMTedgeproperties(int laction,
		int lvariableID,double lproportion):action(laction), variableID(lvariableID),proportion(lproportion)
		{
	
		}
	std::string FMTedgeproperties::variablename() const
		{
		std::string stype = "";
		if (action >= 0)
			{
			stype = "A";
		}else {
			stype = "G";
			}
		return stype + std::to_string(variableID);
		}

}