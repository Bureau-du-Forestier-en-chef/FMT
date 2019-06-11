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
class FMTsamodel : public FMTmodel
    {
    protected:
        default_random_engine generator;
        vector<FMTspatialaction> spactions;//should be FMTmodel action pointer...
        unique_ptr<FMTsaschedule> cooling_schedule;
        FMTsasolution current_solution;
        FMTsasolution new_solution;
        FMTsasolution best_solution;
        //estimateK (probability of acceptance)
        vector<size_t> mapidmodified;//Id in the map that are different between current and new solution

    public:
        FMTsamodel();//Constructor
        ~FMTsamodel()=default;
        FMTsamodel(const FMTsamodel& rhs);//Copy constructor
        FMTsamodel(const FMTmodel& rhs);//Copy constructor to use parent as argument in constructor
        FMTsamodel& operator = (const FMTsamodel& rhs);//Copy assignment operator

        //Setting parameters for the model

        bool setschedule(const FMTlinearschedule& schedule);//To set a schedule for the simulated annealing
        //bool setschedule(FMTexponentialschedule schedule) const;// need to be created
        bool setinitial_mapping(const FMTforest& forest);
        bool setspactions(const vector<FMTspatialaction>& lspactions);//Set spatial action see FMTspatialaction for info

        //Get informations

        FMTsasolution get_current_solution()const;
        FMTsasolution get_new_solution()const;
        string getcoolingscheduletype()const{return cooling_schedule->get_schedule_type();};

        //Functions to manipulate the model

        double cool_down(double temp)const{return cooling_schedule->reduce_temp(temp);};//Set a default cooling schedule to avoid crash
        double evaluate(double temp);
        FMTgraphstats buildperiod();
        FMTgraphstats move_solution(FMTsamovetype movetype = FMTsamovetype::shotgun);
        bool comparesolutions() const;
        bool setmapidmodified(const vector<size_t>& id);
    };
}

#endif // FMTSAMODEL_H
