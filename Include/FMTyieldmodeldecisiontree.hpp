
#include "FMTyieldmodel.hpp"

#ifndef FMTYIELDMODELDECISIONTREE_H_INCLUDED
#define FMTYIELDMODELDECISIONTREE_H_INCLUDED

namespace Core
{
	class FMTmask;
	// DocString: FMTyieldmodeldecisiontree
	/**
	FMTyieldmodeldecisiontree use a decision tree based on outputs of the model (growth only) to get yield values.
	*/
	class FMTyieldmodeldecisiontree : public FMTyieldmodel
	{
	public:
		// DocString: FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree()
		/**
		Default constructor.
		*/
		FMTyieldmodeldecisiontree() = default;
		// DocString: FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree(const FMTyieldmodeldecisiontree& rhs)
		/**
		Copy constructor.
		*/
		FMTyieldmodeldecisiontree(const FMTyieldmodeldecisiontree& rhs)=default;
		// DocString: FMTyieldmodeldecisiontree::operator = (const FMTyieldmodeldecisiontree& rhs)
		/**
		Default equality operator.
		*/
		FMTyieldmodeldecisiontree& operator = (const FMTyieldmodeldecisiontree& rhs) = default;
		// DocString: FMTyieldmodeldecisiontree::~FMTyieldmodeldecisiontree()
		/**
		Default destructor.
		*/
		~FMTyieldmodeldecisiontree()=default;
		// DocString: FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		Construct a FMTyieldmodeldecisiontree based on a JSON file and an input yield name list.
		*/
		FMTyieldmodeldecisiontree(const boost::property_tree::ptree& jsonProps,const std::vector<std::string>& inputYields, const Core::FMTmask& mainmask);
		// DocString: FMTyieldmodeldecisiontree::Clone()
		/**
		Implements FMTyieldmodel::Clone().
		*/
		std::unique_ptr<FMTyieldmodel>Clone() const;
		// DocString: FMTyieldmodeldecisiontree::Predict
		/**
		Runs the decision tree
		*/
		const std::vector<double>Predict(const Core::FMTyieldrequest& request) const;
	};
}

#endif
