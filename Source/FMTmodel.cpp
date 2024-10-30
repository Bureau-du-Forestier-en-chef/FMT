/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <boost/algorithm/string.hpp>

#include "FMTmodel.h"
#include <chrono>
#include <memory>
#include "FMTschedule.h"
#include "FMTmodelstats.h"
#include "FMTaction.h"
#include "FMTdevelopmentpath.h"
#include "FMTageyieldhandler.h"
#include "FMTtimeyieldhandler.h"
#include "FMTmaskfilter.h"
#include "FMTexceptionhandler.h"
#include "FMTtransitionmask.h"
#include "FMTfuturdevelopment.h"
#include <numeric>
#include <thread>
//#include <cvmarkersobj.h>
#include <memory>
//#include <boost/container/flat_set.hpp>
//using namespace Concurrency::diagnostic;


namespace Models{

	void FMTmodel::setReplicate(size_t p_replicate, int p_ReplanningPeriod)
		{
		for (Core::FMTconstraint& modelConstraint : constraints)
			{
			modelConstraint.setFromReplicate(p_replicate, p_ReplanningPeriod);
			}
		}

	bool  FMTmodel::gotReIgnore(const int& p_replanningPeriod) const
		{
		bool gotIt = false;
		try {
			size_t cId = 0;
			while (!gotIt && cId < constraints.size())
			{
				gotIt = constraints.at(cId).isreignore(p_replanningPeriod);
				++cId;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTmodel::gotReIgnore", __LINE__, __FILE__);
			}
		return gotIt;
		}

	bool FMTmodel::gotReplicate(const int& p_replanningPeriod) const
	{
		bool gotIt = false;
		try {
			size_t cId = 0;
			while (!gotIt && cId < constraints.size())
			{
				gotIt = constraints.at(cId).gotReplicate(p_replanningPeriod);
				++cId;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodel::gotReIgnore", __LINE__, __FILE__);
		}
		return gotIt;
	}

	std::vector<Core::FMTschedule>FMTmodel::buildSchedule(const Core::FMTaction& p_action,
															const std::vector<Core::FMTschedule>& p_schedules) const
	{
		
		std::vector<Core::FMTschedule>newSchedules;
		try {
			Core::FMTmask ActionMask = p_action.getunion(themes);
			ActionMask.update(themes);
			const std::vector<size_t>Static = statictransitionthemes;
			for (size_t thId = 0; thId < themes.size();++thId)
				{
				if (std::find(Static.begin(),Static.end(), thId)== Static.end())
					{
					ActionMask.set(themes.at(thId), "?");
					}
				}
			const int MAX_PERIOD = std::min(p_action.getperiodupperbound(),
				getparameter(Models::FMTintmodelparameters::LENGTH));
			const std::string POST_ATTRIBUTE = themes.back().getbaseattributes().back();
			boost::unordered_map<Core::FMTdevelopment, double>ActToRemove;
			for (const Core::FMTschedule& ACT_SCHEDULE : extendSchedule(p_schedules))
			{
				const int PERIOD = ACT_SCHEDULE.getperiod();
				Core::FMTschedule baseSchedule(PERIOD, ACT_SCHEDULE, ACT_SCHEDULE.douselock());
				for (const auto& ACTION : ACT_SCHEDULE)
					{
					for (const auto& dev : ACTION.second)
						{
						Core::FMTdevelopment newDev(dev.first);
						if (dev.first.getmask().isSubsetOf(ActionMask))
							{
							if (PERIOD > MAX_PERIOD)
								{
								Core::FMTmask maskToChange = dev.first.getmask();
								maskToChange.set(themes.back(), POST_ATTRIBUTE);
								newDev.setmask(maskToChange);
							}else{
								double total = 0;
								for (const double& value : dev.second)
									{
									total += value;
									}
								Core::FMTdevelopment tohash(dev.first);
								tohash.setperiod(PERIOD);
								tohash.setlock(0);
								if (ActToRemove.find(tohash)==ActToRemove.end())
									{
									ActToRemove[tohash] = 0;
									}
								ActToRemove[tohash] += total;
								}

							}
						for (const double& AREA : dev.second)
							{
							baseSchedule.addevent(newDev, AREA, ACTION.first);
							}
						}
					}
				newSchedules.push_back(baseSchedule);

			}
			
			for (const Core::FMTactualdevelopment& dev : area)
				{
				Core::FMTfuturdevelopment futurDev(dev);
				futurDev.setperiod(1);
				futurDev.setlock(0);
				for (int period = 1; period <= MAX_PERIOD;++period)
					{
					newSchedules[period - 1].setperiod(period);
					newSchedules[period - 1].setuselock(false);
					if (futurDev.operable(p_action,yields))
						{
						double value = dev.getarea();
						if (ActToRemove.find(futurDev)!= ActToRemove.end())
							{
							value -= ActToRemove.at(futurDev);
							}
						newSchedules[period - 1].addevent(futurDev, value, p_action);
						}
					futurDev = futurDev.grow();
					}
				}

		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTmodel::buildSchedule", __LINE__, __FILE__);
			}
		return newSchedules;
	}

	std::vector<Core::FMTschedule>FMTmodel::extendSchedule(const std::vector<Core::FMTschedule>& p_schedules) const
	{
		std::vector<Core::FMTschedule>newSchedules;
		try {
			newSchedules.reserve(p_schedules.size());
			for (const Core::FMTschedule& SCHEDULE : p_schedules)
				{
				Core::FMTschedule newSchedule(SCHEDULE.getperiod(),SCHEDULE,SCHEDULE.douselock());
				for (const auto& ACTION : SCHEDULE)
					{
					std::vector<Core::FMTaction>::const_iterator actPtr = std::find_if(actions.begin(), actions.end(),Core::FMTactioncomparator(ACTION.first.getname()));
					if (actPtr != actions.end())
						{
						for (const auto& dev : ACTION.second)
							{
							const Core::FMTdevelopment TARGET(Core::FMTmask(dev.first.getmask(), themes), dev.first.getage(), dev.first.getlock());
							for (const double& VALUE : dev.second)
								{
								newSchedule.addevent(TARGET, VALUE, *actPtr);
								}
							}
						}
					}
				newSchedules.push_back(newSchedule);
				}

		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTmodel::extendSchedule", __LINE__, __FILE__);
			}
		return newSchedules;
	}


	FMTmodel FMTmodel::buildAction(const std::string& p_actionName,
		const std::string& p_Targetyield) const
	{
		FMTmodel newModel(*this);
		try {
			const std::vector<std::string>ATTRIBUTES{ "PRE","POST" };
			newModel.pushTheme("~FMT_THEME_" + p_actionName, ATTRIBUTES);
			Core::FMTaction newAction("~FMT_" + p_actionName,false,true);
			Core::FMTtransition newTransition("~FMT_" + p_actionName);
			std::string BMask;
			for (size_t themeId = 0; themeId < newModel.themes.size() - 1; ++themeId)
			{
				BMask += "? ";
			}
			BMask += ATTRIBUTES.back();
			Core::FMTmask ActionMask;
			boost::unordered_map<Core::FMTmask,double>YieldMasks;
			for (const auto& yield : yields)
				{
				if (yield.second->containsyield(p_Targetyield)&&
					yield.second->gettype()==Core::FMTyldtype::FMTageyld)
					{
					const Core::FMTageyieldhandler* AGE_HANDLER = dynamic_cast<const Core::FMTageyieldhandler*>(yield.second.get());
					const double PEAK = AGE_HANDLER->getpeakfrom(p_Targetyield,FMT_DBL_TOLERANCE);
					if (PEAK > 0)
						{
						const Core::FMTmask MASK = AGE_HANDLER->getmask();
						YieldMasks[MASK] = PEAK;
						const Core::FMTmask NEW_MASK(std::string(MASK) + " "+*ATTRIBUTES.begin(), newModel.themes);
						Core::FMTspec specification;
						specification.setbounds(Core::FMTperbounds(Core::FMTsection::Action, PEAK, PEAK));
						if (ActionMask.empty())
							{
							ActionMask = NEW_MASK;
						}else {
							ActionMask = ActionMask.getunion(NEW_MASK);
							}
						newAction.push_back(NEW_MASK, specification);
						Core::FMTfork fork;
						const Core::FMTtransitionmask TR_MASK(std::string(BMask), newModel.themes, 100.0);
						fork.add(TR_MASK);
						newTransition.push_back(NEW_MASK, fork);
						}
					}
				}
			newModel.addNewMask(ActionMask);
			Core::FMTyields newYields;
			for (const auto& yield : yields)
				{
				const Core::FMTmask YIELD_MASK = yield.second.get()->getmask();
				const Core::FMTmask NEW_MASK(std::string(YIELD_MASK) + " ?", newModel.themes);
				std::unique_ptr<Core::FMTyieldhandler>newYield(yield.second->clone());
				newYield->setMask(NEW_MASK);
				if (YieldMasks.find(YIELD_MASK)!= YieldMasks.end())
					{
					const size_t TO_SPLIT = static_cast<size_t>(YieldMasks.at(YIELD_MASK)) + 1;
					const Core::FMTmask NEW_SPLITTED_MASK(std::string(YIELD_MASK) + " "+ ATTRIBUTES.back(), newModel.themes);
					std::unique_ptr<Core::FMTyieldhandler>newSplittedYield(new Core::FMTageyieldhandler(NEW_SPLITTED_MASK));
					const std::vector<int> BASES = yield.second->getbases();
					const int SPLIT_DISTANCE = static_cast<int>(std::distance(BASES.begin() + TO_SPLIT, BASES.end()));
					for (int i = 1; i <= SPLIT_DISTANCE;++i)
						{
						newSplittedYield->push_base(i);
						}
					for (const std::string& YIELD_NAME : yield.second->getyieldnames())
						{
						const Core::FMTdata BASE_DATA = yield.second->at(YIELD_NAME);
						const std::vector<double>NEW_DATA(BASE_DATA.data.begin() + TO_SPLIT, BASE_DATA.data.end());
						newSplittedYield->push_data(YIELD_NAME, Core::FMTdata(NEW_DATA, BASE_DATA.getop(), BASE_DATA.getSourcesCopy()));
						}
					newYields.push_back(NEW_SPLITTED_MASK, newSplittedYield);
					}
				newYields.push_back(NEW_MASK, newYield);
				}
			newYields.update();
			newAction.update();
			newTransition.update();
			newModel.yields = newYields;
			newModel.actions.insert(newModel.actions.begin(), newAction);
			newModel.transitions.insert(newModel.transitions.begin(),newTransition);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTmodel::BuildAction", __LINE__, __FILE__);
			}
		return newModel;
	}

	void FMTmodel::pushTheme(const std::string& p_themeName,
		const std::vector<std::string>& p_attributes)
	{
		try {
			if (p_attributes.empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_theme, "Missing attributes for "+ p_themeName,
					"FMTmodel::pushTheme", __LINE__, __FILE__);
				}
			const size_t THEME_START = themes.back().getstart() + themes.back().size();
			std::vector<Core::FMTtheme> Oldthemes(themes);
			themes.emplace_back(p_attributes, themes.size(), THEME_START, p_themeName);
			const std::string DEFAULT_ATTRIBUTE = "?";
			const std::string DEFAULT_BASE = *p_attributes.begin();
			for (auto& dev : area)
				{
				dev.setmask(Core::FMTmask(std::string(dev.getmask()) + " " + DEFAULT_BASE,themes));
				}
			for (auto& action : actions)
				{
				action.unshrink(Oldthemes);
				for (auto& op : action)
					{
					op.first = Core::FMTmask(std::string(op.first) + " " + DEFAULT_ATTRIBUTE, themes);
					}
				action.update();
				}
			for (auto& transition : transitions)
				{
				transition.unshrink(Oldthemes);
				for (auto& op : transition)
					{
					op.first = Core::FMTmask(std::string(op.first) + " " + DEFAULT_ATTRIBUTE, themes);
					Core::FMTfork newFork(op.second);
					newFork.clear();
					for (Core::FMTtransitionmask trnm : op.second.getmasktrans())
						{
						trnm.setmask(Core::FMTmask(std::string(trnm.getmask()) + " "+ DEFAULT_ATTRIBUTE, themes));
						newFork.add(trnm);
						}
					op.second = newFork;
					}
				transition.update();
				}
			for (auto& yield : yields)
				{
				yield.first = Core::FMTmask(std::string(yield.first) + " " + DEFAULT_ATTRIBUTE, themes);
				yield.second->setMask(Core::FMTmask(std::string(yield.second->getmask()) + " " + DEFAULT_ATTRIBUTE, themes));
				}
			for (auto& output : outputs)
				{
				std::vector<Core::FMToutputsource> sources = output.getsources();
				for (Core::FMToutputsource& source : sources)
					{
					if (source.isvariable())
						{
						source.setmask(Core::FMTmask(std::string(source.getmask()) + " " + DEFAULT_ATTRIBUTE, themes));
						}
					}
				output.setsources(sources);
				}
			for (auto& output : constraints)
				{
				std::vector<Core::FMToutputsource> sources = output.getsources();
				for (Core::FMToutputsource& source : sources)
				{
					if (source.isvariable())
					{
						source.setmask(Core::FMTmask(std::string(source.getmask()) + " " + DEFAULT_ATTRIBUTE, themes));
					}
				}
				output.setsources(sources);
				}
		
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTmodel::pushTheme", __LINE__, __FILE__);
			}
	}

	Models::FMTmodel FMTmodel::aggregateAllActions(const std::vector<std::string>& p_Aggregates,
		std::vector<std::string> p_ActionOrdering) const
	{
		Models::FMTmodel newModel(*this);
		try {
			if (p_ActionOrdering.empty())
				{
				p_ActionOrdering.reserve(actions.size());
				for (const Core::FMTaction& ACTION : actions)
					{
					p_ActionOrdering.push_back(ACTION.getname());
					}
				}
			const std::map<std::string, std::pair<std::string, Core::FMTmask>> Filters = newModel.aggregateActions(p_Aggregates);
			newModel.aggregateTransitions(Filters);
			std::vector<Core::FMToutput*>Outputs;
			for (Core::FMToutput& output : newModel.outputs)
			{
				Outputs.push_back(&output);
			}
			newModel.aggregateOutputs(Filters, Outputs, p_ActionOrdering);
			std::vector<Core::FMToutput*>Constraints;
			for (Core::FMTconstraint& constraint : newModel.constraints)
			{
				Constraints.push_back(&constraint);
			}
			newModel.aggregateOutputs(Filters, Constraints, p_ActionOrdering);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTmodel::aggregateAllActions", __LINE__, __FILE__);
		}
		return newModel;
	}

	std::vector<std::string>FMTmodel::getSchedulesPriorities(const std::vector<Core::FMTschedule>& p_schedules) const
		{
		std::map<std::string, double>allActions;
		//const int MAX_PERIOD = 10;
		for (const Core::FMTschedule& p_schedule : p_schedules)
			{
			//if (p_schedule.getperiod() <= MAX_PERIOD)
				//{
				for (const auto& ACTION : p_schedule)
					{
					const std::string ACTION_NAME = ACTION.first.getname();
					const double ACTION_AREA = p_schedule.actionarea(ACTION.first);
					if (allActions.find(ACTION_NAME) == allActions.end())
					{
						allActions[ACTION_NAME] = 0.0;
					}
					allActions[ACTION_NAME] += ACTION_AREA;
					/*const size_t ACTION_ID = std::distance(actions.begin(), std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(ACTION_NAME)));
					double total = ACTION_AREA;
					for (const auto& DEV_ACTION : ACTION.second)
						{
						const double AREA = std::accumulate(DEV_ACTION.second.begin(), DEV_ACTION.second.end(), 0.0);
						if (AREA > FMT_DBL_TOLERANCE)
							{
							const std::vector<Core::FMTdevelopmentpath> PATHS = DEV_ACTION.first.operate(actions[ACTION_ID], transitions[ACTION_ID], yields, themes);
							const double COEF = DEV_ACTION.first.getharvestcoef(PATHS, actions[ACTION_ID], yields, "YV_S");
							total += AREA * COEF;
							}
						}
					if (allActions.find(ACTION_NAME) == allActions.end())
						{
						allActions[ACTION_NAME] = 0.0;
						}
					allActions[ACTION_NAME] = total;*/
					}
				//}
			}
		for (const Core::FMTaction& ACTION : actions)
		{
			if (allActions.find(ACTION.getname()) == allActions.end())
			{
				allActions[ACTION.getname()] = 0.0;
			}

		}
		std::vector<std::string>FinalOrder;
		FinalOrder.reserve(allActions.size());
		while (!allActions.empty())
			{
			std::map<std::string, double>::iterator max_it = allActions.end();
			double maxValue = -1.0;
			for (std::map<std::string, double>::iterator it = allActions.begin();it!= allActions.end();++it)
				{
				if (it->second > maxValue)
					{
					max_it = it;
					maxValue = it->second;
					}
				}
			FinalOrder.push_back(max_it->first);
			allActions.erase(max_it);
			}

		return FinalOrder;
		}


	Models::FMTmodel FMTmodel::splitActions(const std::vector<std::string>& p_Actions,
									const std::vector<std::string>& p_masks) const
	{
		Models::FMTmodel newModel(*this);
		try {
			std::vector<Core::FMTmask> masks;
			for (const std::string& mask : p_masks)
			{
				masks.push_back(Core::FMTmask(mask, themes));
			}
			std::vector<Core::FMTaction>NewActions;
			std::vector<Core::FMTtransition>NewTransitions;
			size_t i = 0;
			for (const Core::FMTaction& ACTION : actions)
				{
				if (std::find(p_Actions.begin(), p_Actions.end(), ACTION.getname())!= p_Actions.end())
					{
					const std::vector<Core::FMTaction>SPLITTED = ACTION.split(masks, themes);
					for (const Core::FMTaction& SPLITTED_ACTION : SPLITTED)
					{
						Core::FMTtransition NewTransition = transitions.at(i);
						NewTransition.setName(SPLITTED_ACTION.getname());
						NewTransitions.push_back(NewTransition);
					}
					NewActions.insert(NewActions.end(), SPLITTED.begin(), SPLITTED.end());
					}else {
						NewActions.push_back(ACTION);
						NewTransitions.push_back(transitions.at(i));
						}
				++i;
				}
			
			newModel.actions = NewActions;
			newModel.transitions = NewTransitions;
			
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTmodel::splitActions", __LINE__, __FILE__);
			}
		return newModel;
	}

	std::vector<Core::FMTschedule> FMTmodel::splitSchedules(const std::vector<Core::FMTschedule>& p_schedules) const
	{
		std::vector<Core::FMTschedule>NewSchedules;
		try {
			for (const Core::FMTschedule& SCHEDULE : p_schedules)
			{
				std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>> mapping;
				for (const auto& ACTION_ELEMENTS : SCHEDULE)
					{
					const std::string GLOBAL_ACTION = ACTION_ELEMENTS.first.getname();
					std::vector<Core::FMTaction>::const_iterator ActIt = std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(GLOBAL_ACTION));
					if (ActIt == actions.end())//Got aggregate
						{
						boost::unordered_set<Core::FMTdevelopment>DevelopementsSet;
						
						const std::vector<const Core::FMTaction*> ACTIONS = Core::FMTactioncomparator(GLOBAL_ACTION, true).getallaggregates(actions,true);
						if (ACTIONS.empty())
							{
								_exhandler->raise(Exception::FMTexc::FMTinvalidAandT, "Missing aggregate " + GLOBAL_ACTION,
									"FMTmodel::splitSchedules", __LINE__, __FILE__);
							}
						std::map<Core::FMTdevelopment, std::vector<double>>NewMapping;
						for (const Core::FMTaction* ACTION_Prt : ACTIONS)
						{
							const Core::FMTmask ACTION_MASK = ACTION_Prt->getunion(themes);
							for (const auto& DEV : ACTION_ELEMENTS.second)
							{
								if (DEV.first.getmask().isSubsetOf(ACTION_MASK) &&
									DevelopementsSet.find(DEV.first) == DevelopementsSet.end())
								{
									NewMapping[DEV.first] = DEV.second;
									DevelopementsSet.insert(DEV.first);
								}
							}
							if (!NewMapping.empty())
							{
								mapping[*ACTION_Prt] = NewMapping;
							}
						}
						}else {
							mapping[ACTION_ELEMENTS.first] = ACTION_ELEMENTS.second;
						}
					
					
					}
				NewSchedules.push_back(Core::FMTschedule(SCHEDULE.getperiod(), mapping));
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTmodel::splitSchedules", __LINE__, __FILE__);
			}
		return NewSchedules;
	}

	std::vector<Core::FMTschedule> FMTmodel::aggregateSchedules(const std::vector<Core::FMTschedule>& p_schedules) const
		{
		std::vector<Core::FMTschedule>newSchedules;
		try {
			for (const Core::FMTschedule& SCHEDULE : p_schedules)
				{
				std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>> mapping;
				for (const auto& ACTION_ELEMENTS : SCHEDULE)
					{
					const Core::FMTaction* NewAction = nullptr;
					const std::string ACTION_NAME = ACTION_ELEMENTS.first.getname();
					std::vector<Core::FMTaction>::const_iterator ACTION_IT = std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(ACTION_NAME));
					if (ACTION_IT==actions.end())//Need to change the action!
						{
						const std::vector<std::string>AGGREGATES = ACTION_ELEMENTS.first.getaggregates();
						for (const Core::FMTaction& action : actions)
							{
							if (std::find(AGGREGATES.begin(), AGGREGATES.end(),action.getname())!= AGGREGATES.end())
								{
								NewAction = &action;
								}
							}
					}else {
						NewAction = &*ACTION_IT;
						}
					if (!NewAction)
						{
						_exhandler->raise(Exception::FMTexc::FMTinvalidAandT, "Cant find aggregate for action "+ ACTION_NAME,
							"FMTmodel::aggregateSchedules", __LINE__, __FILE__);
						}
					if (mapping.find(*NewAction)==mapping.end())
						{
							mapping[*NewAction] = ACTION_ELEMENTS.second;
						}else {
							for (const auto& DEV_ELEMENTS : ACTION_ELEMENTS.second)
								{
								if (mapping.at(*NewAction).find(DEV_ELEMENTS.first) == mapping.at(*NewAction).end())
									{
									mapping[*NewAction][DEV_ELEMENTS.first] = DEV_ELEMENTS.second;
								}else {
									mapping[*NewAction][DEV_ELEMENTS.first][0] += DEV_ELEMENTS.second.at(0);
									}

								}
						}
					}
				newSchedules.push_back(Core::FMTschedule(SCHEDULE.getperiod(), mapping));
				}

		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTmodel::aggregateSchedules", __LINE__, __FILE__);
			}
		return newSchedules;
		}


