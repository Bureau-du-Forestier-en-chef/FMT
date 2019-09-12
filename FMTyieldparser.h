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
#include <boost/tokenizer.hpp>
#include <iterator>

using namespace std;
using namespace Core;

namespace WSParser
{

class FMTyieldparser : public FMTparser
    {
     private:
        regex rxyieldtype;
        regex rxcomplex;
		regex rxeqs;
        FMTyldwstype getyldtype(const string& value) const;
        FMTyieldparserop getyldctype(const string& value) const;
        vector<string> getylduse(FMTyields& yielddata,
                                   vector<FMTyieldhandler>::iterator actualyield,
                                   const vector<string>& values) const;
        void checkpreexisting(const vector<string>& preexists) const;
		bool isfunction(const string& strfunction) const;
		double getnumwithproportion(const string& value,const FMTconstants& constants, 
			const vector<double>& proportions, const int& location);
		FMTdata geteq(const string& basestr,
                const FMTconstants& constants,
                const FMTyields& ylds,
                const vector<FMTtheme>& themes);
    public:
        FMTyieldparser();
        FMTyieldparser(const FMTyieldparser& rhs);
        FMTyieldparser& operator = (const FMTyieldparser& rhs);
        FMTyields read(const vector<FMTtheme>& themes,const FMTconstants& constants,string location);
        bool write(const FMTyields& yields, string location);
    };
}
#endif // FMTyieldparser_H_INCLUDED
