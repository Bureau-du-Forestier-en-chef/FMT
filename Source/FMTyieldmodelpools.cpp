#ifdef FMTWITHONNXR
#include "FMTyieldmodelpools.hpp"
#include "FMTpredictor.hpp"
#include  <onnxruntime/core/session/onnxruntime_cxx_api.h>

namespace Core {
	const std::string JSON_PROP_STAND_FILE_PATH = "csvStandardisationFile";

	FMTyieldmodelpools::FMTyieldmodelpools(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
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

	FMTyieldmodelpools::~FMTyieldmodelpools()
	{
		sessionPtr->release();
	}

	FMTyieldmodelpools::FMTyieldmodelpools(const FMTyieldmodelpools& rhs) :
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
	
	std::unique_ptr<FMTyieldmodel>FMTyieldmodelpools::Clone() const
	{
		try {
			//_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			//	"FMTyieldmodel::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelpools(*this));
	}

	const std::string& FMTyieldmodelpools::GetModelName() const
	{
		return modelName;
	}

	const std::string& FMTyieldmodelpools::GetModelType() const
	{
		return modelType;
	}

	const std::vector<float>& FMTyieldmodelpools::GetStandardParamMeans() const
	{
		return standardParamMeans;
	}

	const std::vector<float>& FMTyieldmodelpools::GetStandardParamVars() const
	{
		return standardParamVars;
	}

	const std::vector<std::string>& FMTyieldmodelpools::GetModelYields() const
	{
		return modelYields;
	}

	const std::vector<std::string>& FMTyieldmodelpools::GetModelOutputNames() const
	{
		return modelOutputs;
	}

	const std::string FMTyieldmodelpools::GetModelInfo() const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
				"FMTyieldmodel::GetModelInfo", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodel::GetModelInfo", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::string();
	}

	const std::vector<double> FMTyieldmodelpools::GetInputValues(const Graph::FMTpredictor& predictor) const
	{
		std::vector<double> values;
		std::vector<double> sourceYields = predictor.getSourceYields();

		values.push_back(predictor.getDistance(1));								//s1_distance
		values.push_back(predictor.getDisturbance(1));							//s1_disturbance
		values.push_back(predictor.getDistance(2));								//s2_distance
		values.push_back(predictor.getDisturbance(2));							//s2_disturbance
		values.push_back(predictor.getDistance(3));								//s3_distance
		values.push_back(predictor.getDisturbance(3));							//s3_disturbance
		values.push_back(predictor.getSourceAge());								//age
		values.insert(values.end(), sourceYields.begin(), sourceYields.end());	//YV_G_GFI 
																				//YV_G_GFT
																				//YV_G_GR
																				//YV_G_GF
		
		return values;
	}
}

#endif