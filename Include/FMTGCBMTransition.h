/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTGCBMTRANSITION_Hm_included
#define FMTGCBMTRANSITION_Hm_included

#include <map>
#include <string>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include "FMTObject.h"



namespace Core
{
	// DocString: FMTGCBMTransition
	/**
	@brief Simple container for a GCBM transition, only useful when working with GCBM.
	*/
	class FMTEXPORT FMTGCBMTransition
	{
		// DocString: FMTGCBMTransition::serialize
		/**
		@brief Serialize the FMTGCBMTransition for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(ageafter);
		ar & BOOST_SERIALIZATION_NVP(themes);
		ar & BOOST_SERIALIZATION_NVP(name);
		}
	public:
		// DocString: FMTGCBMTransition::ageafter
		/// Describe the age set on the strata after the disturbance.
		int ageafter;
		// DocString: FMTGCBMTransition::themes
		/// Map where's the key is the theme name and the element is the new attribute set after the transition.
		std::map<std::string, std::string>themes;
		// DocString: FMTGCBMTransition::name
		///The name of the GCBM transition.
		std::string name;
		// DocString: FMTGCBMTransition()
		/**
		@brief Default constructor for FMTGCBMTransition.
		*/
		FMTGCBMTransition();
		// DocString: FMTGCBMTransition(const int&,const std::map<std::string, std::string>&,const std::string&)
		/**
		@brief Construct a GCBM transition from the age after, the themes and the name.
		@param[in] lageafter the age set on the strata after the disturbance.
		@param[in] lthemes the map of theme name to new attribute.
		@param[in] lname the name of the transition.
		*/
		FMTGCBMTransition(const int& lageafter, const std::map<std::string, std::string>& lthemes, const std::string& lname);
		// DocString: FMTGCBMTransition(const FMTGCBMTransition&)
		/**
		@brief Copy constructor for FMTGCBMTransition.
		@param[in] rhs the FMTGCBMTransition to copy.
		*/
		FMTGCBMTransition(const FMTGCBMTransition& rhs);
		// DocString: FMTGCBMTransition::operator=
		/**
		@brief Copy assignment operator for FMTGCBMTransition.
		@param[in] rhs the FMTGCBMTransition to copy.
		@return a reference to this FMTGCBMTransition.
		*/
		FMTGCBMTransition& operator = (const FMTGCBMTransition& rhs);
		// DocString: FMTGCBMTransition::operator==
		/**
		@brief Equality comparison operator of FMTGCBMTransition.
		@param[in] rhs the transition to compare with.
		@return true if both transitions are equal else false.
		*/
		bool operator == (const FMTGCBMTransition& rhs) const;
		// DocString: FMTGCBMTransition::operator!=
		/**
		@brief Inequality comparison operator of FMTGCBMTransition.
		@param[in] rhs the transition to compare with.
		@return true if both transitions are different else false.
		*/
		bool operator != (const FMTGCBMTransition& rhs) const;
		// DocString: ~FMTGCBMTransition()
		/**
		@brief Default destructor for FMTGCBMTransition.
		*/
		~FMTGCBMTransition()=default;
	};
}
#endif
