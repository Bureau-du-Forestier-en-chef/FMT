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
		// DocString: FMTconstantparser:_FillConstants
		/**
		This functions writes (constants) to a file (location).
		*/
	private:
		// DocString: FMTparser::rxconstant
		///Regex to capture constant.
		const static boost::regex rxconstant;
		// DocString: FMTconstantparser::_FillConstants
		/**
		@brief fill the constants with the string
		@param[out] p_constants the filleds constants
		@param[in] p_input the input string.
		@param[in] p_allowNonValid allow non valid string to enter method
		@return true if added a constant else false
		*/
		bool _FillConstants(Core::FMTconstants& p_constants, 
			const std::string& p_input, bool p_allowNonValid = false) const;
		// DocString: FMTparser::GetCleanLinewfor
		/**
		@brief Get a clean line and consider for loops when reading the file and also a temps fill of constants.
		@param[p_stream] p_stream  is the file stream
		@param[in] p_themes model themes
		@param[in] p_const the model constants
		@return a queue of lines to process
		*/
		std::queue<FMTLineInfo> GetCleanLinewfor(std::ifstream& p_stream, 
			const std::vector<Core::FMTtheme>& p_themes,
			const Core::FMTconstants& p_cons) const override;

    };
}
#endif // FMTconstantparser_Hm_included
