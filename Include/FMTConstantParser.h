/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTconstantparser_Hm_included
#define FMTconstantparser_Hm_included

#include "FMTParser.h"
#include <string>

namespace Core
{
	class FMTConstants;
}

namespace Parser
{
// DocString: FMTConstantParser
/**
The constant parser is used the read and write constants.
It's used by the FMTModelParser,
*/
class FMTEXPORT FMTConstantParser : public FMTParser
    {
    public:
		// DocString: FMTConstantParser()
		/**
		Default constructor for FMTConstantParser.
		*/
        FMTConstantParser();
		// DocString: ~FMTConstantParser()
		/**
		Default destructor for FMTConstantParser.
		*/
		~FMTConstantParser() = default;
		// DocString: FMTConstantParser(const FMTConstantParser&)
		/**
		Default copy constructor for FMTConstantParser
		*/
        FMTConstantParser(const FMTConstantParser& rhs)=default;
		// DocString: FMTConstantParser::operator=
		/**
		Default copy assignment operator for FMTConstantParser
		*/
        FMTConstantParser& operator = (const FMTConstantParser& rhs)=default;
		// DocString: FMTConstantParser::read
		/**
		This functions reads a constants file based on a given file (location).
		*/
        Core::FMTConstants read(const std::string& location);
		// DocString: FMTConstantParser::write
		/**
		This functions writes (constants) to a file (location).
		*/
        void write(const Core::FMTConstants& constants, const std::string& location) const;
		// DocString: FMTConstantParser:_fillConstants
		/**
		This functions writes (constants) to a file (location).
		*/
	private:
		// DocString: FMTParser::rxconstant
		///Regex to capture constant.
		const static boost::regex rxconstant;
		// DocString: FMTConstantParser::_fillConstants
		/**
		@brief fill the constants with the string
		@param[out] p_constants the filleds constants
		@param[in] p_input the input string.
		@param[in] p_allowNonValid allow non valid string to enter method
		@return true if added a constant else false
		*/
		bool _fillConstants(Core::FMTConstants& p_constants, 
			const std::string& p_input, bool p_allowNonValid = false) const;
		// DocString: FMTParser::getCleanLinewfor
		/**
		@brief Get a clean line and consider for loops when reading the file and also a temps fill of constants.
		@param[p_stream] p_stream  is the file stream
		@param[in] p_themes model themes
		@param[in] p_const the model constants
		@return a queue of lines to process
		*/
		std::queue<FMTLineInfo> getCleanLinewfor(std::ifstream& p_stream, 
			const std::vector<Core::FMTTheme>& p_themes,
			const Core::FMTConstants& p_cons) const override;

    };
}
#endif // FMTconstantparser_Hm_included
