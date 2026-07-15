/*
Copyright (c) 2019 Gouvernement du Qu�bec

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
#include <algorithm>
//#include <cvmarkersobj.h>
#include <memory>
//#include <boost/container/flat_set.hpp>
//using namespace Concurrency::diagnostic;


namespace Models{

    double FMTmodel::getYieldValue(const std::string& p_mask, const std::string& p_yield, int p_age, int p_period) const 
    {
	    double result = 0;
        try
        {
            const Core::FMTmask MASK = Core::FMTmask(p_mask, themes);
            const Core::FMTdevelopment DEVELOPMENT = Core::FMTdevelopment(MASK, p_age, p_period);
            const Core::FMTyieldrequest REQUEST = DEVELOPMENT.getYieldRequest();
            result = yields.get(REQUEST, p_yield);
        }
        catch (...)
        {
            _exhandler->raiseFromCatch("", "FMTmodel::getYieldValue", __LINE__, __FILE__);
        }
        return result;
    }


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
				gotIt = constraints.at(cId).isReIgnore(p_replanningPeriod);
				++cId;
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTmodel::gotReIgnore", __LINE__, __FILE__);
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
			_exhandler->raiseFromCatch("", "FMTmodel::gotReIgnore", __LINE__, __FILE__);
		}
		return gotIt;
	}

	std::vector<Core::FMTschedule>FMTmodel::buildSchedule(const Core::FMTaction& p_action,
															const FMTmodel& p_BaseModel,
															const std::string& p_Targetyield,
															const std::vector<Core::FMTschedule>& p_schedules) const
	{
		
		std::vector<Core::FMTschedule>newSchedules;
		try {
			Core::FMTmask ActionMask = p_action.getUnion(themes);
			ActionMask.update(themes);
			const std::string BEFORE = "PRE";
			const std::string AFTER = "POST";
			std::vector<const Core::FMTtheme*>themesTest;
			for (const Core::FMTtheme& p_theme : themes)
				{
				themesTest.push_back(&p_theme);
				}
			/*const std::vector<size_t>Static = statictransitionthemes;
			for (size_t thId = 0; thId < themes.size();++thId)
				{
				if (std::find(Static.begin(),Static.end(), thId)== Static.end())
					{
					ActionMask.set(themes.at(thId), "?");
					}
				}*/
			const int MAX_PERIOD = std::min(p_action.getPeriodUpperBound(),
				getParameter(Models::FMTintmodelparameters::LENGTH));
			const std::vector<size_t>AGGREGATE_THEMES = p_BaseModel._getAggregatesThemes(p_Targetyield);
			//const std::string POST_ATTRIBUTE = themes.back().getBaseAttributes().back();
			boost::unordered_map<Core::FMTdevelopment, double>ActToRemove;
			const std::vector<size_t> AGGREGATES = p_BaseModel._getAggregatesThemes(p_Targetyield);
			for (const Core::FMTschedule& ACT_SCHEDULE : p_schedules)
			{
				const int PERIOD = ACT_SCHEDULE.getPeriod();
				Core::FMTschedule baseSchedule(PERIOD, ACT_SCHEDULE, ACT_SCHEDULE.doUseLock());
				for (const auto& ACTION : ACT_SCHEDULE)
					{
					for (const auto& dev : ACTION.second)
						{
						const std::string BASE_THEME = p_BaseModel._getYieldAttribute(dev.first.getMask(),
																					p_Targetyield, AGGREGATES);
						const std::string BEFORE_THEME = BASE_THEME +BEFORE;
						const std::string AFTER_THEME = BASE_THEME + AFTER;
						Core::FMTdevelopment newDev(dev.first);
						Core::FMTmask newMask = Core::FMTmask(newDev.getMask(),themes);
						newMask.set(themes.back(), BEFORE_THEME);
						newDev.setMask(newMask);
						if (//dev.first.getMask().isSubsetOf(ActionMask)&&
							!newMask.isNotThemesSubset(ActionMask,themesTest))
							{
							if (PERIOD > MAX_PERIOD)
								{
								newMask.set(themes.back(), AFTER_THEME);
								newDev.setMask(newMask);
							}else{
								double total = 0;
								for (const double& value : dev.second)
									{
									total += value;
									}
								Core::FMTdevelopment tohash(dev.first);
								tohash.setPeriod(PERIOD);
								//tohash.setLock(0);
								if (ActToRemove.find(tohash)==ActToRemove.end())
									{
									ActToRemove[tohash] = 0;
									}
								ActToRemove[tohash] += total;
								}

							}
						for (const double& AREA : dev.second)
							{
							if (AREA > FMT_DBL_TOLERANCE)
								{
								baseSchedule.addEvent(newDev, AREA, ACTION.first);
								}
							}
						}
					}
				newSchedules.push_back(baseSchedule);

			}
			
			for (const Core::FMTactualdevelopment& dev : area)
				{
				Core::FMTfuturdevelopment futurDev(dev);
				futurDev.setPeriod(1);
				//futurDev.setLock(0);
				for (int period = 1; period <= MAX_PERIOD;++period)
					{
					//newSchedules[period - 1].setPeriod(period);
					//newSchedules[period - 1].setUseLock(false);
					if (futurDev.operable(p_action,yields))
						{
						double value = dev.getArea();
						if (ActToRemove.find(futurDev)!= ActToRemove.end())
							{
							value -= ActToRemove.at(futurDev);
							}
						newSchedules[period - 1].addEvent(futurDev, value, p_action);
						}
					futurDev = futurDev.grow();
					}
				}

		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTmodel::buildSchedule", __LINE__, __FILE__);
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
				Core::FMTschedule newSchedule(SCHEDULE.getPeriod(),SCHEDULE,SCHEDULE.doUseLock());
				for (const auto& ACTION : SCHEDULE)
					{
					std::vector<Core::FMTaction>::const_iterator actPtr = std::find_if(actions.begin(), actions.end(),Core::FMTActionComparator(ACTION.first.getName()));
					if (actPtr != actions.end())
						{
						for (const auto& dev : ACTION.second)
							{
							const Core::FMTdevelopment TARGET(Core::FMTmask(dev.first.getMask(), themes), dev.first.getAge(), dev.first.getLock());
							for (const double& VALUE : dev.second)
								{
								newSchedule.addEvent(TARGET, VALUE, *actPtr);
								}
							}
						}
					}
				newSchedules.push_back(newSchedule);
				}

		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTmodel::extendSchedule", __LINE__, __FILE__);
			}
		return newSchedules;
	}

	std::string FMTmodel::_getAggregatesWrap(const Core::FMTmask& p_mask,
		const std::vector<size_t>& p_themes) const
	{
		std::string Wrap;
		try {
			for (size_t i : p_themes)
				{
				std::string subSet = p_mask.get(themes.at(i));
				if (subSet!="?")
					{
					std::replace(subSet.begin(), subSet.end(), '!', 'N');
					Wrap += subSet;
					}
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTmodel::_getAggregatesWrap", __LINE__, __FILE__);
			}

		return Wrap;
	}

	std::vector<size_t> FMTmodel::_getAggregatesThemes(const std::string& p_yieldName) const
	{
		std::vector<size_t> aggregateThemes;
		try {
			aggregateThemes = getStaticTransitionThemes();
			std::set<size_t>subset;
			for (const auto& HANDLER :
				yields.getHandlerOfType(Core::FMTyldtype::FMTageyld))
			{
				if (HANDLER->containsYield(p_yieldName))
				{
					for (size_t i : aggregateThemes)
					{
						if (themes.at(i).isAggregate(HANDLER->getMask().get(themes.at(i))))
							{
							subset.insert(i);
							}
					}
				}
			}
			aggregateThemes = std::vector<size_t>(subset.begin(), subset.end());
		}catch (...)
		{
			_exhandler->raiseFromCatch("",
				"FMTmodel::_getAggregatesThemes", __LINE__, __FILE__);
		}
		return aggregateThemes;
	}

	std::string FMTmodel::_getYieldAttribute(const Core::FMTmask& p_devMask, 
							const std::string& p_yieldName,
							const std::vector<size_t>& p_AggregatedThemes) const
	{
		try {
			for (const auto& YIELD : yields.findSets(
				Core::FMTmask(std::string(p_devMask),themes)))
			{
				if (YIELD->second->containsYield(p_yieldName)&&
					YIELD->second->getType() == Core::FMTyldtype::FMTageyld)
				{
					return _getAggregatesWrap(YIELD->second->getMask(), p_AggregatedThemes);
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTmodel::_getYieldAttribute", __LINE__, __FILE__);
			}
		return std::string();
	}

	std::set<std::string>FMTmodel::_getYieldsStraticAggregates(const std::string& p_yieldName) const
	{
		std::set<std::string> selections;
		try {
			const std::vector<size_t>AGGREGATE_THEMES = _getAggregatesThemes(p_yieldName);
			for (const auto& HANDLER :
				yields.getHandlerOfType(Core::FMTyldtype::FMTageyld))
			{
				if (HANDLER->containsYield(p_yieldName) &&
					HANDLER->getType() == Core::FMTyldtype::FMTageyld)
				{
					selections.insert(
						_getAggregatesWrap(HANDLER->getMask(), AGGREGATE_THEMES));
				}

			}

		}catch (...)
			{
			_exhandler->raiseFromCatch("", 
				"FMTmodel::_getYieldsStraticAggregates", __LINE__, __FILE__);
			}
		return selections;
	}


	FMTmodel FMTmodel::buildAction(const std::string& p_actionName,
		const std::string& p_Targetyield) const
	{
		FMTmodel newModel(*this);
		try {
			std::vector<std::string>newAttributes;
			const std::string BEFORE = "PRE";
			const std::string AFTER = "POST";
			for (const auto& SUB : _getYieldsStraticAggregates(p_Targetyield))
				{
				newAttributes.push_back(SUB+ BEFORE);
				newAttributes.push_back(SUB + AFTER);
				}
			newModel.pushTheme("~FMT_THEME_" + p_actionName, p_Targetyield,newAttributes);
			Core::FMTaction newAction(p_actionName,false,false);
			Core::FMTtransition newTransition(p_actionName);
			const std::vector<size_t> AGGREGATES = _getAggregatesThemes(p_Targetyield);
			Core::FMTmask ActionMask;			
			boost::unordered_map<Core::FMTmask,double>YieldMasks;
			for (const auto& yield : yields)
				{
				if (yield.second->containsYield(p_Targetyield)&&
					yield.second->getType()==Core::FMTyldtype::FMTageyld)
					{
					const Core::FMTageyieldhandler* AGE_HANDLER = dynamic_cast<const Core::FMTageyieldhandler*>(yield.second.get());
					const double PEAK = AGE_HANDLER->getPeakfrom(p_Targetyield,FMT_DBL_TOLERANCE);
					if (PEAK > 0)
						{
						const Core::FMTmask MASK = AGE_HANDLER->getMask();
						YieldMasks[MASK] = PEAK;
						const Core::FMTmask NEW_MASK(std::string(MASK) + " " +
							_getAggregatesWrap(MASK, AGGREGATES) + BEFORE, newModel.themes);
						Core::FMTspec specification;
						specification.setBounds(Core::FMTperbounds(Core::FMTsection::Action, PEAK, PEAK));
						if (ActionMask.empty())
							{
							ActionMask = NEW_MASK;
						}else {
							ActionMask = ActionMask.getUnion(NEW_MASK);
							}
						newAction.push_back(NEW_MASK, specification);
						Core::FMTfork fork;
						Core::FMTmask AfterMask(std::string(MASK) + " " +
							_getAggregatesWrap(MASK, AGGREGATES) + AFTER, newModel.themes);
						for (size_t i = 0; i < themes.size();++i)
							{
							AfterMask.set(themes.at(i), "?");
							}
						const Core::FMTtransitionmask TR_MASK(std::string(AfterMask), newModel.themes, 100.0);
						fork.add(TR_MASK);
						newTransition.push_back(NEW_MASK, fork);
						}
					}
				}
			newModel.addNewMask(ActionMask);
			Core::FMTyields newYields;
			for (const auto& yield : yields)
				{
				const Core::FMTmask YIELD_MASK = yield.second.get()->getMask();
				const Core::FMTmask NEW_MASK(std::string(YIELD_MASK) + " ?", newModel.themes);
				std::unique_ptr<Core::FMTyieldhandler>newYield(yield.second->clone());
				newYield->setMask(NEW_MASK);
				const std::string AGGREGATE_WRAP = _getAggregatesWrap(YIELD_MASK, AGGREGATES);
				if (YieldMasks.find(YIELD_MASK)!= YieldMasks.end())
					{
					const Core::FMTmask SUB_MASK(std::string(YIELD_MASK) + " "+ AGGREGATE_WRAP +BEFORE, newModel.themes);
					newYield->setMask(SUB_MASK);
					const size_t TO_SPLIT = static_cast<size_t>(YieldMasks.at(YIELD_MASK)) - 1;
					const Core::FMTmask NEW_SPLITTED_MASK(std::string(YIELD_MASK) + " " +
						AGGREGATE_WRAP + AFTER, newModel.themes);
					std::unique_ptr<Core::FMTyieldhandler>newSplittedYield(new Core::FMTageyieldhandler(NEW_SPLITTED_MASK));
					const std::vector<int> BASES = yield.second->getBases();
					const size_t SPLIT_DISTANCE = std::distance(BASES.begin() + TO_SPLIT, BASES.end());
					for (size_t i = TO_SPLIT; i < SPLIT_DISTANCE;++i)
						{
						newSplittedYield->pushBase(BASES.at(i));
						}
					for (const std::string& YIELD_NAME : yield.second->getYieldNames())
						{
						const Core::FMTdata BASE_DATA = yield.second->at(YIELD_NAME);
						const std::vector<double>NEW_DATA(BASE_DATA.data.begin() + TO_SPLIT, BASE_DATA.data.end());
						newSplittedYield->pushData(YIELD_NAME, Core::FMTdata(NEW_DATA, BASE_DATA.getOp(), BASE_DATA.getSourcesCopy()));
						}
					newYields.push_back(SUB_MASK, newYield);
					newYields.push_back(NEW_SPLITTED_MASK, newSplittedYield);
					}else{ 
						newYields.push_back(NEW_MASK, newYield);
						}
				}
			newYields.update();
			newAction.update();
			newTransition.update();
			newModel.yields = newYields;
			newModel.actions.insert(newModel.actions.begin(), newAction);
			newModel.transitions.insert(newModel.transitions.begin(),newTransition);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTmodel::buildAction", __LINE__, __FILE__);
			}
		return newModel;
	}

	void FMTmodel::pushTheme(const std::string& p_themeName,const std::string& p_yieldName,
		const std::vector<std::string>& p_attributes)
	{
		try {
			if (p_attributes.empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_theme, "Missing attributes for "+ p_themeName,
					"FMTmodel::pushTheme", __LINE__, __FILE__);
				}
			const std::vector<size_t> AGGREGATES = _getAggregatesThemes(p_yieldName);
			const size_t THEME_START = themes.back().getStart() + themes.back().size();
			std::vector<Core::FMTtheme> Oldthemes(themes);
			themes.emplace_back(p_attributes, themes.size(), THEME_START, p_themeName);
			for (auto& dev : area)
				{
				const std::string BASE_THEME = _getYieldAttribute(dev.getMask(), p_yieldName, AGGREGATES) + "PRE";
				dev.setMask(Core::FMTmask(std::string(dev.getMask()) + " " + BASE_THEME, themes));
				}
			const std::string DEFAULT_ATTRIBUTE = "?";
			for (auto& action : actions)
				{
				action.unShrink(Oldthemes);
				for (auto& op : action)
					{
					op.first = Core::FMTmask(std::string(op.first) + " " + DEFAULT_ATTRIBUTE, themes);
					}
				action.update();
				}
			for (auto& transition : transitions)
				{
				transition.unShrink(Oldthemes);
				for (auto& op : transition)
					{
					op.first = Core::FMTmask(std::string(op.first) + " " + DEFAULT_ATTRIBUTE, themes);
					Core::FMTfork newFork(op.second);
					newFork.clear();
					for (Core::FMTtransitionmask trnm : op.second.getMaskTrans())
						{
						trnm.setMask(Core::FMTmask(std::string(trnm.getMask()) + " "+ DEFAULT_ATTRIBUTE, themes));
						newFork.add(trnm);
						}
					op.second = newFork;
					}
				transition.update();
				}
			yields.unShrink(Oldthemes);
			for (auto& yield : yields)
			{
				yield.first = Core::FMTmask(std::string(yield.first) + " " + DEFAULT_ATTRIBUTE, themes);
				yield.second->setMask(Core::FMTmask(std::string(yield.second->getMask()) + " " + DEFAULT_ATTRIBUTE, themes));
			}
			yields.update();
			lifespan.unShrink(Oldthemes);
			for (auto& lifespanItem : lifespan)
			{
				lifespanItem.first = Core::FMTmask(std::string(lifespanItem.first) + " " + DEFAULT_ATTRIBUTE, themes);
			}
			lifespan.update();
			for (auto& output : outputs)
				{
				std::vector<Core::FMToutputsource> sources = output.getSources();
				for (Core::FMToutputsource& source : sources)
					{
					if (source.isVariable())
						{
						source.setMask(Core::FMTmask(std::string(source.getMask()) + " " + DEFAULT_ATTRIBUTE, themes));
						}
					}
				output.setSources(sources);
				}
			for (auto& output : constraints)
				{
				std::vector<Core::FMToutputsource> sources = output.getSources();
				for (Core::FMToutputsource& source : sources)
				{
					if (source.isVariable())
					{
						source.setMask(Core::FMTmask(std::string(source.getMask()) + " " + DEFAULT_ATTRIBUTE, themes));
					}
				}
				output.setSources(sources);
				}
		
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTmodel::pushTheme", __LINE__, __FILE__);
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
					p_ActionOrdering.push_back(ACTION.getName());
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
			_exhandler->raiseFromCatch("", "FMTmodel::aggregateAllActions", __LINE__, __FILE__);
		}
		return newModel;
	}

	std::vector<std::string>FMTmodel::getSchedulesPriorities(const std::vector<Core::FMTschedule>& p_schedules) const
		{
		std::map<std::string, double>allActions;
		const int MAX_PERIOD = 10;
		for (const Core::FMTschedule& p_schedule : p_schedules)
			{
			if (p_schedule.getPeriod() <= MAX_PERIOD)
				{
				for (const auto& ACTION : p_schedule)
					{
					const std::string ACTION_NAME = ACTION.first.getName();
					const double ACTION_AREA = p_schedule.actionArea(ACTION.first);
					if (allActions.find(ACTION_NAME) == allActions.end())
					{
						allActions[ACTION_NAME] = 0.0;
					}
					allActions[ACTION_NAME] += ACTION_AREA;
					/*const size_t ACTION_ID = std::distance(actions.begin(), std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator(ACTION_NAME)));
					double total = ACTION_AREA;
					for (const auto& DEV_ACTION : ACTION.second)
						{
						const double AREA = std::accumulate(DEV_ACTION.second.begin(), DEV_ACTION.second.end(), 0.0);
						if (AREA > FMT_DBL_TOLERANCE)
							{
							const std::vector<Core::FMTdevelopmentpath> PATHS = DEV_ACTION.first.operate(actions[ACTION_ID], transitions[ACTION_ID], yields, themes);
							const double COEF = DEV_ACTION.first.getHarvestCoef(PATHS, actions[ACTION_ID], yields, "YV_S");
							total += AREA * COEF;
							}
						}
					if (allActions.find(ACTION_NAME) == allActions.end())
						{
						allActions[ACTION_NAME] = 0.0;
						}
					allActions[ACTION_NAME] = total;*/
					}
				}
			}
		for (const Core::FMTaction& ACTION : actions)
		{
			if (allActions.find(ACTION.getName()) == allActions.end())
			{
				allActions[ACTION.getName()] = 0.0;
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
				if (std::find(p_Actions.begin(), p_Actions.end(), ACTION.getName())!= p_Actions.end())
					{
					const std::vector<Core::FMTaction>SPLITTED = ACTION.split(masks, themes);
					for (const Core::FMTaction& SPLITTED_ACTION : SPLITTED)
					{
						Core::FMTtransition NewTransition = transitions.at(i);
						NewTransition.setName(SPLITTED_ACTION.getName());
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
			_exhandler->raiseFromCatch("", "FMTmodel::splitActions", __LINE__, __FILE__);
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
					const std::string GLOBAL_ACTION = ACTION_ELEMENTS.first.getName();
					std::vector<Core::FMTaction>::const_iterator ActIt = std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator(GLOBAL_ACTION));
					if (ActIt == actions.end())//Got aggregate
						{
						boost::unordered_set<Core::FMTdevelopment>DevelopmentsSet;
						
						const std::vector<const Core::FMTaction*> ACTIONS = Core::FMTActionComparator(GLOBAL_ACTION, true).getAllAggregates(actions,true);
						if (ACTIONS.empty())
							{
								_exhandler->raise(Exception::FMTexc::FMTinvalidAandT, "Missing aggregate " + GLOBAL_ACTION,
									"FMTmodel::splitSchedules", __LINE__, __FILE__);
							}
						std::map<Core::FMTdevelopment, std::vector<double>>NewMapping;
						for (const Core::FMTaction* ACTION_Prt : ACTIONS)
						{
							const Core::FMTmask ACTION_MASK = ACTION_Prt->getUnion(themes);
							for (const auto& DEV : ACTION_ELEMENTS.second)
							{
								if (DEV.first.getMask().isSubsetOf(ACTION_MASK) &&
									DevelopmentsSet.find(DEV.first) == DevelopmentsSet.end())
								{
									NewMapping[DEV.first] = DEV.second;
									DevelopmentsSet.insert(DEV.first);
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
				NewSchedules.push_back(Core::FMTschedule(SCHEDULE.getPeriod(), mapping));
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTmodel::splitSchedules", __LINE__, __FILE__);
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
					const std::string ACTION_NAME = ACTION_ELEMENTS.first.getName();
					std::vector<Core::FMTaction>::const_iterator ACTION_IT = std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator(ACTION_NAME));
					if (ACTION_IT==actions.end())//Need to change the action!
						{
						const std::vector<std::string>AGGREGATES = ACTION_ELEMENTS.first.getAggregates();
						for (const Core::FMTaction& action : actions)
							{
							if (std::find(AGGREGATES.begin(), AGGREGATES.end(),action.getName())!= AGGREGATES.end())
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
				newSchedules.push_back(Core::FMTschedule(SCHEDULE.getPeriod(), mapping));
				}

		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTmodel::aggregateSchedules", __LINE__, __FILE__);
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
				for (const Core::FMTaction* Action : Core::FMTActionComparator(Aggregate, true).getAllAggregates(actions))
					{
					Core::FMTaction NoCompress(*Action);
					std::vector<std::string>aggregates;
					for (const std::string& AGGREGATE : NoCompress.getAggregates())
						{
						if (std::find(p_ActionsMapping.begin(), p_ActionsMapping.end(), AGGREGATE)== p_ActionsMapping.end())
							{
							aggregates.push_back(AGGREGATE);
							}
						}
					NoCompress.setAggregates(aggregates);
					if (ActionCover.find(Action->getName()) != ActionCover.end())
						{
						_exhandler->raise(Exception::FMTexc::FMTinvalidAandT, "Action " + Action->getName() +" already in aggregate",
							"FMTmodel::aggregateActions", __LINE__, __FILE__);
						}
					ActionCover.insert(Action->getName());
					NoCompress.unShrink(themes);
					const Core::FMTmask Filter = NoCompress.getUnion(themes);

					ActionFilters[Action->getName()] = std::pair<std::string, Core::FMTmask>(Aggregate, Filter);
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
					if (ActionCover.find(Action.getName())== ActionCover.end())
						{
						missingActions += Action.getName() + ",";
						}
					}
				missingActions.pop_back();
				_exhandler->raise(Exception::FMTexc::FMTinvalidAandT, "Missing aggregate for actions "+ missingActions,
					"FMTmodel::aggregateActions", __LINE__, __FILE__);
				}

			//Handle aggregate of the action
			for (const Core::FMTaction ACTION : actions)
			{
				for (const std::string& AGGREGATE : ACTION.getAggregates())
				{
					if (std::find(p_ActionsMapping.begin(), p_ActionsMapping.end(), AGGREGATE)== p_ActionsMapping.end())
						{
						if (ActionFilters.find(AGGREGATE) != ActionFilters.end())
						{
							ActionFilters[AGGREGATE].second = ActionFilters[AGGREGATE].second.getUnion(ActionFilters[ACTION.getName()].second);
						}else {
							if (ActionFilters.find(ACTION.getName()) != ActionFilters.end())
								{
								ActionFilters[AGGREGATE] = std::pair<std::string, Core::FMTmask>("", ActionFilters[ACTION.getName()].second);
								}
							
							}
						
						}

				}
			}
			


			for (auto& Filter : ActionFilters)
			{
				Filter.second.second = addNewMask(Filter.second.second);
				
			}
			//setActions(NewActions);
			actions = NewActions;
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTmodel::aggregateActions", __LINE__, __FILE__);
		}
		return ActionFilters;
	}

void FMTmodel::aggregateTransitions(const std::map<std::string, std::pair<std::string, Core::FMTmask>>& p_Filters)
	{
	
	try {
		std::vector<Core::FMTtransition>NewTransitions;
		for (const Core::FMTaction& action : actions)
			{
			if (action.getName() != "_DEATH")
				{
				NewTransitions.push_back(Core::FMTtransition(action.getName()));
				}
			}
		Core::FMTtransition DEATH_TRANSITION = transitions.back();
		DEATH_TRANSITION.unShrink(themes);
		NewTransitions.push_back(DEATH_TRANSITION);
		for (Core::FMTtransition& transition : transitions)
			{
			if (p_Filters.find(transition.getName()) != p_Filters.end())
			{
				Core::FMTtransition NewTransition(transition.getName());
				//NewTransition.unShrink(themes);
				transition.unShrink(themes);
				for (const auto& element : transition)
				{
					const Core::FMTmask Intersection = p_Filters.at(transition.getName()).second.getIntersect(element.first);
					if (!Intersection.isNotThemesSubset(element.first,themes))
						{
						const Core::FMTmask NEW_MASK = addNewMask(Intersection);
						NewTransition.push_back(NEW_MASK, element.second);
						}
					
				}
				//NewTransition.update();
				std::vector< Core::FMTtransition>::iterator transitionIterator = std::find_if(NewTransitions.begin(), NewTransitions.end(), Core::FMTtransitioncomparator(p_Filters.at(transition.getName()).first));
				if (transitionIterator == NewTransitions.end())
					{
					_exhandler->raise(Exception::FMTexc::FMTinvalidAandT,
						"Cannot find transition "+ transition.getName(), "FMTmodel::aggregateTransitions", __LINE__, __FILE__, Core::FMTsection::Transition);
					}
				*transitionIterator += NewTransition;
				}
			}
		for (Core::FMTtransition& transition : NewTransitions)
			{
			transition.update();
			}
	setTransitions(NewTransitions);
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTmodel::aggregateTransitions", __LINE__, __FILE__);
		}
	}

void FMTmodel::aggregateOutputs(const std::map<std::string, std::pair<std::string, Core::FMTmask>>& p_Filters,
	std::vector<Core::FMToutput*>& p_Outputs, const std::vector<std::string>& p_ActionOrdering)
{
	try {
		for (Core::FMToutput* output : p_Outputs)
			{
			std::map<std::string, std::map<Core::FMTmask,std::string>>Dominances;
			std::vector<bool>ValidMask(output->getSources().size(), false);
			size_t sourceId = 0;
			for (const Core::FMToutputsource& source : output->getSources())
				{

				if (source.isAction() &&
					p_Filters.find(source.getAction()) != p_Filters.end() &&
					!p_Filters.at(source.getAction()).first.empty())
					{
					
					const Core::FMTmask& MASK = source.getMask();
					const Core::FMTmask INTERSECT_MASK = MASK.getIntersect(p_Filters.at(source.getAction()).second);
					//Validate the MASK...
					bool gotValidMask = true;
					size_t themeId = 0;
					while (gotValidMask && themeId < themes.size())
						{
						gotValidMask = INTERSECT_MASK.getSubsetCount(themes[themeId]) != 0;
						++themeId;
						}
					if (gotValidMask)
					{
						ValidMask[sourceId] = true;
						const std::string& ACTION = source.getAction();
						const std::string& AGGREGATE = p_Filters.at(source.getAction()).first;
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
								if (!SOURCES.first.isNotThemesSubset(INTERSECT_MASK, themes))
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
			const std::vector<Core::FMToperator> BASE_OPERATORS = output->getOpes();
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
							excludedByMask[mapping.first] = excludedByMask[mapping.first].getUnion(actionMapping.first);
						}
					
					}
				
			}
			sourceId = 0;
			for (const Core::FMToutputsource& source : output->getSources())
				{
				Core::FMToutputsource NewSource(source);
				if (ValidMask[sourceId])
					{
						const Core::FMTmask& SOURCE_MASK = source.getMask();
						const Core::FMTmask INTERSECT_MASK = SOURCE_MASK.getIntersect(p_Filters.at(source.getAction()).second);
						const std::string& ACTION = source.getAction();
						const std::string& AGGREGATE = p_Filters.at(source.getAction()).first;
						if (OutputDominances.at(AGGREGATE).find(INTERSECT_MASK) != OutputDominances.at(AGGREGATE).end() &&
							OutputDominances.at(AGGREGATE).at(INTERSECT_MASK) == ACTION)
						{
							if (!INTERSECT_MASK.isNotThemesSubset(p_Filters.at(source.getAction()).second, themes))
							{
								//const Core::FMTmask INTERSECT_MASK = SOURCE_MASK.getIntersect(p_Filters.at(source.getAction()).second);
								NewSource.setMask(addNewMask(INTERSECT_MASK));
							}
							else {
								NewSource.setMask(addNewMask(p_Filters.at(source.getAction()).second));
							}
							if (!p_Filters.at(source.getAction()).first.empty())
							{
								NewSource.setAction(p_Filters.at(source.getAction()).first);
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
									gotValidMask = MaskCopy.getSubsetCount(themes[themeId]) < INTERSECT_MASK.getSubsetCount(themes[themeId]);
									++themeId;
								}*/
								bool gotNoIntersect = false;
								size_t themeId = 0;
								while (!gotNoIntersect && themeId < themes.size())
								{
									gotNoIntersect = MaskCopy.getSubsetCount(themes[themeId]) < INTERSECT_MASK.getSubsetCount(themes[themeId]);
									++themeId;
								}
								if (gotNoIntersect)
								{
									Core::FMTmask FinalMask(INTERSECT_MASK);
									for (const Core::FMTtheme& THEME : themes)
									{
										if (MaskCopy.getSubsetCount(THEME) < INTERSECT_MASK.getSubsetCount(THEME))
										{
											FinalMask.setExclusiveBits(REST, THEME);
										}
									}
									FinalMask = addNewMask(FinalMask);
									NewSource.setMask(FinalMask);
									REST = REST.getUnion(FinalMask);
								}
								else {
									NewSource.setYield(NULL_YIELD);
								}
								NewSource.setAction(AGGREGATE);
								
						}else {
							NewSource.setYield(NULL_YIELD);
							NewSource.setAction(AGGREGATE);	
						}
					}
				NewSources.push_back(NewSource);
				++sourceId;
				}
			output->setSources(NewSources);
			}

	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTmodel::aggregateOutputs", __LINE__, __FILE__);
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
		_exhandler->raiseFromCatch("", "FMTmodel::addNewMask", __LINE__, __FILE__);
		}
	return baseMask;
}

std::default_random_engine* FMTmodel::getGeneratorPtr() const
	{
	return &m_generator;
	}


void FMTmodel::setDefaultObjects()
	{
	try {
		const bool QUIET_LOG = parameters.getBoolParameter(QUIET_LOGGING);
		if (std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator("_DEATH")) == actions.end())
		{	
			if (!QUIET_LOG)
			{
				_exhandler->raise(Exception::FMTexc::FMTundefineddeathaction,
				"_DEATH","FMTmodel::setDefaultObjects", __LINE__, __FILE__, Core::FMTsection::Action);
			}
			actions.push_back(defaultDeathAction(lifespan, themes));
			//actions.back().passinobject(*this);
		}
		if (std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator("_DEATH")) == transitions.end())
		{
			if (!QUIET_LOG)
			{
				_exhandler->raise(Exception::FMTexc::FMTundefineddeathtransition,
					"_DEATH", "FMTmodel::setDefaultObjects", __LINE__, __FILE__, Core::FMTsection::Transition);
			}
			transitions.push_back(defaultDeathTransition(lifespan, themes));
			//transitions.back().passinobject(*this);
		}
		for (Core::FMTaction& action : actions)
		{
			action.update();
		}
		statictransitionthemes = getStaticTransitionThemes();
	}catch (...)
		{
		_exhandler->raiseFromCatch("","FMTmodel::setDefaultObjects", __LINE__, __FILE__);
		}
	
	}

std::vector<size_t>FMTmodel::getStaticTransitionThemes() const
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
			bestthemes = transition.getStaticThemes(bestthemes);
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
		_exhandler->raiseFromCatch("", "FMTmodel::getStaticTransitionThemes", __LINE__, __FILE__);
		}
	return statics;
	}

std::vector<const Core::FMTtheme*>FMTmodel::getStaticPresolveThemes() const
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
			for (const Core::FMTtheme* theme : locateStaticThemes(constraint))
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
			action.unShrink(themes);
			for (const auto& actit : action)
				{
				std::vector<const Core::FMTtheme*>newstatics;
				for (const Core::FMTtheme* theme :  actit.first.getStaticThemes(fullstatics))
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
		uyields.unShrink(themes);
		for (auto& yieldit : uyields)
			{
			std::vector<const Core::FMTtheme*>newstatics;
			for (const Core::FMTtheme* theme : yieldit.first.getStaticThemes(fullstatics))
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
		_exhandler->raiseFromCatch("", "FMTmodel::getStaticPresolveThemes", __LINE__, __FILE__);
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
	setArea(p_area);
	setDefaultObjects();
	cleanActionsNTransitions();
	setSeed(getParameter(Models::FMTintmodelparameters::SEED));
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
std::vector<Core::FMTactualdevelopment>FMTmodel::getArea(int period,bool beforegrowanddeath) const
    {
    return area;
    }

std::unique_ptr<FMTmodel> FMTmodel::getCopy(int period) const
	{
	return std::unique_ptr<FMTmodel>(new FMTmodel(*this));
	}

void FMTmodel::cleanActionsNTransitions()
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
				const std::vector<Core::FMTtransition>::iterator trn_it = std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator(actions[id].getName()));
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
		_exhandler->raiseFromCatch("","FMTmodel::cleanActionsNTransitions", __LINE__, __FILE__);
		}
	}


bool FMTmodel::useActionSerie() const
{
	bool gotseries = false;
	try {
		for (const Core::FMTaction& action : actions)
		{
			if (action.isPartOfASerie())
			{
				gotseries = true;
				break;
			}
		}
	}catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::useactionseries", __LINE__, __FILE__);
	}

	return gotseries;
}

size_t FMTmodel::getSeriesMaxSize() const
{
	size_t maxsize = 0;
	try {
		if (useActionSerie())
			{
			for (const Core::FMTaction& action : actions)
				{
				maxsize = std::max(maxsize, action.getLargestSerieSize());
				}
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTmodel::getSeriesMaxSize", __LINE__, __FILE__);
		}
	return maxsize;
}







Core::FMTaction FMTmodel::defaultDeathAction(const Core::FMTlifespans& llifespan,
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
			specifier.addBounds(Core::FMTagebounds(Core::FMTsection::Action, std::numeric_limits<int>::max(), intobject.second));
			death_action.push_back(amask, specifier);
		}
		const std::string GCBMaggregate = "~GCBM:" + std::to_string(FMTGCBMDEATHID) + ":Stand Replacing Natural Succession";
		death_action.pushAggregate(GCBMaggregate);
		death_action.shrink();
	return death_action;
	}
Core::FMTtransition FMTmodel::defaultDeathTransition(const Core::FMTlifespans& llifespan,
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

void FMTmodel::setParallelLogger(Logging::FMTlogger& logger)
	{
	try {
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			"FMTmodel::setParallelLogger", __LINE__, __FILE__);
	}catch (...)
		{
		_exhandler->printExceptions("", "FMTmodel::setParallelLogger", __LINE__, __FILE__);
		}
	}


bool FMTmodel::isOptimal() const
{
	return false;
}

double FMTmodel::getObjectiveValue() const
{
	try {
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Calling pure virtual function ",
			"FMTmodel::getObjectiveValue", __LINE__, __FILE__);
	}
	catch (...)
	{
		_exhandler->printExceptions("", "FMTmodel::setParallelLogger", __LINE__, __FILE__);
	}
	return 0.0;
}



