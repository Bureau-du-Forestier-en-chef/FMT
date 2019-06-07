#ifndef FMTmodelparser_H_INCLUDED
#define FMTmodelparser_H_INCLUDED

//#include "FMTwsParser.h"
#include "FMTparser.h"
#include "FMTconstantparser.h"
#include "FMTlandscapeparser.h"
#include "FMTareaparser.h"
#include "FMTyieldparser.h"
#include "FMTactionparser.h"
#include "FMTtransitionparser.h"
#include "FMTlifespanparser.h"
#include "FMToutputparser.h"
#include "FMToptimizationparser.h"
#include "FMTscheduleparser.h"


#include "FMTmodel.h"
#include "boost/filesystem.hpp"
#include <map>


using namespace std;
using namespace Core;
using namespace Models;

namespace WSParser
{

class FMTmodelparser : public FMTparser
    {
	FMTmodel referenceread(map<string, vector<int>>& common_sections,
					vector<FMTmodel>& models,
					const string& con, const string& lan,
					const string& lif, const string& are, const string& yld,
					const string& act, const string& tr, const string& out,
					string opt = string(),bool allow_mapping=false);
    public:
        FMTmodelparser();
        FMTmodelparser(const FMTmodelparser& rhs);
        FMTmodelparser& operator = (const FMTmodelparser& rhs);
        FMTmodel read(const string& con,const string& lan,
                      const string& lif,const string& are,const string& yld,
                      const string& act,const string& tr,const string& out,
						string opt = string());
		vector<FMTmodel>readproject(const string& primary_location,
			vector<string>scenarios = vector<string>());
		vector<vector<FMTschedule>>readschedules(const string& primary_location,
			const vector<FMTmodel>& models);
        //void settransitionsNactions(vector<FMTaction>& actions,vector<FMTtransition>& Transitions);
        bool write(const FMTmodel& model,const string& folder);
    };
}

#endif // FMTmodelparser_H_INCLUDED
