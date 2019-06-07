#ifndef FMToutputproperties_H_INCLUDED
#define FMToutputproperties_H_INCLUDED

#include "FMTgraphdescription.h"
#include "FMToutputsource.h"
#include "FMTyields.h"
#include "FMTaction.h"


namespace Graph
{
	class FMToutputproperties
	{
	public:
		int constraintID;
		int variableID;
		FMToutputproperties();
		FMToutputproperties(const FMToutputproperties& rhs);
		FMToutputproperties(const int& lconstraintID, const int& lvariableID);
		FMToutputproperties& operator = (const FMToutputproperties& rhs);
		string variablename() const;
		string constraintname() const;
		~FMToutputproperties()=default;
	};
}
#endif