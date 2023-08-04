/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <boost/algorithm/string.hpp>

#include "FMTmodel.hpp"
#include <chrono>
#include <memory>
#include "FMTschedule.hpp"
#include "FMTmodelstats.hpp"
#include "FMTaction.hpp"
#include "FMTdevelopmentpath.hpp"
#include "FMTageyieldhandler.hpp"
#include "FMTtimeyieldhandler.hpp"
#include "FMTmaskfilter.hpp"
#include "FMTexceptionhandler.hpp"
#include "FMTtransitionmask.hpp"
//#include "FMTmodelparser.hpp"


namespace Models{


void FMTmodel::setdefaultobjects()
	{
	try {
		if (std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator("_DEATH")) == actions.end())
		{
			_exhandler->raise(Exception::FMTexc::FMTundefineddeathaction,
				"_DEATH","FMTmodel::setdefaultobjects", __LINE__, __FILE__, Core::FMTsection::Action);
			actions.push_back(defaultdeathaction(lifespan, themes));
			//actions.back().passinobject(*this);
		}
		if (std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator("_DEATH")) == transitions.end())
		{
			_exhandler->raise(Exception::FMTexc::FMTundefineddeathtransition,
				"_DEATH","FMTmodel::setdefaultobjects", __LINE__, __FILE__, Core::FMTsection::Transition);
			transitions.push_back(defaultdeathtransition(lifespan, themes));
			//transitions.back().passinobject(*this);
		}
		for (Core::FMTaction& action : actions)
		{
			action.update();
		}
		statictransitionthemes = getstatictransitionthemes();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::setdefaultobjects", __LINE__, __FILE__);
		}
	
	}

