
#include "FMTyieldmodel.h"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <boost/thread/recursive_mutex.hpp>

#ifndef FMTYIELDMODELDECISIONTREE_Hm_included
#define FMTYIELDMODELDECISIONTREE_Hm_included

namespace Models
{
	class FMTModel;
}

namespace Core
{
	class FMTMask;
	class FMTConstraint;
	class FMTOutput;
	// DocString: FMTYieldModelDecisionTree
	/**
	FMTYieldModelDecisionTree use a decision tree based on outputs of the model (growth only) to get yield values.
	*/
	class FMTYieldModelDecisionTree : public FMTYieldModel
	{
	// DocString: FMTYieldModelDecisionTree::mtx
	///Mutex for naturalGrowth.
	//mutable std::recursive_mutex mtx;
	mutable boost::recursive_mutex mtx;
	// DocString: FMTYieldModelDecisionTree::values
	///Map of resulting yield id with the decision made per period in a vector
	mutable std::map<size_t,std::vector<double>>values;
	// DocString: FMTYieldModelDecisionTree::reference
	///The reference output for the ratio
	std::unique_ptr<FMTOutput> reference;
	// DocString: FMTYieldModelDecisionTree::nodes
	///The decision nodes constraints are used for bounds
	std::vector<FMTConstraint>nodes;
	// DocString: FMTYieldModelDecisionTree::default_values
	///Yields default values if update period
	std::vector<double>default_values;
	// DocString: FMTYieldModelDecisionTree::JSON_PROP_TREE
	///The json tree key
	const std::string JSON_PROP_TREE = "Tree";
	// DocString: FMTYieldModelDecisionTree::JSON_PROP_DECISIONS
	///The json decisions key
	const std::string JSON_PROP_DECISIONS = "Decisions";
	// DocString: FMTYieldModelDecisionTree::JSON_PROP_REFERENCE
	///The json Reference key
	const std::string JSON_PROP_REFERENCE = "Reference";
	// DocString: FMTYieldModelDecisionTree::JSON_PROP_DEFAULT
	///The json default values key
	const std::string JSON_PROP_DEFAULT = "Default";
	// DocString: FMTYieldModelDecisionTree::getMask
	/**
	Get the mask of the actual decision tree. Will return a valid mask of the reference output
	*/
	Core::FMTMask getMask() const;
	// DocString: FMTYieldModelDecisionTree::buildconstraint
	/**
	Take the yield, the main mask lowerbound and upperbound and build up a constraint on the form of:
	*OUTPUT constraint
	*SOURCE mainmask _INVENT yld
	constraint <= upperbound
	constraint >= lowerbound
	lag is the amount of period added or removed when getting constraint output values.
	*/
	Core::FMTConstraint buildConstraint(const std::string& name, const std::string& yld, const Core::FMTMask& mask, const double& lowerbound, const double& upperbound,const int& lag) const;
	// DocString: FMTYieldModelDecisionTree::getadecision
	/**
	From de constraint id return a new constraint id based on the naturalGrowth and the period with the decision tree
	*/
	size_t getADecision(const std::unique_ptr<Models::FMTModel>& naturalGrowth,const size_t& constraint_id, const int& period/*, std::string& decision_stack*/) const;
	// DocString: FMTYieldModelDecisionTree::getnaturalgrowth
	/**
	Build the natural growth model with a request (original model) to be able to call getadecision on the growthmodel
	this function has to be thread safe. Only build a small model dedicated to the reference output 
	*/
	std::unique_ptr<Models::FMTModel> getNaturalGrowth(const Core::FMTYieldRequest& request) const;
	// DocString: FMTYieldModelDecisionTree::modify
	/**
	This will modify the class if presolve = true will do presolve, if not will do postSolve.
	*/
	std::unique_ptr<FMTYieldModel> modify(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& newthemes,bool presolve = true) const;
	public:
		// DocString: FMTYieldModelDecisionTree::FMTYieldModelDecisionTree()
		/**
		Default constructor.
		*/
		FMTYieldModelDecisionTree() = default;
		// DocString: FMTYieldModelDecisionTree::FMTYieldModelDecisionTree(const FMTYieldModelDecisionTree& rhs)
		/**
		Copy constructor.
		*/
		FMTYieldModelDecisionTree(const FMTYieldModelDecisionTree& rhs);
		// DocString: FMTYieldModelDecisionTree::operator = (const FMTYieldModelDecisionTree& rhs)
		/**
		Default equality operator.
		*/
		FMTYieldModelDecisionTree& operator = (const FMTYieldModelDecisionTree& rhs)=default;
		// DocString: FMTYieldModelDecisionTree::~FMTYieldModelDecisionTree()
		/**
		Default destructor.
		*/
		~FMTYieldModelDecisionTree()=default;
		// DocString: FMTYieldModelDecisionTree::FMTYieldModelDecisionTree(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		Construct a FMTYieldModelDecisionTree based on a JSON file and an input yield name list and also on the global mask
		*/
		FMTYieldModelDecisionTree(const boost::property_tree::ptree& jsonProps,const std::vector<std::string>& inputYields,const Core::FMTMask& mainmask);
		// DocString: FMTYieldModelDecisionTree::Clone()
		/**
		Implements FMTYieldModel::Clone().
		*/
		std::unique_ptr<FMTYieldModel>Clone() const;
		// DocString: FMTYieldModelDecisionTree::predict
		/**
		Runs the decision tree so if the values are not set build a naturalGrowth model and run the decision tree and get the decision for all
		periods. Finaly return the predictions.
		*/
		const std::vector<double>predict(const Core::FMTYieldRequest& request) const;
		// DocString: FMTYieldModelDecisionTree::getModelType()
		/**
		Return the modeltype of the FMTYieldModel.
		*/
		static std::string getModelType();
		// DocString: FMTYieldModelDecisionTree::presolve
		/**
		Return a presolved FMTYieldModelDecisionTree
		*/
		virtual std::unique_ptr<FMTYieldModel> presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& newthemes) const;
		// DocString: FMTYieldModelDecisionTree::postSolve
		/**
		Change the output and constraints mask to return to the original constraints and outputs.
		*/
		virtual std::unique_ptr<FMTYieldModel> postSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& basethemes) const;
		// DocString: FMTyieldmodeldecisiontreel::getPeriodicValues
		/**
		Try to turn the FMTYieldModel into periodic constant values. if returns an non empty vector then
		each first dimension is the yield id and each second dimension are the periodic value calculated by the yield.
		*/
		virtual std::vector<std::vector<double>>getPeriodicValues() const;
	};
}

#endif
