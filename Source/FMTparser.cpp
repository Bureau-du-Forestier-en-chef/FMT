/*
Copyright (c) 2019 Gouvernement du Québec 

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTparser.h"
#if (BOOST_VERSION / 100 % 1000) < 85
	#include <boost/filesystem/convenience.hpp>
#endif
#include <boost/icl/interval.hpp>
#include <boost/icl/interval_set.hpp>
#include <boost/algorithm/string/erase.hpp>
#include "FMTtheme.h"
#include "FMTyields.h"
#include "FMTaction.h"
#include "FMTbounds.hpp"
#include "FMTconstants.h"
#include "FMTlayer.hpp"
#include "FMTexceptionhandler.h"

#if defined FMTWITHGDAL
	#include "gdal_version.h"
	#include "ogr_srs_api.h"
	#include "gdal.h"
	#include "gdal_priv.h"
	#include "ogrsf_frmts.h"
	#include "gdal_rat.h"
	#if defined (_MSC_VER)
	#include "cpl_conv.h"
	#endif
	#include <cpl_string.h>
#endif 

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <stack>
#include <string>

namespace Parser
{

	const boost::regex Parser::FMTparser::rxvectortheme = boost::regex("^(THEME)([\\d]*)$");
	const boost::regex Parser::FMTparser::rxnumber = boost::regex("-?[\\d.,]+(?:E-?[\\d.,]+)?", boost::regex_constants::icase);
	const boost::regex Parser::FMTparser::rxremovecomment = boost::regex("^(.*?)([;]+.*)");
	const boost::regex Parser::FMTparser::rxvalid = boost::regex("^(?!\\s*$).+");
	const boost::regex Parser::FMTparser::rxinclude = boost::regex("^(\\*INCLUDE)([\\s\\t]*)(.+)");
	const boost::regex Parser::FMTparser::rxfor = boost::regex("^(FOREACH)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(IN)([\\s\\t]*)((\\([\\s\\t]*)(_TH)(\\d*)([\\s\\t]*\\([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*\\)[\\s\\t]*\\))|(\\([\\s\\t]*)(_TH)(\\d*)([\\s\\t]*\\))|(\\()(\\d*)(\\.\\.)(\\d*)(\\))|(\\()([^\\)]*)(\\)))|(\\bFOR\\b)([\\s\\t]*)([^\\:\\=]*)([\\:\\=\\s\\t]*)(\\d*)([\\s\\t]*)(TO)([\\s\\t]*)(\\d*)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex Parser::FMTparser::rxend = boost::regex("^(ENDFOR)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex Parser::FMTparser::rxayld = boost::regex("^(.+)(\\@YLD[\\s\\t]*\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|([-]*\\d*.[-]*\\d*))(\\.\\.)((#[^\\.]*)|([-]*\\d*.[-]*\\d*)|(_MAXAGE))(\\))(.+)|(.+)(\\@YLD[\\s\\t]*\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|([-]*\\d*)|([-]*\\d*.[-]*\\d*))(\\))(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex Parser::FMTparser::rxaage = boost::regex("^(.+)(\\@AGE[\\s\\t]*\\()([\\s\\t]*)((#[^\\.]*)|(\\d*)|(\\d*.\\d*))(\\.\\.)((#[^\\.]*)|(\\d*)|(\\d*.\\d*)|(_MAXAGE))(\\))(.+)|(.+)(\\@AGE[\\s\\t]*\\()([\\s\\t]*)((#[^\\.]*)|(\\d*)|(\\d*.\\d*))(\\))(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex Parser::FMTparser::rxayldage = boost::regex("^(.+)(\\@YLD[\\s\\t]*\\()([^,]*)([,])([^\\)]*)(\\))(.+)|^(.+)(\\@AGE[\\s\\t]*\\()([^\\)]*)(\\))(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex Parser::FMTparser::rxbounds = boost::regex("^(.+)(\\.\\.)(.+)|(.+)");
	const boost::regex Parser::FMTparser::rxoperators = boost::regex("([^\\+\\-\\/\\*]*)([\\+\\-\\/\\*]*)", boost::regex_constants::icase);
	const boost::regex Parser::FMTparser::rxprimary = boost::regex("^([^\\[]*)(\\[)([^\\]]*)(.+)");
	const boost::regex Parser::FMTparser::rxseparator = boost::regex("([\\s\\t]*)([^\\s\\t]*)");
	const boost::regex Parser::FMTparser::m_rxperiods = boost::regex("^([\\s\\t]*)((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*)|(_LENGTH))|(#.+|[\\d]*))", boost::regex_constants::ECMAScript | boost::regex_constants::icase);



	std::map<Core::FMTsection, std::string>FMTparser::primary_sections = std::map<Core::FMTsection, std::string>();

bool FMTparser::GDALinitialization = false;

Core::FMTsection FMTparser::from_extension(const std::string& ext) const
    {
	try {
		const std::array<std::string, 21>baseextensions = { ".run",".lan",".are",".act",".trn",".yld",".out",".opt",".con",".seq",".lif",
								"._lan","._are","._act","._trn","._yld","._out","._opt","._con","._seq","._lif" };
		const std::string lowercase = boost::to_lower_copy(ext);
		const std::array<std::string, 21>::const_iterator it = std::find(baseextensions.begin(), baseextensions.end(), lowercase);
		const size_t id = (it - baseextensions.begin());
		switch (id)
		{
		case 0:return Core::FMTsection::Control;
		case 1:return Core::FMTsection::Landscape;
		case 2:return Core::FMTsection::Area;
		case 3:return Core::FMTsection::Action;
		case 4:return Core::FMTsection::Transition;
		case 5:return Core::FMTsection::Yield;
		case 6:return Core::FMTsection::Outputs;
		case 7:return Core::FMTsection::Optimize;
		case 8:return Core::FMTsection::Constants;
		case 9:return Core::FMTsection::Schedule;
		case 10:return Core::FMTsection::Lifespan;
		case 11:return Core::FMTsection::Landscape;
		case 12:return Core::FMTsection::Area;
		case 13:return Core::FMTsection::Action;
		case 14:return Core::FMTsection::Transition;
		case 15:return Core::FMTsection::Yield;
		case 16:return Core::FMTsection::Outputs;
		case 17:return Core::FMTsection::Optimize;
		case 18:return Core::FMTsection::Constants;
		case 19:return Core::FMTsection::Schedule;
		case 20:return Core::FMTsection::Lifespan;
		default:      return Core::FMTsection::Empty;
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::from_extension", __LINE__, __FILE__);
	}
	return Core::FMTsection::Empty;
    }

#if defined FMTWITHGDAL
void FMTparser::initializeGDAL()
	{
	if (!GDALinitialization)
		{
		const std::string baseruntimelocation = getruntimelocation();
		const std::string  runtimelocation = baseruntimelocation + "\\GDAL_DATA";
		if (!boost::filesystem::is_directory(boost::filesystem::path(runtimelocation)))
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
				"Can not find GDAL_DATA at " + runtimelocation, "FMTparser::FMTparser()", __LINE__, __FILE__);
		}
		CPLSetConfigOption("GDAL_DATA", runtimelocation.c_str());
		//No need of drivers from shared library see : https://gdal.org/api/gdaldriver_cpp.html ; https://gdal.org/api/cpl.html ; https://trac.osgeo.org/gdal/wiki/ConfigOptions
		CPLSetConfigOption("GDAL_DRIVER_PATH", "");
		#if (GDAL_VERSION_MAJOR>=3)//Since GDAL 3.0
				const std::string  projruntimelocation = baseruntimelocation + "/proj";
				std::vector<const char*>projsearch;
				projsearch.push_back(projruntimelocation.c_str());
				projsearch.push_back(NULL);
				OSRSetPROJSearchPaths(&projsearch[0]);
		#endif
		GDALAllRegister();
		GDALinitialization = true;
		}
	}
std::vector<GDALDriver*> FMTparser::getallGDALdrivers(const char* spatialtype,bool testcreation) const
{
	GDALDriverManager* manager = GetGDALDriverManager();
	std::vector<GDALDriver*>drivers;
	for (int driverid = 0; driverid < manager->GetDriverCount(); ++driverid)
	{
		GDALDriver* driver = manager->GetDriver(driverid);
		if (driver != nullptr&&driver->GetMetadataItem(spatialtype) &&
			GDALGetDriverShortName(driver) != nullptr&&
			driver->GetMetadataItem(GDAL_DMD_EXTENSIONS) != nullptr&&
			driver->GetMetadataItem(GDAL_DCAP_OPEN) != nullptr&&
			(!testcreation || driver->GetMetadataItem(GDAL_DCAP_CREATE) != nullptr))
		{
			drivers.push_back(driver);
		}
	}
	return drivers;
}

std::vector< std::vector<std::string>>FMTparser::getGDALextensions(const char* spatialtype, bool testcreation) const
{
	std::vector< std::vector<std::string>>driversExtensions;
	for (GDALDriver* driver : getallGDALdrivers(spatialtype, testcreation))
	{
		std::vector<std::string>extensions;
		const std::string extension(driver->GetMetadataItem(GDAL_DMD_EXTENSIONS));
		std::vector<std::string> extensionsOf;
		boost::split(extensionsOf, extension, boost::is_any_of(" "), boost::algorithm::token_compress_on);
		for (const std::string& value : extensionsOf)
		{
			extensions.push_back(value);
		}

		driversExtensions.push_back(extensions);
	}
	return driversExtensions;
}

std::vector<std::string>FMTparser::getGDALvectordrivernames(bool testcreation) const
{
	std::vector<std::string>names;
	for (GDALDriver* driver : getallGDALdrivers(GDAL_DCAP_VECTOR, testcreation))
	{
		names.push_back(std::string(GDALGetDriverShortName(driver)));
	}
	return names;
}

std::vector<std::string>FMTparser::getGDALrasterdrivernames(bool testcreation) const
{
	std::vector<std::string>names;
	for (GDALDriver* driver : getallGDALdrivers(GDAL_DCAP_RASTER, testcreation))
	{
		names.push_back(std::string(GDALGetDriverShortName(driver)));
	}
	return names;
}
std::vector<std::vector<std::string>>FMTparser::getGDALvectordriverextensions(bool testcreation) const
{
	return getGDALextensions(GDAL_DCAP_VECTOR, testcreation);
}

std::vector<std::vector<std::string>>FMTparser::getGDALrasterdriverextensions(bool testcreation) const
{
	return getGDALextensions(GDAL_DCAP_RASTER, testcreation);
}

#endif


FMTparser::FMTparser() : Core::FMTobject(),
        _incomment(false),
         _forvalues(),
		_included(),
		mtx(),
		_section(Core::FMTsection::Empty),
		_constreplacement(0),
        _line(0),
		_comment(),
       _location(),
		mostrecentfile()
        {
		#ifdef FMTWITHGDAL
			initializeGDAL();
		#endif
        }

FMTparser::FMTparser(const FMTparser& rhs):
		Core::FMTobject(rhs),
         _incomment(rhs._incomment),
        _forvalues(rhs._forvalues),
		_included(rhs._included),
		mtx(),
		_section(rhs._section),
        _constreplacement(rhs._constreplacement),
        _line(rhs._line),
		_comment(rhs._comment),
        _location(rhs._location),
		mostrecentfile(rhs.mostrecentfile)
    {
		boost::lock(mtx, rhs.mtx);
    }

FMTparser& FMTparser::operator = (const FMTparser& rhs)
    {
        if (this!=&rhs)
            {
			boost::lock(mtx, rhs.mtx);
			boost::lock_guard<boost::recursive_mutex> self_lock(mtx, boost::adopt_lock);
			boost::lock_guard<boost::recursive_mutex> other_lock(rhs.mtx, boost::adopt_lock);
			Core::FMTobject::operator = (rhs);
            _incomment = rhs._incomment;
            _forvalues = rhs._forvalues;
			_included = rhs._included;
			_section = rhs._section;
            _constreplacement = rhs._constreplacement;
			_comment = rhs._comment;
            _line = rhs._line;
            _location = rhs._location;
			mostrecentfile = rhs.mostrecentfile;
            }
    return *this;
    }

void FMTparser::setheader(const std::string& header)
{
	_comment = header;
}

void FMTparser::setsection(const Core::FMTsection& section) const
{
	_section = section;
}


template<typename T>
T FMTparser::getnum(const std::string& value, const Core::FMTconstants& constant, int period) const
{
	T nvalue = 0;
	try {
		//const std::string newvalue = boost::erase_all_copy(value, ",");
		if (isnum(value/*newvalue*/))
		{
			nvalue = getnum<T>(value,true/*newvalue*/);
		}
		else if (constant.isconstant(value/*newvalue*/))
		{
			nvalue = constant.get<T>(value/*newvalue*/, period);
			_exhandler->raise(Exception::FMTexc::FMTconstants_replacement,
				value + " at line " + std::to_string(_line), "FMTparser::getnum", __LINE__, __FILE__, _section);
			++_constreplacement;
		}
		else {
			_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
				value + " at line " + std::to_string(_line), "FMTparser::getnum", __LINE__, __FILE__, _section);
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::getnum", __LINE__, __FILE__, _section);
	}
	return nvalue;
}