std::vector<size_t>FMTmodel::getstatictransitionthemes() const
	{
	std::vector<size_t>statics;
	try {
		std::vector<Core::FMTtheme>bestthemes=themes;
		for (const Core::FMTtransition& transition : transitions)
		{
			bestthemes = transition.getstaticthemes(bestthemes);
		}
		for (const Core::FMTtheme& theme : bestthemes)
			{
			std::vector<Core::FMTtheme>::const_iterator basit = std::find_if(themes.begin(), themes.end(), Core::FMTthemecomparator(theme));
			if (basit!=themes.end())
				{
				statics.push_back(std::distance(themes.cbegin(), basit));
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getstatictransitionthemes", __LINE__, __FILE__);
		}
	return statics;
	}

std::vector<Core::FMTtheme>FMTmodel::getstaticpresolvethemes() const
	{
	std::vector<Core::FMTtheme>fullstatics(themes);
	try {
		for (const Core::FMTconstraint& constraint : constraints)
			{
			std::vector<Core::FMTtheme>newstatics;
			for (const Core::FMTtheme& theme : locatestaticthemes(constraint))
				{
				if (std::find_if(fullstatics.begin(), fullstatics.end(), Core::FMTthemecomparator(theme)) != fullstatics.end())
					{
					newstatics.push_back(theme);
					}
				}
			fullstatics= newstatics;
			}
		std::vector<Core::FMTaction>uactions(actions);
		for (Core::FMTaction& action : uactions)
			{
			action.unshrink(themes);
			for (const auto& actit : action)
				{
				std::vector<Core::FMTtheme>newstatics;
				for (const Core::FMTtheme& theme :  actit.first.getstaticthemes(fullstatics))
					{
					if (std::find_if(fullstatics.begin(), fullstatics.end(), Core::FMTthemecomparator(theme)) != fullstatics.end())
						{
						newstatics.push_back(theme);
						}
					}
				fullstatics = newstatics;
				}
			}
		Core::FMTyields uyields(yields);
		uyields.unshrink(themes);
		for (auto& yieldit : uyields)
			{
			std::vector<Core::FMTtheme>newstatics;
			for (const Core::FMTtheme& theme : yieldit.first.getstaticthemes(fullstatics))
			{
				if (std::find_if(fullstatics.begin(), fullstatics.end(), Core::FMTthemecomparator(theme)) != fullstatics.end())
				{
					newstatics.push_back(theme);
				}
			}
			fullstatics = newstatics;
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getstaticpresolvethemes", __LINE__, __FILE__);
		}
	return fullstatics;
	}


FMTmodel::FMTmodel() : Core::FMTobject(),parameters(),area(),themes(),actions(), transitions(),yields(),lifespan(),outputs(), constraints(),name(), statictransitionthemes()
{
	
}

FMTmodel::FMTmodel(const std::vector<Core::FMTactualdevelopment>& larea, const std::vector<Core::FMTtheme>& lthemes,
	const std::vector<Core::FMTaction>& lactions,
	const std::vector<Core::FMTtransition>& ltransitions, const Core::FMTyields& lyields, const Core::FMTlifespans& llifespan,
	const std::string& lname, const std::vector<Core::FMToutput>& loutputs,std::vector<Core::FMTconstraint> lconstraints,FMTmodelparameters lparameters) :
	Core::FMTobject(), parameters(lparameters),area(), themes(lthemes), actions(lactions), transitions(ltransitions),
	yields(lyields), lifespan(llifespan), outputs(loutputs), constraints(lconstraints), name(lname), statictransitionthemes()
	{
	setarea(larea);
	setdefaultobjects();
	cleanactionsntransitions();
	
	}

FMTmodel::FMTmodel(const FMTmodel& rhs):Core::FMTobject(rhs),parameters(rhs.parameters),area(rhs.area),themes(rhs.themes),actions(rhs.actions),
		 transitions(rhs.transitions),yields(rhs.yields),lifespan(rhs.lifespan), outputs(rhs.outputs), constraints(rhs.constraints),name(rhs.name),
		statictransitionthemes(rhs.statictransitionthemes)

	{
	
	}

FMTmodel& FMTmodel::operator = (const FMTmodel& rhs)
    {
    if (this!=&rhs)
        {
        Core::FMTobject::operator = (rhs);
		parameters = rhs.parameters;
        area = rhs.area;
        themes = rhs.themes;
        actions = rhs.actions;
        transitions = rhs.transitions;
        yields = rhs.yields;
        lifespan = rhs.lifespan;
		outputs = rhs.outputs;
		constraints = rhs.constraints;
        name = rhs.name;
		statictransitionthemes = rhs.statictransitionthemes;
        }
    return *this;
    }
std::vector<Core::FMTactualdevelopment>FMTmodel::getarea(int period,bool beforegrowanddeath) const
    {
    return area;
    }

FMTmodel FMTmodel::getcopy(int period) const
	{
	FMTmodel newmodel(*this);
	/*if (period > 0)
		{
		std::vector<Core::FMTaction>actionssubset;
		for (const Core::FMTaction& action : actions)
			{
			if (period<=action.getperiodupperbound())
				{
				actionssubset.push_back(action);
				}
			}
		newmodel.setactions(actionssubset);
		std::vector<Core::FMTconstraint>cleanedconstraint;
		std::vector<Core::FMTconstraint> modelconstraints = this->getconstraints();
		cleanedconstraint.push_back(*modelconstraints.begin());
		modelconstraints.erase(modelconstraints.begin());
		for (const Core::FMTconstraint& constraint : constraints)
			{
			if (period<=constraint.getageupperbound())
				{
				cleanedconstraint.push_back(constraint);
				}
			}
		newmodel.setconstraints(cleanedconstraint);
		}*/
	return newmodel;
	}

void FMTmodel::cleanactionsntransitions()
	{
	try {
		std::vector<Core::FMTaction>newactions;
		std::vector<Core::FMTtransition>newtransitions;
		//sort(actions.begin(), actions.end());
		for (size_t id = 0; id < actions.size(); ++id)
		{
			if (!actions[id].empty())
			{
				const std::vector<Core::FMTtransition>::iterator trn_it = std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator(actions[id].getname()));
				if (trn_it != transitions.end() && !trn_it->empty())
				{
					newactions.push_back(actions[id]);
					newtransitions.push_back(*trn_it);
				}
			}
		}
		actions = newactions;
		transitions = newtransitions;
		actions.shrink_to_fit();
		transitions.shrink_to_fit();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::cleanactionsntransitions", __LINE__, __FILE__);
		}
	}


bool FMTmodel::useactionserie() const
{
	bool gotseries = false;
	try {
		for (const Core::FMTaction& action : actions)
		{
			if (action.ispartofaserie())
			{
				gotseries = true;
				break;
			}
		}
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::useactionseries", __LINE__, __FILE__);
	}

	return gotseries;
}

size_t FMTmodel::getseriesmaxsize() const
{
	size_t maxsize = 0;
	try {
		if (useactionserie())
			{
			for (const Core::FMTaction& action : actions)
				{
				maxsize = std::max(maxsize, action.getlargestseriesize());
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getseriesmaxsize", __LINE__, __FILE__);
		}
	return maxsize;
}







Core::FMTaction FMTmodel::defaultdeathaction(const Core::FMTlifespans& llifespan,
										const std::vector<Core::FMTtheme>& lthemes)
	{
		const std::string actionname = "_DEATH";
		const bool lock = false;
		const bool reset = true;
		Core::FMTaction death_action(actionname, lock, reset);
		for (const auto& intobject : llifespan)
		{
			const std::string mask(intobject.first);
			const Core::FMTmask amask(mask, lthemes);
			Core::FMTspec specifier;
			specifier.addbounds(Core::FMTagebounds(Core::FMTsection::Action, std::numeric_limits<int>::max(), intobject.second));
			death_action.push_back(amask, specifier);
		}
		const std::string GCBMaggregate = "~GCBM:" + std::to_string(FMTGCBMDEATHID) + ":Stand Replacing Natural Succession";
		death_action.push_aggregate(GCBMaggregate);
		death_action.shrink();
	return death_action;
	}
Core::FMTtransition FMTmodel::defaultdeathtransition(const Core::FMTlifespans& llifespan,
											const std::vector<Core::FMTtheme>& lthemes)
	{
	const std::string transitionname = "_DEATH";
	Core::FMTtransition death_Transition(transitionname);
	const double target_proportion = 100;
	for (const auto& lfobject : llifespan)
		{
		const std::string mask(lfobject.first);
		const Core::FMTmask amask(mask, lthemes);
		Core::FMTfork fork;
		Core::FMTtransitionmask trmask(mask, lthemes, target_proportion);
		fork.add(trmask);
		death_Transition.push_back(amask, fork);
		}
	death_Transition.shrink();
	return death_Transition;
	}

void FMTmodel::setparallellogger(Logging::FMTlogger& logger)
	{
	try {
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			"FMTmodel::setparallellogger", __LINE__, __FILE__);
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTmodel::setparallellogger", __LINE__, __FILE__);
		}
	}



void FMTmodel::addoutput(const std::string& name,
	const std::string& maskstring, Core::FMTotar outputtarget,
	std::string action, std::string yield, std::string description, int targettheme)
	{
	try {
		std::vector<Core::FMToutputsource>sources;
		sources.push_back(Core::FMToutputsource(Core::FMTspec(), Core::FMTmask(maskstring, themes), outputtarget, yield, action,static_cast<int>(outputs.size()),targettheme));
		std::vector<Core::FMToperator>operators;
		outputs.push_back(Core::FMToutput(name, description,"", sources, operators));
		outputs.shrink_to_fit();
	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::addoutput", __LINE__, __FILE__);
	}

	}


void FMTmodel::addyieldhandlersfromptr(const std::vector<std::unique_ptr<Core::FMTyieldhandler>>& yieldhandlers)
{
	try {
		yields.unshrink(themes);
		for (const std::unique_ptr<Core::FMTyieldhandler>& yldhandler : yieldhandlers)
		{
			yields.push_back(yldhandler->getmask(),yldhandler);
		}
		yields.update();
	}catch(...){
			_exhandler->printexceptions("", "FMTmodel::addyieldhandlersfromptr", __LINE__, __FILE__);
	}
	
	
}


void FMTmodel::setconstraints(const std::vector<Core::FMTconstraint>& lconstraint)
	{
	try {
		constraints = lconstraint;
		constraints.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setconstraints", __LINE__, __FILE__);
	}
	}

bool  FMTmodel::operator == (const FMTmodel& rhs) const
	{
	return (name == rhs.name &&
		area == rhs.area &&
		themes == rhs.themes &&
		actions == rhs.actions &&
		transitions == rhs.transitions &&
		yields == rhs.yields &&
		lifespan == rhs.lifespan &&
		outputs == rhs.outputs &&
		constraints == rhs.constraints);
	}

bool FMTmodel::operator < (const FMTmodel& rhs) const
	{
	//strict ordering
	if (name < rhs.name)
		return true;
	if (rhs.name < name)
		return false;
	//To do
	/*if (area < rhs.area)
		return true;
	if (rhs.area < area)
		return false;
	if (themes < rhs.themes)
		return true;
	if (rhs.themes < themes)
		return false;
	if (actions < rhs.actions)
		return true;
	if (rhs.actions < actions)
		return false;
	if (transitions < rhs.transitions)
		return true;
	if (rhs.transitions < transitions)
		return false;
	if (yields < rhs.yields)
		return true;
	if (rhs.yields < yields)
		return false;
	if (lifespan < rhs.lifespan)
		return true;
	if (rhs.lifespan < lifespan)
		return false;
	if (outputs < rhs.outputs)
		return true;
	if (rhs.outputs < outputs)
		return false;
	if (constraints < rhs.constraints)
		return true;
	if (rhs.constraints < constraints)
		return false;*/
	return false;
	}



void FMTmodel::setarea(const std::vector<Core::FMTactualdevelopment>& ldevs)
    {
	try {
		area.clear();
		area.reserve(ldevs.size());
		for(Core::FMTactualdevelopment adev: ldevs)
		{
			if (adev.getlock()>0)
				{
				adev = adev.reducelocktodeath(lifespan);
				}
			std::vector<Core::FMTactualdevelopment>::iterator devit = std::find_if(area.begin(), area.end(), Core::FMTactualdevelopmentcomparator(&adev));
			if(devit != area.end())
			{
				devit->setarea(devit->getarea()+adev.getarea());
			}else{
				area.push_back(adev);
			}	
		}
		std::sort(area.begin(), area.end());
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setarea", __LINE__, __FILE__);
	}


    }
void FMTmodel::setthemes(const std::vector<Core::FMTtheme>& lthemes)
    {
	try {
		themes = lthemes;
		themes.shrink_to_fit();
		//After theme change every masks needs to be reevaluated?.
		statictransitionthemes = getstatictransitionthemes();
		statictransitionthemes.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setthemes", __LINE__, __FILE__);
	}


    }
void FMTmodel::setactions(const std::vector<Core::FMTaction>& lactions)
    {
	try {
		std::vector<Core::FMTtransition>newtransitions;
		std::vector<Core::FMTaction>newbaseactions;
		for (const Core::FMTaction& action : lactions)
		{
			std::vector<Core::FMTtransition>::const_iterator trn_iterator = std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator(action.getname()));
			if (trn_iterator != transitions.end())
			{
				newtransitions.push_back(*trn_iterator);
				newbaseactions.push_back(action);
			}
		}
		actions = newbaseactions;
		transitions = newtransitions;
		for (Core::FMTaction& action : actions)
		{
			action.update();
		}
		this->setdefaultobjects();
		actions.shrink_to_fit();
		transitions.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setactions", __LINE__, __FILE__);
	}


    }
void FMTmodel::settransitions(const std::vector<Core::FMTtransition>& ltransitions)
    {
	try {
		transitions = ltransitions;
		for (Core::FMTtransition& transition : transitions)
		{
			transition.update();
		}
		this->setdefaultobjects();
		statictransitionthemes = getstatictransitionthemes();
		statictransitionthemes.shrink_to_fit();
		transitions.shrink_to_fit();
	}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodel::settransitions", __LINE__, __FILE__);
		}

    }
void FMTmodel::setyields(const Core::FMTyields& lylds)
    {
	try {
		yields = lylds;
		yields.update();
	}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodel::setyields", __LINE__, __FILE__);
		}
    }
void FMTmodel::setlifespan(const Core::FMTlifespans& llifespan)
    {
	try {
		lifespan = llifespan;
		lifespan.update();
		this->setdefaultobjects();
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setlifespan", __LINE__, __FILE__);
	}

    }

void FMTmodel::setname(const std::string& newname)
	{
	name = newname;
	}

void FMTmodel::setoutputs(const std::vector<Core::FMToutput>& newoutputs)
	{
	try {
		outputs = newoutputs;
		outputs.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setoutputs", __LINE__, __FILE__);
	}

	}


