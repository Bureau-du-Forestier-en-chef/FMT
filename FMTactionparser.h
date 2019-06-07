#ifndef FMTactionparser_H_INCLUDED
#define FMTactionparser_H_INCLUDED
#include "FMTlist.h"
#include "FMTparser.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTyields.h"


using namespace boost::python;
using namespace std;
using namespace Core;

namespace WSParser
{

class FMTactionparser : public FMTparser
    {
    private:
        regex rxsection;
        regex rxoperator;
        vector<string>operators;
        vector<FMTaction*> sameactionas(const string& all_set,vector<FMTaction>& actions) const;
    public:
        FMTactionparser();
        FMTactionparser(const FMTactionparser& rhs);
        FMTactionparser& operator = (const FMTactionparser& rhs);
        string getbounds(string& line, FMTspec& spec,const FMTconstants& constants, const FMTyields& ylds);
        map<string,vector<string>>valagg(vector<FMTaction>& actions,map<string,vector<string>>& aggregates);
        vector<FMTaction>read(const vector<FMTtheme>& themes,
			const FMTyields& yields,const FMTconstants& constants,
			string location, map<string,vector<string>>& aggregates);
        bool write(const vector<FMTaction>& actions,
			string location,const  map<string, vector<string>>& aggregates);
    };

}

#endif // FMTactionparser_H_INCLUDED