void FMTmodel::addOutput(const std::string& name,
	const std::string& maskstring, Core::FMTotar outputtarget,
	std::string action, std::string yield, std::string description, int targetTheme)
	{
	try {
		std::vector<Core::FMToutputsource>sources;
		sources.push_back(Core::FMToutputsource(Core::FMTspec(), Core::FMTmask(maskstring, themes), outputtarget, yield, action,static_cast<int>(outputs.size()),targetTheme));
		std::vector<Core::FMToperator>operators;
		outputs.push_back(Core::FMToutput(name, description,"", sources, operators));
		outputs.shrink_to_fit();
	}
	catch (...)
	{
		_exhandler->printExceptions("", "FMTmodel::addOutput", __LINE__, __FILE__);
	}

	}


void FMTmodel::addYieldHandlersFromPtr(const std::vector<std::unique_ptr<Core::FMTyieldhandler>>& yieldhandlers)
{
	try {
		yields.unShrink(themes);
		for (const std::unique_ptr<Core::FMTyieldhandler>& yldhandler : yieldhandlers)
		{
			yields.push_back(yldhandler->getMask(),yldhandler);
		}
		yields.update();
	}catch(...){
			_exhandler->printExceptions("", "FMTmodel::addYieldHandlersFromPtr", __LINE__, __FILE__);
	}
	
	
}