std::vector<Core::FMTtheme> FMTmodel::locatestaticthemes(const Core::FMToutput& output, bool ignoreoutputvariables) const
{
	std::vector<Core::FMTtheme> bestthemes;
	try {
		//bestthemes = locatestatictransitionsthemes();
		//bestthemes = output.getstaticthemes(bestthemes, yields, ignoreoutputvariables);
		const std::vector<Core::FMTtheme>transitionstatic = locatestatictransitionsthemes();
		for (const Core::FMTtheme& theme : output.getstaticthemes(themes, yields, ignoreoutputvariables))
			{
			if (std::find_if(transitionstatic.begin(), transitionstatic.end(), Core::FMTthemecomparator(theme)) != transitionstatic.end())
				{
				bestthemes.push_back(theme);
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::locatestaticthemes", __LINE__, __FILE__);
		}
	return bestthemes;
}

std::vector<Core::FMTtheme>FMTmodel::locatestatictransitionsthemes() const
{
	std::vector<Core::FMTtheme>bestthemes;
	try {
		for (const size_t& location : statictransitionthemes)
		{
			bestthemes.push_back(themes.at(location));
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::locatestatictransitionsthemes", __LINE__, __FILE__);
	}
	return bestthemes;

}

std::vector<Core::FMTtheme>FMTmodel::locatenodestaticthemes(const Core::FMToutputnode& node,
	bool ignoreoutputvariables,
	std::vector<Core::FMTtheme> basethemes) const
{
	std::vector<Core::FMTtheme>statics;
	if (!basethemes.empty())
		{
		statics = basethemes;
	}
	else {
		statics = themes;
	}
	try {
		std::vector<std::string>yieldstolookat;
		if (node.source.isvariable())
		{
			if (!ignoreoutputvariables)
			{
				statics = node.source.getmask().getstaticthemes(statics);
			}
			const std::string yieldvalue = node.source.getyield();
			for (const std::string& yldbound : node.source.getylds())
			{
				if (yields.isyld(yldbound))
				{
					yieldstolookat.push_back(yldbound);
				}
			}
			if (!yieldvalue.empty())
			{
				yieldstolookat.push_back(yieldvalue);
			}
		}
		std::vector<std::pair<Core::FMTmask,std::unique_ptr<Core::FMTyieldhandler>>>::const_iterator handlerit = yields.begin();
		while (handlerit != yields.end() && !yieldstolookat.empty())
		{
			std::vector<std::string>::const_iterator yieldit = yieldstolookat.begin();
			while (yieldit != yieldstolookat.end() && !handlerit->second->containsyield(*yieldit))
			{
				++yieldit;
			}
			if (yieldit != yieldstolookat.end())
			{
				statics = Core::FMTmask(std::string(handlerit->first), themes).getstaticthemes(statics);
				yieldstolookat.erase(yieldit);
			}
			++handlerit;
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::locatenodestaticthemes", __LINE__, __FILE__);
	}
	return statics;


}



std::vector<Core::FMTtheme> FMTmodel::locatestaticthemes(const Core::FMToutputnode& node, bool ignoreoutputvariables) const
{
	std::vector<Core::FMTtheme>statics;
	try {
		statics = locatestatictransitionsthemes();
		statics = locatenodestaticthemes(node, ignoreoutputvariables, statics);
		
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::locatestaticthemes", __LINE__, __FILE__);
	}
	return statics;

}

std::vector<Core::FMTtheme> FMTmodel::locatedynamicthemes(const Core::FMToutput& output, bool ignoreoutputvariables) const
{
	std::vector<Core::FMTtheme>dynamicthemes;
	try {
		const std::vector<Core::FMTtheme>staticthemes = locatestaticthemes(output, ignoreoutputvariables);
		for (const Core::FMTtheme& theme : themes)
			{
			if (std::find_if(staticthemes.begin(), staticthemes.end(), Core::FMTthemecomparator(theme))==staticthemes.end())
				{
				dynamicthemes.push_back(theme);
				}
			}
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::locatedynamicthemes", __LINE__, __FILE__);
	}
	return dynamicthemes;
}


Core::FMTmask FMTmodel::getdynamicmask(const Core::FMToutput& output, bool ignoreoutputvariables) const
	{
	Core::FMTmask selection;
	try {
		const std::vector<Core::FMTtheme>staticcthemes = locatestaticthemes(output, ignoreoutputvariables);
		std::string basename;
		for (const Core::FMTtheme& theme : themes)
			{
			
			basename += "? ";
			}
		
		basename.pop_back();
		const Core::FMTmask submask(basename,themes);
		boost::dynamic_bitset<>bits = submask.getbitsetreference();
		for (const Core::FMTtheme& theme : staticcthemes)
			{
			const size_t start = static_cast<size_t>(theme.getstart());
			for (size_t bitid = start; bitid < (theme.size() + start); ++bitid)
				{
				bits[bitid] = false;
				}
			}
		selection = Core::FMTmask(basename, bits);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getdynamicmask", __LINE__, __FILE__);
		}
	return selection;
	}

Core::FMTmask FMTmodel::getdynamicmask(const Core::FMToutputnode& node, bool ignoreoutputvariables) const
{
	Core::FMTmask selection;
	try {
		std::vector<Core::FMTtheme>staticcthemes = locatestatictransitionsthemes();
		staticcthemes = locatenodestaticthemes(node, ignoreoutputvariables, staticcthemes);
		
		std::string basename;
		for (const Core::FMTtheme& theme : themes)
		{

			basename += "? ";
		}

		basename.pop_back();
		
		
		const Core::FMTmask submask(basename, themes);
		boost::dynamic_bitset<>bits = submask.getbitsetreference();
		for (const Core::FMTtheme& theme : staticcthemes)
		{
			
			const size_t start = static_cast<size_t>(theme.getstart());
			for (size_t bitid = start; bitid < (theme.size() + start); ++bitid)
			{
				bits[bitid] = false;
			}
		}
		selection = Core::FMTmask(basename, bits);
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getdynamicmask", __LINE__, __FILE__);
	}
	return selection;

}

bool FMTmodel::isstaticnode(const Core::FMToutputnode& node, double ratioofset) const
{
	try {
		if (node.source.isinventory()&&!node.source.isaction())
		{
			for (const size_t& staticid : statictransitionthemes)
			{
				const double nvalues = static_cast<double>(node.source.getmask().getsubsetcount(themes.at(staticid)));
				const double themesize = static_cast<double>(themes.at(staticid).size());
				if ((nvalues/themesize)<=ratioofset)
				{
					return true;
				}

			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::isstaticnode", __LINE__, __FILE__);
		}
	return false;
}

Core::FMTmask FMTmodel::getstaticmask(const Core::FMToutputnode& node, bool ignoreoutputvariables) const
{
	Core::FMTmask selection;
	try {
		const Core::FMTmask dymask =this->getdynamicmask(node, ignoreoutputvariables);
		const Core::FMTmask intersection = node.source.getmask();
		selection = dymask.getunion(intersection);
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getstaticmask", __LINE__, __FILE__);
	}
	return selection;
}


void FMTmodel::validatelistspec(const Core::FMTspec& specifier) const
	{
	try {
		for (const std::string& yldname : specifier.getylds())
		{
			if (!yields.isyld(yldname))
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_yield,yldname,
					"FMTmodel::validatelistspec", __LINE__, __FILE__);
			}
		}
	}catch (...)
		{
			_exhandler->raisefromcatch("","FMTmodel::validatelistspec", __LINE__, __FILE__);
		}
	}


bool FMTmodel::isvalid()
    {
	try {
		//this->setsection(Core::FMTsection::Landscape);
		for (const Core::FMTtheme& theme : themes)
		{
			if (theme.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTempty_theme,
					"for theme id: " + std::to_string(theme.getid()),
					"FMTmodel::isvalid", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
		}
		//this->setsection(Core::FMTsection::Area);
		for (const Core::FMTactualdevelopment& developement : area)
		{
			std::string name = std::string(developement.getmask());
			Core::FMTtheme::validate(themes, name);
		}
		//this->setsection(Core::FMTsection::Yield);
		this->validatelistmasks(yields);

		//this->setsection(Core::FMTsection::Lifespan);
		this->validatelistmasks(lifespan);

		//this->setsection(Core::FMTsection::Action);
		for (const Core::FMTaction& action : actions)
		{
			this->validatelistmasks(action);
			for (const auto& specobject : action)
			{
				validatelistspec(specobject.second);
			}
		}
		//this->setsection(Core::FMTsection::Transition);
		for (const Core::FMTtransition& transition : transitions)
		{
			this->validatelistmasks(transition);
			for (const auto& specobject : transition)
			{
				validatelistspec(specobject.second);
			}
		}
		if (actions.size() != transitions.size())
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalidAandT, "Model: " + name,
				"FMTmodel::isvalid",__LINE__, __FILE__);
		}
		for (size_t id = 0; id < actions.size(); ++id)
		{
			if (actions[id].getname() != transitions[id].getname())
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_action,
					"Model: " + name + " " + actions[id].getname(),
					"FMTmodel::isvalid", __LINE__, __FILE__);
			}
		}
		//this->setsection(Core::FMTsection::Outputs);
		for (const Core::FMToutput& output : outputs)
		{
			//Need a validate output function
			for (const Core::FMToutputsource& source : output.getsources())
			{
				if (source.isvariable())
				{
					std::string name = std::string(source.getmask());
					Core::FMTtheme::validate(themes, name, "for output " + output.getname());
					const std::string actionname = source.getaction();
					if (!actionname.empty())//need to check the targeted action!
					{

					}
				}
				validatelistspec(source);
			}
		}
		//this->setsection(Core::FMTsection::Empty);
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::isvalid", __LINE__, __FILE__);
	}

    return true;
    }

void  FMTmodel::clearactionscache()
	{
		for (auto& action:actions)
		{
			action.clearcache();
		}
	}

void  FMTmodel::clearyieldcache()
	{
		yields.clearcache();
	}

void  FMTmodel::cleartransitioncache()
	{
		for (auto& transition:transitions)
		{
			transition.clearcache();
		}
	}

void FMTmodel::clearcache()
	{
		cleartransitioncache();
		clearyieldcache();
		clearactionscache();
	}

Core::FMTmask FMTmodel::getbasemask(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
	{
	Core::FMTmask basemask(boost::dynamic_bitset<>(area.begin()->getmask().size(), false));
	try {
		optionaldevelopments.insert(optionaldevelopments.end(), area.begin(), area.end());
		Core::FMTmask areamask(boost::dynamic_bitset<>(area.begin()->getmask().size(), false));
		for (const Core::FMTactualdevelopment& developement : optionaldevelopments)
			{
			areamask = areamask.getunion(developement.getmask());
			}
		size_t trid = 0;
		std::vector<bool>jumptransitions;
		for (const Core::FMTtransition& transition : transitions)
		{
			if (transition.size() == 1 &&
				actions.at(trid).size()==1&&
				transition.begin()->second.getmasktrans().size()==1&&
				std::string(actions.at(trid).begin()->first)==std::string(transition.begin()->first)&&
				std::string(transition.begin()->first)==std::string(transition.begin()->second.getmasktrans().begin()->getmask())&&
				!transition.begin()->second.getmasktrans().begin()->getmask().issubsetof(areamask))//scrap weird thing
				{
				jumptransitions.push_back(true);
			}else {
				for (const auto& transitionobject : transition)
				{
					const Core::FMTmask source(std::string(transitionobject.first),themes);
					for (const Core::FMTtransitionmask& fork : transitionobject.second.getmasktrans())
					{
						const Core::FMTmask maskwithoutaggregates = fork.getmask().removeaggregates(themes);
						basemask = basemask.getunion(maskwithoutaggregates);
					}
				}
				jumptransitions.push_back(false);
			}
			++trid;
		}
		if (!getparameter(FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES))
		{
			basemask = basemask.getunion(areamask);
		}else {
			trid = 0;
			for (const Core::FMTtransition& transition : transitions)
			{
				if (!jumptransitions.at(trid))
				{
					for (const auto& transitionobject : transition)
					{
						const Core::FMTmask source(std::string(transitionobject.first), themes);
						basemask = basemask.getunion(source.removeaggregates(themes, true));
					}
				}
				++trid;
			}
			size_t acid = 0;
			for (const Core::FMTaction& action : actions)
			{
				if (!jumptransitions.at(acid))
				{
					for (const auto& actionobject : action)
					{
						const Core::FMTmask opq(std::string(actionobject.first), themes);
						basemask = basemask.getunion(opq.removeaggregates(themes, true));
					}
				}
				++acid;
			}
			for (const auto& yieldobject : yields)
			{
				const Core::FMTmask source(std::string(yieldobject.first), themes);
				basemask = basemask.getunion(source.removeaggregates(themes, true));
			}
			for (const auto& lifespanobject : lifespan)
			{
				const Core::FMTmask source(std::string(lifespanobject.first), themes);
				basemask = basemask.getunion(source.removeaggregates(themes, true));
			}
			for (const Core::FMTconstraint& constraint : constraints)
			{
				for (const Core::FMToutputsource& source : constraint.getsources())
				{
					if (source.isvariable())
					{
						basemask = basemask.getunion(source.getmask().removeaggregates(themes, true));
					}
				}
			}
			boost::dynamic_bitset<>bits(basemask.size(),false);
			const boost::dynamic_bitset<>& areamaskref = areamask.getbitsetreference();
			const boost::dynamic_bitset<>& basemaskref = basemask.getbitsetreference();
			for (const Core::FMTtheme& theme : themes)
				{
				const size_t start = static_cast<size_t>(theme.getstart());
				if (basemask.getsubsetcount(theme) == 0)
					{ 
					const size_t areacount = areamask.getsubsetcount(theme);
					size_t biton = (theme.size() + start) - 1;
					if (areacount>=1)
						{
						while (!areamaskref[biton])
							{
							--biton;
							}
						}
					bits[biton] = true;
				}else {
					for (size_t bitid = start; bitid < (theme.size() + start); ++bitid)
						{
						bits[bitid] = (areamaskref[bitid]||basemaskref[bitid]);
						}
					}
				}
			basemask = basemask.getunion(Core::FMTmask(bits));
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::getbasemask", __LINE__, __FILE__);
		}
	return basemask;
	}

Core::FMTmaskfilter FMTmodel::getpostsolvefilter(const std::vector<Core::FMTtheme>& originalthemes,const Core::FMTmask& devmask) const
{
	try {
		const Core::FMTmask presolvemask = getselectedmask(originalthemes);
		const Core::FMTmask emptythemes = devmask.getpostsolvemask(presolvemask, originalthemes);
		return Core::FMTmaskfilter(presolvemask,emptythemes);
	}catch (...)
	{
		_exhandler->printexceptions("for " + name, "FMTmodel::getpostsolvefilter", __LINE__, __FILE__);
	}
	return Core::FMTmaskfilter();
}

Core::FMTmaskfilter FMTmodel::getpresolvefilter(const std::vector<Core::FMTtheme>& originalthemes) const
{
	try {
		const Core::FMTmask presolvemask = getselectedmask(originalthemes);
		return Core::FMTmaskfilter(presolvemask, presolvemask);
	}
	catch (...)
	{
		_exhandler->printexceptions("for " + name, "FMTmodel::getpresolvefilter", __LINE__, __FILE__);
	}
	return Core::FMTmaskfilter();
}

Core::FMTmask FMTmodel::getselectedmask(const std::vector<Core::FMTtheme>& originalthemes) const
	{
	Core::FMTmask newmask;
	size_t newmasksize = 0;
	for (const Core::FMTtheme& theme : originalthemes)
		{
		newmasksize += theme.size();
		}
	//const boost::dynamic_bitset<>& basewithoutpresolve = basedevelopment.getmask().getbitsetreference();
	boost::dynamic_bitset<>selection(newmasksize, false);
	//boost::dynamic_bitset<> selection(basewithoutpresolve);
	try {
		size_t bitselection = 0;
		size_t presolvedthemeid = 0;
		size_t themeid = 0;
		while (presolvedthemeid<themes.size()&&themeid< originalthemes.size())
			{
			const Core::FMTtheme& originaltheme = originalthemes.at(themeid);
			//const std::map<std::string, std::string> prsolvedvalues = themes.at(presolvedthemeid).getvaluenames();
			const std::vector<std::string>& prsolvedvalues = themes.at(presolvedthemeid).getbaseattributes();
			size_t foundcount = 0;
			std::vector<bool>themebits(originaltheme.size(),false);
			size_t bitid = 0;
			for (const std::string& themevalues : originaltheme.getbaseattributes())
				{
				if (std::find(prsolvedvalues.begin(), prsolvedvalues.end(), themevalues)!= prsolvedvalues.end()/*prsolvedvalues.find(themevalues.first) != prsolvedvalues.end()*/)
					{
					themebits[bitid] = true;
					++foundcount;
					}
				++bitid;
				
				}
			if (foundcount == prsolvedvalues.size())
				{
				for (const bool& bitvalue : themebits)
					{
					selection[bitselection] = bitvalue;
					++bitselection;
					}
				++presolvedthemeid;
			}else {
				/*for (size_t noloc = bitselection; noloc<(bitselection+themebits.size()); ++noloc)
					{
					selection[noloc] = basewithoutpresolve[noloc];
					}*/
				bitselection += themebits.size();
				}
			++themeid;
			}
		//tail
		/*for (size_t noloc = bitselection; noloc < basewithoutpresolve.size(); ++noloc)
			{
			selection[noloc] = basewithoutpresolve[noloc];
			}*/
		newmask = Core::FMTmask(selection);
		/*for (const Core::FMTtheme& theme : originalthemes)
		{
			*_logger << "Count " << newmask.getsubsetcount(theme) << "\n";
			if (newmask.getsubsetcount(theme) == 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
					"Non valid base presolve mask for model: " + name + " for theme id: " + std::to_string(theme.getid()),
					"FMTmodel::getselectedmask", __LINE__, __FILE__);
			}
		}*/


	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::getselectedmask", __LINE__, __FILE__);
		}
	return newmask;
	}

FMTmodel FMTmodel::basepresolve() const
{
	std::unique_ptr<FMTmodel>mdlptr;
	try {
		mdlptr = presolve(area);

	}catch (...)
		{
		_exhandler->printexceptions("for " + name, "FMTmodel::basepresolve", __LINE__, __FILE__);
		}
	return *mdlptr;
}


std::unique_ptr<FMTmodel> FMTmodel::presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
	{
	std::unique_ptr<FMTmodel>presolvedmodel;
	int presolvepass = getparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS);
	try {
		_logger->logwithlevel("Presolving " + getname() + "\n", 1);
		Core::FMTmaskfilter oldpresolvefilter(getbasemask(optionaldevelopments));
		//Base data
		std::vector<Core::FMTtheme>oldthemes(themes);
		std::vector<Core::FMTactualdevelopment>oldarea(area);
		std::vector<Core::FMTaction>oldactions(actions);
		std::vector<Core::FMTtransition>oldtransitions(transitions);
		Core::FMTyields oldyields(yields);
		Core::FMTlifespans oldlifespans(lifespan);
		std::vector<Core::FMToutput>oldoutputs(outputs);
		std::vector<Core::FMTconstraint>oldconstraints(constraints);
		size_t originalsize = themes.size() + area.size() + actions.size() + transitions.size() + lifespan.size() + outputs.size()+constraints.size()+yields.size();
		for (const Core::FMTaction& action : actions)
			{
			originalsize += action.size();
			}
		for (const Core::FMTtransition& transition : transitions)
			{
			originalsize += transition.size();
			}
		size_t newsize = 0;
		size_t oldsize = originalsize;
		//Core::FMTmask oldselectedattributes;
		//Passiterator
		bool didonepass = false;
		//_logger->redirectofile("C:/Users/cyrgu3/Desktop/test/nodes.txt");
		//std::set<int>constraintsids;
		std::vector<int>constraintsids;
		constraintsids.reserve(constraints.size());
		int constraintid = 0;
		for (const Core::FMTconstraint& constraint : constraints)
			{
			//constraintsids.insert(constraintid);
			constraintsids.push_back(constraintid);
			++constraintid;
			}
		while (presolvepass > 0 && newsize<oldsize)
		{
			//Presolved data
			
			std::vector<Core::FMTtheme>newthemes;
			std::vector<Core::FMTactualdevelopment>newarea;
			std::vector<Core::FMTaction>newactions;
			std::vector<Core::FMTtransition>newtransitions;
			Core::FMTyields newyields;
			Core::FMTlifespans newlifespans;
			std::vector<Core::FMToutput>newoutputs;
			std::vector<Core::FMTconstraint>newconstraints;
			Core::FMTmaskfilter newfilter(oldpresolvefilter);
			if (didonepass)
			{
				newfilter = newfilter.presolve(oldthemes);
				oldsize = newsize;
			}
			newsize = 0;
			//Core::FMTmask selectedattributes; //selected attribute keeps the binaries used by the new attribute selection.
			//Checkout to reduce the themes complexity
			size_t themeid = 0;
			size_t themestart = 0;
			std::set<int> keptthemeid;
			int oldthemeid = 0;
			for (const Core::FMTtheme& theme : oldthemes)
			{
				const Core::FMTtheme presolvedtheme = theme.presolve(newfilter, themeid, themestart);
				if (!presolvedtheme.empty())
				{
					keptthemeid.insert(oldthemeid);
					newthemes.push_back(presolvedtheme);
				}
				++oldthemeid;
			}
			newsize += newthemes.size();
			if (!newfilter.emptyflipped())
			{
				newarea.reserve(oldarea.size());
				boost::unordered_map<Core::FMTmask,Core::FMTmask>topresolve;
				for (const Core::FMTactualdevelopment& development : oldarea)
				{
					const Core::FMTmask devmask = development.getmask();
					Core::FMTactualdevelopment newdev(development);
					boost::unordered_map<Core::FMTmask, Core::FMTmask>::const_iterator mskit = topresolve.find(devmask);
					if (mskit != topresolve.end())
					{
						newdev.setmask(mskit->second);
					}
					else {
						const Core::FMTmask presolvedmask = devmask.presolve(newfilter, newthemes);
						topresolve[devmask] = presolvedmask;
						newdev.setmask(presolvedmask);
					}
					
					if (getparameter(FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES))
					{
						std::vector<Core::FMTactualdevelopment>::iterator devit = std::find_if(newarea.begin(), newarea.end(), Core::FMTactualdevelopmentcomparator(&newdev));
						if (devit != newarea.end())
						{
							devit->setarea(devit->getarea() + newdev.getarea());
						}
						else {
							newarea.push_back(newdev);
						}
					}else {
						newarea.push_back(newdev);
						}
					}
			}else {
				newarea = oldarea;
			}
			newsize += newarea.size();
			//reduce the number of actions and presolve the actions
			const std::vector<Core::FMTtheme>maskthemes = newfilter.getselectedthemes(oldthemes);
			for (const Core::FMTaction& action : oldactions)
			{
				const Core::FMTmask testedmask = action.getunion(oldthemes);
				if (newfilter.canpresolve(testedmask,maskthemes))
				{
					const Core::FMTaction presolvedaction = action.presolve(newfilter, oldthemes, newthemes,!didonepass);
					newactions.push_back(presolvedaction);
					newsize += presolvedaction.size();
				}
			}
			newsize += newactions.size();
			//std::cin.get();
			//reduce the number of transitions and presolve the transitions
			for (const Core::FMTtransition& transition : oldtransitions)
			{
				if (std::find_if(newactions.begin(), newactions.end(), Core::FMTactioncomparator(transition.getname())) != newactions.end())
				{
					const Core::FMTtransition presolvedtransition = transition.presolve(newfilter, oldthemes, newthemes,!didonepass);
					newtransitions.push_back(presolvedtransition);
					newsize += presolvedtransition.size();
				}
			}
			newsize += newtransitions.size();
			//Presolve yields
			newyields = oldyields.presolve(newfilter, oldthemes, newthemes);
			//Presolve lifespan data
			newlifespans = oldlifespans.presolve(newfilter, oldthemes, newthemes,!didonepass);
			newsize += newlifespans.size();
			//Outputs and data
			std::set<int> keptoutputid;
			int oloutputdid=0;
			for (const Core::FMToutput& output : oldoutputs)
			{
				const Core::FMToutput presolvedoutput = output.presolve(newfilter, oldthemes, maskthemes, newthemes, newactions, oldyields);
				if(!presolvedoutput.empty())
				{
					keptoutputid.insert(oloutputdid);
					newoutputs.push_back(presolvedoutput);
				}
				oloutputdid+=1;
			}
			newsize += newoutputs.size();
			for (Core::FMToutput& output : newoutputs)
			{
				output.changesourcesid(keptoutputid,keptthemeid);
			}
			//Constraints and data
			//Add feature to automatically interpret the output[0] as constant in sources
			//std::set<int>newconstraintsids;
			std::vector<int>newconstraintsids;
			int constraintid = 0;
			//std::set<int>::const_iterator oriit = constraintsids.begin();
			std::vector<int>::const_iterator oriit = constraintsids.begin();
			for (const Core::FMTconstraint& constraint : oldconstraints)
			{
				const int originalid = *oriit;
				/*const*/Core::FMTconstraint presolvedconstraint = constraint.presolve(newfilter, oldthemes, maskthemes, newthemes, newactions, oldyields);
				if (!presolvedconstraint.outputempty())
				{
					presolvedconstraint.changesourcesid(keptoutputid, keptthemeid);
					if (presolvedconstraint.canbeturnedtoyields())
					{
						presolvedconstraint.turntoyieldsandactions(newthemes, newactions, newyields,originalid);
					}else if(presolvedconstraint.canbeturnedtoyieldsbasedontransitions())
					{
						//*_logger << "Turning to " << std::string(Core::FMToutput(presolvedconstraint)) << "\n";
						presolvedconstraint.turntoyieldsbasedontransition(newthemes, newtransitions ,newactions, newyields,originalid);
						//Just to be sure that if there is a subset of a mask from the output in the section AREA, it's forced to change... 
						//Because after turntoyieldsbasedontransition(), the model cannot produce those type of development
						//need to check if it's in the area then push_it back after the presolve
						//newconstraintsids.insert(originalid);
						//newconstraints.push_back(presolvedconstraint);
					}
					else{
						//newconstraintsids.insert(originalid);
						newconstraintsids.push_back(originalid);
						newconstraints.push_back(presolvedconstraint);
					}
				}else if (presolvedconstraint.isspatial())
				{
					newconstraintsids.push_back(originalid);
					newconstraints.push_back(presolvedconstraint);
				}
				++oriit;
			}
			newsize += newconstraints.size();
			newsize += newyields.size();
			constraintsids.swap(newconstraintsids);
			oldthemes.swap(newthemes);
			oldarea.swap(newarea);
			oldactions.swap(newactions);
			oldtransitions.swap(newtransitions);
			oldyields.swap(newyields);
			oldlifespans.swap(newlifespans);
			oldoutputs.swap(newoutputs);
			oldconstraints.swap(newconstraints);
			//oldselectedattributes.swap(selectedattributes);
			oldpresolvefilter.swap(newfilter);
			--presolvepass;
			didonepass = true;
		}
		//logger Nb action : 1000 (presolved 500)
	oldthemes.shrink_to_fit();
	oldarea.shrink_to_fit();
	oldactions.shrink_to_fit();
	oldtransitions.shrink_to_fit();
	oldoutputs.shrink_to_fit();
	oldconstraints.shrink_to_fit();
	_logger->logwithlevel("Presolve stopped after " + std::to_string(getparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS) - presolvepass) + " iterations\n", 1);
	_logger->logwithlevel("Developments "+std::to_string(oldarea.size()) + "(" + std::to_string(static_cast<int>(oldarea.size())-static_cast<int>(area.size())) + "), "
				+"Themes "+std::to_string(oldthemes.size())+"(" + std::to_string(static_cast<int>(oldthemes.size())-static_cast<int>(themes.size())) + "), "
				+"Yields "+std::to_string(oldyields.size()) + "(" + std::to_string(static_cast<int>(oldyields.size())-static_cast<int>(yields.size())) + "), "
				+"Actions "+std::to_string(oldactions.size()) + "(" + std::to_string(static_cast<int>(oldactions.size())-static_cast<int>(actions.size())) + "), "
				+"Transitions "+std::to_string(oldtransitions.size()) + "(" + std::to_string(static_cast<int>(oldtransitions.size())-static_cast<int>(transitions.size())) + "), "
				+"Outputs "+std::to_string(oldoutputs.size()) + "(" + std::to_string(static_cast<int>(oldoutputs.size())-static_cast<int>(outputs.size())) + "), "
				+"Constraints "+std::to_string(oldconstraints.size()) + "(" + std::to_string(static_cast<int>(oldconstraints.size()) - static_cast<int>(constraints.size())) + ") and "
				+"Elements "+ std::to_string(newsize)+"("+std::to_string(static_cast<int>(newsize)- static_cast<int>(originalsize)) +")\n",1);
	std::array<std::string,7>sections{"Area","Themes","Yields","Actions","Transitions","Outputs","Constraints"};
	std::vector<size_t>sizeofsections;
	sizeofsections.push_back(oldarea.size());
	sizeofsections.push_back(oldthemes.size());
	sizeofsections.push_back(oldyields.size());
	sizeofsections.push_back(oldactions.size());
	sizeofsections.push_back(oldtransitions.size());
	sizeofsections.push_back(oldoutputs.size());
	sizeofsections.push_back(oldconstraints.size());
	size_t sectionid = 0;
	for (const std::string& section : sections)
	{
		if (sizeofsections.at(sectionid)==0)
		{
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Empty section: " + section + " after presolve",
				"FMTmodel::presolve", __LINE__, __FILE__);
		}
		++sectionid;
	}
	
	presolvedmodel = std::unique_ptr<FMTmodel>(new FMTmodel(oldarea, oldthemes, oldactions, oldtransitions, oldyields, oldlifespans, name, oldoutputs, oldconstraints,parameters));
	presolvedmodel->cleanactionsntransitions();
	}catch (...)
		{
		_exhandler->printexceptions("for "+name+"at presolve pass "+std::to_string(presolvepass),"FMTmodel::presolve", __LINE__, __FILE__);
		}
	return presolvedmodel;
	}

