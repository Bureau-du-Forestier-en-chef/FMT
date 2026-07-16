/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTSpatialSchedule_H
#define FMTSpatialSchedule_H

#include "FMTlayer.hpp"
#include "FMTlinegraph.h"
#include "FMTeventcontainer.h"
#include "FMTbindingspatialaction.h"
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
class FMTforest;
class FMTSpatialGraphs;
class FMTPatchRules;
enum  FMTSpatialScheduletype
	{
		FMTcomplete = 1,
		FMTpartial = 2
	};

// DocString: FMTSpatialSchedule
/**
This class is a map containing a linear graph for each cell. The graph represent the stand,
the action and the transition at each each period for the cell. Can be build randomly or
with a schedule.
*/
class FMTEXPORT FMTSpatialSchedule : public FMTlayer<FMTVirtualLineGraph>
{
    public:
		typedef std::vector<std::vector<Spatial::FMTbindingspatialaction>> actionbindings;
		// DocString: FMTSpatialSchedule(FMTSpatialSchedule&&)
		/**
		Default move constructor for FMTSpatialSchedule.
		*/
		//FMTSpatialSchedule(FMTSpatialSchedule&& rhs) noexcept;
        // DocString: FMTSpatialSchedule()
		/**
		Default constructor of FMTSpatialSchedule
		*/
        FMTSpatialSchedule()=default;
		// DocString: ~FMTSpatialSchedule()
	   /**
	   Default destructor of FMTSpatialSchedule
	   */
		virtual ~FMTSpatialSchedule() = default;
		// DocString: FMTSpatialSchedule::=
		/**
		Copy assignment of FMTSpatialSchedule
		*/
		FMTSpatialSchedule& operator=(const FMTSpatialSchedule& rhs) = default;
		// DocString: FMTSpatialSchedule(const FMTSpatialSchedule)
		/**
		Copy constructor of FMTSpatialSchedule
		*/
		FMTSpatialSchedule(const FMTSpatialSchedule& other) = default;
        // DocString: FMTSpatialSchedule(const FMTforest,size_t)
		/**
		@brief Constructor of FMTSpatialSchedule based on FMTforest. It's initializing every graph in the map base on developments types in each cell.
		@param[in] p_InitialMap the initial map of the schedule.
		@param[in] p_LengthReserve the amount of memory to reserve for each linegraph.
		*/
        FMTSpatialSchedule(const FMTforest& p_InitialMap,
				size_t p_LengthReserve, FMTSpatialGraphs& p_SpatialGraph);
       
		// DocString: FMTSpatialSchedule(const FMTSpatialSchedule,const std::vector<FMTcoordinate>)
		/**
		Create a partial copy of the complete solution base on coordinates.
		*/
		FMTSpatialSchedule(const FMTSpatialSchedule& other,
			const std::vector<FMTcoordinate>::const_iterator& firstcoord,
			const std::vector<FMTcoordinate>::const_iterator& endcoord);
        
        // DocString: FMTSpatialSchedule::==
		/**
		Comparison operator equal to
		*/
        bool operator == (const FMTSpatialSchedule& rhs)const;
        // DocString: FMTSpatialSchedule::!=
		/**
		Comparison operator different than
		*/
        bool operator != (const FMTSpatialSchedule& rhs)const;
        // DocString: FMTSpatialSchedule::empty
        /**
        Test whether the map is empty.
        */
        bool empty() const {return mapping.empty();};
        // DocString: FMTSpatialSchedule::actPeriod
        /**
        Return the last period in the graph which is the active one.
        */
        int actPeriod() const;
        // DocString: FMTSpatialSchedule::getForestPeriod
        /**
        Return the FMTforest corresponding to the period asked. If periodStart, the forest pass wil be the one before the actions as been set otherwise, it will be the forest after the actions as been set. 
        */
        FMTforest getForestPeriod(const int& period,bool periodStart=false) const;
		// DocString: FMTSpatialSchedule::allowAction
		/**
		Check in all events around the location during periods corresponding to green up delay
		if an action present in neighbors is in the adjacency limit.
		*/
		bool allowAction(const int& targetaction,const std::vector<Spatial::FMTbindingspatialaction>&bindingactions,const FMTcoordinate& location, const int& period) const;
	   // DocString: FMTSpatialSchedule::getUpdatedScheduling
		/**
		Return for all actions the FMTcoordinate with operable developments at the end of the graph.
		*/
	   std::vector<std::set<Spatial::FMTcoordinate>>getUpdatedScheduling(
											const Models::FMTModel& model,	
											 const std::vector<int>& actiontargets,
											boost::unordered_map<Core::FMTDevelopment, std::vector<bool>>& cachedaction,
										  const std::vector<boost::unordered_set<Core::FMTDevelopment>>& scheduleoperabilities,
										   bool schedule_only = true,
										   std::vector<std::set<Spatial::FMTcoordinate>> original= std::vector<std::set<Spatial::FMTcoordinate>>(),
											std::vector<FMTcoordinate> updatedcoordinate= std::vector<FMTcoordinate>()) const;

