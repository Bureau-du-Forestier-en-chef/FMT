#include "stdafx.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "FMTlpmodel.h"
#include "FMToutput.h"
#include "FMTtheme.h"
#include "FMTmask.h"
#include "FMTschedule.h"
#include "FMTexceptionhandler.h"
#include "FMTexception.h"
#include "FMTexcellogger.h"
#include "FMTmodelcache.h"
#include "FMTexception.h"
#include "FMTobject.h"
#include <boost/algorithm/string.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread.hpp>
#include "FMTforest.h"
#include "FMTareaparser.h"
#include "FMToperatingareascheme.h"
#include "FMToperatingarea.h"
#include "FMTgraphstats.h"
#include "FMTexcelexceptionhandler.h"
#include <map>
#include "FMTSerie.h"


namespace Wrapper
{


	FMTmodelcache::FMTmodelcache():
		Models::FMTlpmodel(),
		cachingswitch(false),
		mtx(new boost::recursive_mutex()),
		outputsmap(),
		themesmap(),
		maskcache(),
		outputcache(),
		maplocation(),
		map(),
		generalcache(),
		SerieCache(),
		globalmask(),
		maskcachemtx(new boost::recursive_mutex()),
		outputcachemtx(new boost::recursive_mutex()),
		generalcachemtx(new boost::recursive_mutex()),
		SerieCachemtx(new boost::recursive_mutex()),
		OAcache(new std::vector<Heuristics::FMToperatingarea>()),
		all_exceptions()

	{

	}

	FMTmodelcache::~FMTmodelcache() = default;


	FMTmodelcache& FMTmodelcache::operator = (const FMTmodelcache& rhs)
	{
		if (this!=&rhs)
		{
			cachingswitch = rhs.cachingswitch;
			boost::lock(*mtx, *rhs.mtx);
			boost::lock_guard<boost::recursive_mutex> self_lock(*mtx, boost::adopt_lock);
			boost::lock_guard<boost::recursive_mutex> other_lock(*rhs.mtx, boost::adopt_lock);
			Core::FMTobject::operator = (rhs);
			outputsmap = rhs.outputsmap;
			themesmap = rhs.themesmap;
			maskcache = rhs.maskcache;
			outputcache = rhs.outputcache;
			maplocation = rhs.maplocation;
			generalcache = rhs.generalcache;
			SerieCache = rhs.SerieCache;
			Models::FMTlpmodel::operator=(rhs);
			globalmask = rhs.globalmask;
			OAcache = std::move(std::unique_ptr<std::vector<Heuristics::FMToperatingarea>>(new std::vector<Heuristics::FMToperatingarea>(*rhs.OAcache)));
			all_exceptions = rhs.all_exceptions;
			if (rhs.map)
				{
				map = std::move(std::unique_ptr<Spatial::FMTforest>(new Spatial::FMTforest(*rhs.map)));
				}
			//allocateressource();
		}
		return *this;
	}

	FMTmodelcache::FMTmodelcache(const FMTmodelcache& rhs):
		Models::FMTlpmodel(rhs),
		cachingswitch(rhs.cachingswitch),
		mtx(new boost::recursive_mutex()),
		outputsmap(rhs.outputsmap),
		themesmap(rhs.themesmap),
		maskcache(rhs.maskcache),
		outputcache(rhs.outputcache),
		maplocation(rhs.maplocation),
		map(),
		generalcache(rhs.generalcache),
		SerieCache(rhs.SerieCache),
		globalmask(rhs.globalmask),
		maskcachemtx(new boost::recursive_mutex()),
		outputcachemtx(new boost::recursive_mutex()),
		generalcachemtx(new boost::recursive_mutex()),
		SerieCachemtx(new boost::recursive_mutex()),
		OAcache(),
		all_exceptions(rhs.all_exceptions)
	{
		boost::lock_guard<boost::recursive_mutex> guard1(*mtx);
		boost::lock_guard<boost::recursive_mutex> guard2(*rhs.mtx);
		if (rhs.map)
		{
			map = std::move(std::unique_ptr<Spatial::FMTforest>(new Spatial::FMTforest(*rhs.map)));
		}
		OAcache = std::move(std::unique_ptr<std::vector<Heuristics::FMToperatingarea>>(new std::vector<Heuristics::FMToperatingarea>(*rhs.OAcache)));
		//allocateressource();
	}


