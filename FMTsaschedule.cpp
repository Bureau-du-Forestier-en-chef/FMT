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
