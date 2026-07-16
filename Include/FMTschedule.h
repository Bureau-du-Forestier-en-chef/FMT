/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTschedule_Hm_included
#define FMTschedule_Hm_included

#include <map>
#include <vector>
#include "FMTdevelopment.h"
#include "FMTaction.h"
#include "FMTobject.h"
#include <iterator>
#include <boost/serialization/export.hpp>
#include <boost/unordered_set.hpp>

namespace Core
{
class FMTConstraint;
// DocString: FMTSchedule
/**
FMTSchedule is the representation of a spatially referenced schedule. It can be used by the FMTLpModel and the FMTSesModel.
It describes the area of a given developments for which an FMTAction can be operated (key).
The vector is a reprensentation of lock level at position 0 in the vector the lock level = 0 (if lock level 0 exist in the graph
, at position 1 lock level = 1.
*/
class FMTEXPORT FMTSchedule : public FMTObject
    {
    public:
		// DocString: FMTSchedule(const int&)
		/**
		FMTSchedule constructor for an empty schedule at given period
		*/
		FMTSchedule(const int& lperiod, const FMTObject& obj, const bool& luselock);
		// DocString: FMTSchedule(const int&,std::vector<FMTSchedule>&)
		/**
		FMTSchedule constructor for a complete construction of FMTSchedule with multiple FMTSchedule
		*/
		FMTSchedule(const int& lperiod, std::vector<FMTSchedule>& schedules);
		// DocString: FMTSchedule(int,std::map<FMTAction,std::map<FMTDevelopment,std::vector<double>>>)
		/**
		FMTSchedule constructor for a complete construction of FMTSchedule
		*/
        FMTSchedule(int lperiod, std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>> mapping);
		// DocString: FMTSchedule(int,const std::map<FMTAction,std::map<FMTDevelopment,std::map<int,double>>>&)
		/**
		FMTSchedule constructor _lock based mapping (When a schedule is parsed).
		*/
		FMTSchedule(int lperiod, const std::map<FMTAction, std::map<FMTDevelopment, std::map<int,double>>>& mapping);
		// DocString: FMTSchedule()
		/**
		Default FMTSchedule constructor.
		*/
        FMTSchedule();
		// DocString: ~FMTSchedule()
		/**
		Default ~FMTSchedule constructor.
		*/
		~FMTSchedule() = default;
		// DocString: FMTSchedule(const FMTSchedule&)
		/**
		Default FMTSchedule copy constructor.
		*/
        FMTSchedule(const FMTSchedule& rhs);
		// DocString: FMTSchedule::operator=
		/**
		Default FMTSchedule copy assignment.
		*/
        FMTSchedule& operator = (const FMTSchedule& rhs);
		// DocString: FMTSchedule::operator==
		/**
		FMTSchedule equality operator.
		*/
		bool operator == (const FMTSchedule& rhs) const;
		// DocString: FMTSchedule::operator!=
		/**
		FMTSchedule nonequality operator.
		*/
		bool operator != (const FMTSchedule& rhs) const;
		// DocString: FMTSchedule::operator+=
		/**
		This function append an FMTSchedule to an other FMTSchedule.
		*/
		FMTSchedule& operator += (const FMTSchedule& rhs);
		// DocString: FMTSchedule::operator+
		/**
		This function append an FMTSchedule to an other FMTSchedule and return a new one.
		*/
		FMTSchedule operator + (const FMTSchedule& rhs) const;
		// DocString: FMTSchedule::operator std::string
		/**
		Convert the FMTSchedule to a string like seen in a regular scedule file.
		*/
        operator std::string() const;
		// DocString: FMTSchedule::getPeriod
		/**
		Getter for the schedule period.
		*/
		inline int getPeriod() const
			{
			return period;
			}
		// DocString: FMTSchedule::doUseLock
		/**
		Return true if the schedule can contains locked development.
		*/
		inline bool doUseLock() const
		{
			return uselock;
		}
		// DocString: FMTSchedule::setUseLock
		/**
		Mark the schedule has can contains locked development.
		*/
		void setUseLock(const bool& lock);
		// DocString: FMTSchedule::actionArea
		/**
		Return the total harvested area of a FMTAction present in the FMTSchedule.
		*/
        double actionArea(const FMTAction& action) const;
		// DocString: FMTSchedule::addEvent
		/**
		Add a new event to the shedule based on the development,action and area. See clean function also.
		*/
		void addEvent(const Core::FMTDevelopment& dev, const double& area, const Core::FMTAction& action);
		// DocString: FMTSchedule::clean
		/**
		After calling multiple time addEvent the schedule will end up with empty area if uselock == false.
		So you need to call this function to clean the schedule before using it.
		*/
		void clean();
		// DocString: FMTSchedule::area
		/**
		Return the total area of the schedule operated by all schedule actions.
		*/
        double area() const;
		// DocString: FMTSchedule::operated
		/**
		Check if a given FMTDevelopment has been actioned for the this FMTSchedule.
		*/
		bool operated(const FMTAction& action,
			const FMTDevelopment& development) const;
		// DocString: FMTSchedule::getOperabilities
		/**
		Get all operabilities of the schedules in an hash table.
		*/
		std::vector<boost::unordered_set<FMTDevelopment>> getOperabilities(const std::vector<FMTAction>& actions) const;
		// DocString: FMTSchedule::sort
		/**
		Sort the vector of double of the lock level per area for the FMTSchedule.
		*/
		void sort();
		// DocString: FMTSchedule::empty
		/**
		Return true is the FMTSchedule is not empty.
		*/
		bool empty() const;
		// DocString: FMTSchedule::size
		/**
		Return the size of the FMTSchedule.
		*/
		size_t size() const;
		// DocString: FMTSchedule::presolve
		/**
		Presolve all masks base on a baseMask a preolved mask
		and presolved themes and presolved actions list. The returned schedule can also be empty.
		*/
		FMTSchedule presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& newthemes, const std::vector<FMTAction>&presolvedaction) const;
		// DocString: FMTSchedule::postSolve
		/**
		Postsolve all masks base on a baseMask and the original themes and actions.
		*/
		FMTSchedule postSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalbasethemes, const std::vector<FMTAction>&originalbasebaseactions) const;
		/**
		Setter for the period of the Schedule and the period developments of the schedule.
		*/
		void setPeriod(const int& newperiod);
		// DocString: FMTSchedule::getNewSchedule
		/**
		Multiply this schedule with a factor and return it
		*/
		FMTSchedule getNewSchedule(const double& factor) const;
		// DocString: FMTSchedule::isFuturConstraints
		/**
		Check if the schedule period will be bounded by new constraints.
		*/
		bool isFuturConstraints(const std::vector<Core::FMTConstraint>& constraints) const;
		/**
		Functions that make sure the FMTSchedule acts like a std c++ container
		*/
		typedef typename std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::value_type value_type;
		typedef typename std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::iterator iterator;
		typedef typename std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator const_iterator;
		std::map<FMTDevelopment, std::vector<double>>& operator[](const FMTAction& action);
		const std::map<FMTDevelopment, std::vector<double>>& at(const FMTAction& action) const;
		iterator find(const FMTAction& actionkey);
		const_iterator find(const FMTAction& actionkey) const;
		iterator begin();
		const_iterator begin() const;
		iterator  end();
		const_iterator end() const;
	private:
		// DocString: FMTSchedule::serialize
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {
				ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
				ar& BOOST_SERIALIZATION_NVP(period);
				ar& BOOST_SERIALIZATION_NVP(elements);
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTSchedule::serialize", __LINE__, __FILE__);
			}
		}
		// DocString: FMTSchedule::period
		///The period at which the schedule needs to be applied
		int period;
		// DocString: FMTSchedule::uselock
		///If true the schedule contains locked development by default it is False.
		bool uselock;
		// DocString: FMTSchedule::elements
		///Main FMTSchedule elements action has key, followed by a map of FMTDevelopment and a vector of double
		///The vector of double represent the _lock level.
		std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>> elements;
		// DocString: FMTSchedule::sameElements
		/**
		Return true if the FMTSchedule contains the same elements.
		*/
		bool sameElements(const FMTSchedule& rhs) const;
    };


}
BOOST_CLASS_EXPORT_KEY(Core::FMTSchedule)
#endif // FMTschedule_Hm_included
