/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <boost/algorithm/string.hpp>

#include "FMTmodel.h"
#include <chrono>
#include <memory>
#include "FMTschedule.h"


namespace Models{


void FMTmodel::setdefaultobjects()
	{
	try {
		if (std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator("_DEATH")) == actions.end())
		{
			_exhandler->raise(Exception::FMTexc::FMTundefineddeathaction,
				"_DEATH","FMTmodel::setdefaultobjects", __LINE__, __FILE__, Core::FMTsection::Action);
			actions.push_back(defaultdeathaction(lifespan, themes));
			actions.back().passinobject(*this);
		}
		if (std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator("_DEATH")) == transitions.end())
		{
			_exhandler->raise(Exception::FMTexc::FMTundefineddeathtransition,
				"_DEATH","FMTmodel::setdefaultobjects", __LINE__, __FILE__, Core::FMTsection::Transition);
			transitions.push_back(defaultdeathtransition(lifespan, themes));
			transitions.back().passinobject(*this);
		}
		for (Core::FMTaction& action : actions)
		{
			action.update();
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::setdefaultobjects", __LINE__, __FILE__);
		}
	
	}


FMTmodel::FMTmodel() : Core::FMTobject(),area(),themes(),actions(), transitions(),yields(),lifespan(),outputs(), constraints(),name()
{

}

FMTmodel::FMTmodel(const std::vector<Core::FMTactualdevelopment>& larea, const std::vector<Core::FMTtheme>& lthemes,
	const std::vector<Core::FMTaction>& lactions,
	const std::vector<Core::FMTtransition>& ltransitions, const Core::FMTyields& lyields, const Core::FMTlifespans& llifespan,
	const std::string& lname, const std::vector<Core::FMToutput>& loutputs,std::vector<Core::FMTconstraint> lconstraints) :
	Core::FMTobject(), area(larea), themes(lthemes), actions(lactions), transitions(ltransitions),
	yields(lyields), lifespan(llifespan), outputs(loutputs), constraints(lconstraints), name(lname)
	{
	setdefaultobjects();
	}

FMTmodel::FMTmodel(const FMTmodel& rhs):Core::FMTobject(rhs),area(rhs.area),themes(rhs.themes),actions(rhs.actions),
		 transitions(rhs.transitions),yields(rhs.yields),lifespan(rhs.lifespan), outputs(rhs.outputs), constraints(rhs.constraints),name(rhs.name){}

FMTmodel& FMTmodel::operator = (const FMTmodel& rhs)
    {
    if (this!=&rhs)
        {
        Core::FMTobject::operator = (rhs);
        area = rhs.area;
        themes = rhs.themes;
        actions = rhs.actions;
        transitions = rhs.transitions;
        yields = rhs.yields;
        lifespan = rhs.lifespan;
		outputs = rhs.outputs;
		constraints = rhs.constraints;
        name = rhs.name;
        }
    return *this;
    }
std::vector<Core::FMTactualdevelopment>FMTmodel::getarea(int period) const
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
		sort(actions.begin(), actions.end());
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
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::cleanactionsntransitions", __LINE__, __FILE__);
		}
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
			specifier.addbounds(Core::FMTagebounds(Core::FMTsection::Action, intobject.second, intobject.second));
			death_action.push_back(amask, specifier);
		}
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



void FMTmodel::addoutput(const std::string& name,
	const std::string& maskstring, Core::FMTotar outputtarget,
	std::string action, std::string yield, std::string description, int targettheme)
	{
	try {
		std::vector<Core::FMToutputsource>sources;
		sources.push_back(Core::FMToutputsource(Core::FMTspec(), Core::FMTmask(maskstring, themes), outputtarget, yield, action));
		std::vector<Core::FMToperator>operators;
		outputs.push_back(Core::FMToutput(name, description, targettheme, sources, operators));
	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::addoutput", __LINE__, __FILE__);
	}

	}


void FMTmodel::setconstraints(const std::vector<Core::FMTconstraint>& lconstraint)
	{
	try {
		constraints = lconstraint;
		for (Core::FMTconstraint& constraint : constraints)
		{
			constraint.passinobject(*this);
		}
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
		area = ldevs;
		for (Core::FMTactualdevelopment& development : area)
		{
			development.passinobject(*this);
		}
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setarea", __LINE__, __FILE__);
	}


    }
void FMTmodel::setthemes(const std::vector<Core::FMTtheme>& lthemes)
    {
	try {
		themes = lthemes;
		for (Core::FMTtheme& theme : themes)
		{
			theme.passinobject(*this);
		}
		//After theme change every masks needs to be reevaluated?.
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setthemes", __LINE__, __FILE__);
	}


    }
