#include "FMToutputproperties.h"

namespace Graph
{
	FMToutputproperties::FMToutputproperties():constraintID(), variableID()
		{
	
		}
	FMToutputproperties::FMToutputproperties(const FMToutputproperties& rhs) : 
		constraintID(rhs.constraintID), variableID(rhs.variableID)
		{

		}
	FMToutputproperties::FMToutputproperties(const int& lconstraintID, const int& lvariableID): 
		constraintID(lconstraintID), variableID(lvariableID)
		{

		}

	FMToutputproperties& FMToutputproperties::operator = (const FMToutputproperties& rhs)
		{
		if(this != &rhs)
			{
			constraintID = rhs.constraintID;
			variableID = rhs.variableID;
			}
		return *this;
		}

	string FMToutputproperties::variablename() const
		{
		return "OS" + to_string(variableID);
		}

	string FMToutputproperties::constraintname() const
		{
		return "OS" + to_string(constraintID);
		}
}