	std::map<std::string, std::pair<std::string, Core::FMTmask>> FMTmodel::aggregateActions(std::vector<std::string> p_ActionsMapping)
	{
		std::map<std::string, std::pair<std::string, Core::FMTmask>>ActionFilters;
		try {
			std::vector<Core::FMTaction>NewActions;
			std::set<std::string>ActionCover;
			if (std::find(p_ActionsMapping.begin(), p_ActionsMapping.end(), "_DEATH") == p_ActionsMapping.end())
			{
				p_ActionsMapping.push_back("_DEATH");
			}
			for (const std::string& Aggregate : p_ActionsMapping)
				{
				Core::FMTaction NewAction(Aggregate);
				for (const Core::FMTaction* Action : Core::FMTactioncomparator(Aggregate, true).getallaggregates(actions))
					{
					Core::FMTaction NoCompress(*Action);
					std::vector<std::string>aggregates;
					for (const std::string& AGGREGATE : NoCompress.getaggregates())
						{
						if (std::find(p_ActionsMapping.begin(), p_ActionsMapping.end(), AGGREGATE)== p_ActionsMapping.end())
							{
							aggregates.push_back(AGGREGATE);
							}
						}
					NoCompress.setaggregates(aggregates);
					if (ActionCover.find(Action->getname()) != ActionCover.end())
						{
						_exhandler->raise(Exception::FMTexc::FMTinvalidAandT, "Action " + Action->getname() +" already in aggregate",
							"FMTmodel::aggregateActions", __LINE__, __FILE__);
						}
					ActionCover.insert(Action->getname());
					NoCompress.unshrink(themes);
					const Core::FMTmask Filter = NoCompress.getunion(themes);

					ActionFilters[Action->getname()] = std::pair<std::string, Core::FMTmask>(Aggregate, Filter);
					NewAction += NoCompress;
					}
				if (!NewAction.empty())
					{
					NewAction.update();
					NewActions.push_back(NewAction);
					}
				}
			if (ActionCover.size() != actions.size())
				{
				std::string missingActions;
				for (const Core::FMTaction Action : actions)
					{
					if (ActionCover.find(Action.getname())== ActionCover.end())
						{
						missingActions += Action.getname() + ",";
						}
					}
				missingActions.pop_back();
				_exhandler->raise(Exception::FMTexc::FMTinvalidAandT, "Missing aggregate for actions "+ missingActions,
					"FMTmodel::aggregateActions", __LINE__, __FILE__);
				}

			//Handle aggregate of the action
			for (const Core::FMTaction ACTION : actions)
			{
				for (const std::string& AGGREGATE : ACTION.getaggregates())
				{
					if (std::find(p_ActionsMapping.begin(), p_ActionsMapping.end(), AGGREGATE)== p_ActionsMapping.end())
						{
						if (ActionFilters.find(AGGREGATE) != ActionFilters.end())
						{
							ActionFilters[AGGREGATE].second = ActionFilters[AGGREGATE].second.getunion(ActionFilters[ACTION.getname()].second);
						}else {
							if (ActionFilters.find(ACTION.getname()) != ActionFilters.end())
								{
								ActionFilters[AGGREGATE] = std::pair<std::string, Core::FMTmask>("", ActionFilters[ACTION.getname()].second);
								}
							
							}
						
						}

				}
			}
			


			for (auto& Filter : ActionFilters)
			{
				Filter.second.second = addNewMask(Filter.second.second);
				
			}
			//setactions(NewActions);
			actions = NewActions;
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodel::aggregateActions", __LINE__, __FILE__);
		}
		return ActionFilters;
	}

void FMTmodel::aggregateTransitions(const std::map<std::string, std::pair<std::string, Core::FMTmask>>& p_Filters)
	{
	
	try {
		std::vector<Core::FMTtransition>NewTransitions;
		for (const Core::FMTaction& action : actions)
			{
			if (action.getname() != "_DEATH")
				{
				NewTransitions.push_back(Core::FMTtransition(action.getname()));
				}
			}
		Core::FMTtransition DEATH_TRANSITION = transitions.back();
		DEATH_TRANSITION.unshrink(themes);
		NewTransitions.push_back(DEATH_TRANSITION);
		for (Core::FMTtransition& transition : transitions)
			{
			if (p_Filters.find(transition.getname()) != p_Filters.end())
			{
				Core::FMTtransition NewTransition(transition.getname());
				//NewTransition.unshrink(themes);
				transition.unshrink(themes);
				for (const auto& element : transition)
				{
					const Core::FMTmask Intersection = p_Filters.at(transition.getname()).second.getintersect(element.first);
					if (!Intersection.isnotthemessubset(element.first,themes))
						{
						const Core::FMTmask NEW_MASK = addNewMask(Intersection);
						NewTransition.push_back(NEW_MASK, element.second);
						}
					
				}
				//NewTransition.update();
				std::vector< Core::FMTtransition>::iterator transitionIterator = std::find_if(NewTransitions.begin(), NewTransitions.end(), Core::FMTtransitioncomparator(p_Filters.at(transition.getname()).first));
				if (transitionIterator == NewTransitions.end())
					{
					_exhandler->raise(Exception::FMTexc::FMTinvalidAandT,
						"Cannot find transition "+ transition.getname(), "FMTmodel::aggregateTransitions", __LINE__, __FILE__, Core::FMTsection::Transition);
					}
				*transitionIterator += NewTransition;
				}
			}
		for (Core::FMTtransition& transition : NewTransitions)
			{
			transition.update();
			}
	settransitions(NewTransitions);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::aggregateTransitions", __LINE__, __FILE__);
		}
	}

void FMTmodel::aggregateOutputs(const std::map<std::string, std::pair<std::string, Core::FMTmask>>& p_Filters,
	std::vector<Core::FMToutput*>& p_Outputs, const std::vector<std::string>& p_ActionOrdering)
{
	try {
		for (Core::FMToutput* output : p_Outputs)
			{
			std::map<std::string, std::map<Core::FMTmask,std::string>>Dominances;
			std::vector<bool>ValidMask(output->getsources().size(), false);
			size_t sourceId = 0;
			for (const Core::FMToutputsource& source : output->getsources())
				{

				if (source.isaction() &&
					p_Filters.find(source.getaction()) != p_Filters.end() &&
					!p_Filters.at(source.getaction()).first.empty())
					{
					
					const Core::FMTmask& MASK = source.getmask();
					const Core::FMTmask INTERSECT_MASK = MASK.getintersect(p_Filters.at(source.getaction()).second);
					//Validate the MASK...
					bool gotValidMask = true;
					size_t themeId = 0;
					while (gotValidMask && themeId < themes.size())
						{
						gotValidMask = INTERSECT_MASK.getsubsetcount(themes[themeId]) != 0;
						++themeId;
						}
					if (gotValidMask)
					{
						ValidMask[sourceId] = true;
						const std::string& ACTION = source.getaction();
						const std::string& AGGREGATE = p_Filters.at(source.getaction()).first;
						if (Dominances.find(AGGREGATE) == Dominances.end())
						{
							Dominances[AGGREGATE] = std::map<Core::FMTmask, std::string>();
							Dominances[AGGREGATE][INTERSECT_MASK] = ACTION;
						}
						else {
							bool gotIntersect = false;
							std::map<Core::FMTmask, std::string>newMap;
							for (const auto& SOURCES : Dominances[AGGREGATE])
							{
								bool removeIt = false;
								if (!SOURCES.first.isnotthemessubset(INTERSECT_MASK, themes))
								{
									const std::vector<std::string>::const_iterator NEW_HIERARCHY = std::find(p_ActionOrdering.begin(), p_ActionOrdering.end(), ACTION);
									const std::vector<std::string>::const_iterator BASE_HIERARCHY = std::find(p_ActionOrdering.begin(), p_ActionOrdering.end(), SOURCES.second);
									if (NEW_HIERARCHY < BASE_HIERARCHY)
									{
										//Dominances[AGGREGATE][MASK] = ACTION;
										newMap[INTERSECT_MASK] = ACTION;
										removeIt = true;
									}
									gotIntersect = true;
								}
								if (!removeIt)
								{
									newMap[SOURCES.first] = SOURCES.second;
								}

							}
							if (!gotIntersect)
							{
								newMap[INTERSECT_MASK] = ACTION;
							}
							Dominances[AGGREGATE] = newMap;

						}
					}

					
					}
				++sourceId;
				}
			//Now you only select the source that is dominant and have exactly the same mask!!
			std::vector<Core::FMToutputsource>NewSources;
			const std::vector<Core::FMToperator> BASE_OPERATORS = output->getopes();
			const std::string& NULL_YIELD = yields.getNullYield();
			std::map<std::string, std::map<Core::FMTmask, std::string>>OutputDominances(Dominances);
			std::map<std::string,Core::FMTmask>excludedByMask;
			for (const auto& mapping : OutputDominances)
			{
				excludedByMask[mapping.first] = Core::FMTmask();
				for (const auto& actionMapping : mapping.second)
					{
						if (excludedByMask[mapping.first].empty())
						{
							excludedByMask[mapping.first] = actionMapping.first;
						}
						else {
							excludedByMask[mapping.first] = excludedByMask[mapping.first].getunion(actionMapping.first);
						}
					
					}
				
			}
			sourceId = 0;
			for (const Core::FMToutputsource& source : output->getsources())
				{
				Core::FMToutputsource NewSource(source);
				if (ValidMask[sourceId])
					{
						const Core::FMTmask& SOURCE_MASK = source.getmask();
						const Core::FMTmask INTERSECT_MASK = SOURCE_MASK.getintersect(p_Filters.at(source.getaction()).second);
						const std::string& ACTION = source.getaction();
						const std::string& AGGREGATE = p_Filters.at(source.getaction()).first;
						if (OutputDominances.at(AGGREGATE).find(INTERSECT_MASK) != OutputDominances.at(AGGREGATE).end() &&
							OutputDominances.at(AGGREGATE).at(INTERSECT_MASK) == ACTION)
						{
							if (!INTERSECT_MASK.isnotthemessubset(p_Filters.at(source.getaction()).second, themes))
							{
								//const Core::FMTmask INTERSECT_MASK = SOURCE_MASK.getintersect(p_Filters.at(source.getaction()).second);
								NewSource.setmask(addNewMask(INTERSECT_MASK));
							}
							else {
								NewSource.setmask(addNewMask(p_Filters.at(source.getaction()).second));
							}
							if (!p_Filters.at(source.getaction()).first.empty())
							{
								NewSource.setaction(p_Filters.at(source.getaction()).first);
							}
							OutputDominances.at(AGGREGATE).erase(INTERSECT_MASK);
						}else if (OutputDominances.find(AGGREGATE)!= OutputDominances.end())
								{
								Core::FMTmask& REST = excludedByMask[AGGREGATE];
								Core::FMTmask MaskCopy(INTERSECT_MASK);
								MaskCopy.setIntersect(REST);
								/*bool gotValidMask = false;
								size_t themeId = 0;
								while (!gotValidMask && themeId < themes.size())
								{
									gotValidMask = MaskCopy.getsubsetcount(themes[themeId]) < INTERSECT_MASK.getsubsetcount(themes[themeId]);
									++themeId;
								}*/
								bool gotNoIntersect = false;
								size_t themeId = 0;
								while (!gotNoIntersect && themeId < themes.size())
								{
									gotNoIntersect = MaskCopy.getsubsetcount(themes[themeId]) < INTERSECT_MASK.getsubsetcount(themes[themeId]);
									++themeId;
								}
								if (gotNoIntersect)
								{
									Core::FMTmask FinalMask(INTERSECT_MASK);
									for (const Core::FMTtheme& THEME : themes)
									{
										if (MaskCopy.getsubsetcount(THEME) < INTERSECT_MASK.getsubsetcount(THEME))
										{
											FinalMask.setExclusiveBits(REST, THEME);
										}
									}
									FinalMask = addNewMask(FinalMask);
									NewSource.setmask(FinalMask);
									REST = REST.getunion(FinalMask);
								}
								else {
									NewSource.setYield(NULL_YIELD);
								}
								NewSource.setaction(AGGREGATE);
								
						}else {
							NewSource.setYield(NULL_YIELD);
							NewSource.setaction(AGGREGATE);	
						}
					}
				NewSources.push_back(NewSource);
				++sourceId;
				}
			output->setsources(NewSources);
			}

	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::aggregateOutputs", __LINE__, __FILE__);
		}

}

Core::FMTmask FMTmodel::addNewMask(const Core::FMTmask& p_incompleteMask)
{
	Core::FMTmask baseMask;
	try {
		std::string NewMask;
		for (Core::FMTtheme& theme : themes)
		{
			NewMask += theme.updateFromMask(p_incompleteMask) + " ";
		}
		NewMask.pop_back();
		baseMask = Core::FMTmask(NewMask, themes);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::addNewMask", __LINE__, __FILE__);
		}
	return baseMask;
}

std::default_random_engine* FMTmodel::getGeneratorPtr() const
	{
	return &m_generator;
	}


void FMTmodel::setdefaultobjects()
	{
	try {
		if (std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator("_DEATH")) == actions.end())
		{
			_exhandler->raise(Exception::FMTexc::FMTundefineddeathaction,
				"_DEATH","FMTmodel::setdefaultobjects", __LINE__, __FILE__, Core::FMTsection::Action);
			actions.push_back(defaultdeathaction(lifespan, themes));
			//actions.back().passinobject(*this);
		}
		if (std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator("_DEATH")) == transitions.end())
		{
			_exhandler->raise(Exception::FMTexc::FMTundefineddeathtransition,
				"_DEATH","FMTmodel::setdefaultobjects", __LINE__, __FILE__, Core::FMTsection::Transition);
			transitions.push_back(defaultdeathtransition(lifespan, themes));
			//transitions.back().passinobject(*this);
		}
		for (Core::FMTaction& action : actions)
		{
			action.update();
		}
		statictransitionthemes = getstatictransitionthemes();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::setdefaultobjects", __LINE__, __FILE__);
		}
	
	}

