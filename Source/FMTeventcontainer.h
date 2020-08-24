/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTeventCONTAINER_H
#define FMTeventCONTAINER_H

#include "FMTevent.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/set.hpp>


namespace Spatial
{
// DocString: FMTeventcontainer
/**
This class is a container for FMTevent. It's build to handle event
comparison or distance calculation in an efficient way by allowing the
user to get events by period, action id and/or coordinates.
One container can hold all events for a model. They are sorted by
period, centroid, action_id and finally by size.
You can  remove FMTcoordinate and add FMTcoordinate by period and action.
*/
class FMTeventcontainer
{
    friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
        ar & BOOST_SERIALIZATION_NVP(events);
	}

    public:
        typedef typename std::set<FMTevent>::value_type value_type;
        typedef typename std::set<FMTevent>::iterator iterator;
        typedef typename std::set<FMTevent>::const_iterator const_iterator;
        typedef typename std::set<FMTevent>::const_reverse_iterator const_reverse_iterator;
        // DocString: FMTeventcontainer()
        /**
        Default constructor
        */
        FMTeventcontainer();
        // DocString: ~FMTeventcontainer()
        /**
        Destructor
        */
        ~FMTeventcontainer()=default;
        // DocString: FMTeventcontainer(const FMTeventcontainer&)
        /**
        Copy constructor
        */
        FMTeventcontainer(const FMTeventcontainer& rhs);
        // DocString: FMTeventcontainer::operator=
        /**
        Copy assignment operator
        */
        FMTeventcontainer& operator=(const FMTeventcontainer& rhs);
        // DocString: FMTeventcontainer::operator==
        /**
        Comparison operator equal to
        */
        bool operator==(const FMTeventcontainer& rhs) const;
         // DocString: FMTeventcontainer::swap
        /**
        Exchange contents of events with the content of rhs.events
        */
        void swap(FMTeventcontainer& rhs);
        // DocString: FMTeventcontainer::begin
        /**
        Return a const iterator to the first FMTevent in events.
        */
        const_iterator begin() const{return events.cbegin();}
        // DocString: FMTeventcontainer::end
        /**
        Return a const iterator to the past-the-end element in events.
        */
		const_iterator end() const{return events.cend();}
        // DocString: FMTeventcontainer::empty
        /**
        Test whether the container is empty.
        */
		bool empty() const {return events.empty();}
		// DocString: FMTeventcontainer::size
        /**
        Return container size.
        */
        size_t size() const {return events.size();}
        // DocString: FMTeventcontainer::find
        /**
        Get const iterator to the first event found with the coord at the period specified.
        */
		const_iterator find(const int& period, const FMTcoordinate& coord) const;
		// DocString: FMTeventcontainer::find
		/**
		Get const iterator to the element.
		*/
		const_iterator find(const FMTevent& event) const { return events.find(event); }
        // DocString: FMTeventcontainer::insert
        /**
        Insert an event in the container.
        */
        void insert(const FMTevent& event){events.insert(event);}
		// DocString: FMTeventcontainer::merge
		/**
		Merge two FMTeventcontainer
		*/
		void merge(const FMTeventcontainer& levents);
        // DocString: FMTeventcontainer::erase(const FMTevent&)
        /**
        Erase an event from the container.
        */
        void erase(const FMTevent& event);
        // DocString: FMTeventcontainer::erase(const const_iterator)
        /**
        Erase an event from the container and return the next iterator.
        */
        const_iterator erase (const const_iterator event);
        // DocString: FMTeventcontainer::clear
        /**
        Clear content of the container.
        */
        void clear (){events.clear();}
        // DocString: FMTeventcontainer::lasperiod
        /*
        Return the period of the last event in the container
        */
        int lastperiod()const;
        // DocString: FMTeventcontainer::firstperiod
        /*
        Return the period of the first event in the container
        */
        int firstperiod()const;
        // DocString: FMTeventcontainer::erasecoordinate
        /*
        Erase all coordinate corresponding to coord in events from the period start.
        */
        void erasecoordinate (const FMTcoordinate coord, const int& periodstart);
        // DocString: FMTeventcontainer::getbounds(const int&)
        /*
        Return a pair of iterator pointing to the first and the last elements in the period
        */
        std::pair<const_iterator,const_iterator> getbounds(const int& period) const;
        // DocString: FMTeventcontainer::addaction()
        /*
        Add an action at coordinate and add to existing event if possible or aggregate events
        if possible.
        */
        void addaction (const FMTcoordinate coord, const int& period,const int& actionid);
        // DocString: FMTeventcontainer::getevents(const int&, const int&)
        /*
        Get events at specified period with specified action
        */
        std::vector<const_iterator> getevents(const int& period, const int& action_id) const;
         // DocString: FMTeventcontainer::getevents(const int&, const std::vector<int>&)
        /*
        Get events at specified period with multiple actions id
        */
        std::vector<const_iterator> getevents(const int& period, const std::vector<int>& actions_id) const;
        // DocString: FMTeventcontainer::getevents(const int&, const FMTcoordinate&)
        /*
        ///Get events at specified period with specified coordinates
        */
        std::vector<const_iterator> getevents(const int& period, const FMTcoordinate& coord) const;
        // DocString: FMTeventcontainer::getevents(const int&, const std::set<FMTcoordinate>&)
        /*
        ///Get events at specified period with multiple specified coordinates
        */
        std::vector<const_iterator> getevents(const int& period, const std::set<FMTcoordinate>& territory) const;
        // DocString: FMTeventcontainer::minimaldistance
        /**
        This function is used when we want to verify if the event is within a certain distance from
        other events. If it's not, the specified distance distance+1 is return, else the minimal distance
        is return. It's principally use to calculate penalty for simulated annealing models.
        */
        double minimaldistance(const FMTevent&,const unsigned int& distancel,const int& period, const std::vector<int>& actionsid) const;

     private:
        ///Set containing the events
        std::set<FMTevent> events;
        //Actually not needed
        /*iterator end(){return events.end();}
        iterator begin(){return events.begin();}*/
        ///Returns an iterator pointing to the first element in the container which is considered to go after val.
        const_iterator upper_bound(const int& period) const;
        ///Returns an iterator pointing to the first element in the container which is considered to go after val.
        const_iterator upper_bound(const int& period,const FMTcoordinate& maxxycoord) const;
        ///Returns an iterator pointing to the first element in the container which is not considered to go before val.
        ///(i.e., either it is equivalent or goes after)
        const_iterator lower_bound(const int& period) const;
        ///Returns an iterator pointing to the first element in the container which is not considered to go before val.
        ///(i.e., either it is equivalent or goes after)
        const_iterator lower_bound(const int& period, const FMTcoordinate& minxycoord) const;

};
}
#endif // FMTeventCONTAINER_H
