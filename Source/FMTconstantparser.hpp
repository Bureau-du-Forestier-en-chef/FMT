/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTconstantparser_H_INCLUDED
#define FMTconstantparser_H_INCLUDED

#include "FMTparser.hpp"
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
        FMTconstantparser(const FMTconstantparser& rhs);
		// DocString: FMTconstantparser::operator=
		/**
		Default copy assignment operator for FMTconstantparser
		*/
        FMTconstantparser& operator = (const FMTconstantparser& rhs);
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
#endif // FMTconstantparser_H_INCLUDED
