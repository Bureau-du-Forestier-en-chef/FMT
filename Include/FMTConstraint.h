/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTconstraint_Hm_included
#define FMTconstraint_Hm_included

#include "FMTOutput.h"
#include <vector>
#include <string>
#include <limits>
#include "FMTBounds.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Core
{
class FMTTransition;
class FMTTheme;
class FMTAction;
class FMTYields;
// DocString: FMTconstrainttype
/**
@brief Enumerator of the constraint and objective types found in an optimization section, including spatial constraints for the SA model.
*/
enum  FMTconstrainttype
{
	FMTMAXobjective = 1,
	FMTMINobjective = 2,
	FMTMAXMINobjective = 3,
	FMTMINMAXobjective = 4,
	FMTevenflow = 5,
	FMTnondeclining = 6,
	FMTsequence = 7,
	FMTstandard = 8,///Regular constraint type like output = 1...
	FMTspatialsize = 9,
	FMTspatialadjacency = 10,
	FMTspatialgreenup = 11,
	FMTSpatialGroup = 12,
	FMTrandomaction = 13//need to be deleted
};
// DocString: FMTConstraint
/**
@brief Representation of a line in the optimize section, an objective being considered a constraint.
@details A FMTConstraint is also a FMTOutput with a specification (FMTSpec). The main specification used is the period bounds, RHS and variation.
*/
class FMTEXPORT FMTConstraint: public FMTOutput,public FMTSpec
	{
	public:
		// DocString: FMTConstraint()
		/**
		@brief Default constructor for FMTConstraint.
		*/
		FMTConstraint();
		// DocString: FMTConstraint(FMTconstrainttype,const FMTOutput&)
		/**
		@brief Partial constructor specifying the output and the constraint type.
		@param[in] ltype the constraint type.
		@param[in] loutput the output.
		*/
		FMTConstraint(FMTconstrainttype ltype, const FMTOutput& loutput);
		// DocString: FMTConstraint(const FMTConstraint&)
		/**
		@brief Copy constructor for FMTConstraint.
		@param[in] rhs the FMTConstraint to copy.
		*/
		FMTConstraint(const FMTConstraint& rhs);
		// DocString: FMTConstraint::operator=
		/**
		@brief Copy assignment operator for FMTConstraint.
		@param[in] rhs the FMTConstraint to copy.
		@return a reference to this FMTConstraint.
		*/
		FMTConstraint& operator = (const FMTConstraint& rhs);
		// DocString: FMTConstraint::operator==
		/**
		@brief Equality comparison operator of FMTConstraint.
		@param[in] rhs the constraint to compare with.
		@return true if both constraints are equal else false.
		*/
		bool operator == (const FMTConstraint& rhs) const;
		// DocString: FMTConstraint::operator!=
		/**
		@brief Inequality comparison operator of FMTConstraint.
		@param[in] rhs the constraint to compare with.
		@return true if both constraints are different else false.
		*/
		bool operator != (const FMTConstraint& rhs) const;
		// DocString: FMTConstraint::extraVariables
		/**
		@brief Return true if the constraint needs extra variables in the matrix formulation.
		@return true if extra variables are needed else false.
		*/
		bool extraVariables() const;
		// DocString: FMTConstraint::isGoal
		/**
		@brief Return true if the constraint has a goal weight.
		@return true if the constraint has a goal weight else false.
		*/
		bool isGoal() const;
		// DocString: FMTConstraint::isSetFrom
		/**
		@brief Return true if the constraint needs to be set from a specific model.
		@param[in] modeltype the model type.
		@return true if the constraint needs to be set from the model else false.
		*/
		bool isSetFrom(const std::string& modeltype) const;
		// DocString: FMTConstraint::isReIgnore
		/**
		@brief Return true if the constraint must be ignored from the replanning period.
		@param[in] replanningperiod the replanning period.
		@return true if the constraint must be ignored else false.
		*/
		bool isReIgnore(const int& replanningperiod) const;
		// DocString: FMTConstraint::doSupportRandom
		/**
		@brief Return true if the constraint has only one source based on action area and supports the _RANDOM keyword.
		@return true if the constraint supports random else false.
		*/
		bool doSupportRandom() const;
		// DocString: FMTConstraint::setFrom
		/**
		@brief Return a new constraint set with the value of the given model type.
		@param[in] modeltype the model type.
		@param[in] value the value of the global model output.
		@return the constraint set from the model.
		*/
		Core::FMTConstraint setFrom(const std::string& modeltype,const double& value) const;
		// DocString: FMTConstraint::getScheduleWeight
		/**
		@brief Return the global schedule weight of the objective for a local model.
		@return the schedule weight.
		*/
		double getScheduleWeight() const;
		// DocString: FMTConstraint::getFromReplicate
		/**
		@brief Return a new constraint with the replicate value, if the constraint is built with a replicate table. See _REPLICATE.
		@param[in] replicate the replicate.
		@param[in] period the period.
		@return the constraint with the replicate value.
		*/
		Core::FMTConstraint getFromReplicate(const size_t& replicate,const int& period) const;
		// DocString: FMTConstraint::setFromReplicate
		/**
		@brief If the constraint is build with a replicate tables it will gives a new constraint with the corresponding replicate value
		See keyword _REPLICATE in optimization seciton.
		@param[in] p_replicate the replanning replicate
		@param[in] p_period the period to calculate.
		*/
		void setFromReplicate(size_t p_replicate,int p_period);
		
		// DocString: FMTConstraint::gotReplicate
		/**
		@brief Check if the constraint contains a replicate.
		@param[in] p_period the period.
		@return true if the constraint contains a replicate else false.
		*/
		bool gotReplicate(const int& p_period) const;
		// DocString: FMTConstraint::getPenalties
		/**
		@brief Return the names of the penalties of the constraint if it is a goal, like _GOAL(penaltyname, weight).
		@param[out] sense the sense of the penalties.
		@return the penalty names.
		*/
		std::vector<std::string>getPenalties(double & sense) const;
		// DocString: FMTConstraint::getVariableLevels
		/**
		@brief Return the level names of the constraint if it is a level.
		@return the level names.
		*/
		std::vector<std::string>getVariableLevels() const;
		// DocString: FMTConstraint::getGoal
		/**
		@brief Fill up the name and weight of the goal of the constraint.
		@param[out] name the goal name.
		@param[out] value the goal weight.
		*/
		void getGoal(std::string& name,double& value) const;
		// DocString: FMTConstraint::setGoal
		/**
		@brief Set a goal with a name and a value if the constraint is not an objective.
		@param[in] goalname the goal name.
		@param[in] value the goal value.
		*/
		void setGoal(const std::string& goalname, const double& value);
		// DocString: FMTConstraint::setPenalties
		/**
		@brief Set the penalties if the constraint is an objective.
		@param[in] penaltyoperator the penalty operator.
		@param[in] variables the penalty variables.
		*/
		void setPenalties(const std::string& penaltyoperator, const std::vector<std::string>& variables);
		// DocString: FMTConstraint::isObjective
		/**
		@brief Return true if the constraint is an objective.
		@return true if the constraint is an objective else false.
		*/
		bool isObjective() const;
		// DocString: FMTConstraint::sense
		/**
		@brief Return the sense of the objective, 1 for minimisation and -1 for maximisation.
		@return the sense of the objective.
		*/
		double sense() const;
		// DocString: FMTConstraint::getBounds
		/**
		@brief Fill up the lower and upper bound for a given period if the constraint applies to it.
		@param[out] lower the lower bound.
		@param[out] upper the upper bound.
		@param[in] period the period.
		*/
		void getBounds(double& lower, double& upper,int period = 0) const;
		// DocString: FMTConstraint::setRhs
		/**
		@brief Set the RHS of the constraint with a lower and upper bound.
		@param[in] lower the lower bound.
		@param[in] upper the upper bound.
		*/
		void setRhs(double lower,double upper);
		// DocString: FMTConstraint::setLength
		/**
		@brief Set the length of the constraint.
		@param[in] firstPeriod the first period.
		@param[in] lastPeriod the last period.
		*/
		void setLength(int firstPeriod = 1,int lastPeriod = std::numeric_limits<int>::max());
		// DocString: FMTConstraint::getVariations
		/**
		@brief Get the variation for constraints like _EVEN, _SEQ or _NDY.
		@param[out] lower the lower variation.
		@param[out] upper the upper variation.
		*/
		void getVariations(double& lower, double& upper) const;
		// DocString: FMTConstraint::isMultiple
		/**
		@brief Return true if the constraint allows some variability like _EVEN.
		@return true if the constraint is multiple else false.
		*/
		bool isMultiple() const;
		// DocString: FMTConstraint::acrossPeriod
		/**
		@brief Return true if the constraint needs to be set across multiple periods like _EVEN, _SEQ or _NDY.
		@return true if the constraint is across periods else false.
		*/
		bool acrossPeriod() const;
		// DocString: FMTConstraint::hash
		/**
		@brief Return the hash of the constraint.
		@param[in] hashrhs if true includes the RHS in the hash.
		@param[in] hashoutputonly if true hashes the output only.
		@return the hash value.
		*/
		size_t hash(bool hashrhs = true,bool hashoutputonly = false) const;
		// DocString: FMTConstraint::hasHasOutput
		/**
		@brief Return the hash of the output of the constraint.
		@return the hash of the output.
		*/
		size_t hasHasOutput() const;
		// DocString: FMTConstraint::setOutput
		/**
		@brief Set the output of the constraint.
		@param[in] out the output to set.
		*/
		void setOutput(const FMTOutput& out);
		// DocString: FMTConstraint::setConstraintType
		/**
		@brief Set the type of the constraint.
		@param[in] ltype the constraint type to set.
		*/
		void setConstraintType(FMTconstrainttype ltype);
		// DocString: FMTConstraint::getConstraintType
		/**
		@brief Return the type of the constraint.
		@return the constraint type.
		*/
		FMTconstrainttype getConstraintType() const;
		// DocString: FMTConstraint::operator std::string
		/**
		@brief Return the string representation of the constraint as in the optimization section.
		@return the string representation of the constraint.
		*/
		operator std::string() const;
		// DocString: FMTConstraint::isSpatial
		/**
		@brief Return true if the constraint is a spatial constraint.
		@return true if the constraint is spatial else false.
		*/
		bool isSpatial() const;
		// DocString: FMTConstraint::isRandomAction
		/**
		@brief Return true if the constraint is a random action constraint.
		@return true if the constraint is a random action else false.
		*/
		bool isRandomAction() const;
		// DocString: FMTConstraint::getGroup
		/**
		@brief Return the group of the constraint (objective = 0, spatial = 1, other = 2 or user defined).
		@return the group of the constraint.
		*/
		size_t getGroup() const;
		// DocString: FMTConstraint::outputEmpty
		/**
		@brief Return true if the output of the constraint is empty.
		@return true if the output is empty else false.
		*/
		size_t outputEmpty() const;
		// DocString: FMTConstraint::presolve
		/**
		@brief Presolve the constraint, potentially returning an empty constraint. See FMTOutput::presolve.
		@param[in] filter the mask filter.
		@param[in] originalthemes the original themes.
		@param[in] selectedthemes the selected themes.
		@param[in] newthemes the presolved themes.
		@param[in] actions the actions.
		@param[in] p_valideActions the valid actions.
		@param[in] yields the yields.
		@return the presolved constraint.
		*/
		FMTConstraint presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			const std::vector<const FMTTheme*>& selectedthemes,
			const std::vector<FMTTheme>& newthemes,
			const std::vector<FMTAction>& actions,
			const std::vector<bool>& p_valideActions,
			const FMTYields& yields) const;
		// DocString: FMTConstraint::presolveRef
		/**
		@brief Presolve the constraint in place. See FMTOutput::presolve.
		@param[in] p_filter the mask filter.
		@param[in] p_originalThemes the original themes.
		@param[in] p_selectedThemes the selected themes.
		@param[in] p_newThemes the presolved themes.
		@param[in] p_actions the actions.
		@param[in] p_valideActions the valid actions.
		@param[in] p_yields the yields.
		*/
		void presolveRef(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_originalThemes,
			const std::vector<const FMTTheme*>& p_selectedThemes,
			const std::vector<FMTTheme>& p_newThemes,
			const std::vector<FMTAction>& p_actions,
			const std::vector<bool>& p_valideActions,
			const FMTYields& p_yields);
		// DocString: ~FMTConstraint()
		/**
		@brief Default destructor for FMTConstraint.
		*/
		~FMTConstraint()=default;
		// DocString: FMTConstraint::evaluate
		/**
		@brief Return a value representing the quality of the solution for the constraint.
		@details For a real constraint, the closer to 0 the better; for an objective, smaller is better and can be negative.
		@param[in] temporalvalues the temporal values.
		@return the quality value.
		*/
		double evaluate(const std::vector<double>& temporalvalues) const;
		// DocString: FMTConstraint::getActionIds
		/**
		@brief Return the action ids to which the spatial constraint applies.
		@param[in] actions the actions.
		@return the action ids.
		*/
		std::vector<int>getActionIds(const std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTConstraint::isActionsused
		/**
		@brief Return a vector of booleans, true for the actions to which the spatial constraint applies.
		@param[in] actions the actions.
		@return a vector of booleans, true for used actions.
		*/
		std::vector<bool>isActionsused(const std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTConstraint::canBeTurnedToYieldsBasedOnTransitions
		/**
		@brief Return true if the constraint can be turned to a simple action bound using a simple yield, checking the transitions.
		@param[in] themes the themes.
		@param[in] trans the transitions.
		@param[in] actions the actions.
		@param[in] p_valideActions the valid actions
		@return true if the constraint can be turned to yields else false.
		*/
		bool canBeTurnedToYieldsBasedOnTransitions(const std::vector<Core::FMTTheme>& p_themes,
												const std::vector<Core::FMTTransition>& p_trans,
												const std::vector<Core::FMTAction>& p_actions,
												const std::vector<bool>& p_valideActions) const;
		// DocString: FMTConstraint::turnToYieldsBasedOnTransition
		/**
		@brief Turn the constraint to yields and actions based on the transitions.
		@param[in] themes the themes.
		@param[in] trans the transitions.
		@param[in,out] actions the actions.
		@param[in] p_valideActions the valid actions.
		@param[in,out] yields the yields.
		@param[in] constraintid the constraint id.
		*/
		void turnToYieldsBasedOnTransition(	const std::vector<Core::FMTTheme>& themes,
											const std::vector<Core::FMTTransition>& trans,
											std::vector<Core::FMTAction>&actions,
											const std::vector<bool>& p_valideActions,
											Core::FMTYields& yields,
											const int& constraintid) const;
		// DocString: FMTConstraint::canBeTurnedToYields
		/**
		@brief Return true if the constraint can be turned to a simple action bound using a simple yield.
		@return true if the constraint can be turned to yields else false.
		*/
		bool canBeTurnedToYields() const;
		// DocString: FMTConstraint::turnToYieldsAndActions
		/**
		@brief Generate two yield handlers from the constraint and include them in the yields, modifying the actions, to reduce the matrix size.
		@param[in] themes the themes.
		@param[in,out] actions the actions.
		@param[in] p_valideActions the valid actions.
		@param[in,out] yields the yields.
		@param[in] constraintid the constraint id.
		*/
		void turnToYieldsAndActions(const std::vector<Core::FMTTheme>& themes,
						 std::vector<Core::FMTAction>&actions,
						const std::vector<bool>& p_valideActions,
						 Core::FMTYields& yields,
						 const int& constraintid) const;

		// DocString: FMTConstraint::getThemeTarget
		/**
		@brief Return the theme target of the constraint.
		@return the theme target.
		*/
		int getThemeTarget() const;
		// DocString: FMTConstraint::getWeight
		/**
		@brief Return the weight of the constraint.
		@return the weight.
		*/
		double getWeight() const;
	private:
		// DocString: FMTConstraint::serialize
		/**
		@brief Serialize the FMTConstraint through its bases FMTOutput and FMTSpec for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("output", boost::serialization::base_object<FMTOutput>(*this));
			ar& boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTSpec>(*this));
			ar& BOOST_SERIALIZATION_NVP(type);
		}
		// DocString: FMTConstraint::type
		///This enumerator sets the type of constraint of the FMTConstraint (objective/constraint/evenflow...)
		FMTconstrainttype type;
		// DocString: FMTConstraint::standardString
		/**
		@brief Format the constraint for its string conversion.
		@param[in,out] line the line string.
		@param[in,out] period_bounds the period bounds string.
		@param[in,out] goal the goal string.
		@param[in,out] global the global string.
		@param[in] asInt if true formats as int.
		*/
		void standardString(std::string& line, std::string& period_bounds,
			std::string& goal, std::string& global,bool asInt = false) const;
		// DocString: FMTConstraint::getMaxAndMin
		/**
		@brief Return the maximal and minimal value of a vector of double for the constraint.
		@param[in] values the values.
		@param[out] min the minimal value.
		@param[out] max the maximal value.
		*/
		void getMaxAndMin(const std::vector<double>& values, double& min, double& max) const;
		// DocString: FMTConstraint::getSum
		/**
		@brief Return the sum of a vector of double.
		@param[in] values the values.
		@return the sum of the values.
		*/
		double getSum(const std::vector<double>& values) const;
		// DocString: FMTConstraint::getPeriodicVariationCost
		/**
		@brief Return the periodic variation of harvest for a vector of double.
		@param[in] values the values.
		@param[in] evaluateupper if true evaluates the upper variation.
		@return the periodic variation cost.
		*/
		double getPeriodicVariationCost(const std::vector<double>& values, bool evaluateupper = false) const;
		// DocString: FMTConstraint::getVariability
		/**
		@brief Return the variability based on a vector of double and the variations.
		@param[in] values the values.
		@param[in] var the upper variation.
		@param[in] lowarvar the lower variation.
		@return the variability.
		*/
		double getVariability(const std::vector<double>& values, const double& var, const double& lowarvar) const;
		// DocString: FMTConstraint::_getReplicateValues
		/**
		@brief get the bound values for a specific replicate.
		@param[in] p_replicate the targeted replicate
		@param[in] p_period the period to select the bounds.
		@param[out] p_bound the bound value
		@return true if found else false
		*/
		bool _getReplicateValue(size_t p_replicate, int p_period, double& p_bound) const;
		// DocString: FMTConstraint::_setIterationChange
		/**
		@brief Based on constraint period it will set bounds to the double specified in the vector for the constraint period.
		The target period is only used if the constraint does not have a specific period.
		@param[in] p_bound the bound that we want to set.
		*/
		void _setIterationChange(double p_bound);
		// DocString: FMTConstraint::getiterationchange
		/**
		@brief Return a new constraint with the bounds set for the constraint period from the specified value.
		@param[in] p_PeriodChanges the period to get the new constraint.
		@return the constraint with the iteration change.
		*/
		Core::FMTConstraint _getIterationChange(double p_PeriodChanges) const;

	};
}

BOOST_CLASS_EXPORT_KEY(Core::FMTConstraint)
#endif
