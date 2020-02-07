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

#ifndef FMToutputparser_H_INCLUDED
#define FMToutputparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTconstants.h"
#include "FMToutput.h"

namespace WSParser

{

class FMToutputparser : public FMTparser
    {
	std::regex rxoutput;
	std::regex rxsource;
	std::regex rxtar;
	std::regex rxgrp;
    public:
        FMToutputparser();
        FMToutputparser(const FMToutputparser& rhs);
        FMToutputparser& operator = (const FMToutputparser& rhs);
		std::vector<Core::FMToutput> read(const std::vector<Core::FMTtheme>& themes,
                            const std::vector<Core::FMTaction>& actions,
                            const Core::FMTyields& ylds,const Core::FMTconstants& constants,
							std::string location);
        bool write(const std::vector<Core::FMToutput>& outputs, std::string location);
    };

}


#endif // FMToutputparser_H_INCLUDED
