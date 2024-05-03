#ifdef FMTWITHONNXR
#include "FMTyieldmodelnep.h"
#include "FMTpredictor.h"
#include "FMTexceptionhandler.h"


namespace Core {

	FMTyieldmodelnep::FMTyieldmodelnep(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields):
		FMTyieldmodelnn(jsonProps, inputYields)
	{
		
	}


	std::unique_ptr<FMTyieldmodel>FMTyieldmodelnep::Clone() const
	{
		try {
			return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelnep(*this));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodelnep::Clone", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	std::string FMTyieldmodelnep::GetModelType()
	{
		return "NEP";
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
}

#endif