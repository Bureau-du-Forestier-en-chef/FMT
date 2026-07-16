/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlandscapeparser_Hm_included
#define FMTlandscapeparser_Hm_included


#include "FMTparser.h"
#include "FMTutility.h"
#include <boost/regex.hpp>
#include <string>
#include <map>
#include <vector>

namespace Core
{
	class FMTTheme;
	class FMTConstants;
}


namespace Parser
{
// DocString: FMTLandscapeParser
/**
The landscape parser have some read write capabilities for the landscape section.
It can also get a landscape section from a vector or raster files (generating FMTTheme).
If FMT is compiled without the compile proprocessor FMTWITHGDAL then some funcionalities of the FMTLandscapeParser wont be
available to the user. This class is also used by the FMTModelParser.
*/
	class FMTEXPORT FMTLandscapeParser : public FMTParser
	{
	private:
		// DocString: FMTLandscapeParser::rxcleanlans
		///This regex is the main regex to catch themes and aggregates.
		const static boost::regex rxcleanlans;
		// DocString: FMTLandscapeParser::rxindex
		///This regex catch the index of a given theme.
		const static boost::regex rxindex;
		// DocString: FMTLandscapeParser::rxparameter
		///This regex catch the parameters of a indexed theme.
		const static boost::regex rxparameter;
		// DocString: FMTLandscapeParser::rxPreAttributes
		const static boost::regex rxPreAttributes;
		// DocString: FMTLandscapeParser::rxPreAggregate
		const static boost::regex rxPreAggregate;
		// DocString: FMTLandscapeParser::getIndexes
		/**
		This function is used to catch the indexed attributes of a theme in the landscape section
		the returned map hold the parameters name (key) and value (item). It uses the complete string line (indexm_line) of
		the landscape section and (constants).
		*/
		std::map<std::string, double> getIndexes(std::string indexm_line, const Core::FMTConstants& constants);
		// DocString: FMTLandscapeParser::ParseState
		enum class ParseState {NORMAL = 0, IN_PRE_DECLARATION = 1};
		// DocString: FMTLandscapeParser::PreDeclarationContext
		struct PreDeclarationContext {
			ParseState state;
			std::string currentKey; 
			std::map<std::string, std::pair<std::vector<std::string>, std::vector<std::string>>> declarations;
			
			PreDeclarationContext();
			void clearTheme(std::string nameID);
		};
		//DocString: FMTLandscapeParser::processPreDeclarationLine
		bool processPreDeclarationLine(const std::string& line,
			PreDeclarationContext& context,
			const Core::FMTConstants& constants);
		// DocString: FMTLandscapeParser::ThemeParsingContext
		struct ThemeParsingContext {
			std::vector<std::string> attributes;
			std::vector<std::string> attributenames;
			std::vector<std::string> aggregates;
			std::vector<std::vector<std::string>> aggregatenames;
			std::vector<std::map<std::string, double>> indexes_values;
			std::string themename;
			std::string aggregatename;
			size_t id;
			size_t start;
			size_t stop;
			int pasttheme;
			bool aggregate_redefinition;

			ThemeParsingContext();
			void clear();
		};
		// DocString: FMTLandscapeParser::processThemeLine
		void processThemeLine(const boost::smatch& kmatch,
			ThemeParsingContext& ctx,
			PreDeclarationContext& preContext,
			std::vector<Core::FMTTheme>& themes,
			const Core::FMTConstants& constants,
			size_t& unknownID);
		// DocString: FMTLandscapeParser::processAggregateLine
		void processAggregateLine(const boost::smatch& kmatch,
			ThemeParsingContext& ctx,
			std::vector<Core::FMTTheme>& themes,
			const Core::FMTConstants& constants);
		// DocString: FMTLandscapeParser::processAggregateValueLine
		void processAggregateValueLine(const std::string& line,
			ThemeParsingContext& ctx,
			std::vector<Core::FMTTheme>& themes);
		// DocString: FMTLandscapeParser::processAttributeLine
		void processAttributeLine(const std::string& line,
			ThemeParsingContext& ctx,
			const Core::FMTConstants& constants);
    public:
		// DocString: FMTLandscapeParser::FMTLandscapeParser()
		/**
		Default constructor for FMTLandscapeParser
		*/
        FMTLandscapeParser();
		// DocString: FMTLandscapeParser::~FMTLandscapeParser()
		/**
		Default destructor for FMTLandscapeParser
		*/
		~FMTLandscapeParser()=default;
		// DocString: FMTLandscapeParser::FMTLandscapeParser(const FMTLandscapeParser&)
		/**
		Default copy constructor for FMTLandscapeParser
		*/
        FMTLandscapeParser(const FMTLandscapeParser& rhs)=default;
		// DocString: FMTLandscapeParser::operator=
		/**
		Default copy assignment for FMTLandscapeParser
		*/
        FMTLandscapeParser& operator = (const FMTLandscapeParser& rhs)=default;
		// DocString: FMTLandscapeParser::read
		/**
		Using the (location) of the landscape section and some (constants) this function will read the landscape section
		and returns a vector of themes present in the landscape section.
		*/
        std::vector<Core::FMTTheme>read(const Core::FMTConstants& constants,const std::string& location);
		#ifdef FMTWITHGDAL
			// DocString: FMTLandscapeParser::readVectors
			/**
			Using the (location) of a spatial vector file like a shapefile, this function will
			return a vector of themes present in the spatial vector file.
			*/
			std::vector<Core::FMTTheme>readVectors(const  std::string& location);
			// DocString: FMTLandscapeParser::readRasters
			/**
			Using the vector of (locations) of raster files, this funtion will return a vector
			of themes present in the rasters. Each raster represent a theme in the location vector.
			*/
			std::vector<Core::FMTTheme>readRasters(const  std::vector< std::string>& locations);
		#endif
		// DocString: FMTLandscapeParser::write
		/**
		With a file (location) and a vector of (themes) this function will fill up a landscape section.
		*/
        void write(const  std::vector<Core::FMTTheme>& themes,const std::string& location) const;
    };
}


#endif // FMTlandscapeparser_Hm_included
