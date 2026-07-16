#ifdef FMTWITHONNXR
#include "FMTyieldmodelnn.h"

#ifndef FMTYIELDMODELNEP_Hm_included
#define FMTYIELDMODELNEP_Hm_included

namespace Core
{
	// DocString: FMTYieldModelPools
	/**
	FMTYieldModelPools is a machine learning model to predict carbon NEP.
	*/
	class FMTYieldModelNep : public FMTYieldModelNn
	{
	public:
		// DocString: FMTYieldModelNep::FMTYieldModelNep()
		/**
		Default constructor.
		*/
		FMTYieldModelNep() = default;
		// DocString: FMTYieldModelNep::FMTYieldModelNep(const FMTYieldModelNep& rhs)
		/**
		Copy constructor.
		*/
		FMTYieldModelNep(const FMTYieldModelNep& rhs)=default;
		// DocString: FMTYieldModelNep::operator = (const FMTYieldModelNep& rhs)
		/**
		Default equality operator.
		*/
		FMTYieldModelNep& operator = (const FMTYieldModelNep& rhs) = default;
		// DocString: FMTYieldModelNep::~FMTYieldModelNep()
		/**
		Default destructor.
		*/
		~FMTYieldModelNep()=default;
		// DocString: FMTYieldModelNep::FMTYieldModelNep(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		Construct a FMTYieldModelNep based on a JSON file and an input yield name list.
		*/
		FMTYieldModelNep(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields);
		// DocString: FMTYieldModelNep::Clone()
		/**
		Implements FMTYieldModel::Clone().
		*/
		std::unique_ptr<FMTYieldModel>Clone() const;
		// DocString: FMTYieldModelNep::getModelOutputNames()
		/**
		Implements FMTYieldModel::getInputValues(const Graph::FMTpredictor& predictor).
		*/
		const std::vector<double> getInputValues(const Graph::FMTpredictor& predictor) const;
		// DocString: FMTYieldModelNep::getModelType()
		/**
		Return the modeltype of the FMTYieldModel.
		*/
		static std::string getModelType();
	};
}

#endif

#endif