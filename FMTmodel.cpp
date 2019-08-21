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
	if (find_if(actions.begin(), actions.end(), FMTactioncomparator("_DEATH")) == actions.end())
		{
		_exhandler->raise(FMTexc::WSundefineddeathaction, FMTwssect::Action,
			"_DEATH", __LINE__, __FILE__);
		actions.push_back(defaultdeathaction());
		}
	if (find_if(transitions.begin(), transitions.end(), FMTtransitioncomparator("_DEATH")) == transitions.end())
		{
		_exhandler->raise(FMTexc::WSundefineddeathtransition, FMTwssect::Transition,
			"_DEATH", __LINE__, __FILE__);
		transitions.push_back(defaultdeathtransition());
		}
	for (FMTaction& action : actions)
		{
		action.setbounds();
		}
	}

FMTmodel::FMTmodel() : FMTobject(),area(),themes(),actions(), action_aggregates(),transitions(),yields(),lifespan(),outputs(), constraints(),name()
{

}

FMTmodel::FMTmodel(const vector<FMTactualdevelopment>& larea,const vector<FMTtheme>& lthemes,const vector<FMTaction>& lactions, const map<string, vector<string>>& laction_aggregates,
        const vector<FMTtransition>& ltransitions,const FMTyields& lyields,const FMTlifespans& llifespan,const string& lname, const vector<FMToutput>& loutputs) :FMTobject(),area(larea),themes(lthemes),actions(lactions),
		action_aggregates(laction_aggregates),transitions(ltransitions),yields(lyields),lifespan(llifespan),outputs(loutputs), constraints(),name(lname)
		{
		setdefaultobjects();
		}

FMTmodel::FMTmodel(const vector<FMTactualdevelopment>& larea, const vector<FMTtheme>& lthemes,
	const vector<FMTaction>& lactions, const map<string, vector<string>>& laction_aggregates,
	const vector<FMTtransition>& ltransitions, const FMTyields& lyields, const FMTlifespans& llifespan,
	const string& lname, const vector<FMToutput>& loutputs, const vector<FMTconstraint>& lconstraints) :
	FMTobject(), area(larea), themes(lthemes), actions(lactions),
	action_aggregates(laction_aggregates), transitions(ltransitions),
	yields(lyields), lifespan(llifespan), outputs(loutputs), constraints(lconstraints), name(lname)
	{
	setdefaultobjects();
	}

FMTmodel::FMTmodel(const FMTmodel& rhs):FMTobject(rhs),area(rhs.area),themes(rhs.themes),actions(rhs.actions),
		action_aggregates(rhs.action_aggregates), transitions(rhs.transitions),yields(rhs.yields),lifespan(rhs.lifespan), outputs(rhs.outputs), constraints(rhs.constraints),name(rhs.name){}

FMTmodel& FMTmodel::operator = (const FMTmodel& rhs)
    {
    if (this!=&rhs)
        {
        FMTobject::operator = (rhs);
        area = rhs.area;
        themes = rhs.themes;
        actions = rhs.actions;
        transitions = rhs.transitions;
		action_aggregates = rhs.action_aggregates;
        yields = rhs.yields;
        lifespan = rhs.lifespan;
		outputs = rhs.outputs;
		constraints = rhs.constraints;
        name = rhs.name;
        }
    return *this;
    }
vector<FMTactualdevelopment>FMTmodel::getarea(int period) const
    {
    return area;
    }

