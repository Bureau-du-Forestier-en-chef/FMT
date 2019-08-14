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

#ifndef FMTactionparser_H_INCLUDED
#define FMTactionparser_H_INCLUDED
#include "FMTlist.h"
#include "FMTparser.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTyields.h"


using namespace boost::python;
using namespace std;
using namespace Core;

namespace WSParser
{

class FMTactionparser : public FMTparser
    {
    private:
        regex rxsection;
        regex rxoperator;
        vector<string>operators;
        vector<FMTaction*> sameactionas(const string& all_set,vector<FMTaction>& actions) const;
    public:
        FMTactionparser();
        FMTactionparser(const FMTactionparser& rhs);
        FMTactionparser& operator = (const FMTactionparser& rhs);
        string getbounds(string& line, FMTspec& spec,const FMTconstants& constants, const FMTyields& ylds);
        map<string,vector<string>>valagg(vector<FMTaction>& actions,map<string,vector<string>>& aggregates);
        vector<FMTaction>read(const vector<FMTtheme>& themes,
			const FMTyields& yields,const FMTconstants& constants,
			string location, map<string,vector<string>>& aggregates);
        bool write(const vector<FMTaction>& actions,
			string location,const  map<string, vector<string>>& aggregates);
    };

}

#endif // FMTactionparser_H_INCLUDED
