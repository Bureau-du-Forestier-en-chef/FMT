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
/*
#include "xlocale.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_rat.h"
*/
#include "FMTtheme.h"
#include "FMTparser.h"

using namespace std;
using namespace Core;

namespace WSParser
{

class FMTlandscapeparser : public FMTparser
    {
    private:
        regex rxcleanlans;
        regex rxindex;
        regex rxparameter;
        map<string,double>getindexes(string index_line,const FMTconstants& constants);
    public:
        FMTlandscapeparser();
        FMTlandscapeparser(const FMTlandscapeparser& rhs);
        FMTlandscapeparser& operator = (const FMTlandscapeparser& rhs);
        vector<FMTtheme>read(const FMTconstants& constants,string location);
        vector<FMTtheme>readvectors(const string& location);
        vector<FMTtheme>readrasters(const vector<string>& locations);
        bool write(const vector<FMTtheme>& themes,string location);
    };
}


#endif // FMTlandscapeparser_H_INCLUDED
