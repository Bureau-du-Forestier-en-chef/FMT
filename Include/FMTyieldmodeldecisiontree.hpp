
#include "FMTyieldmodel.hpp"
#include <vector>
#include <string>
#include <memory>
#include <boost/thread/recursive_mutex.hpp>

#ifndef FMTYIELDMODELDECISIONTREE_H_INCLUDED
#define FMTYIELDMODELDECISIONTREE_H_INCLUDED

namespace Models
{
	class FMTmodel;
}

namespace Core
{
	class FMTmask;
	class FMTconstraint;
	class FMToutput;
	// DocString: FMTyieldmodeldecisiontree
	/**
	FMTyieldmodeldecisiontree use a decision tree based on outputs of the model (growth only) to get yield values.
	*/
	class FMTyieldmodeldecisiontree : public FMTyieldmodel
	{
	// DocString: FMTyieldmodeldecisiontree::mtx
	///Mutex for naturalgrowth.
	//mutable std::recursive_mutex mtx;
	mutable boost::recursive_mutex mtx;
	// DocString: FMTyieldmodeldecisiontree::values
	///Map of resulting yield id with the decision made per period in a vector
	mutable std::map<size_t,std::vector<double>>values;
	// DocString: FMTyieldmodeldecisiontree::reference
	///The reference output for the ratio
	std::unique_ptr<FMToutput> reference;
	// DocString: FMTyieldmodeldecisiontree::nodes
	///The decision nodes constraints are used for bounds
	std::vector<FMTconstraint>nodes;
	// DocString: FMTyieldmodeldecisiontree::default_values
	///Yields default values if update period
	std::vector<double>default_values;
	// DocString: FMTyieldmodeldecisiontree::JSON_PROP_TREE
	///The json tree key
	const std::string JSON_PROP_TREE = "Tree";
	// DocString: FMTyieldmodeldecisiontree::JSON_PROP_DECISIONS
	///The json decisions key
	const std::string JSON_PROP_DECISIONS = "Decisions";
	// DocString: FMTyieldmodeldecisiontree::JSON_PROP_REFERENCE
	///The json Reference key
	const std::string JSON_PROP_REFERENCE = "Reference";
	// DocString: FMTyieldmodeldecisiontree::JSON_PROP_DEFAULT
	///The json default values key
	const std::string JSON_PROP_DEFAULT = "Default";
	// DocString: FMTyieldmodeldecisiontree::GetMask
	/**
	Get the mask of the actual decision tree. Will return a valid mask of the reference output
	*/
	Core::FMTmask GetMask() const;
	// DocString: FMTyieldmodeldecisiontree::buildconstraint
	/**
	Take the yield, the main mask lowerbound and upperbound and build up a constraint on the form of:
	*OUTPUT constraint
	*SOURCE mainmask _INVENT yld
	constraint <= upperbound
	constraint >= lowerbound
	lag is the amount of period added or removed when getting constraint output values.
	*/
	Core::FMTconstraint BuildConstraint(const std::string& name, const std::string& yld, const Core::FMTmask& mask, const double& lowerbound, const double& upperbound,const int& lag) const;
	// DocString: FMTyieldmodeldecisiontree::getadecision
	/**
	From de constraint id return a new constraint id based on the naturalgrowth and the period with the decision tree
	*/
	size_t GetADecision(const std::unique_ptr<Models::FMTmodel>& naturalgrowth,const size_t& constraint_id, const int& period/*, std::string& decision_stack*/) const;
	// DocString: FMTyieldmodeldecisiontree::getnaturalgrowth
	/**
	Build the natural growth model with a request (original model) to be able to call getadecision on the growthmodel
	this function has to be thread safe. Only build a small model dedicated to the reference output 
	*/
	std::unique_ptr<Models::FMTmodel> GetNaturalGrowth(const Core::FMTyieldrequest& request) const;
	// DocString: FMTyieldmodeldecisiontree::modify
	/**
	This will modify the class if presolve = true will do presolve, if not will do postsolve.
	*/
	std::unique_ptr<FMTyieldmodel> Modify(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& newthemes,bool presolve = true) const;
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
		FMTyieldmodeldecisiontree(const FMTyieldmodeldecisiontree& rhs);
		// DocString: FMTyieldmodeldecisiontree::operator = (const FMTyieldmodeldecisiontree& rhs)
		/**
		Default equality operator.
		*/
		FMTyieldmodeldecisiontree& operator = (const FMTyieldmodeldecisiontree& rhs)=default;
		// DocString: FMTyieldmodeldecisiontree::~FMTyieldmodeldecisiontree()
		/**
		Default destructor.
		*/
		~FMTyieldmodeldecisiontree()=default;
		// DocString: FMTyieldmodeldecisiontree::FMTyieldmodeldecisiontree(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		Construct a FMTyieldmodeldecisiontree based on a JSON file and an input yield name list and also on the global mask
		*/
		FMTyieldmodeldecisiontree(const boost::property_tree::ptree& jsonProps,const std::vector<std::string>& inputYields,const Core::FMTmask& mainmask);
		// DocString: FMTyieldmodeldecisiontree::Clone()
		/**
		Implements FMTyieldmodel::Clone().
		*/
		std::unique_ptr<FMTyieldmodel>Clone() const;
		// DocString: FMTyieldmodeldecisiontree::Predict
		/**
		Runs the decision tree so if the values are not set build a naturalgrowth model and run the decision tree and get the decision for all
		periods. Finaly return the predictions.
		*/
		const std::vector<double>Predict(const Core::FMTyieldrequest& request) const;
		// DocString: FMTyieldmodeldecisiontree::GetModelType()
		/**
		Return the modeltype of the FMTyieldmodel.
		*/
		static std::string GetModelType();
		// DocString: FMTyieldmodeldecisiontree::presolve
		/**
		Return a presolved FMTyieldmodeldecisiontree
		*/
		virtual std::unique_ptr<FMTyieldmodel> presolve(const FMTmaskfilter& filter,
			const std::vector<FMTtheme>& newthemes) const;
		// DocString: FMTyieldmodeldecisiontree::postsolve
		/**
		Change the output and constraints mask to return to the original constraints and outputs.
		*/
		virtual std::unique_ptr<FMTyieldmodel> postsolve(const FMTmaskfilter& filter,
			const std::vector<FMTtheme>& basethemes) const;
		// DocString: FMTyieldmodeldecisiontreel::std::string()
		/**
		When it comes to write down in a string the yield model.
		*/
		virtual  operator std::string() const;
		// DocString: FMTyieldmodeldecisiontreel::getperiodicvalues
		/**
		Try to turn the FMTyieldmodel into periodic constant values. if returns an non empty vector then
		each first dimension is the yield id and each second dimension are the periodic value calculated by the yield.
		*/
		virtual std::vector<std::vector<double>>getperiodicvalues() const;
	};
}

#endif
