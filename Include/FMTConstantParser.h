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
@brief Parser reading and writing constants, used by the model parser.
*/
class FMTEXPORT FMTConstantParser : public FMTParser
    {
    public:
		// DocString: FMTConstantParser()
		/**
		@brief Default constructor for FMTConstantParser.
		*/
        FMTConstantParser();
		// DocString: ~FMTConstantParser()
		/**
		@brief Default destructor for FMTConstantParser.
		*/
		~FMTConstantParser() = default;
		// DocString: FMTConstantParser(const FMTConstantParser&)
		/**
		@brief Copy constructor for FMTConstantParser.
		@param[in] rhs the FMTConstantParser to copy.
		*/
        FMTConstantParser(const FMTConstantParser& rhs)=default;
		// DocString: FMTConstantParser::operator=
		/**
		@brief Copy assignment operator for FMTConstantParser.
		@param[in] rhs the FMTConstantParser to copy.
		@return a reference to this FMTConstantParser.
		*/
        FMTConstantParser& operator = (const FMTConstantParser& rhs)=default;
		// DocString: FMTConstantParser::read
		/**
		@brief Read a constants file.
		@param[in] location the file location.
		@return the constants.
		*/
        Core::FMTConstants read(const std::string& location);
		// DocString: FMTConstantParser::write
		/**
		@brief Write constants to a file.
		@param[in] constants the constants.
		@param[in] location the file location.
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
		@brief Fill the constants from a string.
		@param[out] p_constants the filled constants.
		@param[in] p_input the input string.
		@param[in] p_allowNonValid if true allows non valid strings.
		@return true if a constant is added else false.
		*/
		bool _fillConstants(Core::FMTConstants& p_constants, 
			const std::string& p_input, bool p_allowNonValid = false) const;
		// DocString: FMTParser::getCleanLinewfor
		/**
		@brief Get a clean line considering for loops and a temporary fill of constants.
		@param[in,out] p_stream the file stream.
		@param[in] p_themes the model themes.
		@param[in] p_cons the model constants.
		@return a queue of lines to process.
		*/
		std::queue<FMTLineInfo> getCleanLinewfor(std::ifstream& p_stream, 
			const std::vector<Core::FMTTheme>& p_themes,
			const Core::FMTConstants& p_cons) const override;

    };
}
#endif // FMTconstantparser_Hm_included
