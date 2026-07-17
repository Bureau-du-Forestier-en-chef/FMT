/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/



#include "FMTSpatialSchedule.h"
#include "FMTmodel.h"
#include "FMTforest.h"
#include <numeric>
#include <algorithm>
#include <set>
#include <iterator>
#include "FMTbindingspatialaction.h"
#include "FMToutput.h"
#include "FMTeventrelation.h"
#include "FMTGCBMtransition.h"
#include "FMTexceptionhandler.h"
#include <boost/thread.hpp>
#include "FMTSpatialGraphs.h"
#include "FMTsemodel.h"
#include "FMTPatchRules.h"


namespace Spatial
{

	boost::unordered_map<Core::FMTDevelopment,std::vector<int>>
		FMTSpatialSchedule::m_OperabilityCache = boost::unordered_map<Core::FMTDevelopment,std::vector<int>>();

	std::vector<Spatial::FMTCoordinate>FMTSpatialSchedule::m_Movables = 
								std::vector<Spatial::FMTCoordinate>();


    FMTSpatialSchedule::FMTSpatialSchedule(const FMTForest& p_InitialMap, 
										size_t p_LengthReserve,
										FMTSpatialGraphs& p_SpatialGraph) :
		FMTLayer<FMTVirtualLineGraph>(), m_scheduleType(FMTSpatialScheduletype::FMTcomplete),
		m_ConstraintsFactor(), m_events(), m_Tracker()
    {
        FMTLayer<FMTVirtualLineGraph>::operator = (p_InitialMap.copyExtent<FMTVirtualLineGraph>());//Setting layer information
		m_Tracker = p_SpatialGraph.getBaseSolution();
		boost::unordered_map<Core::FMTDevelopment, FMTCoordinate>cacheGraph;
		size_t id = 0;
        for(FMTLayer<Core::FMTDevelopment>::const_iterator devit = p_InitialMap.begin(); devit != p_InitialMap.end(); ++devit)
        {
			if (cacheGraph.find(devit->second)==cacheGraph.end())
				{
				std::vector<Core::FMTActualDevelopment> actdevelopment(1, Core::FMTActualDevelopment(devit->second, p_InitialMap.getCellSize()));
				actdevelopment[0] = actdevelopment.at(0).reduceLockToDeath(p_SpatialGraph.getModel().lifespan);
				Graph::FMTLineGraph local_graph(p_LengthReserve);
				std::queue<Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>::FMTvertex_descriptor> actives = local_graph.initialize(actdevelopment);
				mapping[devit->first] = p_SpatialGraph.getVirtualGraph(local_graph);
				cacheGraph[devit->second] = devit->first;
			}else {
				mapping[devit->first] = mapping[cacheGraph[devit->second]];
				}
			++id;
        }
    }

	FMTSpatialSchedule FMTSpatialSchedule::getBaseSchedule(const FMTSpatialGraphs& p_SpatialGraph) const
	{
		FMTSpatialSchedule NewSchedule(*this);
		for (auto& GRAPH : NewSchedule)
			{
			GRAPH.second.setBaseGraph(NewSchedule.m_Tracker);
			}
		NewSchedule.m_events.clear();
		return NewSchedule;
	}

	void FMTSpatialSchedule::setSpatialGraphs(const Spatial::FMTSpatialSchedule& p_ToCopy,
														FMTSpatialGraphs& p_SpatialGraph)
	{
		try {
			mapping.clear();
			FMTLayer<FMTVirtualLineGraph>::operator=(p_ToCopy.copyExtent<FMTVirtualLineGraph>());
			for (const auto& DATA : p_ToCopy)
				{
				mapping[DATA.first] =  FMTVirtualLineGraph(DATA.second, p_SpatialGraph);
				}
			m_Tracker = p_ToCopy.m_Tracker;
			m_events = p_ToCopy.m_events;
			m_scheduleType = p_ToCopy.m_scheduleType;
			m_ConstraintsFactor = p_ToCopy.m_ConstraintsFactor;
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::setSpatialGraphs", __LINE__, __FILE__);
		}
	}



	FMTSpatialSchedule::FMTSpatialSchedule(const FMTSpatialSchedule& other,
		const std::vector<FMTCoordinate>::const_iterator& firstcoord,
		const std::vector<FMTCoordinate>::const_iterator& endcoord):
		FMTLayer<FMTVirtualLineGraph>(),
		m_scheduleType(FMTSpatialScheduletype::FMTpartial),
		m_ConstraintsFactor(other.m_ConstraintsFactor),
		m_events(other.m_events),
		m_Tracker(other.m_Tracker)
	{
		FMTLayer<FMTVirtualLineGraph>::operator=(other.copyExtent<FMTVirtualLineGraph>());
		std::vector<FMTCoordinate>::const_iterator it = firstcoord;
		while (it!= endcoord)
			{
			mapping[*it] = other.mapping.at(*it);
			++it;
			}
	}

	void FMTSpatialSchedule::swap(FMTSpatialSchedule& rhs)
		{
		try {
			FMTLayer<FMTVirtualLineGraph>::swap(rhs);
			std::swap(m_scheduleType, rhs.m_scheduleType);
			m_events.swap(rhs.m_events);
			m_Tracker.swap(rhs.m_Tracker);
			m_ConstraintsFactor.swap(rhs.m_ConstraintsFactor);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::swap", __LINE__, __FILE__);
			}
		}