void FMTmodel::setConstraints(const std::vector<Core::FMTconstraint>& lconstraint)
	{
	try {
		constraints = lconstraint;
		constraints.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printExceptions("", "FMTmodel::setConstraints", __LINE__, __FILE__);
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



void FMTmodel::setArea(const std::vector<Core::FMTactualdevelopment>& ldevs)
    {
	try {
		std::map<Core::FMTdevelopment,double,std::less<Core::FMTdevelopment>>sortedArea;
		for (Core::FMTactualdevelopment adev : ldevs)
		{
			if (adev.getLock() > 0)
				{
				adev = adev.reduceLockToDeath(lifespan);
				}
			std::pair<std::map<Core::FMTdevelopment,double>::iterator, bool> inserted = sortedArea.insert(std::pair<Core::FMTdevelopment,double>(adev,adev.getArea()));
			//sortedArea.insert(adev);
			if (!inserted.second)
				{
				//Core::FMTactualdevelopment* theDev = const_cast<Core::FMTactualdevelopment*>(&*inserted.first);
				//theDev->setArea(theDev->getArea() + adev.getArea());
				inserted.first->second += adev.getArea();
				}
		}
		area.clear();
		area.reserve(sortedArea.size());
		for (const auto& DEV_AREA : sortedArea)
			{
			area.push_back(Core::FMTactualdevelopment(DEV_AREA.first, DEV_AREA.second));
			}
		//std::vector<Core::FMTactualdevelopment>newArea(sortedArea.begin(), sortedArea.end());
		//area.swap(newArea);
		
		/*area.clear();
		area.reserve(ldevs.size());
		for(Core::FMTactualdevelopment adev: ldevs)
		{
			if (adev.getLock()>0)
				{
				adev = adev.reduceLockToDeath(lifespan);
				}
			std::vector<Core::FMTactualdevelopment>::iterator devit = std::find_if(area.begin(), area.end(), Core::FMTactualdevelopmentcomparator(&adev));
			if(devit != area.end())
			{
				devit->setArea(devit->getarea()+adev.getarea());
			}else{
				area.push_back(adev);
			}	
		}
		std::sort(area.begin(), area.end());*/
	}catch (...)
	{
		_exhandler->printExceptions("", "FMTmodel::setArea", __LINE__, __FILE__);
	}


    }
void FMTmodel::setThemes(const std::vector<Core::FMTtheme>& lthemes)
    {
	try {
		themes = lthemes;
		themes.shrink_to_fit();
		//After theme change every masks needs to be reevaluated?.
		statictransitionthemes = getStaticTransitionThemes();
		statictransitionthemes.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printExceptions("", "FMTmodel::setThemes", __LINE__, __FILE__);
	}


    }
void FMTmodel::setActions(const std::vector<Core::FMTaction>& lactions)
    {
	try {
		std::vector<Core::FMTtransition>newtransitions;
		std::vector<Core::FMTaction>newbaseactions;
		for (const Core::FMTaction& action : lactions)
		{
			std::vector<Core::FMTtransition>::const_iterator trn_iterator = std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator(action.getName()));
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
		this->setDefaultObjects();
		actions.shrink_to_fit();
		transitions.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printExceptions("", "FMTmodel::setActions", __LINE__, __FILE__);
	}


    }
void FMTmodel::setTransitions(const std::vector<Core::FMTtransition>& ltransitions)
    {
	try {
		transitions = ltransitions;
		for (Core::FMTtransition& transition : transitions)
		{
			transition.update();
		}
		this->setDefaultObjects();
		statictransitionthemes = getStaticTransitionThemes();
		statictransitionthemes.shrink_to_fit();
		transitions.shrink_to_fit();
	}catch (...)
		{
			_exhandler->printExceptions("", "FMTmodel::setTransitions", __LINE__, __FILE__);
		}

    }
void FMTmodel::setYields(const Core::FMTyields& lylds)
    {
	try {
		yields = lylds;
		yields.update();
		yields.setModel(this);
	}catch (...)
		{
			_exhandler->printExceptions("", "FMTmodel::setYields", __LINE__, __FILE__);
		}
    }
void FMTmodel::setLifespan(const Core::FMTlifespans& llifespan)
    {
	try {
		lifespan = llifespan;
		lifespan.update();
		this->setDefaultObjects();
	}catch (...)
	{
		_exhandler->printExceptions("", "FMTmodel::setLifespan", __LINE__, __FILE__);
	}

    }

void FMTmodel::setName(const std::string& newname)
	{
	name = newname;
	}

void FMTmodel::setOutputs(const std::vector<Core::FMToutput>& newoutputs)
	{
	try {
		outputs = newoutputs;
		outputs.shrink_to_fit();
	}catch (...)
	{
		_exhandler->printExceptions("", "FMTmodel::setOutputs", __LINE__, __FILE__);
	}

	}


std::vector<const Core::FMTtheme*> FMTmodel::locateStaticThemes(const Core::FMToutput& output, bool ignoreoutputvariables) const
{
	std::vector<const Core::FMTtheme*> bestthemes;
	try {
		const std::vector<const Core::FMTtheme*>transitionstatic = locateStaticTransitionsThemes();
		for (const Core::FMTtheme* theme : output.getStaticThemes(themes, yields, ignoreoutputvariables))
			{
			if (std::find(transitionstatic.begin(), transitionstatic.end(), theme) != transitionstatic.end())
				{
				bestthemes.push_back(theme);
				}
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("","FMTmodel::locateStaticThemes", __LINE__, __FILE__);
		}
	return bestthemes;
}

std::vector<const Core::FMTtheme*>FMTmodel::locateStaticTransitionsThemes() const
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
		_exhandler->raiseFromCatch("", "FMTmodel::locateStaticTransitionsThemes", __LINE__, __FILE__);
	}
	return bestthemes;

}

std::vector<const Core::FMTtheme*>FMTmodel::locateNodeStaticThemes(const Core::FMToutputnode& node,
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
		if (node.source.isVariable())
		{
			if (!ignoreoutputvariables)
			{
				statics = node.source.getMask().getStaticThemes(statics);
			}
			const std::string SOURCE_YIELD_VALUE = node.source.getYield();
			const std::string FACTOR_YIELD_VALUE = node.factor.getYield();
			for (const std::string& yldbound : node.source.getYlds())
			{
				if (yields.isYld(yldbound))
				{
					yieldstolookat.push_back(yldbound);
				}
			}
			if (!SOURCE_YIELD_VALUE.empty())
			{
				yieldstolookat.push_back(SOURCE_YIELD_VALUE);
			}
			if (!FACTOR_YIELD_VALUE.empty())
			{
				yieldstolookat.push_back(FACTOR_YIELD_VALUE);
			}
		}
		std::vector<std::pair<Core::FMTmask,std::unique_ptr<Core::FMTyieldhandler>>>::const_iterator handlerit = yields.begin();
		while (handlerit != yields.end() && !yieldstolookat.empty())
		{
			std::vector<std::string>::const_iterator yieldit = yieldstolookat.begin();
			while (yieldit != yieldstolookat.end() && !handlerit->second->containsYield(*yieldit))
			{
				++yieldit;
			}
			if (yieldit != yieldstolookat.end())
			{
				statics = Core::FMTmask(std::string(handlerit->first), themes).getStaticThemes(statics);
				yieldstolookat.erase(yieldit);
			}
			++handlerit;
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::locateNodeStaticThemes", __LINE__, __FILE__);
	}
	return statics;


}



std::vector<const Core::FMTtheme*> FMTmodel::locateStaticThemes(const Core::FMToutputnode& node, bool ignoreoutputvariables) const
{
	std::vector<const Core::FMTtheme*>statics;
	try {
		statics = locateStaticTransitionsThemes();
		statics = locateNodeStaticThemes(node, ignoreoutputvariables, statics);
		
	}catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::locateStaticThemes", __LINE__, __FILE__);
	}
	return statics;

}

