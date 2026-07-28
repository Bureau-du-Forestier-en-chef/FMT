/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTactualdevelopment_Hm_included
#define FMTactualdevelopment_Hm_included

#include "FMTDevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Core
{
class FMTLifespans;
// DocString: FMTActualDevelopment
/**
@brief Child class of FMTDevelopment representing an actual forest stand, holding the area of the stand.
*/
class FMTEXPORT FMTActualDevelopment : public FMTDevelopment
	{
	
	public:
		// DocString: FMTActualDevelopment::(FMTActualDevelopment&& rhs)
		/**
		@brief Move copy constructor
		@param[in] rhs development to swap
		*/
		FMTActualDevelopment(FMTActualDevelopment&& rhs);
		// DocString: FMTActualDevelopment::operator=(FMTActualDevelopment&& rhs)
		/**
		@brief Move assignment operator for FMTActualDevelopment.
		@param[in] rhs the development to move from.
		@return a reference to this FMTActualDevelopment.
		*/
		FMTActualDevelopment& operator=(FMTActualDevelopment&& rhs);
		// DocString: FMTActualDevelopment()
		/**
		@brief Default constructor for FMTActualDevelopment.
		*/
		FMTActualDevelopment();
		// DocString: ~FMTActualDevelopment()
		/**
		@brief Default destructor for FMTActualDevelopment.
		*/
		~FMTActualDevelopment()=default;
		// DocString: FMTActualDevelopment(const FMTActualDevelopment&)
		/**
		@brief Copy constructor for FMTActualDevelopment.
		@param[in] rhs the FMTActualDevelopment to copy.
		*/
		FMTActualDevelopment(const FMTActualDevelopment& rhs);
		// DocString: FMTActualDevelopment(const FMTMask&,const int&,const int&,const int&,const double&)
		/**
		@brief Construct an actual development from a mask, an age, a lock level and an area.
		@param[in] mask the mask.
		@param[in] age the age.
		@param[in] lock the lock level.
		@param[in] area the area.
		*/
		FMTActualDevelopment(const FMTMask& mask, const int& age, const int& lock, const double& area);
		// DocString: FMTActualDevelopment(const FMTDevelopment&,double)
		/**
		@brief Construct an actual development from a development and an area.
		@param[in] rhs the development.
		@param[in] larea the area.
		*/
		FMTActualDevelopment(const FMTDevelopment& rhs, double larea);
		// DocString: FMTActualDevelopment::operator=
		/**
		@brief Copy assignment operator for FMTActualDevelopment.
		@param[in] rhs the FMTActualDevelopment to copy.
		@return a reference to this FMTActualDevelopment.
		*/
		FMTActualDevelopment& operator = (const FMTActualDevelopment& rhs);
		// DocString: FMTActualDevelopment::operator==
		/**
		@brief Equality comparison operator of FMTActualDevelopment.
		@param[in] rhs the development to compare with.
		@return true if both developments are equal else false.
		*/
		bool operator == (const FMTActualDevelopment& rhs) const;
		// DocString: FMTActualDevelopment::operator!=
		/**
		@brief Inequality comparison operator of FMTActualDevelopment.
		@param[in] rhs the development to compare with.
		@return true if both developments are different else false.
		*/
		bool operator != (const FMTActualDevelopment& rhs) const;
		// DocString: FMTActualDevelopment::operator<
		/**
		@brief Less than comparison operator of FMTActualDevelopment.
		@param[in] rhs the development to compare with.
		@return true if this development is less than rhs else false.
		*/
		bool operator < (const FMTActualDevelopment& rhs) const;
		// DocString: FMTActualDevelopment::operator std::string
		/**
		@brief Return the string representation of the actual development as in the area section.
		@return the string representation of the actual development.
		*/
		operator std::string() const override;
		// DocString: FMTActualDevelopment::getArea
		/**
		@brief Return the area of the actual development.
		@return the area of the actual development.
		*/
		double getArea() const override;
		// DocString: FMTActualDevelopment::setArea
		/**
		@brief Set the area of the actual development.
		@param[in] newarea the area to set.
		*/
		void setArea(const double& newarea);
		// DocString: FMTActualDevelopment::clone
		/**
		@brief Return a unique pointer of FMTDevelopment cloning the actual development.
		@return a unique pointer to the cloned development.
		*/
		std::unique_ptr<FMTDevelopment> Clone() const override;
		// DocString: FMTActualDevelopment::presolve
		/**
		@brief Presolve the actual development using the FMTDevelopment presolve function.
		@param[in] filter the mask filter.
		@param[in] presolvedthemes the presolved themes.
		@return the presolved actual development.
		*/
		FMTActualDevelopment presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&presolvedthemes) const;
		// DocString: FMTActualDevelopment::reduceLockToDeath
		/**
		@brief Reduce the lock of the actual development to its lifespan if the lifespan is shorter than the lock.
		@details Raises FMTdeathwithlock; if the error is set to warning, returns an actual development with the lock reduced accordingly, otherwise returns a copy of itself. Mainly used in FMTModel::setArea.
		@param[in] lifespans the lifespans.
		@return the actual development with the lock reduced to death.
		*/
		FMTActualDevelopment reduceLockToDeath(const FMTLifespans& lifespans) const;
	private:
		// DocString: FMTActualDevelopment::serialize
		/**
		@brief Serialize the FMTActualDevelopment through its base FMTDevelopment for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {

				ar& boost::serialization::make_nvp("development", boost::serialization::base_object<FMTDevelopment>(*this));
				ar& BOOST_SERIALIZATION_NVP(area);
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTActualDevelopment::serialize", __LINE__, __FILE__);
			}
		}
		// DocString: FMTActualDevelopment::area
		///The area of the actual development.
		double area;
	};

// DocString: FMTActualDevelopmentComparator
/**
@brief Comparator used to check if a base development is the actual development.
*/
class FMTActualDevelopmentComparator
	{
	public:
		// DocString: FMTActualDevelopmentComparator(const FMTDevelopment*)
		/**
		@brief Construct the comparator from the base development.
		@param[in] base the base development to match.
		*/
		FMTActualDevelopmentComparator(const FMTDevelopment* base);
		// DocString: FMTActualDevelopmentComparator::operator()(const FMTActualDevelopment&)
		/**
		@brief Matching test operator for the comparator.
		@param[in] actualdev the actual development to test.
		@return true if the actual development matches else false.
		*/
		bool operator()(const FMTActualDevelopment& actualdev) const;

	private:
		// DocString: FMTActualDevelopmentComparator::m_basedev
		///The FMTDevelopment that we compare
		const FMTDevelopment* m_basedev;
	};

}

BOOST_CLASS_EXPORT_KEY(Core::FMTActualDevelopment)
#endif
