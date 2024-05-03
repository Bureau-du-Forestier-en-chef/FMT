/*
Copyright (c) 2019 Gouvernement du Québec
 
SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTparser_H_INCLUDED
#define FMTparser_H_INCLUDED

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



#include "FMTobject.h"
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
	class FMTtheme;
	class FMTaction;
	class FMTyields;
	class FMTspec;
	class FMTconstants;
	template<typename T>
	class FMTbounds;
}

namespace Spatial
{
	template<typename T>
	class FMTlayer;
}


/// Namespace handling all FMT's parsers. Everything related to I/O should be located in this namespace.
namespace Parser
{
// DocString: FMTparser
/**
The FMTparser is the main parent class of all the other FMTparser available in FMT.
It has alot of usefull function common to all parser for validation and error throw when turning a std::string to something usefull in FMT.
*/
class FMTEXPORT FMTparser: public Core::FMTobject
    {
    private:
		// DocString: FMTparser::rxvectortheme
		///Regex to capture theme.
		const static boost::regex rxvectortheme;
		// DocString: FMTparser::rxnumber
		///Regex to capture constants or numeric value
		const static boost::regex rxnumber;
		// DocString: FMTparser::rxremovecomment
		///Regex to capture comments 
		const static boost::regex rxremovecomment;
		// DocString: FMTparser::rxvalid
		///Regex to validate if a string is valid for the parser.
		const static boost::regex rxvalid;
		// DocString: FMTparser::rxinclude
		///Regex to catch the include keyword.
		const static boost::regex rxinclude;
		// DocString: FMTparser::rxfor
		///Regex to capture for loops.
		const static boost::regex rxfor;
		// DocString: FMTparser::rxend
		///Regex to capture the end of forloops.
		const static boost::regex rxend;
		// DocString: FMTparser::primary_sections
		///When the modelparser is used this value is saved each time calling FMTparser::getprimary() allow use to use _PRIMARY key word in include files.
		static std::map<Core::FMTsection, std::string>primary_sections;
		// DocString: FMTparser::_incomment
		///Is true if the parser is in a bracket comment {} else false
        bool _incomment;
		// DocString: FMTparser::_forvalues
		///lines comming from a forloops present in the file
		std::queue<std::string>_forvalues;
		// DocString: FMTparser::_included
		///lines comming from an included file.
		std::queue<std::string>_included;
		// DocString: FMTparser::GDALinitialization;
		///True if GDAL has been initialized.
		static bool GDALinitialization;
		// DocString: FMTparser::safeGetline
		///A safe get line function for Linux/Windows
		std::istream& safeGetline(std::istream& is, std::string& t) const;
		#ifdef FMTWITHGDAL
		// DocString: FMTparser::GDALinitialization
		/**
		Initialize GDAL once
		*/
		static void initializeGDAL();
		#endif
    protected:
		// DocString: FMTparser::mtx
		///Mutex for multi-threading.
		mutable boost::recursive_mutex mtx;
		// DocString: FMTparser::FMTsection
		///The section in which the child class is in
		mutable Core::FMTsection _section;
		// DocString: FMTparser::rxayld
		///A regex for capturing age/period/yield specification
		const static boost::regex rxayld;
		// DocString: FMTparser::rxaage
		///A regex for capturing age specificaiton
		const static boost::regex rxaage;
		// DocString: FMTparser::rxayldage
		///A regex for capturing age or yield specification
		const static boost::regex rxayldage;
		// DocString: FMTparser::rxbounds
		///A regex for capturing bounds for yield or age specification
		const static boost::regex rxbounds;
		// DocString: FMTparser::rxoperators
		///A regex for capturing simple operators
		const static boost::regex rxoperators;
		// DocString: FMTparser::rxprimary
		///A regex for capturing the primary file
		const static boost::regex rxprimary;
		// DocString: FMTparser::_constreplacement
		///Number of constant replaced by a numerical number
        mutable int _constreplacement;
		// DocString: FMTparser::_line
		///Line id at which the parser is reading the values.
        mutable int _line;
		// DocString: FMTparser::_comment
		///Comment found in the actual line read by the parser.
		std::string _comment;
		// DocString: FMTparser::_location
		///File location of the section read by the parser.
		mutable std::string _location;
		// DocString: FMTparser::mostrecentfile
		///The most recent file time read by this parser.
		mutable std::time_t mostrecentfile;
		// DocString: FMTparser::setsection
		/**
		It sets the section member of the FMTobject.
		*/
		void setsection(const Core::FMTsection& section) const;
		#ifdef FMTWITHGDAL
			// DocString: FMTparser::getallGDALdrivernames
			/**
			Return a vector of GDAL drivers for a given spatialtype (raster/vector)
			*/
			std::vector<GDALDriver*> getallGDALdrivers(const char* spatialtype,bool testcreation=true) const;
			// DocString: FMTparser::getGDALextensions
			/**
			Return a vector(organize the extentions by drivers) of vector of GDAL extensions accepted for the given spatialtype.
			*/
			std::vector<std::vector<std::string>>getGDALextensions(const char* spatialtype,bool testcreation = true) const;
			// DocString: FMTparser::setcategories
			/**
			Write the categories in the band information
			*/
			void setcategories(GDALRasterBand* band, const std::vector<std::string>& categories) const;
			// DocString: FMTareaparser::getprojtransform
			/**
			Create a memory layer with a new int field named fieldname based on forel.
			Based on the baselayer.
			*/
			OGRCoordinateTransformation* getprojtransform(OGRLayer* baselayer,bool fittoforel =true) const;
			// DocString: FMTareaparser::gettransformmemlayercopy
			/**
			Create a memory layer with a new int field named fieldname based on forel.
			Based on the baselayer. With a new spatial reference or can be the same has baselayer.
			*/
			GDALDataset* gettransformmemlayercopy(OGRLayer* baselayer,const OGRSpatialReference* newreference, const std::string& fieldname) const;
			// DocString: FMTparser::getFORELspatialref
			/**
			Return and OGRspatialReference corresponding to the one used for FORELs in Quebec.
			*/
			std::unique_ptr<OGRSpatialReference> getFORELspatialref() const;
			// DocString: FMTparser::createdataset
			/**
			The function create an empty GDALDataset for a given FMTlayer.
			*/
			template<typename T>
			GDALDataset* createdataset(const std::string& location, const Spatial::FMTlayer<T>& layer, const int datatypeid,std::string format = "GTiff") const;
			// DocString: FMTparser::getdataset
			/**
			Open in readonly a GDALdataset from a given (location), will throw if anything went wrong.
			*/
			GDALDataset* getdataset(const std::string& location) const;
			// DocString: FMTparser::getvectordataset
			/**
			Open in readonly a GDALdataset from a given (location) of a vector file, will throw if anything went wrong.
			*/
			GDALDataset* getvectordataset(const std::string& location) const;
			// DocString: FMTparser::getlayer
			/**
			Open a GDALlayer from a vector GDALdataset for a given layer (id) and a actual (dataset), will throw if anything went wrong.
			*/
			OGRLayer* getlayer(GDALDataset* dataset,int id) const;
			// DocString: FMTparser::getband
			/**
			Open a GDALrasterband from a given GDALdataset and a (bandid).
			*/
			GDALRasterBand* getband(GDALDataset* dataset,int bandid=1) const;
			// DocString: FMTparser::createband
			/**
			Create a GDALrasterband in a GDALdataset where you can add categories.
			*/
			GDALRasterBand* createband(GDALDataset* dataset,const std::vector<std::string>& categories,int bandid=1) const;
			// DocString: FMTparser::getcat
			/**
			Get categories of a GDALdataset for a given (bandid).
			*/
			std::vector<std::string> getcat(GDALDataset* dataset,int bandid=1) const;
			// DocString: FMTparser::getoverview
			/**
			Get an overview of a GDALraserband.
			*/
			GDALRasterBand* getoverview(GDALRasterBand* band,int view=1) const;
			// DocString: FMTparser::getWSfields
			/**
			Will fill up all the fields id of the themes,age,area,lock of a given (layer).
			*/
			void getWSfields(OGRLayer* layer, std::map<int,int>& themes,int& age,int& area,int& lock, std::string agefield="", std::string areafield="", std::string lockfield="") const;
			// DocString: FMTparser::createvectormemoryds
			/**
			
			*/
			GDALDataset* createvectormemoryds() const;
			#endif
		// DocString: FMTparser::getbaseoperators
		/**
		Gives a vector of operators normaly found in the different sections.
		*/
		std::array<std::string, 5>getbaseoperators() const;
		// DocString: FMTparser::sameas
		/**
		Used to deal with the _SAMEAS keyword.
		*/
		std::vector<std::string> sameas(const std::string& allset) const;
		// DocString: FMTparser::getprimary
		/**
		Using a primaryfile it returns the path to the different sections file locations.
		*/
		std::map<Core::FMTsection, std::string> getprimary(const std::string& primarylocation);
		// DocString: FMTparser::isyld
		/**
		@brief valid if the value is a valid yield.
		@param[in] p_ylds the yield section
		@param[in] p_value the input value
		@param[in] p_section the section
		@param[in] p_trhowError if true will throw if it is not a yield.
		@return true if it is a yield.
		*/
		bool isyld(const Core::FMTyields& p_ylds,const std::string& p_value,
			Core::FMTsection p_section, bool p_throwError = true) const;
		// DocString: FMTparser::isact
		/**
		Test if a value (action) is actualy an action.
		*/
        bool isact(Core::FMTsection section,const std::vector<Core::FMTaction>& actions, std::string action) const;
		// DocString: FMTparser::setspecs
		/**
		Set the specifications of a given or multiple (specs) and return the rest of line.
		*/
		std::string setspecs(Core::FMTsection section, Core::FMTkwor key,const Core::FMTyields& ylds,const Core::FMTconstants& constants, std::vector<Core::FMTspec>& specs, const std::string& line);
		// DocString: FMTparser::setspec
		/**
		Set the specifications of a given (spec) and return the rest of line.
		*/
		std::string setspec(Core::FMTsection section, Core::FMTkwor key,const Core::FMTyields& ylds,const Core::FMTconstants& constants, Core::FMTspec& spec, const std::string& line);
		// DocString: FMTparser::from_extension
		/**
		Return the FMTsection given the extension of the file.
		*/
		Core::FMTsection from_extension(const std::string& ext) const;
		// DocString: FMTparser::readcsv
		/**
		Read a csv file from a (location) using a (separator) and returns a vector of vector of lines.
		*/
		std::vector<std::vector<std::string>>readcsv(const std::string& location,const char& separator);
    public:
		// DocString: FMTparser::rxseparator
		///A regex for splitting general string.
		const static boost::regex rxseparator;
		// DocString: FMTparser()
		/**
		Default constructor for FMTparser.
		*/
        FMTparser();
		// DocString: FMTparser(const FMTparser&)
		/**
		Default copy constructor for FMTparser.
		*/
        FMTparser(const FMTparser& rhs);
		// DocString: FMTparser::operator=
		/**
		Default copy assignment for FMTparser.
		*/
        FMTparser& operator = (const FMTparser& rhs);
		// DocString: ~FMTparser()
		/**
		Default destructor for FMTparser.
		*/
        virtual ~FMTparser()=default;
		// DocString: FMTparser::getmostrecentfiletime
		/**
		Get the most recentfiletime of the files read by ther parser.
		*/
		std::time_t getmostrecentfiletime() const;
		// DocString: FMTparser::setheader
		/**
		Taking a string has argument set this string has an header in write mode
		when the file is opened. It will replace the _comment attribute with the header string.
		*/
		void setheader(const std::string& header);
		// DocString: FMTparser::tryopening
		/**
		Open a input stream at a given location.
		*/
        bool tryopening(const std::ifstream& stream, const std::string& location) const;
		// DocString: FMTparser::tryopening
		/**
		Open a output stream at a given location.
		*/
        bool tryopening(std::ofstream& stream, const std::string& location) const;
		// DocString: FMTparser::isvalidfile
		/**
		Check if a file location is valid.
		*/
		bool isvalidfile(const std::string& location) const;
		// DocString: FMTparser::isvalid
		/**
		Check if line is valid and clean it.
		*/
        bool isvalid(const std::string& line) const;
		// DocString: FMTparser::regexloop
		/**
		Capture a forloops from a line and return the variables to loop on.
		*/
		std::vector<std::string>regexloop(const boost::regex& cutregex, std::string& str) const;
		// DocString: FMTparser::spliter
		/**
		Split a string into multiple strings.
		*/
		std::vector<std::string>spliter(std::string strmask,const boost::regex& xspliter) const;
		// DocString: FMTparser::clearcomments
		/**
		Clear comments from a line and fill up the _comment private value.
		*/
        void clearcomments(std::string& line);
		// DocString: FMTparser::upper
		/**
		Convert a string to upper case string.
		*/
		std::string upper(const std::string& lowercases) const;
		// DocString: FMTparser::getcleanline
		/**
		Clean a line and make it ready to be read by a parser.
		*/
		std::string getcleanline(std::ifstream& stream);
		// DocString: FMTparser::tryinclude
		/**
		Read an included file and put the results into a queue.
		*/
		std::queue<std::string> tryinclude(const std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons);
		// DocString: FMTparser::returninclude
		/**
		return the next line to be read by the parser considering included files.
		*/
		std::string returninclude(const std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons);
		// DocString: FMTparser::getforloops
		/**
		Fill up the for loops queue.
		*/
		bool getforloops(std::string& line, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& cons, std::vector<std::string>& allvalues, std::string& target);
		// DocString: FMTparser::getcleanlinewfor
		/**
		Get a clean line and consider for loops when reading the file.
		*/
		std::string getcleanlinewfor(std::ifstream& stream,const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& cons);
		// DocString: FMTparser::isnum
		/**
		Return true if the value is a number.
		*/
        bool isnum(std::string value) const;
		// DocString: FMTparser::isnum
		/**
		Return true if the value is a number else it will raise (if throwerror = true) being non valid number.
		*/
		bool isnum(const std::string& value,const Core::FMTconstants& constant,bool throwerror= true) const;
		// DocString: FMTparser::getnum
		/**
		Template function to get a numeric value from a string (value) based on constant and period.
		*/
        template<typename T>
		T getnum(const std::string& value, const Core::FMTconstants& constant, int period = 0) const;
		// DocString: FMTparser::getnum
		/**
		Template function to get a numeric value from a string (value).
		*/
		template<typename T>
		T getnum(const std::string& value,bool omitnumtest=false) const;
		// DocString: FMTparser::tryfillnumber
		/**
		Try to get a number from a string (value) return false if failed.
		*/
		template<typename T>
		bool tryfillnumber(T& number, const std::string& value, const Core::FMTconstants& constant, int period = 0) const;
		// DocString: FMTparser::bounds
		/**
		Templated function to get a FMTbounds from a (value) base on constants and a (section).
		*/
        template<typename T>
		Core::FMTbounds<T>bounds(const Core::FMTconstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const;
		#ifdef FMTWITHGDAL
		// DocString: FMTparser::getWSfields
		/**
		Will Create a OGR vector file based on a given drivername and a location.
		*/
		GDALDataset* createOGRdataset(std::string location = std::string(),std::string gdaldrivername = "CSV") const;
		// DocString: FMTparser::createlayer()
		/**
		Create a layer with from a dataset, it will delete layer with the same name before creation if it exist in the dataset
		*/
		OGRLayer* createlayer(GDALDataset* dataset,
			const std::string& name, std::vector<std::string> creationoptions = std::vector<std::string>()) const;
		// DocString: FMTparser::getGDALvectordrivernames
		/**
		Return a vector of GDAL vector driver names
		*/
		std::vector<std::string>getGDALvectordrivernames(bool testcreation = true) const;
		// DocString: FMTparser::getGDALrasterdrivernames
		/**
		Return a vector of GDAL raster driver names
		*/
		std::vector<std::string>getGDALrasterdrivernames(bool testcreation = true) const;
		// DocString: FMTparser::getGDALvectordriverextensions
		/**
		Return a vector(organize the extentions by drivers) of vector of GDAL vector driver extensions
		*/
		std::vector<std::vector<std::string>>getGDALvectordriverextensions(bool testcreation = true) const;
		// DocString: FMTparser::getGDALrasterdriverextensions
		/**
		Return a vector(organize the extentions by drivers) of vector of GDAL raster driver extensions
		*/
		std::vector<std::vector<std::string>>getGDALrasterdriverextensions(bool testcreation = true) const;
		#endif
    };

}

#endif