void FMTmodel::postsolve(const FMTmodel& originalbasemodel)
	{
	*this = FMTmodel(originalbasemodel);
	}

Core::FMTschedule FMTmodel::presolveschedule(const Core::FMTschedule& originalbaseschedule,
	const FMTmodel& originalbasemodel) const
	{
	Core::FMTschedule newschedule;
	try {
		const Core::FMTmaskfilter newfilter = getpresolvefilter(originalbasemodel.getthemes());
		newschedule = originalbaseschedule.presolve(newfilter, this->themes, this->actions);
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::presolveschedule", __LINE__, __FILE__);
		}
	return newschedule;
	}


FMTmodelstats FMTmodel::getmodelstats() const
	{
	size_t themesdatasize = 0;
	for (const Core::FMTtheme& theme : themes)
		{
		themesdatasize += theme.size();
		}
	size_t actionsdatasize = 0;
	for (const Core::FMTaction& action : actions)
		{
		actionsdatasize += action.size();
		}
	size_t transitionsdatasize = 0;
	for (const Core::FMTtransition& transition : transitions)
		{
		transitionsdatasize += transition.size();
		}
	size_t outputssdatasize = 0;
	for (const Core::FMToutput& output : outputs)
		{
		outputssdatasize += output.size();
		}
	size_t constraintsdatasize = 0;
	for (const Core::FMTconstraint& constraint : constraints)
		{
		constraintsdatasize += constraint.size();
		}
	return FMTmodelstats(themes.size(), themesdatasize, actions.size(), actionsdatasize, transitions.size(), transitionsdatasize,
		yields.size(), lifespan.size(), outputs.size(), outputssdatasize, constraints.size(), constraintsdatasize);
	}

