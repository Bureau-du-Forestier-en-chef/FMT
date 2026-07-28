/*
Copyright (c) 2019 Gouvernement du Québec 

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTParser.h"
#if (BOOST_VERSION / 100 % 1000) < 85
	#include <boost/filesystem/convenience.hpp>
#endif
#include <boost/icl/interval.hpp>
#include <boost/icl/interval_set.hpp>
#include <boost/algorithm/string/erase.hpp>
#include "FMTTheme.h"
#include "FMTYields.h"
#include "FMTAction.h"
#include "FMTBounds.hpp"
#include "FMTConstants.h"
#include "FMTLayer.hpp"
#include "FMTExceptionHandler.h"

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

	FMTParser::FMTLineInfo::FMTLineInfo(const std::string p_line, int p_number, const std::string& p_file):
		m_lineValue(p_line),m_lineNumber(p_number),m_file(p_file)
		{

		}

	const boost::regex Parser::FMTParser::m_VECTOR_THEME = boost::regex("^(THEME)([\\d]*)$");
	const boost::regex Parser::FMTParser::m_NUMBER = boost::regex("-?[\\d.,]+(?:E-?[\\d.,]+)?", boost::regex_constants::icase);
	const boost::regex Parser::FMTParser::m_REMOVE_COMMENT = boost::regex("^(.*?)([;]+.*)");
	const boost::regex Parser::FMTParser::m_VALID = boost::regex("^(?!\\s*$).+");
	const boost::regex Parser::FMTParser::m_INCLUDE = boost::regex("^(\\*INCLUDE)([\\s\\t]*)(.+)");
	// regex de foreeach ici?
	const boost::regex Parser::FMTParser::m_FOR = boost::regex(
		"^(FOREACH)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(IN)([\\s\\t]*)((\\([\\s\\t]*)(_TH)(\\d*)([\\s\\t]*\\([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*\\)[\\s\\t]*\\))|"//thematic for each
		"(\\([\\s\\t]*)(_TH)(\\d*)([\\s\\t]*\\))|(\\()(\\d*)(\\.\\.)(\\d*)(\\))|(\\()([^\\)]*)(\\)))|(\\bFOR\\b)([\\s\\t]*)([^\\:\\=]*)([\\:\\=\\s\\t]*)(\\d*)([\\s\\t]*)(TO)([\\s\\t]*)(\\d*)|"//regular for loops
		"(FOREACH)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(IN)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(\\bQUERY\\b)([\\s\\t]*)(\\()(.*)(\\))"//query for loops match 35 indicate we got a query, 41 is the path to the file, 46 is the query
		,boost::regex_constants::ECMAScript | boost::regex_constants::icase | boost::regex_constants::match_not_dot_newline);
	const boost::regex Parser::FMTParser::m_END = boost::regex("^(ENDFOR)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex Parser::FMTParser::m_AYLD = boost::regex("^(.+)(\\@YLD[\\s\\t]*\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|([-]*\\d*.[-]*\\d*))(\\.\\.)((#[^\\.]*)|"
		"([-]*\\d*.[-]*\\d*)|(_MAXAGE))(\\))(.+)|"
		"(.+)(\\@YLD[\\s\\t]*\\()([\\s\\t]*)(.+)(\\,)([\\s\\t]*)((#[^\\.]*)|"
		"([-]*\\d*)|([-]*\\d*.[-]*\\d*))(\\))(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex Parser::FMTParser::m_AAGE = boost::regex("^(.+)(\\@AGE[\\s\\t]*\\()([\\s\\t]*)((#[^\\.]*)|(\\d*)|"
		"(\\d*.\\d*))(\\.\\.)((#[^\\.]*)|(\\d*)|(\\d*.\\d*)|(_MAXAGE))(\\))(.+)|"
		"(. + )(\\@AGE[\\s\\t] * \\()([\\s\\t] *)((#[^ \\.] *) | (\\d*) | (\\d * .\\d*))(\\))(. + )",
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex Parser::FMTParser::m_AYLD_AGE = boost::regex(
		"^(.+)(\\@YLD[\\s\\t]*\\()([^,]*)([,])([^\\)]*)(\\))(.+)|"
		"^(.+)(\\@AGE[\\s\\t]*\\()([^\\)]*)(\\))(.+)",
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex Parser::FMTParser::m_BOUNDS = boost::regex("^(.+)(\\.\\.)(.+)|(.+)");
	const boost::regex Parser::FMTParser::m_OPERATORS = boost::regex("([^\\+\\-\\/\\*]*)([\\+\\-\\/\\*]*)", boost::regex_constants::icase);
	const boost::regex Parser::FMTParser::m_PRIMARY = boost::regex("^([^\\[]*)(\\[)([^\\]]*)(.+)");
	const boost::regex Parser::FMTParser::m_SEPARATOR = boost::regex("([\\s\\t]*)([^\\s\\t]*)");
	const boost::regex Parser::FMTParser::m_PERIODS = boost::regex(
		"^([\\s\\t]*)((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*)|(_LENGTH))|(#.+|[\\d]*))",
		boost::regex_constants::ECMAScript | boost::regex_constants::icase);



	std::map<Core::FMTsection, std::string>FMTParser::primarym_sections = std::map<Core::FMTsection, std::string>();

bool FMTParser::gdalInitialization = false;

Core::FMTsection FMTParser::fromExtension(const std::string& ext)
    {
	try {
		const std::array<std::string, 21>BASE_EXTENSIONS = { ".run",".lan",".are",".act",".trn",".yld",".out",".opt",".con",".seq",".lif",
								"._lan","._are","._act","._trn","._yld","._out","._opt","._con","._seq","._lif" };
		const std::string lowercase = boost::to_lower_copy(ext);
		const std::array<std::string, 21>::const_iterator it = std::find(BASE_EXTENSIONS.begin(), BASE_EXTENSIONS.end(), lowercase);
		const size_t id = (it - BASE_EXTENSIONS.begin());
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
		_exhandler->raiseFromCatch("", "FMTParser::fromExtension", __LINE__, __FILE__);
	}
	return Core::FMTsection::Empty;
    }

std::string FMTParser::createSubDirectory(const std::string& p_Directory,
										const std::string& p_SubDirectory)
{
	std::string fullPath;
	try {
		const boost::filesystem::path BASE(p_Directory);
		if (!boost::filesystem::is_directory(BASE))
			{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
				p_Directory, "FMTParser::createSubDirectory", __LINE__, __FILE__);
			}
		const boost::filesystem::path SUB(p_SubDirectory);
		const boost::filesystem::path FINAL = BASE / SUB;
		if (!boost::filesystem::is_directory(FINAL))
			{
			boost::filesystem::create_directory(FINAL);
			}
		fullPath = FINAL.string();
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTParser::createSubDirectory", __LINE__, __FILE__);
		}
	return fullPath;
}

#if defined FMTWITHGDAL
void FMTParser::_initializeGDAL()
	{
	if (!gdalInitialization)
		{
		const std::string baseruntimelocation = getRuntimeLocation();
		const std::string  runtimelocation = baseruntimelocation + "\\GDAL_DATA";
		if (!boost::filesystem::is_directory(boost::filesystem::path(runtimelocation)))
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
				"Can not find GDAL_DATA at " + runtimelocation, "FMTParser::FMTParser()", __LINE__, __FILE__);
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
		gdalInitialization = true;
		}
	}
std::vector<GDALDriver*> FMTParser::_getAllGDALDrivers(const char* spatialtype,bool testcreation) const
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

std::vector< std::vector<std::string>>FMTParser::getGDALExtensions(const char* spatialtype, bool testcreation) const
{
	std::vector< std::vector<std::string>>driversExtensions;
	for (GDALDriver* driver : _getAllGDALDrivers(spatialtype, testcreation))
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

std::vector<std::string>FMTParser::getGDALVectorDriverNames(bool testcreation) const
{
	std::vector<std::string>names;
	for (GDALDriver* driver : _getAllGDALDrivers(GDAL_DCAP_VECTOR, testcreation))
	{
		names.push_back(std::string(GDALGetDriverShortName(driver)));
	}
	return names;
}

std::vector<std::string>FMTParser::getGDALRasterDriverNames(bool testcreation) const
{
	std::vector<std::string>names;
	for (GDALDriver* driver : _getAllGDALDrivers(GDAL_DCAP_RASTER, testcreation))
	{
		names.push_back(std::string(GDALGetDriverShortName(driver)));
	}
	return names;
}
std::vector<std::vector<std::string>>FMTParser::getGDALVectorDriverExtensions(bool testcreation) const
{
	return getGDALExtensions(GDAL_DCAP_VECTOR, testcreation);
}

std::vector<std::vector<std::string>>FMTParser::getGDALRasterDriverExtensions(bool testcreation) const
{
	return getGDALExtensions(GDAL_DCAP_RASTER, testcreation);
}

#endif


FMTParser::FMTParser() : Core::FMTObject(),
        m_inComment(false),
        //m_ForValues(),
		//m_included(),
		m_MTX(),
		m_section(Core::FMTsection::Empty),
		m_constrePlacement(0),
        m_line(0),
		m_comment(),
       m_location(),
		m_MostRecentFile()
        {
		#ifdef FMTWITHGDAL
			_initializeGDAL();
		#endif
        }

FMTParser::FMTParser(const FMTParser& rhs):
		Core::FMTObject(rhs),
         m_inComment(rhs.m_inComment),
        //m_ForValues(rhs.m_ForValues),
		//m_included(rhs.m_included),
		m_MTX(),
		m_section(rhs.m_section),
        m_constrePlacement(rhs.m_constrePlacement),
        m_line(rhs.m_line),
		m_comment(rhs.m_comment),
        m_location(rhs.m_location),
		m_MostRecentFile(rhs.m_MostRecentFile)
    {
		boost::lock(m_MTX, rhs.m_MTX);
    }

FMTParser& FMTParser::operator = (const FMTParser& rhs)
    {
        if (this!=&rhs)
            {
			boost::lock(m_MTX, rhs.m_MTX);
			boost::lock_guard<boost::recursive_mutex> self_lock(m_MTX, boost::adopt_lock);
			boost::lock_guard<boost::recursive_mutex> other_lock(rhs.m_MTX, boost::adopt_lock);
			Core::FMTObject::operator = (rhs);
            m_inComment = rhs.m_inComment;
            //m_ForValues = rhs.m_ForValues;
			//m_included = rhs.m_included;
			m_section = rhs.m_section;
            m_constrePlacement = rhs.m_constrePlacement;
			m_comment = rhs.m_comment;
            m_line = rhs.m_line;
            m_location = rhs.m_location;
			m_MostRecentFile = rhs.m_MostRecentFile;
            }
    return *this;
    }

void FMTParser::setHeader(const std::string& header)
{
	m_comment = header;
}

void FMTParser::_setSection(const Core::FMTsection& section) const
{
	m_section = section;
}


template<typename T>
T FMTParser::_getNum(const std::string& value, const Core::FMTConstants& constant, int period) const
{
	T nvalue = 0;
	try {
		//const std::string newvalue = boost::erase_all_copy(value, ",");
		if (_isNum(value/*newvalue*/))
		{
			nvalue = _getNum<T>(value,true/*newvalue*/);
		}
		else if (constant.isConstant(value/*newvalue*/))
		{
			nvalue = constant.get<T>(value/*newvalue*/, period);
			_exhandler->raise(Exception::FMTexc::FMTconstants_replacement,
				value + " at line " + std::to_string(m_line) + " in " + m_location,
				"FMTParser::_getNum", __LINE__, __FILE__, m_section);
			++m_constrePlacement;
		}
		else {
			_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
				value + " at line " + std::to_string(m_line) + " in " + m_location,
				"FMTParser::_getNum", __LINE__, __FILE__, m_section);
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::_getNum", __LINE__, __FILE__, m_section);
	}
	return nvalue;
}

