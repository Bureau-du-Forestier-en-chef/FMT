/*
Copyright (c) 2019 Gouvernement du Qubec

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
class FMTtransition;
class FMTtheme;
class FMTaction;
class FMTyields;
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
// DocString: FMTconstraint
/**
FMTconstraint is a representation of a line in the optimize section. an objective is considered a constraint in FMT.
So even for the object a FMTconstraint will be generated using a optimization section.
A FMTconstraint is also a FMToutput with some specification (FMTspec).
The main specification used is the period bounds (1.._LENGTH), RHS and variation (_GOAL).
*/
class FMTEXPORT FMTconstraint: public FMToutput,public FMTspec
	{
	public:
		// DocString: FMTconstraint()
		/**
		Default constructor for FMTconstraint.
		*/
		FMTconstraint();
		// DocString: FMTconstraint(FMTconstrainttype,const FMToutput&)
		/**
		Partial constructor for FMTconstraint specifying the output and the constraint type.
		*/
		FMTconstraint(FMTconstrainttype ltype, const FMToutput& loutput);
		// DocString: FMTconstraint(const FMTconstraint&)
		/**
		Copy constructor for FMTconstraint.
		*/
		FMTconstraint(const FMTconstraint& rhs);
		// DocString: FMTconstraint::operator=
		/**
		Copy assignment for FMTconstraint.
		*/
		FMTconstraint& operator = (const FMTconstraint& rhs);
		// DocString: FMTconstraint::operator==
		/**
		FMTconstraint equality operator check if FMTconstraint are the same.
		*/
		bool operator == (const FMTconstraint& rhs) const;
		// DocString: FMTconstraint::operator!=
		/**
		FMTconstraint not equality operator check if FMTconstraint are not the same.
		*/
		bool operator != (const FMTconstraint& rhs) const;
		// DocString: FMTconstraint::extraVariables
		/**
		Returns true if the FMTconstraint need extraVariables in the matrix formulation.
		*/
		bool extraVariables() const;
		// DocString: FMTconstraint::isGoal
		/**
		Returns true if the constraint have goal weight
		*/
		bool isGoal() const;
		// DocString: FMTconstraint::isSetFrom
		/**
		Returns true if the constraint needs to be set from a specific model.
		*/
		bool isSetFrom(const std::string& modeltype) const;
		// DocString: FMTconstraint::isReIgnore
		/**
		Returns true if we need to ignore the constraint from the replanning period
		*/
		bool isReIgnore(const int& replanningperiod) const;
		// DocString: FMTconstraint::doSupportRandom
		/**
		Return true if the constraint has only one source and only based on action area.
		So the contraint will support _RANDOM keywod.
		*/
		bool doSupportRandom() const;
		// DocString: FMTconstraint::setFrom
		/**
		Using the value of the global model for the constraint output it will
		return a new constraints set with the model type value.
		*/
		Core::FMTconstraint setFrom(const std::string& modeltype,const double& value) const;
		// DocString: FMTconstraint::getScheduleWeight
		/**
		If you are an objective and are a local model you can have a global schedule weight in the objective function.
		*/
		double getScheduleWeight() const;
		// DocString: FMTconstraint::getFromReplicate
		/**
		If the constraint is build with a replicate tables it will gives a new constraint with the corresponding replicate value
		See keyword _REPLICATE in optimization seciton.
		*/
		Core::FMTconstraint getFromReplicate(const size_t& replicate,const int& period) const;
		// DocString: FMTconstraint::setFromReplicate
		/**
		@brief If the constraint is build with a replicate tables it will gives a new constraint with the corresponding replicate value
		See keyword _REPLICATE in optimization seciton.
		@param[in] p_replicate the replanning replicate
		@param[in] p_period the period to calculate.
		*/
		void setFromReplicate(size_t p_replicate,int p_period);
		
		// DocString: FMTconstraint::gotReplicate
		/**
		@brief check if contains replicate
		@param[in] the period.
		@return true if contains replicate
		*/
		bool gotReplicate(const int& p_period) const;
		// DocString: FMTconstraint::getPenalties
		/**
		If the constraint is goal then you can get the name of the penalties of the constraint. like _GOAL(penaltyname,weight)
		*/
		std::vector<std::string>getPenalties(double & sense) const;
		// DocString: FMTconstraint::getVariableLevels
		/**
		If the constraint is a level this function will return the level name of the constraint.
		*/
		std::vector<std::string>getVariableLevels() const;
		// DocString: FMTconstraint::getGoal
		/**
		If the constraint has a goal this function fill up the name of the goal and it's weight.
		*/
		void getGoal(std::string& name,double& value) const;
		// DocString: FMTconstraint::setGoal
		/**
		If the constraint is not an objective set a goal of goalname with a value
		*/
		void setGoal(const std::string& goalname, const double& value);
		// DocString: FMTconstraint::setPenalties
		/**
		If the constraint is an objectif it will set penalties
		*/
		void setPenalties(const std::string& penaltyoperator, const std::vector<std::string>& variables);
		// DocString: FMTconstraint::isObjective
		/**
		Returns true if the constraint is an objective.
		*/
		bool isObjective() const;
		// DocString: FMTconstraint::sense
		/**
		If the constraint is an objective the function returns the sense of the objective,
		1 = minimisation, -1 = maximisation.
		*/
		double sense() const;
		// DocString: FMTconstraint::getBounds
		/**
		This function fills up the (lower) and (upper) bound for a given period, if
		the constraint can be applied to this (period). Use with osisolverinterfacer with rowsetbounds().
		*/
		void getBounds(double& lower, double& upper,int period = 0) const;
		// DocString: FMTconstraint::setRhs
		/**
		This functions sets the RHS of this FMTconstraint with a row (lower) and (upper) bound.
		*/
		void setRhs(double lower,double upper);
		// DocString: FMTconstraint::setLength
		/**
		This functions sets the length of the FMTconstraint
		*/
		void setLength(int firstPeriod = 1,int lastPeriod = std::numeric_limits<int>::max());
		// DocString: FMTconstraint::getVariations
		/**
		Get the variation for constraint like _EVEN _SEQ,_NDY etc... _EVEN(lowervariation,uppervariation).
		*/
		void getVariations(double& lower, double& upper) const;
		// DocString: FMTconstraint::isMultiple
		/**
		Returns true if the FMTconstraint allow some sort of variability like _EVEN(lowervariation,uppervariation).
		*/
		bool isMultiple() const;
		// DocString: FMTconstraint::acrossPeriod
		/**
		Returns true if the FMTconstraint needs so be set across multiple periods like _EVEN,_SEQ,_NDY
		*/
		bool acrossPeriod() const;
		// DocString: FMTconstraint::hash
		/**
		Hasghing for FMTconstraint.
		*/
		size_t hash(bool hashrhs = true,bool hashoutputonly = false) const;
		// DocString: FMTconstraint::hasHasOutput
		/**
		Hasghing for FMToutput.
		*/
		size_t hasHasOutput() const;
		// DocString: FMTconstraint::setOutput
		/**
		Setter for the FMToutput of the FMTconstraint.
		*/
		void setOutput(const FMToutput& out);
		// DocString: FMTconstraint::setConstraintType
		/**
		Setter for the FMTconstrainttype of FMTconstraint.
		*/
		void setConstraintType(FMTconstrainttype ltype);
		// DocString: FMTconstraint::getConstraintType
		/**
		Getter for the constrainttype.
		*/
		FMTconstrainttype getConstraintType() const;
		// DocString: FMTconstraint::operator std::string
		/**
		Get the string reprensentation like in the optimization section  of a FMTconstraint.
		*/
		operator std::string() const;
		// DocString: FMTconstraint::isSpatial
		/**
		Returns true if spatial constraint
		*/
		bool isSpatial() const;
		// DocString: FMTconstraint::isRandomAction
		/**
		Returns true if spatial constraint
		*/
		bool isRandomAction() const;
		// DocString: FMTconstraint::getGroup
		/**
		Returns the group of the constraint (objective = 0,spatial = 1,other = 2 or userdefined)
		*/
		size_t getGroup() const;
		// DocString: FMTconstraint::outputEmpty
		/**
		Returns true if the FMTconstraint output is empty.
		*/
		size_t outputEmpty() const;
		// DocString: FMTconstraint::presolve
		/**
		Like FMToutput class this function presolve the FMTconstraint and can returns an empty FMTconstraint if 
		based on a (basemask), the (originalthemes), the presolved themes, the presolved actions and presolved yields.
		See FMToutput presolve for more information.
		*/
		FMTconstraint presolve(const FMTmaskfilter& filter,
			const std::vector<FMTtheme>& originalthemes,
			const std::vector<const FMTtheme*>& selectedthemes,
			const std::vector<FMTtheme>& newthemes,
			const std::vector<FMTaction>& actions,
			const std::vector<bool>& p_valideActions,
			const FMTyields& yields) const;
		// DocString: FMTconstraint::presolveRef
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
		void presolveRef(const FMTmaskfilter& p_filter,
			const std::vector<FMTtheme>& p_originalThemes,
			const std::vector<const FMTtheme*>& p_selectedThemes,
			const std::vector<FMTtheme>& p_newThemes,
			const std::vector<FMTaction>& p_actions,
			const std::vector<bool>& p_valideActions,
			const FMTyields& p_yields);
		// DocString: ~FMTconstraint()
		/**
		Default destructor for FMTconstraint
		*/
		~FMTconstraint()=default;
		// DocString: FMTconstraint::evaluate
		/**
		Given a vector of double this function will return a double value representing the quality of the solution.
		For a real constraint the more the value is close to 0 the better it is. For an objective smaller is better (can be negative).
		*/
		double evaluate(const std::vector<double>& temporalvalues) const;
		// DocString: FMTconstraint::getActionIds
		/**
		If the constraint is a spatial constraint it will return a vector of action ids to which the constraint apply to.
		*/
		std::vector<int>getActionIds(const std::vector<Core::FMTaction>& actions) const;
		// DocString: FMTconstraint::isActionsused
		/**
		If the constraint is a spatial constraint it will return a vector of action ids to which the constraint apply to.
		*/
		std::vector<bool>isActionsused(const std::vector<Core::FMTaction>& actions) const;
		// DocString: FMTconstraint::canBeTurnedToYieldsBasedOnTransitions
		/**
		If the constraint can be turned to a simple action bound using a simple yield by checking the transitions before changing it, this function will returns true. 
		*/
		bool canBeTurnedToYieldsBasedOnTransitions() const;
		// DocString: FMTconstraint::turnToYieldsBasedOnTransition
		/**
		This function check for 
		*/
		void turnToYieldsBasedOnTransition(	const std::vector<Core::FMTtheme>& themes,
											const std::vector<Core::FMTtransition>& trans,
											std::vector<Core::FMTaction>&actions,
											const std::vector<bool>& p_valideActions,
											Core::FMTyields& yields,
											const int& constraintid) const;
		// DocString: FMTconstraint::canBeTurnedToYields
		/**
		If the constraint can be turned to a simple action bound using a simple yield this function will returns true.
		*/
		bool canBeTurnedToYields() const;
		// DocString: FMTconstraint::turnToYieldsAndActions
		/**
		This function will generate two yields handler from the constraint 1st: value = 0 for specific mask then the default handler.
		You need to specify the constraint id to generate a decent yield name. It will modify the actions in the action vectors.
		Then the yields will be included in the yields section. The constraint can be ignored after this.
		This will help reduce the size of the matrix.
		*/
		void turnToYieldsAndActions(const std::vector<Core::FMTtheme>& themes,
						 std::vector<Core::FMTaction>&actions,
						const std::vector<bool>& p_valideActions,
						 Core::FMTyields& yields,
						 const int& constraintid) const;

		int getThemeTarget() const;
		double getWeight() const;
	private:
		// DocString: FMTconstraint::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("output", boost::serialization::base_object<FMToutput>(*this));
			ar& boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTspec>(*this));
			ar& BOOST_SERIALIZATION_NVP(type);
		}
		// DocString: FMTconstraint::type
		///This enumerator sets the type of constraint of the FMTconstraint (objective/constraint/evenflow...)
		FMTconstrainttype type;
		// DocString: FMTconstraint::standardString
		/**
		Function used for string formating for FMTconstraint in to string conversion.
		*/
		void standardString(std::string& line, std::string& period_bounds,
			std::string& goal, std::string& global,bool asInt = false) const;
		// DocString: FMTconstraint::getMaxAndMin
		/**
		Returns the maximal and minimal value of a vector of double for the constraint
		*/
		void getMaxAndMin(const std::vector<double>& values, double& min, double& max) const;
		// DocString: FMTconstraint::getSum
		/**
		Returns the sum of a vector of double
		*/
		double getSum(const std::vector<double>& values) const;
		// DocString: FMTconstraint::getPeriodicVariationCost
		/**
		Returns the variation of harvest for a vector of double
		*/
		double getPeriodicVariationCost(const std::vector<double>& values, bool evaluateupper = false) const;
		// DocString: FMTconstraint::getVariability
		/**
		Returns the variability based on a double input
		*/
		double getVariability(const std::vector<double>& values, const double& var, const double& lowarvar) const;
		// DocString: FMTconstraint::_getReplicateValues
		/**
		@brief get the bound values for a specific replicate.
		@param[in] p_replicate the targeted replicate
		@param[in] p_period the period to select the bounds.
		@param[out] p_bound the bound value
		@return true if found else false
		*/
		bool _getReplicateValue(size_t p_replicate, int p_period, double& p_bound) const;
		// DocString: FMTconstraint::_setIterationChange
		/**
		@brief Based on constraint period it will set bounds to the double specified in the vector for the constraint period.
		The target period is only used if the constraint does not have a specific period.
		@param[in] p_bound the bound that we want to set.
		*/
		void _setIterationChange(double p_bound);
		// DocString: FMTconstraint::getiterationchange
		/**
		@brief Based on constraint period it will set bounds to the double specified in the vector for the constraint period.
		The target period is only used if the constraint does not have a specific period.
		@param[in] p_PeriodChanges the period to get the new contraint.
		*/
		Core::FMTconstraint _getIterationChange(double p_PeriodChanges) const;

	};
}

BOOST_CLASS_EXPORT_KEY(Core::FMTconstraint)
#endif