template int FMTparser::getnum<int>(const std::string& value, const Core::FMTconstants& constant, int period) const;
template double FMTparser::getnum<double>(const std::string& value, const Core::FMTconstants& constant, int period) const;
template size_t FMTparser::getnum<size_t>(const std::string& value, const Core::FMTconstants& constant, int period) const;

template<typename T>
T FMTparser::getnum(const std::string& value, bool omitnumtest) const
{
	T nvalue = 0;
	try {
		const std::string newvalue = boost::erase_all_copy(value, ",");
		if (omitnumtest||isnum(newvalue))
		{
			nvalue = static_cast<T>(std::stod(newvalue));
		}
		else {
			_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
				value + " at line " + std::to_string(_line), "FMTparser::getnum", __LINE__, __FILE__, _section);
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::getnum", __LINE__, __FILE__, _section);
	}
	return nvalue;
}

template int FMTparser::getnum<int>(const std::string& value, bool omitnumtest) const;
template double FMTparser::getnum<double>(const std::string& value, bool omitnumtest) const;
template size_t FMTparser::getnum<size_t>(const std::string& value, bool omitnumtest) const;

template<typename T>
bool FMTparser::tryfillnumber(T& number, const std::string& value, const Core::FMTconstants& constant, int period) const
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
template bool FMTparser::tryfillnumber<int>(int& number, const std::string& value, const Core::FMTconstants& constant, int period) const;
template bool FMTparser::tryfillnumber<double>(double& number, const std::string& value, const Core::FMTconstants& constant, int period) const;