template int FMTParser::_getNum<int>(const std::string& value, const Core::FMTConstants& constant, int period) const;
template double FMTParser::_getNum<double>(const std::string& value, const Core::FMTConstants& constant, int period) const;
template size_t FMTParser::_getNum<size_t>(const std::string& value, const Core::FMTConstants& constant, int period) const;

template<typename T>
T FMTParser::_getNum(const std::string& value, bool omitnumtest) const
{
	T nvalue = 0;
	try {
		const std::string newvalue = boost::erase_all_copy(value, ",");
		if (omitnumtest||_isNum(newvalue))
		{
			nvalue = static_cast<T>(std::stod(newvalue));
		}
		else {
			_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
				value + " at line " + std::to_string(m_line) + " in " + m_location,
				"FMTParser::_getNum", __LINE__, __FILE__, m_section);
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::_getNum", __LINE__, __FILE__, m_section);
	}
	return nvalue;
}

template int FMTParser::_getNum<int>(const std::string& value, bool omitnumtest) const;
template double FMTParser::_getNum<double>(const std::string& value, bool omitnumtest) const;
template size_t FMTParser::_getNum<size_t>(const std::string& value, bool omitnumtest) const;

template<typename T>
bool FMTParser::_tryFillNumber(T& number, const std::string& value, const Core::FMTConstants& constant, int period) const
{
	bool gotit = true;
	try {
		number = _getNum<T>(value, constant, period);
	}
	catch (...)
	{
		gotit = false;
	}
	return gotit;
}
template bool FMTParser::_tryFillNumber<int>(int& number, const std::string& value, const Core::FMTConstants& constant, int period) const;
template bool FMTParser::_tryFillNumber<double>(double& number, const std::string& value, const Core::FMTConstants& constant, int period) const;

template<typename T>
Core::FMTBounds<T> FMTParser::bounds(const Core::FMTConstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const
{
	T lupper = std::numeric_limits<T>::max();
	T llower = std::numeric_limits<T>::lowest();
	try {
		try {
			T intvalue = _getNum<T>(value, constants);
			const std::array<std::string, 5> baseoperators = this->getBaseOperators();
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
				"for value " + value, "FMTParser::bounds", __LINE__, __FILE__, m_section);
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::bounds", __LINE__, __FILE__, m_section);
	}
	return Core::FMTBounds<T>(section, lupper, llower);
}

template Core::FMTBounds<double> FMTParser::bounds<double>(const Core::FMTConstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const;
template Core::FMTBounds<int> FMTParser::bounds<int>(const Core::FMTConstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const;



#ifdef FMTWITHGDAL

std::unique_ptr<OGRSpatialReference> FMTParser::_getFORELSpatialRef() const
{
	std::unique_ptr<OGRSpatialReference>ptr(new OGRSpatialReference());
	ptr->importFromEPSG(32198);
	return std::move(ptr);
}


template<typename T>
GDALDataset* FMTParser::_createDataset(const std::string& location, const Spatial::FMTLayer<T>& layer, const int datatypeid, std::string format) const
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
				std::string(pszFormat), "FMTParser::_createDataset", __LINE__, __FILE__, m_section);
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
		poDstDS = poDriver->Create(location.c_str(), layer.getXSize(), layer.getYSize(), 1, datatype, papszOptions);
		if (poDstDS == nullptr)
		{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddataset
				, poDstDS->GetDescription(), "FMTParser::_createDataset", __LINE__, __FILE__, m_section);
		}
		std::vector<double>geotrans = layer.getGeoTransform();
		const std::string projection = layer.getProjection();
		poDstDS->SetProjection(projection.c_str());
		poDstDS->SetGeoTransform(&geotrans[0]);
		poDstDS->FlushCache();
		CSLDestroy(papszOptions);
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::_createDataset", __LINE__, __FILE__);
	}
	return poDstDS;
}

template GDALDataset* FMTParser::_createDataset<int>(const std::string& location, const Spatial::FMTLayer<int>& layer, const int datatypeid,std::string format) const;
template GDALDataset* FMTParser::_createDataset<std::string>(const std::string& location, const Spatial::FMTLayer<std::string>& layer, const int datatypeid, std::string format) const;
template GDALDataset* FMTParser::_createDataset<double>(const std::string& location, const Spatial::FMTLayer<double>& layer, const int datatypeid, std::string format) const;




GDALDataset* FMTParser::_getDataset(const std::string& location) const
    {
	GDALDataset* data = nullptr;
	try{
	if (_isValidFile(location))
		{
		data = (GDALDataset*)GDALOpen(location.c_str(), GA_ReadOnly);
		if (data == nullptr)
			{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,
				data->GetDescription(),"FMTParser::_getDataset", __LINE__, __FILE__,m_section);
			}
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::_getDataset", __LINE__, __FILE__, m_section);
	}
    return data;
    }

GDALRasterBand* FMTParser::_getBand(GDALDataset* dataset,int bandid) const
    {
	GDALRasterBand* band = nullptr;
	try{
		band = dataset->GetRasterBand(bandid);
		if (band == nullptr)
			{
			_exhandler->raise(Exception::FMTexc::FMTinvalidband,
				dataset->GetDescription(),"FMTParser::_getBand", __LINE__, __FILE__, m_section);
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTParser::_getBand", __LINE__, __FILE__, m_section);
		}
    return band;
    }

void FMTParser::_setCategories(GDALRasterBand* band,const std::vector<std::string>& categories) const
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
		_exhandler->raiseFromCatch("", "FMTParser::_setCategories", __LINE__, __FILE__, m_section);
		}

	}

