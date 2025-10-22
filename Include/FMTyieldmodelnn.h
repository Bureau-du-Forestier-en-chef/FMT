
/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTobject.h"
#include "FMTyieldmodel.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


#ifndef FMTYIELDMODELNN_Hm_included
#define FMTYIELDMODELNN_Hm_included

namespace Ort 
{
	struct Env;
	struct Session;
}


namespace Core 
{
	// DocString: FMTyieldmodelnn
	/**
	FMTyieldmodelnn is an abstract class to be implemented as a machine learning model.
	*/
	class FMTyieldmodelnn : public FMTyieldmodel
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTyieldmodel", boost::serialization::base_object<FMTyieldmodel>(*this));
		}
	protected:
	#ifdef FMTWITHONNXR
		static std::unique_ptr<Ort::Env> envPtr;
		std::unique_ptr<Ort::Session> sessionPtr;
	#endif
		static const float UNKNOWN_DISTURBANCE_CODE;
		const std::string JSON_PROP_MODEL_TYPE = "modelType";
		const std::string JSON_PROP_MODEL_YIELDS = "modelYields";
		const std::string JSON_PROP_MODEL_OUTPUTS = "outputNames";
		const std::string JSON_PROP_STAND_FILE_PATH = "csvStandardisationFile";
		std::string modelType;
		std::vector<float> standardParamMeans = {};
		std::vector<float> standardParamVars = {};
		std::vector<std::string> modelOutputs = {};
		// DocString: FMTyieldmodelnn::GetNextLineAndSplitIntoTokens
		/**
		Utility function used to read a csv file line by line.
		*/
		static const std::vector<std::string> GetNextLineAndSplitIntoTokens(std::istream& str);
		// DocString: FMTyieldmodelnn::Standardize
		/**
		Utility function that applies the standardization formula, a feature scaling technique, on the inputs of a machine learning model.
		*/
		static const std::vector<float> Standardize(std::vector<float>& input, const std::vector<float>& means, const std::vector<float>& vars);
		
		// DocString: FMTyieldmodelnn::ValidateInputYields
		/**
		Validates that there is the expected number of inputs in the model.
		*/
		void ValidateInputYields(std::vector<std::string>& expectedYields, std::vector<std::string>& inputYields) const;
		// DocString: FMTyieldmodelnn::GetModelType()
		/**
		Returns the model type.
		*/
		const std::string& GetModelType() const;
		// DocString: FMTyieldmodelnn::GetStandardParamMeans()
		/**
		Returns input variables' means used in the standartization process when predicting.
		*/
		const std::vector<float>& GetStandardParamMeans() const;
		// DocString: FMTyieldmodelnn::GetStandardParamVars()
		/**
		Returns input variables' variances used in the standartization process when predicting.
		*/
		const std::vector<float>& GetStandardParamVars() const;
		// DocString: FMTyieldmodelnn::GetModelOutputNames()
		/**
		Return model outputs' names.
		*/
		const std::vector<std::string>& GetModelOutputNames() const;
		// DocString: FMTyieldmodelnn::GetInputValues()
		/**
		Returns inputs values based on a predictor passed as parameter.
		*/
		virtual const std::vector<double> GetInputValues(const Graph::FMTpredictor& predictor) const = 0;
		// DocString: FMTyieldmodelnn::RemoveNans
		/**
		Replaces nan values with default values.
		*/
		const void RemoveNans(std::vector<float>& input) const;
		// DocString: FMTyieldmodelnn::FMTyieldmodelpools(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		Construct a FMTyieldmodelnn based on a JSON file and an input yield name list.
		*/
		FMTyieldmodelnn(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields);
	public:
		// DocString: FMTyieldmodelnn::~FMTyieldmodel()
		/**
		Destructor for FMTyieldmodelnn.
		*/
		virtual ~FMTyieldmodelnn();
		// DocString: FMTyieldmodelnn::FMTyieldmodelnn()
		/**
		Constructor for FMTyieldmodelnn.
		*/
		FMTyieldmodelnn()=default;
		// DocString: FMTyieldmodelnn::FMTyieldmodelnn()
		/**
		Copy constructor for FMTyieldmodelnn.
		*/
		FMTyieldmodelnn(const FMTyieldmodelnn& rhs);
		// DocString: FMTyieldmodelnn::Predict
		/**
		Runs the machine learning model to predict its outputs.
		*/
		const std::vector<double>Predict(const Core::FMTyieldrequest& request) const;
	};
}

#endif