/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#ifndef FMTACT_Hm_included
#define FMTACT_Hm_included

#include "FMTexceptionhandler.h"
#include "FMTlist.hpp"
#include "FMTbounds.hpp"
#include "FMTSerie.h"
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

class FMTActionComparator;
// DocString: FMTAction
/**
FMTAction hold informations to check if a FMTDevelopment is operable or not to this.
The operability testing for a given action is in the FMTDevelopment class.
FMTAction consist of a list of specifications that dictate the operability of 
a subsets of FMTdevelopments.
*/
class FMTEXPORT FMTAction : public FMTList<FMTSpec>
    {
	friend class FMTActionComparator;
	public:
		// DocString: FMTAction::operator+=
		/**
		 * @brief append OtherAction to this action actions need to be the same kind of age reset otherwise it will throw an exception.
		 * @param OtherAction the other action to append to this one.
		 * @returns A reference the the newly appended action.
		 */
		FMTAction& operator+=(const FMTAction & OtherAction);
		// DocString: FMTAction()
		/**
		Default constructor for FMTAction
		*/
		FMTAction();
		// DocString: ~FMTAction()
		/**
		Destructor for FMTAction
		*/
		virtual ~FMTAction() = default;
		// DocString: FMTAction(const std::string&)
		/**
		Construct a empty FMTAction with a given name
		*/
		FMTAction(const std::string& lname);
		// DocString: FMTAction(const std::string&,const bool&,const bool&)
		/**
		Construct a empty FMTAction with a given name, respectlock? and resetage?
		*/
		FMTAction(const std::string& lname, const bool& lock, const bool& reset);
		// DocString: FMTAction::pushAggregate
		/**
		Push back aggregate to the aggregates data member vector
		*/
		void pushAggregate(const std::string& aggregate);
		// DocString: FMTAction::pushPartials
		/**
		Push back a partiel yield name to the partials data member vector
		*/
		void pushPartials(const std::string& yield);
		// DocString: FMTAction(const FMTAction&)
		/**
		Copy constructor of FMTAction
		*/
		FMTAction(const FMTAction& rhs);
		// DocString: FMTAction::operator=
		/**
		Copy assignement of FMTAction
		*/
		FMTAction& operator = (const FMTAction& rhs);
		// DocString: FMTAction::update
		/**
		Every FMTList container need to have a update() function
		this function calls setBounds() to update all bounds and also shrink
		the FMTList (all for optimization of the process)
		*/
		void update() override;
		// DocString: FMTAction::presolve
		/**
		Eliminate FMTspecification and presolve all masks base on a baseMask a preolved mask
		and presolved themes. The returned action can also be empty.
		*/
		FMTAction presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			std::vector<FMTTheme>& newthemes, bool compressdata = false) const;
		// DocString: FMTAction::presolveRef
		/**
		@brief Eliminate FMTspecification and presolve all masks base on a baseMask a preolved mask
		and presolved themes. The returned action can also be empty.
		@param[in] p_filter
		@param[in] p_originalthemes
		@param[in] p_newthemes
		@param[in] p_compressdata
		*/
		void presolveRef(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_originalthemes,
			std::vector<FMTTheme>& p_newthemes, bool p_compressdata = false);
		// DocString: FMTAction::getAgeLowerBound
		/**
		Get the age lower bound for all FMTspecs.
		*/
		inline const int& getAgeLowerBound() const
		{
			return agelowerbound;
		}
		// DocString: FMTAction::getAgeUpperBound
		/**
		Get the age upper bound for all FMTspecs.
		*/
		inline const int& getAgeUpperBound() const
		{
			return ageupperbound;
		}
		// DocString: FMTAction::getPeriodLowerBound
		/**
		Get the period lower bound for all FMTspecs.
		*/
		inline const int& getPeriodLowerBound() const
		{
			return periodlowerbound;
		}
		// DocString: FMTAction::getPeriodUpperBound
		/**
		Get the period upper bound for all FMTspecs.
		*/
		inline const int& getPeriodUpperBound() const
		{
			return periodupperbound;
		}
		// DocString: FMTAction::hash
		/**
		Return the hash value base on the action name.
		*/
		inline size_t hash() const
		{
			return boost::hash<std::string>()(name);
		}
		// DocString: FMTAction::getName
		/**
		Get action name of the FMTAction.
		*/
		inline std::string getName() const
		{
			return name;
		}
		// DocString: FMTAction::doRespectLock
		/**
		If True the action needs to respect the _lock stade of the development.
		Else the action doesn't car about the _lock of the development.
		*/
		inline bool doRespectLock() const
		{
			return lock;
		}
		// DocString: FMTAction::isResetAge
		/**
		If True When operated by this the FMTDevelopment is set to 0.
		Else it stays the same
		*/
		inline bool isResetAge() const
		{
			return reset;
		}
		// DocString: FMTAction::isPartOfASerie
		/**
		Return true if the action is part of a serie.
		*/
		inline bool isPartOfASerie() const
		{
			return !m_series.empty();
		}
		// DocString: FMTAction::isAllowedInSerie
		/**
		Return true if the serie mask is part of one serie of the action if not
		return false.
		*/
		bool isAllowedInSerie(const std::vector<std::string>& serie) const;
		// DocString: FMTAction::getSerie
		/**
		@brief get the serie with the partial mask p_SerieMask
		@param[in] p_SerieMask the partial serie mask
		@return pointer to valid serie
		*/
		const FMTSerie* getSerie(const std::vector<std::string>& p_SerieMask) const;
		// DocString: FMTAction::getSeries
		/**
		@brief get all series of the actions
		@return vector of series
		*/
		const std::vector<FMTSerie>& getSeries() const;
		// DocString: FMTAction::getSeriesNames
		/**
		Returns the series names of the action.
		*/
		std::vector<std::string> getSeriesNames() const;
		// DocString: FMTAction::getLargestSerieSize
		/**
		Returns the size of the largest serie.
		*/
		size_t getLargestSerieSize() const;
		// DocString: FMTAction::setSeries
		/**
		Giving a serie complete names, and the actions id of the serie occuring before this action,
		Will go across all series and check if part of it and build it if in.
		*/
		void setSeries(std::vector<Core::FMTSerie> p_series);
		// DocString: FMTAction::useYield
		/**
		Return true if the yield is used by the action to set operability.
		*/
		bool useYield(const std::string& yldname) const;
		// DocString: FMTAction::getAggregates
		/**
		Gets all the aggregates name of which the action is part of.
		*/
		std::vector<std::string> getAggregates() const;
		// DocString: FMTAction::setAggregates
		/**
		@brief Set a new vector of aggregates
		@param[in] p_aggregates the vector of aggregates to set.
		*/
		void setAggregates(const std::vector<std::string>& p_aggregates);
		// DocString: FMTAction::getPartials
		/**
		Gets all the partial yields name of the FMTAction.
		*/
		std::vector<std::string> getPartials() const;
		// DocString: FMTAction::operator<
		/**
		FMTAction less than operator (first test the action length and then the string name to place the action).
		*/
		bool operator < (const FMTAction& rhs) const;
		// DocString: FMTAction::operator==
		/**
		FMTAction equality operator check if FMTactions have the same name.
		*/
		bool operator == (const FMTAction& rhs) const;
		// DocString: FMTAction::operator!=
		/**
		FMTAction nonequality operator check if FMTactions have the not same name.
		*/
		bool operator != (const FMTAction& rhs) const;
		// DocString: FMTAction::operator std::string
		/**
		Convert the FMTAction to a string (like in a regular .act file)
		*/
		operator std::string() const;
		// DocString: FMTAction::partial
		/**
		Check if the yield needs to be considered has partial for this action.
		*/
		bool partial(const std::string& yield) const;
		// DocString: FMTAction::getGCBMActionId
		/**
		Returns the corresponding GCBM action id.
		*/
		int getGCBMActionId() const;
		// DocString: FMTAction::getGCBMActionName
		/**
		Returns the corresponding GCBM action name.
		*/
		std::string getGCBMActionName() const;
		// DocString: FMTAction::split
		/**
		@brief split action into different action using the p_mask
		@param[in] the masks used to split the actions.
		@param[in] the themes to generate the masks
		@return multiple actions resulting from the split
		*/
		std::vector<Core::FMTAction>split(const std::vector<Core::FMTMask>& p_mask,
										const std::vector<Core::FMTTheme>& p_themes) const;
		// DocString: FMTAction::notUse
		/**
		@brief Check if all specs are period lower == 0 and upper == 0
		@return true if not use else false.
		*/
		bool notUse() const;
		// DocString: FMTAction::isPartOf
		/**
		@brief Check if action is part of action name or aggregate.
		@param[in] p_name action name or aggregate.
		@return true if is part else false.
		*/
		bool isPartOf(const std::string& p_name) const;
		// DocString: FMTAction:: isInSeries
		/**
		@brief return true if you find this action first in a serie
		@return true if first else false
		*/
		bool isInSeries() const;
	protected:
		// DocString: FMTAction::aggregates
		///An action can be part of a aggregate so this data member gets the name of all aggregate the action is being part of.
		std::vector<std::string> aggregates;
		// DocString: FMTAction::partials
		///Keeps the yields name for determining the amount of wood harvested in case of partial cut.
        std::vector<std::string> partials;
		// DocString: FMTAction::agelowerbound
		///Those data members are for optimization only,
		///the class determine within which bounds the aciton can take place for a given development.
		int agelowerbound, ageupperbound, periodlowerbound, periodupperbound;
		// DocString: FMTAction::name
		///The name of the action
		std::string name;
		// DocString: FMTAction::lock
		///If lock is true the action is not _lockexempt when false the action is _LOCKEXEMPT
		bool lock;
		// DocString: FMTAction::reset
		///If reset is true then the action is age reset Y else the action  doen't reset age
		bool reset;
		// DocString: FMTAction::m_series
		///The action series that the action is part of
		std::vector<FMTSerie> m_series;
		// DocString: FMTAction::m_InSerie
		///True if the actions is in a serie
		bool m_InSerie;
		// DocString: FMTAction::setBounds
		/**
		This function is for optimization it iterates on the specifications to get the 
		agelowerbound, ageupperbound,periodlowerbound,periodupperbound and sets all member data.
		*/
		void setBounds();
		// DocString: FMTAction::getGCBMActionDef
		/**
		Returns the corresponding GCBM action definition.
		*/
		std::vector<std::string> getGCBMActionDef() const;
	private:
		// DocString: FMTAction::serialize
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {
				ar& boost::serialization::make_nvp("specs", boost::serialization::base_object<FMTList<FMTSpec>>(*this));
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
				_exhandler->printExceptions("", "FMTAction::serialize", __LINE__, __FILE__);
			}
		}
    };