template<typename T>
Core::FMTbounds<T> FMTparser::bounds(const Core::FMTconstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const
{
	T lupper = std::numeric_limits<T>::max();
	T llower = std::numeric_limits<T>::lowest();
	try {
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
				llower = std::numeric_limits<T>::lowest();
			}
			else if (optype == 2)
			{
				lupper = std::numeric_limits<T>::max();
				llower = intvalue;
			}
		}
		catch (...)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"for value " + value, "FMTparser::bounds", __LINE__, __FILE__, _section);
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::bounds", __LINE__, __FILE__, _section);
	}
	return Core::FMTbounds<T>(section, lupper, llower);
}

template Core::FMTbounds<double> FMTparser::bounds<double>(const Core::FMTconstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const;
template Core::FMTbounds<int> FMTparser::bounds<int>(const Core::FMTconstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const;



#ifdef FMTWITHGDAL

std::unique_ptr<OGRSpatialReference> FMTparser::getFORELspatialref() const
{
	std::unique_ptr<OGRSpatialReference>ptr(new OGRSpatialReference());
	ptr->importFromEPSG(32198);
	return std::move(ptr);
}


template<typename T>
GDALDataset* FMTparser::createdataset(const std::string& location, const Spatial::FMTlayer<T>& layer, const int datatypeid, std::string format) const
{
	GDALDataType datatype = static_cast<GDALDataType>(datatypeid);
	const char* pszFormat = format.c_str();
	GDALDriver *poDriver = nullptr;
	GDALDataset *poDstDS = nullptr;
	try {
		poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
		if (poDriver == nullptr)
		{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddriver,
				std::string(pszFormat), "FMTparser::createdataset", __LINE__, __FILE__, _section);
		}
		char **papszOptions = NULL;
		if (format== "GTiff")
		{
			papszOptions = CSLSetNameValue(papszOptions, "TILED", "YES");
			papszOptions = CSLSetNameValue(papszOptions, "BLOCKXSIZE", "128");
			papszOptions = CSLSetNameValue(papszOptions, "BLOCKYSIZE", "128");
			papszOptions = CSLSetNameValue(papszOptions, "COMPRESS", "LZW");
			papszOptions = CSLSetNameValue(papszOptions, "ZLEVEL", "9");
			papszOptions = CSLSetNameValue(papszOptions, "BIGTIFF", "YES");
		}else if (format=="BMP")
			{
			papszOptions = CSLSetNameValue(papszOptions, "WORLDFILE", "YES");
			}
		poDstDS = poDriver->Create(location.c_str(), layer.GetXSize(), layer.GetYSize(), 1, datatype, papszOptions);
		if (poDstDS == nullptr)
		{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddataset
				, poDstDS->GetDescription(), "FMTparser::createdataset", __LINE__, __FILE__, _section);
		}
		std::vector<double>geotrans = layer.getgeotransform();
		const std::string projection = layer.getprojection();
		poDstDS->SetProjection(projection.c_str());
		poDstDS->SetGeoTransform(&geotrans[0]);
		poDstDS->FlushCache();
		CSLDestroy(papszOptions);
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::createdataset", __LINE__, __FILE__);
	}
	return poDstDS;
}

template GDALDataset* FMTparser::createdataset<int>(const std::string& location, const Spatial::FMTlayer<int>& layer, const int datatypeid,std::string format) const;
template GDALDataset* FMTparser::createdataset<std::string>(const std::string& location, const Spatial::FMTlayer<std::string>& layer, const int datatypeid, std::string format) const;
template GDALDataset* FMTparser::createdataset<double>(const std::string& location, const Spatial::FMTlayer<double>& layer, const int datatypeid, std::string format) const;




