/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMToptimizationparser_Hm_included
#define FMToptimizationparser_Hm_included

#include "FMTparser.h"
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include "FMTutility.h"
#include <boost/regex.hpp>
#include <limits>


namespace Core
{
	class FMToutput;
	class FMTconstraint;
	class FMTtheme;
	class FMTconstants;
	class FMTperbounds;
	class FMTyields;
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
	public:
		FMToptimizationparser();
		FMToptimizationparser(const FMToptimizationparser& rhs)=default;
		FMToptimizationparser& operator = (const FMToptimizationparser& rhs)=default;
		// DocString: FMToptimizationparser::read
		/*
		* @brief Read a optimization file from a given pm_location
		* @param[in]  p_themes the themes used.
		* @param[in]  p_actions the actions used.
		* @param[in]  p_constants the constants.
		* @param[in]  p_outputs the outputs.
		* @param[in]  p_yields the yields.
		* @param[out] p_excluded the actions to exclude.
		* @param[in] pm_location the location of the file.
		* @return Valid constraints.
		*/
		std::vector<Core::FMTconstraint> read(const std::vector<Core::FMTtheme>& p_themes,
				  const std::vector<Core::FMTaction>& p_actions,
				  const Core::FMTconstants& p_constants,
				  const std::vector<Core::FMToutput>& p_outputs,
			      const Core::FMTyields& p_yields,
				  std::vector<Core::FMTaction>& p_excluded,
				  const std::string& pm_location);
		// DocString: FMToptimizationparser::getConstraints
		/*
		* @brief Read a line and get a constraint
		* @param[in] pm_line the line read.
		* @param[in] p_constants the constants.
		* @param[in] p_yields the yields.
		* @param[in] p_outputs the model outputs.
		* @param[in] p_themes the model themes.
		* @param[in] p_actions the model actions.
		* @return A valid constraint.
		*/
		std::vector<Core::FMTconstraint> getConstraints(const std::string& pm_line,
			const Core::FMTconstants& p_constants, const Core::FMTyields& p_yields,
			const std::vector<Core::FMToutput>& p_outputs,
			const std::vector<Core::FMTtheme>& p_themes,
			const std::vector<Core::FMTaction>& p_actions);
		// DocString: FMToptimizationparser::write
		/*
		* @brief Write down the constraints into an optimization file.
		* @param[in] p_ constraints the constraints to write in a file.
		* @param[in] pm_location the location at which we want to write the constraints.
		*/
		void write(const std::vector<Core::FMTconstraint>& p_constraints,
			const std::string& pm_location) const;
		~FMToptimizationparser()=default;
	private:
		const static boost::regex m_rxsections;
		const static boost::regex m_rxobjectives;
		const static boost::regex m_rxexclude;
		const static boost::regex m_rxconstraints;
		const static boost::regex m_rxequations;
		const static boost::regex m_m_ENDing;
		const static boost::regex m_rxoutput;
		const static boost::regex m_rxpenalty;
		const static boost::regex m_rxspecialoutput;
		const static boost::regex m_rxspatial;
		const static boost::regex m_rxspecialobjective;
		const static boost::regex m_rxstartwithoperator;
		//std::queue<std::string>m_ineach;
		// DocString: FMToptimizationparser::getObjective
		/*
		* @brief Based on a line in the optimization section return an objective.
		* @param[in]  pm_line the line read.
		* @param[in]  p_constants the constants.
		* @param[in] p_yields the yields.
		* @param[in] p_outputs the model outputs.
		* @param[in] p_themes the model themes.
		* @return A valid objective.
		*/
		Core::FMTconstraint getObjective(const std::string& pm_line,
			const Core::FMTconstants& p_constants,
			const Core::FMTyields& p_yields,
			const std::vector<Core::FMToutput>& p_outputs,
			const std::vector<Core::FMTtheme>& p_themes);
		FMToptimizationsection getsection(const std::string& line) const;
		bool setending(Core::FMTconstraint& constraint, std::string& line, const Core::FMTconstants& constants);
		Core::FMToutput resume_output(const std::map<std::string, double>& nodes,
			const std::vector<Core::FMToutput>& outputs,
			const std::vector<Core::FMTtheme>& themes,
			const Core::FMTconstants& constants);
		std::queue<std::pair<std::string, int>>getoptline(std::ifstream& stream, const std::vector<Core::FMTtheme>& themes,
								const Core::FMTconstants& cons, const std::vector<Core::FMToutput>& outputs);
		std::queue<std::string> geteachlines(const std::string& line, const Core::FMTconstants& constants, const std::vector<Core::FMToutput>& outputs, const std::vector<Core::FMTtheme>& themes) const;
		// DocString: FMToptimizationparser::getEquation
		/*
		* @brief Take the orgiginal line of the contraint and try to simplify the equation before formating it for an output
		* @param[in]  pm_line the line read by the parser.
		* @param[in]  p_constants the constants.
		* @param[in] p_yields the yields.
		* @param[in] p_outputs the outputs.
		* @param[in] p_lhssize the size of the RHS.
		* @return valid outputs nodes simplified.
		*/
		std::map<std::string, double>getEquation(const std::string& pm_line, const Core::FMTconstants& p_constants,
			const Core::FMTyields& p_yields, const std::vector<Core::FMToutput>& p_outputs,
			size_t p_lhssize = std::numeric_limits<size_t>::max());
		std::vector<Core::FMTconstraint> getperiodsbounds(std::string periodstr, const Core::FMTconstraint& constraint, const Core::FMTconstants& constants) const;
		void fillbounds(const std::string& operatorvalue, const double& rhs, double& lower, double& upper) const;
		// DocString: FMToptimizationparser::getSpatialConstraint
		/*
		* @brief From a base contraint turn it into spatial constraint based on the pm_line.
		* @param[in]  p_baseconstraint the base constraint.
		* @param[in]  p_match the regex match
		* @param[in]  pm_line the line read by the parser.
		* @param[in]  p_constants the constants.
		* @param[in]  p_yields the yields.
		* @param[in]  p_actions the actions.
		* @param[in]  p_outputs the outputs.
		* @param[in]  p_themes the themes.
		* @return valid spatial constraint.
		*/
		Core::FMTconstraint getSpatialConstraint(const Core::FMTconstraint& p_baseconstraint,
			const boost::smatch& p_match, const std::string& pm_line,
			const Core::FMTconstants& p_constants,
			const Core::FMTyields& p_yields,
			const std::vector<Core::FMTaction>& p_actions,
			const std::vector<Core::FMToutput>& p_outputs,
			const std::vector<Core::FMTtheme>& p_themes);
		std::vector<std::vector<double>>getreplicatechanges(const std::string& replicateargument);
	};
}

#endif
