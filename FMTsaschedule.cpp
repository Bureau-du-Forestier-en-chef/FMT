#include "FMTsaschedule.h"

#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace Spatial
{


//FMTsaschedule base class


    FMTsaschedule::FMTsaschedule():schedule_type()
    {

    }

    double FMTsaschedule::reduce_temp(double& temp)
    {
        return 0.0;
    }

    string FMTsaschedule::get_schedule_type()const
    {
        return schedule_type;
    }

    unique_ptr<FMTsaschedule> FMTsaschedule::Clone()const
    {
        return unique_ptr<FMTsaschedule>(new FMTsaschedule(*this));
    }


//FMTlinearschedule


    FMTlinearschedule::FMTlinearschedule(double delta): FMTsaschedule(),decrement_model(delta),schedule_type("linear")
    {
        //Constructor
    }

    double FMTlinearschedule::reduce_temp(double& temp)
    {
        return temp-decrement_model;
    }

    string FMTlinearschedule::get_schedule_type()const
    {
        return schedule_type;
    }

    unique_ptr<FMTsaschedule> FMTlinearschedule::Clone()const
    {
        return unique_ptr<FMTsaschedule>(new FMTlinearschedule(*this));
    }

}
