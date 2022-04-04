#include "FMTyieldmodel.hpp"

#ifndef FMTYIELDMODELNEP_H_INCLUDED
#define FMTYIELDMODELNEP_H_INCLUDED

namespace Core
{
	class FMTyieldmodelnep : public FMTyieldmodel
	{
	private:
		std::string modelName;
		std::string modelType;
		std::vector<float> standardParamMeans = {};
		std::vector<float> standardParamVars = {};
		std::vector<std::string> modelYields = {};
		std::vector<std::string> modelOutputs = {};
	public:
		FMTyieldmodelnep() = default;
		FMTyieldmodelnep(const FMTyieldmodelnep& rhs);
		FMTyieldmodelnep& operator = (const FMTyieldmodelnep& rhs) = default;
		~FMTyieldmodelnep();
		FMTyieldmodelnep(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields);
		std::unique_ptr<FMTyieldmodel>Clone() const;
		const std::string& GetModelName() const;
		const std::string& GetModelType() const;
		const std::vector<float>& GetStandardParamMeans() const;
		const std::vector <float>& GetStandardParamVars() const;
		const std::vector<std::string>& GetModelYields() const;
		const std::vector<std::string>& GetModelOutputNames() const;
		const std::string GetModelInfo() const;
		const std::vector<double> GetInputValues(const Graph::FMTpredictor& predictor) const;
	};
}

#endif