#ifdef FMTWITHONNXR
#include "FMTyieldmodelnn.hpp"

#ifndef FMTYIELDMODELPOOLS_H_INCLUDED
#define FMTYIELDMODELPOOLS_H_INCLUDED

namespace Core
{
	// DocString: FMTyieldmodelpools
	/**
	FMTyieldmodelpools is a machine learning model to predict carbon pools.
	*/
	class FMTyieldmodelpools : public FMTyieldmodelnn
	{
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
		FMTyieldmodelpools(const FMTyieldmodelpools& rhs)=default;
		// DocString: FMTyieldmodelpools:: operator = (const FMTyieldmodelpools& rhs)
		/**
		Default equality operator.
		*/
		FMTyieldmodelpools& operator = (const FMTyieldmodelpools& rhs) = default;
		// DocString: FMTyieldmodelpools::~FMTyieldmodelpools()
		/**
		Default destructor.
		*/
		~FMTyieldmodelpools()=default;
		// DocString: FMTyieldmodelpools::FMTyieldmodelpools(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		Construct a FMTyieldmodelpools based on a JSON file and an input yield name list.
		*/
		FMTyieldmodelpools(const boost::property_tree::ptree & jsonProps,std::vector<std::string>& inputYields);
		// DocString: FMTyieldmodelpools::Clone()
		/**
		Implements FMTyieldmodel::Clone().
		*/
		std::unique_ptr<FMTyieldmodel>Clone() const;
		// DocString: FMTyieldmodelpools::GetInputValues()
		/**
		Implements FMTyieldmodel::GetInputValues().
		*/
		const std::vector<double> GetInputValues(const Graph::FMTpredictor& predictor) const;
		// DocString: FMTyieldmodelpools::GetModelType()
		/**
		Return the modeltype of the FMTyieldmodel.
		*/
		static std::string GetModelType();
	};
}

#endif
#endif