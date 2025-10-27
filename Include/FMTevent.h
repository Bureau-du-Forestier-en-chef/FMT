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
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		//ar & BOOST_SERIALIZATION_NVP(ignition);
		//ar & BOOST_SERIALIZATION_NVP(active);
		ar & BOOST_SERIALIZATION_NVP(action_id);
		ar & BOOST_SERIALIZATION_NVP(period);
		ar & BOOST_SERIALIZATION_NVP(elements);
	}
    protected:
        // DocString: FMTevent::ignition
        /**
        Coordinate where the event started
        */
        //FMTcoordinate ignition;
		// DocString: FMTevent::active
		/**
		Used in spread, the active coordinate are the next to spread.
		*/
		//std::vector<FMTcoordinate>active;
		// DocString: FMTsaevent::action_id
        /**
        Action id from the FMTmodel.
        */
        int action_id;
        // DocString: FMTsaevent::period
        /**
        Period at which the event take place
        */
        int period;
    public:
		// DocString: FMTevent::midposition()
		/**
		Get mid point of elements.
		*/
		inline std::set<FMTcoordinate>::const_iterator midposition() const
		{
			std::set<FMTcoordinate>::const_iterator it = elements.begin();
			if (elements.size() > 1)
			{
				const size_t midlocation = (elements.size() / 2);
				std::advance(it, midlocation);
			}
			return it;
		}
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
		// DocString: FMTsaevent(const FMTcoordinate&,const int&,const int&)
        /**
        Constructor with coordinate, action_id and period
        */
        FMTevent(const FMTcoordinate& location,const int& laction_id,const int& lperiod);
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
        // DocString: FMTevent::empty()
        /**
        Test whether event is empty
        */
        bool empty() const {return elements.empty();}
        // DocString: FMTevent::size()
        /**
        Return event size
        */
        size_t size() const {return elements.size();}
        // DocString: FMTevent::hash()
        /**

        */
		size_t hash() const;// {return boost::hash<Spatial::FMTcoordinate>()(ignition); }
		// DocString: FMTevent::getrelation(const FMTevent&)
		/**

		*/
		FMTeventrelation getrelation(const FMTevent& rhs) const;
        // DocString: FMTevent::operator==(const FMTevent&)
        /**
        Comparison operator equal to
        */
        virtual bool operator==(const FMTevent& rhs) const;
        // DocString: FMTevent::operator!=(const FMTevent&)
        /**
        Comparison operator different than
        */
        bool operator!=(const FMTevent& rhs) const;
        // DocString: FMTevent::operator<(const FMTevent&)
        /**
        Comparison operator less than
        */
        virtual bool operator<(const FMTevent& rhs) const;
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
        void setactionid(const int& laction_id){action_id=laction_id;}
        // DocString: FMTevent::setperiod(const int&)
        /**
        Setter for period
        */
        void setperiod(const int& lperiod){period=lperiod;}
        // DocString: FMTevent::getactionid()
        /**
        Getter of the action id
        */
        inline const int& getactionid() const {return action_id;}
        // DocString: FMTevent::getperiod()
        /**
        Getter of the period
        */
        inline const int& getperiod() const {return period;}
        // DocString: FMTevent::ignit(const FMTspatialaction&, const FMTcoordinate&, const int&, const int&)
        /**

        */
        virtual std::vector<std::set<FMTcoordinate>::const_iterator> ignit(const size_t& eventmaximalsize,const std::set<FMTcoordinate>::const_iterator& ignit, const int& laction_id, const int& lperiod);
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
        bool splitevent(const unsigned int& ldistance, std::vector<FMTevent>& splittedevents) const;
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
		std::vector<FMTcoordinate>getenveloppe() const;
		// DocString: FMTevent::getclosescoordinate
		/**
		Return the closes coordinate between two FMTevent
		*/
		void getclosescoordinates(const FMTevent& rhs,
			std::set<FMTcoordinate>::const_iterator& thiscoordinate,
			std::set<FMTcoordinate>::const_iterator& rhscoordinate) const;
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
