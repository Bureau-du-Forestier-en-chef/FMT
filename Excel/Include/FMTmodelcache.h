#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include "FMTLpModel.h"
#include "FMTLogger.h"

namespace boost
{
	class recursive_mutex;
}

namespace Spatial
{
	class FMTForest;
}

namespace Heuristics
{
	class FMTOperatingArea;
}

namespace Logging
{
	class FMTExcelLogger;
}

namespace Testing
{
	class UnitTestFMTmodelcache;
}

namespace Wrapper
{

	class __declspec(dllexport) FMTModelCache: private Models::FMTLpModel
	{
	public:

		// DocString: FMTModelCache::FMTModelCache
		/**
		@brief Constructs an empty model cache.
		*/
		FMTModelCache();

		// DocString: FMTModelCache::FMTModelCache
		/**
		@brief Copy constructor.

		@param[in] rhs Source cache.
		*/
		FMTModelCache(
			const FMTModelCache& rhs);

		// DocString: FMTModelCache::operator=
		/**
		@brief Assigns another cache instance.

		@param[in] rhs Source cache.

		@return Reference to this instance.
		*/
		FMTModelCache& operator=(
			const FMTModelCache& rhs);

		// DocString: FMTModelCache::~FMTModelCache
		/**
		@brief Destroys the cache.
		*/
		virtual ~FMTModelCache();

		// DocString: FMTModelCache::FMTModelCache
		/**
		@brief Constructs a cache from a model.

		@param[in] lmodel Source model.
		@param[in] lmaplocation Spatial map location.
		*/
		FMTModelCache(
			const Models::FMTModel& lmodel,
			const std::string& lmaplocation);

		// DocString: FMTModelCache::setLength
		/**
		@brief Sets the planning horizon length.

		@param[in] period Number of planning periods.
		*/
		void setLength(
			const int& period);

		// DocString: FMTModelCache::setSolution
		/**
		@brief Sets a predefined solution.

		@param[in] schedules Schedule collection.
		*/
		void setSolution(
			const std::vector<Core::FMTSchedule>& schedules);

		// DocString: FMTModelCache::buildnsolve
		/**
		@brief Builds and optionally solves the optimization model.

		@param[in] solve True to solve the model after building it.

		@return True if the build succeeded and, when requested,
		the model was solved successfully.
		*/
		bool buildnsolve(
			bool solve = true);

		// DocString: FMTModelCache::getValue
		/**
		@brief Gets the value of an output.

		@param[in] outputname Output name.
		@param[in] themeselection Theme selection filter.
		@param[in] period Evaluation period.

		@return Output value.
		*/
		double getValue(
			const std::string& outputname,
			const std::string& themeselection,
			const int& period) const;

		// DocString: FMTModelCache::getYield
		/**
		@brief Gets a yield value.

		@param[in] yieldname Yield name.
		@param[in] themeselection Theme selection filter.
		@param[in] age Evaluation age.
		@param[in] period Evaluation period.

		@return Yield value.
		*/
		double getYield(
			const std::string& yieldname,
			const std::string& themeselection,
			const int& age,
			const int& period) const;

		// DocString: FMTModelCache::writejpeg
		/**
		@brief Exports a theme representation as an image.

		@param[in] themeid Theme index.
		@param[in] attributevalues Theme attribute values.
		@param[in] jpeglocation Output image path.

		@return True if the image was successfully generated.
		*/
		bool writejpeg(
			const size_t& themeid,
			const std::vector<std::string> attributevalues,
			const std::string& jpeglocation) const;

		// DocString: FMTModelCache::getAttributes
		/**
		@brief Gets attributes associated with a theme value.

		@param[in] themeid Theme index.
		@param[in] value Theme value.
		@param[in] aggregates Indicates whether aggregates are included.

		@return Attribute list.
		*/
		std::vector<std::string> getAttributes(
			const int& themeid,
			const std::string& value,
			const bool& aggregates) const;

		// DocString: FMTModelCache::getattributesdescription
		/**
		@brief Gets attribute descriptions for a theme value.

		@param[in] themeid Theme index.
		@param[in] value Theme value.

		@return Attribute descriptions.
		*/
		std::vector<std::string> getattributesdescription(
			const int& themeid,
			const std::string& value) const;

