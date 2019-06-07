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
        vector<FMTschedule>read(const vector<FMTtheme>& themes,const vector<FMTaction>& actions,string location);
        void write(const vector<FMTschedule>& schedules,string location);
    };

}


#endif // FMTscheduleparser_H_INCLUDED
