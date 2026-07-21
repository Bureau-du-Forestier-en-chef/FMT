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
	@brief Class reporting the statistics of a FMTModel, used with the presolve and postsolve functions.
	@details Calling presolve or postsolve changes the model stats.
	*/
	class FMTEXPORT FMTModelStats
	{
	friend class boost::serialization::access;
	// DocString: FMTModelStats::serialize
	/**
	@brief Serialize the FMTModelStats for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
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
		// DocString: FMTModelStats::themes
		/// The number of themes the FMTModel.
		int themes;
		// DocString: FMTModelStats::themesdata
		/// The themes's data size.
		int themesdata;
		// DocString: FMTModelStats::actions
		/// The number of actions of the FMTModel.
		int actions;
		// DocString: FMTModelStats::actionsdata
		/// The actions's data size.
		int actionsdata;
		// DocString: FMTModelStats::transitions
		/// The number of transitions of the FMTModel.
		int transitions;
		// DocString: FMTModelStats::transitionsdata
		/// The transitions's data size.
		int transitionsdata;
		// DocString: FMTModelStats::yieldsdata
		/// The yields's data size.
		int yieldsdata;
		// DocString: FMTModelStats::lifespansdata
		/// The lifespans's data size.
		int lifespansdata;
		// DocString: FMTModelStats::outputs
		/// The number of outputs of the FMTModel.
		int outputs;
		// DocString: FMTModelStats::outputsdata
		/// The outputsdata's data size.
		int outputsdata;
		// DocString: FMTModelStats::constraints
		/// The number of constraints of the FMTModel.
		int constraints;
		// DocString: FMTModelStats::constraintsdata
		/// The constraintsdata's data size.
		int constraintsdata;
		// DocString: FMTModelStats(const size_t,const size_t,const size_t, const size_t,const size_t,const size_t, const size_t, const size_t,const size_t, const size_t,const size_t,const size_t)
		/**
		@brief Constructor for complete stats.
		@param[in] lthemes the number of themes.
		@param[in] lthemesdata the themes data size.
		@param[in] lactions the number of actions.
		@param[in] lactionsdata the actions data size.
		@param[in] ltransitions the number of transitions.
		@param[in] ltransitionsdata the transitions data size.
		@param[in] lyieldsdata the yields data size.
		@param[in] llifespansdata the lifespans data size.
		@param[in] loutputs the number of outputs.
		@param[in] loutputsdata the outputs data size.
		@param[in] lconstraints the number of constraints.
		@param[in] lconstraintsdata the constraints data size.
		*/
		FMTModelStats(const size_t& lthemes,const size_t& lthemesdata, const size_t& lactions, const size_t& lactionsdata,
			const size_t& ltransitions, const size_t& ltransitionsdata, const size_t& lyieldsdata,const size_t& llifespansdata,
			const size_t& loutputs, const size_t& loutputsdata,const size_t& lconstraints, const size_t& lconstraintsdata);
		// DocString: FMTModelStats()
		/**
		@brief Default constructor for FMTModelStats.
		*/
		FMTModelStats() = default;
		// DocString: FMTModelStats(const FMTModelStats)
		/**
		@brief Copy constructor for FMTModelStats.
		@param[in] rhs the FMTModelStats to copy.
		*/
		FMTModelStats(const FMTModelStats& rhs) = default;
		// DocString: ~FMTModelStats()
		/**
		@brief Default destructor for FMTModelStats.
		*/
		~FMTModelStats() = default;
		// DocString: FMTModelStats::operator=
		/**
		@brief Copy assignment operator for FMTModelStats.
		@param[in] rhs the FMTModelStats to copy.
		@return a reference to this FMTModelStats.
		*/
		FMTModelStats& operator = (const FMTModelStats& rhs) = default;
		// DocString: FMTModelStats::operator+=
		/**
		@brief Addition assignment operator with another FMTModelStats.
		@param[in] rhs the stats to add.
		@return a reference to this FMTModelStats.
		*/
		FMTModelStats& operator += (const FMTModelStats& rhs);
		// DocString: FMTModelStats::operator-=
		/**
		@brief Subtraction assignment operator with another FMTModelStats.
		@param[in] rhs the stats to subtract.
		@return a reference to this FMTModelStats.
		*/
		FMTModelStats& operator -= (const FMTModelStats& rhs);
		// DocString: FMTModelStats::operator+
		/**
		@brief Addition operator with another FMTModelStats.
		@param[in] rhs the stats to add.
		@return the resulting FMTModelStats.
		*/
		FMTModelStats operator + (const FMTModelStats& rhs);
		// DocString: FMTModelStats::operator-
		/**
		@brief Subtraction operator with another FMTModelStats.
		@param[in] rhs the stats to subtract.
		@return the resulting FMTModelStats.
		*/
		FMTModelStats operator - (const FMTModelStats& rhs);
		// DocString: FMTModelStats::operator==
		/**
		@brief Equality comparison operator of FMTModelStats.
		@param[in] rhs the stats to compare with.
		@return true if both stats are equal else false.
		*/
		bool operator == (const FMTModelStats& rhs) const;
		// DocString: FMTModelStats::operator!=
		/**
		@brief Inequality comparison operator of FMTModelStats.
		@param[in] rhs the stats to compare with.
		@return true if both stats are different else false.
		*/
		bool operator != (const FMTModelStats& rhs) const;
		// DocString: FMTModelStats::operator std::string()
		/**
		@brief Return the string representation of the stats.
		@return the string representation of the stats.
		*/
		operator std::string() const;
		
	};
}

#endif