		// DocString: FMTModelCache::getAggregates
		/**
		@brief Gets aggregates defined for a theme.

		@param[in] themeid Theme index.

		@return Aggregate names.
		*/
		std::vector<std::string> getAggregates(
			const int& themeid) const;

		// DocString: FMTModelCache::getActions
		/**
		@brief Gets actions matching an aggregate filter.

		@param[in] filter Aggregate filter.

		@return Action names.
		*/
		std::vector<std::string> getActions(
			const std::string& filter) const;

		// DocString: FMTModelCache::getactionaggregates
		/**
		@brief Gets aggregates associated with an action.

		@param[in] filter Action name.

		@return Aggregate names.
		*/
		std::vector<std::string> getactionaggregates(
			const std::string& filter) const;

		// DocString: FMTModelCache::getOutputs
		/**
		@brief Gets all output names.

		@return Output names.
		*/
		std::vector<std::string> getOutputs() const;

		// DocString: FMTModelCache::getYields
		/**
		@brief Gets all yield names.

		@return Yield names.
		*/
		std::vector<std::string> getYields() const;

		// DocString: FMTModelCache::getThemes
		/**
		@brief Gets all theme names.

		@return Theme names.
		*/
		std::vector<std::string> getThemes() const;

		// DocString: FMTModelCache::getConstraints
		/**
		@brief Gets constraints associated with an output.

		@param[in] output Output name.

		@return Matching constraints.
		*/
		std::vector<std::string> getConstraints(
			const std::string& output) const;

		// DocString: FMTModelCache::getBuildExceptions
		/**
		@brief Gets build exceptions of a given type.

		@param[in] exceptionid Exception identifier.

		@return Exception messages.
		*/
		std::vector<std::string> getBuildExceptions(
			const int& exceptionid) const;

		// DocString: FMTModelCache::getnoaction
		/**
		@brief Gets developments with no available actions.

		@param[in] filter Theme selection filter.

		@return Development masks.
		*/
		std::vector<std::string> getnoaction(
			const std::string& filter) const;

		// DocString: FMTModelCache::getRotations
		/**
		@brief Gets rotation series matching a selection.

		@param[in] themeselection Theme selection filter.
		@param[in] aggregate Aggregate name.

		@return Rotation series.
		*/
		std::set<Core::FMTSerie> getRotations(
			const std::string& themeselection,
			const std::string& aggregate) const;

		// DocString: FMTModelCache::haveSerie
		/**
		@brief Determines whether a rotation series exists.

		@param[in] p_serie Series name.
		@param[in] themeselection Theme selection filter.
		@param[in] aggregate Aggregate name.

		@return True if the series exists.
		*/
		bool haveSerie(
			const std::string& p_serie,
			const std::string& themeselection,
			const std::string& aggregate) const;

		// DocString: FMTModelCache::getGraphStatsSubset
		/**
		@brief Gets graph statistics for a thematic subset.

		@param[in] p_ThemeSelection Theme selection filter.

		@return Graph statistics.
		*/
		std::vector<int> getGraphStatsSubset(
			const std::string& p_ThemeSelection) const;

		// DocString: FMTModelCache::getGraphStats
		/**
		@brief Gets graph statistics for the entire model.

		@return Graph statistics.
		*/
		std::vector<int> getGraphStats() const;

		// DocString: FMTModelCache::getperiods
		/**
		@brief Gets the number of planning periods.

		@return Planning horizon length.
		*/
		int getperiods() const;

		// DocString: FMTModelCache::getlogger
		/**
		@brief Gets the Excel logger associated with the model.

		@return Excel logger or nullptr if unavailable.
		*/
		Logging::FMTExcelLogger* getlogger();

		// DocString: FMTModelCache::putlogger
		/**
		@brief Assigns a logger to the model.

		@param[in] log Logger instance.
		*/
		void putlogger(
			const std::unique_ptr<Logging::FMTLogger>& log);

		// DocString: FMTModelCache::Juxtaposition
		/**
		@brief Computes operating area adjacency statistics.

		@param[in] themeselection Theme selections.
		@param[in] yieldname Yield name.
		@param[in] output Output name.
		@param[in] ratio Threshold ratio.
		@param[in] perimeters Operating area perimeter.

		@return Adjacency percentages by period.
		*/
		std::vector<double> Juxtaposition(
			const std::vector<std::string>& themeselection,
			const std::string& yieldname,
			const std::string& output,
			const double& ratio,
			const double& perimeters) const;
	private:

