/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlifespanparser_H_INCLUDED
#define FMTlifespanparser_H_INCLUDED


#include "FMTparser.hpp"
#include <string>

namespace Core {
	class FMTlifespans;
	class FMTtheme;
	class FMTconstants;
}

namespace Parser

{
// DocString: FMTlifespanparser
/**
The lifespan parser is used to read the lifespan part of the model.The class is made to
read and write lifespan section.
*/
class FMTEXPORT FMTlifespanparser : public FMTparser
    {
    public:
		// DocString: FMTlifespanparser()
		/**
		Default constructor for FMTlifespanparser.
		*/
        FMTlifespanparser();
		// DocString: ~FMTlifespanparser()
		/**
		Default destructor for FMTlifespanparser.
		*/
		~FMTlifespanparser() = default;
		// DocString: FMTlifespanparser(const FMTlifespanparser&)
		/**
		Default copy constructor for FMTlifespanparser.
		*/
        FMTlifespanparser(const FMTlifespanparser& rhs);
		// DocString: FMTlifespanparser::operator=
		/**
		Default copy assignment for FMTlifespanparser.
		*/
        FMTlifespanparser& operator = (const FMTlifespanparser& rhs);
		// DocString: FMTlifespanparser::read
		/**
		Read function for the lifespan section, needs (themes), (constants) and finaly the (location) of the lifespansection.
		*/
        Core::FMTlifespans read(const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& constants,const std::string& location);
		// DocString: FMTlifespanparser::write
		/**
		Write function for the lifespan section, needs (lifespan) and  the (location) of the new lifespansection.
		*/
        void write(const Core::FMTlifespans& lifespan,const std::string& location) const;
    };

}


#endif // FMTlifespanparser_H_INCLUDED
