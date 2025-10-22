/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTconstantparser_Hm_included
#define FMTconstantparser_Hm_included

#include "FMTparser.h"
#include <string>

namespace Core
{
	class FMTconstants;
}

namespace Parser
{
// DocString: FMTconstantparser
/**
The constant parser is used the read and write constants.
It's used by the FMTmodelparser,
*/
class FMTEXPORT FMTconstantparser : public FMTparser
    {
	// DocString: FMTparser::rxconstant
	///Regex to capture constant.
	const static boost::regex rxconstant;
    public:
		// DocString: FMTconstantparser()
		/**
		Default constructor for FMTconstantparser.
		*/
        FMTconstantparser();
		// DocString: ~FMTconstantparser()
		/**
		Default destructor for FMTconstantparser.
		*/
		~FMTconstantparser() = default;
		// DocString: FMTconstantparser(const FMTconstantparser&)
		/**
		Default copy constructor for FMTconstantparser
		*/
        FMTconstantparser(const FMTconstantparser& rhs)=default;
		// DocString: FMTconstantparser::operator=
		/**
		Default copy assignment operator for FMTconstantparser
		*/
        FMTconstantparser& operator = (const FMTconstantparser& rhs)=default;
		// DocString: FMTconstantparser::read
		/**
		This functions reads a constants file based on a given file (location).
		*/
        Core::FMTconstants read(const std::string& location);
		// DocString: FMTconstantparser::write
		/**
		This functions writes (constants) to a file (location).
		*/
        void write(const Core::FMTconstants& constants, const std::string& location) const;
    };
}
#endif // FMTconstantparser_Hm_included
