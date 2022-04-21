/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMToptimizationparser_H_INCLUDED
#define FMToptimizationparser_H_INCLUDED

#include "FMTparser.hpp"
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include "FMTutility.hpp"
#include <regex>
#include <boost/regex.hpp>
#include <limits>

namespace Core
{
	class FMToutput;
	class FMTconstraint;
	class FMTtheme;
	class FMTconstants;
	class FMTperbounds;
}


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

	class FMTEXPORT FMToptimizationparser : public FMTparser
	{
		const static std::regex rxsections;
		const static std::regex rxobjectives;
		const static std::regex rxexclude;
		const static std::regex rxconstraints;
		const static boost::regex rxequations;
		const static std::regex rxperiods;
		const static std::regex rxending;
		const static std::regex rxoutput;
		const static std::regex rxpenalty;
		const static std::regex rxspecialoutput;
		const static std::regex rxspatial;
		const static std::regex rxspecialobjective;
		std::queue<std::string>ineach;
		Core::FMTconstraint getobjective(const std::string& line, const Core::FMTconstants& constants,
                             const std::vector<Core::FMToutput>& outputs,
                             const std::vector<Core::FMTtheme>& themes);
		FMToptimizationsection getsection(const std::string& line) const;
		bool setending(Core::FMTconstraint& constraint,std::string& line, const Core::FMTconstants& constants);
		Core::FMTperbounds getperbound(const std::string& lower, const std::string& upper, const Core::FMTconstants& constants) const;
		void setperiods(Core::FMTspec& constraint, const std::string& lower,const std::string& upper, const Core::FMTconstants& constants) const;
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
		void fillbounds(const std::string& operatorvalue, const double& rhs, double& lower,double& upper) const;
		Core::FMTconstraint getspatialconstraint(const Core::FMTconstraint& baseconstraint,
			const std::smatch& match, const std::string& line,
			const Core::FMTconstants& constants,
			const std::vector<Core::FMTaction>& actions,
			const std::vector<Core::FMToutput>& outputs,
			const std::vector<Core::FMTtheme>& themes);
		std::vector<std::vector<double>>getreplicatechanges(const std::string& replicateargument);
	public:
		FMToptimizationparser();
		FMToptimizationparser(const FMToptimizationparser& rhs)=default;
		FMToptimizationparser& operator = (const FMToptimizationparser& rhs)=default;
		std::vector<Core::FMTconstraint> read(const std::vector<Core::FMTtheme>& themes,
				  const std::vector<Core::FMTaction>& actions,
				  const Core::FMTconstants& constants,
				  const std::vector<Core::FMToutput>& outputs,
				  std::vector<Core::FMTaction>& excluded,
				  const std::string& location);
		std::vector<Core::FMTconstraint> getconstraints(const std::string& line, const Core::FMTconstants& constants,
			const std::vector<Core::FMToutput>& outputs, const std::vector<Core::FMTtheme>& themes, const std::vector<Core::FMTaction>& actions);
		void write(const std::vector<Core::FMTconstraint>& constraints,const std::string& location) const;
		~FMToptimizationparser()=default;
	};
}

#endif
