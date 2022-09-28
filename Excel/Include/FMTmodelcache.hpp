#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "FMTobject.hpp"
#include "FMTlogger.hpp"

namespace Models
{
	class FMTmodel;
}

namespace Core
{
	class FMToutput;
	class FMTtheme;
	class FMTmask;
	class FMTschedule;
}

namespace Logging
{
	class FMTexcellogger;
}

namespace boost
{
	class recursive_mutex;
}

namespace Spatial
{
	class FMTforest;
}

namespace Wrapper
{

	class FMTmodelcache: public Core::FMTobject
	{
		std::unique_ptr<boost::recursive_mutex>mtx;
		std::unique_ptr<Models::FMTmodel>model;
		std::unordered_map<std::string,size_t>outputs;//output name as key
		std::unordered_map<std::string,size_t>themes;//Themes name as key
		std::string maplocation;
		mutable std::unique_ptr<Spatial::FMTforest>map;
		Core::FMTmask themeselectiontomask(const std::string& themeselection) const;
		Core::FMToutput getoutput(const std::string& outputname, const Core::FMTmask& subset) const;
		void loadmap() const;
	public:
		FMTmodelcache();
		FMTmodelcache(const FMTmodelcache& rhs);
		FMTmodelcache& operator = (const FMTmodelcache& rhs);
		~FMTmodelcache();
		FMTmodelcache(const Models::FMTmodel& lmodel,const std::vector<Core::FMTschedule>& schedules,const std::string& lmaplocation);
		double getvalue(const std::string& outputname, const std::string& themeselection, const int& period) const;
		double getyield(const std::string& yieldname, const std::string& themeselection, const int& age, const int& period) const;
		bool writejpeg(const size_t& themeid,const std::vector<std::string>attributevalues, const std::string& jpeglocation) const;
		std::vector<std::string> getattributes(const int& themeid, const std::string& value) const;
		std::vector<std::string> getaggregates(const int& themeid) const;
		std::vector<std::string> getactions(const std::string& filter) const;
		std::vector<std::string> getoutputs() const;
		std::vector<std::string> getyields() const;
		std::vector<std::string> getthemes() const;
		int getperiods() const;
		Logging::FMTexcellogger* getlogger();
		void putlogger(const std::shared_ptr<Logging::FMTlogger>& log);

	};

}