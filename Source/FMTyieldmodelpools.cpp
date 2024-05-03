#ifdef FMTWITHONNXR
#include "FMTyieldmodelpools.h"
#include "FMTpredictor.h"
#include "FMTexceptionhandler.h"

namespace Core {
	const std::string JSON_PROP_STAND_FILE_PATH = "csvStandardisationFile";

	FMTyieldmodelpools::FMTyieldmodelpools(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields):
		FMTyieldmodelnn(jsonProps, inputYields)
	{

	}

	
	std::unique_ptr<FMTyieldmodel>FMTyieldmodelpools::Clone() const
	{
		try {
			return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelpools(*this));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodelpools::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
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

	std::string FMTyieldmodelpools::GetModelType()
	{
		return "POOLS";
	}

}

#endif