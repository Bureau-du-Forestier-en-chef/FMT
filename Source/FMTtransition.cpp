/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtransition.h"
#include "FMTyieldrequest.h"
#include "FMTdevelopment.h"
#include "FMTmask.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTtheme.h"
#include "FMTdevelopmentpath.h"
#include "FMTexceptionhandler.h"
#include "FMTtransitionmask.h"

namespace Core{
FMTtransition::FMTtransition():name(){}

FMTtransition::FMTtransition(const std::string& lname) : FMTlist<FMTfork>(), name(lname)
	{

	}

bool FMTtransition::isleaking() const
	{
	for (const auto& forkobj : *this)
		{
		if (forkobj.second.sumprop() != 100)
			{
			return true;
			}

		}
	return false;
	}

FMTtransition& FMTtransition::operator+=(const FMTtransition& OtherTransition)
{
	try {
		FMTlist<FMTfork>::operator+=(OtherTransition);
	}catch (...)
	{
		_exhandler->raisefromcatch("for transition " + this->getname(), "FMTtransition::operator+=", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
return *this;
}


FMTtransition::FMTtransition(const FMTtransition& rhs) : FMTlist<FMTfork>(rhs),name(rhs.name)
    {

    }
FMTtransition& FMTtransition::operator = (const FMTtransition& rhs)
    {
    if (this!=&rhs)
        {
        name = rhs.name;
        FMTlist<FMTfork>::operator = (rhs);
        }
    return *this;
    }

FMTtransition FMTtransition::single() const
    {
    FMTtransition newtra(*this);
	for (auto& forkobj : newtra)
		{
		forkobj.second = forkobj.second.single();
		}
    return newtra;
    }

bool FMTtransition::operator < (const FMTtransition& rhs) const
    {
    return name < rhs.name;
    }
FMTtransition::operator std::string() const
    {
	std::string line = "*CASE "+name+"\n";
    for (const auto& forkobj:*this)
        {
        line+="*SOURCE "+ std::string(forkobj.first);
        line+= std::string(forkobj.second);
        }
    return line;
    }
bool FMTtransition::operator == (const FMTtransition& rhs) const
    {
    return(name == rhs.name &&
		FMTlist<FMTfork>::operator==(rhs));
    }
bool FMTtransition::operator != (const FMTtransition& rhs) const
    {
    return !(*this==rhs);
    }

unsigned int FMTtransition::age_after(const std::vector<FMTdevelopment>& devs,
                               const FMTaction& action,
                               const FMTyields& ylds,
                               const std::vector<FMTtheme>& themes) const
    {
    unsigned int total_age = 0;
    unsigned int age_count = 0;
	try {
		for (const FMTdevelopment& dev : devs)
		{
			if (dev.operable(action, ylds))
			{
				const std::vector<FMTdevelopmentpath>newpaths = dev.operate(action, *this, ylds, themes);
				for (const FMTdevelopmentpath& path : newpaths)
				{
					total_age += path.development->getage();
					++age_count;
				}
			}
		}
		if (age_count == 0)
		{
			return 0;
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("for transition "+this->getname(),"FMTtransition::age_after", __LINE__, __FILE__, Core::FMTsection::Transition);
		}
    return (total_age/age_count);
    }

std::vector<const FMTtheme*>FMTtransition::getstaticthemes(const std::vector<const FMTtheme*>& themes) const
	{
	std::vector<const FMTtheme*>staticthemes = themes;
	try {
		std::vector<std::pair<FMTmask, FMTfork>>::const_iterator fork_it = this->begin();
		while (fork_it != this->end() && !staticthemes.empty())
		{
			for (const FMTtransitionmask& trnmask : fork_it->second.getmasktrans())
			{
				const FMTmask msk = trnmask.getmask();
				staticthemes = msk.getstaticthemes(staticthemes);
			}
			++fork_it;
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for transition " + this->getname(), "FMTtransition::getstaticthemes", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
	return staticthemes;
	}

std::vector<Core::FMTmask> FMTtransition::canproduce(const Core::FMTmask& testmask,const std::vector<Core::FMTtheme>& themes) const
	{
		std::vector<Core::FMTmask> possiblesourcesfortransitions;
		try {
			for(const auto& forkobj : *this)
			{
				const Core::FMTmask unshrinkedsourcemask(std::string(forkobj.first), themes);
				for (const FMTtransitionmask& transmask : forkobj.second.getmasktrans())
				{
					Core::FMTmask refinedmask = transmask.getmask().refine(unshrinkedsourcemask,themes);
					const Core::FMTmask intersect = refinedmask.getintersect(testmask);
					if(!testmask.isnotthemessubset(intersect,themes))
					{
						possiblesourcesfortransitions.push_back(unshrinkedsourcemask.refine(testmask, themes));
						break;
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("for transition " + this->getname(), "FMTtransition::getallpossibletransitionsmasks", __LINE__, __FILE__, Core::FMTsection::Transition);
		}
		return possiblesourcesfortransitions;	

	}

const FMTfork* FMTtransition::getfork(const Core::FMTdevelopment& developement,const FMTyields& ylds) const
    {
	try{
	const Core::FMTyieldrequest& request = developement.getyieldrequest();
    for(const FMTfork* fork : this->findsets(developement.getmask()))
       {
		if (fork->allowwithoutyield(developement.getperiod(), developement.getage(), developement.getlock()))
			{
			const std::vector<FMTyldbounds>&bounds = fork->getyldbounds();
			size_t bid = 0;
			bool usefork = true;
			for (const std::string& yldname : fork->getylds())
			{
				if (bounds.at(bid).out(ylds.get(request,yldname)))
					{
					usefork = false;
					break;
					}
				++bid;
			}
			if (usefork)
				{
				return fork;
				}
			}
        }
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for transition " + this->getname(), "FMTtransition::getfork", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
    return nullptr;
    }

FMTmask FMTtransition::main_target(const std::vector<FMTdevelopment>& devs,
	const FMTyields& ylds) const
{
	FMTmask bestmask;
	try{
		std::map<FMTmask, unsigned int>hits;
	for (const FMTdevelopment& dev : devs)
	{
		const FMTfork* fork = this->getfork(dev,ylds);
		if (fork)
		{
			for (const FMTtransitionmask& target : fork->getmasktrans())
			{
				const FMTmask msktarget = target.getmask();
				if (hits.find(msktarget) == hits.end())
				{
					hits[msktarget] = 1;
				}
				else {
					++hits[msktarget];
				}
			}
		}
	}

	unsigned int besthit = 0;
	for (std::map<FMTmask, unsigned int>::const_iterator it = hits.begin(); it != hits.end(); it++)
	{
		if (it->second > besthit)
		{
			besthit = it->second;
			bestmask = it->first;
		}
	}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for transition " + this->getname(), "FMTtransition::main_target", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
    return bestmask;
    }
 std::map<std::string, std::vector<FMTdevelopment>> FMTtransition::attribute_targets(const std::vector<FMTdevelopment>& devs,
	 const FMTyields& ylds, const std::vector<FMTtheme>& themes) const
	{
	 std::map<std::string, std::vector<FMTdevelopment>>results;
	 try {
		 for (const FMTdevelopment& dev : devs)
		 {
			 const FMTfork* fork = this->getfork(dev,ylds);
			 if (fork)
			 {
				 std::string key = this->name;
				 for (const FMTtheme& theme : themes)
				 {
					 key += ("-" + dev.getmask().get(theme));
				 }
				 if (results.find(key) == results.end())
				 {
					 results[key] = std::vector<FMTdevelopment>();
				 }
				 results[key].push_back(dev);
			 }
		 }
	 }
	 catch (...)
	 {
		 _exhandler->raisefromcatch("for transition " + this->getname(), "FMTtransition::attribute_targets", __LINE__, __FILE__, Core::FMTsection::Transition);
	 }
	 return results;
	 }

 FMTtransitioncomparator::FMTtransitioncomparator(std::string name) :transition_name(name) {}
 bool FMTtransitioncomparator::operator()(const FMTtransition& transition) const
	{
	return (transition.getname()  == transition_name);
	}

 FMTtransition FMTtransition::presolve(const FMTmaskfilter& filter,
	 const std::vector<FMTtheme>& originalthemes,
	 std::vector<FMTtheme>& newthemes,bool compressdata) const
	{
	FMTtransition newtransition(*this);
	try {
	newtransition.presolvelist(filter, originalthemes, newthemes);
	newtransition.update();
	if (compressdata)
		{
		newtransition.compressmasks(newthemes);
		}
	if (!filter.emptyflipped())
		{
		for (auto& transitionobject : newtransition)
			{
			transitionobject.second = transitionobject.second.presolve(filter, newthemes);
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("for transition "+this->getname(),"FMTtransition::presolve", __LINE__, __FILE__, Core::FMTsection::Transition);
		}	
	return newtransition;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTtransition)

