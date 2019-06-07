#ifndef FMTlifespanparser_H_INCLUDED
#define FMTlifespanparser_H_INCLUDED

#include "FMTlist.h"
#include "FMTparser.h"
#include "FMTlifespans.h"
#include "FMTtheme.h"
#include "FMTconstants.h"

namespace WSParser

{
class FMTlifespanparser : public FMTparser
    {
    public:
        FMTlifespanparser();
        FMTlifespanparser(const FMTlifespanparser& rhs);
        FMTlifespanparser& operator = (const FMTlifespanparser& rhs);
        FMTlifespans read(const vector<FMTtheme>& themes,const FMTconstants& constants,string location);
        bool write(const FMTlifespans& lifespan,string location);
    };

}


#endif // FMTlifespanparser_H_INCLUDED
