/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMToutputparser_Hm_included
#define FMToutputparser_Hm_included

#include "FMTparser.h"
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
The FMTOutputParser is made to read and write a vector of FMTOutput from or into a given file.
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
	Main function used by read and addOutputs to parse a file and fill a vector of outputs
	*/
	void readNFill(std::vector<Core::FMTOutput>* outputs, 
					const std::vector<Core::FMTTheme>& themes,
					const std::vector<Core::FMTAction>& actions,
					const Core::FMTYields& ylds,const Core::FMTConstants& constants,
					const std::string& location);
	// DocString: FMTOutputParser::appendToOutput
	/**
	When you need to append output data to output.
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
		Default constructor for FMTOutputParser.
		*/
        FMTOutputParser();
		// DocString: ~FMTOutputParser()
		/**
		Default destructor for FMTOutputParser.
		*/
		~FMTOutputParser() = default;
		// DocString: FMTOutputParser(const FMTOutputParser&)
		/**
		Default copy constructor for FMTOutputParser.
		*/
        FMTOutputParser(const FMTOutputParser& rhs)=default;
		// DocString: FMTOutputParser::operator=
		/**
		Default copy assignment for FMTOutputParser.
		*/
        FMTOutputParser& operator = (const FMTOutputParser& rhs)=default;
		// DocString: FMTOutputParser::read
		/**
		This function read a output file (location) based on (themes),(actions),(yields),(constants) and returns a vector of FMTOutput.
		*/
		std::vector<Core::FMTOutput> read(const std::vector<Core::FMTTheme>& themes,
                            const std::vector<Core::FMTAction>& actions,
                            const Core::FMTYields& ylds,const Core::FMTConstants& constants,
							const std::string& location);
		// DocString: FMTOutputParser::addOutputs
		/**
		This function read a output file and add the desired outputs(outputsnames) found in the output file(location) to the vector of outputs(oldoutputs) 
		based on (themes),(actions),(yields),(constants) and returns a vector of FMTOutput. If outputsnames is empty, all the outputs in the file will
		be add to the vector of outputs.
		*/
		std::vector<Core::FMTOutput> addOutputs(const std::vector<Core::FMTOutput> oldoutputs, 
							const std::vector<Core::FMTTheme>& themes,
                            const std::vector<Core::FMTAction>& actions,
                            const Core::FMTYields& ylds,const Core::FMTConstants& constants,
							const std::string& location,
							std::vector<std::string> outputsnames = std::vector<std::string>());
		// DocString: FMTOutputParser::write
		/**
		This function write a vector of FMTOutput to a file at a given (location).
		*/
        void write(const std::vector<Core::FMTOutput>& outputs,const std::string& location) const;
    };

}


#endif // FMToutputparser_Hm_included
