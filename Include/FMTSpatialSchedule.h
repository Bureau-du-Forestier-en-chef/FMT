/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTSpatialSchedule_H
#define FMTSpatialSchedule_H

#include "FMTLayer.hpp"
#include "FMTLineGraph.h"
#include "FMTEventContainer.h"
#include "FMTBindingSpatialAction.h"
#include "FMTVirtualLineGraph.h"
#include "FMTSolutionTracker.h"

namespace Core {
	class FMTGCBMTransition;
	class FMTOutput;
	class FMTModel;
	class FMTConstraint;
	class FMTYields;
	class FMTAction;
	class FMTDevelopment;
}

namespace Models{
	class FMTSeModel;
}



namespace Spatial
{
class FMTForest;
class FMTSpatialGraphs;
class FMTPatchRules;
enum  FMTSpatialScheduletype
	{
		FMTcomplete = 1,
		FMTpartial = 2
	};

// DocString: FMTSpatialSchedule
/**
@brief Map containing a line graph for each cell, representing the stand, the action and the transition at each period for the cell.
@details It can be built randomly or with a schedule.
*/
class FMTEXPORT FMTSpatialSchedule : public FMTLayer<FMTVirtualLineGraph>
{
    public:
		typedef std::vector<std::vector<Spatial::FMTBindingSpatialAction>> actionbindings;
		// DocString: FMTSpatialSchedule(FMTSpatialSchedule&&)
		/**
		@brief Default move constructor for FMTSpatialSchedule.
		@param[in,out] rhs the FMTSpatialSchedule to move from.
		*/
		//FMTSpatialSchedule(FMTSpatialSchedule&& rhs) noexcept;
        // DocString: FMTSpatialSchedule()
        /**
        @brief Default constructor for FMTSpatialSchedule.
        */
        FMTSpatialSchedule()=default;
		// DocString: ~FMTSpatialSchedule()
		/**
		@brief Default destructor for FMTSpatialSchedule.
		*/
		virtual ~FMTSpatialSchedule() = default;
		// DocString: FMTSpatialSchedule::=
		/**
		@brief Copy assignment operator for FMTSpatialSchedule.
		@param[in] rhs the FMTSpatialSchedule to copy.
		@return a reference to this FMTSpatialSchedule.
		*/
		FMTSpatialSchedule& operator=(const FMTSpatialSchedule& rhs) = default;
		// DocString: FMTSpatialSchedule(const FMTSpatialSchedule)
		/**
		@brief Copy constructor for FMTSpatialSchedule.
		@param[in] other the FMTSpatialSchedule to copy.
		*/
		FMTSpatialSchedule(const FMTSpatialSchedule& other) = default;
        // DocString: FMTSpatialSchedule(const FMTForest,size_t)
        /**
        @brief Construct a FMTSpatialSchedule from a forest, initializing every graph in the map based on the development types of each cell.
        @param[in] p_InitialMap the initial map of the schedule.
        @param[in] p_LengthReserve the amount of memory to reserve for each line graph.
        @param[in,out] p_SpatialGraph the spatial graphs.
        */
        FMTSpatialSchedule(const FMTForest& p_InitialMap,
				size_t p_LengthReserve, FMTSpatialGraphs& p_SpatialGraph);
       
		// DocString: FMTSpatialSchedule(const FMTSpatialSchedule,const std::vector<FMTCoordinate>)
		/**
		@brief Create a partial copy of the complete solution based on coordinates.
		@param[in] other the complete solution.
		@param[in] firstcoord the first coordinate.
		@param[in] endcoord the end coordinate.
		*/
		FMTSpatialSchedule(const FMTSpatialSchedule& other,
			const std::vector<FMTCoordinate>::const_iterator& firstcoord,
			const std::vector<FMTCoordinate>::const_iterator& endcoord);
        