std::vector<size_t>FMTmodel::getstatictransitionthemes() const
	{
	std::vector<size_t>statics;
	try {
		std::vector<const Core::FMTtheme*>bestthemes;
		for (const Core::FMTtheme& theme : themes)
		{
			bestthemes.push_back(&theme);
		}
		for (const Core::FMTtransition& transition : transitions)
		{
			bestthemes = transition.getstaticthemes(bestthemes);
		}
		for (const Core::FMTtheme* theme : bestthemes)
			{
			std::vector<Core::FMTtheme>::const_iterator basit = std::find(themes.begin(), themes.end(), *theme);
			if (basit!=themes.end())
				{
				statics.push_back(std::distance(themes.cbegin(), basit));
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getstatictransitionthemes", __LINE__, __FILE__);
		}
	return statics;
	}

std::vector<const Core::FMTtheme*>FMTmodel::getstaticpresolvethemes() const
	{
	std::vector<const Core::FMTtheme*>fullstatics;
	for (const Core::FMTtheme& theme : themes)
	{
		fullstatics.push_back(&theme);
	}
	try {
		for (const Core::FMTconstraint& constraint : constraints)
			{
			std::vector<const Core::FMTtheme*>newstatics;
			for (const Core::FMTtheme* theme : locatestaticthemes(constraint))
				{
				if (std::find(fullstatics.begin(), fullstatics.end(), theme) != fullstatics.end())
					{
					newstatics.push_back(theme);
					}
				}
			fullstatics= newstatics;
			}
		std::vector<Core::FMTaction>uactions(actions);
		for (Core::FMTaction& action : uactions)
			{
			action.unshrink(themes);
			for (const auto& actit : action)
				{
				std::vector<const Core::FMTtheme*>newstatics;
				for (const Core::FMTtheme* theme :  actit.first.getstaticthemes(fullstatics))
					{
					if (std::find(fullstatics.begin(), fullstatics.end(), theme) != fullstatics.end())
						{
						newstatics.push_back(theme);
						}
					}
				fullstatics = newstatics;
				}
			}
		Core::FMTyields uyields(yields);
		uyields.unshrink(themes);
		for (auto& yieldit : uyields)
			{
			std::vector<const Core::FMTtheme*>newstatics;
			for (const Core::FMTtheme* theme : yieldit.first.getstaticthemes(fullstatics))
			{
				if (std::find(fullstatics.begin(), fullstatics.end(),theme) != fullstatics.end())
				{
					newstatics.push_back(theme);
				}
			}
			fullstatics = newstatics;
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getstaticpresolvethemes", __LINE__, __FILE__);
		}
	return fullstatics;
	}


FMTmodel::FMTmodel() : Core::FMTobject(), m_generator(),parameters(),area(),themes(),actions(), transitions(),yields(),lifespan(),outputs(), constraints(),name(), statictransitionthemes()
{
	
}


FMTmodel::FMTmodel(FMTmodel&& rhs) : Core::FMTobject(), m_generator(), parameters(), area(), themes(), actions(), transitions(), yields(), lifespan(), outputs(), constraints(), name(), statictransitionthemes()
{
	*this = std::move(rhs);
}

FMTmodel& FMTmodel::operator =(FMTmodel&& rhs)
{
	if (this != &rhs)
	{
		Core::FMTobject::operator = (rhs);
		m_generator = std::move(rhs.m_generator);
		parameters.swap(rhs.parameters);
		area.swap(rhs.area);
		themes.swap(rhs.themes);
		actions.swap(rhs.actions);
		transitions.swap(rhs.transitions);
		yields.swap(rhs.yields);
		lifespan.swap(rhs.lifespan);
		outputs.swap(rhs.outputs);
		constraints.swap(rhs.constraints);
		name = std::move(rhs.name);
		statictransitionthemes.swap(rhs.statictransitionthemes);
		yields.setModel(this);

	}
	return *this;
}

void FMTmodel::setSeed(const int& p_seed)
	{
	m_generator = std::default_random_engine(p_seed);
	}

void FMTmodel::_gutsOfConstructor(const std::vector<Core::FMTactualdevelopment>& p_area)
	{
	setarea(p_area);
	setdefaultobjects();
	cleanactionsntransitions();
	setSeed(getparameter(Models::FMTintmodelparameters::SEED));
	yields.setModel(this);
	}


FMTmodel::FMTmodel(const std::vector<Core::FMTactualdevelopment>& larea, const std::vector<Core::FMTtheme>& lthemes,
	const std::vector<Core::FMTaction>& lactions,
	const std::vector<Core::FMTtransition>& ltransitions, const Core::FMTyields& lyields, const Core::FMTlifespans& llifespan,
	const std::string& lname, const std::vector<Core::FMToutput>& loutputs,std::vector<Core::FMTconstraint> lconstraints,FMTmodelparameters lparameters) :
	Core::FMTobject(), m_generator(), parameters(lparameters),area(), themes(lthemes), actions(lactions), transitions(ltransitions),
	yields(lyields), lifespan(llifespan), outputs(loutputs), constraints(lconstraints), name(lname), statictransitionthemes()
	{
	_gutsOfConstructor(larea);
	}

FMTmodel::FMTmodel(const FMTmodel& rhs):Core::FMTobject(rhs), m_generator(rhs.m_generator),parameters(rhs.parameters),area(rhs.area),themes(rhs.themes),actions(rhs.actions),
		 transitions(rhs.transitions),yields(rhs.yields),lifespan(rhs.lifespan), outputs(rhs.outputs), constraints(rhs.constraints),name(rhs.name),
		statictransitionthemes(rhs.statictransitionthemes)

	{
	yields.setModel(this);
	}

FMTmodel& FMTmodel::operator = (const FMTmodel& rhs)
    {
    if (this!=&rhs)
        {
        Core::FMTobject::operator = (rhs);
		m_generator = rhs.m_generator;
		parameters = rhs.parameters;
        area = rhs.area;
        themes = rhs.themes;
        actions = rhs.actions;
        transitions = rhs.transitions;
        yields = rhs.yields;
        lifespan = rhs.lifespan;
		outputs = rhs.outputs;
		constraints = rhs.constraints;
        name = rhs.name;
		statictransitionthemes = rhs.statictransitionthemes;
		yields.setModel(this);
        }
	
    return *this;
    }
std::vector<Core::FMTactualdevelopment>FMTmodel::getarea(int period,bool beforegrowanddeath) const
    {
    return area;
    }

std::unique_ptr<FMTmodel> FMTmodel::getcopy(int period) const
	{
	return std::unique_ptr<FMTmodel>(new FMTmodel(*this));
	}

void FMTmodel::cleanactionsntransitions()
	{
	try {
		std::vector<Core::FMTaction>newactions;
		newactions.reserve(actions.size());
		std::vector<Core::FMTtransition>newtransitions;
		newtransitions.reserve(transitions.size());
		//sort(actions.begin(), actions.end());
		for (size_t id = 0; id < actions.size(); ++id)
		{
			if (!actions[id].empty())
			{
				const std::vector<Core::FMTtransition>::iterator trn_it = std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator(actions[id].getname()));
				if (trn_it != transitions.end() && !trn_it->empty())
				{
					newactions.push_back(actions[id]);
					newtransitions.push_back(*trn_it);
				}
			}
		}
		actions = newactions;
		transitions = newtransitions;
		actions.shrink_to_fit();
		transitions.shrink_to_fit();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::cleanactionsntransitions", __LINE__, __FILE__);
		}
	}


bool FMTmodel::useactionserie() const
{
	bool gotseries = false;
	try {
		for (const Core::FMTaction& action : actions)
		{
			if (action.ispartofaserie())
			{
				gotseries = true;
				break;
			}
		}
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::useactionseries", __LINE__, __FILE__);
	}

	return gotseries;
}

size_t FMTmodel::getseriesmaxsize() const
{
	size_t maxsize = 0;
	try {
		if (useactionserie())
			{
			for (const Core::FMTaction& action : actions)
				{
				maxsize = std::max(maxsize, action.getlargestseriesize());
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getseriesmaxsize", __LINE__, __FILE__);
		}
	return maxsize;
}







Core::FMTaction FMTmodel::defaultdeathaction(const Core::FMTlifespans& llifespan,
										const std::vector<Core::FMTtheme>& lthemes)
	{
		const std::string actionname = "_DEATH";
		const bool lock = false;
		const bool reset = true;
		Core::FMTaction death_action(actionname, lock, reset);
		for (const auto& intobject : llifespan)
		{
			const std::string mask(intobject.first);
			const Core::FMTmask amask(mask, lthemes);
			Core::FMTspec specifier;
			specifier.addbounds(Core::FMTagebounds(Core::FMTsection::Action, std::numeric_limits<int>::max(), intobject.second));
			death_action.push_back(amask, specifier);
		}
		const std::string GCBMaggregate = "~GCBM:" + std::to_string(FMTGCBMDEATHID) + ":Stand Replacing Natural Succession";
		death_action.push_aggregate(GCBMaggregate);
		death_action.shrink();
	return death_action;
	}
Core::FMTtransition FMTmodel::defaultdeathtransition(const Core::FMTlifespans& llifespan,
											const std::vector<Core::FMTtheme>& lthemes)
	{
	const std::string transitionname = "_DEATH";
	Core::FMTtransition death_Transition(transitionname);
	const double target_proportion = 100;
	for (const auto& lfobject : llifespan)
		{
		const std::string mask(lfobject.first);
		const Core::FMTmask amask(mask, lthemes);
		Core::FMTfork fork;
		Core::FMTtransitionmask trmask(mask, lthemes, target_proportion);
		fork.add(trmask);
		death_Transition.push_back(amask, fork);
		}
	death_Transition.shrink();
	return death_Transition;
	}

void FMTmodel::setparallellogger(Logging::FMTlogger& logger)
	{
	try {
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			"FMTmodel::setparallellogger", __LINE__, __FILE__);
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTmodel::setparallellogger", __LINE__, __FILE__);
		}
	}


bool FMTmodel::isoptimal() const
{
	return false;
}

double FMTmodel::getobjectivevalue() const
{
	try {
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			"FMTmodel::getobjectivevalue", __LINE__, __FILE__);
	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setparallellogger", __LINE__, __FILE__);
	}
	return 0.0;
}



void FMTmodel::addoutput(const std::string& name,
	const std::string& maskstring, Core::FMTotar outputtarget,
	std::string action, std::string yield, std::string description, int targettheme)
	{
	try {
		std::vector<Core::FMToutputsource>sources;
		sources.push_back(Core::FMToutputsource(Core::FMTspec(), Core::FMTmask(maskstring, themes), outputtarget, yield, action,static_cast<int>(outputs.size()),targettheme));
		std::vector<Core::FMToperator>operators;
		outputs.push_back(Core::FMToutput(name, description,"", sources, operators));
		outputs.shrink_to_fit();
	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::addoutput", __LINE__, __FILE__);
	}

	}


void FMTmodel::addyieldhandlersfromptr(const std::vector<std::unique_ptr<Core::FMTyieldhandler>>& yieldhandlers)
{
	try {
		yields.unshrink(themes);
		for (const std::unique_ptr<Core::FMTyieldhandler>& yldhandler : yieldhandlers)
		{
			yields.push_back(yldhandler->getmask(),yldhandler);
		}
		yields.update();
	}catch(...){
			_exhandler->printexceptions("", "FMTmodel::addyieldhandlersfromptr", __LINE__, __FILE__);
	}
	
	
}


void FMTmodel::setconstraints(const std::vector<Core::FMTconstraint>& lconstraint)
	{
	try {
		constraints = lconstraint;
		constraints.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setconstraints", __LINE__, __FILE__);
	}
	}

bool  FMTmodel::operator == (const FMTmodel& rhs) const
	{
	return (name == rhs.name &&
		area == rhs.area &&
		themes == rhs.themes &&
		actions == rhs.actions &&
		transitions == rhs.transitions &&
		yields == rhs.yields &&
		lifespan == rhs.lifespan &&
		outputs == rhs.outputs &&
		constraints == rhs.constraints);
	}

bool FMTmodel::operator < (const FMTmodel& rhs) const
	{
	//strict ordering
	if (name < rhs.name)
		return true;
	if (rhs.name < name)
		return false;
	//To do
	/*if (area < rhs.area)
		return true;
	if (rhs.area < area)
		return false;
	if (themes < rhs.themes)
		return true;
	if (rhs.themes < themes)
		return false;
	if (actions < rhs.actions)
		return true;
	if (rhs.actions < actions)
		return false;
	if (transitions < rhs.transitions)
		return true;
	if (rhs.transitions < transitions)
		return false;
	if (yields < rhs.yields)
		return true;
	if (rhs.yields < yields)
		return false;
	if (lifespan < rhs.lifespan)
		return true;
	if (rhs.lifespan < lifespan)
		return false;
	if (outputs < rhs.outputs)
		return true;
	if (rhs.outputs < outputs)
		return false;
	if (constraints < rhs.constraints)
		return true;
	if (rhs.constraints < constraints)
		return false;*/
	return false;
	}



void FMTmodel::setarea(const std::vector<Core::FMTactualdevelopment>& ldevs)
    {
	try {
		std::set<Core::FMTactualdevelopment,std::less<Core::FMTdevelopment>>sortedArea;
		for (Core::FMTactualdevelopment adev : ldevs)
		{
			if (adev.getlock() > 0)
				{
				adev = adev.reducelocktodeath(lifespan);
				}
			std::pair<std::set<Core::FMTactualdevelopment>::iterator, bool> inserted = sortedArea.insert(adev);
			sortedArea.insert(adev);
			if (!inserted.second)
				{
				Core::FMTactualdevelopment* theDev = const_cast<Core::FMTactualdevelopment*>(&*inserted.first);
				theDev->setarea(theDev->getarea() + adev.getarea());
				}
		}
		std::vector<Core::FMTactualdevelopment>newArea(sortedArea.begin(), sortedArea.end());
		area.swap(newArea);
		
		/*area.clear();
		area.reserve(ldevs.size());
		for(Core::FMTactualdevelopment adev: ldevs)
		{
			if (adev.getlock()>0)
				{
				adev = adev.reducelocktodeath(lifespan);
				}
			std::vector<Core::FMTactualdevelopment>::iterator devit = std::find_if(area.begin(), area.end(), Core::FMTactualdevelopmentcomparator(&adev));
			if(devit != area.end())
			{
				devit->setarea(devit->getarea()+adev.getarea());
			}else{
				area.push_back(adev);
			}	
		}
		std::sort(area.begin(), area.end());*/
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setarea", __LINE__, __FILE__);
	}


    }
void FMTmodel::setthemes(const std::vector<Core::FMTtheme>& lthemes)
    {
	try {
		themes = lthemes;
		themes.shrink_to_fit();
		//After theme change every masks needs to be reevaluated?.
		statictransitionthemes = getstatictransitionthemes();
		statictransitionthemes.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setthemes", __LINE__, __FILE__);
	}


    }
void FMTmodel::setactions(const std::vector<Core::FMTaction>& lactions)
    {
	try {
		std::vector<Core::FMTtransition>newtransitions;
		std::vector<Core::FMTaction>newbaseactions;
		for (const Core::FMTaction& action : lactions)
		{
			std::vector<Core::FMTtransition>::const_iterator trn_iterator = std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator(action.getname()));
			if (trn_iterator != transitions.end())
			{
				newtransitions.push_back(*trn_iterator);
				newbaseactions.push_back(action);
			}
		}
		actions = newbaseactions;
		transitions = newtransitions;
		for (Core::FMTaction& action : actions)
		{
			action.update();
		}
		this->setdefaultobjects();
		actions.shrink_to_fit();
		transitions.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setactions", __LINE__, __FILE__);
	}


    }
void FMTmodel::settransitions(const std::vector<Core::FMTtransition>& ltransitions)
    {
	try {
		transitions = ltransitions;
		for (Core::FMTtransition& transition : transitions)
		{
			transition.update();
		}
		this->setdefaultobjects();
		statictransitionthemes = getstatictransitionthemes();
		statictransitionthemes.shrink_to_fit();
		transitions.shrink_to_fit();
	}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodel::settransitions", __LINE__, __FILE__);
		}

    }
