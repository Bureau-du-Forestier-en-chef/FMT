#ifndef FMTSPATIALSCHEDULE_H
#define FMTSPATIALSCHEDULE_H

#include "FMTlayer.h"
#include "FMTforest.h"
#include "FMTspatialnodescache.h"
#include "FMTlinegraph.h"
#include "FMTGCBMtransition.h"
#include "FMTbindingspatialaction.h"


namespace Spatial
{
// DocString: FMTspatialschedule
/**
This class is a map containing a linear graph for each cell. The graph represent the stand,
the action and the transition at each each period for the cell. Can be build randomly or
with a schedule.
*/
class FMTspatialschedule : public FMTlayer<Graph::FMTlinegraph>
{
	mutable FMTspatialnodescache cache;
    public:
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
        // DocString: FMTspatialschedule::empty()
        /**
        Test whether the map is empty.
        */
        bool empty() const {return mapping.empty();};
        // DocString: FMTspatialschedule::lasperiod()
        /**
        Return the last period in the graph.
        */
        int actperiod() const;
        // DocString: FMTspatialschedule::copyfromselected(const FMTspatialschedule, const std::vector<size_t>)
        /**

        */
        bool copyfromselected(const FMTspatialschedule& rhs, const std::vector<size_t>& selected);
        // DocString: FMTspatialschedule::swapfromselected(const FMTspatialschedule, const std::vector<size_t>)
        /**

        */
		bool swapfromselected(FMTspatialschedule& rhs, const std::vector<size_t>& selected);
        // DocString: FMTspatialschedule::getforestperiod(const int)
        /**
        Return the FMTforest corresponding to the period asked.
        */
        FMTforest getforestperiod(const int& period) const;
		// DocString: FMTspatialschedule::allow_action
		/**
		Check in all events around the location during periods corresponding to green up delay
		if an action present in neighbors is in the adjacency limit.
		*/
		bool allow_action(const int& targetaction,const std::vector<Spatial::FMTbindingspatialaction>&bindingactions,const FMTcoordinate& location, const int& period) const;
	   // DocString: FMTspatialschedule::getupdatedscheduling()
		/**
		Return for all actions the FMTcoordinate with operable developments at the end of the graph.
		*/
	   std::vector<std::set<Spatial::FMTcoordinate>>getupdatedscheduling(
											const std::vector<Core::FMTaction>& actions,
										   const Core::FMTschedule& selection,
											boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>& cachedaction,
										   const Core::FMTyields& yields = Core::FMTyields(),
										   bool schedule_only = true,
										   std::vector<std::set<Spatial::FMTcoordinate>> original= std::vector<std::set<Spatial::FMTcoordinate>>(),
											std::vector<FMTcoordinate> updatedcoordinate= std::vector<FMTcoordinate>()) const;

