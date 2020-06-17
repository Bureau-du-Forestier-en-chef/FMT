/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTforest.h"

namespace Spatial
{
FMTforest::FMTforest():FMTlayer<Core::FMTdevelopment>(){}
FMTforest::FMTforest(const FMTlayer<Core::FMTdevelopment>& rhs):FMTlayer<Core::FMTdevelopment>(rhs){}
FMTforest::FMTforest(const FMTforest& rhs):FMTlayer<Core::FMTdevelopment>(rhs){}
FMTforest& FMTforest::operator = (const FMTforest& rhs)
    {
    if (this!=&rhs)
        {
        FMTlayer<Core::FMTdevelopment>::operator = (rhs);
        }
    return *this;
    }

FMTlayer<std::string>FMTforest::getlock() const
    {
    FMTlayer<std::string>newlayer(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
    for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
        {
        if (it->second.lock > 0 )
            {
            newlayer.mapping[it->first] = "_Lock " + std::to_string(it->second.lock);
            }
        }
    return newlayer;
    }

void FMTforest::setperiod(int period)
	{
	for (std::map<FMTcoordinate, Core::FMTdevelopment>::iterator it = mapping.begin(); it != mapping.end(); ++it)
		{
		it->second.period = period;
		}
	}


std::vector<Core::FMTactualdevelopment>FMTforest::getarea() const
            {
	std::vector<Core::FMTactualdevelopment>devs;
            for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                const Core::FMTactualdevelopment newadev(it->second,1*cellsize);
				std::vector<Core::FMTactualdevelopment>::iterator devit = std::find_if(devs.begin(),devs.end(), Core::FMTactualdevelopmentcomparator(&newadev));
                if (devit==devs.end())
                    {
                    devs.push_back(newadev);
                    }else{
                    devit->setarea(devit->getarea()+(1 * cellsize));
                    }
                }
            return devs;
            }
FMTforest FMTforest::grow() const
            {
            FMTforest newforest(*this);
            for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator devit = mapping.begin(); devit != mapping.end(); ++devit)
                {
                const Core::FMTdevelopment* dev = &devit->second;
                const Core::FMTdevelopment newdev = dev->grow();
                newforest.mapping[devit->first] = newdev;
                }
            return newforest;
            }

FMTforest FMTforest::operate(const std::vector<FMTsesevent<Core::FMTdevelopment>>& cuts,const FMTspatialaction& action,const Core::FMTtransition& Transition,
                     const Core::FMTyields& ylds,const std::vector<Core::FMTtheme>& themes, boost::unordered_map<Core::FMTdevelopment,Core::FMTdevelopment>& cached_transitions,
					Core::FMTschedule& schedule) const
            {
            FMTforest newforest(FMTlayer<Core::FMTdevelopment>(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize));
			//non spatial schedule
			///
			if (schedule.find(action) == schedule.end())
				{
				schedule[action] = std::map<Core::FMTdevelopment, std::vector<double>>();
				}
			///
            for(const FMTsesevent<Core::FMTdevelopment>& cut : cuts)
                {
                for(std::map<FMTcoordinate,const Core::FMTdevelopment*>::const_iterator devit  = cut.mapped_elements.begin(); devit!= cut.mapped_elements.end(); devit++)
                    {
					//keeping non spatial schedule
					///
					const Core::FMTdevelopment lockclear = devit->second->clearlock();
					if (schedule.at(action).find(lockclear) == schedule.at(action).end())
						{
						schedule[action][lockclear] = std::vector<double>(1, 0.0);
						}
					schedule[action][lockclear][0] += this->getcellsize();
					///
                    const Core::FMTdevelopment* dev = devit->second;
					if (cached_transitions.find(*dev) == cached_transitions.end())
						{
						const std::vector<Core::FMTdevelopmentpath>paths = dev->operate(action, Transition, ylds, themes);
						newforest.mapping[devit->first] = *paths[0].development;
						cached_transitions[*dev] = *paths[0].development;
					}else{
						newforest.mapping[devit->first] = cached_transitions[*dev];
						}
                    }
                }
            return newforest;
            }

std::vector<FMTlayer<std::string>> FMTforest::getthemes(const std::vector<Core::FMTtheme>& themes) const
            {
			std::vector<FMTlayer<std::string>> newlayers(themes.size(),FMTlayer<std::string>(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize));
            for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                for(size_t id = 0 ; id < newlayers.size();++id)
                    {
					const std::string value = it->second.mask.get(themes[id]);
                    newlayers[id].mapping[it->first] = value;
                    }
                }
            return newlayers;
            }
FMTlayer<int>FMTforest::getage() const
            {
            FMTlayer<int>newlayer(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
            for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator it = mapping.begin(); it != mapping.end(); ++it)
                {
                newlayer.mapping[it->first] = it->second.age;
                }
            return newlayer;
            }

