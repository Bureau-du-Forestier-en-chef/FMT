#include "FMTforest.h"

namespace Spatial
{
FMTforest::FMTforest():FMTlayer<FMTdevelopment>(){}
FMTforest::FMTforest(const FMTlayer<FMTdevelopment>& rhs):FMTlayer<FMTdevelopment>(rhs){}
FMTforest::FMTforest(const FMTforest& rhs):FMTlayer<FMTdevelopment>(rhs){}
FMTforest& FMTforest::operator = (const FMTforest& rhs)
    {
    if (this!=&rhs)
        {
        FMTlayer<FMTdevelopment>::operator = (rhs);
        }
    return *this;
    }

FMTlayer<string>FMTforest::getlock() const
    {
    FMTlayer<string>newlayer(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
    for(map<FMTcoordinate,FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
        {
        if (it->second.lock > 0 )
            {
            newlayer.mapping[it->first] = "_Lock " + to_string(it->second.lock);
            }
        }
    return newlayer;
    }

void FMTforest::setperiod(int period)
	{
	for (map<FMTcoordinate, FMTdevelopment>::iterator it = mapping.begin(); it != mapping.end(); ++it)
		{
		it->second.period = period;
		}
	}

vector<FMTactualdevelopment>FMTforest::getarea() const
            {
            vector<FMTactualdevelopment>devs;
            for(map<FMTcoordinate,FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                FMTactualdevelopment newadev(it->second,1*cellsize);
                vector<FMTactualdevelopment>::iterator devit = std::find(devs.begin(),devs.end(),newadev);
                if (devit==devs.end())
                    {
                    devs.push_back(newadev);
                    }else{
                    devit->area+=(1*cellsize);
                    }
                }
            return devs;
            }
FMTforest FMTforest::grow() const
            {
            FMTforest newforest(*this);
            for(map<FMTcoordinate,FMTdevelopment>::const_iterator devit = mapping.begin(); devit != mapping.end(); ++devit)
                {
                const FMTdevelopment* dev = &devit->second;
                FMTdevelopment newdev = dev->grow();
                newforest.mapping[devit->first] = newdev;
                }
            return newforest;
            }

FMTforest FMTforest::operate(const vector<FMTevent<FMTdevelopment>>& cuts,const FMTspatialaction& action,const FMTtransition& Transition,
                     const FMTyields& ylds,const vector<FMTtheme>& themes, boost::unordered_map<FMTdevelopment, FMTdevelopment>& cached_transitions) const
            {
            FMTforest newforest(FMTlayer<FMTdevelopment>(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize));
            for(const FMTevent<FMTdevelopment>& cut : cuts)
                {
                for(map<FMTcoordinate,const FMTdevelopment*>::const_iterator devit  = cut.elements.begin(); devit!= cut.elements.end(); devit++)
                    {
                    const FMTdevelopment* dev = devit->second;
					if (cached_transitions.find(*dev) == cached_transitions.end())
						{
						vector<FMTdevelopmentpath>paths = dev->operate(action, Transition, ylds, themes);
						newforest.mapping[devit->first] = *paths[0].development;
						cached_transitions[*dev] = *paths[0].development;
					}else{
						newforest.mapping[devit->first] = cached_transitions[*dev];
						}
                    }
                }
            return newforest;
            }

vector<FMTlayer<string>> FMTforest::getthemes(const vector<FMTtheme>& themes) const
            {
            vector<FMTlayer<string>> newlayers(themes.size(),FMTlayer<string>(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize));
            for(map<FMTcoordinate,FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                for(size_t id = 0 ; id < newlayers.size();++id)
                    {
                    string value = it->second.mask.get(themes[id]);
                    newlayers[id].mapping[it->first] = value;
                    }
                }
            return newlayers;
            }
FMTlayer<int>FMTforest::getage() const
            {
            FMTlayer<int>newlayer(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
            for(map<FMTcoordinate,FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                newlayer.mapping[it->first] = it->second.age;
                }
            return newlayer;
            }

map<FMTaction,FMTforest> FMTforest::getschedule(const FMTschedule& selection,
										boost::unordered_map<FMTdevelopment, vector<bool>>& cached_operability,
                                             const FMTyields& yields,
                                             bool schedule_only) const
            {
            map<FMTaction,FMTforest>scheduling;
			//cached_operability.clear();
			//unordered_map<FMTdevelopment, map<FMTaction, bool>> cached_operability;
			//unordered_map<FMTdevelopment, vector<bool>>cached_operability;//could be usefull between call also (should be a mutable member)...
            for(map<FMTcoordinate,FMTdevelopment>::const_iterator itc = mapping.begin(); itc != mapping.end(); ++itc)
                {
                const FMTdevelopment* cdev = &itc->second;
				if (cached_operability.find(*cdev) == cached_operability.end())
					{
					if (!schedule_only)
						{
						cached_operability[*cdev] = vector<bool>(selection.elements.size(), false);
						}
					int action_id = 0;
					for(map<FMTaction,map<FMTdevelopment,vector<double>>>::const_iterator it = selection.elements.begin();it != selection.elements.end();it++)
						{
						const map<FMTdevelopment,vector<double>>* devsptr = &it->second;
						/*if ((devsptr->find(itc->second)!=devsptr->end()) ||
							(!schedule_only && cdev->operable(it->first,yields)))*/
						if (selection.operated(it->first, itc->second)||
							(!schedule_only && cdev->operable(it->first,yields)))
							{
							if (scheduling.find(it->first)==scheduling.end())
								{
								scheduling[it->first] = FMTforest(FMTlayer<FMTdevelopment>(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize));
								}
							scheduling[it->first].mapping[itc->first] = itc->second;
							if (!schedule_only)
								{
								cached_operability[*cdev][action_id] = true;
								}
							}
						++action_id;
						}
					}else{
						int action_id = 0;
						for (map<FMTaction, map<FMTdevelopment, vector<double>>>::const_iterator it = selection.elements.begin(); it != selection.elements.end(); it++)
							{
							if (cached_operability[*cdev][action_id])
								{
								if(scheduling.find(it->first) == scheduling.end())
									{
									scheduling[it->first] = FMTforest(FMTlayer<FMTdevelopment>(this->geotransform, this->maxx, this->maxy, this->SRS_WKT, this->cellsize));
									}
								scheduling[it->first].mapping[itc->first] = itc->second;
								}
							++action_id;
							}
					}
                }
            return scheduling;
            }

FMTforest FMTforest::getcopy(bool copydata) const
    {
    FMTlayer<FMTdevelopment> forest(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
    if(copydata)
        {
        forest = *this;
        }
    return forest;
    }

FMTforest FMTforest::getallowable(const FMTspatialaction& targetaction,
                            const FMTdisturbancestack& disturbances) const
    {
    FMTlayer<FMTdevelopment> cleanedforest(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
    for(map<FMTcoordinate,FMTdevelopment>::const_iterator itc = mapping.begin(); itc != mapping.end(); ++itc)
        {
        if (disturbances.allow(targetaction,itc->first))
            {
            cleanedforest.mapping[itc->first] = itc->second;
            }
        }
    return cleanedforest;
    }

vector<FMTevent<FMTdevelopment>> FMTforest::buildharvest(const double& target,
                                       const FMTspatialaction& targetaction,
                                       default_random_engine& generator,
										const int& pass) const
    {
    double harvested_area = 0;
    //default_random_engine generator;
    vector<FMTevent<FMTdevelopment>>cuts;
    vector<int>locations(mapping.size());
    unsigned int locid = 0;
    std::iota(locations.begin(),locations.end(),0);
    int tooclosecall = 0;
    int initdone = 0;
    int spreaddone = 0;
    if(!mapping.empty())
        {
        shuffle(locations.begin(),locations.end(),generator);
        map<FMTcoordinate,FMTdevelopment>::const_iterator randomit;
        while(harvested_area<target&&locid<locations.size())
            {
            randomit = mapping.begin();
            std::advance(randomit, locations[locid]);
            FMTcut newcut(this->mapping,pass);
            if (newcut.ignit(targetaction,randomit->first))
                {
                ++initdone;
                if (newcut.spread(targetaction))
                    {
                    ++spreaddone;
                    bool tooclose = false;
                    for (const FMTevent<FMTdevelopment>& cut : cuts)
                        {
                        if(cut.within(targetaction.adjacency,newcut))
                            {
                            tooclose = true;
                            ++tooclosecall;
                            break;
                            }
                        }
                    if(!tooclose)
                        {
                        cuts.push_back(newcut);
                        harvested_area+=((newcut.elements.size())*cellsize);
                        }
                    }
                }
            ++locid;
            }
        }
    return cuts;
    }
}
