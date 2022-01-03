/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTobject.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/property_tree/json_parser.hpp>

#ifdef FMTWITHONNXR
#include  <onnxruntime/core/session/onnxruntime_cxx_api.h>
#endif


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
	private:
		std::string modelname;
		std::unique_ptr<Ort::Env> envPtr = NULL;
		std::unique_ptr<Ort::Session> sessionPtr = NULL;
		std::unique_ptr<Ort::AllocatorWithDefaultOptions> allocatorPtr = NULL;
		std::vector<double> standardParamMeans = {};
		std::vector<double> standardParamVars = {};
		std::vector<const char*> inputNames = {};
		std::vector<const char*> outputNames = {};
	public:
		FMTyieldmodel()=default;
		FMTyieldmodel(const FMTyieldmodel & rhs);
		FMTyieldmodel& operator = (const FMTyieldmodel& rhs) = default;
		virtual ~FMTyieldmodel() = default;
		FMTyieldmodel(const boost::property_tree::ptree & jsonProps);
		const std::string& GetModelName() const;
		const std::vector<double>& GetStandardParamMeans() const;
		const std::vector<double>& GetStandardParamVars() const;
		const std::vector<const char*>& GetInputNames() const;
		const std::vector<const char*>& GetOutputNames() const;
		const std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str);
		void standardize(std::vector<double>& input, const std::vector<double>& means, const std::vector<double>& vars) const;
		virtual std::unique_ptr<FMTyieldmodel>Clone() const;
		virtual std::string GetModelInfo() const;
		virtual bool Validate(const std::vector<std::string>& YieldsAvailable) const;
		virtual std::vector<std::string>GetYieldsOutputs() const;
		virtual std::vector<double>Predict(const Core::FMTyieldrequest& request) const;
	};
}


#endif