void FMTmodel::setyields(const Core::FMTyields& lylds)
    {
	try {
		yields = lylds;
		yields.update();
		yields.setModel(this);
	}catch (...)
		{
			_exhandler->printexceptions("", "FMTmodel::setyields", __LINE__, __FILE__);
		}
    }
void FMTmodel::setlifespan(const Core::FMTlifespans& llifespan)
    {
	try {
		lifespan = llifespan;
		lifespan.update();
		this->setdefaultobjects();
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setlifespan", __LINE__, __FILE__);
	}

    }

void FMTmodel::setname(const std::string& newname)
	{
	name = newname;
	}

void FMTmodel::setoutputs(const std::vector<Core::FMToutput>& newoutputs)
	{
	try {
		outputs = newoutputs;
		outputs.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::setoutputs", __LINE__, __FILE__);
	}

	}


std::vector<const Core::FMTtheme*> FMTmodel::locatestaticthemes(const Core::FMToutput& output, bool ignoreoutputvariables) const
{
	std::vector<const Core::FMTtheme*> bestthemes;
	try {
		const std::vector<const Core::FMTtheme*>transitionstatic = locatestatictransitionsthemes();
		for (const Core::FMTtheme* theme : output.getstaticthemes(themes, yields, ignoreoutputvariables))
			{
			if (std::find(transitionstatic.begin(), transitionstatic.end(), theme) != transitionstatic.end())
				{
				bestthemes.push_back(theme);
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::locatestaticthemes", __LINE__, __FILE__);
		}
	return bestthemes;
}

std::vector<const Core::FMTtheme*>FMTmodel::locatestatictransitionsthemes() const
{
	std::vector<const Core::FMTtheme*>bestthemes;
	try {
		for (const size_t& location : statictransitionthemes)
		{
			bestthemes.push_back(&themes.at(location));
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::locatestatictransitionsthemes", __LINE__, __FILE__);
	}
	return bestthemes;

}

std::vector<const Core::FMTtheme*>FMTmodel::locatenodestaticthemes(const Core::FMToutputnode& node,
	bool ignoreoutputvariables,
	std::vector<const Core::FMTtheme* > basethemes) const
{
	std::vector<const Core::FMTtheme*>statics;
	if (!basethemes.empty())
		{
		statics = basethemes;
	}
	else {
		for (const Core::FMTtheme& theme : themes)
		{
			statics.push_back(&theme);
		}
	}
	try {
		std::vector<std::string>yieldstolookat;
		if (node.source.isvariable())
		{
			if (!ignoreoutputvariables)
			{
				statics = node.source.getmask().getstaticthemes(statics);
			}
			const std::string yieldvalue = node.source.getyield();
			for (const std::string& yldbound : node.source.getylds())
			{
				if (yields.isyld(yldbound))
				{
					yieldstolookat.push_back(yldbound);
				}
			}
			if (!yieldvalue.empty())
			{
				yieldstolookat.push_back(yieldvalue);
			}
		}
		std::vector<std::pair<Core::FMTmask,std::unique_ptr<Core::FMTyieldhandler>>>::const_iterator handlerit = yields.begin();
		while (handlerit != yields.end() && !yieldstolookat.empty())
		{
			std::vector<std::string>::const_iterator yieldit = yieldstolookat.begin();
			while (yieldit != yieldstolookat.end() && !handlerit->second->containsyield(*yieldit))
			{
				++yieldit;
			}
			if (yieldit != yieldstolookat.end())
			{
				statics = Core::FMTmask(std::string(handlerit->first), themes).getstaticthemes(statics);
				yieldstolookat.erase(yieldit);
			}
			++handlerit;
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::locatenodestaticthemes", __LINE__, __FILE__);
	}
	return statics;


}



std::vector<const Core::FMTtheme*> FMTmodel::locatestaticthemes(const Core::FMToutputnode& node, bool ignoreoutputvariables) const
{
	std::vector<const Core::FMTtheme*>statics;
	try {
		statics = locatestatictransitionsthemes();
		statics = locatenodestaticthemes(node, ignoreoutputvariables, statics);
		
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::locatestaticthemes", __LINE__, __FILE__);
	}
	return statics;

}

std::vector<const Core::FMTtheme*> FMTmodel::locatedynamicthemes(const Core::FMToutput& output, bool ignoreoutputvariables) const
{
	std::vector<const Core::FMTtheme*>dynamicthemes;
	try {
		const std::vector<const Core::FMTtheme*>staticthemes = locatestaticthemes(output, ignoreoutputvariables);
		for (const Core::FMTtheme& theme : themes)
			{
			if (std::find(staticthemes.begin(), staticthemes.end(), &theme)==staticthemes.end())
				{
				dynamicthemes.push_back(&theme);
				}
			}
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::locatedynamicthemes", __LINE__, __FILE__);
	}
	return dynamicthemes;
}


Core::FMTmask FMTmodel::getdynamicmask(const Core::FMToutput& output, bool ignoreoutputvariables) const
	{
	Core::FMTmask selection;
	try {
		const std::vector<const Core::FMTtheme*>staticcthemes = locatestaticthemes(output, ignoreoutputvariables);
		std::string basename;
		for (const Core::FMTtheme& theme : themes)
			{
			
			basename += "? ";
			}
		
		basename.pop_back();
		const Core::FMTmask submask(basename,themes);
		 boost::dynamic_bitset<uint8_t>bits = submask.getbitsetreference();
		for (const Core::FMTtheme* theme : staticcthemes)
			{
			const size_t start = static_cast<size_t>(theme->getstart());
			for (size_t bitid = start; bitid < (theme->size() + start); ++bitid)
				{
				bits[bitid] = false;
				}
			}
		selection = Core::FMTmask(basename, bits);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getdynamicmask", __LINE__, __FILE__);
		}
	return selection;
	}

Core::FMTmask FMTmodel::getdynamicmask(const Core::FMToutputnode& node, bool ignoreoutputvariables) const
{
	Core::FMTmask selection;
	try {
		std::vector<const Core::FMTtheme*>staticcthemes = locatestatictransitionsthemes();
		staticcthemes = locatenodestaticthemes(node, ignoreoutputvariables, staticcthemes);
		
		std::string basename;
		for (const Core::FMTtheme& theme : themes)
		{

			basename += "? ";
		}

		basename.pop_back();
		
		
		const Core::FMTmask submask(basename, themes);
		 boost::dynamic_bitset<uint8_t>bits = submask.getbitsetreference();
		for (const Core::FMTtheme* theme : staticcthemes)
		{
			
			const size_t start = static_cast<size_t>(theme->getstart());
			for (size_t bitid = start; bitid < (theme->size() + start); ++bitid)
			{
				bits[bitid] = false;
			}
		}
		selection = Core::FMTmask(basename, bits);
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getdynamicmask", __LINE__, __FILE__);
	}
	return selection;

}

bool FMTmodel::isstaticnode(const Core::FMToutputnode& node, double ratioofset) const
{
	try {
		if (node.source.isinventory()&&!node.source.isaction())
		{
			for (const size_t& staticid : statictransitionthemes)
			{
				const double nvalues = static_cast<double>(node.source.getmask().getsubsetcount(themes.at(staticid)));
				const double themesize = static_cast<double>(themes.at(staticid).size());
				if ((nvalues/themesize)<=ratioofset)
				{
					return true;
				}

			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::isstaticnode", __LINE__, __FILE__);
		}
	return false;
}

Core::FMTmask FMTmodel::getstaticmask(const Core::FMToutputnode& node, bool ignoreoutputvariables) const
{
	Core::FMTmask selection;
	try {
		const Core::FMTmask dymask =this->getdynamicmask(node, ignoreoutputvariables);
		const Core::FMTmask intersection = node.source.getmask();
		selection = dymask.getunion(intersection);
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getstaticmask", __LINE__, __FILE__);
	}
	return selection;
}


void FMTmodel::validatelistspec(const Core::FMTspec& specifier) const
	{
	try {
		for (const std::string& yldname : specifier.getylds())
		{
			if (!yields.isyld(yldname))
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_yield,yldname,
					"FMTmodel::validatelistspec", __LINE__, __FILE__);
			}
		}
	}catch (...)
		{
			_exhandler->raisefromcatch("","FMTmodel::validatelistspec", __LINE__, __FILE__);
		}
	}


bool FMTmodel::isvalid()
    {
	try {
		//this->setsection(Core::FMTsection::Landscape);
		for (const Core::FMTtheme& theme : themes)
		{
			if (theme.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTempty_theme,
					"for theme id: " + std::to_string(theme.getid()),
					"FMTmodel::isvalid", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
		}
		//this->setsection(Core::FMTsection::Area);
		for (const Core::FMTactualdevelopment& developement : area)
		{
			std::string name = std::string(developement.getmask());
			Core::FMTtheme::validate(themes, name);
		}
		//this->setsection(Core::FMTsection::Yield);
		this->validatelistmasks(yields);

		//this->setsection(Core::FMTsection::Lifespan);
		this->validatelistmasks(lifespan);

		//this->setsection(Core::FMTsection::Action);
		for (const Core::FMTaction& action : actions)
		{
			this->validatelistmasks(action);
			for (const auto& specobject : action)
			{
				validatelistspec(specobject.second);
			}
		}
		//this->setsection(Core::FMTsection::Transition);
		for (const Core::FMTtransition& transition : transitions)
		{
			this->validatelistmasks(transition);
			for (const auto& specobject : transition)
			{
				validatelistspec(specobject.second);
			}
		}
		if (actions.size() != transitions.size())
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalidAandT, "Model: " + name,
				"FMTmodel::isvalid",__LINE__, __FILE__);
		}
		for (size_t id = 0; id < actions.size(); ++id)
		{
			if (actions[id].getname() != transitions[id].getname())
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_action,
					"Model: " + name + " " + actions[id].getname(),
					"FMTmodel::isvalid", __LINE__, __FILE__);
			}
		}
		//this->setsection(Core::FMTsection::Outputs);
		for (const Core::FMToutput& output : outputs)
		{
			//Need a validate output function
			for (const Core::FMToutputsource& source : output.getsources())
			{
				if (source.isvariable())
				{
					std::string name = std::string(source.getmask());
					Core::FMTtheme::validate(themes, name, "for output " + output.getname());
					const std::string actionname = source.getaction();
					if (!actionname.empty())//need to check the targeted action!
					{

					}
				}
				validatelistspec(source);
			}
		}
		//this->setsection(Core::FMTsection::Empty);
	}catch (...)
	{
		_exhandler->printexceptions("", "FMTmodel::isvalid", __LINE__, __FILE__);
	}

    return true;
    }

void  FMTmodel::clearactionscache()
	{
		for (auto& action:actions)
		{
			action.clearcache();
		}
	}

void  FMTmodel::clearyieldcache()
	{
		yields.clearcache();
	}

void  FMTmodel::cleartransitioncache()
	{
		for (auto& transition:transitions)
		{
			transition.clearcache();
		}
	}

void FMTmodel::clearcache()
	{
		cleartransitioncache();
		clearyieldcache();
		clearactionscache();
	}

Core::FMTmask FMTmodel::getbasemask(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
	{
	Core::FMTmask basemask( boost::dynamic_bitset<uint8_t>(area.begin()->getmask().size(), false));
	try {
		optionaldevelopments.insert(optionaldevelopments.end(), area.begin(), area.end());
		Core::FMTmask areamask( boost::dynamic_bitset<uint8_t>(area.begin()->getmask().size(), false));
		for (const Core::FMTactualdevelopment& developement : optionaldevelopments)
			{
			areamask = areamask.getunion(developement.getmask());
			}
		size_t trid = 0;
		std::vector<bool>jumptransitions;
		for (const Core::FMTtransition& transition : transitions)
		{
			if (transition.size() == 1 &&
				actions.at(trid).size()==1&&
				transition.begin()->second.getmasktrans().size()==1&&
				std::string(actions.at(trid).begin()->first)==std::string(transition.begin()->first)&&
				std::string(transition.begin()->first)==std::string(transition.begin()->second.getmasktrans().begin()->getmask())&&
				!transition.begin()->second.getmasktrans().begin()->getmask().isSubsetOf(areamask))//scrap weird thing
				{
				jumptransitions.push_back(true);
			}else {
				for (const auto& transitionobject : transition)
				{
					const Core::FMTmask source(std::string(transitionobject.first),themes);
					for (const Core::FMTtransitionmask& fork : transitionobject.second.getmasktrans())
					{
						const Core::FMTmask maskwithoutaggregates = fork.getmask().removeaggregates(themes);
						basemask = basemask.getunion(maskwithoutaggregates);
					}
				}
				jumptransitions.push_back(false);
			}
			++trid;
		}
		if (!getparameter(FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES))
		{
			basemask = basemask.getunion(areamask);
		}else {
			trid = 0;
			for (const Core::FMTtransition& transition : transitions)
			{
				if (!jumptransitions.at(trid))
				{
					for (const auto& transitionobject : transition)
					{
						const Core::FMTmask source(std::string(transitionobject.first), themes);
						basemask = basemask.getunion(source.removeaggregates(themes, true));
					}
				}
				++trid;
			}
			size_t acid = 0;
			for (const Core::FMTaction& action : actions)
			{
				if (!jumptransitions.at(acid))
				{
					for (const auto& actionobject : action)
					{
						const Core::FMTmask opq(std::string(actionobject.first), themes);
						basemask = basemask.getunion(opq.removeaggregates(themes, true));
					}
				}
				++acid;
			}
			for (const auto& yieldobject : yields)
			{
				const Core::FMTmask source(std::string(yieldobject.first), themes);
				basemask = basemask.getunion(source.removeaggregates(themes, true));
			}
			for (const auto& lifespanobject : lifespan)
			{
				const Core::FMTmask source(std::string(lifespanobject.first), themes);
				basemask = basemask.getunion(source.removeaggregates(themes, true));
			}
			for (const Core::FMTconstraint& constraint : constraints)
			{
				for (const Core::FMToutputsource& source : constraint.getsources())
				{
					if (source.isvariable())
					{
						basemask = basemask.getunion(source.getmask().removeaggregates(themes, true));
					}
				}
			}
			 boost::dynamic_bitset<uint8_t>bits(basemask.size(),false);
			const boost::dynamic_bitset<uint8_t>& areamaskref = areamask.getbitsetreference();
			const boost::dynamic_bitset<uint8_t>& basemaskref = basemask.getbitsetreference();
			for (const Core::FMTtheme& theme : themes)
				{
				const size_t start = static_cast<size_t>(theme.getstart());
				if (basemask.getsubsetcount(theme) == 0)
					{ 
					const size_t areacount = areamask.getsubsetcount(theme);
					size_t biton = (theme.size() + start) - 1;
					if (areacount>=1)
						{
						while (!areamaskref[biton])
							{
							--biton;
							}
						}
					bits[biton] = true;
				}else {
					for (size_t bitid = start; bitid < (theme.size() + start); ++bitid)
						{
						bits[bitid] = (areamaskref[bitid]||basemaskref[bitid]);
						}
					}
				}
			basemask = basemask.getunion(Core::FMTmask(bits));
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::getbasemask", __LINE__, __FILE__);
		}
	return basemask;
	}

Core::FMTmaskfilter FMTmodel::getpostsolvefilter(const std::vector<Core::FMTtheme>& originalthemes,const Core::FMTmask& devmask) const
{
	try {
		const Core::FMTmask presolvemask = getselectedmask(originalthemes);
		const Core::FMTmask emptythemes = devmask.getpostsolvemask(presolvemask, originalthemes);
		return Core::FMTmaskfilter(presolvemask,emptythemes);
	}catch (...)
	{
		_exhandler->printexceptions("for " + name, "FMTmodel::getpostsolvefilter", __LINE__, __FILE__);
	}
	return Core::FMTmaskfilter();
}

Core::FMTmaskfilter FMTmodel::getpresolvefilter(const std::vector<Core::FMTtheme>& originalthemes) const
{
	try {
		const Core::FMTmask presolvemask = getselectedmask(originalthemes);
		return Core::FMTmaskfilter(presolvemask, presolvemask);
	}
	catch (...)
	{
		_exhandler->printexceptions("for " + name, "FMTmodel::getpresolvefilter", __LINE__, __FILE__);
	}
	return Core::FMTmaskfilter();
}

Core::FMTmask FMTmodel::getselectedmask(const std::vector<Core::FMTtheme>& originalthemes) const
	{
	Core::FMTmask newmask;
	size_t newmasksize = 0;
	for (const Core::FMTtheme& theme : originalthemes)
		{
		newmasksize += theme.size();
		}
	//const boost::dynamic_bitset<uint8_t>& basewithoutpresolve = basedevelopment.getmask().getbitsetreference();
	 boost::dynamic_bitset<uint8_t>selection(newmasksize, false);
	// boost::dynamic_bitset<uint8_t> selection(basewithoutpresolve);
	try {
		size_t bitselection = 0;
		size_t presolvedthemeid = 0;
		size_t themeid = 0;
		while (presolvedthemeid<themes.size()&&themeid< originalthemes.size())
			{
			const Core::FMTtheme& originaltheme = originalthemes.at(themeid);
			//const std::map<std::string, std::string> prsolvedvalues = themes.at(presolvedthemeid).getvaluenames();
			const std::vector<std::string>& prsolvedvalues = themes.at(presolvedthemeid).getbaseattributes();
			size_t foundcount = 0;
			std::vector<bool>themebits(originaltheme.size(),false);
			size_t bitid = 0;
			for (const std::string& themevalues : originaltheme.getbaseattributes())
				{
				if (std::find(prsolvedvalues.begin(), prsolvedvalues.end(), themevalues)!= prsolvedvalues.end()/*prsolvedvalues.find(themevalues.first) != prsolvedvalues.end()*/)
					{
					themebits[bitid] = true;
					++foundcount;
					}
				++bitid;
				
				}
			if (foundcount == prsolvedvalues.size())
				{
				for (const bool& bitvalue : themebits)
					{
					selection[bitselection] = bitvalue;
					++bitselection;
					}
				++presolvedthemeid;
			}else {
				/*for (size_t noloc = bitselection; noloc<(bitselection+themebits.size()); ++noloc)
					{
					selection[noloc] = basewithoutpresolve[noloc];
					}*/
				bitselection += themebits.size();
				}
			++themeid;
			}
		//tail
		/*for (size_t noloc = bitselection; noloc < basewithoutpresolve.size(); ++noloc)
			{
			selection[noloc] = basewithoutpresolve[noloc];
			}*/
		newmask = Core::FMTmask(selection);
		/*for (const Core::FMTtheme& theme : originalthemes)
		{
			*_logger << "Count " << newmask.getsubsetcount(theme) << "\n";
			if (newmask.getsubsetcount(theme) == 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
					"Non valid base presolve mask for model: " + name + " for theme id: " + std::to_string(theme.getid()),
					"FMTmodel::getselectedmask", __LINE__, __FILE__);
			}
		}*/


	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::getselectedmask", __LINE__, __FILE__);
		}
	return newmask;
	}

FMTmodel FMTmodel::basepresolve() const
{
	std::unique_ptr<FMTmodel>mdlptr;
	try {
		mdlptr = presolve(area);

	}catch (...)
		{
		_exhandler->printexceptions("for " + name, "FMTmodel::basepresolve", __LINE__, __FILE__);
		}
	return *mdlptr;
}


std::unique_ptr<FMTmodel> FMTmodel::presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
	{
	std::unique_ptr<FMTmodel>presolvedModel(new FMTmodel());
	int presolvepass = getparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS);
	try {
		_logger->logwithlevel("Presolving " + getname() + "\n", 1);
		presolvedModel->setname(getname());
		presolvedModel->parameters = parameters;
		Core::FMTmaskfilter oldpresolvefilter(getbasemask(optionaldevelopments));
		presolvedModel->themes = themes;
		presolvedModel->area = area;
		presolvedModel->actions = actions;
		std::vector<bool>validActions(presolvedModel->actions.size(), true);
		presolvedModel->transitions = transitions;
		std::vector<bool>validTransitions(presolvedModel->transitions.size(), true);
		presolvedModel->yields = yields;
		presolvedModel->lifespan = lifespan;
		presolvedModel->outputs = outputs;
		size_t originalsize = themes.size() + area.size() + actions.size() + transitions.size() + lifespan.size() + outputs.size()+constraints.size()+yields.size();
		for (const Core::FMTaction& action : actions)
			{
			originalsize += action.size();
			}
		for (const Core::FMTtransition& transition : transitions)
			{
			originalsize += transition.size();
			}
		size_t newsize = 0;
		size_t oldsize = originalsize;
		bool didonepass = false;

		std::vector<int>constraintsIds;
		constraintsIds.reserve(constraints.size());
		int constraintid = 0;
		for (const Core::FMTconstraint& constraint : constraints)
			{
			constraintsIds.push_back(constraintid);
			++constraintid;
			}
		presolvedModel->constraints = constraints;
		std::vector<bool>validConstraints(presolvedModel->constraints.size(), true);
		while (presolvepass > 0 && newsize<oldsize)
		{
			std::vector<Core::FMTtheme>newthemes;
			newthemes.reserve(themes.size());
			std::vector<Core::FMTactualdevelopment>newarea;
			newarea.reserve(area.size());
			//std::vector<Core::FMTaction>newactions;
			//newactions.reserve(actions.size());
			//std::vector<Core::FMTtransition>newtransitions;
			//newtransitions.reserve(transitions.size());
			Core::FMTyields newyields;
			newyields.reserve(yields);
			Core::FMTlifespans newlifespans;
			newlifespans.reserve(lifespan);
			std::vector<Core::FMToutput>newoutputs;
			newoutputs.reserve(outputs.size());
			//std::vector<Core::FMTconstraint>newconstraints;
			//newconstraints.reserve(constraints.size());
			//Presolved data
			Core::FMTmaskfilter newfilter(oldpresolvefilter);
			if (didonepass)
			{
				newfilter = newfilter.presolve(presolvedModel->themes);
				oldsize = newsize;
			}
			newsize = 0;
			//Core::FMTmask selectedattributes; //selected attribute keeps the binaries used by the new attribute selection.
			//Checkout to reduce the themes complexity
			size_t themeid = 0;
			size_t themestart = 0;
			std::set<int> keptthemeid;
			int oldthemeid = 0;
			for (const Core::FMTtheme& theme : presolvedModel->themes)
			{
				Core::FMTtheme PresolvedTheme = theme.presolve(newfilter, themeid, themestart);
				if (!PresolvedTheme.empty())
				{
					keptthemeid.insert(oldthemeid);
					newthemes.push_back(PresolvedTheme);
				}
				++oldthemeid;
			}
			newsize += newthemes.size();
			if (!newfilter.emptyflipped())
			{
				newarea.reserve(presolvedModel->area.size());
				boost::unordered_map<Core::FMTmask,Core::FMTmask>topresolve;
				topresolve.reserve(presolvedModel->area.size());
				for (const Core::FMTactualdevelopment& development : presolvedModel->area)
				{
					const Core::FMTmask& devmask = development.getmask();
					Core::FMTactualdevelopment newdev(development);
					boost::unordered_map<Core::FMTmask, Core::FMTmask>::const_iterator mskit = topresolve.find(devmask);
					if (mskit != topresolve.end())
					{
						newdev.setmask(mskit->second);
					}
					else {
						const Core::FMTmask presolvedmask = devmask.presolve(newfilter, newthemes);
						topresolve[devmask] = presolvedmask;
						newdev.setmask(presolvedmask);
					}
					
					if (getparameter(FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES))
					{
						std::vector<Core::FMTactualdevelopment>::iterator devit = std::find_if(newarea.begin(), newarea.end(), Core::FMTactualdevelopmentcomparator(&newdev));
						if (devit != newarea.end())
						{
							devit->setarea(devit->getarea() + newdev.getarea());
						}
						else {
							newarea.push_back(newdev);
						}
					}else {
						newarea.push_back(newdev);
						}
					}
			}else {
				newarea = presolvedModel->area;
			}
			newsize += newarea.size();
			//reduce the number of actions and presolve the actions
			const std::vector<const Core::FMTtheme*>maskthemes = newfilter.getselectedthemes(presolvedModel->themes);
			size_t actionIds = 0;
			for (Core::FMTaction& PresolvedAction : presolvedModel->actions)
			{
				
				if (validActions[actionIds])
				{
					validActions[actionIds] = false;
					const Core::FMTmask testedmask = PresolvedAction.getunion(presolvedModel->themes);
					if (newfilter.canpresolve(testedmask, maskthemes) && !PresolvedAction.notUse())
					{
						PresolvedAction.presolveRef(newfilter, presolvedModel->themes, newthemes, !didonepass);
						validActions[actionIds] = true;
						newsize += PresolvedAction.size();
						newsize += 1;
					}
					
				}
			++actionIds;
			}
			//newsize += newactions.size();
			//std::cin.get();
			//reduce the number of transitions and presolve the transitions
			size_t transitionIds = 0;
			for (Core::FMTtransition& presolvedTransition : presolvedModel->transitions)
			{
				if (validTransitions[transitionIds])
				{
					validTransitions[transitionIds] = false;
					std::vector<Core::FMTaction>::const_iterator actionIt = std::find_if(presolvedModel->actions.begin(), presolvedModel->actions.end(), Core::FMTactioncomparator(presolvedTransition.getname()));
					const size_t ACTION_LOCATION = std::distance(presolvedModel->actions.cbegin(), actionIt);
					if (actionIt != presolvedModel->actions.end() && validActions[ACTION_LOCATION])
					{
						presolvedTransition.presolveRef(newfilter, presolvedModel->themes, newthemes, !didonepass);
						validTransitions[transitionIds] = true;
						newsize += presolvedTransition.size();
						newsize += 1;
					}
				}
				
				++transitionIds;
			}
			//newsize += newtransitions.size();
			//Presolve yields
			newyields = presolvedModel->yields.presolve(newfilter, presolvedModel->themes, newthemes);
			//Presolve lifespan data
			newlifespans = presolvedModel->lifespan.presolve(newfilter, presolvedModel->themes, newthemes,!didonepass);
			newsize += newlifespans.size();
			//Outputs and data
			std::set<int> keptoutputid;
			int oloutputdid=0;
			for (const Core::FMToutput& output : presolvedModel->outputs)
			{
				Core::FMToutput PresolvedOutput = output.presolve(newfilter, presolvedModel->themes, maskthemes,
																newthemes, presolvedModel->actions, validActions,
																presolvedModel->yields);
				if(!PresolvedOutput.empty())
				{
					keptoutputid.insert(oloutputdid);
					newoutputs.push_back(PresolvedOutput);
				}
				oloutputdid+=1;
			}
			newsize += newoutputs.size();
			for (Core::FMToutput& output : newoutputs)
			{
				output.changesourcesid(keptoutputid,keptthemeid);
			}
			//Constraints and data
			//Add feature to automatically interpret the output[0] as constant in sources
			//std::set<int>newconstraintsids;

			std::vector<int>::const_iterator oriit = constraintsIds.begin();
			std::vector<int>newConstraintsIds;
			newConstraintsIds.reserve(presolvedModel->constraints.size());
			size_t constraintId = 0;
			for (Core::FMTconstraint& presolvedConstraint : presolvedModel->constraints)
			{
				const int originalid = *oriit;
				if (validConstraints[constraintId])
				{
					validConstraints[constraintId] = false;
					presolvedConstraint.presolveRef(newfilter, presolvedModel->themes, maskthemes, newthemes, presolvedModel->actions,
													validActions, presolvedModel->yields);
					if (!presolvedConstraint.outputempty() ||
						(presolvedConstraint.isobjective() && presolvedConstraint.isgoal()))
					{
						presolvedConstraint.changesourcesid(keptoutputid, keptthemeid);
						if (presolvedConstraint.canbeturnedtoyields())
						{
							presolvedConstraint.turntoyieldsandactions(newthemes, presolvedModel->actions,validActions, newyields, originalid);
						}
						else if (presolvedConstraint.canbeturnedtoyieldsbasedontransitions())
						{
							presolvedConstraint.turntoyieldsbasedontransition(newthemes, presolvedModel->transitions, presolvedModel->actions,validActions, newyields, originalid);
						}
						else {
							newConstraintsIds.push_back(originalid);
							validConstraints[constraintId] = true;
						}
					}
					else if (presolvedConstraint.isspatial())
					{
						newConstraintsIds.push_back(originalid);
						validConstraints[constraintId] = true;
					}
					++oriit;
				}
			++constraintId;
			}

			/*
			std::vector<int>newconstraintsids;
			newconstraintsids.reserve(oldconstraints.size());
			int constraintid = 0;
			std::vector<int>::const_iterator oriit = constraintsids.begin();
			for (const Core::FMTconstraint& constraint : oldconstraints)
			{
				const int originalid = *oriit;
				Core::FMTconstraint presolvedconstraint = constraint.presolve(newfilter, oldthemes, maskthemes, newthemes, newactions, oldyields);
				if (!presolvedconstraint.outputempty()||
					(presolvedconstraint.isobjective() && presolvedconstraint.isgoal()))
				{
					presolvedconstraint.changesourcesid(keptoutputid, keptthemeid);
					if (presolvedconstraint.canbeturnedtoyields())
					{
						presolvedconstraint.turntoyieldsandactions(newthemes, newactions, newyields,originalid);
					}else if(presolvedconstraint.canbeturnedtoyieldsbasedontransitions())
					{
						presolvedconstraint.turntoyieldsbasedontransition(newthemes, newtransitions ,newactions, newyields,originalid);
					}
					else{
						newconstraintsids.push_back(originalid);
						newconstraints.push_back(presolvedconstraint);
					}
				}else if (presolvedconstraint.isspatial())
				{
					newconstraintsids.push_back(originalid);
					newconstraints.push_back(presolvedconstraint);
				}
				++oriit;
			}
			newsize += newconstraints.size();
			*/
			newsize += newConstraintsIds.size();
			newsize += newyields.size();
			constraintsIds.swap(newConstraintsIds);
			presolvedModel->themes.swap(newthemes);
			presolvedModel->area.swap(newarea);
			//oldactions.swap(newactions);
			//presolvedModel->transitions.swap(newtransitions);
			presolvedModel->yields.swap(newyields);
			presolvedModel->lifespan.swap(newlifespans);
			presolvedModel->outputs.swap(newoutputs);
			//oldconstraints.swap(newconstraints);
			oldpresolvefilter.swap(newfilter);
			--presolvepass;
			didonepass = true;
		}
	/*oldthemes.shrink_to_fit();
	oldarea.shrink_to_fit();
	oldactions.shrink_to_fit();
	oldtransitions.shrink_to_fit();
	oldoutputs.shrink_to_fit();*/
	//Clean up the non valid constraints
	_cleanVector<Core::FMTconstraint>(presolvedModel->constraints, validConstraints);
	_cleanVector<Core::FMTaction>(presolvedModel->actions, validActions);
	_cleanVector<Core::FMTtransition>(presolvedModel->transitions, validTransitions);
	//presolvedConstraints.shrink_to_fit();
	//oldconstraints.shrink_to_fit();
	_logger->logwithlevel("Presolve stopped after " + std::to_string(getparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS) - presolvepass) + " iterations\n", 1);
	_logger->logwithlevel("Developments "+std::to_string(presolvedModel->area.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->area.size())-static_cast<int>(area.size())) + "), "
				+"Themes "+std::to_string(presolvedModel->themes.size())+"(" + std::to_string(static_cast<int>(presolvedModel->themes.size())-static_cast<int>(themes.size())) + "), "
				+"Yields "+std::to_string(presolvedModel->yields.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->yields.size())-static_cast<int>(yields.size())) + "), "
				+"Actions "+std::to_string(presolvedModel->actions.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->actions.size())-static_cast<int>(actions.size())) + "), "
				+"Transitions "+std::to_string(presolvedModel->transitions.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->transitions.size())-static_cast<int>(transitions.size())) + "), "
				+"Outputs "+std::to_string(presolvedModel->outputs.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->outputs.size())-static_cast<int>(outputs.size())) + "), "
				+"Constraints "+std::to_string(presolvedModel->constraints.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->constraints.size()) - static_cast<int>(constraints.size())) + ") and "
				+"Elements "+ std::to_string(newsize)+"("+std::to_string(static_cast<int>(newsize)- static_cast<int>(originalsize)) +")\n",1);
	std::array<std::string,6>sections{"Area","Themes","Yields","Actions","Transitions","Outputs" };//,"Constraints"};
	std::array<size_t, 6>sizeofsections{ presolvedModel->area.size() ,
										presolvedModel->themes.size() ,
										presolvedModel->yields.size() ,
										presolvedModel->actions.size() ,
										presolvedModel->transitions.size() ,
										presolvedModel->outputs.size() };
	size_t sectionid = 0;
	for (const std::string& section : sections)
	{
		if (sizeofsections.at(sectionid)==0)
		{
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Empty section: " + section + " after presolve",
				"FMTmodel::presolve", __LINE__, __FILE__);
		}
		++sectionid;
	}
	
	//presolvedmodel = std::unique_ptr<FMTmodel>(new FMTmodel(oldarea, oldthemes, oldactions, oldtransitions, oldyields, oldlifespans, name, oldoutputs, presolvedConstraints,parameters));
	presolvedModel->_gutsOfConstructor(presolvedModel->area);
	}catch (...)
		{
		_exhandler->printexceptions("for "+name+"at presolve pass "+std::to_string(presolvepass),"FMTmodel::presolve", __LINE__, __FILE__);
		}
	return presolvedModel;
	}

