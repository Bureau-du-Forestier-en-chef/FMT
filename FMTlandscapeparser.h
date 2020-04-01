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

#ifndef FMTlandscapeparser_H_INCLUDED
#define FMTlandscapeparser_H_INCLUDED

#include "FMTtheme.h"
#include "FMTconstants.h"
#include "FMTparser.h"
#include "FMTutility.h"
#include <regex>
#include <string>
#include <map>
#include <vector>


namespace Parser
{

class FMTlandscapeparser : public FMTparser
    {
    private:
        std::regex rxcleanlans;
		std::regex rxindex;
		std::regex rxparameter;
		std::map<std::string,double>getindexes(std::string index_line,const Core::FMTconstants& constants);
    public:
        FMTlandscapeparser();
		~FMTlandscapeparser() = default;
        FMTlandscapeparser(const FMTlandscapeparser& rhs);
        FMTlandscapeparser& operator = (const FMTlandscapeparser& rhs);
        std::vector<Core::FMTtheme>read(const Core::FMTconstants& constants,std::string location);
		#ifdef FMTWITHGDAL
			std::vector<Core::FMTtheme>readvectors(const  std::string& location);
			std::vector<Core::FMTtheme>readrasters(const  std::vector< std::string>& locations);
		#endif
        bool write(const  std::vector<Core::FMTtheme>& themes, std::string location);
    };
}


#endif // FMTlandscapeparser_H_INCLUDED
