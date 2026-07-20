/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMToptimizationparser_Hm_included
#define FMToptimizationparser_Hm_included

#include "FMTParser.h"
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
	// DocString: FMToptimizationsection
	/**
	@brief Enumerator of the sections of an optimization file.
	*/
	enum FMToptimizationsection
		{
		objective = 1,
		exclude = 2,
		constraints = 3,
		format = 4,
		none = 5
		};

	// DocString: FMTOptimizationParser
	/**
	@brief Parser reading and writing optimization sections into constraints and objectives.
	*/
	class FMTEXPORT FMTOptimizationParser : public FMTParser
	{
	public:
		// DocString: FMTOptimizationParser()
		/**
		@brief Default constructor for FMTOptimizationParser.
		*/
		FMTOptimizationParser();
		// DocString: FMTOptimizationParser(const FMTOptimizationParser&)
		/**
		@brief Copy constructor for FMTOptimizationParser.
		@param[in] rhs the FMTOptimizationParser to copy.
		*/
		FMTOptimizationParser(const FMTOptimizationParser& rhs)=default;
		// DocString: FMTOptimizationParser::operator=
		/**
		@brief Copy assignment operator for FMTOptimizationParser.
		@param[in] rhs the FMTOptimizationParser to copy.
		@return a reference to this FMTOptimizationParser.
		*/
		FMTOptimizationParser& operator = (const FMTOptimizationParser& rhs)=default;
		// DocString: FMTOptimizationParser::read
		/**
		@brief Read an optimization file.
		@param[in] p_themes the themes.
		@param[in] p_actions the actions.
		@param[in] p_constants the constants.
		@param[in] p_outputs the outputs.
		@param[in] p_yields the yields.
		@param[out] p_excluded the actions to exclude.
		@param[in] pm_location the file location.
		@return the constraints.
		*/
		std::vector<Core::FMTConstraint> read(const std::vector<Core::FMTTheme>& p_themes,
				  const std::vector<Core::FMTAction>& p_actions,
				  const Core::FMTConstants& p_constants,
				  const std::vector<Core::FMTOutput>& p_outputs,
			      const Core::FMTYields& p_yields,
				  std::vector<Core::FMTAction>& p_excluded,
				  const std::string& pm_location);
		// DocString: FMTOptimizationParser::getConstraints
		/**
		@brief Read a line and get the constraints.
		@param[in] pm_line the line.
		@param[in] p_constants the constants.
		@param[in] p_yields the yields.
		@param[in] p_outputs the outputs.
		@param[in] p_themes the themes.
		@param[in] p_actions the actions.
		@return the constraints.
		*/
		std::vector<Core::FMTConstraint> getConstraints(const std::string& pm_line,
			const Core::FMTConstants& p_constants, const Core::FMTYields& p_yields,
			const std::vector<Core::FMTOutput>& p_outputs,
			const std::vector<Core::FMTTheme>& p_themes,
			const std::vector<Core::FMTAction>& p_actions);
		// DocString: FMTOptimizationParser::write
		/**
		@brief Write the constraints into an optimization file.
		@param[in] p_constraints the constraints to write.
		@param[in] pm_location the file location.
		*/
		void write(const std::vector<Core::FMTConstraint>& p_constraints,
			const std::string& pm_location) const;
		// DocString: ~FMTOptimizationParser()
		/**
		@brief Default destructor for FMTOptimizationParser.
		*/
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
		/**
		@brief Return an objective from a line of the optimization section.
		@param[in] pm_line the line.
		@param[in] p_constants the constants.
		@param[in] p_yields the yields.
		@param[in] p_outputs the outputs.
		@param[in] p_themes the themes.
		@return the objective.
		*/
		Core::FMTConstraint getObjective(const std::string& pm_line,
			const Core::FMTConstants& p_constants,
			const Core::FMTYields& p_yields,
			const std::vector<Core::FMTOutput>& p_outputs,
			const std::vector<Core::FMTTheme>& p_themes);
		// DocString: FMTOptimizationParser::getSection
		/**
		@brief Return the optimization section of a line.
		@param[in] line the line.
		@return the optimization section.
		*/
		FMToptimizationsection getSection(const std::string& line) const;
		// DocString: FMTOptimizationParser::setEnding
		/**
		@brief Set the ending bounds of a constraint from a line.
		@param[in,out] constraint the constraint.
		@param[in,out] line the line.
		@param[in] constants the constants.
		@return true if the ending is set else false.
		*/
		bool setEnding(Core::FMTConstraint& constraint, std::string& line, const Core::FMTConstants& constants);
		// DocString: FMTOptimizationParser::resumeOutput
		/**
		@brief Build an output from a map of nodes.
		@param[in] nodes the nodes.
		@param[in] outputs the outputs.
		@param[in] themes the themes.
		@param[in] constants the constants.
		@return the resumed output.
		*/
		Core::FMTOutput resumeOutput(const std::map<std::string, double>& nodes,
			const std::vector<Core::FMTOutput>& outputs,
			const std::vector<Core::FMTTheme>& themes,
			const Core::FMTConstants& constants);
		// DocString: FMTOptimizationParser::getOptline
		/**
		@brief Get the optimization lines from a stream, considering for loops and outputs.
		@param[in,out] stream the file stream.
		@param[in] themes the themes.
		@param[in] cons the constants.
		@param[in] outputs the outputs.
		@return a queue of lines to process.
		*/
		std::queue<FMTParser::FMTLineInfo>getOptline(std::ifstream& stream, const std::vector<Core::FMTTheme>& themes,
								const Core::FMTConstants& cons, const std::vector<Core::FMTOutput>& outputs);
		// DocString: FMTOptimizationParser::getEachLines
		/**
		@brief Unroll the _EACH lines of a constraint line.
		@param[in] line the line.
		@param[in] constants the constants.
		@param[in] outputs the outputs.
		@param[in] themes the themes.
		@return the resulting lines.
		*/
		std::queue<std::string> getEachLines(const std::string& line, const Core::FMTConstants& constants, const std::vector<Core::FMTOutput>& outputs, const std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTOptimizationParser::getEquation
		/**
		@brief Simplify the equation of a constraint line before formatting it for an output.
		@param[in] pm_line the line.
		@param[in] p_constants the constants.
		@param[in] p_yields the yields.
		@param[in] p_outputs the outputs.
		@param[in] p_lhssize the size of the RHS.
		@return the simplified output nodes.
		*/
		std::map<std::string, double>getEquation(const std::string& pm_line, const Core::FMTConstants& p_constants,
			const Core::FMTYields& p_yields, const std::vector<Core::FMTOutput>& p_outputs,
			size_t p_lhssize = std::numeric_limits<size_t>::max());
		// DocString: FMTOptimizationParser::getPeriodsBounds
		/**
		@brief Return the constraints for each period bound of a constraint.
		@param[in] periodstr the period string.
		@param[in] constraint the base constraint.
		@param[in] constants the constants.
		@return the period constraints.
		*/
		std::vector<Core::FMTConstraint> getPeriodsBounds(std::string periodstr, const Core::FMTConstraint& constraint, const Core::FMTConstants& constants) const;
		// DocString: FMTOptimizationParser::fillBounds
		/**
		@brief Fill up the lower and upper bounds from an operator and a RHS.
		@param[in] operatorvalue the operator.
		@param[in] rhs the RHS.
		@param[out] lower the lower bound.
		@param[out] upper the upper bound.
		*/
		void fillBounds(const std::string& operatorvalue, const double& rhs, double& lower, double& upper) const;
		// DocString: FMTOptimizationParser::getSpatialConstraint
		/**
		@brief Turn a base constraint into a spatial constraint from a line.
		@param[in] p_baseconstraint the base constraint.
		@param[in] p_match the regex match.
		@param[in] pm_line the line.
		@param[in] p_constants the constants.
		@param[in] p_yields the yields.
		@param[in] p_actions the actions.
		@param[in] p_outputs the outputs.
		@param[in] p_themes the themes.
		@return the spatial constraint.
		*/
		Core::FMTConstraint getSpatialConstraint(const Core::FMTConstraint& p_baseconstraint,
			const boost::smatch& p_match, const std::string& pm_line,
			const Core::FMTConstants& p_constants,
			const Core::FMTYields& p_yields,
			const std::vector<Core::FMTAction>& p_actions,
			const std::vector<Core::FMTOutput>& p_outputs,
			const std::vector<Core::FMTTheme>& p_themes);
		// DocString: FMTOptimizationParser::getReplicateChanges
		/**
		@brief Return the replicate bound changes from a replicate argument.
		@param[in] replicateargument the replicate argument.
		@return the replicate changes.
		*/
		std::vector<std::vector<double>>getReplicateChanges(const std::string& replicateargument);
	};
}

#endif