		friend class Testing::UnitTestFMTmodelcache;

		// DocString: FMTModelCache::m_cachingSwitch
		/**
		@brief Indicates whether caching is enabled.
		*/
		bool m_cachingSwitch;

		// DocString: FMTModelCache::m_mutex
		/**
		@brief Synchronizes access to model resources.
		*/
		std::unique_ptr<boost::recursive_mutex> m_mutex;

		// DocString: FMTModelCache::m_outputsMap
		/**
		@brief Maps output names to output indices.
		*/
		std::unordered_map<std::string, size_t> m_outputsMap;

		// DocString: FMTModelCache::m_themesMap
		/**
		@brief Maps theme names to theme indices.
		*/
		std::unordered_map<std::string, size_t> m_themesMap;

		// DocString: FMTModelCache::m_maskCache
		/**
		@brief Cache of thematic masks.
		*/
		mutable std::unordered_map<std::string, Core::FMTMask> m_maskCache;

		// DocString: FMTModelCache::m_outputCache
		/**
		@brief Cache of filtered outputs.
		*/
		mutable std::unordered_map<std::string, Core::FMTOutput> m_outputCache;

		// DocString: FMTModelCache::m_mapLocation
		/**
		@brief Source location of the spatial forest map.
		*/
		std::string m_mapLocation;

		// DocString: FMTModelCache::m_map
		/**
		@brief Loaded spatial forest representation.
		*/
		mutable std::unique_ptr<Spatial::FMTForest> m_map;

		// DocString: FMTModelCache::m_generalCache
		/**
		@brief Cache of computed numeric values.
		*/
		mutable std::unordered_map<std::string, double> m_generalCache;

		// DocString: FMTModelCache::m_serieCache
		/**
		@brief Cache of rotation series.
		*/
		mutable std::unordered_map<
			std::string,
			std::set<Core::FMTSerie>> m_serieCache;

		// DocString: FMTModelCache::m_globalMask
		/**
		@brief Default mask containing all developments.
		*/
		Core::FMTMask m_globalMask;

		// DocString: FMTModelCache::m_maskCacheMutex
		/**
		@brief Synchronizes access to the mask cache.
		*/
		std::unique_ptr<boost::recursive_mutex> m_maskCacheMutex;

		// DocString: FMTModelCache::m_outputCacheMutex
		/**
		@brief Synchronizes access to the output cache.
		*/
		std::unique_ptr<boost::recursive_mutex> m_outputCacheMutex;

		// DocString: FMTModelCache::m_generalCacheMutex
		/**
		@brief Synchronizes access to the general cache.
		*/
		std::unique_ptr<boost::recursive_mutex> m_generalCacheMutex;

		// DocString: FMTModelCache::m_serieCacheMutex
		/**
		@brief Synchronizes access to the rotation series cache.
		*/
		std::unique_ptr<boost::recursive_mutex> m_serieCacheMutex;

		// DocString: FMTModelCache::m_operatingAreaCache
		/**
		@brief Cached operating area definitions.
		*/
		std::unique_ptr<
			std::vector<Heuristics::FMTOperatingArea>>
			m_operatingAreaCache;

		// DocString: FMTModelCache::m_buildExceptions
		/**
		@brief Build exceptions generated during model construction.
		*/
		std::unordered_map<
			int,
			std::vector<std::string>>
			m_buildExceptions;

		// DocString: FMTModelCache::_themeSelectionToMask
		/**
		@brief Converts a thematic selection string into an FMT mask.

		@param[in] p_themeSelection Theme selection expression.

		@return Matching mask or an empty mask if invalid.
		*/
		Core::FMTMask _themeSelectionToMask(
			const std::string& p_themeSelection) const;

		// DocString: FMTModelCache::getOutput
		/**
		@brief Gets an output filtered by a mask.

		@param[in] outputname Output name.
		@param[in] subset Selection mask.

		@return Matching output definition.
		*/
		Core::FMTOutput getOutput(
			const std::string& outputname,
			const Core::FMTMask& subset) const;

