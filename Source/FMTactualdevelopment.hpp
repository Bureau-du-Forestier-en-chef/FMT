/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTactualdevelopment_H_INCLUDED
#define FMTactualdevelopment_H_INCLUDED

#include "FMTdevelopment.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Core
{
class FMTlifespans;

class FMTEXPORT FMTactualdevelopment : public FMTdevelopment
	{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		try {

			ar & boost::serialization::make_nvp("development", boost::serialization::base_object<FMTdevelopment>(*this));
			ar & BOOST_SERIALIZATION_NVP(area);
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTactualdevelopment::serialize", __LINE__, __FILE__);
			}
	}
		double area;
	public:
		FMTactualdevelopment();
		~FMTactualdevelopment()=default;
		FMTactualdevelopment(const FMTactualdevelopment& rhs);
		FMTactualdevelopment(const FMTmask& mask, const int& age, const int& lock, const double& area);
		FMTactualdevelopment(const FMTdevelopment& rhs, double larea);
		FMTactualdevelopment& operator = (const FMTactualdevelopment& rhs);
		bool operator == (const FMTactualdevelopment& rhs) const;
		bool operator != (const FMTactualdevelopment& rhs) const;
		bool operator < (const FMTactualdevelopment& rhs) const;
		operator std::string() const override;
		double getarea() const override;
		void setarea(const double& newarea);
		std::unique_ptr<FMTdevelopment> Clone() const override;
		FMTactualdevelopment presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>&presolvedthemes) const;
		// DocString: FMTactualdevelopment::reducelocktodeath
		/**
		This function check if the actualdevelopement lifespan is shorter than the lock present in area section. If not so, it raise the 
		error FMTdeathwithlock and if the error is set to warning, it returns a FMTactualdevelopment with the lock reduced accordingly.
		If the lifespan is shorter than the lock, it returns a copy of itself. This is mainly use in FMTmodel::setarea.  
		*/
		FMTactualdevelopment reducelocktodeath(const FMTlifespans& lifespans) const;
	};

class FMTactualdevelopmentcomparator
	{
		const FMTdevelopment* basedev;
	public:
		FMTactualdevelopmentcomparator(const FMTdevelopment* base);
		bool operator()(const FMTactualdevelopment& actualdev) const;

	};

}

BOOST_CLASS_EXPORT_KEY(Core::FMTactualdevelopment)
#endif
