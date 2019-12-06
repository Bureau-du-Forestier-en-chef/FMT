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

#ifndef FMTparser_H_INCLUDED
#define FMTparser_H_INCLUDED

#include <regex>
#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <list>
/*#include "FMTCore.h"*/
#include "FMTbounds.h"
#include "FMTutility.h"
#include "FMTtheme.h"
#include "FMTyields.h"
#include "FMTaction.h"
#include "FMTconstants.h"
#include "FMTlayer.h"

#if defined (__CYGWIN__)
	#include "xlocale.h"
#endif

#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_rat.h"
#include <boost/filesystem.hpp>

#if defined (_MSC_VER)
	#include "cpl_conv.h"
#endif


#include "FMTexception.h"

using namespace std;
using namespace Core;
using namespace Spatial;

namespace WSParser
{


class FMTparser: public FMTobject
    {
    private:
        regex rxremovecomment;
        regex rxvalid;
		regex rxinclude;
        regex rxfor;
        regex rxend;
        bool _incomment;
        queue<string>_forvalues;
		queue<string>_included;
	istream& safeGetline(istream& is, string& t) const;
    protected:
        regex rxayld;
        regex rxaage;
		regex rxoperators;
		regex rxprimary;
        int _constreplacement;
        int _line;
        /*FMTexceptionhandler* _exhandler;*/
		string _comment;
        string _location;
        FMTwssect _section;
        template<typename T>
        GDALDataset* createdataset(const string& location,const FMTlayer<T>& layer, const GDALDataType datatype) const
            {
            const char *pszFormat = "GTiff";
            GDALDriver *poDriver;
            //char **papszMetadata;
            poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
            if( poDriver == nullptr )
                {
                _exhandler->raise(FMTexc::FMTinvaliddriver,_section,string(pszFormat), __LINE__, __FILE__);
                }
            char **papszOptions = NULL;
            papszOptions = CSLSetNameValue( papszOptions, "TILED", "YES" );
            papszOptions = CSLSetNameValue( papszOptions, "BLOCKXSIZE", "128" );
            papszOptions = CSLSetNameValue( papszOptions, "BLOCKYSIZE", "128" );
            papszOptions = CSLSetNameValue( papszOptions, "COMPRESS", "LZW" );
            papszOptions = CSLSetNameValue( papszOptions, "ZLEVEL", "9" );
            papszOptions = CSLSetNameValue( papszOptions, "BIGTIFF", "YES" );
			GDALDataset *poDstDS;
			poDstDS  = poDriver->Create(location.c_str(), layer.GetXSize(), layer.GetYSize(), 1, datatype, papszOptions);
            if (poDstDS == nullptr)
                {
                _exhandler->raise(FMTexc::FMTinvaliddataset,_section,poDstDS->GetDescription(), __LINE__, __FILE__);
                }
            //setprojection and geotransform
            vector<double>geotrans = layer.getgeotransform();
            string projection = layer.getprojection();
			//string test = "C:/Mrnmicro/Applic/gdal-2.3.2/data";
			//CPLSetConfigOption("GDAL_DATA", test.c_str());
			//const char* pszOldValTmp = CPLGetConfigOption("GDAL_DATA", NULL);
			//OGRSpatialReference oSRS;
			//oSRS.importFromEPSG(32198);
			//char *pszSRS_WKT = NULL;
			//oSRS->SetWellKnownGeogCS("NAD83");
			//oSRS.exportToWkt(&pszSRS_WKT);
			poDstDS->SetProjection(projection.c_str());
            poDstDS->SetGeoTransform(&geotrans[0]);
			//char *conff = NULL;
			//CPLGetConfigOption("GDAL_DATA", conff);
			/*if (pszOldValTmp)
			{
				Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << pszOldValTmp << "\n";
			}*/


            poDstDS->FlushCache();
            /*GDALClose(poDstDS);
            GDALDataset* newds = (GDALDataset*)GDALOpen(location.c_str(), GA_Update);
            if (newds == nullptr)
                {
                _exhandler->raise(FMTexc::FMTinvaliddataset,_section,newds->GetDescription());
                }*/
            return poDstDS;
            }
		map<FMTwssect, string> getprimary(const string& primarylocation);
        GDALDataset* getdataset(const string& location) const;
        vector<string> sameas(const string& allset) const;
        GDALDataset* getvectordataset(const string& location) const;
        OGRLayer* getlayer(GDALDataset* dataset,int id) const;
        GDALRasterBand* getband(GDALDataset* dataset,int bandid=1) const;
        GDALRasterBand* createband(GDALDataset* dataset,const vector<string>& categories,int bandid=1) const;
        vector<string> getcat(GDALDataset* dataset,int bandid=1) const;
        GDALRasterBand* getoverview(GDALRasterBand* band,int view=1) const;
        void getWSfields(OGRLayer* layer,map<int,int>& themes,int& age,int& area,int& lock,string agefield="",string areafield="",string lockfield="") const;
        bool isyld(const FMTyields& ylds,const string& value,FMTwssect section);
        bool isact(FMTwssect section,const vector<FMTaction>& actions,string action);
        void build(FMTexceptionhandler* exhandler=nullptr);
        string setspec(FMTwssect section,FMTwskwor key,const FMTyields& ylds,const FMTconstants& constants,FMTspec& spec, const string& line);
        FMTwssect from_extension(const string& ext);
		vector<vector<string>>readcsv(const string& location,const char& separator);
    public:
        regex rxseparator;
        FMTparser();
        FMTparser(const FMTparser& rhs);
        FMTparser& operator = (const FMTparser& rhs);
        virtual ~FMTparser()=default;
        bool tryopening(const ifstream& stream, const string& location);
        bool tryopening(const ofstream& stream, const string& location);
		bool isvalidfile(const string& location) const;
		//bool isvalidfolder(const string& location) const;
        bool isvalid(string& line) const;
        vector<string>regexloop(regex& cutregex,string& str) const;
        vector<string>spliter(string strmask,regex& xspliter) const;
        void clearcomments(string& line);
        string upper(const string& lowercases) const;
        string getcleanline(ifstream& stream);
		queue<string> tryinclude(const string& line, const vector<FMTtheme>& themes, const FMTconstants& cons);
		string returninclude(const string& line, const vector<FMTtheme>& themes, const FMTconstants& cons);
		bool getforloops(string& line, const vector<FMTtheme>& themes, const FMTconstants& cons, vector<string>& allvalues, string& target);
        string getcleanlinewfor(ifstream& stream,const vector<FMTtheme>& themes,const FMTconstants& cons);
        bool isnum(const string& value) const;
		bool validate(const vector<FMTtheme>& themes,string& mask) const;
		bool checkmask(const vector<FMTtheme>& themes, const vector<string>& values,string& mask) const;
		//double getexactdouble(const string& value) const;
        template<typename T>
        T getnum(string value,const FMTconstants& constant, int period = 0)
            {
            T nvalue = 0;
            boost::erase_all(value, ",");
            if (isnum(value))
                {
                nvalue = T(stod(value));
                }else if(constant.isconstant(value))
                    {
                    nvalue= constant.get<T>(value,period);
                    _exhandler->raise(FMTexc::WSconstants_replacement,_section,value +" line "+to_string(_line), __LINE__, __FILE__);
                    ++_constreplacement;
                    }else{
                    _exhandler->raise(FMTexc::WSinvalid_number,_section,value +" line "+to_string(_line), __LINE__, __FILE__);
                    }
            return nvalue;
            }
		template<typename T>
		T getnum(string value)
			{
			T nvalue = 0;
			boost::erase_all(value, ",");
			if (isnum(value))
				{
				nvalue = T(stod(value));
			}else {
				_exhandler->raise(FMTexc::WSinvalid_number, _section, value + " line " + to_string(_line), __LINE__, __FILE__);
				}
			return nvalue;
			}
		template<typename T>
		bool tryfillnumber(T& number, string value, const FMTconstants& constant, int period = 0)
			{
			bool gotit = true;
			try {
				number = getnum<T>(value, constant, period);
			}
			catch (...) 
				{
				gotit = false;
				}
			return gotit;
			}
        template<typename T>
        FMTbounds<T>bounds(const FMTconstants& constants,string value,string ope,FMTwssect section)
            {
            T lupper = numeric_limits<T>::max();
            T llower = numeric_limits<T>::min();
            vector<string>operators = {"=","<=",">=","<",">"};
            T intvalue = getnum<T>(value,constants);
            vector<string>::iterator it = find(operators.begin(),operators.end(),ope);
            size_t optype = (it - operators.begin());
            if(optype==0)
                {
                lupper = intvalue;
                llower = lupper;
                }else if(optype==1)
                    {
                    lupper = intvalue;
                    llower = numeric_limits<T>::min();
                    }else if(optype==2)
                        {
                        lupper = numeric_limits<T>::max();
                        llower = intvalue;
                        }
            return FMTbounds<T>(section,lupper,llower);
            }
    };

}

#endif
