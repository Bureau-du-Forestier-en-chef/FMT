/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdevelopmentpath_Hm_included
#define FMTdevelopmentpath_Hm_included


#include <memory>
#include "FMTutility.h"
#include "FMTdevelopment.h"

namespace Core
{

class FMTmask;

class FMTEXPORT FMTdevelopmentpath
	{
	public:
		FMTdevelopmentpath();
		FMTdevelopmentpath& operator = (const FMTdevelopmentpath& rhs)=default;
		FMTdevelopmentpath(const FMTdevelopmentpath& rhs) = default;
		~FMTdevelopmentpath() = default;
		// DocString: FMTdevelopmentpath::getDevelopment
		/**
		@brief Get the new development
		@return a newly generated developement
		*/
		const FMTdevelopment& getDevelopment() const;
		// DocString: FMTdevelopmentpath::getDevelopmentReference
		/**
		@brief Get the new development
		@return a newly generated developement
		*/
		FMTdevelopment& getDevelopmentReference();
		// DocString: FMTdevelopmentpath::getProportion
		/**
		@brief Get the proportion of area going to this dev
		@return the proportion
		*/
		double getProportion() const;
		// DocString: FMTdevelopmentpath::setProportion
		/**
		@brief Set the proportion of area going to this dev
		@param[in] the proportion
		*/
		void setProportion(double p_proportion);
		FMTdevelopmentpath(const FMTmask& p_mask, 
			int p_age, int p_lock, int p_period,double p_proportion);
	private:
		FMTdevelopment m_development;
		double m_proportion;
	};
}


#endif