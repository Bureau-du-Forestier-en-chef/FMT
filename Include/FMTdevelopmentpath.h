/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdevelopmentpath_H_INCLUDED
#define FMTdevelopmentpath_H_INCLUDED


#include <memory>
#include "FMTutility.h"

namespace Core
{
class FMTdevelopment;

class FMTEXPORT FMTdevelopmentpath
	{
	public:
		FMTdevelopmentpath();
		// DocString: FMTdevelopmentpath::getDevelopment
		/**
		@brief Get the new development
		@return a newly generated developement
		*/
		const FMTdevelopment& getDevelopment() const;
		// DocString: FMTdevelopmentpath::getProportion
		/**
		@brief Get the proportion of area going to this dev
		@return the proportion
		*/
		double getProportion() const;
		// DocString: FMTdevelopmentpath::setProportion
		/**
		@brief set the proportion of the path
		@param[in] the new proportion
		*/
		void setProportion(double p_proportion);
		FMTdevelopmentpath(const FMTdevelopmentpath& rhs);
		FMTdevelopmentpath(const FMTdevelopment& ldevelopment, 
			const double& lproportion);
		FMTdevelopmentpath& operator = (const FMTdevelopmentpath& rhs);
		~FMTdevelopmentpath()=default;
	private:
		std::unique_ptr<FMTdevelopment> m_development;
		double m_proportion;
		bool m_own_development;
	};
}


#endif