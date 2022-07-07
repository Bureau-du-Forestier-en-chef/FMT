#ifdef FMTWITHONNXR
#include "FMTyieldmodelnep.hpp"
#include "FMTyieldrequest.hpp"
#include "FMTgraph.hpp"
#include "FMTlinegraph.hpp"
#include "FMTyields.hpp"
#include "FMTpredictor.hpp"
#include "FMTsrmodel.hpp"
#include  <onnxruntime/core/session/onnxruntime_cxx_api.h>
#include <boost/property_tree/json_parser.hpp>


namespace Core {
	const std::string JSON_PROP_STAND_FILE_PATH = "csvStandardisationFile";

	FMTyieldmodelnep::FMTyieldmodelnep(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
	{
		boost::property_tree::ptree::const_assoc_iterator modelNameIt = jsonProps.find(JSON_PROP_MODEL_NAME);
		modelName = modelNameIt->second.data();
		boost::property_tree::ptree::const_assoc_iterator modelTypeIt = jsonProps.find(JSON_PROP_MODEL_TYPE);
		modelType = modelTypeIt->second.data();
		boost::property_tree::ptree::const_assoc_iterator stdParamsFileNameIt = jsonProps.find(JSON_PROP_STAND_FILE_PATH);
		std::string stdParamsFileName = stdParamsFileNameIt->second.data();

		std::wstring wideModelName = std::wstring(modelName.begin(), modelName.end());
		sessionPtr = std::unique_ptr<Ort::Session>(new Ort::Session(*envPtr.get(), wideModelName.c_str(), Ort::SessionOptions{}));

		std::ifstream file(stdParamsFileName);
		std::vector<std::string> headers = GetNextLineAndSplitIntoTokens(file);
		headers.erase(headers.begin());
		std::vector<std::string> strMeans = GetNextLineAndSplitIntoTokens(file);
		strMeans.erase(strMeans.begin());
		std::vector<std::string> strVars = GetNextLineAndSplitIntoTokens(file);
		strVars.erase(strVars.begin());

		std::vector<std::string> yields;
		for (auto& item : jsonProps.get_child(JSON_PROP_MODEL_YIELDS))
		{
			yields.push_back(item.second.get_value<std::string>());
		}

		ValidateInputYields(yields, inputYields);

		modelYields = inputYields;

		for (auto& item : jsonProps.get_child(JSON_PROP_MODEL_OUTPUTS))
		{
			modelOutputs.push_back(item.second.get_value<std::string>());
		}

		standardParamMeans = std::vector<float>(strMeans.size());
		standardParamVars = std::vector<float>(strVars.size());
		for (size_t i = 0; i < strMeans.size(); i++)
		{
			standardParamMeans[i] = std::stof(strMeans[i]);
			standardParamVars[i] = std::stof(strVars[i]);
		}
	}

	FMTyieldmodelnep::~FMTyieldmodelnep()
	{
		sessionPtr->release();
	}

	FMTyieldmodelnep::FMTyieldmodelnep(const FMTyieldmodelnep& rhs) :
		modelName(rhs.GetModelName()),
		modelType(rhs.GetModelType()),
		standardParamMeans(rhs.GetStandardParamMeans()),
		standardParamVars(rhs.GetStandardParamVars()),
		modelYields(rhs.GetModelYields()),
		modelOutputs(rhs.GetModelOutputNames())
	{
		std::wstring wideModelName = std::wstring(modelName.begin(), modelName.end());
		sessionPtr = std::unique_ptr<Ort::Session>(new Ort::Session(*envPtr.get(), wideModelName.c_str(), Ort::SessionOptions{}));
	}

	std::unique_ptr<FMTyieldmodel>FMTyieldmodelnep::Clone() const
	{
		try {
			//_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			//	"FMTyieldmodel::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelnep(*this));
	}

	const std::string& FMTyieldmodelnep::GetModelName() const
	{
		return modelName;
	}

	const std::string& FMTyieldmodelnep::GetModelType() const
	{
		return modelType;
	}

	const std::vector<float>& FMTyieldmodelnep::GetStandardParamMeans() const
	{
		return standardParamMeans;
	}

	const std::vector<float>& FMTyieldmodelnep::GetStandardParamVars() const
	{
		return standardParamVars;
	}

	const std::vector<std::string>& FMTyieldmodelnep::GetModelYields() const
	{
		return modelYields;
	}

	const std::vector<std::string>& FMTyieldmodelnep::GetModelOutputNames() const
	{
		return modelOutputs;
	}

	const std::vector<double> FMTyieldmodelnep::GetInputValues(const Graph::FMTpredictor& predictor) const
	{
		std::vector<double> values;
		std::vector<double> sourceYields = predictor.getSourceYields();
		std::vector<double> targetYields = predictor.getTargetYields();

		values.push_back(predictor.getDistance(1));								//s1_distance
		values.push_back(predictor.getDisturbance(1));							//s1_disturbance
		values.push_back(predictor.getDistance(2));								//s2_distance
		values.push_back(predictor.getDisturbance(2));							//s2_disturbance
		values.push_back(predictor.getDistance(3));								//s3_distance
		values.push_back(predictor.getDisturbance(3));							//s3_disturbance
		values.push_back(predictor.getSourceAge());								//source_age
		values.insert(values.end(), sourceYields.begin(), sourceYields.end());	//source_YV_G_GFI 
																				//source_YV_G_GFT
																				//source_YV_G_GR
																				//source_YV_G_GF
		values.push_back(predictor.getDistance(0));								//target_distance
		values.push_back(predictor.getDisturbance(0));							//target_disturbance
		values.push_back(predictor.getTargetAge());								//target_age
		values.insert(values.end(), targetYields.begin(), targetYields.end());	//target_YV_G_GFI 
																				//target_YV_G_GFT
																				//target_YV_G_GR
																				//target_YV_G_GF


		return values;
	}

	const void FMTyieldmodelnep::RemoveNans(std::vector<float>& input) const
	{
		for (int i = 0; i < input.size(); i++)
		{
			if (isnan(input[i]))
			{
				if (i == 0 || i == 2 || i == 4)
					input[i] = 0;
				if (i == 1 || i == 3 || i == 5)
					input[i] = FMTyieldmodel::UNKNOWN_DISTURBANCE_CODE;
			}
		}
	}
}

#endif