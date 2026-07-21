/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#ifndef FMTACT_Hm_included
#define FMTACT_Hm_included

#include "FMTExceptionHandler.h"
#include "FMTList.hpp"
#include "FMTBounds.hpp"
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
@brief List of specifications dictating the operability of a subset of developments to this action.
@details The operability testing for a given action is done in the FMTDevelopment class.
*/
class FMTEXPORT FMTAction : public FMTList<FMTSpec>
    {
	friend class FMTActionComparator;
	public:
		// DocString: FMTAction::operator+=
		/**
		@brief Append another action to this action; actions need to have the same age reset otherwise an exception is thrown.
		@param[in] OtherAction the other action to append to this one.
		@return a reference to this appended action.
		*/
		FMTAction& operator+=(const FMTAction & OtherAction);
		// DocString: FMTAction()
		/**
		@brief Default constructor for FMTAction.
		*/
		FMTAction();
		// DocString: ~FMTAction()
		/**
		@brief Default virtual destructor for FMTAction.
		*/
		virtual ~FMTAction() = default;
		// DocString: FMTAction(const std::string&)
		/**
		@brief Construct an empty action with a given name.
		@param[in] lname the name of the action.
		*/
		FMTAction(const std::string& lname);
		// DocString: FMTAction(const std::string&,const bool&,const bool&)
		/**
		@brief Construct an empty action with a given name, respect lock and reset age flags.
		@param[in] lname the name of the action.
		@param[in] p_lock if true the action respects the lock.
		@param[in] p_reset if true the action resets the age.
		*/
		FMTAction(const std::string& lname, const bool& p_lock, const bool& p_reset);
		// DocString: FMTAction::pushAggregate
		/**
		@brief Push an aggregate to the aggregates of the action.
		@param[in] aggregate the aggregate to push.
		*/
		void pushAggregate(const std::string& aggregate);
		// DocString: FMTAction::pushPartials
		/**
		@brief Push a partial yield name to the partials of the action.
		@param[in] yield the partial yield name to push.
		*/
		void pushPartials(const std::string& yield);
		// DocString: FMTAction(const FMTAction&)
		/**
		@brief Copy constructor for FMTAction.
		@param[in] rhs the FMTAction to copy.
		*/
		FMTAction(const FMTAction& rhs);
		// DocString: FMTAction::operator=
		/**
		@brief Copy assignment operator for FMTAction.
		@param[in] rhs the FMTAction to copy.
		@return a reference to this FMTAction.
		*/
		FMTAction& operator = (const FMTAction& rhs);
		// DocString: FMTAction::update
		/**
		@brief Update the action by setting the bounds and shrinking the list, for optimization.
		*/
		void update() override;
		// DocString: FMTAction::presolve
		/**
		@brief Presolve the action by eliminating specifications and presolving the masks; the returned action can be empty.
		@param[in] filter the mask filter.
		@param[in] originalthemes the original themes.
		@param[in,out] newthemes the presolved themes.
		@param[in] compressdata if true compresses the data.
		@return the presolved action.
		*/
		FMTAction presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			std::vector<FMTTheme>& newthemes, bool compressdata = false) const;
		// DocString: FMTAction::presolveRef
		/**
		@brief Presolve the action in place by eliminating specifications and presolving the masks; the action can become empty.
		@param[in] p_filter the mask filter.
		@param[in] p_originalthemes the original themes.
		@param[in,out] p_newthemes the presolved themes.
		@param[in] p_compressdata if true compresses the data.
		*/
		void presolveRef(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_originalthemes,
			std::vector<FMTTheme>& p_newthemes, bool p_compressdata = false);
		// DocString: FMTAction::getAgeLowerBound
		/**
		@brief Get the age lower bound for all specifications.
		@return the age lower bound.
		*/
		inline const int& getAgeLowerBound() const
		{
			return m_agelowerbound;
		}
		// DocString: FMTAction::getAgeUpperBound
		/**
		@brief Get the age upper bound for all specifications.
		@return the age upper bound.
		*/
		inline const int& getAgeUpperBound() const
		{
			return m_ageupperbound;
		}
		// DocString: FMTAction::getPeriodLowerBound
		/**
		@brief Get the period lower bound for all specifications.
		@return the period lower bound.
		*/
		inline const int& getPeriodLowerBound() const
		{
			return m_periodlowerbound;
		}
		// DocString: FMTAction::getPeriodUpperBound
		/**
		@brief Get the period upper bound for all specifications.
		@return the period upper bound.
		*/
		inline const int& getPeriodUpperBound() const
		{
			return m_periodupperbound;
		}
		// DocString: FMTAction::hash
		/**
		@brief Return the hash of the action based on its name.
		@return the hash value.
		*/
		inline size_t hash() const
		{
			return boost::hash<std::string>()(m_name);
		}
		// DocString: FMTAction::getName
		/**
		@brief Get the name of the action.
		@return the name of the action.
		*/
		inline std::string getName() const
		{
			return m_name;
		}
		// DocString: FMTAction::doRespectLock
		/**
		@brief Return true if the action needs to respect the lock state of the development.
		@return true if the action respects the lock else false.
		*/
		inline bool doRespectLock() const
		{
			return m_lock;
		}
		// DocString: FMTAction::isResetAge
		/**
		@brief Return true if the development age is reset to 0 when operated by this action.
		@return true if the action resets the age else false.
		*/
		inline bool isResetAge() const
		{
			return m_reset;
		}
		// DocString: FMTAction::isPartOfASerie
		/**
		@brief Return true if the action is part of a serie.
		@return true if the action is part of a serie else false.
		*/
		inline bool isPartOfASerie() const
		{
			return !m_series.empty();
		}
		// DocString: FMTAction::isAllowedInSerie
		/**
		@brief Return true if the serie mask is part of one serie of the action.
		@param[in] serie the serie mask.
		@return true if the serie is allowed else false.
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
		@brief Return the series names of the action.
		@return the series names.
		*/
		std::vector<std::string> getSeriesNames() const;
		// DocString: FMTAction::getLargestSerieSize
		/**
		@brief Return the size of the largest serie of the action.
		@return the size of the largest serie.
		*/
		size_t getLargestSerieSize() const;
		// DocString: FMTAction::setSeries
		/**
		@brief Set the series of the action, checking which ones the action is part of and building them.
		@param[in] p_series the series to set.
		*/
		void setSeries(std::vector<Core::FMTSerie> p_series);
		// DocString: FMTAction::useYield
		/**
		@brief Return true if the yield is used by the action to set operability.
		@param[in] yldname the yield name.
		@return true if the yield is used else false.
		*/
		bool useYield(const std::string& yldname) const;
		// DocString: FMTAction::getAggregates
		/**
		@brief Return the names of the aggregates the action is part of.
		@return the aggregates of the action.
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
		@brief Return the partial yield names of the action.
		@return the partials of the action.
		*/
		std::vector<std::string> getPartials() const;
		// DocString: FMTAction::operator<
		/**
		@brief Less than comparison operator of FMTAction, testing the action length then the name.
		@param[in] rhs the action to compare with.
		@return true if this action is less than rhs else false.
		*/
		bool operator < (const FMTAction& rhs) const;
		// DocString: FMTAction::operator==
		/**
		@brief Equality comparison operator of FMTAction, checking if the actions have the same name.
		@param[in] rhs the action to compare with.
		@return true if both actions have the same name else false.
		*/
		bool operator == (const FMTAction& rhs) const;
		// DocString: FMTAction::operator!=
		/**
		@brief Inequality comparison operator of FMTAction, checking if the actions have a different name.
		@param[in] rhs the action to compare with.
		@return true if both actions have a different name else false.
		*/
		bool operator != (const FMTAction& rhs) const;
		// DocString: FMTAction::operator std::string
		/**
		@brief Convert the action to a string as in a .act file.
		@return the string representation of the action.
		*/
		operator std::string() const;
		// DocString: FMTAction::partial
		/**
		@brief Check if the yield needs to be considered as partial for this action.
		@param[in] yield the yield name.
		@return true if the yield is partial else false.
		*/
		bool partial(const std::string& yield) const;
		// DocString: FMTAction::getGCBMActionId
		/**
		@brief Return the corresponding GCBM action id.
		@return the GCBM action id.
		*/
		int getGCBMActionId() const;
		// DocString: FMTAction::getGCBMActionName
		/**
		@brief Return the corresponding GCBM action name.
		@return the GCBM action name.
		*/
		std::string getGCBMActionName() const;
		// DocString: FMTAction::split
		/**
		@brief Split the action into different actions using the masks.
		@param[in] p_mask the masks used to split the action.
		@param[in] p_themes the themes to generate the masks.
		@return the actions resulting from the split.
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
		// DocString: FMTAction::m_aggregates
		///An action can be part of a aggregate so this data member gets the name of all aggregate the action is being part of.
		std::vector<std::string> m_aggregates;
		// DocString: FMTAction::m_partials
		///Keeps the yields name for determining the amount of wood harvested in case of partial cut.
        std::vector<std::string> m_partials;
		// DocString: FMTAction::m_agelowerbound
		///Those data members are for optimization only,
		///the class determine within which bounds the aciton can take place for a given development.
		int m_agelowerbound, m_ageupperbound, m_periodlowerbound, m_periodupperbound;
		// DocString: FMTAction::m_name
		///The name of the action
		std::string m_name;
		// DocString: FMTAction::m_lock
		///If lock is true the action is not _lockexempt when false the action is _LOCKEXEMPT
		bool m_lock;
		// DocString: FMTAction::m_reset
		///If reset is true then the action is age reset Y else the action  doen't reset age
		bool m_reset;
		// DocString: FMTAction::m_series
		///The action series that the action is part of
		std::vector<FMTSerie> m_series;
		// DocString: FMTAction::m_InSerie
		///True if the actions is in a serie
		bool m_InSerie;
		// DocString: FMTAction::_setBounds
		/**
		@brief Set the age and period bounds member data by iterating on the specifications, for optimization.
		*/
		void _setBounds();
		// DocString: FMTAction::_getGCBMActionDef
		/**
		@brief Return the corresponding GCBM action definition.
		@return the GCBM action definition.
		*/
		std::vector<std::string> _getGCBMActionDef() const;
	private:
		// DocString: FMTAction::serialize
		/**
		@brief Serialize the FMTAction through its base FMTList<FMTSpec> for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {
				ar& boost::serialization::make_nvp("specs", boost::serialization::base_object<FMTList<FMTSpec>>(*this));
				ar& boost::serialization::make_nvp("partials", m_partials);
				ar& boost::serialization::make_nvp("agelowerbound", m_agelowerbound);
				ar& boost::serialization::make_nvp("ageupperbound", m_ageupperbound);
				ar& boost::serialization::make_nvp("periodlowerbound", m_periodlowerbound);
				ar& boost::serialization::make_nvp("periodupperbound", m_periodupperbound);
				ar& boost::serialization::make_nvp("name", m_name);
				ar& boost::serialization::make_nvp("aggregates", m_aggregates);
				ar& boost::serialization::make_nvp("lock", m_lock);
				ar& boost::serialization::make_nvp("reset", m_reset);
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTAction::serialize", __LINE__, __FILE__);
			}
		}
    };

// DocString: FMTActionComparator
/**
@brief Comparator used to check if an action name (or aggregate) already exists in a std container.
*/
class FMTActionComparator
	{
	public:
		// DocString: FMTActionComparator(std::string,bool)
		/**
		@brief Construct the comparator from the action name to match.
		@param[in] p_name the name of the action to match.
		@param[in] lcheckaggregate if true also returns actions within the named aggregate.
		*/
		FMTActionComparator(std::string p_name, bool lcheckaggregate = false);
		// DocString: FMTActionComparator::getAllAggregates
		/**
		@brief Return the actions matching the action name, or only the matching aggregates.
		@param[in] actions the actions to search.
		@param[in] aggregateonly if true only returns matching aggregates.
		@return the matching actions.
		*/
		std::vector<const FMTAction*> getAllAggregates(const std::vector<FMTAction>&actions,bool aggregateonly = false) const;
		// DocString: FMTActionComparator::getAllAggregatesSet
		/**
		@brief Return the ids of the actions matching the action name, or only the matching aggregates.
		@param[in] actions the actions to search.
		@param[in] aggregateonly if true only returns matching aggregates.
		@return the ids of the matching actions.
		*/
		std::unordered_set<int> getAllAggregatesSet(const std::vector<FMTAction>&actions, bool aggregateonly = false) const;
		// DocString: FMTActionComparator::operator()(const FMTAction&)
		/**
		@brief Matching test operator for the comparator.
		@param[in] action the action to test.
		@return true if the action matches else false.
		*/
		bool operator()(const FMTAction& action) const;
	private:
	// DocString: FMTActionComparator::m_actionName
	///The action named that we are looking for.
	std::string m_actionName;
	// DocString: FMTActionComparator::m_checkAggregate
	///If true the comparator will also check for aggregates.
	bool m_checkAggregate;
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
