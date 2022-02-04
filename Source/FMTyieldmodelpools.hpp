#include "FMTyieldmodel.hpp"

#ifndef FMTYIELDMODELPOOLS_H_INCLUDED
#define FMTYIELDMODELPOOLS_H_INCLUDED

namespace Core
{
	class FMTyieldmodelpools : public FMTyieldmodel
	{
	private:
		std::string modelName;
		std::string modelType;
		std::vector<float> standardParamMeans = {};
		std::vector<float> standardParamVars = {};
		std::vector<std::string> modelYields = {};
	public:
		FMTyieldmodelpools() = default;
		FMTyieldmodelpools(const FMTyieldmodelpools& rhs);
		FMTyieldmodelpools& operator = (const FMTyieldmodelpools& rhs) = default;
		virtual ~FMTyieldmodelpools() = default;
		FMTyieldmodelpools(const boost::property_tree::ptree & jsonProps);
		std::unique_ptr<FMTyieldmodel>Clone() const;
		const std::string& GetModelName() const;
		const std::string& GetModelType() const;
		const std::vector<float>& GetStandardParamMeans() const;
		const std::vector<float>& GetStandardParamVars() const;
		const std::vector<std::string>& GetModelYields() const;
		const std::string GetModelInfo() const;
		bool Validate(const std::vector<std::string>&YieldsAvailable) const;
		const std::vector<std::string>GetYieldsOutputs() const;
		//const std::vector<double>Predict(const Core::FMTyieldrequest & request) const;
		const std::vector<double> GetInputValues(const Graph::FMTcarbonpredictor& predictor) const;
	};
}

#endif