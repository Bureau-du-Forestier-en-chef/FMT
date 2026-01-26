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

namespace Core {
	class FMTGCBMtransition;
	class FMToutput;
	class FMTmodel;
	class FMTconstraint;
	class FMTyields;
	class FMTaction;
	class FMTdevelopment;
}

namespace Models{
	class FMTsemodel;
}



namespace Spatial
{
class FMTforest;
class FMTSpatialGraphs;
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
	//mutable FMTspatialnodescache cache;
	FMTSpatialScheduletype scheduletype;
	std::vector<double>constraintsfactor;
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
        FMTSpatialSchedule();
        // DocString: FMTSpatialSchedule(const FMTforest,size_t)
		/**
		@brief Constructor of FMTSpatialSchedule based on FMTforest. It's initializing every graph in the map base on developments types in each cell.
		@param[in] p_InitialMap the initial map of the schedule.
		@param[in] p_LengthReserve the amount of memory to reserve for each linegraph.
		*/
        FMTSpatialSchedule(const FMTforest& p_InitialMap,
				size_t p_LengthReserve, FMTSpatialGraphs& p_SpatialGraph);
        // DocString: ~FMTSpatialSchedule()
		/**
		Default destructor of FMTSpatialSchedule
		*/
        virtual ~FMTSpatialSchedule() = default;
        // DocString: FMTSpatialSchedule(const FMTSpatialSchedule)
		/**
		Copy constructor of FMTSpatialSchedule
		*/
        FMTSpatialSchedule(const FMTSpatialSchedule& other);
		// DocString: FMTSpatialSchedule(const FMTSpatialSchedule,const std::vector<FMTcoordinate>)
		/**
		Create a partial copy of the complete solution base on coordinates.
		*/
		FMTSpatialSchedule(const FMTSpatialSchedule& other,
			const std::vector<FMTcoordinate>::const_iterator& firstcoord,
			const std::vector<FMTcoordinate>::const_iterator& endcoord);
        // DocString: FMTSpatialSchedule::=
		/**
		Copy assignment of FMTSpatialSchedule
		*/
        FMTSpatialSchedule& operator=(const FMTSpatialSchedule& rhs);
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
        // DocString: FMTSpatialSchedule::actperiod
        /**
        Return the last period in the graph which is the active one.
        */
        int actperiod() const;
        // DocString: FMTSpatialSchedule::getforestperiod
        /**
        Return the FMTforest corresponding to the period asked. If periodstart, the forest pass wil be the one before the actions as been set otherwise, it will be the forest after the actions as been set. 
        */
        FMTforest getforestperiod(const int& period,bool periodstart=false) const;
		// DocString: FMTSpatialSchedule::allow_action
		/**
		Check in all events around the location during periods corresponding to green up delay
		if an action present in neighbors is in the adjacency limit.
		*/
		bool allow_action(const int& targetaction,const std::vector<Spatial::FMTbindingspatialaction>&bindingactions,const FMTcoordinate& location, const int& period) const;
	   // DocString: FMTSpatialSchedule::getupdatedscheduling
		/**
		Return for all actions the FMTcoordinate with operable developments at the end of the graph.
		*/
	   std::vector<std::set<Spatial::FMTcoordinate>>getupdatedscheduling(
											const Models::FMTmodel& model,	
											 const std::vector<int>& actiontargets,
											boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>& cachedaction,
										  const std::vector<boost::unordered_set<Core::FMTdevelopment>>& scheduleoperabilities,
										   bool schedule_only = true,
										   std::vector<std::set<Spatial::FMTcoordinate>> original= std::vector<std::set<Spatial::FMTcoordinate>>(),
											std::vector<FMTcoordinate> updatedcoordinate= std::vector<FMTcoordinate>()) const;

