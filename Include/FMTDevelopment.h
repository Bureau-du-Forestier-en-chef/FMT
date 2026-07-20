/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTDEV_Hm_included
#define FMTDEV_Hm_included

#include "FMTMask.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include "FMTObject.h"
#include <memory>
#include <vector>
#include <boost/serialization/export.hpp>
#include "FMTExceptionHandler.h"

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
@brief Base class of FMTActualDevelopment and FMTFuturDevelopment representing a forest stand.
@details Holds the age, period of existence, lock level, and a mask describing the growth, spatial reference and other information of the stand.
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
		@brief Move assignment operator for FMTDevelopment.
		@param[in] rhs the development to move from.
		@return a reference to this FMTDevelopment.
		*/
		FMTDevelopment& operator=(FMTDevelopment&& rhs);
		// DocString: FMTDevelopment::getAge
		/**
		@brief Get the age of the development.
		@return the age of the development.
		*/
		inline int getAge() const
			{
			return static_cast<int>(age);
			}
		// DocString: FMTDevelopment::getShortAge
		/**
		@brief Get the age of the development as a short int.
		@return the age of the development as a short int.
		*/
		inline uint8_t getShortAge() const
		{
			return age;
		}
		// DocString: FMTDevelopment::getLock
		/**
		@brief Get the lock of the development.
		@return the lock of the development.
		*/
		inline int getLock() const
			{
			return static_cast<int>(lock);
			}
		// DocString: FMTDevelopment::getPeriod
		/**
		@brief Get the period of the development.
		@return the period of the development.
		*/
		inline int getPeriod() const
			{
			return static_cast<int>(period);
			}
		// DocString: FMTDevelopment::getMask
		/**
		@brief Get a const reference to the mask of the development.
		@return a const reference to the mask of the development.
		*/
		inline const Core::FMTMask& getMask() const
			{
			return mask;
			}
		// DocString: FMTDevelopment::getMaskCopy
		/**
		@brief Get a copy of the mask of the development.
		@return a copy of the mask of the development.
		*/
		inline Core::FMTMask getMaskCopy() const
		{
			return mask;
		}
		// DocString: FMTDevelopment::setAge
		/**
		@brief Set the age of the development.
		@param[in] lage the age to set.
		*/
		void setAge(const int& lage);
		// DocString: FMTDevelopment::setLock
		/**
		@brief Set the lock of the development.
		@param[in] llock the lock to set.
		*/
		void setLock(const int& llock);
		// DocString: FMTDevelopment::setPeriod
		/**
		@brief Set the period of the development.
		@param[in] lperiod the period to set.
		*/
		void setPeriod(const int& lperiod);
		// DocString: FMTDevelopment::setMask
		/**
		@brief Set the mask of the development.
		@param[in] lmask the mask to set.
		*/
		void setMask(const Core::FMTMask& lmask);
		// DocString: FMTDevelopment()
		/**
		@brief Default constructor for FMTDevelopment.
		*/
        FMTDevelopment();
		// DocString: ~FMTDevelopment()
		/**
		@brief Default virtual destructor for FMTDevelopment.
		*/
		virtual ~FMTDevelopment() = default;
		// DocString: FMTDevelopment(const FMTMask&,const int&,const int&)
		/**
		@brief Construct a development from a mask, an age and a lock level.
		@param[in] mask the mask.
		@param[in] age the age.
		@param[in] lock the lock level.
		*/
		FMTDevelopment(const FMTMask& mask, const int& age, const int& lock);
		// DocString: FMTDevelopment(const FMTMask,const int&,const int&,const int&)
		/**
		@brief Construct a development from a mask, an age, a lock level and a period.
		@param[in] lmask the mask.
		@param[in] lage the age.
		@param[in] llock the lock level.
		@param[in] lperiod the period.
		*/
		FMTDevelopment(const FMTMask& lmask, const int& lage, const int& llock, const int& lperiod);
		// DocString: FMTDevelopment(const FMTDevelopment&)
		/**
		@brief Copy constructor for FMTDevelopment.
		@param[in] rhs the FMTDevelopment to copy.
		*/
		FMTDevelopment(const FMTDevelopment& rhs);
		// DocString: FMTDevelopment::Clone()
		/**
		@brief Virtual clone function for FMTDevelopment.
		@return a unique pointer to the cloned development.
		*/
		virtual std::unique_ptr<FMTDevelopment> Clone() const;
		// DocString: FMTDevelopment::operator=
		/**
		@brief Copy assignment operator for FMTDevelopment.
		@param[in] rhs the FMTDevelopment to copy.
		@return a reference to this FMTDevelopment.
		*/
        FMTDevelopment& operator = (const FMTDevelopment& rhs);
		// DocString: FMTDevelopment::operator==
		/**
		@brief Equality comparison operator of FMTDevelopment.
		@param[in] rhs the development to compare with.
		@return true if both developments are equal else false.
		*/
        bool operator == (const FMTDevelopment& rhs) const;
		// DocString: FMTDevelopment::operator!=
		/**
		@brief Inequality comparison operator of FMTDevelopment.
		@param[in] rhs the development to compare with.
		@return true if both developments are different else false.
		*/
        bool operator != (const FMTDevelopment& rhs) const;
		// DocString: FMTDevelopment::operator<
		/**
		@brief Less than comparison operator of FMTDevelopment.
		@param[in] rhs the development to compare with.
		@return true if this development is less than rhs else false.
		*/
		bool operator < (const FMTDevelopment& rhs) const;
		// DocString: FMTDevelopment::grow
		/**
		@brief Return a grown copy of this development, increasing the age and period and decreasing the lock level if it was greater than 0.
		@return the grown development.
		*/
        FMTFuturDevelopment grow() const;
		// DocString: FMTDevelopment::getYieldRequest
		/**
		@brief Get a yield request for this development, without reference to a graph unless one is provided.
		@param[in] graphyieldrequest an optional graph vertex to yield.
		@return the yield request.
		*/
		FMTYieldRequest getYieldRequest(const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::operable
		/**
		@brief Check if this development is operable to an action based on multiple yields.
		@param[in] action the action.
		@param[in] ylds the yields.
		@param[in] graphyieldrequest an optional graph vertex to yield.
		@return true if the development is operable to the action else false.
		*/
        bool operable(const FMTAction& action,const Core::FMTYields& ylds,
			const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::anyOperable
		/**
		@brief Check if this development is operable to any of the actions based on multiple yields.
		@param[in] actions the actions.
		@param[in] ylds the yields.
		@param[in] graphyieldrequest an optional graph vertex to yield.
		@return true if the development is operable to any action else false.
		*/
		bool anyOperable(const std::vector<const FMTAction*>& actions, const Core::FMTYields& ylds,
			const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::operator std::string
		/**
		@brief Return the string representation of the development as in the area section.
		@return the string representation of the development.
		*/
        virtual operator std::string() const;
		// DocString: FMTDevelopment::getArea
		/**
		@brief Return the area of the development.
		@return the area of the development.
		*/
		virtual double getArea() const;
		// DocString: FMTDevelopment::clearLock
		/**
		@brief Return a copy of this development with the lock level set to 0.
		@return the development with a cleared lock.
		*/
		FMTDevelopment clearLock() const;
		// DocString: FMTDevelopment::operate
		/**
		@brief Return the development paths resulting from operating this development with an action and a transition based on yields and sorted themes.
		@param[in] action the action.
		@param[in] Transition the transition.
		@param[in] ylds the yields.
		@param[in] themes the sorted themes.
		@return the resulting development paths.
		*/
		std::vector<FMTDevelopmentPath> operate(const FMTAction& action,
			const FMTTransition& Transition,
			const Core::FMTYields& ylds,
			const std::vector<FMTTheme>& themes) const;
		// DocString: FMTDevelopment::getInventoryCoef
		/**
		@brief Return the inventory coefficient of the development for a given yield based on the yields.
		@param[in] ylds the yields.
		@param[in] target_yield the target yield.
		@param[in] graphyieldrequest an optional graph vertex to yield.
		@return the inventory coefficient.
		*/
		double getInventoryCoef(const Core::FMTYields& ylds,const std::string& target_yield,
			const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::getHarvestCoef
		/**
		@brief Return the harvest coefficient of the development for a given yield following multiple development paths for an action based on yields.
		@param[in] topaths the development paths.
		@param[in] action the action.
		@param[in] ylds the yields.
		@param[in] target_yield the target yield.
		@param[in] graphyieldrequest an optional graph vertex to yield.
		@return the harvest coefficient.
		*/
		double getHarvestCoef(const std::vector<FMTDevelopmentPath>& topaths,
			const FMTAction& action,const Core::FMTYields& ylds,const std::string& target_yield,
			const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::is
		/**
		@brief Check if this development respects a specification based on multiple yields.
		@param[in] specification the specification.
		@param[in] ylds the yields.
		@param[in] graphyieldrequest an optional graph vertex to yield.
		@return true if the development respects the specification else false.
		*/
		bool is(const FMTSpec& specification, const Core::FMTYields& ylds,
			const Graph::FMTGraphVertexToYield* graphyieldrequest = nullptr) const;
		// DocString: FMTDevelopment::worthTestingOperability
		/**
		@brief Return true if it is worth testing operability for the action, used for optimization.
		@param[in] action the action.
		@return true if it is worth testing operability else false.
		*/
		bool worthTestingOperability(const FMTAction& action) const;
		// DocString: FMTDevelopment::anyWorthTestingOperability
		/**
		@brief Return the pointer distances from the first action of the actions worth testing for operability.
		@param[in] actions the actions.
		@param[in] firstaction the first action.
		@return the distances of the actions worth testing.
		*/
		std::vector<int> anyWorthTestingOperability(const std::vector<const FMTAction*>& actions, const FMTAction& firstaction) const;
		// DocString: FMTDevelopment::isAnyWorthTestingOperability
		/**
		@brief Return a vector of booleans, true if the corresponding action is worth testing for operability.
		@param[in] actions the actions.
		@param[in] allactions all the actions.
		@return a vector of booleans, true if worth testing operability.
		*/
		std::vector<bool> isAnyWorthTestingOperability(const std::vector<const FMTAction*>& actions,
			const std::vector<FMTAction>& allactions) const noexcept;
		// DocString: FMTDevelopment::hash
		/**
		@brief Return the hash of the development combining its mask, age, lock and period.
		@return the hash value.
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
			@brief Serialize the FMTDevelopment through its base FMTObject for multiprocessing across multiple cpus (pickle in Python).
			@tparam Archive the archive type.
			@param[in,out] ar the archive to serialize to or from.
			@param[in] version the serialization version.
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
