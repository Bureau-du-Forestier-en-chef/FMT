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

#ifndef FMTactionparser_H_INCLUDED
#define FMTactionparser_H_INCLUDED
#include "FMTlist.h"
#include "FMTparser.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTutility.h"
#include <regex>
#include <vector>
#include <map>


namespace Parser
{
// DocString: FMTactionparser
/**
FMTactionparser reads and writes actions from or into an action files.
This parser is also used by the modelparser.
*/
class FMTactionparser : public FMTparser
    {
    private:
		// DocString: FMTactionparser::rxsection
		///This the main regex used to catch all the keywords of the action section.
        std::regex rxsection;
		// DocString: FMTactionparser::rxoperator
		///This regex catches the operators used in the action section.
		std::regex rxoperator;
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
        FMTactionparser(const FMTactionparser& rhs);
		// DocString: FMTactionparser::operator
		/**
		Default copy assignment operator for FMTactionparser
		*/
        FMTactionparser& operator = (const FMTactionparser& rhs);
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
			const std::string& location) const;
    };

}

#endif // FMTactionparser_H_INCLUDED
