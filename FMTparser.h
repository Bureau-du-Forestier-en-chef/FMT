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
#include "FMTbounds.h"
#include "FMTutility.h"
#include "FMTtheme.h"
#include "FMTyields.h"
#include "FMTaction.h"
#include "FMTconstants.h"
#include "FMTlayer.h"
#include <array>

#if defined (__CYGWIN__)
	#include "xlocale.h"
#endif

#ifdef FMTWITHGDAL
	#include "gdal.h"
	#include "gdal_priv.h"
	#include "ogrsf_frmts.h"
	#include "gdal_rat.h"
	#if defined (_MSC_VER)
		#include "cpl_conv.h"
	#endif
#endif

#include <boost/filesystem.hpp>

#include "FMTexception.h"


namespace WSParser
{


class FMTparser: public Core::FMTobject
    {
    private:
		std::regex rxnumber;
        std::regex rxremovecomment;
		std::regex rxvalid;
		std::regex rxinclude;
		std::regex rxfor;
		std::regex rxend;
        bool _incomment;
		std::queue<std::string>_forvalues;
		std::queue<std::string>_included;
		std::istream& safeGetline(std::istream& is, std::string& t) const;
    protected:
		std::regex rxayld;
		std::regex rxaage;
		std::regex rxoperators;
		std::regex rxprimary;
        mutable int _constreplacement;
        int _line;
		std::string _comment;
		std::string _location;
		static std::array<std::string, 5>baseoperators;
		static std::array<std::string, 21>baseextensions;
		#ifdef FMTWITHGDAL
			template<typename T>
			GDALDataset* createdataset(const std::string& location,const Spatial::FMTlayer<T>& layer, const GDALDataType datatype) const
				{
				const char *pszFormat = "GTiff";
				GDALDriver *poDriver;
				poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
				if( poDriver == nullptr )
					{
					_exhandler->raise(Exception::FMTexc::FMTinvaliddriver,_section,std::string(pszFormat), __LINE__, __FILE__);
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
					_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,_section,poDstDS->GetDescription(), __LINE__, __FILE__);
					}
				std::vector<double>geotrans = layer.getgeotransform();
				const std::string projection = layer.getprojection();
				poDstDS->SetProjection(projection.c_str());
				poDstDS->SetGeoTransform(&geotrans[0]);
				poDstDS->FlushCache();
				return poDstDS;
				}
			GDALDataset* getdataset(const std::string& location) const;
			GDALDataset* getvectordataset(const std::string& location) const;
			OGRLayer* getlayer(GDALDataset* dataset,int id) const;
			GDALRasterBand* getband(GDALDataset* dataset,int bandid=1) const;
			GDALRasterBand* createband(GDALDataset* dataset,const std::vector<std::string>& categories,int bandid=1) const;
			std::vector<std::string> getcat(GDALDataset* dataset,int bandid=1) const;
			GDALRasterBand* getoverview(GDALRasterBand* band,int view=1) const;
			void getWSfields(OGRLayer* layer, std::map<int,int>& themes,int& age,int& area,int& lock, std::string agefield="", std::string areafield="", std::string lockfield="") const;
		#endif
		std::vector<std::string> sameas(const std::string& allset) const;
		std::map<FMTwssect, std::string> getprimary(const std::string& primarylocation);
		bool isyld(const Core::FMTyields& ylds,const std::string& value,FMTwssect section) const;
        bool isact(FMTwssect section,const std::vector<Core::FMTaction>& actions, std::string action) const;
		std::string setspec(FMTwssect section,FMTwskwor key,const Core::FMTyields& ylds,const Core::FMTconstants& constants, Core::FMTspec& spec, const std::string& line);
        FMTwssect from_extension(const std::string& ext) const;
		std::vector<std::vector<std::string>>readcsv(const std::string& location,const char& separator);
    public:
		std::regex rxseparator;
        FMTparser();
        FMTparser(const FMTparser& rhs);
        FMTparser& operator = (const FMTparser& rhs);
        virtual ~FMTparser()=default;
        bool tryopening(const std::ifstream& stream, const std::string& location);
        bool tryopening(const std::ofstream& stream, const std::string& location);
		bool isvalidfile(const std::string& location) const;
        bool isvalid(std::string& line) const;
		std::vector<std::string>regexloop(std::regex& cutregex, std::string& str) const;
		std::vector<std::string>spliter(std::string strmask, std::regex& xspliter) const;
        void clearcomments(std::string& line);
		std::string upper(const std::string& lowercases) const;
		std::string getcleanline(std::ifstream& stream);
		std::queue<std::string> tryinclude(const std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons);
		std::string returninclude(const std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons);
		bool getforloops(std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons, std::vector<std::string>& allvalues, std::string& target);
		std::string getcleanlinewfor(std::ifstream& stream,const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& cons);
        bool isnum(const std::string& value) const;
        template<typename T>
        T getnum(const std::string& value,const Core::FMTconstants& constant, int period = 0) const
            {
            T nvalue = 0;
			const std::string newvalue = boost::erase_all_copy(value, ",");
            if (isnum(newvalue))
                {
                nvalue = getnum<T>(newvalue);
                }else if(constant.isconstant(newvalue))
                    {
                    nvalue= constant.get<T>(newvalue,period);
                    _exhandler->raise(Exception::FMTexc::WSconstants_replacement,_section,value +" line "+ std::to_string(_line), __LINE__, __FILE__);
                    ++_constreplacement;
                    }else{
                    _exhandler->raise(Exception::FMTexc::WSinvalid_number,_section,value +" line "+ std::to_string(_line), __LINE__, __FILE__);
                    }
            return nvalue;
            }
		template<typename T>
		T getnum(const std::string& value) const
			{
			T nvalue = 0;
			const std::string newvalue = boost::erase_all_copy(value, ",");
			if (isnum(newvalue))
				{
				nvalue = static_cast<T>(std::stod(newvalue));
			}else {
				_exhandler->raise(Exception::FMTexc::WSinvalid_number, _section, value + " line " + std::to_string(_line), __LINE__, __FILE__);
				}
			return nvalue;
			}
		template<typename T>
		bool tryfillnumber(T& number,const std::string& value, const Core::FMTconstants& constant, int period = 0) const
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
		Core::FMTbounds<T>bounds(const Core::FMTconstants& constants, const std::string& value, const std::string& ope,FMTwssect section) const
            {
            T lupper = std::numeric_limits<T>::max();
            T llower = std::numeric_limits<T>::min();
            T intvalue = getnum<T>(value,constants);
			const std::array<std::string, 5>::const_iterator it = std::find(baseoperators.begin(), baseoperators.end(),ope);
            const size_t optype = (it - baseoperators.begin());
            if(optype==0)
                {
                lupper = intvalue;
                llower = lupper;
                }else if(optype==1)
                    {
                    lupper = intvalue;
                    llower = std::numeric_limits<T>::min();
                    }else if(optype==2)
                        {
                        lupper = std::numeric_limits<T>::max();
                        llower = intvalue;
                        }
            return Core::FMTbounds<T>(section,lupper,llower);
            }
    };

}

#endif
