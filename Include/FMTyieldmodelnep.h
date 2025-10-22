#ifdef FMTWITHONNXR
#include "FMTyieldmodelnn.h"

#ifndef FMTYIELDMODELNEP_Hm_included
#define FMTYIELDMODELNEP_Hm_included

namespace Core
{
	// DocString: FMTyieldmodelpools
	/**
	FMTyieldmodelpools is a machine learning model to predict carbon NEP.
	*/
	class FMTyieldmodelnep : public FMTyieldmodelnn
	{
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
		FMTyieldmodelnep(const FMTyieldmodelnep& rhs)=default;
		// DocString: FMTyieldmodelnep::operator = (const FMTyieldmodelnep& rhs)
		/**
		Default equality operator.
		*/
		FMTyieldmodelnep& operator = (const FMTyieldmodelnep& rhs) = default;
		// DocString: FMTyieldmodelnep::~FMTyieldmodelnep()
		/**
		Default destructor.
		*/
		~FMTyieldmodelnep()=default;
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
		// DocString: FMTyieldmodelnep::GetModelOutputNames()
		/**
		Implements FMTyieldmodel::GetInputValues(const Graph::FMTpredictor& predictor).
		*/
		const std::vector<double> GetInputValues(const Graph::FMTpredictor& predictor) const;
		// DocString: FMTyieldmodelnep::GetModelType()
		/**
		Return the modeltype of the FMTyieldmodel.
		*/
		static std::string GetModelType();
	};
}

#endif

#endif