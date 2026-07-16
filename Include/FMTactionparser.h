/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTactionparser_Hm_included
#define FMTactionparser_Hm_included
#include "FMTparser.h"
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
FMTActionParser reads and writes actions from or into an action files.
This parser is also used by the modelparser.
*/
class FMTEXPORT FMTActionParser : public FMTParser
    {
    private:
		// DocString: FMTActionParser::rxsection
		///This the main regex used to catch all the keywords of the action section.
        const static boost::regex rxsection;
		// DocString: FMTActionParser::rxoperator
		///This regex catches the operators used in the action section.
		const static boost::regex rxoperator;
		// DocString: FMTActionParser::m_SERIES_MATCH
		///This regex catchesthe series.
		const static boost::regex m_SERIES_MATCH;
		// DocString: FMTActionParser::_getSerie
		/**
		@brief get a serie from a line
		@param[in] p_line parsed line
		@param[in] p_actions the actions generated
		@return a valid serie
		*/
		Core::FMTSerie _getSerie(const std::string& p_line,
			const std::vector<Core::FMTAction>& p_actions) const;
		// DocString: FMTActionParser::sameActionAs
		/**
		Using a string containing potential action names (all_set) and a vector of (actions) this function
		returns a vector of pointer to the corresponding action present in the all_set string.
		*/
		std::vector<Core::FMTAction*> sameActionAs(const std::string& all_set, std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTActionParser::getBounds
		/**
		This function fill up some specification (spec) usiing a string line of the area section.
		*/
		std::string getBounds(std::string& line, Core::FMTSpec& spec, const Core::FMTConstants& constants, const Core::FMTYields& ylds);
		// DocString: FMTActionParser::valAgg
		/**
		This functions turns aggregates of aggregates into simple aggregates of action map.
		*/
		std::map<std::string, std::vector<std::string>>valAgg(std::vector<Core::FMTAction>& actions, std::map<std::string, std::vector<std::string>>& aggregates);
	public:
		// DocString: FMTActionParser()
		/**
		Default constructor for FMTActionParser
		*/
        FMTActionParser();
		// DocString: ~FMTActionParser()
		/**
		Default destructor for FMTActionParser
		*/
		~FMTActionParser() = default;
		// DocString: FMTActionParser(const FMTActionParser&)
		/**
		Default copy constructor for FMTActionParser
		*/
        FMTActionParser(const FMTActionParser& rhs)=default;
		// DocString: FMTActionParser::operator
		/**
		Default copy assignment operator for FMTActionParser
		*/
        FMTActionParser& operator = (const FMTActionParser& rhs)=default;
		// DocString: FMTActionParser::read
		/**
		The read function will read actions from a (location) action file using a complete vector of (themes),
		a (yields) section, some and some (constants) and will returns a vector of FMTactions.
		*/
		std::vector<Core::FMTAction>read(const std::vector<Core::FMTTheme>& themes,
			const Core::FMTYields& yields,const Core::FMTConstants& constants,
			const std::string& location);
		// DocString: FMTActionParser::write
		/**
		Using the (location) of the action section to fill this function will write a vector of (actions)
		to the given location.
		*/
        void write(const std::vector<Core::FMTAction>& actions,
			const std::string& location,bool withgcbmagg = false) const;
		// DocString: FMTActionParser::getactionsidsofmodelyields
		/**
		Returns the corresponding actions ids of the vector of actions read from the .json file in YieldPredModels.
		*/
		std::vector<Core::FMTAction>getGCBMactionsaggregate(const std::vector<Core::FMTAction>& actions) const;
    };

}

#endif // FMTactionparser_Hm_included
