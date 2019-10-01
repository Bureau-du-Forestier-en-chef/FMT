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

#ifndef FMTscheduleparser_H_INCLUDED
#define FMTscheduleparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTschedule.h"


using namespace boost::python;
using namespace std;
using namespace Core;

namespace WSParser
{

class FMTscheduleparser: public FMTparser
    {
		int getvariable() const;
    public:
        FMTscheduleparser();
        FMTscheduleparser(const FMTscheduleparser& rhs);
        FMTscheduleparser& operator = (const FMTscheduleparser& rhs);
        vector<FMTschedule>read(const vector<FMTtheme>& themes,
			const vector<FMTaction>& actions,string location,double tolerance = 0.00000001);
        void write(const vector<FMTschedule>& schedules,string location);
    };

}


#endif // FMTscheduleparser_H_INCLUDED