GDALRasterBand* FMTParser::_createBand(GDALDataset* dataset,const std::vector<std::string>& categories,int bandid) const
    {
    GDALRasterBand* band = _getBand(dataset,bandid);
	try {
		double nodatavalue = -9999;
		if (band->GetRasterDataType()==GDALDataType::GDT_Byte)
		{
			nodatavalue = static_cast<double>(std::numeric_limits<uint8_t>::max());
		}
		band->SetNoDataValue(nodatavalue);
		band->Fill(nodatavalue);
		_setCategories(band, categories);
		band->FlushCache();
	}catch (...)
		{
		_exhandler->raiseFromCatch("","FMTParser::_createBand", __LINE__, __FILE__, m_section);
		}
    return band;
    }

std::vector<std::string>FMTParser::_getCat(GDALDataset* dataset,int bandid) const
    {
	std::vector<std::string>values;
	try {
		char** meta = dataset->GetMetadata();
		GDALRasterBand* band = _getBand(dataset,bandid);
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
		_exhandler->raiseFromCatch("","FMTParser::_getCat", __LINE__, __FILE__, m_section);
		}
    return values;
    }

GDALRasterBand* FMTParser::_getOverview(GDALRasterBand* band,int view) const
    {
	GDALRasterBand* overview=nullptr;
	try{
	overview = band->GetOverview(view);
    if (overview == nullptr)
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidoverview,band->GetDataset()->GetDescription(),
			"FMTParser::_getOverview",__LINE__, __FILE__, m_section);
        }
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTParser::_getOverview", __LINE__, __FILE__);
		}
    return overview;
    }

GDALDataset* FMTParser::_getVectorDataset(const std::string& location) const
    {
	GDALDataset* dataset = nullptr;
	try {
		if (_isValidFile(location))
		{
			dataset = (GDALDataset*)GDALOpenEx(location.c_str(), GDAL_OF_VECTOR | GDAL_OF_READONLY, nullptr, nullptr, nullptr);
			if (dataset == nullptr)
			{
				_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,
					dataset->GetDescription(),"FMTParser::_getVectorDataset", __LINE__, __FILE__, m_section);
			}
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("","FMTParser::_getVectorDataset", __LINE__, __FILE__, m_section);
		}
    return dataset;
    }

GDALDataset* FMTParser::_createVectorMemoryDs() const
	{
	GDALDataset* dataset = nullptr;
	try{
		dataset = createOGRDataset("Memoryds", "Memory");
		}
		catch (...)
			{
				_exhandler->raiseFromCatch("","FMTParser::getemptymemoryds", __LINE__, __FILE__, m_section);
			}
	return dataset;
	}

OGRCoordinateTransformation* FMTParser::_getProjTransform(OGRLayer* baselayer, bool fittoforel) const
	{
	OGRCoordinateTransformation* coordtransf = nullptr;
	try {
		std::unique_ptr<OGRSpatialReference> forelspref = _getFORELSpatialRef();
		OGRSpatialReference* lspref = baselayer->GetSpatialRef();
		if (fittoforel)
			{
			coordtransf = OGRCreateCoordinateTransformation(lspref, &*forelspref);
		}else {
			coordtransf = OGRCreateCoordinateTransformation(lspref,lspref);
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTParser::_getProjTransform", __LINE__, __FILE__, m_section);
		}
	return coordtransf;
	}

GDALDataset* FMTParser::_getTransFormMemLayerCopy(OGRLayer* baselayer, const OGRSpatialReference* newreference, const std::string& fieldname) const
	{
	GDALDataset* memds = nullptr;
	try {
		OGRwkbGeometryType lgeomtype = baselayer->GetGeomType();
		if (lgeomtype != wkbMultiPolygon && lgeomtype != wkbPolygon)
			{
			_exhandler->raise(Exception::FMTexc::FMTinvalidlayer,
				"Geometry type from layer is not valid, must be wkbMultiPolygon or wkbPolygon : " + std::to_string(lgeomtype),
				"FMTParser::_getTransFormMemLayerCopy", __LINE__, __FILE__, m_section);
			}
		memds = _createVectorMemoryDs();
		std::unique_ptr<OGRSpatialReference>nonconstreference(new OGRSpatialReference(*newreference));
		OGRLayer* memlayer = memds->CreateLayer("Memlayer", nonconstreference.get(), lgeomtype, NULL);
		if (memlayer == NULL)
			{
			_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
				"Layer in memory", "FMTParser::_getTransFormMemLayerCopy", __LINE__, __FILE__, m_section);
			}
		OGRFieldDefn oField(fieldname.c_str(), OFTInteger);
		if (memlayer->CreateField(&oField) != OGRERR_NONE)
			{
			_exhandler->raise(Exception::FMTexc::FMTgdal_constructor_error,
				"Field definition", "FMTParser::_getTransFormMemLayerCopy", __LINE__, __FILE__, m_section);
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTParser::_getTransFormMemLayerCopy", __LINE__, __FILE__, m_section);
		}
	return memds;
	}

GDALDataset* FMTParser::createOGRDataset(
	std::string location,
	std::string gdaldrivername) const
{
	GDALDataset* newdataset = nullptr;
	try {
		//GDALAllRegister();
		GDALDriver* newdriver = GetGDALDriverManager()->GetDriverByName(gdaldrivername.c_str());
		if (newdriver == NULL)
			{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddriver,
				gdaldrivername, "FMTParser::createOGRDataset", __LINE__, __FILE__, m_section);
			}
		const boost::filesystem::path pathObj(location);
		if (boost::filesystem::exists(pathObj))
			{
			if (newdriver->Delete(location.c_str()) != 0)
				{
				_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,
					"Cannot delete " + location, "FMTParser::createOGRDataset", __LINE__, __FILE__, m_section);
				}
			}
		newdataset = newdriver->Create(location.c_str(), 0, 0, 0, GDT_Unknown,NULL);
		if (newdataset == NULL)
		{
			_exhandler->raise(Exception::FMTexc::FMTinvaliddataset,
				"Can not create new dataset at " + location, "FMTParser::createOGRDataset", __LINE__, __FILE__, m_section);
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTParser::createOGRDataset", __LINE__, __FILE__, m_section);
		}
	return newdataset;
}

OGRLayer* FMTParser::createLayer(GDALDataset* dataset,
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
				"Cannote create new layer FMTresults for " + name, "FMTParser::createLayer", __LINE__, __FILE__, m_section);
			//Non Valid Layer
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::createLayer", __LINE__, __FILE__);
	}
	return newlayer;
}


OGRLayer* FMTParser::_getLayer(GDALDataset* dataset,int id) const
    {
	OGRLayer * layer = nullptr;
	try{
	layer = dataset->GetLayer(id);
    if( layer == nullptr )
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidlayer,dataset->GetDescription(),
			"FMTParser::_getLayer",__LINE__, __FILE__, m_section);
        }
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::_getLayer", __LINE__, __FILE__, m_section);
	}
    return layer;
    }

void FMTParser::_getWSFields(OGRLayer* layer, std::map<int, int>& themes, int& age, int& area, int& lock, std::string agefield, std::string areafield, std::string lockfield) const
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
				if (boost::regex_search(fname, kmatch,m_VECTOR_THEME)/*fname.find("THEME") != std::string::npos*/)
				{
					//fname.erase(0, 5);
					//themes[getNum<int>(fname) - 1] = iField;
					themes[_getNum<int>(std::string(kmatch[2])) - 1] = iField;
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
					layer->GetDescription(),"FMTParser::_getWSFields", __LINE__, __FILE__, m_section);
			}
			if (age == -1 && !agefield.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingfield, agefield + " " + layer->GetDescription(),
					"FMTParser::_getWSFields",__LINE__, __FILE__, m_section);
			}
			if (area == -1 && !areafield.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingfield, areafield + " " + layer->GetDescription(),
					"FMTParser::_getWSFields", __LINE__, __FILE__, m_section);
			}
			if (lock == -1 && !lockfield.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTmissingfield, lockfield + " " + layer->GetDescription(),
					"FMTParser::_getWSFields", __LINE__, __FILE__, m_section);
			}
	}catch (...)
			{
			_exhandler->raiseFromCatch("","FMTParser::_getWSFields", __LINE__, __FILE__, m_section);
			}
}

#endif

