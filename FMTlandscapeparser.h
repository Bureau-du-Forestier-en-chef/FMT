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
