/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTtransitionparser_Hm_included
#define FMTtransitionparser_Hm_included

#include "FMTparser.h"
#include <boost/regex.hpp>
#include "string"
#include <vector>


namespace Core {
	class FMTMask;
	class FMTYields;
	class FMTTransition;
	class FMTAction;
	class FMTTheme;
	class FMTConstants;
	class FMTTransitionMask;
	class FMTGCBMTransition;
}

namespace Parser
{
// DocString: FMTTransitionParser
/**
The transition parser can read and write a transitions sections and transform the information into
an std::vector<Core::FMTTransition> object.
*/
class FMTEXPORT FMTTransitionParser : public FMTParser
    {
		// DocString: FMTTransitionParser::rxsection
		///Regex to capture the transition section.
		const static boost::regex rxsection;
		// DocString: FMTTransitionParser::rxlock
		///Regex to capture the lock level of the transition mask.
		const static boost::regex rxlock;
		// DocString: FMTTransitionParser::rxage
		///Regex to capture the age level of the transition mask.
		const static boost::regex rxage;
		// DocString: FMTTransitionParser::rxreplace
		///Regex to capture the age level of the transition mask.
		const static boost::regex rxreplace;
		// DocString: FMTTransitionParser::rxtyld
		///Regex to capture yield bounds of the transition mask.
		const static boost::regex rxtyld;
    public:
		// DocString: FMTTransitionParser()
		/**
		Default constructor for FMTTransitionParser
		*/
        FMTTransitionParser();
		// DocString: ~FMTTransitionParser()
		/**
		Default destructor for FMTTransitionParser.
		*/
		~FMTTransitionParser() = default;
		// DocString: FMTTransitionParser(const FMTTransitionParser&)
		/**
		Copy constructor for FMTTransitionParser.
		*/
        FMTTransitionParser(const FMTTransitionParser& rhs)=default;
		// DocString: FMTTransitionParser::FMTTransitionParser=
		/**
		Copy assignment for FMTTransitionParser.
		*/
        FMTTransitionParser& operator = (const FMTTransitionParser& rhs)=default;
		// DocString: FMTTransitionParser::getSource
		/**
		Returns a mask for the transition mask and fill up the spec of the transition mask.
		*/
		Core::FMTMask getSource(std::string& line, Core::FMTSpec& spec,const std::vector<Core::FMTTheme>& themes,
							Core::FMTsection section,const Core::FMTConstants& constant,
                          const Core::FMTYields& ylds);
		// DocString: FMTTransitionParser::getMaskTran
		/**
		Get a vector of transition masks based on a single transition line mask of the transition file.
		*/
        std::vector<Core::FMTTransitionMask> getMaskTran(const std::string& line,const std::vector<Core::FMTTheme>& themes,
                                        const Core::FMTConstants& constants, const Core::FMTYields& ylds,
                                        const Core::FMTMask& sourcemask, int& replaced);
		// DocString: FMTTransitionParser::read
		/**
		Get a vector of FMTTransition objects based on (themes), (actions),(yields),(constants) and finaly the location of the file.
		*/
        std::vector<Core::FMTTransition>read(const std::vector<Core::FMTTheme>& themes,
                           const std::vector<Core::FMTAction>& actions,
                           const Core::FMTYields& ylds,const Core::FMTConstants& constants,
						   const std::string& location);
		// DocString: FMTTransitionParser::write
		/**
		Write a vector of FMTtransitions into a file location.
		*/
        void write(const std::vector<Core::FMTTransition>& transitions,const std::string& location) const;
		// DocString: FMTTransitionParser::writeGCBM
		/**
		Write a vector of FMTGCBMtransitions into a text file at (location).
		*/
		void writeGCBM(const std::vector<Core::FMTGCBMTransition>& transitions, const std::string& location) const;
		// DocString: FMTTransitionParser::readGCBM
		/**
		From a text file (location) readGCBM transitions
		*/
		std::vector<Core::FMTGCBMTransition>readGCBM(const std::string& location) const;
    };
}
#endif // FMTtransitionparser_Hm_included
