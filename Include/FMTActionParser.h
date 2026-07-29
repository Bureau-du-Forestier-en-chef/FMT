/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTactionparser_Hm_included
#define FMTactionparser_Hm_included
#include "FMTParser.h"
#include "FMTutility.h"
#include <boost/regex.hpp>
#include <vector>
#include <map>

namespace Core 
{
	class FMTConstants;
	class FMTTheme;
	class FMTAction;
	class FMTYields;
	class FMTSerie;
}


namespace Parser
{
// DocString: FMTActionParser
/**
@brief Parser reading and writing actions from or into action files, also used by the model parser.
*/
class FMTEXPORT FMTActionParser : public FMTParser
    {
	public:
		// DocString: FMTActionParser()
		/**
		@brief Default constructor for FMTActionParser.
		*/
        FMTActionParser();
		// DocString: ~FMTActionParser()
		/**
		@brief Default destructor for FMTActionParser.
		*/
		~FMTActionParser() = default;
		// DocString: FMTActionParser(const FMTActionParser&)
		/**
		@brief Copy constructor for FMTActionParser.
		@param[in] rhs the FMTActionParser to copy.
		*/
        FMTActionParser(const FMTActionParser& rhs)=default;
		// DocString: FMTActionParser::operator
		/**
		@brief Copy assignment operator for FMTActionParser.
		@param[in] rhs the FMTActionParser to copy.
		@return a reference to this FMTActionParser.
		*/
        FMTActionParser& operator = (const FMTActionParser& rhs)=default;
		// DocString: FMTActionParser::read
		/**
		@brief Read actions from an action file.
		@param[in] themes the themes.
		@param[in] yields the yields.
		@param[in] constants the constants.
		@param[in] location the file location.
		@return the actions.
		*/
		std::vector<Core::FMTAction>read(const std::vector<Core::FMTTheme>& themes,
			const Core::FMTYields& yields,const Core::FMTConstants& constants,
			const std::string& location);
		// DocString: FMTActionParser::write
		/**
		@brief Write actions to an action file.
		@param[in] actions the actions.
		@param[in] location the file location.
		@param[in] withgcbmagg if true writes the GCBM aggregates.
		*/
        void write(const std::vector<Core::FMTAction>& actions,
			const std::string& location,bool withgcbmagg = false) const;
		// DocString: FMTActionParser::getactionsidsofmodelyields
		/**
		@brief Return the GCBM action aggregates corresponding to the actions.
		@param[in] actions the actions.
		@return the GCBM action aggregates.
		*/
		std::vector<Core::FMTAction>getGCBMactionsaggregate(const std::vector<Core::FMTAction>& actions) const;
    private:
		// DocString: FMTActionParser::m_rxsection
		///This the main regex used to catch all the keywords of the action section.
        const static boost::regex m_rxsection;
		// DocString: FMTActionParser::m_rxoperator
		///This regex catches the operators used in the action section.
		const static boost::regex m_rxoperator;
		// DocString: FMTActionParser::m_SERIES_MATCH
		///This regex catchesthe series.
		const static boost::regex m_SERIES_MATCH;
		// DocString: FMTActionParser::_getSerie
		/**
		@brief Get a serie from a line.
		@param[in] p_line the parsed line.
		@param[in] p_actions the generated actions.
		@return the serie.
		*/
		Core::FMTSerie _getSerie(const std::string& p_line,
			const std::vector<Core::FMTAction>& p_actions) const;
		// DocString: FMTActionParser::_sameActionAs
		/**
		@brief Return pointers to the actions present in a set string.
		@param[in] all_set the set string of action names.
		@param[in,out] actions the actions.
		@return the matching actions.
		*/
		std::vector<Core::FMTAction*> _sameActionAs(const std::string& all_set, std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTActionParser::_getBounds
		/**
		@brief Fill up a specification from a line of the action section.
		@param[in,out] line the line.
		@param[in,out] spec the specification.
		@param[in] constants the constants.
		@param[in] ylds the yields.
		@return the rest of the line.
		*/
		std::string _getBounds(std::string& line, Core::FMTSpec& spec, const Core::FMTConstants& constants, const Core::FMTYields& ylds);
		// DocString: FMTActionParser::_valAgg
		/**
		@brief Turn aggregates of aggregates into simple aggregates of actions.
		@param[in,out] actions the actions.
		@param[in,out] aggregates the aggregates.
		@return the simplified aggregates.
		*/
		std::map<std::string, std::vector<std::string>>_valAgg(std::vector<Core::FMTAction>& actions, std::map<std::string, std::vector<std::string>>& aggregates);
    };

}

#endif // FMTactionparser_Hm_included
