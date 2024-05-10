/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTactualdevelopment_H_INCLUDED
#define FMTactualdevelopment_H_INCLUDED

#include "FMTdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Core
{
class FMTlifespans;
// DocString: FMTactualdevelopment
/**
FMTactualdevelopement is the child class of FMTdevelopement. It stands for the representation of an
actual forest stand so it holds the amount of area of this stand as a member variable.
*/
class FMTEXPORT FMTactualdevelopment : public FMTdevelopment
	{
	
	public:
		// DocString: FMTactualdevelopment::(FMTactualdevelopment&& rhs)
		/**
		@brief Move copy constructor
		@param[in] rhs development to swap
		*/
		FMTactualdevelopment(FMTactualdevelopment&& rhs);
		// DocString: FMTactualdevelopment::operator=(FMTactualdevelopment&& rhs)
		/**
		@brief Move copy assignement
		@param[in] rhs development to swap
		*/
		FMTactualdevelopment& operator=(FMTactualdevelopment&& rhs);
		// DocString: FMTactualdevelopment()
		/**
		Default constructor for FMTactualdevelopement
		*/
		FMTactualdevelopment();
		// DocString: ~FMTactualdevelopment()
		/**
		Default destructor for FMTactualdevelopement.
		*/
		~FMTactualdevelopment()=default;
		// DocString: FMTactualdevelopment(const FMTactualdevelopment&)
		/**
		FMTactualdevelopement copy constructor.
		*/
		FMTactualdevelopment(const FMTactualdevelopment& rhs);
		// DocString: FMTactualdevelopment(const FMTmask&,const int&,const int&,const int&,const double&)
		/**
		FMTactualdevelopement constructor using a FMTmask the age and the lock level period  and the area of the FMTactualdevelopment.
		*/
		FMTactualdevelopment(const FMTmask& mask, const int& age, const int& lock, const double& area);
		// DocString: FMTactualdevelopment(const FMTdevelopment&,double)
		/**
		FMTactualdevelopement constructor using a FMTdevelopment and an area.
		*/
		FMTactualdevelopment(const FMTdevelopment& rhs, double larea);
		// DocString: FMTactualdevelopment::operator=
		/**
		FMTactualdevelopement copy assignment.
		*/
		FMTactualdevelopment& operator = (const FMTactualdevelopment& rhs);
		// DocString: FMTactualdevelopment::operator==
		/**
		FMTactualdevelopement equality operator.
		*/
		bool operator == (const FMTactualdevelopment& rhs) const;
		// DocString: FMTactualdevelopment::operator!=
		/**
		FMTactualdevelopement nonequality operator.
		*/
		bool operator != (const FMTactualdevelopment& rhs) const;
		// DocString: FMTactualdevelopment::operator<
		/**
		FMTactualdevelopment less than operator.
		*/
		bool operator < (const FMTactualdevelopment& rhs) const;
		// DocString: FMTactualdevelopment::operator std::string
		/**
		Returns the string reprensentation of a FMTactualdevelopment like in the area section.
		*/
		operator std::string() const override;
		// DocString: FMTactualdevelopment::getarea
		/**
		Returns the area of the FMTactualdevelopement.
		*/
		double getarea() const override;
		// DocString: FMTactualdevelopment::setarea
		/**
		Set the newarea has the area of the FMTactualdevelopement.
		*/
		void setarea(const double& newarea);
		// DocString: FMTactualdevelopment::clone
		/**
		Return a unique_ptr of FMTdevelopment of the FMTactualdevelopement.
		*/
		std::unique_ptr<FMTdevelopment> Clone() const override;
		// DocString: FMTactualdevelopment::presolve
		/**
		Will presolve the FMTactualdevelopment using the FMTdevelopement::presolve() function.
		*/
		FMTactualdevelopment presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>&presolvedthemes) const;
		// DocString: FMTactualdevelopment::reducelocktodeath
		/**
		This function check if the actualdevelopement lifespan is shorter than the lock present in area section. If not so, it raise the 
		error FMTdeathwithlock and if the error is set to warning, it returns a FMTactualdevelopment with the lock reduced accordingly.
		If the lifespan is shorter than the lock, it returns a copy of itself. This is mainly use in FMTmodel::setarea.  
		*/
		FMTactualdevelopment reducelocktodeath(const FMTlifespans& lifespans) const;
	private:
		// DocString: FMTactualdevelopment::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {

				ar& boost::serialization::make_nvp("development", boost::serialization::base_object<FMTdevelopment>(*this));
				ar& BOOST_SERIALIZATION_NVP(area);
			}
			catch (...)
			{
				_exhandler->printexceptions("", "FMTactualdevelopment::serialize", __LINE__, __FILE__);
			}
		}
		// DocString: FMTactualdevelopment::area
		///The area of the actual developement.
		double area;
	};

// DocString: FMTactualdevelopmentcomparator
/**
FMTactioncomparator to check if the base FMTdevelopement is the actualdev.
*/
class FMTactualdevelopmentcomparator
	{
		// DocString: FMTactioncomparator::basedev
		///The FMTdevelopement that we compare
		const FMTdevelopment* basedev;
	public:
		// DocString: FMTactualdevelopmentcomparator(const FMTdevelopment*)
		/**
		FMTactualdevelopment constructor with the base FMTdevelopment.
		*/
		FMTactualdevelopmentcomparator(const FMTdevelopment* base);
		// DocString: FMTactualdevelopmentcomparator::operator()(const FMTactualdevelopment&)
		/**
		Matching test operator for FMTactualdevelopmentcomparator.
		*/
		bool operator()(const FMTactualdevelopment& actualdev) const;

	};

}

BOOST_CLASS_EXPORT_KEY(Core::FMTactualdevelopment)
#endif
