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
#include <unordered_set>
#include <boost/unordered_set.hpp>
#include "FMTobject.h"


namespace Spatial
{

class FMTbindingspatialaction;
class  FMTSpatialGraphs;

// DocString: FMTeventcontainer
/**
This class is a container for FMTevent. It's build to handle event
comparison or distance calculation in an efficient way by allowing the
user to get events by period, action id and/or coordinates.
One container can hold all events for a model. They are sorted by
period, centroid, action_id and finally by size.
You can  remove FMTcoordinate and add FMTcoordinate by period and action.
*/
class FMTEXPORT FMTeventcontainer : public Core::FMTobject
{
    public:
        typedef typename std::set<FMTevent>::value_type value_type;
        typedef typename std::set<FMTevent>::iterator iterator;
        typedef typename std::set<FMTevent>::const_iterator const_iterator;
        typedef typename std::set<FMTevent>::const_reverse_iterator const_reverse_iterator;
        // DocString: FMTeventcontainer()
        /**
        Default constructor
        */
        FMTeventcontainer()=default;
        // DocString: ~FMTeventcontainer()
        /**
        Destructor
        */
        ~FMTeventcontainer()=default;
        // DocString: FMTeventcontainer(const FMTeventcontainer&)
        /**
        Copy constructor
        */
        FMTeventcontainer(const FMTeventcontainer& rhs)=default;
        // DocString: FMTeventcontainer::operator=
        /**
        Copy assignment operator
        */
        FMTeventcontainer& operator=(const FMTeventcontainer& rhs)=default;
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
        const_iterator begin() const;
        // DocString: FMTeventcontainer::end
        /**
        Return a const iterator to the past-the-end element in events.
        */
        const_iterator end() const;
        // DocString: FMTeventcontainer::empty
        /**
        Test whether the container is empty.
        */
        bool empty() const;
		// DocString: FMTeventcontainer::size
        /**
        Return container size.
        */
        size_t size() const;
        // DocString: FMTeventcontainer::find
        /**
        Get const iterator to the first event found with the coord at the period specified.
        */
		const_iterator find(const int& period, const FMTcoordinate& coord) const;
		// DocString: FMTeventcontainer::find
		/**
		Get const iterator to the element.
		*/
        const_iterator find(const FMTevent& event) const;
        // DocString: FMTeventcontainer::insert
        /**
        Insert an event in the container.
        */
        void insert(const FMTevent& event);
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
        void clear();
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
        //void erasecoordinate(const FMTcoordinate& coord, const int& periodstart);
		// DocString: FMTeventcontainer::erasecoordinate
	   /*
	   Erase all coordinate corresponding to coord in events from the period start.
	   */
		void erasecoordinate(const FMTcoordinate& coord, const int& periodstart,const std::vector<std::vector<bool>>& actionstarget);
        // DocString: FMTeventcontainer::getbounds(const int&)
        /*
        Return a pair of iterator pointing to the first and the last elements in the period
        */
        std::pair<const_iterator,const_iterator> getbounds(const int& period) const;
        // DocString: FMTeventcontainer::addaction(const FMTcoordinate&, const int&, const int&,const FMTbindingspatialaction&)
        /*
        Add an action at coordinate and add to existing event if possible or aggregate events based on binding
        */
        void addaction (const FMTcoordinate& coord, const int& period,
            const int& actionid,const FMTbindingspatialaction& binding, size_t p_GraphFamily);
		// DocString: FMTeventcontainer::addactions()
		/*
		Add actions at coordinate and add to existing event if possible or aggregate events based on (bindings). 
        (bindings) must be the size of the vector of action in model because we use (actionids) to find the right ones.  
		*/
		void addactions(const FMTcoordinate& coord, const int& period, 
            const std::vector<int>& actionids, const std::vector<FMTbindingspatialaction>& bindings, size_t p_GraphFamily);
        // DocString: FMTeventcontainer::getevents(const int&, const int&)
        /*
        Get events at specified period with specified action
        */
        std::vector<const_iterator> getevents(const int& period, const int& action_id) const;
		// DocString: FMTeventcontainer::getevents(const int&, const int&,const FMTcoordinate&,const FMTcoordinate&)
		/*
		Get events at specified period with specified action for a minimal and maximalcoordinate.
		*/
		std::vector<const_iterator> getevents(const int& period, const int& action_id,
			const FMTcoordinate& minimalcoordinate, const FMTcoordinate& maximalcoordinate) const;
		// DocString: FMTeventcontainer::getmultipleevents()
		/*
		Get events at specified period with specified action for a minimal and maximalcoordinate.
		*/
		std::vector<std::vector<const_iterator>> getmultipleevents(const int& period, const std::vector<int>& action_ids,
						const FMTcoordinate& minimalcoordinate, const FMTcoordinate& maximalcoordinate) const;
        // DocString: FMTeventcontainer::getevents(const int&, const std::vector<int>&)
        /*
        Get events at specified period with multiple actions id
        */
        std::vector<const_iterator> getevents(const int& period, const std::vector<int>& actions_id) const;
		// DocString: FMTeventcontainer::getevents(const int&, const std::vector<bool>&)
		/*
		 Get events at specified period is action is used
		 */
		std::vector<const_iterator> getevents(const int& period, const std::vector<bool>& actions_used) const;
		// DocString: FMTeventcontainer::getevents(const int&, const std::vector<bool>&,const FMTcoordinate&, const FMTcoordinate&)
		/*
		Get events at specified period for multiple (actions_used), between a minimal coordinate and a maximal coordinate.
		*/
		std::vector<const_iterator> getevents(const int& period, const std::vector<bool>& actions_used,
			const FMTcoordinate& minimalcoordinate, const FMTcoordinate& maximalcoordinate) const;
        // DocString: FMTeventcontainer::getevents(const int&, const FMTcoordinate&)
        /*
        ///Get events at specified period with specified coordinates
        */
        std::vector<const_iterator> getevents(const int& period, const FMTcoordinate& coord) const;
		// DocString: FMTeventcontainer::getevents(const int&, const FMTcoordinate&)
		/*
		Get events starting from a period based on a buffer around events returns the events to delete if you remove coord
		The newevents container is the container containing modified events resulting from erasing the coord. Following a move,
		get the infeasibility of the erased events, add the infeasibility of the events in the newevents added the this container with erased events.
		*/
		FMTeventcontainer geteventstoerase(const int& fromperiod, const std::vector<std::vector<bool>>& actionstarget,
			const FMTcoordinate& coord,const size_t& buffer,FMTeventcontainer& newevents) const;
		// DocString: FMTeventcontainer::geteventstoadd
		/*
		Add the action to the coordinate and return the modified events that infeasibility need to be calculated (removed). The newevents
		contains the new events to calculated infeasibility you need to delete the vector returned and add the newevents to the container and
		calculate the infeasibility of those events.
		*/
		FMTeventcontainer geteventstoadd(const FMTcoordinate& coord, const int& period, const int& actionid,
									const FMTbindingspatialaction& binding, FMTeventcontainer& newevents,
                                    size_t p_GraphFamily) const;
		// DocString: FMTeventcontainer::getcontainer
		/*
		Get container subset on the interesting coordinates.
		*/
		FMTeventcontainer getcontainer(std::vector<FMTcoordinate> coordinates, const int& minperiod, const int& maxperiod, const size_t& buffer) const;