std::vector<const Core::FMTtheme*> FMTmodel::locateDynamicThemes(const Core::FMToutput& output, bool ignoreoutputvariables) const
{
	std::vector<const Core::FMTtheme*>dynamicthemes;
	try {
		const std::vector<const Core::FMTtheme*>staticthemes = locateStaticThemes(output, ignoreoutputvariables);
		for (const Core::FMTtheme& theme : themes)
			{
			if (std::find(staticthemes.begin(), staticthemes.end(), &theme)==staticthemes.end())
				{
				dynamicthemes.push_back(&theme);
				}
			}
	}catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::locateDynamicThemes", __LINE__, __FILE__);
	}
	return dynamicthemes;
}


Core::FMTmask FMTmodel::getDynamicMask(const Core::FMToutput& output, bool ignoreoutputvariables) const
	{
	Core::FMTmask selection;
	try {
		const std::vector<const Core::FMTtheme*>staticcthemes = locateStaticThemes(output, ignoreoutputvariables);
		std::string basename;
		for (const Core::FMTtheme& theme : themes)
			{
			
			basename += "? ";
			}
		
		basename.pop_back();
		const Core::FMTmask submask(basename,themes);
		 boost::dynamic_bitset<uint8_t>bits = submask.getBitsetReference();
		for (const Core::FMTtheme* theme : staticcthemes)
			{
			const size_t start = static_cast<size_t>(theme->getStart());
			for (size_t bitid = start; bitid < (theme->size() + start); ++bitid)
				{
				bits[bitid] = false;
				}
			}
		selection = Core::FMTmask(basename, bits);
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTmodel::getDynamicMask", __LINE__, __FILE__);
		}
	return selection;
	}

