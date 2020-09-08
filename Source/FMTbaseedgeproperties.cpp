/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTbaseedgeproperties.h"

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
		const double& lproportion) :action(laction)
	{


	}

	void FMTbaseedgeproperties::setactionID(const int& newid)
	{
		action = newid;
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

	FMTbaseedgeproperties::FMTbaseedgeproperties(const int& laction) :action(laction)
	{

	}

}