std::string FMTParser::_setSpecs(Core::FMTsection section, Core::FMTkwor key,const Core::FMTYields& ylds,const Core::FMTConstants& constants,std::vector<Core::FMTSpec>& specs, const std::string& line)
    {
	std::string rest = "";
	try {
			boost::smatch kmatch;
			
			if (boost::regex_search(line, kmatch, m_AYLD_AGE))
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
					_isYld(ylds, yld, section);
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
					if (boost::regex_search(bound,bmatch,m_BOUNDS))
					{
						std::string singlebound = std::string(bmatch[4]);
						if(singlebound.empty())
						{
							std::string strlower = std::string(bmatch[1]);
							boost::trim(strlower);
							lowerbound = _getNum<double>(strlower, constants);
							std::string strupper = std::string(bmatch[3]);
							boost::trim(strupper);
							if( strupper != "_MAXAGE")
							{
								upperbound = _getNum<double>(strupper, constants);						
							}
						}else{
							lowerbound =  _getNum<double>(singlebound, constants);
							upperbound =  _getNum<double>(singlebound, constants);	
						}
						bounds.insert(boost::icl::continuous_interval<double>::closed(lowerbound,upperbound));
					}
				}
				for(boost::icl::interval_set<double>::reverse_iterator rit = bounds.rbegin(); rit != bounds.rend(); ++rit)
				{
					Core::FMTSpec newspec;
					if (pushaagebound)
					{
						const int intupper = (rit->upper() == std::numeric_limits<double>::max()) ? std::numeric_limits<int>::max() : static_cast<int>(rit->upper());
						const int intlower = (rit->lower() == std::numeric_limits<double>::min()) ? std::numeric_limits<int>::min() : static_cast<int>(rit->lower());
						newspec.addBounds(Core::FMTAgeBounds(section, key, intupper, intlower));
					}else {
						newspec.addBounds(Core::FMTYldBounds(section, key, yld, rit->upper(), rit->lower()));
						}
					specs.push_back(newspec);
				}
				rest = " "+std::string(kmatch[1])+std::string(kmatch[7])+std::string(kmatch[12]);
			}else{
				rest=line;
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTParser::_setSpec", __LINE__, __FILE__, m_section);
			}
    return rest;
    }

std::string FMTParser::_setSpec(
	Core::FMTsection section, 
	Core::FMTkwor key,
	const Core::FMTYields& ylds,
	const Core::FMTConstants& constants,
	Core::FMTSpec& spec, 
	const std::string& line)
    {
	std::string rest = "";
	try {
			boost::smatch kmatch;
			
			if (boost::regex_search(line, kmatch, FMTParser::m_AYLD))
			{
				std::string yld = std::string(kmatch[4]) + std::string(kmatch[20]);
				boost::trim(yld);
				bool pushaagebound = false;
				if (yld=="_AGE")
					{
					pushaagebound = true;
					}else {
					_isYld(ylds, yld, section);
					}
				const std::string singlebound = std::string(kmatch[21]) + std::string(kmatch[22]) + std::string(kmatch[23]);
				double upperbound = std::numeric_limits<double>::max();
				double lowerbound = std::numeric_limits<double>::min();
				if (singlebound.empty())
				{
					if (std::string(kmatch[14]).empty())
					{
						const std::string strupper = std::string(kmatch[12]) + std::string(kmatch[13]);
						upperbound = _getNum<double>(strupper, constants);
					}
					const std::string strlower = std::string(kmatch[8]) + std::string(kmatch[9]);
					lowerbound = _getNum<double>(strlower, constants);
				}
				else {
					lowerbound = _getNum<double>(singlebound, constants);
					if (section == Core::FMTsection::Transition)
					{
						upperbound = lowerbound;
					}
				}
				
				if (pushaagebound)
					{
					spec.addBounds(Core::FMTAgeBounds(section, key, static_cast<int>(upperbound), static_cast<int>(lowerbound)));
				}else {
					spec.addBounds(Core::FMTYldBounds(section, key, yld, upperbound, lowerbound));
					}
				
				rest = " " + std::string(kmatch[1]) + std::string(kmatch[16]) + std::string(kmatch[28]);
			}
			else {
				rest = line;
			}

			if (boost::regex_search(rest, kmatch, FMTParser::m_AAGE))
			{
				std::string singlebound = std::string(kmatch[20]) + std::string(kmatch[21]) + std::string(kmatch[22]);
				int upperbound = std::numeric_limits<int>::max();
				int lowerbound = std::numeric_limits<int>::min();
				if (singlebound.empty())
				{
					if (std::string(kmatch[13]).empty())
					{
						std::string strupper = std::string(kmatch[10]) + std::string(kmatch[11]) + std::string(kmatch[12]);
						upperbound = _getNum<int>(strupper, constants);
					}
					std::string strlower = std::string(kmatch[5]) + std::string(kmatch[6]) + std::string(kmatch[7]);
					lowerbound = _getNum<int>(strlower, constants);
				}
				else {
					lowerbound = _getNum<int>(singlebound, constants);
					upperbound = lowerbound;
				}
				spec.addBounds(Core::FMTAgeBounds(section, key, upperbound, lowerbound));
				rest = " " + std::string(kmatch[1]) + std::string(kmatch[15]) + std::string(kmatch[16]) + std::string(kmatch[24]);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTParser::_setSpec", __LINE__, __FILE__, m_section);
			}
    return rest;
    }

bool FMTParser::_isAct(Core::FMTsection section,const std::vector<Core::FMTAction>& actions, std::string action) const
    {
	try{
    if (std::find_if(actions.begin(),actions.end(), Core::FMTActionComparator(action,true))==actions.end())
        {
			_exhandler->raise(Exception::FMTexc::FMTignore, "No action named " + action + " at line " + std::to_string(m_line), "FMTParser::_isAct", __LINE__, __FILE__, section);
        return false;
        }else
		{
        return true;
        }
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::_isAct", __LINE__, __FILE__, m_section);
	}
	return false;
    }



bool FMTParser::_isYld(const Core::FMTYields& p_ylds,
				const std::string& p_value, Core::FMTsection pm_section,
				bool p_throwError) const
    {
	try{
    if (p_ylds.isYld(p_value))
        {
        return true;
        }
	if (p_throwError)
		{
		_exhandler->raise(Exception::FMTexc::FMTinvalid_yield, p_value + " at line " + std::to_string(m_line),
			"FMTParser::_isYld", __LINE__, __FILE__, pm_section);
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTParser::_isYld", __LINE__, __FILE__, m_section);
		}
     return false;
    }


std::time_t FMTParser::getMostRecentFiletime() const
	{
	return m_MostRecentFile;
	}




bool FMTParser::tryOpening(const std::ifstream& stream, const std::string& location) const
        {
		try{
        m_location = location;
		const boost::filesystem::path extpath(location);
		std::string extension = extpath.extension().string();
        transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        this->_setSection(fromExtension(extension));
		_logger->logWithLevel("Reading " + location+"\n", 2);
		//*_logger << "extt " << m_section << "\n";
        if (!stream.is_open())
            {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,location,
				"FMTParser::tryOpening", __LINE__, __FILE__, m_section);
            return false;
            }
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTParser::tryOpening", __LINE__, __FILE__, m_section);
		}
		boost::filesystem::path pathtofile(location);
		const std::time_t temp = boost::filesystem::last_write_time(pathtofile);
		if (std::difftime(m_MostRecentFile,temp)>0|| m_MostRecentFile ==0)
			{
			m_MostRecentFile = temp;
			}
        return true;
        }

bool FMTParser::tryOpening(std::ofstream& stream, const std::string& location) const
        {
		try{
        m_location = location;
        if (!stream.is_open())
            {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,location,
				"FMTParser::tryOpening", __LINE__, __FILE__, m_section);
            return false;
            }
		if (!m_comment.empty())//If you find an header write it down
			{
			const std::string seperator = ";" + std::string(100, '-');
			stream << seperator << "\n";
			stream << ";" << _logger->getLogStamp() << "\n";
			std::vector<std::string>lines;
			boost::split(lines,m_comment, boost::is_any_of("\n"), boost::token_compress_on);
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
			_exhandler->raiseFromCatch("", "FMTParser::tryOpening", __LINE__, __FILE__, m_section);
			}
        return true;
        }

bool FMTParser::_isValidFile(const std::string& location) const
	{
	try {
		const boost::filesystem::path pathObj(location);
		std::string extension = pathObj.extension().string();
		transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		if (!boost::filesystem::exists(pathObj) || !boost::filesystem::is_regular_file(pathObj))
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_path, location,
				"FMTParser::_isValidFile", __LINE__, __FILE__, m_section);
			return false;
		}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTParser::_isValidFile", __LINE__, __FILE__, m_section);
		}
	return true;
	}

bool FMTParser::_isNum(std::string value) const
    {
	//boost::erase_all(value, ",");
	return boost::regex_match(value,m_NUMBER);
    }


