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

#ifndef FMTyieldparser_H_INCLUDED
#define FMTyieldparser_H_INCLUDED

#include "FMTlist.h"
#include "FMTparser.h"
#include "FMTyields.h"
#include "FMTutility.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTdata.h"
#include <boost/tokenizer.hpp>
#include <iterator>
#include <regex>
#include <string>
#include <vector>
#include <utility>

namespace Parser
{

class FMTyieldparser : public FMTparser
    {
     private:
		std::regex rxyieldtype;
		std::regex rxcomplex;
		std::regex rxeqs;
		std::regex rxdiscount;
		Core::FMTyldtype getyldtype(const std::string& value) const;
		Core::FMTyieldparserop getyldctype(const std::string& value) const;
		std::vector<std::string> getylduse(Core::FMTyields& yielddata,
			std::vector<std::pair<Core::FMTmask, Core::FMTyieldhandler>>::iterator actualyield,
                                   const std::vector<std::string>& values) const;
        void checkpreexisting(const std::vector<std::string>& preexists) const;
		bool isfunction(const std::string& strfunction) const;
		double getnumwithproportion(const std::string& value,const Core::FMTconstants& constants,
			const std::vector<double>& proportions, const int& location);
		Core::FMTdata geteq(const std::string& basestr,
                const Core::FMTconstants& constants,
                const Core::FMTyields& ylds,
                const std::vector<Core::FMTtheme>& themes);
    public:
        FMTyieldparser();
		~FMTyieldparser() = default;
        FMTyieldparser(const FMTyieldparser& rhs);
        FMTyieldparser& operator = (const FMTyieldparser& rhs);
        Core::FMTyields read(const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& constants,const std::string& location);
        void write(const Core::FMTyields& yields,const std::string& location) const;
    };
}
#endif // FMTyieldparser_H_INCLUDED
