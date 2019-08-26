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

#ifndef FMToptimizationparser_H_INCLUDED
#define FMToptimizationparser_H_INCLUDED

#include "FMTparser.h"
#include "FMToutput.h"
#include "FMTconstraint.h"
#include <algorithm>
#include <vector>
#include <map>
#include <string>

using namespace std;
//using namespace Graph;


namespace WSParser
{
	enum FMToptimizationsection
		{
		objective = 1,
		exclude = 2,
		constraints = 3,
		format = 4,
		none = 5
		};

	class FMToptimizationparser : public FMTparser
	{
		regex rxsections;
		regex rxobjectives;
		regex rxexclude;
		regex rxconstraints;
		regex rxequations;
		regex rxgoal;
		regex rxoutput;
		regex rxpenalty;
		regex rxspecialoutput;
		std::queue<string>ineach;
		FMTconstraint getobjective(const string& line, const FMTconstants& constants,
                             const vector<FMToutput>& outputs,
                             const vector<FMTtheme>& themes);
		FMToptimizationsection getsection(const string& line) const;
		FMTconstraint getconstraint(const string& line, const FMTconstants& constants,
			const vector<FMToutput>& outputs, const vector<FMTtheme>& themes);
		bool setgoal(FMTconstraint& constraint, const string& line, const FMTconstants& constants);
		void setperiods(FMTconstraint& constraint, const string& lower,const string& upper, const FMTconstants& constants);
		FMToutput resume_output(const map<string,double>& nodes,
                          const vector<FMToutput>& outputs,
                          const vector<FMTtheme>& themes,
                          const FMTconstants& constants);
		string getoptline(ifstream& stream, const vector<FMTtheme>& themes,
			const FMTconstants& cons,const vector<FMToutput>& outputs);
		std::queue<string> geteachlines(const string& line,const vector<FMToutput>& outputs, const vector<FMTtheme>& themes) const;
		map<string,double>getequation(const string& line, const FMTconstants& constants,
                                const vector<FMToutput>& outputs,size_t lhssize=numeric_limits<size_t>::max());
	public:
		FMToptimizationparser();
		FMToptimizationparser(const FMToptimizationparser& rhs);
		FMToptimizationparser& operator = (const FMToptimizationparser& rhs);
		vector<FMTconstraint> read(const vector<FMTtheme>& themes,
				  const vector<FMTaction>& actions,
				  const FMTconstants& constants,
			      const map<string, vector<string>>& actions_aggregate,
				  const vector<FMToutput>& outputs,
				  vector<FMTaction>& excluded,
				  const string& location);
		void write(const vector<FMTconstraint>& constraints,string location);
		~FMToptimizationparser()=default;
	};
}

#endif
