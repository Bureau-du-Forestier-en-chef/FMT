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
This is the base class use to create FMTsaevent or FMTsesevent. It is a set of FMTCoordinate (elements)
corresponding to the FMTcoordinates presents in the event.
*/
class FMTEXPORT FMTEvent
    {
    public:
		// DocString: FMTEvent::midPosition()
		/**
		Get mid point of elements.
		*/
        std::set<FMTCoordinate>::const_iterator midPosition() const;
        // DocString: ~FMTEvent()
        /**
        Destructor
        */
        ~FMTEvent() = default;
        // DocString: FMTEvent(const FMTEvent&)
        /**
        Copy constructor
        */
        FMTEvent(const FMTEvent& rhs) = default;
        // DocString: FMTEvent::operator=
        /**
        Copy assignment operator
        */
        FMTEvent& operator=(const FMTEvent& rhs) = default;
        // DocString: FMTEvent()
        /**
        Constructor
        */
        FMTEvent() = default;
		// DocString: FMTsaevent(const FMTCoordinate&,const int&,const int&)
        /**
        Constructor with coordinate, action_id and period
        */
        FMTEvent(const FMTCoordinate& p_location,
            int p_actionId,
            int p_period,
            size_t p_centroidGraphFamily);
        // DocString: FMTEvent::empty()
        /**
        Test whether event is empty
        */
        bool empty() const;
        // DocString: FMTEvent::size()
        /**
        Return event size
        */
        size_t size() const;
        // DocString: FMTEvent::hash()
        /**

        */
		size_t hash() const;
		// DocString: FMTEvent::getRelation(const FMTEvent&)
		/**

		*/
		FMTEventRelation getRelation(const FMTEvent& rhs) const;
        // DocString: FMTEvent::operator==(const FMTEvent&)
        /**
        Comparison operator equal to
        */
        bool operator==(const FMTEvent& rhs) const;
        // DocString: FMTEvent::operator!=(const FMTEvent&)
        /**
        Comparison operator different than
        */
        bool operator!=(const FMTEvent& rhs) const;
        // DocString: FMTEvent::operator<(const FMTEvent&)
        /**
        Comparison operator less than
        */
        bool operator<(const FMTEvent& rhs) const;
        // DocString: FMTEvent::perimeter()
        /**
        The perimeter of the event, including any internal holes in the
        event.
        */
        size_t perimeter() const;
        // DocString: FMTEvent::height()
        /**
        Return height of the event
        */
        size_t height() const;
        // DocString: FMTEvent::width()
        /**
        Return width of the event
        */
        size_t width() const;
        // DocString: FMTEvent::averageCentroid()
        /**
        Return centroid based on the envelope of the event
        */
        FMTCoordinate averageCentroid() const;
        // DocString: FMTEvent::getStatsHeader()
        /**
        Return the header of the patch stats (size, perimeter, height and width)
        */
        static std::string getStatsHeader();
        // DocString: FMTEvent::getStats()
        /**
        Return string containing size, perimeter, height and width
        */
        std::string getStats() const;
        // DocString: FMTEvent::erase(const FMTCoordinate&)
        /**
        Erase coordinate from event
        */
        virtual void erase(const FMTCoordinate& newlocation);
        // DocString: FMTEvent::merge(const FMTEvent& event)
        /**
        Merge two events
        */
        virtual void merge(const FMTEvent& event);
        // DocString: FMTEvent::insert(const FMTCoordinate&)
        /**
        Insert coordinate in the event
        */
        virtual void insert(const FMTCoordinate& newlocation);
        // DocString: FMTEvent::setActionId(const int&)
        /**
        Setter for action id
        */
        void setActionId(const int& laction_id);
        // DocString: FMTEvent::getActionId()
        /**
        Getter of the action id
        */
        const int& getActionId() const;
        // DocString: FMTEvent::getPeriod()
        /**
        Getter of the period
        */
        const int& getPeriod() const;
        // DocString: FMTEvent::getGraphFamily()
        /**
        Getter for the graph family
        */
        const size_t& getGraphFamily() const;
        // DocString: FMTEvent::getElements
        /**
        @brief Getter of the period
        @return The event elements
        */
        const std::set<FMTCoordinate>& getElements() const;
        // DocString: FMTEvent::ignit(const FMTspatialaction&, const FMTCoordinate&, const int&, const int&)
        /**

        */
        virtual std::vector<std::set<FMTCoordinate>::const_iterator> ignit(const size_t& eventmaximalsize,
            const std::set<FMTCoordinate>::const_iterator& ignit, int p_actionId, int p_period, size_t p_GraphFamily);
        // DocString: FMTEvent::spread(const size_t& eventminimalsize,const size_t& eventmaximalsize,const size_t& neighboringsize,const std::set<FMTCoordinate>& territory, std::vector<std::set<FMTCoordinate>::const_iterator> active)
        /**

        */
        virtual bool spread(const size_t& eventminimalsize,const size_t& eventmaximalsize,
			const size_t& neighboringsize,const std::set<FMTCoordinate>& territory, std::vector<std::set<FMTCoordinate>::const_iterator> active);
        // DocString: FMTEvent::distance(const FMTEvent&)
        /**
        Return the distance between this event and the event pass as argument
        */
        double distance(const FMTEvent& rhs) const;
        // DocString: FMTEvent::within(unsigned int, const FMTEvent&)
        /**
        Return true if the event is within specified distance of the envelope
        */
        template<typename T>
        bool within(const T& dist, const FMTEvent& rhs) const;
        // DocString: FMTEvent::withinc(unsigned int, const FMTCoordinate&)
        /**
        Return true if coordinate is within specified distance of the coordinates
        */
        template<typename T>
        bool within(const T& dist, const FMTCoordinate& location) const;
        // DocString: FMTEvent::contain(const FMTCoordinate&)
        /**
        Return true if coordinate is in elements
        */
        bool contain(const FMTCoordinate& coord)const;
        // DocString: FMTEvent::willSplitEvent(const FMTCoordinate&)
        /**
        @param[in] p_coordinate the coordinate that will be removed
        @return true if it will split the event
        */
        bool willSplitEvent(const FMTCoordinate& p_coordinate) const;
        // DocString: FMTEvent::splitEvent(const unsigned int&, std::vector<FMTsaevent>&)
        /**
        Check if events are split and fill vector of split events
        Remove all elements from the current event and put it in the events in splittedevents
        */
        bool splitEvent(std::vector<FMTEvent>& splittedevents) const;
		// DocString: FMTEvent::getTerritory
	   /**
		Returns the adjacent territory of the event base on a distance.
	   */
		std::set<FMTCoordinate>getTerritory(const size_t& distance) const;
		// DocString: FMTEvent::getBorders
	   /**
		Returns coordinate of the border of the even
	   */
		std::vector<std::set<FMTCoordinate>::const_iterator>getBorders() const;
        // DocString: FMTEvent::getOutsideBorders
       /**
        @brief get ouside borders of the event
        @return a set of coordinates
       */
        std::vector<std::pair<FMTCoordinate,FMTCoordinate>>getOutsideBordersPair() const;
		// DocString: FMTEvent::getenveloppe
		/**
		Coordinates that create the envelope of the event
		//0//-//1//
		//-//-//-//
		//2//-//3//
		Returns coordinate of the enveloppe
		*/
		std::array<FMTCoordinate,4>getEnveloppe() const;
		// DocString: FMTEvent::getclosescoordinate
		/**
		Return the closes coordinate between two FMTEvent
		*/
		void getClosesCoordinates(const FMTEvent& rhs,
			std::set<FMTCoordinate>::const_iterator& thiscoordinate,
			std::set<FMTCoordinate>::const_iterator& rhscoordinate) const;
    private:
        friend class boost::serialization::access;
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
