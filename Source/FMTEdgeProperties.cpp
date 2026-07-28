/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTEdgeProperties.h"

namespace Graph
{
	void FMTEdgeProperties::setVariableID(const int& newvariableID)
		{
		variableID = newvariableID;
		}


	bool FMTEdgeProperties::operator == (const FMTEdgeProperties& rhs) const
		{
		return (action == rhs.action &&
			variableID == rhs.variableID &&
			proportion == rhs.proportion);
		}
	bool FMTEdgeProperties::operator != (const FMTEdgeProperties& rhs) const
		{
		return (!(*this==rhs));
		}

	FMTEdgeProperties::FMTEdgeProperties(const int& laction,
		const int& lvariableID,const double& lproportion):FMTBaseEdgeProperties(laction), variableID(lvariableID),proportion(static_cast<float>(lproportion))
		{
	
		}
	std::string FMTEdgeProperties::variableName() const
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

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTEdgeProperties)