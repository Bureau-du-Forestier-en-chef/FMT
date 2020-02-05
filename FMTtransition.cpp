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

#include "FMTtransition.h"

namespace Core{
FMTtransition::FMTtransition():name(){}
FMTtransition::FMTtransition(std::string& lname):FMTlist<FMTfork>(),name(lname)
    {

    }
FMTtransition::FMTtransition(const std::string& lname) : FMTlist<FMTfork>(), name(lname)
	{

	}

bool FMTtransition::isleaking() const
	{
	std::vector<FMTfork>::const_iterator fork_it = this->databegin();
	for (size_t id = 0; id < this->size(); ++id)
		{
		if (fork_it->sumprop() != 100)
			{
			return true;
			}
		}
	return false;
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
	std::vector<FMTfork>::iterator data_it = newtra.databegin();
	for (size_t id = 0 ; id < newtra.size(); ++id)
		{
		*data_it = data_it->single();
		++data_it;
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
	std::vector<FMTfork>::const_iterator data_it = this->databegin();
	std::vector<FMTmask>::const_iterator mask_it = this->maskbegin();
    for (size_t id = 0; id < this->size(); ++id)
        {
        line+="*SOURCE "+ std::string(*mask_it);
        line+= std::string(*data_it);
		++data_it;
		++mask_it;
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
    for(const FMTdevelopment& dev : devs)
        {
		if (dev.operable(action,ylds))
			{
			const std::vector<FMTdevelopmentpath>newpaths = dev.operate(action, *this, ylds, themes);
			for (const FMTdevelopmentpath& path : newpaths)
				{
				total_age += path.development->age;
				++age_count;
				}
			}
        }
	if (age_count==0)
		{
		return 0;
		}
    return (total_age/age_count);
    }

std::vector<FMTtheme>FMTtransition::getstaticthemes(const std::vector<FMTtheme>& themes) const
	{
	std::vector<FMTfork>::const_iterator fork_it = this->databegin();
	std::vector<FMTtheme>staticthemes = themes;
	while (fork_it!= this->dataend() && !staticthemes.empty())
		{
		for (const FMTtransitionmask& trnmask : fork_it->getmasktrans())
			{
			const FMTmask msk = trnmask.getmask();
			staticthemes = msk.getstaticthemes(staticthemes);
			}
		++fork_it;
		}
	return staticthemes;
	}

const FMTfork* FMTtransition::getfork(const FMTdevelopment& dev,
                               const FMTyields& ylds) const
    {
	const std::vector<const FMTfork*> forks = this->findsets(dev.mask);
    if (!forks.empty())
        {
        for(const FMTfork* fork : forks)
            {
			const std::map<std::string,double>yields = ylds.getylds(dev,*fork);
            if (fork->allow(dev.period,dev.age,dev.lock,yields))
                {
                return fork;
                }
            }
        }
    return nullptr;
    }

 FMTmask FMTtransition::main_target(const std::vector<FMTdevelopment>& devs,
                            const FMTyields& ylds) const
    {
	 std::map<FMTmask,unsigned int>hits;
    for(const FMTdevelopment& dev : devs)
        {
        const FMTfork* fork = this->getfork(dev,ylds);
        if (fork)
            {
            for (const FMTtransitionmask& target :  fork->getmasktrans())
                {
                const FMTmask msktarget = target.getmask();
                if (hits.find(msktarget)==hits.end())
                    {
                    hits[msktarget] = 1;
                    }else{
                    ++hits[msktarget];
                    }
                }
            }
        }
    FMTmask bestmask;
    unsigned int besthit = 0;
    for(std::map<FMTmask,unsigned int>::const_iterator it = hits.begin();it!=hits.end();it++)
        {
        if(it->second > besthit)
            {
            besthit = it->second;
            bestmask = it->first;
            }
        }
    return bestmask;
    }
 std::map<std::string, std::vector<FMTdevelopment>> FMTtransition::attribute_targets(const std::vector<FMTdevelopment>& devs,
	 const FMTyields& ylds, const std::vector<FMTtheme>& themes) const
	{
	 std::map<std::string, std::vector<FMTdevelopment>>results;
	 for (const FMTdevelopment& dev : devs)
		{
		 const FMTfork* fork = this->getfork(dev, ylds);
		 if (fork)
			{
			 std::string key = this->name;
			 for (const FMTtheme& theme : themes)
				{
				key+= ("-"+dev.mask.get(theme));
				}
			if (results.find(key)==results.end())
				{
				results[key] = std::vector<FMTdevelopment>();
				}
			results[key].push_back(dev);
			}
		}
	 return results;
	 }

 FMTtransitioncomparator::FMTtransitioncomparator(std::string name) :transition_name(name) {}
 bool FMTtransitioncomparator::operator()(const FMTtransition& transition) const
	{
	return (transition.name  == transition_name);
	}


}
