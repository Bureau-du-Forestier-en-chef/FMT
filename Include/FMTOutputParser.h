/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMToutputparser_Hm_included
#define FMToutputparser_Hm_included

#include "FMTParser.h"
#include "FMTutility.h"
#include <boost/regex.hpp>
#include <string>

namespace Core
{
	class FMTTheme;
	class FMTAction;
	class FMTYields;
	class FMTConstants;
	class FMTOutput;
	class FMTOutputSource;
	class FMTOperator;
}

namespace Parser

{
// DocString: FMTOutputParser
/**
@brief Parser reading and writing a vector of FMTOutput from or into a file.
*/
class FMTEXPORT FMTOutputParser : public FMTParser
    {
	// DocString: FMTOutputParser::rxoutput
	///Regex to capture the name of the output and other informations.
	const static boost::regex rxoutput;
	// DocString: FMTOutputParser::rxsource
	///Regex to capture the output sources.
	const static boost::regex rxsource;
	// DocString: FMTOutputParser::rxtar
	///Regex to capture the output source specifications target.
	const static boost::regex rxtar;
	// DocString: FMTOutputParser::rxgrp
	///Regex to capture outputs groups
	const static boost::regex rxgrp;
	// DocString: FMTOutputParser::rxoutputconstant
	///Regex to capture constant output.
	const static boost::regex rxoutputconstant;
	// DocString: FMTOutputParser::readNFill
	/**
	@brief Parse a file and fill a vector of outputs, used by read and addOutputs.
	@param[in,out] outputs the outputs to fill.
	@param[in] themes the themes.
	@param[in] actions the actions.
	@param[in] ylds the yields.
	@param[in] constants the constants.
	@param[in] location the file location.
	*/
	void readNFill(std::vector<Core::FMTOutput>* outputs, 
					const std::vector<Core::FMTTheme>& themes,
					const std::vector<Core::FMTAction>& actions,
					const Core::FMTYields& ylds,const Core::FMTConstants& constants,
					const std::string& location);
	// DocString: FMTOutputParser::appendToOutput
	/**
	@brief Append output data to an output.
	@param[in] strvalue the value string.
	@param[in] outputid the output id.
	@param[in] themetarget the theme target.
	@param[in] lastoutput the last output index.
	@param[in,out] lastoperator the last operator.
	@param[in,out] stroperators the operator strings.
	@param[in,out] sources the output sources.
	@param[in,out] operators the operators.
	*/
	void appendToOutput(
		const std::string& strvalue,
		const int& outputid,
		const int& themetarget,
		const size_t& lastoutput,
		std::string& lastoperator,
		std::vector<std::string>& stroperators,
		std::vector<Core::FMTOutputSource>& sources,
		std::vector<Core::FMTOperator>& operators) const;

    public:
		// DocString: FMTOutputParser()
		/**
		@brief Default constructor for FMTOutputParser.
		*/
        FMTOutputParser();
		// DocString: ~FMTOutputParser()
		/**
		@brief Default destructor for FMTOutputParser.
		*/
		~FMTOutputParser() = default;
		// DocString: FMTOutputParser(const FMTOutputParser&)
		/**
		@brief Copy constructor for FMTOutputParser.
		@param[in] rhs the FMTOutputParser to copy.
		*/
        FMTOutputParser(const FMTOutputParser& rhs)=default;
		// DocString: FMTOutputParser::operator=
		/**
		@brief Copy assignment operator for FMTOutputParser.
		@param[in] rhs the FMTOutputParser to copy.
		@return a reference to this FMTOutputParser.
		*/
        FMTOutputParser& operator = (const FMTOutputParser& rhs)=default;
		// DocString: FMTOutputParser::read
		/**
		@brief Read an output file.
		@param[in] themes the themes.
		@param[in] actions the actions.
		@param[in] ylds the yields.
		@param[in] constants the constants.
		@param[in] location the file location.
		@return the outputs.
		*/
		std::vector<Core::FMTOutput> read(const std::vector<Core::FMTTheme>& themes,
                            const std::vector<Core::FMTAction>& actions,
                            const Core::FMTYields& ylds,const Core::FMTConstants& constants,
							const std::string& location);
		// DocString: FMTOutputParser::addOutputs
		/**
		@brief Read an output file and add the desired outputs to a vector of outputs.
		@param[in] oldoutputs the existing outputs.
		@param[in] themes the themes.
		@param[in] actions the actions.
		@param[in] ylds the yields.
		@param[in] constants the constants.
		@param[in] location the file location.
		@param[in] outputsnames the output names to add, all if empty.
		@return the outputs.
		*/
		std::vector<Core::FMTOutput> addOutputs(const std::vector<Core::FMTOutput> oldoutputs, 
							const std::vector<Core::FMTTheme>& themes,
                            const std::vector<Core::FMTAction>& actions,
                            const Core::FMTYields& ylds,const Core::FMTConstants& constants,
							const std::string& location,
							std::vector<std::string> outputsnames = std::vector<std::string>());
		// DocString: FMTOutputParser::write
		/**
		@brief Write a vector of outputs to a file.
		@param[in] outputs the outputs.
		@param[in] location the file location.
		*/
        void write(const std::vector<Core::FMTOutput>& outputs,const std::string& location) const;
    };

}


#endif // FMToutputparser_Hm_included
