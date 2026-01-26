/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTEVENT_Hm_included
#define FMTEVENT_Hm_included

#include "FMTcoordinate.h"
#include <random>
#include <limits>
#include <iterator>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/set.hpp>
#include <array>

namespace Spatial
{
class FMTeventrelation;
// DocString: FMTevent
/**
This is the base class use to create FMTsaevent or FMTsesevent. It is a set of FMTcoordinate (elements)
corresponding to the FMTcoordinates presents in the event.
*/
class FMTEXPORT FMTevent
    {
    public:
		// DocString: FMTevent::midposition()
		/**
		Get mid point of elements.
		*/
        std::set<FMTcoordinate>::const_iterator midposition() const;
        // DocString: ~FMTevent()
        /**
        Destructor
        */
        ~FMTevent() = default;
        // DocString: FMTevent(const FMTevent&)
        /**
        Copy constructor
        */
        FMTevent(const FMTevent& rhs) = default;
        // DocString: FMTevent::operator=
        /**
        Copy assignment operator
        */
        FMTevent& operator=(const FMTevent& rhs) = default;
        // DocString: FMTevent()
        /**
        Constructor
        */
        FMTevent() = default;
		// DocString: FMTsaevent(const FMTcoordinate&,const int&,const int&)
        /**
        Constructor with coordinate, action_id and period
        */
        FMTevent(const FMTcoordinate& p_location,
            int p_actionId,
            int p_period,
            size_t p_centroidGraphFamily);
        // DocString: FMTevent::empty()
        /**
        Test whether event is empty
        */
        bool empty() const;
        // DocString: FMTevent::size()
        /**
        Return event size
        */
        size_t size() const;
        // DocString: FMTevent::hash()
        /**

        */
		size_t hash() const;
		// DocString: FMTevent::getrelation(const FMTevent&)
		/**

		*/
		FMTeventrelation getrelation(const FMTevent& rhs) const;
        // DocString: FMTevent::operator==(const FMTevent&)
        /**
        Comparison operator equal to
        */
        bool operator==(const FMTevent& rhs) const;
        // DocString: FMTevent::operator!=(const FMTevent&)
        /**
        Comparison operator different than
        */
        bool operator!=(const FMTevent& rhs) const;
        // DocString: FMTevent::operator<(const FMTevent&)
        /**
        Comparison operator less than
        */
        bool operator<(const FMTevent& rhs) const;
        // DocString: FMTevent::perimeter()
        /**
        The perimeter of the event, including any internal holes in the
        event.
        */
        size_t perimeter() const;
        // DocString: FMTevent::height()
        /**
        Return height of the event
        */
        size_t height() const;
        // DocString: FMTevent::width()
        /**
        Return width of the event
        */
        size_t width() const;
        // DocString: FMTevent::averagecentroid()
        /**
        Return centroid based on the envelope of the event
        */
        FMTcoordinate averagecentroid() const;
        // DocString: FMTevent::getstatsheader()
        /**
        Return the header of the patch stats (size, perimeter, height and width)
        */
        static std::string getstatsheader();
        // DocString: FMTevent::getstats()
        /**
        Return string containing size, perimeter, height and width
        */
        std::string getstats() const;
        // DocString: FMTevent::erase(const FMTcoordinate&)
        /**
        Erase coordinate from event
        */
        virtual void erase(const FMTcoordinate& newlocation);
        // DocString: FMTevent::merge(const FMTevent& event)
        /**
        Merge two events
        */
        virtual void merge(const FMTevent& event);
        // DocString: FMTevent::insert(const FMTcoordinate&)
        /**
        Insert coordinate in the event
        */
        virtual void insert(const FMTcoordinate& newlocation);
        // DocString: FMTevent::setactionid(const int&)
        /**
        Setter for action id
        */
        void setactionid(const int& laction_id);
        // DocString: FMTevent::getactionid()
        /**
        Getter of the action id
        */
        const int& getactionid() const;
        // DocString: FMTevent::getperiod()
        /**
        Getter of the period
        */
        const int& getperiod() const;
        // DocString: FMTevent::getGraphFamily()
        /**
        Getter for the graph family
        */
        const size_t& getGraphFamily() const;
        // DocString: FMTevent::getElements
        /**
        @brief Getter of the period
        @return The event elements
        */
        const std::set<FMTcoordinate>& getElements() const;
        // DocString: FMTevent::ignit(const FMTspatialaction&, const FMTcoordinate&, const int&, const int&)
        /**

        */
        virtual std::vector<std::set<FMTcoordinate>::const_iterator> ignit(const size_t& eventmaximalsize,
            const std::set<FMTcoordinate>::const_iterator& ignit, int p_actionId, int p_period, size_t p_GraphFamily);
        // DocString: FMTevent::spread(const size_t& eventminimalsize,const size_t& eventmaximalsize,const size_t& neighboringsize,const std::set<FMTcoordinate>& territory, std::vector<std::set<FMTcoordinate>::const_iterator> active)
        /**

        */
        virtual bool spread(const size_t& eventminimalsize,const size_t& eventmaximalsize,
			const size_t& neighboringsize,const std::set<FMTcoordinate>& territory, std::vector<std::set<FMTcoordinate>::const_iterator> active);
        // DocString: FMTevent::distance(const FMTevent&)
        /**
        Return the distance between this event and the event pass as argument
        */
        double distance(const FMTevent& rhs) const;
        // DocString: FMTevent::within(unsigned int, const FMTevent&)
        /**
        Return true if the event is within specified distance of the envelope
        */
        template<typename T>
        bool within(const T& dist, const FMTevent& rhs) const;
        // DocString: FMTevent::withinc(unsigned int, const FMTcoordinate&)
        /**
        Return true if coordinate is within specified distance of the coordinates
        */
        template<typename T>
        bool within(const T& dist, const FMTcoordinate& location) const;
        // DocString: FMTevent::contain(const FMTcoordinate&)
        /**
        Return true if coordinate is in elements
        */
        bool contain(const FMTcoordinate& coord)const;
        // DocString: FMTevent::potentialysplittedevent(const FMTcoordinate&)
        /**
        Fastest way to evaluate if an event will be split after erasing a coordinate.
        Only when neighbors the 4 around the pixel
        */
        bool potentialysplitevent(const FMTcoordinate& coord) const;
        // DocString: FMTevent::splitevent(const unsigned int&, std::vector<FMTsaevent>&)
        /**
        Check if events are split and fill vector of split events
        Remove all elements from the current event and put it in the events in splittedevents
        */
        bool splitevent(std::vector<FMTevent>& splittedevents) const;
		// DocString: FMTevent::getterritory
	   /**
		Returns the adjacent territory of the event base on a distance.
	   */
		std::set<FMTcoordinate>getterritory(const size_t& distance) const;
		// DocString: FMTevent::getborders
	   /**
		Returns coordinate of the border of the even
	   */
		std::vector<std::set<FMTcoordinate>::const_iterator>getborders() const;
		// DocString: FMTevent::getenveloppe
		/**
		Coordinates that create the envelope of the event
		//0//-//1//
		//-//-//-//
		//2//-//3//
		Returns coordinate of the enveloppe
		*/
		std::array<FMTcoordinate,4>getEnveloppe() const;
		// DocString: FMTevent::getclosescoordinate
		/**
		Return the closes coordinate between two FMTevent
		*/
		void getclosescoordinates(const FMTevent& rhs,
			std::set<FMTcoordinate>::const_iterator& thiscoordinate,
			std::set<FMTcoordinate>::const_iterator& rhscoordinate) const;
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar& BOOST_SERIALIZATION_NVP(m_actionId);
            ar& BOOST_SERIALIZATION_NVP(m_period);
            ar& BOOST_SERIALIZATION_NVP(elements);
        }
        // DocString: FMTevent::m_actionId
        /**
        Action id from the FMTmodel.
        */
        int m_actionId;
        // DocString: FMTevent::m_period
        /**
        Period at which the event take place
        */
        int m_period;
        // DocString: FMTevent::m_centroidGraphFamily
        /**
        The graph family of the centroid
        */
        size_t m_centroidGraphFamily;
        // DocString: FMTevent::m_elements
        /**
        Coordinates presents in the event
       */
        std::set<FMTcoordinate>m_elements;
    };

}

namespace boost {

    template <>
    struct hash<Spatial::FMTevent>
    {
    std::size_t operator()(const Spatial::FMTevent& event) const
		 {
       return (event.hash());
		}
    };

}
#endif // FMTEVENT_Hm_included
