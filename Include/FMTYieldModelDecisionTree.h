
#include "FMTYieldModel.h"
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
	@brief Yield model using a decision tree based on the growth outputs of the model to get yield values.
	*/
	class FMTYieldModelDecisionTree : public FMTYieldModel
	{
	
	public:
		// DocString: FMTYieldModelDecisionTree::FMTYieldModelDecisionTree()
		/**
		@brief Default constructor for FMTYieldModelDecisionTree.
		*/
		FMTYieldModelDecisionTree() = default;
		// DocString: FMTYieldModelDecisionTree::FMTYieldModelDecisionTree(const FMTYieldModelDecisionTree& rhs)
		/**
		@brief Copy constructor for FMTYieldModelDecisionTree.
		@param[in] rhs the FMTYieldModelDecisionTree to copy.
		*/
		FMTYieldModelDecisionTree(const FMTYieldModelDecisionTree& rhs);
		// DocString: FMTYieldModelDecisionTree::operator = (const FMTYieldModelDecisionTree& rhs)
		/**
		@brief Copy assignment operator for FMTYieldModelDecisionTree.
		@param[in] rhs the FMTYieldModelDecisionTree to copy.
		@return a reference to this FMTYieldModelDecisionTree.
		*/
		FMTYieldModelDecisionTree& operator = (const FMTYieldModelDecisionTree& rhs)=default;
		// DocString: FMTYieldModelDecisionTree::~FMTYieldModelDecisionTree()
		/**
		@brief Default destructor for FMTYieldModelDecisionTree.
		*/
		~FMTYieldModelDecisionTree()=default;
		// DocString: FMTYieldModelDecisionTree::FMTYieldModelDecisionTree(const boost::property_tree::ptree& jsonProps, std::vector<std::string>& inputYields)
		/**
		@brief Construct a decision tree yield model from a JSON tree, an input yield name list and the global mask.
		@param[in] jsonProps the JSON properties.
		@param[in] inputYields the input yield names.
		@param[in] mainmask the global mask.
		*/
		FMTYieldModelDecisionTree(const boost::property_tree::ptree& jsonProps,const std::vector<std::string>& inputYields,const Core::FMTMask& mainmask);
		// DocString: FMTYieldModelDecisionTree::clone()
		/**
		@brief clone the yield model.
		@return a unique pointer to the cloned yield model.
		*/
		std::unique_ptr<FMTYieldModel>clone() const;
		// DocString: FMTYieldModelDecisionTree::predict
		/**
		@brief Run the decision tree, building a natural growth model if needed, and return the predictions.
		@param[in] request the yield request.
		@return the predicted values.
		*/
		const std::vector<double>predict(const Core::FMTYieldRequest& request) const;
		// DocString: FMTYieldModelDecisionTree::getModelType()
		/**
		@brief Return the model type of the yield model.
		@return the model type.
		*/
		static std::string getModelType();
		// DocString: FMTYieldModelDecisionTree::preSolve
		/**
		@brief Return a presolved copy of the yield model.
		@param[in] filter the mask filter.
		@param[in] newthemes the presolved themes.
		@return the presolved yield model.
		*/
		virtual std::unique_ptr<FMTYieldModel> preSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& newthemes) const;
		// DocString: FMTYieldModelDecisionTree::postSolve
		/**
		@brief Return a postsolved copy of the yield model, restoring the original output and constraint masks.
		@param[in] filter the mask filter.
		@param[in] basethemes the original themes.
		@return the postsolved yield model.
		*/
		virtual std::unique_ptr<FMTYieldModel> postSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& basethemes) const;
		// DocString: FMTyieldmodeldecisiontreel::getPeriodicValues
		/**
		@brief Try to turn the yield model into periodic constant values.
		@return a vector where the first dimension is the yield id and the second is the periodic values, empty if not possible.
		*/
		virtual std::vector<std::vector<double>>getPeriodicValues() const;
	private:
		// DocString: FMTYieldModelDecisionTree::m_mtx
		///Mutex for naturalGrowth.
		//mutable std::recursive_mutex m_mtx;
		mutable boost::recursive_mutex m_mtx;
		// DocString: FMTYieldModelDecisionTree::m_values
		///Map of resulting yield id with the decision made per period in a vector
		mutable std::map<size_t, std::vector<double>>m_values;
		// DocString: FMTYieldModelDecisionTree::m_reference
		///The reference output for the ratio
		std::unique_ptr<FMTOutput> m_reference;
		// DocString: FMTYieldModelDecisionTree::m_nodes
		///The decision nodes constraints are used for bounds
		std::vector<FMTConstraint>m_nodes;
		// DocString: FMTYieldModelDecisionTree::m_default_values
		///Yields default values if update period
		std::vector<double>m_default_values;
		// DocString: FMTYieldModelDecisionTree::m_update
		///Update period yield string.
		std::string m_update;
		// DocString: FMTYieldModelDecisionTree::m_JSON_PROP_TREE
		///The json tree key
		const std::string m_JSON_PROP_TREE = "Tree";
		// DocString: FMTYieldModelDecisionTree::m_JSON_PROP_DECISIONS
		///The json decisions key
		const std::string m_JSON_PROP_DECISIONS = "Decisions";
		// DocString: FMTYieldModelDecisionTree::m_JSON_PROP_REFERENCE
		///The json Reference key
		const std::string m_JSON_PROP_REFERENCE = "Reference";
		// DocString: FMTYieldModelDecisionTree::m_JSON_PROP_DEFAULT
		///The json default values key
		const std::string m_JSON_PROP_DEFAULT = "Default";
		// DocString: FMTYieldModelDecisionTree::m_JSON_PROP_UPDATE
		///The json default values key
		const std::string m_JSON_PROP_UPDATE = "Update";
		// DocString: FMTYieldModelDecisionTree::_getMask
		/**
		@brief Return the mask of the decision tree, a valid mask of the reference output.
		@return the mask.
		*/
		Core::FMTMask _getMask() const;
		// DocString: FMTYieldModelDecisionTree::_buildConstraint
		/**
		@brief Build a constraint on a yield with a lower and upper bound.
		@param[in] name the constraint name.
		@param[in] yld the yield.
		@param[in] mask the main mask.
		@param[in] lowerbound the lower bound.
		@param[in] upperbound the upper bound.
		@param[in] lag the number of periods added or removed when getting the constraint output values.
		@return the constraint.
		*/
		Core::FMTConstraint _buildConstraint(const std::string& name, const std::string& yld, const Core::FMTMask& mask, const double& lowerbound, const double& upperbound, const int& lag) const;
		// DocString: FMTYieldModelDecisionTree::_getADecision
		/**
		@brief Return a new constraint id based on the natural growth and the period using the decision tree.
		@param[in] p_naturalGrowth the natural growth model.
		@param[in] p_constraint_id the constraint id.
		@param[in] p_period the period.
		@param[in] p_update the update period.
		@return the new constraint id.
		*/
		size_t _getADecision(const std::unique_ptr<Models::FMTModel>& p_naturalGrowth, 
			size_t p_constraint_id, int p_period, int p_update) const;
		// DocString: FMTYieldModelDecisionTree::_getNaturalGrowth
		/**
		@brief Build a small natural growth model dedicated to the reference output for a request, in a thread safe way.
		@param[in] request the yield request.
		@return the natural growth model.
		*/
		std::unique_ptr<Models::FMTModel> _getNaturalGrowth(const Core::FMTYieldRequest& request) const;
		// DocString: FMTYieldModelDecisionTree::_modify
		/**
		@brief Modify the model by presolving or postsolving it.
		@param[in] filter the mask filter.
		@param[in] newthemes the themes.
		@param[in] preSolve if true presolves, otherwise postsolves.
		@return the modified yield model.
		*/
		std::unique_ptr<FMTYieldModel> _modify(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& newthemes, bool preSolve = true) const;
		// DocString: FMTYieldModelDecisionTree::_getUpdatePeriod
		/**
		@brief Get the update period based on the model inputs or the m_model parameters if not in inputs
		@param[in] request the yield request.
		@return the update period.
		*/
		int _getUpdatePeriod(const Core::FMTYieldRequest& p_request) const;
	};
}

#endif
