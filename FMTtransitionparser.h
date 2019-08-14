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
