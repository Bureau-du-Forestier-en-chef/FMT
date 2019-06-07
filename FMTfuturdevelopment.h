#ifndef FMTfuturdevelopment_H_INCLUDED
#define FMTfuturdevelopment_H_INCLUDED

#include "FMTdevelopment.h"
#include <boost/serialization/serialization.hpp>


namespace Core
{
	class FMTfuturdevelopment : public FMTdevelopment
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("development", boost::serialization::base_object<FMTdevelopment>(*this));
		}
		public:
			FMTfuturdevelopment();
			FMTfuturdevelopment(const FMTdevelopment& dev);
			FMTfuturdevelopment(const FMTfuturdevelopment& rhs);
			bool operator == (const FMTfuturdevelopment& rhs) const;
			FMTfuturdevelopment& operator = (const FMTfuturdevelopment& rhs);
			unique_ptr<FMTdevelopment> Clone() const override;
			double getarea() const override;

		};
}

#endif // FMTDEV_H_INCLUDED