	   // DocString: FMTSpatialSchedule::evaluatespatialconstraint
		/**
		Return the constraint evaluation value of a spatial constraint. If the subset is not a nullptr the 
		*/
	   double evaluateSpatialConstraint(const Core::FMTConstraint& p_SpatialConstraint,
					const FMTSpatialGraphs& p_SpatialGraph) const;
	   // DocString: FMTSpatialSchedule::evaluateDistance
		/**
		Return the constraint evaluation value of a spatial constraint.
		*/
	   /*double evaluateDistance(const FMTevent& eventof,
		   const double& lowerdistancetoevent,
		   const double& upperdistancetoevent,
		   const int& period, const std::vector<bool>& actionsused,
		   std::unordered_set<size_t>& relations,
		   const std::vector<FMTeventcontainer::const_iterator>& events) const;*/
		// DocString: FMTSpatialSchedule::verifySpatialFeasibility
		/**
		For the target action, return a set of FMTcoordinate corresponding to the cells that are spatially allowable from coordinates that are operables. Mainly only check for coord
		respecting the greenup constraint.
		*/
		std::set<FMTcoordinate> verifySpatialFeasibility(const int& targetaction,
			const std::vector<Spatial::FMTbindingspatialaction>& bindingactions,
			const int& period, const std::set<FMTcoordinate>& operables) const;
		// DocString: FMTSpatialSchedule::buildHarvest
		/**

		*/
		FMTeventcontainer buildHarvest(const double& target, const Spatial::FMTbindingspatialaction& targetaction, std::default_random_engine& generator, std::set<FMTcoordinate> mapping_pass,
			const int& previousperiod, const int& actionid, std::vector<FMTcoordinate>& operated) const;
		// DocString: FMTSpatialSchedule::operateEvents
		/**

		*/
		double operateEvents(const FMTeventcontainer& cuts,const Core::FMTAction& action, const int& action_id, const Core::FMTTransition& Transition,
					 const Core::FMTYields& ylds, const std::vector<Core::FMTTheme>& themes);
		// DocString: FMTSpatialSchedule::operateCoord
		/**
		 Operate a coordinate and add it to the events ... That's why the FMTbindingspatialaction is needed
		*/
		void operateCoord(const FMTcoordinate& coord,const Core::FMTAction& action, const int& action_id, const FMTbindingspatialaction& bindingspaction, const Core::FMTTransition& Transition,
					 const Core::FMTYields& ylds, const std::vector<Core::FMTTheme>& themes);
		// DocString: FMTSpatialSchedule::grow
		/**

		*/
		void grow();
		// DocString: FMTSpatialSchedule::setnewperiod
		/**

		*/
		//void setnewperiod();
		// DocString: FMTSpatialSchedule::getSchedules
		/**
		Return operated schedules from linegraph. 
		*/
		std::vector<Core::FMTSchedule> getSchedules(const FMTSpatialGraphs p_Graphs,bool withlock=false) const;
		// DocString: FMTSpatialSchedule::isBetterThan
		/**
			Compare two spatialschedule and return a vector of bool with true if the constraint group has a better value then the
			compared solution else false.
		*/
		std::vector<int> isBetterThan(const FMTSpatialSchedule& newsolution,
								const Spatial::FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getConstraintEvaluation
		/**
			Returns the double value of the evaluated solution constraint.
		*/
		double getConstraintEvaluation(const FMTSpatialGraphs& p_Graphs, size_t p_ConstraintId) const;
		// DocString: FMTSpatialSchedule::getConstraintsValues
		/**
			Fill up a vector of values for for each contraints (used for normalization)
		*/
		std::vector<double> getConstraintsValues(const Spatial::FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getWeightedFactors
		/**
			Generates factors based on the actual solution.
		*/
		std::vector<double> getWeightedFactors(const Spatial::FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getdualinfeasibility
		/**
			Returns dual infeasibility of a set of constraints.
		*/
		double getPrimalInfeasibility(const std::vector<const Core::FMTConstraint*>& constraints,
			const Spatial::FMTSpatialGraphs& p_Graphs, bool withfactorization = false) const;
		// DocString: FMTSpatialSchedule::logSolutionStatus
		/**
			Log the status of the solution
		*/
		void logSolutionStatus(const size_t& iteration, const double& objective, const double& primalinfeasibility) const;
		// DocString: FMTSpatialSchedule::getSolutionStatus
		/**
			Get the primal infeasibility and objective value
		*/
		void getSolutionStatus(double& objective, double& primalinfeasibility,
			const FMTSpatialGraphs& p_Graphs, 
			bool withsense = true, bool withfactorization = false,bool withspatial = true) const;
		// DocString: FMTSpatialSchedule::getGlobalObjective
		/**
		Usefull to evaluate the quality of the solution it mixes objective to infeasibility and return it has double
		the lower the returned value is better is the solution. You can get a negative global objective.
		*/
		double getGlobalObjective(const FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getobjectivevaluey
		/**
			Returns the objective value of the spatialschedule
		*/
		double getObjectiveValue(const FMTSpatialGraphs& p_Graphs,bool withsense = true) const;

		// DocString: FMTSpatialSchedule::getPatchStats
		/**
			
		*/
		std::string getPatchStats(const std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTSpatialSchedule::lastDistLayer
		/**
			Return sum of all graphs outputs related to constraint.
		*/
		FMTlayer<std::string> lastDistLayer(const std::vector<Core::FMTAction>& modelactions, const int& period) const;
		// DocString: FMTSpatialSchedule::getGCBMtransitions
		/**
		
		*/
		std::vector<Core::FMTGCBMTransition> getGCBMtransitions(FMTlayer<std::string>& stackedactions, const std::vector<Core::FMTAction>& modelactions, const std::vector<Core::FMTTheme>& classifiers, const int& period) const;
		// DocString: FMTSpatialSchedule::getPredictors
		/**
			Get the predictors for the whole solution and write down the predictorsids into a layer.
		*/
		std::vector<std::vector<Graph::FMTpredictor>> getPredictors(FMTlayer<int>& predictorids, const Models::FMTModel& model, const std::vector<std::string>& yieldnames, const int& period,bool periodonevalues = false,bool withGCBMid = true) const;
		
		// DocString: FMTSpatialSchedule::eraseLastPeriod
		 /**
		 This function erase the last period of the FMTSpatialSchedule.
		 */
		void eraseLastPeriod();
		// DocString: FMTSpatialSchedule::getBindingActions
		 /**
		 Get the binding actions based on model constraints.
		 */
		std::vector<Spatial::FMTbindingspatialaction> getBindingActions(const Models::FMTModel& model, const int& period) const;
		// DocString: FMTSpatialSchedule::getBindingActionsByPeriod
		 /**
		 Get the binding actions based on model constraints in a vector by period.
		 */
		actionbindings getBindingActionsByPeriod(const Models::FMTModel& model) const;
		// DocString: FMTSpatialSchedule::referenceBuild
		/**
		This is the main function to simulate a schedule of actions (schedule) on the actual
		spatially explicit forest. If the (schedule_only) switch is turned on the simulator wont try
		to find some operable developments (not present in the potential schedule)
		even if the area harvested target for that action is not reach. The user can also set the seed
		to get different solutions from the simulator.
		*/
		std::map<std::string, double> referenceBuild(const Core::FMTSchedule& schedule, const Models::FMTModel& model,
										const std::vector<boost::unordered_set<Core::FMTDevelopment>>& scheduleoperabilities,
										std::default_random_engine& p_randomEngine,
										bool schedule_only = true,
										bool scheduleatfirstpass = true);
		// DocString: FMTSpatialSchedule::greedyReferenceBuild
		/**
		This function call multiple time the simulate function to find the best possible spatialisation for
		a given schedule using random draw. It uses a schedule of actions (schedule) on the actual
		spatially explicit forest. If the (schedule_only) switch is turned on the simulator wont try
		to find some operable developments (not present in the potential schedule)
		even if the area harvested target for that action is not reach. The user can also set the seed
		to get different solutions from the simulator.
		*/
		std::map<std::string, double> greedyReferenceBuild(const Core::FMTSchedule& schedule, const FMTSpatialGraphs& p_Graphs,
										const size_t& randomiterations,
										unsigned int seed = 0,
										double tolerance = FMT_DBL_TOLERANCE,
										bool log = true);
		// DocString: FMTSpatialSchedule::randomBuild
		/**
		With a generator randomly create a solution for one period.
		*/
		Graph::FMTgraphstats randomBuild(const Models::FMTModel& model, std::default_random_engine& generator);
		// DocString: FMTSpatialSchedule::setGrow
		/**
		@brief set natural growth on coordinates (remove every actions)
		@param[in] p_coordinates coordinates to set to growth
		@param[in] p_model the model to use
		*/
		void setGrow(const std::vector<FMTcoordinate>& p_coordinates,
									const Models::FMTModel& p_model);
		// DocString: FMTSpatialSchedule::perturbGraph
		/**
		Change one graph in the solution remove it's contribution to objective and add contribution to the newly generated to the objective.
		*/
		void perturbGraph(const FMTcoordinate& coordinate,const int& period,
			const Models::FMTModel& model, std::default_random_engine& generator,
			const actionbindings& bindings);
		// DocString: FMTSpatialSchedule::isBetterByGroup
		/**
		Compare solution by constraint group.
		*/
		bool isBetterByGroup(const FMTSpatialSchedule& rhs, const FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::swap
		/**
		Swap operator for FMTSpatialSchedule.
		*/
		void swap(FMTSpatialSchedule& rhs);
		// DocString: FMTSpatialSchedule::getMovableCoordinates
		/**
		
		*/
		std::vector<Spatial::FMTcoordinate>getMovableCoordinates(const Models::FMTModel& model,const int& period) const;

		// DocString: FMTSpatialSchedule::getPeriodWithMaximalEvents
		/**
		Get the period at with you got the maximal number of  periods.
		*/
		int getPeriodWithMaximalEvents(const std::vector<bool>& actions) const;
		// DocString: FMTSpatialSchedule::getAreaConflictCoordinates
		/**
		Returns the coordinates of events that does not have the right area in the worst period... and set the worst period
		*/
		std::vector<std::vector<Spatial::FMTcoordinate>>getAreaConflictCoordinates(const actionbindings& bindingactions,const int& period,bool conflictonly=true) const;
		// DocString: FMTSpatialSchedule::getAdjacencyConflictCoordinates
		/**
		Returns adjacency conflicts coordinate that need to be destroyed
		*/
		std::vector<std::vector<Spatial::FMTcoordinate>>getAdjacencyConflictCoordinates(const actionbindings& bindingactions,const int& period, bool conflictonly = true) const;
		// DocString: FMTSpatialSchedule::isPartial
		/**
		return true if solution is partial.
		*/
		bool isPartial() const;
		// DocString: FMTSpatialSchedule::emptyEvents
		/**
		Return true if there's no events
		*/
		bool emptyEvents() const;
		// DocString: FMTSpatialSchedule::copyFromPartial
		/**
		Copy elements from a partial solution.
		*/
		void copyFromPartial(const FMTSpatialSchedule& rhs);
		// DocString: FMTSpatialSchedule::copyFromPartial
		/**
		Copy elements from a partial solution but also allow swap.
		*/
		void copyFromPartial(FMTSpatialSchedule& rhs);
		// DocString: FMTSpatialSchedule::setConstraintsFactor
		/**
		Set the constraints factors for nomalization
		*/
		void setConstraintsFactor(const Models::FMTModel& model,const std::vector<double>&factors);
		// DocString: FMTSpatialSchedule::needsRefactorization
		/**
		Return true if the solution looks unscaled and need new factors
		*/
		bool needsRefactorization(const FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getConstraintsFactor
		/**
		Get the constraints factors for nomalization
		*/
		const std::vector<double>& getConstraintsFactor() const;
		// DocString: FMTSpatialSchedule::getOutput
		/**
		Get the output value of a output for a given period using the solution..
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or development name if level == FMToutputlevel::developpement
		*/
		std::map<std::string,double> getOutput(const FMTSpatialGraphs& p_Graphs, const Core::FMTOutput& p_output,
			int p_period,Core::FMToutputlevel level = Core::FMToutputlevel::totalonly) const;
		// DocString: FMTSpatialSchedule::getSpatialOutput
		/**
		Get the output value for the whole layer for a given period and output.
		*/
		FMTlayer<double> getSpatialOutput(const Models::FMTModel& model,
						const Core::FMTOutput& output,const int& period) const;
		// DocString: FMTSpatialSchedule::getOutputByCoordinate
		/**
		Return the output value by coordinate for a given output/model/period.
		*/
		std::vector<std::pair<FMTcoordinate, double>>getOutputByCoordinate(const Models::FMTModel & model,
			const Core::FMTOutput& output, const int& period) const;
		// DocString: FMTSpatialSchedule::postSolve
		/**
		@brief postSolve the spatial solution.
		@param[in] p_Filter filter used for presolve
		@param[in] p_PresolveActions the vector of presolved actions
		@param[in] p_OriginalBaseModel the original model.
		*/
		void postSolve(const Core::FMTMaskFilter&  p_Filter,
			const std::vector<Core::FMTAction>& p_PresolveActions,
			Spatial::FMTSpatialGraphs& p_Graphs);
		// DocString: FMTSpatialSchedule::presolve
		/**
		@brief Returned a presolved solution of the original solution.
		@param[in] p_filter filter to keep mask
		@param[in] p_presolvedThemes The themes of the presolved model.
		@param[in] the number of vertices to reserve per linegraph
		@return A presolved FMTSpatialSchedule.
		*/
		FMTSpatialSchedule presolve(const Core::FMTMaskFilter& p_filter,
			FMTSpatialGraphs& p_Graphs,size_t p_ReserveSize) const;
		// DocString: FMTSpatialSchedule::getArea
		/**
		@brief Get the area of a given period based on the solution of the model.
		@param[in] period the period selected
		@param[in] beforegrowanddeath true if we want before the growth (true) or after (false)
		@return the vector of actualdevelopment...
		*/
		std::vector<Core::FMTActualDevelopment>getArea(int period = 0, bool beforegrowanddeath = false) const;

		FMTSpatialSchedule getBaseSchedule(const FMTSpatialGraphs& p_SpatialGraph) const;
		void setSpatialGraphs(const Spatial::FMTSpatialSchedule& p_ToCopy, FMTSpatialGraphs& p_SpatialGraph);
		std::vector<FMTcoordinate> getGroupsConflict(const Core::FMTConstraint& p_SpatialConstraint,
			const FMTSpatialGraphs& p_SpatialGraph) const;
		struct EventSpread
			{
			EventSpread(FMTlayer::const_iterator p_InIt,
				FMTlayer::iterator p_OutIt, FMTeventcontainer::iterator p_Event):
				m_InEvent(p_InIt), m_OutEvent(p_OutIt), m_Event(p_Event){}
			FMTlayer::const_iterator m_InEvent;
			FMTlayer::iterator  m_OutEvent;
			FMTeventcontainer::iterator m_Event;
			};
		std::vector<EventSpread>getPotentialSpread(int p_period);
		bool canDoEventSpread(int p_period) const;
		void setSpread(
			std::vector<EventSpread>::const_iterator p_first,
			std::vector<EventSpread>::const_iterator p_end);
		void setStaticsMovableCoordinates(const Models::FMTModel& p_model);
		void enableSolutionTracker(const FMTSpatialGraphs& p_SpatialGraph);
		void disableSolutionTracker();
	protected:
		// DocString: FMTSpatialSchedule::getFromEvents(const Core::FMTConstraint&, const std::vector<Core::FMTAction>&, const int&, const int&)
		 /**
		 Get theline graph using the eventcontainer
		 */
		std::vector<const Graph::FMTlinegraph*>getFromEvents(const Core::FMTConstraint& constraint, const std::vector<Core::FMTAction>& actions, const int& start, const int& stop) const;
		// DocString: FMTSpatialSchedule::getFromEvents(const Core::FMTOutputNode&, const std::vector<Core::FMTAction>&, const int&)
		 /**
		 Get the coordinate presents in the events for the outputnode, the action and the period asked.
		 */
		std::vector<FMTcoordinate>getFromEvents(const Core::FMTOutputNode& node, const std::vector<Core::FMTAction>& actions, const int& period) const;
		// DocString: FMTSpatialSchedule::getOutputFromGraph
		 /**
		 Get the output requested from a given linegraph into periods_values
		 */
		std::map<std::string,double> getOutputFromGraph(const Graph::FMTlinegraph& linegraph, const Models::FMTModel & model,
			const Core::FMTOutputNode& node, const double* solution,const int&period, const Core::FMTMask& nodemask,
			boost::unordered_map<Core::FMTMask, double>& nodecache, const Core::FMTTheme*  p_theme,Core::FMToutputlevel level = Core::FMToutputlevel::totalonly) const;
		// DocString: FMTSpatialSchedule::getmaximalpatchsizes
		 /**
		 Return the maximal patch size of a vector of spatialactions.
		 */
		//std::vector<size_t>getmaximalpatchsizes(const std::vector<FMTspatialaction>& spactions) const;
		// DocString: FMTSpatialSchedule::inScheduleOperabilities
		 /**
		
		 */
		bool inScheduleOperabilities(const std::vector<boost::unordered_set<Core::FMTDevelopment>>& scheduleoperabilities,
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
		FMTeventcontainer m_events;
		// DocString: FMTSpatialSchedule::m_Tracker
		// Non spatial solution tracker
		FMTSolutionTracker m_Tracker;
		//DocString: FMTSpatialSchedule::m_OperabilityCache
		//Operability cache
		static boost::unordered_map<Core::FMTDevelopment,
			std::vector<int>>m_OperabilityCache;
		//DocString: FMTSpatialSchedule::m_Movables
		//Coordinates where you can actualy operate something.
		static std::vector<Spatial::FMTcoordinate>m_Movables;
		//DocString: FMTSpatialSchedule::m_BUFFER_LOOKUP
		//buffer lookup for events
		static const int m_BUFFER_LOOKUP = 1;
		
		// DocString: FMTSpatialSchedule::_evaluateSpatialAdjacency
		 /**
		evaluate adjacency conflicts for each events
		*/
		double _evaluateSpatialAdjacency(
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;

		double _evaluateSpatialGroups(
			const FMTSpatialGraphs& p_SpatialGraph,
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			int p_theme,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;

		std::vector<FMTeventcontainer::const_iterator> _getSpatialGroupsConflict(
			const FMTSpatialGraphs& p_SpatialGraph,
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			int p_theme,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;

		std::vector<FMTeventcontainer::const_iterator> _getAdjacencyConflict(
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;


		size_t _getNonSpatialCellsCount() const;

		static double _getExponentialFactorization(double p_value, double p_factor);

		double _getConstraintFloorValue(double p_inValue) const;

		double _getRulesEvaluation(const std::vector<FMTPatchRules>& p_rules,
			const FMTSpatialGraphs& p_SpatialGraph) const;

		static std::vector<std::pair<Core::FMTDevelopment, std::vector<int>>> _putTabouInCache(
			const Models::FMTModel& p_model,
			std::map<Core::FMTDevelopment, std::vector<bool>>& p_tabou);
		static void _setTabouOutOfCache(
			std::vector<std::pair<Core::FMTDevelopment, std::vector<int>>>& p_GoodValues);

		

		

		
		
		
};
}


#endif // FMTSpatialSchedule_H
