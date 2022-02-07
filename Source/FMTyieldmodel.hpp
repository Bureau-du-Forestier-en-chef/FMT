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

//namespace Ort
//{
//	class Env;
//}

namespace Graph
{
	class FMTcarbonpredictor;
}

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
	protected:
		static std::unique_ptr<Ort::Env> envPtr;
		static const float UNKNOWN_DISTURBANCE_CODE;
		const std::string JSON_PROP_MODEL_NAME = "modelFileName";
		const std::string JSON_PROP_MODEL_TYPE = "modelType";
		const std::string JSON_PROP_MODEL_YIELDS = "modelYields";
		const std::string JSON_PROP_STAND_FILE_PATH = "csvStandardisationFile";
	public:
		static const std::vector<std::string> GetNextLineAndSplitIntoTokens(std::istream& str);
		static const std::vector<float> Standardize(std::vector<float>& input, const std::vector<float>& means, const std::vector<float>& vars);
		static const void RemoveNans(std::vector<float>& input);
		const std::vector<double>Predict(const Core::FMTyieldrequest& request) const;
		virtual const std::string& GetModelName() const = 0;
		virtual const std::string& GetModelType() const = 0;
		virtual const std::vector<float>& GetStandardParamMeans() const = 0;
		virtual const std::vector<float>& GetStandardParamVars() const = 0;
		virtual const std::vector<std::string>& GetModelYields() const = 0;
		virtual std::unique_ptr<FMTyieldmodel>Clone() const = 0;
		virtual const std::string GetModelInfo() const = 0;
		virtual bool Validate(const std::vector<std::string>& YieldsAvailable) const = 0;
		virtual const std::vector<std::string>GetYieldsOutputs() const = 0;
		virtual const std::vector<double> GetInputValues(const Graph::FMTcarbonpredictor& predictor) const = 0;
	};
}


#endif