void FMTmodel::setactions(const std::vector<Core::FMTaction>& lactions)
    {
	try {
		actions = lactions;
		for (Core::FMTaction& action : actions)
		{
			action.passinobject(*this);
			action.update();
		}
		this->setdefaultobjects();
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
			transition.passinobject(*this);
			transition.update();
		}
		this->setdefaultobjects();
	}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodel::settransitions", __LINE__, __FILE__);
		}

    }
void FMTmodel::setyields(const Core::FMTyields& lylds)
    {
	try {
		yields = lylds;
		yields.passinobject(*this);
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
		lifespan.passinobject(*this);
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
		for (Core::FMToutput& output : outputs)
		{
			output.passinobject(*this);
		}
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setoutputs", __LINE__, __FILE__);
	}

	}


std::vector<Core::FMTtheme> FMTmodel::locatestaticthemes() const
{
	std::vector<Core::FMTtheme>bestthemes = getthemes();
	try {
		for (const Core::FMTtransition& transition : gettransitions())
		{
			bestthemes = transition.getstaticthemes(bestthemes);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::locatestaticthemes", __LINE__, __FILE__);
		}
	return bestthemes;
}

std::vector<Core::FMTtheme> FMTmodel::locatedynamicthemes() const
{
	std::vector<Core::FMTtheme>dynamicthemes;
	try {
		const std::vector<Core::FMTtheme>staticthemes = locatestaticthemes();
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


Core::FMTmask FMTmodel::getdynamicmask(std::vector<Core::FMTtheme>optionalstatictokeep) const
	{
	Core::FMTmask selection;
	try {
		const std::vector<Core::FMTtheme>staticcthemes = locatestaticthemes();
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
			if (std::find_if(optionalstatictokeep.begin(),optionalstatictokeep.end(),Core::FMTthemecomparator(theme))==optionalstatictokeep.end())
				{
				const size_t start = static_cast<size_t>(theme.getstart());
				for (size_t bitid = start; bitid < (theme.size() + start); ++bitid)
					{
					bits[bitid] = false;
					}
				}
			}
		selection = Core::FMTmask(basename, bits);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getdynamicmask", __LINE__, __FILE__);
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
		this->setsection(Core::FMTsection::Landscape);
		for (const Core::FMTtheme& theme : themes)
		{
			if (theme.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTempty_theme,
					"for theme id: " + std::to_string(theme.getid()),
					"FMTmodel::isvalid", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
		}
		this->setsection(Core::FMTsection::Area);
		for (const Core::FMTactualdevelopment& developement : area)
		{
			std::string name = std::string(developement.mask);
			this->validate(themes, name);
		}
		this->setsection(Core::FMTsection::Yield);
		this->validatelistmasks(yields);

		this->setsection(Core::FMTsection::Lifespan);
		this->validatelistmasks(lifespan);

		this->setsection(Core::FMTsection::Action);
		for (const Core::FMTaction& action : actions)
		{
			this->validatelistmasks(action);
			for (const auto& specobject : action)
			{
				validatelistspec(specobject.second);
			}
		}
		this->setsection(Core::FMTsection::Transition);
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
		this->setsection(Core::FMTsection::Outputs);
		for (const Core::FMToutput& output : outputs)
		{
			//Need a validate output function
			for (const Core::FMToutputsource& source : output.getsources())
			{
				if (source.isvariable())
				{
					std::string name = std::string(source.getmask());
					validate(themes, name, "for output " + output.getname());
					const std::string actionname = source.getaction();
					if (!actionname.empty())//need to check the targeted action!
					{

					}
				}
				validatelistspec(source);
			}
		}
		this->setsection(Core::FMTsection::Empty);
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::isvalid", __LINE__, __FILE__);
	}

    return true;
    }

Core::FMTmask FMTmodel::getbasemask(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
	{
	optionaldevelopments.insert(optionaldevelopments.end(), area.begin(), area.end());
	Core::FMTmask basemask(optionaldevelopments.begin()->mask);
	try {
		for (const Core::FMTtransition& transition : transitions)
		{
			for (const auto& transitionobject : transition)
			{
				for (const Core::FMTtransitionmask& fork : transitionobject.second.getmasktrans())
				{
					const Core::FMTmask maskwithoutaggregates = fork.getmask().removeaggregates(themes);
					basemask = basemask.getunion(maskwithoutaggregates);
				}
			}
		}
		for (const Core::FMTactualdevelopment& developement : optionaldevelopments)
		{
			basemask = basemask.getunion(developement.mask);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::getbasemask", __LINE__, __FILE__);
		}
	return basemask;
	}

Core::FMTmask FMTmodel::getselectedmask(const std::vector<Core::FMTtheme>& originalthemes) const
	{
	
	size_t newmasksize = 0;
	for (const Core::FMTtheme& theme : originalthemes)
		{
		newmasksize += theme.size();
		}
	boost::dynamic_bitset<>selection(newmasksize, false);
	try {
		size_t bitselection = 0;
		size_t presolvedthemeid = 0;
		size_t themeid = 0;
		while (presolvedthemeid<themes.size()&&themeid< originalthemes.size())
			{
			const Core::FMTtheme& originaltheme = originalthemes.at(themeid);
			const std::map<std::string, std::string> prsolvedvalues = themes.at(presolvedthemeid).getvaluenames();
			size_t foundcount = 0;
			std::vector<bool>themebits(originaltheme.size(),false);
			size_t bitid = 0;
			for (const auto& themevalues : originaltheme.getvaluenames())
				{
				if (prsolvedvalues.find(themevalues.first) != prsolvedvalues.end())
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
				bitselection += themebits.size();
				}
			++themeid;
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::getselectedmask", __LINE__, __FILE__);
		}
	return Core::FMTmask(selection);
	}


std::unique_ptr<FMTmodel> FMTmodel::presolve(int presolvepass,std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
	{
	std::unique_ptr<FMTmodel>presolvedmodel;
	try {
		Core::FMTmask basemask = getbasemask(optionaldevelopments);
		//Base data
		std::vector<Core::FMTtheme>oldthemes(themes);
		std::vector<Core::FMTactualdevelopment>oldarea(area);
		std::vector<Core::FMTaction>oldactions(actions);
		std::vector<Core::FMTtransition>oldtransitions(transitions);
		Core::FMTyields oldyields(yields);
		Core::FMTlifespans oldlifespans(lifespan);
		std::vector<Core::FMToutput>oldoutputs(outputs);
		std::vector<Core::FMTconstraint>oldconstraints(constraints);
		Core::FMTmask oldselectedattributes;
		//Passiterator
		bool didonepass = false;
		while (presolvepass > 0)
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
			if (didonepass)
			{
				basemask = basemask.presolve(oldselectedattributes, oldthemes);
			}
			Core::FMTmask selectedattributes; //selected attribute keeps the binaries used by the new attribute selection.
			//Checkout to reduce the themes complexity
			int themeid = 0;
			int themestart = 0;
			size_t themedataremoved = 0;
			for (const Core::FMTtheme& theme : oldthemes)
			{
				const Core::FMTtheme presolvedtheme = theme.presolve(basemask, themeid, themestart, selectedattributes);
				if (!presolvedtheme.empty())
				{
					themedataremoved += (theme.size() - presolvedtheme.size());
					newthemes.push_back(presolvedtheme);
				}
			}
			if (!selectedattributes.empty())
			{
				for (const Core::FMTactualdevelopment& development : oldarea)
				{
					newarea.push_back(development.presolve(selectedattributes, newthemes));
				}
			}
			else {
				newarea = oldarea;
			}
			//reduce the number of actions and presolve the actions
			size_t actiondataremoved = 0;
			for (const Core::FMTaction& action : oldactions)
			{
				const Core::FMTmask testedmask = action.getunion(oldthemes);
				if (!basemask.isnotthemessubset(testedmask, oldthemes))
				{
					const Core::FMTaction presolvedaction = action.presolve(basemask, oldthemes, selectedattributes, newthemes);
					actiondataremoved += (action.size() - presolvedaction.size());
					newactions.push_back(presolvedaction);
				}
			}
			//reduce the number of transitions and presolve the transitions
			size_t transitiondataremoved = 0;
			for (const Core::FMTtransition& transition : oldtransitions)
			{
				if (std::find_if(newactions.begin(), newactions.end(), Core::FMTactioncomparator(transition.getname())) != newactions.end())
				{
					const Core::FMTtransition presolvedtransition = transition.presolve(basemask, oldthemes, selectedattributes, newthemes);
					transitiondataremoved += (transition.size() - presolvedtransition.size());
					newtransitions.push_back(presolvedtransition);
				}
			}
			//Presolve yields
			newyields = oldyields.presolve(basemask, oldthemes, selectedattributes, newthemes);
			//Presolve lifespan data
			newlifespans = oldlifespans.presolve(basemask, oldthemes, selectedattributes, newthemes);
			//Outputs and data
			size_t outputdataremoved = 0;
			for (const Core::FMToutput& output : oldoutputs)
			{
				const Core::FMToutput presolvedoutput = output.presolve(basemask, oldthemes, selectedattributes, newthemes, newactions, oldyields);
				outputdataremoved += (output.size() - presolvedoutput.size());
				if (!presolvedoutput.empty())
				{
					newoutputs.push_back(presolvedoutput);
				}
			}
			//Constraints and data
			size_t constraintdataremoved = 0;
			for (const Core::FMTconstraint& constraint : oldconstraints)
			{
				const Core::FMTconstraint presolvedconstraint = constraint.presolve(basemask, oldthemes, selectedattributes, newthemes, newactions, oldyields);
				constraintdataremoved += (constraint.size() - presolvedconstraint.size());
				if (!presolvedconstraint.outputempty())
				{
					newconstraints.push_back(presolvedconstraint);
				}

			}
			oldthemes = newthemes;
			oldarea = newarea;
			oldactions = newactions;
			oldtransitions = newtransitions;
			oldyields = newyields;
			oldlifespans = newlifespans;
			oldoutputs = newoutputs;
			oldconstraints = newconstraints;
			oldselectedattributes = selectedattributes;
			--presolvepass;
			didonepass = true;
		}
	presolvedmodel = std::unique_ptr<FMTmodel>(new FMTmodel(oldarea, oldthemes, oldactions, oldtransitions, oldyields, oldlifespans, name, oldoutputs, oldconstraints));
	presolvedmodel->passinobject(*this);
	presolvedmodel->cleanactionsntransitions();
	}catch (...)
		{
		_exhandler->raisefromcatch("for "+name,"FMTmodel::presolve", __LINE__, __FILE__);
		}
	return presolvedmodel;
	}

std::unique_ptr<FMTmodel>FMTmodel::postsolve(const FMTmodel& originalbasemodel) const
	{
	return std::unique_ptr<FMTmodel>(new FMTmodel(originalbasemodel));
	}

Core::FMTschedule FMTmodel::presolveschedule(const Core::FMTschedule& originalbaseschedule,
	const FMTmodel& originalbasemodel) const
	{
	Core::FMTschedule newschedule;
	try {
		const Core::FMTmask presolvedmask = this->getselectedmask(originalbasemodel.themes);
		newschedule = originalbaseschedule.presolve(presolvedmask, this->themes, this->actions);
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
		basedev.period = period;
		}
	}


void FMTmodel::passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger)
	{
	FMTobject::passinlogger(logger);
	this->passinobject(*this);
	}	

void FMTmodel::passinexceptionhandler(const std::shared_ptr<Exception::FMTexceptionhandler>& exhandler)
	{
	FMTobject::passinexceptionhandler(exhandler);
	this->passinobject(*this);
	}

void FMTmodel::passinobject(const Core::FMTobject& rhs)
	{
	try {
		for (Core::FMTactualdevelopment& dev : area)
		{
			dev.passinobject(rhs);
		}
		for (Core::FMTtheme& theme : themes)
		{
			theme.passinobject(rhs);
		}
		for (Core::FMTaction& action : actions)
		{
			action.passinobject(rhs);
		}
		for (Core::FMTtransition& transition : transitions)
		{
			transition.passinobject(rhs);
		}
		yields.passinobject(rhs);
		lifespan.passinobject(rhs);
		for (Core::FMToutput& output : outputs)
		{
			output.passinobject(rhs);
		}
		for (Core::FMTconstraint& constraint : constraints)
		{
			constraint.passinobject(rhs);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::passinobject", __LINE__, __FILE__);
		}
	}

void FMTmodel::setdefaultlogger()
{
	FMTobject::setdefaultlogger();
	this->passinobject(*this);
}

void FMTmodel::setquietlogger()
{
	FMTobject::setquietlogger();
	this->passinobject(*this);
}

void FMTmodel::setdebuglogger()
{
	FMTobject::setdebuglogger();
	this->passinobject(*this);
}

void FMTmodel::setdefaultexceptionhandler()
{
	FMTobject::setdefaultexceptionhandler();
	this->passinobject(*this);
}

void FMTmodel::setquietexceptionhandler()
{
	FMTobject::setquietexceptionhandler();
	this->passinobject(*this);
}

void FMTmodel::setdebugexceptionhandler()
{
	FMTobject::setdebugexceptionhandler();
	this->passinobject(*this);
}

void FMTmodel::setfreeexceptionhandler()
{
	FMTobject::setfreeexceptionhandler();
	this->passinobject(*this);
}

void FMTmodel::disablenestedexceptions()
{
	FMTobject::disablenestedexceptions();
	this->passinobject(*this);
}

void FMTmodel::enablenestedexceptions()
{
	FMTobject::enablenestedexceptions();
	this->passinobject(*this);
}




FMTmodelcomparator::FMTmodelcomparator(std::string name) :model_name(name) {}

bool FMTmodelcomparator::operator()(const FMTmodel& model) const
	{
	return(model_name == model.getname());
	}






}



BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTmodel)
