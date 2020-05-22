/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSAEVENTCONTAINER_H
#define FMTSAEVENTCONTAINER_H

#include "FMTsaevent.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/set.hpp>


namespace Spatial
{
// DocString: FMTsaeventcontainer
/**
This class is a container for FMTsaevent. It's build to handle event
comparison or distance calculation in an efficient way by allowing the
user to get events by period, action id and/or coordinates.
One container can hold all events for a model. They are sorted by
period, centroid, action_id and finally by size.
You can  remove FMTcoordinate and add FMTcoordinate by period and action.
*/
class FMTsaeventcontainer
{
    friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
        ar & BOOST_SERIALIZATION_NVP(events);
	}

    public:
        typedef typename std::set<FMTsaevent>::value_type value_type;
        typedef typename std::set<FMTsaevent>::iterator iterator;
        typedef typename std::set<FMTsaevent>::const_iterator const_iterator;
        typedef typename std::set<FMTsaevent>::const_reverse_iterator const_reverse_iterator;
        // DocString: FMTsaeventcontainer()
        /**
        Default constructor
        */
        FMTsaeventcontainer();
        // DocString: ~FMTsaeventcontainer()
        /**
        Destructor
        */
        ~FMTsaeventcontainer()=default;
        // DocString: FMTsaeventcontainer(const FMTsaeventcontainer&)
        /**
        Copy constructor
        */
        FMTsaeventcontainer(const FMTsaeventcontainer& rhs);
        // DocString: FMTsaeventcontainer::operator=
        /**
        Copy assignment operator
        */
        FMTsaeventcontainer& operator=(const FMTsaeventcontainer& rhs);
        // DocString: FMTsaeventcontainer::operator==
        /**
        Comparison operator equal to
        */
        bool operator==(const FMTsaeventcontainer& rhs) const;
         // DocString: FMTsaeventcontainer::swap
        /**
        Exchange contents of events with the content of rhs.events
        */
        void swap(FMTsaeventcontainer& rhs);
        // DocString: FMTsaeventcontainer::begin
        /**
        Return a const iterator to the first FMTsaevent in events.
        */
        const_iterator begin() const{return events.cbegin();}
        // DocString: FMTsaeventcontainer::end
        /**
        Return a const iterator to the past-the-end element in events.
        */
		const_iterator end() const{return events.cend();}
        // DocString: FMTsaeventcontainer::empty
        /**
        Test whether the container is empty.
        */
		bool empty() const {return events.empty();}
		// DocString: FMTsaeventcontainer::size
        /**
        Return container size.
        */
        size_t size() const {return events.size();}
        // DocString: FMTsaeventcontainer::find
        /**
        RGet const iterator to the element.
        */
        const_iterator find(const FMTsaevent& event) const {return events.find(event);}
        // DocString: FMTsaeventcontainer::insert
        /**
        Insert an event in the container.
        */
        void insert(const FMTsaevent& event){events.insert(event);}
        // DocString: FMTsaeventcontainer::erase(const FMTsaevent&)
        /**
        Erase an event from the container.
        */
        void erase(const FMTsaevent& event);
        // DocString: FMTsaeventcontainer::erase(const const_iterator)
        /**
        Erase an event from the container and return the next iterator.
        */
        const_iterator erase (const const_iterator event);
        // DocString: FMTsaeventcontainer::clear
        /**
        Clear content of the container.
        */
        void clear (){events.clear();}
        // DocString: FMTsaeventcontainer::lasperiod
        /*
        Return the period of the last event in the container
        */
        int lastperiod()const;
        // DocString: FMTsaeventcontainer::firstperiod
        /*
        Return the period of the first event in the container
        */
        int firstperiod()const;
        // DocString: FMTsaeventcontainer::erasecoordinate
        /*
        Erase all coordinate corresponding to coord in events from the period start.
        */
        void erasecoordinate (const FMTcoordinate coord, const int& periodstart);
        // DocString: FMTsaeventcontainer::getbounds(const int&)
        /*
        Return a pair of iterator pointing to the first and the last elements in the period
        */
        std::pair<const_iterator,const_iterator> getbounds(const int& period) const;
        // DocString: FMTsaeventcontainer::addaction()
        /*
        Add an action at coordinate and add to existing event if possible or aggregate events
        if possible.
        */
        void addaction (const FMTcoordinate coord, const int& period,const int& actionid);
        // DocString: FMTsaeventcontainer::getevents(const int&, const int&)
        /*
        Get events at specified period with specified action
        */
        std::vector<const_iterator> getevents(const int& period, const int& action_id) const;
         // DocString: FMTsaeventcontainer::getevents(const int&, const std::vector<int>&)
        /*
        Get events at specified period with multiple actions id
        */
        std::vector<const_iterator> getevents(const int& period, const std::vector<int>& actions_id) const;
        // DocString: FMTsaeventcontainer::getevents(const int&, const FMTcoordinate&)
        /*
        ///Get events at specified period with specified coordinates
        */
        std::vector<const_iterator> getevents(const int& period, const FMTcoordinate& coord) const;
        // DocString: FMTsaeventcontainer::getevents(const int&, const std::set<FMTcoordinate>&)
        /*
        ///Get events at specified period with multiple specified coordinates
        */
        std::vector<const_iterator> getevents(const int& period, const std::set<FMTcoordinate>& territory) const;
        // DocString: FMTsaeventcontainer::minimaldistance
        /**
        This function is used when we want to verify if the event is within a certain distance from
        other events. If it's not, the specified distance distance+1 is return, else the minimal distance
        is return. It's principally use to calculate penalty for simulated annealing models.
        */
        double minimaldistance(const FMTsaevent&,const unsigned int& distancel,const int& period, const std::vector<int>& actionsid) const;

     private:
        ///Set containing the events
        std::set<FMTsaevent> events;
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
#endif // FMTSAEVENTCONTAINER_H
