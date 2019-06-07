#include "FMTdevelopmentpath.h"

namespace Core

{ 
	FMTdevelopmentpath::FMTdevelopmentpath(): development(), proportion()
		{

		}
	FMTdevelopmentpath::~FMTdevelopmentpath() {}
	FMTdevelopmentpath::FMTdevelopmentpath(const FMTdevelopmentpath& rhs) : development(), proportion(rhs.proportion)
		{
		//development = make_unique<FMTdevelopment>(*rhs.development);
		development = unique_ptr<FMTdevelopment>(new FMTdevelopment(*rhs.development));
		}
	FMTdevelopmentpath::FMTdevelopmentpath(const FMTdevelopment& ldevelopment,
		const double& lproportion) :development(), proportion(lproportion)
		{
		//development = make_unique<FMTdevelopment>(ldevelopment);
		development = unique_ptr<FMTdevelopment>(new FMTdevelopment(ldevelopment));
		}
	FMTdevelopmentpath& FMTdevelopmentpath::operator = (const FMTdevelopmentpath& rhs)
		{
		if (this!=&rhs)
			{
			//development = make_unique<FMTdevelopment>(*rhs.development);
			development = unique_ptr<FMTdevelopment>(new FMTdevelopment(*rhs.development));
			proportion = rhs.proportion;
			}
		return *this;
		}

}


