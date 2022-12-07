#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "FMTlpmodel.hpp"
#include "FMToutput.hpp"
#include "FMTtheme.hpp"
#include "FMTmask.hpp"
#include "FMTschedule.hpp"
#include "FMTexceptionhandler.hpp"
#include "FMTexception.hpp"
#include "FMTexcellogger.hpp"
#include "FMTmodelcache.hpp"
#include "FMTexception.hpp"
#include "FMTobject.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread.hpp>
#include "FMTforest.hpp"
#include "FMTareaparser.hpp"
#include <map>

namespace Wrapper
{

	FMTmodelcache::FMTmodelcache():
		Core::FMTobject(),
		mtx(new boost::recursive_mutex()),
		model(),
		outputs(),
		themes(),
		maplocation(),
		map(),
		generalcache()
	{
		
	}

	FMTmodelcache::~FMTmodelcache() = default;


	FMTmodelcache& FMTmodelcache::operator = (const FMTmodelcache& rhs)
	{
		if (this!=&rhs)
		{
			boost::lock(*mtx, *rhs.mtx);
			boost::lock_guard<boost::recursive_mutex> self_lock(*mtx, boost::adopt_lock);
			boost::lock_guard<boost::recursive_mutex> other_lock(*rhs.mtx, boost::adopt_lock);
			Core::FMTobject::operator = (rhs);
			model = std::move(rhs.model->clone());
			outputs = rhs.outputs;
			themes = rhs.themes;
			maplocation = rhs.maplocation;
			generalcache = rhs.generalcache;
			if (rhs.map)
				{
				map = std::move(std::unique_ptr<Spatial::FMTforest>(new Spatial::FMTforest(*rhs.map)));
				}
			
		}
		return *this;
	}

	FMTmodelcache::FMTmodelcache(const FMTmodelcache& rhs):
		Core::FMTobject(rhs),
		mtx(new boost::recursive_mutex()),
		model(),
		outputs(rhs.outputs),
		themes(rhs.themes),
		maplocation(rhs.maplocation),
		map(),
		generalcache(rhs.generalcache)
	{
		boost::lock_guard<boost::recursive_mutex> guard1(*mtx);
		boost::lock_guard<boost::recursive_mutex> guard2(*rhs.mtx);
		model = std::move(rhs.model->clone());
		if (rhs.map)
		{
			map = std::move(std::unique_ptr<Spatial::FMTforest>(new Spatial::FMTforest(*rhs.map)));
		}
		
	}

	void FMTmodelcache::loadmap() const
	{
		try {
			boost::lock_guard<boost::recursive_mutex> guard1(*mtx);
			if (!map && !maplocation.empty())
				{
				Parser::FMTareaparser areaparser;
				map = std::unique_ptr<Spatial::FMTforest>(new Spatial::FMTforest(areaparser.vectormaptoFMTforest(maplocation,250,model->getthemes(),"AGE","SUPERFICIE",1.0,1.0,"STANLOCK")));
				}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::loadmap", __LINE__, __FILE__);
		}
	}

	bool FMTmodelcache::writejpeg(const size_t& themeid, const std::vector<std::string>attributevalues, const std::string& jpeglocation) const
	{
		try {
			loadmap();
			if (map)
				{
				Parser::FMTareaparser areaparser;
				//_exhandler->raise(Exception::FMTexc::FMTinvalid_theme,
				//	"THEME id " + std::to_string(themeid), "FMTmodelcache::themeselectiontomask", __LINE__, __FILE__);
				const Core::FMTtheme theme = model->getthemes().at(themeid);
				std::map<std::string, std::string> layer_map;
				const std::vector<std::string>& allatributes = theme.getbaseattributes();
				size_t attributeid = 0;
				for (const std::string& attribute : allatributes)
					{
					if (attributeid<attributevalues.size())
						{
						layer_map[attribute] = attributevalues.at(attributeid);
					}else {
						layer_map[attribute] = attributevalues.back();
						}
						++attributeid; 
					}
				return areaparser.writeforesttheme(
					*map,
					theme,
					jpeglocation,
					layer_map,
					"BMP");
				}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::writejpeg", __LINE__, __FILE__);
		}
		return false;
	}


