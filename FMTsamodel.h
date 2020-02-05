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
   enum class FMTsawarmuptype
   {
        log,
        bigdelta,
        logmax,
        bootstrapmagic
    };
class FMTsamodel : public FMTmodel
    {
    protected:
        double min_ratio_moves,max_ratio_moves;
		std::string outputs_write_location;
        int number_of_moves;
		std::vector<std::map<std::string, std::pair<std::vector<double>, std::vector<double>>>> constraints_values_penalties;//move,constraint_name : <outputs,penalties>
		std::default_random_engine generator;
		std::vector<Spatial::FMTspatialaction> spactions;//should be FMTmodel action pointer...
		std::vector<int> accepted_solutions;
		std::vector<size_t> mapidmodified;//Id in the map that are different between current and new solution
		std::vector<double> probabs;
		std::unique_ptr<Spatial::FMTsaschedule> cooling_schedule;
        Spatial::FMTsasolution best_solution;
		Spatial::FMTsasolution current_solution;
		Spatial::FMTsasolution new_solution;
    public:
        FMTsamodel();//Constructor
        ~FMTsamodel()=default;
        FMTsamodel(const FMTsamodel& rhs);//Copy constructor
        FMTsamodel(const FMTmodel& rhs);//Copy constructor to use parent as argument in constructor
        FMTsamodel& operator = (const FMTsamodel& rhs);//Copy assignment operator
        //Setting parameters for the model
        double warmup(const double initprob, const size_t iterations, bool keep_best=false, FMTsawarmuptype type = FMTsawarmuptype::bootstrapmagic);
        void write_outputs_at(std::string path);
        bool setschedule(const Spatial::FMTexponentialschedule& schedule);//To set a schedule for the simulated annealing
        bool setinitial_mapping(const Spatial::FMTforest& forest);
        bool setspactions(const std::vector<Spatial::FMTspatialaction>& lspactions);//Set spatial action see FMTspatialaction for info
        bool set_min_max_moves(const double min_r,const double max_r);
        //Get informations
        int get_number_moves()const;
        void get_outputs(std::string addon = "") ;//Write outputs at outputs_write_location a the end of simulation
        Spatial::FMTsasolution get_current_solution()const;
        Spatial::FMTsasolution get_new_solution()const;
		std::string getcoolingscheduletype()const{return cooling_schedule->get_schedule_type();};
        void write_solutions_events(std::string out_path)const;//Write events
		std::vector<Spatial::FMTspatialaction> getspatialactions()const;
        //Functions to manipulate the model
        void acceptnew();//Change new_solution to current and empty new_solution
        bool testprobability(const double& p) ;//Metropolis criterion
        double cool_down(double temp)const{return cooling_schedule->reduce_temp(temp);};//Set a default cooling schedule to avoid crash
        bool evaluate(const double temp,bool all_data=false);//To compare the two solutions ... if new<current true
        Graph::FMTgraphstats buildperiod();//To build initial solution
		Graph::FMTgraphstats move_solution(Spatial::FMTsamovetype movetype = Spatial::FMTsamovetype::shotgun);//move_operator
		Graph::FMTgraphstats g_move_solution(const double min_ratio,const double max_ratio, Spatial::FMTsamovetype movetype = Spatial::FMTsamovetype::shotgun);//overloading move operator
        bool comparesolutions() const;//To verify if solutions are not identical
        bool setmapidmodified(const std::vector<size_t>& id);
    };
}

#endif // FMTSAMODEL_H