Core::FMTPerBounds FMTParser::_getPerBound(const std::string& p_lower,
	const std::string& p_upper,
	const Core::FMTConstants& p_constants) const
{
	Core::FMTPerBounds bound;
	try {

		int startPeriod = std::numeric_limits<int>::max();
		if (p_lower != "_LENGTH")
		{
			startPeriod = _getNum<int>(p_lower, p_constants);
		}
		int stopPeriod = startPeriod;
		if (!p_upper.empty() && p_upper != "_LENGTH")
		{
			stopPeriod = _getNum<int>(p_upper, p_constants);
		}
		else if (p_upper == "_LENGTH")
		{
			stopPeriod = std::numeric_limits<int>::max();
		}
		bound = Core::FMTPerBounds(Core::FMTsection::Optimize, stopPeriod, startPeriod);
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("",
			"FMTParser::_getPerBound", __LINE__, __FILE__, m_section);
	}
	return bound;
}

void FMTParser::_setPeriodWithBounds(Core::FMTSpec& p_spec,
	const std::string& p_lower,
	const std::string& p_upper,
	const Core::FMTConstants& p_constants) const
{
	const Core::FMTPerBounds PERIOD_BOUND = _getPerBound(p_lower, p_upper, p_constants);
	p_spec.setBounds(PERIOD_BOUND);
}

bool FMTParser::_setPeriods(Core::FMTSpec& p_spec,
	const std::string& p_periods,
	const Core::FMTConstants& p_constants) const
{
	bool isSet = false;
	try {
		boost::smatch theMatch;
		if (boost::regex_search(p_periods, theMatch, m_PERIODS))
		{
			const std::string JUST_LENGTH = std::string(theMatch[7]);
			const std::string LOWER_PERIOD = std::string(theMatch[4]) + std::string(theMatch[8]) + JUST_LENGTH;
			const std::string UPPER_PERIOD = std::string(theMatch[6]) + JUST_LENGTH;
			_setPeriodWithBounds(p_spec, LOWER_PERIOD, UPPER_PERIOD, p_constants);
			isSet = true;
		}
	}catch (...)
	{
		_exhandler->raiseFromCatch("for spec " + std::string(p_spec),
			"FMTParser::_setPeriods", __LINE__, __FILE__, m_section);
	}
	return isSet;
}

bool FMTParser::_isNum(const std::string& value, const Core::FMTConstants& constant, bool throwerror) const
{
	try {
		if (!(_isNum(value) || constant.isConstant(value)))
			{
			if (throwerror)
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalidyield_number, value,
					"FMTParser::_isNum", __LINE__, __FILE__, m_section);
				}
		}
		else {
			return true;
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::_isNum", __LINE__, __FILE__, m_section);
	}
	return false;
}