void FMTmodel::postsolve(const FMTmodel& originalbasemodel)
	{
	try{
		*this = FMTmodel(originalbasemodel);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::postsolve", __LINE__, __FILE__);
		}
	}

Core::FMTschedule FMTmodel::presolveschedule(const Core::FMTschedule& originalbaseschedule,
	const FMTmodel& originalbasemodel) const
	{
	Core::FMTschedule newschedule;
	try {
		const Core::FMTmaskfilter newfilter = getpresolvefilter(originalbasemodel.getthemes());
		newschedule = originalbaseschedule.presolve(newfilter, this->themes, this->actions);
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTmodel::presolveschedule", __LINE__, __FILE__);
		}
	return newschedule;
	}


FMTmodelstats FMTmodel::getmodelstats() const
	{
	size_t themesdatasize = 0;
	for (const Core::FMTtheme& theme : themes)
		{
		themesdatasize += theme.size();
		}
	size_t actionsdatasize = 0;
	for (const Core::FMTaction& action : actions)
		{
		actionsdatasize += action.size();
		}
	size_t transitionsdatasize = 0;
	for (const Core::FMTtransition& transition : transitions)
		{
		transitionsdatasize += transition.size();
		}
	size_t outputssdatasize = 0;
	for (const Core::FMToutput& output : outputs)
		{
		outputssdatasize += output.size();
		}
	size_t constraintsdatasize = 0;
	for (const Core::FMTconstraint& constraint : constraints)
		{
		constraintsdatasize += constraint.size();
		}
	return FMTmodelstats(themes.size(), themesdatasize, actions.size(), actionsdatasize, transitions.size(), transitionsdatasize,
		yields.size(), lifespan.size(), outputs.size(), outputssdatasize, constraints.size(), constraintsdatasize);
	}

