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

class FMTMask;

class FMTEXPORT FMTDevelopmentPath
	{
	public:
		FMTDevelopmentPath();
		FMTDevelopmentPath& operator = (const FMTDevelopmentPath& rhs)=default;
		FMTDevelopmentPath(const FMTDevelopmentPath& rhs) = default;
		~FMTDevelopmentPath() = default;
		// DocString: FMTDevelopmentPath::getDevelopment
		/**
		@brief Get the new development
		@return a newly generated development
		*/
		const FMTDevelopment& getDevelopment() const;
		// DocString: FMTDevelopmentPath::getDevelopmentReference
		/**
		@brief Get the new development
		@return a newly generated development
		*/
		FMTDevelopment& getDevelopmentReference();
		// DocString: FMTDevelopmentPath::getProportion
		/**
		@brief Get the proportion of area going to this dev
		@return the proportion
		*/
		double getProportion() const;
		// DocString: FMTDevelopmentPath::setProportion
		/**
		@brief Set the proportion of area going to this dev
		@param[in] the proportion
		*/
		void setProportion(double p_proportion);
		FMTDevelopmentPath(const FMTMask& p_mask, 
			int p_age, int p_lock, int p_period,double p_proportion);
	private:
		FMTDevelopment m_development;
		double m_proportion;
	};
}


#endif