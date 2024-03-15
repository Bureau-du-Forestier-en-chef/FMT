/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTSPATIALSCHEDULE_H
#define FMTSPATIALSCHEDULE_H

#include "FMTlayer.hpp"
#include "FMTlinegraph.hpp"
#include "FMTeventcontainer.hpp"
#include "FMTbindingspatialaction.hpp"
#include "FMTspatialnodescache.hpp"

namespace Core {
	class FMTGCBMtransition;
	class FMToutput;
	class FMTmodel;
	class FMTconstraint;
	class FMTyields;
	class FMTaction;
	class FMTdevelopment;
}



namespace Spatial
{
class FMTforest;
enum  FMTspatialscheduletype
	{
		FMTcomplete = 1,
		FMTpartial = 2
	};

// DocString: FMTspatialschedule
/**
This class is a map containing a linear graph for each cell. The graph represent the stand,
the action and the transition at each each period for the cell. Can be build randomly or
with a schedule.
*/
class FMTEXPORT FMTspatialschedule : public FMTlayer<Graph::FMTlinegraph>
{
	mutable FMTspatialnodescache cache;
	FMTspatialscheduletype scheduletype;
	std::vector<double>constraintsfactor;
    public:
		typedef std::vector<std::vector<Spatial::FMTbindingspatialaction>> actionbindings;
		// DocString: FMTspatialschedule(FMTspatialschedule&&)
		/**
		Default move constructor for FMTspatialschedule.
		*/
		FMTspatialschedule(FMTspatialschedule&& rhs) noexcept;
        // DocString: FMTspatialschedule()
		/**
		Default constructor of FMTspatialschedule
		*/
        FMTspatialschedule();
        // DocString: FMTspatialschedule(const FMTforest)
		/**
		Constructor of FMTspatialschedule based on FMTforest. It's initializing
		every graph in the map base on developments types in each cell.
		*/
        FMTspatialschedule(const FMTforest& initialmap);
        // DocString: ~FMTspatialschedule()
		/**
		Default destructor of FMTspatialschedule
		*/
        virtual ~FMTspatialschedule() = default;
        // DocString: FMTspatialschedule(const FMTspatialschedule)
		/**
		Copy constructor of FMTspatialschedule
		*/
        FMTspatialschedule(const FMTspatialschedule& other);
		// DocString: FMTspatialschedule(const FMTspatialschedule,const std::vector<FMTcoordinate>)
		/**
		Create a partial copy of the complete solution base on coordinates.
		*/
		FMTspatialschedule(const FMTspatialschedule& other,
			const std::vector<FMTcoordinate>::const_iterator& firstcoord,
			const std::vector<FMTcoordinate>::const_iterator& endcoord);
        // DocString: FMTspatialschedule::=
		/**
		Copy assignment of FMTspatialschedule
		*/
        FMTspatialschedule& operator=(const FMTspatialschedule& rhs);
        // DocString: FMTspatialschedule::==
		/**
		Comparison operator equal to
		*/
        bool operator == (const FMTspatialschedule& rhs)const;
        // DocString: FMTspatialschedule::!=
		/**
		Comparison operator different than
		*/
        bool operator != (const FMTspatialschedule& rhs)const;
        // DocString: FMTspatialschedule::empty
        /**
        Test whether the map is empty.
        */
        bool empty() const {return mapping.empty();};
        // DocString: FMTspatialschedule::actperiod
        /**
        Return the last period in the graph which is the active one.
        */
        int actperiod() const;
        // DocString: FMTspatialschedule::copyfromselected(const FMTspatialschedule, const std::vector<size_t>)
        /**

        */
        bool copyfromselected(const FMTspatialschedule& rhs, const std::vector<size_t>& selected);
        // DocString: FMTspatialschedule::swapfromselected
        /**

        */
		bool swapfromselected(FMTspatialschedule& rhs, const std::vector<size_t>& selected);
        // DocString: FMTspatialschedule::getforestperiod
        /**
        Return the FMTforest corresponding to the period asked. If periodstart, the forest pass wil be the one before the actions as been set otherwise, it will be the forest after the actions as been set. 
        */
        FMTforest getforestperiod(const int& period,bool periodstart=false) const;
		// DocString: FMTspatialschedule::allow_action
		/**
		Check in all events around the location during periods corresponding to green up delay
		if an action present in neighbors is in the adjacency limit.
		*/
		bool allow_action(const int& targetaction,const std::vector<Spatial::FMTbindingspatialaction>&bindingactions,const FMTcoordinate& location, const int& period) const;
	   // DocString: FMTspatialschedule::getupdatedscheduling
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

