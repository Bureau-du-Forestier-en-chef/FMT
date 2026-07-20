/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTOUTPUTNODE_Hm_included
#define FMTOUTPUTNODE_Hm_included

#include "FMTOutputSource.h"
#include <map>

#include "FMTutility.h"

namespace Core
{
class FMTAction;

// DocString: FMTOutputNode
/**
@brief Node of an output representing a set of developments in the graph, holding a source, a factor and a constant.
*/
class FMTEXPORT FMTOutputNode
	{
	public:
		FMTOutputSource source;
		FMTOutputSource factor;
		double constant;
		// DocString: FMTOutputNode()
		/**
		@brief Default constructor for FMTOutputNode.
		*/
		FMTOutputNode();
		// DocString: FMTOutputNode(const FMTOutputSource&, const FMTOutputSource&, const double&)
		/**
		@brief Construct an output node from a source, a factor and a constant.
		@param[in] lsource the output source.
		@param[in] lfactor the factor.
		@param[in] lconstant the constant.
		*/
		FMTOutputNode(const FMTOutputSource& lsource, const FMTOutputSource& lfactor, const double& lconstant);
		// DocString: FMTOutputNode(const FMTOutputNode&)
		/**
		@brief Copy constructor for FMTOutputNode.
		@param[in] rhs the FMTOutputNode to copy.
		*/
		FMTOutputNode(const FMTOutputNode& rhs);
		// DocString: FMTLpHeuristic::FMTOutputNode
		/**
		@brief Construct a general node targeting an action name or actions aggregate, for use with the scheduler heuristic.
		@param[in] generalmask the general mask.
		@param[in] actionaggregate the action name or aggregate.
		*/
		FMTOutputNode(const Core::FMTMask& generalmask,const std::string& actionaggregate);
		// DocString: FMTOutputNode::hash
		/**
		@brief Return the hash of the output node.
		@return the hash value.
		*/
		size_t hash() const;
		// DocString: FMTOutputNode::hashForValue
		/**
		@brief Return the hash of the output node for its value.
		@return the hash value.
		*/
		size_t hashForValue() const;
		// DocString: FMTOutputNode::allowCashdeduction
		/**
		@brief Return true if the output node allows cache deduction.
		@return true if the output node allows cache deduction else false.
		*/
		bool allowCashdeduction() const;
		// DocString: FMTOutputNode::singlePeriod
		/**
		@brief Return true if the output node covers a single period.
		@return true if the output node covers a single period else false.
		*/
		bool singlePeriod() const;
		// DocString: FMTOutputNode::multiPeriod
		/**
		@brief Return true if the output node covers multiple periods.
		@return true if the output node covers multiple periods else false.
		*/
		bool multiPeriod() const;
		// DocString: FMTOutputNode::isPastPeriod
		/**
		@brief Return true if the output node targets a past period.
		@return true if the output node targets a past period else false.
		*/
		bool isPastPeriod() const;
		// DocString: FMTOutputNode::isNull
		/**
		@brief Return true if the output node is null.
		@return true if the output node is null else false.
		*/
		bool isNull() const;
		// DocString: FMTOutputNode::operator<
		/**
		@brief Less than comparison operator of FMTOutputNode.
		@param[in] rhs the output node to compare with.
		@return true if this output node is less than rhs else false.
		*/
		bool operator < (const FMTOutputNode& rhs) const;
		// DocString: FMTOutputNode::operator==
		/**
		@brief Equality comparison operator of FMTOutputNode.
		@param[in] rhs the output node to compare with.
		@return true if both output nodes are equal else false.
		*/
		bool operator == (const FMTOutputNode& rhs) const;
		// DocString: FMTOutputNode::isSubsetOf(const FMTOutputNode&)
		/**
		@brief Return true if this output node is a subset of another.
		@param[in] rhs the other output node.
		@return true if this output node is a subset of rhs else false.
		*/
		bool isSubsetOf(const FMTOutputNode& rhs) const;
		// DocString: FMTOutputNode::isSubsetOf(const FMTOutputNode&,const std::vector<Core::FMTAction>&)
		/**
		@brief Return true if this output node is a subset of another considering the actions.
		@param[in] rhs the other output node.
		@param[in] actions the actions.
		@return true if this output node is a subset of rhs else false.
		*/
		bool isSubsetOf(const FMTOutputNode& rhs,const std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTOutputNode::isSameButDifferentAction
		/**
		@brief Return true if this output node is the same as another but with a different action.
		@param[in] rhs the other output node.
		@return true if same but different action else false.
		*/
		bool isSameButDifferentAction(const FMTOutputNode& rhs) const;
		// DocString: FMTOutputNode::getOutputId
		/**
		@brief Return the output id of the output node.
		@return the output id.
		*/
		inline int getOutputId() const
			{
			return source.getOutputOrigin();
			}
		// DocString: FMTOutputNode::isActionbased
		/**
		@brief Return true if the output node is action based.
		@return true if the output node is action based else false.
		*/
		inline bool isActionbased() const
			{
			return source.isAction();
			}
		// DocString: FMTOutputNode::isSameValues
		/**
		@brief Return true if this output node has the same values as another.
		@param[in] rhs the other output node.
		@return true if same values else false.
		*/
		bool isSameValues(const FMTOutputNode& rhs) const;
		// DocString: FMTOutputNode::operator std::string
		/**
		@brief Return the string representation of the output node.
		@return the string representation of the output node.
		*/
		operator std::string() const;
		// DocString: FMTOutputNode::getHashString
		/**
		@brief Return the hash string of the output node.
		@return the hash string.
		*/
		std::string getHashString() const;
		// DocString: FMTOutputNode::getHashMask
		/**
		@brief Return the hash mask of the output node.
		@return the hash mask.
		*/
		Core::FMTMask getHashMask() const;
		// DocString: FMTOutputNode::fillHashMaskSpec
		/**
		@brief Fill a base mask with the hash mask specification of the output node.
		@param[in,out] baseMask the base mask to fill.
		*/
		void fillHashMaskSpec(Core::FMTMask& baseMask) const;
		// DocString: FMTOutputNode::setPeriod
		/**
		@brief Return a copy of the output node set to a given period.
		@param[in] period the period.
		@return the output node set to the period.
		*/
		FMTOutputNode setPeriod(int period) const;
		// DocString: FMTOutputNode::operator=
		/**
		@brief Copy assignment operator for FMTOutputNode.
		@param[in] rhs the FMTOutputNode to copy.
		@return a reference to this FMTOutputNode.
		*/
		FMTOutputNode& operator = (const FMTOutputNode& rhs);
		// DocString: FMTOutputNode::setToGraph
		/**
		@brief Set the output node to the graph for the targeted periods.
		@param[in,out] targetedperiods the targeted periods.
		@param[in] period the period.
		@param[in] max_period the maximum period.
		@return the resulting value.
		*/
		int setToGraph(std::vector<int>& targetedperiods, int period,int max_period);
		// DocString: ~FMTOutputNode()
		/**
		@brief Default destructor for FMTOutputNode.
		*/
		~FMTOutputNode()=default;
		// DocString: FMTOutputNode::fillUpEquation
		/**
		@brief Fill up the equation of the output node.
		@param[in,out] allequations the equations.
		@param[in] graphvalues the graph values.
		@param[in] equation the equation.
		@param[in] nodeid the node id.
		*/
		void fillUpEquation(std::map<std::string,std::vector<std::string>>& allequations,
			const std::map<std::string, double>& graphvalues,
			const std::vector<std::string>& equation, const size_t& nodeid) const;
	};

// DocString: FMTOutputNodeValueComparator
/**
@brief Comparator comparing output nodes by value.
*/
class FMTOutputNodeValueComparator
	{
	public:
		// DocString: FMTOutputNodeValueComparator::operator()
		/**
		@brief Compare two output nodes by value.
		@param[in] node1 the first output node.
		@param[in] node2 the second output node.
		@return true if node1 is less than node2 else false.
		*/
		bool operator()(const FMTOutputNode& node1, const FMTOutputNode& node2) const;

	};

// DocString: FMTOutputNodeOriginComparator
/**
@brief Comparator comparing output nodes by origin.
*/
class FMTOutputNodeOriginComparator
{
public:
	// DocString: FMTOutputNodeOriginComparator::operator()
	/**
	@brief Compare two output nodes by origin.
	@param[in] node1 the first output node.
	@param[in] node2 the second output node.
	@return true if node1 is less than node2 else false.
	*/
	bool operator()(const FMTOutputNode& node1, const FMTOutputNode& node2) const;

};

// DocString: FMTOutputNodeHasher
/**
@brief Hasher for output nodes.
*/
class FMTOutputNodeHasher 
	{
	public:
		// DocString: FMTOutputNodeHasher::operator()
		/**
		@brief Return the hash of an output node.
		@param[in] node the output node.
		@return the hash value.
		*/
		size_t operator()(const FMTOutputNode & node) const;
	};


}

BOOST_CLASS_EXPORT_KEY(Core::FMTOutputNode)

#endif
