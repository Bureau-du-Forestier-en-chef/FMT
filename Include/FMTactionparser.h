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
// DocString: FMTactionparser
/**
FMTactionparser reads and writes actions from or into an action files.
This parser is also used by the modelparser.
*/
class FMTEXPORT FMTactionparser : public FMTparser
    {
    private:
		// DocString: FMTactionparser::rxsection
		///This the main regex used to catch all the keywords of the action section.
        const static boost::regex rxsection;
		// DocString: FMTactionparser::rxoperator
		///This regex catches the operators used in the action section.
		const static boost::regex rxoperator;
		// DocString: FMTactionparser::m_SERIES_MATCH
		///This regex catchesthe series.
		const static boost::regex m_SERIES_MATCH;
		// DocString: FMTactionparser::_getSerie
		/**
		@brief get a serie from a line
		@param[in] p_line parsed line
		@param[in] p_actions the actions generated
		@return a valid serie
		*/
		Core::FMTSerie _getSerie(const std::string& p_line,
			const std::vector<Core::FMTAction>& p_actions) const;
		// DocString: FMTactionparser::sameActionAs
		/**
		Using a string containing potential action names (all_set) and a vector of (actions) this function
		returns a vector of pointer to the corresponding action present in the all_set string.
		*/
		std::vector<Core::FMTAction*> sameActionAs(const std::string& all_set, std::vector<Core::FMTAction>& actions) const;
		// DocString: FMTactionparser::getBounds
		/**
		This function fill up some specification (spec) usiing a string line of the area section.
		*/
		std::string getBounds(std::string& line, Core::FMTSpec& spec, const Core::FMTConstants& constants, const Core::FMTYields& ylds);
		// DocString: FMTactionparser::valAgg
		/**
		This functions turns aggregates of aggregates into simple aggregates of action map.
		*/
		std::map<std::string, std::vector<std::string>>valAgg(std::vector<Core::FMTAction>& actions, std::map<std::string, std::vector<std::string>>& aggregates);
	public:
		// DocString: FMTactionparser()
		/**
		Default constructor for FMTactionparser
		*/
        FMTactionparser();
		// DocString: ~FMTactionparser()
		/**
		Default destructor for FMTactionparser
		*/
		~FMTactionparser() = default;
		// DocString: FMTactionparser(const FMTactionparser&)
		/**
		Default copy constructor for FMTactionparser
		*/
        FMTactionparser(const FMTactionparser& rhs)=default;
		// DocString: FMTactionparser::operator
		/**
		Default copy assignment operator for FMTactionparser
		*/
        FMTactionparser& operator = (const FMTactionparser& rhs)=default;
		// DocString: FMTactionparser::read
		/**
		The read function will read actions from a (location) action file using a complete vector of (themes),
		a (yields) section, some and some (constants) and will returns a vector of FMTactions.
		*/
		std::vector<Core::FMTAction>read(const std::vector<Core::FMTTheme>& themes,
			const Core::FMTYields& yields,const Core::FMTConstants& constants,
			const std::string& location);
		// DocString: FMTactionparser::write
		/**
		Using the (location) of the action section to fill this function will write a vector of (actions)
		to the given location.
		*/
        void write(const std::vector<Core::FMTAction>& actions,
			const std::string& location,bool withgcbmagg = false) const;
		// DocString: FMTactionparser::getactionsidsofmodelyields
		/**
		Returns the corresponding actions ids of the vector of actions read from the .json file in YieldPredModels.
		*/
		std::vector<Core::FMTAction>getGCBMactionsaggregate(const std::vector<Core::FMTAction>& actions) const;
    };

}

#endif // FMTactionparser_Hm_included