bool FMTmodel::empty() const
	{
	return (area.empty() && actions.empty() && transitions.empty() &&
		yields.empty() && outputs.empty() && constraints.empty() && lifespan.empty());
	}

void FMTmodel::push_back(const FMTmodel& rhs)
	{
	try{
	//Need to check if the model have the same stats!
	const FMTmodelstats basestats = this->getmodelstats();
	const FMTmodelstats rhsstats = rhs.getmodelstats();
	if (basestats.themes == rhsstats.themes && basestats.themesdata == rhsstats.themesdata)
	{
		std::vector<Core::FMTtheme>newthemes = themes;//Need to concat themes!
		//Need to had some double check to make sure every elements are unique
		std::vector<Core::FMTactualdevelopment>newarea = area;
		for (const Core::FMTactualdevelopment& dev : rhs.area)//Need to check presence of!
		{
			std::vector<Core::FMTactualdevelopment>::iterator actualdev = std::find_if(newarea.begin(), newarea.end(), Core::FMTactualdevelopmentcomparator(&dev));
			if (actualdev == newarea.end())
			{
				newarea.push_back(dev);
			}
			else {
				actualdev->setarea(actualdev->getarea() + dev.getarea());
			}
		}
		std::vector<Core::FMTaction>finalactions = actions;
		std::vector<Core::FMTtransition>finaltransitions = transitions;
		size_t id = 0;
		for (const Core::FMTaction& action : rhs.actions)
		{
			std::vector<Core::FMTaction>::iterator actionitr = std::find_if(finalactions.begin(), finalactions.end(), Core::FMTactioncomparator(action.getname()));
			if (actionitr == finalactions.end())
			{
				finalactions.push_back(action);
				finaltransitions.push_back(rhs.transitions.at(id));
			}
			else {
				Core::FMTaction rhsaction(action);
				actionitr->unshrink(themes);
				rhsaction.unshrink(newthemes);
				actionitr->push_back(rhsaction);
				std::vector<Core::FMTtransition>::iterator transitionitr = std::find_if(finaltransitions.begin(), finaltransitions.end(), Core::FMTtransitioncomparator(action.getname()));
				if (transitionitr != transitions.end())
				{
					Core::FMTtransition rhstransition(rhs.transitions.at(id));
					rhstransition.unshrink(rhs.themes);
					transitionitr->unshrink(themes);
					transitionitr->push_back(rhstransition);
				}
			}
			++id;
		}
		std::vector<Core::FMToutput>finaloutputs = outputs;
		for (const Core::FMToutput& output : rhs.outputs)
		{
			if (std::find_if(finaloutputs.begin(), finaloutputs.end(), Core::FMToutputcomparator(output.getname())) == finaloutputs.end())
			{
				finaloutputs.push_back(output);
			}
		}
		std::vector<Core::FMTconstraint>finalconstraints = constraints;
		if (!rhs.constraints.empty())
		{
			std::vector<Core::FMTconstraint>constraintssubset = rhs.constraints;
			constraintssubset.erase(constraintssubset.begin());
			for (const Core::FMTconstraint& constraint : constraintssubset)
			{
				if (std::find_if(finalconstraints.begin(), finalconstraints.end(), Core::FMToutputcomparator(constraint.getname())) == finalconstraints.end())
				{
					finalconstraints.push_back(constraint);
				}
			}
		}
		Core::FMTyields newyields(yields);
		Core::FMTyields rhsyields(rhs.yields);
		newyields.unshrink(themes);
		rhsyields.unshrink(rhs.themes);
		newyields.push_back(rhs.yields);
		newyields.update();
		Core::FMTlifespans newlifespan(lifespan);
		Core::FMTlifespans rhslifespan(rhs.lifespan);
		newlifespan.unshrink(themes);
		rhslifespan.unshrink(rhs.themes);
		newlifespan.push_back(rhslifespan);
		/*area = newarea;
		themes = newthemes;
		actions = finalactions;
		transitions = finaltransitions;
		yields = newyields;
		lifespan = newlifespan;
		outputs = finaloutputs;
		constraints = finalconstraints;*/
		*this = rhs;
		}
		}catch (...)
			{
				_exhandler->printexceptions("", "FMTmodel::push_back", __LINE__, __FILE__);
			}


	}