    bool FMTSpatialSchedule::operator == (const FMTSpatialSchedule& rhs)const
    {
        if (!(m_events==rhs.m_events)) return false;
        for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator coordit = this->mapping.begin();
                                                                         coordit!= this->mapping.end(); ++coordit)
        {
            if (coordit->second.getLineGraph() != rhs.mapping.at(coordit->first).getLineGraph())
            {
                return false;
            }
        }
    return true;
    }

    bool FMTSpatialSchedule::operator != (const FMTSpatialSchedule& rhs)const
    {
        return (!(*this==rhs));
    }

    int FMTSpatialSchedule::actPeriod() const
    {
        //Graph::FMTLineGraph flgraph = mapping.begin()->second;
        return mapping.begin()->second.getLineGraph().getPeriod();
    }


	std::vector<Core::FMTActualDevelopment>FMTSpatialSchedule::getArea(int period, bool beforegrowanddeath) const
	{
		std::vector<Core::FMTActualDevelopment>OutValues;
		try {
			if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::getForestPeriod", __LINE__, __FILE__);
				}
			const double CELL_SIZE = getCellSize();
			std::map<Core::FMTDevelopment, double>outArea;
			for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				const Graph::FMTLineGraph& local_graph = graphit->second.getLineGraph();
				const std::vector<double> solutions(1, this->getCellSize());
				Core::FMTDevelopment graphDev;
				if (beforegrowanddeath)
				{
					graphDev = local_graph.getPeriodStartDev(period);
				}
				else {
					graphDev = local_graph.getPeriodStopDev(period);
				}
				if (outArea.find(graphDev)== outArea.end())
				{
					outArea[graphDev] = 0;
				}
				outArea[graphDev] += CELL_SIZE;
			}
			OutValues = std::vector<Core::FMTActualDevelopment>(outArea.size());
			size_t id = 0;
			for (const auto& out : outArea)
			{
				OutValues[id] = Core::FMTActualDevelopment(out.first, out.second);
				++id;
			}
			}catch (...)
				{
				_exhandler->raiseFromCatch("For period " + std::to_string(period), "FMTSpatialSchedule::getArea", __LINE__, __FILE__);
				}
			return OutValues;
	}

    FMTForest FMTSpatialSchedule::getForestPeriod(const int& period,bool periodStart) const
    {
        FMTForest forest(this->copyExtent<Core::FMTDevelopment>());//Setting layer information
		try {
			if (m_scheduleType!=FMTSpatialScheduletype::FMTcomplete)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::getForestPeriod", __LINE__, __FILE__);
				}
			const std::vector<double> solutions(1, this->getCellSize());
			for(std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				const Graph::FMTLineGraph& local_graph = graphit->second.getLineGraph();
				if(periodStart)
				{
					forest[graphit->first] = local_graph.getPeriodStartDev(period);	
				}else{
					forest[graphit->first] = local_graph.getPeriodStopDev(period);	
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("For period "+std::to_string(period), "FMTSpatialSchedule::getForestPeriod", __LINE__, __FILE__);
			}
        return forest;
    }


	bool FMTSpatialSchedule::allowAction(const int& targetaction, const std::vector<Spatial::FMTBindingSpatialAction>&bindingactions, const FMTCoordinate& location, const int& period) const
	{
		try
		{
			const size_t targetmaximalsize = bindingactions.at(targetaction).getMaximalSize();
			const int lowergup = static_cast<int>(bindingactions.at(targetaction).getMinimalGreenUp());
			const size_t loweradjacency = bindingactions.at(targetaction).getMinimalAdjacency();
			const uint16_t loweradjacencyof = static_cast<uint16_t>(loweradjacency);
			for (int green_up = std::max(1,period- lowergup); green_up <= period; ++green_up)
				{
				for (const int& mact : bindingactions.at(targetaction).getNeighbors())
							{
							const uint16_t distance = static_cast<int>(loweradjacency + std::max(targetmaximalsize, bindingactions.at(mact).getMaximalSize()));
							const uint16_t minx = distance > location.getX() ? 0 : location.getX() - distance;
							const uint16_t miny = distance > location.getY() ? 0 : location.getY() - distance;
							const uint16_t maxofx = (distance + location.getX()) > maxx ? maxx : (distance + location.getX());
							const uint16_t maxofy = (distance + location.getY()) > maxy ? maxy : (distance + location.getY());
							const FMTCoordinate minimallocation(minx, miny);
							const FMTCoordinate maximallocation(maxofx, maxofy);
							for (const FMTEventContainer::const_iterator eventit : m_events.getEvents(green_up, mact, minimallocation, maximallocation))
								{
								if (eventit->within(loweradjacencyof, location))
									{
									return false;

									}
								}
							}

				}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::allowAction", __LINE__, __FILE__);
		}
		return true;
	}


	std::vector<std::set<Spatial::FMTCoordinate>> FMTSpatialSchedule::getUpdatedScheduling(
																	//const std::vector<Core::FMTAction>& actions,
																	const Models::FMTModel& model,
																	const std::vector<int>& actiontargets,
																	//const Core::FMTSchedule& selection,
																	boost::unordered_map<Core::FMTDevelopment, std::vector<bool>>& cachedactions,
																	const std::vector<boost::unordered_set<Core::FMTDevelopment>>& scheduleoperabilities,
																	//const Core::FMTYields& yields,
																	bool schedule_only,
																	std::vector<std::set<Spatial::FMTCoordinate>> original,
																	std::vector<FMTCoordinate> updatedcoordinate) const
		{
		try {
			if (original.empty())
				{
				original.resize(model.actions.size());
				updatedcoordinate.reserve(mapping.size());
				for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator itc = mapping.begin(); itc != mapping.end(); ++itc)
					{
					updatedcoordinate.push_back(itc->first);
					}
				}
			//boost::unordered_map<Core::FMTDevelopment,std::vector<bool>>cachedactions;
			//cachedactions.reserve(updatedcoordinate.size());
			const int8_t DEATH_ID = static_cast<int8_t>(model.actions.size())-1;
			for (const FMTCoordinate& updated : updatedcoordinate)
				{
				const Graph::FMTLineGraph& lg = mapping.at(updated).getLineGraph();
				const Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>::FMTvertex_descriptor& active = lg.getActiveVertex();
				
				boost::unordered_map<Core::FMTDevelopment, std::vector<bool>>::iterator cacheit = cachedactions.end();
				const Core::FMTDevelopment& test = lg.getDevelopment(active);

				if (lg.isNotDead(active, DEATH_ID))
					{
						const Core::FMTDevelopment& active_development = lg.getDevelopment(active);
						//*_logger << "op at period " << active_development.getPeriod() << "\n";
						const int LAST_ACTION_ID = lg.getInEdgeActionId(active);
						cacheit = cachedactions.find(active_development);
						if (cacheit == cachedactions.end())
						{
							std::pair<boost::unordered_map<Core::FMTDevelopment, std::vector<bool>>::iterator, bool>insertedpair = cachedactions.insert(std::make_pair(active_development, std::vector<bool>(model.actions.size(), false)));
							cacheit = insertedpair.first;
							for (const int& actionid : actiontargets)
							{
								if ((actionid > LAST_ACTION_ID || LAST_ACTION_ID < 0) &&
									((schedule_only && (inScheduleOperabilities(scheduleoperabilities, &active_development, actionid, model.actions.at(actionid))/*selection.operated(action, active_development)*/)) ||
										(!schedule_only && active_development.operable(model.actions.at(actionid), model.yields))))
								{
									cacheit->second[actionid] = true;
								}
								else {
									cacheit->second[actionid] = false;
								}
							}
						}
					}

						for (const int& actionid : actiontargets)
						{
							std::set<Spatial::FMTCoordinate>& settochange = original[actionid];
							if (cacheit!=cachedactions.end() &&
								cacheit->second.at(actionid))
							{
								settochange.insert(updated);
							}
							else if (!settochange.empty())//Pile of empty actions
							{
								settochange.erase(updated);
							}
						}
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getUpdatedScheduling", __LINE__, __FILE__);
			}
		return original;
		}



	std::set<FMTCoordinate> FMTSpatialSchedule::verifySpatialFeasibility(const int& targetaction,
		const std::vector<Spatial::FMTBindingSpatialAction>& bindingactions,
		const int& period, const std::set<FMTCoordinate>& operables) const
	{
		std::set<FMTCoordinate> spatiallyallowable;
		try
		{
			for (std::set<FMTCoordinate>::const_iterator itc = operables.begin(); itc != operables.end(); ++itc)
			{
				if (allowAction(targetaction, bindingactions,*itc, period))
				{
					spatiallyallowable.insert(*itc);
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::verifyspatialfeasability", __LINE__, __FILE__);
		}
		return spatiallyallowable;
	}



	FMTEventContainer FMTSpatialSchedule::buildHarvest(const double & target, const Spatial::FMTBindingSpatialAction& targetaction,
		std::default_random_engine & generator, std::set<FMTCoordinate> mapping_pass,
		const int& period, const int& actionid, std::vector<FMTCoordinate>& operated) const
	{
		//To gain efficiency, maybe tracking cell that have been ignit actually, we are suposing that we are trying every cell, but its not true because of the random generator
		double harvested_area = 0;
		FMTEventContainer cuts;
		try {
			size_t count = mapping_pass.size();
			int tooclosecall = 0;
			int initdone = 0;
			int spreaddone = 0;
			const bool check_adjacency = (std::find(targetaction.getNeighbors().begin(), targetaction.getNeighbors().end(), actionid) != targetaction.getNeighbors().end());
			if (!mapping_pass.empty())
			{
				std::set<FMTCoordinate>::const_iterator randomit;
				while (harvested_area < target && count > 0 && !mapping_pass.empty())
				{
					std::uniform_int_distribution<int> celldistribution(0, static_cast<int>(mapping_pass.size()) - 1);
					const int cell = celldistribution(generator);//Get cell to ignit
					randomit = mapping_pass.begin();
					std::advance(randomit, cell);
					const size_t FAMILY = find(*randomit)->second.getGraphFamily();
					FMTEvent newcut;
					std::vector<std::set<FMTCoordinate>::const_iterator>actives = newcut.ignit(targetaction.getMaximalSize(),randomit, actionid, period, FAMILY);
					if (!actives.empty())
					{
						++initdone;
						if (newcut.spread(targetaction.getMinimalSize(), targetaction.getMaximalSize(), targetaction.getMinimalNeighborSize(), mapping_pass, actives))
						{
							++spreaddone;
							bool tooclose = false;
							if (check_adjacency)
							{
								const size_t adjacency = targetaction.getMinimalAdjacency();
								const size_t maximaldistance = adjacency + targetaction.getMaximalSize();
								const unsigned int adjacencyof = static_cast<unsigned int>(adjacency);
								for (const std::set<FMTEvent>::const_iterator cutit : cuts.getEvents(period, newcut.getTerritory(maximaldistance)))
								{
									if (cutit->within(adjacencyof, newcut))
									{
										tooclose = true;
										++tooclosecall;
										break;
									}
								}
							}
							if (!tooclose)
							{
								cuts.insert(newcut);
								operated.reserve(newcut.getElements().size());
								for (const FMTCoordinate& toRemove : newcut.getElements())
								{
									operated.push_back(toRemove);
									mapping_pass.erase(toRemove);
								}
								harvested_area += (static_cast<double>(newcut.getElements().size())*cellsize);
								count = mapping_pass.size() + 1;
							}
						}
					}
					--count;
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::buildHarvest", __LINE__, __FILE__);
		}
		return cuts;
	}


	double FMTSpatialSchedule::operateEvents(const FMTEventContainer& cuts, const Core::FMTAction& action, const int& action_id, const Core::FMTTransition& Transition,
									 const Core::FMTYields& ylds, const std::vector<Core::FMTTheme>& themes)
	{
		double operatedarea = 0;
		try {
			for (const FMTEvent& cut : cuts)
			{
				for (std::set<FMTCoordinate>::const_iterator coordit = cut.getElements().begin(); coordit != cut.getElements().end(); coordit++)
				{
					Graph::FMTLineGraph lg = mapping.at(*coordit).getLineGraph();
					const size_t pathssize = lg.operate(action, action_id, Transition, ylds, themes);
					mapping.at(*coordit).setLineGraph(lg, m_Tracker);
					if (pathssize > 1)
					{
						_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than one verticies for the graph after operate ... See if you have multiple transitions. Coord at " + std::string(*coordit),
							"FMTSpatialSchedule::operateEvents", __LINE__, __FILE__);
					}
							operatedarea += cellsize;
				}
			}
			m_events.merge(cuts);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::operateEvents", __LINE__, __FILE__);
		}
		return operatedarea;
	}

	void FMTSpatialSchedule::operateCoord(const FMTCoordinate& coord,const Core::FMTAction& action, const int& action_id, const FMTBindingSpatialAction& bindingspaction, const Core::FMTTransition& Transition,
					 const Core::FMTYields& ylds, const std::vector<Core::FMTTheme>& themes)
	{
		try
		{
			Graph::FMTLineGraph lg = mapping.at(coord).getLineGraph();
			const size_t pathssize = lg.operate(action, action_id, Transition, ylds, themes);
			auto& VGraph = mapping.at(coord);
			VGraph.setLineGraph(lg, m_Tracker);
			if (pathssize > 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than one verticies for the graph after operate ... See if you have multiple transitions. Coord at " + std::string(coord),
					"FMTSpatialSchedule::operateCoord", __LINE__, __FILE__);
			}
			m_events.addAction(coord,actPeriod(),action_id,
				bindingspaction, VGraph.getGraphFamily());
		}catch(...){
				_exhandler->printExceptions("", "FMTSpatialSchedule::operateCoord", __LINE__, __FILE__);
		}
	}

	void FMTSpatialSchedule::grow()
	{
		try {
			for (std::map<FMTCoordinate, FMTVirtualLineGraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				Graph::FMTLineGraph local_graph = graphit->second.getLineGraph();
				local_graph.grow();
				graphit->second.setLineGraph(local_graph, m_Tracker);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::grow", __LINE__, __FILE__);
		}
	}


	std::vector<Core::FMTSchedule> FMTSpatialSchedule::getSchedules(
												const FMTSpatialGraphs p_Graphs, bool withlock) const
	{
		std::vector<Core::FMTSchedule> Schedules;
		try {
			if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTspatialschedule::getSchedules", __LINE__, __FILE__);
			}
			Schedules = p_Graphs.getSchedules(m_Tracker, withlock);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTspatialschedule::getSchedules", __LINE__, __FILE__);
		}
		return Schedules;
	}

	double FMTSpatialSchedule::_evaluateSpatialGroups(
		const FMTSpatialGraphs& p_SpatialGraph,
		int p_period,
		int p_greenup,
		int p_lowerLookup,
		int p_upperLookup,
		int p_theme,
		bool p_testLower,
		const std::vector<bool>& p_actions) const
	{
		double returnvalue = 0.0;
		try {
			if (!p_testLower)
				{
				const int FIRST_PERIOD = std::max(1, p_period - p_greenup);
				returnvalue = m_events.getDispersion(p_actions, p_SpatialGraph,
					p_theme, FIRST_PERIOD, p_period, p_upperLookup);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::_evaluateSpatialGroups", __LINE__, __FILE__);
			}
		return returnvalue;
	}

	std::vector<FMTEventContainer::const_iterator> FMTSpatialSchedule::_getSpatialGroupsConflict(
		const FMTSpatialGraphs& p_SpatialGraph,
		int p_period,
		int p_greenup,
		int p_lowerLookup,
		int p_upperLookup,
		int p_theme,
		bool p_testLower,
		const std::vector<bool>& p_actions) const
	{
		std::vector<FMTEventContainer::const_iterator>conflicts;
		try {
			if (!p_testLower)
			{
				const int FIRST_PERIOD = std::max(1, p_period - p_greenup);
				conflicts = m_events.getDispersionConflicts(p_actions, p_SpatialGraph,
					p_theme, FIRST_PERIOD, p_period, p_upperLookup);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::_getSpatialGroupsConflict", __LINE__, __FILE__);
			}
		return conflicts;
	}


	double FMTSpatialSchedule::_evaluateSpatialAdjacency(
		int p_period,
		int p_greenup,
		int p_lowerLookup,
		int p_upperLookup,
		bool p_testLower,
		const std::vector<bool>& p_actions) const
	{
		double returnvalue = 0;
		try {
			int baselookup = 0;
			double lower = 0.0;
			double upper = 0.0;
			if (p_testLower)
			{
				baselookup = p_lowerLookup;
				lower = static_cast<double>(p_lowerLookup);
			}else
			{
				baselookup = std::max(baselookup, p_upperLookup);
				upper = static_cast<double>(p_upperLookup);
			}


			for (const FMTEventContainer::const_iterator eventit : m_events.getEvents(p_period, p_actions))
			{
				const uint16_t containerlookup = static_cast<uint16_t>(baselookup + eventit->size());

				//0//-//1//
				//-//-//-//
				//2//-//3//
				const std::array<FMTCoordinate,4> enveloppe = eventit->getEnveloppe();
				const uint16_t minimalx = containerlookup < enveloppe.at(0).getX() ? enveloppe.at(0).getX() - containerlookup : 0;
				const uint16_t minimaly = containerlookup < enveloppe.at(0).getY() ? enveloppe.at(0).getY() - containerlookup : 0;
				const uint16_t maximalx = enveloppe.at(3).getX() + containerlookup;
				const uint16_t maximaly = enveloppe.at(3).getY() + containerlookup;
				const FMTCoordinate minimalcoord(minimalx, minimaly);
				const FMTCoordinate maximalcoord(maximalx, maximaly);
				double totalwithincount = 0;
				for (int gupperiod = std::max(1, p_period - p_greenup); gupperiod <= p_period; ++gupperiod)
				{
					const double periodfactor = static_cast<double>((p_greenup - (p_period - gupperiod))) + 1;
					for (const FMTEventContainer::const_iterator eventof : m_events.getEvents(gupperiod, p_actions, minimalcoord, maximalcoord))
					{
						if (eventit != eventof)//They will have the same address if it's the same event!
						{
							/*const FMTEventRelation ofrelation = eventof->getRelation(*eventit);
							const FMTEventRelation itrelation = eventit->getRelation(*eventof);
							if (relations.find(ofrelation) == relations.end() &&
								relations.find(itrelation) == relations.end())
							{*/
							if (p_testLower && eventit->within(static_cast<size_t>(p_lowerLookup), *eventof)) //too close
							{
								returnvalue += ((lower - eventit->distance(*eventof)) * periodfactor);
								//conflicts.push_back(eventof);
							}
							else if (!p_testLower && !eventit->within(static_cast<size_t>(p_upperLookup), *eventof))
							{
								returnvalue += ((eventit->distance(*eventof) - upper) * periodfactor);
								//conflicts.push_back(eventof);
							}
							/*relations.insert(ofrelation);
							relations.insert(itrelation);
						}*/

						}
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::_evaluateSpatialAdjacency", __LINE__, __FILE__);
		}
		return returnvalue;
	}


	std::vector<FMTEventContainer::const_iterator> FMTSpatialSchedule::_getAdjacencyConflict(
		int p_period,
		int p_greenup,
		int p_lowerLookup,
		int p_upperLookup,
		bool p_testLower,
		const std::vector<bool>& p_actions) const
	{
		std::vector<FMTEventContainer::const_iterator> conflicts;
		try {
			int baselookup = 0;
			double lower = 0.0;
			double upper = 0.0;
			if (p_testLower)
			{
				baselookup = p_lowerLookup;
				lower = static_cast<double>(p_lowerLookup);
			}
			else
			{
				baselookup = std::max(baselookup, p_upperLookup);
				upper = static_cast<double>(p_upperLookup);
			}


			for (const FMTEventContainer::const_iterator eventit : m_events.getEvents(p_period, p_actions))
			{
				const uint16_t containerlookup = static_cast<uint16_t>(baselookup + eventit->size());

				//0//-//1//
				//-//-//-//
				//2//-//3//
				const std::array<FMTCoordinate, 4> enveloppe = eventit->getEnveloppe();
				const uint16_t minimalx = containerlookup < enveloppe.at(0).getX() ? enveloppe.at(0).getX() - containerlookup : 0;
				const uint16_t minimaly = containerlookup < enveloppe.at(0).getY() ? enveloppe.at(0).getY() - containerlookup : 0;
				const uint16_t maximalx = enveloppe.at(3).getX() + containerlookup;
				const uint16_t maximaly = enveloppe.at(3).getY() + containerlookup;
				const FMTCoordinate minimalcoord(minimalx, minimaly);
				const FMTCoordinate maximalcoord(maximalx, maximaly);
				double totalwithincount = 0;
				for (int gupperiod = std::max(1, p_period - p_greenup); gupperiod <= p_period; ++gupperiod)
				{
					const double periodfactor = static_cast<double>((p_greenup - (p_period - gupperiod))) + 1;
					for (const FMTEventContainer::const_iterator eventof : m_events.getEvents(gupperiod, p_actions, minimalcoord, maximalcoord))
					{
						if (eventit != eventof)//They will have the same address if it's the same event!
						{
							/*const FMTEventRelation ofrelation = eventof->getRelation(*eventit);
							const FMTEventRelation itrelation = eventit->getRelation(*eventof);
							if (relations.find(ofrelation) == relations.end() &&
								relations.find(itrelation) == relations.end())
							{*/
							if (p_testLower && eventit->within(static_cast<size_t>(p_lowerLookup), *eventof)) //too close
							{
								conflicts.push_back(eventof);
							}
							else if (!p_testLower && !eventit->within(static_cast<size_t>(p_upperLookup), *eventof))
							{
								conflicts.push_back(eventof);
							}
							/*relations.insert(ofrelation);
							relations.insert(itrelation);
						}*/

						}
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::_getAdjacencyConflict", __LINE__, __FILE__);
		}
		return conflicts;
	}


std::vector<FMTCoordinate> FMTSpatialSchedule::getGroupsConflict(const Core::FMTConstraint& p_SpatialConstraint,
		const FMTSpatialGraphs& p_SpatialGraph) const
	{
		std::vector<FMTCoordinate>conflict;
		try {
			int periodStart = 0;
			int periodStop = 0;
			if (mapping.begin()->second.getLineGraph().constraintLenght(p_SpatialConstraint, periodStart, periodStop))
				{
				std::set<FMTCoordinate>allCoordinates;
				const std::vector<bool>ACTION_USED = p_SpatialConstraint.isActionsused(p_SpatialGraph.getModel().actions);
				const Core::FMTconstrainttype SPATIAL_CONSTRAINT_TYPE = p_SpatialConstraint.getConstraintType();
				for (int period = periodStart; period <= periodStop; ++period)
					{
					double lower = 0;
					double upper = 0;
					p_SpatialConstraint.getBounds(lower, upper, period);
					const bool TEST_LOWER_BOUND = (lower == std::numeric_limits<double>::lowest()) ? false : true;
					const Core::FMTYldBounds& GUP_BOUNDS = p_SpatialConstraint.getYieldBound("GUP");
					const Core::FMTYldBounds& THEME_BOUNDS = p_SpatialConstraint.getYieldBound("THEME");
					int themeTarget = -1;
					if (THEME_BOUNDS.getLower() >= 0.0)
						{
						themeTarget = (static_cast<int>(THEME_BOUNDS.getLower()) - 1);
						}
					const std::vector<FMTEventContainer::const_iterator> EVENT_CONFLICTS = _getSpatialGroupsConflict(p_SpatialGraph,
						period, static_cast<int>(GUP_BOUNDS.getLower()),
						static_cast<int>(lower),
						static_cast<int>(upper),
						themeTarget,
						TEST_LOWER_BOUND, ACTION_USED);
					for (FMTEventContainer::const_iterator EventIt : EVENT_CONFLICTS)
						{
						const  std::set<FMTCoordinate> EVENT = EventIt->getElements();
						allCoordinates.insert(EVENT.begin(), EVENT.end());
						}
					}
				conflict = std::vector<FMTCoordinate>(allCoordinates.begin(), allCoordinates.end());
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getGroupsConflict", __LINE__, __FILE__);
			}
		return conflict;
	}
	


	double FMTSpatialSchedule::evaluateSpatialConstraint(const Core::FMTConstraint& p_SpatialConstraint,
					const FMTSpatialGraphs& p_SpatialGraph) const
	{
	double returnvalue = 0;
	try {
		int periodStart = 0;
		int periodStop = 0;
		if (this->mapping.begin()->second.getLineGraph().constraintLenght(p_SpatialConstraint, periodStart, periodStop))
		{
			const std::vector<bool>ACTION_USED = p_SpatialConstraint.isActionsused(p_SpatialGraph.getModel().actions);
			const Core::FMTconstrainttype SPATIAL_CONSTRAINT_TYPE = p_SpatialConstraint.getConstraintType();
			for (int period = periodStart; period <= periodStop; ++period)
				{
				double lower = 0;
				double upper = 0;
				p_SpatialConstraint.getBounds(lower, upper, period);
				const bool TEST_LOWER_BOUND = (lower == std::numeric_limits<double>::lowest()) ? false : true;
				switch (p_SpatialConstraint.getConstraintType())
				{
				case Core::FMTconstrainttype::FMTspatialsize:
						{
						returnvalue += m_events.evaluateSize(ACTION_USED,
							period, 
							static_cast<size_t>(lower), static_cast<size_t>(upper),
							TEST_LOWER_BOUND);
						break;
						}
					case Core::FMTconstrainttype::FMTspatialadjacency:
						{
							const Core::FMTYldBounds& GUP_BOUNDS = p_SpatialConstraint.getYieldBound("GUP");
							returnvalue += _evaluateSpatialAdjacency(
								period,
								static_cast<int>(GUP_BOUNDS.getLower()),
								static_cast<int>(lower),
								static_cast<int>(upper),
								TEST_LOWER_BOUND,
								ACTION_USED);
							break;
						}
					case Core::FMTconstrainttype::FMTSpatialGroup:
						{
						const Core::FMTYldBounds& GUP_BOUNDS = p_SpatialConstraint.getYieldBound("GUP");
						const Core::FMTYldBounds& THEME_BOUNDS = p_SpatialConstraint.getYieldBound("THEME");
						int themeTarget = -1;
						if (THEME_BOUNDS.getLower()>=0.0)
						{
							themeTarget = (static_cast<int>(THEME_BOUNDS.getLower()) - 1);
						}
						returnvalue += _evaluateSpatialGroups(p_SpatialGraph,
							period, static_cast<int>(GUP_BOUNDS.getLower()),
							static_cast<int>(lower),
							static_cast<int>(upper),
								themeTarget,
							TEST_LOWER_BOUND, ACTION_USED);
						break;
						}
					default:
						break;
					};

				}
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTSpatialSchedule::evaluateSpatialConstraint", __LINE__, __FILE__);
		}
	return returnvalue;
	}


	double FMTSpatialSchedule::_getRulesEvaluation(const std::vector<FMTPatchRules>& p_rules,
		const FMTSpatialGraphs& p_SpatialGraph) const
	{
		double cost = 0;
		try {
			for (const FMTPatchRules& RULE : p_rules)
				{
				cost += RULE.evaluate(m_events, p_SpatialGraph);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", 
				"FMTSpatialSchedule::_getRulesEvaluation", __LINE__, __FILE__);
			}
		return cost;
	}


	double FMTSpatialSchedule::getConstraintEvaluation(const FMTSpatialGraphs& p_Graphs, size_t p_ConstraintId) const
	{
		double value = 0;
		try {
			assert(isPartial() || mapping.size() == _getNonSpatialCellsCount());
			const Core::FMTConstraint& CONSTRAINT = p_Graphs.getModel().constraints.at(p_ConstraintId);
			if (!CONSTRAINT.isSpatial())
			{
				int pStart, pStop = 0;
				if (!this->mapping.empty() && 
					this->mapping.begin()->second.getLineGraph().constraintLenght(CONSTRAINT, pStart, pStop))
				{
					value = p_Graphs.getConstraintsValue(p_ConstraintId, m_Tracker);
				}	
			}
			else{
				value = this->evaluateSpatialConstraint(CONSTRAINT, 
														p_Graphs);

				}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getConstraintEvaluation", __LINE__, __FILE__);
		}
	return value;
	}

	std::vector<double> FMTSpatialSchedule::getWeightedFactors(const Spatial::FMTSpatialGraphs& p_Graphs) const
	{
		std::vector<double>allvalues;
		try {
			double objective = 0;
			double infeasibilities = 0;
			getSolutionStatus(objective, infeasibilities, p_Graphs, false, false);
			const double global = (objective + infeasibilities);
			std::vector<Core::FMTConstraint>constraints = p_Graphs.getModel().getconstraints();
			const double sizefactor = static_cast<double>(constraints.size());
			const double localw = std::abs(global / sizefactor);
			double value = 1;
			if (objective>0)
				{
				value = std::abs(localw / objective);
				}
			allvalues.push_back(value);
			constraints.erase(constraints.begin());
			size_t ConstraintId = 0;
			for (const Core::FMTConstraint& constraint : constraints)
				{
				const double constraintvalue = getConstraintEvaluation(p_Graphs,ConstraintId);
				double value = 1;
				if (constraintvalue>0)
					{
					value = localw / constraintvalue;
					}
				allvalues.push_back(value);
				++ConstraintId;
				}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getWeightedFactors", __LINE__, __FILE__);
		}
		return allvalues;

	}


	std::vector<double> FMTSpatialSchedule::getConstraintsValues(const Spatial::FMTSpatialGraphs& p_Graphs) const
	{
		std::vector<double>allvalues;
		try {
			assert(isPartial() || mapping.size() == _getNonSpatialCellsCount());
			const std::vector<Core::FMTConstraint>& CONSTRAINTS = p_Graphs.getModel().getconstraints();
			allvalues.push_back(this->getObjectiveValue(p_Graphs,false));
			for (size_t ConstraintId = 1; ConstraintId < CONSTRAINTS.size(); ++ConstraintId)
				{
				allvalues.push_back(getConstraintEvaluation(p_Graphs, ConstraintId));
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getConstraintsValues", __LINE__, __FILE__);
			}
		return allvalues;
	}

	double FMTSpatialSchedule::getPrimalInfeasibility(const std::vector<const Core::FMTConstraint*>& constraints, 
									const Spatial::FMTSpatialGraphs& p_Graphs, bool withfactorization) const
	{
		double value = 0;
		try {
			size_t fid = 1;
			for (const Core::FMTConstraint* constraint: constraints)
				{
				const size_t ID = std::distance(&(*p_Graphs.getModel().constraints.begin()), constraint);
				double cntvalue = getConstraintEvaluation(p_Graphs, ID);
				if (withfactorization && !m_ConstraintsFactor.empty())
					{
					cntvalue = _getExponentialFactorization(
						cntvalue, m_ConstraintsFactor.at(fid));
					}
					value += cntvalue;
				fid += 1;
				}

		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getPrimalInfeasibility", __LINE__, __FILE__);
			}
		return value;
	}

	double FMTSpatialSchedule::getObjectiveValue(const FMTSpatialGraphs& p_Graphs,bool withsense) const
	{
		double value = 0;
		try {
			value = p_Graphs.getConstraintsValue(0, m_Tracker)*
					(withsense ? p_Graphs.getModel().constraints.at(0).sense() : 1);

		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getObjectiveValue", __LINE__, __FILE__);
		}
		return value;
	}


	std::vector<int> FMTSpatialSchedule::isBetterThan(const FMTSpatialSchedule& newsolution,
											const Spatial::FMTSpatialGraphs& p_Graphs) const
		{
		std::vector<int> groupevaluation;
		try {
			if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::isBetterThan", __LINE__, __FILE__);
			}
			const std::vector<Core::FMTConstraint> constraints = p_Graphs.getModel().getconstraints();
			size_t maximalgroup = 0;
			for (const Core::FMTConstraint& constraint : constraints)
				{
				const size_t groupid = constraint.getGroup();
				if (groupid> maximalgroup)
					{
					maximalgroup = groupid;
					}
				}
			groupevaluation = std::vector<int>(maximalgroup + 1, 0);
			std::vector<double>groupsprimalinfeasibilitygap(maximalgroup + 1, 0);
			size_t i = 0;
			for (const Core::FMTConstraint& constraint : constraints)
				{
				const double oldvalue = this->getConstraintEvaluation(p_Graphs,i);
				const double newvalue= newsolution.getConstraintEvaluation(p_Graphs,i);
				if (!(newvalue==0 && oldvalue==0))
					{
					const size_t groupid = constraint.getGroup();
					const double constraintdif = (oldvalue - newvalue);
					groupsprimalinfeasibilitygap[groupid] += constraintdif;
					}
				++i;
				}
			for (size_t groupid = 0 ;groupid < groupevaluation.size();++groupid)
				{
				if (groupsprimalinfeasibilitygap[groupid]>0)//Make sure the primalinfeasibility get better for the group!
					{
					groupevaluation[groupid] = -1;
					}
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::isBetterThan", __LINE__, __FILE__);
			}
		return groupevaluation;
		}

	void FMTSpatialSchedule::logSolutionStatus(const size_t& iteration,const double& objective, const double& primalinfeasibility) const
	{
	try {
		if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot use a non complete schedule ",
				"FMTSpatialSchedule::logSolutionStatus", __LINE__, __FILE__);
		}
		_logger->logWithLevel("Iteration "+std::to_string(iteration)+" Primal Inf(" + std::to_string(primalinfeasibility) + ") Obj(" + std::to_string(objective) + ")\n", 1);
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTSpatialSchedule::logSolutionStatus", __LINE__, __FILE__);
		}
	}

	void FMTSpatialSchedule::getSolutionStatus(double& objective, double& primalinfeasibility, 
		const FMTSpatialGraphs& p_Graphs, bool withsense,bool withfactorization, bool withspatial) const
	{
		try {
			//std::vector<Core::FMTConstraint>constraints = model.getconstraints();
			objective = this->getObjectiveValue(p_Graphs, withsense);
			if (withfactorization&&!m_ConstraintsFactor.empty())
				{
				objective = _getExponentialFactorization(
					objective, m_ConstraintsFactor.at(0));
				}
			/*constraints.erase(constraints.begin());
			std::vector<Core::FMTConstraint>constraintssubset;
			for (const Core::FMTConstraint& constraint : constraints)
				{
				if (withspatial || !constraint.isSpatial())
					{
					constraintssubset.push_back(constraint);
					}

				}*/
			std::vector<const Core::FMTConstraint*>constraintssubset;
			constraintssubset.reserve(p_Graphs.getModel().constraints.size());
			for (size_t cid = 1;cid < p_Graphs.getModel().constraints.size();++cid)
			{
				if (withspatial || !p_Graphs.getModel().constraints.at(cid).isSpatial())
				{
					constraintssubset.push_back(&p_Graphs.getModel().constraints.at(cid));
				}
			}
			primalinfeasibility = this->getPrimalInfeasibility(constraintssubset, p_Graphs, withfactorization);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getSolutionStatus", __LINE__, __FILE__);
		}
	}

	double FMTSpatialSchedule::getGlobalObjective(const FMTSpatialGraphs& p_Graphs) const
	{
		double global = 0;
		try {
			double objective = 0;
			double infeasibilities = 0;
			getSolutionStatus(objective, infeasibilities, p_Graphs,false,true);
			global = (objective + infeasibilities);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getGlobalObjective", __LINE__, __FILE__);
			}
		return global;
	}

	

	std::map<std::string, double> FMTSpatialSchedule::getOutput(const FMTSpatialGraphs& p_Graphs,
		const Core::FMTOutput& p_output,
		int p_period, Core::FMToutputlevel level ) const
	{
		std::map<std::string,double>values;
		try {
			
			values = p_Graphs.getOutput(m_Tracker, p_output, p_period, level);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getOutput", __LINE__, __FILE__);
		}
		return values;
		
	}


	
	


	FMTLayer<double> FMTSpatialSchedule::getSpatialOutput(const Models::FMTModel& model, const Core::FMTOutput& output, const int& period) const
	{
		FMTLayer<double>outputlayer = copyExtent<double>();
		try {
			for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				const double GRAPH_VALUE = graphit->second.getOutput(model,
					m_Tracker, output, period);
				if ((std::abs(GRAPH_VALUE) - FMT_DBL_TOLERANCE) > 0)
				{
					outputlayer[graphit->first] = GRAPH_VALUE;
				}
			}

		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getSpatialOutput", __LINE__, __FILE__);
		}
		return outputlayer;
	}
	


	std::vector<std::pair<FMTCoordinate, double>>FMTSpatialSchedule::getOutputByCoordinate(const Models::FMTModel & model, 
														const Core::FMTOutput& output, const int& period) const
	{
		std::vector<std::pair<FMTCoordinate, double>>allvalues;
		try {
			for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
				const double GRAPH_VALUE = graphit->second.getOutput(model,
					m_Tracker, output, period);
				if ((std::abs(GRAPH_VALUE)-FMT_DBL_TOLERANCE)>0)
					{
					allvalues.push_back(std::pair<FMTCoordinate, double>(graphit->first, GRAPH_VALUE));
					}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getOutputByCoordinate", __LINE__, __FILE__);
		}
		return allvalues;
	}


	std::string FMTSpatialSchedule::getPatchStats(const std::vector<Core::FMTAction>& actions) const
	{
		std::string result = "";
		try {
			if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::getPatchStats", __LINE__, __FILE__);
			}
			result += "Period Action "+FMTEvent::getStatsHeader()+"\n";
			for (int period =m_events.firstPeriod(); period <= m_events.lastPeriod(); ++period)
			{
				for (int action_id = 0; action_id < actions.size(); ++action_id)
				{
					std::vector<FMTEventContainer::const_iterator> evsit = m_events.getEvents(period, action_id);
					for (const auto& eventit : evsit)
					{
						result += std::to_string(period) + " " + actions.at(action_id).getName() + " " + eventit->getStats() + "\n";
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getPatchStats", __LINE__, __FILE__);
		}
		return result;
	}

	FMTLayer<std::string> FMTSpatialSchedule::lastDistLayer(const std::vector<Core::FMTAction>& modelactions, const int& period) const
	{
		FMTLayer<std::string> distlayer(this->copyExtent<std::string>());
		try {
			if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::lastDistLayer", __LINE__, __FILE__);
				}
			for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
				const int lastactid = graphit->second.getLineGraph().getLastActionId(period);
				if (lastactid >= 0)
				{
					distlayer[graphit->first] = modelactions.at(lastactid).getName();
				}

			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::lastDistLayer", __LINE__, __FILE__);
		}
		return distlayer;
	}

	std::vector<std::vector<Graph::FMTPredictor>>FMTSpatialSchedule::getPredictors(FMTLayer<int>& predictorids, const Models::FMTModel& model, const std::vector<std::string>& yieldnames, const int& period,bool periodonevalues, bool withGCBMid) const
	{
		std::vector<std::vector<Graph::FMTPredictor>>predictors;
		try {
			if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::getPredictors", __LINE__, __FILE__);
			}
			std::set<std::vector<Graph::FMTPredictor>>predictorsset;
			std::vector<std::set<std::vector<Graph::FMTPredictor>>::iterator>predictorlocalisations;
			for (std::map<FMTCoordinate, FMTVirtualLineGraph >::const_iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
				const std::vector<Graph::FMTPredictor> graphpredictors = graphit->second.getLineGraph().getPeriodPredictors(period,model, yieldnames,periodonevalues,withGCBMid);
				std::set<std::vector<Graph::FMTPredictor>>::iterator setit = predictorsset.end();
				bool pushed=false;
				if (!graphpredictors.empty())
				{
					setit = predictorsset.find(graphpredictors);
					if (setit == predictorsset.end())
					{
						const std::pair<std::set<std::vector<Graph::FMTPredictor>>::iterator, bool> ret = predictorsset.insert(graphpredictors);
						predictorlocalisations.push_back(ret.first);
						pushed = true;
					}
				}
				if(!pushed)
				{
					predictorlocalisations.push_back(setit);
				}
			}
			predictors.insert(predictors.end(), predictorsset.begin(), predictorsset.end());
			size_t graphid = 0;
			for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphit = this->begin(); graphit != this->end(); ++graphit)
				{
				if (predictorlocalisations.at(graphid)!= predictorsset.end())
					{
					const int predictorid = static_cast<int>(std::distance(predictorsset.begin(), predictorlocalisations.at(graphid)));
					predictorids[graphit->first] = predictorid;
					}/*else{
						*_logger<<period<<" "<<graphid<<" "<<std::string(graphit->first)<<"\n";
					}*/
				++graphid;
				}

		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getPredictors", __LINE__, __FILE__);
		}
		return predictors;
	}

	std::vector<Core::FMTGCBMTransition>FMTSpatialSchedule::getGCBMtransitions(FMTLayer<std::string>& stackedactions, const std::vector<Core::FMTAction>& modelactions, const std::vector<Core::FMTTheme>& classifiers, const int& period) const
	{
		std::vector<Core::FMTGCBMTransition>GCBM;
		try {
			if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::getGCBMtransitions", __LINE__, __FILE__);
			}
			std::map<std::string, std::vector<int>> ageaftercontainer;
			std::map<std::string, std::map<std::string, std::map<std::string, int>>> finalattributes;
			//Iter through spatialschedule
			for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
				// lastaction id = -1 no action in period
				const std::vector<int> periodactionids = graphit->second.getLineGraph().getPeriodActionIds(period, false);
	
				std::string actionsStr;
				for (const int& aid : periodactionids)
				{
					if (aid >= 0)
					{
						if (!actionsStr.empty()) actionsStr += ";";
						actionsStr += modelactions.at(aid).getName();
					}
				}
				if (!actionsStr.empty())
				{
					stackedactions[graphit->first] = actionsStr;
					//For each classifier, append the value at the begining of the period and keep track of value at the end in finalattributes. Also keep the ageafter.
					if (!classifiers.empty())
					{
						const Core::FMTDevelopment sdev = graphit->second.getLineGraph().getPeriodStartDev(period);
						const Core::FMTDevelopment fdev = graphit->second.getLineGraph().getPeriodStopDev(period);
						const Core::FMTDevelopment snpdev = graphit->second.getLineGraph().getPeriodStartDev(period + 1);
						const int fage = snpdev.getAge();
						std::map<std::string, std::string> themeattributes;
						for (const auto& theme : classifiers)
						{
							std::string themename = "THEME" + std::to_string(theme.getId() + 1);
							const std::string fclass = fdev.getMask().get(theme);
							themeattributes[themename] = fclass;
							const std::string sclass = sdev.getMask().get(theme);
							stackedactions[graphit->first] += "-" + sclass;
						}
						std::string stackname = stackedactions.at(graphit->first);
						if (ageaftercontainer.find(stackname) != ageaftercontainer.end())
						{
							ageaftercontainer[stackname].push_back(fage);
						}
						else {
							ageaftercontainer[stackname] = std::vector<int>(1, fage);
						}
						if (finalattributes.find(stackname) != finalattributes.end())
						{
							for (std::map<std::string, std::string>::const_iterator attit = themeattributes.begin(); attit != themeattributes.end(); ++attit)
							{
								finalattributes[stackname][attit->first][attit->second] = 1;
							}
						}
						else
						{
							for (std::map<std::string, std::string>::const_iterator attit = themeattributes.begin(); attit != themeattributes.end(); ++attit)
							{
								finalattributes[stackname][attit->first][attit->second] += 1;
							}
						}
					}
				}
			}
			//Iter through ageafter container where the first key is the stackname
			for (std::map<std::string, std::vector<int>>::const_iterator ageit = ageaftercontainer.begin(); ageit != ageaftercontainer.end(); ++ageit)
			{
				//Calculate average age
				//Last argument in accumulate is the first element to add ... So we put a float and the return is a float to be able to round up
				const int ageafter = static_cast<int>(std::round(std::accumulate(ageit->second.begin(), ageit->second.end(), 0.0) / static_cast<float>(ageit->second.size())));
				std::map<std::string, std::string>theme_collection;
				//For each theme return the finalattributes that is the most present
				for (std::map<std::string, std::map<std::string, int>>::const_iterator themeit = finalattributes.at(ageit->first).begin(); themeit != finalattributes.at(ageit->first).end(); ++themeit)
				{
					int maxhit = 0;
					std::string returntheme = "";
					for (std::map<std::string, int>::const_iterator cit = themeit->second.begin(); cit != themeit->second.end(); ++cit)
					{
						if (cit->second > maxhit)
						{
							maxhit = cit->second;
							returntheme = cit->first;
						}
					}
					theme_collection[themeit->first] = returntheme;
				}
				GCBM.push_back(Core::FMTGCBMTransition(ageafter, theme_collection, ageit->first));
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getGCBMtransitions", __LINE__, __FILE__);
		}
		return GCBM;
	}

	void FMTSpatialSchedule::eraseLastPeriod()
		{
		try {
			const int lastPeriod = this->mapping.begin()->second.getLineGraph().getPeriod() - 1;
			for (std::map<FMTCoordinate, FMTVirtualLineGraph>::iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
				{
				//Graph::FMTLineGraph copied = graphit->second.getLineGraph();
				//copied.clearfromperiod(lastPeriod, true);
				//graphit->second.setLineGraph(copied,m_NonSpatialSolution);
				graphit->second.setLastPeriod(m_Tracker);
				}
			std::set<FMTEvent>::const_iterator periodit = m_events.getBounds(lastPeriod).first;
			while (periodit!=m_events.end())
				{
				periodit = m_events.erase(periodit);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::eraseLastPeriod", __LINE__, __FILE__);
			}

		}


	std::vector<FMTCoordinate>FMTSpatialSchedule::getFromEvents(const Core::FMTOutputNode& node, const std::vector<Core::FMTAction>& actions, const int& period) const
	{
		//std::vector<const Graph::FMTLineGraph*>graphs;
		std::vector<FMTCoordinate>coordinates;
		try {
			if (node.isActionbased()&& node.source.isVariable())
			{
				std::vector<bool>targetedactions(actions.size(),false);
				for (const Core::FMTAction* actionptr : node.source.targets(actions))
					{
					targetedactions[std::distance(&(*actions.cbegin()), actionptr)] = true;
					}
				for (std::set<FMTEvent>::const_iterator eventit : m_events.getEvents(period, targetedactions))
					{
						for (const FMTCoordinate& coordinate : eventit->getElements())
						{
							coordinates.push_back(coordinate);
						}
					}
				std::sort(coordinates.begin(), coordinates.end());
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getFromEvents", __LINE__, __FILE__);
		}
		return coordinates;

	}

	std::vector<const Graph::FMTLineGraph*>FMTSpatialSchedule::getFromEvents(const Core::FMTConstraint& constraint, const std::vector<Core::FMTAction>& actions, const int& start,const int& stop) const
		{
		std::vector<const Graph::FMTLineGraph*>graphs;

		try {
		if (constraint.isActionbased())
			{
			std::vector<int>targetedactions;
			for (const Core::FMTOutputSource& osource : constraint.getSources())
				{
				if (osource.isVariable())
					{
					for (const Core::FMTAction* actionptr : osource.targets(actions))
						{
						const int actionid = static_cast<int>(std::distance(&(*actions.cbegin()), actionptr));
						if (std::find(targetedactions.begin(),targetedactions.end(),actionid)== targetedactions.end())
							{
							targetedactions.push_back(actionid);
							}

						}
					}
				}
			std::sort(targetedactions.begin(), targetedactions.end());
			std::set<FMTCoordinate>collected;
			for (int period = start; period <=stop; ++period)
				{
				for (std::set<FMTEvent>::const_iterator eventit : m_events.getEvents(period, targetedactions))
					{
					for (const FMTCoordinate& coordinate : eventit->getElements())
						{
						const Graph::FMTLineGraph* graphptr = &(mapping.find(coordinate)->second.getLineGraph());
						if (collected.find(coordinate)==collected.end())
							{
							collected.insert(coordinate);
							graphs.push_back(graphptr);
							}
						/*if (std::find(graphs.begin(), graphs.end(), graphptr) == graphs.end())
							{
							graphs.push_back(graphptr);
							}*/
						}
					}
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getFromEvents", __LINE__, __FILE__);
			}
		return graphs;
		}

std::map<std::string,double> FMTSpatialSchedule::getOutputFromGraph(const Graph::FMTLineGraph& linegraph, const Models::FMTModel & model,
											 const Core::FMTOutputNode& node, const double* solution, const int&period,
											const Core::FMTMask& nodemask,
											boost::unordered_map<Core::FMTMask, double>& nodecache,
											const Core::FMTTheme* p_theme,Core::FMToutputlevel level)const
	{
	std::map<std::string, double> values;
	try{
	if (!(node.isActionbased()&&linegraph.isOnlyGrow(period)))
	{
		boost::unordered_map<Core::FMTMask, double>::const_iterator cashit = nodecache.find(nodemask);
		if (cashit != nodecache.end() && level == Core::FMToutputlevel::totalonly)//get it from cashing
		{
			values["Total"] = cashit->second;
		}else {
			if (level == Core::FMToutputlevel::standard)//Only feel the target theme
			{
				const int themeid = node.source.getThemeTarget();
				if (themeid >= 0)
					{
					p_theme = &model.themes.at(themeid);
					}
			}
			values = linegraph.getSource(model, node, period, *p_theme, solution,level);
			if (level != Core::FMToutputlevel::developpement)//No caching for developpement
			{
				nodecache[nodemask] = values.at("Total");
			}
		}
	}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getOutputFromGraph", __LINE__, __FILE__);
		}
	return values;
	}

void FMTSpatialSchedule::postSolve(const Core::FMTMaskFilter& p_Filter,
								const std::vector<Core::FMTAction>& p_PresolveActions,
								Spatial::FMTSpatialGraphs& p_Graphs)
	{
	try {
		m_Tracker = p_Graphs.getBaseSolution();
		const std::vector<Core::FMTAction> postsolveactions = p_Graphs.getModel().getactions();
		std::vector<int>actionmapping;
		actionmapping.reserve(p_PresolveActions.size());
		for (const Core::FMTAction action : p_PresolveActions)
		{
			const int loc = static_cast<int>(std::distance(postsolveactions.begin(), std::find_if(postsolveactions.begin(), postsolveactions.end(), Core::FMTActionComparator(action.getName()))));
			actionmapping.push_back(loc);
		}
	
		for (std::map<FMTCoordinate, FMTVirtualLineGraph>::iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
			graphit->second = graphit->second.postSolve(p_Filter, actionmapping,
								p_Graphs, m_Tracker);
			}
		FMTEventContainer newevents;
		for (FMTEventContainer::iterator eventit= m_events.begin(); eventit!=m_events.end();eventit++)
			{
			FMTEvent newevent(*eventit);
			newevent.setActionId(actionmapping.at(eventit->getActionId()));
			newevents.insert(newevent);
			}
		m_events.swap(newevents);
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTSpatialSchedule::postSolve", __LINE__, __FILE__);
		}
	}

FMTSpatialSchedule FMTSpatialSchedule::presolve(const Core::FMTMaskFilter& p_filter,
	FMTSpatialGraphs& p_Graphs, size_t p_ReserveSize) const
	{
	FMTSpatialSchedule presolvedSchedule;
	try {
		if (actPeriod() != 1)//just presolve if no solution
			{
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Cannot presolve a schedule with more than 1 period",
				"FMTSpatialSchedule::presolve", __LINE__, __FILE__);
			}
		presolvedSchedule.m_scheduleType = FMTSpatialScheduletype::FMTcomplete;
		presolvedSchedule.FMTLayer<FMTVirtualLineGraph>::operator = (copyExtent<FMTVirtualLineGraph>());
		boost::unordered_map<Core::FMTDevelopment, FMTCoordinate>cacheGraph;
		std::vector<FMTCoordinate>coordinates(mapping.size());
		presolvedSchedule.m_Tracker = p_Graphs.getBaseSolution();
		size_t id = 0;
		for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphIt = mapping.begin(); graphIt != mapping.end(); ++graphIt)
			{
			//std::queue<Graph::FMTLineGraph::FMTvertex_descriptor> allDescriptors = graphIt->second.getLineGraph().getActiveVertices();
			const Core::FMTDevelopment& NON_PRESOLVED = graphIt->second.getLineGraph().getPeriodStopDev(0);
			const Core::FMTActualDevelopment PRESOLVED_DEV = Core::FMTActualDevelopment(NON_PRESOLVED, getCellSize()).presolve(p_filter, p_Graphs.getModel().themes);
			if (cacheGraph.find(PRESOLVED_DEV) == cacheGraph.end())
			{
				std::vector<Core::FMTActualDevelopment> actDevelopment;
				actDevelopment.push_back(PRESOLVED_DEV);
				Graph::FMTLineGraph local_graph(p_ReserveSize);
				std::queue<Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>::FMTvertex_descriptor> actives = local_graph.initialize(actDevelopment);
				presolvedSchedule.mapping[graphIt->first] = p_Graphs.getVirtualGraph(local_graph);
				cacheGraph[PRESOLVED_DEV] = graphIt->first;
			}else {
				presolvedSchedule.mapping[graphIt->first] = presolvedSchedule.mapping[cacheGraph[PRESOLVED_DEV]];
				}
			coordinates[id] = graphIt->first;
			++id;
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSpatialSchedule::presolve", __LINE__, __FILE__);
		}
	return presolvedSchedule;
	}


bool FMTSpatialSchedule::inScheduleOperabilities(const std::vector<boost::unordered_set<Core::FMTDevelopment>>& scheduleoperabilities,
	Core::FMTDevelopment const* dev, const int& actionid, const Core::FMTAction& action) const
{
	try {
		const boost::unordered_set<Core::FMTDevelopment>& location = scheduleoperabilities.at(actionid);
		if (!location.empty())
			{
			boost::unordered_set<Core::FMTDevelopment>::const_iterator opit;

			if (!action.doRespectLock())
			{
				const Core::FMTDevelopment target = dev->clearLock();
				opit = location.find(target);
			}
			else {
				opit = location.find(*dev);
			}

			return (opit != location.end());
			}
		
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTSpatialSchedule::inScheduleOperabilities", __LINE__, __FILE__);
	}
	return false;
}

FMTSpatialSchedule::actionbindings FMTSpatialSchedule::getBindingActionsByPeriod(const Models::FMTModel& model) const
{
	std::vector<std::vector<Spatial::FMTBindingSpatialAction>>allbindings;
	for (int period = 1 ; period < mapping.begin()->second.getLineGraph().getPeriod(); ++period)
		{
		allbindings.push_back(getBindingActions(model,period));
		}
	return allbindings;
}


std::vector<Spatial::FMTBindingSpatialAction> FMTSpatialSchedule::getBindingActions(const Models::FMTModel& model, const int& period) const
	{
	const size_t nactions = model.actions.size();
	std::vector<double>minimalsizes(nactions,0);
	std::vector<double>maximalsizes(nactions,std::numeric_limits<double>::max());
	std::vector<double>minimalgreenups(nactions,0);
	std::vector<double>maximalgreenups(nactions, std::numeric_limits<double>::max());
	std::vector<double>minimalnsizes(nactions,0);
	std::vector<double>maximalnsizes(nactions,std::numeric_limits<double>::max());
	std::vector<double>minimaladjacencys(nactions, 0);
	std::vector<double>maximaladjacencys(nactions, std::numeric_limits<double>::max());
	std::vector<std::vector<int>>neighboring(nactions, std::vector<int>());
	std::vector<Spatial::FMTBindingSpatialAction>bindings;
	try {
		bindings.reserve(nactions);
		for (const Core::FMTConstraint& constraint : model.constraints)
		{
			if (constraint.isSpatial() &&
				period >= constraint.getPeriodLowerBound() &&
				period <= constraint.getPeriodUpperBound())
			{
				double lower = 0;
				double upper = 0;
				constraint.getBounds(lower, upper, period);
				const std::vector<int>actionids = constraint.getActionIds(model.actions);
				for (const int& actionid : actionids)
					{

					for (const int& subid : actionids)
						{
						if (std::find(neighboring.at(actionid).begin(), neighboring.at(actionid).end(), subid) == neighboring.at(actionid).end())
							{
							neighboring.at(actionid).push_back(subid);
							}
						}
					}
				if (constraint.getConstraintType() == Core::FMTconstrainttype::FMTspatialsize)
				{
					//const Core::FMTYldBounds yldbounds = constraint.getYldsBounds().at("NSIZE");
					const Core::FMTYldBounds& yldbounds = constraint.getYieldBound("NSIZE");
					const double lowernsize = yldbounds.getLower();
					const double uppernsize = yldbounds.getUpper();
					
					//*_logger << "Sizes " << actionids.size() << " " << lower << " " << upper << "\n";
					for (const int& actionid : actionids)
					{
						if (upper < maximalsizes.at(actionid))
						{
							maximalsizes[actionid] = upper;
						}
						if (lower > minimalsizes.at(actionid))
						{
							//*_logger << "Size " << actionid <<" "<< lower << " " << upper << "\n";
							minimalsizes[actionid] = lower;
						}
						if (uppernsize < maximalnsizes.at(actionid))
						{
							maximalnsizes[actionid] = uppernsize;
						}
						if (lowernsize > minimalnsizes.at(actionid))
						{
							minimalnsizes[actionid] = lowernsize;
						}
					}
				}
				else if (constraint.getConstraintType() == Core::FMTconstrainttype::FMTspatialadjacency)
				{
					//const Core::FMTYldBounds yldbounds = constraint.getYldsBounds().at("GUP");
					const Core::FMTYldBounds& yldbounds = constraint.getYieldBound("GUP");
					const double lowergup = yldbounds.getLower();
					const double uppergup = yldbounds.getUpper();
					for (const int& actionid : actionids)
					{
						if (upper < maximaladjacencys.at(actionid))
						{
							maximaladjacencys[actionid] = upper;
						}
						if (lower > minimaladjacencys.at(actionid))
						{
							minimaladjacencys[actionid] = lower;
						}
						if (uppergup < maximalgreenups.at(actionid))
						{
							maximalgreenups[actionid] = uppergup;
						}
						if (lowergup > minimalgreenups.at(actionid))
						{
							minimalgreenups[actionid] = lowergup;
						}
					}
				}

			}


		}
		for (size_t actionid = 0; actionid < nactions;++actionid)
			{
			const size_t minimalsize = static_cast<size_t>(minimalsizes.at(actionid));
			const size_t maximalsize = (maximalsizes.at(actionid) == std::numeric_limits<double>::max()) ? std::numeric_limits<size_t>::max() : static_cast<size_t>(maximalsizes.at(actionid));
			const size_t minimalgreenup = static_cast<size_t>(minimalgreenups.at(actionid));
			const size_t maximalgreenup = (maximalgreenups.at(actionid) == std::numeric_limits<double>::max()) ? std::numeric_limits<size_t>::max() : static_cast<size_t>(maximalgreenups.at(actionid));
			const size_t minimalnsize = static_cast<size_t>(minimalnsizes.at(actionid));
			const size_t maximalnsize = (maximalnsizes.at(actionid) == std::numeric_limits<double>::max()) ? std::numeric_limits<size_t>::max() : static_cast<size_t>(maximalnsizes.at(actionid));
			const size_t minimaladjacency = static_cast<size_t>(minimaladjacencys.at(actionid));
			const size_t maximaladjacency = (maximaladjacencys.at(actionid) == std::numeric_limits<double>::max()) ? std::numeric_limits<size_t>::max() : static_cast<size_t>(maximaladjacencys.at(actionid));
			//if (actionid==2||actionid==14)
			//{
			/**_logger << "actionid " << actionid << "\n";
				*_logger << "minimalsize " << minimalsize << "\n";
				*_logger << "maximalsize " << maximalsize << "\n";
				*_logger << "minimalgreenup " << minimalgreenup << "\n";
				*_logger << "maximalgreenup " << maximalgreenup << "\n";
				*_logger << "minimalnsize " << minimalnsize << "\n";
				*_logger << "maximalnsize " << maximalnsize << "\n";
				*_logger << "minimaladjacency " << minimaladjacency << "\n";
				*_logger << "maximaladjacenc " << maximaladjacency << "\n";*/
			//}
			bindings.emplace_back(neighboring.at(actionid), minimalgreenup, maximalgreenup, minimaladjacency, maximaladjacency,minimalsize, maximalsize, minimalnsize, maximalnsize);
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTSpatialSchedule::getBindingActions", __LINE__, __FILE__);
		}
	return bindings;
	}


std::map<std::string, double> FMTSpatialSchedule::referenceBuild(const Core::FMTSchedule& schedule,
	const Models::FMTModel& model,
	const std::vector<boost::unordered_set<Core::FMTDevelopment>>& scheduleoperabilities,
	std::default_random_engine& p_randomEngine,
	bool schedule_only,
	bool scheduleatfirstpass)
{
	std::map<std::string, double>results;
	try {
		if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot use a non complete schedule ",
				"FMTSpatialSchedule::referenceBuild", __LINE__, __FILE__);
		}
		const int period = this->actPeriod();
		//this->setnewperiod();
		//std::default_random_engine generator(seed);
		double total_area = 0;
		std::vector<double>targets(model.actions.size(),0.0);
		std::vector<double>originaltargets(model.actions.size(),0.0);
		std::vector<int>actiontargets;
		int actionid = 0;
		for (const Core::FMTAction& action : model.actions)
			{
			const double actionArea = schedule.actionArea(action.getName());
			if (actionArea>0)
				{
				originaltargets[actionid]=actionArea;
				targets[actionid] = actionArea;
				actiontargets.push_back(actionid);
				total_area += actionArea;
				}
			++actionid;
			}
		std::vector<int>basetargets(actiontargets);
		double allocated_area = 0;
		const std::vector<Spatial::FMTBindingSpatialAction>bindingactions = this->getBindingActions(model, period);
		if (!schedule.empty() || !schedule_only)
		{
			double pass_allocated_area = 0;
			bool schedulepass = scheduleatfirstpass;
			bool schedulechange = false;
			int pass = 0;
			boost::unordered_map<Core::FMTDevelopment, std::vector<bool>>cachedactions;
			std::vector<std::set<Spatial::FMTCoordinate>>actions_operabilities = this->getUpdatedScheduling(model, actiontargets, cachedactions, scheduleoperabilities, schedulepass);
			do {
				pass_allocated_area = 0;
				int action_id = 0;
				if (schedulechange)
				{
					cachedactions.clear();
					actions_operabilities = this->getUpdatedScheduling(model, actiontargets, cachedactions, scheduleoperabilities, schedulepass);
					schedulechange = false;
				}
				std::vector<int>targetstoremove;
				for (const int& action_id : actiontargets)
				{
					const double& action_area = targets.at(action_id);
					const std::set<Spatial::FMTCoordinate>& allowable_coordinates = actions_operabilities.at(action_id);
					if (!allowable_coordinates.empty() && action_area > 0)
					{
						const std::set<Spatial::FMTCoordinate> spatially_allowable = this->verifySpatialFeasibility(action_id, bindingactions, period, allowable_coordinates);
						if (!spatially_allowable.empty())
						{
							std::vector<Spatial::FMTCoordinate> updatedcells;
							const Spatial::FMTEventContainer harvest = this->buildHarvest(action_area, bindingactions.at(action_id), p_randomEngine, spatially_allowable, period, action_id, updatedcells);
							if (harvest.size() > 0)
							{
								const double operatedarea = this->operateEvents(harvest,model.actions.at(action_id), action_id, model.transitions[action_id], model.yields, model.themes);
								actions_operabilities = this->getUpdatedScheduling(model, actiontargets, cachedactions, scheduleoperabilities, schedulepass, actions_operabilities, updatedcells);
								targets[action_id] -= operatedarea;
								if (targets[action_id]<=0)//remove this index from the actiontargets!
									{
									targetstoremove.push_back(action_id);
									}
								pass_allocated_area += operatedarea;
							}
						}
					}
				}
				if (!targetstoremove.empty())
					{
					std::vector<int>newtargets;
					for (const int& action_id : actiontargets)
						{
						if (std::find(targetstoremove.begin(), targetstoremove.end(),action_id)== targetstoremove.end())
							{
							newtargets.push_back(action_id);
							}

						}
					actiontargets = newtargets;
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
		this->grow();
		results["Total"] = allocated_area / total_area;
		for (const int& action_id : basetargets)
			{
			const double& originalvalue = originaltargets.at(action_id);
			results[model.actions.at(action_id).getName()] = ((originalvalue - targets.at(action_id)) / originalvalue);
			}
	}
	catch (...)
	{
		_exhandler->printExceptions("", "FMTSpatialSchedule::referenceBuild", __LINE__, __FILE__);
	}
	return results;


}

std::map<std::string, double> FMTSpatialSchedule::greedyReferenceBuild(const Core::FMTSchedule& schedule, const FMTSpatialGraphs& p_Graphs,
	const size_t& randomiterations,
	unsigned int seed,
	double tolerance,
	bool log)
{
	std::map<std::string, double>bestresults;
	FMTSpatialSchedule solutioncopy(*this);
	const size_t maxstall = 3;
	std::default_random_engine generator(seed);
	const double factorgap = 0.1;
	const std::vector<boost::unordered_set<Core::FMTDevelopment>>scheduleoperabilities = schedule.getOperabilities(p_Graphs.getModel().actions);
	size_t stalcount = 0;
	size_t iteration = 0;
	const unsigned int initialseed = seed;
	try {
		if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot use a non complete schedule ",
				"FMTSpatialSchedule::greedyReferenceBuild", __LINE__, __FILE__);
		}
		bestresults["Total"] = 0;
		const double totaliterations = static_cast<double>(randomiterations);
		double lastprimalinf = 0;
		double lastobjective = 0;
		double lastschedulefactor = 1;
		size_t failediterations = 0;
		bool didregular = false;
		bool mergingprimal = true;
		const double objsense = p_Graphs.getModel().constraints.at(0).sense();
		while ((stalcount < maxstall && failediterations < randomiterations) && ((randomiterations > 1) || (randomiterations == 1) && iteration < 1))//&& iteration < randomiterations
		{
			double factorit = (static_cast<double>(iteration) / totaliterations);
			double schedulefactor = (randomiterations == 1) ? 1 : 1 - ((1 - factorit) * factorgap);//bottom up
			if (factorit > 1)
			{
				std::uniform_real_distribution<double>scheduledistribution(lastschedulefactor - 0.05, std::min(lastschedulefactor + 0.05, 1.00));
				schedulefactor = scheduledistribution(generator);
			}
			//*_logger << "fact " << factorit<<" "<< schedulefactor << "\n";
			if (failediterations == (randomiterations - 1) && !didregular)//The last try will be the regular stuff
			{
				seed = initialseed;
				schedulefactor = 1;
				didregular = true;
			}
			bool scheduleonly = false;
			const Core::FMTSchedule factoredschedule = schedule.getNewSchedule(schedulefactor);
			const std::map<std::string, double>results = solutioncopy.referenceBuild(factoredschedule, p_Graphs.getModel(), scheduleoperabilities, generator, false, true);
			double newprimalinf = 0;
			double newobjective = 0;
			solutioncopy.getSolutionStatus(newobjective, newprimalinf, p_Graphs, true, false, false);
			const double primalgap = (newprimalinf - lastprimalinf);
			const double objgap = ((newobjective * objsense) - (lastobjective * objsense));
			if (iteration == 0 || (mergingprimal && (primalgap <= FMT_DBL_TOLERANCE)) ||
				(!mergingprimal && ((primalgap <= FMT_DBL_TOLERANCE) && (objgap <= FMT_DBL_TOLERANCE))))/*solutioncopy.isBetterByGroup(*this, model)*/
			{
				bestresults = results;
				lastschedulefactor = schedulefactor;

				if (iteration == 0)
				{
					*this = solutioncopy;
					this->getSolutionStatus(newobjective, newprimalinf, p_Graphs, true, false, false);
				}
				else {
					this->swap(solutioncopy);
				}


				lastprimalinf = newprimalinf;
				lastobjective = newobjective;
				failediterations = 0;
				if ((primalgap >= -FMT_DBL_TOLERANCE) && (objgap >= -FMT_DBL_TOLERANCE))
				{
					++stalcount;
				}
				else {
					stalcount = 0;
				}

			}
			else {
				++failediterations;
			}
			if (mergingprimal && failediterations == randomiterations)
			{
				mergingprimal = false;
				failediterations = 0;
			}
			if (log && (iteration % 10) == 0)
			{
				this->logSolutionStatus(iteration, lastobjective, lastprimalinf);
			}
			solutioncopy.eraseLastPeriod();//clear the last period to redo a simulate and test again!
			++seed;
			++iteration;
		}
		if (log && stalcount == maxstall)
		{
			_logger->logWithLevel("Stalled after " + std::to_string(iteration) + " iterations Skipping\n", 1);
		}
		if (log && failediterations == randomiterations)
		{
			_logger->logWithLevel("Solution stuck after " + std::to_string(iteration) + " iterations Skipping\n", 1);
		}
		//Need the remove the incomplete stuff from the cash before going to the next step.
		//*_logger << "lastfactor " << lastschedulefactor << "\n";
		for (std::map<std::string, double>::iterator facit = bestresults.begin(); facit != bestresults.end(); facit++)
		{
			//facit->second *= (lastschedulefactor < 1 ? 1 + (1 - lastschedulefactor) : 1 - (lastschedulefactor - 1));
			facit->second *= lastschedulefactor;
		}
		bestresults["Primalinfeasibility"] = lastprimalinf;
		bestresults["Objective"] = lastobjective;
	}
	catch (...)
	{
		_exhandler->printExceptions("", "FMTSesModel::greedyReferenceBuild", __LINE__, __FILE__);
	}

	return bestresults;
}

Graph::FMTGraphStats FMTSpatialSchedule::randomBuild(const Models::FMTModel& model,
														std::default_random_engine& generator)
	{
	Graph::FMTGraphStats periodstats;
	try {
		assert(isPartial()||mapping.size() == _getNonSpatialCellsCount());
		const int period = this->mapping.begin()->second.getLineGraph().getPeriod();
		const std::vector<FMTBindingSpatialAction> bindings = getBindingActions(model, period);
		std::uniform_int_distribution<int> dosomethingactions(1, model.getParameter(Models::FMTintmodelparameters::LENGTH));
		for (std::map<FMTCoordinate, FMTVirtualLineGraph>::iterator graphit = mapping.begin(); graphit !=mapping.end(); ++graphit)
			{
			Graph::FMTLineGraph local_graph = graphit->second.getLineGraph();
			if (dosomethingactions(generator) == 1)//Do something with 1 chance on length of model!
			{
				const std::vector<int> actionids = local_graph.randomBuildPeriod(model, generator,
																			m_OperabilityCache);
				if (!actionids.empty())
				{
					m_events.addActions(graphit->first, period,
						actionids, bindings, graphit->second.getGraphFamily());
				}
			}
			else { 
				local_graph.grow();
			}
			periodstats += local_graph.getStats();
			graphit->second.setLineGraph(local_graph, m_Tracker);
			

			}
		assert(isPartial()||mapping.size() == _getNonSpatialCellsCount());

	}catch (...)
		{
		_exhandler->printExceptions("", "FMTSpatialSchedule::randomBuild", __LINE__, __FILE__);
		}
	return periodstats;
	}

void FMTSpatialSchedule::setGrow(const std::vector<FMTCoordinate>& p_coordinates,
	const Models::FMTModel& p_model)
{
	try {
		for (const FMTCoordinate& COORDINATE : p_coordinates)
			{
			auto& VirtualGraph = mapping.at(COORDINATE);
			for (int period = 1; period <= p_model.getParameter(Models::FMTintmodelparameters::LENGTH);++period)
				{
				std::map<Core::FMTDevelopment, std::vector<bool>>tabouOperability;
				const std::vector<std::vector<bool>>GRAPH_ACTIONS = VirtualGraph.getLineGraph().getactions(p_model, 
																					period, tabouOperability);
				FMTEventContainer newEvents;
				const FMTEventContainer EVENTS_TO_ERASE = m_events.getEventsToErase(period, GRAPH_ACTIONS,
																COORDINATE, m_BUFFER_LOOKUP, newEvents);
				m_events.addUpdate(newEvents, EVENTS_TO_ERASE);
				}
			VirtualGraph.setNaturalGrowth(m_Tracker);
			}

	}catch (...)
		{
		_exhandler->printExceptions("", 
			"FMTSpatialSchedule::setGrow", __LINE__, __FILE__);
		}
}


std::vector<std::pair<Core::FMTDevelopment, std::vector<int>>> FMTSpatialSchedule::_putTabouInCache(
							const Models::FMTModel& p_model,
							std::map<Core::FMTDevelopment, std::vector<bool>>& p_tabou)
	{
	std::vector<std::pair<Core::FMTDevelopment, std::vector<int>>>ToFitIncache;
	try {
		for (std::map<Core::FMTDevelopment, std::vector<bool>>::const_iterator it = p_tabou.begin(); it != p_tabou.end(); it++)
			{
				const std::vector<int>& DEV_OP = Graph::FMTLineGraph::getSetOperability(it->first,
					p_model, m_OperabilityCache);
				std::vector<int>nonTabou;
				nonTabou.reserve(DEV_OP.size());
				for (int p_action : DEV_OP)
				{
					if (!it->second.at(p_action))
					{
						nonTabou.push_back(p_action);
					}
				}
				if (nonTabou.size() != DEV_OP.size())
				{
					ToFitIncache.emplace_back(it->first, DEV_OP);
					m_OperabilityCache[it->first] = nonTabou;
				}
			}
	}catch (...)
		{
		_exhandler->printExceptions("", "FMTSpatialSchedule::_putTabouInCache", __LINE__, __FILE__);
		}
	return ToFitIncache;
	}

void FMTSpatialSchedule::_setTabouOutOfCache(
	std::vector<std::pair<Core::FMTDevelopment, std::vector<int>>>& p_GoodValues)
{
	for (const auto& CACHE_FIX : p_GoodValues)
	{
		m_OperabilityCache[CACHE_FIX.first] = CACHE_FIX.second;
	}
}

void FMTSpatialSchedule::perturbGraph(const FMTCoordinate& coordinate,const int&period,const Models::FMTModel& model,
	std::default_random_engine& generator, const actionbindings& bindings)
	{
	try {
		const auto& Vgraph = mapping.at(coordinate);
		const size_t graphsize = Vgraph.getLineGraph().size();
		std::map<Core::FMTDevelopment, std::vector<bool>>tabuoperability;
		const std::vector<std::vector<bool>>actions = Vgraph.getLineGraph().getactions(model, period, tabuoperability);
		bool dontbuildgrowth = false;
		std::vector<std::pair<Core::FMTDevelopment, std::vector<int>>>ToFitIncache;
		if (!actions.empty())
			{
			ToFitIncache = _putTabouInCache(model, tabuoperability);
				FMTEventContainer newevents;
				const FMTEventContainer eventstoerase = m_events.getEventsToErase(period, actions,
					coordinate, m_BUFFER_LOOKUP, newevents);
				newevents = m_events.addUpdate(newevents, eventstoerase);

			}else {
			dontbuildgrowth = true;
			}
		Graph::FMTLineGraph newGraph = Vgraph.getLineGraph().copyToPeriod(period);
		int localperiod = period;
		while (graphsize != newGraph.size())
		{
			size_t maxsize = 0;
			const std::vector<int>actionids = newGraph.randomBuildPeriod(model, generator,
														m_OperabilityCache, dontbuildgrowth);
			if (!actionids.empty())
				{
				m_events.addActions(coordinate, localperiod,
					actionids, bindings.at(period - 1), Vgraph.getGraphFamily());
				}
			++localperiod;
		}
		mapping[coordinate].setLineGraph(newGraph, m_Tracker);
		_setTabouOutOfCache(ToFitIncache);
	}catch (...)
		{
		_exhandler->printExceptions("", "FMTSpatialSchedule::perturbGraph", __LINE__, __FILE__);
		}
	}



int FMTSpatialSchedule::getPeriodWithMaximalEvents(const std::vector<bool>& actions) const
{
	int period = 0;
	try {
		size_t maxnumferofevents = 0;
		for (int lperiod = 1; lperiod < actPeriod(); ++lperiod)
		{
			const size_t periodsize = m_events.getEvents(lperiod, actions).size();
			if (periodsize > maxnumferofevents)
			{
				maxnumferofevents = periodsize;
				period = lperiod;
			}
		}
	}catch (...)
	{
	_exhandler->printExceptions("", "FMTSpatialSchedule::getPeriodWithMaximalEvents", __LINE__, __FILE__);
	}
	return period;
}


std::vector<std::vector<Spatial::FMTCoordinate>>FMTSpatialSchedule::getAreaConflictCoordinates(const actionbindings& bindingactions,const int& period, bool conflictonly) const
{
	std::vector<std::vector<Spatial::FMTCoordinate>>coordinates;
	try {
		int actionid = 0;
		for (const Spatial::FMTBindingSpatialAction& actionbind : bindingactions.at(period-1))
		{
			if (actionbind.isSpatiallyAreaBinding()||!conflictonly)
			{
				for (const FMTEventContainer::const_iterator eventit : m_events.getEvents(period, actionid))
				{
					size_t eventsize = eventit->size();
					std::vector<Spatial::FMTCoordinate>eventcoordinates;
					if (eventsize < actionbind.getMinimalSize()||!conflictonly)
					{
						for (const Spatial::FMTCoordinate& coordinate : eventit->getElements())
						{
							eventcoordinates.push_back(coordinate);
						}
					}else if (eventsize > actionbind.getMaximalSize())
						{
						std::vector<std::set<FMTCoordinate>::const_iterator>borders = eventit->getBorders();
						while (eventsize> actionbind.getMaximalSize()&&
							!borders.empty())
							{
							eventcoordinates.push_back(*borders.back());
							borders.pop_back();
							--eventsize;
							}
						}
					if (!eventcoordinates.empty())
					{
						coordinates.push_back(eventcoordinates);
					}
					
				}
			}
			++actionid;
		}
		if (coordinates.empty())
		{
			return getAreaConflictCoordinates(bindingactions, period, false);
		}

	}
	catch (...)
	{
		_exhandler->printExceptions("", "FMTSpatialSchedule::getAreaConflictCoordinates", __LINE__, __FILE__);
	}
	return coordinates;
}

std::vector<std::vector<Spatial::FMTCoordinate>>FMTSpatialSchedule::getAdjacencyConflictCoordinates(const actionbindings& bindingactions,const int& period, bool conflictonly) const
{
	std::vector<std::vector<Spatial::FMTCoordinate>>coordinates;
	try {
			boost::unordered_set<FMTEventRelation> relations;
			for (const Spatial::FMTBindingSpatialAction& actionbind : bindingactions.at(period - 1))
			{
			if (actionbind.isSpatiallyAdjacencyBinding())
				{
				std::vector<bool>actionids(bindingactions.at(period - 1).size(),false);
				for (const int& action : actionbind.getNeighbors())
					{
					actionids[action] = true;
					}
				if (!conflictonly)
				{
					for (FMTEventContainer::const_iterator cit : m_events.getEvents(period, actionids))
					{
						const std::vector<Spatial::FMTCoordinate>eventscoords(cit->getElements().begin(), cit->getElements().end());
						coordinates.push_back(eventscoords);
					}
					return coordinates;
				}
				
				const std::vector<FMTEventContainer::const_iterator>CONFLICTS = _getAdjacencyConflict(period, 
												static_cast<int>(actionbind.getMinimalGreenUp()),
											static_cast<int>(actionbind.getMinimalAdjacency()),
											static_cast<int>(actionbind.getMaximalAdjacency()),
												actionbind.testMinimalAdjacency(),actionids);

				for (FMTEventContainer::const_iterator cit : CONFLICTS)
					{
					const std::vector<Spatial::FMTCoordinate>eventscoords(cit->getElements().begin(), cit->getElements().end());
					coordinates.push_back(eventscoords);
					}
				}
			}
		if (conflictonly && coordinates.empty())
			{
				return getAdjacencyConflictCoordinates(bindingactions, period, false);
			}
	}catch (...)
		{
		_exhandler->printExceptions("", "FMTSpatialSchedule::getAdjacencyConflictCoordinates", __LINE__, __FILE__);
		}
	return coordinates;
}



std::vector<Spatial::FMTCoordinate>FMTSpatialSchedule::getMovableCoordinates(const Models::FMTModel& model, const int& period) const
{
	std::vector<Spatial::FMTCoordinate>coordinates;
	try {
		coordinates.reserve(this->mapping.size());
		//if (statics!=nullptr)
		//{
		for (const Spatial::FMTCoordinate& coordinate : m_Movables)
			{
			std::map<Spatial::FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphit = mapping.find(coordinate);
			if (graphit->second.getLineGraph().isMovable(model, period,m_OperabilityCache))
				{
				coordinates.push_back(coordinate);
				}
			}
		/* } else {
			for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
				if (graphit->second.getLineGraph().ismovable(actions, model.yields, period,operability))
				{
					coordinates.push_back(graphit->first);
				}
			}

			}*/


		
	}
	catch (...)
	{
		_exhandler->printExceptions("", "FMTSpatialSchedule::getgraphs", __LINE__, __FILE__);
	}
	return coordinates;
}

bool FMTSpatialSchedule::isPartial() const
{
	return (m_scheduleType == FMTSpatialScheduletype::FMTpartial);
}

bool FMTSpatialSchedule::emptyEvents() const
{
	return m_events.empty();
}

void FMTSpatialSchedule::copyFromPartial(FMTSpatialSchedule& rhs)
{
	try {
		if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot use a non complete schedule ",
				"FMTSpatialSchedule::copyFromPartial", __LINE__, __FILE__);
		}
		if (rhs.m_scheduleType != FMTSpatialScheduletype::FMTpartial)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot copy from complete solution ",
				"FMTSpatialSchedule::copyFromPartial", __LINE__, __FILE__);
		}
		//m_Graphs.clear();
		for (std::map<FMTCoordinate, FMTVirtualLineGraph>::iterator graphit = rhs.mapping.begin(); graphit != rhs.mapping.end(); ++graphit)
		{
			mapping[graphit->first].setLineGraph(graphit->second.getLineGraph(), m_Tracker);
		}
		m_events.swap(rhs.m_events);
		m_ConstraintsFactor.swap(rhs.m_ConstraintsFactor);
		m_Tracker.swap(rhs.m_Tracker);
		assert(mapping.size() == _getNonSpatialCellsCount());
	}
	catch (...)
	{
		_exhandler->printExceptions("", "FMTSpatialSchedule::copyFromPartial", __LINE__, __FILE__);
	}

}

size_t FMTSpatialSchedule::_getNonSpatialCellsCount() const
	{
	return m_Tracker.getNonSpatialCellsCount();
	}

double FMTSpatialSchedule::_getExponentialFactorization(double p_value, double p_factor)
{
	double returned = 0.0;
	const double EXPONENTIAL_VALUE = std::exp(std::abs(p_value) *
							p_factor) - 1.0;
	if (p_value < FMT_DBL_TOLERANCE)
	{
		returned = -EXPONENTIAL_VALUE;
	}
	else {
		returned = EXPONENTIAL_VALUE;
	}
	return returned;
}

double FMTSpatialSchedule::_getConstraintFloorValue(double p_inValue) const
	{
	double returned = 0;
	if (p_inValue>FMT_DBL_TOLERANCE)
		{
		returned = static_cast<double>(m_ConstraintsFactor.size() - 1);
		}
	return returned;
	}

void FMTSpatialSchedule::setSpread(
	std::vector<FMTSpatialSchedule::EventSpread>::const_iterator p_first,
	std::vector<FMTSpatialSchedule::EventSpread>::const_iterator p_end)
{
	try {
		std::map<const FMTEvent*, FMTEvent>ToChange;
		for (std::vector<FMTSpatialSchedule::EventSpread>::const_iterator It = p_first; It != p_end; ++It)
			{
			ToChange.insert(std::pair<const FMTEvent*, FMTEvent>(&(*(It->m_Event)), *It->m_Event));
			}
		for (std::vector<FMTSpatialSchedule::EventSpread>::const_iterator It = p_first; It != p_end; ++It)
			{
			It->m_OutEvent->second.setLineGraph(
				It->m_InEvent->second.getLineGraph(),
				m_Tracker);
			ToChange[&(*It->m_Event)].insert(It->m_OutEvent->first);
			}
		FMTEventContainer ToRemove;
		FMTEventContainer ToAdd;
		for (const auto& NEW_EVENT : ToChange)
			{
			ToRemove.insert(*NEW_EVENT.first);
			ToAdd.insert(NEW_EVENT.second);
			}
		m_events.addUpdate(ToAdd, ToRemove);
	}catch (...)
		{
		_exhandler->printExceptions("",
			"FMTSpatialSchedule::setSpread", __LINE__, __FILE__);
		}
}

bool FMTSpatialSchedule::canDoEventSpread(int p_period) const
	{
	bool canDoIt = false;
	std::pair<FMTEventContainer::const_iterator,
		FMTEventContainer::const_iterator> BOUNDS = m_events.getBounds(p_period);
	FMTEventContainer::iterator it = BOUNDS.first;
	FMTEventContainer::iterator last = BOUNDS.second;
	while (it != last && !canDoIt)
		{
			for (const auto& COORDINATE : it->getOutsideBordersPair())
			{
				FMTLayer::const_iterator CANDIDATE_IT = find(COORDINATE.second);
				if (CANDIDATE_IT != end()) // InMap
				{
					FMTLayer::const_iterator BASE_IT = find(COORDINATE.first);
					const Graph::FMTLineGraph& CANDIDATE = CANDIDATE_IT->second.getLineGraph();
					const Graph::FMTLineGraph& BASE = BASE_IT->second.getLineGraph();
					if (CANDIDATE.isOnlyGrow() &&
						BASE.isSameBase(CANDIDATE))
					{
						canDoIt = true;
					}
				}
			}
			++it;
		}
	return canDoIt;
	}


std::vector<FMTSpatialSchedule::EventSpread> FMTSpatialSchedule::getPotentialSpread(int p_period)
{
	std::vector<FMTSpatialSchedule::EventSpread>ValidSpreads;
	try {
		std::pair<FMTEventContainer::const_iterator,
			FMTEventContainer::const_iterator> BOUNDS = m_events.getBounds(p_period);
		FMTEventContainer::iterator it = BOUNDS.first;
		FMTEventContainer::iterator last = BOUNDS.second;
		std::set<FMTCoordinate>selected;
		for (;it!=last;++it)
			{
			for (const auto& COORDINATE : it->getOutsideBordersPair())
				{
				FMTLayer::iterator CANDIDATE_IT = find(COORDINATE.second);
				if (CANDIDATE_IT!=end()) // InMap
					{
					FMTLayer::const_iterator BASE_IT = find(COORDINATE.first);
					const Graph::FMTLineGraph& CANDIDATE = CANDIDATE_IT->second.getLineGraph();
					const Graph::FMTLineGraph& BASE = BASE_IT->second.getLineGraph();
					if (CANDIDATE.isOnlyGrow() &&
						BASE.isSameBase(CANDIDATE)&&
						selected.insert(CANDIDATE_IT->first).second)
						{
						ValidSpreads.push_back(FMTSpatialSchedule::EventSpread(BASE_IT, CANDIDATE_IT, it));
						}
					}
				}
			}
		}catch (...)
			{
			_exhandler->printExceptions("", 
				"FMTSpatialSchedule::getPotentialSpread", __LINE__, __FILE__);
			}
	return ValidSpreads;
}




bool FMTSpatialSchedule::needsRefactorization(const FMTSpatialGraphs& p_Graphs) const
{
	try {
		if (!m_ConstraintsFactor.empty())
			{
			size_t cntid = 0;
			for (const double& value : getConstraintsValues(p_Graphs))
				{
				const double valuewfactor = m_ConstraintsFactor.at(cntid)*value;
				if (valuewfactor >1000||valuewfactor <-1000)
					{
					return true;
					}
				++cntid;
				}
			}
	}
	catch (...)
	{
		_exhandler->printExceptions("", "FMTSpatialSchedule::needsrefactortorization", __LINE__, __FILE__);
	}
return false;
}

void FMTSpatialSchedule::setConstraintsFactor(const Models::FMTModel& model,const std::vector<double>&factors)
	{
	try {
		if (model.constraints.size()!=factors.size())
		{
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Cannot set factors with size different from constraints",
				"FMTSpatialSchedule::setConstraintsFactor", __LINE__, __FILE__);
		}
		m_ConstraintsFactor = factors;
		double minimalfactor = std::numeric_limits<double>::max();
		double maximalfactor = -std::numeric_limits<double>::max();
		size_t cntid = 0;
		for (const double& value : factors)
			{
			if (value<0)
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Cannot set negative factor "+std::to_string(value)+" for constraint "+std::string(model.constraints.at(cntid)),
					"FMTSpatialSchedule::setConstraintsFactor", __LINE__, __FILE__);
				}
			if (value<minimalfactor)
				{
				minimalfactor = value;
				}
			if (value>maximalfactor)
				{
				maximalfactor = value;
				}
			++cntid;
			}
		_logger->logWithLevel("Constraints normalization Min(" + std::to_string(minimalfactor) + ") Max(" + std::to_string(maximalfactor) + ")\n", 2);
	}catch (...)
		{
		_exhandler->printExceptions("", "FMTSpatialSchedule::setConstraintsFactor", __LINE__, __FILE__);
		}

	}


const std::vector<double>& FMTSpatialSchedule::getConstraintsFactor() const
	{
	return m_ConstraintsFactor;
	}




void FMTSpatialSchedule::setStaticsMovableCoordinates(const Models::FMTModel& p_model)
	{
	if (m_Movables.empty())
		{
		try {
			m_Movables.reserve(this->mapping.size());
			const int lastPeriod = this->mapping.begin()->second.getLineGraph().getPeriod() - 1;
			for (std::map<FMTCoordinate, FMTVirtualLineGraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				for (int period = 1; period <= lastPeriod; ++period)
				{
					if (graphit->second.getLineGraph().isMovable(p_model, period,m_OperabilityCache))
					{
						m_Movables.push_back(graphit->first);
						break;
					}
				}
			}
			std::sort(m_Movables.begin(), m_Movables.end());
		}catch (...)
			{
			_exhandler->printExceptions("", 
				"FMTSpatialSchedule::setStaticsMovableCoordinates", __LINE__, __FILE__);
			}
		}
	}

void FMTSpatialSchedule::enableSolutionTracker(const FMTSpatialGraphs& p_SpatialGraph)
{
	try {
		p_SpatialGraph.enableSolutionTracker(this->m_Tracker);
	}catch (...)
	{
		_exhandler->printExceptions("",
			"FMTSpatialSchedule::enableSolutionTracker", __LINE__, __FILE__);
	}
}
void FMTSpatialSchedule::disableSolutionTracker()
{
	try {
		this->m_Tracker.disableTracking();
	}catch (...)
	{
		_exhandler->printExceptions("",
			"FMTSpatialSchedule::disableSolutionTracker", __LINE__, __FILE__);
	}
}




bool FMTSpatialSchedule::isBetterByGroup(const FMTSpatialSchedule& rhs, const FMTSpatialGraphs& p_Graphs) const
	{
	try {
		if (m_scheduleType != FMTSpatialScheduletype::FMTcomplete)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot use a non complete schedule ",
				"FMTSpatialSchedule::isBetterByGroup", __LINE__, __FILE__);
		}
		size_t gotbetter = 0;
		size_t groupid = 0;
		const std::vector<int>groupvalues = isBetterThan(rhs, p_Graphs);
		for (const int& value : groupvalues)
		{
			if (value >= 0)
			{
				++gotbetter;
			}
			++groupid;
		}
		return (gotbetter == groupvalues.size());
	}catch (...)
		{
		_exhandler->printExceptions("", "FMTSpatialSchedule::isBetterByGroup", __LINE__, __FILE__);
		}
	return false;
	}


}
