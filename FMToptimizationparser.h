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
#include <queue>
#include "FMTutility.h"
#include <regex>
#include "FMTconstants.h"
#include "FMTtheme.h"
#include <limits>

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
		std::regex rxsections;
		std::regex rxobjectives;
		std::regex rxexclude;
		std::regex rxconstraints;
		std::regex rxequations;
		std::regex rxgoal;
		std::regex rxoutput;
		std::regex rxpenalty;
		std::regex rxspecialoutput;
		std::queue<std::string>ineach;
		Core::FMTconstraint getobjective(const std::string& line, const Core::FMTconstants& constants,
                             const std::vector<Core::FMToutput>& outputs,
                             const std::vector<Core::FMTtheme>& themes);
		FMToptimizationsection getsection(const std::string& line) const;
		Core::FMTconstraint getconstraint(const std::string& line, const Core::FMTconstants& constants,
			const std::vector<Core::FMToutput>& outputs, const std::vector<Core::FMTtheme>& themes);
		bool setgoal(Core::FMTconstraint& constraint, const std::string& line, const Core::FMTconstants& constants);
		void setperiods(Core::FMTconstraint& constraint, const std::string& lower,const std::string& upper, const Core::FMTconstants& constants);
		Core::FMToutput resume_output(const std::map<std::string,double>& nodes,
                          const std::vector<Core::FMToutput>& outputs,
                          const std::vector<Core::FMTtheme>& themes,
                          const Core::FMTconstants& constants);
		std::string getoptline(std::ifstream& stream, const std::vector<Core::FMTtheme>& themes,
			const Core::FMTconstants& cons,const std::vector<Core::FMToutput>& outputs);
		std::queue<std::string> geteachlines(const std::string& line,const Core::FMTconstants& constants,const std::vector<Core::FMToutput>& outputs, const std::vector<Core::FMTtheme>& themes) const;
		std::map<std::string,double>getequation(const std::string& line, const Core::FMTconstants& constants,
                                const std::vector<Core::FMToutput>& outputs,size_t lhssize= std::numeric_limits<size_t>::max());
	public:
		FMToptimizationparser();
		FMToptimizationparser(const FMToptimizationparser& rhs);
		FMToptimizationparser& operator = (const FMToptimizationparser& rhs);
		std::vector<Core::FMTconstraint> read(const std::vector<Core::FMTtheme>& themes,
				  const std::vector<Core::FMTaction>& actions,
				  const Core::FMTconstants& constants,
				  const std::vector<Core::FMToutput>& outputs,
				  std::vector<Core::FMTaction>& excluded,
				  const std::string& location);
		void write(const std::vector<Core::FMTconstraint>& constraints, std::string location);
		~FMToptimizationparser()=default;
	};
}

#endif
