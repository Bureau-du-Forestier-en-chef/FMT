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

namespace Wrapper
{

	FMTmodelcache::FMTmodelcache():
		Core::FMTobject(),
		mtx(new boost::recursive_mutex()),
		model(),
		outputs(),
		themes()
	{
		
	}


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
		}
		return *this;
	}

	FMTmodelcache::FMTmodelcache(const FMTmodelcache& rhs):
		Core::FMTobject(rhs),
		mtx(new boost::recursive_mutex()),
		model(),
		outputs(rhs.outputs),
		themes(rhs.themes)
	{
		boost::lock_guard<boost::recursive_mutex> guard1(*mtx);
		boost::lock_guard<boost::recursive_mutex> guard2(*rhs.mtx);
		model = std::move(rhs.model->clone());
	}


	FMTmodelcache::FMTmodelcache(const Models::FMTmodel& lmodel, const std::vector<Core::FMTschedule>& schedules):
		Core::FMTobject(),
		mtx(new boost::recursive_mutex()),
		model(),
		outputs(),
		themes()
	{
		try {
			boost::lock_guard<boost::recursive_mutex> guard1(*mtx);
			model = std::unique_ptr<Models::FMTmodel>( new Models::FMTlpmodel(lmodel, Models::FMTsolverinterface::MOSEK));
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
			model->doplanning(false, schedules);
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTmodelcache::FMTmodelcache()", __LINE__, __FILE__);
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
		if (themeselection.find('=')!=std::string::npos)
			{
			std::vector<std::string> results;
			boost::split(results, themeselection, boost::is_any_of(";"));
			std::string mask;
			for (size_t thid = 0; thid < themes.size(); ++thid)
			{
				mask += "? ";
			}
			mask.pop_back();
			const std::vector<Core::FMTtheme>& themeofmodel = model->getthemes();
			subset =Core::FMTmask(mask, themeofmodel);
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
					const size_t themeid = static_cast<size_t>(std::stoi(value.substr(value.find("THEME") + 5, value.size())) - 1);
					if (!(themeid < themes.size()))
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_theme,
							"THEME id " + std::to_string(themeid), "FMTmodelcache::themeselectiontomask", __LINE__, __FILE__);
					}
					localtheme = &themeofmodel.at(themeid);

				}
				else {
					if (themes.find(themename) == themes.end())
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_theme,
							themename, "FMTmodelcache::themeselectiontomask", __LINE__, __FILE__);
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
					else {
						const std::vector<std::string>& attributenames = localtheme->getattributenames();
						std::vector<std::string>::const_iterator ait = std::find(attributenames.begin(), attributenames.end(), attribute);
						if (ait == attributenames.end())//raise
						{
							_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
								attribute, "FMTmodelcache::themeselectiontomask", __LINE__, __FILE__);
						}
						else {

							subset.set(*localtheme, localtheme->getbaseattributes().at(std::distance(attributenames.begin(), ait)));
						}
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
			std::unordered_map<std::string,size_t>::const_iterator outit = outputs.find(outputname);
			if (outit== outputs.end())
				{
				_exhandler->raise(Exception::FMTexc::FMTundefined_output,
					outputname, "FMTmodelcache::themeselectiontomask", __LINE__, __FILE__);
				}
			const std::vector<Core::FMToutput>& outputsofmodel = model->getoutputs();
			if (subset.empty())
				{
				return outputsofmodel.at(outit->second);
			}else {
				newoutput = outputsofmodel.at(outit->second).intersectwithmask(subset, model->getthemes());
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
			if (outputs.find(outputname)== outputs.end())
			{
				return 0;
			}
			const Core::FMTmask subset = themeselectiontomask(themeselection);
			const Core::FMToutput theoutput = getoutput(outputname, subset);
			boost::lock_guard<boost::recursive_mutex> guard(*mtx);
			value = model->getoutput(theoutput, period, Core::FMToutputlevel::totalonly).at("Total");

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
			const Core::FMTyields& yields = model->getyields();
			if (!yields.isyld(yieldname))
				{
				return 0;
				}
			const Core::FMTmask subset = themeselectiontomask(themeselection);
			const Core::FMTdevelopment adev(subset, age, 0, period);
			const Core::FMTyieldrequest yieldrequest = adev.getyieldrequest();
			boost::lock_guard<boost::recursive_mutex> guard(*mtx);
			value = yields.get(yieldrequest, yieldname);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getyield", __LINE__, __FILE__);
		}
		return value;
	}

	std::vector<std::string> FMTmodelcache::getattributes(const int& themeid, const std::string& value) const
	{
		std::vector<std::string> attributes;
		try {
			const std::vector<Core::FMTtheme>& themes = model->getthemes();
			if (!(static_cast<size_t>(themeid)<themes.size()))
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_theme,
					"THEME"+std::to_string(themeid), "FMTmodelcache::getattribute", __LINE__, __FILE__);
				}
			attributes = themes.at(themeid).getattributes(value);
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
			if (!(static_cast<size_t>(themeid) < themes.size()))
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_theme,
					"THEME" + std::to_string(themeid), "FMTmodelcache::getaggregates", __LINE__, __FILE__);
			}
			aggregates = themes.at(themeid).getaggregates();
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getaggregates", __LINE__, __FILE__);
		}
		return aggregates;
	
	}



	std::vector<std::string> FMTmodelcache::getactions() const
	{
		std::vector<std::string>actions;
		try {
			for (const Core::FMTaction& action : model->getactions())
				{
				actions.push_back(action.getname());
				}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTmodelcache::getactions", __LINE__, __FILE__);
			}
		return actions;
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