	   // DocString: FMTspatialschedule::evaluatespatialconstraint
		/**
		Return the constraint evaluation value of a spatial constraint. If the subset is not a nullptr the 
		*/
	   double evaluatespatialconstraint(const Core::FMTconstraint& spatialconstraint,
		   const Models::FMTmodel& model/*, const FMTeventcontainer* subset = nullptr*/) const;
	   // DocString: FMTspatialschedule::evaluatedistance
		/**
		Return the constraint evaluation value of a spatial constraint.
		*/
	   /*double evaluatedistance(const FMTevent& eventof,
		   const double& lowerdistancetoevent,
		   const double& upperdistancetoevent,
		   const int& period, const std::vector<bool>& actionsused,
		   std::unordered_set<size_t>& relations,
		   const std::vector<FMTeventcontainer::const_iterator>& events) const;*/
		// DocString: FMTspatialschedule::verifyspatialfeasability
		/**
		For the target action, return a set of FMTcoordinate corresponding to the cells that are spatially allowable from coordinates that are operables. Mainly only check for coord
		respecting the greenup constraint.
		*/
		std::set<FMTcoordinate> verifyspatialfeasability(const int& targetaction,
			const std::vector<Spatial::FMTbindingspatialaction>& bindingactions,
			const int& period, const std::set<FMTcoordinate>& operables) const;
		// DocString: FMTspatialschedule::buildharvest
		/**

		*/
		FMTeventcontainer buildharvest(const double& target, const Spatial::FMTbindingspatialaction& targetaction, std::default_random_engine& generator, std::set<FMTcoordinate> mapping_pass,
			const int& previousperiod, const int& actionid, std::vector<FMTcoordinate>& operated) const;
		// DocString: FMTspatialschedule::operateevents
		/**

		*/
		double operateevents(const FMTeventcontainer& cuts,const Core::FMTaction& action, const int& action_id, const Core::FMTtransition& Transition,
					 const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes);
		// DocString: FMTspatialschedule::operatecoord
		/**
		 Operate a coordinate and add it to the events ... That's why the FMTbindingspatialaction is needed
		*/
		void operatecoord(const FMTcoordinate& coord,const Core::FMTaction& action, const int& action_id, const FMTbindingspatialaction& bindingspaction, const Core::FMTtransition& Transition,
					 const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes);
		// DocString: FMTspatialschedule::grow
		/**

		*/
		void grow();
		// DocString: FMTspatialschedule::setnewperiod
		/**

		*/
		//void setnewperiod();
		// DocString: FMTspatialschedule::getschedules
		/**
		Return operated schedules from linegraph. 
		*/
		std::vector<Core::FMTschedule> getschedules(const std::vector<Core::FMTaction>& modelactions,bool withlock=false) const;
		// DocString: FMTspatialschedule::getgraphsoutputs
		/**
			Return sum of all graphs outputs related to constraint.
		*/
		std::vector<double> getgraphsoutputs(const Models::FMTmodel& model, const Core::FMTconstraint& constraint,
											 const FMTspatialschedule*	friendlysolution = nullptr) const;
		// DocString: FMTspatialschedule::isbetterthan
		/**
			Compare two spatialschedule and return a vector of bool with true if the constraint group has a better value then the
			compared solution else false.
		*/
		std::vector<int> isbetterthan(const FMTspatialschedule& newsolution,
									 const Models::FMTmodel& model) const;
		// DocString: FMTspatialschedule::getconstraintevaluation
		/**
			Returns the double value of the evaluated solution constraint.
		*/
		double getconstraintevaluation(const Core::FMTconstraint&constraint,
			const Models::FMTmodel& model,const FMTspatialschedule*	friendlysolution = nullptr) const;
		// DocString: FMTspatialschedule::getconstraintsvalues
		/**
			Fill up a vector of values for for each contraints (used for normalization)
		*/
		std::vector<double> getconstraintsvalues(const Models::FMTmodel& model,
			const FMTspatialschedule*	friendlysolution = nullptr) const;
		// DocString: FMTspatialschedule::getweightedfactors
		/**
			Generates factors based on the actual solution.
		*/
		std::vector<double> getweightedfactors(const Models::FMTmodel& model,
			const FMTspatialschedule*	friendlysolution = nullptr) const;
		// DocString: FMTspatialschedule::getdualinfeasibility
		/**
			Returns dual infeasibility of a set of constraints.
		*/
		double getprimalinfeasibility(const std::vector<const Core::FMTconstraint*>& constraints,
			const Models::FMTmodel& model,const FMTspatialschedule*	friendlysolution = nullptr, bool withfactorization = false) const;
		// DocString: FMTspatialschedule::logsolutionstatus
		/**
			Log the status of the solution
		*/
		void logsolutionstatus(const size_t& iteration, const double& objective, const double& primalinfeasibility) const;
		// DocString: FMTspatialschedule::getsolutionstatus
		/**
			Get the primal infeasibility and objective value
		*/
		void getsolutionstatus(double& objective, double& primalinfeasibility,const Models::FMTmodel& model,
			const FMTspatialschedule*	friendlysolution = nullptr, bool withsense = true, bool withfactorization = false,bool withspatial = true) const;
		// DocString: FMTspatialschedule::getglobalobjective
		/**
		Usefull to evaluate the quality of the solution it mixes objective to infeasibility and return it has double
		the lower the returned value is better is the solution. You can get a negative global objective.
		*/
		double getglobalobjective(const Models::FMTmodel& model,
			const FMTspatialschedule*	friendlysolution = nullptr) const;
		// DocString: FMTspatialschedule::getobjectivevaluey
		/**
			Returns the objective value of the spatialschedule
		*/
		double getobjectivevalue(const Core::FMTconstraint& constraint, const Models::FMTmodel& model,
			const FMTspatialschedule*	friendlysolution = nullptr,bool withsense = true) const;
		// DocString: FMTspatialschedule::setgraphfromcache
		/**
			Removes the cached values for every nodes of the model of a given graph.If remove = false it add values to cache
		*/
		void setgraphfromcache(const Graph::FMTlinegraph& graph, const Models::FMTmodel& model, const int&startingperiod, bool remove = true);

