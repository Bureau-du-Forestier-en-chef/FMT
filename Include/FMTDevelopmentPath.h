/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdevelopmentpath_Hm_included
#define FMTdevelopmentpath_Hm_included


#include <memory>
#include "FMTutility.h"
#include "FMTDevelopment.h"

namespace Core
{

class FMTMask;

// DocString: FMTDevelopmentPath
/**
@brief Development path holding a generated development and the proportion of area going to it.
*/
class FMTEXPORT FMTDevelopmentPath
	{
	public:
		// DocString: FMTDevelopmentPath()
		/**
		@brief Default constructor for FMTDevelopmentPath.
		*/
		FMTDevelopmentPath();
		// DocString: FMTDevelopmentPath::operator=
		/**
		@brief Copy assignment operator for FMTDevelopmentPath.
		@param[in] rhs the FMTDevelopmentPath to copy.
		@return a reference to this FMTDevelopmentPath.
		*/
		FMTDevelopmentPath& operator = (const FMTDevelopmentPath& rhs)=default;
		// DocString: FMTDevelopmentPath(const FMTDevelopmentPath&)
		/**
		@brief Copy constructor for FMTDevelopmentPath.
		@param[in] rhs the FMTDevelopmentPath to copy.
		*/
		FMTDevelopmentPath(const FMTDevelopmentPath& rhs) = default;
		// DocString: ~FMTDevelopmentPath()
		/**
		@brief Default destructor for FMTDevelopmentPath.
		*/
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
		@brief Set the proportion of area going to this development.
		@param[in] p_proportion the proportion to set.
		*/
		void setProportion(double p_proportion);
		// DocString: FMTDevelopmentPath(const FMTMask&,int,int,int,double)
		/**
		@brief Construct a development path from a mask, an age, a lock, a period and a proportion.
		@param[in] p_mask the mask.
		@param[in] p_age the age.
		@param[in] p_lock the lock.
		@param[in] p_period the period.
		@param[in] p_proportion the proportion of area going to this development.
		*/
		FMTDevelopmentPath(const FMTMask& p_mask, 
			int p_age, int p_lock, int p_period,double p_proportion);
	private:
		FMTDevelopment m_development;
		double m_proportion;
	};
}


#endif