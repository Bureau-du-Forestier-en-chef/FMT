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
	class FMTmask;
	class FMTyields;
	class FMTtransition;
	class FMTaction;
	class FMTtheme;
	class FMTconstants;
	class FMTtransitionmask;
	class FMTGCBMtransition;
}

namespace Parser
{
// DocString: FMTtransitionparser
/**
The transition parser can read and write a transitions sections and transform the information into
an std::vector<Core::FMTtransition> object.
*/
class FMTEXPORT FMTtransitionparser : public FMTparser
    {
		// DocString: FMTtransitionparser::rxsection
		///Regex to capture the transition section.
		const static boost::regex rxsection;
		// DocString: FMTtransitionparser::rxlock
		///Regex to capture the lock level of the transition mask.
		const static boost::regex rxlock;
		// DocString: FMTtransitionparser::rxage
		///Regex to capture the age level of the transition mask.
		const static boost::regex rxage;
		// DocString: FMTtransitionparser::rxreplace
		///Regex to capture the age level of the transition mask.
		const static boost::regex rxreplace;
		// DocString: FMTtransitionparser::rxtyld
		///Regex to capture yield bounds of the transition mask.
		const static boost::regex rxtyld;
    public:
		// DocString: FMTtransitionparser()
		/**
		Default constructor for FMTtransitionparser
		*/
        FMTtransitionparser();
		// DocString: ~FMTtransitionparser()
		/**
		Default destructor for FMTtransitionparser.
		*/
		~FMTtransitionparser() = default;
		// DocString: FMTtransitionparser(const FMTtransitionparser&)
		/**
		Copy constructor for FMTtransitionparser.
		*/
        FMTtransitionparser(const FMTtransitionparser& rhs)=default;
		// DocString: FMTtransitionparser::FMTtransitionparser=
		/**
		Copy assignment for FMTtransitionparser.
		*/
        FMTtransitionparser& operator = (const FMTtransitionparser& rhs)=default;
		// DocString: FMTtransitionparser::getsource
		/**
		Returns a mask for the transition mask and fill up the spec of the transition mask.
		*/
		Core::FMTmask getsource(std::string& line, Core::FMTspec& spec,const std::vector<Core::FMTtheme>& themes,
							Core::FMTsection section,const Core::FMTconstants& constant,
                          const Core::FMTyields& ylds);
		// DocString: FMTtransitionparser::getmasktran
		/**
		Get a vector of transition masks based on a single transition line mask of the transition file.
		*/
        std::vector<Core::FMTtransitionmask> getmasktran(const std::string& line,const std::vector<Core::FMTtheme>& themes,
                                        const Core::FMTconstants& constants, const Core::FMTyields& ylds,
                                        const Core::FMTmask& sourcemask, int& replaced);
		// DocString: FMTtransitionparser::read
		/**
		Get a vector of FMTtransition objects based on (themes), (actions),(yields),(constants) and finaly the location of the file.
		*/
        std::vector<Core::FMTtransition>read(const std::vector<Core::FMTtheme>& themes,
                           const std::vector<Core::FMTaction>& actions,
                           const Core::FMTyields& ylds,const Core::FMTconstants& constants,
						   const std::string& location);
		// DocString: FMTtransitionparser::write
		/**
		Write a vector of FMTtransitions into a file location.
		*/
        void write(const std::vector<Core::FMTtransition>& transitions,const std::string& location) const;
		// DocString: FMTtransitionparser::writeGCBM
		/**
		Write a vector of FMTGCBMtransitions into a text file at (location).
		*/
		void writeGCBM(const std::vector<Core::FMTGCBMtransition>& transitions, const std::string& location) const;
		// DocString: FMTtransitionparser::readGCBM
		/**
		From a text file (location) readGCBM transitions
		*/
		std::vector<Core::FMTGCBMtransition>readGCBM(const std::string& location) const;
    };
}
#endif // FMTtransitionparser_Hm_included
