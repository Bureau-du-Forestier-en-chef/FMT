/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#ifndef FMTACT_H_INCLUDED
#define FMTACT_H_INCLUDED

#include "FMTexceptionhandler.h"
#include "FMTlist.hpp"
#include "FMTbounds.hpp"
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <string>
#include <vector>
#include <unordered_set>
#include <boost/serialization/export.hpp>
#include <limits>


#define FMTGCBMDEATHID 15
#define FMTGCBMGROWTHID 16
#define FMTGCBMUNKNOWNID 17
#define FMTGCBMWILDFIREID 10
#define FMTMaxNumberOfActions _I8_MAX + 1


namespace Core
{

class FMTactioncomparator;
// DocString: FMTaction
/**
FMTaction hold informations to check if a FMTdevelopement is operable or not to this.
The operability testing for a given action is in the FMTdevelopement class.
FMTaction consist of a list of specifications that dictate the operability of 
a subsets of FMTdevelopments.
*/
class FMTEXPORT FMTaction : public FMTlist<FMTspec>
    {
	friend class FMTactioncomparator;
	public:
		// DocString: FMTaction::operator+=
		/**
		 * @brief append OtherAction to this action actions need to be the same kind of age reset otherwise it will throw an exception.
		 * @param OtherAction the other action to append to this one.
		 * @returns A reference the the newly appended action.
		 */
		FMTaction& operator+=(const FMTaction & OtherAction);
		// DocString: FMTaction()
		/**
		Default constructor for FMTaction
		*/
		FMTaction();
		// DocString: ~FMTaction()
		/**
		Destructor for FMTaction
		*/
		virtual ~FMTaction() = default;
		// DocString: FMTaction(const std::string&)
		/**
		Construct a empty FMTaction with a given name
		*/
		FMTaction(const std::string& lname);
		// DocString: FMTaction(const std::string&,const bool&,const bool&)
		/**
		Construct a empty FMTaction with a given name, respectlock? and resetage?
		*/
		FMTaction(const std::string& lname, const bool& lock, const bool& reset);
		// DocString: FMTaction::push_aggregate
		/**
		Push back aggregate to the aggregates data member vector
		*/
		void push_aggregate(const std::string& aggregate);
		// DocString: FMTaction::push_partials
		/**
		Push back a partiel yield name to the partials data member vector
		*/
		void push_partials(const std::string& yield);
		// DocString: FMTaction(const FMTaction&)
		/**
		Copy constructor of FMTaction
		*/
		FMTaction(const FMTaction& rhs);
		// DocString: FMTaction::operator=
		/**
		Copy assignement of FMTaction
		*/
		FMTaction& operator = (const FMTaction& rhs);
		// DocString: FMTaction::update
		/**
		Every FMTlist container need to have a update() function
		this function calls setbounds() to update all bounds and also shrink
		the FMTlist (all for optimization of the process)
		*/
		void update() override;
		// DocString: FMTaction::presolve
		/**
		Eliminate FMTspecification and presolve all masks base on a basemask a preolved mask
		and presolved themes. The returned action can also be empty.
		*/
		FMTaction presolve(const FMTmaskfilter& filter,
			const std::vector<FMTtheme>& originalthemes,
			std::vector<FMTtheme>& newthemes, bool compressdata = false) const;
		// DocString: FMTaction::presolveRef
		/**
		@brief Eliminate FMTspecification and presolve all masks base on a basemask a preolved mask
		and presolved themes. The returned action can also be empty.
		@param[in] p_filter
		@param[in] p_originalthemes
		@param[in] p_newthemes
		@param[in] p_compressdata
		*/
		void presolveRef(const FMTmaskfilter& p_filter,
			const std::vector<FMTtheme>& p_originalthemes,
			std::vector<FMTtheme>& p_newthemes, bool p_compressdata = false);
		// DocString: FMTaction::getagelowerbound
		/**
		Get the age lower bound for all FMTspecs.
		*/
		inline const int& getagelowerbound() const
		{
			return agelowerbound;
		}
		// DocString: FMTaction::getageupperbound
		/**
		Get the age upper bound for all FMTspecs.
		*/
		inline const int& getageupperbound() const
		{
			return ageupperbound;
		}
		// DocString: FMTaction::getperiodlowerbound
		/**
		Get the period lower bound for all FMTspecs.
		*/
		inline const int& getperiodlowerbound() const
		{
			return periodlowerbound;
		}
		// DocString: FMTaction::getperiodupperbound
		/**
		Get the period upper bound for all FMTspecs.
		*/
		inline const int& getperiodupperbound() const
		{
			return periodupperbound;
		}
		// DocString: FMTaction::hash
		/**
		Return the hash value base on the action name.
		*/
		inline size_t hash() const
		{
			return boost::hash<std::string>()(name);
		}
		// DocString: FMTaction::getname
		/**
		Get action name of the FMTaction.
		*/
		inline std::string getname() const
		{
			return name;
		}
		// DocString: FMTaction::dorespectlock
		/**
		If True the action needs to respect the _lock stade of the developement.
		Else the action doesn't car about the _lock of the developement.
		*/
		inline bool dorespectlock() const
		{
			return lock;
		}
		// DocString: FMTaction::isresetage
		/**
		If True When operated by this the FMTdevelopement is set to 0.
		Else it stays the same
		*/
		inline bool isresetage() const
		{
			return reset;
		}
		// DocString: FMTaction::ispartofaserie
		/**
		Return true if the action is part of a serie.
		*/
		inline bool ispartofaserie() const
		{
			return !series.empty();
		}
		// DocString: FMTaction::isallowedinserie
		/**
		Return true if the serie mask is part of one serie of the action if not
		return false.
		*/
		bool isallowedinserie(const std::vector<std::string>& serie) const;
		// DocString: FMTaction::getseriesnames
		/**
		Returns the series names of the action.
		*/
		std::vector<std::string>getseriesnames() const;
		// DocString: FMTaction::getlargestseriesize
		/**
		Returns the size of the largest serie.
		*/
		size_t getlargestseriesize() const;
		// DocString: FMTaction::setseries
		/**
		Giving a serie complete names, and the actions id of the serie occuring before this action,
		Will go across all series and check if part of it and build it if in.
		*/
		void setseries(std::vector<std::vector<std::string>> seriesnames);
		// DocString: FMTaction::useyield
		/**
		Return true if the yield is used by the action to set operability.
		*/
		bool useyield(const std::string& yldname) const;
		// DocString: FMTaction::getaggregates
		/**
		Gets all the aggregates name of which the action is part of.
		*/
		std::vector<std::string>getaggregates() const;
		// DocString: FMTaction::setaggregates
		/**
		@brief Set a new vector of aggregates
		@param[in] p_aggregates the vector of aggregates to set.
		*/
		void setaggregates(const std::vector<std::string>& p_aggregates);
		// DocString: FMTaction::getpartials
		/**
		Gets all the partial yields name of the FMTaction.
		*/
		std::vector<std::string>getpartials() const;
		// DocString: FMTaction::operator<
		/**
		FMTaction less than operator (first test the action length and then the string name to place the action).
		*/
		bool operator < (const FMTaction& rhs) const;
		// DocString: FMTaction::operator==
		/**
		FMTaction equality operator check if FMTactions have the same name.
		*/
		bool operator == (const FMTaction& rhs) const;
		// DocString: FMTaction::operator!=
		/**
		FMTaction nonequality operator check if FMTactions have the not same name.
		*/
		bool operator != (const FMTaction& rhs) const;
		// DocString: FMTaction::operator std::string
		/**
		Convert the FMTaction to a string (like in a regular .act file)
		*/
		operator std::string() const;
		// DocString: FMTaction::partial
		/**
		Check if the yield needs to be considered has partial for this action.
		*/
		bool partial(const std::string& yield) const;
		// DocString: FMTaction::getGCBMactionid
		/**
		Returns the corresponding GCBM action id.
		*/
		int getGCBMactionid() const;
		// DocString: FMTaction::getGCBMactionname
		/**
		Returns the corresponding GCBM action name.
		*/
		std::string getGCBMactionname() const;
		// DocString: FMTaction::split
		/**
		@brief split action into different action using the p_mask
		@param[in] the masks used to split the actions.
		@param[in] the themes to generate the masks
		@return multiple actions resulting from the split
		*/
		std::vector<Core::FMTaction>split(const std::vector<Core::FMTmask>& p_mask,
										const std::vector<Core::FMTtheme>& p_themes) const;
		// DocString: FMTaction::notUse
		/**
		@brief Check if all specs are period lower == 0 and upper == 0
		@return true if not use else false.
		*/
		bool notUse() const;
		// DocString: FMTaction::isPartOf
		/**
		@brief Check if action is part of action name or aggregate.
		@param[in] p_name action name or aggregate.
		@return true if is part else false.
		*/
		bool isPartOf(const std::string& p_name) const;
	protected:
		// DocString: FMTaction::aggregates
		///An action can be part of a aggregate so this data member gets the name of all aggregate the action is being part of.
		std::vector<std::string>aggregates;
		// DocString: FMTaction::partials
		///Keeps the yields name for determining the amount of wood harvested in case of partial cut.
        std::vector<std::string>partials;
		// DocString: FMTaction::agelowerbound
		///Those data members are for optimization only,
		///the class determine within which bounds the aciton can take place for a given development.
		int agelowerbound, ageupperbound, periodlowerbound, periodupperbound;
		// DocString: FMTaction::name
		///The name of the action
		std::string name;
		// DocString: FMTaction::lock
		///If lock is true the action is not _lockexempt when false the action is _LOCKEXEMPT
		bool lock;
		// DocString: FMTaction::reset
		///If reset is true then the action is age reset Y else the action  doen't reset age
		bool reset;
		// DocString: FMTaction::series
		///The action series that the action is part of
		std::vector<std::vector<std::string>>series;
		// DocString: FMTaction::setbounds
		/**
		This function is for optimization it iterates on the specifications to get the 
		agelowerbound, ageupperbound,periodlowerbound,periodupperbound and sets all member data.
		*/
		void setbounds();
		// DocString: FMTaction::getGCBMactiondef
		/**
		Returns the corresponding GCBM action definition.
		*/
		std::vector<std::string>getGCBMactiondef() const;
	private:
		// DocString: FMTaction::serialize
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {
				ar& boost::serialization::make_nvp("specs", boost::serialization::base_object<FMTlist<FMTspec>>(*this));
				ar& BOOST_SERIALIZATION_NVP(partials);
				ar& BOOST_SERIALIZATION_NVP(agelowerbound);
				ar& BOOST_SERIALIZATION_NVP(ageupperbound);
				ar& BOOST_SERIALIZATION_NVP(periodlowerbound);
				ar& BOOST_SERIALIZATION_NVP(periodupperbound);
				ar& BOOST_SERIALIZATION_NVP(name);
				ar& BOOST_SERIALIZATION_NVP(aggregates);
				ar& BOOST_SERIALIZATION_NVP(lock);
				ar& BOOST_SERIALIZATION_NVP(reset);
			}
			catch (...)
			{
				_exhandler->printexceptions("", "FMTaction::serialize", __LINE__, __FILE__);
			}
		}
    };

