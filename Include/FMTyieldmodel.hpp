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

namespace Graph
{
	class FMTpredictor;
}

namespace Core 
{
	class FMTyieldrequest;

	// DocString: FMTyieldmodel
	/**
	FMTyieldmodel is an abstract class to be implemented as a machine learning model.
	*/
	class FMTyieldmodel : public FMTobject
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
		}
	protected:

	#ifdef FMTWITHONNXR
		static std::unique_ptr<Ort::Env> envPtr;
		std::unique_ptr<Ort::Session> sessionPtr;
	#endif

		static const float UNKNOWN_DISTURBANCE_CODE;
		const std::string JSON_PROP_MODEL_NAME = "modelFileName";
		const std::string JSON_PROP_MODEL_TYPE = "modelType";
		const std::string JSON_PROP_MODEL_YIELDS = "modelYields";
		const std::string JSON_PROP_MODEL_OUTPUTS = "outputNames";
		const std::string JSON_PROP_STAND_FILE_PATH = "csvStandardisationFile";
	public:
		// DocString: FMTyieldmodel::GetNextLineAndSplitIntoTokens
		/**
		Utility function used to read a csv file line by line.
		*/
		static const std::vector<std::string> GetNextLineAndSplitIntoTokens(std::istream& str);
		// DocString: FMTyieldmodel::Standardize
		/**
		Utility function that applies the standardization formula, a feature scaling technique, on the inputs of a machine learning model.
		*/
		static const std::vector<float> Standardize(std::vector<float>& input, const std::vector<float>& means, const std::vector<float>& vars);
		// DocString: FMTyieldmodel::Predict
		/**
		Runs the machine learning model to predict its outputs.
		*/
		const std::vector<double>Predict(const Core::FMTyieldrequest& request) const;
		// DocString: FMTyieldmodel::ValidateInputYields
		/**
		Validates that there is the expected number of inputs in the model.
		*/
		void ValidateInputYields(std::vector<std::string>& expectedYields, std::vector<std::string>& inputYields) const;
		// DocString: FMTyieldmodel::~FMTyieldmodel()
		/**
		Default destructor for FMTyieldmodel.
		*/
		virtual ~FMTyieldmodel() = default;
		// DocString: FMTyieldmodel::GetModelName()
		/**
		Returns the model name.
		*/
		virtual const std::string& GetModelName() const = 0;
		// DocString: FMTyieldmodel::GetModelType()
		/**
		Returns the model type.
		*/
		virtual const std::string& GetModelType() const = 0;
		// DocString: FMTyieldmodel::GetStandardParamMeans()
		/**
		Returns input variables' means used in the standartization process when predicting.
		*/
		virtual const std::vector<float>& GetStandardParamMeans() const = 0;
		// DocString: FMTyieldmodel::GetStandardParamVars()
		/**
		Returns input variables' variances used in the standartization process when predicting.
		*/
		virtual const std::vector<float>& GetStandardParamVars() const = 0;
		// DocString: FMTyieldmodel::GetModelYields()
		/**
		Return model yields' names.
		*/
		virtual const std::vector<std::string>& GetModelYields() const = 0;
		// DocString: FMTyieldmodel::GetModelOutputNames()
		/**
		Return model outputs' names.
		*/
		virtual const std::vector<std::string>& GetModelOutputNames() const = 0;
		// DocString: FMTyieldmodel::Clone()
		/**
		Creates and returns a copy of the FMTyieldmodel. Calls the copy constructor.
		*/
		virtual std::unique_ptr<FMTyieldmodel>Clone() const = 0;
		// DocString: FMTyieldmodel::GetInputValues()
		/**
		Returns inputs values based on a predictor passed as parameter.
		*/
		virtual const std::vector<double> GetInputValues(const Graph::FMTpredictor& predictor) const = 0;
		// DocString: FMTyieldmodel::RemoveNans
		/**
		Replaces nan values with default values.
		*/
		virtual const void RemoveNans(std::vector<float>& input) const = 0;
	};
}

#endif