std::vector<std::string>FMTParser::regexLoop(const boost::regex& cutregex, std::string& str) const
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
			_exhandler->raiseFromCatch("", "FMTParser::regexLoop", __LINE__, __FILE__, m_section);
		}
        return result;
        }
    bool FMTParser::_isValid(const std::string& line) const
        {
        return boost::regex_match(line,m_VALID);
        }
    void FMTParser::_clearComments(std::string& line) const
        {
		try {
			if (line.find(";")!=std::string::npos)
				{
				boost::smatch kmatch;
				if (boost::regex_search(line, kmatch, m_REMOVE_COMMENT))
				{
					m_comment = std::string(kmatch[2]);
					boost::to_upper(m_comment);
					line = std::string(kmatch[1]);
				}
				}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTParser::_clearComments", __LINE__, __FILE__, m_section);
		}
        }
	std::string FMTParser::_upper(const std::string& lowercases) const
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
			_exhandler->raiseFromCatch("", "FMTParser::upper", __LINE__, __FILE__, m_section);
		}
        return uppercases;
        }
	std::vector<std::string>FMTParser::spliter(std::string strmask,const boost::regex& xspliter) const
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
            if (_isValid(value))
                {
                vecmask.push_back(value);
                }
            endsize = (std::string(kmatch[0]).size());
            strmask.erase(strmask.begin(),strmask.begin()+endsize);
            }
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTParser::spliter", __LINE__, __FILE__, m_section);
		}
        return vecmask;
        }

	std::string FMTParser::getCleanLine(const std::string& p_input) const
	{
		std::string out;
		try {
			std::istringstream inputStream(p_input);
			out = getCleanLine(inputStream);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTParser::getCleanLine", __LINE__, __FILE__, m_section);
			}
		return out;
	}


	std::string FMTParser::getCleanLine(std::istream& stream) const
        {
        ++m_line;
		std::string newline;
		std::string line;
		try{
			if (_safeGetline(stream, line))
			{
				m_comment.clear();
				_clearComments(line);

				//std::string fullline = newline;
			   // newline = "";
			
				newline.reserve(line.size());
				for(int loc = 0; loc < static_cast<int>(line.size()); ++loc)
					{
					const char& VALUE = line.at(loc);
					if(m_inComment)
						{
						if (VALUE =='}')
							{
							m_inComment = false;
							}
						}else{
						if (VALUE =='{')
							{
							m_inComment = true;
							}else{
							newline+=toupper(VALUE);
							}
						}
					}
				if (m_inComment && !m_comment.empty() && m_comment.find('}') != std::string::npos)
					{
					m_inComment = false;
					}
				if (!m_comment.empty() && m_comment.find('{') != std::string::npos)
					{
					m_inComment = true;
					}
				 } else if(std::ifstream* ifs = dynamic_cast<std::ifstream*>(&stream) )
					{
					 ifs->close();
					}
			
        boost::trim(newline);
		if (newline.empty() && m_inComment && !m_comment.empty() && m_comment.find('}') != std::string::npos &&
			(m_comment.find('{') == std::string::npos || m_comment.find('{') < m_comment.find('}')))
			{
			m_inComment = false;
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTParser::getCleanLine", __LINE__, __FILE__, m_section);
			}
        return newline;
        }

	std::string FMTParser::_getLine(std::queue<FMTLineInfo>& p_Lines) const
		{
		std::string returnedValue = p_Lines.front().m_lineValue;
		m_line = p_Lines.front().m_lineNumber;
		m_location = p_Lines.front().m_file;
		p_Lines.pop();
		return returnedValue;
		}

std::map<std::string, std::vector<std::string>>  FMTParser::_getForLoops(const std::string& p_line,
	const std::vector<Core::FMTTheme>& p_themes,
	const Core::FMTConstants& p_cons) const
	{
	std::map<std::string, std::vector<std::string>> allValues;
	try{
	boost::smatch kmatch;
	if (boost::regex_search(p_line, kmatch, m_FOR))
	{
		const std::string TARGET = std::string(kmatch[3]);
		if (allValues.find(TARGET)== allValues.end())
			{
			allValues[TARGET] = std::vector<std::string>();
			}
		if (!std::string(kmatch[24]).empty())
		{
			std::string val = std::string(kmatch[24]);
			boost::trim_if(val, boost::is_any_of(FMT_STR_SEPARATOR));
			boost::replace_all(val, "\n", "");
			const std::string forstrsep = FMT_STR_SEPARATOR + std::string(",");
			boost::split(allValues[TARGET], val, boost::is_any_of(forstrsep), boost::token_compress_on);
			if (val.find("..")!= std::string::npos)
				{
				std::vector<std::string>newvalues;
				for (const std::string& oldvalue: allValues[TARGET])
					{
					if (oldvalue.find("..") != std::string::npos)
						{
						std::vector<std::string>lowerNupper;
						boost::split(lowerNupper, oldvalue, boost::is_any_of(".."), boost::token_compress_on);
						const int lower = _getNum<int>(lowerNupper.at(0), p_cons);
						const int upper = _getNum<int>(lowerNupper.at(1), p_cons);
						for (int id = lower; id <= upper; ++id)
							{
							newvalues.push_back(std::to_string(id));
							}
						
					}else {
						newvalues.push_back(oldvalue);
						}
					}
				allValues[TARGET] = newvalues;
				}
		}
		else if (!std::string(kmatch[16]).empty())
		{
			const int theme = _getNum<int>(std::string(kmatch[16])) - 1;
			std::string att = "?";
			allValues[TARGET] = p_themes[theme].getAttributes(att);
		}
		else if (!std::string(kmatch[10]).empty())
		{
			const int theme = _getNum<int>(std::string(kmatch[10])) - 1;
			std::string aggregate = kmatch[12];
			allValues[TARGET] = p_themes[theme].getAttributes(aggregate,true);
		}
		else if (!std::string(kmatch[19]).empty() || !std::string(kmatch[26]).empty())
		{
			std::string localTarget = TARGET;
			if (!std::string(kmatch[28]).empty())
				{
				allValues.erase(TARGET);
				localTarget = kmatch[28];
				if (allValues.find(localTarget) == allValues.end())
					{
					allValues[localTarget] = std::vector<std::string>();
					}
				}
			const std::string slower = std::string(kmatch[19]) + std::string(kmatch[30]);
			const std::string supper = std::string(kmatch[21]) + std::string(kmatch[34]);
			const int lower = _getNum<int>(slower, p_cons);
			const int upper = _getNum<int>(supper, p_cons);
			for (int id = lower; id <= upper; ++id)
			{
				allValues[localTarget].push_back(std::to_string(id));
			}
		}else if (!std::string(kmatch[35]).empty())//In a Query
		{
			const std::string PATH_TO_DATABASE = std::string(kmatch[41]);
			const std::string QUERY = _processConstants(kmatch[46], p_cons);
			const std::string BASE_TARGET = std::string(kmatch[37]);
			#ifdef FMTWITHGDAL
				allValues = _queryDatabase(PATH_TO_DATABASE, BASE_TARGET, QUERY);
			#else
			_exhandler->raise(Exception::FMTexc::FMTGDALerror,
				"The following Query is not supported " + QUERY,
				"FMTParser::_getForLoops", __LINE__, __FILE__);
			#endif
		}
	}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTParser::_getForLoops", __LINE__, __FILE__, m_section);
		}
	return allValues;
	}

bool FMTParser::_isForLoops(const std::string& p_line) const
	{
	bool IsValid = false;
		try {
			boost::smatch kmatch;
			if (boost::regex_search(p_line, kmatch, m_FOR))
			{
				IsValid = true;
			}
			//IsValid = (boost::regex_match(p_line, m_FOR));
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTParser::_isForLoops", __LINE__, __FILE__, m_section);
		}
		return IsValid;
	}

bool FMTParser::_isForLoopsEnd(const std::string& p_line) const
{
	bool IsValid = false;
	try {
		IsValid = (boost::regex_match(p_line, m_END));
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::_isForLoopsEnd", __LINE__, __FILE__, m_section);
	}
	return IsValid;
}


std::string FMTParser::_getAbsolutePath(std::string p_Path) const
	{
	try {
		boost::filesystem::path includedpath(p_Path);
		if (p_Path.find("_PRIMARY") != std::string::npos)
		{
			if (primarym_sections.find(m_section) != primarym_sections.end())
			{
				boost::filesystem::path new_path = primarym_sections[m_section];
				
				size_t openParen = p_Path.find("(");
				size_t closeParen = p_Path.find(")");
				// Utile pour les template à Louis. LANGA3 2025-11-25
				if (openParen != std::string::npos && closeParen != std::string::npos)
				{
					std::string scenarioName = p_Path.substr(openParen + 1, closeParen - openParen - 1);
					std::string extension = new_path.extension().string().insert(1, "_");
					std::string filename = new_path.stem().string();
					
					new_path = new_path.parent_path() / "Scenarios" / scenarioName / (filename + extension);
				}
				p_Path = new_path.string();
			}
		}
		else if (includedpath.is_absolute())
		{
			const boost::filesystem::path l_ppath(m_location);
			const boost::filesystem::path parent_path = l_ppath.parent_path();
			p_Path = boost::filesystem::canonical(includedpath, parent_path).string();
		}
		else if (boost::starts_with(p_Path, ".\\.\\"))
		{
			const boost::filesystem::path FORMATED_PATH(m_location);
			const boost::filesystem::path PARENT_PATH = FORMATED_PATH.parent_path();
			boost::replace_all(p_Path, ".\\", "..\\");
			const size_t FIRST_DOT = p_Path.find(".");
			if (FIRST_DOT!=std::string::npos)
				{
				p_Path = p_Path.substr(0, FIRST_DOT) + PARENT_PATH.string()+"\\" + p_Path.substr(FIRST_DOT, p_Path.size());
				}
		}
		else if (boost::starts_with(p_Path, "."))
		{
			const boost::filesystem::path l_ppath(m_location);
			const boost::filesystem::path parent_path = l_ppath.parent_path();
			p_Path.erase(0, 1);
			const boost::filesystem::path full_path = parent_path / p_Path;
			p_Path = full_path.string();
		}
		else if (p_Path.find('/') == std::string::npos &&
			p_Path.find('\\') == std::string::npos)
		{
			const boost::filesystem::path L_PPATH(m_location);
			const boost::filesystem::path PARENT_PATH = L_PPATH.parent_path();
			const boost::filesystem::path FULL_PATH = PARENT_PATH / p_Path;
			p_Path = FULL_PATH.string();
		}
		const boost::filesystem::path basepath = boost::filesystem::path(p_Path);
		if (!boost::filesystem::exists(p_Path))//If does not exist then try with the _ type path.
		{
			const boost::filesystem::path llpath(m_location);
			const boost::filesystem::path actllpath(p_Path);
			const std::string extension = llpath.extension().string();
			std::string actualextension = actllpath.extension().string();
			if (extension.find("_") != std::string::npos &&
				actualextension.find("_") == std::string::npos)
			{
				actualextension.erase(0, 1);
				const std::string newextension = "._" + actualextension;
				boost::filesystem::path full_path(p_Path);
				full_path.replace_extension(newextension);
				p_Path = full_path.string();
			}
		}
		if (!boost::filesystem::exists(p_Path))//Look back to the original folder...
		{
			const boost::filesystem::path llpath(p_Path);
			const boost::filesystem::path parentdir = llpath.parent_path();
			const boost::filesystem::path finalpath = parentdir / boost::filesystem::path("../..") / basepath.filename();
			p_Path = finalpath.string();
		}
		if (!boost::filesystem::exists(p_Path))
		{
			boost::replace_all(p_Path, "\\..\\..\\", "\\");
			boost::replace_all(p_Path, "../..\\", "");
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTParser::_getAbsolutePath", __LINE__, __FILE__, m_section);
		}
	return p_Path;
	}

std::queue<FMTParser::FMTLineInfo> FMTParser::_tryInclude(
	const std::vector<Core::FMTTheme>& p_themes, const Core::FMTConstants& p_cons,
	std::queue<FMTLineInfo>p_ForOut) const
	{
	std::queue<FMTLineInfo>includedm_lines;
	try{
		while (!p_ForOut.empty())
		{
			boost::smatch kmatch;
			const FMTLineInfo& ELEMENT = p_ForOut.front();
			if (boost::regex_match(ELEMENT.m_lineValue, FMTParser::m_INCLUDE) &&
				boost::regex_search(ELEMENT.m_lineValue, kmatch, FMTParser::m_INCLUDE))
			{
				const std::string LOCATION = _getAbsolutePath(kmatch[3]);
				FMTParser newparser;
				std::ifstream newstream(LOCATION);
				if (newparser.tryOpening(newstream, LOCATION))
				{
					while (newstream.is_open())
					{
						std::queue<FMTLineInfo> OutLoops = newparser._getCleanLinewfor(newstream,p_themes,p_cons);
						while (!OutLoops.empty())
						{
							const FMTLineInfo& FOR_ELEMENT = OutLoops.front();
							if (!FOR_ELEMENT.m_lineValue.empty())
								{
								includedm_lines.push(FOR_ELEMENT);
								}
							OutLoops.pop();
						}
						
					}
				}
			}else if (!ELEMENT.m_lineValue.empty())
				{
				includedm_lines.push(ELEMENT);
				}
			p_ForOut.pop();
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTParser::_tryInclude", __LINE__, __FILE__, m_section);
	}
	return includedm_lines;
	}

#ifdef FMTWITHGDAL
	std::map<std::string, std::vector<std::string>>FMTParser::_queryDatabase(
		const std::string& p_DataBaseLocation,
		const std::string& p_VariableName,
		const std::string& p_Query) const
	{
		std::map<std::string, std::vector<std::string>>fieldsData;
		try {
			const std::string ABSOLUTE_DATABASE_PATH = _getAbsolutePath(p_DataBaseLocation);
			boost::filesystem::path ABS_PATH(ABSOLUTE_DATABASE_PATH);
			std::string capExtension = ABS_PATH.extension().string();
			boost::to_lower(capExtension);
			const std::string LOWER_CASE_PATH = (ABS_PATH.parent_path() / ABS_PATH.stem()).string() + capExtension;
			GDALDataset* DATASET = _getVectorDataset(LOWER_CASE_PATH);
			const char* DIALECT_OPTION = nullptr;
			OGRGeometry* NULL_GEOMETRY = nullptr;
			OGRLayer* SUBSET_LAYER = DATASET->ExecuteSQL(p_Query.c_str(), NULL_GEOMETRY, DIALECT_OPTION);
			if (SUBSET_LAYER == nullptr)
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalidlayer,
					"Invalid Query " + ABSOLUTE_DATABASE_PATH + " with this query \n" + p_Query + " at line " +
					std::to_string(m_line), "FMTParser::_queryDatabase", __LINE__, __FILE__, m_section);
				}
			OGRFeatureDefn* FIELD_DEFINITIONS = SUBSET_LAYER->GetLayerDefn();
			const size_t FEATURE_COUNT = static_cast<size_t>(SUBSET_LAYER->GetFeatureCount());
			if (FEATURE_COUNT == 0)
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"No lines generated for this database " + ABSOLUTE_DATABASE_PATH + " with this query \n" + p_Query + " at line " +
					std::to_string(m_line), "FMTParser::_queryDatabase", __LINE__, __FILE__, m_section);
				}
			const size_t FIELD_COUNT = static_cast<size_t>(FIELD_DEFINITIONS->GetFieldCount());
			if (FIELD_COUNT == 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"No columns generated for this database " + ABSOLUTE_DATABASE_PATH + " with this query \n" + p_Query + " at line " +
					std::to_string(m_line), "FMTParser::_queryDatabase", __LINE__, __FILE__, m_section);
			}
			std::vector<std::vector<std::string>>Data(FIELD_COUNT, std::vector<std::string>(FEATURE_COUNT));
			OGRFeature* feature;
			size_t featureId = 0;
			while ((feature = SUBSET_LAYER->GetNextFeature()) != NULL)
				{
				for (int iField = 0; iField < FIELD_DEFINITIONS->GetFieldCount(); ++iField)
					{
					Data[iField][featureId] = getCleanLine(feature->GetFieldAsString(iField));
					}
				OGRFeature::DestroyFeature(feature);
				++featureId;
				}
			
			GDALClose(DATASET);
			for (int iField = 0; iField < FIELD_DEFINITIONS->GetFieldCount(); ++iField)
				{
				OGRFieldDefn* FIELD_DEF = FIELD_DEFINITIONS->GetFieldDefn(iField);
				std::string FieldName = FIELD_DEF->GetNameRef();
				FieldName = p_VariableName + "." + FieldName;
				boost::to_upper(FieldName);
				fieldsData[FieldName] = std::vector<std::string>();
				fieldsData[FieldName].swap(Data[iField]);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("Failed at line "+ std::to_string(m_line) +" in "+ m_location,
				"FMTParser::_queryDatabase", __LINE__, __FILE__, m_section);
			}
		return fieldsData;
	}