double FMTmodel::getinitialarea() const
	{
	double totalarea = 0;
	for (const Core::FMTactualdevelopment& basedev : area)
		{
		totalarea += basedev.getarea();
		}
	return totalarea;
	}

void FMTmodel::setareaperiod(const int& period)
	{
	for (Core::FMTactualdevelopment& basedev : area)
		{
		basedev.setperiod(period);
		}
	}

Core::FMTschedule FMTmodel::getpotentialschedule(std::vector<Core::FMTactualdevelopment> toremove,
	std::vector<Core::FMTactualdevelopment> selection, bool withlock) const
{
	int period = 1;
	if (!selection.empty())
	{
		period = selection.back().getperiod();
	}
	Core::FMTschedule schedule(period,*this, withlock);

	try {
		boost::unordered_set<Core::FMTdevelopment>nottoprocess(toremove.begin(), toremove.end());
		size_t actionid = 0;
		for (const Core::FMTaction& action : actions)
			{
			std::vector<Core::FMTactualdevelopment>newselection;
			for (const Core::FMTactualdevelopment& actdev : selection)
				{
				if (nottoprocess.find(actdev)==nottoprocess.end()&&actdev.operable(action, yields))
					{
					schedule.addevent(actdev, 1.0, action);
					for (const Core::FMTdevelopmentpath& path : actdev.operate(action, transitions.at(actionid), yields, themes))
						{
						newselection.emplace_back(*path.development,1.0);
						}
					}
				}
			selection.insert(selection.end(),newselection.begin(), newselection.end());
			++actionid;
			}
		schedule.clean();
	}catch (...)
	{
		_exhandler->raisefromcatch("", " FMTmodel::getpotentialschedule", __LINE__, __FILE__);
	}
	return schedule;
}

