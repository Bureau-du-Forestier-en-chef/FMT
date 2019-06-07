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
