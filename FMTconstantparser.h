#ifndef FMTconstantparser_H_INCLUDED
#define FMTconstantparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTconstants.h"

using namespace std;
using namespace Core;

namespace WSParser
{

class FMTconstantparser : public FMTparser
    {
    public:
        FMTconstantparser();
        FMTconstantparser(const FMTconstantparser& rhs);
        FMTconstantparser& operator = (const FMTconstantparser& rhs);
        FMTconstants read(string location);
        bool write(const FMTconstants& constants,string location);
    };
}
#endif // FMTconstantparser_H_INCLUDED
