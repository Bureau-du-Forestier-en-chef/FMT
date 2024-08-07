/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMToutputparser_H_INCLUDED
#define FMToutputparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTutility.h"
#include <boost/regex.hpp>
#include <string>

namespace Core
{
	class FMTtheme;
	class FMTaction;
	class FMTyields;
	class FMTconstants;
	class FMToutput;
	class FMToutputsource;
	class FMToperator;
}

namespace Parser

{
// DocString: FMToutputparser
/**
The FMToutputparser is made to read and write a vector of FMToutput from or into a given file.
*/
class FMTEXPORT FMToutputparser : public FMTparser
    {
	// DocString: FMToutputparser::rxoutput
	///Regex to capture the name of the output and other informations.
	const static boost::regex rxoutput;
	// DocString: FMToutputparser::rxsource
	///Regex to capture the output sources.
	const static boost::regex rxsource;
	// DocString: FMToutputparser::rxtar
	///Regex to capture the output source specifications target.
	const static boost::regex rxtar;
	// DocString: FMToutputparser::rxgrp
	///Regex to capture outputs groups
	const static boost::regex rxgrp;
	// DocString: FMToutputparser::rxoutputconstant
	///Regex to capture constant output.
	const static boost::regex rxoutputconstant;
	// DocString: FMToutputparser::readnfill
	/**
	Main function used by read and addoutputs to parse a file and fill a vector of outputs
	*/
	void readnfill(std::vector<Core::FMToutput>* outputs, 
					const std::vector<Core::FMTtheme>& themes,
					const std::vector<Core::FMTaction>& actions,
					const Core::FMTyields& ylds,const Core::FMTconstants& constants,
					const std::string& location);
	// DocString: FMToutputparser::appendtooutput
	/**
	When you need to append output data to output.
	*/
	void appendtooutput(
		const std::string& strvalue,
		const int& outputid,
		const int& themetarget,
		const size_t& lastoutput,
		std::string& lastoperator,
		std::vector<std::string>& stroperators,
		std::vector<Core::FMToutputsource>& sources,
		std::vector<Core::FMToperator>& operators) const;

    public:
		// DocString: FMToutputparser()
		/**
		Default constructor for FMToutputparser.
		*/
        FMToutputparser();
		// DocString: ~FMToutputparser()
		/**
		Default destructor for FMToutputparser.
		*/
		~FMToutputparser() = default;
		// DocString: FMToutputparser(const FMToutputparser&)
		/**
		Default copy constructor for FMToutputparser.
		*/
        FMToutputparser(const FMToutputparser& rhs)=default;
		// DocString: FMToutputparser::operator=
		/**
		Default copy assignment for FMToutputparser.
		*/
        FMToutputparser& operator = (const FMToutputparser& rhs)=default;
		// DocString: FMToutputparser::read
		/**
		This function read a output file (location) based on (themes),(actions),(yields),(constants) and returns a vector of FMToutput.
		*/
		std::vector<Core::FMToutput> read(const std::vector<Core::FMTtheme>& themes,
                            const std::vector<Core::FMTaction>& actions,
                            const Core::FMTyields& ylds,const Core::FMTconstants& constants,
							const std::string& location);
		// DocString: FMToutputparser::addoutputs
		/**
		This function read a output file and add the desired outputs(outputsnames) found in the output file(location) to the vector of outputs(oldoutputs) 
		based on (themes),(actions),(yields),(constants) and returns a vector of FMToutput. If outputsnames is empty, all the outputs in the file will
		be add to the vector of outputs.
		*/
		std::vector<Core::FMToutput> addoutputs(const std::vector<Core::FMToutput> oldoutputs, 
							const std::vector<Core::FMTtheme>& themes,
                            const std::vector<Core::FMTaction>& actions,
                            const Core::FMTyields& ylds,const Core::FMTconstants& constants,
							const std::string& location,
							std::vector<std::string> outputsnames = std::vector<std::string>());
		// DocString: FMToutputparser::write
		/**
		This function write a vector of FMToutput to a file at a given (location).
		*/
        void write(const std::vector<Core::FMToutput>& outputs,const std::string& location) const;
    };

}


#endif // FMToutputparser_H_INCLUDED