Core::FMTmask FMTmodel::getDynamicMask(const Core::FMToutputnode& node, bool ignoreoutputvariables) const
{
	Core::FMTmask selection;
	try {
		std::vector<const Core::FMTtheme*>staticcthemes = locateStaticTransitionsThemes();
		const std::vector<const Core::FMTtheme*> NODE_THEMES = locateNodeStaticThemes(node, ignoreoutputvariables, staticcthemes);
		
		std::string basename;
		for (const Core::FMTtheme& theme : themes)
		{

			basename += "? ";
		}

		basename.pop_back();
		//return Core::FMTmask(basename, themes);
		const Core::FMTmask submask(basename, themes);
		 boost::dynamic_bitset<uint8_t>bits = submask.getBitsetReference();
		for (const Core::FMTtheme* theme : NODE_THEMES)
		{
			
			const size_t start = static_cast<size_t>(theme->getStart());
			for (size_t bitid = start; bitid < (theme->size() + start); ++bitid)
			{
				bits[bitid] = false;
			}
		}
		selection = Core::FMTmask(basename, bits);
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::getDynamicMask", __LINE__, __FILE__);
	}
	return selection;

}

bool FMTmodel::isStaticNode(const Core::FMToutputnode& node, double ratioofset) const
{
	try {
		if (node.source.isInventory()&&!node.source.isAction())
		{
			for (const size_t& staticid : statictransitionthemes)
			{
				const double nvalues = static_cast<double>(node.source.getMask().getSubsetCount(themes.at(staticid)));
				const double themesize = static_cast<double>(themes.at(staticid).size());
				if ((nvalues/themesize)<=ratioofset)
				{
					return true;
				}

			}
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTmodel::isStaticNode", __LINE__, __FILE__);
		}
	return false;
}

Core::FMTmask FMTmodel::getStaticMask(const Core::FMToutputnode& node, bool ignoreoutputvariables) const
{
	Core::FMTmask selection;
	try {
		const Core::FMTmask dymask =this->getDynamicMask(node, ignoreoutputvariables);
		const Core::FMTmask intersection = node.source.getMask();
		selection = dymask.getUnion(intersection);
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::getStaticMask", __LINE__, __FILE__);
	}
	return selection;
}


void FMTmodel::validateListSpec(const Core::FMTspec& specifier) const
	{
	try {
		for (const std::string& yldname : specifier.getYlds())
		{
			if (!yields.isYld(yldname))
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_yield,yldname,
					"FMTmodel::validateListSpec", __LINE__, __FILE__);
			}
		}
	}catch (...)
		{
			_exhandler->raiseFromCatch("","FMTmodel::validateListSpec", __LINE__, __FILE__);
		}
	}


bool FMTmodel::isValid()
    {
	try {
		//this->setSection(Core::FMTsection::Landscape);
		for (const Core::FMTtheme& theme : themes)
		{
			if (theme.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTempty_theme,
					"for theme id: " + std::to_string(theme.getId()),
					"FMTmodel::isValid", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
		}
		//this->setSection(Core::FMTsection::Area);
		for (const Core::FMTactualdevelopment& development : area)
		{
			std::string name = std::string(development.getMask());
			Core::FMTtheme::validate(themes, name);
		}
		//this->setSection(Core::FMTsection::Yield);
		this->validateListMasks(yields);

		//this->setSection(Core::FMTsection::Lifespan);
		this->validateListMasks(lifespan);

		//this->setSection(Core::FMTsection::Action);
		for (const Core::FMTaction& action : actions)
		{
			this->validateListMasks(action);
			for (const auto& specobject : action)
			{
				validateListSpec(specobject.second);
			}
		}
		//this->setSection(Core::FMTsection::Transition);
		for (const Core::FMTtransition& transition : transitions)
		{
			this->validateListMasks(transition);
			for (const auto& specobject : transition)
			{
				validateListSpec(specobject.second);
			}
		}
		if (actions.size() != transitions.size())
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalidAandT, "Model: " + name,
				"FMTmodel::isValid",__LINE__, __FILE__);
		}
		for (size_t id = 0; id < actions.size(); ++id)
		{
			if (actions[id].getName() != transitions[id].getName())
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_action,
					"Model: " + name + " " + actions[id].getName(),
					"FMTmodel::isValid", __LINE__, __FILE__);
			}
		}
		//this->setSection(Core::FMTsection::Outputs);
		for (const Core::FMToutput& output : outputs)
		{
			//Need a validate output function
			for (const Core::FMToutputsource& source : output.getSources())
			{
				if (source.isVariable())
				{
					std::string name = std::string(source.getMask());
					Core::FMTtheme::validate(themes, name, "for output " + output.getName());
					const std::string actionname = source.getAction();
					if (!actionname.empty())//need to check the targeted action!
					{

					}
				}
				validateListSpec(source);
			}
		}
		//this->setSection(Core::FMTsection::Empty);
	}catch (...)
	{
		_exhandler->printExceptions("", "FMTmodel::isValid", __LINE__, __FILE__);
	}

    return true;
    }

void  FMTmodel::clearActionsCache()
	{
		for (auto& action:actions)
		{
			action.clearCache();
		}
	}

void  FMTmodel::clearYieldCache()
	{
		yields.clearCache();
	}

void  FMTmodel::clearTransitionCache()
	{
		for (auto& transition:transitions)
		{
			transition.clearCache();
		}
	}

void FMTmodel::clearCache()
	{
		clearTransitionCache();
		clearYieldCache();
		clearActionsCache();
	}

