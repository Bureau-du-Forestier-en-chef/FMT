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
	bool FMTedgeproperties::isaction(const vector<FMTaction>& actions,
								const FMTaction& rhsaction) const
		{
		if (actions[action] == rhsaction)
			{
			return true;
			}
		return false;
		}
	int FMTedgeproperties::getvariable() const
		{
		return variableID;
		}

	int FMTedgeproperties::getactionID() const
		{
		return action;
		}

	double FMTedgeproperties::getproportion() const
		{
		return proportion;
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
	string FMTedgeproperties::variablename() const
		{
		string stype = "";
		if (action >= 0)
			{
			stype = "A";
		}else {
			stype = "G";
			}
		return stype + to_string(variableID);
		}

}