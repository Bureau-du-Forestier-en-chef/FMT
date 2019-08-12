#include "FMTtransition.h"

namespace Core{
FMTtransition::FMTtransition():name(){}
FMTtransition::FMTtransition(string& lname):FMTlist<FMTfork>(),name(lname)
    {

    }
FMTtransition::FMTtransition(const string& lname) : FMTlist<FMTfork>(), name(lname)
	{

	}
/*FMTtransition::FMTtransition(const string& lname,map<FMTmask,vector<FMTfork>>& mapping): FMTlist<FMTfork>(mapping),name(lname)
    {

    }*/

bool FMTtransition::isleaking() const
	{
	vector<FMTfork>::const_iterator fork_it = this->databegin();
	for (size_t id = 0; id < this->size(); ++id)
		{
		if (fork_it->sumprop() != 100)
		{
			//_exhandler->raise(FMTexc::WSleakingtransition, _section, transition.name + " of " + to_string(100 - fork_it->sumprop()) + string(*fork_it), __LINE__, __FILE__);
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
	vector<FMTfork>::iterator data_it = newtra.databegin();
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
FMTtransition::operator string() const
    {
    string line = "*CASE "+name+"\n";
	vector<FMTfork>::const_iterator data_it = this->databegin();
	vector<FMTmask>::const_iterator mask_it = this->maskbegin();
    for (size_t id = 0; id < this->size(); ++id)
        {
        line+="*SOURCE "+string(*mask_it);
        line+=string(*data_it);
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

unsigned int FMTtransition::age_after(const vector<FMTdevelopment>& devs,
                               const FMTaction& action,
                               const FMTyields& ylds,
                               const vector<FMTtheme>& themes) const
    {
    unsigned int total_age = 0;
    unsigned int age_count = 0;
    for(const FMTdevelopment& dev : devs)
        {
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " test OP "<<action.name<<" ON "<<string(dev) << "\n";
		if (dev.operable(action,ylds))
			{
			vector<FMTdevelopmentpath>newpaths = dev.operate(action, *this, ylds, themes);
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

vector<FMTtheme>FMTtransition::getstaticthemes(const vector<FMTtheme>& themes) const
	{
	vector<FMTfork>::const_iterator fork_it = this->databegin();
	vector<FMTtheme>staticthemes = themes;
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
    vector<const FMTfork*> forks = this->findsets(dev.mask);
	
	
    if (!forks.empty())
        {
        for(const FMTfork* fork : forks)
            {
            map<string,double>yields = ylds.getylds(dev,*fork);

            if (fork->allow(dev.period,dev.age,dev.lock,yields))
                {
                return fork;
                }
            }
        }
    return nullptr;
    }

 FMTmask FMTtransition::main_target(const vector<FMTdevelopment>& devs,
                            const FMTyields& ylds) const
    {
    map<FMTmask,unsigned int>hits;
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
    for(map<FMTmask,unsigned int>::const_iterator it = hits.begin();it!=hits.end();it++)
        {
        if(it->second > besthit)
            {
            besthit = it->second;
            bestmask = it->first;
            }
        }
    return bestmask;
    }
 map<string, vector<FMTdevelopment>> FMTtransition::attribute_targets(const vector<FMTdevelopment>& devs,
	 const FMTyields& ylds, const vector<FMTtheme>& themes) const
	{
	 map<string, vector<FMTdevelopment>>results;
	 for (const FMTdevelopment& dev : devs)
		{
		 const FMTfork* fork = this->getfork(dev, ylds);
		 if (fork)
			{
			 string key = this->name;
			 for (const FMTtheme& theme : themes)
				{
				key+= ("-"+dev.mask.get(theme));
				}
			if (results.find(key)==results.end())
				{
				results[key] = vector<FMTdevelopment>();
				}
			results[key].push_back(dev);
			}
		}
	 return results;
	 }

 FMTtransitioncomparator::FMTtransitioncomparator(string name) :transition_name(name) {}
 bool FMTtransitioncomparator::operator()(const FMTtransition& transition) const
	{
	return (transition.name  == transition_name);
	}


}
