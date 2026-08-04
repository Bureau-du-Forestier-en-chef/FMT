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
@brief Container of FMTEvent handling event comparison and distance calculation efficiently, allowing the user to get events by period, action id and coordinates.
@details One container can hold all the events of a model, sorted by period, centroid, action id and finally size; coordinates can be added and removed by period and action.
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
        @brief Default constructor for FMTEventContainer.
        */
        FMTEventContainer()=default;
        // DocString: ~FMTEventContainer()
        /**
        @brief Default destructor for FMTEventContainer.
        */
        ~FMTEventContainer()=default;
        // DocString: FMTEventContainer(const FMTEventContainer&)
        /**
        @brief Copy constructor for FMTEventContainer.
        @param[in] rhs the FMTEventContainer to copy.
        */
        FMTEventContainer(const FMTEventContainer& rhs)=default;
        // DocString: FMTEventContainer::operator=
        /**
        @brief Copy assignment operator for FMTEventContainer.
        @param[in] rhs the FMTEventContainer to copy.
        @return a reference to this FMTEventContainer.
        */
        FMTEventContainer& operator=(const FMTEventContainer& rhs)=default;
        // DocString: FMTEventContainer::operator==
        /**
        @brief Comparison operator for FMTEventContainer.
        @param[in] rhs the FMTEventContainer to compare to.
        @return true if both containers are equal else false.
        */
        bool operator==(const FMTEventContainer& rhs) const;
         // DocString: FMTEventContainer::swap
         /**
         @brief Swap the events of this container with another one.
         @param[in,out] rhs the FMTEventContainer to swap with.
         */
        void swap(FMTEventContainer& rhs);
        // DocString: FMTEventContainer::begin
        /**
        @brief Return a const iterator to the first event.
        @return a const iterator to the first event.
        */
        const_iterator begin() const;
        // DocString: FMTEventContainer::end
        /**
        @brief Return a const iterator past the last event.
        @return a const iterator past the last event.
        */
        const_iterator end() const;
        // DocString: FMTEventContainer::empty
        /**
        @brief Test whether the container is empty.
        @return true if the container is empty else false.
        */
        bool empty() const;
		// DocString: FMTEventContainer::size
		/**
		@brief Return the number of events in the container.
		@return the container size.
		*/
        size_t size() const;
        // DocString: FMTEventContainer::find
        /**
        @brief Return a const iterator to the first event found with a coordinate at a period.
        @param[in] period the period.
        @param[in] coord the coordinate.
        @return a const iterator to the event.
        */
		const_iterator find(const int& period, const FMTCoordinate& coord) const;
		// DocString: FMTEventContainer::find
		/**
		@brief Return a const iterator to an event.
		@param[in] event the event.
		@return a const iterator to the event.
		*/
        const_iterator find(const FMTEvent& event) const;
        // DocString: FMTEventContainer::insert
        /**
        @brief Insert an event in the container.
        @param[in] event the event to insert.
        */
        void insert(const FMTEvent& event);
		// DocString: FMTEventContainer::merge
		/**
		@brief Merge another FMTEventContainer into this one.
		@param[in] levents the container to merge.
		*/
		void merge(const FMTEventContainer& levents);
        // DocString: FMTEventContainer::erase(const FMTEvent&)
        /**
        @brief Erase an event from the container.
        @param[in] event the event to erase.
        */
        void erase(const FMTEvent& event);
        // DocString: FMTEventContainer::erase(const const_iterator)
        /**
        @brief Erase an event from the container and return the next iterator.
        @param[in] event the iterator to the event to erase.
        @return the next iterator.
        */
        const_iterator erase (const const_iterator event);
        // DocString: FMTEventContainer::clear
        /**
        @brief Clear the content of the container.
        */
        void clear();
        // DocString: FMTEventContainer::lasperiod
        /**
        @brief Return the period of the last event in the container.
        @return the last period.
        */
        int lastPeriod()const;
        // DocString: FMTEventContainer::firstPeriod
        /**
        @brief Return the period of the first event in the container.
        @return the first period.
        */
        int firstPeriod()const;
		// DocString: FMTEventContainer::eraseCoordinate
		/**
		@brief Erase all the coordinates corresponding to a coordinate in the events from a period.
		@param[in] p_coord the coordinate.
		@param[in] p_periodStart the first period.
		@param[in] p_actionstarget the action targets.
		*/
		void eraseCoordinate(const FMTCoordinate& p_coord, 
            const int& p_periodStart,const std::vector<std::vector<bool>>& p_actionstarget);
        // DocString: FMTEventContainer::getBounds(const int&)
        /**
        @brief Return a pair of iterators pointing to the first and the last elements of a period.
        @param[in] period the period.
        @return the pair of iterators.
        */
        std::pair<const_iterator,const_iterator> getBounds(const int& period) const;
        // DocString: FMTEventContainer::addAction(const FMTCoordinate&, const int&, const int&,const FMTBindingSpatialAction&)
        /**
        @brief Add an action at a coordinate, adding to an existing event if possible or aggregating events based on the binding.
        @param[in] coord the coordinate.
        @param[in] period the period.
        @param[in] actionid the action id.
        @param[in] binding the binding spatial action.
        @param[in] p_GraphFamily the graph family.
        */
        void addAction (const FMTCoordinate& coord, const int& period,
            const int& actionid,const FMTBindingSpatialAction& binding, size_t p_GraphFamily);
		// DocString: FMTEventContainer::addActions()
		/**
		@brief Add actions at a coordinate, adding to an existing event if possible or aggregating events based on the bindings.
		@details The bindings must be the size of the vector of actions in the model because the action ids are used to find the right ones.
		@param[in] coord the coordinate.
		@param[in] period the period.
		@param[in] actionids the action ids.
		@param[in] bindings the binding spatial actions.
		@param[in] p_GraphFamily the graph family.
		*/
		void addActions(const FMTCoordinate& coord, const int& period, 
            const std::vector<int>& actionids, const std::vector<FMTBindingSpatialAction>& bindings, size_t p_GraphFamily);
        // DocString: FMTEventContainer::getEvents(const int&, const int&)
        /**
        @brief Return the events at a period for an action.
        @param[in] period the period.
        @param[in] action_id the action id.
        @return the events.
        */
        std::vector<const_iterator> getEvents(const int& period, const int& action_id) const;
		// DocString: FMTEventContainer::getEvents(const int&, const int&,const FMTCoordinate&,const FMTCoordinate&)
		/**
		@brief Return the events at a period for an action between a minimal and a maximal coordinate.
		@param[in] period the period.
		@param[in] action_id the action id.
		@param[in] minimalcoordinate the minimal coordinate.
		@param[in] maximalcoordinate the maximal coordinate.
		@return the events.
		*/
		std::vector<const_iterator> getEvents(const int& period, const int& action_id,
			const FMTCoordinate& minimalcoordinate, const FMTCoordinate& maximalcoordinate) const;
		// DocString: FMTEventContainer::getMultipleEvents()
		/**
		@brief Return the events at a period for multiple actions between a minimal and a maximal coordinate.
		@param[in] period the period.
		@param[in] action_ids the action ids.
		@param[in] minimalcoordinate the minimal coordinate.
		@param[in] maximalcoordinate the maximal coordinate.
		@return the events per action.
		*/
		std::vector<std::vector<const_iterator>> getMultipleEvents(const int& period, const std::vector<int>& action_ids,
						const FMTCoordinate& minimalcoordinate, const FMTCoordinate& maximalcoordinate) const;
        // DocString: FMTEventContainer::getEvents(const int&, const std::vector<int>&)
        /**
        @brief Return the events at a period for multiple action ids.
        @param[in] period the period.
        @param[in] actions_id the action ids.
        @return the events.
        */
        std::vector<const_iterator> getEvents(const int& period, const std::vector<int>& actions_id) const;
		// DocString: FMTEventContainer::getEvents(const int&, const std::vector<bool>&)
		/**
		@brief Return the events at a period for the used actions.
		@param[in] period the period.
		@param[in] actions_used the used actions.
		@return the events.
		*/
		std::vector<const_iterator> getEvents(const int& period, const std::vector<bool>& actions_used) const;
		// DocString: FMTEventContainer::getEvents(const int&, const std::vector<bool>&,const FMTCoordinate&, const FMTCoordinate&)
		/**
		@brief Return the events at a period for the used actions between a minimal and a maximal coordinate.
		@param[in] period the period.
		@param[in] actions_used the used actions.
		@param[in] minimalcoordinate the minimal coordinate.
		@param[in] maximalcoordinate the maximal coordinate.
		@return the events.
		*/
		std::vector<const_iterator> getEvents(const int& period, const std::vector<bool>& actions_used,
			const FMTCoordinate& minimalcoordinate, const FMTCoordinate& maximalcoordinate) const;
        // DocString: FMTEventContainer::getEvents(const int&, const FMTCoordinate&)
        /**
        @brief Return the events at a period for a coordinate.
        @param[in] period the period.
        @param[in] coord the coordinate.
        @return the events.
        */
        std::vector<const_iterator> getEvents(const int& period, const FMTCoordinate& coord) const;
		// DocString: FMTEventContainer::getEventsToErase
		/**
		@brief Return the events to erase when removing a coordinate, starting from a period based on a buffer around the events, and fill the modified events.
		@param[in] fromperiod the first period.
		@param[in] actionstarget the action targets.
		@param[in] coord the coordinate.
		@param[in] buffer the buffer.
		@param[in,out] newevents the modified events resulting from erasing the coordinate.
		@return the events to erase.
		*/
		FMTEventContainer getEventsToErase(const int& fromperiod, const std::vector<std::vector<bool>>& actionstarget,
			const FMTCoordinate& coord,const size_t& buffer,FMTEventContainer& newevents) const;
		// DocString: FMTEventContainer::getEventsToAdd
		/**
		@brief Add an action to a coordinate and return the modified events whose infeasibility needs to be recalculated, filling the new events.
		@param[in] coord the coordinate.
		@param[in] period the period.
		@param[in] actionid the action id.
		@param[in] binding the binding spatial action.
		@param[in,out] newevents the new events.
		@param[in] p_GraphFamily the graph family.
		@return the events to add.
		*/
		FMTEventContainer getEventsToAdd(const FMTCoordinate& coord, const int& period, const int& actionid,
									const FMTBindingSpatialAction& binding, FMTEventContainer& newevents,
                                    size_t p_GraphFamily) const;
		// DocString: FMTEventContainer::getContainer
		/**
		@brief Return a subset of the container on the coordinates of interest.
		@param[in] coordinates the coordinates.
		@param[in] minperiod the minimal period.
		@param[in] maxperiod the maximal period.
		@param[in] buffer the buffer.
		@return the container subset.
		*/
		FMTEventContainer getContainer(std::vector<FMTCoordinate> coordinates, const int& minperiod, const int& maxperiod, const size_t& buffer) const;

		// DocString: FMTEventContainer::addUpdate
		/**
		@brief Update the container by adding the new events and removing the events to remove, used to compute the infeasibility.
		@param[in] newevents the new events.
		@param[in] eventstoremove the events to remove.
		@return the updated container.
		*/
		FMTEventContainer addUpdate(const FMTEventContainer& newevents,const FMTEventContainer& eventstoremove);

        // DocString: FMTEventContainer::getEvents(const int&, const std::set<FMTCoordinate>&)
        /**
        @brief Return the events at a period for multiple coordinates.
        @param[in] period the period.
        @param[in] territory the coordinates.
        @return the events.
        */
        std::vector<const_iterator> getEvents(const int& period, const std::set<FMTCoordinate>& territory) const;
        // DocString: FMTEventContainer::minimalDistance
        /**
        @brief Return the minimal distance between an event and the other events for a period, or the distance plus one if the event is not within the distance.
        @details Mainly used to compute the penalty of simulated annealing models.
        @param[in] p_event the event.
        @param[in] p_distancel the distance.
        @param[in] p_period the period.
        @param[in] p_actionsid the action ids.
        @return the minimal distance.
        */
        double minimalDistance(const FMTEvent& p_event,const unsigned int& p_distancel,
            const int& p_period, const std::vector<int>& p_actionsid) const;
		// DocString: FMTEventContainer::evaluateDistance
		/**
		@brief Add up the distances of the events that are within (or not within) a distance range for a period.
		@param[in] p_event the event.
		@param[in] p_lowerdistancetoevent the lower distance to the event.
		@param[in] p_upperdistancetoevent the upper distance to the event.
		@param[in] p_period the period.
		@param[in] p_actionsused the used actions.
		@param[in,out] p_relations the event relations.
		@return the evaluated distance.
		*/
		double evaluateDistance(const FMTEvent& p_event,
			const double& p_lowerdistancetoevent,
			const double& p_upperdistancetoevent,
			const int& p_period, const std::vector<bool>& p_actionsused,
			boost::unordered_set<FMTEventRelation>& p_relations) const;

        // DocString: FMTEventContainer::evaluateSize
        /**
        @brief Evaluate the size infeasibility of the events for a period against a size range.
        @param[in] p_actions the actions.
        @param[in] p_period the period.
        @param[in] p_lowerBound the lower size bound.
        @param[in] p_upperBound the upper size bound.
        @param[in] p_testLower if true tests the lower bound.
        @return the size infeasibility.
        */
        double evaluateSize(const std::vector<bool>& p_actions,
            int p_period,size_t  p_lowerBound, size_t p_upperBound, bool p_testLower) const;

        // DocString: FMTEventContainer::getDispersion(const std::vector<bool>&,const FMTSpatialGraphs&,int,int,int,int)
        /**
        @brief Evaluate the dispersion infeasibility of the events for a theme and a period range.
        @param[in] p_actions the actions.
        @param[in] p_Graphs the spatial graphs.
        @param[in] p_themeId the theme id.
        @param[in] p_FirstPeriod the first period.
        @param[in] p_LastPeriod the last period.
        @param[in] p_bound the bound.
        @return the dispersion infeasibility.
        */
        double getDispersion(const std::vector<bool>& p_actions,
                            const FMTSpatialGraphs& p_Graphs,
                            int p_themeId,int p_FirstPeriod, 
                            int p_LastPeriod, int p_bound) const;

        // DocString: FMTEventContainer::getDispersion(int,const FMTSpatialGraphs&,int,int,int,int)
        /**
        @brief Evaluate the dispersion infeasibility of the events for a rule, a theme and a period range.
        @param[in] p_RuleId the rule id.
        @param[in] p_Graphs the spatial graphs.
        @param[in] p_themeId the theme id.
        @param[in] p_FirstPeriod the first period.
        @param[in] p_LastPeriod the last period.
        @param[in] p_bound the bound.
        @return the dispersion infeasibility.
        */
        double getDispersion(int p_RuleId,
            const FMTSpatialGraphs& p_Graphs,
            int p_themeId, int p_FirstPeriod,
            int p_LastPeriod, int p_bound) const;

        // DocString: FMTEventContainer::getDispersionConflicts(const std::vector<bool>&,const FMTSpatialGraphs&,int,int,int,int)
        /**
        @brief Return the events in dispersion conflict for a theme and a period range.
        @param[in] p_actions the actions.
        @param[in] p_Graphs the spatial graphs.
        @param[in] p_themeId the theme id.
        @param[in] p_FirstPeriod the first period.
        @param[in] p_LastPeriod the last period.
        @param[in] p_bound the bound.
        @return the conflicting events.
        */
        std::vector<FMTEventContainer::const_iterator> getDispersionConflicts(const std::vector<bool>& p_actions,
                                                const FMTSpatialGraphs& p_Graphs,
                                                int p_themeId,int p_FirstPeriod,
                                                int p_LastPeriod, int p_bound) const;
        // DocString: FMTEventContainer::getDispersionConflicts(int,const FMTSpatialGraphs&,int,int,int,int)
        /**
        @brief Return the events in dispersion conflict for a rule, a theme and a period range.
        @param[in] p_RuleId the rule id.
        @param[in] p_Graphs the spatial graphs.
        @param[in] p_themeId the theme id.
        @param[in] p_FirstPeriod the first period.
        @param[in] p_LastPeriod the last period.
        @param[in] p_bound the bound.
        @return the conflicting events.
        */
        std::vector<FMTEventContainer::const_iterator> getDispersionConflicts(int p_RuleId,
                                                            const FMTSpatialGraphs& p_Graphs,
                                                            int p_themeId, int p_FirstPeriod,
                                                            int p_LastPeriod, int p_bound) const;
        // DocString: FMTEventContainer::getEventsOf
        /**
        @brief Return the events of a rule for a period range.
        @param[in] p_RuleId the rule id.
        @param[in] p_MinimalPeriod the minimal period.
        @param[in] p_MaximalPeriod the maximal period.
        @return the events.
        */
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
        // DocString: FMTEventContainer::serialize
        /**
        @brief Serialize the FMTEventContainer for multiprocessing across multiple cpus (pickle in Python).
        @tparam Archive the archive type.
        @param[in,out] ar the archive to serialize to or from.
        @param[in] version the serialization version.
        */
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
        const_iterator _upperBound(const int& period) const;
        ///Returns an iterator pointing to the first element in the container which is considered to go after val.
        const_iterator _upperBound(const int& period,const FMTCoordinate& maxxycoord) const;
        ///Returns an iterator pointing to the first element in the container which is not considered to go before val.
        ///(i.e., either it is equivalent or goes after)
        const_iterator _lowerBound(const int& period) const;
        ///Returns an iterator pointing to the first element in the container which is not considered to go before val.
        ///(i.e., either it is equivalent or goes after)
        const_iterator _lowerBound(const int& period, const FMTCoordinate& minxycoord) const;
		///Update the container by erasing a coordinate from the iterators
		///(i.e., either it is equivalent or goes after)
        ///Now it's splitting automatically if a coord is not within a distance of 1
		void erase(const FMTCoordinate& coord, const std::vector<const_iterator>& iterators);
		// DocString: FMTEventContainer::pushAction
		/**
		@brief Push an action in the container at a coordinate for a period.
		@param[in] iterators the event iterators.
		@param[in] coord the coordinate.
		@param[in] period the period.
		@param[in] actionid the action id.
		@param[in] neighborsize the neighbor size.
		@param[in] p_GraphFamily the graph family.
		*/
		void pushAction(const std::vector<FMTEventContainer::const_iterator>& iterators,
			const FMTCoordinate& coord, const int& period, const int& actionid,
            size_t neighborsize, size_t p_GraphFamily);
        // DocString: FMTEventContainer::getAroundEvents()
        /**
        @brief Return the iterators to the events considered around a coordinate based on a neighbor size.
        @param[in] iterators the event iterators.
        @param[in] coord the coordinate.
        @param[in] neighborsize the neighbor size.
        @return the surrounding events.
        */
		std::vector<FMTEventContainer::const_iterator> getAroundEvents(const std::vector<FMTEventContainer::const_iterator>& iterators,
			const FMTCoordinate& coord, const size_t& neighborsize) const;
		// DocString: FMTEventContainer::fastsort
		/**
		@brief Sort the first elements of the event iterators by proximity to a coordinate.
		@param[in,out] iterators the event iterators.
		@param[in] coord the coordinate.
		@param[in] nelement the number of elements to sort.
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
		public:
			FMTEventIteratorSorter(const FMTCoordinate& coordinate);
			bool operator() (const FMTEventContainer::const_iterator& eventit1, const FMTEventContainer::const_iterator& eventit2) const;
		private:
		FMTCoordinate	m_basecoordinate;
		};


};


}
#endif // FMTeventCONTAINER_H