Core::FMTmask FMTmodel::getBaseMask(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
	{
	Core::FMTmask baseMask( boost::dynamic_bitset<uint8_t>(area.begin()->getMask().size(), false));
	try {
		optionaldevelopments.insert(optionaldevelopments.end(), area.begin(), area.end());
		Core::FMTmask areamask( boost::dynamic_bitset<uint8_t>(area.begin()->getMask().size(), false));
		for (const Core::FMTactualdevelopment& development : optionaldevelopments)
			{
			areamask = areamask.getUnion(development.getMask());
			}
		size_t trid = 0;
		std::vector<bool>jumptransitions;
		for (const Core::FMTtransition& transition : transitions)
		{
			if (transition.size() == 1 &&
				actions.at(trid).size()==1&&
				transition.begin()->second.getMaskTrans().size()==1&&
				std::string(actions.at(trid).begin()->first)==std::string(transition.begin()->first)&&
				std::string(transition.begin()->first)==std::string(transition.begin()->second.getMaskTrans().begin()->getMask())&&
				!transition.begin()->second.getMaskTrans().begin()->getMask().isSubsetOf(areamask))//scrap weird thing
				{
				jumptransitions.push_back(true);
			}else {
				for (const auto& transitionobject : transition)
				{
					const Core::FMTmask source(std::string(transitionobject.first),themes);
					for (const Core::FMTtransitionmask& fork : transitionobject.second.getMaskTrans())
					{
						const Core::FMTmask maskwithoutaggregates = fork.getMask().removeAggregates(themes);
						baseMask = baseMask.getUnion(maskwithoutaggregates);
					}
				}
				jumptransitions.push_back(false);
			}
			++trid;
		}
		if (!getParameter(FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES))
		{
			baseMask = baseMask.getUnion(areamask);
		}else {
			trid = 0;
			for (const Core::FMTtransition& transition : transitions)
			{
				if (!jumptransitions.at(trid))
				{
					for (const auto& transitionobject : transition)
					{
						const Core::FMTmask source(std::string(transitionobject.first), themes);
						baseMask = baseMask.getUnion(source.removeAggregates(themes, true));
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
						baseMask = baseMask.getUnion(opq.removeAggregates(themes, true));
					}
				}
				++acid;
			}
			for (const auto& yieldobject : yields)
			{
				const Core::FMTmask source(std::string(yieldobject.first), themes);
				baseMask = baseMask.getUnion(source.removeAggregates(themes, true));
			}
			for (const auto& lifespanobject : lifespan)
			{
				const Core::FMTmask source(std::string(lifespanobject.first), themes);
				baseMask = baseMask.getUnion(source.removeAggregates(themes, true));
			}
			for (const Core::FMTconstraint& constraint : constraints)
			{
				for (const Core::FMToutputsource& source : constraint.getSources())
				{
					if (source.isVariable())
					{
						baseMask = baseMask.getUnion(source.getMask().removeAggregates(themes, true));
					}
				}
			}
			 boost::dynamic_bitset<uint8_t>bits(baseMask.size(),false);
			const boost::dynamic_bitset<uint8_t>& areamaskref = areamask.getBitsetReference();
			const boost::dynamic_bitset<uint8_t>& basemaskref = baseMask.getBitsetReference();
			for (const Core::FMTtheme& theme : themes)
				{
				const size_t start = static_cast<size_t>(theme.getStart());
				if (baseMask.getSubsetCount(theme) == 0)
					{ 
					const size_t areacount = areamask.getSubsetCount(theme);
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
			baseMask = baseMask.getUnion(Core::FMTmask(bits));
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("","FMTmodel::getBaseMask", __LINE__, __FILE__);
		}
	return baseMask;
	}

Core::FMTmaskfilter FMTmodel::getPostsolveFilter(const std::vector<Core::FMTtheme>& originalthemes,const Core::FMTmask& devmask) const
{
	try {
		const Core::FMTmask presolvemask = getSelectedMask(originalthemes);
		const Core::FMTmask emptythemes = devmask.getPostsolveMask(presolvemask, originalthemes);
		return Core::FMTmaskfilter(presolvemask,emptythemes);
	}catch (...)
	{
		_exhandler->printExceptions("for " + name, "FMTmodel::getPostsolveFilter", __LINE__, __FILE__);
	}
	return Core::FMTmaskfilter();
}

Core::FMTmaskfilter FMTmodel::getPresolveFilter(const std::vector<Core::FMTtheme>& originalthemes) const
{
	try {
		const Core::FMTmask presolvemask = getSelectedMask(originalthemes);
		return Core::FMTmaskfilter(presolvemask, presolvemask);
	}
	catch (...)
	{
		_exhandler->printExceptions("for " + name, "FMTmodel::getPresolveFilter", __LINE__, __FILE__);
	}
	return Core::FMTmaskfilter();
}

Core::FMTmask FMTmodel::getSelectedMask(const std::vector<Core::FMTtheme>& originalthemes) const
	{
	Core::FMTmask newmask;
	size_t newmasksize = 0;
	for (const Core::FMTtheme& theme : originalthemes)
		{
		newmasksize += theme.size();
		}
	//const boost::dynamic_bitset<uint8_t>& basewithoutpresolve = basedevelopment.getMask().getBitsetReference();
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
			const std::vector<std::string>& prsolvedvalues = themes.at(presolvedthemeid).getBaseAttributes();
			size_t foundcount = 0;
			std::vector<bool>themebits(originaltheme.size(),false);
			size_t bitid = 0;
			for (const std::string& themevalues : originaltheme.getBaseAttributes())
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
			*_logger << "Count " << newmask.getSubsetCount(theme) << "\n";
			if (newmask.getSubsetCount(theme) == 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
					"Non valid base presolve mask for model: " + name + " for theme id: " + std::to_string(theme.getId()),
					"FMTmodel::getSelectedMask", __LINE__, __FILE__);
			}
		}*/


	}catch (...)
		{
		_exhandler->raiseFromCatch("","FMTmodel::getSelectedMask", __LINE__, __FILE__);
		}
	return newmask;
	}

FMTmodel FMTmodel::basePresolve() const
{
	std::unique_ptr<FMTmodel>mdlptr;
	try {
		mdlptr = presolve(area);

	}catch (...)
		{
		_exhandler->printExceptions("for " + name, "FMTmodel::basePresolve", __LINE__, __FILE__);
		}
	return *mdlptr;
}


std::unique_ptr<FMTmodel> FMTmodel::presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
	{
	std::unique_ptr<FMTmodel>presolvedModel(new FMTmodel());
	int presolvepass = getParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS);
	try {
		const bool QUIET_LOG = parameters.getBoolParameter(QUIET_LOGGING);
		if (!QUIET_LOG)
			_logger->logWithLevel("Presolving " + getName() + "\n", 1);
		presolvedModel->setName(getName());
		presolvedModel->parameters = parameters;
		Core::FMTmaskfilter oldpresolvefilter(getBaseMask(optionaldevelopments));
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
				// test gab ici
				if (!PresolvedTheme.empty())
				{ 
					keptthemeid.insert(oldthemeid);
					newthemes.push_back(PresolvedTheme);
				}
				++oldthemeid;
			}
			newsize += newthemes.size();
			if (!newfilter.emptyFlipped())
			{
				newarea.reserve(presolvedModel->area.size());
				boost::unordered_map<Core::FMTmask,Core::FMTmask>topresolve;
				topresolve.reserve(presolvedModel->area.size());
				for (const Core::FMTactualdevelopment& development : presolvedModel->area)
				{
					const Core::FMTmask& devmask = development.getMask();
					Core::FMTactualdevelopment newDev(development);
					boost::unordered_map<Core::FMTmask, Core::FMTmask>::const_iterator mskit = topresolve.find(devmask);
					if (mskit != topresolve.end())
					{
						newDev.setMask(mskit->second);
					}
					else {
						const Core::FMTmask presolvedmask = devmask.presolve(newfilter, newthemes);
						topresolve[devmask] = presolvedmask;
						newDev.setMask(presolvedmask);
					}
					
					if (getParameter(FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES))
					{
						std::vector<Core::FMTactualdevelopment>::iterator devit = std::find_if(newarea.begin(), newarea.end(), Core::FMTactualdevelopmentcomparator(&newDev));
						if (devit != newarea.end())
						{
							devit->setArea(devit->getArea() + newDev.getArea());
						}
						else {
							newarea.push_back(newDev);
						}
					}else {
						newarea.push_back(newDev);
						}
					}
			}else {
				newarea = presolvedModel->area;
			}
			newsize += newarea.size();
			//reduce the number of actions and presolve the actions
			const std::vector<const Core::FMTtheme*>maskthemes = newfilter.getSelectedThemes(presolvedModel->themes);
			size_t actionIds = 0;
			for (Core::FMTaction& PresolvedAction : presolvedModel->actions)
			{
				
				if (validActions[actionIds])
				{
					validActions[actionIds] = false;
					const Core::FMTmask TESTED_MASK = PresolvedAction.getUnion(presolvedModel->themes);
					if (newfilter.canPresolve(TESTED_MASK, maskthemes) && !PresolvedAction.notUse())
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
					std::vector<Core::FMTaction>::const_iterator actionIt = std::find_if(
						presolvedModel->actions.begin(), 
						presolvedModel->actions.end(), 
						Core::FMTActionComparator(presolvedTransition.getName()));
					const size_t ACTIONm_location = std::distance(presolvedModel->actions.cbegin(), actionIt);
					const Core::FMTmask TESTED_MASK = presolvedTransition.getUnion(presolvedModel->themes);
					if (actionIt != presolvedModel->actions.end() && 
						validActions[ACTIONm_location] && 
						newfilter.canPresolve(TESTED_MASK, maskthemes))
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
				output.changeSourcesId(keptoutputid,keptthemeid);
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
				int originalId = -1;
				if (oriit != constraintsIds.end())
					{
					originalId = *oriit;
					}
				if (validConstraints[constraintId])
				{
					validConstraints[constraintId] = false;
					presolvedConstraint.presolveRef(newfilter, presolvedModel->themes, maskthemes, newthemes, presolvedModel->actions,
													validActions, presolvedModel->yields);
					if (!presolvedConstraint.outputEmpty() ||
						(presolvedConstraint.isObjective() && presolvedConstraint.isGoal()))
					{
						presolvedConstraint.changeSourcesId(keptoutputid, keptthemeid);
						if (presolvedConstraint.canBeTurnedToYields())
						{
							presolvedConstraint.turnToYieldsAndActions(newthemes, presolvedModel->actions,validActions, newyields, originalId);
						}
						else if (presolvedConstraint.canBeTurnedToYieldsBasedOnTransitions())
						{
							presolvedConstraint.turnToYieldsBasedOnTransition(newthemes, presolvedModel->transitions, presolvedModel->actions,validActions, newyields, originalId);
						}
						else {
							newConstraintsIds.push_back(originalId);
							validConstraints[constraintId] = true;
						}
					}
					else if (presolvedConstraint.isSpatial())
					{
						newConstraintsIds.push_back(originalId);
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
				if (!presolvedconstraint.outputEmpty()||
					(presolvedconstraint.isObjective() && presolvedconstraint.isGoal()))
				{
					presolvedconstraint.changeSourcesId(keptoutputid, keptthemeid);
					if (presolvedconstraint.canBeTurnedToYields())
					{
						presolvedconstraint.turnToYieldsAndActions(newthemes, newactions, newyields,originalid);
					}else if(presolvedconstraint.canBeTurnedToYieldsBasedOnTransitions())
					{
						presolvedconstraint.turnToYieldsBasedOnTransition(newthemes, newtransitions ,newactions, newyields,originalid);
					}
					else{
						newconstraintsids.push_back(originalid);
						newconstraints.push_back(presolvedconstraint);
					}
				}else if (presolvedconstraint.isSpatial())
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
	if (!QUIET_LOG)
	{ 
		_logger->logWithLevel("Presolve stopped after " + std::to_string(getParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS) - presolvepass) + " iterations\n", 1);
		_logger->logWithLevel("Developments "+std::to_string(presolvedModel->area.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->area.size())-static_cast<int>(area.size())) + "), "
					+"Themes "+std::to_string(presolvedModel->themes.size())+"(" + std::to_string(static_cast<int>(presolvedModel->themes.size())-static_cast<int>(themes.size())) + "), "
					+"Yields "+std::to_string(presolvedModel->yields.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->yields.size())-static_cast<int>(yields.size())) + "), "
					+"Actions "+std::to_string(presolvedModel->actions.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->actions.size())-static_cast<int>(actions.size())) + "), "
					+"Transitions "+std::to_string(presolvedModel->transitions.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->transitions.size())-static_cast<int>(transitions.size())) + "), "
					+"Outputs "+std::to_string(presolvedModel->outputs.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->outputs.size())-static_cast<int>(outputs.size())) + "), "
					+"Constraints "+std::to_string(presolvedModel->constraints.size()) + "(" + std::to_string(static_cast<int>(presolvedModel->constraints.size()) - static_cast<int>(constraints.size())) + ") and "
					+"Elements "+ std::to_string(newsize)+"("+std::to_string(static_cast<int>(newsize)- static_cast<int>(originalsize)) +")\n",1);
	}
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
		_exhandler->printExceptions("for "+name+"at presolve pass "+std::to_string(presolvepass),"FMTmodel::presolve", __LINE__, __FILE__);
		}
	return presolvedModel;
	}

