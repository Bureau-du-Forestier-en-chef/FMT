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

using namespace Spatial;

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
        string outputs_write_location;
        int number_of_moves;
        vector<map<string,pair<vector<double>,vector<double>>>> constraints_values_penalties;//move,constraint_name : <outputs,penalties>
        default_random_engine generator;
        vector<FMTspatialaction> spactions;//should be FMTmodel action pointer...
        vector<int> accepted_solutions;
        vector<size_t> mapidmodified;//Id in the map that are different between current and new solution
        vector<double> probabs;
        unique_ptr<FMTsaschedule> cooling_schedule;
        FMTsasolution best_solution;
        FMTsasolution current_solution;
        FMTsasolution new_solution;


    public:
        FMTsamodel();//Constructor
        ~FMTsamodel()=default;
        FMTsamodel(const FMTsamodel& rhs);//Copy constructor
        FMTsamodel(const FMTmodel& rhs);//Copy constructor to use parent as argument in constructor
        FMTsamodel& operator = (const FMTsamodel& rhs);//Copy assignment operator

        //Setting parameters for the model

        double warmup(const double initprob, const size_t iterations, bool keep_best=false, FMTsawarmuptype type = FMTsawarmuptype::bootstrapmagic);
        void write_outputs_at(string path);
        bool setschedule(const FMTexponentialschedule& schedule);//To set a schedule for the simulated annealing
        bool setinitial_mapping(const FMTforest& forest);
        bool setspactions(const vector<FMTspatialaction>& lspactions);//Set spatial action see FMTspatialaction for info
        bool set_min_max_moves(const double min_r,const double max_r);

        //Get informations

        int get_number_moves()const;
        void get_outputs(string addon = "") ;//Write outputs at outputs_write_location a the end of simulation
        FMTsasolution get_current_solution()const;
        FMTsasolution get_new_solution()const;
        string getcoolingscheduletype()const{return cooling_schedule->get_schedule_type();};
        void write_solutions_events(string out_path)const;//Write events
        vector<FMTspatialaction> getspatialactions()const;

        //Functions to manipulate the model

        void acceptnew();//Change new_solution to current and empty new_solution
        bool testprobability(const double& p) ;//Metropolis criterion
        double cool_down(double temp)const{return cooling_schedule->reduce_temp(temp);};//Set a default cooling schedule to avoid crash
        bool evaluate(const double temp,bool all_data=false);//To compare the two solutions ... if new<current true
        FMTgraphstats buildperiod();//To build initial solution
        FMTgraphstats move_solution(FMTsamovetype movetype = FMTsamovetype::shotgun);//move_operator
        FMTgraphstats g_move_solution(const double min_ratio,const double max_ratio, FMTsamovetype movetype = FMTsamovetype::shotgun);//overloading move operator
        bool comparesolutions() const;//To verify if solutions are not identical
        bool setmapidmodified(const vector<size_t>& id);
    };
}

#endif // FMTSAMODEL_H
