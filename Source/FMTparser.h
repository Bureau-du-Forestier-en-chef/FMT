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
#include <boost/filesystem/operations.hpp>
#include <ctime>

#include "FMTobject.h"


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
		std::regex rxvectortheme;
		// DocString: FMTparser::rxnumber
		///Regex to capture constants or numeric value
		std::regex rxnumber;
		// DocString: FMTparser::rxremovecomment
		///Regex to capture comments 
        std::regex rxremovecomment;
		// DocString: FMTparser::rxvalid
		///Regex to validate if a string is valid for the parser.
		std::regex rxvalid;
		// DocString: FMTparser::rxinclude
		///Regex to catch the include keyword.
		std::regex rxinclude;
		// DocString: FMTparser::rxfor
		///Regex to capture for loops.
		std::regex rxfor;
		// DocString: FMTparser::rxend
		///Regex to capture the end of forloops.
		std::regex rxend;
		// DocString: FMTparser::_incomment
		///Is true if the parser is in a bracket comment {} else false
        bool _incomment;
		// DocString: FMTparser::_forvalues
		///lines comming from a forloops present in the file
		std::queue<std::string>_forvalues;
		// DocString: FMTparser::_included
		///lines comming from an included file.
		std::queue<std::string>_included;
		// DocString: FMTparser::safeGetline
		///A safe get line function for Linux/Windows
		std::istream& safeGetline(std::istream& is, std::string& t) const;
    protected:
		// DocString: FMTparser::FMTsection
		///The section in which the child class is in
		mutable Core::FMTsection _section;
		// DocString: FMTparser::rxayld
		///A regex for capturing age/period/yield specification
		std::regex rxayld;
		// DocString: FMTparser::rxaage
		///A regex for capturing age specificaiton
		std::regex rxaage;
		// DocString: FMTparser::rxoperators
		///A regex for capturing simple operators
		std::regex rxoperators;
		// DocString: FMTparser::rxprimary
		///A regex for capturing the primary file
		std::regex rxprimary;
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
		// DocString: FMTparser:setsection
		/**
		It sets the section member of the FMTobject.
		*/
		void setsection(const Core::FMTsection& section) const;
		#ifdef FMTWITHGDAL
			// DocString: FMTparser:getFORELspatialref
			/**
			Return and OGRspatialReference corresponding to the one used for FORELs in Quebec.
			*/
			static inline OGRSpatialReference getFORELspatialref() 
			{
					OGRSpatialReference FMTspref(nullptr);
					FMTspref.importFromEPSG(32198);
					return FMTspref;
			}
			// DocString: FMTparser::createdataset
			/**
			The function create an empty GDALDataset for a given FMTlayer.
			*/
			template<typename T>
			GDALDataset* createdataset(const std::string& location,const Spatial::FMTlayer<T>& layer, const GDALDataType datatype) const
				{
				const char *pszFormat = "GTiff";
				GDALDriver *poDriver = nullptr;
				GDALDataset *poDstDS = nullptr;
				try{
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
				CSLDestroy( papszOptions );
				}catch (...)
					{
					_exhandler->raisefromcatch("", "FMTparser::createdataset", __LINE__, __FILE__);
					}
				return poDstDS;
				}
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
		Test if a value is actualy a yield.
		*/
		bool isyld(const Core::FMTyields& ylds,const std::string& value, Core::FMTsection section) const;
		// DocString: FMTparser::isact
		/**
		Test if a value (action) is actualy an action.
		*/
        bool isact(Core::FMTsection section,const std::vector<Core::FMTaction>& actions, std::string action) const;
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
		std::regex rxseparator;
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
		// DocString: FMTparser::tryopening
		/**
		Open a input stream at a given location.
		*/
        bool tryopening(const std::ifstream& stream, const std::string& location) const;
		// DocString: FMTparser::tryopening
		/**
		Open a output stream at a given location.
		*/
        bool tryopening(const std::ofstream& stream, const std::string& location) const;
		// DocString: FMTparser::isvalidfile
		/**
		Check if a file location is valid.
		*/
		bool isvalidfile(const std::string& location) const;
		// DocString: FMTparser::isvalid
		/**
		Check if line is valid and clean it.
		*/
        bool isvalid(std::string& line) const;
		// DocString: FMTparser::regexloop
		/**
		Capture a forloops from a line and return the variables to loop on.
		*/
		std::vector<std::string>regexloop(std::regex& cutregex, std::string& str) const;
		// DocString: FMTparser::spliter
		/**
		Split a string into multiple strings.
		*/
		std::vector<std::string>spliter(std::string strmask, std::regex& xspliter) const;
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
        T getnum(const std::string& value,const Core::FMTconstants& constant, int period = 0) const
            {
            T nvalue = 0;
			try {
				const std::string newvalue = boost::erase_all_copy(value, ",");
				if (isnum(newvalue))
				{
					nvalue = getnum<T>(newvalue);
				}
				else if (constant.isconstant(newvalue))
				{
					nvalue = constant.get<T>(newvalue, period);
					_exhandler->raise(Exception::FMTexc::FMTconstants_replacement,
						value + "at line " + std::to_string(_line), "FMTparser::getnum", __LINE__, __FILE__, _section);
					++_constreplacement;
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
						value + "at line " + std::to_string(_line), "FMTparser::getnum", __LINE__, __FILE__, _section);
				}
			}
			catch (...)
				{
					_exhandler->raisefromcatch("", "FMTparser::getnum", __LINE__, __FILE__, _section);
				}
            return nvalue;
            }
		// DocString: FMTparser::getnum
		/**
		Template function to get a numeric value from a string (value).
		*/
		template<typename T>
		T getnum(const std::string& value) const
			{
			T nvalue = 0;
			try{
				const std::string newvalue = boost::erase_all_copy(value, ",");
				if (isnum(newvalue))
					{
					nvalue = static_cast<T>(std::stod(newvalue));
				}else {
					_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
						value + "at line " + std::to_string(_line),"FMTparser::getnum", __LINE__, __FILE__, _section);
					}
				}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTparser::getnum", __LINE__, __FILE__, _section);
			}
			return nvalue;
			}
		// DocString: FMTparser::tryfillnumber
		/**
		Try to get a number from a string (value) return false if failed.
		*/
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
		// DocString: FMTparser::bounds
		/**
		Templated function to get a FMTbounds from a (value) base on constants and a (section).
		*/
        template<typename T>
		Core::FMTbounds<T>bounds(const Core::FMTconstants& constants, const std::string& value, const std::string& ope, Core::FMTsection section) const
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
			}catch (...)
				{
					_exhandler->raisefromcatch("", "FMTparser::bounds", __LINE__, __FILE__, _section);
				}
            return Core::FMTbounds<T>(section,lupper,llower);
            }
    };

}

#endif
