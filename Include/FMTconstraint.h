/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTconstraint_Hm_included
#define FMTconstraint_Hm_included

#include "FMToutput.h"
#include <vector>
#include <string>
#include <limits>
#include "FMTbounds.hpp"
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
Constraints and objectives types found in a regular optimization section
also some spatial size/adjacency and greenup constraints for the SAmodel. 
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
FMTConstraint is a representation of a line in the optimize section. an objective is considered a constraint in FMT.
So even for the object a FMTConstraint will be generated using a optimization section.
A FMTConstraint is also a FMTOutput with some specification (FMTSpec).
The main specification used is the period bounds (1.._LENGTH), RHS and variation (_GOAL).
*/
class FMTEXPORT FMTConstraint: public FMTOutput,public FMTSpec
	{
	public:
		// DocString: FMTConstraint()
		/**
		Default constructor for FMTConstraint.
		*/
		FMTConstraint();
		// DocString: FMTConstraint(FMTconstrainttype,const FMTOutput&)
		/**
		Partial constructor for FMTConstraint specifying the output and the constraint type.
		*/
		FMTConstraint(FMTconstrainttype ltype, const FMTOutput& loutput);
		// DocString: FMTConstraint(const FMTConstraint&)
		/**
		Copy constructor for FMTConstraint.
		*/
		FMTConstraint(const FMTConstraint& rhs);
		// DocString: FMTConstraint::operator=
		/**
		Copy assignment for FMTConstraint.
		*/
		FMTConstraint& operator = (const FMTConstraint& rhs);
		// DocString: FMTConstraint::operator==
		/**
		FMTConstraint equality operator check if FMTConstraint are the same.
		*/
		bool operator == (const FMTConstraint& rhs) const;
		// DocString: FMTConstraint::operator!=
		/**
		FMTConstraint not equality operator check if FMTConstraint are not the same.
		*/
		bool operator != (const FMTConstraint& rhs) const;
		// DocString: FMTConstraint::extraVariables
		/**
		Returns true if the FMTConstraint need extraVariables in the matrix formulation.
		*/
		bool extraVariables() const;
		// DocString: FMTConstraint::isGoal
		/**
		Returns true if the constraint have goal weight
		*/
		bool isGoal() const;
		// DocString: FMTConstraint::isSetFrom
		/**
		Returns true if the constraint needs to be set from a specific model.
		*/
		bool isSetFrom(const std::string& modeltype) const;
		// DocString: FMTConstraint::isReIgnore
		/**
		Returns true if we need to ignore the constraint from the replanning period
		*/
		bool isReIgnore(const int& replanningperiod) const;
		// DocString: FMTConstraint::doSupportRandom
		/**
		Return true if the constraint has only one source and only based on action area.
		So the contraint will support _RANDOM keywod.
		*/
		bool doSupportRandom() const;
		// DocString: FMTConstraint::setFrom
		/**
		Using the value of the global model for the constraint output it will
		return a new constraints set with the model type value.
		*/
		Core::FMTConstraint setFrom(const std::string& modeltype,const double& value) const;
		// DocString: FMTConstraint::getScheduleWeight
		/**
		If you are an objective and are a local model you can have a global schedule weight in the objective function.
		*/
		double getScheduleWeight() const;
		// DocString: FMTConstraint::getFromReplicate
		/**
		If the constraint is build with a replicate tables it will gives a new constraint with the corresponding replicate value
		See keyword _REPLICATE in optimization seciton.
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
		@brief check if contains replicate
		@param[in] the period.
		@return true if contains replicate
		*/
		bool gotReplicate(const int& p_period) const;
		// DocString: FMTConstraint::getPenalties
		/**
		If the constraint is goal then you can get the name of the penalties of the constraint. like _GOAL(penaltyname,weight)
		*/
		std::vector<std::string>getPenalties(double & sense) const;
		// DocString: FMTConstraint::getVariableLevels
		/**
		If the constraint is a level this function will return the level name of the constraint.
		*/
		std::vector<std::string>getVariableLevels() const;
		// DocString: FMTConstraint::getGoal
		/**
		If the constraint has a goal this function fill up the name of the goal and it's weight.
		*/
		void getGoal(std::string& name,double& value) const;
		// DocString: FMTConstraint::setGoal
		/**
		If the constraint is not an objective set a goal of goalname with a value
		*/
		void setGoal(const std::string& goalname, const double& value);
		// DocString: FMTConstraint::setPenalties
		/**
		If the constraint is an objectif it will set penalties
		*/
		void setPenalties(const std::string& penaltyoperator, const std::vector<std::string>& variables);
		// DocString: FMTConstraint::isObjective
		/**
		Returns true if the constraint is an objective.
		*/
		bool isObjective() const;
		// DocString: FMTConstraint::sense
		/**
		If the constraint is an objective the function returns the sense of the objective,
		1 = minimisation, -1 = maximisation.
		*/
		double sense() const;
		// DocString: FMTConstraint::getBounds
		/**
		This function fills up the (lower) and (upper) bound for a given period, if
		the constraint can be applied to this (period). Use with osisolverinterfacer with rowsetbounds().
		*/
		void getBounds(double& lower, double& upper,int period = 0) const;
		// DocString: FMTConstraint::setRhs
		/**
		This functions sets the RHS of this FMTConstraint with a row (lower) and (upper) bound.
		*/
		void setRhs(double lower,double upper);
		// DocString: FMTConstraint::setLength
		/**
		This functions sets the length of the FMTConstraint
		*/
		void setLength(int firstPeriod = 1,int lastPeriod = std::numeric_limits<int>::max());
		// DocString: FMTConstraint::getVariations
		/**
		Get the variation for constraint like _EVEN _SEQ,_NDY etc... _EVEN(lowervariation,uppervariation).
		*/
		void getVariations(double& lower, double& upper) const;
		// DocString: FMTConstraint::isMultiple
		/**
		Returns true if the FMTConstraint allow some sort of variability like _EVEN(lowervariation,uppervariation).
		*/
		bool isMultiple() const;
		// DocString: FMTConstraint::acrossPeriod
		/**
		Returns true if the FMTConstraint needs so be set across multiple periods like _EVEN,_SEQ,_NDY
		*/
		bool acrossPeriod() const;
		// DocString: FMTConstraint::hash
		/**
		Hasghing for FMTConstraint.
		*/
		size_t hash(bool hashrhs = true,bool hashoutputonly = false) const;
		// DocString: FMTConstraint::hasHasOutput
		/**
		Hasghing for FMTOutput.
		*/
		size_t hasHasOutput() const;
		// DocString: FMTConstraint::setOutput
		/**
		Setter for the FMTOutput of the FMTConstraint.
		*/
		void setOutput(const FMTOutput& out);
		// DocString: FMTConstraint::setConstraintType
		/**
		Setter for the FMTconstrainttype of FMTConstraint.
		*/
		void setConstraintType(FMTconstrainttype ltype);
		// DocString: FMTConstraint::getConstraintType
		/**
		Getter for the constrainttype.
		*/
		FMTconstrainttype getConstraintType() const;
		// DocString: FMTConstraint::operator std::string
		/**
		Get the string reprensentation like in the optimization section  of a FMTConstraint.
		*/
		operator std::string() const;
		// DocString: FMTConstraint::isSpatial
		/**
		Returns true if spatial constraint
		*/
		bool isSpatial() const;
		// DocString: FMTConstraint::isRandomAction
		/**
		Returns true if spatial constraint
		*/
		bool isRandomAction() const;
		// DocString: FMTConstraint::getGroup
		/**
		Returns the group of the constraint (objective = 0,spatial = 1,other = 2 or userdefined)
		*/
		size_t getGroup() const;
		// DocString: FMTConstraint::outputEmpty
		/**
		Returns true if the FMTConstraint output is empty.
		*/
		size_t outputEmpty() const;
		// DocString: FMTConstraint::presolve
		/**
		Like FMTOutput class this function presolve the FMTConstraint and can returns an empty FMTConstraint if 
		based on a (baseMask), the (originalthemes), the presolved themes, the presolved actions and presolved yields.
		See FMTOutput presolve for more information.
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
		@brief Presolve this constraint
		@param[in] p_filter
		@param[in] p_originalThemes
		@param[in] p_selectedThemes
		@param[in] p_newThemes
		@param[in] p_actions
		@param[in] p_valideActions
		@param[in] p_yields
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
		Default destructor for FMTConstraint
		*/
		~FMTConstraint()=default;
		// DocString: FMTConstraint::evaluate
		/**
		Given a vector of double this function will return a double value representing the quality of the solution.
		For a real constraint the more the value is close to 0 the better it is. For an objective smaller is better (can be negative).
		*/
		double evaluate(const std::vector<double>& temporalvalues) const;
		// DocString: FMTConstraint::getActionIds
		/**
		If the constraint is a spatial constraint it will return a vector of action ids to which the constraint apply to.
		*/
		std::vector<int>getActionIds(const std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTConstraint::isActionsused
		/**
		If the constraint is a spatial constraint it will return a vector of action ids to which the constraint apply to.
		*/
		std::vector<bool>isActionsused(const std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTConstraint::canBeTurnedToYieldsBasedOnTransitions
		/**
		If the constraint can be turned to a simple action bound using a simple yield by checking the transitions before changing it, this function will returns true. 
		*/
		bool canBeTurnedToYieldsBasedOnTransitions() const;
		// DocString: FMTConstraint::turnToYieldsBasedOnTransition
		/**
		This function check for 
		*/
		void turnToYieldsBasedOnTransition(	const std::vector<Core::FMTTheme>& themes,
											const std::vector<Core::FMTTransition>& trans,
											std::vector<Core::FMTAction>&actions,
											const std::vector<bool>& p_valideActions,
											Core::FMTYields& yields,
											const int& constraintid) const;
		// DocString: FMTConstraint::canBeTurnedToYields
		/**
		If the constraint can be turned to a simple action bound using a simple yield this function will returns true.
		*/
		bool canBeTurnedToYields() const;
		// DocString: FMTConstraint::turnToYieldsAndActions
		/**
		This function will generate two yields handler from the constraint 1st: value = 0 for specific mask then the default handler.
		You need to specify the constraint id to generate a decent yield name. It will modify the actions in the action vectors.
		Then the yields will be included in the yields section. The constraint can be ignored after this.
		This will help reduce the size of the matrix.
		*/
		void turnToYieldsAndActions(const std::vector<Core::FMTTheme>& themes,
						 std::vector<Core::FMTAction>&actions,
						const std::vector<bool>& p_valideActions,
						 Core::FMTYields& yields,
						 const int& constraintid) const;

		int getThemeTarget() const;
		double getWeight() const;
	private:
		// DocString: FMTConstraint::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		Function used for string formating for FMTConstraint in to string conversion.
		*/
		void standardString(std::string& line, std::string& period_bounds,
			std::string& goal, std::string& global,bool asInt = false) const;
		// DocString: FMTConstraint::getMaxAndMin
		/**
		Returns the maximal and minimal value of a vector of double for the constraint
		*/
		void getMaxAndMin(const std::vector<double>& values, double& min, double& max) const;
		// DocString: FMTConstraint::getSum
		/**
		Returns the sum of a vector of double
		*/
		double getSum(const std::vector<double>& values) const;
		// DocString: FMTConstraint::getPeriodicVariationCost
		/**
		Returns the variation of harvest for a vector of double
		*/
		double getPeriodicVariationCost(const std::vector<double>& values, bool evaluateupper = false) const;
		// DocString: FMTConstraint::getVariability
		/**
		Returns the variability based on a double input
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
		@brief Based on constraint period it will set bounds to the double specified in the vector for the constraint period.
		The target period is only used if the constraint does not have a specific period.
		@param[in] p_PeriodChanges the period to get the new contraint.
		*/
		Core::FMTConstraint _getIterationChange(double p_PeriodChanges) const;

	};
}

BOOST_CLASS_EXPORT_KEY(Core::FMTConstraint)
#endif