	   // DocString: FMTspatialschedule::evaluateaspatialconstraint()
		/**
		Return the constraint evaluation value of a spatial constraint.
		*/
	   double evaluatespatialconstraint(const Core::FMTconstraint& spatialconstraint,const Models::FMTmodel& model) const;
		// DocString: FMTspatialschedule::getallowable
		/**
		For the target action, return a set of FMTcoordinate corresponding to the cells that are spatially allowable from coordinates that are operables.
		*/
		std::set<FMTcoordinate> verifyspatialfeasability(const int& targetaction,
			const std::vector<Spatial::FMTbindingspatialaction>& bindingactions,
			const int& period, const std::set<FMTcoordinate>& operables) const;
		// DocString: FMTspatialschedule::buildharvest
		/**

		*/
		FMTeventcontainer buildharvest(const double& target, const Spatial::FMTbindingspatialaction& targetaction, std::default_random_engine& generator, std::set<FMTcoordinate> mapping_pass,
			const int& previousperiod, const int& actionid, std::vector<FMTcoordinate>& operated) const;
		// DocString: FMTspatialschedule::operate(const FMTeventcontainer&, const std::vector<Core::FMTaction>&, const int&)
		/**

		*/
		double operate(const FMTeventcontainer& cuts,const Core::FMTaction& action, const int& action_id, const Core::FMTtransition& Transition,
					 const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes);
		// DocString: FMTspatialschedule::addevents(const FMTeventcontainer&)
		/**

		*/
		void addevents(const FMTeventcontainer& newevents);
		// DocString: FMTspatialschedule::grow()
		/**

		*/
		void grow();
		// DocString: FMTspatialschedule::setnewperiod()
		/**

		*/
		void setnewperiod();
		// DocString: FMTspatialschedule::getschedules(const std::vector<Core::FMTaction>)
		/**
		Return operated schedules from linegraph. 
		*/
		std::vector<Core::FMTschedule> getschedules(const std::vector<Core::FMTaction>& modelactions) const;
		// DocString: FMTspatialschedule::getgraphsoutputs()
		/**
			Return sum of all graphs outputs related to constraint.
		*/
		std::vector<double> getgraphsoutputs(const Models::FMTmodel& model, const Core::FMTconstraint& constraint,
											 const FMTspatialschedule*	friendlysolution = nullptr) const;
		// DocString: FMTspatialschedule::isbetterthan()
		/**
			Compare two spatialschedule and return a vector of bool with true if the constraint group has a better value then the
			compared solution else false.
		*/
		std::vector<int> isbetterthan(const FMTspatialschedule& newsolution,
									 const Models::FMTmodel& model) const;
		// DocString: FMTspatialschedule::getconstraintevaluation()
		/**
			Returns the double value of the evaluated solution constraint.
		*/
		double getconstraintevaluation(const Core::FMTconstraint&constraint,
			const Models::FMTmodel& model,const FMTspatialschedule*	friendlysolution = nullptr) const;
		// DocString: FMTspatialschedule::getdualinfeasibility()
		/**
			Returns dual infeasibility of a set of constraints.
		*/
		double getprimalinfeasibility(const std::vector<Core::FMTconstraint>& constraints, const Models::FMTmodel& model,
			const FMTspatialschedule*	friendlysolution = nullptr) const;
		// DocString: FMTspatialschedule::logsolutionstatus()
		/**
			Log the status of the solution
		*/
		void logsolutionstatus(const size_t& iteration, const double& objective, const double& primalinfeasibility) const;
		// DocString: FMTspatialschedule::getsolutionstatus()
		/**
			Get the primal infeasibility and objective value
		*/
		void getsolutionstatus(double& objective, double& primalinfeasibility,const Models::FMTmodel& model,
			const FMTspatialschedule*	friendlysolution = nullptr, bool withsense = true) const;
		// DocString: FMTspatialschedule::getglobalobjective
		/**
		Usefull to evaluate the quality of the solution it mixes objective to infeasibility and return it has double
		the lower the returned value is better is the solution. You can get a negative global objective.
		*/
		double getglobalobjective(const Models::FMTmodel& model,const FMTspatialschedule*	friendlysolution = nullptr) const;
		// DocString: FMTspatialschedule::getobjectivevaluey()
		/**
			Returns the objective value of the spatialschedule
		*/
		double getobjectivevalue(const Core::FMTconstraint& constraint, const Models::FMTmodel& model,
			const FMTspatialschedule*	friendlysolution = nullptr,bool withsense = true) const;
		// DocString: FMTspatialschedule::removegraphfromcache
		/**
			Removes the cached values for every nodes of the model of a given graph.
		*/
		void removegraphfromcache(const Graph::FMTlinegraph& graph, const Models::FMTmodel& model);
		// DocString: FMTspatialschedule::addgraphfromcache
		/**
			Removes the cached values for every nodes of the model of a given graph.
		*/
		void addgraphtocache(const Graph::FMTlinegraph& graph,const Models::FMTmodel& model);

