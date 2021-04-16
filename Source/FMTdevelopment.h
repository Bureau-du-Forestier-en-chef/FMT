/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
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
#include <boost/serialization/export.hpp>


namespace Core
{

class FMTfuturdevelopment;
class  FMTtransition;
// DocString: FMTdevelopment
/**
FMTdevelopment is the base class of FMTactualdevelopement and FMTfuturdevelopement. This class is the thing that look like
a forest stand. It holds informations about it's age, at which period it exist, the lock level of the stand and finaly
the mask hold a description of the growth/spatial reference/ and other potential sutff.
*/
class FMTdevelopment : public FMTobject
    {
	// DocString: FMTdevelopment::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		try {
			ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
			ar & BOOST_SERIALIZATION_NVP(mask);
			ar & BOOST_SERIALIZATION_NVP(age);
			ar & BOOST_SERIALIZATION_NVP(lock);
			ar & BOOST_SERIALIZATION_NVP(period);
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTdevelopment::serialize", __LINE__, __FILE__);
			}
	}
    public:
		// DocString: FMTdevelopment::mask
		///Mask of the FMTdevelopment data member
        FMTmask mask;
		// DocString: FMTdevelopment::age
		///Age is the age of the FMTdevelopment in period (no unit)
		int age;
		// DocString: FMTdevelopment::lock
		///Lock is the lock level of the FMTdevelopment if lock == 0 then the development is not locked
		int lock;
		// DocString: FMTdevelopment::period
		///period is the period at which the development exist from 0 to ...
        int period;
		// DocString: FMTdevelopment()
		/**
		Default constructor for FMTdevelopement
		*/
        FMTdevelopment();
		// DocString: ~FMTdevelopment()
		/**
		Default destructor for FMTdevelopement.
		*/
		virtual ~FMTdevelopment() = default;
		// DocString: FMTdevelopment(const FMTmask&,const int&,const int&)
		/**
		FMTdevelopement constructor using a FMTmask the age and the lock level of the FMTdevelopment.
		*/
		FMTdevelopment(const FMTmask& mask, const int& age, const int& lock);
		// DocString: FMTdevelopment(const FMTmask,const int&,const int&,const int&)
		/**
		FMTdevelopement constructor using a FMTmask the age and the lock level and the period of the FMTdevelopment.
		*/
		FMTdevelopment(const FMTmask& lmask, const int& lage, const int& llock, const int& lperiod);
		// DocString: FMTdevelopment(const FMTdevelopment&)
		/**
		FMTdevelopement copy constructor.
		*/
		FMTdevelopment(const FMTdevelopment& rhs);
		// DocString: FMTdevelopment::Clone()
		/**
		Virtual clone function for FMTdevelopement for abstraction
		*/
		virtual std::unique_ptr<FMTdevelopment> Clone() const;
		// DocString: FMTdevelopment::operator=
		/**
		FMTdevelopement copy assignment.
		*/
        FMTdevelopment& operator = (const FMTdevelopment& rhs);
		// DocString: FMTdevelopment::operator==
		/**
		FMTdevelopement equality operator.
		*/
        bool operator == (const FMTdevelopment& rhs) const;
		// DocString: FMTdevelopment::operator!=
		/**
		FMTdevelopement nonequality operator.
		*/
        bool operator != (const FMTdevelopment& rhs) const;
		// DocString: FMTdevelopment::operator<
		/**
		FMTdevelopement less than operator.
		*/
		bool operator < (const FMTdevelopment& rhs) const;
		// DocString: FMTdevelopment::grow
		/**
		The function returns a grown up copy of this FMTdevelopement increasing the age and the period and
		decreasing the lock level if the lock level was > 0.
		*/
        FMTfuturdevelopment grow() const;
		// DocString: FMTdevelopment::operable
		/**
		Check if this FMTdevelopement is operable to an FMTaction (action) based on multiple yields (yields).
		*/
        bool operable(const FMTaction& action,const FMTyields& ylds) const;
		// DocString: FMTdevelopment::anyoperable
		/**
		Check if this FMTdevelopement is operable to any of FMTaction in the (actions) vector based on multiple yields (yields).
		*/
		bool anyoperable(const std::vector<const FMTaction*>& actions,const FMTyields& ylds) const;
		// DocString: FMTdevelopment::operator std::string
		/**
		Returns the string reprensentation of a FMTdevelopment like in the area section.
		*/
        virtual operator std::string() const;
		// DocString: FMTdevelopment::getarea
		/**
		Returns the area of the FMTdevelopment
		*/
		virtual double getarea() const;
		// DocString: FMTdevelopment::clearlock
		/**
		Returns a copy of this FMTdevelopment but set the lock level to 0.
		*/
		FMTdevelopment clearlock() const;
		// DocString: FMTdevelopment::operate
		/**
		Returns a vector of FMTdevelopementpath if the FMTdevelopement is operated by 
		an (action), a (Transition) bases on multiple yields and the sorted FMTthemes of the model.
		*/
		std::vector<FMTdevelopmentpath> operate(const FMTaction& action,
			const FMTtransition& Transition,
			const FMTyields& ylds,
			const std::vector<FMTtheme>& themes) const;
		// DocString: FMTdevelopment::getinventorycoef
		/**
		Returns inventory coefficiant of the FMTdevelopement for a given yield (target_yield) based on (yields).
		The amount of (yield) do this FMTdevelopement has.
		*/
		double getinventorycoef(const FMTyields& ylds, const std::string& target_yield) const;
		// DocString: FMTdevelopment::getharvestcoef
		/**
		Returns the harvest coefficiant of the FMTdevelopement for a given yield (target_yield) if it follows
		multiple developement path for a given action and based on yields.
		It gives the amount of yield the action produce if the *this FMTdevelopement is operated.
		*/
		double getharvestcoef(const std::vector<FMTdevelopmentpath>& topaths,
			const FMTaction& action,const FMTyields& ylds,const std::string& target_yield) const;
		// DocString: FMTdevelopment::is
		/**
		Check if this FMTdevelopment respects some specifications based on multiple yields.
		*/
		bool is(const FMTspec& specification, const FMTyields& ylds) const;
		// DocString: FMTdevelopment::worthtestingoperability
		/**
		This function returns true if the function worth testing for operability for the action.
		Use for optimization.
		*/
		bool worthtestingoperability(const FMTaction& action) const;
		// DocString: FMTdevelopment::anyworthtestingoperability
		/**
		Check if any action in the actions vector worth testing the operability and returns it's pointer distance from
		the firestaction.
		*/
		std::vector<int> anyworthtestingoperability(const std::vector<const FMTaction*>& actions, const FMTaction& firstaction) const;
		// DocString: FMTdevelopment::isanyworthtestingoperability
		/**
		Return a vector if true worth testting if false dont test for operability.
		*/
		std::vector<bool> isanyworthtestingoperability(const std::vector<const FMTaction*>& actions,
			const std::vector<FMTaction>& allactions) const noexcept;
		// DocString: FMTdevelopment::hash
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
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Core::FMTdevelopment)
BOOST_CLASS_TRACKING(Core::FMTdevelopment, boost::serialization::track_always)
BOOST_CLASS_EXPORT_KEY(Core::FMTdevelopment)

#endif // FMTDEV_H_INCLUDED
