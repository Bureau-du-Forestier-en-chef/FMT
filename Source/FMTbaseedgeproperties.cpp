/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTbaseedgeproperties.h"
#include "FMTaction.h"

namespace Graph
{

	bool FMTbaseedgeproperties::isaction(const std::vector<Core::FMTaction>& actions,
		const Core::FMTaction& rhsaction) const
	{
		if (actions[action] == rhsaction)
		{
			return true;
		}
		return false;
	}

	FMTbaseedgeproperties::FMTbaseedgeproperties(const int& laction,
		const int& lvariableID,
		const double& lproportion) :action()
	{
		setactionID(laction);

	}

	void FMTbaseedgeproperties::setactionID(const int& newid)
	{
		action = static_cast<int8_t>(newid);
	}

	bool FMTbaseedgeproperties::operator < (const FMTbaseedgeproperties& rhs) const
	{
		return (action < rhs.action);
	}

	bool FMTbaseedgeproperties::operator == (const FMTbaseedgeproperties& rhs) const
	{
		return (action == rhs.action);
	}
	bool FMTbaseedgeproperties::operator != (const FMTbaseedgeproperties& rhs) const
	{
		return (!(*this == rhs));
	}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTbaseedgeproperties)