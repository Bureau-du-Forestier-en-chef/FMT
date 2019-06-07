#ifndef FMTOUTPUTNODE_H_INCLUDED
#define FMTOUTPUTNODE_H_INCLUDED

#include "FMToutputsource.h"
#include "FMToperator.h"

namespace Core
{
class FMToutputnode
	{
	public:
		FMToutputsource source;
		FMToutputsource factor;
		double constant;
		FMToutputnode();
		FMToutputnode(const FMToutputsource& lsource, const FMToutputsource& lfactor, const double& lconstant);
		FMToutputnode(const FMToutputnode& rhs);
		size_t hash(const int& period) const;
		bool singleperiod() const;
		bool ispastperiod() const;
		FMToutputnode setperiod(int period) const;
		FMToutputnode& operator = (const FMToutputnode& rhs);
		~FMToutputnode()=default;
	};
}

#endif
