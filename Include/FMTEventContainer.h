/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTeventCONTAINER_H
#define FMTeventCONTAINER_H

#include "FMTEvent.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/set.hpp>
#include <unordered_set>
#include <boost/unordered_set.hpp>
#include "FMTObject.h"


namespace Spatial
{

class FMTBindingSpatialAction;
class FMTSpatialGraphs;

// DocString: FMTEventContainer
/**
This class is a container for FMTEvent. It's build to handle event
comparison or distance calculation in an efficient way by allowing the
user to get events by period, action id and/or coordinates.
One container can hold all events for a model. They are sorted by
period, centroid, action_id and finally by size.
You can  remove FMTCoordinate and add FMTCoordinate by period and action.
*/
class FMTEXPORT FMTEventContainer : public Core::FMTObject
{
    public:
        typedef typename std::set<FMTEvent>::value_type value_type;
        typedef typename std::set<FMTEvent>::iterator iterator;
        typedef typename std::set<FMTEvent>::const_iterator const_iterator;
        typedef typename std::set<FMTEvent>::const_reverse_iterator const_reverse_iterator;
        // DocString: FMTEventContainer()
        /**
        Default constructor
        */
        FMTEventContainer()=default;
        // DocString: ~FMTEventContainer()
        /**
        Destructor
        */
        ~FMTEventContainer()=default;
        // DocString: FMTEventContainer(const FMTEventContainer&)
        /**
        Copy constructor
        */
        FMTEventContainer(const FMTEventContainer& rhs)=default;
        // DocString: FMTEventContainer::operator=
        /**
        Copy assignment operator
        */
        FMTEventContainer& operator=(const FMTEventContainer& rhs)=default;
        // DocString: FMTEventContainer::operator==
        /**
        Comparison operator equal to
        */
        bool operator==(const FMTEventContainer& rhs) const;
         // DocString: FMTEventContainer::swap
        /**
        Exchange contents of events with the content of rhs.events
        */
        void swap(FMTEventContainer& rhs);
        // DocString: FMTEventContainer::begin
        /**
        Return a const iterator to the first FMTEvent in events.
        */
        const_iterator begin() const;
        // DocString: FMTEventContainer::end
        /**
        Return a const iterator to the past-the-end element in events.
        */
        const_iterator end() const;
        // DocString: FMTEventContainer::empty
        /**
        Test whether the container is empty.
        */
        bool empty() const;
		// DocString: FMTEventContainer::size
        /**
        Return container size.
        */
        size_t size() const;
        // DocString: FMTEventContainer::find
        /**
        Get const iterator to the first event found with the coord at the period specified.
        */
		const_iterator find(const int& period, const FMTCoordinate& coord) const;
		// DocString: FMTEventContainer::find
		/**
		Get const iterator to the element.
		*/
        const_iterator find(const FMTEvent& event) const;
        // DocString: FMTEventContainer::insert
        /**
        Insert an event in the container.
        */
        void insert(const FMTEvent& event);
		// DocString: FMTEventContainer::merge
		/**
		Merge two FMTEventContainer
		*/
		void merge(const FMTEventContainer& levents);
        // DocString: FMTEventContainer::erase(const FMTEvent&)
        /**
        Erase an event from the container.
        */
        void erase(const FMTEvent& event);
        // DocString: FMTEventContainer::erase(const const_iterator)
        /**
        Erase an event from the container and return the next iterator.
        */
        const_iterator erase (const const_iterator event);
        // DocString: FMTEventContainer::clear
        /**
        Clear content of the container.
        */
        void clear();
        // DocString: FMTEventContainer::lasperiod
        /*
        Return the period of the last event in the container
        */
        int lastPeriod()const;
        // DocString: FMTEventContainer::firstPeriod
        /*
        Return the period of the first event in the container
        */
        int firstPeriod()const;
        // DocString: FMTEventContainer::eraseCoordinate
        /*
        Erase all coordinate corresponding to coord in events from the period start.
        */
        //void eraseCoordinate(const FMTCoordinate& coord, const int& periodStart);
		// DocString: FMTEventContainer::eraseCoordinate
	   /*
	   Erase all coordinate corresponding to coord in events from the period start.
	   */
		void eraseCoordinate(const FMTCoordinate& coord, const int& periodStart,const std::vector<std::vector<bool>>& actionstarget);
        // DocString: FMTEventContainer::getBounds(const int&)
        /*
        Return a pair of iterator pointing to the first and the last elements in the period
        */
        std::pair<const_iterator,const_iterator> getBounds(const int& period) const;
        // DocString: FMTEventContainer::addAction(const FMTCoordinate&, const int&, const int&,const FMTBindingSpatialAction&)
        /*
        Add an action at coordinate and add to existing event if possible or aggregate events based on binding
        */
        void addAction (const FMTCoordinate& coord, const int& period,
            const int& actionid,const FMTBindingSpatialAction& binding, size_t p_GraphFamily);
		// DocString: FMTEventContainer::addActions()
		/*
		Add actions at coordinate and add to existing event if possible or aggregate events based on (bindings). 
        (bindings) must be the size of the vector of action in model because we use (actionids) to find the right ones.  
		*/
		void addActions(const FMTCoordinate& coord, const int& period, 
            const std::vector<int>& actionids, const std::vector<FMTBindingSpatialAction>& bindings, size_t p_GraphFamily);
        // DocString: FMTEventContainer::getEvents(const int&, const int&)
        /*
        Get events at specified period with specified action
        */
        std::vector<const_iterator> getEvents(const int& period, const int& action_id) const;
		// DocString: FMTEventContainer::getEvents(const int&, const int&,const FMTCoordinate&,const FMTCoordinate&)
		/*
		Get events at specified period with specified action for a minimal and maximalcoordinate.
		*/
		std::vector<const_iterator> getEvents(const int& period, const int& action_id,
			const FMTCoordinate& minimalcoordinate, const FMTCoordinate& maximalcoordinate) const;
		// DocString: FMTEventContainer::getMultipleEvents()
		/*
		Get events at specified period with specified action for a minimal and maximalcoordinate.
		*/
		std::vector<std::vector<const_iterator>> getMultipleEvents(const int& period, const std::vector<int>& action_ids,
						const FMTCoordinate& minimalcoordinate, const FMTCoordinate& maximalcoordinate) const;
        // DocString: FMTEventContainer::getEvents(const int&, const std::vector<int>&)
        /*
        Get events at specified period with multiple actions id
        */
        std::vector<const_iterator> getEvents(const int& period, const std::vector<int>& actions_id) const;
		// DocString: FMTEventContainer::getEvents(const int&, const std::vector<bool>&)
		/*
		 Get events at specified period is action is used
		 */
		std::vector<const_iterator> getEvents(const int& period, const std::vector<bool>& actions_used) const;
		// DocString: FMTEventContainer::getEvents(const int&, const std::vector<bool>&,const FMTCoordinate&, const FMTCoordinate&)
		/*
		Get events at specified period for multiple (actions_used), between a minimal coordinate and a maximal coordinate.
		*/
		std::vector<const_iterator> getEvents(const int& period, const std::vector<bool>& actions_used,
			const FMTCoordinate& minimalcoordinate, const FMTCoordinate& maximalcoordinate) const;
        // DocString: FMTEventContainer::getEvents(const int&, const FMTCoordinate&)
        /*
        ///Get events at specified period with specified coordinates
        */
        std::vector<const_iterator> getEvents(const int& period, const FMTCoordinate& coord) const;
		// DocString: FMTEventContainer::getEvents(const int&, const FMTCoordinate&)
		/*
		Get events starting from a period based on a buffer around events returns the events to delete if you remove coord
		The newevents container is the container containing modified events resulting from erasing the coord. Following a move,
		get the infeasibility of the erased events, add the infeasibility of the events in the newevents added the this container with erased events.
		*/
		FMTEventContainer getEventsToErase(const int& fromperiod, const std::vector<std::vector<bool>>& actionstarget,
			const FMTCoordinate& coord,const size_t& buffer,FMTEventContainer& newevents) const;
		// DocString: FMTEventContainer::getEventsToAdd
		/*
		Add the action to the coordinate and return the modified events that infeasibility need to be calculated (removed). The newevents
		contains the new events to calculated infeasibility you need to delete the vector returned and add the newevents to the container and
		calculate the infeasibility of those events.
		*/
		FMTEventContainer getEventsToAdd(const FMTCoordinate& coord, const int& period, const int& actionid,
									const FMTBindingSpatialAction& binding, FMTEventContainer& newevents,
                                    size_t p_GraphFamily) const;
		// DocString: FMTEventContainer::getContainer
		/*
		Get container subset on the interesting coordinates.
		*/
		FMTEventContainer getContainer(std::vector<FMTCoordinate> coordinates, const int& minperiod, const int& maxperiod, const size_t& buffer) const;

