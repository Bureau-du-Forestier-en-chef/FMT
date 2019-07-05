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
#include <sstream>

using namespace Spatial;

namespace Models
{
class FMTsamodel : public FMTmodel
    {
    protected:
        bool write_outputs;
        string outputs_write_location;
        int number_of_moves;
        stringstream outputs_stream;
        default_random_engine generator;
        vector<FMTspatialaction> spactions;//should be FMTmodel action pointer...
        vector<int> bests_solutions;
        vector<size_t> mapidmodified;//Id in the map that are different between current and new solution
        unique_ptr<FMTsaschedule> cooling_schedule;
        FMTsasolution current_solution;
        FMTsasolution new_solution;


    public:
        FMTsamodel();//Constructor
        ~FMTsamodel()=default;
        FMTsamodel(const FMTsamodel& rhs);//Copy constructor
        FMTsamodel(const FMTmodel& rhs);//Copy constructor to use parent as argument in constructor
        FMTsamodel& operator = (const FMTsamodel& rhs);//Copy assignment operator

        //Setting parameters for the model

        void write_outputs_at(string path);
        bool setschedule(const FMTlinearschedule& schedule);//To set a schedule for the simulated annealing
        //bool setschedule(FMTexponentialschedule schedule) const;// need to be created
        bool setinitial_mapping(const FMTforest& forest);
        bool setspactions(const vector<FMTspatialaction>& lspactions);//Set spatial action see FMTspatialaction for info

        //Get informations

        void get_outputs();//Write outputs at outputs_write_location a the end of simulation
        FMTsasolution get_current_solution()const;
        FMTsasolution get_new_solution()const;
        string getcoolingscheduletype()const{return cooling_schedule->get_schedule_type();};
        void write_solutions_events(string out_path)const;//Write events
        vector<FMTspatialaction> getspatialactions()const;

        //Functions to manipulate the model

        void acceptnew();//Change new_solution to current and empty new_solution
        void testprobability(double temp);
        double cool_down(double temp)const{return cooling_schedule->reduce_temp(temp);};//Set a default cooling schedule to avoid crash
        void write_outputs_stream(const string& constraint_string, const vector<double>& outputs, const vector<double>& penalties,bool initial_solution);
        //void write_output_to_file(const string& constraint_string, const vector<double>& outputs, const vector<double>& penalties,bool initial_solution) const;
        bool evaluate();//To compare the two solutions ... if new<current true
        FMTgraphstats buildperiod();//To build initial solution
        FMTgraphstats move_solution(FMTsamovetype movetype = FMTsamovetype::shotgun);//move operator
        bool comparesolutions() const;//To verify if solutions are not identical
        bool setmapidmodified(const vector<size_t>& id);
    };
}

#endif // FMTSAMODEL_H
