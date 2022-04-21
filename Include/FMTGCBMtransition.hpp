/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTGCBMTRANSITION_H_INCLUDED
#define FMTGCBMTRANSITION_H_INCLUDED

#include <map>
#include <string>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include "FMTobject.hpp"



namespace Core
{
	// DocString: FMTGCBMtransition
	/**
	FMTGCBMtransition is a class that act like a simple container for a GCBM transition.
	It is only usefull when working with GCBM.
	*/
	class FMTEXPORT FMTGCBMtransition
	{
		// DocString: FMTGCBMtransition::serialize
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
		// DocString: FMTGCBMtransition::ageafter
		/// Describe the age set on the strata after the disturbance.
		int ageafter;
		// DocString: FMTGCBMtransition::themes
		/// Map where's the key is the theme name and the element is the new attribute set after the transition.
		std::map<std::string, std::string>themes;
		// DocString: FMTGCBMtransition::name
		///The name of the GCBM transition.
		std::string name;
		// DocString: FMTGCBMtransition()
		/**
		Default constructor for FMTGCBMtransition
		*/
		FMTGCBMtransition();
		// DocString: FMTGCBMtransition(const int&,const std::map<std::string, std::string>&,const std::string&)
		/**
		Constructor for FMTGCBMtransition for ageafter,themes and name of the transition.
		*/
		FMTGCBMtransition(const int& lageafter, const std::map<std::string, std::string>& lthemes, const std::string& lname);
		// DocString: FMTGCBMtransition(const FMTGCBMtransition&)
		/**
		FMTGCBMtransition copy constructor.
		*/
		FMTGCBMtransition(const FMTGCBMtransition& rhs);
		// DocString: FMTGCBMtransition::operator=
		/**
		FMTGCBMtransition assignment operator.
		*/
		FMTGCBMtransition& operator = (const FMTGCBMtransition& rhs);
		// DocString: ~FMTGCBMtransition()
		/**
		Default destructor for FMTGCBMtransition
		*/
		~FMTGCBMtransition()=default;
	};
}
#endif
