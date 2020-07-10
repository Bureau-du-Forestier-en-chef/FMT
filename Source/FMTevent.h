/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTEVENT_H_INCLUDED
#define FMTEVENT_H_INCLUDED

#include "FMTcoordinate.h"
#include <random>
#include <limits>
#include <iterator>
#include "FMTspatialaction.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/set.hpp>
namespace Spatial
{
// DocString: FMTevent
/**
This is the base class use to create FMTsaevent or FMTsesevent. It is a set of FMTcoordinate (elements)
corresponding to the FMTcoordinates presents in the event.
*/
class FMTevent
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(ignition);
		ar & BOOST_SERIALIZATION_NVP(enveloppe);
		ar & BOOST_SERIALIZATION_NVP(elements);
	}
	// DocString: FMTevent::midposition()
    /**
	Get mid point of elements.
	*/
	FMTcoordinate midposition() const
		{
		const size_t midlocation = (elements.size() / 2);
		std::set<FMTcoordinate>::const_iterator it = elements.begin();
		std::advance(it,midlocation);
		return (*it);
		}
    protected:
        // DocString: FMTevent::ignition
        /**
        Coordinate where the event started
        */
        FMTcoordinate ignition;
        // DocString: FMTevent::enveloppe
        /**
        Coordinates that create the envelope of the event
        //0//-//1//
		//-//-//-//
		//2//-//3//
        */
		std::vector<FMTcoordinate>enveloppe;
    public:
        // DocString: FMTevent::elements
        /**
        Coordinates presents in the event
        */
        std::set<FMTcoordinate>elements;
        // DocString: ~FMTevent()
        /**
        Destructor
        */
        virtual ~FMTevent() = default;
        // DocString: FMTevent()
        /**
        Constructor
        */
        FMTevent();
        // DocString: FMTevent(const FMTcoordinate&)
        /**
        Constructor with coordinate for ignition
        */
		FMTevent(const FMTcoordinate& location);
        // DocString: FMTevent(const FMTevent&)
        /**
        Copy constructor
        */
		FMTevent(const FMTevent& rhs);
        // DocString: FMTevent::operator=
        /**
        Copy assignment operator
        */
        FMTevent& operator=(const FMTevent& rhs);
        // DocString: FMTevent::empty
        /**
        Test whether event is empty
        */
        bool empty() const {return elements.empty();}
        // DocString: FMTevent::size
        /**
        Return event size
        */
        size_t size() const {return elements.size();}
        // DocString: FMTevent::hash
        /**

        */
        size_t hash() const{return boost::hash<Spatial::FMTcoordinate>()(ignition);}
        // DocString: FMTevent::operator==
        /**
        Comparison operator equal to
        */
        virtual bool operator==(const FMTevent& rhs) const;
        // DocString: FMTevent::operator!=
        /**
        Comparison operator different than
        */
        bool operator!=(const FMTevent& rhs) const;
        // DocString: FMTevent::operator<
        /**
        Comparison operator less than
        */
        virtual bool operator<(const FMTevent& rhs) const {return (this->midposition() < rhs.midposition());}
        // DocString: FMTevent::perimeter
        /**
        The perimeter of the event, including any internal holes in the
        event.
        */
        size_t perimeter() const;
        // DocString: FMTevent::height
        /**
        Return height of the event
        */
        size_t height() const;
        // DocString: FMTevent::width
        /**
        Return width of the event
        */
        size_t width() const;
        // DocString: FMTevent::averagecentroid
        /**
        Return centroid based on the envelope of the event
        */
        FMTcoordinate averagecentroid() const;
        // DocString: FMTevent::getstats
        /**
        Return string containing size, perimeter, height and width
        */
        std::string getstats() const;
        // DocString: FMTevent::erase
        /**
        Erase coordinate from event
        */
        virtual void erase(const FMTcoordinate& newlocation);
        // DocString: FMTevent::merge
        /**
        Merge two events
        */
        virtual void merge(const FMTevent& event);
        // DocString: FMTevent::insert
        /**
        Insert coordinate in the event
        */
        virtual void insert(const FMTcoordinate& newlocation);
        // DocString: FMTevent::distance
        /**
        Return the distance between this event and the event pass as argument
        */
        double distance(const FMTevent& rhs) const;
        // DocString: FMTevent::within
        /**
        Return true if the event is within specified distance of the envelope
        */
        bool within(unsigned int dist, const FMTevent& rhs) const;
        // DocString: FMTevent::withinc
        /**
        Return true if coordinate is within specified distance of the envelope
        */
        bool withinc(unsigned int dist, const FMTcoordinate& location) const;
        // DocString: FMTevent::contain
        /**
        Return true if coordinate is in elements
        */
        bool contain(const FMTcoordinate& coord)const;
        // DocString: FMTevent::withinelements
        /**
        Return true if coordinate is within specified distance of at least one coordinate in elements
        */
        bool whithinelements(unsigned int dist, const FMTcoordinate& location) const;
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
#endif // FMTEVENT_H_INCLUDED