// DocString: FMTActionComparator
/**
FMTActionComparator to check if the action_name already exist in a std container.
It can also check for aggregates.
*/
class FMTActionComparator
	{
	// DocString: FMTActionComparator::action_name
	///The action named that we are looking for.
	std::string action_name;
	// DocString: FMTActionComparator::checkaggregate
	///If true the comparator will also check for aggregates.
	bool checkaggregate;
	public:
		// DocString: FMTActionComparator(std::string,bool)
		/**
		FMTActionComparator constructor name is the name of the action we want to match
		if checkaggregate = true it will also return actions within the named aggregate.
		*/
		FMTActionComparator(std::string name, bool lcheckaggregate = false);
		// DocString: FMTActionComparator::getAllAggregates
		/**
		Looking at the actions vector if aggregateonly = false it will returns actions mathching
		the action_name if aggregateonly = true it will return only matching aggregates.
		*/
		std::vector<const FMTAction*> getAllAggregates(const std::vector<FMTAction>&actions,bool aggregateonly = false) const;
		// DocString: FMTActionComparator::getAllAggregatesSet
		/**
		Looking at the actions vector if aggregateonly = false it will returns actions ids mathching
		the action_name if aggregateonly = true it will return only matching aggregates.
		*/
		std::unordered_set<int> getAllAggregatesSet(const std::vector<FMTAction>&actions, bool aggregateonly = false) const;
		// DocString: FMTActionComparator::operator()(const FMTAction&)
		/**
		Matching test operator for FMTActionComparator.
		*/
		bool operator()(const FMTAction& action) const;
	};
}

namespace boost {

/**
boost hash for FMTAction
*/
  template <>
  struct hash<Core::FMTAction>
  {
    std::size_t operator()(const Core::FMTAction& act) const
        {
        return (act.hash());
        }
  };


}
BOOST_CLASS_EXPORT_KEY(Core::FMTAction)
#endif // FMTACT_Hm_included
