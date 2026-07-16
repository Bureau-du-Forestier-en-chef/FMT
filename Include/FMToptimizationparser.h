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
	class FMTOutput;
	class FMTConstraint;
	class FMTTheme;
	class FMTConstants;
	class FMTPerBounds;
	class FMTYields;
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

	class FMTEXPORT FMTOptimizationParser : public FMTParser
	{
	public:
		FMTOptimizationParser();
		FMTOptimizationParser(const FMTOptimizationParser& rhs)=default;
		FMTOptimizationParser& operator = (const FMTOptimizationParser& rhs)=default;
		// DocString: FMTOptimizationParser::read
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
		std::vector<Core::FMTConstraint> read(const std::vector<Core::FMTTheme>& p_themes,
				  const std::vector<Core::FMTAction>& p_actions,
				  const Core::FMTConstants& p_constants,
				  const std::vector<Core::FMTOutput>& p_outputs,
			      const Core::FMTYields& p_yields,
				  std::vector<Core::FMTAction>& p_excluded,
				  const std::string& pm_location);
		// DocString: FMTOptimizationParser::getConstraints
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
		std::vector<Core::FMTConstraint> getConstraints(const std::string& pm_line,
			const Core::FMTConstants& p_constants, const Core::FMTYields& p_yields,
			const std::vector<Core::FMTOutput>& p_outputs,
			const std::vector<Core::FMTTheme>& p_themes,
			const std::vector<Core::FMTAction>& p_actions);
		// DocString: FMTOptimizationParser::write
		/*
		* @brief Write down the constraints into an optimization file.
		* @param[in] p_ constraints the constraints to write in a file.
		* @param[in] pm_location the location at which we want to write the constraints.
		*/
		void write(const std::vector<Core::FMTConstraint>& p_constraints,
			const std::string& pm_location) const;
		~FMTOptimizationParser()=default;
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
		// DocString: FMTOptimizationParser::getObjective
		/*
		* @brief Based on a line in the optimization section return an objective.
		* @param[in]  pm_line the line read.
		* @param[in]  p_constants the constants.
		* @param[in] p_yields the yields.
		* @param[in] p_outputs the model outputs.
		* @param[in] p_themes the model themes.
		* @return A valid objective.
		*/
		Core::FMTConstraint getObjective(const std::string& pm_line,
			const Core::FMTConstants& p_constants,
			const Core::FMTYields& p_yields,
			const std::vector<Core::FMTOutput>& p_outputs,
			const std::vector<Core::FMTTheme>& p_themes);
		FMToptimizationsection getSection(const std::string& line) const;
		bool setEnding(Core::FMTConstraint& constraint, std::string& line, const Core::FMTConstants& constants);
		Core::FMTOutput resumeOutput(const std::map<std::string, double>& nodes,
			const std::vector<Core::FMTOutput>& outputs,
			const std::vector<Core::FMTTheme>& themes,
			const Core::FMTConstants& constants);
		std::queue<FMTParser::FMTLineInfo>getOptline(std::ifstream& stream, const std::vector<Core::FMTTheme>& themes,
								const Core::FMTConstants& cons, const std::vector<Core::FMTOutput>& outputs);
		std::queue<std::string> getEachLines(const std::string& line, const Core::FMTConstants& constants, const std::vector<Core::FMTOutput>& outputs, const std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTOptimizationParser::getEquation
		/*
		* @brief Take the orgiginal line of the contraint and try to simplify the equation before formating it for an output
		* @param[in]  pm_line the line read by the parser.
		* @param[in]  p_constants the constants.
		* @param[in] p_yields the yields.
		* @param[in] p_outputs the outputs.
		* @param[in] p_lhssize the size of the RHS.
		* @return valid outputs nodes simplified.
		*/
		std::map<std::string, double>getEquation(const std::string& pm_line, const Core::FMTConstants& p_constants,
			const Core::FMTYields& p_yields, const std::vector<Core::FMTOutput>& p_outputs,
			size_t p_lhssize = std::numeric_limits<size_t>::max());
		std::vector<Core::FMTConstraint> getPeriodsBounds(std::string periodstr, const Core::FMTConstraint& constraint, const Core::FMTConstants& constants) const;
		void fillBounds(const std::string& operatorvalue, const double& rhs, double& lower, double& upper) const;
		// DocString: FMTOptimizationParser::getSpatialConstraint
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
		Core::FMTConstraint getSpatialConstraint(const Core::FMTConstraint& p_baseconstraint,
			const boost::smatch& p_match, const std::string& pm_line,
			const Core::FMTConstants& p_constants,
			const Core::FMTYields& p_yields,
			const std::vector<Core::FMTAction>& p_actions,
			const std::vector<Core::FMTOutput>& p_outputs,
			const std::vector<Core::FMTTheme>& p_themes);
		std::vector<std::vector<double>>getReplicateChanges(const std::string& replicateargument);
	};
}

#endif
