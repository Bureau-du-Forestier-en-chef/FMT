#ifdef FMTWITHONNXR
#include "FMTyieldmodel.hpp"

#ifndef FMTYIELDMODELPOOLS_H_INCLUDED
#define FMTYIELDMODELPOOLS_H_INCLUDED

namespace Core
{
	// DocString: FMTyieldmodelpools
	/**
	FMTyieldmodelpools is a machine learning model to predict carbon pools.
	*/
	class FMTyieldmodelpools : public FMTyieldmodel
	{
	private:
		std::string modelName;
		std::string modelType;
		std::vector<float> standardParamMeans = {};
		std::vector<float> standardParamVars = {};
		std::vector<std::string> modelYields = {};
		std::vector<std::string> modelOutputs = {};
	public:
		// DocString: FMTyieldmodelpools::FMTyieldmodelpools()
		/**
		Default constructor.
		*/
		FMTyieldmodelpools() = default;
		// DocString: FMTyieldmodelpools::FMTyieldmodelpools(const FMTyieldmodelpools& rhs)
		/**
		Copy constructor.
		*/
		FMTyieldmodelpools(const FMTyieldmodelpools& rhs);
		// DocString: FMTyieldmodelpools:: operator = (const FMTyieldmodelpools& rhs)
		/**
		Default equality operator.
		*/
		FMTyieldmodelpools& operator = (const FMTyieldmodelpools& rhs) = default;
		// DocString: FMTyieldmodelpools::~FMTyieldmodelpools()
		/**
		Default destructor.
		*/
		~FMTyieldmodelpools();
		// DocString: FMTyieldmodelpools::FMTyieldmodelpools(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		Construct a FMTyieldmodelpools based on a JSON file and an input yield name list.
		*/
		FMTyieldmodelpools(const boost::property_tree::ptree & jsonProps, std::vector<std::string>& inputYields);
		// DocString: FMTyieldmodelpools::Clone()
		/**
		Implements FMTyieldmodel::Clone().
		*/
		std::unique_ptr<FMTyieldmodel>Clone() const;
		// DocString: FMTyieldmodelpools::GetModelName()
		/**
		Implements FMTyieldmodel::GetModelName().
		*/
		const std::string& GetModelName() const;
		// DocString: FMTyieldmodelpools::GetModelType()
		/**
		Implements FMTyieldmodel::GetModelType().
		*/
		const std::string& GetModelType() const;
		// DocString: FMTyieldmodelpools::GetStandardParamMeans()
		/**
		Implements FMTyieldmodel::GetStandardParamMeans().
		*/
		const std::vector<float>& GetStandardParamMeans() const;
		// DocString: FMTyieldmodelpools::GetStandardParamVars()
		/**
		Implements FMTyieldmodel::GetStandardParamVars().
		*/
		const std::vector<float>& GetStandardParamVars() const;
		// DocString: FMTyieldmodelpools::GetModelYields()
		/**
		Implements FMTyieldmodel::GetModelYields().
		*/
		const std::vector<std::string>& GetModelYields() const;
		// DocString: FMTyieldmodelpools::GetModelOutputNames()
		/**
		Implements FMTyieldmodel::GetModelOutputNames().
		*/
		const std::vector<std::string>& GetModelOutputNames() const;
		// DocString: FMTyieldmodelpools::GetInputValues()
		/**
		Implements FMTyieldmodel::GetInputValues().
		*/
		const std::vector<double> GetInputValues(const Graph::FMTpredictor& predictor) const;
		// DocString: FMTyieldmodelpools::RemoveNans()
		/**
		Implements FMTyieldmodel::RemoveNans().
		*/
		const void RemoveNans(std::vector<float>& input) const;
	};
}

#endif
#endif