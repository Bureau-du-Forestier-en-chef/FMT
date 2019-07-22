#ifndef FMTactualdevelopment_H_INCLUDED
#define FMTactualdevelopment_H_INCLUDED

#include "FMTdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
namespace Core

{
class FMTactualdevelopment : public FMTdevelopment
	{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		//ar.template register_type<FMTdevelopment>();
		//ar & boost::serialization::make_nvp("development", BOOST_SERIALIZATION_BASE_OBJECT_NVP(FMTdevelopment));
		ar & boost::serialization::make_nvp("development", boost::serialization::base_object<FMTdevelopment>(*this));
		ar & BOOST_SERIALIZATION_NVP(area);
	}
	public:
		double area;
		FMTactualdevelopment();
		~FMTactualdevelopment()=default;
		FMTactualdevelopment(const FMTactualdevelopment& rhs);
		FMTactualdevelopment(FMTmask mask, int age, int lock, double area);
		FMTactualdevelopment(const FMTdevelopment& rhs, double larea);
		FMTactualdevelopment& operator = (const FMTactualdevelopment& rhs);
		bool operator == (const FMTactualdevelopment& rhs) const;
		bool operator != (const FMTactualdevelopment& rhs) const;
		bool operator < (const FMTactualdevelopment& rhs) const;
		operator string() const override;
		double getarea() const override;
		unique_ptr<FMTdevelopment> Clone() const override;
	};
}

#endif
