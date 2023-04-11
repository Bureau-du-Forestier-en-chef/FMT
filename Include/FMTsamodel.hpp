/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSAMODEL_H
#define FMTSAMODEL_H

#include "FMTsemodel.hpp"
#include <memory>
#include <vector>
#include <random>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Spatial
{
	class FMTforest;
    class FMTsaschedule;
    class FMTexponentialschedule;
}


namespace Models
{
/**
Different techniques to find the initial temperature of the simulated annealing, found in
Ben-Ameur, W. 2004. Computing the Initial Temperature of Simulated Annealing. Comput. Optim. Appl. (December). doi:10.1023/B:COAP.0000044187.23143.bd.
*/
enum class FMTsawarmuptype
    {
    log = 1,
    bigdelta =2,
    logmax = 3,
    bootstrapmagic = 4
    };

/**
This model is an area restricted model (ARM) using the simulated annealing
meta-heuristic to solve the spatial optimization problem. Constraints must
be goals with weight and the objective function must maximize or minimize
those constraints. Only the functions needed to build a simulated annealing
algorithm are implemented in this class, the algorithm must be written by the
user using the functions exposed in this class.

An FMTforest is needed to set the initial map. An FMTsaschedule is needed as
cooling schedule and FMTspatialaction must be set for the model.
*/

class FMTEXPORT FMTsamodel final: public FMTsemodel
    {
    // DocString: FMTsamodel::Serialize
    /**
    Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
    */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::make_nvp("semodel", boost::serialization::base_object<FMTsemodel>(*this));
    }
    // DocString: FMTsamodel()
    /**
    Constructor for presolve use
    */
    FMTsamodel(const FMTsemodel& rhs);
    protected:
        // DocString: FMTsamodel::getcoordinatestomove
        /**
        Will return coordinates that might be good candidat to disturb
        */
        std::vector<Spatial::FMTcoordinate>getcoordinatestomove(const Spatial::FMTspatialschedule& actual, 
            const Spatial::FMTspatialschedule::actionbindings& bindings,
            const int& period, const std::vector<Spatial::FMTcoordinate>* movable,boost::unordered_map<Core::FMTdevelopment, bool>* operability) const;
		// DocString: FMTsamodel::evaluate
		/**
		Evaluate the actual and a candidat solution and return true if the candidat solution is choose to replace
		the actual solution.Based on a temp.
		*/
		bool evaluate(const double& temp,const Spatial::FMTspatialschedule& actual, const Spatial::FMTspatialschedule& candidat) const;
		// DocString: FMTsamodel::move
		/**
		Perturb a solution and produce a new one
		*/
		Spatial::FMTspatialschedule move(const Spatial::FMTspatialschedule& actual,
						const Spatial::FMTspatialschedule::actionbindings& bindings,
						const std::vector<Spatial::FMTcoordinate>*movable = nullptr,
						boost::unordered_map<Core::FMTdevelopment, bool>*operability= nullptr) const;
		// DocString: FMTsamodel::warmup
		/**
		Using an initprobability close to one, a base solution and a bunch of iterations get a initial temperature.
		*/
		double warmup(const Spatial::FMTspatialschedule& actual,
			const Spatial::FMTspatialschedule::actionbindings& bindings,
			const std::vector<Spatial::FMTcoordinate>*movable = nullptr,
			boost::unordered_map<Core::FMTdevelopment, bool>*operability = nullptr,
			double initprobability = 0.5,size_t iterations=10);
        // DocString: FMTsamodel::initialgrow
        /**
        Do an initial grow till you reach the length of the model with the actual solution
        */
        void initialgrow();
        // DocString: FMTsamodel::initialbuild
        /**
        Call a random build if there's no solution
        */
        void randombuild();
        // DocString: FMTsamodel::schedulesbuild
        /**
        Call schedules if there's no solution
        */
        void schedulesbuild(const std::vector<Core::FMTschedule>&schedules);
        // DocString: FMTsamodel::swap_ptr
        /**
        Swap with an abstract FMTmodel
        */
        virtual void swap_ptr(const std::unique_ptr<FMTmodel>& rhs);
	public:
		// DocString: FMTsamodel::initialsolve
		/**
		Try to solve the model from a coldstart.
		*/
		bool initialsolve();
        // DocString: FMTsamodel::build
        /**
        This function TRY to build the solution FMTschedule to spatialschedule if there's a schedule if not it will
        randomly build the model to be ready to solve.
        */
        virtual bool build(std::vector<Core::FMTschedule> schedules = std::vector<Core::FMTschedule>());
        // DocString: FMTsamodel::solve
        /**
        This function call initialsolve on the solver.
        */
        virtual bool solve();
        // DocString: FMTsamodel::presolve
        /**
        This function use a vector of developments and the actual transitions of the model and return new unique pointer to presolved FMTmodel.
        The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data if the model is badly formulated.
        */
        virtual std::unique_ptr<FMTmodel>presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments = std::vector<Core::FMTactualdevelopment>()) const;
	private:
        ///
       // Spatial::FMTsamovetype movetype;
        /// Range of ratio of the map to perturb at each iteration.
        double min_ratio_moves,max_ratio_moves;
        ///Path to were the outputs will be written.
		std::string outputs_write_location;
		///Counter for number of moves done.
        int number_of_moves;
        ///Container with the penalties value for each constraints.
        ///Must think about to change it maybe implement a new class.
        ///FMTsaeventcontainer == std::set<FMTsaevent>
		std::vector<std::map<std::string, std::pair<std::vector<double>, std::vector<double>>>> constraints_values_penalties;
		///Random number generator.
		mutable std::default_random_engine generator;
		///Spatial actions presents in the model.
		//std::vector<Spatial::FMTspatialaction> spactions;//should be FMTmodel action pointer...
		///Vector to track accepted solution base on the iteration number.
		std::vector<int> accepted_solutions;
		///Id in the map that are different between current and new solution.
		std::vector<size_t> mapidmodified;
		///Acceptation probability at each iterations.
		std::vector<double> probabs;
		///Cooling schedule for simulated annealing algorithm.
		std::unique_ptr<Spatial::FMTsaschedule> cooling_schedule;
		///Best solution
        //Spatial::FMTsasolution best_solution;
        ///Current solution
		//Spatial::FMTsasolution current_solution;
		///New solution generated from current. Empty when no moves has been done.
		//Spatial::FMTsasolution new_solution;
        ///Overloading move operator
		//Graph::FMTgraphstats g_move_solution(const double min_ratio,const double max_ratio);
        ///It's use to verify if solutions are not identical
        //bool comparesolutions() const;
    public:
        ///Constructor
        FMTsamodel();
        ///Destructor
        ~FMTsamodel();
        ///Copy constructor
        FMTsamodel(const FMTsamodel& rhs);
        ///Copy constructor to use parent as argument in constructor
        FMTsamodel(const FMTmodel& rhs);
        ///Copy constructor to use parent as argument in constructor
        FMTsamodel(const FMTmodel& rhs,const Spatial::FMTforest& forest);
        ///Copy assignment operator
        FMTsamodel& operator = (const FMTsamodel& rhs);
		// DocString: FMTsamodel::clone
		/**
		Get a clone of the FMTsamodel
		*/
		virtual std::unique_ptr<FMTmodel>clone() const final;
        /****************************************
        Functions to set parameters for the model.
        */

        ///Warmup function to set the initial temperature base on the initialization probability defined by user.
        ///Need to think a way to have a near feasible solution at start
        //double warmup(const double initprob, const size_t iterations,
         //             bool keep_best=false, FMTsawarmuptype type = FMTsawarmuptype::bootstrapmagic);
        ///Setter of the outputs_write_location.
        void write_outputs_at(std::string path);
        ///Setter of the cooling_schedule.
        bool setschedule(const Spatial::FMTexponentialschedule& schedule);
        ///Setter of spatial actions. See FMTspatialaction for info.
        //bool setspactions(const std::vector<Spatial::FMTspatialaction>& lspactions);
        ///Setter of min_ratio_moves and max_ratio_moves.
        bool set_min_max_moves(const double min_r,const double max_r);
        ///
        //bool set_movetype(const Spatial::FMTsamovetype movet);

        /*****************************************
        Functions to get informations on the model.
        */

        ///Getter of the number of moves done in the simulated annealing.
        int get_number_moves()const;
        ///Write info about the accepted solutions (iteration,probs,value,penaltie) at the output location.
        void get_outputs(std::string addon = "");
        ///Getter of the current solution
        //Spatial::FMTsasolution get_current_solution()const;
        ///Getter of the new_solution
       // Spatial::FMTsasolution get_new_solution()const;
        ///Getter for the cooling_schedule type
        std::string getcoolingscheduletype()const; 
        ///Write raster with events( of FMT actions) for every solution in the model.
        //void write_solutions_events(std::string out_path)const;//Write events
        ///Getter for spatial actions of the model.
		//std::vector<Spatial::FMTspatialaction> getspatialactions()const;

        /********************************
        Functions to manipulate the model.
        */

        ///Change new_solution to current and empty new_solution.
        //void acceptnew();
        ///Evaluate Metropolis criterion.
        bool testprobability(const double& p);
        ///Reduce temperature according to the cooling schedule.
        double cool_down(double temp)const;
        ///Compare two solutions base on the objective function.
        //bool evaluate(const double temp,bool all_data=false);
        ///To build initial solution. Return stats for all graphs created in the map.
        Graph::FMTgraphstats buildperiod();
		///Move operator for simulated annealing
		//Graph::FMTgraphstats move_solution();
        ///Setter of the map id modified
        bool setmapidmodified(const std::vector<size_t>& id);
    };
}

BOOST_CLASS_EXPORT_KEY(Models::FMTsamodel)

#endif // FMTSAMODEL_H