void FMTmodel::postSolve(const FMTmodel& originalbasemodel)
	{
	try{
		*this = FMTmodel(originalbasemodel);
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTmodel::postSolve", __LINE__, __FILE__);
		}
	}

Core::FMTschedule FMTmodel::presolveSchedule(const Core::FMTschedule& originalbaseschedule,
	const FMTmodel& originalbasemodel) const
	{
	Core::FMTschedule newSchedule;
	try {
		const Core::FMTmaskfilter newfilter = getPresolveFilter(originalbasemodel.getThemes());
		newSchedule = originalbaseschedule.presolve(newfilter, this->themes, this->actions);
	}catch (...)
		{
		_exhandler->raiseFromCatch("","FMTmodel::presolveSchedule", __LINE__, __FILE__);
		}
	return newSchedule;
	}


FMTmodelstats FMTmodel::getModelStats() const
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
	const FMTmodelstats basestats = this->getModelStats();
	const FMTmodelstats rhsstats = rhs.getModelStats();
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
				actualdev->setArea(actualdev->getArea() + dev.getArea());
			}
		}
		std::vector<Core::FMTaction>finalactions = actions;
		std::vector<Core::FMTtransition>finaltransitions = transitions;
		size_t id = 0;
		for (const Core::FMTaction& action : rhs.actions)
		{
			std::vector<Core::FMTaction>::iterator actionitr = std::find_if(finalactions.begin(), finalactions.end(), Core::FMTActionComparator(action.getName()));
			if (actionitr == finalactions.end())
			{
				finalactions.push_back(action);
				finaltransitions.push_back(rhs.transitions.at(id));
			}
			else {
				Core::FMTaction rhsaction(action);
				actionitr->unShrink(themes);
				rhsaction.unShrink(newthemes);
				actionitr->push_back(rhsaction);
				std::vector<Core::FMTtransition>::iterator transitionitr = std::find_if(finaltransitions.begin(), finaltransitions.end(), Core::FMTtransitioncomparator(action.getName()));
				if (transitionitr != transitions.end())
				{
					Core::FMTtransition rhstransition(rhs.transitions.at(id));
					rhstransition.unShrink(rhs.themes);
					transitionitr->unShrink(themes);
					transitionitr->push_back(rhstransition);
				}
			}
			++id;
		}
		std::vector<Core::FMToutput>finaloutputs = outputs;
		for (const Core::FMToutput& output : rhs.outputs)
		{
			if (std::find_if(finaloutputs.begin(), finaloutputs.end(), Core::FMToutputcomparator(output.getName())) == finaloutputs.end())
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
				if (std::find_if(finalconstraints.begin(), finalconstraints.end(), Core::FMToutputcomparator(constraint.getName())) == finalconstraints.end())
				{
					finalconstraints.push_back(constraint);
				}
			}
		}
		Core::FMTyields newyields(yields);
		Core::FMTyields rhsyields(rhs.yields);
		newyields.unShrink(themes);
		rhsyields.unShrink(rhs.themes);
		newyields.push_back(rhs.yields);
		newyields.update();
		Core::FMTlifespans newlifespan(lifespan);
		Core::FMTlifespans rhslifespan(rhs.lifespan);
		newlifespan.unShrink(themes);
		rhslifespan.unShrink(rhs.themes);
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
				_exhandler->printExceptions("", "FMTmodel::push_back", __LINE__, __FILE__);
			}


	}

double FMTmodel::getInitialArea() const
	{
	double totalarea = 0;
	for (const Core::FMTactualdevelopment& basedev : area)
		{
		totalarea += basedev.getArea();
		}
	return totalarea;
	}

void FMTmodel::setAreaPeriod(const int& period)
	{
	for (Core::FMTactualdevelopment& basedev : area)
		{
		basedev.setPeriod(period);
		}
	}

int FMTmodel::getAreaPeriod() const
	{
	int period = 0;
	if (!area.empty())
		{
		period = area.cbegin()->getPeriod();
		}
	return period;
	}

