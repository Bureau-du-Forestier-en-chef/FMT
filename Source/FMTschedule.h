/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTschedule_H_INCLUDED
#define FMTschedule_H_INCLUDED

#include <map>
#include <vector>
#include "FMTdevelopment.h"
#include "FMTaction.h"
#include "FMTobject.h"
#include "FMTconstraint.h"
#include <iterator>
#include <boost/serialization/export.hpp>
#include <boost/unordered_set.hpp>

namespace Core
{
// DocString: FMTschedule
/**
FMTschedule is the representation of a spatialy referenced schedule. It can be used by the FMTlpmodel and the FMTsesmodel.
It describes the area of a given developements for which an FMTaction can be operated (key).
The vector is a reprensentation of lock level at position 0 in the vector the lock level = 0 (if lock level 0 exist in the graph
, at position 1 lock level = 1.
*/
class FMTschedule : public FMTobject
    {
	// DocString: FMTschedule::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		try {
			ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
			ar & BOOST_SERIALIZATION_NVP(period);
			ar & BOOST_SERIALIZATION_NVP(elements);
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTschedule::serialize", __LINE__, __FILE__);
		}
	}
	// DocString: FMTschedule::period
	///The period at which the schedule needs to be applied
    int period;
	// DocString: FMTschedule::uselock
	///If true the schedule contains locked development by default it is False.
	bool uselock;
	// DocString: FMTschedule::elements
	///Main FMTschedule elements action has key, followed by a map of FMTdevelopement and a vector of double
	///The vector of double represent the _lock level.
	std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>> elements;
	// DocString: FMTschedule::sameelements
	/**
	Return true if the FMTschedule contains the same elements.
	*/
	bool sameelements(const FMTschedule& rhs) const;
    public:
		// DocString: FMTschedule(const int&)
		/**
		FMTschedule constructor for an empty schedule at given period
		*/
		FMTschedule(const int& lperiod, const FMTobject& obj, const bool& luselock);
		// DocString: FMTschedule(const int&,std::vector<FMTschedule>&)
		/**
		FMTschedule constructor for a complete construction of FMTschedule with multiple FMTschedule
		*/
		FMTschedule(const int& lperiod, std::vector<FMTschedule>& schedules);
		// DocString: FMTschedule(int,std::map<FMTaction,std::map<FMTdevelopment,std::vector<double>>>)
		/**
		FMTschedule constructor for a complete construction of FMTschedule
		*/
        FMTschedule(int lperiod, std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>> mapping);
		// DocString: FMTschedule(int,const std::map<FMTaction,std::map<FMTdevelopment,std::map<int,double>>>&)
		/**
		FMTschedule constructor _lock based mapping (When a schedule is parsed).
		*/
		FMTschedule(int lperiod, const std::map<FMTaction, std::map<FMTdevelopment, std::map<int,double>>>& mapping);
		// DocString: FMTschedule()
		/**
		Default FMTschedule constructor.
		*/
        FMTschedule();
		// DocString: ~FMTschedule()
		/**
		Default ~FMTschedule constructor.
		*/
		~FMTschedule() = default;
		// DocString: FMTschedule(const FMTschedule&)
		/**
		Default FMTschedule copy constructor.
		*/
        FMTschedule(const FMTschedule& rhs);
		// DocString: FMTschedule::operator=
		/**
		Default FMTschedule copy assignment.
		*/
        FMTschedule& operator = (const FMTschedule& rhs);
		// DocString: FMTschedule::operator==
		/**
		FMTschedule equality operator.
		*/
		bool operator == (const FMTschedule& rhs) const;
		// DocString: FMTschedule::operator!=
		/**
		FMTschedule nonequality operator.
		*/
		bool operator != (const FMTschedule& rhs) const;
		// DocString: FMTschedule::operator+=
		/**
		This function append an FMTschedule to an other FMTschedule.
		*/
		FMTschedule& operator += (const FMTschedule& rhs);
		// DocString: FMTschedule::operator+
		/**
		This function append an FMTschedule to an other FMTschedule and return a new one.
		*/
		FMTschedule operator + (const FMTschedule& rhs) const;
		// DocString: FMTschedule::operator std::string
		/**
		Convert the FMTschedule to a string like seen in a regular scedule file.
		*/
        operator std::string() const;
		// DocString: FMTschedule::getperiod
		/**
		Getter for the schedule period.
		*/
		inline int getperiod() const
			{
			return period;
			}
		// DocString: FMTschedule::douselock
		/**
		Return true if the schedule can contains locked developement.
		*/
		inline bool douselock() const
		{
			return uselock;
		}
		// DocString: FMTschedule::setuselock
		/**
		Mark the schedule has can contains locked developement.
		*/
		void setuselock(const bool& lock);
		// DocString: FMTschedule::actionarea
		/**
		Return the total harvested area of a FMTaction present in the FMTschedule.
		*/
        double actionarea(const FMTaction& action) const;
		// DocString: FMTschedule::addevent
		/**
		Add a new event to the shedule based on the developement,action and area. See clean function also.
		*/
		void addevent(const Core::FMTdevelopment& dev, const double& area, const Core::FMTaction& action);
		// DocString: FMTschedule::clean
		/**
		After calling multiple time addevent the schedule will end up with empty area if uselock == false.
		So you need to call this function to clean the schedule before using it.
		*/
		void clean();
		// DocString: FMTschedule::area
		/**
		Return the total area of the schedule operated by all schedule actions.
		*/
        double area() const;
		// DocString: FMTschedule::operated
		/**
		Check if a given FMTdevelopment has been actioned for the this FMTschedule.
		*/
		bool operated(const FMTaction& action,
			const FMTdevelopment& developement) const;
		// DocString: FMTschedule::getoperabilities
		/**
		Get all operabilities of the schedules in an hash table.
		*/
		std::vector<boost::unordered_set<FMTdevelopment>> getoperabilities(const std::vector<FMTaction>& actions) const;
		// DocString: FMTschedule::sort
		/**
		Sort the vector of double of the lock level per area for the FMTschedule.
		*/
		void sort();
		// DocString: FMTschedule::empty
		/**
		Return true is the FMTschedule is not empty.
		*/
		bool empty() const;
		// DocString: FMTschedule::size
		/**
		Return the size of the FMTschedule.
		*/
		size_t size() const;
		// DocString: FMTschedule::presolve
		/**
		Presolve all masks base on a basemask a preolved mask
		and presolved themes and presolved actions list. The returned schedule can also be empty.
		*/
		FMTschedule presolve(const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& newthemes, const std::vector<FMTaction>&presolvedaction) const;
		// DocString: FMTschedule::postsolve
		/**
		Postsolve all masks base on a basemask and the original themes and actions.
		*/
		FMTschedule postsolve(const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& originalbasethemes, const std::vector<FMTaction>&originalbasebaseactions) const;
		/**
		Setter for the period of the Schedule and the period developements of the schedule.
		*/
		void setperiod(const int& newperiod);
		// DocString: FMTschedule::getnewschedule
		/**
		Multiply this schedule with a factor and return it
		*/
		FMTschedule getnewschedule(const double& factor) const;
		// DocString: FMTschedule::isfuturconstraints
		/**
		Check if the schedule period will be bounded by new constraints.
		*/
		bool isfuturconstraints(const std::vector<Core::FMTconstraint>& constraints) const;
		/**
		Functions that make sure the FMTschedule acts like a std c++ container
		*/
		typedef typename std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::value_type value_type;
		typedef typename std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::iterator iterator;
		typedef typename std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator const_iterator;
		std::map<FMTdevelopment, std::vector<double>>& operator[](const FMTaction& action);
		const std::map<FMTdevelopment, std::vector<double>>& at(const FMTaction& action) const;
		iterator find(const FMTaction& actionkey);
		const_iterator find(const FMTaction& actionkey) const;
		iterator begin();
		const_iterator begin() const;
		iterator  end();
		const_iterator end() const;
    };


}
BOOST_CLASS_EXPORT_KEY(Core::FMTschedule)
#endif // FMTschedule_H_INCLUDED