        // DocString: FMTSpatialSchedule::==
        /**
        @brief Comparison operator for FMTSpatialSchedule.
        @param[in] rhs the FMTSpatialSchedule to compare to.
        @return true if both schedules are equal else false.
        */
        bool operator == (const FMTSpatialSchedule& rhs)const;
        // DocString: FMTSpatialSchedule::!=
        /**
        @brief Comparison operator for FMTSpatialSchedule.
        @param[in] rhs the FMTSpatialSchedule to compare to.
        @return true if both schedules are different else false.
        */
        bool operator != (const FMTSpatialSchedule& rhs)const;
        // DocString: FMTSpatialSchedule::empty
        /**
        @brief Test whether the map is empty.
        @return true if the map is empty else false.
        */
        bool empty() const {return m_mapping.empty();};
        // DocString: FMTSpatialSchedule::actPeriod
        /**
        @brief Return the last period in the graph, which is the active one.
        @return the active period.
        */
        int actPeriod() const;
        // DocString: FMTSpatialSchedule::getForestPeriod
        /**
        @brief Return the forest corresponding to a period.
        @details If periodStart is true the forest is the one before the actions were set, otherwise the one after.
        @param[in] period the period.
        @param[in] periodStart if true returns the forest before the actions.
        @return the forest.
        */
        FMTForest getForestPeriod(const int& period,bool periodStart=false) const;
		// DocString: FMTSpatialSchedule::allowAction
		/**
		@brief Check the events around a location during the green up delay periods to see if a neighboring action is within the adjacency limit.
		@param[in] targetaction the target action.
		@param[in] bindingactions the binding actions.
		@param[in] location the location.
		@param[in] period the period.
		@return true if the action is allowed else false.
		*/
		bool allowAction(const int& targetaction,const std::vector<Spatial::FMTBindingSpatialAction>&bindingactions,const FMTCoordinate& location, const int& period) const;
	   // DocString: FMTSpatialSchedule::getUpdatedScheduling
	   /**
	   @brief Return, for all actions, the coordinates with operable developments at the end of the graph.
	   @param[in] model the model.
	   @param[in] actiontargets the action targets.
	   @param[in,out] cachedaction the operability cache per development.
	   @param[in] scheduleoperabilities the schedule operabilities.
	   @param[in] schedule_only if true only considers the developments of the schedule.
	   @param[in] original the original scheduling.
	   @param[in] updatedcoordinate the updated coordinates.
	   @return the operable coordinates per action.
	   */
	   std::vector<std::set<Spatial::FMTCoordinate>>getUpdatedScheduling(
											const Models::FMTModel& model,	
											 const std::vector<int>& actiontargets,
											boost::unordered_map<Core::FMTDevelopment, std::vector<bool>>& cachedaction,
										  const std::vector<boost::unordered_set<Core::FMTDevelopment>>& scheduleoperabilities,
										   bool schedule_only = true,
										   std::vector<std::set<Spatial::FMTCoordinate>> original= std::vector<std::set<Spatial::FMTCoordinate>>(),
											std::vector<FMTCoordinate> updatedcoordinate= std::vector<FMTCoordinate>()) const;