bool FMTmodel::empty() const
	{
	return (area.empty() && actions.empty() && transitions.empty() &&
		yields.empty() && outputs.empty() && constraints.empty() && lifespan.empty());
	}

void FMTmodel::push_back(const FMTmodel& rhs)
	{
	try{
	//Need to check if the model have the same stats!
	const FMTmodelstats basestats = this->getmodelstats();
	const FMTmodelstats rhsstats = rhs.getmodelstats();
	if (basestats.themes == rhsstats.themes && basestats.themesdata == rhsstats.themesdata)
	{
		std::vector<Core::FMTtheme>newthemes = themes;//Need to concat themes!
		//Need to had some double check to make sure every elements are unique
		std::vector<Core::FMTactualdevelopment>newarea = area;
		for (const Core::FMTactualdevelopment& dev : rhs.area)//Need to check presence of!
		{
			std::vector<Core::FMTactualdevelopment>::iterator actualdev = std::find_if(newarea.begin(), newarea.end(), Core::FMTactualdevelopmentcomparator(&dev));
			if (actualdev == newarea.end())
			{
				newarea.push_back(dev);
			}
			else {
				actualdev->setarea(actualdev->getarea() + dev.getarea());
			}
		}
		std::vector<Core::FMTaction>finalactions = actions;
		std::vector<Core::FMTtransition>finaltransitions = transitions;
		size_t id = 0;
		for (const Core::FMTaction& action : rhs.actions)
		{
			std::vector<Core::FMTaction>::iterator actionitr = std::find_if(finalactions.begin(), finalactions.end(), Core::FMTactioncomparator(action.getname()));
			if (actionitr == finalactions.end())
			{
				finalactions.push_back(action);
				finaltransitions.push_back(rhs.transitions.at(id));
			}
			else {
				Core::FMTaction rhsaction(action);
				actionitr->unshrink(themes);
				rhsaction.unshrink(newthemes);
				actionitr->push_back(rhsaction);
				std::vector<Core::FMTtransition>::iterator transitionitr = std::find_if(finaltransitions.begin(), finaltransitions.end(), Core::FMTtransitioncomparator(action.getname()));
				if (transitionitr != transitions.end())
				{
					Core::FMTtransition rhstransition(rhs.transitions.at(id));
					rhstransition.unshrink(rhs.themes);
					transitionitr->unshrink(themes);
					transitionitr->push_back(rhstransition);
				}
			}
			++id;
		}
		std::vector<Core::FMToutput>finaloutputs = outputs;
		for (const Core::FMToutput& output : rhs.outputs)
		{
			if (std::find_if(finaloutputs.begin(), finaloutputs.end(), Core::FMToutputcomparator(output.getname())) == finaloutputs.end())
			{
				finaloutputs.push_back(output);
			}
		}
		std::vector<Core::FMTconstraint>finalconstraints = constraints;
		if (!rhs.constraints.empty())
		{
			std::vector<Core::FMTconstraint>constraintssubset = rhs.constraints;
			constraintssubset.erase(constraintssubset.begin());
			for (const Core::FMTconstraint& constraint : constraintssubset)
			{
				if (std::find_if(finalconstraints.begin(), finalconstraints.end(), Core::FMToutputcomparator(constraint.getname())) == finalconstraints.end())
				{
					finalconstraints.push_back(constraint);
				}
			}
		}
		Core::FMTyields newyields(yields);
		Core::FMTyields rhsyields(rhs.yields);
		newyields.unshrink(themes);
		rhsyields.unshrink(rhs.themes);
		newyields.push_back(rhs.yields);
		newyields.update();
		Core::FMTlifespans newlifespan(lifespan);
		Core::FMTlifespans rhslifespan(rhs.lifespan);
		newlifespan.unshrink(themes);
		rhslifespan.unshrink(rhs.themes);
		newlifespan.push_back(rhslifespan);
		/*area = newarea;
		themes = newthemes;
		actions = finalactions;
		transitions = finaltransitions;
		yields = newyields;
		lifespan = newlifespan;
		outputs = finaloutputs;
		constraints = finalconstraints;*/
		*this = rhs;
		}
		}catch (...)
			{
				_exhandler->printexceptions("", "FMTmodel::push_back", __LINE__, __FILE__);
			}


	}

