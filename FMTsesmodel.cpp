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

#include "FMTsesmodel.h"

namespace Models
    {
    FMTsesmodel::FMTsesmodel(): FMTmodel(),mapping(), operatedschedule(),disturbances(),spactions()
        {

        }
    FMTsesmodel::FMTsesmodel(const FMTsesmodel& rhs):
        FMTmodel(rhs),
        mapping(rhs.mapping),
		operatedschedule(rhs.operatedschedule),
        disturbances(rhs.disturbances),
        spactions(rhs.spactions)
        {

        }
    FMTsesmodel::FMTsesmodel(const FMTmodel& rhs):
        FMTmodel(rhs),
        mapping(), operatedschedule(),disturbances(),spactions()
        {

        }
    FMTsesmodel& FMTsesmodel::operator = (const FMTsesmodel& rhs)
        {
        if (this!=&rhs)
            {
            FMTmodel::operator = (rhs);
            mapping = rhs.mapping;
			operatedschedule = rhs.operatedschedule;
            disturbances = rhs.disturbances;
            spactions = rhs.spactions;
            }
        return *this;
        }
    Spatial::FMTforest FMTsesmodel::getmapping() const
        {
        return mapping;
        }
    bool FMTsesmodel::setspactions(const std::vector<Spatial::FMTspatialaction>& lspactions)
        {
		std::vector<Core::FMTtransition>newtransitions;
		std::vector<Spatial::FMTspatialaction>newspatials;
		std::vector<Core::FMTaction>newbaseactions;
		for (const Spatial::FMTspatialaction& spaction : lspactions)
			{
			std::vector<Core::FMTtransition>::const_iterator trn_iterator = std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator(spaction.getname()));
			if (trn_iterator!= transitions.end())
				{
				newtransitions.push_back(*trn_iterator);
				newspatials.push_back(spaction);
				newbaseactions.push_back(spaction);
			}else{
				_exhandler->raise(Exception::FMTexc::WSinvalid_transition,
					FMTwssect::Transition, "Missing transition case for action : " + spaction.getname(),__LINE__, __FILE__);
				return false;
				}
			}
        spactions = lspactions;
		actions = newbaseactions;
		transitions = newtransitions;
		return true;
        }
    bool FMTsesmodel::setinitialmapping(const Spatial::FMTforest& forest)
        {
        disturbances = Spatial::FMTdisturbancestack();
        mapping = forest;
		mapping.setperiod(1);
		return true;
        }

	Spatial::FMTdisturbancestack FMTsesmodel::getdisturbances() const
        {
        return disturbances;
        }


    std::map<std::string,double> FMTsesmodel::simulate(const Core::FMTschedule& schedule,
                             bool schedule_only,
                             unsigned int seed)
        {
		Core::FMTschedule newschedule(static_cast<int>(disturbances.data.size()+1), std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>());
		std::default_random_engine generator(seed);
        const double total_area = schedule.area();
		std::map<std::string,double>targets;
        for(std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator ait = schedule.elements.begin(); ait!=schedule.elements.end(); ait++)
            {
            targets[ait->first.getname()] = schedule.actionarea(ait->first);
            }
        disturbances.push(std::map<std::string, std::vector<Spatial::FMTevent<Core::FMTdevelopment>>>());
		double allocated_area = 0;
		if (!schedule.elements.empty() || !schedule_only)
			{
			double pass_allocated_area = 0;
			bool schedulepass = true;
			int pass = 0;
			int action_pass = 0;
			boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>cached_operability;
			std::vector<boost::unordered_map<Core::FMTdevelopment,Core::FMTdevelopment>>cached_operated(spactions.size(), boost::unordered_map<Core::FMTdevelopment,Core::FMTdevelopment>());
			do {
				pass_allocated_area = 0;
				const clock_t begin_time = clock();
				std::map<Core::FMTaction,Spatial::FMTforest> forests  = mapping.getschedule(schedule, cached_operability,yields,schedulepass);
				for (const Spatial::FMTspatialaction& spatial_action : spactions) 
					{
					std::vector<Spatial::FMTspatialaction>::iterator acit = std::find_if(spactions.begin(), spactions.end(), Core::FMTactioncomparator(spatial_action.getname()));
					if (acit != spactions.end())
						{
						const double action_area = targets[acit->getname()];
						Spatial::FMTforest* allowable_forest = &forests[spatial_action];
						if (allowable_forest->area() > 0 && action_area > 0)
							{
							const size_t location = std::distance(spactions.begin(), acit);
							const Spatial::FMTforest spatialy_allowable = allowable_forest->getallowable(*acit, disturbances);
							if (spatialy_allowable.area()>0)
								{
								std::vector<Spatial::FMTevent<Core::FMTdevelopment>>events = spatialy_allowable.buildharvest(action_area, *acit, generator,action_pass);
								if (events.size() > 0)
									{ 
									Spatial::FMTforest newoperated = spatialy_allowable.operate(events, *acit, transitions[location], yields, themes,cached_operated[location], newschedule); //can also use caching here...
									if (!newoperated.mapping.empty())
										{
										disturbances.add(acit->getname(), events);
										mapping.replace(newoperated.mapping.begin(), newoperated.mapping.end());
										targets[acit->getname()] -= (newoperated.area());
										pass_allocated_area += (newoperated.area());
										}
									}
								}
							}
						}
					++action_pass;
					}
				allocated_area += pass_allocated_area;
				++pass;
				if(!schedule_only && pass_allocated_area == 0)
					{
					if(schedulepass)
						{
						schedulepass = false;
						}else{
						schedule_only = true;
						}
					}
				}while(allocated_area<total_area&&(pass_allocated_area!=0||(!schedule_only)));
			}
        mapping = mapping.grow();
		std::map<std::string, double>results;
		results["Total"] = allocated_area / total_area;
		for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator ait = schedule.elements.begin(); ait != schedule.elements.end(); ait++)
			{
			double total_action_area = schedule.actionarea(ait->first);
			results[ait->first.getname()] = ((total_action_area - targets[ait->first.getname()]) / total_action_area);
			}
		operatedschedule.push_back(newschedule);
        return results;
        }


	std::vector<Core::FMTschedule> FMTsesmodel::getschedule() const
		{
		return operatedschedule;
		}

	std::string FMTsesmodel::getdisturbancestats() const
		{
		return disturbances.getpatchstats();
		}

	std::unique_ptr<FMTmodel>FMTsesmodel::presolve(int presolvepass,
		std::vector<Core::FMTactualdevelopment> optionaldevelopments ) const
		{
		if (disturbances.data.empty())//just presolve if no solution
			{
			const std::vector<Core::FMTactualdevelopment>areas = mapping.getarea();
			optionaldevelopments.insert(optionaldevelopments.end(), areas.begin(), areas.end());
			std::unique_ptr<FMTsesmodel>presolvedses(new FMTsesmodel(*(FMTmodel::presolve(presolvepass, optionaldevelopments))));
			const Core::FMTmask presolvedmask = presolvedses->getselectedmask(themes);
			const Core::FMTmask basemask = this->getbasemask(optionaldevelopments);
			presolvedses->mapping = this->mapping.presolve(presolvedmask,presolvedses->themes);
			std::vector<Spatial::FMTspatialaction>newspatialactions;
			for (const Spatial::FMTspatialaction& spaction : spactions)
				{
				if (std::find_if(presolvedses->actions.begin(), presolvedses->actions.end(),Core::FMTactioncomparator(spaction.getname()))!= presolvedses->actions.end())
					{
					const Spatial::FMTspatialaction presolvedspaction(spaction.presolve(basemask, themes, presolvedmask, presolvedses->themes),
						spaction.neighbors, spaction.green_up, spaction.adjacency, spaction.minimal_size, spaction.maximal_size, spaction.neighbors_size);
					newspatialactions.push_back(presolvedspaction);
					}
				}
			presolvedses->spactions = newspatialactions;
			return presolvedses;
			}
		return std::unique_ptr<FMTmodel>(nullptr);
		}

    }
