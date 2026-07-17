/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTBaseEdgeProperties.h"
#include "FMTAction.h"

namespace Graph
{

	bool FMTBaseEdgeProperties::isAction(const std::vector<Core::FMTAction>& actions,
		const Core::FMTAction& rhsaction) const
	{
		if (actions[action] == rhsaction)
		{
			return true;
		}
		return false;
	}

	FMTBaseEdgeProperties::FMTBaseEdgeProperties(const int& laction,
		const int& lvariableID,
		const double& lproportion) :action()
	{
		setactionID(laction);

	}

	void FMTBaseEdgeProperties::setactionID(const int& newid)
	{
		action = static_cast<int8_t>(newid);
	}

	bool FMTBaseEdgeProperties::operator < (const FMTBaseEdgeProperties& rhs) const
	{
		return (action < rhs.action);
	}

	bool FMTBaseEdgeProperties::operator == (const FMTBaseEdgeProperties& rhs) const
	{
		return (action == rhs.action);
	}
	bool FMTBaseEdgeProperties::operator != (const FMTBaseEdgeProperties& rhs) const
	{
		return (!(*this == rhs));
	}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTBaseEdgeProperties)