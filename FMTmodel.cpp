/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "FMTmodel.h"

namespace Models{


void FMTmodel::setdefaultobjects()
	{
	if (std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator("_DEATH")) == actions.end())
		{
		_exhandler->raise(Exception::FMTexc::WSundefineddeathaction, FMTwssect::Action,
			"_DEATH", __LINE__, __FILE__);
		actions.push_back(defaultdeathaction(lifespan,themes));
		}
	if (std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator("_DEATH")) == transitions.end())
		{
		_exhandler->raise(Exception::FMTexc::WSundefineddeathtransition, FMTwssect::Transition,
			"_DEATH", __LINE__, __FILE__);
		transitions.push_back(defaultdeathtransition(lifespan,themes));
		}
	for (Core::FMTaction& action : actions)
		{
		action.setbounds();
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

void FMTmodel::cleanactionsntransitions()
	{
	std::vector<Core::FMTaction>newactions;
	std::vector<Core::FMTtransition>newtransitions;
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
	sort(actions.begin(), actions.end());
	sort(transitions.begin(), transitions.end());
	}

std::vector<Core::FMTtheme>FMTmodel::getthemes() const
    {
    return themes;
    }
std::vector<Core::FMTaction>FMTmodel::getactions() const
    {
    return actions;
    }

std::vector<Core::FMTtransition>FMTmodel::gettransitions() const
    {
    return transitions;
    }
Core::FMTyields FMTmodel::getyields() const
    {
    return yields;
    }
Core::FMTlifespans FMTmodel::getlifespan() const
    {
    return lifespan;
    }
std::vector<Core::FMToutput> FMTmodel::getoutputs() const
	{
	return outputs;
	}
Core::FMTaction FMTmodel::defaultdeathaction(const Core::FMTlifespans& llifespan,
										const std::vector<Core::FMTtheme>& lthemes)
	{
	const std::string actionname = "_DEATH";
	const bool lock = false; 
	const bool reset = true;
	Core::FMTaction death_action(actionname, lock, reset);
	for (const auto& intobject: llifespan)
		{
		const std::string mask = intobject.first.getstr();
		const Core::FMTmask amask(mask,lthemes);
		Core::FMTspec specifier;
		specifier.addbounds(Core::FMTagebounds(FMTwssect::Action, intobject.second, intobject.second));
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
		const std::string mask = lfobject.first.getstr();
		const Core::FMTmask amask(mask, lthemes);
		Core::FMTfork fork;
		Core::FMTtransitionmask trmask(mask, lthemes, target_proportion);
		fork.add(trmask);
		death_Transition.push_back(amask, fork);
		}
	death_Transition.shrink();
	return death_Transition;
	}

std::vector<Core::FMTconstraint>FMTmodel::getconstraints() const
	{
	return constraints;
	}

void FMTmodel::addoutput(const std::string& name,
	const std::string& maskstring, FMTotar outputtarget,
	std::string action, std::string yield, std::string description, int targettheme)
	{
	std::vector<Core::FMToutputsource>sources;
	sources.push_back(Core::FMToutputsource(Core::FMTspec(), Core::FMTmask(maskstring, themes), outputtarget, yield, action));
	std::vector<Core::FMToperator>operators;
	outputs.push_back(Core::FMToutput(name, description,targettheme, sources, operators));
	}


void FMTmodel::setconstraints(const std::vector<Core::FMTconstraint>& lconstraint)
	{
	constraints = lconstraint;
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
void FMTmodel::setarea(const std::vector<Core::FMTactualdevelopment>& ldevs)
    {
    area = ldevs;
    }
void FMTmodel::setthemes(const std::vector<Core::FMTtheme>& lthemes)
    {
    themes = lthemes;
    }
void FMTmodel::setactions(const std::vector<Core::FMTaction>& lactions)
    {
    actions = lactions;
    }
void FMTmodel::settransitions(const std::vector<Core::FMTtransition>& ltransitions)
    {
    transitions = ltransitions;
    }
void FMTmodel::setyields(const Core::FMTyields& lylds)
    {
    yields = lylds;
    }
void FMTmodel::setlifespan(const Core::FMTlifespans& llifespan)
    {
    lifespan = llifespan;
    }

std::vector<Core::FMTtheme> FMTmodel::locatestaticthemes() const
{
	std::vector<Core::FMTtheme>bestthemes = getthemes();
	for (const Core::FMTtransition& transition : gettransitions())
	{
		bestthemes = transition.getstaticthemes(bestthemes);
	}
	return bestthemes;
}

bool FMTmodel::isvalid()
    {
	for (const Core::FMTtheme& theme :themes)
		{
		if (theme.empty())
			{
			_exhandler->raise(Exception::FMTexc::WSempty_theme,FMTwssect::Landscape,
				"for theme id: " + std::to_string(theme.getid()), __LINE__, __FILE__);
			}
		}
	for (const Core::FMTactualdevelopment& developement : area)
		{
		for (const Core::FMTtheme& theme : themes)
			{
			if (developement.mask.get(theme).empty())
				{
				_exhandler->raise(Exception::FMTexc::WSundefined_attribute,FMTwssect::Area,
					" for developement " + std::string(developement), __LINE__, __FILE__);
				}
			}
		}

    if (actions.size() != transitions.size())
        {
        _exhandler->raise(Exception::FMTexc::FMTinvalidAandT,FMTwssect::Empty,"Model: "+name,__LINE__,__FILE__);
        }
    for(size_t id = 0 ; id < actions.size();++id)
        {
        if(actions[id].getname() != transitions[id].getname())
            {
            _exhandler->raise(Exception::FMTexc::WSinvalid_action,FMTwssect::Empty,"Model: "+name+" "+actions[id].getname(), __LINE__, __FILE__);
            }
        }
    return true;
    }

FMTmodelcomparator::FMTmodelcomparator(std::string name) :model_name(name) {}

bool FMTmodelcomparator::operator()(const FMTmodel& model) const
	{
	return(model_name == model.name);
	}

}