		// DocString: FMTspatialschedule::getpatchstats
		/**
			
		*/
		std::string getpatchstats(const std::vector<Core::FMTaction>& actions) const;
		// DocString: FMTspatialschedule::lastdistlayer
		/**
			Return sum of all graphs outputs related to constraint.
		*/
		FMTlayer<std::string> lastdistlayer(const std::vector<Core::FMTaction>& modelactions, const int& period) const;
		// DocString: FMTspatialschedule::getGCBMtransitions
		/**
		
		*/
		std::vector<Core::FMTGCBMtransition> getGCBMtransitions(FMTlayer<std::string>& stackedactions, const std::vector<Core::FMTaction>& modelactions, const std::vector<Core::FMTtheme>& classifiers, const int& period) const;
		// DocString: FMTspatialschedule::getpredictors
		/**
			Get the predictors for the whole solution and write down the predictorsids into a layer.
		*/
		std::vector<std::vector<Graph::FMTpredictor>> getpredictors(FMTlayer<int>& predictorids, const Models::FMTmodel& model, const std::vector<std::string>& yieldnames, const int& period,bool periodonevalues = false,bool withGCBMid = true) const;
		
		// DocString: FMTspatialschedule::eraselastperiod
		 /**
		 This function erase the last period of the FMTspatialschedule.
		 */
		void eraselastperiod();
		// DocString: FMTspatialschedule::getbindingactions
		 /**
		 Get the binding actions based on model constraints.
		 */
		std::vector<Spatial::FMTbindingspatialaction> getbindingactions(const Models::FMTmodel& model, const int& period) const;
		// DocString: FMTspatialschedule::getbindingactionsbyperiod
		 /**
		 Get the binding actions based on model constraints in a vector by period.
		 */
		actionbindings getbindingactionsbyperiod(const Models::FMTmodel& model) const;
		// DocString: FMTspatialschedule::referencebuild
		/**
		This is the main function to simulate a schedule of actions (schedule) on the actual
		spatialy explicit forest. If the (schedule_only) switch is turned on the simulator wont try
		to find some operable developements (not present in the potential schedule)
		even if the area harvested target for that action is not reach. The user can also set the seed
		to get different solutions from the simulator.
		*/
		std::map<std::string, double> referencebuild(const Core::FMTschedule& schedule, const Models::FMTmodel& model,
										const std::vector<boost::unordered_set<Core::FMTdevelopment>>& scheduleoperabilities,
										bool schedule_only = true,
										bool scheduleatfirstpass = true,
										unsigned int seed = 0);
		// DocString: FMTspatialschedule::greedyreferencebuild
		/**
		This function call multiple time the simulate function to find the best possible spatialisation for
		a given schedule using random draw. It uses a schedule of actions (schedule) on the actual
		spatialy explicit forest. If the (schedule_only) switch is turned on the simulator wont try
		to find some operable developements (not present in the potential schedule)
		even if the area harvested target for that action is not reach. The user can also set the seed
		to get different solutions from the simulator.
		*/
		std::map<std::string, double> greedyreferencebuild(const Core::FMTschedule& schedule, const Models::FMTmodel& model,
										const size_t& randomiterations,
										unsigned int seed = 0,
										double tolerance = FMT_DBL_TOLERANCE,
										bool log = true);
		// DocString: FMTspatialschedule::randombuild
		/**
		With a generator randomly create a solution for one period.
		*/
		Graph::FMTgraphstats randombuild(const Models::FMTmodel& model, std::default_random_engine& generator);
		// DocString: FMTspatialschedule::perturbgraph
		/**
		Change one graph in the solution remove it's contribution to objective and add contribution to the newly generated to the objective.
		*/
		void perturbgraph(const FMTcoordinate& coordinate,const int& period,
			const Models::FMTmodel& model, std::default_random_engine& generator,
			const actionbindings& bindings);
		// DocString: FMTspatialschedule::isbetterbygroup
		/**
		Compare solution by constraint group.
		*/
		bool isbetterbygroup(const FMTspatialschedule& rhs, const Models::FMTmodel& model) const;
		// DocString: FMTspatialschedule::swap
		/**
		Swap operator for FMTspatialschedule.
		*/
		void swap(FMTspatialschedule& rhs);
		// DocString: FMTspatialschedule::getmovablecoordinates
		/**
		
		*/
		std::vector<Spatial::FMTcoordinate>getmovablecoordinates(const Models::FMTmodel& model,const int& period,
																					const std::vector<Spatial::FMTcoordinate>* statics,
																					boost::unordered_map<Core::FMTdevelopment, bool>*operability = nullptr) const;

