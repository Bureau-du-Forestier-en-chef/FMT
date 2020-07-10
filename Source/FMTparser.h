/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
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

#include "FMTobject.h"


namespace Parser
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
        mutable int _line;
		std::string _comment;
		mutable std::string _location;
		#ifdef FMTWITHGDAL
			template<typename T>
			GDALDataset* createdataset(const std::string& location,const Spatial::FMTlayer<T>& layer, const GDALDataType datatype) const
				{
				const char *pszFormat = "GTiff";
				GDALDriver *poDriver;
				poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
				if( poDriver == nullptr )
					{
					_exhandler->raise(Exception::FMTexc::FMTinvaliddriver,
						std::string(pszFormat),"FMTparser::createdataset", __LINE__, __FILE__, _section);
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
					_exhandler->raise(Exception::FMTexc::FMTinvaliddataset
						,poDstDS->GetDescription(),"FMTparser::createdataset", __LINE__, __FILE__, _section);
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
		std::array<std::string, 5>getbaseoperators() const;
		std::vector<std::string> sameas(const std::string& allset) const;
		std::map<Core::FMTsection, std::string> getprimary(const std::string& primarylocation);
		bool isyld(const Core::FMTyields& ylds,const std::string& value, Core::FMTsection section) const;
        bool isact(Core::FMTsection section,const std::vector<Core::FMTaction>& actions, std::string action) const;
		std::string setspec(Core::FMTsection section, Core::FMTkwor key,const Core::FMTyields& ylds,const Core::FMTconstants& constants, Core::FMTspec& spec, const std::string& line);
		Core::FMTsection from_extension(const std::string& ext) const;
		std::vector<std::vector<std::string>>readcsv(const std::string& location,const char& separator);
    public:
		std::regex rxseparator;
        FMTparser();
        FMTparser(const FMTparser& rhs);
        FMTparser& operator = (const FMTparser& rhs);
        virtual ~FMTparser()=default;
        bool tryopening(const std::ifstream& stream, const std::string& location) const;
        bool tryopening(const std::ofstream& stream, const std::string& location) const;
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
        bool isnum(std::string value) const;
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
                    _exhandler->raise(Exception::FMTexc::FMTconstants_replacement,
						value +"at line "+ std::to_string(_line),"FMTparser::getnum", __LINE__, __FILE__, _section);
                    ++_constreplacement;
                    }else{
                    _exhandler->raise(Exception::FMTexc::FMTinvalid_number,
						value +"at line "+ std::to_string(_line), "FMTparser::getnum", __LINE__, __FILE__, _section);
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
				_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
					value + "at line " + std::to_string(_line),"FMTparser::getnum", __LINE__, __FILE__, _section);
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
		Core::FMTbounds<T>bounds(const Core::FMTconstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const
            {
				T lupper = std::numeric_limits<T>::max();
				T llower = std::numeric_limits<T>::min();
				try {
				T intvalue = getnum<T>(value, constants);
				const std::array<std::string, 5> baseoperators = this->getbaseoperators();
				const std::array<std::string, 5>::const_iterator it = std::find(baseoperators.begin(), baseoperators.end(), ope);
				const size_t optype = (it - baseoperators.begin());
				if (optype == 0)
				{
					lupper = intvalue;
					llower = lupper;
				}
				else if (optype == 1)
				{
					lupper = intvalue;
					llower = std::numeric_limits<T>::min();
				}
				else if (optype == 2)
				{
					lupper = std::numeric_limits<T>::max();
					llower = intvalue;
				}
			}catch (...)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"for value "+value,"FMTparser::bounds", __LINE__, __FILE__, _section);
				}
            return Core::FMTbounds<T>(section,lupper,llower);
            }
    };

}

#endif
