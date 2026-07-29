/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTschedule_Hm_included
#define FMTschedule_Hm_included

#include <map>
#include <vector>
#include "FMTDevelopment.h"
#include "FMTAction.h"
#include "FMTObject.h"
#include <iterator>
#include <boost/serialization/export.hpp>
#include <boost/unordered_set.hpp>

namespace Core
{
class FMTConstraint;
// DocString: FMTSchedule
/**
@brief Spatially referenced schedule describing the area of developments for which an action can be operated, usable by FMTLpModel and FMTSesModel.
@details The vector represents the lock level: position 0 is lock level 0, position 1 is lock level 1, and so on.
*/
class FMTEXPORT FMTSchedule : public FMTObject
    {
    public:
		// DocString: FMTSchedule(const int&)
		/**
		@brief Construct an empty schedule at a given period.
		@param[in] lperiod the period.
		@param[in] obj the object to copy the handlers from.
		@param[in] luselock if true the schedule can contain locked developments.
		*/
		FMTSchedule(const int& lperiod, const FMTObject& obj, const bool& luselock);
		// DocString: FMTSchedule(const int&,std::vector<FMTSchedule>&)
		/**
		@brief Construct a schedule from multiple schedules at a given period.
		@param[in] lperiod the period.
		@param[in] schedules the schedules to combine.
		*/
		FMTSchedule(const int& lperiod, std::vector<FMTSchedule>& schedules);
		// DocString: FMTSchedule(int,std::map<FMTAction,std::map<FMTDevelopment,std::vector<double>>>)
		/**
		@brief Construct a complete schedule from a period and a mapping.
		@param[in] lperiod the period.
		@param[in] mapping the mapping of actions to developments and areas.
		*/
        FMTSchedule(int lperiod, std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>> mapping);
		// DocString: FMTSchedule(int,const std::map<FMTAction,std::map<FMTDevelopment,std::map<int,double>>>&)
		/**
		@brief Construct a schedule from a period and a lock based mapping, when a schedule is parsed.
		@param[in] lperiod the period.
		@param[in] mapping the lock based mapping.
		*/
		FMTSchedule(int lperiod, const std::map<FMTAction, std::map<FMTDevelopment, std::map<int,double>>>& mapping);
		// DocString: FMTSchedule()
		/**
		@brief Default constructor for FMTSchedule.
		*/
        FMTSchedule();
		// DocString: ~FMTSchedule()
		/**
		@brief Default destructor for FMTSchedule.
		*/
		~FMTSchedule() = default;
		// DocString: FMTSchedule(const FMTSchedule&)
		/**
		@brief Copy constructor for FMTSchedule.
		@param[in] rhs the FMTSchedule to copy.
		*/
        FMTSchedule(const FMTSchedule& rhs);
		// DocString: FMTSchedule::operator=
		/**
		@brief Copy assignment operator for FMTSchedule.
		@param[in] rhs the FMTSchedule to copy.
		@return a reference to this FMTSchedule.
		*/
        FMTSchedule& operator = (const FMTSchedule& rhs);
		// DocString: FMTSchedule::operator==
		/**
		@brief Equality comparison operator of FMTSchedule.
		@param[in] rhs the schedule to compare with.
		@return true if both schedules are equal else false.
		*/
		bool operator == (const FMTSchedule& rhs) const;
		// DocString: FMTSchedule::operator!=
		/**
		@brief Inequality comparison operator of FMTSchedule.
		@param[in] rhs the schedule to compare with.
		@return true if both schedules are different else false.
		*/
		bool operator != (const FMTSchedule& rhs) const;
		// DocString: FMTSchedule::operator+=
		/**
		@brief Append another schedule to this schedule.
		@param[in] rhs the schedule to append.
		@return a reference to this FMTSchedule.
		*/
		FMTSchedule& operator += (const FMTSchedule& rhs);
		// DocString: FMTSchedule::operator+
		/**
		@brief Append another schedule to this schedule and return a new one.
		@param[in] rhs the schedule to append.
		@return the resulting schedule.
		*/
		FMTSchedule operator + (const FMTSchedule& rhs) const;
		// DocString: FMTSchedule::operator std::string
		/**
		@brief Convert the schedule to a string as in a regular schedule file.
		@return the string representation of the schedule.
		*/
        operator std::string() const;
		// DocString: FMTSchedule::getPeriod
		/**
		@brief Return the period of the schedule.
		@return the period of the schedule.
		*/
		inline int getPeriod() const
			{
			return m_period;
			}
		// DocString: FMTSchedule::doUseLock
		/**
		@brief Return true if the schedule can contain locked developments.
		@return true if the schedule uses lock else false.
		*/
		inline bool doUseLock() const
		{
			return m_uselock;
		}
		// DocString: FMTSchedule::setUseLock
		/**
		@brief Mark the schedule as able to contain locked developments.
		@param[in] lock if true the schedule can contain locked developments.
		*/
		void setUseLock(const bool& lock);
		// DocString: FMTSchedule::actionArea
		/**
		@brief Return the total harvested area of an action in the schedule.
		@param[in] action the action.
		@return the total harvested area of the action.
		*/
        double actionArea(const FMTAction& action) const;
		// DocString: FMTSchedule::addEvent
		/**
		@brief Add a new event to the schedule from a development, an area and an action.
		@param[in] dev the development.
		@param[in] area the area.
		@param[in] action the action.
		*/
		void addEvent(const Core::FMTDevelopment& dev, const double& area, const Core::FMTAction& action);
		// DocString: FMTSchedule::clean
		/**
		@brief Clean the schedule of empty areas after multiple addEvent calls, to be called before using it.
		*/
		void clean();
		// DocString: FMTSchedule::area
		/**
		@brief Return the total area of the schedule operated by all actions.
		@return the total area of the schedule.
		*/
        double area() const;
		// DocString: FMTSchedule::operated
		/**
		@brief Check if a development has been operated by an action in the schedule.
		@param[in] action the action.
		@param[in] development the development.
		@return true if the development has been operated else false.
		*/
		bool operated(const FMTAction& action,
			const FMTDevelopment& development) const;
		// DocString: FMTSchedule::getOperabilities
		/**
		@brief Return all operabilities of the schedule in a hash table.
		@param[in] actions the actions.
		@return the operabilities of the schedule.
		*/
		std::vector<boost::unordered_set<FMTDevelopment>> getOperabilities(const std::vector<FMTAction>& actions) const;
		// DocString: FMTSchedule::sort
		/**
		@brief Sort the vector of lock level per area of the schedule.
		*/
		void sort();
		// DocString: FMTSchedule::empty
		/**
		@brief Return true if the schedule is empty.
		@return true if the schedule is empty else false.
		*/
		bool empty() const;
		// DocString: FMTSchedule::size
		/**
		@brief Return the size of the schedule.
		@return the size of the schedule.
		*/
		size_t size() const;
		// DocString: FMTSchedule::preSolve
		/**
		@brief Presolve the schedule, potentially returning an empty schedule.
		@param[in] filter the mask filter.
		@param[in] newthemes the presolved themes.
		@param[in] presolvedaction the presolved actions.
		@return the presolved schedule.
		*/
		FMTSchedule preSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& newthemes, const std::vector<FMTAction>&presolvedaction) const;
		// DocString: FMTSchedule::postSolve
		/**
		@brief Postsolve the schedule using the original themes and actions.
		@param[in] filter the mask filter.
		@param[in] originalbasethemes the original themes.
		@param[in] originalbasebaseactions the original actions.
		@return the postsolved schedule.
		*/
		FMTSchedule postSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalbasethemes, const std::vector<FMTAction>&originalbasebaseactions) const;
		// DocString: FMTSchedule::setPeriod
		/**
		@brief Set the period of the schedule and of its developments.
		@param[in] newPeriod the period to set.
		*/
		void setPeriod(const int& newPeriod);
		// DocString: FMTSchedule::getNewSchedule
		/**
		@brief Multiply this schedule by a factor and return it.
		@param[in] factor the factor.
		@return the multiplied schedule.
		*/
		FMTSchedule getNewSchedule(const double& factor) const;
		// DocString: FMTSchedule::isFuturConstraints
		/**
		@brief Check if the schedule period will be bounded by new constraints.
		@param[in] constraints the constraints.
		@return true if the schedule has future constraints else false.
		*/
		bool isFuturConstraints(const std::vector<Core::FMTConstraint>& constraints) const;
		/**
		Functions that make sure the FMTSchedule acts like a std c++ container
		*/
		typedef typename std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::value_type value_type;
		typedef typename std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::iterator iterator;
		typedef typename std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator const_iterator;
		// DocString: FMTSchedule::operator[]
		/**
		@brief Return a reference to the developments and areas for a given action.
		@param[in] action the action.
		@return a reference to the map of developments to areas.
		*/
		std::map<FMTDevelopment, std::vector<double>>& operator[](const FMTAction& action);
		// DocString: FMTSchedule::at
		/**
		@brief Return a const reference to the developments and areas for a given action.
		@param[in] action the action.
		@return a const reference to the map of developments to areas.
		*/
		const std::map<FMTDevelopment, std::vector<double>>& at(const FMTAction& action) const;
		// DocString: FMTSchedule::find
		/**
		@brief Return an iterator to the given action.
		@param[in] actionkey the action.
		@return an iterator to the action.
		*/
		iterator find(const FMTAction& actionkey);
		// DocString: FMTSchedule::find() const
		/**
		@brief Return a const iterator to the given action.
		@param[in] actionkey the action.
		@return a const iterator to the action.
		*/
		const_iterator find(const FMTAction& actionkey) const;
		// DocString: FMTSchedule::begin
		/**
		@brief Return an iterator to the beginning of the schedule.
		@return an iterator to the beginning.
		*/
		iterator begin();
		// DocString: FMTSchedule::begin() const
		/**
		@brief Return a const iterator to the beginning of the schedule.
		@return a const iterator to the beginning.
		*/
		const_iterator begin() const;
		// DocString: FMTSchedule::end
		/**
		@brief Return an iterator to the end of the schedule.
		@return an iterator to the end.
		*/
		iterator  end();
		// DocString: FMTSchedule::end() const
		/**
		@brief Return a const iterator to the end of the schedule.
		@return a const iterator to the end.
		*/
		const_iterator end() const;
	private:
		// DocString: FMTSchedule::serialize
		/**
		@brief Serialize the FMTSchedule through its base FMTObject for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {
				ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
				ar& boost::serialization::make_nvp("period", m_period);
				ar& boost::serialization::make_nvp("elements", m_elements);
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTSchedule::serialize", __LINE__, __FILE__);
			}
		}
		// DocString: FMTSchedule::m_period
		///The period at which the schedule needs to be applied
		int m_period;
		// DocString: FMTSchedule::m_uselock
		///If true the schedule contains locked development by default it is False.
		bool m_uselock;
		// DocString: FMTSchedule::m_elements
		///Main FMTSchedule elements action has key, followed by a map of FMTDevelopment and a vector of double
		///The vector of double represent the _lock level.
		std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>> m_elements;
		// DocString: FMTSchedule::_sameElements
		/**
		@brief Return true if the schedule contains the same elements as another.
		@param[in] rhs the schedule to compare with.
		@return true if both schedules contain the same elements else false.
		*/
		bool _sameElements(const FMTSchedule& rhs) const;
    };


}
BOOST_CLASS_EXPORT_KEY(Core::FMTSchedule)
#endif // FMTschedule_Hm_included
