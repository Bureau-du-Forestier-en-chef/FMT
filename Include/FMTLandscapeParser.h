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
	private:
		// DocString: FMTLandscapeParser::m_rxcleanlans
		///This regex is the main regex to catch themes and aggregates.
		const static boost::regex m_rxcleanlans;
		// DocString: FMTLandscapeParser::m_rxindex
		///This regex catch the index of a given theme.
		const static boost::regex m_rxindex;
		// DocString: FMTLandscapeParser::m_rxparameter
		///This regex catch the parameters of a indexed theme.
		const static boost::regex m_rxparameter;
		// DocString: FMTLandscapeParser::m_rxPreAttributes
		const static boost::regex m_rxPreAttributes;
		// DocString: FMTLandscapeParser::m_rxPreAggregate
		const static boost::regex m_rxPreAggregate;
		// DocString: FMTLandscapeParser::_getIndexes
		/**
		@brief Return the indexed attributes of a theme in the landscape section.
		@param[in] indexm_line the index line.
		@param[in] constants the constants.
		@return a map of parameter name to value.
		*/
		std::map<std::string, double> _getIndexes(
			std::string indexm_line, 
			const Core::FMTConstants& constants);
		// DocString: FMTLandscapeParser::ParseState
		/**
		@brief Enumerator of the parsing states of the landscape parser.
		*/
		enum class ParseState {NORMAL = 0, IN_PRE_DECLARATION = 1};
		// DocString: FMTLandscapeParser::preDeclarationContext
		/**
		@brief Context holding the state of a pre declaration during parsing.
		*/
		struct preDeclarationContext {
			ParseState state;
			std::string currentKey; 
			std::map<std::string, std::pair<std::vector<std::string>, std::vector<std::string>>> declarations;
			
			// DocString: FMTLandscapeParser::preDeclarationContext()
			/**
			@brief Default constructor for preDeclarationContext.
			*/
			preDeclarationContext();
			// DocString: FMTLandscapeParser::preDeclarationContext::clearTheme
			/**
			@brief Clear the declarations of a theme.
			@param[in] nameID the theme name.
			*/
			void clearTheme(std::string nameID);
		};
		// DocString: FMTLandscapeParser::_processPreDeclarationLine
		/**
		@brief Process a pre declaration line.
		@param[in] line the line.
		@param[in,out] context the pre declaration context.
		@param[in] constants the constants.
		@return true if the line is a pre declaration else false.
		*/
		bool _processPreDeclarationLine(
			const std::string& line,
			preDeclarationContext& context,
			const Core::FMTConstants& constants);
		// DocString: FMTLandscapeParser::themeParsingContext
		/**
		@brief Context holding the state of a theme being parsed.
		*/
		struct themeParsingContext {
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

			// DocString: FMTLandscapeParser::themeParsingContext()
			/**
			@brief Default constructor for themeParsingContext.
			*/
			themeParsingContext();
			// DocString: FMTLandscapeParser::themeParsingContext::clear
			/**
			@brief Clear the theme parsing context.
			*/
			void clear();
		};
		// DocString: FMTLandscapeParser::_processThemeLine
		/**
		@brief Process a theme line.
		@param[in] kmatch the regex match.
		@param[in,out] ctx the theme parsing context.
		@param[in,out] preContext the pre declaration context.
		@param[in,out] themes the themes.
		@param[in] constants the constants.
		@param[in,out] unknownID the unknown id counter.
		*/
		void _processThemeLine(
			const boost::smatch& kmatch,
			themeParsingContext& ctx,
			preDeclarationContext& preContext,
			std::vector<Core::FMTTheme>& themes,
			const Core::FMTConstants& constants,
			size_t& unknownID);
		// DocString: FMTLandscapeParser::_processAggregateLine
		/**
		@brief Process an aggregate line.
		@param[in] kmatch the regex match.
		@param[in,out] ctx the theme parsing context.
		@param[in,out] themes the themes.
		@param[in] constants the constants.
		*/
		void _processAggregateLine(
			const boost::smatch& kmatch,
			themeParsingContext& ctx,
			std::vector<Core::FMTTheme>& themes,
			const Core::FMTConstants& constants);
		// DocString: FMTLandscapeParser::_processAggregateValueLine
		/**
		@brief Process an aggregate value line.
		@param[in] line the line.
		@param[in,out] ctx the theme parsing context.
		@param[in,out] themes the themes.
		*/
		void _processAggregateValueLine(
			const std::string& line,
			themeParsingContext& ctx,
			std::vector<Core::FMTTheme>& themes);
		// DocString: FMTLandscapeParser::_processAttributeLine
		/**
		@brief Process an attribute line.
		@param[in] line the line.
		@param[in,out] ctx the theme parsing context.
		@param[in] constants the constants.
		*/
		void _processAttributeLine(
			const std::string& line,
			themeParsingContext& ctx,
			const Core::FMTConstants& constants);
    };
}


#endif // FMTlandscapeparser_Hm_included
