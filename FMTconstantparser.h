/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef FMTconstantparser_H_INCLUDED
#define FMTconstantparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTconstants.h"
#include <string>

namespace Parser
{
// DocString: FMTconstantparser
/**
The constant parser is used the read and write constants.
It's used by the FMTmodelparser,
*/
class FMTconstantparser : public FMTparser
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
