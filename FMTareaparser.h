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

#ifndef FMTareaparser_H_INCLUDED
#define FMTareaparser_H_INCLUDED

#include "FMTdevelopment.h"
#include "FMTparser.h"
#include "FMTlayer.h"
#include "FMTforest.h"
#include "FMTdisturbancestack.h"
#include "FMTGCBMtransition.h"
#include <iterator>
#include <boost/lexical_cast.hpp>

/*
#include "xlocale.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_rat.h"
*/

using namespace std;
using namespace Core;

namespace WSParser
{

class FMTareaparser : public FMTparser
    {
    private:
        regex rxcleanarea;
        void validate_raster(const vector<string>&data_rasters) const;
		string getdisturbancepath(const string& location, const int& period) const;
		vector<FMTGCBMtransition> getGCBMtransitions(const FMTlayer<string>& stacked_actions,
													const FMTlayer<int>& ages,
													const FMTforest& newfor,
													const vector<FMTtheme>& themes) const;

    public:
        FMTareaparser();
        FMTareaparser(const FMTareaparser& rhs);
        FMTareaparser& operator = (const FMTareaparser& rhs);
        vector<FMTactualdevelopment>read(const vector<FMTtheme>& themes,const FMTconstants& constants,string location);
        Spatial::FMTforest readrasters(const vector<FMTtheme>& themes,const vector<string>&data_rasters,
                             const string& age,double agefactor = 1.0,double areafactor = 0.0001,string lock = "") const;
        template<typename T>
        bool writelayer(const Spatial::FMTlayer<T>& layer,string location,const map<T,string>& mapping) const;
        bool writeforest(const Spatial::FMTforest& for_layer,
                         const vector<FMTtheme>& themes,
                         const vector<string>&data_rasters,
                         const string& age,
                         const string& lock,
                         vector<map<string,string>> mapping = vector<map<string,string>>()) const;
        /*bool writedisturbances(const string& location,
                               const Spatial::FMTdisturbancestack& disturbances,
                               const Spatial::FMTforest& for_layer,
                               map<string,string> mapping = map<string,string>()) const;*///add default argument to aim at at multiple themes for writing disturbances
		vector<FMTGCBMtransition> writedisturbances(const string& location,
								const Spatial::FMTdisturbancestack& disturbances,
								const Spatial::FMTforest& for_layer,
								const Spatial::FMTforest& out_layer,
								const vector<FMTtheme>& themes,
								map<string, string> mapping = map<string, string>()) const;

        vector<FMTactualdevelopment>readvectors(const vector<FMTtheme>& themes,const string& data_vectors,
                                   const string& agefield,const string& areafield,double agefactor = 1.0,
                                   double areafactor = 1,string lockfield = "",
								   double minimal_area = 0.0) const;

        bool write(const vector<FMTactualdevelopment>& areas, string location);
    };
}
#endif // FMTareaparser_H_INCLUDED