		// DocString: FMTModelCache::_loadMap
		/**
		@brief Loads the spatial map on demand.
		*/
		void _loadMap() const;

		// DocString: FMTModelCache::getcachekey
		/**
		@brief Builds a cache key.

		@param[in] type Cache category.
		@param[in] outputname Output or yield name.
		@param[in] themeselection Theme selection.
		@param[in] age Age component.
		@param[in] period Period component.

		@return Unique cache key.
		*/
		std::string getcachekey(
			const std::string& type,
			const std::string& outputname,
			const std::string& themeselection,
			const int& age,
			const int& period) const;

		// DocString: FMTModelCache::fillfromcache
		/**
		@brief Gets a numeric value from the cache.

		@param[out] value Retrieved value.
		@param[in] cachekey Cache key.

		@return True if the value was found.
		*/
		bool fillfromcache(
			double& value,
			const std::string& cachekey) const;

		// DocString: FMTModelCache::getSeriesFromCache
		/**
		@brief Gets rotation series from the cache.

		@param[out] value Retrieved series.
		@param[in] cachekey Cache key.

		@return True if the series was found.
		*/
		bool getSeriesFromCache(
			std::set<Core::FMTSerie>& value,
			const std::string& cachekey) const;

		// DocString: FMTModelCache::setSeriesToCache
		/**
		@brief Stores rotation series in the cache.

		@param[in] cachekey Cache key.
		@param[in] value Series to cache.
		*/
		void setSeriesToCache(
			const std::string& cachekey,
			const std::set<Core::FMTSerie>& value) const;

		// DocString: FMTModelCache::settocache
		/**
		@brief Stores a numeric value in the cache.

		@param[in] cachekey Cache key.
		@param[in] value Value to cache.
		*/
		void settocache(
			const std::string& cachekey,
			const double& value) const;

		// DocString: FMTModelCache::getfrommaskcache
		/**
		@brief Gets a mask from the cache.

		@param[in] cachekey Cache key.
		@param[out] mask Retrieved mask.

		@return True if the mask was found.
		*/
		bool getfrommaskcache(
			const std::string& cachekey,
			Core::FMTMask& mask) const;

		// DocString: FMTModelCache::writetomaskcache
		/**
		@brief Stores a mask in the cache.

		@param[in] cachekey Cache key.
		@param[in] mask Mask to cache.
		*/
		void writetomaskcache(
			const std::string& cachekey,
			const Core::FMTMask& mask) const;

		// DocString: FMTModelCache::getfromoutputcache
		/**
		@brief Gets an output from the cache.

		@param[in] cachekey Cache key.
		@param[out] output Retrieved output.

		@return True if the output was found.
		*/
		bool getfromoutputcache(
			const std::string& cachekey,
			Core::FMTOutput& output) const;

		// DocString: FMTModelCache::writetooutputcache
		/**
		@brief Stores an output in the cache.

		@param[in] cachekey Cache key.
		@param[in] output Output to cache.
		*/
		void writetooutputcache(
			const std::string& cachekey,
			const Core::FMTOutput& output) const;

		// DocString: FMTModelCache::getvaluefrommodel
		/**
		@brief Evaluates an output directly from the model.

		@param[in] output Output definition.
		@param[in] period Evaluation period.

		@return Computed output value.
		*/
		double getvaluefrommodel(
			const Core::FMTOutput& output,
			const int& period) const;

		// DocString: FMTModelCache::getyieldfrommodel
		/**
		@brief Evaluates a yield directly from the model.

		@param[in] request Yield request.
		@param[in] yieldname Yield name.

		@return Yield value.
		*/
		double getyieldfrommodel(
			const Core::FMTYieldRequest& request,
			const std::string& yieldname) const;

		// DocString: FMTModelCache::_allocateResources
		/**
		@brief Allocates cache storage structures.

		Pre-allocates containers based on model size to reduce
		runtime memory allocations.
		*/
		void _allocateResources();

		// DocString: FMTModelCache::_setBaseResources
		/**
		@brief Initializes cache structures and lookup tables.

		Builds theme and output indices, creates the global mask
		and enables caching.
		*/
		void _setBaseResources();
	};

}