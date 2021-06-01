/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTfuturdevelopment_H_INCLUDED
#define FMTfuturdevelopment_H_INCLUDED

#include "FMTdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <memory>
#include <boost/serialization/export.hpp>

namespace Core
{
	class FMTEXPORT FMTfuturdevelopment : public FMTdevelopment
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			try {
			ar & boost::serialization::make_nvp("development", boost::serialization::base_object<FMTdevelopment>(*this));
			}catch (...)
			{
			_exhandler->printexceptions("", "FMTfuturdevelopment::serialize", __LINE__, __FILE__);
			}
		}
		public:
			FMTfuturdevelopment();
			~FMTfuturdevelopment() = default;
			FMTfuturdevelopment(const FMTdevelopment& dev);
			FMTfuturdevelopment(const FMTfuturdevelopment& rhs);
			bool operator == (const FMTfuturdevelopment& rhs) const;
			FMTfuturdevelopment& operator = (const FMTfuturdevelopment& rhs);
			std::unique_ptr<FMTdevelopment> Clone() const override;
			double getarea() const override;

		};
}

BOOST_CLASS_EXPORT_KEY(Core::FMTfuturdevelopment)
#endif // FMTDEV_H_INCLUDED