	FMTmodelcache::FMTmodelcache(const Models::FMTmodel& lmodel,
			const std::vector<Core::FMTschedule>& schedules,
			const std::string& lmaplocation):
		Core::FMTobject(),
		mtx(new boost::recursive_mutex()),
		model(),
		outputs(),
		themes(),
		maplocation(lmaplocation),
		map(),
		generalcache()
	{
		try {
			boost::lock_guard<boost::recursive_mutex> guard1(*mtx);
			model = std::unique_ptr<Models::FMTmodel>( new Models::FMTlpmodel(lmodel, Models::FMTsolverinterface::CLP));
			size_t outid = 0;
			for (const Core::FMToutput& output : model->getoutputs())
			{
				outputs[output.getname()] = outid;
				++outid;
			}
			size_t theid = 0;
			for (const Core::FMTtheme& theme : model->getthemes())
			{
				themes[theme.getname()] = theid;
				++theid;
			}
			model->setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD,true);
			model->setparameter(Models::FMTdblmodelparameters::TOLERANCE, 0.001);
			int period = 0;
			for (const Core::FMTschedule& schedule : schedules)
			{
				period = std::max(period, schedule.getperiod());
			}
			model->setparameter(Models::FMTintmodelparameters::LENGTH,period);
			//model->doplanning(false, schedules);
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTmodelcache::FMTmodelcache()", __LINE__, __FILE__);
			}
	}

	void FMTmodelcache::setsolution(const std::vector<Core::FMTschedule>& schedules) 
	{
		try {
			model->doplanning(false, schedules);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::initialize", __LINE__, __FILE__);
		}
	}

	int FMTmodelcache::getperiods() const
	{
		return model->getparameter(Models::FMTintmodelparameters::LENGTH);
	}


	Core::FMTmask FMTmodelcache::themeselectiontomask(const std::string& themeselection) const
	{
		Core::FMTmask subset;
		try {
		std::string mask;
		for (size_t thid = 0; thid < themes.size(); ++thid)
		{
			mask += "? ";
		}
		mask.pop_back();
		const std::vector<Core::FMTtheme>& themeofmodel = model->getthemes();
		subset = Core::FMTmask(mask, themeofmodel);
		if (themeselection.find('=')!=std::string::npos)
		{
			std::vector<std::string> results;
			boost::split(results, themeselection, boost::is_any_of(";"));
			for (std::string& value : results)
			{ 
				boost::trim(value);
				size_t stfind = value.find('=');
				if (stfind != std::string::npos)//need to subsettheme!
				{
					const std::string themename = value.substr(0, stfind);
					const std::string attribute = value.substr(stfind + 1, value.size());
					Core::FMTtheme const* localtheme = nullptr;
					if (themename.find("THEME") != std::string::npos)
					{
						const size_t themeid = static_cast<size_t>(std::stoi(themename.substr(themename.find("THEME") + 5)) - 1);
						if (!(themeid < themes.size()))
						{
							return Core::FMTmask();
						//_exhandler->raise(Exception::FMTexc::FMTinvalid_theme,
						//	"THEME id " + std::to_string(themeid), "FMTmodelcache::themeselectiontomask", __LINE__, __FILE__);
						}
						localtheme = &themeofmodel.at(themeid);
					}
					else {
						if (themes.find(themename) == themes.end())
						{
							return Core::FMTmask();
						//_exhandler->raise(Exception::FMTexc::FMTinvalid_theme,
						//	themename, "FMTmodelcache::themeselectiontomask", __LINE__, __FILE__);
						}
						localtheme = &themeofmodel.at(themes.at(themename));
					}
					if (localtheme)
					{
						if (localtheme->isaggregate(attribute) ||
							localtheme->isattribute((attribute)))
						{
							subset.set(*localtheme, attribute);
						}
						//attributes names is a vector of string that the string can be empty ... so we cannot search in it
						else if(!attribute.empty()) {
							const std::vector<std::string>& attributenames = localtheme->getattributenames();
							std::vector<std::string>::const_iterator ait = std::find(attributenames.begin(), attributenames.end(), attribute);
							if (ait == attributenames.end())//raise
							{
								return Core::FMTmask();
							//_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
							//	attribute, "FMTmodelcache::themeselectiontomask", __LINE__, __FILE__);
							}
							else {
								const std::string attname = localtheme->getbaseattributes().at(std::distance(attributenames.begin(), ait));
								subset.set(*localtheme, attname);
							}
						}
						else {
							return Core::FMTmask();
						}
					}
				}
			}	
		}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTmodelcache::themeselectiontomask", __LINE__, __FILE__);
			}
		return subset;
	}

	Core::FMToutput FMTmodelcache::getoutput(const std::string& outputname, const Core::FMTmask& subset) const
	{
		Core::FMToutput newoutput;
		try {
			std::vector<Core::FMTtheme> modelthemes = model->getthemes();
			std::unordered_map<std::string,size_t>::const_iterator outit = outputs.find(outputname);
			const std::vector<Core::FMToutput>& outputsofmodel = model->getoutputs();
			if (!outputsofmodel.empty()&&
				outit!= outputs.end())
				{
				const Core::FMToutput modeloutput = outputsofmodel.at(outit->second);
				if (subset.size() == subset.count()||subset.empty())//Only ? in mask so get the output from the model ... 
					{
					return modeloutput;
					}else {
						newoutput = modeloutput.intersectwithmask(subset, modelthemes);
					}
				}
			}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getoutput", __LINE__, __FILE__);
		}
		return newoutput;
	}

	double  FMTmodelcache::getvalue(const std::string& outputname, const std::string& themeselection, const int& period) const
	{
		double value = 0;
		try {
			const std::string cachekey = getcachekey("OUTPUT", outputname, themeselection, 0, period);
			const bool incache = fillfromcache(value, cachekey);
			if (!incache)
			{
				const Core::FMTmask subset = themeselectiontomask(themeselection);
				const Core::FMToutput theoutput = getoutput(outputname, subset);
				if (!outputs.empty() &&
					outputs.find(outputname) != outputs.end() &&
					!theoutput.empty())
				{
					boost::lock_guard<boost::recursive_mutex> guard(*mtx);
					value = model->getoutput(theoutput, period, Core::FMToutputlevel::totalonly).at("Total");
					settocache(cachekey, value);
				}
			}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getvalue", __LINE__, __FILE__);
		}
		return value;
	}

	double FMTmodelcache::getyield(const std::string& yieldname, const std::string& themeselection, const int& age, const int& period) const
	{
		double value = 0;
		try {
			const std::string cachekey = getcachekey("YIELD", yieldname, themeselection, age, period);
			const bool incache = fillfromcache(value, cachekey);
			if (!incache)
			{
				const Core::FMTyields& yields = model->getyields();
				const Core::FMTmask subset = themeselectiontomask(themeselection);
				if (!yields.empty() &&
					yields.isyld(yieldname) &&
					!(subset.empty() && !themeselection.empty()))
				{
					const Core::FMTdevelopment adev(subset, age, 0, period);
					const Core::FMTyieldrequest yieldrequest = adev.getyieldrequest();
					boost::lock_guard<boost::recursive_mutex> guard(*mtx);
					value = yields.get(yieldrequest, yieldname);
					settocache(cachekey, value);
				}
			}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getyield", __LINE__, __FILE__);
		}
		return value;
	}

	std::vector<std::string> FMTmodelcache::getattributes(const int& themeid, const std::string& value,const bool& aggregates) const
	{
		std::vector<std::string> attributes;
		try {
			const std::vector<Core::FMTtheme>& themes = model->getthemes();
			if (!themes.empty() &&
				(static_cast<size_t>(themeid) < themes.size()))
				{
				if (themes.at(themeid).isattribute(value) || themes.at(themeid).isaggregate(value))
					{
					attributes = themes.at(themeid).getattributes(value,aggregates);
				}else {
					attributes =  themes.at(themeid).getattributes("?",aggregates);
					}
				}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getattribute", __LINE__, __FILE__);
		}
		return attributes;
	}

	std::vector<std::string> FMTmodelcache::getaggregates(const int& themeid) const
	{
		std::vector<std::string> aggregates;
		try {
			const std::vector<Core::FMTtheme>& themes = model->getthemes();
			if (!themes.empty()&&
				(static_cast<size_t>(themeid) < themes.size()))
			{
				aggregates = themes.at(themeid).getaggregates();
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getaggregates", __LINE__, __FILE__);
		}
		return aggregates;
	
	}

	std::string FMTmodelcache::getcachekey(const std::string& type,
		const std::string& outputname, const std::string& themeselection,
		const int& age, const int& period) const
	{
		std::string key;
		try {
			key = type + "_" + outputname + "_" + themeselection + "_" + std::to_string(age) + "_" + std::to_string(period);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getcachekey", __LINE__, __FILE__);
		}
		return key;
	}
	bool FMTmodelcache::fillfromcache(double& value, const std::string& cachekey) const
	{
		bool gotincash = false;
		try {
			std::unordered_map<std::string,double>::const_iterator cacheit = generalcache.find(cachekey);
			if (cacheit!= generalcache.end())
				{
				value = cacheit->second;
				gotincash = true;
				}
			
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::fillfromcache", __LINE__, __FILE__);
		}
		return gotincash;

	}
	void FMTmodelcache::settocache(const std::string& cachekey, const double& value) const
	{
		try {
			boost::lock_guard<boost::recursive_mutex> guard(*mtx);
			generalcache[cachekey] = value;
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::settocache", __LINE__, __FILE__);
		}
	}

	
	std::vector<std::string> FMTmodelcache::getactionaggregates(const std::string& filter) const
	{
		try {
			for (const Core::FMTaction& action : model->getactions())
			{
				if (action.getname()==filter)
				{
					return action.getaggregates();
				}
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getactionaggregates", __LINE__, __FILE__);
		}
		return std::vector<std::string>();

	}


	std::vector<std::string> FMTmodelcache::getactions(const std::string& filter) const
	{
		std::vector<std::string>actions;
		try {
			for (const Core::FMTaction& action : model->getactions())
				{
				const std::vector<std::string> aggregates = action.getaggregates();
				if (filter=="?" || std::find(aggregates.begin(), aggregates.end(), filter) != aggregates.end())
					{
					actions.push_back(action.getname());
					}
				}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTmodelcache::getactions", __LINE__, __FILE__);
			}
		return actions;
	}

	std::vector<std::string> FMTmodelcache::getconstraints(const std::string& output) const
	{
		std::vector<std::string>constraints;
		try {
			for (const Core::FMTconstraint& constraint : model->getconstraints())
			{
				const std::string value = std::string(constraint);
				if (value.find(output)!=std::string::npos)
					{
					constraints.push_back(value);
					}
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getconstraints", __LINE__, __FILE__);
		}
		return constraints;

	}



	std::vector<std::string> FMTmodelcache::getoutputs() const
	{
		std::vector<std::string>outputs;
		try {
			for (const Core::FMToutput& output : model->getoutputs())
			{
				outputs.push_back(output.getname());
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getoutputs", __LINE__, __FILE__);
		}
		return outputs;

	}

	std::vector<std::string> FMTmodelcache::getyields() const
	{
		std::vector<std::string>yields;
		try {
			for (const std::string& name : model->getyields().getallyieldnames())
			{
				yields.push_back(name);
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getyields", __LINE__, __FILE__);
		}
		return yields;

	}

	std::vector<std::string> FMTmodelcache::getthemes() const
	{
		std::vector<std::string>themenames;
		try {
			for (const Core::FMTtheme& theme : model->getthemes())
			{
				themenames.push_back(theme.getname());
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getthemes", __LINE__, __FILE__);
		}
		return themenames;

	}



	Logging::FMTexcellogger* FMTmodelcache::getlogger()
		{
		boost::lock_guard<boost::recursive_mutex> guard(*mtx);
		Logging::FMTexcellogger* log = dynamic_cast<Logging::FMTexcellogger*>(this->_logger.get());
		return log;
		}

	void FMTmodelcache::putlogger(const std::shared_ptr<Logging::FMTlogger>& log)
		{
		boost::lock_guard<boost::recursive_mutex> guard(*mtx);
		this->passinlogger(log);
		}

}