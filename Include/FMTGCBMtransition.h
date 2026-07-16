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
#include "FMTobject.h"



namespace Core
{
	// DocString: FMTGCBMTransition
	/**
	FMTGCBMTransition is a class that act like a simple container for a GCBM transition.
	It is only usefull when working with GCBM.
	*/
	class FMTEXPORT FMTGCBMTransition
	{
		// DocString: FMTGCBMTransition::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		Default constructor for FMTGCBMTransition
		*/
		FMTGCBMTransition();
		// DocString: FMTGCBMTransition(const int&,const std::map<std::string, std::string>&,const std::string&)
		/**
		Constructor for FMTGCBMTransition for ageafter,themes and name of the transition.
		*/
		FMTGCBMTransition(const int& lageafter, const std::map<std::string, std::string>& lthemes, const std::string& lname);
		// DocString: FMTGCBMTransition(const FMTGCBMTransition&)
		/**
		FMTGCBMTransition copy constructor.
		*/
		FMTGCBMTransition(const FMTGCBMTransition& rhs);
		// DocString: FMTGCBMTransition::operator=
		/**
		FMTGCBMTransition assignment operator.
		*/
		FMTGCBMTransition& operator = (const FMTGCBMTransition& rhs);
		// DocString: FMTGCBMTransition::operator==
		/**
		FMTGCBMTransition equality operator.
		*/
		bool operator == (const FMTGCBMTransition& rhs) const;
		// DocString: FMTGCBMTransition::operator!=
		/**
		FMTGCBMTransition equality operator.
		*/
		bool operator != (const FMTGCBMTransition& rhs) const;
		// DocString: ~FMTGCBMTransition()
		/**
		Default destructor for FMTGCBMTransition
		*/
		~FMTGCBMTransition()=default;
	};
}
#endif
