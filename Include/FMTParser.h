/*
Copyright (c) 2019 Gouvernement du Québec
 
SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTparser_Hm_included
#define FMTparser_Hm_included

#include <boost/regex.hpp>
#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "FMTutility.h"
#include <array>
#include <ctime>


#include <boost/thread/recursive_mutex.hpp>



#include "FMTObject.h"
#if defined FMTWITHGDAL
	class OGRSpatialReference;
	class GDALDataset;
	class OGRLayer;
	class GDALRasterBand;
	class GDALDriver;
	class OGRSpatialReference;
	class OGRCoordinateTransformation;
#endif



namespace Core {
	class FMTTheme;
	class FMTAction;
	class FMTYields;
	class FMTSpec;
	class FMTPerBounds;
	class FMTConstants;
	template<typename T>
	class FMTBounds;
}

namespace Spatial
{
	template<typename T>
	class FMTLayer;
}

namespace Testing
{
	class UnitTestFMTParser;
}


/// Namespace handling all FMT's parsers. Everything related to I/O should be located in this namespace.
namespace Parser
{
// DocString: FMTParser
/**
@brief Main parent class of all the parsers in FMT.
@details Provides many functions common to all parsers for validation and error handling when turning a string into something usable in FMT.
*/
class FMTEXPORT FMTParser: public Core::FMTObject
    {
    public:
		// DocString: FMTParser()
		/**
		@brief Default constructor for FMTParser.
		*/
        FMTParser();
		// DocString: FMTParser(const FMTParser&)
		/**
		@brief Copy constructor for FMTParser.
		@param[in] rhs the FMTParser to copy.
		*/
        FMTParser(const FMTParser& rhs);
		// DocString: FMTParser::operator=
		/**
		@brief Copy assignment operator for FMTParser.
		@param[in] rhs the FMTParser to copy.
		@return a reference to this FMTParser.
		*/
        FMTParser& operator = (const FMTParser& rhs);
		// DocString: ~FMTParser()
		/**
		@brief Default destructor for FMTParser.
		*/
        virtual ~FMTParser()=default;
		// DocString: FMTParser::getMostRecentFiletime
		/**
		@brief Return the most recent file time of the files read by the parser.
		@return the most recent file time.
		*/
		std::time_t getMostRecentFiletime() const;
		// DocString: FMTParser::setHeader
		/**
		@brief Set a string as the header written when the file is opened in write mode, replacing the comment attribute.
		@param[in] header the header string.
		*/
		void setHeader(const std::string& header);
		#ifdef FMTWITHGDAL
		// DocString: FMTParser::_getWSFields
		/**
		@brief Create an OGR vector dataset based on a driver name and a location.
		@param[in] location the location of the dataset.
		@param[in] gdaldrivername the GDAL driver name.
		@return the created dataset.
		*/
		GDALDataset* createOGRDataset(
			std::string location = std::string(),
			std::string gdaldrivername = "CSV") const;
		// DocString: FMTParser::createLayer()
		/**
		@brief Create a layer in a dataset, deleting any existing layer with the same name.
		@param[in] dataset the dataset.
		@param[in] name the layer name.
		@param[in] creationoptions the creation options.
		@return the created layer.
		*/
		OGRLayer* createLayer(GDALDataset* dataset,
			const std::string& name, std::vector<std::string> creationoptions = std::vector<std::string>()) const;
		// DocString: FMTParser::getGDALVectorDriverNames
		/**
		@brief Return a vector of GDAL vector driver names.
		@param[in] testcreation if true only returns drivers that support creation.
		@return the GDAL vector driver names.
		*/
		std::vector<std::string>getGDALVectorDriverNames(bool testcreation = true) const;
		// DocString: FMTParser::getGDALRasterDriverNames
		/**
		@brief Return a vector of GDAL raster driver names.
		@param[in] testcreation if true only returns drivers that support creation.
		@return the GDAL raster driver names.
		*/
		std::vector<std::string>getGDALRasterDriverNames(bool testcreation = true) const;
		// DocString: FMTParser::getGDALVectorDriverExtensions
		/**
		@brief Return a vector of GDAL vector driver extensions organized by driver.
		@param[in] testcreation if true only returns drivers that support creation.
		@return the GDAL vector driver extensions organized by driver.
		*/
		std::vector<std::vector<std::string>>getGDALVectorDriverExtensions(bool testcreation = true) const;
		// DocString: FMTParser::getGDALRasterDriverExtensions
		/**
		@brief Return a vector of GDAL raster driver extensions organized by driver.
		@param[in] testcreation if true only returns drivers that support creation.
		@return the GDAL raster driver extensions organized by driver.
		*/
		std::vector<std::vector<std::string>>getGDALRasterDriverExtensions(bool testcreation = true) const;
		#endif
		// DocString: FMTParser::tryOpening
		/**
		@brief Open an input stream at a given location.
		@param[in] stream the input stream.
		@param[in] location the location.
		@return true if the stream is opened else false.
		*/
		bool tryOpening(const std::ifstream& stream, const std::string& location) const;
		// DocString: FMTParser::tryOpening
		/**
		@brief Open an output stream at a given location.
		@param[in,out] stream the output stream.
		@param[in] location the location.
		@return true if the stream is opened else false.
		*/
		bool tryOpening(std::ofstream& stream, const std::string& location) const;
		// DocString: FMTParser::getCleanLine
		/**
		@brief Clean a line and make it ready to be read by a parser.
		@param[in,out] stream the input stream.
		@return the cleaned line.
		*/
		std::string getCleanLine(std::istream& stream) const;
		// DocString: FMTParser::getCleanLine
		/**
		@brief Clean a line and make it ready to be read by a parser.
		@param[in] p_input the input line.
		@return the cleaned line.
		*/
		std::string getCleanLine(const std::string& p_input) const;
		// DocString: FMTParser::fromExtension
		/**
		@brief Return the FMTsection matching the extension of a file.
		@param[in] p_Extension the file extension.
		@return the FMTsection matching the extension.
		*/
		static Core::FMTsection fromExtension(const std::string& p_Extension);
		// DocString: FMTParser::createSubDirectory
		/**
		@brief Create a sub directory in directory
		@param[in] p_Directory the base directory
		@param[in]  p_SubDirectory the sub directory
		@return full directory path
		*/
		std::string createSubDirectory(const std::string& p_Directory, const std::string& p_SubDirectory);
	protected:
		// DocString: FMTParser::FMTLineInfo
		/**
		@brief Helper holding a line value, its number and its file, used during parsing.
		*/
		class FMTLineInfo
			{
			public:
				// DocString: FMTParser::FMTLineInfo(const std::string, int, const std::string&)
				/**
				@brief Construct a line info from a line, its number and its file.
				@param[in] p_line the line value.
				@param[in] p_number the line number.
				@param[in] p_file the file.
				*/
				FMTLineInfo(const std::string p_line, int p_number, const std::string& p_file);
				std::string m_lineValue;
				int m_lineNumber;
				std::string m_file;
			};
		// DocString: FMTParser::m_SEPARATOR
		///A regex for splitting general string.
		const static boost::regex m_SEPARATOR;
		// DocString: FMTParser::m_MTX
		///Mutex for multi-threading.
		mutable boost::recursive_mutex m_MTX;
		// DocString: FMTParser::FMTsection
		///The section in which the child class is in
		mutable Core::FMTsection m_section;
		// DocString: FMTParser::m_AYLD
		///A regex for capturing age/period/yield specification
		const static boost::regex m_AYLD;
		// DocString: FMTParser::m_AAGE
		///A regex for capturing age specificaiton
		const static boost::regex m_AAGE;
		// DocString: FMTParser::m_AYLD_AGE
		///A regex for capturing age or yield specification
		const static boost::regex m_AYLD_AGE;
		// DocString: FMTParser::m_BOUNDS
		///A regex for capturing bounds for yield or age specification
		const static boost::regex m_BOUNDS;
		// DocString: FMTParser::m_OPERATORS
		///A regex for capturing simple operators
		const static boost::regex m_OPERATORS;
		// DocString: FMTParser::m_PRIMARY
		///A regex for capturing the primary file
		const static boost::regex m_PRIMARY;
		// DocString: FMTParser::m_constrePlacement
		///Number of constant replaced by a numerical number
		mutable int m_constrePlacement;
		// DocString: FMTParser::m_line
		///Line id at which the parser is reading the values.
		mutable int m_line;
		// DocString: FMTParser::m_comment
		///Comment found in the actual line read by the parser.
		mutable std::string m_comment;
		// DocString: FMTParser::m_location
		///File location of the section read by the parser.
		mutable std::string m_location;
		// DocString: FMTParser::m_MostRecentFile
		///The most recent file time read by this parser.
		mutable std::time_t m_MostRecentFile;
		// DocString: FMTParser::_setSection
		/**
		@brief Set the section member of the parser.
		@param[in] section the section to set.
		*/
		void _setSection(const Core::FMTsection& section) const;
	#ifdef FMTWITHGDAL
		// DocString: FMTParser::getallGDALdrivernames
		/**
		@brief Return the GDAL drivers for a given spatial type.
		@param[in] spatialtype the spatial type (raster or vector).
		@param[in] testcreation if true only returns drivers that support creation.
		@return the GDAL drivers.
		*/
		std::vector<GDALDriver*> _getAllGDALDrivers(const char* spatialtype, bool testcreation = true) const;
		// DocString: FMTParser::getGDALExtensions
		/**
		@brief Return the GDAL extensions organized by driver for a given spatial type.
		@param[in] spatialtype the spatial type (raster or vector).
		@param[in] testcreation if true only returns drivers that support creation.
		@return the GDAL extensions organized by driver.
		*/
		std::vector<std::vector<std::string>>getGDALExtensions(const char* spatialtype, bool testcreation = true) const;
		// DocString: FMTParser::_setCategories
		/**
		@brief Write the categories in the band information.
		@param[in,out] band the raster band.
		@param[in] categories the categories.
		*/
		void _setCategories(GDALRasterBand* band, const std::vector<std::string>& categories) const;
		// DocString: FMTAreaParser::_getProjTransform
		/**
		@brief Return a coordinate transformation for a layer, optionally fitting to FOREL.
		@param[in] baselayer the base layer.
		@param[in] fittoforel if true fits to FOREL.
		@return the coordinate transformation.
		*/
		OGRCoordinateTransformation* _getProjTransform(OGRLayer* baselayer, bool fittoforel = true) const;
		// DocString: FMTAreaParser::_getTransFormMemLayerCopy
		/**
		@brief Create a memory dataset copying a layer with a new field, optionally with a new spatial reference.
		@param[in] baselayer the base layer.
		@param[in] newreference the new spatial reference.
		@param[in] fieldname the new field name.
		@return the memory dataset.
		*/
		GDALDataset* _getTransFormMemLayerCopy(OGRLayer* baselayer, const OGRSpatialReference* newreference, const std::string& fieldname) const;
		// DocString: FMTParser::_getFORELSpatialRef
		/**
		@brief Return the spatial reference used for FORELs in Quebec.
		@return the FOREL spatial reference.
		*/
		std::unique_ptr<OGRSpatialReference> _getFORELSpatialRef() const;
		// DocString: FMTParser::_createDataset
		/**
		@brief Create an empty GDAL dataset for a layer.
		@tparam T the layer value type.
		@param[in] location the location.
		@param[in] layer the layer.
		@param[in] datatypeid the data type id.
		@param[in] format the format.
		@return the created dataset.
		*/
		template<typename T>
		GDALDataset* _createDataset(const std::string& location, const Spatial::FMTLayer<T>& layer, const int datatypeid, std::string format = "GTiff") const;
		// DocString: FMTParser::_getDataset
		/**
		@brief Open a GDAL dataset in read only from a location, throwing on error.
		@param[in] location the location.
		@return the dataset.
		*/
		GDALDataset* _getDataset(const std::string& location) const;
		// DocString: FMTParser::_getVectorDataset
		/**
		@brief Open a GDAL vector dataset in read only from a location, throwing on error.
		@param[in] location the location.
		@return the dataset.
		*/
		GDALDataset* _getVectorDataset(const std::string& location) const;
		// DocString: FMTParser::_getLayer
		/**
		@brief Open a GDAL layer from a vector dataset by its id, throwing on error.
		@param[in] dataset the dataset.
		@param[in] id the layer id.
		@return the layer.
		*/
		OGRLayer* _getLayer(GDALDataset* dataset, int id) const;
		// DocString: FMTParser::_getBand
		/**
		@brief Open a GDAL raster band from a dataset.
		@param[in] dataset the dataset.
		@param[in] bandid the band id.
		@return the raster band.
		*/
		GDALRasterBand* _getBand(GDALDataset* dataset, int bandid = 1) const;
		// DocString: FMTParser::_createBand
		/**
		@brief Create a GDAL raster band in a dataset with categories.
		@param[in] dataset the dataset.
		@param[in] categories the categories.
		@param[in] bandid the band id.
		@return the raster band.
		*/
		GDALRasterBand* _createBand(GDALDataset* dataset, const std::vector<std::string>& categories, int bandid = 1) const;
		// DocString: FMTParser::_getCat
		/**
		@brief Return the categories of a dataset for a given band.
		@param[in] dataset the dataset.
		@param[in] bandid the band id.
		@return the categories.
		*/
		std::vector<std::string> _getCat(GDALDataset* dataset, int bandid = 1) const;
		// DocString: FMTParser::_getOverview
		/**
		@brief Return an overview of a raster band.
		@param[in] band the raster band.
		@param[in] view the overview index.
		@return the overview raster band.
		*/
		GDALRasterBand* _getOverview(GDALRasterBand* band, int view = 1) const;
		// DocString: FMTParser::_getWSFields
		/**
		@brief Fill up the field ids of the themes, age, area and lock of a layer.
		@param[in] layer the layer.
		@param[in,out] themes the theme field ids.
		@param[in,out] age the age field id.
		@param[in,out] area the area field id.
		@param[in,out] lock the lock field id.
		@param[in] agefield the age field name.
		@param[in] areafield the area field name.
		@param[in] lockfield the lock field name.
		*/
		void _getWSFields(OGRLayer* layer, std::map<int, int>& themes, int& age, int& area, int& lock, std::string agefield = "", std::string areafield = "", std::string lockfield = "") const;
		// DocString: FMTParser::_createVectorMemoryDs
		/**
		@brief Create a vector memory dataset.
		@return the memory dataset.
		*/
		GDALDataset* _createVectorMemoryDs() const;
	#endif
	
		// DocString: FMTParser::_isValidFile
		/**
		@brief Check if a file location is valid.
		@param[in] location the location.
		@return true if the file is valid else false.
		*/
		bool _isValidFile(const std::string& location) const;
		// DocString: FMTParser::_isValid
		/**
		@brief Check if a line is valid and clean it.
		@param[in] line the line.
		@return true if the line is valid else false.
		*/
		bool _isValid(const std::string& line) const;
		// DocString: FMTParser::regexLoop
		/**
		@brief Capture a for loop from a line and return the variables to loop on.
		@param[in] cutregex the regex.
		@param[in,out] str the line.
		@return the variables to loop on.
		*/
		std::vector<std::string>regexLoop(const boost::regex& cutregex, std::string& str) const;
		// DocString: FMTParser::spliter
		/**
		@brief Split a string into multiple strings.
		@param[in] strmask the string to split.
		@param[in] xspliter the splitting regex.
		@return the split strings.
		*/
		std::vector<std::string>spliter(std::string strmask, const boost::regex& xspliter) const;
		// DocString: FMTParser::_clearComments
		/**
		@brief Clear the comments from a line and fill up the comment member.
		@param[in,out] line the line.
		*/
		void _clearComments(std::string& line) const;
		// DocString: FMTParser::_upper
		/**
		@brief Convert a string to upper case.
		@param[in] lowercases the string to convert.
		@return the upper case string.
		*/
		std::string _upper(const std::string& lowercases) const;
		// DocString: FMTParser::returnInclude
		/**
		@brief Return the next lines to be read by the parser, considering included files.
		@param[in] p_themes the model themes.
		@param[in] p_cons the model constants.
		@param[in] p_ForOut the output of the for loops unroll.
		@return a queue of lines to process.
		*/
		virtual std::queue<FMTLineInfo> _tryInclude(const std::vector<Core::FMTTheme>& p_themes, const Core::FMTConstants& p_cons,
															std::queue<FMTLineInfo>p_ForOut) const;

		// DocString: FMTParser::_getCleanLinewfor
		/**
		@brief Get a clean line considering for loops when reading the file.
		@param[in,out] p_stream the file stream.
		@param[in] p_themes the model themes.
		@param[in] p_cons the model constants.
		@return a queue of lines to process.
		*/
		virtual std::queue<FMTLineInfo> _getCleanLinewfor(std::ifstream& p_stream, const std::vector<Core::FMTTheme>& p_themes,
																const Core::FMTConstants& p_cons) const;
		// DocString: FMTParser::_getAllLines
		/**
		@brief Get all the lines of a file.
		@param[in,out] p_stream the file stream.
		@return a queue of lines to process.
		*/
		std::queue<FMTLineInfo> _getAllLines(std::ifstream& p_stream) const;
		// DocString: FMTParser::_processForLoopsNInclude
		/**
		@brief Process the for loops and include lines.
		@param[in] p_themes the model themes.
		@param[in] p_cons the model constants.
		@param[in] p_AllLines all the lines read from the file.
		@return a queue of lines to process.
		*/
		std::queue<FMTLineInfo> _processForLoopsNInclude(
			const std::vector<Core::FMTTheme>& p_themes,
			const Core::FMTConstants& p_cons,
			std::queue<FMTLineInfo>p_AllLines) const;
		// DocString: FMTParser::_isNum
		/**
		@brief Return true if the value is a number.
		@param[in] value the value.
		@return true if the value is a number else false.
		*/
		bool _isNum(std::string value) const;
		// DocString: FMTParser::_isNum
		/**
		@brief Return true if the value is a number, raising if it is not a valid number when throwerror is true.
		@param[in] value the value.
		@param[in] constant the constants.
		@param[in] throwerror if true raises on a non valid number.
		@return true if the value is a number else false.
		*/
		bool _isNum(const std::string& value, const Core::FMTConstants& constant, bool throwerror = true) const;
		// DocString: FMTParser::_getNum
		/**
		@brief Return a numeric value from a string based on the constants and a period.
		@tparam T the numeric type.
		@param[in] value the value.
		@param[in] constant the constants.
		@param[in] period the period.
		@return the numeric value.
		*/
		template<typename T>
		T _getNum(const std::string& value, const Core::FMTConstants& constant, int period = 0) const;
		// DocString: FMTParser::_getNum
		/**
		@brief Return a numeric value from a string.
		@tparam T the numeric type.
		@param[in] value the value.
		@param[in] omitnumtest if true omits the number test.
		@return the numeric value.
		*/
		template<typename T>
		T _getNum(const std::string& value, bool omitnumtest = false) const;
		// DocString: FMTParser::_tryFillNumber
		/**
		@brief Try to get a number from a string, returning false on failure.
		@tparam T the numeric type.
		@param[out] number the number.
		@param[in] value the value.
		@param[in] constant the constants.
		@param[in] period the period.
		@return true if the number is filled else false.
		*/
		template<typename T>
		bool _tryFillNumber(T& number, const std::string& value, const Core::FMTConstants& constant, int period = 0) const;
		// DocString: FMTParser::bounds
		/**
		@brief Return a FMTBounds from a value based on the constants and a section.
		@tparam T the bounded value type.
		@param[in] constants the constants.
		@param[in] value the value.
		@param[in] ope the operator.
		@param[in] section the section.
		@return the bounds.
		*/
		template<typename T>
		Core::FMTBounds<T>bounds(const Core::FMTConstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const;
		// DocString: FMTParser::getBaseOperators
		/**
		@brief Return the operators normally found in the different sections.
		@return the base operators.
		*/
		std::array<std::string, 5>getBaseOperators() const;
		// DocString: FMTParser::_sameAs
		/**
		@brief Handle the _sameAs keyword.
		@param[in] allset the set string.
		@return the resulting strings.
		*/
		std::vector<std::string> _sameAs(const std::string& allset) const;
		// DocString: FMTParser::_getPrimary
		/**
		@brief Return the file locations of the different sections from a primary file.
		@param[in] primarylocation the primary file location.
		@return a map of section to file location.
		*/
		std::map<Core::FMTsection, std::string> _getPrimary(const std::string& primarylocation);
		// DocString: FMTParser::_isYld
		/**
		@brief Check if a value is a valid yield.
		@param[in] p_ylds the yield section.
		@param[in] p_value the value.
		@param[in] pm_section the section.
		@param[in] p_throwError if true raises if the value is not a yield.
		@return true if the value is a yield else false.
		*/
		bool _isYld(const Core::FMTYields& p_ylds, const std::string& p_value,
			Core::FMTsection pm_section, bool p_throwError = true) const;
		// DocString: FMTParser::_isAct
		/**
		@brief Check if a value is an action.
		@param[in] section the section.
		@param[in] actions the actions.
		@param[in] action the value.
		@return true if the value is an action else false.
		*/
		bool _isAct(Core::FMTsection section, const std::vector<Core::FMTAction>& actions, std::string action) const;
		// DocString: FMTParser::_setSpecs
		/**
		@brief Set the specifications of one or multiple specs and return the rest of the line.
		@param[in] section the section.
		@param[in] key the keyword.
		@param[in] ylds the yields.
		@param[in] constants the constants.
		@param[in,out] specs the specifications.
		@param[in] line the line.
		@return the rest of the line.
		*/
		std::string _setSpecs(Core::FMTsection section, Core::FMTkwor key, const Core::FMTYields& ylds, const Core::FMTConstants& constants, std::vector<Core::FMTSpec>& specs, const std::string& line);
		// DocString: FMTParser::_setSpec
		/**
		@brief Set the specification of a spec and return the rest of the line.
		@param[in] section the section.
		@param[in] key the keyword.
		@param[in] ylds the yields.
		@param[in] constants the constants.
		@param[in,out] spec the specification.
		@param[in] line the line.
		@return the rest of the line.
		*/
		std::string _setSpec(Core::FMTsection section, Core::FMTkwor key, const Core::FMTYields& ylds, const Core::FMTConstants& constants, Core::FMTSpec& spec, const std::string& line);
		// DocString: FMTParser::readCsv
		/**
		@brief Read a CSV file from a location using a separator.
		@param[in] location the location.
		@param[in] separator the separator.
		@return a vector of vector of lines.
		*/
		std::vector<std::vector<std::string>>readCsv(const std::string& location, const char& separator);
		// DocString: FMTParser::_getPerBound
		/**
		@brief Return the period bound of an input.
		@param[in] p_lower the lower bound.
		@param[in] p_upper the upper bound.
		@param[in] p_constants the constants.
		@return the period bound.
		*/
		Core::FMTPerBounds _getPerBound(const std::string& p_lower,
			const std::string& p_upper,
			const Core::FMTConstants& p_constants) const;
		// DocString: FMTParser::_setPeriodWithBounds
		/**
		@brief Set the periods of a spec from a lower and upper bound.
		@param[in,out] p_spec the spec.
		@param[in] p_lower the lower bound.
		@param[in] p_upper the upper bound.
		@param[in] p_constants the constants.
		*/
		void _setPeriodWithBounds(Core::FMTSpec& p_spec,
			const std::string& p_lower,
			const std::string& p_upper,
			const Core::FMTConstants& p_constants) const;
		// DocString: FMTParser::_setPeriods
		/**
		@brief Set the periods of a spec from a parsed period string.
		@param[in,out] p_spec the spec.
		@param[in] p_periods the period string.
		@param[in] p_constants the constants.
		@return true if the periods are set else false.
		*/
		bool _setPeriods(Core::FMTSpec& p_spec,
			const std::string& p_periods,
			const Core::FMTConstants& p_constants) const;
		// DocString: FMTParser::_getForLoops
		/**
		@brief Fill up the for loops queue with the targeted variables.
		@param[in] p_line the for loop header line.
		@param[in] p_themes the model themes.
		@param[in] p_cons the model constants.
		@return the for loop values with the target as key.
		*/
		std::map<std::string, std::vector<std::string>> _getForLoops(const std::string& p_line,
			const std::vector<Core::FMTTheme>& p_themes,
			const Core::FMTConstants& p_cons) const;
		// DocString: FMTParser::_getForLoops
		/**
		@brief Unroll the for loops in the queue and set the results in the queue.
		@param[in] p_themes the model themes.
		@param[in] p_constants the model constants.
		@param[in,out] p_queue the resulting queue of lines.
		*/
		void _processForLoops(const std::vector<Core::FMTTheme>& p_themes,
			const Core::FMTConstants& p_constants,
			std::queue<FMTLineInfo>& p_queue) const;
		// DocString: FMTParser::_isForLoops
		/**
		@brief Return true if a line is a for loop header.
		@param[in] p_line the line.
		@return true if the line is a for loop else false.
		*/
		bool _isForLoops(const std::string& p_line) const;
		// DocString: FMTParser::_isForLoopsEnd
		/**
		@brief Return true if a line is the end of a for loop.
		@param[in] p_line the line.
		@return true if the line is the end of a for loop else false.
		*/
		bool _isForLoopsEnd(const std::string& p_line) const;
		// DocString: FMTParser:::getLine
		/**
		@brief Get and pop the front line of a queue.
		@param[in,out] p_Lines the queue of lines.
		@return the front line value.
		*/
		std::string _getLine(std::queue<FMTLineInfo>& p_Lines) const;
	private:
		friend class Testing::UnitTestFMTParser;
		// DocString: FMTParser::m_VECTOR_THEME
		///Regex to capture theme.
		const static boost::regex m_VECTOR_THEME;
		// DocString: FMTParser::m_NUMBER
		///Regex to capture constants or numeric value
		const static boost::regex m_NUMBER;
		// DocString: FMTParser::m_REMOVE_COMMENT
		///Regex to capture comments 
		const static boost::regex m_REMOVE_COMMENT;
		// DocString: FMTParser::m_VALID
		///Regex to validate if a string is valid for the parser.
		const static boost::regex m_VALID;
		// DocString: FMTParser::m_INCLUDE
		///Regex to catch the include keyword.
		const static boost::regex m_INCLUDE;
		// DocString: FMTParser::m_FOR
		///Regex to capture for loops.
		const static boost::regex m_FOR;
		// DocString: FMTParser::m_END
		///Regex to capture the end of forloops.
		const static boost::regex m_END;
		// DocString: FMTParser::m_PERIODS
		///Regex to capture period bounds.
		const static boost::regex m_PERIODS;
		// DocString: FMTParser::primarym_sections
		///When the modelparser is used this value is saved each time calling FMTParser::getPrimary() allow use to use _PRIMARY key word in include files.
		static std::map<Core::FMTsection, std::string>primarym_sections;
		// DocString: FMTParser::m_inComment
		///Is true if the parser is in a bracket comment {} else false
		mutable bool m_inComment;
		// DocString: FMTParser::m_ForValues
		///lines comming from a forloops present in the file
		//std::queue<std::string>m_ForValues;
		// DocString: FMTParser::m_included
		///lines comming from an included file.
		//std::queue<std::string>m_included;
		// DocString: FMTParser::gdalInitialization;
		///True if GDAL has been initialized.
		static bool gdalInitialization;
		// DocString: FMTParser::_safeGetline
		/**
		@brief Safe getline function for Linux and Windows.
		@param[in,out] is the input stream.
		@param[in,out] t the line.
		@return the input stream.
		*/
		std::istream& _safeGetline(std::istream& is, std::string& t) const;
		#ifdef FMTWITHGDAL
			// DocString: FMTParser::gdalInitialization
			/**
			Initialize GDAL once
			*/
			static void _initializeGDAL();
			// DocString: FMTParser::_queryDatabase
			/**
			@brief Return the for loop query lines based on a key and a field name.
			@param[in] p_DataBaseLocation the database location.
			@param[in] p_VariableName the variable name.
			@param[in] p_Query the query.
			@return the query lines with the variable.
			*/
			std::map<std::string, std::vector<std::string>>_queryDatabase(
				const std::string& p_DataBaseLocation,
				const std::string& p_VariableName,
				const std::string& p_Query) const;
		#endif
			// DocString: FMTParser::_getAbsolutePath
			/**
			@brief Return the absolute path of a path.
			@param[in] p_Path the path.
			@return the absolute path.
			*/
			std::string _getAbsolutePath(std::string p_Path) const;
			// DocString: FMTParser::_setForLoopLines
			/**
			@brief Concatenate lines to form a foreach when the foreach is on multiple lines.
			@param[in,out] p_queue the queue of lines.
			@return the front line.
			*/
			FMTLineInfo _setForLoopLines(std::queue<FMTLineInfo>& p_queue) const;
			// DocString: FMTParser:: _processConstants
			/**
			@brief Turn a string with constants into a string with regular numbers.
			@param[in] p_input the input string.
			@param[in] p_constants the constants.
			@return a valid string without constants.
			*/
			std::string _processConstants(std::string p_input,
				const Core::FMTConstants& p_constants) const;
    };

}

#endif