// DocString: FMTactioncomparator
/**
FMTactioncomparator to check if the action_name already exist in a std container.
It can also check for aggregates.
*/
class FMTactioncomparator
	{
	// DocString: FMTactioncomparator::action_name
	///The action named that we are looking for.
	std::string action_name;
	// DocString: FMTactioncomparator::checkaggregate
	///If true the comparator will also check for aggregates.
	bool checkaggregate;
	public:
		// DocString: FMTactioncomparator(std::string,bool)
		/**
		FMTactioncomparator constructor name is the name of the action we want to match
		if checkaggregate = true it will also return actions within the named aggregate.
		*/
		FMTactioncomparator(std::string name, bool lcheckaggregate = false);
		// DocString: FMTactioncomparator::getallaggregates
		/**
		Looking at the actions vector if aggregateonly = false it will returns actions mathching
		the action_name if aggregateonly = true it will return only matching aggregates.
		*/
		std::vector<const FMTaction*>getallaggregates(const std::vector<FMTaction>&actions,bool aggregateonly = false) const;
		// DocString: FMTactioncomparator::getallaggregatesset
		/**
		Looking at the actions vector if aggregateonly = false it will returns actions ids mathching
		the action_name if aggregateonly = true it will return only matching aggregates.
		*/
		std::unordered_set<int>getallaggregatesset(const std::vector<FMTaction>&actions, bool aggregateonly = false) const;
		// DocString: FMTactioncomparator::operator()(const FMTaction&)
		/**
		Matching test operator for FMTactioncomparator.
		*/
		bool operator()(const FMTaction& action) const;

	};


}

namespace boost {

/**
boost hash for FMTaction
*/
  template <>
  struct hash<Core::FMTaction>
  {
    std::size_t operator()(const Core::FMTaction& act) const
        {
        return (act.hash());
        }
  };


}
BOOST_CLASS_EXPORT_KEY(Core::FMTaction)
#endif // FMTACT_H_INCLUDED
