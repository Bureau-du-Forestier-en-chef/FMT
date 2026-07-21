
/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTObject.h"
#include "FMTYieldModel.h"
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
	// DocString: FMTYieldModelNn
	/**
	@brief Abstract machine learning yield model based on a neural network.
	*/
	class FMTYieldModelNn : public FMTYieldModel
	{
		friend class boost::serialization::access;
		// DocString: FMTYieldModelNn::serialize
		/**
		@brief Serialize the FMTYieldModelNn through its base FMTYieldModel for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTyieldmodel", boost::serialization::base_object<FMTYieldModel>(*this));
		}
	public:
		// DocString: FMTYieldModelNn::~FMTYieldModel()
		/**
		@brief Destructor for FMTYieldModelNn.
		*/
		virtual ~FMTYieldModelNn();
		// DocString: FMTYieldModelNn::FMTYieldModelNn()
		/**
		@brief Default constructor for FMTYieldModelNn.
		*/
		FMTYieldModelNn()=default;
		// DocString: FMTYieldModelNn::FMTYieldModelNn()
		/**
		@brief Copy constructor for FMTYieldModelNn.
		@param[in] rhs the FMTYieldModelNn to copy.
		*/
		FMTYieldModelNn(const FMTYieldModelNn& rhs);
		// DocString: FMTYieldModelNn::predict
		/**
		@brief Run the machine learning model to predict its outputs for a request.
		@param[in] request the yield request.
		@return the predicted values.
		*/
		const std::vector<double>predict(const Core::FMTYieldRequest& request) const;
	protected:
	#ifdef FMTWITHONNXR
		static std::unique_ptr<Ort::Env> m_envPtr;
		std::unique_ptr<Ort::Session> m_sessionPtr;
	#endif
		static const float m_UNKNOWN_DISTURBANCE_CODE;
		const std::string m_JSON_PROP_MODEL_TYPE = "modelType";
		const std::string m_JSON_PROP_MODEL_YIELDS = "modelYields";
		const std::string m_JSON_PROP_MODEL_OUTPUTS = "outputNames";
		const std::string m_JSON_PROP_STAND_FILE_PATH = "csvStandardisationFile";
		std::string m_modelType;
		std::vector<float> m_standardParamMeans = {};
		std::vector<float> m_standardParamVars = {};
		std::vector<std::string> m_modelOutputs = {};
		// DocString: FMTYieldModelNn::_getNextLineAndSplitIntoTokens
		/**
		@brief Read a CSV file line by line, splitting into tokens.
		@param[in,out] str the input stream.
		@return the tokens of the line.
		*/
		static const std::vector<std::string> _getNextLineAndSplitIntoTokens(std::istream& str);
		// DocString: FMTYieldModelNn::_standardize
		/**
		@brief Apply the standardization feature scaling to the inputs of a machine learning model.
		@param[in,out] input the input values.
		@param[in] means the means.
		@param[in] vars the variances.
		@return the standardized values.
		*/
		static const std::vector<float> _standardize(std::vector<float>& input, const std::vector<float>& means, const std::vector<float>& vars);
		
		// DocString: FMTYieldModelNn::_validateInputYields
		/**
		@brief Validate that there is the expected number of inputs in the model.
		@param[in,out] expectedYields the expected yields.
		@param[in,out] inputYields the input yields.
		*/
		void _validateInputYields(std::vector<std::string>& expectedYields, std::vector<std::string>& inputYields) const;
		// DocString: FMTYieldModelNn::getModelType()
		/**
		@brief Return the model type.
		@return the model type.
		*/
		const std::string& getModelType() const;
		// DocString: FMTYieldModelNn::_getStandardParamMeans()
		/**
		@brief Return the input variable means used in the standardization process.
		@return the standard parameter means.
		*/
		const std::vector<float>& _getStandardParamMeans() const;
		// DocString: FMTYieldModelNn::_getStandardParamVars()
		/**
		@brief Return the input variable variances used in the standardization process.
		@return the standard parameter variances.
		*/
		const std::vector<float>& _getStandardParamVars() const;
		// DocString: FMTYieldModelNn::getModelOutputNames()
		/**
		@brief Return the model output names.
		@return the model output names.
		*/
		const std::vector<std::string>& getModelOutputNames() const;
		// DocString: FMTYieldModelNn::getInputValues()
		/**
		@brief Return the input values based on a predictor.
		@param[in] predictor the predictor.
		@return the input values.
		*/
		virtual const std::vector<double> getInputValues(const Graph::FMTPredictor& predictor) const = 0;
		// DocString: FMTYieldModelNn::removeNans
		/**
		@brief Replace nan values with default values.
		@param[in,out] input the input values.
		*/
		const void removeNans(std::vector<float>& input) const;
		// DocString: FMTYieldModelNn::FMTYieldModelPools(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		@brief Construct a FMTYieldModelNn from a JSON tree and an input yield name list.
		@param[in] jsonProps the JSON properties.
		@param[in,out] inputYields the input yield names.
		*/
		FMTYieldModelNn(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields);
	};
}

#endif