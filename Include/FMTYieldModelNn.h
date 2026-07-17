
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
	FMTYieldModelNn is an abstract class to be implemented as a machine learning model.
	*/
	class FMTYieldModelNn : public FMTYieldModel
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTyieldmodel", boost::serialization::base_object<FMTYieldModel>(*this));
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
		// DocString: FMTYieldModelNn::getNextLineAndSplitIntoTokens
		/**
		Utility function used to read a csv file line by line.
		*/
		static const std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str);
		// DocString: FMTYieldModelNn::standardize
		/**
		Utility function that applies the standardization formula, a feature scaling technique, on the inputs of a machine learning model.
		*/
		static const std::vector<float> standardize(std::vector<float>& input, const std::vector<float>& means, const std::vector<float>& vars);
		
		// DocString: FMTYieldModelNn::validateInputYields
		/**
		Validates that there is the expected number of inputs in the model.
		*/
		void validateInputYields(std::vector<std::string>& expectedYields, std::vector<std::string>& inputYields) const;
		// DocString: FMTYieldModelNn::getModelType()
		/**
		Returns the model type.
		*/
		const std::string& getModelType() const;
		// DocString: FMTYieldModelNn::getStandardParamMeans()
		/**
		Returns input variables' means used in the standartization process when predicting.
		*/
		const std::vector<float>& getStandardParamMeans() const;
		// DocString: FMTYieldModelNn::getStandardParamVars()
		/**
		Returns input variables' variances used in the standartization process when predicting.
		*/
		const std::vector<float>& getStandardParamVars() const;
		// DocString: FMTYieldModelNn::getModelOutputNames()
		/**
		Return model outputs' names.
		*/
		const std::vector<std::string>& getModelOutputNames() const;
		// DocString: FMTYieldModelNn::getInputValues()
		/**
		Returns inputs values based on a predictor passed as parameter.
		*/
		virtual const std::vector<double> getInputValues(const Graph::FMTPredictor& predictor) const = 0;
		// DocString: FMTYieldModelNn::removeNans
		/**
		Replaces nan values with default values.
		*/
		const void removeNans(std::vector<float>& input) const;
		// DocString: FMTYieldModelNn::FMTYieldModelPools(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		Construct a FMTYieldModelNn based on a JSON file and an input yield name list.
		*/
		FMTYieldModelNn(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields);
	public:
		// DocString: FMTYieldModelNn::~FMTYieldModel()
		/**
		Destructor for FMTYieldModelNn.
		*/
		virtual ~FMTYieldModelNn();
		// DocString: FMTYieldModelNn::FMTYieldModelNn()
		/**
		Constructor for FMTYieldModelNn.
		*/
		FMTYieldModelNn()=default;
		// DocString: FMTYieldModelNn::FMTYieldModelNn()
		/**
		Copy constructor for FMTYieldModelNn.
		*/
		FMTYieldModelNn(const FMTYieldModelNn& rhs);
		// DocString: FMTYieldModelNn::predict
		/**
		Runs the machine learning model to predict its outputs.
		*/
		const std::vector<double>predict(const Core::FMTYieldRequest& request) const;
	};
}

#endif