		// DocString: FMTEventContainer::addUpdate
		/*
		Update the events container.
		Used to calculate infeasibility.
		*/
		FMTEventContainer addUpdate(const FMTEventContainer& newevents,const FMTEventContainer& eventstoremove);

        // DocString: FMTEventContainer::getEvents(const int&, const std::set<FMTCoordinate>&)
        /*
        ///Get events at specified period with multiple specified coordinates
        */
        std::vector<const_iterator> getEvents(const int& period, const std::set<FMTCoordinate>& territory) const;
        // DocString: FMTEventContainer::minimalDistance
        /**
        This function is used when we want to verify if the event is within a certain distance from
        other events. If it's not, the specified distance distance+1 is return, else the minimal distance
        is return. It's principally use to calculate penalty for simulated annealing models.
        */
        double minimalDistance(const FMTEvent&,const unsigned int& distancel,const int& period, const std::vector<int>& actionsid) const;
		// DocString: FMTEventContainer::minimalDistance
		/**
		The function add each distance from the event if the even is within the ditancetoevent for a given period if within = true. If within = false
		the if the event is not within the distance it will add up the distances.
		*/
		double evaluateDistance(const FMTEvent&,
			const double& lowerdistancetoevent,
			const double& upperdistancetoevent,
			const int& period, const std::vector<bool>& actionsused,
			boost::unordered_set<FMTEventRelation>& relations) const;

