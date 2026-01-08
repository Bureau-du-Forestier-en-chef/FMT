/*
Copyright (c) 2019 Gouvernement du Québec

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
	class FMTtheme;
	class FMTconstants;
}


namespace Parser
{
// DocString: FMTlandscapeparser
/**
The landscape parser have some read write capabilities for the landscape section.
It can also get a landscape section from a vector or raster files (generating FMTtheme).
If FMT is compiled without the compile proprocessor FMTWITHGDAL then some funcionalities of the FMTlandscapeparser wont be
available to the user. This class is also used by the FMTmodelparser.
*/
	class FMTEXPORT FMTlandscapeparser : public FMTparser
	{
	private:
		// DocString: FMTlandscapeparser::rxcleanlans
		///This regex is the main regex to catch themes and aggregates.
		const static boost::regex rxcleanlans;
		// DocString: FMTlandscapeparser::rxindex
		///This regex catch the index of a given theme.
		const static boost::regex rxindex;
		// DocString: FMTlandscapeparser::rxparameter
		///This regex catch the parameters of a indexed theme.
		const static boost::regex rxparameter;
		// DocString: FMTlandscapeparser::rxattributes
		const static boost::regex rxattributes;
		// DocString: FMTlandscapeparser::getindexes
		/**
		This function is used to catch the indexed attributes of a theme in the landscape section
		the returned map hold the parameters name (key) and value (item). It uses the complete string line (indexm_line) of
		the landscape section and (constants).
		*/
		std::map<std::string, double> getindexes(std::string indexm_line, const Core::FMTconstants& constants);
		// DocString: FMTlandscapeparser::ParseState
		enum class ParseState { NORMAL, IN_PRE_DECLARATION };
		// DocString: FMTlandscapeparser::PreDeclarationContext
		struct PreDeclarationContext {
			ParseState state;
			int currentThemeId;
			std::map<size_t, std::pair<std::vector<std::string>, std::vector<std::string>>> declarations;
			
			PreDeclarationContext();
		};
		//DocString: FMTlandscapeparser::ProcessPreDeclarationLine
		bool ProcessPreDeclarationLine(const std::string& line,
			PreDeclarationContext& context,
			const Core::FMTconstants& constants);
		// DocString: FMTlandscapeparser::ThemeParsingContext
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
		// DocString: FMTlandscapeparser::ProcessThemeLine
		void ProcessThemeLine(const boost::smatch& kmatch,
			ThemeParsingContext& ctx,
			PreDeclarationContext& preContext,
			std::vector<Core::FMTtheme>& themes,
			const Core::FMTconstants& constants,
			size_t& unknownID);
		// DocString: FMTlandscapeparser::ProcessAggregateLine
		void ProcessAggregateLine(const boost::smatch& kmatch,
			ThemeParsingContext& ctx,
			std::vector<Core::FMTtheme>& themes,
			const Core::FMTconstants& constants);
		// DocString: FMTlandscapeparser::ProcessAggregateValueLine
		void ProcessAggregateValueLine(const std::string& line,
			ThemeParsingContext& ctx,
			std::vector<Core::FMTtheme>& themes);
		// DocString: FMTlandscapeparser::ProcessAttributeLine
		void ProcessAttributeLine(const std::string& line,
			ThemeParsingContext& ctx,
			const Core::FMTconstants& constants);
    public:
		// DocString: FMTlandscapeparser::FMTlandscapeparser()
		/**
		Default constructor for FMTlandscapeparser
		*/
        FMTlandscapeparser();
		// DocString: FMTlandscapeparser::~FMTlandscapeparser()
		/**
		Default destructor for FMTlandscapeparser
		*/
		~FMTlandscapeparser()=default;
		// DocString: FMTlandscapeparser::FMTlandscapeparser(const FMTlandscapeparser&)
		/**
		Default copy constructor for FMTlandscapeparser
		*/
        FMTlandscapeparser(const FMTlandscapeparser& rhs)=default;
		// DocString: FMTlandscapeparser::operator=
		/**
		Default copy assignment for FMTlandscapeparser
		*/
        FMTlandscapeparser& operator = (const FMTlandscapeparser& rhs)=default;
		// DocString: FMTlandscapeparser::read
		/**
		Using the (location) of the landscape section and some (constants) this function will read the landscape section
		and returns a vector of themes present in the landscape section.
		*/
        std::vector<Core::FMTtheme>read(const Core::FMTconstants& constants,const std::string& location);
		#ifdef FMTWITHGDAL
			// DocString: FMTlandscapeparser::readvectors
			/**
			Using the (location) of a spatial vector file like a shapefile, this function will
			return a vector of themes present in the spatial vector file.
			*/
			std::vector<Core::FMTtheme>readvectors(const  std::string& location);
			// DocString: FMTlandscapeparser::readrasters
			/**
			Using the vector of (locations) of raster files, this funtion will return a vector
			of themes present in the rasters. Each raster represent a theme in the location vector.
			*/
			std::vector<Core::FMTtheme>readrasters(const  std::vector< std::string>& locations);
		#endif
		// DocString: FMTlandscapeparser::write
		/**
		With a file (location) and a vector of (themes) this function will fill up a landscape section.
		*/
        void write(const  std::vector<Core::FMTtheme>& themes,const std::string& location) const;
    };
}


#endif // FMTlandscapeparser_Hm_included