#endif

	FMTParser::FMTLineInfo FMTParser::_setForLoopLines(std::queue<FMTLineInfo>& p_queue) const
	{
		const FMTLineInfo FULL_LINE = p_queue.front();
		p_queue.pop();
		std::string Line = FULL_LINE.m_lineValue;
		m_line = FULL_LINE.m_lineNumber;
		if (!_isForLoops(FULL_LINE.m_lineValue) && Line.find("FOREACH") != std::string::npos)
		{
			size_t OpeningBraces = 0;
			size_t ClosingBraces = 0;
			do {
				const FMTLineInfo NEW_LINE = p_queue.front();
				m_line = NEW_LINE.m_lineNumber;
				Line += "\n" + NEW_LINE.m_lineValue;
				p_queue.pop();
				OpeningBraces = std::count(Line.begin(), Line.end(), '(');
				ClosingBraces = std::count(Line.begin(), Line.end(), ')');
			} while (OpeningBraces > ClosingBraces && !p_queue.empty());
		}
		return FMTLineInfo(Line, FULL_LINE.m_lineNumber, FULL_LINE.m_file);
	}

	std::string FMTParser::_processConstants(std::string p_input,
		const Core::FMTConstants& p_constants) const
	{
		try {
			// The constant token stops at whitespace, tabs, quotes, commas and
			// parentheses so that a delimiter surrounding the constant (a quote or
			// punctuation such as ',' '(' ')') is not part of the captured token.
			const boost::regex Constants("(#[^\\s\\t'\"(),]*)");
			boost::smatch matches;
			const std::string Source(p_input);
			std::string::const_iterator start = Source.begin();
			const std::string::const_iterator end = Source.end();
			std::string Result;
			Result.reserve(Source.size());
			while (boost::regex_search(start, end, matches, Constants))
				{
				const std::string TARGET = matches[1].str();
				Result.append(start, matches[1].first);
				const char BEFORE = (matches[1].first != Source.begin()) ? *(matches[1].first - 1) : '\0';
				const char AFTER = (matches[1].second != end) ? *(matches[1].second) : '\0';
				const bool WRAPPED_IN_QUOTES = (BEFORE == '\'' && AFTER == '\'') || (BEFORE == '"' && AFTER == '"');
				if (WRAPPED_IN_QUOTES)
					{
					Result.append(TARGET);
					}
				else
					{
					const int VALUE = _getNum<int>(TARGET, p_constants);
					Result.append(std::to_string(VALUE));
					}
				start = matches[1].second;
				}
			Result.append(start, end);
			p_input.swap(Result);
		}catch (...)
			{
			_exhandler->raiseFromCatch("Failed to constants " + std::to_string(m_line) + " in " + m_location,
				"FMTParser::_processConstants", __LINE__, __FILE__, m_section);
			}
		return p_input;
	}


	void FMTParser::_processForLoops(const std::vector<Core::FMTTheme>& p_themes,
		const Core::FMTConstants& p_constants,
		std::queue<FMTLineInfo>& p_queue) const
	{
		std::queue<FMTLineInfo>FinalQueue;
		std::vector<std::vector<FMTLineInfo>>ForloopQueues;
		std::vector<std::vector<std::string>>VariablesData;
		std::vector<std::string>Variables;
		bool FirstLoopFound = false;
		bool SeenOtherForLoop = false;
		size_t OpenedForLoops = 0;
		do {
			/*const std::pair<std::string, int> FULL_LINE = p_queue.front();
			std::string Line = FULL_LINE.first;
			m_line = FULL_LINE.second;
			p_queue.pop();*/
			const FMTLineInfo FULL_LINE = _setForLoopLines(p_queue);
			m_line = FULL_LINE.m_lineNumber;
			const bool GOT_FOR_LOOPS = _isForLoops(FULL_LINE.m_lineValue);
			bool IgnoreThisLine = false;
			if (_isForLoopsEnd(FULL_LINE.m_lineValue))
			{
				if (OpenedForLoops==1)
					{
					IgnoreThisLine = true;
					}
				--OpenedForLoops;
			}
			else if (GOT_FOR_LOOPS)
			{
				if (!FirstLoopFound)
					{
					IgnoreThisLine = true;
					const std::map<std::string, std::vector<std::string>> LOOP_VARIABLES = _getForLoops(FULL_LINE.m_lineValue, p_themes, p_constants);
					std::map<size_t, std::vector<std::string>>SortedVariables;
					for (const auto& LOOP : LOOP_VARIABLES)
						{
						size_t SIZE = LOOP.first.size();
						if (SortedVariables.find(SIZE)== SortedVariables.end())
							{
							SortedVariables[SIZE] = std::vector<std::string>();
							}
						SortedVariables[SIZE].push_back(LOOP.first);
						}
					for (const auto& SORTED : SortedVariables)
						{
						Variables.insert(Variables.end(), SORTED.second.begin(), SORTED.second.end());
						}
					std::reverse(Variables.begin(), Variables.end());
					for (const std::string& VARIABLE : Variables)
						{
						VariablesData.push_back(LOOP_VARIABLES.at(VARIABLE));
						}

					ForloopQueues = std::vector<std::vector<FMTLineInfo>>(VariablesData.begin()->size());
				}else {
					SeenOtherForLoop = true;
				}
				FirstLoopFound = true;
				++OpenedForLoops;
			}
			if (!IgnoreThisLine)
				{
				if (OpenedForLoops > 0)
					{
						size_t UnrolId = 0;
						for (size_t REPLACER_ID = 0; REPLACER_ID < VariablesData.begin()->size(); ++REPLACER_ID)
						{
							std::string ModifiedLine = FULL_LINE.m_lineValue;
							size_t variableID = 0;
							for (const auto& VARIABLE : Variables)
							{
								ModifiedLine = boost::regex_replace(ModifiedLine, boost::regex(VARIABLE), VariablesData[variableID][REPLACER_ID]);
								boost::trim(ModifiedLine);
								++variableID;
							}
							ForloopQueues[UnrolId].push_back(FMTLineInfo(ModifiedLine,m_line,m_location));
							++UnrolId;
						}
					}else {
						FinalQueue.push(FULL_LINE);
						}
				}
			} while (!p_queue.empty() && (!FirstLoopFound||OpenedForLoops > 0));
			if (OpenedForLoops > 0)
				{
				_exhandler->raise(Exception::FMTexc::FMTunclosedforloop,
					"Missing EndFor after line " + std::to_string(m_line), "FMTParser::_getCleanLinewfor", __LINE__, __FILE__, m_section);
				}
			//Fill up the queue with loops variables...
			for (const std::vector<FMTLineInfo>& UNROLED : ForloopQueues)
				{
				for (const FMTLineInfo& UNROLED_LINE : UNROLED)
					{
					FinalQueue.push(UNROLED_LINE);	
					}
				}
			//Fill the rest in the queue
			while (!p_queue.empty())
				{
				const FMTLineInfo FULL_LINE = _setForLoopLines(p_queue);
				const bool GOT_FOR_LOOPS = _isForLoops(FULL_LINE.m_lineValue);
				if (GOT_FOR_LOOPS)
					{
					SeenOtherForLoop = true;
					}
				FinalQueue.push(FULL_LINE);
				}
			if (SeenOtherForLoop)
				{
				_processForLoops(p_themes, p_constants, FinalQueue);
				}
			p_queue = FinalQueue;
	}

	std::queue<FMTParser::FMTLineInfo> FMTParser::_getAllLines(std::ifstream& p_stream) const
	{
		std::queue<FMTLineInfo> lines;
		try {
			while (p_stream.is_open())//Need to reopen this stream!!!
			{
				const std::string LINE = getCleanLine(p_stream);
				if (!LINE.empty())
				{
					lines.push(FMTLineInfo(LINE, m_line, m_location));
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTParser::_getAllLines", __LINE__, __FILE__, m_section);
			}
		return lines;
	}

	std::queue<FMTParser::FMTLineInfo> FMTParser::_processForLoopsNInclude(
		const std::vector<Core::FMTTheme>& p_themes,
		const Core::FMTConstants& p_cons,
		std::queue<FMTLineInfo>p_AllLines) const
	{
		try {
			if (!p_AllLines.empty())
				{
				_processForLoops(p_themes, p_cons, p_AllLines);
				p_AllLines = _tryInclude(p_themes, p_cons, p_AllLines);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTParser::_processForLoopsNInclude",
								__LINE__, __FILE__, m_section);
			}
		return p_AllLines;
	}

	std::queue<FMTParser::FMTLineInfo> FMTParser::_getCleanLinewfor(std::ifstream& p_stream,
																		const std::vector<Core::FMTTheme>& p_themes,
																		const Core::FMTConstants& p_cons) const
	{
		std::queue<FMTLineInfo> lines;
		try {
			lines = _getAllLines(p_stream);
			lines = _processForLoopsNInclude(p_themes,p_cons, lines);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTParser::_getCleanLinewfor", __LINE__, __FILE__, m_section);
			}
		return lines;
	}
/*
std::string FMTParser::_getCleanLinewfor(std::ifstream& stream,const std::vector<Core::FMTTheme>& themes,const Core::FMTConstants& cons)
    {
	boost::smatch kmatch;
	std::string line;
	try {
		while (!m_included.empty())
		{
			line = m_included.front();
			m_included.pop();
			return line;
		}
		if (m_ForValues.empty())
		{
			std::map<std::string, std::vector<std::string>>loops_replaces;
			std::map<std::string, std::pair<int, int>>forlocation;
			int lineid = 0;
			std::stack<std::string>keys;
			std::vector<std::pair<int, std::string>>Sequence;
			std::vector<std::string>ordered_keys;
			do {

				line = getCleanLine(stream);
				std::map<std::string,std::vector<std::string>>allValues;
				std::string target;
				bool gotfor = _getForLoops(line, themes, cons, allValues);
				if (!gotfor && line.find("FOREACH") != std::string::npos)
				{
					size_t OpeningBraces = 0;
					size_t ClosingBraces = 0;
					do {
						line += "\n"+getCleanLine(stream);
						OpeningBraces = std::count(line.begin(), line.end(), '(');
						ClosingBraces = std::count(line.begin(), line.end(), ')');

					} while (OpeningBraces>ClosingBraces);
					gotfor = _getForLoops(line, themes, cons, allValues);
				}
				if (gotfor)
				{
					//loops_replaces[target] = allvalues;
					//forlocation[target] = std::pair<int, int>(lineid, 0);
					//keys.push(target);
					loops_replaces.insert(allValues.begin(), allValues.end());
					for (const auto& LOOPS_ELEMENT : allValues)
						{
						forlocation[LOOPS_ELEMENT.first] = std::pair<int, int>(lineid, 0);
						keys.push(LOOPS_ELEMENT.first);
						}
				}
				else if (keys.empty())
				{
					return returnInclude(line, themes, cons);
				}
				else if (boost::regex_search(line, kmatch, FMTParser::m_END))
				{
					std::string endfor = keys.top();
					keys.pop();
					forlocation[endfor].second = lineid;
					ordered_keys.push_back(endfor);
				}
				else if (_isValid(line)) {
					Sequence.push_back(std::pair<int, std::string>(lineid, line));
					++lineid;
				}else if (!stream.is_open() && !keys.empty())
					{
						_exhandler->raise(Exception::FMTexc::FMTunclosedforloop,
							"Missing EndFor after line " + std::to_string(m_line), "FMTParser::_getCleanLinewfor", __LINE__, __FILE__, m_section);
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
				m_ForValues.push(nline.second);
			}
		}

		while (!m_ForValues.empty())
		{
			line = m_ForValues.front();
			m_ForValues.pop();
			return returnInclude(line, themes, cons);
		}
	return returnInclude(line, themes, cons);
	}catch (...)
	 {
		 _exhandler->raisefromcatch("", "FMTParser::_getCleanLinewfor", __LINE__, __FILE__, m_section);
	 }
	return std::string();
    }
*/

std::map<Core::FMTsection, std::string> FMTParser::_getPrimary(const std::string& primarylocation)
	{
		
		std::map<Core::FMTsection, std::string>targets;
		try {
			std::ifstream primarystream(primarylocation);
			const boost::filesystem::path primary_path(primarylocation);
			if (FMTParser::tryOpening(primarystream, primarylocation))
			{
				while (primarystream.is_open())
				{
					std::string line = getCleanLine(primarystream);
					if (_isValid(line))
					{
						boost::smatch kmatch;
						if (boost::regex_search(line, kmatch, m_PRIMARY))
						{
							const std::string file_name = std::string(kmatch[3]);
							const boost::filesystem::path file_path(file_name);
							std::string extension = file_path.extension().string();
							boost::to_lower(extension);
							const Core::FMTsection section = fromExtension(extension);
							targets[section] = (primary_path.parent_path() / file_path).string();
						}
					}
				}
			}
			if (targets.find(Core::FMTsection::Constants) == targets.end())
			{
				targets[Core::FMTsection::Constants] = "";
			}
		primarym_sections = targets;
		}catch (...)
			{
			_exhandler->raiseFromCatch("at "+ primarylocation,"FMTParser::_getPrimary", __LINE__, __FILE__, m_section);
			}
		return targets;
	}

std::array<std::string, 5>FMTParser::getBaseOperators() const
	{
	return std::array<std::string, 5>({ "=", "<=", ">=", "<", ">" });
	}

std::vector<std::string> FMTParser::_sameAs(const std::string& allset) const
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
					"Empty target "+allset, "FMTParser::_sameAs", __LINE__, __FILE__, m_section);
				}
            all_elements.push_back(realname);
			std::string sameAsstr = allset.substr(allset.find(separator)+separator.size());
            boost::trim(sameAsstr);
            all_elements.push_back(sameAsstr);
            }else{
			std::string realname = allset;
            boost::trim(realname);
            all_elements.push_back(realname);
            }
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTParser::_sameAs", __LINE__, __FILE__, m_section);
		}
        return all_elements;
        }

std::vector<std::vector<std::string>>FMTParser::readCsv(const std::string& location,const char& separator)
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
				Parser::FMTParser newParser;
				if (newParser.tryOpening(csvstream, location))
					{
					//bool inactualdevs = false;
					while (gotsomething)
						{
						if (newParser._safeGetline(csvstream, line))
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
		_exhandler->raiseFromCatch("at " + location ,"FMTParser::readCsv", __LINE__, __FILE__, m_section);
		}
	return lines;
	}




std::istream& FMTParser::_safeGetline(std::istream& is, std::string& t) const
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
		_exhandler->raiseFromCatch("", "FMTParser::_safeGetline", __LINE__, __FILE__, m_section);
	}
	return is;
	}

}