GDALDataset* FMTparser::getdataset(const std::string& location) const
    {
	GDALDataset* data = nullptr;
	try{
	if (isvalidfile(location))
		{
		data = (GDALDataset*)GDALOpen(location.c_str(), GA_ReadOnly);
		if (data == nullptr)
			{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,
				data->GetDescription(),"FMTparser::getdataset", __LINE__, __FILE__,_section);
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::getdataset", __LINE__, __FILE__, _section);
	}
    return data;
    }

GDALRasterBand* FMTparser::getband(GDALDataset* dataset,int bandid) const
    {
	GDALRasterBand* band = nullptr;
	try{
		band = dataset->GetRasterBand(bandid);
		if (band == nullptr)
			{
			_exhandler->raise(Exception::FMTexc::FMTinvalidband,
				dataset->GetDescription(),"FMTparser::getband", __LINE__, __FILE__, _section);
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::getband", __LINE__, __FILE__, _section);
		}
    return band;
    }

void FMTparser::setcategories(GDALRasterBand* band,const std::vector<std::string>& categories) const
	{
	try {
		if (!categories.empty())
			{
			CPLStringList strlist;
			for (const std::string& value : categories)
				{
				strlist.AddString(value.c_str());
				}
			band->SetCategoryNames(strlist);
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::setcategories", __LINE__, __FILE__, _section);
		}

	}

GDALRasterBand* FMTparser::createband(GDALDataset* dataset,const std::vector<std::string>& categories,int bandid) const
    {
    GDALRasterBand* band = getband(dataset,bandid);
	try {
		double nodatavalue = -9999;
		if (band->GetRasterDataType()==GDALDataType::GDT_Byte)
		{
			nodatavalue = static_cast<double>(std::numeric_limits<uint8_t>::max());
		}
		band->SetNoDataValue(nodatavalue);
		band->Fill(nodatavalue);
		setcategories(band, categories);
		band->FlushCache();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTparser::createband", __LINE__, __FILE__, _section);
		}
    return band;
    }

std::vector<std::string>FMTparser::getcat(GDALDataset* dataset,int bandid) const
    {
	std::vector<std::string>values;
	try {
		char** meta = dataset->GetMetadata();
		GDALRasterBand* band = getband(dataset,bandid);
		char** names  = band->GetCategoryNames();
		int id = 0;
		while(names[id])
			{
			boost::to_upper(names[id]);
			values.push_back(names[id]);
			++id;
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTparser::getcat", __LINE__, __FILE__, _section);
		}
    return values;
    }

GDALRasterBand* FMTparser::getoverview(GDALRasterBand* band,int view) const
    {
	GDALRasterBand* overview=nullptr;
	try{
	overview = band->GetOverview(view);
    if (overview == nullptr)
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidoverview,band->GetDataset()->GetDescription(),
			"FMTparser::getoverview",__LINE__, __FILE__, _section);
        }
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::getoverview", __LINE__, __FILE__);
		}
    return overview;
    }

GDALDataset* FMTparser::getvectordataset(const std::string& location) const
    {
	GDALDataset* dataset = nullptr;
	try {
		if (isvalidfile(location))
		{
			dataset = (GDALDataset*)GDALOpenEx(location.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
			if (dataset == nullptr)
			{
				_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,
					dataset->GetDescription(),"FMTparser::getvectordataset", __LINE__, __FILE__, _section);
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTparser::getvectordataset", __LINE__, __FILE__, _section);
		}
    return dataset;
    }

GDALDataset* FMTparser::createvectormemoryds() const
	{
	GDALDataset* dataset = nullptr;
	try{
		dataset = createOGRdataset("Memoryds", "Memory");
		}
		catch (...)
			{
				_exhandler->raisefromcatch("","FMTparser::getemptymemoryds", __LINE__, __FILE__, _section);
			}
	return dataset;
	}

OGRCoordinateTransformation* FMTparser::getprojtransform(OGRLayer* baselayer, bool fittoforel) const
	{
	OGRCoordinateTransformation* coordtransf = nullptr;
	try {
		std::unique_ptr<OGRSpatialReference> forelspref = getFORELspatialref();
		OGRSpatialReference* lspref = baselayer->GetSpatialRef();
		if (fittoforel)
			{
			coordtransf = OGRCreateCoordinateTransformation(lspref, &*forelspref);
		}else {
			coordtransf = OGRCreateCoordinateTransformation(lspref,lspref);
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::getprojtransform", __LINE__, __FILE__, _section);
		}
	return coordtransf;
	}

GDALDataset* FMTparser::gettransformmemlayercopy(OGRLayer* baselayer, const OGRSpatialReference* newreference, const std::string& fieldname) const
	{
	GDALDataset* memds = nullptr;
	try {
		OGRwkbGeometryType lgeomtype = baselayer->GetGeomType();
		if (lgeomtype != wkbMultiPolygon && lgeomtype != wkbPolygon)
			{
			_exhandler->raise(Exception::FMTexc::FMTinvalidlayer,
				"Geometry type from layer is not valid, must be wkbMultiPolygon or wkbPolygon : " + std::to_string(lgeomtype),
				"FMTparser::gettransformmemlayercopy", __LINE__, __FILE__, _section);
			}
		memds = createvectormemoryds();
		std::unique_ptr<OGRSpatialReference>nonconstreference(new OGRSpatialReference(*newreference));
		OGRLayer* memlayer = memds->CreateLayer("Memlayer", nonconstreference.get(), lgeomtype, NULL);
		if (memlayer == NULL)
			{
			_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
				"Layer in memory", "FMTparser::gettransformmemlayercopy", __LINE__, __FILE__, _section);
			}
		OGRFieldDefn oField(fieldname.c_str(), OFTInteger);
		if (memlayer->CreateField(&oField) != OGRERR_NONE)
			{
			_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
				"Field definition", "FMTparser::gettransformmemlayercopy", __LINE__, __FILE__, _section);
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::gettransformmemlayercopy", __LINE__, __FILE__, _section);
		}
	return memds;
	}

GDALDataset* FMTparser::createOGRdataset(std::string location,
										std::string gdaldrivername) const
{
	GDALDataset* newdataset = nullptr;
	try {
		//GDALAllRegister();
		GDALDriver* newdriver = GetGDALDriverManager()->GetDriverByName(gdaldrivername.c_str());
		if (newdriver == NULL)
			{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddriver,
				gdaldrivername, "FMTparser::createOGRdataset", __LINE__, __FILE__, _section);
			}
		const boost::filesystem::path pathObj(location);
		if (boost::filesystem::exists(pathObj))
			{
			if (newdriver->Delete(location.c_str()) != 0)
				{
				_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,
					"Cannot delete " + location, "FMTparser::createOGRdataset", __LINE__, __FILE__, _section);
				}
			}
		newdataset = newdriver->Create(location.c_str(), 0, 0, 0, GDT_Unknown,NULL);
		if (newdataset == NULL)
		{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,
				"Can not create new dataset at " + location, "FMTparser::createOGRdataset", __LINE__, __FILE__, _section);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::createOGRdataset", __LINE__, __FILE__, _section);
		}
	return newdataset;
}

OGRLayer* FMTparser::createlayer(GDALDataset* dataset,
	const std::string& name, std::vector<std::string> creationoptions) const
{
	OGRLayer* newlayer = nullptr;
	try {
		std::vector<char*>optionsstrings;
		optionsstrings.reserve(creationoptions.size());
		for (std::string& value : creationoptions)
		{
			optionsstrings.push_back(&value[0]);
		}
		optionsstrings.push_back(nullptr);
		char** alloptions = &optionsstrings[0];
		newlayer = dataset->CreateLayer(name.c_str(), NULL, wkbNone, alloptions);
		if (newlayer == NULL)
		{
			_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
				"Cannote create new layer FMTresults for " + name, "FMTparser::createlayer", __LINE__, __FILE__, _section);
			//Non Valid Layer
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::createlayer", __LINE__, __FILE__);
	}
	return newlayer;
}


OGRLayer* FMTparser::getlayer(GDALDataset* dataset,int id) const
    {
	OGRLayer * layer = nullptr;
	try{
	layer = dataset->GetLayer(id);
    if( layer == nullptr )
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidlayer,dataset->GetDescription(),
			"FMTparser::getlayer",__LINE__, __FILE__, _section);
        }
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::getlayer", __LINE__, __FILE__, _section);
	}
    return layer;
    }

void FMTparser::getWSfields(OGRLayer* layer, std::map<int, int>& themes, int& age, int& area, int& lock, std::string agefield, std::string areafield, std::string lockfield) const
{
	try {
			OGRFeatureDefn *fdef = layer->GetLayerDefn();
			std::string capage = agefield;
			std::string caparea = areafield;
			std::string caplock = lockfield;
			boost::to_upper(capage);
			boost::to_upper(caparea);
			boost::to_upper(caplock);
			std::map<int, int>themes_fields;
			age = -1;
			area = -1;
			lock = -1;
			for (int iField = 0; iField < fdef->GetFieldCount(); ++iField)
			{
				OGRFieldDefn* fielddef = fdef->GetFieldDefn(iField);
				std::string fname = fielddef->GetNameRef();
				boost::to_upper(fname);
				boost::smatch kmatch;
				if (boost::regex_search(fname, kmatch,rxvectortheme)/*fname.find("THEME") != std::string::npos*/)
				{
					//fname.erase(0, 5);
					//themes[getnum<int>(fname) - 1] = iField;
					themes[getnum<int>(std::string(kmatch[2])) - 1] = iField;
				}
				else if (fname == caplock)
				{
					lock = iField;
				}
				else if (fname == capage)
				{
					age = iField;
				}
				else if (fname == caparea)
				{
					area = iField;
				}
			}
			if (themes.size() == 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
					layer->GetDescription(),"FMTparser::getWSfields", __LINE__, __FILE__, _section);
			}
			if (age == -1 && !agefield.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingfield, agefield + " " + layer->GetDescription(),
					"FMTparser::getWSfields",__LINE__, __FILE__, _section);
			}
			if (area == -1 && !areafield.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingfield, areafield + " " + layer->GetDescription(),
					"FMTparser::getWSfields", __LINE__, __FILE__, _section);
			}
			if (lock == -1 && !lockfield.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingfield, lockfield + " " + layer->GetDescription(),
					"FMTparser::getWSfields", __LINE__, __FILE__, _section);
			}
	}catch (...)
			{
			_exhandler->raisefromcatch("","FMTparser::getWSfields", __LINE__, __FILE__, _section);
			}
}

#endif

