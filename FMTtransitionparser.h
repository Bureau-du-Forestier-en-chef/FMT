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

#ifndef FMTtransitionparser_H_INCLUDED
#define FMTtransitionparser_H_INCLUDED

#include "FMTparser.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMTtransitionmask.h"
#include "FMTyields.h"
#include <regex>
#include "string"
#include <vector>
#include "FMTmask.h"

namespace Parser
{

class FMTtransitionparser : public FMTparser
    {
		std::regex rxsection;
		std::regex rxlock;
		std::regex rxage;
		std::regex rxreplace;
		std::regex rxtyld;
    public:
        FMTtransitionparser();
        FMTtransitionparser(const FMTtransitionparser& rhs);
        FMTtransitionparser& operator = (const FMTtransitionparser& rhs);
		Core::FMTmask getsource(std::string& line, Core::FMTspec& spec,const std::vector<Core::FMTtheme>& themes,
							Core::FMTwssect section,const Core::FMTconstants& constant,
                          const Core::FMTyields& ylds);
        std::vector<Core::FMTtransitionmask> getmasktran(const std::string& line,const std::vector<Core::FMTtheme>& themes,
                                        const Core::FMTconstants& constants, const Core::FMTyields& ylds,
                                        const Core::FMTmask& sourcemask, int& replaced);
        std::vector<Core::FMTtransition>read(const std::vector<Core::FMTtheme>& themes,
                           const std::vector<Core::FMTaction>& actions,
                           const Core::FMTyields& ylds,const Core::FMTconstants& constants,
							std::string location);
        bool write(const std::vector<Core::FMTtransition>& transitions, std::string location);
    };
}
#endif // FMTtransitionparser_H_INCLUDED
