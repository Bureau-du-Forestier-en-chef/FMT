#ifdef FMTWITHONNXR
#include "FMTyieldmodelnn.h"

#ifndef FMTYIELDMODELPOOLS_Hm_included
#define FMTYIELDMODELPOOLS_Hm_included

namespace Core
{
	// DocString: FMTYieldModelPools
	/**
	FMTYieldModelPools is a machine learning model to predict carbon pools.
	*/
	class FMTYieldModelPools : public FMTYieldModelNn
	{
	public:
		// DocString: FMTYieldModelPools::FMTYieldModelPools()
		/**
		Default constructor.
		*/
		FMTYieldModelPools() = default;
		// DocString: FMTYieldModelPools::FMTYieldModelPools(const FMTYieldModelPools& rhs)
		/**
		Copy constructor.
		*/
		FMTYieldModelPools(const FMTYieldModelPools& rhs)=default;
		// DocString: FMTYieldModelPools:: operator = (const FMTYieldModelPools& rhs)
		/**
		Default equality operator.
		*/
		FMTYieldModelPools& operator = (const FMTYieldModelPools& rhs) = default;
		// DocString: FMTYieldModelPools::~FMTYieldModelPools()
		/**
		Default destructor.
		*/
		~FMTYieldModelPools()=default;
		// DocString: FMTYieldModelPools::FMTYieldModelPools(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		Construct a FMTYieldModelPools based on a JSON file and an input yield name list.
		*/
		FMTYieldModelPools(const boost::property_tree::ptree & jsonProps,std::vector<std::string>& inputYields);
		// DocString: FMTYieldModelPools::Clone()
		/**
		Implements FMTYieldModel::Clone().
		*/
		std::unique_ptr<FMTYieldModel>Clone() const;
		// DocString: FMTYieldModelPools::getInputValues()
		/**
		Implements FMTYieldModel::getInputValues().
		*/
		const std::vector<double> getInputValues(const Graph::FMTpredictor& predictor) const;
		// DocString: FMTYieldModelPools::getModelType()
		/**
		Return the modeltype of the FMTYieldModel.
		*/
		static std::string getModelType();
	};
}

#endif
#endif