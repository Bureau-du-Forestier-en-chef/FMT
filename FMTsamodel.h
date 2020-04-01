/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef FMTSAMODEL_H
#define FMTSAMODEL_H

#include "FMTmodel.h"
#include "FMTspatialaction.h"
#include "FMTlayer.h"
#include "FMTforest.h"
#include "FMTsaschedule.h"
#include "FMTsasolution.h"
#include <memory>
#include <vector>
#include <random>


namespace Models
{
/**
Different techniques to find the initial temperature of the simulated annealing, found in
Ben-Ameur, W. 2004. Computing the Initial Temperature of Simulated Annealing. Comput. Optim. Appl. (December). doi:10.1023/B:COAP.0000044187.23143.bd.
*/
enum class FMTsawarmuptype
    {
    log,
    bigdelta,
    logmax,
    bootstrapmagic
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

class FMTsamodel : public FMTmodel
    {
    protected:
        /// Range of ratio of the map to perturb at each iteration.
        double min_ratio_moves,max_ratio_moves;
        ///Path to were the outputs will be written.
		std::string outputs_write_location;
		///Counter for number of moves done.
        int number_of_moves;
        ///Container with the penalties value for each constraints.
        ///Must think about to change it maybe implement a new class.
		std::vector<std::map<std::string, std::pair<std::vector<double>, std::vector<double>>>> constraints_values_penalties;
		///Random number generator.
		std::default_random_engine generator;
		///Spatial actions presents in the model.
		std::vector<Spatial::FMTspatialaction> spactions;//should be FMTmodel action pointer...
		///Vector to track accepted solution base on the iteration number.
		std::vector<int> accepted_solutions;
		///Id in the map that are different between current and new solution.
		std::vector<size_t> mapidmodified;
		///Acceptation probability at each iterations.
		std::vector<double> probabs;
		///Cooling schedule for simulated annealing algorithm.
		std::unique_ptr<Spatial::FMTsaschedule> cooling_schedule;
		///Best solution
        Spatial::FMTsasolution best_solution;
        ///Current solution
		Spatial::FMTsasolution current_solution;
		///New solution generated from current. Empty when no moves has been done.
		Spatial::FMTsasolution new_solution;
        ///Overloading move operator
		Graph::FMTgraphstats g_move_solution(const double min_ratio,const double max_ratio, Spatial::FMTsamovetype movetype = Spatial::FMTsamovetype::shotgun);
        ///It's use to verify if solutions are not identical
        bool comparesolutions() const;
    public:
        ///Constructor
        FMTsamodel();
        ///Destructor
        ~FMTsamodel()=default;
        ///Copy constructor
        FMTsamodel(const FMTsamodel& rhs);
        ///Copy constructor to use parent as argument in constructor
        FMTsamodel(const FMTmodel& rhs);
        ///Copy assignment operator
        FMTsamodel& operator = (const FMTsamodel& rhs);

        /****************************************
        Functions to set parameters for the model.
        */

        ///Warmup function to set the initial temperature base on the initialization probability defined by user.
        ///Need to think a way to have a near feasible solution at start
        double warmup(const double initprob, const size_t iterations,
                      bool keep_best=false, FMTsawarmuptype type = FMTsawarmuptype::bootstrapmagic);
        ///Setter of the outputs_write_location.
        void write_outputs_at(std::string path);
        ///Setter of the cooling_schedule.
        bool setschedule(const Spatial::FMTexponentialschedule& schedule);
        ///Setter of the initial_mapping base on FMTforest.
        bool setinitial_mapping(const Spatial::FMTforest& forest);
        ///Setter of spatial actions. See FMTspatialaction for info.
        bool setspactions(const std::vector<Spatial::FMTspatialaction>& lspactions);
        ///Setter of min_ratio_moves and max_ratio_moves.
        bool set_min_max_moves(const double min_r,const double max_r);

        /*****************************************
        Functions to get informations on the model.
        */

        ///Getter of the number of moves done in the simulated annealing.
        int get_number_moves()const;
        ///Write info about the accepted solutions (iteration,probs,value,penaltie) at the output location.
        void get_outputs(std::string addon = "");
        ///Getter of the current solution
        Spatial::FMTsasolution get_current_solution()const;
        ///Getter of the new_solution
        Spatial::FMTsasolution get_new_solution()const;
        ///Getter for the cooling_schedule type
		std::string getcoolingscheduletype()const{return cooling_schedule->get_schedule_type();};
        ///Write raster with events( of FMT actions) for every solution in the model.
        void write_solutions_events(std::string out_path)const;//Write events
        ///Getter for spatial actions of the model.
		std::vector<Spatial::FMTspatialaction> getspatialactions()const;

        /********************************
        Functions to manipulate the model.
        */

        ///Change new_solution to current and empty new_solution.
        void acceptnew();
        ///Evaluate Metropolis criterion.
        bool testprobability(const double& p);
        ///Reduce temperature according to the cooling schedule.
        double cool_down(double temp)const{return cooling_schedule->reduce_temp(temp);};
        ///Compare two solutions base on the objective function.
        bool evaluate(const double temp,bool all_data=false);
        ///To build initial solution. Return stats for all graphs created in the map.
        Graph::FMTgraphstats buildperiod();
		///Move operator for simulated annealing
		Graph::FMTgraphstats move_solution(Spatial::FMTsamovetype movetype = Spatial::FMTsamovetype::shotgun);
        ///Setter of the map id modified
        bool setmapidmodified(const std::vector<size_t>& id);
    };
}

#endif // FMTSAMODEL_H
