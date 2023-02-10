#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include "FMTlpmodel.hpp"
#include "FMTlogger.hpp"


namespace boost
{
	class recursive_mutex;
}

namespace Spatial
{
	class FMTforest;
}

namespace Logging
{
	class FMTexcellogger;
}

namespace Wrapper
{

	class FMTmodelcache: private Models::FMTlpmodel
	{
		bool cachingswitch;
		std::unique_ptr<boost::recursive_mutex>mtx;
		std::unordered_map<std::string,size_t>outputsmap;//output name as key
		std::unordered_map<std::string,size_t>themesmap;//Themes name as key
		mutable std::unordered_map<std::string,Core::FMTmask>maskcache;
		mutable std::unordered_map<std::string,Core::FMToutput>outputcache;
		std::string maplocation;
		mutable std::unique_ptr<Spatial::FMTforest>map;
		mutable std::unordered_map<std::string,double>generalcache;
		Core::FMTmask globalmask;
		std::unique_ptr<boost::recursive_mutex>maskcachemtx;
		std::unique_ptr<boost::recursive_mutex>outputcachemtx;
		std::unique_ptr<boost::recursive_mutex>generalcachemtx;
		Core::FMTmask themeselectiontomask(const std::string& themeselection) const;
		Core::FMToutput getoutput(const std::string& outputname, const Core::FMTmask& subset) const;
		void loadmap() const;
		std::string getcachekey(const std::string& type,
			const std::string& outputname, const std::string& themeselection,
			const int& age, const int& period) const;
		bool fillfromcache(double& value,const std::string& cachekey) const;
		void settocache(const std::string& cachekey, const double& value) const;
		bool getfrommaskcache(const std::string& cachekey,Core::FMTmask& mask) const;
		void writetomaskcache(const std::string& cachekey,const Core::FMTmask& mask) const;
		bool getfromoutputcache(const std::string& cachekey, Core::FMToutput& output) const;
		void writetooutputcache(const std::string& cachekey, const Core::FMToutput& output) const;
		double getvaluefrommodel(const Core::FMToutput& output, const int& period) const;
		double getyieldfrommodel(const Core::FMTyieldrequest& request, const std::string& yieldname) const;
		void allocateressource();
	public:
		FMTmodelcache();
		FMTmodelcache(const FMTmodelcache& rhs);
		FMTmodelcache& operator = (const FMTmodelcache& rhs);
		virtual ~FMTmodelcache();
		FMTmodelcache(const Models::FMTmodel& lmodel,const std::vector<Core::FMTschedule>& schedules,const std::string& lmaplocation);
		void setsolution(const std::vector<Core::FMTschedule>& schedules);
		double getvalue(const std::string& outputname, const std::string& themeselection, const int& period) const;
		double getyield(const std::string& yieldname, const std::string& themeselection, const int& age, const int& period) const;
		bool writejpeg(const size_t& themeid,const std::vector<std::string>attributevalues, const std::string& jpeglocation) const;
		std::vector<std::string> getattributes(const int& themeid, const std::string& value, const bool& aggregates) const;
		std::vector<std::string> getaggregates(const int& themeid) const;
		std::vector<std::string> getactions(const std::string& filter) const;
		std::vector<std::string> getactionaggregates(const std::string& filter) const;
		std::vector<std::string> getoutputs() const;
		std::vector<std::string> getyields() const;
		std::vector<std::string> getthemes() const;
		std::vector<std::string> getconstraints(const std::string& output) const;
		int getperiods() const;
		Logging::FMTexcellogger* getlogger();
		void putlogger(const std::shared_ptr<Logging::FMTlogger>& log);

	};

}