std::string FMTparser::setspecs(Core::FMTsection section, Core::FMTkwor key,const Core::FMTyields& ylds,const Core::FMTconstants& constants,std::vector<Core::FMTspec>& specs, const std::string& line)
    {
	std::string rest = "";
	try {
			boost::smatch kmatch;
			
			if (boost::regex_search(line, kmatch, rxayldage))
			{
				bool pushaagebound = false;
				std::string boundsmatch = std::string(kmatch[5])+std::string(kmatch[10]);
				boost::trim(boundsmatch);
				std::vector<std::string> strbounds;
				boost::split(strbounds,boundsmatch,boost::is_any_of(","),boost::algorithm::token_compress_on);
				std::string yld = std::string(kmatch[3]);
				boost::trim(yld);
				if(std::string(kmatch[9]).empty() && yld!="_AGE")
				{
					isyld(ylds, yld, section);
				}else{
					pushaagebound = true;	
				}
				boost::icl::interval_set <double> bounds;
				for (std::string& bound : strbounds)
				{
					boost::trim(bound);
					boost::smatch bmatch;
					double upperbound = std::numeric_limits<double>::max();
					double lowerbound = std::numeric_limits<double>::min();
					if (boost::regex_search(bound,bmatch,rxbounds))
					{
						std::string singlebound = std::string(bmatch[4]);
						if(singlebound.empty())
						{
							std::string strlower = std::string(bmatch[1]);
							boost::trim(strlower);
							lowerbound = getnum<double>(strlower, constants);
							std::string strupper = std::string(bmatch[3]);
							boost::trim(strupper);
							if( strupper != "_MAXAGE")
							{
								upperbound = getnum<double>(strupper, constants);						
							}
						}else{
							lowerbound =  getnum<double>(singlebound, constants);
							upperbound =  getnum<double>(singlebound, constants);	
						}
						bounds.insert(boost::icl::continuous_interval<double>::closed(lowerbound,upperbound));
					}
				}
				for(boost::icl::interval_set<double>::reverse_iterator rit = bounds.rbegin(); rit != bounds.rend(); ++rit)
				{
					Core::FMTspec newspec;
					if (pushaagebound)
					{
						const int intupper = (rit->upper() == std::numeric_limits<double>::max()) ? std::numeric_limits<int>::max() : static_cast<int>(rit->upper());
						const int intlower = (rit->lower() == std::numeric_limits<double>::min()) ? std::numeric_limits<int>::min() : static_cast<int>(rit->lower());
						newspec.addbounds(Core::FMTagebounds(section, key, intupper, intlower));
					}else {
						newspec.addbounds(Core::FMTyldbounds(section, key, yld, rit->upper(), rit->lower()));
						}
					specs.push_back(newspec);
				}
				rest = " "+std::string(kmatch[1])+std::string(kmatch[7])+std::string(kmatch[12]);
			}else{
				rest=line;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTparser::setspec", __LINE__, __FILE__, _section);
			}
    return rest;
    }

std::string FMTparser::setspec(Core::FMTsection section, Core::FMTkwor key,const Core::FMTyields& ylds,const Core::FMTconstants& constants,Core::FMTspec& spec, const std::string& line)
    {
	std::string rest = "";
	try {
			boost::smatch kmatch;
			
			if (boost::regex_search(line, kmatch, FMTparser::rxayld))
			{
				std::string yld = std::string(kmatch[4]) + std::string(kmatch[20]);
				boost::trim(yld);
				bool pushaagebound = false;
				if (yld=="_AGE")
					{
					pushaagebound = true;
					}else {
					isyld(ylds, yld, section);
					}
				const std::string singlebound = std::string(kmatch[21]) + std::string(kmatch[22]) + std::string(kmatch[23]);
				double upperbound = std::numeric_limits<double>::max();
				double lowerbound = std::numeric_limits<double>::min();
				if (singlebound.empty())
				{
					if (std::string(kmatch[14]).empty())
					{
						const std::string strupper = std::string(kmatch[12]) + std::string(kmatch[13]);
						upperbound = getnum<double>(strupper, constants);
					}
					const std::string strlower = std::string(kmatch[8]) + std::string(kmatch[9]);
					lowerbound = getnum<double>(strlower, constants);
				}
				else {
					lowerbound = getnum<double>(singlebound, constants);
				}
				
				if (pushaagebound)
					{
					spec.addbounds(Core::FMTagebounds(section, key, static_cast<int>(upperbound), static_cast<int>(lowerbound)));
				}else {
					spec.addbounds(Core::FMTyldbounds(section, key, yld, upperbound, lowerbound));
					}
				
				rest = " " + std::string(kmatch[1]) + std::string(kmatch[16]) + std::string(kmatch[28]);
			}
			else {
				rest = line;
			}

			if (boost::regex_search(rest, kmatch, FMTparser::rxaage))
			{
				std::string singlebound = std::string(kmatch[20]) + std::string(kmatch[21]) + std::string(kmatch[22]);
				int upperbound = std::numeric_limits<int>::max();
				int lowerbound = std::numeric_limits<int>::min();
				if (singlebound.empty())
				{
					if (std::string(kmatch[13]).empty())
					{
						std::string strupper = std::string(kmatch[10]) + std::string(kmatch[11]) + std::string(kmatch[12]);
						upperbound = getnum<int>(strupper, constants);
					}
					std::string strlower = std::string(kmatch[5]) + std::string(kmatch[6]) + std::string(kmatch[7]);
					lowerbound = getnum<int>(strlower, constants);
				}
				else {
					lowerbound = getnum<int>(singlebound, constants);
					upperbound = lowerbound;
				}
				spec.addbounds(Core::FMTagebounds(section, key, upperbound, lowerbound));
				rest = " " + std::string(kmatch[1]) + std::string(kmatch[15]) + std::string(kmatch[16]) + std::string(kmatch[24]);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTparser::setspec", __LINE__, __FILE__, _section);
			}
    return rest;
    }

bool FMTparser::isact(Core::FMTsection section,const std::vector<Core::FMTaction>& actions, std::string action) const
    {
	try{
    if (std::find_if(actions.begin(),actions.end(), Core::FMTactioncomparator(action,true))==actions.end())
        {
			_exhandler->raise(Exception::FMTexc::FMTignore, "No action named " + action + " at line " + std::to_string(_line), "FMTparser::isact", __LINE__, __FILE__, section);
        return false;
        }else
		{
        return true;
        }
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::isact", __LINE__, __FILE__, _section);
	}
	return false;
    }



bool FMTparser::isyld(const Core::FMTyields& p_ylds,
				const std::string& p_value, Core::FMTsection p_section,
				bool p_throwError) const
    {
	try{
    if (p_ylds.isyld(p_value))
        {
        return true;
        }
	if (p_throwError)
		{
		_exhandler->raise(Exception::FMTexc::FMTinvalid_yield, p_value + " at line " + std::to_string(_line),
			"FMTparser::isyld", __LINE__, __FILE__, p_section);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::isyld", __LINE__, __FILE__, _section);
		}
     return false;
    }


std::time_t FMTparser::getmostrecentfiletime() const
	{
	return mostrecentfile;
	}




bool FMTparser::tryopening(const std::ifstream& stream, const std::string& location) const
        {
		try{
        _location = location;
		const boost::filesystem::path extpath(location);
		std::string extension = extpath.extension().string();
        transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        this->setsection(from_extension(extension));
		_logger->logwithlevel("Reading " + location+"\n", 2);
		//*_logger << "extt " << _section << "\n";
        if (!stream.is_open())
            {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,location,
				"FMTparser::tryopening", __LINE__, __FILE__, _section);
            return false;
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::tryopening", __LINE__, __FILE__, _section);
		}
		boost::filesystem::path pathtofile(location);
		const std::time_t temp = boost::filesystem::last_write_time(pathtofile);
		if (std::difftime(mostrecentfile,temp)>0|| mostrecentfile ==0)
			{
			mostrecentfile = temp;
			}
        return true;
        }

bool FMTparser::tryopening(std::ofstream& stream, const std::string& location) const
        {
		try{
        _location = location;
        if (!stream.is_open())
            {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,location,
				"FMTparser::tryopening", __LINE__, __FILE__, _section);
            return false;
            }
		if (!_comment.empty())//If you find an header write it down
			{
			const std::string seperator = ";" + std::string(100, '-');
			stream << seperator << "\n";
			stream << ";" << _logger->getlogstamp() << "\n";
			std::vector<std::string>lines;
			boost::split(lines,_comment, boost::is_any_of("\n"), boost::token_compress_on);
			for (const std::string& line : lines)
				{
				if (!line.empty())
					{
					stream << ";" << line << "\n";
					}
				}
			stream << seperator << "\n";
			}
		
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTparser::tryopening", __LINE__, __FILE__, _section);
			}
        return true;
        }

bool FMTparser::isvalidfile(const std::string& location) const
	{
	try {
		const boost::filesystem::path pathObj(location);
		std::string extension = pathObj.extension().string();
		transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		if (!boost::filesystem::exists(pathObj) || !boost::filesystem::is_regular_file(pathObj))
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_path, location,
				"FMTparser::isvalidfile", __LINE__, __FILE__, _section);
			return false;
		}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::isvalidfile", __LINE__, __FILE__, _section);
		}
	return true;
	}