		// DocString: FMTspatialschedule::getgraphsoutputs(const Models::FMTmodel&, const Core::FMTconstraint&, const int&, const int&)
		/**
			Return sum of all graphs outputs related to constraint.
		*/
		std::string getpatchstats(const std::vector<Core::FMTaction>& actions) const;
		// DocString: FMTspatialschedule::getgraphsoutputs(const Models::FMTmodel&, const Core::FMTconstraint&, const int&, const int&)
		/**
			Return sum of all graphs outputs related to constraint.
		*/
		FMTlayer<std::string> lastdistlayer(const std::vector<Core::FMTaction>& modelactions, const int& period) const;
		// DocString: FMTspatialschedule::getGCBMtransitions(const std::vector<Core::FMTtheme>&)
		/**
		
		*/
		std::vector<Core::FMTGCBMtransition> getGCBMtransitions(FMTlayer<std::string>& stackedactions, const std::vector<Core::FMTaction>& modelactions, const std::vector<Core::FMTtheme>& classifiers, const int& period) const;
		// DocString: FMTspatialschedule::eraselastperiod()
		 /**
		 This function erase the last period of the FMTspatialschedule.
		 */
		void eraselastperiod();
		// DocString: FMTspatialschedule::getbindingactions
		 /**
		 Get the binding actions based on model constraints.
		 */
		std::vector<Spatial::FMTbindingspatialaction> getbindingactions(const Models::FMTmodel& model, const int& period) const;
		// DocString: FMTspatialschedule::referencebuild
		/**
		This is the main function to simulate a schedule of actions (schedule) on the actual
		spatialy explicit forest. If the (schedule_only) switch is turned on the simulator wont try
		to find some operable developements (not present in the potential schedule)
		even if the area harvested target for that action is not reach. The user can also set the seed
		to get different solutions from the simulator.
		*/
		std::map<std::string, double> referencebuild(const Core::FMTschedule& schedule, const Models::FMTmodel& model,
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
										double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTspatialschedule::randombuild
		/**
		With a generator randomly create a solution for one period.
		*/
		Graph::FMTgraphstats randombuild(const Models::FMTmodel& model, std::default_random_engine& generator);
		// DocString: FMTspatialschedule::perturbgraph
		/**
		Change one graph in the solution remove it's contribution to objective and add contribution to the newly generated to the objective.
		*/
		void perturbgraph(const FMTcoordinate& coordinate,const Graph::FMTlinegraph& graph,const int& period, const Models::FMTmodel& model, std::default_random_engine& generator);
		// DocString: FMTspatialschedule::isbetterbygroup
		/**
		Compare solution by constraint group.
		*/
		bool isbetterbygroup(const FMTspatialschedule& rhs, const Models::FMTmodel& model) const;
		// DocString: FMTspatialschedule::getgraphs
		/**
		Returns a vector of const iterators to graphs.This thing will need to be cached to increase inficiency
		*/
		std::vector<std::map<Spatial::FMTcoordinate, Graph::FMTlinegraph>::const_iterator>getgraphs() const;
	protected:
		// DocString: FMTspatialschedule::events
		/**
		
		*/
        FMTeventcontainer events;
		// DocString: FMTspatialschedule::getfromevents()
		 /**
		 Get theline graph using the eventcontainer
		 */
		std::vector<const Graph::FMTlinegraph*>getfromevents(const Core::FMTconstraint& constraint, const std::vector<Core::FMTaction>& actions, const int& start, const int& stop) const;
		// DocString: FMTspatialschedule::getfromevents()
		 /**
		 Get theline graph using the eventcontainer
		 */
		std::vector<FMTcoordinate>getfromevents(const Core::FMToutputnode& node, const std::vector<Core::FMTaction>& actions, const int& period) const;
		// DocString: FMTspatialschedule::setoutputfromgraph
		 /**
		 set the output requested from a given linegraph into periods_values
		 */
		double getoutputfromgraph(const Graph::FMTlinegraph& linegraph, const Models::FMTmodel & model,
			const Core::FMToutputnode& node, const double* solution,const int&period, const size_t& hashvalue,
			std::unordered_map<size_t, double>& nodecache) const;
		// DocString: FMTspatialschedule::getgraphsbystatic()
		 /**
		 Based on variable outputnode in the constraint returns a subset of the solution based on the static themes.
		 */
		void setgraphcachebystatic(const std::vector<FMTcoordinate>& coordinates, const Core::FMToutputnode& node) const;
		// DocString: FMTspatialschedule::getmaximalpatchsizes()
		 /**
		 Return the maximal patch size of a vector of spatialactions.
		 */
		//std::vector<size_t>getmaximalpatchsizes(const std::vector<FMTspatialaction>& spactions) const;
    private:
};
}


#endif // FMTSPATIALSCHEDULE_H
