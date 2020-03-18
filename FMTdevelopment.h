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

#ifndef FMTDEV_H_INCLUDED
#define FMTDEV_H_INCLUDED

#include "FMTmask.h"
#include "FMTlifespans.h"
#include "FMTtransition.h"
#include "FMTyields.h"
#include "FMTaction.h"
#include "FMTtransitionmask.h"
#include "FMTdevelopmentpath.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include "FMTbounds.h"
#include "FMTobject.h"
#include <memory>
#include <vector>


namespace Core
{

class FMTfuturdevelopment;
class  FMTtransition;
/**
FMTdevelopment is the base class of FMTactualdevelopement and FMTfuturdevelopement. This class is the thing that look like
a forest stand. It holds informations about it's age, at which period it exist, the lock level of the stand and finaly
the mask hold a description of the growth/spatial reference/ and other potential sutff.
*/
class FMTdevelopment : public FMTobject
    {
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(mask);
		ar & BOOST_SERIALIZATION_NVP(age);
		ar & BOOST_SERIALIZATION_NVP(lock);
		ar & BOOST_SERIALIZATION_NVP(period);
	}
    public:
		///Mask of the FMTdevelopment data member
        FMTmask mask;
		///Age is the age of the FMTdevelopment in period (no unit)
		///Lock is the lock level of the FMTdevelopment if lock == 0 then the development is not locked
		///period is the period at which the development exist from 0 to ...
        int age,lock,period;
		/**
		Default constructor for FMTdevelopement
		*/
        FMTdevelopment();
		/**
		Default destructor for FMTdevelopement.
		*/
		virtual ~FMTdevelopment() = default;
		/**
		FMTdevelopement constructor using a FMTmask the age and the lock level of the FMTdevelopment.
		*/
		FMTdevelopment(const FMTmask& mask, const int& age, const int& lock);
		/**
		FMTdevelopement constructor using a FMTmask the age and the lock level and the period of the FMTdevelopment.
		*/
		FMTdevelopment(const FMTmask& lmask, const int& lage, const int& llock, const int& lperiod);
		/**
		FMTdevelopement copy constructor.
		*/
		FMTdevelopment(const FMTdevelopment& rhs);
		/**
		Virtual clone function for FMTdevelopement for abstraction
		*/
		virtual std::unique_ptr<FMTdevelopment> Clone() const;
		/**
		FMTdevelopement copy assignment.
		*/
        FMTdevelopment& operator = (const FMTdevelopment& rhs);
		/**
		FMTdevelopement equality operator.
		*/
        bool operator == (const FMTdevelopment& rhs) const;
		/**
		FMTdevelopement nonequality operator.
		*/
        bool operator != (const FMTdevelopment& rhs) const;
		/**
		FMTdevelopement less than operator.
		*/
		bool operator < (const FMTdevelopment& rhs) const;
		/**
		The function returns a grown up copy of this FMTdevelopement increasing the age and the period and
		decreasing the lock level if the lock level was > 0.
		*/
        FMTfuturdevelopment grow() const;
		/**
		Check if this FMTdevelopement is operable to an FMTaction (action) based on multiple yields (yields).
		*/
        bool operable(const FMTaction& action,const FMTyields& ylds) const;
		/**
		Check if this FMTdevelopement is operable to any of FMTaction in the (actions) vector based on multiple yields (yields).
		*/
		bool anyoperable(const std::vector<const FMTaction*>& actions,const FMTyields& ylds) const;
		/**
		Returns the string reprensentation of a FMTdevelopment like in the area section.
		*/
        virtual operator std::string() const;
		/**
		Returns the area of the FMTdevelopment
		*/
		virtual double getarea() const;
		/**
		Returns a copy of this FMTdevelopment but set the lock level to 0.
		*/
		FMTdevelopment clearlock() const;
		/**
		Returns a vector of FMTdevelopementpath if the FMTdevelopement is operated by 
		an (action), a (Transition) bases on multiple yields and the sorted FMTthemes of the model.
		*/
		std::vector<FMTdevelopmentpath> operate(const FMTaction& action,
			const FMTtransition& Transition,
			const FMTyields& ylds,
			const std::vector<FMTtheme>& themes) const;
		/**
		Returns inventory coefficiant of the FMTdevelopement for a given yield (target_yield) based on (yields).
		The amount of (yield) do this FMTdevelopement has.
		*/
		double getinventorycoef(const FMTyields& ylds, const std::string& target_yield) const;
		/**
		Returns the harvest coefficiant of the FMTdevelopement for a given yield (target_yield) if it follows
		multiple developement path for a given action and based on yields.
		It gives the amount of yield the action produce if the *this FMTdevelopement is operated.
		*/
		double getharvestcoef(const std::vector<FMTdevelopmentpath>& topaths,
			const FMTaction& action,const FMTyields& ylds,const std::string& target_yield) const;
		/**
		Check if this FMTdevelopment respects some specifications based on multiple yields.
		*/
		bool is(const FMTspec& specification, const FMTyields& ylds) const;
		/**
		This function returns true if the function worth testing for operability for the action.
		Use for optimization.
		*/
		bool worthtestingoperability(const FMTaction& action) const;
		/**
		Check if any action in the actions vector worth testing the operability and returns it's pointer distance from
		the firestaction.
		*/
		std::vector<int> anyworthtestingoperability(const std::vector<const FMTaction*>& actions, const FMTaction& firstaction) const;
		/**
		Hash function for FMTdevelopment combining mask,age,lock and period of the FMTdevelopement.
		*/
		inline size_t hash() const
			{
			std::size_t seed =0;
			boost::hash_combine(seed, boost::hash<Core::FMTmask>()(mask));
			boost::hash_combine(seed, boost::hash<int>()(age));
			boost::hash_combine(seed, boost::hash<int>()(lock));
			boost::hash_combine(seed, boost::hash<int>()(period));
			return seed;
			}
    };

}

namespace boost {
	/**
	boost hash function for FMTdevelopment.
	*/
  template <>
  struct hash<Core::FMTdevelopment>
  {
    std::size_t operator()(const Core::FMTdevelopment& dev) const
        {
		return dev.hash();
        }
  };


}

#endif // FMTDEV_H_INCLUDED