std::map<Core::FMTaction,FMTforest> FMTforest::getschedule(const Core::FMTschedule& selection,
										boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>& cached_operability,
                                             const Core::FMTyields& yields,
                                             bool schedule_only) const
            {
			std::map<Core::FMTaction,FMTforest>scheduling;
			for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator itc = mapping.begin(); itc != mapping.end(); ++itc)
                {
                const Core::FMTdevelopment* cdev = &itc->second;
				if (cached_operability.find(*cdev) == cached_operability.end())
					{
					if (!schedule_only)
						{
						cached_operability[*cdev] = std::vector<bool>(selection.size(), false);
						}
					int action_id = 0;
					for(std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator it = selection.begin();it != selection.end();it++)
						{
						//const std::map<Core::FMTdevelopment, std::vector<double>>* devsptr = &it->second;
						if (selection.operated(it->first, itc->second)||
							(!schedule_only && cdev->operable(it->first,yields)))
							{
							if (scheduling.find(it->first)==scheduling.end())
								{
								scheduling[it->first] = FMTforest(FMTlayer<Core::FMTdevelopment>(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize));
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
						for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator it = selection.begin(); it != selection.end(); it++)
							{
							if (cached_operability[*cdev][action_id])
								{
								if(scheduling.find(it->first) == scheduling.end())
									{
									scheduling[it->first] = FMTforest(FMTlayer<Core::FMTdevelopment>(this->geotransform, this->maxx, this->maxy, this->SRS_WKT, this->cellsize));
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
    FMTlayer<Core::FMTdevelopment> forest(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
    if(copydata)
        {
        forest = *this;
        }
    return forest;
    }

FMTforest FMTforest::getallowable(const FMTspatialaction& targetaction,
                            const FMTdisturbancestack& disturbances) const
    {
    FMTlayer<Core::FMTdevelopment> cleanedforest(this->geotransform,this->maxx,this->maxy,this->SRS_WKT,this->cellsize);
    for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator itc = mapping.begin(); itc != mapping.end(); ++itc)
        {
        if (disturbances.allow(targetaction,itc->first))
            {
            cleanedforest.mapping[itc->first] = itc->second;
            }
        }
    return cleanedforest;
    }

std::vector<FMTsesevent<Core::FMTdevelopment>> FMTforest::buildharvest(const double& target,
                                       const FMTspatialaction& targetaction,
										std::default_random_engine& generator,
										const int& pass) const
    {
    double harvested_area = 0;
	std::vector<FMTsesevent<Core::FMTdevelopment>>cuts;
	std::vector<int>locations(mapping.size());
    unsigned int locid = 0;
    std::iota(locations.begin(),locations.end(),0);
    int tooclosecall = 0;
    int initdone = 0;
    int spreaddone = 0;
	bool check_adjacency = (std::find(targetaction.neighbors.begin(), targetaction.neighbors.end(), targetaction.getname()) != targetaction.neighbors.end());
    if(!mapping.empty())
        {
		std::shuffle(locations.begin(),locations.end(),generator);
		std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator randomit;
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
					if (check_adjacency)
						{
						for (const FMTsesevent<Core::FMTdevelopment>& cut : cuts)
							{
							if (cut.within(static_cast<unsigned int>(targetaction.adjacency), newcut))
								{
								tooclose = true;
								++tooclosecall;
								break;
								}
							}
						}
                    if(!tooclose)
                        {
                        cuts.push_back(newcut);
                        harvested_area+=(static_cast<double>(newcut.elements.size())*cellsize);
                        }
                    }
                }
            ++locid;
            }
        }
    return cuts;
    }

FMTforest FMTforest::presolve(const Core::FMTmask& selectedmask, const std::vector<Core::FMTtheme>&presolvedthemes) const
	{
	FMTforest newforest(*this);
	for (std::map<FMTcoordinate,Core::FMTdevelopment>::iterator coordit  = newforest.mapping.begin();
		coordit != newforest.mapping.end();++coordit)
		{
		coordit->second = Core::FMTactualdevelopment(coordit->second, 0.0).presolve(selectedmask, presolvedthemes);
		}
	return newforest;
	}

FMTforest FMTforest::postsolve(const Core::FMTmask& selectedmask, const std::vector<Core::FMTtheme>&originalbasethemes) const
	{
	FMTforest newforest(*this);
	for (std::map<FMTcoordinate, Core::FMTdevelopment>::iterator coordit = newforest.mapping.begin();
		coordit != newforest.mapping.end(); ++coordit)
		{
		coordit->second.mask = coordit->second.mask.postsolve(selectedmask, originalbasethemes);
		}
	return newforest;
	}

}