        double evaluateSize(const std::vector<bool>& p_actions,
            int p_period,size_t  p_lowerBound, size_t p_upperBound, bool p_testLower) const;

        double getDispersion(const std::vector<bool>& p_actions,
                            const FMTSpatialGraphs& p_Graphs,
                            int p_themeId,int p_FirstPeriod, 
                            int p_LastPeriod, int p_bound) const;

        double getDispersion(int p_RuleId,
            const FMTSpatialGraphs& p_Graphs,
            int p_themeId, int p_FirstPeriod,
            int p_LastPeriod, int p_bound) const;

        std::vector<FMTEventContainer::const_iterator> getDispersionConflicts(const std::vector<bool>& p_actions,
                                                const FMTSpatialGraphs& p_Graphs,
                                                int p_themeId,int p_FirstPeriod,
                                                int p_LastPeriod, int p_bound) const;
        std::vector<FMTEventContainer::const_iterator> getDispersionConflicts(int p_RuleId,
                                                            const FMTSpatialGraphs& p_Graphs,
                                                            int p_themeId, int p_FirstPeriod,
                                                            int p_LastPeriod, int p_bound) const;
        std::vector<FMTEventContainer::const_iterator>getEventsOf(int p_RuleId, 
                                                        int p_MinimalPeriod, int p_MaximalPeriod) const;
       
     private:
         class BoundingBox
            {
            public:
                BoundingBox();
                void add(FMTEventContainer::const_iterator p_event);
                double evaluateUpperBound(int p_UpperBound) const;
                std::vector<FMTEventContainer::const_iterator> getEvents() const;
            private:
                FMTCoordinate m_bottomLeft;
                uint16_t m_Width;
                uint16_t m_Height;
                FMTEventContainer::const_iterator m_Top;
                FMTEventContainer::const_iterator m_Bottom;
                FMTEventContainer::const_iterator m_Left;
                FMTEventContainer::const_iterator m_Right;
                bool _isNull() const;
                double _getSize() const;
            };
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
            {
             ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
             ar& BOOST_SERIALIZATION_NVP(m_events);
            }
        ///Set containing the events
        std::set<FMTEvent> m_events;
        //Actually not needed
        ///Returns an iterator pointing to the first element in the container which is considered to go after val.
        const_iterator upperBound(const int& period) const;
        ///Returns an iterator pointing to the first element in the container which is considered to go after val.
        const_iterator upperBound(const int& period,const FMTCoordinate& maxxycoord) const;
        ///Returns an iterator pointing to the first element in the container which is not considered to go before val.
        ///(i.e., either it is equivalent or goes after)
        const_iterator lowerBound(const int& period) const;
        ///Returns an iterator pointing to the first element in the container which is not considered to go before val.
        ///(i.e., either it is equivalent or goes after)
        const_iterator lowerBound(const int& period, const FMTCoordinate& minxycoord) const;
		///Update the container by erasing a coordinate from the iterators
		///(i.e., either it is equivalent or goes after)
        ///Now it's splitting automatically if a coord is not within a distance of 1
		void erase(const FMTCoordinate& coord, const std::vector<const_iterator>& iterators);
		// DocString: FMTEventContainer::pushAction
		/*
		Push an action in container
		*/
		void pushAction(const std::vector<FMTEventContainer::const_iterator>& iterators,
			const FMTCoordinate& coord, const int& period, const int& actionid,
            size_t neighborsize, size_t p_GraphFamily);
        // DocString: FMTEventContainer::getAroundEvents()
		/*
		Return iterators to events considerate around based on neighborsize
		*/
		std::vector<FMTEventContainer::const_iterator> getAroundEvents(const std::vector<FMTEventContainer::const_iterator>& iterators,
			const FMTCoordinate& coord, const size_t& neighborsize) const;
		// DocString: FMTEventContainer::fastsort
		/*
		Sort events by proximity to a coordinate
		*/
		void nthElements(std::vector<FMTEventContainer::const_iterator>& iterators,const FMTCoordinate& coord,const size_t& nelement) const;


        std::vector<BoundingBox> _getBoundingBoxes(const std::vector<bool>& p_actions, 
                           const FMTSpatialGraphs& p_Graphs, int p_themeId,
                           int p_FirstPeriod, int p_LastPeriod) const;

        std::vector<BoundingBox> _getBoundingBoxes(int p_RuleId,
                        const FMTSpatialGraphs& p_Graphs, int p_themeId,
                        int p_FirstPeriod, int p_LastPeriod) const;

		class FMTEventIteratorSorter
		{
		FMTCoordinate	basecoordinate;
		public:
			FMTEventIteratorSorter(const FMTCoordinate& coordinate);
			bool operator() (const FMTEventContainer::const_iterator& eventit1, const FMTEventContainer::const_iterator& eventit2) const;
		};


};


}
#endif // FMTeventCONTAINER_H
