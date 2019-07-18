#ifndef FMTtransitionparser_H_INCLUDED
#define FMTtransitionparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMTtransitionmask.h"
#include "FMTyields.h"

namespace WSParser
{

class FMTtransitionparser : public FMTparser
    {
        regex rxsection;
        regex rxlock;
        regex rxage;
        regex rxreplace;
        regex rxtyld;
    public:
        FMTtransitionparser();
        FMTtransitionparser(const FMTtransitionparser& rhs);
        FMTtransitionparser& operator = (const FMTtransitionparser& rhs);
        FMTmask getsource(string& line, FMTspec& spec,const vector<FMTtheme>& themes,
                          FMTwssect section,const FMTconstants& constant,
                          const FMTyields& ylds);
        vector<FMTtransitionmask> getmasktran(const string& line,const vector<FMTtheme>& themes,
                                        const FMTconstants& constants, const FMTyields& ylds,
                                        const FMTmask& sourcemask, int& replaced);
        vector<FMTtransition>read(const vector<FMTtheme>& themes,
                           const vector<FMTaction>& actions,
                           const FMTyields& ylds,const FMTconstants& constants,
                           string location);
        bool write(const vector<FMTtransition>& transitions, string location);
    };
}
#endif // FMTtransitionparser_H_INCLUDED
