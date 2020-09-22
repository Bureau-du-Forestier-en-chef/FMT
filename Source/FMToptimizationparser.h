/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
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
#include <boost/regex.hpp>
#include <limits>

namespace Parser
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
		boost::regex rxequations;
		std::regex rxperiods;
		std::regex rxgoal;
		std::regex rxoutput;
		std::regex rxpenalty;
		std::regex rxspecialoutput;
		std::queue<std::string>ineach;
		Core::FMTconstraint getobjective(const std::string& line, const Core::FMTconstants& constants,
                             const std::vector<Core::FMToutput>& outputs,
                             const std::vector<Core::FMTtheme>& themes);
		FMToptimizationsection getsection(const std::string& line) const;
		std::vector<Core::FMTconstraint> getconstraints(const std::string& line, const Core::FMTconstants& constants,
			const std::vector<Core::FMToutput>& outputs, const std::vector<Core::FMTtheme>& themes);
		bool setgoal(Core::FMTconstraint& constraint, const std::string& line, const Core::FMTconstants& constants);
		void setperiods(Core::FMTconstraint& constraint, const std::string& lower,const std::string& upper, const Core::FMTconstants& constants) const;
		Core::FMToutput resume_output(const std::map<std::string,double>& nodes,
                          const std::vector<Core::FMToutput>& outputs,
                          const std::vector<Core::FMTtheme>& themes,
                          const Core::FMTconstants& constants);
		std::string getoptline(std::ifstream& stream, const std::vector<Core::FMTtheme>& themes,
			const Core::FMTconstants& cons,const std::vector<Core::FMToutput>& outputs);
		std::queue<std::string> geteachlines(const std::string& line,const Core::FMTconstants& constants,const std::vector<Core::FMToutput>& outputs, const std::vector<Core::FMTtheme>& themes) const;
		std::map<std::string,double>getequation(const std::string& line, const Core::FMTconstants& constants,
                                const std::vector<Core::FMToutput>& outputs,size_t lhssize= std::numeric_limits<size_t>::max());
		std::vector<Core::FMTconstraint> getperiodsbounds(std::string periodstr,const Core::FMTconstraint& constraint, const Core::FMTconstants& constants) const;
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
		void write(const std::vector<Core::FMTconstraint>& constraints,const std::string& location) const;
		~FMToptimizationparser()=default;
	};
}

#endif