bool FMTparser::isnum(std::string value) const
    {
	//boost::erase_all(value, ",");
	return boost::regex_match(value,rxnumber);
    }


Core::FMTperbounds FMTparser::getPerBound(const std::string& p_lower,
	const std::string& p_upper,
	const Core::FMTconstants& p_constants) const
{
	Core::FMTperbounds bound;
	try {

		int startPeriod = std::numeric_limits<int>::max();
		if (p_lower != "_LENGTH")
		{
			startPeriod = getnum<int>(p_lower, p_constants);
		}
		int stopPeriod = startPeriod;
		if (!p_upper.empty() && p_upper != "_LENGTH")
		{
			stopPeriod = getnum<int>(p_upper, p_constants);
		}
		else if (p_upper == "_LENGTH")
		{
			stopPeriod = std::numeric_limits<int>::max();
		}
		bound = Core::FMTperbounds(Core::FMTsection::Optimize, stopPeriod, startPeriod);
	}
	catch (...)
	{
		_exhandler->raisefromcatch("",
			"FMTparser::getPerBound", __LINE__, __FILE__, _section);
	}
	return bound;
}

void FMTparser::setPeriodWithBounds(Core::FMTspec& p_spec,
	const std::string& p_lower,
	const std::string& p_upper,
	const Core::FMTconstants& p_constants) const
{
	const Core::FMTperbounds PERIOD_BOUND = getPerBound(p_lower, p_upper, p_constants);
	p_spec.setbounds(PERIOD_BOUND);
}

bool FMTparser::setPeriods(Core::FMTspec& p_spec,
	const std::string& p_periods,
	const Core::FMTconstants& p_constants) const
{
	bool isSet = false;
	try {
		boost::smatch theMatch;
		if (boost::regex_search(p_periods, theMatch, m_rxperiods))
		{
			const std::string JUST_LENGTH = std::string(theMatch[7]);
			const std::string LOWER_PERIOD = std::string(theMatch[4]) + std::string(theMatch[8]) + JUST_LENGTH;
			const std::string UPPER_PERIOD = std::string(theMatch[6]) + JUST_LENGTH;
			setPeriodWithBounds(p_spec, LOWER_PERIOD, UPPER_PERIOD, p_constants);
			isSet = true;
		}
	}catch (...)
	{
		_exhandler->raisefromcatch("for spec " + std::string(p_spec),
			"FMTparser::setPeriods", __LINE__, __FILE__, _section);
	}
	return isSet;
}

bool FMTparser::isnum(const std::string& value, const Core::FMTconstants& constant, bool throwerror) const
{
	try {
		if (!(isnum(value) || constant.isconstant(value)))
			{
			if (throwerror)
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalidyield_number, value,
					"FMTparser::isnum", __LINE__, __FILE__, _section);
				}
		}
		else {
			return true;
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::isnum", __LINE__, __FILE__, _section);
	}
	return false;
}



