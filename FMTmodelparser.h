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

#ifndef FMTmodelparser_H_INCLUDED
#define FMTmodelparser_H_INCLUDED

//#include "FMTwsParser.h"
#include "FMTparser.h"
#include "FMTconstantparser.h"
#include "FMTlandscapeparser.h"
#include "FMTareaparser.h"
#include "FMTyieldparser.h"
#include "FMTactionparser.h"
#include "FMTtransitionparser.h"
#include "FMTlifespanparser.h"
#include "FMToutputparser.h"
#include "FMToptimizationparser.h"
#include "FMTscheduleparser.h"


#include "FMTmodel.h"
#include "boost/filesystem.hpp"
#include <map>


using namespace std;
using namespace Core;
using namespace Models;

namespace WSParser
{

class FMTmodelparser : public FMTparser
    {
	FMTmodel referenceread(map<string, vector<int>>& common_sections,
					vector<FMTmodel>& models,
					const string& con, const string& lan,
					const string& lif, const string& are, const string& yld,
					const string& act, const string& tr, const string& out,
					string opt = string(),bool allow_mapping=false);
    public:
        FMTmodelparser();
        FMTmodelparser(const FMTmodelparser& rhs);
        FMTmodelparser& operator = (const FMTmodelparser& rhs);
        FMTmodel read(const string& con,const string& lan,
                      const string& lif,const string& are,const string& yld,
                      const string& act,const string& tr,const string& out,
						string opt = string());
		vector<FMTmodel>readproject(const string& primary_location,
			vector<string>scenarios = vector<string>(),
			bool readarea = true,bool readoutputs = true, bool readoptimize = true);
		vector<vector<FMTschedule>>readschedules(const string& primary_location,
			const vector<FMTmodel>& models);
        //void settransitionsNactions(vector<FMTaction>& actions,vector<FMTtransition>& Transitions);
        bool write(const FMTmodel& model,const string& folder);
    };
}

#endif // FMTmodelparser_H_INCLUDED
