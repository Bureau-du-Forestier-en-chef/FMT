/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
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

    bool FMTsesmodel::setspactions(const std::vector<Spatial::FMTspatialaction>& lspactions)
        {
		try {
			std::vector<Core::FMTtransition>newtransitions;
			std::vector<Spatial::FMTspatialaction>newspatials;
			std::vector<Core::FMTaction>newbaseactions;
			for (const Spatial::FMTspatialaction& spaction : lspactions)
			{
				std::vector<Core::FMTtransition>::const_iterator trn_iterator = std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator(spaction.getname()));
				if (trn_iterator != transitions.end())
				{
					newtransitions.push_back(*trn_iterator);
					newspatials.push_back(spaction);
					newbaseactions.push_back(spaction);
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTinvalid_transition,
						"Missing transition case for action : " + spaction.getname(),
						"FMTsesmodel::setspactions", __LINE__, __FILE__, Core::FMTsection::Transition);
					return false;
				}
			}
			spactions = lspactions;
			actions = newbaseactions;
			transitions = newtransitions;
		}catch (...)
			{
				_exhandler->printexceptions("", "FMTsesmodel::setspactions", __LINE__, __FILE__);
			}
		return true;
        }
    bool FMTsesmodel::setinitialmapping(const Spatial::FMTforest& forest)
        {
		try {
			disturbances = Spatial::FMTdisturbancestack();
			mapping = forest;
			mapping.setperiod(1);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::setinitialmapping", __LINE__, __FILE__);
		}

		return true;
        }




	std::map<std::string, double> FMTsesmodel::montecarlosimulate(const Core::FMTschedule& schedule,
																	const size_t& randomiterations,
																	bool schedule_only,
																	unsigned int seed,
																	double tolerance)
		{
		std::map<std::string, double>bestresults;
		FMTsesmodel bestmodel;
		try {
			bestresults["Total"] = 0;
			for (size_t iteration = 0; iteration < randomiterations;++iteration)
				{
				FMTsesmodel modelcopy(*this);
				const std::map<std::string, double>results = modelcopy.simulate(schedule, schedule_only, seed);
				if (!results.empty() && results.at("Total")>bestresults.at("Total"))
					{
					if (iteration>0)
						{
						_logger->logwithlevel("Better solution found at Monte-Carlo iteration " +
							std::to_string(iteration) + " value of " + std::to_string(results.at("Total")) + "\n", 1);
						}
					bestresults = results;
					bestmodel = modelcopy;
					if (bestresults.at("Total")>=(1.0-tolerance*1.0))
						{
						break;
						}
					}
				++seed;
				}
			if (bestresults.at("Total")>0)
				{
				*this = bestmodel;
				}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::montecarlosimulate", __LINE__, __FILE__);
		}

		return bestresults;
		}


    std::map<std::string,double> FMTsesmodel::simulate(const Core::FMTschedule& schedule,
                             bool schedule_only,
                             unsigned int seed)
        {
		std::map<std::string, double>results;
		try {
			Core::FMTschedule newschedule(static_cast<int>(disturbances.data.size() + 1), std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>());
			std::default_random_engine generator(seed);
			const double total_area = schedule.area();
			std::map<std::string, double>targets;
			for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator ait = schedule.begin(); ait != schedule.end(); ait++)
			{
				targets[ait->first.getname()] = schedule.actionarea(ait->first);
			}
			disturbances.push(std::map<std::string, std::vector<Spatial::FMTsesevent<Core::FMTdevelopment>>>());
			double allocated_area = 0;
			if (!schedule.empty() || !schedule_only)
			{
				double pass_allocated_area = 0;
				bool schedulepass = true;
				int pass = 0;
				int action_pass = 0;
				boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>cached_operability;
				std::vector<boost::unordered_map<Core::FMTdevelopment, Core::FMTdevelopment>>cached_operated(spactions.size(), boost::unordered_map<Core::FMTdevelopment, Core::FMTdevelopment>());
				do {
					pass_allocated_area = 0;
					//const clock_t begin_time = clock();
					std::map<Core::FMTaction, Spatial::FMTforest> forests = mapping.getschedule(schedule, cached_operability, yields, schedulepass);
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
								if (spatialy_allowable.area() > 0)
								{
									std::vector<Spatial::FMTsesevent<Core::FMTdevelopment>>events = spatialy_allowable.buildharvest(action_area, *acit, generator, action_pass);
									if (events.size() > 0)
									{
										Spatial::FMTforest newoperated = spatialy_allowable.operate(events, *acit, transitions[location], yields, themes, cached_operated[location], newschedule); //can also use caching here...
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
					if (!schedule_only && pass_allocated_area == 0)
					{
						if (schedulepass)
						{
							schedulepass = false;
						}
						else {
							schedule_only = true;
						}
					}
				} while (allocated_area < total_area && (pass_allocated_area != 0 || (!schedule_only)));
			}
			mapping = mapping.grow();
			results["Total"] = allocated_area / total_area;
			for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator ait = schedule.begin(); ait != schedule.end(); ait++)
			{
				double total_action_area = schedule.actionarea(ait->first);
				results[ait->first.getname()] = ((total_action_area - targets[ait->first.getname()]) / total_action_area);
			}
			operatedschedule.push_back(newschedule);
		}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTsesmodel::simulate", __LINE__, __FILE__);
			}
        return results;
        }

    std::map<std::string,double> FMTsesmodel::newsimulate(const Core::FMTschedule& schedule,
                             Spatial::FMTspatialschedule spschedule,
                             bool schedule_only,
                             unsigned int seed)
        {
		std::map<std::string, double>results;
		//Last period in the linegraph
		const int previousperiod = spschedule.lastperiod();
		try {
		    //Create a newschedule for the period we simulate
			Core::FMTschedule newschedule(previousperiod+1, std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>());
			std::default_random_engine generator(seed);
			const double total_area = schedule.area();
			std::map<std::string, double>targets;
			for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator ait = schedule.begin(); ait != schedule.end(); ait++)
			{
				targets[ait->first.getname()] = schedule.actionarea(ait->first);
			}
			//disturbances.push(std::map<std::string, std::vector<Spatial::FMTsesevent<Core::FMTdevelopment>>>());
			double allocated_area = 0;
			//Validate this if ??
			if (!schedule.empty() || !schedule_only)
			{
				double pass_allocated_area = 0;
				bool schedulepass = true;
				int pass = 0;
				int action_pass = 0;
				boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>cached_operability;
				std::vector<boost::unordered_map<Core::FMTdevelopment, Core::FMTdevelopment>>cached_operated(spactions.size(), boost::unordered_map<Core::FMTdevelopment, Core::FMTdevelopment>());
				do {
					pass_allocated_area = 0;
					std::map<Core::FMTaction, std::set<Spatial::FMTcoordinate>> scheduled_coord = spschedule.getschedule(schedule, cached_operability, previousperiod, yields, schedulepass);
					for (const Spatial::FMTspatialaction& spatial_action : spactions)
					{
						std::vector<Spatial::FMTspatialaction>::iterator acit = std::find_if(spactions.begin(), spactions.end(), Core::FMTactioncomparator(spatial_action.getname()));
						if (acit != spactions.end())
						{
							const double action_area = targets[acit->getname()];
							std::set<Spatial::FMTcoordinate>* allowable_coordinates = &scheduled_coord[spatial_action];
							if (!allowable_coordinates->empty() && action_area > 0)
							{
								const size_t location = std::distance(spactions.begin(), acit);
								const std::set<Spatial::FMTcoordinate> spatialy_allowable = spschedule.verifyspatialfeasability(*acit, actions, previousperiod, *allowable_coordinates);
								if (!spatialy_allowable.empty())
								{
								    ///� faire
								    /*
									std::vector<Spatial::FMTsesevent<Core::FMTdevelopment>>events = spatialy_allowable.buildharvest(action_area, *acit, generator, action_pass);
									if (events.size() > 0)
									{
										Spatial::FMTforest newoperated = spatialy_allowable.operate(events, *acit, transitions[location], yields, themes, cached_operated[location], newschedule); //can also use caching here...
										if (!newoperated.mapping.empty())
										{
											disturbances.add(acit->getname(), events);
											mapping.replace(newoperated.mapping.begin(), newoperated.mapping.end());
											targets[acit->getname()] -= (newoperated.area());
											pass_allocated_area += (newoperated.area());
										}
									}*/
								}
							}
						}
						++action_pass;
					}
					allocated_area += pass_allocated_area;
					++pass;
					if (!schedule_only && pass_allocated_area == 0)
					{
						if (schedulepass)
						{
							schedulepass = false;
						}
						else {
							schedule_only = true;
						}
					}
				} while (allocated_area < total_area && (pass_allocated_area != 0 || (!schedule_only)));
			}
			mapping = mapping.grow();
			results["Total"] = allocated_area / total_area;
			for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator ait = schedule.begin(); ait != schedule.end(); ait++)
			{
				double total_action_area = schedule.actionarea(ait->first);
				results[ait->first.getname()] = ((total_action_area - targets[ait->first.getname()]) / total_action_area);
			}
			operatedschedule.push_back(newschedule);
		}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTsesmodel::simulate", __LINE__, __FILE__);
			}
        return results;
        }



	std::string FMTsesmodel::getdisturbancestats() const
		{
		return disturbances.getpatchstats();
		}

	std::unique_ptr<FMTmodel>FMTsesmodel::presolve(int presolvepass,
		std::vector<Core::FMTactualdevelopment> optionaldevelopments ) const
		{
		try {
			if (disturbances.data.empty())//just presolve if no solution
			{
				const std::vector<Core::FMTactualdevelopment>areas = mapping.getarea();
				optionaldevelopments.insert(optionaldevelopments.end(), areas.begin(), areas.end());
				std::unique_ptr<FMTmodel>presolvedmod(new FMTsesmodel(*(FMTmodel::presolve(presolvepass, optionaldevelopments))));
				FMTsesmodel*presolvedses = dynamic_cast<FMTsesmodel*>(presolvedmod.get());
				const Core::FMTmask presolvedmask = presolvedses->getselectedmask(themes);
				const Core::FMTmask basemask = this->getbasemask(optionaldevelopments);
				presolvedses->mapping = this->mapping.presolve(presolvedmask, presolvedses->themes);
				std::vector<Spatial::FMTspatialaction>newspatialactions;
				for (const Spatial::FMTspatialaction& spaction : spactions)
				{
					if (std::find_if(presolvedses->actions.begin(), presolvedses->actions.end(), Core::FMTactioncomparator(spaction.getname())) != presolvedses->actions.end())
					{
						const Spatial::FMTspatialaction presolvedspaction(spaction.presolve(basemask, themes, presolvedmask, presolvedses->themes),
							spaction.neighbors, spaction.green_up, spaction.adjacency, spaction.minimal_size, spaction.maximal_size, spaction.neighbors_size);
						newspatialactions.push_back(presolvedspaction);
					}
				}
				presolvedses->spactions = newspatialactions;
				return presolvedmod;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTsesmodel::presolve", __LINE__, __FILE__);
			}
		return std::unique_ptr<FMTmodel>(nullptr);
		}

	std::unique_ptr<FMTmodel>FMTsesmodel::postsolve(const FMTmodel& originalbasemodel) const
		{
		try {
			if (!disturbances.data.empty())//just postsolve if you have a solution
			{
				std::unique_ptr<FMTmodel>presolvedmod(new FMTsesmodel(*(FMTmodel::postsolve(originalbasemodel))));
				const Core::FMTmask presolvedmask = this->getselectedmask(themes);
				FMTsesmodel*postsolvedses = dynamic_cast<FMTsesmodel*>(presolvedmod.get());
				postsolvedses->mapping = this->mapping.postsolve(presolvedmask, postsolvedses->themes);
				//Disturbance stack doesn't need changes
				//take care of the FMTspatialactions
				std::vector<Spatial::FMTspatialaction>newspatialactions;
				for (const Spatial::FMTspatialaction& spaction : spactions)
				{
					std::vector<Core::FMTaction>::const_iterator spactit = std::find_if(postsolvedses->actions.begin(), postsolvedses->actions.end(), Core::FMTactioncomparator(spaction.getname()));
					if (spactit != postsolvedses->actions.end())
					{
						const Spatial::FMTspatialaction postsolvedspaction(*spactit,
							spaction.neighbors, spaction.green_up, spaction.adjacency, spaction.minimal_size, spaction.maximal_size, spaction.neighbors_size);
						newspatialactions.push_back(postsolvedspaction);
					}
				}
				postsolvedses->spactions = newspatialactions;
				return presolvedmod;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsesmodel::presolve", __LINE__, __FILE__);
			}
		return std::unique_ptr<FMTmodel>(nullptr);
		}

    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsesmodel)