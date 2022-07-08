#ifdef FMTWITHONNXR
#include "FMTyieldmodel.hpp"

#ifndef FMTYIELDMODELNEP_H_INCLUDED
#define FMTYIELDMODELNEP_H_INCLUDED

namespace Core
{
	// DocString: FMTyieldmodelpools
	/**
	FMTyieldmodelpools is a machine learning model to predict carbon NEP.
	*/
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
		// DocString: FMTyieldmodelnep::FMTyieldmodelnep()
		/**
		Default constructor.
		*/
		FMTyieldmodelnep() = default;
		// DocString: FMTyieldmodelnep::FMTyieldmodelnep(const FMTyieldmodelnep& rhs)
		/**
		Copy constructor.
		*/
		FMTyieldmodelnep(const FMTyieldmodelnep& rhs);
		// DocString: FMTyieldmodelnep::operator = (const FMTyieldmodelnep& rhs)
		/**
		Default equality operator.
		*/
		FMTyieldmodelnep& operator = (const FMTyieldmodelnep& rhs) = default;
		// DocString: FMTyieldmodelnep::~FMTyieldmodelnep()
		/**
		Default destructor.
		*/
		~FMTyieldmodelnep();
		// DocString: FMTyieldmodelnep::FMTyieldmodelnep(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		Construct a FMTyieldmodelnep based on a JSON file and an input yield name list.
		*/
		FMTyieldmodelnep(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields);
		// DocString: FMTyieldmodelnep::Clone()
		/**
		Implements FMTyieldmodel::Clone().
		*/
		std::unique_ptr<FMTyieldmodel>Clone() const;
		// DocString: FMTyieldmodelnep::GetModelName()
		/**
		Implements FMTyieldmodel::GetModelType().
		*/
		const std::string& GetModelName() const;
		// DocString: FMTyieldmodelnep::GetModelType()
		/**
		Implements FMTyieldmodel::GetModelType().
		*/
		const std::string& GetModelType() const;
		// DocString: FMTyieldmodelnep::GetStandardParamMeans()
		/**
		Implements FMTyieldmodel::GetStandardParamMeans().
		*/
		const std::vector<float>& GetStandardParamMeans() const;
		// DocString: FMTyieldmodelnep::GetStandardParamVars()
		/**
		Implements FMTyieldmodel::GetStandardParamVars().
		*/
		const std::vector <float>& GetStandardParamVars() const;
		// DocString: FMTyieldmodelnep::GetModelYields()
		/**
		Implements FMTyieldmodel::GetModelYields().
		*/
		const std::vector<std::string>& GetModelYields() const;
		// DocString: FMTyieldmodelnep::GetModelOutputNames()
		/**
		Implements FMTyieldmodel::GetModelOutputNames().
		*/
		const std::vector<std::string>& GetModelOutputNames() const;
		// DocString: FMTyieldmodelnep::GetModelOutputNames()
		/**
		Implements FMTyieldmodel::GetInputValues(const Graph::FMTpredictor& predictor).
		*/
		const std::vector<double> GetInputValues(const Graph::FMTpredictor& predictor) const;
		// DocString: FMTyieldmodelnep::RemoveNans()
		/**
		Implements FMTyieldmodel::RemoveNans().
		*/
		const void RemoveNans(std::vector<float>& input) const;
	};
}

#endif

#endif