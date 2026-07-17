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
FMTActualDevelopment is the child class of FMTDevelopment. It stands for the representation of an
actual forest stand so it holds the amount of area of this stand as a member variable.
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
		@brief Move copy assignement
		@param[in] rhs development to swap
		*/
		FMTActualDevelopment& operator=(FMTActualDevelopment&& rhs);
		// DocString: FMTActualDevelopment()
		/**
		Default constructor for FMTActualDevelopment
		*/
		FMTActualDevelopment();
		// DocString: ~FMTActualDevelopment()
		/**
		Default destructor for FMTActualDevelopment.
		*/
		~FMTActualDevelopment()=default;
		// DocString: FMTActualDevelopment(const FMTActualDevelopment&)
		/**
		FMTActualDevelopment copy constructor.
		*/
		FMTActualDevelopment(const FMTActualDevelopment& rhs);
		// DocString: FMTActualDevelopment(const FMTMask&,const int&,const int&,const int&,const double&)
		/**
		FMTActualDevelopment constructor using a FMTMask the age and the lock level period  and the area of the FMTActualDevelopment.
		*/
		FMTActualDevelopment(const FMTMask& mask, const int& age, const int& lock, const double& area);
		// DocString: FMTActualDevelopment(const FMTDevelopment&,double)
		/**
		FMTActualDevelopment constructor using a FMTDevelopment and an area.
		*/
		FMTActualDevelopment(const FMTDevelopment& rhs, double larea);
		// DocString: FMTActualDevelopment::operator=
		/**
		FMTActualDevelopment copy assignment.
		*/
		FMTActualDevelopment& operator = (const FMTActualDevelopment& rhs);
		// DocString: FMTActualDevelopment::operator==
		/**
		FMTActualDevelopment equality operator.
		*/
		bool operator == (const FMTActualDevelopment& rhs) const;
		// DocString: FMTActualDevelopment::operator!=
		/**
		FMTActualDevelopment nonequality operator.
		*/
		bool operator != (const FMTActualDevelopment& rhs) const;
		// DocString: FMTActualDevelopment::operator<
		/**
		FMTActualDevelopment less than operator.
		*/
		bool operator < (const FMTActualDevelopment& rhs) const;
		// DocString: FMTActualDevelopment::operator std::string
		/**
		Returns the string reprensentation of a FMTActualDevelopment like in the area section.
		*/
		operator std::string() const override;
		// DocString: FMTActualDevelopment::getArea
		/**
		Returns the area of the FMTActualDevelopment.
		*/
		double getArea() const override;
		// DocString: FMTActualDevelopment::setArea
		/**
		Set the newarea has the area of the FMTActualDevelopment.
		*/
		void setArea(const double& newarea);
		// DocString: FMTActualDevelopment::clone
		/**
		Return a unique_ptr of FMTDevelopment of the FMTActualDevelopment.
		*/
		std::unique_ptr<FMTDevelopment> Clone() const override;
		// DocString: FMTActualDevelopment::presolve
		/**
		Will presolve the FMTActualDevelopment using the FMTDevelopment::presolve() function.
		*/
		FMTActualDevelopment presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&presolvedthemes) const;
		// DocString: FMTActualDevelopment::reduceLockToDeath
		/**
		This function check if the actualdevelopment lifespan is shorter than the lock present in area section. If not so, it raise the 
		error FMTdeathwithlock and if the error is set to warning, it returns a FMTActualDevelopment with the lock reduced accordingly.
		If the lifespan is shorter than the lock, it returns a copy of itself. This is mainly use in FMTModel::setArea.  
		*/
		FMTActualDevelopment reduceLockToDeath(const FMTLifespans& lifespans) const;
	private:
		// DocString: FMTActualDevelopment::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
FMTActionComparator to check if the base FMTDevelopment is the actualdev.
*/
class FMTActualDevelopmentComparator
	{
		// DocString: FMTActionComparator::basedev
		///The FMTDevelopment that we compare
		const FMTDevelopment* basedev;
	public:
		// DocString: FMTActualDevelopmentComparator(const FMTDevelopment*)
		/**
		FMTActualDevelopment constructor with the base FMTDevelopment.
		*/
		FMTActualDevelopmentComparator(const FMTDevelopment* base);
		// DocString: FMTActualDevelopmentComparator::operator()(const FMTActualDevelopment&)
		/**
		Matching test operator for FMTActualDevelopmentComparator.
		*/
		bool operator()(const FMTActualDevelopment& actualdev) const;

	};

}

BOOST_CLASS_EXPORT_KEY(Core::FMTActualDevelopment)
#endif
