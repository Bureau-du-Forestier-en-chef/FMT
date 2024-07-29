/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdevelopmentpath_H_INCLUDED
#define FMTdevelopmentpath_H_INCLUDED


#include <memory>

namespace Core
{
class FMTdevelopment;

class FMTdevelopmentpath
	{
	public:
		std::unique_ptr<FMTdevelopment> development;
		double proportion;
		FMTdevelopmentpath();
		FMTdevelopmentpath(const FMTdevelopmentpath& rhs);
		FMTdevelopmentpath(const FMTdevelopment& ldevelopment, 
			const double& lproportion);
		FMTdevelopmentpath& operator = (const FMTdevelopmentpath& rhs);
		~FMTdevelopmentpath()=default;
	private:
		bool m_own_development;
	};
}


#endif