		// DocString: FMTspatialschedule::getperiodwithmaximalevents
		/**
		Get the period at with you got the maximal number of  periods.
		*/
		int getperiodwithmaximalevents(const std::vector<bool>& actions) const;
		// DocString: FMTspatialschedule::getareaconflictcoordinates
		/**
		Returns the coordinates of events that does not have the right area in the worst period... and set the worst period
		*/
		std::vector<std::vector<Spatial::FMTcoordinate>>getareaconflictcoordinates(const actionbindings& bindingactions,const int& period,bool conflictonly=true) const;
		// DocString: FMTspatialschedule::getadjacencyconflictcoordinates
		/**
		Returns adjacency conflicts coordinate that need to be destroyed
		*/
		std::vector<std::vector<Spatial::FMTcoordinate>>getadjacencyconflictcoordinates(const actionbindings& bindingactions,const int& period, bool conflictonly = true) const;
		// DocString: FMTspatialschedule::getstaticsmovablecoordinates
		/**
		Returns a vector of coordinate that are considered movable
		*/
		std::vector<Spatial::FMTcoordinate>getstaticsmovablecoordinates(const Models::FMTmodel& model) const;
		// DocString: FMTspatialschedule::ispartial
		/**
		return true if solution is partial.
		*/
		bool ispartial() const;
		// DocString: FMTspatialschedule::emptyevents
		/**
		Return true if there's no events
		*/
		bool emptyevents() const;
		// DocString: FMTspatialschedule::copyfrompartial
		/**
		Copy elements from a partial solution.
		*/
		void copyfrompartial(const FMTspatialschedule& rhs);
		// DocString: FMTspatialschedule::copyfrompartial
		/**
		Copy elements from a partial solution but also allow swap.
		*/
		void copyfrompartial(FMTspatialschedule& rhs);
		// DocString: FMTspatialschedule::setconstraintsfactor
		/**
		Set the constraints factors for nomalization
		*/
		void setconstraintsfactor(const Models::FMTmodel& model,const std::vector<double>&factors);
		// DocString: FMTspatialschedule::needsrefactortorization
		/**
		Return true if the solution looks unscaled and need new factors
		*/
		bool needsrefactortorization(const Models::FMTmodel& model) const;
		// DocString: FMTspatialschedule::dorefactortorization
		/**
		Return true if the solution looks unscaled and need new factors
		*/
		void dorefactortorization(const Models::FMTmodel& model);
		// DocString: FMTspatialschedule::getconstraintsfactor
		/**
		Get the constraints factors for nomalization
		*/
		std::vector<double> getconstraintsfactor() const;
		// DocString: FMTspatialschedule::getoutput
		/**
		Get the output value of a output for a given period using the solution..
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or developement name if level == FMToutputlevel::developpement
		*/
		std::map<std::string, std::vector<double>> getoutput(const Models::FMTmodel & model, const Core::FMToutput& output,
			const int& periodstart, const int& periodstop,
			Core::FMToutputlevel level = Core::FMToutputlevel::totalonly) const;
		// DocString: FMTspatialschedule::getoutput
		/**
		Get the output value for the whole layer for a given period and output.
		*/
		FMTlayer<double> getoutput(const Models::FMTmodel& model, const Core::FMToutput& output,const int& period) const;
		// DocString: FMTspatialschedule::getoutputbycoordinate
		/**
		Return the output value by coordinate for a given output/model/period.
		*/
		std::vector<std::pair<FMTcoordinate, double>>getoutputbycoordinate(const Models::FMTmodel & model,
			const Core::FMToutput& output, const int& period) const;
		// DocString: FMTspatialschedule::postsolve
		/**
		postsolve the spatial solution.
		*/
		void postsolve(const Core::FMTmaskfilter&  filter,const std::vector<Core::FMTaction>& presolveactions,const Models::FMTmodel& originalbasemodel);
		// DocString: FMTspatialschedule::presolve
		/**
		@brief Returned a presolved solution of the original solution.
		@param[in] p_filter filter to keep mask
		@param[in] p_presolvedThemes The themes of the presolved model.
		@return A presolved FMTspatialschedule.
		*/
		FMTspatialschedule presolve(const Core::FMTmaskfilter& p_filter, const std::vector<Core::FMTtheme>& p_presolvedThemes) const;
		// DocString: FMTsemodel::getarea
		/**
		@brief Get the area of a given period based on the solution of the model.
		@param[in] period the period selected
		@param[in] beforegrowanddeath true if we want before the growth (true) or after (false)
		@return the vector of actualdevelopment...
		*/
		std::vector<Core::FMTactualdevelopment>getarea(int period = 0, bool beforegrowanddeath = false) const;
	protected:
		// DocString: FMTspatialschedule::events
		/**
		
		*/
        FMTeventcontainer events;
		// DocString: FMTspatialschedule::getfromevents(const Core::FMTconstraint&, const std::vector<Core::FMTaction>&, const int&, const int&)
		 /**
		 Get theline graph using the eventcontainer
		 */
		std::vector<const Graph::FMTlinegraph*>getfromevents(const Core::FMTconstraint& constraint, const std::vector<Core::FMTaction>& actions, const int& start, const int& stop) const;
		// DocString: FMTspatialschedule::getfromevents(const Core::FMToutputnode&, const std::vector<Core::FMTaction>&, const int&)
		 /**
		 Get the coordinate presents in the events for the outputnode, the action and the period asked.
		 */
		std::vector<FMTcoordinate>getfromevents(const Core::FMToutputnode& node, const std::vector<Core::FMTaction>& actions, const int& period) const;
		// DocString: FMTspatialschedule::getoutputfromgraph
		 /**
		 Get the output requested from a given linegraph into periods_values
		 */
		std::map<std::string,double> getoutputfromgraph(const Graph::FMTlinegraph& linegraph, const Models::FMTmodel & model,
			const Core::FMToutputnode& node, const double* solution,const int&period, const Core::FMTmask& nodemask,
			boost::unordered_map<Core::FMTmask, double>& nodecache, Core::FMToutputlevel level = Core::FMToutputlevel::totalonly) const;

