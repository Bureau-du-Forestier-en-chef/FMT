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
	class FMTconstants;
	class FMTtheme;
	class FMTaction;
	class FMTyields;
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
		// DocString: FMTactionparser::sameactionas
		/**
		Using a string containing potential action names (all_set) and a vector of (actions) this function
		returns a vector of pointer to the corresponding action present in the all_set string.
		*/
		std::vector<Core::FMTaction*> sameactionas(const std::string& all_set, std::vector<Core::FMTaction>& actions) const;
		// DocString: FMTactionparser::getbounds
		/**
		This function fill up some specification (spec) usiing a string line of the area section.
		*/
		std::string getbounds(std::string& line, Core::FMTspec& spec, const Core::FMTconstants& constants, const Core::FMTyields& ylds);
		// DocString: FMTactionparser::valagg
		/**
		This functions turns aggregates of aggregates into simple aggregates of action map.
		*/
		std::map<std::string, std::vector<std::string>>valagg(std::vector<Core::FMTaction>& actions, std::map<std::string, std::vector<std::string>>& aggregates);
		// DocString: FMTactionparser::cleanactionseries
		/**
		Remove action series that are already part of other series and returns the new series.
		*/
		std::vector<std::vector<std::string>> cleanactionseries(const std::vector<std::vector<std::string>>& series) const;
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
		std::vector<Core::FMTaction>read(const std::vector<Core::FMTtheme>& themes,
			const Core::FMTyields& yields,const Core::FMTconstants& constants,
			const std::string& location);
		// DocString: FMTactionparser::write
		/**
		Using the (location) of the action section to fill this function will write a vector of (actions)
		to the given location.
		*/
        void write(const std::vector<Core::FMTaction>& actions,
			const std::string& location,bool withgcbmagg = false) const;
		// DocString: FMTactionparser::getactionsidsofmodelyields
		/**
		Returns the corresponding actions ids of the vector of actions read from the .json file in YieldPredModels.
		*/
		std::vector<Core::FMTaction>getGCBMactionsaggregate(const std::vector<Core::FMTaction>& actions) const;
    };

}

#endif // FMTactionparser_Hm_included
