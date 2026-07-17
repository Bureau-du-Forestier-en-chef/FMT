/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTmodelstats_Hm_included
#define FMTmodelstats_Hm_included

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <string>
#include "FMTException.h"

namespace Models
{
	// DocString: FMTModelStats
	/**
	This class is used to report the stats of a FMTModel. Used alot with the presolve and postSolve functions.
	Calling presolve or postSolve will changes the model stats.
	*/
	class FMTEXPORT FMTModelStats
	{
	friend class boost::serialization::access;
	// DocString: FMTModelStats::serialize
	/**
	Serialize is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(themes);
		ar & BOOST_SERIALIZATION_NVP(themesdata);
		ar & BOOST_SERIALIZATION_NVP(actions);
		ar & BOOST_SERIALIZATION_NVP(actionsdata);
		ar & BOOST_SERIALIZATION_NVP(transitions);
		ar & BOOST_SERIALIZATION_NVP(transitionsdata);
		ar & BOOST_SERIALIZATION_NVP(yieldsdata);
		ar & BOOST_SERIALIZATION_NVP(lifespansdata);
		ar & BOOST_SERIALIZATION_NVP(outputs);
		ar & BOOST_SERIALIZATION_NVP(outputsdata);
		ar & BOOST_SERIALIZATION_NVP(constraints);
		ar & BOOST_SERIALIZATION_NVP(constraintsdata);
		}
	public:
		// DocString : FMTModelStats::themes
		/// The number of themes the FMTModel.
		int themes;
		// DocString : FMTModelStats::themesdata
		/// The themes's data size.
		int themesdata;
		// DocString : FMTModelStats::actions
		/// The number of actions of the FMTModel.
		int actions;
		// DocString : FMTModelStats::actionsdata
		/// The actions's data size.
		int actionsdata;
		// DocString : FMTModelStats::transitions
		/// The number of transitions of the FMTModel.
		int transitions;
		// DocString : FMTModelStats::transitionsdata
		/// The transitions's data size.
		int transitionsdata;
		// DocString : FMTModelStats::yieldsdata
		/// The yields's data size.
		int yieldsdata;
		// DocString : FMTModelStats::lifespansdata
		/// The lifespans's data size.
		int lifespansdata;
		// DocString : FMTModelStats::outputs
		/// The number of outputs of the FMTModel.
		int outputs;
		// DocString : FMTModelStats::outputsdata
		/// The outputsdata's data size.
		int outputsdata;
		// DocString : FMTModelStats::constraints
		/// The number of constraints of the FMTModel.
		int constraints;
		// DocString : FMTModelStats::constraintsdata
		/// The constraintsdata's data size.
		int constraintsdata;
		// DocString : FMTModelStats(const size_t,const size_t,const size_t, const size_t,const size_t,const size_t, const size_t, const size_t,const size_t, const size_t,const size_t,const size_t)
		/**
		FMTModelStats constructor for complete stats.
		*/
		FMTModelStats(const size_t& lthemes,const size_t& lthemesdata, const size_t& lactions, const size_t& lactionsdata,
			const size_t& ltransitions, const size_t& ltransitionsdata, const size_t& lyieldsdata,const size_t& llifespansdata,
			const size_t& loutputs, const size_t& loutputsdata,const size_t& lconstraints, const size_t& lconstraintsdata);
		// DocString : FMTModelStats()
		/**
		FMTModelStats default constructor.
		*/
		FMTModelStats() = default;
		// DocString : FMTModelStats(const FMTModelStats)
		/**
		FMTModelStats default copy constructor.
		*/
		FMTModelStats(const FMTModelStats& rhs) = default;
		// DocString : ~FMTModelStats()
		/**
		FMTModelStats destructor.
		*/
		~FMTModelStats() = default;
		// DocString : FMTModelStats::operator=
		/**
		FMTModelStats default copy assignment.
		*/
		FMTModelStats& operator = (const FMTModelStats& rhs) = default;
		// DocString : FMTModelStats::operator+=
		/**
		FMTModelStats addition assignment with an other FMTModelStats (rhs)
		*/
		FMTModelStats& operator += (const FMTModelStats& rhs);
		// DocString : FMTModelStats::operator-=
		/**
		FMTModelStats substraction assignment with an other FMTModelStats (rhs)
		*/
		FMTModelStats& operator -= (const FMTModelStats& rhs);
		// DocString : FMTModelStats::operator+
		/**
		FMTModelStats copy addition assignment with an other FMTModelStats (rhs)
		*/
		FMTModelStats operator + (const FMTModelStats& rhs);
		// DocString : FMTModelStats::operator-
		/**
		FMTModelStats copy substraction assignment with an other FMTModelStats (rhs)
		*/
		FMTModelStats operator - (const FMTModelStats& rhs);
		// DocString : FMTModelStats::operator==
		/**
		FMTModelStats equality operator.
		*/
		bool operator == (const FMTModelStats& rhs) const;
		// DocString : FMTModelStats::operator!=
		/**
		FMTModelStats non equality operator.
		*/
		bool operator != (const FMTModelStats& rhs) const;
		// DocString : FMTModelStats::operator std::string()
		/**
		FMTModelStats to string operator.
		*/
		operator std::string() const;
		
	};
}

#endif