		// DocString: FMTspatialschedule::setgraphcachebystatic
		 /**
		 
		 */
		void setgraphcachebystatic(const std::vector<FMTcoordinate>& coordinates, const Core::FMToutputnode& node) const;
		// DocString: FMTspatialschedule::getmaximalpatchsizes
		 /**
		 Return the maximal patch size of a vector of spatialactions.
		 */
		//std::vector<size_t>getmaximalpatchsizes(const std::vector<FMTspatialaction>& spactions) const;
		// DocString: FMTspatialschedule::inscheduleoperabilities
		 /**
		
		 */
		bool inscheduleoperabilities(const std::vector<boost::unordered_set<Core::FMTdevelopment>>& scheduleoperabilities,
			Core::FMTdevelopment const* dev,const int& actionid, const Core::FMTaction& action) const;
		// DocString: FMTspatialschedule::getoutputfromnode
		 /**
		 Return the value of a given node at different linegraph const iterator
		 */
		std::vector<FMTlayer<Graph::FMTlinegraph>::const_iterator> getoutputfromnode(const Models::FMTmodel& model, const Core::FMToutputnode& node, const int& period) const;
    private:
		// DocString: FMTspatialschedule::evaluatespatialadjacency
		 /**
		Evaluate adjacency conflicts for each events
		*/
		double evaluatespatialadjacency(
			const int& period,
			const size_t& greenup,
			const size_t& lowerlookup,
			const size_t& upperlookup,
			const bool& testlower,
			const bool& testupper,
			std::vector<FMTeventcontainer::const_iterator>& conflicts,
			boost::unordered_set<FMTeventrelation>& relations,
			const std::vector<bool>& actionused) const;
		
};
}


#endif // FMTSPATIALSCHEDULE_H