void FMTmodel::cleanactionsntransitions()
	{
	vector<FMTaction>newactions;
	vector<FMTtransition>newtransitions;
	for (size_t id = 0; id < actions.size(); ++id)
	{
		if (!actions[id].empty())
		{
			vector<FMTtransition>::iterator trn_it = find_if(transitions.begin(), transitions.end(), FMTtransitioncomparator(actions[id].name));
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

vector<FMTtheme>FMTmodel::getthemes() const
    {
    return themes;
    }
vector<FMTaction>FMTmodel::getactions() const
    {
    return actions;
    }
map<string, vector<string>> FMTmodel::getactionaggregates() const
	{
	return action_aggregates;
	}
vector<FMTtransition>FMTmodel::gettransitions() const
    {
    return transitions;
    }
FMTyields FMTmodel::getyields() const
    {
    return yields;
    }
FMTlifespans FMTmodel::getlifespan() const
    {
    return lifespan;
    }
vector<FMToutput> FMTmodel::getoutputs() const
	{
	return outputs;
	}
FMTaction FMTmodel::defaultdeathaction() const
	{
	string actionname = "_DEATH";
	bool lock = true; 
	bool reset = true;
	FMTaction death_action(actionname, lock, reset);
	vector<FMTmask>::const_iterator mask_iterator = lifespan.maskbegin();
	vector<int>::const_iterator data_iterator = lifespan.databegin();
	for (size_t id = 0; id < lifespan.size(); ++id)
		{
		string mask = mask_iterator->getstr();
		FMTmask amask(mask,themes);
		FMTspec specifier;
		specifier.addbounds(FMTagebounds(FMTwssect::Action, *data_iterator, *data_iterator));
		death_action.push_back(amask, specifier);
		++mask_iterator;
		++data_iterator;
		}
	death_action.shrink();
	return death_action;
	}
FMTtransition FMTmodel::defaultdeathtransition() const
	{
	string transitionname = "_DEATH";
	FMTtransition death_Transition(transitionname);
	double target_proportion = 100;
	vector<FMTmask>::const_iterator mask_iterator = lifespan.maskbegin();
	vector<int>::const_iterator data_iterator = lifespan.databegin();
	for (size_t id = 0; id < lifespan.size(); ++id)
		{
		string mask = mask_iterator->getstr();
		FMTmask amask(mask, themes);
		FMTfork fork;
		FMTtransitionmask trmask(mask, themes, target_proportion);
		fork.add(trmask);
		death_Transition.push_back(amask, fork);
		}
	death_Transition.shrink();
	return death_Transition;
	}

vector<FMTconstraint>FMTmodel::getconstraints() const
	{
	return constraints;
	}

bool FMTmodel::addoutput(const string& name,
	const string& maskstring, FMTotar outputtarget,
	string action, string yield, string description, int targettheme)
	{
	vector<FMToutputsource>sources;
	sources.push_back(FMToutputsource(FMTspec(), FMTmask(maskstring, themes), outputtarget, yield, action));
	vector<FMToperator>operators;
	outputs.push_back(FMToutput(name, description,targettheme, sources, operators));
	return true;
	}


void FMTmodel::setconstraints(const vector<FMTconstraint>& lconstraint)
	{
	constraints = lconstraint;
	}

bool  FMTmodel::operator == (const FMTmodel& rhs) const
	{
	return (name == rhs.name &&
		area == rhs.area &&
		themes == rhs.themes &&
		actions == rhs.actions &&
		action_aggregates == rhs.action_aggregates &&
		transitions == rhs.transitions &&
		yields == rhs.yields &&
		lifespan == rhs.lifespan &&
		outputs == rhs.outputs &&
		constraints == rhs.constraints);
	}
bool FMTmodel::setarea(const vector<FMTactualdevelopment>& ldevs)
    {
    area = ldevs;
	return true;
    }
bool FMTmodel::setthemes(const vector<FMTtheme>& lthemes)
    {
    themes = lthemes;
	return true;
    }
bool FMTmodel::setactions(const vector<FMTaction>& lactions)
    {
    actions = lactions;
	return true;
    }
bool FMTmodel::settransitions(const vector<FMTtransition>& ltransitions)
    {
    transitions = ltransitions;
    return true;
    }
bool FMTmodel::setyields(const FMTyields& lylds)
    {
    yields = lylds;
	return true;
    }
bool FMTmodel::setlifespan(const FMTlifespans& llifespan)
    {
    lifespan = llifespan;
	return true;
    }

bool FMTmodel::valid()
    {
    if (actions.size() != transitions.size())
        {
        _exhandler->raise(FMTexc::FMTinvalidAandT,FMTwssect::Empty,"Model: "+name,__LINE__,__FILE__);
        }
    for(size_t id = 0 ; id < actions.size();++id)
        {
        if(actions[id].name != transitions[id].name)
            {
            _exhandler->raise(FMTexc::WSinvalid_action,FMTwssect::Empty,"Model: "+name+" "+actions[id].name, __LINE__, __FILE__);
            }
        }
    return true;
    }

FMTmodelcomparator::FMTmodelcomparator(string name) :model_name(name) {}

bool FMTmodelcomparator::operator()(const FMTmodel& model) const
	{
	return(model_name == model.name);
	}

}
