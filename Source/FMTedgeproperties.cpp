/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTedgeproperties.h"

namespace Graph
{


	void FMTedgeproperties::setvariableID(const int& newvariableID)
		{
		variableID = newvariableID;
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

	FMTedgeproperties::FMTedgeproperties(const int& laction,
		const int& lvariableID,const double& lproportion):FMTbaseedgeproperties(laction), variableID(lvariableID),proportion(lproportion)
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