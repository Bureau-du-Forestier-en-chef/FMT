/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTobject.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


#ifndef FMTYIELDMODEL_H_INCLUDED
#define FMTYIELDMODEL_H_INCLUDED



namespace Core 
{
	class FMTyieldrequest;
	class FMTyieldmodel : public FMTobject
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
			ar & modelname;
		}
		std::string modelname;
	public:
		FMTyieldmodel()=default;
		FMTyieldmodel(const FMTyieldmodel& rhs) = default;
		FMTyieldmodel& operator = (const FMTyieldmodel& rhs) = default;
		virtual ~FMTyieldmodel() = default;
		FMTyieldmodel(const std::string& themodelname);
		const std::string& GetModelName() const;
		virtual std::unique_ptr<FMTyieldmodel>Clone() const;
		virtual std::string GetModelInfo() const;
		virtual bool Validate(const std::vector<std::string>& YieldsAvailable) const;
		virtual std::vector<std::string>GetYieldsOutputs() const;
		virtual std::vector<double>Predict(const Core::FMTyieldrequest& request) const;
	};
}


#endif