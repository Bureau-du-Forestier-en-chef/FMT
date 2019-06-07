#ifndef FMTdevelopmentpath_H_INCLUDED
#define FMTdevelopmentpath_H_INCLUDED

#include "FMTdevelopment.h"
#include <memory>

namespace Core
{
class FMTdevelopment;

class FMTdevelopmentpath
	{
	public:
		unique_ptr<FMTdevelopment> development;
		double proportion;
		FMTdevelopmentpath();
		FMTdevelopmentpath(const FMTdevelopmentpath& rhs);
		FMTdevelopmentpath(const FMTdevelopment& ldevelopment, 
			const double& lproportion);
		FMTdevelopmentpath& operator = (const FMTdevelopmentpath& rhs);
		~FMTdevelopmentpath();
	};
}


#endif