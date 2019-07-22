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


//FMTexponentialschedule


    FMTexponentialschedule::FMTexponentialschedule(double lalpha): FMTsaschedule(),alpha(lalpha),schedule_type("exponential")
    {
        //Constructor
    }

    double FMTexponentialschedule::reduce_temp(double& temp)
    {
        return temp*alpha;
    }

    string FMTexponentialschedule::get_schedule_type()const
    {
        return schedule_type;
    }

    unique_ptr<FMTsaschedule> FMTexponentialschedule::Clone()const
    {
        return unique_ptr<FMTsaschedule>(new FMTexponentialschedule(*this));
    }

}