	void FMTmodelcache::allocateressource()
		{
		try {
			if (cachingswitch&&
				this->getgraphsize() > 0 &&
				outputcache.empty()&&
				generalcache.empty()&&
				maskcache.empty())
				{
				const size_t nperiods = static_cast<size_t>(getperiods());
				size_t maxsizet = 0;
				for (const Core::FMTtheme& theme : themes)
					{
					maxsizet = std::max(maxsizet, theme.size());
					}
				maxsizet *= themes.size();
				outputcache.reserve(outputs.size() * maxsizet);
				generalcache.reserve(outputs.size() * nperiods * maxsizet);
				maskcache.reserve(maxsizet);
				}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTmodelcache::allocateressource", __LINE__, __FILE__);
			}
		
		}

	void FMTmodelcache::loadmap() const
	{
		try {
			boost::lock_guard<boost::recursive_mutex> guard1(*mtx);
			if (!map && !maplocation.empty())
				{
				Parser::FMTareaparser areaparser;
				map = std::unique_ptr<Spatial::FMTforest>(new Spatial::FMTforest(areaparser.vectormaptoFMTforest(maplocation,250,themes,"AGE","SUPERFICIE",1.0,1.0,"STANLOCK")));
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
				const Core::FMTtheme& theme = themes.at(themeid);
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

	void FMTmodelcache::setbaseressources()
	{
		try {
			size_t outid = 0;
			for (const Core::FMToutput& output : outputs)
			{
				outputsmap[output.getname()] = outid;
				++outid;
			}
			size_t theid = 0;
			for (const Core::FMTtheme& theme : themes)
			{
				themesmap[theme.getname()] = theid;
				++theid;
			}
			std::string mask;
			for (size_t thid = 0; thid < themes.size(); ++thid)
			{
				mask += "? ";
			}
			mask.pop_back();
			globalmask = Core::FMTmask(mask, themes);
			//setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
			Models::FMTmodel::setparameter(Models::FMTdblmodelparameters::TOLERANCE, 0.001);
			/*int period = 0;
			for (const Core::FMTschedule& schedule : schedules)
			{
				period = std::max(period, schedule.getperiod());
			}
			setparameter(Models::FMTintmodelparameters::LENGTH, period);*/
			cachingswitch = true;
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::setbaseressources", __LINE__, __FILE__);
		}

	}


	FMTmodelcache::FMTmodelcache(const Models::FMTmodel& lmodel, const std::string& lmaplocation):
		Models::FMTlpmodel(lmodel, Models::FMTsolverinterface::MOSEK),
		cachingswitch(false),
		mtx(new boost::recursive_mutex()),
		outputsmap(),
		themesmap(),
		maskcache(),
		outputcache(),
		maplocation(lmaplocation),
		map(),
		generalcache(),
		SerieCache(),
		globalmask(),
		maskcachemtx(new boost::recursive_mutex()),
		outputcachemtx(new boost::recursive_mutex()),
		generalcachemtx(new boost::recursive_mutex()),
		SerieCachemtx(new boost::recursive_mutex()),
		OAcache(new std::vector<Heuristics::FMToperatingarea>()),
		all_exceptions()
	{
		try {
			boost::lock_guard<boost::recursive_mutex> guard1(*mtx);
			setbaseressources();


		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::FMTmodelcache()", __LINE__, __FILE__);
		}

	}

	void  FMTmodelcache::setlength(const int& period)
	{
		try {
			setparameter(Models::FMTintmodelparameters::LENGTH, period);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::setlength", __LINE__, __FILE__);
		}

	}


	void FMTmodelcache::setsolution(const std::vector<Core::FMTschedule>& schedules) 
	{
		try {
			setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
			Exception::FMTexcelexceptionhandler* excelhandler = dynamic_cast<Exception::FMTexcelexceptionhandler*>(_exhandler.get());
			excelhandler->resetbuildexceptions();//Reset the exceptions
			doplanning(false, schedules);
			all_exceptions = excelhandler->getbuildexceptions();//get the exceptions generated
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::setsolution", __LINE__, __FILE__);
		}
	}

	bool FMTmodelcache::buildnsolve(bool solve)
	{
		try {
			Exception::FMTexcelexceptionhandler* excelhandler = dynamic_cast<Exception::FMTexcelexceptionhandler*>(_exhandler.get());
			excelhandler->resetbuildexceptions();//Reset the exceptions
			const bool optimal = doplanning(solve);
			all_exceptions = excelhandler->getbuildexceptions();//get the exceptions generated
			return (!solve || optimal);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::solve", __LINE__, __FILE__);
		}
		return false;
	}


	int FMTmodelcache::getperiods() const
	{
		return getparameter(Models::FMTintmodelparameters::LENGTH);
	}

	bool FMTmodelcache::getfrommaskcache(const std::string& cachekey, Core::FMTmask& mask) const
	{
		try {
			if (cachingswitch)
				{
					boost::lock_guard<boost::recursive_mutex> guard1(*maskcachemtx);
					std::unordered_map<std::string, Core::FMTmask>::iterator mskit = maskcache.find(cachekey);
					if (mskit != maskcache.end())
					{
						mask = mskit->second;
						return true;
					}
				}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getfrommaskcache", __LINE__, __FILE__);
		}
		mask= globalmask;
		return false;
	}
	void FMTmodelcache::writetomaskcache(const std::string& cachekey,const Core::FMTmask& mask) const
	{
		try {
			if (cachingswitch)
				{
				boost::lock_guard<boost::recursive_mutex> guard1(*maskcachemtx);
				maskcache[cachekey] = mask;
				}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::writetomaskcache", __LINE__, __FILE__);
		}
	}


	Core::FMTmask FMTmodelcache::themeSelectionToMask(const std::string& p_themeSelection) const
	{
		try {
		Core::FMTmask subset;
		if (getfrommaskcache(p_themeSelection, subset))
			{
			return subset;
			}
		if (p_themeSelection.find('=')!=std::string::npos)
		{
			
			std::vector<std::string> results;
			boost::split(results, p_themeSelection, boost::is_any_of(";"));
			std::string emptyMask;
			std::vector<std::string>composition;
			for (const Core::FMTtheme& theme : themes)
			{
				emptyMask += "? ";
				composition.push_back("?");
			}
			emptyMask.pop_back();
			subset = Core::FMTmask(emptyMask, themes);
			
			for (std::string& value : results)
			{ 
				boost::trim(value);
				size_t stfind = value.find('=');
				if (stfind != std::string::npos)//need to subsettheme!
				{
					const std::string themename = value.substr(0, stfind);
					Core::FMTtheme const* localtheme = nullptr;
					if (themename.find("THEME") != std::string::npos)
					{
						const size_t themeid = static_cast<size_t>(std::stoi(themename.substr(themename.find("THEME") + 5)) - 1);
						if (!(themeid < themes.size()))
						{
							return Core::FMTmask();
						}
						localtheme = &themes.at(themeid);
					}
					else {
						if (themesmap.find(themename) == themesmap.end())
						{
							return Core::FMTmask();
						}
						localtheme = &themes.at(themesmap.at(themename));
					}
					std::string data = value.substr(stfind + 1, value.size());
					boost::trim(data);
					std::vector<std::string>attributes;
					if (!data.empty() && data.at(0) == '{' && data.back() == '}')
					{
						data.pop_back();
						data.erase(data.begin());
						boost::split(attributes, data, boost::is_any_of(","));
					}
					else {
						attributes.push_back(data);
					}

					composition[std::distance(&*themes.begin(), localtheme)] = data;
					
					for (std::string& attribute : attributes)
					{
						boost::trim(attribute);
						Core::FMTmask localMask(subset);
						
						if (localtheme)
						{
							if (localtheme->isattribute(attribute) ||
								localtheme->isaggregate(attribute) ||
								attribute == "?")
							{
								localMask.set(*localtheme, attribute);
							}
							//attributes names is a vector of string that the string can be empty ... so we cannot search in it
							else if (!attribute.empty()) {
								const std::vector<std::string>& attributenames = localtheme->getattributenames();
								std::vector<std::string>::const_iterator ait = std::find(attributenames.begin(), attributenames.end(), attribute);
								if (ait == attributenames.end())//raise
								{
									return Core::FMTmask();
								}
								else {
									const std::string attname = localtheme->getbaseattributes().at(std::distance(attributenames.begin(), ait));
									localMask.set(*localtheme, attname);

								}
							}
							else {
								return Core::FMTmask();
							}
						}
					if (attributes.size()==1 ||
						*attributes.begin() == attribute)
						{
						subset = localMask;
						}else {
						subset = subset.getunion(localMask);
						std::string final_value;
						for (const std::string& VAL : composition)
							{
							final_value += VAL + " ";
							}
						final_value.pop_back();
						subset = Core::FMTmask(final_value, subset.getbitsetreference());
						}
					
					}
				}
			}	
			
			writetomaskcache(p_themeSelection, subset);
			return subset;
			
		}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTmodelcache::themeselectiontomask", __LINE__, __FILE__);
			}
		return Core::FMTmask();
	}

	


	Core::FMToutput FMTmodelcache::getoutput(const std::string& outputname, const Core::FMTmask& subset) const
	{
		try {
			std::unordered_map<std::string,size_t>::const_iterator outit = outputsmap.find(outputname);
			if (!outputs.empty()&&
				outit!= outputsmap.end())
				{
				if (subset.size() == subset.count()||subset.empty())//Only ? in mask so get the output from the model ... 
					{
					return  outputs.at(outit->second);
					}else {
					Core::FMToutput output;
					const std::string cachekey = outputname +"_"+ std::string(subset);
					if (!getfromoutputcache(cachekey, output))
						{
						output = outputs.at(outit->second).intersectwithmask(subset, themes);
						writetooutputcache(cachekey, output);
						}
					return output;
					}
				}
			}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getoutput", __LINE__, __FILE__);
		}
		return Core::FMToutput();
	}
	double FMTmodelcache::getvaluefrommodel(const Core::FMToutput& output, const int& period) const
	{
		double value = 0;
		try {

					boost::lock_guard<boost::recursive_mutex> guard(*mtx);
					value = Models::FMTlpmodel::getoutput(output, period, Core::FMToutputlevel::totalonly).at("Total");
					
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getvaluefrommodel", __LINE__, __FILE__);
		}
		return value;

	}



	bool FMTmodelcache::getfromoutputcache(const std::string& cachekey, Core::FMToutput& output) const
	{
		bool incache = false;
		try {
			if (cachingswitch)
			{
				boost::lock_guard<boost::recursive_mutex> guard(*outputcachemtx);
				std::unordered_map<std::string, Core::FMToutput>::iterator outit = outputcache.find(cachekey);
				if (outit != outputcache.end())
				{
					incache = true;
					output = outit->second;
				}
			}
		}catch (...)
		{
		_exhandler->printexceptions("", "FMTmodelcache::getfromoutputcache", __LINE__, __FILE__);
		}
		return incache;
	}

	void FMTmodelcache::writetooutputcache(const std::string& cachekey, const Core::FMToutput& output) const
	{
		try {
			if (cachingswitch)
			{
				boost::lock_guard<boost::recursive_mutex> guard(*outputcachemtx);
				outputcache[cachekey] = output;
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::writetooutputcache", __LINE__, __FILE__);
		}

	}

	std::vector<std::string> FMTmodelcache::getnoaction(const std::string& filter) const
	{
		std::vector<std::string>masks;
		try {
			const Core::FMTmask mask_filter = themeSelectionToMask(filter);
			if (!mask_filter.empty())
				{
				for (const Core::FMTdevelopment* dev : this->getnochoice(mask_filter))
					{
					masks.push_back(std::string(*dev));
					}
				}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getnoaction", __LINE__, __FILE__);
		}
		return masks;
	}

	std::vector<double> FMTmodelcache::Juxtaposition(const std::vector<std::string>& themeselection,const std::string& yieldname, const std::string& output, const double& ratio, const double& perimeters) const
	{
	std::vector<double>allratios;
	try{
		if (!maplocation.empty())
		{
			std::vector<Heuristics::FMToperatingareascheme>allscheme;
			std::vector<size_t>allmasks;
			boost::unordered_map<Core::FMTmask, size_t>masklocation;
			size_t idofit = 0;
			std::vector<std::string>selected;
			for (const std::string& thselection : themeselection)
			{
				const Core::FMTmask subset = themeSelectionToMask(thselection);
				if (!subset.empty())
				{
					std::vector<Heuristics::FMToperatingarea>::const_iterator itof = std::find_if(OAcache->begin(), OAcache->end(), Heuristics::FMToperatingareacomparator(subset));
					if (itof == OAcache->end())
						{
						allscheme.push_back(Heuristics::FMToperatingareascheme(Heuristics::FMToperatingarea(subset, perimeters), 2, 6, 6, 1, 1, 1));
					}else {
						const size_t location = std::distance(OAcache->cbegin(), itof);
						allmasks.push_back(location);
						masklocation[subset] = location;
					}
					selected.push_back(thselection);
					++idofit;
				}
				
			}
			if (!allscheme.empty())
				{
				Parser::FMTareaparser areaparser;
				const std::vector<Heuristics::FMToperatingareascheme> Ioop = areaparser.getschemeneighbors(allscheme, themes, maplocation, "AGE", "SUPERFICIE", 1.0, 1.0, "STANLOCK");
				for (const Heuristics::FMToperatingareascheme& scheme : Ioop)
					{
					allmasks.push_back(OAcache->size());
					masklocation[scheme.getmask()] = OAcache->size();
					OAcache->push_back(scheme);
					}
				}
			
			const double totalcos = static_cast<double>(allmasks.size());
			const bool got_output = (outputsmap.find(output) != outputsmap.end());
			for (int period = 1; period <= getperiods(); ++period)
				{
				size_t oaid = 0;
				double notrespected = 0.0;
				for (const std::string& thselection : selected)
					{
					if (getyield(yieldname, thselection, 0, period)>0)
					{
						for (const Core::FMTmask& neighbor : OAcache->at(allmasks.at(oaid)).getneighbors())
						{
							if (masklocation.find(neighbor)!= masklocation.end())
							{
								const std::string nselection = selected.at(masklocation.at(neighbor));
								if (!got_output || ((getyield(yieldname, nselection, 0, period) > 0) &&
									(getvalue(output, nselection, period) >= ratio)))
								{
									++notrespected;
									break;
								}
							}
								
						}
					}
					++oaid;
					}
				allratios.push_back(((totalcos - notrespected) / totalcos) * 100);
				}
		}
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTmodelcache::Juxtaposition", __LINE__, __FILE__);
		}
	return allratios;
	}


	double  FMTmodelcache::getvalue(const std::string& outputname, const std::string& themeselection, const int& period) const
	{
		double value = 0;
		try {
			const std::string cachekey = getcachekey("OUTPUT", outputname, themeselection, 0, period);
			const bool incache = fillfromcache(value, cachekey);
			if (!incache)
			{
				const Core::FMTmask subset = themeSelectionToMask(themeselection);
				const Core::FMToutput theoutput = getoutput(outputname, subset);
				if (!outputs.empty() &&
					outputsmap.find(outputname) != outputsmap.end() &&
					!theoutput.empty())
				{
					value = getvaluefrommodel(theoutput, period);
				}
				settocache(cachekey, value);
				
			}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getvalue", __LINE__, __FILE__);
		}
		return value;
	}

	double  FMTmodelcache::getyieldfrommodel(const Core::FMTyieldrequest& request, const std::string& yieldname) const
	{
		double value = 0;
		try {

			boost::lock_guard<boost::recursive_mutex> guard(*mtx);
			value = yields.get(request, yieldname);
			
		}
		catch (...)
		{
			_exhandler->printexceptions("", "getyieldfrommodel", __LINE__, __FILE__);
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
				//const Core::FMTyields& yields = model->getyields();
				const Core::FMTmask subset = themeSelectionToMask(themeselection);
				if (!yields.empty() &&
					yields.isyld(yieldname) &&
					!(subset.empty() && !themeselection.empty()))
				{
					const Core::FMTdevelopment adev(subset, age, 0, period);
					const Graph::FMTgraphvertextoyield graph_info = getGraphVertexToYield();
					const Core::FMTyieldrequest yieldrequest = adev.getyieldrequest(&graph_info);
					value = getyieldfrommodel(yieldrequest, yieldname);
				}
				settocache(cachekey, value);
				
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

	std::vector<std::string> FMTmodelcache::getattributesdescription(const int& themeid, const std::string& value) const
	{
		std::vector<std::string> attributes;
		try {
			if (!themes.empty() &&
				(static_cast<size_t>(themeid) < themes.size()))
			{
				std::vector<std::string>basenames = themes.at(themeid).getattributenames();
				std::vector<std::string>references;
				if (themes.at(themeid).isattribute(value))
				{
					references = themes.at(themeid).getattributes(value);
				}
				else {
					references = themes.at(themeid).getattributes("?");
				}
				for (const std::string& refvalue : references)
					{
					size_t ref_id = std::distance(references.begin(),std::find(references.begin(), references.end(), refvalue));
					attributes.push_back(basenames.at(ref_id));
					}

			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getattributesdescription", __LINE__, __FILE__);
		}
		return attributes;
	}


	std::vector<std::string> FMTmodelcache::getaggregates(const int& themeid) const
	{
		std::vector<std::string> aggregates;
		try {
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
				boost::lock_guard<boost::recursive_mutex> guard(*generalcachemtx);
				std::unordered_map<std::string, double>::const_iterator cacheit = generalcache.find(cachekey);
				if (cacheit != generalcache.end())
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
	bool FMTmodelcache::getSeriesFromCache(std::set<Core::FMTSerie>& value, const std::string& cachekey) const
	{
		bool gotincash = false;
		try {
			boost::lock_guard<boost::recursive_mutex> guard(*SerieCachemtx);
			std::unordered_map<std::string, std::set<Core::FMTSerie>>::const_iterator cacheit = SerieCache.find(cachekey);
			if (cacheit != SerieCache.end())
			{
				value = cacheit->second;
				gotincash = true;
			}

		}
		catch (...)
		{
			_exhandler->printexceptions("", " FMTmodelcache::getSeriesFromCache", __LINE__, __FILE__);
		}
		return gotincash;

	}



	void FMTmodelcache::settocache(const std::string& cachekey, const double& value) const
	{
		try {
			if (cachingswitch)
				{
				boost::lock_guard<boost::recursive_mutex> guard(*generalcachemtx);
				generalcache[cachekey] = value;
				}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::settocache", __LINE__, __FILE__);
		}
	}

	void FMTmodelcache::setSeriesToCache(const std::string& cachekey, const std::set<Core::FMTSerie>& value) const
	{
		try {
			if (cachingswitch)
			{
				boost::lock_guard<boost::recursive_mutex> guard(*SerieCachemtx);
				SerieCache[cachekey] = value;
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::setSeriesToCache", __LINE__, __FILE__);
		}
	}

	
	std::vector<std::string> FMTmodelcache::getactionaggregates(const std::string& filter) const
	{
		try {
			for (const Core::FMTaction& action : actions)
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
		std::vector<std::string>actionsname;
		actionsname.reserve(actions.size());
		try {
			for (const Core::FMTaction& action : actions)
				{
				const std::vector<std::string> aggregates = action.getaggregates();
				if (filter=="?" || std::find(aggregates.begin(), aggregates.end(), filter) != aggregates.end())
					{
					actionsname.push_back(action.getname());
					}
				}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTmodelcache::getactions", __LINE__, __FILE__);
			}
		return actionsname;
	}

	std::vector<std::string> FMTmodelcache::getconstraints(const std::string& output) const
	{
		std::vector<std::string>constraintsname;
		constraintsname.reserve(constraints.size());
		try {
			for (const Core::FMTconstraint& constraint : constraints)
			{
				const std::string value = std::string(constraint);
				if (value.find(output)!=std::string::npos)
					{
					constraintsname.push_back(value);
					}
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getconstraints", __LINE__, __FILE__);
		}
		return constraintsname;
	}

	std::vector<std::string>FMTmodelcache::getbuildexceptions(const int& exceptionid) const
		{
		std::vector<std::string>values;
		try {
			if (all_exceptions.find(exceptionid)!= all_exceptions.end())
			{
				values = all_exceptions.at(exceptionid);
			}
			
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTmodelcache::getbuildexceptions", __LINE__, __FILE__);
			}
		return values;
		}

	std::set<Core::FMTSerie> FMTmodelcache::getRotations(const std::string& themeselection, const std::string& aggregate) const
	{
		std::set<Core::FMTSerie> series;
		try {
			const std::string CacheKey = getcachekey("SERIE", aggregate, themeselection, 0,0);
			const bool gotSeries = getSeriesFromCache(series, CacheKey);
			if (!gotSeries)
			{
				const Core::FMTmask subset = themeSelectionToMask(themeselection);
				if (!aggregate.empty() && !subset.empty())
				{
					series = FMTsrmodel::getRotations(subset, aggregate);
				}
				setSeriesToCache(CacheKey, series);
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getrotations", __LINE__, __FILE__);
		}
		return series;
	}

	bool FMTmodelcache::haveSerie(const std::string& p_serie, const std::string& themeselection, const std::string& aggregate) const
	{
		bool gotIt = false;
		try {
			const Core::FMTmask subset = themeSelectionToMask(themeselection);
			if (!aggregate.empty() && !subset.empty())
			{
				const std::string CacheKey = getcachekey("SERIE", aggregate, themeselection, 0, 0);
				std::set<Core::FMTSerie>AllSeries;
				const bool gotSeries = getSeriesFromCache(AllSeries, CacheKey);
				if (!gotSeries)
				{
					AllSeries = FMTsrmodel::getRotations(subset, aggregate);
					setSeriesToCache(CacheKey, AllSeries);
				}
				const int length = getparameter(Models::FMTintmodelparameters::LENGTH);
				Core::FMTSerie lowerBound(p_serie, 0);
				Core::FMTSerie upperBound(p_serie, length+1);
				std::set<Core::FMTSerie>::const_iterator it = AllSeries.lower_bound(lowerBound);
				std::set<Core::FMTSerie>::const_iterator itUpper = AllSeries.upper_bound(upperBound);
				for (; it != itUpper;++it)
					{
					if (it->getSerie() == p_serie)
						{
						gotIt = true;
						break;
						}
					}				
				
			}

		}catch (...)
			{
				_exhandler->printexceptions("", "FMTmodelcache::haveSerie", __LINE__, __FILE__);
			}
		return gotIt;
	}


	std::vector<int> FMTmodelcache::getgraphstats() const
	{
		std::vector<int>stats;
		try {
			Graph::FMTgraphstats graphstats = FMTsrmodel::getgraphstats();
			stats.push_back(graphstats.cols);
			stats.push_back(graphstats.rows);
			stats.push_back(graphstats.vertices);
			stats.push_back(graphstats.edges);
			stats.push_back(graphstats.transfer_rows);
			stats.push_back(graphstats.output_rows);
			stats.push_back(graphstats.output_cols);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getgraphstats", __LINE__, __FILE__);
		}
		return stats;
	}



	std::vector<std::string> FMTmodelcache::getoutputs() const
	{
		std::vector<std::string>outputsname;
		outputsname.reserve(outputs.size());
		try {
			for (const Core::FMToutput& output : outputs)
			{
				outputsname.push_back(output.getname());
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getoutputs", __LINE__, __FILE__);
		}
		return outputsname;

	}

	std::vector<std::string> FMTmodelcache::getyields() const
	{
		try {
			return yields.getallyieldnames();
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTmodelcache::getyields", __LINE__, __FILE__);
		}
		return std::vector<std::string>();

	}

	std::vector<std::string> FMTmodelcache::getthemes() const
	{
		std::vector<std::string>themenames;
		themenames.reserve(themes.size());
		try {
			for (const Core::FMTtheme& theme : themes)
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
		Core::FMTobject::passinlogger(log);
		//this->passinlogger(log);
		}

}