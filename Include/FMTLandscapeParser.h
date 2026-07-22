/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlandscapeparser_Hm_included
#define FMTlandscapeparser_Hm_included


#include "FMTParser.h"
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
@brief Parser reading and writing the landscape section, and generating themes from vector or raster files.
@details Some functionalities require FMT to be compiled with FMTWITHGDAL. Also used by the model parser.
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
		@brief Return the indexed attributes of a theme in the landscape section.
		@param[in] indexm_line the index line.
		@param[in] constants the constants.
		@return a map of parameter name to value.
		*/
		std::map<std::string, double> getIndexes(std::string indexm_line, const Core::FMTConstants& constants);
		// DocString: FMTLandscapeParser::ParseState
		/**
		@brief Enumerator of the parsing states of the landscape parser.
		*/
		enum class ParseState {NORMAL = 0, IN_PRE_DECLARATION = 1};
		// DocString: FMTLandscapeParser::PreDeclarationContext
		/**
		@brief Context holding the state of a pre declaration during parsing.
		*/
		struct PreDeclarationContext {
			ParseState state;
			std::string currentKey; 
			std::map<std::string, std::pair<std::vector<std::string>, std::vector<std::string>>> declarations;
			
			// DocString: FMTLandscapeParser::PreDeclarationContext()
			/**
			@brief Default constructor for PreDeclarationContext.
			*/
			PreDeclarationContext();
			// DocString: FMTLandscapeParser::PreDeclarationContext::clearTheme
			/**
			@brief Clear the declarations of a theme.
			@param[in] nameID the theme name.
			*/
			void clearTheme(std::string nameID);
		};
		// DocString: FMTLandscapeParser::processPreDeclarationLine
		/**
		@brief Process a pre declaration line.
		@param[in] line the line.
		@param[in,out] context the pre declaration context.
		@param[in] constants the constants.
		@return true if the line is a pre declaration else false.
		*/
		bool processPreDeclarationLine(const std::string& line,
			PreDeclarationContext& context,
			const Core::FMTConstants& constants);
		// DocString: FMTLandscapeParser::ThemeParsingContext
		/**
		@brief Context holding the state of a theme being parsed.
		*/
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

			// DocString: FMTLandscapeParser::ThemeParsingContext()
			/**
			@brief Default constructor for ThemeParsingContext.
			*/
			ThemeParsingContext();
			// DocString: FMTLandscapeParser::ThemeParsingContext::clear
			/**
			@brief Clear the theme parsing context.
			*/
			void clear();
		};
		// DocString: FMTLandscapeParser::processThemeLine
		/**
		@brief Process a theme line.
		@param[in] kmatch the regex match.
		@param[in,out] ctx the theme parsing context.
		@param[in,out] preContext the pre declaration context.
		@param[in,out] themes the themes.
		@param[in] constants the constants.
		@param[in,out] unknownID the unknown id counter.
		*/
		void processThemeLine(const boost::smatch& kmatch,
			ThemeParsingContext& ctx,
			PreDeclarationContext& preContext,
			std::vector<Core::FMTTheme>& themes,
			const Core::FMTConstants& constants,
			size_t& unknownID);
		// DocString: FMTLandscapeParser::processAggregateLine
		/**
		@brief Process an aggregate line.
		@param[in] kmatch the regex match.
		@param[in,out] ctx the theme parsing context.
		@param[in,out] themes the themes.
		@param[in] constants the constants.
		*/
		void processAggregateLine(const boost::smatch& kmatch,
			ThemeParsingContext& ctx,
			std::vector<Core::FMTTheme>& themes,
			const Core::FMTConstants& constants);
		// DocString: FMTLandscapeParser::processAggregateValueLine
		/**
		@brief Process an aggregate value line.
		@param[in] line the line.
		@param[in,out] ctx the theme parsing context.
		@param[in,out] themes the themes.
		*/
		void processAggregateValueLine(const std::string& line,
			ThemeParsingContext& ctx,
			std::vector<Core::FMTTheme>& themes);
		// DocString: FMTLandscapeParser::processAttributeLine
		/**
		@brief Process an attribute line.
		@param[in] line the line.
		@param[in,out] ctx the theme parsing context.
		@param[in] constants the constants.
		*/
		void processAttributeLine(const std::string& line,
			ThemeParsingContext& ctx,
			const Core::FMTConstants& constants);
    public:
		// DocString: FMTLandscapeParser::FMTLandscapeParser()
		/**
		@brief Default constructor for FMTLandscapeParser.
		*/
        FMTLandscapeParser();
		// DocString: FMTLandscapeParser::~FMTLandscapeParser()
		/**
		@brief Default destructor for FMTLandscapeParser.
		*/
		~FMTLandscapeParser()=default;
		// DocString: FMTLandscapeParser::FMTLandscapeParser(const FMTLandscapeParser&)
		/**
		@brief Copy constructor for FMTLandscapeParser.
		@param[in] rhs the FMTLandscapeParser to copy.
		*/
        FMTLandscapeParser(const FMTLandscapeParser& rhs)=default;
		// DocString: FMTLandscapeParser::operator=
		/**
		@brief Copy assignment operator for FMTLandscapeParser.
		@param[in] rhs the FMTLandscapeParser to copy.
		@return a reference to this FMTLandscapeParser.
		*/
        FMTLandscapeParser& operator = (const FMTLandscapeParser& rhs)=default;
		// DocString: FMTLandscapeParser::read
		/**
		@brief Read the landscape section and return the themes.
		@param[in] constants the constants.
		@param[in] location the file location.
		@return the themes.
		*/
        std::vector<Core::FMTTheme>read(const Core::FMTConstants& constants,const std::string& location);
		#ifdef FMTWITHGDAL
			// DocString: FMTLandscapeParser::readVectors
			/**
			@brief Return the themes present in a spatial vector file.
			@param[in] location the file location.
			@return the themes.
			*/
			std::vector<Core::FMTTheme>readVectors(const  std::string& location);
			// DocString: FMTLandscapeParser::readRasters
			/**
			@brief Return the themes present in a vector of raster files, one theme per raster.
			@param[in] locations the raster file locations.
			@return the themes.
			*/
			std::vector<Core::FMTTheme>readRasters(const  std::vector< std::string>& locations);
		#endif
		// DocString: FMTLandscapeParser::write
		/**
		@brief Write a landscape section from a vector of themes.
		@param[in] themes the themes.
		@param[in] location the file location.
		*/
        void write(const  std::vector<Core::FMTTheme>& themes,const std::string& location) const;
    };
}


#endif // FMTlandscapeparser_Hm_included