double FMTmodel::getinitialarea() const
	{
	double totalarea = 0;
	for (const Core::FMTactualdevelopment& basedev : area)
		{
		totalarea += basedev.getarea();
		}
	return totalarea;
	}

void FMTmodel::setareaperiod(const int& period)
	{
	for (Core::FMTactualdevelopment& basedev : area)
		{
		basedev.setperiod(period);
		}
	}

int FMTmodel::getAreaPeriod() const
	{
	int period = 0;
	if (!area.empty())
		{
		period = area.cbegin()->getperiod();
		}
	return period;
	}

Core::FMTschedule FMTmodel::getpotentialschedule(std::vector<Core::FMTactualdevelopment> toremove,
	std::vector<Core::FMTactualdevelopment> selection, bool withlock) const
{
	int period = 1;
	if (!selection.empty())
	{
		period = selection.back().getperiod();
	}
	Core::FMTschedule schedule(period,*this, withlock);

	try {
		boost::unordered_set<Core::FMTdevelopment>nottoprocess(toremove.begin(), toremove.end());
		size_t actionid = 0;
		for (const Core::FMTaction& action : actions)
			{
			std::vector<Core::FMTactualdevelopment>newselection;
			for (const Core::FMTactualdevelopment& actdev : selection)
				{
				if (nottoprocess.find(actdev)==nottoprocess.end()&&actdev.operable(action, yields))
					{
					schedule.addevent(actdev, 1.0, action);
					for (const Core::FMTdevelopmentpath& path : actdev.operate(action, transitions.at(actionid), yields, themes))
						{
						newselection.emplace_back(path.getDevelopment(), 1.0);
						}
					}
				}
			selection.insert(selection.end(),newselection.begin(), newselection.end());
			++actionid;
			}
		schedule.clean();
	}catch (...)
	{
		_exhandler->raisefromcatch("", " FMTmodel::getpotentialschedule", __LINE__, __FILE__);
	}
	return schedule;
}

std::vector<Core::FMTschedule>FMTmodel::setupschedulesforbuild(const std::vector<Core::FMTschedule>& schedules) const
{
	std::vector<Core::FMTschedule>newshedules;
try{
	bool gotemptyschedule = true;
	for (int period = 1; period <= parameters.getintparameter(LENGTH);++period)
		{
		Core::FMTschedule newschedule;
		for (const Core::FMTschedule& schedule : schedules)
			{
			if (schedule.getperiod()==period)
				{
				newschedule = schedule;
				gotemptyschedule = false;
				break;
				}
			}
		newshedules.push_back(newschedule);
		}
	if (parameters.getboolparameter(FORCE_PARTIAL_BUILD)&&gotemptyschedule)
		{
		_exhandler->raise(Exception::FMTexc::FMTignore,
			"Building natural growth graph for "+getname(),
			"FMTmodel::setupschedulesforbuild", __LINE__, __FILE__);
		}

	if (!parameters.getboolparameter(FORCE_PARTIAL_BUILD)&&!gotemptyschedule)
		{
		_exhandler->raise(Exception::FMTexc::FMTignore,
			"FMTschedule will be ignored for " + getname(),
			"FMTmodel::setupschedulesforbuild", __LINE__, __FILE__);
		}


}catch (...)
	{
	_exhandler->raisefromcatch("", "FMTmodel::setupschedulesforbuild", __LINE__, __FILE__);
	}
return newshedules;
}



