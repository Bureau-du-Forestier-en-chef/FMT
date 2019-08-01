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
