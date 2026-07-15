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

void FMTtransition::setName(const std::string& p_name)
	{
	name = p_name;
	}

bool FMTtransition::isLeaking() const
	{
	for (const auto& forkobj : *this)
		{
		if (forkobj.second.sumProp() != 100)
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
		_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTtransition::operator+=", __LINE__, __FILE__, Core::FMTsection::Transition);
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

unsigned int FMTtransition::ageAfter(const std::vector<FMTdevelopment>& devs,
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
					total_age += path.getDevelopment().getAge();
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
		_exhandler->raiseFromCatch("for transition "+this->getName(),"FMTtransition::ageAfter", __LINE__, __FILE__, Core::FMTsection::Transition);
		}
    return (total_age/age_count);
    }

std::vector<const FMTtheme*>FMTtransition::getStaticThemes(const std::vector<const FMTtheme*>& themes) const
	{
	std::vector<const FMTtheme*>staticthemes = themes;
	try {
		std::vector<std::pair<FMTmask, FMTfork>>::const_iterator fork_it = this->begin();
		while (fork_it != this->end() && !staticthemes.empty())
		{
			for (const FMTtransitionmask& trnmask : fork_it->second.getMaskTrans())
			{
				const FMTmask msk = trnmask.getMask();
				staticthemes = msk.getStaticThemes(staticthemes);
			}
			++fork_it;
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTtransition::getStaticThemes", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
	return staticthemes;
	}

std::vector<Core::FMTmask> FMTtransition::canProduce(const Core::FMTmask& testmask,const std::vector<Core::FMTtheme>& themes) const
	{
		std::vector<Core::FMTmask> possiblesourcesfortransitions;
		try {
			for(const auto& forkobj : *this)
			{
				const Core::FMTmask unshrinkedsourcemask(std::string(forkobj.first), themes);
				for (const FMTtransitionmask& transmask : forkobj.second.getMaskTrans())
				{
					Core::FMTmask refinedmask = transmask.getMask().refine(unshrinkedsourcemask,themes);
					const Core::FMTmask intersect = refinedmask.getIntersect(testmask);
					if(!testmask.isNotThemesSubset(intersect,themes))
					{
						possiblesourcesfortransitions.push_back(unshrinkedsourcemask.refine(testmask, themes));
						break;
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTtransition::getallpossibletransitionsmasks", __LINE__, __FILE__, Core::FMTsection::Transition);
		}
		return possiblesourcesfortransitions;	

	}

const FMTfork* FMTtransition::getFork(const Core::FMTdevelopment& development,const FMTyields& ylds) const
    {
	try{
	const Core::FMTyieldrequest& request = development.getYieldRequest();
    for(const FMTtransition::const_iterator fork : this->findSets(development.getMask()))
       {
		if (fork->second.allowWithoutYield(development.getPeriod(), development.getAge(), development.getLock()))
			{
			const std::vector<FMTyldbounds>&bounds = fork->second.getYldBounds();
			size_t bid = 0;
			bool usefork = true;
			for (const std::string& yldname : fork->second.getYlds())
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
				return &fork->second;
				}
			}
        }
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTtransition::getFork", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
    return nullptr;
    }

FMTmask FMTtransition::mainTarget(const std::vector<FMTdevelopment>& devs,
	const FMTyields& ylds) const
{
	FMTmask bestmask;
	try{
		std::map<FMTmask, unsigned int>hits;
	for (const FMTdevelopment& dev : devs)
	{
		const FMTfork* fork = this->getFork(dev,ylds);
		if (fork)
		{
			for (const FMTtransitionmask& target : fork->getMaskTrans())
			{
				const FMTmask msktarget = target.getMask();
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
		_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTtransition::mainTarget", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
    return bestmask;
    }
 std::map<std::string, std::vector<FMTdevelopment>> FMTtransition::attributeTargets(const std::vector<FMTdevelopment>& devs,
	 const FMTyields& ylds, const std::vector<FMTtheme>& themes) const
	{
	 std::map<std::string, std::vector<FMTdevelopment>>results;
	 try {
		 for (const FMTdevelopment& dev : devs)
		 {
			 const FMTfork* fork = this->getFork(dev,ylds);
			 if (fork)
			 {
				 std::string key = this->name;
				 for (const FMTtheme& theme : themes)
				 {
					 key += ("-" + dev.getMask().getAttribute(theme));
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
		 _exhandler->raiseFromCatch("for transition " + this->getName(), "FMTtransition::attributeTargets", __LINE__, __FILE__, Core::FMTsection::Transition);
	 }
	 return results;
	 }

 FMTtransitioncomparator::FMTtransitioncomparator(std::string name) :transition_name(name) {}
 bool FMTtransitioncomparator::operator()(const FMTtransition& transition) const
	{
	return (transition.getName()  == transition_name);
	}

 FMTtransition FMTtransition::presolve(const FMTmaskfilter& filter,
	 const std::vector<FMTtheme>& originalthemes,
	 std::vector<FMTtheme>& newthemes,bool compressdata) const
	{
	FMTtransition newtransition(*this);
	try {
		newtransition.presolveRef(filter, originalthemes, newthemes, compressdata);
	}catch (...)
		{
		_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTtransition::presolve", __LINE__, __FILE__, Core::FMTsection::Transition);
		}
	return newtransition;
	}

 void FMTtransition::presolveRef(const FMTmaskfilter& p_filter,
	 const std::vector<FMTtheme>& p_originalThemes,
	 std::vector<FMTtheme>& p_newthemes, bool p_compressdata)
 {
	 try {
		presolveList(p_filter, p_originalThemes, p_newthemes);
		update();
		 if (p_compressdata)
		 {
			 compressMasks(p_newthemes);
		 }
		 if (!p_filter.emptyFlipped())
		 {
			 for (auto& transitionobject : *this)
			 {
				 transitionobject.second.presolveRef(p_filter, p_newthemes);
			 }
		 }
	 }
	 catch (...)
	 {
		 _exhandler->raiseFromCatch("for transition " + this->getName(), "FMTtransition::presolveRef", __LINE__, __FILE__, Core::FMTsection::Transition);
	 }
 }

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTtransition)

