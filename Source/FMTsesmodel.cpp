/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsesmodel.h"

namespace Models
    {
    FMTsesmodel::FMTsesmodel(): FMTmodel(),mapping(), operatedschedule(),spactions(),spschedule()
        {

        }
    FMTsesmodel::FMTsesmodel(const FMTsesmodel& rhs):
        FMTmodel(rhs),
        mapping(rhs.mapping),
		operatedschedule(rhs.operatedschedule),
        spactions(rhs.spactions),
		events(rhs.events),
		spschedule(rhs.spschedule)
        {

        }
    FMTsesmodel::FMTsesmodel(const FMTmodel& rhs):
        FMTmodel(rhs),
        mapping(), operatedschedule(),spactions(),events(),spschedule()
        {

        }
    FMTsesmodel& FMTsesmodel::operator = (const FMTsesmodel& rhs)
        {
        if (this!=&rhs)
            {
            FMTmodel::operator = (rhs);
            mapping = rhs.mapping;
			operatedschedule = rhs.operatedschedule;
            spactions = rhs.spactions;
			events = rhs.events;
			spschedule = rhs.spschedule;
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
	std::vector<Core::FMTschedule> FMTsesmodel::getschedule() const
	{
		try
		{
			return spschedule.getschedules(actions);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::getschedulesp", __LINE__, __FILE__);
		}
		return std::vector<Core::FMTschedule>();
	}
	bool FMTsesmodel::setinitialmapping(const Spatial::FMTforest& forest)
        {
		try {
			mapping = forest;
			mapping.setperiod(1);
			spschedule = Spatial::FMTspatialschedule(forest);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::setinitialmapping", __LINE__, __FILE__);
		}

		return true;
        }


	std::map<std::string, double> FMTsesmodel::montecarlosimulate(const Core::FMTschedule & schedule, const size_t & randomiterations, unsigned int seed, double tolerance)
	{
		std::map<std::string, double>bestresults;
		FMTsesmodel bestmodel;
		try {
			bestresults["Total"] = 0;
			for (size_t iteration = 0; iteration < randomiterations; ++iteration)
			{
				FMTsesmodel modelcopy(*this);
				bool schedulefirstpass = true;
				if (iteration % 2 == 0)
					{
					schedulefirstpass = false;
					}
				const std::map<std::string, double>results = modelcopy.simulate(schedule, false, schedulefirstpass,seed);
				//*_logger << "iteration id: " << iteration << " objective is: "<< results.at("Total") <<"\n";
				if (!results.empty() && results.at("Total") > bestresults.at("Total"))
				{
					if (iteration > 0)
					{
						_logger->logwithlevel("Better solution found at Monte-Carlo iteration " +
							std::to_string(iteration) + " value of " + std::to_string(results.at("Total")) + "\n", 1);
					}
					bestresults = results;
					bestmodel = modelcopy;
					if (bestresults.at("Total") >= (1.0 - tolerance * 1.0))
					{
						break;
					}
				}
				++seed;
			}
			if (bestresults.at("Total") > 0)
			{
				*this = bestmodel;
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::montecarlosimulate", __LINE__, __FILE__);
		}

		return bestresults;
	}


    

    std::map<std::string,double> FMTsesmodel::simulate(	const Core::FMTschedule& schedule,
															bool schedule_only,
															bool scheduleatfirstpass,
															unsigned int seed)
        {
		/*
		Two major problem ... by getting the schedule before operating every actions, if two actions are operable, there is a possbility that you do both. Im getting error in dev operate
		Also, in build harvest, for actions that you dont check for acdjacency, there is a possibility that you have multiple cuts with the same coordinate in it because your always passing the same territory, only the ignit coord are changing
		For the latest, see the modification in spatialschedule buildharbest. Every time a coordinate is selected in an event, it's removed from the territory that is passed to spread.

		*/
		std::map<std::string, double>results;
		try {
			const int period = spschedule.actperiod();
			spschedule.setnewperiod();
			std::default_random_engine generator(seed);
			const double total_area = schedule.area();
			std::map<std::string, double>targets;
			for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator ait = schedule.begin(); ait != schedule.end(); ait++)
			{
				targets[ait->first.getname()] = schedule.actionarea(ait->first);
			}
			double allocated_area = 0;
			if (!schedule.empty() || !schedule_only)
			{
				double pass_allocated_area = 0;
				bool schedulepass = scheduleatfirstpass;
				bool schedulechange = false;
				int pass = 0;
				std::vector<std::set<Spatial::FMTcoordinate>>actions_operabilities = spschedule.getupdatedscheduling(spactions, schedule, yields, schedulepass);
				do {
					//*_logger << "pass: " << pass<<" schedule pass: "<< schedulepass <<"\n";
					pass_allocated_area = 0;
					int action_id = 0;
					if (schedulechange)
						{
						actions_operabilities = spschedule.getupdatedscheduling(spactions, schedule, yields, schedulepass);
						schedulechange = false;
						}
					for (const Spatial::FMTspatialaction& spatial_action : spactions)
					{
						const double action_area = targets[spatial_action.getname()];
						//const std::set<Spatial::FMTcoordinate> allowable_coordinates = spschedule.getscheduling(spatial_action, schedule,yields, schedulepass);
						const std::set<Spatial::FMTcoordinate> allowable_coordinates = actions_operabilities.at(action_id);
						if (!allowable_coordinates.empty() && action_area > 0)
						{
							const std::set<Spatial::FMTcoordinate> spatialy_allowable = spschedule.verifyspatialfeasability(spatial_action, actions, period, allowable_coordinates);
							if (!spatialy_allowable.empty())
							{
								std::vector<Spatial::FMTcoordinate> updatedcells;
								const Spatial::FMTeventcontainer harvest = spschedule.buildharvest(action_area, spatial_action, generator, spatialy_allowable, period, action_id, updatedcells);
								if (harvest.size() > 0)
								{
									const double operatedarea = spschedule.operate(harvest, spatial_action, action_id, transitions[action_id], yields, themes);
									spschedule.addevents(harvest);
									actions_operabilities = spschedule.getupdatedscheduling(spactions,schedule, yields, schedulepass, actions_operabilities, updatedcells);
									targets[spatial_action.getname()] -= operatedarea;
									pass_allocated_area += operatedarea;
								}
							}
						}
						++action_id;
					}
					allocated_area += pass_allocated_area;
					++pass;
					if (!schedule_only && pass_allocated_area == 0)
					{
						if (schedulepass)
						{
							schedulechange = true;
							schedulepass = false;
						}
						else {
							schedule_only = true;
						}
					}
				} while (allocated_area < total_area && (pass_allocated_area != 0 || (!schedule_only)));
			}
			spschedule.grow();
			results["Total"] = allocated_area / total_area;
			for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator ait = schedule.begin(); ait != schedule.end(); ait++)
			{
				double total_action_area = schedule.actionarea(ait->first);
				results[ait->first.getname()] = ((total_action_area - targets[ait->first.getname()]) / total_action_area);
			}
		}catch (...)
			{
				_exhandler->printexceptions("", "FMTsesmodel::newsimulate", __LINE__, __FILE__);
			}
        return results;
        }

	std::string FMTsesmodel::getdisturbancestats() const
	{
		return spschedule.getpatchstats(actions);
	}

	std::unique_ptr<FMTmodel>FMTsesmodel::presolve(int presolvepass,
		std::vector<Core::FMTactualdevelopment> optionaldevelopments ) const
		{
		try {
			if (spschedule.empty())//just presolve if no solution
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
			if (!spschedule.empty())//just postsolve if you have a solution
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
