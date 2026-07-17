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
The FMTParser is the main parent class of all the other FMTParser available in FMT.
It has alot of usefull function common to all parser for validation and error throw when turning a std::string to something usefull in FMT.
*/
class FMTEXPORT FMTParser: public Core::FMTObject
    {
    public:
		// DocString: FMTParser()
		/**
		Default constructor for FMTParser.
		*/
        FMTParser();
		// DocString: FMTParser(const FMTParser&)
		/**
		Default copy constructor for FMTParser.
		*/
        FMTParser(const FMTParser& rhs);
		// DocString: FMTParser::operator=
		/**
		Default copy assignment for FMTParser.
		*/
        FMTParser& operator = (const FMTParser& rhs);
		// DocString: ~FMTParser()
		/**
		Default destructor for FMTParser.
		*/
        virtual ~FMTParser()=default;
		// DocString: FMTParser::getMostRecentFiletime
		/**
		Get the most recentfiletime of the files read by ther parser.
		*/
		std::time_t getMostRecentFiletime() const;
		// DocString: FMTParser::setHeader
		/**
		Taking a string has argument set this string has an header in write mode
		when the file is opened. It will replace the m_comment attribute with the header string.
		*/
		void setHeader(const std::string& header);
		#ifdef FMTWITHGDAL
		// DocString: FMTParser::getWSFields
		/**
		Will Create a OGR vector file based on a given drivername and a location.
		*/
		GDALDataset* createOGRDataset(
			std::string location = std::string(),
			std::string gdaldrivername = "CSV") const;
		// DocString: FMTParser::createLayer()
		/**
		Create a layer with from a dataset, it will delete layer with the same name before creation if it exist in the dataset
		*/
		OGRLayer* createLayer(GDALDataset* dataset,
			const std::string& name, std::vector<std::string> creationoptions = std::vector<std::string>()) const;
		// DocString: FMTParser::getGDALVectorDriverNames
		/**
		Return a vector of GDAL vector driver names
		*/
		std::vector<std::string>getGDALVectorDriverNames(bool testcreation = true) const;
		// DocString: FMTParser::getGDALRasterDriverNames
		/**
		Return a vector of GDAL raster driver names
		*/
		std::vector<std::string>getGDALRasterDriverNames(bool testcreation = true) const;
		// DocString: FMTParser::getGDALVectorDriverExtensions
		/**
		Return a vector(organize the extentions by drivers) of vector of GDAL vector driver extensions
		*/
		std::vector<std::vector<std::string>>getGDALVectorDriverExtensions(bool testcreation = true) const;
		// DocString: FMTParser::getGDALRasterDriverExtensions
		/**
		Return a vector(organize the extentions by drivers) of vector of GDAL raster driver extensions
		*/
		std::vector<std::vector<std::string>>getGDALRasterDriverExtensions(bool testcreation = true) const;
		#endif
		// DocString: FMTParser::tryOpening
		/**
		Open a input stream at a given location.
		*/
		bool tryOpening(const std::ifstream& stream, const std::string& location) const;
		// DocString: FMTParser::tryOpening
		/**
		Open a output stream at a given location.
		*/
		bool tryOpening(std::ofstream& stream, const std::string& location) const;
		// DocString: FMTParser::getCleanLine
		/**
		Clean a line and make it ready to be read by a parser.
		*/
		std::string getCleanLine(std::istream& stream) const;
		// DocString: FMTParser::getCleanLine
		/**
		@brief Clean a line and make it ready to be read by a parser.
		@param[in] p_input input file.
		@return cleaned line.
		*/
		std::string getCleanLine(const std::string& p_input) const;
		// DocString: FMTParser::fromExtension
		/**
		@brief Get the FMTsection from the extension of the file.
		@param[in] Return the FMTsection given the extension of the file.
		@return the FMTsection that fit with the p_Extension
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
		class FMTLineInfo
			{
			public:
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
		// DocString: FMTParser::setSection
		/**
		It sets the section member of the FMTObject.
		*/
		void setSection(const Core::FMTsection& section) const;
	#ifdef FMTWITHGDAL
		// DocString: FMTParser::getallGDALdrivernames
		/**
		Return a vector of GDAL drivers for a given spatialtype (raster/vector)
		*/
		std::vector<GDALDriver*> getAllGDALDrivers(const char* spatialtype, bool testcreation = true) const;
		// DocString: FMTParser::getGDALExtensions
		/**
		Return a vector(organize the extentions by drivers) of vector of GDAL extensions accepted for the given spatialtype.
		*/
		std::vector<std::vector<std::string>>getGDALExtensions(const char* spatialtype, bool testcreation = true) const;
		// DocString: FMTParser::setCategories
		/**
		Write the categories in the band information
		*/
		void setCategories(GDALRasterBand* band, const std::vector<std::string>& categories) const;
		// DocString: FMTAreaParser::getProjTransform
		/**
		Create a memory layer with a new int field named fieldname based on forel.
		Based on the baselayer.
		*/
		OGRCoordinateTransformation* getProjTransform(OGRLayer* baselayer, bool fittoforel = true) const;
		// DocString: FMTAreaParser::getTransFormMemLayerCopy
		/**
		Create a memory layer with a new int field named fieldname based on forel.
		Based on the baselayer. With a new spatial reference or can be the same has baselayer.
		*/
		GDALDataset* getTransFormMemLayerCopy(OGRLayer* baselayer, const OGRSpatialReference* newreference, const std::string& fieldname) const;
		// DocString: FMTParser::getFORELSpatialRef
		/**
		Return and OGRspatialReference corresponding to the one used for FORELs in Quebec.
		*/
		std::unique_ptr<OGRSpatialReference> getFORELSpatialRef() const;
		// DocString: FMTParser::createDataset
		/**
		The function create an empty GDALDataset for a given FMTLayer.
		*/
		template<typename T>
		GDALDataset* createDataset(const std::string& location, const Spatial::FMTLayer<T>& layer, const int datatypeid, std::string format = "GTiff") const;
		// DocString: FMTParser::getDataset
		/**
		Open in readonly a GDALdataset from a given (location), will throw if anything went wrong.
		*/
		GDALDataset* getDataset(const std::string& location) const;
		// DocString: FMTParser::getVectorDataset
		/**
		Open in readonly a GDALdataset from a given (location) of a vector file, will throw if anything went wrong.
		*/
		GDALDataset* getVectorDataset(const std::string& location) const;
		// DocString: FMTParser::getLayer
		/**
		Open a GDALlayer from a vector GDALdataset for a given layer (id) and a actual (dataset), will throw if anything went wrong.
		*/
		OGRLayer* getLayer(GDALDataset* dataset, int id) const;
		// DocString: FMTParser::getBand
		/**
		Open a GDALrasterband from a given GDALdataset and a (bandid).
		*/
		GDALRasterBand* getBand(GDALDataset* dataset, int bandid = 1) const;
		// DocString: FMTParser::createBand
		/**
		Create a GDALrasterband in a GDALdataset where you can add categories.
		*/
		GDALRasterBand* createBand(GDALDataset* dataset, const std::vector<std::string>& categories, int bandid = 1) const;
		// DocString: FMTParser::getCat
		/**
		Get categories of a GDALdataset for a given (bandid).
		*/
		std::vector<std::string> getCat(GDALDataset* dataset, int bandid = 1) const;
		// DocString: FMTParser::getOverview
		/**
		Get an overview of a GDALraserband.
		*/
		GDALRasterBand* getOverview(GDALRasterBand* band, int view = 1) const;
		// DocString: FMTParser::getWSFields
		/**
		Will fill up all the fields id of the themes,age,area,lock of a given (layer).
		*/
		void getWSFields(OGRLayer* layer, std::map<int, int>& themes, int& age, int& area, int& lock, std::string agefield = "", std::string areafield = "", std::string lockfield = "") const;
		// DocString: FMTParser::createVectorMemoryDs
		/**

		*/
		GDALDataset* createVectorMemoryDs() const;
	#endif
	
		// DocString: FMTParser::isValidFile
		/**
		Check if a file location is valid.
		*/
		bool isValidFile(const std::string& location) const;
		// DocString: FMTParser::isValid
		/**
		Check if line is valid and clean it.
		*/
		bool isValid(const std::string& line) const;
		// DocString: FMTParser::regexLoop
		/**
		Capture a forloops from a line and return the variables to loop on.
		*/
		std::vector<std::string>regexLoop(const boost::regex& cutregex, std::string& str) const;
		// DocString: FMTParser::spliter
		/**
		Split a string into multiple strings.
		*/
		std::vector<std::string>spliter(std::string strmask, const boost::regex& xspliter) const;
		// DocString: FMTParser::clearComments
		/**
		Clear comments from a line and fill up the m_comment private value.
		*/
		void clearComments(std::string& line) const;
		// DocString: FMTParser::upper
		/**
		Convert a string to upper case string.
		*/
		std::string upper(const std::string& lowercases) const;
		// DocString: FMTParser::returnInclude
		/**
		@brief return the next line to be read by the parser considering included files.
		@param[in] p_themes model themes
		@param[in] p_const the model constants
		@param[in] p_ForOut is the output of for loops unrol
		@return a queue of lines to process.
		*/
		virtual std::queue<FMTLineInfo> tryInclude(const std::vector<Core::FMTTheme>& p_themes, const Core::FMTConstants& p_cons,
															std::queue<FMTLineInfo>p_ForOut) const;

		// DocString: FMTParser::getCleanLinewfor
		/**
		@brief Get a clean line and consider for loops when reading the file.
		@param[p_stream] p_stream  is the file stream
		@param[in] p_themes model themes
		@param[in] p_const the model constants
		@return a queue of lines to process
		*/
		virtual std::queue<FMTLineInfo> getCleanLinewfor(std::ifstream& p_stream, const std::vector<Core::FMTTheme>& p_themes,
																const Core::FMTConstants& p_cons) const;
		// DocString: FMTParser::getAllLines
		/**
		@brief Get all lines of a file
		@param[p_stream] p_stream  is the file stream
		@return a queue of lines to process
		*/
		std::queue<FMTLineInfo> getAllLines(std::ifstream& p_stream) const;
		// DocString: FMTParser::processForLoopsNInclude
		/**
		@brief process for loops and include lines
		@param[in] p_themes model themes
		@param[in] p_const the model constants
		@param[in] p_AllLines all lines read from file or pre processed
		@return a queue of lines to process.
		*/
		std::queue<FMTLineInfo> processForLoopsNInclude(
			const std::vector<Core::FMTTheme>& p_themes,
			const Core::FMTConstants& p_cons,
			std::queue<FMTLineInfo>p_AllLines) const;
		// DocString: FMTParser::isNum
		/**
		Return true if the value is a number.
		*/
		bool isNum(std::string value) const;
		// DocString: FMTParser::isNum
		/**
		Return true if the value is a number else it will raise (if throwerror = true) being non valid number.
		*/
		bool isNum(const std::string& value, const Core::FMTConstants& constant, bool throwerror = true) const;
		// DocString: FMTParser::getNum
		/**
		Template function to get a numeric value from a string (value) based on constant and period.
		*/
		template<typename T>
		T getNum(const std::string& value, const Core::FMTConstants& constant, int period = 0) const;
		// DocString: FMTParser::getNum
		/**
		Template function to get a numeric value from a string (value).
		*/
		template<typename T>
		T getNum(const std::string& value, bool omitnumtest = false) const;
		// DocString: FMTParser::tryFillNumber
		/**
		Try to get a number from a string (value) return false if failed.
		*/
		template<typename T>
		bool tryFillNumber(T& number, const std::string& value, const Core::FMTConstants& constant, int period = 0) const;
		// DocString: FMTParser::bounds
		/**
		Templated function to get a FMTBounds from a (value) base on constants and a (section).
		*/
		template<typename T>
		Core::FMTBounds<T>bounds(const Core::FMTConstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const;
		// DocString: FMTParser::getBaseOperators
		/**
		Gives a vector of operators normaly found in the different sections.
		*/
		std::array<std::string, 5>getBaseOperators() const;
		// DocString: FMTParser::sameAs
		/**
		Used to deal with the _sameAs keyword.
		*/
		std::vector<std::string> sameAs(const std::string& allset) const;
		// DocString: FMTParser::getPrimary
		/**
		Using a primaryfile it returns the path to the different sections file locations.
		*/
		std::map<Core::FMTsection, std::string> getPrimary(const std::string& primarylocation);
		// DocString: FMTParser::isYld
		/**
		@brief valid if the value is a valid yield.
		@param[in] p_ylds the yield section
		@param[in] p_value the input value
		@param[in] pm_section the section
		@param[in] p_trhowError if true will throw if it is not a yield.
		@return true if it is a yield.
		*/
		bool isYld(const Core::FMTYields& p_ylds, const std::string& p_value,
			Core::FMTsection pm_section, bool p_throwError = true) const;
		// DocString: FMTParser::isAct
		/**
		Test if a value (action) is actualy an action.
		*/
		bool isAct(Core::FMTsection section, const std::vector<Core::FMTAction>& actions, std::string action) const;
		// DocString: FMTParser::setSpecs
		/**
		Set the specifications of a given or multiple (specs) and return the rest of line.
		*/
		std::string setSpecs(Core::FMTsection section, Core::FMTkwor key, const Core::FMTYields& ylds, const Core::FMTConstants& constants, std::vector<Core::FMTSpec>& specs, const std::string& line);
		// DocString: FMTParser::setSpec
		/**
		Set the specifications of a given (spec) and return the rest of line.
		*/
		std::string setSpec(Core::FMTsection section, Core::FMTkwor key, const Core::FMTYields& ylds, const Core::FMTConstants& constants, Core::FMTSpec& spec, const std::string& line);
		// DocString: FMTParser::readCsv
		/**
		Read a csv file from a (location) using a (separator) and returns a vector of vector of lines.
		*/
		std::vector<std::vector<std::string>>readCsv(const std::string& location, const char& separator);
		// DocString: FMTParser::getPerBound
		/**
		@brief get the period bound of an inputs
		@param[in] p_lower bound
		@param[in] p_upper bound
		@param[in] p_constants constants
		@return a filled up bound.
		*/
		Core::FMTPerBounds getPerBound(const std::string& p_lower,
			const std::string& p_upper,
			const Core::FMTConstants& p_constants) const;
		// DocString: FMTParser::setPeriodWithBounds
		/**
		@brief set periods to a spec.
		@param[out] The spec
		@param[in] period lower
		@param[in] period upper
		@param[in] the constants
		@return true if set else false.
		*/
		void setPeriodWithBounds(Core::FMTSpec& p_spec,
			const std::string& p_lower,
			const std::string& p_upper,
			const Core::FMTConstants& p_constants) const;
		// DocString: FMTParser::setPeriods
		/**
		@brief set periods to a spec.
		@param[out] The spec
		@param[in] the period string parsed
		@param[in] the constants
		@return true if set else false.
		*/
		bool setPeriods(Core::FMTSpec& p_spec,
			const std::string& p_periods,
			const Core::FMTConstants& p_constants) const;
		// DocString: FMTParser::getForLoops
		/**
		@brief Fill up the for loops queue p_allValues with targeted variables
		@param[in] p_line is the for loop header line
		@param[in] p_themes the model themes
		@param[in] p_cons the model constants
		@return the for loops values with target has key
		*/
		std::map<std::string, std::vector<std::string>> getForLoops(const std::string& p_line,
			const std::vector<Core::FMTTheme>& p_themes,
			const Core::FMTConstants& p_cons) const;
		// DocString: FMTParser::getForLoops
		/**
		@brief Unrol for loops in the queue and set the results in the queues.
		@param[in] p_themes model themes
		@param[in] p_constants model constant
		@param[out] p_queue the resulting queue of lines
		*/
		void processForLoops(const std::vector<Core::FMTTheme>& p_themes,
			const Core::FMTConstants& p_constants,
			std::queue<FMTLineInfo>& p_queue) const;
		// DocString: FMTParser::isForLoops
		/**
		@brief Test if you got a for loops in the line
		@param[in] p_line is the for loop header line
		@return true if it is a for loops.
		*/
		bool isForLoops(const std::string& p_line) const;
		// DocString: FMTParser::isForLoopsEnd
		/**
		@brief Test if you got a end of the for loops
		@param[in] p_line is the for loop header line
		@return true if it is the end.
		*/
		bool isForLoopsEnd(const std::string& p_line) const;
		// DocString: FMTParser:::getLine
		/**
		@brief get front element in queue and set line then do pop
		@param[in] p_Lines
		@return the line value
		*/
		std::string getLine(std::queue<FMTLineInfo>& p_Lines) const;
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
		///A safe get line function for Linux/Windows
		std::istream& _safeGetline(std::istream& is, std::string& t) const;
		#ifdef FMTWITHGDAL
			// DocString: FMTParser::gdalInitialization
			/**
			Initialize GDAL once
			*/
			static void _initializeGDAL();
			// DocString: FMTParser::_queryDatabase
			/**
			@brief Get all query lines of the for loops baed on key.FIELD_NAME
			@param[in] p_DataBaseLocation
			@param[in] p_VariableName
			@param[in] p_Query
			@return the query lines with the variable like 
			*/
			std::map<std::string, std::vector<std::string>>_queryDatabase(
				const std::string& p_DataBaseLocation,
				const std::string& p_VariableName,
				const std::string& p_Query) const;
		#endif
			// DocString: FMTParser::_getAbsolutePath
			/**
			@brief Get the absolute path of a path
			@param[in] p_Path
			@return the absolute path of the p_Path
			*/
			std::string _getAbsolutePath(std::string p_Path) const;
			// DocString: FMTParser::_setForLoopLines
			/**
			@brief concacanate lines together to form a foreach if foreach on multiline
			@param[in] p_queue queue of lines
			@return the front line
			*/
			FMTLineInfo _setForLoopLines(std::queue<FMTLineInfo>& p_queue) const;
			// DocString: FMTParser:: _processConstants
			/**
			@brief Turn the string with constant into string with regular numbers
			@param[in] p_input input string
			@return a valid string without constants #
			*/
			std::string _processConstants(std::string p_input,
				const Core::FMTConstants& p_constants) const;
    };

}

#endif
