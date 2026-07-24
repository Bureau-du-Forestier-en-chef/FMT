/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTransition.h"
#include "FMTYieldRequest.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTAction.h"
#include "FMTYields.h"
#include "FMTTheme.h"
#include "FMTDevelopmentPath.h"
#include "FMTExceptionHandler.h"
#include "FMTTransitionMask.h"

namespace Core{
FMTTransition::FMTTransition():name(){}

FMTTransition::FMTTransition(const std::string& lname) : FMTList<FMTFork>(), name(lname)
	{

	}

void FMTTransition::setName(const std::string& p_name)
	{
	name = p_name;
	}

bool FMTTransition::isLeaking() const
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

FMTTransition& FMTTransition::operator+=(const FMTTransition& OtherTransition)
{
	try {
		FMTList<FMTFork>::operator+=(OtherTransition);
	}catch (...)
	{
		_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTTransition::operator+=", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
return *this;
}


FMTTransition::FMTTransition(const FMTTransition& rhs) : FMTList<FMTFork>(rhs),name(rhs.name)
    {

    }
FMTTransition& FMTTransition::operator = (const FMTTransition& rhs)
    {
    if (this!=&rhs)
        {
        name = rhs.name;
        FMTList<FMTFork>::operator = (rhs);
        }
    return *this;
    }

FMTTransition FMTTransition::single() const
    {
    FMTTransition newtra(*this);
	for (auto& forkobj : newtra)
		{
		forkobj.second = forkobj.second.single();
		}
    return newtra;
    }

bool FMTTransition::operator < (const FMTTransition& rhs) const
    {
    return name < rhs.name;
    }
FMTTransition::operator std::string() const
    {
	std::string line = "*CASE "+name+"\n";
    for (const auto& forkobj:*this)
        {
        line+="*SOURCE "+ std::string(forkobj.first);
        line+= std::string(forkobj.second);
        }
    return line;
    }
bool FMTTransition::operator == (const FMTTransition& rhs) const
    {
    return(name == rhs.name &&
		FMTList<FMTFork>::operator==(rhs));
    }
bool FMTTransition::operator != (const FMTTransition& rhs) const
    {
    return !(*this==rhs);
    }

unsigned int FMTTransition::ageAfter(const std::vector<FMTDevelopment>& devs,
                               const FMTAction& action,
                               const FMTYields& ylds,
                               const std::vector<FMTTheme>& themes) const
    {
    unsigned int total_age = 0;
    unsigned int age_count = 0;
	try {
		for (const FMTDevelopment& dev : devs)
		{
			if (dev.operable(action, ylds))
			{
				const std::vector<FMTDevelopmentPath>newpaths = dev.operate(action, *this, ylds, themes);
				for (const FMTDevelopmentPath& path : newpaths)
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
		_exhandler->raiseFromCatch("for transition "+this->getName(),"FMTTransition::ageAfter", __LINE__, __FILE__, Core::FMTsection::Transition);
		}
    return (total_age/age_count);
    }

std::vector<const FMTTheme*>FMTTransition::getStaticThemes(const std::vector<const FMTTheme*>& themes) const
	{
	std::vector<const FMTTheme*>staticthemes = themes;
	try {
		std::vector<std::pair<FMTMask, FMTFork>>::const_iterator fork_it = this->begin();
		while (fork_it != this->end() && !staticthemes.empty())
		{
			for (const FMTTransitionMask& trnmask : fork_it->second.getMaskTrans())
			{
				const FMTMask msk = trnmask.getMask();
				staticthemes = msk.getStaticThemes(staticthemes);
			}
			++fork_it;
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTTransition::getStaticThemes", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
	return staticthemes;
	}

std::vector<Core::FMTMask> FMTTransition::canProduce(const Core::FMTMask& testmask,const std::vector<Core::FMTTheme>& themes) const
	{
		std::vector<Core::FMTMask> possiblesourcesfortransitions;
		try {
			for(const auto& forkobj : *this)
			{
				const Core::FMTMask unshrinkedsourcemask(std::string(forkobj.first), themes);
				for (const FMTTransitionMask& transmask : forkobj.second.getMaskTrans())
				{
					Core::FMTMask refinedmask = transmask.getMask().refine(unshrinkedsourcemask,themes);
					const Core::FMTMask intersect = refinedmask.getIntersect(testmask);
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
			_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTTransition::getallpossibletransitionsmasks", __LINE__, __FILE__, Core::FMTsection::Transition);
		}
		return possiblesourcesfortransitions;	

	}

const FMTFork* FMTTransition::getFork(const Core::FMTDevelopment& development,const FMTYields& ylds) const
    {
	try{
	const Core::FMTYieldRequest& request = development.getYieldRequest();
    for(const FMTTransition::const_iterator fork : this->findSets(development.getMask()))
       {
		if (fork->second.allowWithoutYield(development.getPeriod(), development.getAge(), development.getLock()))
			{
			const std::vector<FMTYldBounds>&bounds = fork->second.getYldBounds();
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
		_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTTransition::getFork", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
    return nullptr;
    }

FMTMask FMTTransition::mainTarget(const std::vector<FMTDevelopment>& devs,
	const FMTYields& ylds) const
{
	FMTMask bestmask;
	try{
		std::map<FMTMask, unsigned int>hits;
	for (const FMTDevelopment& dev : devs)
	{
		const FMTFork* fork = this->getFork(dev,ylds);
		if (fork)
		{
			for (const FMTTransitionMask& target : fork->getMaskTrans())
			{
				const FMTMask msktarget = target.getMask();
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
	for (std::map<FMTMask, unsigned int>::const_iterator it = hits.begin(); it != hits.end(); it++)
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
		_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTTransition::mainTarget", __LINE__, __FILE__, Core::FMTsection::Transition);
	}
    return bestmask;
    }
 std::map<std::string, std::vector<FMTDevelopment>> FMTTransition::attributeTargets(const std::vector<FMTDevelopment>& devs,
	 const FMTYields& ylds, const std::vector<FMTTheme>& themes) const
	{
	 std::map<std::string, std::vector<FMTDevelopment>>results;
	 try {
		 for (const FMTDevelopment& dev : devs)
		 {
			 const FMTFork* fork = this->getFork(dev,ylds);
			 if (fork)
			 {
				 std::string key = this->name;
				 for (const FMTTheme& theme : themes)
				 {
					 key += ("-" + dev.getMask().getAttribute(theme));
				 }
				 if (results.find(key) == results.end())
				 {
					 results[key] = std::vector<FMTDevelopment>();
				 }
				 results[key].push_back(dev);
			 }
		 }
	 }
	 catch (...)
	 {
		 _exhandler->raiseFromCatch("for transition " + this->getName(), "FMTTransition::attributeTargets", __LINE__, __FILE__, Core::FMTsection::Transition);
	 }
	 return results;
	 }

 FMTTransitionComparator::FMTTransitionComparator(std::string name) :transition_name(name) {}
 bool FMTTransitionComparator::operator()(const FMTTransition& transition) const
	{
	return (transition.getName()  == transition_name);
	}

 FMTTransition FMTTransition::presolve(const FMTMaskFilter& filter,
	 const std::vector<FMTTheme>& originalthemes,
	 std::vector<FMTTheme>& newthemes,bool compressdata) const
	{
	FMTTransition newtransition(*this);
	try {
		newtransition.presolveRef(filter, originalthemes, newthemes, compressdata);
	}catch (...)
		{
		_exhandler->raiseFromCatch("for transition " + this->getName(), "FMTTransition::presolve", __LINE__, __FILE__, Core::FMTsection::Transition);
		}
	return newtransition;
	}

 void FMTTransition::presolveRef(const FMTMaskFilter& p_filter,
	 const std::vector<FMTTheme>& p_originalThemes,
	 std::vector<FMTTheme>& p_newthemes, bool p_compressdata)
 {
	 try {
		_presolveList(p_filter, p_originalThemes, p_newthemes);
		update();
		 if (p_compressdata)
		 {
			 _compressMasks(p_newthemes);
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
		 _exhandler->raiseFromCatch("for transition " + this->getName(), "FMTTransition::presolveRef", __LINE__, __FILE__, Core::FMTsection::Transition);
	 }
 }

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTTransition)