	   // DocString: FMTSpatialSchedule::evaluatespatialconstraint
	   /**
	   @brief Return the evaluation value of a spatial constraint.
	   @param[in] p_SpatialConstraint the spatial constraint.
	   @param[in] p_SpatialGraph the spatial graphs.
	   @return the constraint evaluation value.
	   */
	   double evaluateSpatialConstraint(const Core::FMTConstraint& p_SpatialConstraint,
					const FMTSpatialGraphs& p_SpatialGraph) const;
	   // DocString: FMTSpatialSchedule::evaluateDistance
	   /**
	   @brief Return the evaluation value of a distance spatial constraint.
	   */
	   /*double evaluateDistance(const FMTEvent& eventof,
		   const double& lowerdistancetoevent,
		   const double& upperdistancetoevent,
		   const int& period, const std::vector<bool>& actionsused,
		   std::unordered_set<size_t>& relations,
		   const std::vector<FMTEventContainer::const_iterator>& events) const;*/
		// DocString: FMTSpatialSchedule::verifySpatialFeasibility
		/**
		@brief Return the cells that are spatially allowable for a target action from operable coordinates, mainly checking the green up constraint.
		@param[in] targetaction the target action.
		@param[in] bindingactions the binding actions.
		@param[in] period the period.
		@param[in] operables the operable coordinates.
		@return the spatially feasible coordinates.
		*/
		std::set<FMTCoordinate> verifySpatialFeasibility(const int& targetaction,
			const std::vector<Spatial::FMTBindingSpatialAction>& bindingactions,
			const int& period, const std::set<FMTCoordinate>& operables) const;
		// DocString: FMTSpatialSchedule::buildHarvest
		/**
		@brief Build a container of harvest events for a target action until the area target is reached.
		@param[in] target the area target.
		@param[in] targetaction the target binding action.
		@param[in,out] generator the random generator.
		@param[in] mapping_pass the candidate coordinates.
		@param[in] previousperiod the previous period.
		@param[in] actionid the action id.
		@param[in,out] operated the operated coordinates.
		@return the harvest events.
		*/
		FMTEventContainer buildHarvest(const double& target, const Spatial::FMTBindingSpatialAction& targetaction, std::default_random_engine& generator, std::set<FMTCoordinate> mapping_pass,
			const int& previousperiod, const int& actionid, std::vector<FMTCoordinate>& operated) const;
		// DocString: FMTSpatialSchedule::operateEvents
		/**
		@brief Operate a container of events for an action and return the operated area.
		@param[in] cuts the events to operate.
		@param[in] action the action.
		@param[in] action_id the action id.
		@param[in] Transition the transition.
		@param[in] ylds the yields.
		@param[in] themes the themes.
		@return the operated area.
		*/
		double operateEvents(const FMTEventContainer& cuts,const Core::FMTAction& action, const int& action_id, const Core::FMTTransition& Transition,
					 const Core::FMTYields& ylds, const std::vector<Core::FMTTheme>& themes);
		// DocString: FMTSpatialSchedule::operateCoord
		/**
		@brief Operate a coordinate and add it to the events.
		@param[in] coord the coordinate.
		@param[in] action the action.
		@param[in] action_id the action id.
		@param[in] bindingspaction the binding spatial action.
		@param[in] Transition the transition.
		@param[in] ylds the yields.
		@param[in] themes the themes.
		*/
		void operateCoord(const FMTCoordinate& coord,const Core::FMTAction& action, const int& action_id, const FMTBindingSpatialAction& bindingspaction, const Core::FMTTransition& Transition,
					 const Core::FMTYields& ylds, const std::vector<Core::FMTTheme>& themes);
		// DocString: FMTSpatialSchedule::grow
		/**
		@brief Grow every graph of the schedule by one period.
		*/
		void grow();
		// DocString: FMTSpatialSchedule::setNewPeriod
		/**
		@brief Set a new period in the schedule.
		*/
		//void setNewPeriod();
		// DocString: FMTSpatialSchedule::getSchedules
		/**
		@brief Return the operated schedules from the line graphs.
		@param[in] p_Graphs the spatial graphs.
		@param[in] withlock if true includes the locked developments.
		@return the schedules.
		*/
		std::vector<Core::FMTSchedule> getSchedules(const FMTSpatialGraphs p_Graphs,bool withlock=false) const;
		// DocString: FMTSpatialSchedule::isBetterThan
		/**
		@brief Compare two spatial schedules and return, for each constraint group, true if this solution has a better value than the compared one.
		@param[in] newsolution the solution to compare to.
		@param[in] p_Graphs the spatial graphs.
		@return a vector of booleans per constraint group.
		*/
		std::vector<int> isBetterThan(const FMTSpatialSchedule& newsolution,
								const Spatial::FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getConstraintEvaluation
		/**
		@brief Return the evaluated value of a solution constraint.
		@param[in] p_Graphs the spatial graphs.
		@param[in] p_ConstraintId the constraint id.
		@return the constraint evaluation value.
		*/
		double getConstraintEvaluation(const FMTSpatialGraphs& p_Graphs, size_t p_ConstraintId) const;
		// DocString: FMTSpatialSchedule::getConstraintsValues
		/**
		@brief Return the values of each constraint, used for normalization.
		@param[in] p_Graphs the spatial graphs.
		@return the constraint values.
		*/
		std::vector<double> getConstraintsValues(const Spatial::FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getWeightedFactors
		/**
		@brief Generate the constraint factors based on the actual solution.
		@param[in] p_Graphs the spatial graphs.
		@return the weighted factors.
		*/
		std::vector<double> getWeightedFactors(const Spatial::FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getdualinfeasibility
		/**
		@brief Return the primal infeasibility of a set of constraints.
		@param[in] constraints the constraints.
		@param[in] p_Graphs the spatial graphs.
		@param[in] withfactorization if true applies the factorization.
		@return the primal infeasibility.
		*/
		double getPrimalInfeasibility(const std::vector<const Core::FMTConstraint*>& constraints,
			const Spatial::FMTSpatialGraphs& p_Graphs, bool withfactorization = false) const;
		// DocString: FMTSpatialSchedule::logSolutionStatus
		/**
		@brief Log the status of the solution.
		@param[in] iteration the iteration.
		@param[in] objective the objective value.
		@param[in] primalinfeasibility the primal infeasibility.
		*/
		void logSolutionStatus(const size_t& iteration, const double& objective, const double& primalinfeasibility) const;
		// DocString: FMTSpatialSchedule::getSolutionStatus
		/**
		@brief Get the objective value and the primal infeasibility of the solution.
		@param[out] objective the objective value.
		@param[out] primalinfeasibility the primal infeasibility.
		@param[in] p_Graphs the spatial graphs.
		@param[in] withsense if true applies the sense.
		@param[in] withfactorization if true applies the factorization.
		@param[in] withspatial if true includes the spatial part.
		*/
		void getSolutionStatus(double& objective, double& primalinfeasibility,
			const FMTSpatialGraphs& p_Graphs, 
			bool withsense = true, bool withfactorization = false,bool withspatial = true) const;
		// DocString: FMTSpatialSchedule::getGlobalObjective
		/**
		@brief Return the global objective, mixing the objective and the infeasibility, where a lower value is a better solution.
		@details The returned value can be negative.
		@param[in] p_Graphs the spatial graphs.
		@return the global objective.
		*/
		double getGlobalObjective(const FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getobjectivevaluey
		/**
		@brief Return the objective value of the spatial schedule.
		@param[in] p_Graphs the spatial graphs.
		@param[in] withsense if true applies the sense.
		@return the objective value.
		*/
		double getObjectiveValue(const FMTSpatialGraphs& p_Graphs,bool withsense = true) const;

		// DocString: FMTSpatialSchedule::getPatchStats
		/**
		@brief Return the patch statistics (area, perimeter) of the events for a set of actions.
		@param[in] actions the actions.
		@return the patch statistics.
		*/
		std::string getPatchStats(const std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTSpatialSchedule::lastDistLayer
		/**
		@brief Return a layer with the sum of all the graph outputs related to a constraint for a period.
		@param[in] modelactions the model actions.
		@param[in] period the period.
		@return the disturbance layer.
		*/
		FMTLayer<std::string> lastDistLayer(const std::vector<Core::FMTAction>& modelactions, const int& period) const;
		// DocString: FMTSpatialSchedule::getGCBMtransitions
		/**
		@brief Return the GCBM transitions of the solution and fill a layer with the stacked actions.
		@param[in,out] stackedactions the stacked actions layer.
		@param[in] modelactions the model actions.
		@param[in] classifiers the classifiers.
		@param[in] period the period.
		@return the GCBM transitions.
		*/
		std::vector<Core::FMTGCBMTransition> getGCBMtransitions(FMTLayer<std::string>& stackedactions, const std::vector<Core::FMTAction>& modelactions, const std::vector<Core::FMTTheme>& classifiers, const int& period) const;
		// DocString: FMTSpatialSchedule::getPredictors
		/**
		@brief Get the predictors for the whole solution and write the predictor ids into a layer.
		@param[in,out] predictorids the predictor ids layer.
		@param[in] model the model.
		@param[in] yieldnames the yield names.
		@param[in] period the period.
		@param[in] periodonevalues if true uses the period one values.
		@param[in] withGCBMid if true uses the GCBM id.
		@return the predictors.
		*/
		std::vector<std::vector<Graph::FMTPredictor>> getPredictors(FMTLayer<int>& predictorids, const Models::FMTModel& model, const std::vector<std::string>& yieldnames, const int& period,bool periodonevalues = false,bool withGCBMid = true) const;
		
		// DocString: FMTSpatialSchedule::eraseLastPeriod
		/**
		@brief Erase the last period of the FMTSpatialSchedule.
		*/
		void eraseLastPeriod();
		// DocString: FMTSpatialSchedule::getBindingActions
		/**
		@brief Get the binding actions based on the model constraints for a period.
		@param[in] model the model.
		@param[in] period the period.
		@return the binding actions.
		*/
		std::vector<Spatial::FMTBindingSpatialAction> getBindingActions(const Models::FMTModel& model, const int& period) const;
		// DocString: FMTSpatialSchedule::getBindingActionsByPeriod
		/**
		@brief Get the binding actions based on the model constraints in a vector by period.
		@param[in] model the model.
		@return the binding actions by period.
		*/
		actionbindings getBindingActionsByPeriod(const Models::FMTModel& model) const;
		// DocString: FMTSpatialSchedule::referenceBuild
		/**
		@brief Simulate a schedule of actions on the actual spatially explicit forest.
		@details If schedule_only is on, the simulator does not try to find operable developments outside the potential schedule even if the harvested area target is not reached.
		@param[in] schedule the schedule.
		@param[in] model the model.
		@param[in] scheduleoperabilities the schedule operabilities.
		@param[in,out] p_randomEngine the random engine.
		@param[in] schedule_only if true only considers the developments of the schedule.
		@param[in] scheduleatfirstpass if true uses the schedule at the first pass.
		@return a map of statistics.
		*/
		std::map<std::string, double> referenceBuild(const Core::FMTSchedule& schedule, const Models::FMTModel& model,
										const std::vector<boost::unordered_set<Core::FMTDevelopment>>& scheduleoperabilities,
										std::default_random_engine& p_randomEngine,
										bool schedule_only = true,
										bool scheduleatfirstpass = true);
		// DocString: FMTSpatialSchedule::greedyReferenceBuild
		/**
		@brief Call the simulate function multiple times to find the best possible spatialization for a schedule using random draws.
		@param[in] schedule the schedule.
		@param[in] p_Graphs the spatial graphs.
		@param[in] randomiterations the number of random iterations.
		@param[in] seed the seed.
		@param[in] tolerance the tolerance.
		@param[in] log if true logs the process.
		@return a map of statistics.
		*/
		std::map<std::string, double> greedyReferenceBuild(const Core::FMTSchedule& schedule, const FMTSpatialGraphs& p_Graphs,
										const size_t& randomiterations,
										unsigned int seed = 0,
										double tolerance = FMT_DBL_TOLERANCE,
										bool log = true);
		// DocString: FMTSpatialSchedule::randomBuild
		/**
		@brief Randomly create a solution for one period using a generator.
		@param[in] model the model.
		@param[in,out] generator the random generator.
		@return the graph stats.
		*/
		Graph::FMTGraphStats randomBuild(const Models::FMTModel& model, std::default_random_engine& generator);
		// DocString: FMTSpatialSchedule::setGrow
		/**
		@brief Set natural growth on coordinates, removing every action.
		@param[in] p_coordinates the coordinates to set to growth.
		@param[in] p_model the model to use.
		*/
		void setGrow(const std::vector<FMTCoordinate>& p_coordinates,
									const Models::FMTModel& p_model);
		// DocString: FMTSpatialSchedule::perturbGraph
		/**
		@brief Change one graph in the solution, removing its contribution to the objective and adding the contribution of the newly generated one.
		@param[in] coordinate the coordinate.
		@param[in] period the period.
		@param[in] model the model.
		@param[in,out] generator the random generator.
		@param[in] bindings the action bindings.
		*/
		void perturbGraph(const FMTCoordinate& coordinate,const int& period,
			const Models::FMTModel& model, std::default_random_engine& generator,
			const actionbindings& bindings);
		// DocString: FMTSpatialSchedule::isBetterByGroup
		/**
		@brief Compare this solution to another one by constraint group.
		@param[in] rhs the solution to compare to.
		@param[in] p_Graphs the spatial graphs.
		@return true if this solution is better by group else false.
		*/
		bool isBetterByGroup(const FMTSpatialSchedule& rhs, const FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::swap
		/**
		@brief Swap this FMTSpatialSchedule with another one.
		@param[in,out] rhs the FMTSpatialSchedule to swap with.
		*/
		void swap(FMTSpatialSchedule& rhs);
		// DocString: FMTSpatialSchedule::getMovableCoordinates
		/**
		@brief Return the coordinates where something can be operated for a period.
		@param[in] model the model.
		@param[in] period the period.
		@return the movable coordinates.
		*/
		std::vector<Spatial::FMTCoordinate>getMovableCoordinates(const Models::FMTModel& model,const int& period) const;

		// DocString: FMTSpatialSchedule::getPeriodWithMaximalEvents
		/**
		@brief Return the period with the maximal number of events for a set of actions.
		@param[in] actions the actions.
		@return the period with the maximal number of events.
		*/
		int getPeriodWithMaximalEvents(const std::vector<bool>& actions) const;
		// DocString: FMTSpatialSchedule::getAreaConflictCoordinates
		/**
		@brief Return the coordinates of the events that do not have the right area in the worst period.
		@param[in] bindingactions the binding actions.
		@param[in] period the period.
		@param[in] conflictonly if true returns only the conflicting coordinates.
		@return the area conflict coordinates.
		*/
		std::vector<std::vector<Spatial::FMTCoordinate>>getAreaConflictCoordinates(const actionbindings& bindingactions,const int& period,bool conflictonly=true) const;
		// DocString: FMTSpatialSchedule::getAdjacencyConflictCoordinates
		/**
		@brief Return the adjacency conflict coordinates that need to be destroyed.
		@param[in] bindingactions the binding actions.
		@param[in] period the period.
		@param[in] conflictonly if true returns only the conflicting coordinates.
		@return the adjacency conflict coordinates.
		*/
		std::vector<std::vector<Spatial::FMTCoordinate>>getAdjacencyConflictCoordinates(const actionbindings& bindingactions,const int& period, bool conflictonly = true) const;
		// DocString: FMTSpatialSchedule::isPartial
		/**
		@brief Return true if the solution is partial.
		@return true if the solution is partial else false.
		*/
		bool isPartial() const;
		// DocString: FMTSpatialSchedule::emptyEvents
		/**
		@brief Return true if there are no events.
		@return true if there are no events else false.
		*/
		bool emptyEvents() const;
		// DocString: FMTSpatialSchedule::copyFromPartial
		/**
		@brief Copy the elements from a partial solution.
		@param[in] rhs the partial solution to copy.
		*/
		void copyFromPartial(const FMTSpatialSchedule& rhs);
		// DocString: FMTSpatialSchedule::copyFromPartial
		/**
		@brief Copy the elements from a partial solution, also allowing a swap.
		@param[in,out] rhs the partial solution to copy.
		*/
		void copyFromPartial(FMTSpatialSchedule& rhs);
		// DocString: FMTSpatialSchedule::setConstraintsFactor
		/**
		@brief Set the constraint factors for normalization.
		@param[in] model the model.
		@param[in] factors the factors.
		*/
		void setConstraintsFactor(const Models::FMTModel& model,const std::vector<double>&factors);
		// DocString: FMTSpatialSchedule::needsRefactorization
		/**
		@brief Return true if the solution looks unscaled and needs new factors.
		@param[in] p_Graphs the spatial graphs.
		@return true if the solution needs refactorization else false.
		*/
		bool needsRefactorization(const FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getConstraintsFactor
		/**
		@brief Get the constraint factors for normalization.
		@return the constraint factors.
		*/
		const std::vector<double>& getConstraintsFactor() const;
		// DocString: FMTSpatialSchedule::getOutput
		/**
		@brief Get the output value for a period using the solution.
		@details The map key is the output name when level is standard or totalonly, or the development name when level is developpement.
		@param[in] p_Graphs the spatial graphs.
		@param[in] p_output the output.
		@param[in] p_period the period.
		@param[in] level the output level.
		@return a map of names to values.
		*/
		std::map<std::string,double> getOutput(const FMTSpatialGraphs& p_Graphs, const Core::FMTOutput& p_output,
			int p_period,Core::FMToutputlevel level = Core::FMToutputlevel::totalonly) const;
		// DocString: FMTSpatialSchedule::getSpatialOutput
		/**
		@brief Get the output value for the whole layer for a period and an output.
		@param[in] model the model.
		@param[in] output the output.
		@param[in] period the period.
		@return the spatial output layer.
		*/
		FMTLayer<double> getSpatialOutput(const Models::FMTModel& model,
						const Core::FMTOutput& output,const int& period) const;
		// DocString: FMTSpatialSchedule::getOutputByCoordinate
		/**
		@brief Return the output value by coordinate for an output, a model and a period.
		@param[in] model the model.
		@param[in] output the output.
		@param[in] period the period.
		@return the output value per coordinate.
		*/
		std::vector<std::pair<FMTCoordinate, double>>getOutputByCoordinate(const Models::FMTModel & model,
			const Core::FMTOutput& output, const int& period) const;
		// DocString: FMTSpatialSchedule::postSolve
		/**
		@brief Postsolve the spatial solution.
		@param[in] p_Filter the filter used for the preSolve.
		@param[in] p_PresolveActions the presolved actions.
		@param[in,out] p_Graphs the spatial graphs.
		*/
		void postSolve(const Core::FMTMaskFilter&  p_Filter,
			const std::vector<Core::FMTAction>& p_PresolveActions,
			Spatial::FMTSpatialGraphs& p_Graphs);
		// DocString: FMTSpatialSchedule::preSolve
		/**
		@brief Return a presolved solution of the original solution.
		@param[in] p_filter the filter used to keep the mask.
		@param[in,out] p_Graphs the spatial graphs.
		@param[in] p_ReserveSize the number of vertices to reserve per line graph.
		@return the presolved schedule.
		*/
		FMTSpatialSchedule preSolve(const Core::FMTMaskFilter& p_filter,
			FMTSpatialGraphs& p_Graphs,size_t p_ReserveSize) const;
		// DocString: FMTSpatialSchedule::getArea
		/**
		@brief Get the area of a period based on the solution.
		@param[in] period the period.
		@param[in] beforegrowanddeath if true returns before growth and death.
		@return the actual developments.
		*/
		std::vector<Core::FMTActualDevelopment>getArea(int period = 0, bool beforegrowanddeath = false) const;

		// DocString: FMTSpatialSchedule::getBaseSchedule
		/**
		@brief Return the base schedule of the spatial graphs.
		@param[in] p_SpatialGraph the spatial graphs.
		@return the base schedule.
		*/
		FMTSpatialSchedule getBaseSchedule(const FMTSpatialGraphs& p_SpatialGraph) const;
		// DocString: FMTSpatialSchedule::setSpatialGraphs
		/**
		@brief Set the spatial graphs from another schedule.
		@param[in] p_ToCopy the schedule to copy.
		@param[in,out] p_SpatialGraph the spatial graphs.
		*/
		void setSpatialGraphs(const Spatial::FMTSpatialSchedule& p_ToCopy, FMTSpatialGraphs& p_SpatialGraph);
		// DocString: FMTSpatialSchedule::getGroupsConflict
		/**
		@brief Return the coordinates in conflict with a group spatial constraint.
		@param[in] p_SpatialConstraint the spatial constraint.
		@param[in] p_SpatialGraph the spatial graphs.
		@return the group conflict coordinates.
		*/
		std::vector<FMTCoordinate> getGroupsConflict(const Core::FMTConstraint& p_SpatialConstraint,
			const FMTSpatialGraphs& p_SpatialGraph) const;
		// DocString: FMTSpatialSchedule::EventSpread
		/**
		@brief Helper holding the input iterator, output iterator and event of an event spread move.
		*/
		struct EventSpread
			{
			EventSpread(FMTLayer::const_iterator p_InIt,
				FMTLayer::iterator p_OutIt, FMTEventContainer::iterator p_Event):
				m_InEvent(p_InIt), m_OutEvent(p_OutIt), m_Event(p_Event){}
			FMTLayer::const_iterator m_InEvent;
			FMTLayer::iterator  m_OutEvent;
			FMTEventContainer::iterator m_Event;
			};
		// DocString: FMTSpatialSchedule::getPotentialSpread
		/**
		@brief Return the potential event spreads for a period.
		@param[in] p_period the period.
		@return the potential event spreads.
		*/
		std::vector<EventSpread>getPotentialSpread(int p_period);
		// DocString: FMTSpatialSchedule::canDoEventSpread
		/**
		@brief Return true if an event spread can be done for a period.
		@param[in] p_period the period.
		@return true if an event spread can be done else false.
		*/
		bool canDoEventSpread(int p_period) const;
		// DocString: FMTSpatialSchedule::setSpread
		/**
		@brief Apply a range of event spreads to the solution.
		@param[in] p_first the first event spread.
		@param[in] p_end the end of the event spreads.
		*/
		void setSpread(
			std::vector<EventSpread>::const_iterator p_first,
			std::vector<EventSpread>::const_iterator p_end);
		// DocString: FMTSpatialSchedule::setStaticsMovableCoordinates
		/**
		@brief Set the static movable coordinates from a model.
		@param[in] p_model the model.
		*/
		void setStaticsMovableCoordinates(const Models::FMTModel& p_model);
		// DocString: FMTSpatialSchedule::enableSolutionTracker
		/**
		@brief Enable the non spatial solution tracker using the spatial graphs.
		@param[in] p_SpatialGraph the spatial graphs.
		*/
		void enableSolutionTracker(const FMTSpatialGraphs& p_SpatialGraph);
		// DocString: FMTSpatialSchedule::disableSolutionTracker
		/**
		@brief Disable the non spatial solution tracker.
		*/
		void disableSolutionTracker();
	protected:
		// DocString: FMTSpatialSchedule::_getFromEvents(const Core::FMTConstraint&, const std::vector<Core::FMTAction>&, const int&, const int&)
		/**
		@brief Return the line graphs of the events for a constraint, actions and a period range.
		@param[in] constraint the constraint.
		@param[in] actions the actions.
		@param[in] start the start period.
		@param[in] stop the stop period.
		@return the line graphs.
		*/
		std::vector<const Graph::FMTLineGraph*>_getFromEvents(const Core::FMTConstraint& constraint, const std::vector<Core::FMTAction>& actions, const int& start, const int& stop) const;
		// DocString: FMTSpatialSchedule::_getFromEvents(const Core::FMTOutputNode&, const std::vector<Core::FMTAction>&, const int&)
		/**
		@brief Return the coordinates present in the events for an output node, actions and a period.
		@param[in] node the output node.
		@param[in] actions the actions.
		@param[in] period the period.
		@return the coordinates.
		*/
		std::vector<FMTCoordinate>_getFromEvents(const Core::FMTOutputNode& node, const std::vector<Core::FMTAction>& actions, const int& period) const;
		// DocString: FMTSpatialSchedule::_getOutputFromGraph
		/**
		@brief Get the requested output from a line graph into the period values.
		@param[in] linegraph the line graph.
		@param[in] model the model.
		@param[in] node the output node.
		@param[in] solution the solution.
		@param[in] period the period.
		@param[in] nodemask the node mask.
		@param[in,out] nodecache the node cache.
		@param[in] p_theme the theme.
		@param[in] level the output level.
		@return a map of names to values.
		*/
		std::map<std::string,double> _getOutputFromGraph(const Graph::FMTLineGraph& linegraph, const Models::FMTModel & model,
			const Core::FMTOutputNode& node, const double* solution,const int&period, const Core::FMTMask& nodemask,
			boost::unordered_map<Core::FMTMask, double>& nodecache, const Core::FMTTheme*  p_theme,Core::FMToutputlevel level = Core::FMToutputlevel::totalonly) const;
		// DocString: FMTSpatialSchedule::getMaximalPatchSizes
		/**
		@brief Return the maximal patch size of a vector of spatial actions.
		*/
		//std::vector<size_t>getMaximalPatchSizes(const std::vector<FMTspatialaction>& spactions) const;
		// DocString: FMTSpatialSchedule::_inScheduleOperabilities
		/**
		@brief Return true if a development is operable for an action in the schedule operabilities.
		@param[in] scheduleoperabilities the schedule operabilities.
		@param[in] dev the development.
		@param[in] actionid the action id.
		@param[in] action the action.
		@return true if the development is operable else false.
		*/
		bool _inScheduleOperabilities(const std::vector<boost::unordered_set<Core::FMTDevelopment>>& scheduleoperabilities,
			Core::FMTDevelopment const* dev,const int& actionid, const Core::FMTAction& action) const;
    private:
		// DocString: FMTSpatialSchedule::m_scheduleType
		//Schedule type complete or partial
		FMTSpatialScheduletype m_scheduleType = FMTSpatialScheduletype::FMTcomplete;
		// DocString: FMTSpatialSchedule::m_ConstraintsFactor
		//Constraints factors.
		std::vector<double>m_ConstraintsFactor;
		// DocString: FMTSpatialSchedule::m_events
		//Events of the schedule
		FMTEventContainer m_events;
		// DocString: FMTSpatialSchedule::m_Tracker
		// Non spatial solution tracker
		FMTSolutionTracker m_Tracker;
		// DocString: FMTSpatialSchedule::m_OperabilityCache
		//Operability cache
		static boost::unordered_map<Core::FMTDevelopment,
			std::vector<int>>m_OperabilityCache;
		// DocString: FMTSpatialSchedule::m_Movables
		//Coordinates where you can actualy operate something.
		static std::vector<Spatial::FMTCoordinate>m_Movables;
		// DocString: FMTSpatialSchedule::m_BUFFER_LOOKUP
		//buffer lookup for events
		static const int m_BUFFER_LOOKUP = 1;
		
		// DocString: FMTSpatialSchedule::_evaluateSpatialAdjacency
		/**
		@brief Evaluate the adjacency conflicts for each event.
		@param[in] p_period the period.
		@param[in] p_greenup the green up.
		@param[in] p_lowerLookup the lower lookup.
		@param[in] p_upperLookup the upper lookup.
		@param[in] p_testLower if true tests the lower bound.
		@param[in] p_actions the actions.
		@return the adjacency conflict evaluation.
		*/
		double _evaluateSpatialAdjacency(
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;

		// DocString: FMTSpatialSchedule::_evaluateSpatialGroups
		/**
		@brief Evaluate the group conflicts for each event.
		@param[in] p_SpatialGraph the spatial graphs.
		@param[in] p_period the period.
		@param[in] p_greenup the green up.
		@param[in] p_lowerLookup the lower lookup.
		@param[in] p_upperLookup the upper lookup.
		@param[in] p_theme the theme.
		@param[in] p_testLower if true tests the lower bound.
		@param[in] p_actions the actions.
		@return the group conflict evaluation.
		*/
		double _evaluateSpatialGroups(
			const FMTSpatialGraphs& p_SpatialGraph,
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			int p_theme,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;

		// DocString: FMTSpatialSchedule::_getSpatialGroupsConflict
		/**
		@brief Return the events in conflict with the group rules.
		@param[in] p_SpatialGraph the spatial graphs.
		@param[in] p_period the period.
		@param[in] p_greenup the green up.
		@param[in] p_lowerLookup the lower lookup.
		@param[in] p_upperLookup the upper lookup.
		@param[in] p_theme the theme.
		@param[in] p_testLower if true tests the lower bound.
		@param[in] p_actions the actions.
		@return the conflicting events.
		*/
		std::vector<FMTEventContainer::const_iterator> _getSpatialGroupsConflict(
			const FMTSpatialGraphs& p_SpatialGraph,
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			int p_theme,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;

		// DocString: FMTSpatialSchedule::_getAdjacencyConflict
		/**
		@brief Return the events in conflict with the adjacency rules.
		@param[in] p_period the period.
		@param[in] p_greenup the green up.
		@param[in] p_lowerLookup the lower lookup.
		@param[in] p_upperLookup the upper lookup.
		@param[in] p_testLower if true tests the lower bound.
		@param[in] p_actions the actions.
		@return the conflicting events.
		*/
		std::vector<FMTEventContainer::const_iterator> _getAdjacencyConflict(
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;


		// DocString: FMTSpatialSchedule::_getNonSpatialCellsCount
		/**
		@brief Return the number of non spatial cells.
		@return the non spatial cells count.
		*/
		size_t _getNonSpatialCellsCount() const;

		// DocString: FMTSpatialSchedule::_getExponentialFactorization
		/**
		@brief Return the exponential factorization of a value.
		@param[in] p_value the value.
		@param[in] p_factor the factor.
		@return the factorized value.
		*/
		static double _getExponentialFactorization(double p_value, double p_factor);

		// DocString: FMTSpatialSchedule::_getConstraintFloorValue
		/**
		@brief Return the floor value of a constraint value.
		@param[in] p_inValue the value.
		@return the floor value.
		*/
		double _getConstraintFloorValue(double p_inValue) const;

		// DocString: FMTSpatialSchedule::_getRulesEvaluation
		/**
		@brief Evaluate the patch rules on the solution.
		@param[in] p_rules the patch rules.
		@param[in] p_SpatialGraph the spatial graphs.
		@return the rules evaluation.
		*/
		double _getRulesEvaluation(const std::vector<FMTPatchRules>& p_rules,
			const FMTSpatialGraphs& p_SpatialGraph) const;

		// DocString: FMTSpatialSchedule::_putTabouInCache
		/**
		@brief Put the tabou developments in the operability cache and return the previous values.
		@param[in] p_model the model.
		@param[in,out] p_tabou the tabou developments.
		@return the previous cache values.
		*/
		static std::vector<std::pair<Core::FMTDevelopment, std::vector<int>>> _putTabouInCache(
			const Models::FMTModel& p_model,
			std::map<Core::FMTDevelopment, std::vector<bool>>& p_tabou);
		// DocString: FMTSpatialSchedule::_setTabouOutOfCache
		/**
		@brief Restore the operability cache values put aside by _putTabouInCache.
		@param[in,out] p_GoodValues the values to restore.
		*/
		static void _setTabouOutOfCache(
			std::vector<std::pair<Core::FMTDevelopment, std::vector<int>>>& p_GoodValues);

		

		

		
		
		
};
}


#endif // FMTSpatialSchedule_H