std::vector<Core::FMTschedule>FMTmodel::setupschedulesforbuild(const std::vector<Core::FMTschedule>& schedules) const
{
	std::vector<Core::FMTschedule>newshedules;
try{
	bool gotemptyschedule = true;
	for (int period = 1; period <= parameters.getintparameter(LENGTH);++period)
		{
		Core::FMTschedule newschedule;
		for (const Core::FMTschedule& schedule : schedules)
			{
			if (schedule.getperiod()==period)
				{
				newschedule = schedule;
				gotemptyschedule = false;
				break;
				}
			}
		newshedules.push_back(newschedule);
		}
	if (parameters.getboolparameter(FORCE_PARTIAL_BUILD)&&gotemptyschedule)
		{
		_exhandler->raise(Exception::FMTexc::FMTignore,
			"Building natural growth graph for "+getname(),
			"FMTmodel::setupschedulesforbuild", __LINE__, __FILE__);
		}

	if (!parameters.getboolparameter(FORCE_PARTIAL_BUILD)&&!gotemptyschedule)
		{
		_exhandler->raise(Exception::FMTexc::FMTignore,
			"FMTschedule will be ignored for " + getname(),
			"FMTmodel::setupschedulesforbuild", __LINE__, __FILE__);
		}


}catch (...)
	{
	_exhandler->raisefromcatch("", "FMTmodel::setupschedulesforbuild", __LINE__, __FILE__);
	}
return newshedules;
}



bool FMTmodel::doplanning(const bool& solve,std::vector<Core::FMTschedule> schedules)
	{
	bool optimal_solved = false;
	try{
		const int presolve_iterations = parameters.getintparameter(PRESOLVE_ITERATIONS);
		std::unique_ptr<FMTmodel> presolved_model;
		if(presolve_iterations>0)
		{
			const std::chrono::time_point<std::chrono::high_resolution_clock>presolvestart = getclock();
			presolved_model = this->presolve(area);
			//Parser::FMTmodelparser mparser;
			//mparser.write(*presolved_model,"C:/Users/admlocal/Desktop/test/");
			_logger->logwithlevel("Presolved " + getname() + " " +getdurationinseconds(presolvestart) + "\n", 1);
		}else{
			presolved_model = this->clone();
		}
		std::vector<Core::FMTschedule> presolved_schedules;
		if(presolve_iterations>0 && !schedules.empty())
		{
			for (const Core::FMTschedule schedule : schedules )
			{
				presolved_schedules.push_back(presolved_model->presolveschedule(schedule,*this));
			}
		}else{
			presolved_schedules = schedules;
		}
		presolved_schedules = setupschedulesforbuild(presolved_schedules);
		const std::chrono::time_point<std::chrono::high_resolution_clock>buildstart = getclock();
		presolved_model->build(presolved_schedules);
		_logger->logwithlevel("Builded " + getname() +" "+getdurationinseconds(buildstart)+ "\n", 1);
		if(solve)
		{
			const std::chrono::time_point<std::chrono::high_resolution_clock>solverstart = getclock();
			optimal_solved = presolved_model->solve();
			_logger->logwithlevel("Solved " + getname() + " " + getdurationinseconds(solverstart) + "\n", 1);
		}
		if (parameters.getboolparameter(POSTSOLVE) && presolve_iterations > 0)
			{
			const std::chrono::time_point<std::chrono::high_resolution_clock>postsolvestart = getclock();
			presolved_model->postsolve(*this);
			_logger->logwithlevel("Postsolved " + getname() + " " + getdurationinseconds(postsolvestart) + "\n", 1);
			}
		this->swap_ptr(presolved_model);
	}catch(...){
		_exhandler->raisefromcatch("", " FMTmodel::doplanning", __LINE__, __FILE__);
	}
	return optimal_solved;
	}