	   // DocString: FMTSpatialSchedule::evaluatespatialconstraint
		/**
		Return the constraint evaluation value of a spatial constraint. If the subset is not a nullptr the 
		*/
	   double EvaluateSpatialConstraint(const Core::FMTconstraint& p_SpatialConstraint,
					const FMTSpatialGraphs& p_SpatialGraph) const;
	   // DocString: FMTSpatialSchedule::evaluatedistance
		/**
		Return the constraint evaluation value of a spatial constraint.
		*/
	   /*double evaluatedistance(const FMTevent& eventof,
		   const double& lowerdistancetoevent,
		   const double& upperdistancetoevent,
		   const int& period, const std::vector<bool>& actionsused,
		   std::unordered_set<size_t>& relations,
		   const std::vector<FMTeventcontainer::const_iterator>& events) const;*/
		// DocString: FMTSpatialSchedule::verifyspatialfeasability
		/**
		For the target action, return a set of FMTcoordinate corresponding to the cells that are spatially allowable from coordinates that are operables. Mainly only check for coord
		respecting the greenup constraint.
		*/
		std::set<FMTcoordinate> verifyspatialfeasability(const int& targetaction,
			const std::vector<Spatial::FMTbindingspatialaction>& bindingactions,
			const int& period, const std::set<FMTcoordinate>& operables) const;
		// DocString: FMTSpatialSchedule::buildharvest
		/**

		*/
		FMTeventcontainer buildharvest(const double& target, const Spatial::FMTbindingspatialaction& targetaction, std::default_random_engine& generator, std::set<FMTcoordinate> mapping_pass,
			const int& previousperiod, const int& actionid, std::vector<FMTcoordinate>& operated) const;
		// DocString: FMTSpatialSchedule::operateevents
		/**

		*/
		double operateevents(const FMTeventcontainer& cuts,const Core::FMTaction& action, const int& action_id, const Core::FMTtransition& Transition,
					 const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes);
		// DocString: FMTSpatialSchedule::operatecoord
		/**
		 Operate a coordinate and add it to the events ... That's why the FMTbindingspatialaction is needed
		*/
		void operatecoord(const FMTcoordinate& coord,const Core::FMTaction& action, const int& action_id, const FMTbindingspatialaction& bindingspaction, const Core::FMTtransition& Transition,
					 const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes);
		// DocString: FMTSpatialSchedule::grow
		/**

		*/
		void grow();
		// DocString: FMTSpatialSchedule::setnewperiod
		/**

		*/
		//void setnewperiod();
		// DocString: FMTSpatialSchedule::getschedules
		/**
		Return operated schedules from linegraph. 
		*/
		std::vector<Core::FMTschedule> getschedules(const FMTSpatialGraphs p_Graphs,bool withlock=false) const;
		// DocString: FMTSpatialSchedule::isbetterthan
		/**
			Compare two spatialschedule and return a vector of bool with true if the constraint group has a better value then the
			compared solution else false.
		*/
		std::vector<int> isbetterthan(const FMTSpatialSchedule& newsolution,
								const Spatial::FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getconstraintevaluation
		/**
			Returns the double value of the evaluated solution constraint.
		*/
		double getconstraintevaluation(const FMTSpatialGraphs& p_Graphs, size_t p_ConstraintId) const;
		// DocString: FMTSpatialSchedule::getconstraintsvalues
		/**
			Fill up a vector of values for for each contraints (used for normalization)
		*/
		std::vector<double> getconstraintsvalues(const Spatial::FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getweightedfactors
		/**
			Generates factors based on the actual solution.
		*/
		std::vector<double> getweightedfactors(const Spatial::FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getdualinfeasibility
		/**
			Returns dual infeasibility of a set of constraints.
		*/
		double getprimalinfeasibility(const std::vector<const Core::FMTconstraint*>& constraints,
			const Spatial::FMTSpatialGraphs& p_Graphs, bool withfactorization = false) const;
		// DocString: FMTSpatialSchedule::logsolutionstatus
		/**
			Log the status of the solution
		*/
		void logsolutionstatus(const size_t& iteration, const double& objective, const double& primalinfeasibility) const;
		// DocString: FMTSpatialSchedule::getsolutionstatus
		/**
			Get the primal infeasibility and objective value
		*/
		void getsolutionstatus(double& objective, double& primalinfeasibility,
			const FMTSpatialGraphs& p_Graphs, 
			bool withsense = true, bool withfactorization = false,bool withspatial = true) const;
		// DocString: FMTSpatialSchedule::getglobalobjective
		/**
		Usefull to evaluate the quality of the solution it mixes objective to infeasibility and return it has double
		the lower the returned value is better is the solution. You can get a negative global objective.
		*/
		double getglobalobjective(const FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::getobjectivevaluey
		/**
			Returns the objective value of the spatialschedule
		*/
		double getobjectivevalue(const FMTSpatialGraphs& p_Graphs,bool withsense = true) const;

		// DocString: FMTSpatialSchedule::getpatchstats
		/**
			
		*/
		std::string getpatchstats(const std::vector<Core::FMTaction>& actions) const;
		// DocString: FMTSpatialSchedule::lastdistlayer
		/**
			Return sum of all graphs outputs related to constraint.
		*/
		FMTlayer<std::string> lastdistlayer(const std::vector<Core::FMTaction>& modelactions, const int& period) const;
		// DocString: FMTSpatialSchedule::getGCBMtransitions
		/**
		
		*/
		std::vector<Core::FMTGCBMtransition> getGCBMtransitions(FMTlayer<std::string>& stackedactions, const std::vector<Core::FMTaction>& modelactions, const std::vector<Core::FMTtheme>& classifiers, const int& period) const;
		// DocString: FMTSpatialSchedule::getpredictors
		/**
			Get the predictors for the whole solution and write down the predictorsids into a layer.
		*/
		std::vector<std::vector<Graph::FMTpredictor>> getpredictors(FMTlayer<int>& predictorids, const Models::FMTmodel& model, const std::vector<std::string>& yieldnames, const int& period,bool periodonevalues = false,bool withGCBMid = true) const;
		
		// DocString: FMTSpatialSchedule::eraselastperiod
		 /**
		 This function erase the last period of the FMTSpatialSchedule.
		 */
		void eraselastperiod();
		// DocString: FMTSpatialSchedule::getbindingactions
		 /**
		 Get the binding actions based on model constraints.
		 */
		std::vector<Spatial::FMTbindingspatialaction> getbindingactions(const Models::FMTmodel& model, const int& period) const;
		// DocString: FMTSpatialSchedule::getbindingactionsbyperiod
		 /**
		 Get the binding actions based on model constraints in a vector by period.
		 */
		actionbindings getbindingactionsbyperiod(const Models::FMTmodel& model) const;
		// DocString: FMTSpatialSchedule::referencebuild
		/**
		This is the main function to simulate a schedule of actions (schedule) on the actual
		spatialy explicit forest. If the (schedule_only) switch is turned on the simulator wont try
		to find some operable developements (not present in the potential schedule)
		even if the area harvested target for that action is not reach. The user can also set the seed
		to get different solutions from the simulator.
		*/
		std::map<std::string, double> referencebuild(const Core::FMTschedule& schedule, const Models::FMTmodel& model,
										const std::vector<boost::unordered_set<Core::FMTdevelopment>>& scheduleoperabilities,
										std::default_random_engine& p_randomEngine,
										bool schedule_only = true,
										bool scheduleatfirstpass = true);
		// DocString: FMTSpatialSchedule::greedyreferencebuild
		/**
		This function call multiple time the simulate function to find the best possible spatialisation for
		a given schedule using random draw. It uses a schedule of actions (schedule) on the actual
		spatialy explicit forest. If the (schedule_only) switch is turned on the simulator wont try
		to find some operable developements (not present in the potential schedule)
		even if the area harvested target for that action is not reach. The user can also set the seed
		to get different solutions from the simulator.
		*/
		std::map<std::string, double> greedyreferencebuild(const Core::FMTschedule& schedule, const FMTSpatialGraphs& p_Graphs,
										const size_t& randomiterations,
										unsigned int seed = 0,
										double tolerance = FMT_DBL_TOLERANCE,
										bool log = true);
		// DocString: FMTSpatialSchedule::randombuild
		/**
		With a generator randomly create a solution for one period.
		*/
		Graph::FMTgraphstats randombuild(const Models::FMTmodel& model, std::default_random_engine& generator);
		// DocString: FMTSpatialSchedule::SetGrow
		/**
		@brief set natural growth on coordinates (remove every actions)
		@param[in] p_coordinates coordinates to set to growth
		@param[in] p_model the model to use
		*/
		void SetGrow(const std::vector<FMTcoordinate>& p_coordinates,
									const Models::FMTmodel& p_model);
		// DocString: FMTSpatialSchedule::perturbgraph
		/**
		Change one graph in the solution remove it's contribution to objective and add contribution to the newly generated to the objective.
		*/
		void perturbgraph(const FMTcoordinate& coordinate,const int& period,
			const Models::FMTmodel& model, std::default_random_engine& generator,
			const actionbindings& bindings);
		// DocString: FMTSpatialSchedule::isbetterbygroup
		/**
		Compare solution by constraint group.
		*/
		bool isbetterbygroup(const FMTSpatialSchedule& rhs, const FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::swap
		/**
		Swap operator for FMTSpatialSchedule.
		*/
		void swap(FMTSpatialSchedule& rhs);
		// DocString: FMTSpatialSchedule::getmovablecoordinates
		/**
		
		*/
		std::vector<Spatial::FMTcoordinate>getmovablecoordinates(const Models::FMTmodel& model,const int& period,
																					const std::vector<Spatial::FMTcoordinate>* statics,
																					boost::unordered_map<Core::FMTdevelopment, bool>*operability = nullptr) const;

		// DocString: FMTSpatialSchedule::getperiodwithmaximalevents
		/**
		Get the period at with you got the maximal number of  periods.
		*/
		int getperiodwithmaximalevents(const std::vector<bool>& actions) const;
		// DocString: FMTSpatialSchedule::getareaconflictcoordinates
		/**
		Returns the coordinates of events that does not have the right area in the worst period... and set the worst period
		*/
		std::vector<std::vector<Spatial::FMTcoordinate>>getareaconflictcoordinates(const actionbindings& bindingactions,const int& period,bool conflictonly=true) const;
		// DocString: FMTSpatialSchedule::getadjacencyconflictcoordinates
		/**
		Returns adjacency conflicts coordinate that need to be destroyed
		*/
		std::vector<std::vector<Spatial::FMTcoordinate>>getadjacencyconflictcoordinates(const actionbindings& bindingactions,const int& period, bool conflictonly = true) const;
		// DocString: FMTSpatialSchedule::getstaticsmovablecoordinates
		/**
		Returns a vector of coordinate that are considered movable
		*/
		std::vector<Spatial::FMTcoordinate>getstaticsmovablecoordinates(const Models::FMTmodel& model) const;
		// DocString: FMTSpatialSchedule::ispartial
		/**
		return true if solution is partial.
		*/
		bool ispartial() const;
		// DocString: FMTSpatialSchedule::emptyevents
		/**
		Return true if there's no events
		*/
		bool emptyevents() const;
		// DocString: FMTSpatialSchedule::copyfrompartial
		/**
		Copy elements from a partial solution.
		*/
		void copyfrompartial(const FMTSpatialSchedule& rhs);
		// DocString: FMTSpatialSchedule::copyfrompartial
		/**
		Copy elements from a partial solution but also allow swap.
		*/
		void copyfrompartial(FMTSpatialSchedule& rhs);
		// DocString: FMTSpatialSchedule::setconstraintsfactor
		/**
		Set the constraints factors for nomalization
		*/
		void setconstraintsfactor(const Models::FMTmodel& model,const std::vector<double>&factors);
		// DocString: FMTSpatialSchedule::needsrefactortorization
		/**
		Return true if the solution looks unscaled and need new factors
		*/
		bool needsrefactortorization(const FMTSpatialGraphs& p_Graphs) const;
		// DocString: FMTSpatialSchedule::dorefactortorization
		/**
		Return true if the solution looks unscaled and need new factors
		*/
		void dorefactortorization(const FMTSpatialGraphs& p_Graphs);
		// DocString: FMTSpatialSchedule::getConstraintsFactor
		/**
		Get the constraints factors for nomalization
		*/
		const std::vector<double>& getConstraintsFactor() const;
		// DocString: FMTSpatialSchedule::getoutput
		/**
		Get the output value of a output for a given period using the solution..
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or developement name if level == FMToutputlevel::developpement
		*/
		std::map<std::string,double> getoutput(const FMTSpatialGraphs& p_Graphs, const Core::FMToutput& p_output,
			int p_period,Core::FMToutputlevel level = Core::FMToutputlevel::totalonly) const;
		// DocString: FMTSpatialSchedule::getSpatialOutput
		/**
		Get the output value for the whole layer for a given period and output.
		*/
		FMTlayer<double> getSpatialOutput(const Models::FMTmodel& model,
						const Core::FMToutput& output,const int& period) const;
		// DocString: FMTSpatialSchedule::getoutputbycoordinate
		/**
		Return the output value by coordinate for a given output/model/period.
		*/
		std::vector<std::pair<FMTcoordinate, double>>getoutputbycoordinate(const Models::FMTmodel & model,
			const Core::FMToutput& output, const int& period) const;
		// DocString: FMTSpatialSchedule::postsolve
		/**
		@brief postsolve the spatial solution.
		@param[in] p_Filter filter used for presolve
		@param[in] p_PresolveActions the vector of presolved actions
		@param[in] p_OriginalBaseModel the original model.
		*/
		void postsolve(const Core::FMTmaskfilter&  p_Filter,
			const std::vector<Core::FMTaction>& p_PresolveActions,
			Spatial::FMTSpatialGraphs& p_Graphs);
		// DocString: FMTSpatialSchedule::presolve
		/**
		@brief Returned a presolved solution of the original solution.
		@param[in] p_filter filter to keep mask
		@param[in] p_presolvedThemes The themes of the presolved model.
		@param[in] the number of verticies to reserve per linegraph
		@return A presolved FMTSpatialSchedule.
		*/
		FMTSpatialSchedule presolve(const Core::FMTmaskfilter& p_filter,
			FMTSpatialGraphs& p_Graphs,size_t p_ReserveSize) const;
		// DocString: FMTSpatialSchedule::getarea
		/**
		@brief Get the area of a given period based on the solution of the model.
		@param[in] period the period selected
		@param[in] beforegrowanddeath true if we want before the growth (true) or after (false)
		@return the vector of actualdevelopment...
		*/
		std::vector<Core::FMTactualdevelopment>getarea(int period = 0, bool beforegrowanddeath = false) const;

		FMTSpatialSchedule GetBaseSchedule(const FMTSpatialGraphs& p_SpatialGraph) const;
		void SetSpatialGraphs(const Spatial::FMTSpatialSchedule& p_ToCopy, FMTSpatialGraphs& p_SpatialGraph);
		std::vector<FMTcoordinate> GetGroupsConflict(const Core::FMTconstraint& p_SpatialConstraint,
			const FMTSpatialGraphs& p_SpatialGraph) const;
	protected:
		// DocString: FMTSpatialSchedule::m_events
		/**
		
		*/
		FMTeventcontainer m_events;
		// DocString: FMTSpatialSchedule::getfromevents(const Core::FMTconstraint&, const std::vector<Core::FMTaction>&, const int&, const int&)
		 /**
		 Get theline graph using the eventcontainer
		 */
		std::vector<const Graph::FMTlinegraph*>getfromevents(const Core::FMTconstraint& constraint, const std::vector<Core::FMTaction>& actions, const int& start, const int& stop) const;
		// DocString: FMTSpatialSchedule::getfromevents(const Core::FMToutputnode&, const std::vector<Core::FMTaction>&, const int&)
		 /**
		 Get the coordinate presents in the events for the outputnode, the action and the period asked.
		 */
		std::vector<FMTcoordinate>getfromevents(const Core::FMToutputnode& node, const std::vector<Core::FMTaction>& actions, const int& period) const;
		// DocString: FMTSpatialSchedule::getoutputfromgraph
		 /**
		 Get the output requested from a given linegraph into periods_values
		 */
		std::map<std::string,double> getoutputfromgraph(const Graph::FMTlinegraph& linegraph, const Models::FMTmodel & model,
			const Core::FMToutputnode& node, const double* solution,const int&period, const Core::FMTmask& nodemask,
			boost::unordered_map<Core::FMTmask, double>& nodecache, const Core::FMTtheme*  p_theme,Core::FMToutputlevel level = Core::FMToutputlevel::totalonly) const;
		// DocString: FMTSpatialSchedule::getmaximalpatchsizes
		 /**
		 Return the maximal patch size of a vector of spatialactions.
		 */
		//std::vector<size_t>getmaximalpatchsizes(const std::vector<FMTspatialaction>& spactions) const;
		// DocString: FMTSpatialSchedule::inscheduleoperabilities
		 /**
		
		 */
		bool inscheduleoperabilities(const std::vector<boost::unordered_set<Core::FMTdevelopment>>& scheduleoperabilities,
			Core::FMTdevelopment const* dev,const int& actionid, const Core::FMTaction& action) const;
    private:
		// DocString: FMTSpatialSchedule::m_NonSpatialSolution
		/**
		The non spatial solution.
		*/
		std::vector<size_t>m_NonSpatialSolution;
		// DocString: FMTSpatialSchedule::_EvaluateSpatialAdjacency
		 /**
		Evaluate adjacency conflicts for each events
		*/
		double _EvaluateSpatialAdjacency(
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;

		double _EvaluateSpatialGroups(
			const FMTSpatialGraphs& p_SpatialGraph,
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			int p_theme,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;

		std::vector<FMTeventcontainer::const_iterator> _GetSpatialGroupsConflict(
			const FMTSpatialGraphs& p_SpatialGraph,
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			int p_theme,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;

		std::vector<FMTeventcontainer::const_iterator> _GetAdjacencyConflict(
			int p_period,
			int p_greenup,
			int p_lowerLookup,
			int p_upperLookup,
			bool p_testLower,
			const std::vector<bool>& p_actions) const;


		size_t _GetNonSpatialCellsCount() const;

		static const int BUFFER_LOOKUP = 1;
		
		
};
}


#endif // FMTSpatialSchedule_H