bool FMTmodel::doplanning(const bool& solve,std::vector<Core::FMTschedule> schedules)
	{
	bool optimal_solved = false;
	try{
		const int presolve_iterations = parameters.getintparameter(PRESOLVE_ITERATIONS);
		std::unique_ptr<FMTmodel> presolved_model;
		if(presolve_iterations>0)
		{
			const std::chrono::time_point<std::chrono::high_resolution_clock>presolvestart = getclock();
			presolved_model = this->presolve(area);
			//Parser::FMTmodelparser mparser;
			//mparser.write(*presolved_model,"C:/Users/admlocal/Desktop/test/");
			_logger->logwithlevel("Presolved " + getname() + " " +getdurationinseconds(presolvestart) + "\n", 1);
		}else{
			presolved_model = this->clone();
		}
		std::vector<Core::FMTschedule> presolved_schedules;
		if(presolve_iterations>0 && !schedules.empty())
		{
			for (const Core::FMTschedule schedule : schedules )
			{
				presolved_schedules.push_back(presolved_model->presolveschedule(schedule,*this));
			}
		}else{
			presolved_schedules = schedules;
		}
		presolved_schedules = setupschedulesforbuild(presolved_schedules);
		const std::chrono::time_point<std::chrono::high_resolution_clock>buildstart = getclock();
		presolved_model->build(presolved_schedules);
		//
		
		//
		_logger->logwithlevel("Builded " + getname() +" "+getdurationinseconds(buildstart)+ "\n", 1);
		if(solve)
		{
			const std::chrono::time_point<std::chrono::high_resolution_clock>solverstart = getclock();
			optimal_solved = presolved_model->solve();
			_logger->logwithlevel("Solved " + getname() + " " + getdurationinseconds(solverstart) + "\n", 1);
		}
		if (parameters.getboolparameter(POSTSOLVE) && presolve_iterations > 0)
			{
			const std::chrono::time_point<std::chrono::high_resolution_clock>postsolvestart = getclock();
			presolved_model->postsolve(*this);
			_logger->logwithlevel("Postsolved " + getname() + " " + getdurationinseconds(postsolvestart) + "\n", 1);
			}
		this->swap_ptr(presolved_model);
	}catch(...){
		_exhandler->raisefromcatch("", " FMTmodel::doplanning", __LINE__, __FILE__);
	}
	return optimal_solved;
	}

std::vector<Core::FMTconstraint> FMTmodel::getreplanningconstraints(const std::string& modeltype, const std::vector<Core::FMTconstraint>& localconstraints, const int& period) const
{
	std::vector<Core::FMTconstraint>newconstraints(localconstraints.begin(), localconstraints.end());
	try {
		size_t constraintid = 0;
		for (const Core::FMTconstraint& constraint : localconstraints)
		{
			if (constraint.issetfrom(modeltype))
			{
				const double value = getoutput(constraint, period, Core::FMToutputlevel::totalonly).at("Total");
				newconstraints[constraintid] = constraint.setfrom(modeltype,value);
			}
			++constraintid;
		}
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getreplanningconstraints", __LINE__, __FILE__);
	}
	return newconstraints;
}
std::map<std::string, std::vector<std::vector<double>>>FMTmodel::getoutputsfromperiods(const std::vector<Core::FMToutput>& theoutputs,
	const int& firstperiod, const int& lastperiod, Core::FMToutputlevel level) const
	{
	std::map<std::string, std::vector<std::vector<double>>>outs;
	try {

		for (size_t outid = 0; outid < theoutputs.size(); ++outid)
			{
			size_t periodid = 0;
			for (int period = firstperiod; period <= lastperiod;++period)
				{
				for (const auto& values : this->getoutput(theoutputs.at(outid), period, level))
					{
					if (outs.find(values.first)==outs.end())
						{
						outs[values.first] = std::vector<std::vector<double>>(theoutputs.size(),std::vector<double>((lastperiod-firstperiod)+1,std::numeric_limits<double>::quiet_NaN()));
						}
					outs[values.first][outid][periodid] = values.second;
					}
				++periodid;
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTmodel::getoutputsfromperiods", __LINE__, __FILE__);
		}
	return outs;
	}

std::unique_ptr<FMTmodel> FMTmodel::clone() const
	{
	return std::unique_ptr<FMTmodel>(new FMTmodel(*this));
	}

Core::FMTschedule FMTmodel::getsolution(int period, bool withlock) const
	{
	return Core::FMTschedule();
	}

std::map<std::string, double> FMTmodel::getoutput(const Core::FMToutput& output,
	int period, Core::FMToutputlevel level) const
{
	return std::map<std::string, double>();
}

bool FMTmodel::setparameter(const FMTintmodelparameters& key, const int& value)
{
	try{
		if (parameters.setintparameter(key, value))
		{
			return (true);
		}
	}catch(...){
		_exhandler->printexceptions("", "FMTmodel::setparameter", __LINE__, __FILE__);
	}
	return false;
}

bool FMTmodel::setparameter(const FMTdblmodelparameters& key, const double& value)
{
	try{
		if (parameters.setdblparameter(key,value)) return (true);
	}catch(...){
		_exhandler->printexceptions("", "FMTmodel::setparameter", __LINE__, __FILE__);
	}
	return false;
}

bool FMTmodel::setparameter(const FMTboolmodelparameters& key, const bool& value)
{
	try{
		if (parameters.setboolparameter(key,value)) return (true);
	}catch(...){
		_exhandler->printexceptions("", "FMTmodel::setparameter", __LINE__, __FILE__);
	}
	return false;
}

bool FMTmodel::setparameter(const FMTstrmodelparameters& p_key, const std::string& p_value)
{
	try {
		if (parameters.setstrparameter(p_key, p_value)) return (true);
	}
	catch (...) {
		_exhandler->printexceptions("", "FMTmodel::setparameter", __LINE__, __FILE__);
	}
	return false;
}

int FMTmodel::getparameter(const FMTintmodelparameters& key)const
{
	int value;
	try{
		value = parameters.getintparameter(key);
	}catch(...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getparameter", __LINE__, __FILE__);
	}
	return value;
}

double FMTmodel::getparameter(const FMTdblmodelparameters& key)const
{
	double value;
	try{
		value = parameters.getdblparameter(key);
	}catch(...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getparameter", __LINE__, __FILE__);
	}
	return value;
}

bool FMTmodel::getparameter(const FMTboolmodelparameters& key) const
{
	bool value;
	try{
		value = parameters.getboolparameter(key);
	}catch(...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getparameter", __LINE__, __FILE__);
	}
	return value;
}

const std::string& FMTmodel::getparameter(const FMTstrmodelparameters& p_key) const
{
	try {
		return parameters.getstrparameter(p_key);
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::getparameter", __LINE__, __FILE__);
	}
	return nullptr;
}

bool FMTmodel::setcompresstime(const int& periodstart, const int& periodstop, const int& value)
{
	bool returnbool = true;
	try{
		for (int period = (periodstart-1);period<periodstop;++period)
		{
			if (!parameters.setperiodcompresstime(period,value)) 
			{
				returnbool=false;
			}
		}
	}catch(...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::setcompresstime", __LINE__, __FILE__);
	}
	return returnbool;
}

std::vector<int> FMTmodel::getcompresstime() const
{
	return parameters.getcompresstime();
}

std::vector<Core::FMTconstraint>FMTmodel::goalconstraints(double penalty) const
{
	std::vector<Core::FMTconstraint>newconstraints;
	try {
		size_t constraintid = 0;
		for (const Core::FMTconstraint& constraint : constraints)
			{
			if (constraint.isobjective())
				{
				Core::FMTconstraint newobjective(constraint);
				std::vector<std::string>variables;
				variables.push_back("_ALL");
				std::string penop = "-";
				if (constraint.sense()==1.0)
					{
					penop = "+";
					}
				newobjective.setpenalties(penop, variables);
				newconstraints.push_back(newobjective);
				}else if(constraint.getconstrainttype()==Core::FMTconstrainttype::FMTstandard) 
					{
					const std::string VariableName("~G"+std::to_string(constraintid));
					Core::FMTconstraint newconstraint(constraint);
					newconstraint.setgoal(VariableName, penalty);
					newconstraints.push_back(newconstraint);
					}
				++constraintid;
				}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::goalconstraints", __LINE__, __FILE__);
	}
	return newconstraints;
}

std::vector<Core::FMTconstraint>FMTmodel::gettacticalconstraints(double penalty,double scheduleweight,double objective, double objectivefactor) const
{
	std::vector<Core::FMTconstraint>newconstraints;
	try {
		const std::vector<Core::FMTconstraint> goaledconstraints = goalconstraints(penalty);
		//_SETFROMGLOBAL|_SETFROMLOCAL|_REIGNORE
		for (const Core::FMTconstraint& constraint : goaledconstraints)
			{
			if (constraint.isobjective())
			{
				double lowerbound = std::numeric_limits<double>::lowest();
				double upperbound = 0.0;
				if (constraint.sense()==1.0)
					{
					upperbound = std::numeric_limits<double>::max();
					lowerbound = 0.0;
					}
				//Limit objective constraints
				Core::FMTconstraint globalset(Core::FMTconstrainttype::FMTstandard,Core::FMToutput(constraint));
				globalset.setrhs(lowerbound, upperbound);
				globalset.setlength(1, 1);
				globalset.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, "_SETFROMGLOBAL", objectivefactor, objectivefactor));
				newconstraints.push_back(globalset);
				//Set schedule weight
				Core::FMTconstraint newobjective(constraint);
				if (scheduleweight > 0)
					{
					newobjective.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, "_SETGLOBALSCHEDULE", scheduleweight, scheduleweight));
					}
				newconstraints.push_back(newobjective);
			}else if(constraint.getconstrainttype()==Core::FMTconstrainttype::FMTstandard) 
				{
				Core::FMTconstraint newconstraint(constraint);
				if (constraint.getperiodlowerbound() == 1 &&
					constraint.getperiodupperbound() == 1)
					{
					newconstraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, "_REIGNORE", 2,2));
					}
				newconstraints.push_back(newconstraint);
				}
			}

	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTmodel::gettacticalconstraints", __LINE__, __FILE__);
	}
	return newconstraints;
}



void FMTmodel::showparameters(const bool& showhelp)const
{
	std::string message=" - Parameters for model "+getname()+"\n";
	try{
	*_logger<<message<<"	-- Int parameters"<<"\n";
	message="	LENGTH                   : "+std::to_string(parameters.getintparameter(LENGTH))+"\n";
	if(showhelp) message+="	(The number of period to optimize or simulate. DEFAULT=30)\n";
	message+="	SEED                     : "+std::to_string(parameters.getintparameter(SEED))+"\n";
	if(showhelp) message+="	(The seed used for stochastique process in FMTsamodel, FMTnssmodel and FMTsesmodel. DEFAULT=25)\n";
	message+="	NUMBER_OF_ITERATIONS     : "+std::to_string(parameters.getintparameter(NUMBER_OF_ITERATIONS))+"\n";
	if(showhelp) message+="	(The number of iterations to do in FMTsesmodel::greedyreferencebuild. DEFAULT=10000)\n";
	message+="	PRESOLVE_ITERATIONS      : "+std::to_string(parameters.getintparameter(PRESOLVE_ITERATIONS))+"\n";
	if(showhelp) message+="	(The number of iterations to do in FMTmodel::presolve. DEFAULT=10)\n";
	message+="	NUMBER_OF_THREADS        : "+std::to_string(parameters.getintparameter(NUMBER_OF_THREADS))+"\n";
	if(showhelp) message+="	(Number of thread use by solver for optimisation. DEFAULT=Number of concurrent threads supported)\n";
	*_logger<<message<<"\n	-- Double parameters"<<"\n";
	message="	TOLERANCE                : "+std::to_string(parameters.getdblparameter(TOLERANCE))+"\n";
	if(showhelp) message+="	(Double tolerance used when setting a solution from schedules. DEFAULT="+std::to_string(FMT_DBL_TOLERANCE)+")\n";
	message+="	GOALING_SCHEDULE_WEIGHT  : "+std::to_string(parameters.getdblparameter(GOALING_SCHEDULE_WEIGHT))+"\n";
	if(showhelp) message+="	(The weight to use when trying goal a schedule from a strategic model. DEFAULT=10000)\n";
	*_logger<<message<<"\n	-- Bool parameters"<<"\n";
	std::string boolval;
	boolval = (parameters.getboolparameter(FORCE_PARTIAL_BUILD)) ? "true\n" : "false\n";
	message="	FORCE_PARTIAL_BUILD      : "+boolval;
	if(showhelp) message+="	(Force partial build on graph and matrix for period which schedules are passed to FMTlpmodel::doplanning. DEFAULT=false)\n";
	boolval = (parameters.getboolparameter(STRICTLY_POSITIVE)) ? "true\n" : "false\n";
	message+="	STRICTLY_POSITIVE        : "+boolval;
	if(showhelp) message+="	(Force matrix to have bound >= 0 for the outputs use in constraints or objective. DEFAULT=true)\n";
	boolval = (parameters.getboolparameter(POSTSOLVE)) ? "true\n" : "false\n";
	message+="	POSTSOLVE                : "+boolval;
	if(showhelp) message+="	(Postsolve model after doplanning. DEFAULT=true)\n";
	boolval = (parameters.getboolparameter(SHOW_LOCK_IN_SCHEDULES)) ? "true\n" : "false\n";
	message+="	SHOW_LOCK_IN_SCHEDULES   : "+boolval;
	if(showhelp) message+="	(When user ask for a schedule, if true, lock will appear in it. DEFAULT=false)\n";
	/*
	std::string compresstimes;
	std::vector<int>::const_iterator cit = compresstime.cbegin();
	while(*cit == *(cit+1))

	//Vector for compresstime at each period
	compresstime = std::vector<int>(30,1);
	*/
	*_logger<<message<<"\n";
	}catch(...)
	{

	}
}

void FMTmodel::swap_ptr(std::unique_ptr<FMTmodel>& rhs)
{
	*this = std::move(*rhs); 
}


FMTmodelcomparator::FMTmodelcomparator(std::string name) :model_name(name) {}

bool FMTmodelcomparator::operator()(const FMTmodel& model) const
	{
	return(model_name == model.getname());
	}






}



BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTmodel)
