#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include "FMTlpmodel.h"
#include "FMTlogger.h"

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

	class __declspec(dllexport) FMTmodelcache: private Models::FMTLpModel
	{
	public:
		FMTmodelcache();
		FMTmodelcache(const FMTmodelcache& rhs);
		FMTmodelcache& operator = (const FMTmodelcache& rhs);
		virtual ~FMTmodelcache();
		FMTmodelcache(const Models::FMTModel& lmodel, const std::string& lmaplocation);
		void setLength(const int& period);
		void setSolution(const std::vector<Core::FMTSchedule>& schedules);
		bool buildnsolve(bool solve = true);
		double getValue(const std::string& outputname, const std::string& themeselection, const int& period) const;
		double getYield(const std::string& yieldname, const std::string& themeselection, const int& age, const int& period) const;
		bool writejpeg(const size_t& themeid, const std::vector<std::string>attributevalues, const std::string& jpeglocation) const;
		std::vector<std::string> getAttributes(const int& themeid, const std::string& value, const bool& aggregates) const;
		std::vector<std::string> getattributesdescription(const int& themeid, const std::string& value) const;
		std::vector<std::string> getAggregates(const int& themeid) const;
		std::vector<std::string> getactions(const std::string& filter) const;
		std::vector<std::string> getactionaggregates(const std::string& filter) const;
		std::vector<std::string> getOutputs() const;
		std::vector<std::string> getYields() const;
		std::vector<std::string> getThemes() const;
		std::vector<std::string> getconstraints(const std::string& output) const;
		std::vector<std::string> getBuildExceptions(const int& exceptionid) const;
		std::vector<std::string> getnoaction(const std::string& filter) const;
		std::set<Core::FMTSerie> getRotations(const std::string& themeselection, const std::string& aggregate) const;
		bool haveSerie(const std::string& p_serie, const std::string& themeselection, const std::string& aggregate) const;
		std::vector<int> getGraphStatsSubset(const std::string& p_ThemeSelection) const;
		std::vector<int> getGraphStats() const;
		int getperiods() const;
		Logging::FMTExcelLogger* getlogger();
		void putlogger(const std::unique_ptr<Logging::FMTLogger>& log);
		std::vector<double> Juxtaposition(const std::vector<std::string>& themeselection, const std::string& yieldname, const std::string& output, const double& ratio, const double& perimeters) const;
	private:
		friend class Testing::UnitTestFMTmodelcache;
		bool cachingswitch;
		std::unique_ptr<boost::recursive_mutex>mtx;
		std::unordered_map<std::string,size_t>outputsmap;//output name as key
		std::unordered_map<std::string,size_t>themesmap;//Themes name as key
		mutable std::unordered_map<std::string,Core::FMTMask>maskcache;
		mutable std::unordered_map<std::string,Core::FMTOutput>outputcache;
		std::string maplocation;
		mutable std::unique_ptr<Spatial::FMTForest>map;
		mutable std::unordered_map<std::string,double>generalcache;
		mutable std::unordered_map<std::string,std::set<Core::FMTSerie>>SerieCache;
		Core::FMTMask globalmask;
		std::unique_ptr<boost::recursive_mutex>maskcachemtx;
		std::unique_ptr<boost::recursive_mutex>outputcachemtx;
		std::unique_ptr<boost::recursive_mutex>generalcachemtx;
		std::unique_ptr<boost::recursive_mutex>SerieCachemtx;
		std::unique_ptr<std::vector<Heuristics::FMTOperatingArea>>OAcache;
		std::unordered_map<int,std::vector<std::string>>all_exceptions;
		// DocString: FMTmodelcache::themeSelectionToMask
		/** 
		* @brief from a selection string like theme1=GS32;theme14=COS4 or theme1={GS405,GS3223};theme14=COS4 return a FMTMask
		* of use the one in cache.
		* @param[in] valid p_themeSelection in string value
		* @return a valid FMTMask representing the selection.
		* @throw invalid mask.
		* */
		Core::FMTMask themeSelectionToMask(const std::string& p_themeSelection) const;
		Core::FMTOutput getOutput(const std::string& outputname, const Core::FMTMask& subset) const;
		void loadmap() const;
		std::string getcachekey(const std::string& type,
			const std::string& outputname, const std::string& themeselection,
			const int& age, const int& period) const;
		bool fillfromcache(double& value,const std::string& cachekey) const;
		bool getSeriesFromCache(std::set<Core::FMTSerie>& value, const std::string& cachekey) const;
		void setSeriesToCache(const std::string& cachekey, const std::set<Core::FMTSerie>& value) const;
		void settocache(const std::string& cachekey, const double& value) const;
		bool getfrommaskcache(const std::string& cachekey,Core::FMTMask& mask) const;
		void writetomaskcache(const std::string& cachekey,const Core::FMTMask& mask) const;
		bool getfromoutputcache(const std::string& cachekey, Core::FMTOutput& output) const;
		void writetooutputcache(const std::string& cachekey, const Core::FMTOutput& output) const;
		double getvaluefrommodel(const Core::FMTOutput& output, const int& period) const;
		double getyieldfrommodel(const Core::FMTYieldRequest& request, const std::string& yieldname) const;
		void allocateressource();
		void setbaseressources();
	};

}