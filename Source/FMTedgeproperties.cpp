/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
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