Core::FMTschedule FMTmodel::getPotentialSchedule(std::vector<Core::FMTactualdevelopment> toRemove,
	std::vector<Core::FMTactualdevelopment> selection, bool withlock) const
{
	int period = 1;
	if (!selection.empty())
	{
		period = selection.back().getPeriod();
	}
	Core::FMTschedule schedule(period,*this, withlock);

	try {
		boost::unordered_set<Core::FMTdevelopment>nottoprocess(toRemove.begin(), toRemove.end());
		size_t actionid = 0;
		for (const Core::FMTaction& action : actions)
			{
			std::vector<Core::FMTactualdevelopment>newselection;
			for (const Core::FMTactualdevelopment& actdev : selection)
				{
				if (nottoprocess.find(actdev)==nottoprocess.end()&&actdev.operable(action, yields))
					{
					schedule.addEvent(actdev, 1.0, action);
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
		_exhandler->raiseFromCatch("", " FMTmodel::getpotentialschedule", __LINE__, __FILE__);
	}
	return schedule;
}

std::vector<Core::FMTschedule>FMTmodel::setUpSchedulesForBuild(const std::vector<Core::FMTschedule>& schedules) const
{
	std::vector<Core::FMTschedule>newshedules;
try{
	bool gotemptyschedule = true;
	for (int period = 1; period <= parameters.getIntParameter(LENGTH);++period)
		{
		Core::FMTschedule newSchedule;
		for (const Core::FMTschedule& schedule : schedules)
			{
			if (schedule.getPeriod()==period)
				{
				newSchedule = schedule;
				gotemptyschedule = false;
				break;
				}
			}
		newshedules.push_back(newSchedule);
		}
	if (parameters.getBoolParameter(FORCE_PARTIAL_BUILD)&&gotemptyschedule)
		{
		_exhandler->raise(Exception::FMTexc::FMTignore,
			"Building natural growth graph for "+getName(),
			"FMTmodel::setUpSchedulesForBuild", __LINE__, __FILE__);
		}

	if (!parameters.getBoolParameter(FORCE_PARTIAL_BUILD)&&!gotemptyschedule)
		{
		_exhandler->raise(Exception::FMTexc::FMTignore,
			"FMTschedule will be ignored for " + getName(),
			"FMTmodel::setUpSchedulesForBuild", __LINE__, __FILE__);
		}


}catch (...)
	{
	_exhandler->raiseFromCatch("", "FMTmodel::setUpSchedulesForBuild", __LINE__, __FILE__);
	}
return newshedules;
}



bool FMTmodel::doPlanning(const bool& solve, std::vector<Core::FMTschedule> schedules)
	{
	bool optimal_solved = false;
	try{
		const bool QUIET_LOG = parameters.getBoolParameter(QUIET_LOGGING);
		const int presolve_iterations = parameters.getIntParameter(PRESOLVE_ITERATIONS);
		std::unique_ptr<FMTmodel> presolved_model;
		if(presolve_iterations>0)
		{
			const std::chrono::time_point<std::chrono::high_resolution_clock>presolvestart = getClock();
			presolved_model = this->presolve(area);
			//Parser::FMTmodelparser mparser;
			//mparser.write(*presolved_model,"C:/Users/admlocal/Desktop/test/");
			if (!QUIET_LOG)
				_logger->logWithLevel("Presolved " + getName() + " " +getDurationInSeconds(presolvestart) + "\n", 1);
		}else{
			presolved_model = this->clone();
		}
		std::vector<Core::FMTschedule> presolved_schedules;
		if(presolve_iterations > 0 && !schedules.empty())
		{
			for (const Core::FMTschedule schedule : schedules )
			{
				presolved_schedules.push_back(presolved_model->presolveSchedule(schedule,*this));
			}
		}else{
			presolved_schedules = schedules;
		}
		presolved_schedules = setUpSchedulesForBuild(presolved_schedules);
		const std::chrono::time_point<std::chrono::high_resolution_clock> buildstart = getClock();
		presolved_model->build(presolved_schedules);

		if (!QUIET_LOG)
			_logger->logWithLevel("Builded " + getName() +" "+getDurationInSeconds(buildstart)+ "\n", 1);
		if (solve)
		{
			const std::chrono::time_point<std::chrono::high_resolution_clock> solverstart = getClock();
			optimal_solved = presolved_model->solve();
			if (!QUIET_LOG)
				_logger->logWithLevel("Solved " + getName() + " " + getDurationInSeconds(solverstart) + "\n", 1);
		}
		if (parameters.getBoolParameter(POSTSOLVE) && presolve_iterations > 0)
			{
			const std::chrono::time_point<std::chrono::high_resolution_clock>postsolvestart = getClock();
			presolved_model->postSolve(*this);
			if (!QUIET_LOG)
				_logger->logWithLevel("Postsolved " + getName() + " " + getDurationInSeconds(postsolvestart) + "\n", 1);
			}
		this->swapPtr(presolved_model);
	}catch(...){
		_exhandler->raiseFromCatch(getName(), " FMTmodel::doplanning", __LINE__, __FILE__);
	}
	return optimal_solved;
	}

std::vector<Core::FMTconstraint> FMTmodel::getReplanningConstraints(const std::string& modeltype, const std::vector<Core::FMTconstraint>& localconstraints, const int& period) const
{
	std::vector<Core::FMTconstraint>newconstraints(localconstraints.begin(), localconstraints.end());
	try {
		size_t constraintid = 0;
		for (const Core::FMTconstraint& constraint : localconstraints)
		{
			if (constraint.isSetFrom(modeltype))
			{
				const double value = getOutput(constraint, period, Core::FMToutputlevel::totalonly).at("Total");
				newconstraints[constraintid] = constraint.setFrom(modeltype,value);
			}
			++constraintid;
		}
	}catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::getReplanningConstraints", __LINE__, __FILE__);
	}
	return newconstraints;
}
std::map<std::string, std::vector<std::vector<double>>>FMTmodel::getOutputsFromPeriods(const std::vector<Core::FMToutput>& theoutputs,
	const int& firstPeriod, const int& lastPeriod, Core::FMToutputlevel level) const
	{
	std::map<std::string, std::vector<std::vector<double>>>outs;
	try {

		for (size_t outid = 0; outid < theoutputs.size(); ++outid)
			{
			size_t periodid = 0;
			for (int period = firstPeriod; period <= lastPeriod;++period)
				{
				for (const auto& values : this->getOutput(theoutputs.at(outid), period, level))
					{
					if (outs.find(values.first)==outs.end())
						{
						outs[values.first] = std::vector<std::vector<double>>(theoutputs.size(),std::vector<double>((lastPeriod-firstPeriod)+1,std::numeric_limits<double>::quiet_NaN()));
						}
					outs[values.first][outid][periodid] = values.second;
					}
				++periodid;
				}
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTmodel::getOutputsFromPeriods", __LINE__, __FILE__);
		}
	return outs;
	}

std::unique_ptr<FMTmodel> FMTmodel::clone() const
	{
	return std::unique_ptr<FMTmodel>(new FMTmodel(*this));
	}

Core::FMTschedule FMTmodel::getSolution(int period, bool withlock) const
	{
	return Core::FMTschedule();
	}

std::map<std::string, double> FMTmodel::getOutput(const Core::FMToutput& output,
	int period, Core::FMToutputlevel level) const
{
	return std::map<std::string, double>();
}

bool FMTmodel::setParameter(const FMTintmodelparameters& key, const int& value)
{
	try{
		if (parameters.setIntParameter(key, value))
		{
			return (true);
		}
	}catch(...){
		_exhandler->printExceptions("", "FMTmodel::setParameter", __LINE__, __FILE__);
	}
	return false;
}

bool FMTmodel::setParameter(const FMTdblmodelparameters& key, const double& value)
{
	try{
		if (parameters.setDblParameter(key,value)) return (true);
	}catch(...){
		_exhandler->printExceptions("", "FMTmodel::setParameter", __LINE__, __FILE__);
	}
	return false;
}

bool FMTmodel::setParameter(const FMTboolmodelparameters& key, const bool& value)
{
	try{
		if (parameters.setBoolParameter(key,value)) return (true);
	}catch(...){
		_exhandler->printExceptions("", "FMTmodel::setParameter", __LINE__, __FILE__);
	}
	return false;
}

bool FMTmodel::setParameter(const FMTstrmodelparameters& p_key, const std::string& p_value)
{
	try {
		if (parameters.setStrParameter(p_key, p_value)) return (true);
	}
	catch (...) {
		_exhandler->printExceptions("", "FMTmodel::setParameter", __LINE__, __FILE__);
	}
	return false;
}

int FMTmodel::getParameter(const FMTintmodelparameters& key)const
{
	int value;
	try{
		value = parameters.getIntParameter(key);
	}catch(...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::getParameter", __LINE__, __FILE__);
	}
	return value;
}

double FMTmodel::getParameter(const FMTdblmodelparameters& key)const
{
	double value;
	try{
		value = parameters.getDblParameter(key);
	}catch(...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::getParameter", __LINE__, __FILE__);
	}
	return value;
}

bool FMTmodel::getParameter(const FMTboolmodelparameters& key) const
{
	bool value;
	try{
		value = parameters.getBoolParameter(key);
	}catch(...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::getParameter", __LINE__, __FILE__);
	}
	return value;
}

const std::string& FMTmodel::getParameter(const FMTstrmodelparameters& p_key) const
{
	try {
		return parameters.getStrParameter(p_key);
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::getParameter", __LINE__, __FILE__);
	}
	return nullptr;
}

bool FMTmodel::setCompressTime(const int& periodStart, const int& periodStop, const int& value)
{
	bool returnbool = true;
	try{
		for (int period = (periodStart-1);period<periodStop;++period)
		{
			if (!parameters.setPeriodCompressTime(period,value)) 
			{
				returnbool=false;
			}
		}
	}catch(...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::setCompressTime", __LINE__, __FILE__);
	}
	return returnbool;
}

std::vector<int> FMTmodel::getCompressTime() const
{
	return parameters.getCompressTime();
}

std::vector<Core::FMTconstraint>FMTmodel::goalConstraints(double penalty) const
{
	std::vector<Core::FMTconstraint>newconstraints;
	try {
		size_t constraintid = 0;
		for (const Core::FMTconstraint& constraint : constraints)
			{
			if (constraint.isObjective())
				{
				Core::FMTconstraint newobjective(constraint);
				std::vector<std::string>variables;
				variables.push_back("_ALL");
				std::string penop = "-";
				if (constraint.sense()==1.0)
					{
					penop = "+";
					}
				newobjective.setPenalties(penop, variables);
				newconstraints.push_back(newobjective);
				}else if(constraint.getConstraintType()==Core::FMTconstrainttype::FMTstandard) 
					{
					const std::string VariableName("~G"+std::to_string(constraintid));
					Core::FMTconstraint newconstraint(constraint);
					newconstraint.setGoal(VariableName, penalty);
					newconstraints.push_back(newconstraint);
					}
				++constraintid;
				}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::goalConstraints", __LINE__, __FILE__);
	}
	return newconstraints;
}

std::vector<Core::FMTconstraint>FMTmodel::getTacticalConstraints(double penalty,double scheduleweight,double objective, double objectivefactor) const
{
	std::vector<Core::FMTconstraint>newconstraints;
	try {
		const std::vector<Core::FMTconstraint> goaledconstraints = goalConstraints(penalty);
		//_SETFROMGLOBAL|_SETFROMLOCAL|_REIGNORE
		for (const Core::FMTconstraint& constraint : goaledconstraints)
			{
			if (constraint.isObjective())
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
				globalset.setRhs(lowerbound, upperbound);
				globalset.setLength(1, 1);
				globalset.addBounds(Core::FMTyldbounds(Core::FMTsection::Optimize, "_SETFROMGLOBAL", objectivefactor, objectivefactor));
				newconstraints.push_back(globalset);
				//Set schedule weight
				Core::FMTconstraint newobjective(constraint);
				if (scheduleweight > 0)
					{
					newobjective.addBounds(Core::FMTyldbounds(Core::FMTsection::Optimize, "_SETGLOBALSCHEDULE", scheduleweight, scheduleweight));
					}
				newconstraints.push_back(newobjective);
			}else if(constraint.getConstraintType()==Core::FMTconstrainttype::FMTstandard) 
				{
				Core::FMTconstraint newconstraint(constraint);
				if (constraint.getPeriodLowerBound() == 1 &&
					constraint.getPeriodUpperBound() == 1)
					{
					newconstraint.addBounds(Core::FMTyldbounds(Core::FMTsection::Optimize, "_REIGNORE", 2,2));
					}
				newconstraints.push_back(newconstraint);
				}
			}

	}catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTmodel::getTacticalConstraints", __LINE__, __FILE__);
	}
	return newconstraints;
}



void FMTmodel::showParameters(const bool& showhelp)const
{
	std::string message=" - Parameters for model "+getName()+"\n";
	try{
	*_logger<<message<<"	-- Int parameters"<<"\n";
	message="	LENGTH                   : "+std::to_string(parameters.getIntParameter(LENGTH))+"\n";
	if(showhelp) message+="	(The number of period to optimize or simulate. DEFAULT=30)\n";
	message+="	SEED                     : "+std::to_string(parameters.getIntParameter(SEED))+"\n";
	if(showhelp) message+="	(The seed used for stochastique process in FMTsamodel, FMTnssmodel and FMTsesmodel. DEFAULT=25)\n";
	message+="	NUMBER_OF_ITERATIONS     : "+std::to_string(parameters.getIntParameter(NUMBER_OF_ITERATIONS))+"\n";
	if(showhelp) message+="	(The number of iterations to do in FMTsesmodel::greedyreferencebuild. DEFAULT=10000)\n";
	message+="	PRESOLVE_ITERATIONS      : "+std::to_string(parameters.getIntParameter(PRESOLVE_ITERATIONS))+"\n";
	if(showhelp) message+="	(The number of iterations to do in FMTmodel::presolve. DEFAULT=10)\n";
	message+="	NUMBER_OF_THREADS        : "+std::to_string(parameters.getIntParameter(NUMBER_OF_THREADS))+"\n";
	if(showhelp) message+="	(Number of thread use by solver for optimisation. DEFAULT=Number of concurrent threads supported)\n";
	*_logger<<message<<"\n	-- Double parameters"<<"\n";
	message="	TOLERANCE                : "+std::to_string(parameters.getDblParameter(TOLERANCE))+"\n";
	if(showhelp) message+="	(Double tolerance used when setting a solution from schedules. DEFAULT="+std::to_string(FMT_DBL_TOLERANCE)+")\n";
	message+="	GOALING_SCHEDULE_WEIGHT  : "+std::to_string(parameters.getDblParameter(GOALING_SCHEDULE_WEIGHT))+"\n";
	if(showhelp) message+="	(The weight to use when trying goal a schedule from a strategic model. DEFAULT=10000)\n";
	*_logger<<message<<"\n	-- Bool parameters"<<"\n";
	std::string boolval;
	boolval = (parameters.getBoolParameter(FORCE_PARTIAL_BUILD)) ? "true\n" : "false\n";
	message="	FORCE_PARTIAL_BUILD      : "+boolval;
	if(showhelp) message+="	(Force partial build on graph and matrix for period which schedules are passed to FMTlpmodel::doplanning. DEFAULT=false)\n";
	boolval = (parameters.getBoolParameter(STRICTLY_POSITIVE)) ? "true\n" : "false\n";
	message+="	STRICTLY_POSITIVE        : "+boolval;
	if(showhelp) message+="	(Force matrix to have bound >= 0 for the outputs use in constraints or objective. DEFAULT=true)\n";
	boolval = (parameters.getBoolParameter(POSTSOLVE)) ? "true\n" : "false\n";
	message+="	POSTSOLVE                : "+boolval;
	if(showhelp) message+="	(Postsolve model after doplanning. DEFAULT=true)\n";
	boolval = (parameters.getBoolParameter(SHOW_LOCK_IN_SCHEDULES)) ? "true\n" : "false\n";
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

void FMTmodel::swapPtr(std::unique_ptr<FMTmodel>& rhs)
{
	*this = std::move(*rhs); 
}


FMTmodelcomparator::FMTmodelcomparator(std::string name) :model_name(name) {}

bool FMTmodelcomparator::operator()(const FMTmodel& model) const
	{
	return(model_name == model.getName());
	}






}



BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTmodel)
