/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTobject.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


#ifndef FMTYIELDMODEL_H_INCLUDED
#define FMTYIELDMODEL_H_INCLUDED

namespace Graph
{
	class FMTgraphvertextoyield;
	class FMTgraphpredictor;
}



namespace Core 
{
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
		FMTyieldmodel() = default;
		FMTyieldmodel(const FMTyieldmodel& rhs) = default;
		FMTyieldmodel& operator = (const FMTyieldmodel& rhs) = default;
		virtual ~FMTyieldmodel() = default;
		std::string GetModelName() const;
		FMTyieldmodel(const std::string& name);
		virtual std::unique_ptr<FMTyieldmodel>Clone() const;
		virtual std::string GetModelInfo() const;
		virtual bool Validate(const std::vector<std::string>& YieldsAvailable) const;
		virtual std::vector<std::string>GetYieldsOutputs() const;
		virtual std::vector<double>Predict(const Graph::FMTgraphvertextoyield& graphinfo,
											const std::vector<double>& sourceyieldvalues) const;
	};
}


#endif
