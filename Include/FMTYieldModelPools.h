#ifdef FMTWITHONNXR
#include "FMTYieldModelNn.h"

#ifndef FMTYIELDMODELPOOLS_Hm_included
#define FMTYIELDMODELPOOLS_Hm_included

namespace Core
{
	// DocString: FMTYieldModelPools
	/**
	@brief Machine learning yield model predicting carbon pools.
	*/
	class FMTYieldModelPools : public FMTYieldModelNn
	{
	public:
		// DocString: FMTYieldModelPools::FMTYieldModelPools()
		/**
		@brief Default constructor for FMTYieldModelPools.
		*/
		FMTYieldModelPools() = default;
		// DocString: FMTYieldModelPools::FMTYieldModelPools(const FMTYieldModelPools& rhs)
		/**
		@brief Copy constructor for FMTYieldModelPools.
		@param[in] rhs the FMTYieldModelPools to copy.
		*/
		FMTYieldModelPools(const FMTYieldModelPools& rhs)=default;
		// DocString: FMTYieldModelPools:: operator = (const FMTYieldModelPools& rhs)
		/**
		@brief Copy assignment operator for FMTYieldModelPools.
		@param[in] rhs the FMTYieldModelPools to copy.
		@return a reference to this FMTYieldModelPools.
		*/
		FMTYieldModelPools& operator = (const FMTYieldModelPools& rhs) = default;
		// DocString: FMTYieldModelPools::~FMTYieldModelPools()
		/**
		@brief Default destructor for FMTYieldModelPools.
		*/
		~FMTYieldModelPools()=default;
		// DocString: FMTYieldModelPools::FMTYieldModelPools(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		@brief Construct a FMTYieldModelPools from a JSON tree and an input yield name list.
		@param[in] jsonProps the JSON properties.
		@param[in,out] inputYields the input yield names.
		*/
		FMTYieldModelPools(const boost::property_tree::ptree & jsonProps,std::vector<std::string>& inputYields);
		// DocString: FMTYieldModelPools::Clone()
		/**
		@brief Clone the yield model.
		@return a unique pointer to the cloned yield model.
		*/
		std::unique_ptr<FMTYieldModel>Clone() const;
		// DocString: FMTYieldModelPools::getInputValues()
		/**
		@brief Return the input values for a predictor.
		@param[in] predictor the predictor.
		@return the input values.
		*/
		const std::vector<double> getInputValues(const Graph::FMTPredictor& predictor) const;
		// DocString: FMTYieldModelPools::getModelType()
		/**
		@brief Return the model type of the yield model.
		@return the model type.
		*/
		static std::string getModelType();
	};
}

#endif
#endif