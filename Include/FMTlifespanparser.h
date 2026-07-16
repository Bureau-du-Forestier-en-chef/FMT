/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlifespanparser_Hm_included
#define FMTlifespanparser_Hm_included


#include "FMTparser.h"
#include <string>

namespace Core {
	class FMTLifespans;
	class FMTTheme;
	class FMTConstants;
}

namespace Parser

{
// DocString: FMTLifespanParser
/**
The lifespan parser is used to read the lifespan part of the model.The class is made to
read and write lifespan section.
*/
class FMTEXPORT FMTLifespanParser : public FMTParser
    {
    public:
		// DocString: FMTLifespanParser()
		/**
		Default constructor for FMTLifespanParser.
		*/
        FMTLifespanParser();
		// DocString: ~FMTLifespanParser()
		/**
		Default destructor for FMTLifespanParser.
		*/
		~FMTLifespanParser() = default;
		// DocString: FMTLifespanParser(const FMTLifespanParser&)
		/**
		Default copy constructor for FMTLifespanParser.
		*/
        FMTLifespanParser(const FMTLifespanParser& rhs);
		// DocString: FMTLifespanParser::operator=
		/**
		Default copy assignment for FMTLifespanParser.
		*/
        FMTLifespanParser& operator = (const FMTLifespanParser& rhs);
		// DocString: FMTLifespanParser::read
		/**
		Read function for the lifespan section, needs (themes), (constants) and finaly the (location) of the lifespansection.
		*/
        Core::FMTLifespans read(const std::vector<Core::FMTTheme>& themes,const Core::FMTConstants& constants,const std::string& location);
		// DocString: FMTLifespanParser::write
		/**
		Write function for the lifespan section, needs (lifespan) and  the (location) of the new lifespansection.
		*/
        void write(const Core::FMTLifespans& lifespan,const std::string& location) const;
    };

}


#endif // FMTlifespanparser_Hm_included
