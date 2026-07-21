/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlifespanparser_Hm_included
#define FMTlifespanparser_Hm_included


#include "FMTParser.h"
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
@brief Parser reading and writing the lifespan section of the model.
*/
class FMTEXPORT FMTLifespanParser : public FMTParser
    {
    public:
		// DocString: FMTLifespanParser()
		/**
		@brief Default constructor for FMTLifespanParser.
		*/
        FMTLifespanParser();
		// DocString: ~FMTLifespanParser()
		/**
		@brief Default destructor for FMTLifespanParser.
		*/
		~FMTLifespanParser() = default;
		// DocString: FMTLifespanParser(const FMTLifespanParser&)
		/**
		@brief Copy constructor for FMTLifespanParser.
		@param[in] rhs the FMTLifespanParser to copy.
		*/
        FMTLifespanParser(const FMTLifespanParser& rhs);
		// DocString: FMTLifespanParser::operator=
		/**
		@brief Copy assignment operator for FMTLifespanParser.
		@param[in] rhs the FMTLifespanParser to copy.
		@return a reference to this FMTLifespanParser.
		*/
        FMTLifespanParser& operator = (const FMTLifespanParser& rhs);
		// DocString: FMTLifespanParser::read
		/**
		@brief Read the lifespan section.
		@param[in] themes the themes.
		@param[in] constants the constants.
		@param[in] location the file location.
		@return the lifespans.
		*/
        Core::FMTLifespans read(const std::vector<Core::FMTTheme>& themes,const Core::FMTConstants& constants,const std::string& location);
		// DocString: FMTLifespanParser::write
		/**
		@brief Write the lifespan section.
		@param[in] lifespan the lifespans.
		@param[in] location the file location.
		*/
        void write(const Core::FMTLifespans& lifespan,const std::string& location) const;
    };

}


#endif // FMTlifespanparser_Hm_included
