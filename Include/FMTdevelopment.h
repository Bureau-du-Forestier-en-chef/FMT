/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTDEV_Hm_included
#define FMTDEV_Hm_included

#include "FMTmask.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include "FMTobject.h"
#include <memory>
#include <vector>
#include <boost/serialization/export.hpp>
#include "FMTexceptionhandler.h"

namespace Graph
{
	class FMTGraphVertexToYield;
}


/// The Core namespace provides classes for simulating stands/strata growth/harvest through time.
namespace Core
{
class FMTFuturDevelopment;
class FMTTransition;
class FMTAction;
class FMTYieldRequest;
class FMTYields;
class FMTDevelopmentPath;
class FMTSpec;
class FMTYldBounds;
// DocString: FMTDevelopment
/**
FMTDevelopment is the base class of FMTActualDevelopment and FMTFuturDevelopment. This class is the thing that look like
a forest stand. It holds informations about it's age, at which period it exist, the lock level of the stand and finaly
the mask hold a description of the growth/spatial reference/ and other potential sutff.
*/
class FMTEXPORT FMTDevelopment : public FMTObject
    {
	public:
		// DocString: FMTDevelopment::(FMTDevelopment&& rhs)
		/**
		@brief Move copy constructor
		@param[in] rhs development to swap
		*/
		FMTDevelopment(FMTDevelopment&& rhs);
		// DocString: FMTDevelopment::operator=(FMTDevelopment&& rhs)
		/**
		@brief Move copy assignement
		@param[in] rhs development to swap
		*/
		FMTDevelopment& operator=(FMTDevelopment&& rhs);
		// DocString: FMTDevelopment::getAge
		/**
		Get the age of the development.
		*/
		inline int getAge() const
			{
			return static_cast<int>(age);
			}
		// DocString: FMTDevelopment::getShortAge
		/**
		Get the age of the development in a short int format
		*/
		inline uint8_t getShortAge() const
		{
			return age;
		}
		// DocString: FMTDevelopment::getLock
		/**
		Get the lock of the development.
		*/
		inline int getLock() const
			{
			return static_cast<int>(lock);
			}
		// DocString: FMTDevelopment::getPeriod
		/**
		Get the period of the development.
		*/
		inline int getPeriod() const
			{
			return static_cast<int>(period);
			}
		// DocString: FMTDevelopment::getMask
		/**
		Get the mask of the development.
		*/
		inline const Core::FMTMask& getMask() const
			{
			return mask;
			}
		// DocString: FMTDevelopment::getMaskCopy
		/**
		Get the mask of the development.
		*/
		inline Core::FMTMask getMaskCopy() const
		{
			return mask;
		}
		// DocString: FMTDevelopment::setAge
		/**
		Set the age of the development.
		*/
		void setAge(const int& lage);
		// DocString: FMTDevelopment::setLock
		/**
		Set the lock of the development.
		*/
		void setLock(const int& llock);
		// DocString: FMTDevelopment::setPeriod
		/**
		Set the period of the development.
		*/
		void setPeriod(const int& lperiod);
		// DocString: FMTDevelopment::setMask
		/**
		Set the mask of the development.
		*/
		void setMask(const Core::FMTMask& lmask);
		// DocString: FMTDevelopment()
		/**
		Default constructor for FMTDevelopment
		*/
        FMTDevelopment();
		// DocString: ~FMTDevelopment()
		/**
		Default destructor for FMTDevelopment.
		*/
		virtual ~FMTDevelopment() = default;
		// DocString: FMTDevelopment(const FMTMask&,const int&,const int&)
		/**
		FMTDevelopment constructor using a FMTMask the age and the lock level of the FMTDevelopment.
		*/
		FMTDevelopment(const FMTMask& mask, const int& age, const int& lock);
		// DocString: FMTDevelopment(const FMTMask,const int&,const int&,const int&)
		/**
		FMTDevelopment constructor using a FMTMask the age and the lock level and the period of the FMTDevelopment.
		*/
		FMTDevelopment(const FMTMask& lmask, const int& lage, const int& llock, const int& lperiod);
		// DocString: FMTDevelopment(const FMTDevelopment&)
		/**
		FMTDevelopment copy constructor.
		*/
		FMTDevelopment(const FMTDevelopment& rhs);
		// DocString: FMTDevelopment::Clone()
		/**
		Virtual clone function for FMTDevelopment for abstraction
		*/
		virtual std::unique_ptr<FMTDevelopment> Clone() const;
		// DocString: FMTDevelopment::operator=
		/**
		FMTDevelopment copy assignment.
		*/
        FMTDevelopment& operator = (const FMTDevelopment& rhs);
		// DocString: FMTDevelopment::operator==
		/**
		FMTDevelopment equality operator.
		*/
        bool operator == (const FMTDevelopment& rhs) const;
		// DocString: FMTDevelopment::operator!=
		/**
		FMTDevelopment nonequality operator.
		*/
        bool operator != (const FMTDevelopment& rhs) const;
		// DocString: FMTDevelopment::operator<
		/**
		FMTDevelopment less than operator.
		*/
		bool operator < (const FMTDevelopment& rhs) const;
		// DocString: FMTDevelopment::grow
		/**
		The function returns a grown up copy of this FMTDevelopment increasing the age and the period and
		decreasing the lock level if the lock level was > 0.
		*/
        FMTFuturDevelopment grow() const;
		// DocString: FMTDevelopment::getYieldRequest
		/**
		Get a yieldrequest without any reference to a FMTGraph for a given (ylds) and this FMTDevelopment.
		*/
		FMTYieldRequest getYieldRequest(const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::operable
		/**
		Check if this FMTDevelopment is operable to an FMTAction (action) based on multiple yields (yields).
		*/
        bool operable(const FMTAction& action,const Core::FMTYields& ylds,
			const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::anyOperable
		/**
		Check if this FMTDevelopment is operable to any of FMTAction in the (actions) vector based on multiple yields (yields).
		*/
		bool anyOperable(const std::vector<const FMTAction*>& actions, const Core::FMTYields& ylds,
			const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::operator std::string
		/**
		Returns the string reprensentation of a FMTDevelopment like in the area section.
		*/
        virtual operator std::string() const;
		// DocString: FMTDevelopment::getArea
		/**
		Returns the area of the FMTDevelopment
		*/
		virtual double getArea() const;
		// DocString: FMTDevelopment::clearLock
		/**
		Returns a copy of this FMTDevelopment but set the lock level to 0.
		*/
		FMTDevelopment clearLock() const;
		// DocString: FMTDevelopment::operate
		/**
		Returns a vector of FMTDevelopmentPath if the FMTDevelopment is operated by 
		an (action), a (Transition) bases on multiple yields and the sorted FMTthemes of the model.
		*/
		std::vector<FMTDevelopmentPath> operate(const FMTAction& action,
			const FMTTransition& Transition,
			const Core::FMTYields& ylds,
			const std::vector<FMTTheme>& themes) const;
		// DocString: FMTDevelopment::getInventoryCoef
		/**
		Returns inventory coefficiant of the FMTDevelopment for a given yield (target_yield) based on (yields).
		The amount of (yield) do this FMTDevelopment has.
		*/
		double getInventoryCoef(const Core::FMTYields& ylds,const std::string& target_yield,
			const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::getHarvestCoef
		/**
		Returns the harvest coefficiant of the FMTDevelopment for a given yield (target_yield) if it follows
		multiple development path for a given action and based on yields.
		It gives the amount of yield the action produce if the *this FMTDevelopment is operated.
		*/
		double getHarvestCoef(const std::vector<FMTDevelopmentPath>& topaths,
			const FMTAction& action,const Core::FMTYields& ylds,const std::string& target_yield,
			const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::is
		/**
		Check if this FMTDevelopment respects some specifications based on multiple yields.
		*/
		bool is(const FMTSpec& specification, const Core::FMTYields& ylds,
			const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::worthTestingOperability
		/**
		This function returns true if the function worth testing for operability for the action.
		Use for optimization.
		*/
		bool worthTestingOperability(const FMTAction& action) const;
		// DocString: FMTDevelopment::anyWorthTestingOperability
		/**
		Check if any action in the actions vector worth testing the operability and returns it's pointer distance from
		the firestaction.
		*/
		std::vector<int> anyWorthTestingOperability(const std::vector<const FMTAction*>& actions, const FMTAction& firstaction) const;
		// DocString: FMTDevelopment::isAnyWorthTestingOperability
		/**
		Return a vector if true worth testting if false dont test for operability.
		*/
		std::vector<bool> isAnyWorthTestingOperability(const std::vector<const FMTAction*>& actions,
			const std::vector<FMTAction>& allactions) const noexcept;
		// DocString: FMTDevelopment::hash
		/**
		Hash function for FMTDevelopment combining mask,age,lock and period of the FMTDevelopment.
		*/
		inline size_t hash() const
			{
			std::size_t seed =0;
			boost::hash_combine(seed, boost::hash<Core::FMTMask>()(mask));
			boost::hash_combine(seed, boost::hash<uint8_t>()(age));
			boost::hash_combine(seed, boost::hash<uint8_t>()(lock));
			boost::hash_combine(seed, boost::hash<uint8_t>()(period));
			return seed;
			}
		private:
			// DocString: FMTDevelopment::_isOutOfBound
			/**
			@brief check if the p_yield is out of bound for the given request.
			@param[in] p_request the yield request
			@param[in] p_yields all the yields
			@param[in] p_bound the bound to test.
			@param[in] p_yield the yield to test the value against the bound.
			@return true if out of bound else false.
			*/
			bool _isOutOfBound(const FMTYieldRequest& p_request,
				const FMTYields& p_yields,
				const Core::FMTYldBounds& p_bound,
				const std::string& p_yield) const;
			// DocString: FMTDevelopment::serialize
			/**
			Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
			*/
			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				try {
					ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
					ar& BOOST_SERIALIZATION_NVP(mask);
					ar& BOOST_SERIALIZATION_NVP(age);
					ar& BOOST_SERIALIZATION_NVP(lock);
					ar& BOOST_SERIALIZATION_NVP(period);
				}
				catch (...)
				{
					_exhandler->printExceptions("", "FMTDevelopment::serialize", __LINE__, __FILE__);
				}
			}
			// DocString: FMTDevelopment::mask
			///Mask of the FMTDevelopment data member
			FMTMask mask;
			// DocString: FMTDevelopment::age
			///Age is the age of the FMTDevelopment in period (no unit)
			uint8_t age;
			// DocString: FMTDevelopment::lock
			///Lock is the lock level of the FMTDevelopment if lock == 0 then the development is not locked
			uint8_t lock;
			// DocString: FMTDevelopment::period
			///period is the period at which the development exist from 0 to ...
			uint8_t period;
    };

}

namespace boost {
	/**
	boost hash function for FMTDevelopment.
	*/
  template <>
  struct hash<Core::FMTDevelopment>
  {
    std::size_t operator()(const Core::FMTDevelopment& dev) const
        {
		return dev.hash();
        }
  };


}
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Core::FMTDevelopment)
BOOST_CLASS_TRACKING(Core::FMTDevelopment, boost::serialization::track_always)
BOOST_CLASS_EXPORT_KEY(Core::FMTDevelopment)

#endif // FMTDEV_Hm_included
