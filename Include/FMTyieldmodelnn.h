
/*
Copyright (c) 2019 Gouvernement du Qubec

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
		// DocString: FMTyieldmodelnn::getNextLineAndSplitIntoTokens
		/**
		Utility function used to read a csv file line by line.
		*/
		static const std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str);
		// DocString: FMTyieldmodelnn::standardize
		/**
		Utility function that applies the standardization formula, a feature scaling technique, on the inputs of a machine learning model.
		*/
		static const std::vector<float> standardize(std::vector<float>& input, const std::vector<float>& means, const std::vector<float>& vars);
		
		// DocString: FMTyieldmodelnn::validateInputYields
		/**
		Validates that there is the expected number of inputs in the model.
		*/
		void validateInputYields(std::vector<std::string>& expectedYields, std::vector<std::string>& inputYields) const;
		// DocString: FMTyieldmodelnn::getModelType()
		/**
		Returns the model type.
		*/
		const std::string& getModelType() const;
		// DocString: FMTyieldmodelnn::getStandardParamMeans()
		/**
		Returns input variables' means used in the standartization process when predicting.
		*/
		const std::vector<float>& getStandardParamMeans() const;
		// DocString: FMTyieldmodelnn::getStandardParamVars()
		/**
		Returns input variables' variances used in the standartization process when predicting.
		*/
		const std::vector<float>& getStandardParamVars() const;
		// DocString: FMTyieldmodelnn::getModelOutputNames()
		/**
		Return model outputs' names.
		*/
		const std::vector<std::string>& getModelOutputNames() const;
		// DocString: FMTyieldmodelnn::getInputValues()
		/**
		Returns inputs values based on a predictor passed as parameter.
		*/
		virtual const std::vector<double> getInputValues(const Graph::FMTpredictor& predictor) const = 0;
		// DocString: FMTyieldmodelnn::removeNans
		/**
		Replaces nan values with default values.
		*/
		const void removeNans(std::vector<float>& input) const;
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
		// DocString: FMTyieldmodelnn::predict
		/**
		Runs the machine learning model to predict its outputs.
		*/
		const std::vector<double>predict(const Core::FMTyieldrequest& request) const;
	};
}

#endif