		// DocString: FMTeventcontainer::addupdate
		/*
		Update the events container.
		Used to calculate infeasibility.
		*/
		FMTeventcontainer addupdate(const FMTeventcontainer& newevents,const FMTeventcontainer& eventstoremove);

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
		// DocString: FMTeventcontainer::minimaldistance
		/**
		The function add each distance from the event if the even is within the ditancetoevent for a given period if within = true. If within = false
		the if the event is not within the distance it will add up the distances.
		*/
		double evaluatedistance(const FMTevent&,
			const double& lowerdistancetoevent,
			const double& upperdistancetoevent,
			const int& period, const std::vector<bool>& actionsused,
			boost::unordered_set<FMTeventrelation>& relations) const;

        double EvaluateSize(const std::vector<bool>& p_actions,
            int p_period,size_t  p_lowerBound, size_t p_upperBound) const;

        double GetDispertion(const std::vector<bool>& p_actions,
                            const FMTSpatialGraphs& p_Graphs,
                            int p_themeId,int p_FirstPeriod, 
                            int p_LastPeriod, int p_bound) const;

        std::vector<FMTeventcontainer::const_iterator> GetDispertionConflicts(const std::vector<bool>& p_actions,
                                                const FMTSpatialGraphs& p_Graphs,
                                                int p_themeId,int p_FirstPeriod,
                                                int p_LastPeriod, int p_bound) const;
     private:
         class BoundingBox
            {
            public:
                BoundingBox();
                void add(FMTeventcontainer::const_iterator p_event);
                size_t GetSize() const;
                std::vector<FMTeventcontainer::const_iterator> GetEvents() const;
            private:
                FMTcoordinate m_bottomLeft;
                uint16_t m_Width;
                uint16_t m_Height;
                FMTeventcontainer::const_iterator m_Top;
                FMTeventcontainer::const_iterator m_Bottom;
                FMTeventcontainer::const_iterator m_Left;
                FMTeventcontainer::const_iterator m_Right;
                bool _IsNull() const;
            };
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
            {
             ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
             ar& BOOST_SERIALIZATION_NVP(m_events);
            }
        ///Set containing the events
        std::set<FMTevent> m_events;
        //Actually not needed
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
		///Update the container by erasing a coordinate from the iterators
		///(i.e., either it is equivalent or goes after)
        ///Now it's splitting automatically if a coord is not within a distance of 1
		void erase(const FMTcoordinate& coord, const std::vector<const_iterator>& iterators);
		// DocString: FMTeventcontainer::pushaction
		/*
		Push an action in container
		*/
		void pushaction(const std::vector<FMTeventcontainer::const_iterator>& iterators,
			const FMTcoordinate& coord, const int& period, const int& actionid,
            size_t neighborsize, size_t p_GraphFamily);
        // DocString: FMTeventcontainer::getaroundevents()
		/*
		Return iterators to events considerate around based on neighborsize
		*/
		std::vector<FMTeventcontainer::const_iterator> getaroundevents(const std::vector<FMTeventcontainer::const_iterator>& iterators,
			const FMTcoordinate& coord, const size_t& neighborsize) const;
		// DocString: FMTeventcontainer::fastsort
		/*
		Sort events by proximity to a coordinate
		*/
		void nthelements(std::vector<FMTeventcontainer::const_iterator>& iterators,const FMTcoordinate& coord,const size_t& nelement) const;


        std::vector<BoundingBox> _GetBoundingBoxes(const std::vector<bool>& p_actions, 
                           const FMTSpatialGraphs& p_Graphs, int p_themeId,
                           int p_FirstPeriod, int p_LastPeriod) const;

		class FMTeventiteratorsorter
		{
		FMTcoordinate	basecoordinate;
		public:
			FMTeventiteratorsorter(const FMTcoordinate& coordinate);
			bool operator() (const FMTeventcontainer::const_iterator& eventit1, const FMTeventcontainer::const_iterator& eventit2) const;
		};


};


}
#endif // FMTeventCONTAINER_H