std::vector<std::string>FMTparser::regexloop(const boost::regex& cutregex, std::string& str) const
        {
		std::vector<std::string>result;
		try{
        auto els_begin = boost::sregex_iterator(str.begin(), str.end(), cutregex);
        auto els_end = boost::sregex_iterator();
        for (boost::sregex_iterator i = els_begin; i != els_end; ++i)
            {
			boost::smatch match = *i;
			std::string match_str = match.str();
            result.push_back(match_str);
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::regexloop", __LINE__, __FILE__, _section);
		}
        return result;
        }
    bool FMTparser::isvalid(const std::string& line) const
        {
        return boost::regex_match(line,rxvalid);
        }
    void FMTparser::clearcomments(std::string& line)
        {
		try {
			if (line.find(";")!=std::string::npos)
				{
				boost::smatch kmatch;
				if (boost::regex_search(line, kmatch, rxremovecomment))
				{
					_comment = std::string(kmatch[2]);
					boost::to_upper(_comment);
					line = std::string(kmatch[1]);
				}
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::clearcomments", __LINE__, __FILE__, _section);
		}
        }
	std::string FMTparser::upper(const std::string& lowercases) const
        {
		std::locale loc;
		std::string uppercases;
		try{
        for (std::string::size_type i=0;i<lowercases.size(); ++i)
            {
            uppercases+=toupper(lowercases[i],loc);
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::upper", __LINE__, __FILE__, _section);
		}
        return uppercases;
        }
	std::vector<std::string>FMTparser::spliter(std::string strmask,const boost::regex& xspliter) const
        {
		boost::smatch kmatch;
		std::string value;
		std::vector<std::string>vecmask;
		try{
        size_t endsize;
        while(!strmask.empty())
            {
			boost::regex_search(strmask,kmatch,xspliter);
            value = std::string(kmatch[2]);
            if (isvalid(value))
                {
                vecmask.push_back(value);
                }
            endsize = (std::string(kmatch[0]).size());
            strmask.erase(strmask.begin(),strmask.begin()+endsize);
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::spliter", __LINE__, __FILE__, _section);
		}
        return vecmask;
        }
	std::string FMTparser::getcleanline(std::ifstream& stream)
        {
        ++_line;
		std::string newline;
		std::string line;
		try{
        if (safeGetline(stream,line))
            {
			_comment.clear();
            clearcomments(line);
			//std::string fullline = newline;
           // newline = "";
            for(int loc = 0; loc < static_cast<int>(line.size()); ++loc)
                {
                char value = line.at(loc);
                if(_incomment)
                    {
                    if (value=='}')
                        {
                        _incomment = false;
                        }
                    }else{
                    if (value=='{')
                        {
                        _incomment = true;
                        }else{
                        newline+=toupper(value);
                        }
                    }
                }
			if (_incomment && !_comment.empty() && _comment.find('}') != std::string::npos)
				{
				_incomment = false;
				}
			if (!_comment.empty() && _comment.find('{') != std::string::npos)
				{
				_incomment = true;
				}
            }else{
            stream.close();
            }
        boost::trim(newline);
		if (newline.empty() && _incomment && !_comment.empty() && _comment.find('}') != std::string::npos &&
			(_comment.find('{') == std::string::npos || _comment.find('{') < _comment.find('}')))
			{
			_incomment = false;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTparser::getcleanline", __LINE__, __FILE__, _section);
			}
        return newline;
        }

bool FMTparser::getforloops(std::string& line,const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons, std::vector<std::string>& allvalues, std::string& target)
	{
	try{
	if (!boost::regex_match(line, rxfor))
		{
		return false;
		}
	boost::smatch kmatch;
	if (boost::regex_search(line, kmatch, rxfor))
	{
		target = std::string(kmatch[3]);
		if (!std::string(kmatch[24]).empty())
		{
			std::string val = std::string(kmatch[24]);
			boost::trim_if(val, boost::is_any_of(FMT_STR_SEPARATOR));
			const std::string forstrsep = FMT_STR_SEPARATOR + std::string(",");
			boost::split(allvalues, val, boost::is_any_of(forstrsep), boost::token_compress_on);
			if (val.find("..")!= std::string::npos)
				{
				std::vector<std::string>newvalues;
				for (const std::string& oldvalue: allvalues)
					{
					if (oldvalue.find("..") != std::string::npos)
						{
						std::vector<std::string>lowerNupper;
						boost::split(lowerNupper, oldvalue, boost::is_any_of(".."), boost::token_compress_on);
						const int lower = getnum<int>(lowerNupper.at(0), cons);
						const int upper = getnum<int>(lowerNupper.at(1), cons);
						for (int id = lower; id <= upper; ++id)
							{
							newvalues.push_back(std::to_string(id));
							}
						
					}else {
						newvalues.push_back(oldvalue);
						}
					}
				allvalues = newvalues;
				}
		}
		else if (!std::string(kmatch[16]).empty())
		{
			const int theme = getnum<int>(std::string(kmatch[16])) - 1;
			std::string att = "?";
			allvalues = themes[theme].getattributes(att);
		}
		else if (!std::string(kmatch[10]).empty())
		{
			const int theme = getnum<int>(std::string(kmatch[10])) - 1;
			std::string aggregate = kmatch[12];
			allvalues = themes[theme].getattributes(aggregate,true);
		}
		else if (!std::string(kmatch[19]).empty() || !std::string(kmatch[26]).empty())
		{
			if (!std::string(kmatch[28]).empty())
				{
				target = kmatch[28];
				}
			const std::string slower = std::string(kmatch[19]) + std::string(kmatch[30]);
			const std::string supper = std::string(kmatch[21]) + std::string(kmatch[34]);
			const int lower = getnum<int>(slower, cons);
			const int upper = getnum<int>(supper, cons);
			for (int id = lower; id <= upper; ++id)
			{
				allvalues.push_back(std::to_string(id));
			}
		}
		return true;
	}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTparser::getforloops", __LINE__, __FILE__, _section);
		}
	return false;
	}

std::queue<std::string> FMTparser::tryinclude(const std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons)
	{
	boost::smatch kmatch;
	std::queue<std::string>included_lines;
	try{
	if (boost::regex_match(line, FMTparser::rxinclude)&&
		boost::regex_search(line, kmatch, FMTparser::rxinclude))
		{
		std::string location = kmatch[3];
		boost::filesystem::path includedpath(location);
		if (location.find("_PRIMARY")!=std::string::npos)
			{
			if (primary_sections.find(_section)!= primary_sections.end())
				{
				location = primary_sections[_section];
				}
			}else if (includedpath.is_absolute())
				{
				const boost::filesystem::path l_ppath(_location);
				const boost::filesystem::path parent_path = l_ppath.parent_path();
				location = boost::filesystem::canonical(includedpath, parent_path).string();
			}else if (boost::starts_with(location, ".\\.\\"))
				{
				const boost::filesystem::path FORMATED_PATH(_location);
				const boost::filesystem::path PARENT_PATH = FORMATED_PATH.parent_path();
				boost::replace_all(location, ".\\.\\", PARENT_PATH.string() + "\\..\\..\\");
			}else if (boost::starts_with(location, "."))
					{
					const boost::filesystem::path l_ppath(_location);
					const boost::filesystem::path parent_path = l_ppath.parent_path();
					location.erase(0, 1);
					const boost::filesystem::path full_path = parent_path / location;
					location = full_path.string();
					}else if(location.find('/')==std::string::npos&&
						location.find('\\') == std::string::npos)
						{
						const boost::filesystem::path L_PPATH(_location);
						const boost::filesystem::path PARENT_PATH = L_PPATH.parent_path();
						const boost::filesystem::path FULL_PATH = PARENT_PATH / location;
						location = FULL_PATH.string();
						}
			const boost::filesystem::path basepath = boost::filesystem::path(location);
		if (!boost::filesystem::exists(location))//If does not exist then try with the _ type path.
			{
			const boost::filesystem::path llpath(_location);
			const boost::filesystem::path actllpath(location);
			const std::string extension = llpath.extension().string();
			std::string actualextension = actllpath.extension().string();
			if (extension.find("_")!=std::string::npos&&
				actualextension.find("_")==std::string::npos)
				{
				actualextension.erase(0, 1);
				const std::string newextension = "._" + actualextension;
				boost::filesystem::path full_path(location);
				full_path.replace_extension(newextension);
				location = full_path.string();
				}
			}
		if (!boost::filesystem::exists(location))//Look back to the original folder...
			{
			const boost::filesystem::path llpath(location);
			const boost::filesystem::path parentdir = llpath.parent_path();
			const boost::filesystem::path finalpath = parentdir / boost::filesystem::path("../..") / basepath.filename();
			location = finalpath.string();
			}

		FMTparser newparser;
		std::ifstream newstream(location);
		if (newparser.tryopening(newstream, location))
			{
			while (newstream.is_open())
				{
				const std::string newline = newparser.getcleanlinewfor(newstream,themes,cons);
				if (!newline.empty())
					{
					included_lines.push(newline);
					}
				}
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::tryinclude", __LINE__, __FILE__, _section);
	}
	return included_lines;
	}

std::string FMTparser::returninclude(const std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons)
	{
	std::string thereturn = "";
	try{
	if (!_included.empty())
		{
		thereturn = _included.front();
		_included.pop();
		}else{
			std::queue<std::string>inlines = tryinclude(line, themes, cons);
			if (!inlines.empty())
				{
				thereturn = inlines.front();
				inlines.pop();
				_included = inlines;
				}else {
				thereturn = line;
				}

			}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::returninclude", __LINE__, __FILE__, _section);
	}
	return thereturn;
	}

std::string FMTparser::getcleanlinewfor(std::ifstream& stream,const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& cons)
    {
	boost::smatch kmatch;
	std::string line;
	try {
		while (!_included.empty())
		{
			line = _included.front();
			_included.pop();
			return line;
		}
		if (_forvalues.empty())
		{
			std::map<std::string, std::vector<std::string>>loops_replaces;
			std::map<std::string, std::pair<int, int>>forlocation;
			int lineid = 0;
			std::stack<std::string>keys;
			std::vector<std::pair<int, std::string>>Sequence;
			std::vector<std::string>ordered_keys;
			do {

				line = getcleanline(stream);
				std::vector<std::string>allvalues;
				std::string target;
				bool gotfor = getforloops(line, themes, cons, allvalues, target);
				if (!gotfor && line.find("FOREACH") != std::string::npos)
				{

					do {
						line += getcleanline(stream);
					} while (line.find(")") == std::string::npos);
					gotfor = getforloops(line, themes, cons, allvalues, target);
				}
				if (gotfor)
				{
					loops_replaces[target] = allvalues;
					forlocation[target] = std::pair<int, int>(lineid, 0);
					keys.push(target);
				}
				else if (keys.empty())
				{
					return returninclude(line, themes, cons);
				}
				else if (boost::regex_search(line, kmatch, FMTparser::rxend))
				{
					std::string endfor = keys.top();
					keys.pop();
					forlocation[endfor].second = lineid;
					ordered_keys.push_back(endfor);
				}
				else if (isvalid(line)) {
					Sequence.push_back(std::pair<int, std::string>(lineid, line));
					++lineid;
				}else if (!stream.is_open() && !keys.empty())
					{
						_exhandler->raise(Exception::FMTexc::FMTunclosedforloop,
							"Missing EndFor after line " + std::to_string(_line), "FMTparser::getcleanlinewfor", __LINE__, __FILE__, _section);
						keys = std::stack<std::string>();
					}
			} while (!keys.empty());
			for (const std::string& key : ordered_keys)
			{
				std::vector<std::pair<int, std::string>>frontsequence;
				std::vector<std::pair<int, std::string>>backsequence;
				std::vector<std::pair<int, std::string>>process;
				for (std::pair<int, std::string> tline : Sequence)
				{
					if (tline.first < forlocation[key].first)
					{
						frontsequence.push_back(tline);
					}
					else if (tline.first >= forlocation[key].first && tline.first < forlocation[key].second)
					{
						process.push_back(tline);
					}
					else {
						backsequence.push_back(tline);
					}
				}
				std::vector<std::pair<int, std::string>>midsequence;
				for (const std::string& value : loops_replaces[key])
				{
					for (std::pair<int, std::string> tline : process)
					{
						midsequence.push_back(std::pair<int, std::string>(tline.first, boost::regex_replace(tline.second, boost::regex(key), value)));
					}
				}
				Sequence.clear();
				Sequence.insert(Sequence.end(), frontsequence.begin(), frontsequence.end());
				Sequence.insert(Sequence.end(), midsequence.begin(), midsequence.end());
				Sequence.insert(Sequence.end(), backsequence.begin(), backsequence.end());
			}
			for (const std::pair<int, std::string>& nline : Sequence)
			{
				_forvalues.push(nline.second);
			}
		}

		while (!_forvalues.empty())
		{
			line = _forvalues.front();
			_forvalues.pop();
			return returninclude(line, themes, cons);
		}
	return returninclude(line, themes, cons);
	}catch (...)
	 {
		 _exhandler->raisefromcatch("", "FMTparser::getcleanlinewfor", __LINE__, __FILE__, _section);
	 }
	return std::string();
    }

std::map<Core::FMTsection, std::string> FMTparser::getprimary(const std::string& primarylocation)
	{
		
		std::map<Core::FMTsection, std::string>targets;
		try {
			std::ifstream primarystream(primarylocation);
			const boost::filesystem::path primary_path(primarylocation);
			if (FMTparser::tryopening(primarystream, primarylocation))
			{
				while (primarystream.is_open())
				{
					std::string line = getcleanline(primarystream);
					if (isvalid(line))
					{
						boost::smatch kmatch;
						if (boost::regex_search(line, kmatch, rxprimary))
						{
							const std::string file_name = std::string(kmatch[3]);
							const boost::filesystem::path file_path(file_name);
							std::string extension = file_path.extension().string();
							boost::to_lower(extension);
							const Core::FMTsection section = from_extension(extension);
							targets[section] = (primary_path.parent_path() / file_path).string();
						}
					}
				}
			}
			if (targets.find(Core::FMTsection::Constants) == targets.end())
			{
				targets[Core::FMTsection::Constants] = "";
			}
		primary_sections = targets;
		}catch (...)
			{
			_exhandler->raisefromcatch("at "+ primarylocation,"FMTparser::getprimary", __LINE__, __FILE__, _section);
			}
		return targets;
	}

std::array<std::string, 5>FMTparser::getbaseoperators() const
	{
	return std::array<std::string, 5>({ "=", "<=", ">=", "<", ">" });
	}

std::vector<std::string> FMTparser::sameas(const std::string& allset) const
        {
		std::vector<std::string>all_elements;
		try{
		std::string separator = "_SAMEAS";
        if (allset.find(separator)!= std::string::npos)
            {
			std::string realname = allset.substr(0, allset.find(separator));
			boost::trim(realname);
			if (realname.empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Empty target "+allset, "FMTparser::sameas", __LINE__, __FILE__, _section);
				}
            all_elements.push_back(realname);
			std::string sameasstr = allset.substr(allset.find(separator)+separator.size());
            boost::trim(sameasstr);
            all_elements.push_back(sameasstr);
            }else{
			std::string realname = allset;
            boost::trim(realname);
            all_elements.push_back(realname);
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTparser::sameas", __LINE__, __FILE__, _section);
		}
        return all_elements;
        }

std::vector<std::vector<std::string>>FMTparser::readcsv(const std::string& location,const char& separator)
	{
	std::vector<std::vector<std::string>>lines;
	try {
		std::string regexstring = "(["+ std::string(1,separator) +"]*)([^"+ std::string(1,separator) +"]*)";
		boost::regex csvsplitregex(regexstring);
		if (!location.empty())
			{
				std::ifstream csvstream(location);
				std::string line;
				std::vector<std::string>splitted;
				bool gotsomething = true;
				Parser::FMTparser newParser;
				if (newParser.tryopening(csvstream, location))
					{
					//bool inactualdevs = false;
					while (gotsomething)
						{
						if (newParser.safeGetline(csvstream, line))
							{
							boost::trim(line);
							if (!line.empty())
								{
								lines.push_back(newParser.spliter(line, csvsplitregex));
								}
						}else {
							gotsomething = false;
						}
						}
					csvstream.close();
					}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("at " + location ,"FMTparser::readcsv", __LINE__, __FILE__, _section);
		}
	return lines;
	}




std::istream& FMTparser::safeGetline(std::istream& is, std::string& t) const
	{
	t.clear();
	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();
	try{
	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case std::streambuf::traits_type::eof():
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTparser::safeGetline", __LINE__, __FILE__, _section);
	}
	return is;
	}

}
