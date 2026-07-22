/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTEVENT_Hm_included
#define FMTEVENT_Hm_included

#include "FMTCoordinate.h"
#include <random>
#include <limits>
#include <iterator>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/set.hpp>
#include <array>

namespace Spatial
{
class FMTEventRelation;
// DocString: FMTEvent
/**
@brief Base class for a spatial event (FMTsaevent or FMTsesevent): a set of FMTCoordinate corresponding to the coordinates present in the event.
*/
class FMTEXPORT FMTEvent
    {
    public:
		// DocString: FMTEvent::midPosition()
		/**
		@brief Get the mid point of the elements of the event.
		@return an iterator to the mid coordinate.
		*/
        std::set<FMTCoordinate>::const_iterator midPosition() const;
        // DocString: ~FMTEvent()
        /**
        @brief Default destructor for FMTEvent.
        */
        ~FMTEvent() = default;
        // DocString: FMTEvent(const FMTEvent&)
        /**
        @brief Copy constructor for FMTEvent.
        @param[in] rhs the FMTEvent to copy.
        */
        FMTEvent(const FMTEvent& rhs) = default;
        // DocString: FMTEvent::operator=
        /**
        @brief Copy assignment operator for FMTEvent.
        @param[in] rhs the FMTEvent to copy.
        @return a reference to this FMTEvent.
        */
        FMTEvent& operator=(const FMTEvent& rhs) = default;
        // DocString: FMTEvent()
        /**
        @brief Default constructor for FMTEvent.
        */
        FMTEvent() = default;
		// DocString: FMTsaevent(const FMTCoordinate&,const int&,const int&)
		/**
		@brief Construct a FMTEvent from a location, an action id, a period and a centroid graph family.
		@param[in] p_location the location.
		@param[in] p_actionId the action id.
		@param[in] p_period the period.
		@param[in] p_centroidGraphFamily the centroid graph family.
		*/
        FMTEvent(const FMTCoordinate& p_location,
            int p_actionId,
            int p_period,
            size_t p_centroidGraphFamily);
        // DocString: FMTEvent::empty()
        /**
        @brief Test whether the event is empty.
        @return true if the event is empty else false.
        */
        bool empty() const;
        // DocString: FMTEvent::size()
        /**
        @brief Return the size of the event.
        @return the event size.
        */
        size_t size() const;
        // DocString: FMTEvent::hash()
        /**
        @brief Hash the event.
        @return the hash of the event.
        */
		size_t hash() const;
		// DocString: FMTEvent::getRelation(const FMTEvent&)
		/**
		@brief Return the relation between this event and another one.
		@param[in] rhs the other event.
		@return the event relation.
		*/
		FMTEventRelation getRelation(const FMTEvent& rhs) const;
        // DocString: FMTEvent::operator==(const FMTEvent&)
        /**
        @brief Comparison operator for FMTEvent.
        @param[in] rhs the FMTEvent to compare to.
        @return true if both events are equal else false.
        */
        bool operator==(const FMTEvent& rhs) const;
        // DocString: FMTEvent::operator!=(const FMTEvent&)
        /**
        @brief Comparison operator for FMTEvent.
        @param[in] rhs the FMTEvent to compare to.
        @return true if both events are different else false.
        */
        bool operator!=(const FMTEvent& rhs) const;
        // DocString: FMTEvent::operator<(const FMTEvent&)
        /**
        @brief Less than operator for FMTEvent.
        @param[in] rhs the FMTEvent to compare to.
        @return true if this event is less than the other else false.
        */
        bool operator<(const FMTEvent& rhs) const;
        // DocString: FMTEvent::perimeter()
        /**
        @brief Return the perimeter of the event, including any internal holes.
        @return the perimeter.
        */
        size_t perimeter() const;
        // DocString: FMTEvent::height()
        /**
        @brief Return the height of the event.
        @return the height.
        */
        size_t height() const;
        // DocString: FMTEvent::width()
        /**
        @brief Return the width of the event.
        @return the width.
        */
        size_t width() const;
        // DocString: FMTEvent::averageCentroid()
        /**
        @brief Return the centroid based on the envelope of the event.
        @return the average centroid.
        */
        FMTCoordinate averageCentroid() const;
        // DocString: FMTEvent::getStatsHeader()
        /**
        @brief Return the header of the patch stats (size, perimeter, height and width).
        @return the stats header.
        */
        static std::string getStatsHeader();
        // DocString: FMTEvent::getStats()
        /**
        @brief Return the patch stats (size, perimeter, height and width) as a string.
        @return the stats.
        */
        std::string getStats() const;
        // DocString: FMTEvent::erase(const FMTCoordinate&)
        /**
        @brief Erase a coordinate from the event.
        @param[in] newlocation the coordinate to erase.
        */
        virtual void erase(const FMTCoordinate& newlocation);
        // DocString: FMTEvent::merge(const FMTEvent& event)
        /**
        @brief Merge another event into this one.
        @param[in] event the event to merge.
        */
        virtual void merge(const FMTEvent& event);
        // DocString: FMTEvent::insert(const FMTCoordinate&)
        /**
        @brief Insert a coordinate in the event.
        @param[in] newlocation the coordinate to insert.
        */
        virtual void insert(const FMTCoordinate& newlocation);
        // DocString: FMTEvent::setActionId(const int&)
        /**
        @brief Set the action id of the event.
        @param[in] laction_id the action id.
        */
        void setActionId(const int& laction_id);
        // DocString: FMTEvent::getActionId()
        /**
        @brief Return the action id of the event.
        @return the action id.
        */
        const int& getActionId() const;
        // DocString: FMTEvent::getPeriod()
        /**
        @brief Return the period of the event.
        @return the period.
        */
        const int& getPeriod() const;
        // DocString: FMTEvent::getGraphFamily()
        /**
        @brief Return the graph family of the event.
        @return the graph family.
        */
        const size_t& getGraphFamily() const;
        // DocString: FMTEvent::getElements
        /**
        @brief Return the elements (coordinates) of the event.
        @return the event elements.
        */
        const std::set<FMTCoordinate>& getElements() const;
        // DocString: FMTEvent::ignit(const FMTspatialaction&, const FMTCoordinate&, const int&, const int&)
        /**
        @brief Ignite the event at a coordinate for an action, a period and a graph family, up to a maximal size.
        @param[in] eventmaximalsize the maximal event size.
        @param[in] ignit the ignition coordinate.
        @param[in] p_actionId the action id.
        @param[in] p_period the period.
        @param[in] p_GraphFamily the graph family.
        @return the active coordinates.
        */
        virtual std::vector<std::set<FMTCoordinate>::const_iterator> ignit(const size_t& eventmaximalsize,
            const std::set<FMTCoordinate>::const_iterator& ignit, int p_actionId, int p_period, size_t p_GraphFamily);
        // DocString: FMTEvent::spread(const size_t& eventminimalsize,const size_t& eventmaximalsize,const size_t& neighboringsize,const std::set<FMTCoordinate>& territory, std::vector<std::set<FMTCoordinate>::const_iterator> active)
        /**
        @brief Spread the event within a territory from the active coordinates, respecting the size bounds.
        @param[in] eventminimalsize the minimal event size.
        @param[in] eventmaximalsize the maximal event size.
        @param[in] neighboringsize the neighboring size.
        @param[in] territory the territory to spread in.
        @param[in] active the active coordinates.
        @return true if the event spread within the bounds else false.
        */
        virtual bool spread(const size_t& eventminimalsize,const size_t& eventmaximalsize,
			const size_t& neighboringsize,const std::set<FMTCoordinate>& territory, std::vector<std::set<FMTCoordinate>::const_iterator> active);
        // DocString: FMTEvent::distance(const FMTEvent&)
        /**
        @brief Return the distance between this event and another one.
        @param[in] rhs the other event.
        @return the distance.
        */
        double distance(const FMTEvent& rhs) const;
        // DocString: FMTEvent::within(unsigned int, const FMTEvent&)
        /**
        @brief Return true if the event is within a distance of another event's envelope.
        @tparam T the distance type.
        @param[in] dist the distance.
        @param[in] rhs the other event.
        @return true if the event is within the distance else false.
        */
        template<typename T>
        bool within(const T& dist, const FMTEvent& rhs) const;
        // DocString: FMTEvent::withinc(unsigned int, const FMTCoordinate&)
        /**
        @brief Return true if a coordinate is within a distance of the event's coordinates.
        @tparam T the distance type.
        @param[in] dist the distance.
        @param[in] location the coordinate.
        @return true if the coordinate is within the distance else false.
        */
        template<typename T>
        bool within(const T& dist, const FMTCoordinate& location) const;
        // DocString: FMTEvent::contain(const FMTCoordinate&)
        /**
        @brief Return true if a coordinate is in the elements of the event.
        @param[in] coord the coordinate.
        @return true if the coordinate is in the event else false.
        */
        bool contain(const FMTCoordinate& coord)const;
        // DocString: FMTEvent::willSplitEvent(const FMTCoordinate&)
        /**
        @brief Return true if removing a coordinate will split the event.
        @param[in] p_coordinate the coordinate that will be removed.
        @return true if it will split the event else false.
        */
        bool willSplitEvent(const FMTCoordinate& p_coordinate) const;
        // DocString: FMTEvent::splitEvent(const unsigned int&, std::vector<FMTsaevent>&)
        /**
        @brief Split the event, filling a vector with the split events and removing the elements from the current event.
        @param[in,out] splittedevents the split events.
        @return true if the event was split else false.
        */
        bool splitEvent(std::vector<FMTEvent>& splittedevents) const;
		// DocString: FMTEvent::getTerritory
		/**
		@brief Return the adjacent territory of the event based on a distance.
		@param[in] distance the distance.
		@return the territory coordinates.
		*/
		std::set<FMTCoordinate>getTerritory(const size_t& distance) const;
		// DocString: FMTEvent::getBorders
		/**
		@brief Return the coordinates of the border of the event.
		@return the border coordinates.
		*/
		std::vector<std::set<FMTCoordinate>::const_iterator>getBorders() const;
        // DocString: FMTEvent::getOutsideBorders
        /**
        @brief Return the outside borders of the event as pairs of coordinates.
        @return the outside border pairs.
        */
        std::vector<std::pair<FMTCoordinate,FMTCoordinate>>getOutsideBordersPair() const;
		// DocString: FMTEvent::getenveloppe
		/**
		@brief Return the four coordinates that create the envelope of the event.
		@return the envelope coordinates.
		*/
		std::array<FMTCoordinate,4>getEnveloppe() const;
		// DocString: FMTEvent::getclosescoordinate
		/**
		@brief Return the closest coordinates between this event and another one.
		@param[in] rhs the other event.
		@param[out] thiscoordinate the closest coordinate of this event.
		@param[out] rhscoordinate the closest coordinate of the other event.
		*/
		void getClosesCoordinates(const FMTEvent& rhs,
			std::set<FMTCoordinate>::const_iterator& thiscoordinate,
			std::set<FMTCoordinate>::const_iterator& rhscoordinate) const;
    private:
        friend class boost::serialization::access;
        // DocString: FMTEvent::serialize
        /**
        @brief Serialize the FMTEvent for multiprocessing across multiple cpus (pickle in Python).
        @tparam Archive the archive type.
        @param[in,out] ar the archive to serialize to or from.
        @param[in] version the serialization version.
        */
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar& BOOST_SERIALIZATION_NVP(m_actionId);
            ar& BOOST_SERIALIZATION_NVP(m_period);
            ar& BOOST_SERIALIZATION_NVP(m_elements);
        }
        // DocString: FMTEvent::m_actionId
        /**
        Action id from the FMTModel.
        */
        int m_actionId;
        // DocString: FMTEvent::m_period
        /**
        Period at which the event take place
        */
        int m_period;
        // DocString: FMTEvent::m_centroidGraphFamily
        /**
        The graph family of the centroid
        */
        size_t m_centroidGraphFamily;
        // DocString: FMTEvent::m_elements
        /**
        Coordinates presents in the event
       */
        std::set<FMTCoordinate>m_elements;
    };

}

namespace boost {

    template <>
    struct hash<Spatial::FMTEvent>
    {
    std::size_t operator()(const Spatial::FMTEvent& event) const
		 {
       return (event.hash());
		}
    };

}
#endif // FMTEVENT_Hm_included
