#pragma once

#include "stdafx.h"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "FMTModelCache.h"

namespace Logging
{
	class FMTExcelLogger;
}

namespace Parser
{
	class FMTModelParser;
}

namespace Exception
{
	class FMTExcelExceptionHandler;
}

namespace Wrapper
{
	public ref class FMTExcelCache
	{
	public:

		// DocString: FMTExcelCache::FMTExcelCache
		/**
		@brief Creates an Excel cache manager.

		Initializes the parser, logger and exception handler.
		*/
		FMTExcelCache();

		// DocString: FMTExcelCache::~FMTExcelCache
		/**
		@brief Releases all cache resources.
		*/
		~FMTExcelCache();
		// DocString: FMTExcelCache::readnsolveTemplates
		/**
		@brief Loads all templates and optionally solves them.

		@param[in] primarylocation Primary project file.
		@param[in] templatefolder Template directory.
		@param[in] length Planning horizon.
		@param[in] solve Indicates whether the models should be solved.

		@return List of successfully loaded scenario names.
		*/
		System::Collections::Generic::List<System::String^>^
			readnsolveTemplates(
				System::String^ primarylocation,
				System::String^ templatefolder,
				int length,
				bool solve);

		// DocString: FMTExcelCache::add
		/**
		@brief Loads a scenario into the cache.

		@param[in] primarylocation Project file.
		@param[in] scenario Scenario name.

		@return True on success.
		*/
		bool add(
			System::String^ primarylocation,
			System::String^ scenario);

		// DocString: FMTExcelCache::addAndBuild
		/**
		@brief Loads and builds a scenario.

		@param[in] primarylocation Project file.
		@param[in] scenario Scenario name.
		@param[in] length Planning horizon.

		@return True if the model was successfully built.
		*/
		bool addAndBuild(
			System::String^ primarylocation,
			System::String^ scenario,
			int length);

		// DocString: FMTExcelCache::remove
		/**
		@brief Removes a scenario from the cache.

		@param[in] primarylocation Project file.
		@param[in] scenario Scenario name.
		*/
		void remove(
			System::String^ primarylocation,
			System::String^ scenario);

		// DocString: FMTExcelCache::getvalue
		/**
		@brief Gets an output value.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] outputname Output name.
		@param[in] themeselection Theme selection.
		@param[in] period Evaluation period.

		@return Output value.
		*/
		double getvalue(
			System::String^ primaryname,
			System::String^ scenario,
			System::String^ outputname,
			System::String^ themeselection,
			int period);

		// DocString: FMTExcelCache::getyield
		/**
		@brief Gets a yield value.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] yieldname Yield name.
		@param[in] themeselection Theme selection.
		@param[in] period Evaluation period.
		@param[in] age Age.

		@return Yield value.
		*/
		double getyield(
			System::String^ primaryname,
			System::String^ scenario,
			System::String^ yieldname,
			System::String^ themeselection,
			int period,
			int age);

		// DocString: FMTExcelCache::getattributes
		/**
		@brief Gets attributes for a theme value.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] themeid Theme index.
		@param[in] value Theme value.
		@param[in] aggregates Include aggregates when true.

		@return Attribute list.
		*/
		System::Collections::Generic::List<System::String^>^
			getattributes(
				System::String^ primaryname,
				System::String^ scenario,
				int themeid,
				System::String^ value,
				bool aggregates);

		// DocString: FMTExcelCache::getattributesdescription
		/**
		@brief Gets attribute descriptions.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] themeid Theme index.
		@param[in] value Theme value.

		@return Attribute descriptions.
		*/
		System::Collections::Generic::List<System::String^>^
			getattributesdescription(
				System::String^ primaryname,
				System::String^ scenario,
				int themeid,
				System::String^ value);

		// DocString: FMTExcelCache::getAggregates
		/**
		@brief Gets theme aggregates.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] themeid Theme index.

		@return Aggregate names.
		*/
		System::Collections::Generic::List<System::String^>^
			getAggregates(
				System::String^ primaryname,
				System::String^ scenario,
				int themeid);
		// DocString: FMTExcelCache::getprimaries
		/**
		@brief Gets all primary project names currently loaded.

		@return List of primary project names.
		*/
		System::Collections::Generic::List<System::String^>^
			getprimaries();

		// DocString: FMTExcelCache::getscenarios
		/**
		@brief Gets all scenarios associated with a primary project.

		@param[in] fichierprimaire Primary project name.

		@return Scenario names.
		*/
		System::Collections::Generic::List<System::String^>^
			getscenarios(
				System::String^ fichierprimaire);

		// DocString: FMTExcelCache::getActions
		/**
		@brief Gets actions matching an aggregate filter.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] filter Aggregate filter.

		@return Action names.
		*/
		System::Collections::Generic::List<System::String^>^
			getActions(
				System::String^ primaryname,
				System::String^ scenario,
				System::String^ filter);

		// DocString: FMTExcelCache::getactionaggregates
		/**
		@brief Gets aggregates associated with an action.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] filter Action name.

		@return Aggregate names.
		*/
		System::Collections::Generic::List<System::String^>^
			getactionaggregates(
				System::String^ primaryname,
				System::String^ scenario,
				System::String^ filter);

		// DocString: FMTExcelCache::getOutputs
		/**
		@brief Gets all outputs available in a scenario.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.

		@return Output names.
		*/
		System::Collections::Generic::List<System::String^>^
			getOutputs(
				System::String^ primaryname,
				System::String^ scenario);

		// DocString: FMTExcelCache::getYields
		/**
		@brief Gets all yields available in a scenario.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.

		@return Yield names.
		*/
		System::Collections::Generic::List<System::String^>^
			getYields(
				System::String^ primaryname,
				System::String^ scenario);

		// DocString: FMTExcelCache::getThemes
		/**
		@brief Gets all themes available in a scenario.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.

		@return Theme names.
		*/
		System::Collections::Generic::List<System::String^>^
			getThemes(
				System::String^ primaryname,
				System::String^ scenario);

		// DocString: FMTExcelCache::getbuildexceptions
		/**
		@brief Gets build exceptions of a given type.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] exception Exception identifier.

		@return Build exception messages.
		*/
		System::Collections::Generic::List<System::String^>^
			getbuildexceptions(
				System::String^ primaryname,
				System::String^ scenario,
				int exception);

		// DocString: FMTExcelCache::getnochoice
		/**
		@brief Gets developments with no available actions.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] filter Theme selection filter.

		@return Development masks.
		*/
		System::Collections::Generic::List<System::String^>^
			getnochoice(
				System::String^ primaryname,
				System::String^ scenario,
				System::String^ filter);
		// DocString: FMTExcelCache::getRotations
		/**
		@brief Gets all rotations for a theme selection.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] themeselection Theme selection filter.
		@param[in] aggregate Aggregate name.

		@return Rotation names and lengths.
		*/
		System::Collections::Generic::List<
			System::Collections::Generic::KeyValuePair<
			System::String^,
			int>>^ getRotations(
				System::String^ primaryname,
				System::String^ scenario,
				System::String^ themeselection,
				System::String^ aggregate);

		// DocString: FMTExcelCache::getRotationsKeys
		/**
		@brief Gets unique rotation names.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] themeselection Theme selection filter.
		@param[in] aggregate Aggregate name.

		@return Unique rotation names.
		*/
		System::Collections::Generic::List<System::String^>^
			getRotationsKeys(
				System::String^ primaryname,
				System::String^ scenario,
				System::String^ themeselection,
				System::String^ aggregate);

		// DocString: FMTExcelCache::containsRotations
		/**
		@brief Determines whether a rotation exists.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] serie Rotation name.
		@param[in] themeselection Theme selection filter.
		@param[in] aggregate Aggregate name.

		@return True if the rotation exists.
		*/
		bool containsRotations(
			System::String^ primaryname,
			System::String^ scenario,
			System::String^ serie,
			System::String^ themeselection,
			System::String^ aggregate);

		// DocString: FMTExcelCache::getperiods
		/**
		@brief Gets all available planning periods.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.

		@return Period numbers.
		*/
		System::Collections::Generic::List<int>^
			getperiods(
				System::String^ primaryname,
				System::String^ scenario);

		// DocString: FMTExcelCache::getConstraints
		/**
		@brief Gets constraints associated with an output.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] output Output name.

		@return Constraint descriptions.
		*/
		System::Collections::Generic::List<System::String^>^
			getConstraints(
				System::String^ primaryname,
				System::String^ scenario,
				System::String^ output);

		// DocString: FMTExcelCache::getgraphstats
		/**
		@brief Gets graph statistics for a scenario.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.

		@return Graph statistics.
		*/
		System::Collections::Generic::List<int>^
			getgraphstats(
				System::String^ primaryname,
				System::String^ scenario);

		// DocString: FMTExcelCache::getGraphStatsSubset
		/**
		@brief Gets graph statistics for a thematic subset.

		@param[in] p_PrimaryName Primary project name.
		@param[in] p_Scenario Scenario name.
		@param[in] p_ThemeSelection Theme selection filter.

		@return Graph statistics.
		*/
		System::Collections::Generic::List<int>^
			getGraphStatsSubset(
				System::String^ p_PrimaryName,
				System::String^ p_Scenario,
				System::String^ p_ThemeSelection);

		// DocString: FMTExcelCache::writejpeg
		/**
		@brief Exports a theme representation to an image.

		@param[in] jpeglocation Output image path.
		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] themeid Theme index.
		@param[in] attributes Theme attribute values.

		@return True if the image was successfully created.
		*/
		bool writejpeg(
			System::String^ jpeglocation,
			System::String^ primaryname,
			System::String^ scenario,
			int themeid,
			System::Collections::Generic::List<
			System::String^>^ attributes);
		// DocString: FMTExcelCache::size
		/**
		@brief Gets the number of cached scenarios.

		@return Number of cached entries.
		*/
		int size();

		// DocString: FMTExcelCache::gotexception
		/**
		@brief Indicates whether an exception has been captured.

		@return True if an exception has occurred.
		*/
		bool gotexception();

		// DocString: FMTExcelCache::unraiseexception
		/**
		@brief Clears the exception flag.
		*/
		void unraiseexception();

		// DocString: FMTExcelCache::getlogoutput
		/**
		@brief Gets the accumulated logger output.

		The logger buffer is cleared after the contents are
		retrieved.

		@return Logger output text.
		*/
		System::String^ getlogoutput();

		// DocString: FMTExcelCache::Juxtaposition
		/**
		@brief Computes adjacency statistics for a collection of
		theme selections.

		@param[in] primaryname Primary project name.
		@param[in] scenario Scenario name.
		@param[in] themeselection Theme selections.
		@param[in] yieldname Yield name.
		@param[in] output Output name.
		@param[in] ratio Adjacency threshold.
		@param[in] perimeters Operating area perimeter.

		@return Adjacency percentages by period.
		*/
		System::Collections::Generic::List<double>^
			Juxtaposition(
				System::String^ primaryname,
				System::String^ scenario,
				System::Collections::Generic::List<
				System::String^>^ themeselection,
				System::String^ yieldname,
				System::String^ output,
				double ratio,
				double perimeters);
private:

	// DocString: FMTExcelCache::m_parser
	/**
	@brief Model parser used to load projects and templates.
	*/
	Parser::FMTModelParser* m_parser;

	// DocString: FMTExcelCache::m_cacheLog
	/**
	@brief Logger used by the cache.
	*/
	Logging::FMTLogger* m_cacheLog;

	// DocString: FMTExcelCache::m_cacheExceptionHandler
	/**
	@brief Exception handler associated with the parser.
	*/
	Exception::FMTExcelExceptionHandler*
		m_cacheExceptionHandler;

	// DocString: FMTExcelCache::m_models
	/**
	@brief Cached models indexed by primary and scenario.
	*/
	std::unordered_map<
		std::string,
		FMTModelCache>* m_models;

	// DocString: FMTExcelCache::m_exceptionRaised
	/**
	@brief Indicates whether an exception was captured.
	*/
	bool m_exceptionRaised;

	// DocString: FMTExcelCache::_getPrimaryName
	/**
	@brief Gets the primary file name without path or extension.

	@param[in] primarylocation Primary file location.

	@return Primary file name.
	*/
	std::string _getPrimaryName(
		const std::string& primarylocation);

	// DocString: FMTExcelCache::_getMapPath
	/**
	@brief Gets the associated map path.

	Searches for a shapefile located in the Carte directory
	associated with the project.

	@param[in] primarylocation Primary project location.

	@return Path to the shapefile.
	*/
	std::string _getMapPath(
		const std::string& primarylocation);

	// DocString: FMTExcelCache::_captureException
	/**
	@brief Records that an exception occurred.

	@param[in] method Method where the exception happened.
	*/
	void _captureException(
		const std::string& method);

	// DocString: FMTExcelCache::_formatForCache
	/**
	@brief Builds a unique cache identifier.

	Combines the primary name and scenario name into a
	single cache key.

	@param[in] primarylocation Primary project.
	@param[in] scenario Scenario name.

	@return Cache identifier.
	*/
	std::string _formatForCache(
		System::String^ primarylocation,
		System::String^ scenario);
	};
}