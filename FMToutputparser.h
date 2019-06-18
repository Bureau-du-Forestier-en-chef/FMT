#ifndef FMToutputparser_H_INCLUDED
#define FMToutputparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTconstants.h"
#include "FMToutput.h"

namespace WSParser

{

class FMToutputparser : public FMTparser
    {
    //regex rxoperators;
    regex rxoutput;
    regex rxsource;
    regex rxtar;
    regex rxgrp;
    public:
        FMToutputparser();
        FMToutputparser(const FMToutputparser& rhs);
        FMToutputparser& operator = (const FMToutputparser& rhs);
        vector<FMToutput> read(const vector<FMTtheme>& themes,
                            const vector<FMTaction>& actions,
                            const FMTyields& ylds,const FMTconstants& constants,
							const map<string,vector<string>>& actions_aggregate,
                            string location);
        bool write(const vector<FMToutput>& outputs,string location);
    };

}


#endif // FMToutputparser_H_INCLUDED
