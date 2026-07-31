#ifdef FMTWITHONNXR
#include "FMTYieldModelPools.h"
#include "FMTPredictor.h"
#include "FMTExceptionHandler.h"

namespace Core {
	const std::string JSON_PROP_STAND_FILE_PATH = "csvStandardisationFile";

	FMTYieldModelPools::FMTYieldModelPools(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields):
		FMTYieldModelNn(jsonProps, inputYields)
	{

	}

	
	std::unique_ptr<FMTYieldModel>FMTYieldModelPools::clone() const
	{
		try {
			return std::unique_ptr<FMTYieldModel>(new FMTYieldModelPools(*this));
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldModelPools::clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldModel>(nullptr);
	}

	

	const std::vector<double> FMTYieldModelPools::getInputValues(const Graph::FMTPredictor& predictor) const
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

	std::string FMTYieldModelPools::getModelType()
	{
		return "POOLS";
	}

}

#endif