std::vector<Core::FMTconstraint> FMTmodel::getreplanningconstraints(const std::string& modeltype, const std::vector<Core::FMTconstraint>& localconstraints, const int& period) const
{
	std::vector<Core::FMTconstraint>newconstraints(localconstraints.begin(), localconstraints.end());
	try {
		size_t constraintid = 0;
		for (const Core::FMTconstraint& constraint : localconstraints)
		{
			if (constraint.issetfrom(modeltype))
			{
				const double value = getoutput(constraint, period, Core::FMToutputlevel::totalonly).at("Total");
				newconstraints[constraintid] = constraint.setfrom(modeltype,value);
			}
			++constraintid;
		}
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getreplanningconstraints", __LINE__, __FILE__);
	}
	return newconstraints;
}
std::map<std::string, std::vector<std::vector<double>>>FMTmodel::getoutputsfromperiods(const std::vector<Core::FMToutput>& theoutputs,
	const int& firstperiod, const int& lastperiod, Core::FMToutputlevel level) const
	{
	std::map<std::string, std::vector<std::vector<double>>>outs;
	try {

		for (size_t outid = 0; outid < theoutputs.size(); ++outid)
			{
			size_t periodid = 0;
			for (int period = firstperiod; period <= lastperiod;++period)
				{
				for (const auto& values : this->getoutput(theoutputs.at(outid), period, level))
					{
					if (outs.find(values.first)==outs.end())
						{
						outs[values.first] = std::vector<std::vector<double>>(theoutputs.size(),std::vector<double>((lastperiod-firstperiod)+1,std::numeric_limits<double>::quiet_NaN()));
						}
					outs[values.first][outid][periodid] = values.second;
					}
				++periodid;
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getoutputsfromperiods", __LINE__, __FILE__);
		}
	return outs;
	}

std::unique_ptr<FMTmodel> FMTmodel::clone() const
	{
	return std::unique_ptr<FMTmodel>(new FMTmodel(*this));
	}

Core::FMTschedule FMTmodel::getsolution(int period, bool withlock) const
	{
	return Core::FMTschedule();
	}

std::map<std::string, double> FMTmodel::getoutput(const Core::FMToutput& output,
	int period, Core::FMToutputlevel level) const
{
	return std::map<std::string, double>();
}

bool FMTmodel::setparameter(const FMTintmodelparameters& key, const int& value)
{
	try{
		if (parameters.setintparameter(key,value)) return (true);
	}catch(...){
		_exhandler->printexceptions("", "FMTmodel::setparameter", __LINE__, __FILE__);
	}
	return false;
}

bool FMTmodel::setparameter(const FMTdblmodelparameters& key, const double& value)
{
	try{
		if (parameters.setdblparameter(key,value)) return (true);
	}catch(...){
		_exhandler->printexceptions("", "FMTmodel::setparameter", __LINE__, __FILE__);
	}
	return false;
}

bool FMTmodel::setparameter(const FMTboolmodelparameters& key, const bool& value)
{
	try{
		if (parameters.setboolparameter(key,value)) return (true);
	}catch(...){
		_exhandler->printexceptions("", "FMTmodel::setparameter", __LINE__, __FILE__);
	}
	return false;
}

int FMTmodel::getparameter(const FMTintmodelparameters& key)const
{
	int value;
	try{
		value = parameters.getintparameter(key);
	}catch(...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getparameter", __LINE__, __FILE__);
	}
	return value;
}

double FMTmodel::getparameter(const FMTdblmodelparameters& key)const
{
	double value;
	try{
		value = parameters.getdblparameter(key);
	}catch(...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getparameter", __LINE__, __FILE__);
	}
	return value;
}

bool FMTmodel::getparameter(const FMTboolmodelparameters& key) const
{
	bool value;
	try{
		value = parameters.getboolparameter(key);
	}catch(...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getparameter", __LINE__, __FILE__);
	}
	return value;
}

bool FMTmodel::setcompresstime(const int& periodstart, const int& periodstop, const int& value)
{
	bool returnbool = true;
	try{
		for (int period = (periodstart-1);period<periodstop;++period)
		{
			if (!parameters.setperiodcompresstime(period,value)) 
			{
				returnbool=false;
			}
		}
	}catch(...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::setcompresstime", __LINE__, __FILE__);
	}
	return returnbool;
}

std::vector<int> FMTmodel::getcompresstime() const
{
	return parameters.getcompresstime();
}

void FMTmodel::showparameters(const bool& showhelp)const
{
	std::string message=" - Parameters for model "+getname()+"\n";
	try{
	*_logger<<message<<"	-- Int parameters"<<"\n";
	message="	LENGTH                   : "+std::to_string(parameters.getintparameter(LENGTH))+"\n";
	if(showhelp) message+="	(The number of period to optimize or simulate. DEFAULT=30)\n";
	message+="	SEED                     : "+std::to_string(parameters.getintparameter(SEED))+"\n";
	if(showhelp) message+="	(The seed used for stochastique process in FMTsamodel, FMTnssmodel and FMTsesmodel. DEFAULT=25)\n";
	message+="	NUMBER_OF_ITERATIONS     : "+std::to_string(parameters.getintparameter(NUMBER_OF_ITERATIONS))+"\n";
	if(showhelp) message+="	(The number of iterations to do in FMTsesmodel::greedyreferencebuild. DEFAULT=10000)\n";
	message+="	PRESOLVE_ITERATIONS      : "+std::to_string(parameters.getintparameter(PRESOLVE_ITERATIONS))+"\n";
	if(showhelp) message+="	(The number of iterations to do in FMTmodel::presolve. DEFAULT=10)\n";
	message+="	NUMBER_OF_THREADS        : "+std::to_string(parameters.getintparameter(NUMBER_OF_THREADS))+"\n";
	if(showhelp) message+="	(Number of thread use by solver for optimisation. DEFAULT=Number of concurrent threads supported)\n";
	*_logger<<message<<"\n	-- Double parameters"<<"\n";
	message="	TOLERANCE                : "+std::to_string(parameters.getdblparameter(TOLERANCE))+"\n";
	if(showhelp) message+="	(Double tolerance used when setting a solution from schedules. DEFAULT="+std::to_string(FMT_DBL_TOLERANCE)+")\n";
	message+="	GOALING_SCHEDULE_WEIGHT  : "+std::to_string(parameters.getdblparameter(GOALING_SCHEDULE_WEIGHT))+"\n";
	if(showhelp) message+="	(The weight to use when trying goal a schedule from a strategic model. DEFAULT=10000)\n";
	*_logger<<message<<"\n	-- Bool parameters"<<"\n";
	std::string boolval;
	boolval = (parameters.getboolparameter(FORCE_PARTIAL_BUILD)) ? "true\n" : "false\n";
	message="	FORCE_PARTIAL_BUILD      : "+boolval;
	if(showhelp) message+="	(Force partial build on graph and matrix for period which schedules are passed to FMTlpmodel::doplanning. DEFAULT=false)\n";
	boolval = (parameters.getboolparameter(STRICTLY_POSITIVE)) ? "true\n" : "false\n";
	message+="	STRICTLY_POSITIVE        : "+boolval;
	if(showhelp) message+="	(Force matrix to have bound >= 0 for the outputs use in constraints or objective. DEFAULT=true)\n";
	boolval = (parameters.getboolparameter(POSTSOLVE)) ? "true\n" : "false\n";
	message+="	POSTSOLVE                : "+boolval;
	if(showhelp) message+="	(Postsolve model after doplanning. DEFAULT=true)\n";
	boolval = (parameters.getboolparameter(SHOW_LOCK_IN_SCHEDULES)) ? "true\n" : "false\n";
	message+="	SHOW_LOCK_IN_SCHEDULES   : "+boolval;
	if(showhelp) message+="	(When user ask for a schedule, if true, lock will appear in it. DEFAULT=false)\n";
	/*
	std::string compresstimes;
	std::vector<int>::const_iterator cit = compresstime.cbegin();
	while(*cit == *(cit+1))

	//Vector for compresstime at each period
	compresstime = std::vector<int>(30,1);
	*/
	*_logger<<message<<"\n";
	}catch(...)
	{

	}
}

void FMTmodel::swap_ptr(const std::unique_ptr<FMTmodel>& rhs)
{
	*this = std::move(*rhs); 
}


FMTmodelcomparator::FMTmodelcomparator(std::string name) :model_name(name) {}

bool FMTmodelcomparator::operator()(const FMTmodel& model) const
	{
	return(model_name == model.getname());
	}






}



BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTmodel)
