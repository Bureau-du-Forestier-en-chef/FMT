/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef FMTSAEVENT_H
#define FMTSAEVENT_H

#include "FMTevent.h"

namespace Spatial
{
// DocString: FMTsaevent
/**
This class is use to handle events created and used in the FMTsamodel.
All the actions of the same kind at the same period that are in
the distance specified by the user in the FMTsamodel are regrouped
in FMTsaevent evaluate spatial objectives.
*/
class FMTsaevent : public FMTevent
{
    friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("event", boost::serialization::base_object<FMTevent>(*this));
		ar & BOOST_SERIALIZATION_NVP(action_id);
		ar & BOOST_SERIALIZATION_NVP(period);
	}
    protected:
        // DocString: FMTsaevent::action_id
        /**
        Action id from the FMTsamodel.
        */
        int action_id;
        // DocString: FMTsaevent::period
        /**
        Period at which the event take place
        */
        int period;
    public:
        // DocString: FMTsaevent()
        /**
        Default constructor
        */
        FMTsaevent();
        // DocString: ~FMTsaevent()
        /**
        Destructor
        */
        ~FMTsaevent()=default;
        // DocString: FMTsaevent(const FMTevent&)
        /**
        Copy constructor with base class
        */
        FMTsaevent(const FMTevent& rhs);
        // DocString: FMTsaevent(const FMTsaevent&)
        /**
        Copy constructor
        */
        FMTsaevent(const FMTsaevent& rhs);
        // DocString: FMTsaevent(const FMTcoordinate&,const int&,const int&)
        /**
        Constructor with coordinate, action_id and period
        */
        FMTsaevent(const FMTcoordinate& location,const int& laction_id,const int& lperiod);
        // DocString: FMTsaevent::operator=
        /**
        Copy assignment operator
        */
        FMTsaevent& operator=(const FMTsaevent& other);
        // DocString: FMTsaevent::operator==
        /**
        Comparison operator equal to
        */
        bool operator==(const FMTsaevent& rhs) const;
        // DocString: FMTsaevent::operator<
        /**
        Comparison operator less than
        */
        bool operator<(const FMTsaevent& rhs) const;
        // DocString: FMTsaevent::setactionid
        /**
        Setter for action id
        */
        void setactionid(const int& laction_id){action_id=laction_id;}
        // DocString: FMTsaevent::setperiod
        /**
        Setter for period
        */
        void setperiod(const int& lperiod){period=lperiod;}
        // DocString: FMTsaevent::getactionid
        /**
        Getter of the action id
        */
        inline int getactionid() const {return action_id;}
        // DocString: FMTsaevent::getperiod
        /**
        Getter of the period
        */
        inline int getperiod() const {return period;}
        // DocString: FMTsaevent::potentialysplittedevent
        /**
        Fastest way to evaluate if an event will be split after erasing a coordinate.
        Only when neighbours the 4 around the pixel
        */
        bool potentialysplitevent(const FMTcoordinate& coord) const;
        // DocString: FMTsaevent::splitevent
        /**
        Check if events are split and fill vector of split events
        Remove all elements from the current event and put it in the events in splittedevents
        */
        bool splitevent(const unsigned int& ldistance, std::vector<FMTsaevent>& splittedevents) const;
};
}
#endif // FMTSAEVENT_H
