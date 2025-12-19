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


namespace Spatial
{



    FMTSpatialSchedule::FMTSpatialSchedule():FMTlayer<FMTVirtualLineGraph>(),
		scheduletype(), constraintsfactor(),m_events(), m_NonSpatialSolution()
    {
        //ctor
    }

    FMTSpatialSchedule::FMTSpatialSchedule(const FMTforest& p_InitialMap, 
										size_t p_LengthReserve,
										FMTSpatialGraphs& p_SpatialGraph) :
		FMTlayer<FMTVirtualLineGraph>(),  scheduletype(FMTSpatialScheduletype::FMTcomplete),
		constraintsfactor(), m_events(), m_NonSpatialSolution()
    {
        FMTlayer<FMTVirtualLineGraph>::operator = (p_InitialMap.copyextent<FMTVirtualLineGraph>());//Setting layer information
		m_NonSpatialSolution = p_SpatialGraph.GetBaseSolution();
		boost::unordered_map<Core::FMTdevelopment, FMTcoordinate>cacheGraph;
		size_t id = 0;
        for(FMTlayer<Core::FMTdevelopment>::const_iterator devit = p_InitialMap.begin(); devit != p_InitialMap.end(); ++devit)
        {
			if (cacheGraph.find(devit->second)==cacheGraph.end())
				{
				std::vector<Core::FMTactualdevelopment> actdevelopment(1, Core::FMTactualdevelopment(devit->second, p_InitialMap.getcellsize()));
				actdevelopment[0] = actdevelopment.at(0).reducelocktodeath(p_SpatialGraph.GetModel().lifespan);
				Graph::FMTlinegraph local_graph(p_LengthReserve);
				std::queue<Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor> actives = local_graph.initialize(actdevelopment);
				mapping[devit->first] = p_SpatialGraph.GetVirtualGraph(local_graph);
				cacheGraph[devit->second] = devit->first;
			}else {
				mapping[devit->first] = mapping[cacheGraph[devit->second]];
				}
			++id;
        }
    }

	FMTSpatialSchedule FMTSpatialSchedule::GetBaseSchedule(const FMTSpatialGraphs& p_SpatialGraph) const
	{
		FMTSpatialSchedule NewSchedule(*this);
		for (auto& GRAPH : NewSchedule)
			{
			GRAPH.second.SetBaseGraph(NewSchedule.m_NonSpatialSolution);
			}
		NewSchedule.m_events.clear();
		return NewSchedule;
	}

	void FMTSpatialSchedule::SetSpatialGraphs(const Spatial::FMTSpatialSchedule& p_ToCopy,
														FMTSpatialGraphs& p_SpatialGraph)
	{
		try {
			mapping.clear();
			FMTlayer<FMTVirtualLineGraph>::operator=(p_ToCopy.copyextent<FMTVirtualLineGraph>());
			for (const auto& DATA : p_ToCopy)
				{
				mapping[DATA.first] =  FMTVirtualLineGraph(DATA.second, p_SpatialGraph);
				}
			m_NonSpatialSolution = p_ToCopy.m_NonSpatialSolution;
			m_events = p_ToCopy.m_events;
			scheduletype = p_ToCopy.scheduletype;
			constraintsfactor = p_ToCopy.constraintsfactor;
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::SetSpatialGraphs", __LINE__, __FILE__);
		}
	}



	/*FMTSpatialSchedule::FMTSpatialSchedule(FMTSpatialSchedule&& rhs) noexcept :
		FMTlayer<FMTVirtualLineGraph>(std::move(rhs)),
		scheduletype(std::move(rhs.scheduletype)),
		constraintsfactor(std::move(rhs.constraintsfactor)),
		m_events(std::move(rhs.m_events)),
		m_NonSpatialSolution(std::move(rhs.m_NonSpatialSolution))
		{
		
		}*/


    FMTSpatialSchedule::FMTSpatialSchedule(const FMTSpatialSchedule& other):
            FMTlayer<FMTVirtualLineGraph>(other),
			scheduletype(other.scheduletype),
			constraintsfactor(other.constraintsfactor),
            m_events(other.m_events),
			m_NonSpatialSolution(other.m_NonSpatialSolution)
    {

    }


	FMTSpatialSchedule::FMTSpatialSchedule(const FMTSpatialSchedule& other,
		const std::vector<FMTcoordinate>::const_iterator& firstcoord,
		const std::vector<FMTcoordinate>::const_iterator& endcoord):
		FMTlayer<FMTVirtualLineGraph>(),
		scheduletype(FMTSpatialScheduletype::FMTpartial),
		constraintsfactor(other.constraintsfactor),
		m_events(other.m_events),
		m_NonSpatialSolution(other.m_NonSpatialSolution)
	{
		FMTlayer<FMTVirtualLineGraph>::operator=(other.copyextent<FMTVirtualLineGraph>());
		std::vector<FMTcoordinate>::const_iterator it = firstcoord;
		while (it!= endcoord)
			{
			mapping[*it] = other.mapping.at(*it);
			++it;
			}
	}

	void FMTSpatialSchedule::swap(FMTSpatialSchedule& rhs)
		{
		try {
			FMTlayer<FMTVirtualLineGraph>::swap(rhs);
			std::swap(scheduletype, rhs.scheduletype);
			m_events.swap(rhs.m_events);
			m_NonSpatialSolution.swap(rhs.m_NonSpatialSolution);
			constraintsfactor.swap(rhs.constraintsfactor);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::swap", __LINE__, __FILE__);
			}
		}

    FMTSpatialSchedule& FMTSpatialSchedule::operator=(const FMTSpatialSchedule& rhs)
    {
        if (this == &rhs) return *this; // handle self assignment
        //assignment operator
        FMTlayer<FMTVirtualLineGraph>::operator = (rhs);
		scheduletype = rhs.scheduletype;
        this->m_events = rhs.m_events;
		m_NonSpatialSolution = rhs.m_NonSpatialSolution;
        return *this;
    }

    bool FMTSpatialSchedule::operator == (const FMTSpatialSchedule& rhs)const
    {
        if (!(m_events==rhs.m_events)) return false;
        for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator coordit = this->mapping.begin();
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

    int FMTSpatialSchedule::actperiod() const
    {
        //Graph::FMTlinegraph flgraph = mapping.begin()->second;
        return mapping.begin()->second.getLineGraph().getperiod();
    }


	std::vector<Core::FMTactualdevelopment>FMTSpatialSchedule::getarea(int period, bool beforegrowanddeath) const
	{
		std::vector<Core::FMTactualdevelopment>OutValues;
		try {
			if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::getforestperiod", __LINE__, __FILE__);
				}
			const double CELL_SIZE = getcellsize();
			std::map<Core::FMTdevelopment, double>outArea;
			for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				const Graph::FMTlinegraph& local_graph = graphit->second.getLineGraph();
				const std::vector<double> solutions(1, this->getcellsize());
				Core::FMTdevelopment graphDev;
				if (beforegrowanddeath)
				{
					graphDev = local_graph.getperiodstartdev(period);
				}
				else {
					graphDev = local_graph.getperiodstopdev(period);
				}
				if (outArea.find(graphDev)== outArea.end())
				{
					outArea[graphDev] = 0;
				}
				outArea[graphDev] += CELL_SIZE;
			}
			OutValues = std::vector<Core::FMTactualdevelopment>(outArea.size());
			size_t id = 0;
			for (const auto& out : outArea)
			{
				OutValues[id] = Core::FMTactualdevelopment(out.first, out.second);
				++id;
			}
			}catch (...)
				{
				_exhandler->raisefromcatch("For period " + std::to_string(period), "FMTSpatialSchedule::getarea", __LINE__, __FILE__);
				}
			return OutValues;
	}

    FMTforest FMTSpatialSchedule::getforestperiod(const int& period,bool periodstart) const
    {
        FMTforest forest(this->copyextent<Core::FMTdevelopment>());//Setting layer information
		try {
			if (scheduletype!=FMTSpatialScheduletype::FMTcomplete)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::getforestperiod", __LINE__, __FILE__);
				}
			const std::vector<double> solutions(1, this->getcellsize());
			for(std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				const Graph::FMTlinegraph& local_graph = graphit->second.getLineGraph();
				if(periodstart)
				{
					forest[graphit->first] = local_graph.getperiodstartdev(period);	
				}else{
					forest[graphit->first] = local_graph.getperiodstopdev(period);	
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("For period "+std::to_string(period), "FMTSpatialSchedule::getforestperiod", __LINE__, __FILE__);
			}
        return forest;
    }


	bool FMTSpatialSchedule::allow_action(const int& targetaction, const std::vector<Spatial::FMTbindingspatialaction>&bindingactions, const FMTcoordinate& location, const int& period) const
	{
		try
		{
			const size_t targetmaximalsize = bindingactions.at(targetaction).getmaximalsize();
			const int lowergup = static_cast<int>(bindingactions.at(targetaction).getminimalgreenup());
			const size_t loweradjacency = bindingactions.at(targetaction).getminimaladjacency();
			const uint16_t loweradjacencyof = static_cast<uint16_t>(loweradjacency);
			for (int green_up = std::max(1,period- lowergup); green_up <= period; ++green_up)
				{
				for (const int& mact : bindingactions.at(targetaction).getneighbors())
							{
							const uint16_t distance = static_cast<int>(loweradjacency + std::max(targetmaximalsize, bindingactions.at(mact).getmaximalsize()));
							const uint16_t minx = distance > location.getx() ? 0 : location.getx() - distance;
							const uint16_t miny = distance > location.gety() ? 0 : location.gety() - distance;
							const uint16_t maxofx = (distance + location.getx()) > maxx ? maxx : (distance + location.getx());
							const uint16_t maxofy = (distance + location.gety()) > maxy ? maxy : (distance + location.gety());
							const FMTcoordinate minimallocation(minx, miny);
							const FMTcoordinate maximallocation(maxofx, maxofy);
							for (const FMTeventcontainer::const_iterator eventit : m_events.getevents(green_up, mact, minimallocation, maximallocation))
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
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::allow_action", __LINE__, __FILE__);
		}
		return true;
	}


	std::vector<std::set<Spatial::FMTcoordinate>> FMTSpatialSchedule::getupdatedscheduling(
																	//const std::vector<Core::FMTaction>& actions,
																	const Models::FMTmodel& model,
																	const std::vector<int>& actiontargets,
																	//const Core::FMTschedule& selection,
																	boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>& cachedactions,
																	const std::vector<boost::unordered_set<Core::FMTdevelopment>>& scheduleoperabilities,
																	//const Core::FMTyields& yields,
																	bool schedule_only,
																	std::vector<std::set<Spatial::FMTcoordinate>> original,
																	std::vector<FMTcoordinate> updatedcoordinate) const
		{
		try {
			if (original.empty())
				{
				original.resize(model.actions.size());
				updatedcoordinate.reserve(mapping.size());
				for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator itc = mapping.begin(); itc != mapping.end(); ++itc)
					{
					updatedcoordinate.push_back(itc->first);
					}
				}
			//boost::unordered_map<Core::FMTdevelopment,std::vector<bool>>cachedactions;
			//cachedactions.reserve(updatedcoordinate.size());
			for (const FMTcoordinate& updated : updatedcoordinate)
				{
				const Graph::FMTlinegraph& lg = mapping.at(updated).getLineGraph();
				const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor& active = lg.getactivevertex();
				const Core::FMTdevelopment& active_development = lg.getdevelopment(active);
				//*_logger << "op at period " << active_development.getperiod() << "\n";
				const int lastactionid = lg.getinedgeactionid(active);
				/*if (active_development.period != selection.getperiod())
					{
					_exhandler->raise(Exception::FMTexc::FMTrangeerror,
						"Wrong developement/schedule period " + std::to_string(active_development.period),
						"FMTSpatialSchedule::getupdatedscheduling", __LINE__, __FILE__);
					}*/
				boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>::iterator cacheit = cachedactions.find(active_development);
				if (cacheit == cachedactions.end())
					{
					std::pair<boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>::iterator,bool>insertedpair = cachedactions.insert(std::make_pair(active_development, std::vector<bool>(model.actions.size(), false)));
					cacheit = insertedpair.first;
					for (const int& actionid : actiontargets)
						{
						if ((actionid > lastactionid || lastactionid < 0) &&
							((schedule_only && (inscheduleoperabilities(scheduleoperabilities, &active_development, actionid, model.actions.at(actionid))/*selection.operated(action, active_development)*/)) ||
							(!schedule_only && active_development.operable(model.actions.at(actionid), model.yields))))
						{
							/*if (model.actions.at(actionid).getname() == "ACT")
							{
								*_logger << "GOT op!" << "\n";
							}*/
							cacheit->second[actionid] = true;
						}
						else {
							cacheit->second[actionid] = false;
						}
						}
					}
				
				for (const int& actionid : actiontargets)
					{
					std::set<Spatial::FMTcoordinate>& settochange = original[actionid];
					if (cacheit->second.at(actionid))
					{
						settochange.insert(updated);
					}else if (!settochange.empty())//Pile of empty actions
						{
						settochange.erase(updated);
						}
					}

				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getupdatedscheduling", __LINE__, __FILE__);
			}
		return original;
		}



	std::set<FMTcoordinate> FMTSpatialSchedule::verifyspatialfeasability(const int& targetaction,
		const std::vector<Spatial::FMTbindingspatialaction>& bindingactions,
		const int& period, const std::set<FMTcoordinate>& operables) const
	{
		std::set<FMTcoordinate> spatialyallowable;
		try
		{
			for (std::set<FMTcoordinate>::const_iterator itc = operables.begin(); itc != operables.end(); ++itc)
			{
				if (allow_action(targetaction, bindingactions,*itc, period))
				{
					spatialyallowable.insert(*itc);
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::verifyspatialfeasability", __LINE__, __FILE__);
		}
		return spatialyallowable;
	}



	FMTeventcontainer FMTSpatialSchedule::buildharvest(const double & target, const Spatial::FMTbindingspatialaction& targetaction,
		std::default_random_engine & generator, std::set<FMTcoordinate> mapping_pass,
		const int& period, const int& actionid, std::vector<FMTcoordinate>& operated) const
	{
		//To gain efficiency, maybe tracking cell that have been ignit actually, we are suposing that we are trying every cell, but its not true because of the random generator
		double harvested_area = 0;
		FMTeventcontainer cuts;
		try {
			size_t count = mapping_pass.size();
			int tooclosecall = 0;
			int initdone = 0;
			int spreaddone = 0;
			const bool check_adjacency = (std::find(targetaction.getneighbors().begin(), targetaction.getneighbors().end(), actionid) != targetaction.getneighbors().end());
			if (!mapping_pass.empty())
			{
				std::set<FMTcoordinate>::const_iterator randomit;
				while (harvested_area < target && count > 0 && !mapping_pass.empty())
				{
					std::uniform_int_distribution<int> celldistribution(0, static_cast<int>(mapping_pass.size()) - 1);
					const int cell = celldistribution(generator);//Get cell to ignit
					randomit = mapping_pass.begin();
					std::advance(randomit, cell);
					FMTevent newcut;
					std::vector<std::set<FMTcoordinate>::const_iterator>actives = newcut.ignit(targetaction.getmaximalsize(),randomit, actionid, period);
					if (!actives.empty())
					{
						++initdone;
						if (newcut.spread(targetaction.getminimalsize(), targetaction.getmaximalsize(), targetaction.getminimalneighborsize(), mapping_pass, actives))
						{
							++spreaddone;
							bool tooclose = false;
							if (check_adjacency)
							{
								const size_t adjacency = targetaction.getminimaladjacency();
								const size_t maximaldistance = adjacency + targetaction.getmaximalsize();
								const unsigned int adjacencyof = static_cast<unsigned int>(adjacency);
								for (const std::set<FMTevent>::const_iterator cutit : cuts.getevents(period, newcut.getterritory(maximaldistance)))
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
								operated.reserve(newcut.elements.size());
								for (const FMTcoordinate& toremove : newcut.elements)
								{
									operated.push_back(toremove);
									mapping_pass.erase(toremove);
								}
								harvested_area += (static_cast<double>(newcut.elements.size())*cellsize);
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
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::buildharvest", __LINE__, __FILE__);
		}
		return cuts;
	}


	double FMTSpatialSchedule::operateevents(const FMTeventcontainer& cuts, const Core::FMTaction& action, const int& action_id, const Core::FMTtransition& Transition,
									 const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes)
	{
		double operatedarea = 0;
		try {
			for (const FMTevent& cut : cuts)
			{
				for (std::set<FMTcoordinate>::const_iterator coordit = cut.elements.begin(); coordit != cut.elements.end(); coordit++)
				{
					Graph::FMTlinegraph lg = mapping.at(*coordit).getLineGraph();
					const size_t pathssize = lg.operate(action, action_id, Transition, ylds, themes);
					mapping.at(*coordit).setLineGraph(lg, m_NonSpatialSolution);
					if (pathssize > 1)
					{
						_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than one verticies for the graph after operate ... See if you have multiple transitions. Coord at " + std::string(*coordit),
							"FMTSpatialSchedule::operateevents", __LINE__, __FILE__);
					}
							operatedarea += cellsize;
				}
			}
			m_events.merge(cuts);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::operateevents", __LINE__, __FILE__);
		}
		return operatedarea;
	}

	void FMTSpatialSchedule::operatecoord(const FMTcoordinate& coord,const Core::FMTaction& action, const int& action_id, const FMTbindingspatialaction& bindingspaction, const Core::FMTtransition& Transition,
					 const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes)
	{
		try
		{
			Graph::FMTlinegraph lg = mapping.at(coord).getLineGraph();
			const size_t pathssize = lg.operate(action, action_id, Transition, ylds, themes);
			mapping.at(coord).setLineGraph(lg, m_NonSpatialSolution);
			if (pathssize > 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than one verticies for the graph after operate ... See if you have multiple transitions. Coord at " + std::string(coord),
					"FMTSpatialSchedule::operatecoord", __LINE__, __FILE__);
			}
			m_events.addaction(coord,actperiod(),action_id,bindingspaction);
		}catch(...){
				_exhandler->printexceptions("", "FMTSpatialSchedule::operatecoord", __LINE__, __FILE__);
		}
	}

	void FMTSpatialSchedule::grow()
	{
		try {
			for (std::map<FMTcoordinate, FMTVirtualLineGraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				Graph::FMTlinegraph local_graph = graphit->second.getLineGraph();
				local_graph.grow();
				graphit->second.setLineGraph(local_graph, m_NonSpatialSolution);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::grow", __LINE__, __FILE__);
		}
	}


	std::vector<Core::FMTschedule> FMTSpatialSchedule::getschedules(
												const FMTSpatialGraphs p_Graphs, bool withlock) const
	{
		std::vector<Core::FMTschedule> Schedules;
		try {
			if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTspatialschedule::getschedules", __LINE__, __FILE__);
			}
			Schedules = p_Graphs.GetSchedules(m_NonSpatialSolution, withlock);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getschedules", __LINE__, __FILE__);
		}
		return Schedules;
	}


	double FMTSpatialSchedule::evaluatespatialadjacency(
		const int& period,
		const size_t& greenup,
		const size_t& lowerlookup,
		const size_t& upperlookup,
		const bool& testlower,
		const bool& testupper,
		std::vector<FMTeventcontainer::const_iterator>& conflicts,
		boost::unordered_set<FMTeventrelation>& relations,
		const std::vector<bool>& actionused) const
	{
		double returnvalue = 0;
		try {
			const int lowergup = static_cast<int>(greenup);
			size_t baselookup = 0;
			double lower = 0.0;
			double upper = 0.0;
			if (testlower)
			{
				baselookup = lowerlookup;
				lower = static_cast<double>(lowerlookup);
			}
			if (testupper)
			{
				baselookup = std::max(baselookup, upperlookup);
				upper = static_cast<double>(upperlookup);
			}


			for (const FMTeventcontainer::const_iterator eventit : m_events.getevents(period, actionused))
			{
				const uint16_t containerlookup = static_cast<uint16_t>(baselookup + eventit->size());

				//0//-//1//
				//-//-//-//
				//2//-//3//
				const std::vector<FMTcoordinate> enveloppe = eventit->getenveloppe();
				const uint16_t minimalx = containerlookup < enveloppe.at(0).getx() ? enveloppe.at(0).getx() - containerlookup : 0;
				const uint16_t minimaly = containerlookup < enveloppe.at(0).gety() ? enveloppe.at(0).gety() - containerlookup : 0;
				const uint16_t maximalx = enveloppe.at(3).getx() + containerlookup;
				const uint16_t maximaly = enveloppe.at(3).gety() + containerlookup;
				const FMTcoordinate minimalcoord(minimalx, minimaly);
				const FMTcoordinate maximalcoord(maximalx, maximaly);
				double totalwithincount = 0;
				for (int gupperiod = std::max(1, period - lowergup); gupperiod <= period; ++gupperiod)
				{
					const double periodfactor = static_cast<double>((lowergup - (period - gupperiod))) + 1;
					for (const FMTeventcontainer::const_iterator eventof : m_events.getevents(gupperiod, actionused, minimalcoord, maximalcoord))
					{
						if (eventit != eventof)//They will have the same address if it's the same event!
						{
							const FMTeventrelation ofrelation = eventof->getrelation(*eventit);
							const FMTeventrelation itrelation = eventit->getrelation(*eventof);
							if (relations.find(ofrelation) == relations.end() &&
								relations.find(itrelation) == relations.end())
							{
								if (testlower&&eventit->within(lowerlookup, *eventof)) //too close
								{
									returnvalue += ((lower - eventit->distance(*eventof)) * periodfactor);
									conflicts.push_back(eventof);
								}else if (testupper && !eventit->within(upperlookup, *eventof))
								{
									returnvalue += ((eventit->distance(*eventof) - upper) * periodfactor);
									conflicts.push_back(eventof);
								}
								relations.insert(ofrelation);
								relations.insert(itrelation);
							}

						}
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::evaluatespatialadjacency", __LINE__, __FILE__);
		}
		return returnvalue;
	}

	


	double FMTSpatialSchedule::evaluatespatialconstraint(const Core::FMTconstraint& spatialconstraint, const Models::FMTmodel& model/*,
		const FMTeventcontainer* subset*/) const
	{
	double returnvalue = 0;
	try {
		int periodstart = 0;
		int periodstop = 0;
		if (this->mapping.begin()->second.getLineGraph().constraintlenght(spatialconstraint, periodstart, periodstop))
		{
			const std::vector<bool>actionused = spatialconstraint.isActionsused(model.actions);
			const Core::FMTconstrainttype spatialconstrainttype = spatialconstraint.getconstrainttype();
			/*FMTeventcontainer const* container = &events;
			if (subset!=nullptr)
				{
				container = subset;
				}*/
			double lower = 0;
			double upper = 0;
			if (spatialconstrainttype==Core::FMTconstrainttype::FMTspatialsize)
				{
				for (int period = periodstart; period <= periodstop; ++period)
					{
					spatialconstraint.getbounds(lower, upper, period);
					for (const FMTeventcontainer::const_iterator& eventit : m_events.getevents(period, actionused))//container->getevents(period, actionused))
						{
						const double event_val = static_cast<double>(eventit->size());
						double event_objective = 0;
						if (event_val < lower)
							{
							event_objective = lower - event_val;
						}else if (event_val > upper)
							{
							event_objective = event_val - upper;
							}
						returnvalue += event_objective;
						}

					}
			}else if (spatialconstrainttype == Core::FMTconstrainttype::FMTspatialadjacency)
				{
				//const Core::FMTyldbounds boundsyld = spatialconstraint.getyldsbounds().at("GUP");
				const Core::FMTyldbounds& boundsyld = spatialconstraint.getyieldbound("GUP");
				//const int lowergup = static_cast<int>(boundsyld.getlower());
				//std::map<int, std::vector<FMTeventcontainer::const_iterator>>allevents;
				boost::unordered_set<FMTeventrelation>relations;
				for (int period = periodstart; period <= periodstop; ++period)
					{
					spatialconstraint.getbounds(lower, upper, period);
					const bool testlower = (lower == std::numeric_limits<double>::lowest()) ? false : true;
					const bool testupper = (upper == std::numeric_limits<double>::infinity()) ? false : true;
					std::vector<FMTeventcontainer::const_iterator>badevents;
					returnvalue += evaluatespatialadjacency(
						period,
						static_cast<size_t>(boundsyld.getlower()),
						static_cast<size_t>(lower),
						static_cast<size_t>(upper),
						testlower,
						testupper,
						badevents,
						relations,
						actionused);
					/*const size_t baselookup = testlower ? static_cast<size_t>(lower) : static_cast<size_t>(upper);//Add up the maximal size of all the actions!
					const unsigned int baselookupof = static_cast<unsigned int>(baselookup);
					std::vector<FMTeventcontainer::const_iterator> allevents = events.getevents(period, actionused);
					const size_t eventsize = allevents.size();
					if (container!=&events)
						{
						allevents = container->getevents(period, actionused);
						}
					for (const FMTeventcontainer::const_iterator eventit : allevents)
					{
						const unsigned int containerlookup = static_cast<unsigned int>(baselookup + eventit->size());
						
						//0//-//1//
						//-//-//-//
						//2//-//3//
						const std::vector<FMTcoordinate> enveloppe = eventit->getenveloppe();
						const unsigned int minimalx = containerlookup < enveloppe.at(0).getx() ? enveloppe.at(0).getx() - containerlookup : 0;
						const unsigned int minimaly = containerlookup < enveloppe.at(0).gety() ? enveloppe.at(0).gety() - containerlookup : 0;
						const unsigned int maximalx = enveloppe.at(3).getx() + containerlookup;
						const unsigned int maximaly = enveloppe.at(3).gety() + containerlookup;
						const FMTcoordinate minimalcoord(minimalx, minimaly);
						const FMTcoordinate maximalcoord(maximalx, maximaly);
						double totalwithincount = 0;
						for (int gupperiod = std::max(1,period- lowergup);gupperiod<=period;++gupperiod)
							{
							const double periodfactor = static_cast<double>((lowergup - (period - gupperiod)))+1;
							for (const FMTeventcontainer::const_iterator eventof : events.getevents(gupperiod, actionused, minimalcoord, maximalcoord))
							{
								if (eventit != eventof)//They will have the same address if it's the same event!
								{
									const FMTeventrelation ofrelation = eventof->getrelation(*eventit);
									const FMTeventrelation itrelation = eventit->getrelation(*eventof);
									if (relations.find(ofrelation) == relations.end()&&
										relations.find(itrelation) == relations.end())
									{
										if (eventit->within(baselookupof, *eventof)) //too close
										{
											if (testlower)
											{
												returnvalue += ((lower - eventit->distance(*eventof))*periodfactor);
											}
											++totalwithincount;
										}
										relations.insert(ofrelation);
										relations.insert(itrelation);
									}

								}
							}
							if (testupper)
							{
								returnvalue += ((static_cast<double>(eventsize) - totalwithincount)*upper)*periodfactor;
							}
							}
					}*/

					}

				}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTSpatialSchedule::evaluatespatialconstraint", __LINE__, __FILE__);
		}
	return returnvalue;
	}


	double FMTSpatialSchedule::getconstraintevaluation(const FMTSpatialGraphs& p_Graphs, size_t p_ConstraintId) const
	{
		double value = 0;
		try {
			#ifndef NDEBUG
				assert(mapping.size() == _GetNonSpatialCellsCount());
			#endif
			const Core::FMTconstraint& CONSTRAINT = p_Graphs.GetModel().constraints.at(p_ConstraintId);
			if (!CONSTRAINT.isspatial())
			{
				int pStart, pStop = 0;
				if (!this->mapping.empty() && 
					this->mapping.begin()->second.getLineGraph().constraintlenght(CONSTRAINT, pStart, pStop))
				{
					value = p_Graphs.GetConstraintsValue(p_ConstraintId, m_NonSpatialSolution);
				}	
			}
			else
				{//evaluate spatial stuff
				value = this->evaluatespatialconstraint(CONSTRAINT, p_Graphs.GetModel());
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getconstraintevaluation", __LINE__, __FILE__);
		}
	return value;
	}

	std::vector<double> FMTSpatialSchedule::getweightedfactors(const Spatial::FMTSpatialGraphs& p_Graphs) const
	{
		std::vector<double>allvalues;
		try {
			double objective = 0;
			double infeasibilities = 0;
			getsolutionstatus(objective, infeasibilities, p_Graphs, false, false);
			const double global = (objective + infeasibilities);
			std::vector<Core::FMTconstraint>constraints = p_Graphs.GetModel().getconstraints();
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
			for (const Core::FMTconstraint& constraint : constraints)
				{
				const double constraintvalue = getconstraintevaluation(p_Graphs,ConstraintId);
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
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getweightedfactors", __LINE__, __FILE__);
		}
		return allvalues;

	}


	std::vector<double> FMTSpatialSchedule::getconstraintsvalues(const Spatial::FMTSpatialGraphs& p_Graphs) const
	{
		std::vector<double>allvalues;
		try {
			#ifndef NDEBUG
				assert(mapping.size() == _GetNonSpatialCellsCount());
			#endif
			std::vector<Core::FMTconstraint>constraints = p_Graphs.GetModel().getconstraints();
			allvalues.push_back(this->getobjectivevalue(p_Graphs,false));
			constraints.erase(constraints.begin());
			size_t ConstraintId = 1;
			for (const Core::FMTconstraint& constraint : constraints)
				{
				allvalues.push_back(getconstraintevaluation(p_Graphs, ConstraintId));
				++ConstraintId;
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getconstraintsvalues", __LINE__, __FILE__);
			}
		return allvalues;
	}

	double FMTSpatialSchedule::getprimalinfeasibility(const std::vector<const Core::FMTconstraint*>& constraints, 
									const Spatial::FMTSpatialGraphs& p_Graphs, bool withfactorization) const
	{
		double value = 0;
		try {
			size_t fid = 1;
			for (const Core::FMTconstraint* constraint: constraints)
				{
				const size_t ID = std::distance(&(*p_Graphs.GetModel().constraints.begin()), constraint);
				double cntvalue = getconstraintevaluation(p_Graphs, ID);
				if (withfactorization && !constraintsfactor.empty())
					{
					cntvalue *= constraintsfactor.at(fid);
					}
					value += cntvalue;
				fid += 1;
				}

		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getprimalinfeasibility", __LINE__, __FILE__);
			}
		return value;
	}

	double FMTSpatialSchedule::getobjectivevalue(const FMTSpatialGraphs& p_Graphs,bool withsense) const
	{
		double value = 0;
		try {
			value = p_Graphs.GetConstraintsValue(0, m_NonSpatialSolution)*
					(withsense ? p_Graphs.GetModel().constraints.at(0).sense() : 1);

		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getobjectivevalue", __LINE__, __FILE__);
		}
		return value;
	}


	std::vector<int> FMTSpatialSchedule::isbetterthan(const FMTSpatialSchedule& newsolution,
											const Spatial::FMTSpatialGraphs& p_Graphs) const
		{
		std::vector<int> groupevaluation;
		try {
			if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::isbetterthan", __LINE__, __FILE__);
			}
			const std::vector<Core::FMTconstraint> constraints = p_Graphs.GetModel().getconstraints();
			size_t maximalgroup = 0;
			for (const Core::FMTconstraint& constraint : constraints)
				{
				const size_t groupid = constraint.getgroup();
				if (groupid> maximalgroup)
					{
					maximalgroup = groupid;
					}
				}
			groupevaluation = std::vector<int>(maximalgroup + 1, 0);
			std::vector<double>groupsprimalinfeasibilitygap(maximalgroup + 1, 0);
			size_t i = 0;
			for (const Core::FMTconstraint& constraint : constraints)
				{
				const double oldvalue = this->getconstraintevaluation(p_Graphs,i);
				const double newvalue= newsolution.getconstraintevaluation(p_Graphs,i);
				if (!(newvalue==0 && oldvalue==0))
					{
					const size_t groupid = constraint.getgroup();
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
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::isbetterthan", __LINE__, __FILE__);
			}
		return groupevaluation;
		}

	void FMTSpatialSchedule::logsolutionstatus(const size_t& iteration,const double& objective, const double& primalinfeasibility) const
	{
	try {
		if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot use a non complete schedule ",
				"FMTSpatialSchedule::logsolutionstatus", __LINE__, __FILE__);
		}
		_logger->logwithlevel("Iteration "+std::to_string(iteration)+" Primal Inf(" + std::to_string(primalinfeasibility) + ") Obj(" + std::to_string(objective) + ")\n", 1);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTSpatialSchedule::logsolutionstatus", __LINE__, __FILE__);
		}
	}

	void FMTSpatialSchedule::getsolutionstatus(double& objective, double& primalinfeasibility, 
		const FMTSpatialGraphs& p_Graphs, bool withsense,bool withfactorization, bool withspatial) const
	{
		try {
			//std::vector<Core::FMTconstraint>constraints = model.getconstraints();
			objective = this->getobjectivevalue(p_Graphs, withsense);
			if (withfactorization&&!constraintsfactor.empty())
				{
				objective = (objective*constraintsfactor.at(0));
				}
			/*constraints.erase(constraints.begin());
			std::vector<Core::FMTconstraint>constraintssubset;
			for (const Core::FMTconstraint& constraint : constraints)
				{
				if (withspatial || !constraint.isspatial())
					{
					constraintssubset.push_back(constraint);
					}

				}*/
			std::vector<const Core::FMTconstraint*>constraintssubset;
			constraintssubset.reserve(p_Graphs.GetModel().constraints.size());
			for (size_t cid = 1;cid < p_Graphs.GetModel().constraints.size();++cid)
			{
				if (withspatial || !p_Graphs.GetModel().constraints.at(cid).isspatial())
				{
					constraintssubset.push_back(&p_Graphs.GetModel().constraints.at(cid));
				}
			}
			primalinfeasibility = this->getprimalinfeasibility(constraintssubset, p_Graphs, withfactorization);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getsolutionstatus", __LINE__, __FILE__);
		}
	}

	double FMTSpatialSchedule::getglobalobjective(const FMTSpatialGraphs& p_Graphs) const
	{
		double global = 0;
		try {
			double objective = 0;
			double infeasibilities = 0;
			getsolutionstatus(objective, infeasibilities, p_Graphs,false,true);
			global = (objective + infeasibilities);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getglobalobjective", __LINE__, __FILE__);
			}
		return global;
	}

	

	std::map<std::string, double> FMTSpatialSchedule::getoutput(const FMTSpatialGraphs& p_Graphs,
		const Core::FMToutput& p_output,
		int p_period, Core::FMToutputlevel level ) const
	{
		std::map<std::string,double>values;
		try {
			
			values = p_Graphs.GetOutput(m_NonSpatialSolution, p_output, p_period, level);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getoutput", __LINE__, __FILE__);
		}
		return values;
		
	}


	
	


	FMTlayer<double> FMTSpatialSchedule::getSpatialOutput(const Models::FMTmodel& model, const Core::FMToutput& output, const int& period) const
	{
		FMTlayer<double>outputlayer = copyextent<double>();
		try {
			for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				const double GRAPH_VALUE = graphit->second.GetOutput(model,
											m_NonSpatialSolution, output, period);
				if ((std::abs(GRAPH_VALUE) - FMT_DBL_TOLERANCE) > 0)
				{
					outputlayer[graphit->first] = GRAPH_VALUE;
				}
			}

		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getSpatialOutput", __LINE__, __FILE__);
		}
		return outputlayer;
	}
	


	std::vector<std::pair<FMTcoordinate, double>>FMTSpatialSchedule::getoutputbycoordinate(const Models::FMTmodel & model, 
														const Core::FMToutput& output, const int& period) const
	{
		std::vector<std::pair<FMTcoordinate, double>>allvalues;
		try {
			for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
				const double GRAPH_VALUE = graphit->second.GetOutput(model,
									m_NonSpatialSolution, output, period);
				if ((std::abs(GRAPH_VALUE)-FMT_DBL_TOLERANCE)>0)
					{
					allvalues.push_back(std::pair<FMTcoordinate, double>(graphit->first, GRAPH_VALUE));
					}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getoutputbycoordinate", __LINE__, __FILE__);
		}
		return allvalues;
	}


	std::string FMTSpatialSchedule::getpatchstats(const std::vector<Core::FMTaction>& actions) const
	{
		std::string result = "";
		try {
			if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::getpatchstats", __LINE__, __FILE__);
			}
			result += "Period Action "+FMTevent::getstatsheader()+"\n";
			for (int period =m_events.firstperiod(); period <= m_events.lastperiod(); ++period)
			{
				for (int action_id = 0; action_id < actions.size(); ++action_id)
				{
					std::vector<FMTeventcontainer::const_iterator> evsit = m_events.getevents(period, action_id);
					for (const auto& eventit : evsit)
					{
						result += std::to_string(period) + " " + actions.at(action_id).getname() + " " + eventit->getstats() + "\n";
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getpatchstats", __LINE__, __FILE__);
		}
		return result;
	}

	FMTlayer<std::string> FMTSpatialSchedule::lastdistlayer(const std::vector<Core::FMTaction>& modelactions, const int& period) const
	{
		FMTlayer<std::string> distlayer(this->copyextent<std::string>());
		try {
			if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::lastdistlayer", __LINE__, __FILE__);
				}
			for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
				const int lastactid = graphit->second.getLineGraph().getlastactionid(period);
				if (lastactid >= 0)
				{
					distlayer[graphit->first] = modelactions.at(lastactid).getname();
				}

			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::lastdistlayer", __LINE__, __FILE__);
		}
		return distlayer;
	}

	std::vector<std::vector<Graph::FMTpredictor>>FMTSpatialSchedule::getpredictors(FMTlayer<int>& predictorids, const Models::FMTmodel& model, const std::vector<std::string>& yieldnames, const int& period,bool periodonevalues, bool withGCBMid) const
	{
		std::vector<std::vector<Graph::FMTpredictor>>predictors;
		try {
			if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::getpredictors", __LINE__, __FILE__);
			}
			std::set<std::vector<Graph::FMTpredictor>>predictorsset;
			std::vector<std::set<std::vector<Graph::FMTpredictor>>::iterator>predictorlocalisations;
			for (std::map<FMTcoordinate, FMTVirtualLineGraph >::const_iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
				const std::vector<Graph::FMTpredictor> graphpredictors = graphit->second.getLineGraph().getperiodpredictors(period,model, yieldnames,periodonevalues,withGCBMid);
				std::set<std::vector<Graph::FMTpredictor>>::iterator setit = predictorsset.end();
				bool pushed=false;
				if (!graphpredictors.empty())
				{
					setit = predictorsset.find(graphpredictors);
					if (setit == predictorsset.end())
					{
						const std::pair<std::set<std::vector<Graph::FMTpredictor>>::iterator, bool> ret = predictorsset.insert(graphpredictors);
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
			for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphit = this->begin(); graphit != this->end(); ++graphit)
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
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getpredictors", __LINE__, __FILE__);
		}
		return predictors;
	}

	std::vector<Core::FMTGCBMtransition>FMTSpatialSchedule::getGCBMtransitions(FMTlayer<std::string>& stackedactions, const std::vector<Core::FMTaction>& modelactions, const std::vector<Core::FMTtheme>& classifiers, const int& period) const
	{
		std::vector<Core::FMTGCBMtransition>GCBM;
		try {
			if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot use a non complete schedule ",
					"FMTSpatialSchedule::getGCBMtransitions", __LINE__, __FILE__);
			}
			std::map<std::string, std::vector<int>> ageaftercontainer;
			std::map<std::string, std::map<std::string, std::map<std::string, int>>> finalattributes;
			//Iter through spatialschedule
			for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
				// lastaction id = -1 no action in period
				int lastactionid = graphit->second.getLineGraph().getlastactionid(period);
				if (lastactionid >= 0)
				{
					stackedactions[graphit->first] = modelactions.at(lastactionid).getname();
					//For each classifier, append the value at the begining of the period and keep track of value at the end in finalattributes. Also keep the ageafter.
					if (!classifiers.empty())
					{
						const Core::FMTdevelopment sdev = graphit->second.getLineGraph().getperiodstartdev(period);
						const Core::FMTdevelopment fdev = graphit->second.getLineGraph().getperiodstopdev(period);
						const Core::FMTdevelopment snpdev = graphit->second.getLineGraph().getperiodstartdev(period + 1);
						const int fage = snpdev.getage();
						std::map<std::string, std::string> themeattributes;
						for (const auto& theme : classifiers)
						{
							std::string themename = "THEME" + std::to_string(theme.getid() + 1);
							const std::string fclass = fdev.getmask().get(theme);
							themeattributes[themename] = fclass;
							const std::string sclass = sdev.getmask().get(theme);
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
				GCBM.push_back(Core::FMTGCBMtransition(ageafter, theme_collection, ageit->first));
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getGCBMtransitions", __LINE__, __FILE__);
		}
		return GCBM;
	}

	void FMTSpatialSchedule::eraselastperiod()
		{
		try {
			const int lastperiod = this->mapping.begin()->second.getLineGraph().getperiod() - 1;
			for (std::map<FMTcoordinate, FMTVirtualLineGraph>::iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
				{
				//Graph::FMTlinegraph copied = graphit->second.getLineGraph();
				//copied.clearfromperiod(lastperiod, true);
				//graphit->second.setLineGraph(copied,m_NonSpatialSolution);
				graphit->second.SetLastPeriod(m_NonSpatialSolution);
				}
			std::set<FMTevent>::const_iterator periodit = m_events.getbounds(lastperiod).first;
			while (periodit!=m_events.end())
				{
				periodit = m_events.erase(periodit);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::eraselastperiod", __LINE__, __FILE__);
			}

		}


	std::vector<FMTcoordinate>FMTSpatialSchedule::getfromevents(const Core::FMToutputnode& node, const std::vector<Core::FMTaction>& actions, const int& period) const
	{
		//std::vector<const Graph::FMTlinegraph*>graphs;
		std::vector<FMTcoordinate>coordinates;
		try {
			if (node.isActionbased()&& node.source.isvariable())
			{
				std::vector<bool>targetedactions(actions.size(),false);
				for (const Core::FMTaction* actionptr : node.source.targets(actions))
					{
					targetedactions[std::distance(&(*actions.cbegin()), actionptr)] = true;
					}
				for (std::set<FMTevent>::const_iterator eventit : m_events.getevents(period, targetedactions))
					{
						for (const FMTcoordinate& coordinate : eventit->elements)
						{
							coordinates.push_back(coordinate);
						}
					}
				std::sort(coordinates.begin(), coordinates.end());
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getfromevents", __LINE__, __FILE__);
		}
		return coordinates;

	}

	std::vector<const Graph::FMTlinegraph*>FMTSpatialSchedule::getfromevents(const Core::FMTconstraint& constraint, const std::vector<Core::FMTaction>& actions, const int& start,const int& stop) const
		{
		std::vector<const Graph::FMTlinegraph*>graphs;

		try {
		if (constraint.isActionbased())
			{
			std::vector<int>targetedactions;
			for (const Core::FMToutputsource& osource : constraint.getsources())
				{
				if (osource.isvariable())
					{
					for (const Core::FMTaction* actionptr : osource.targets(actions))
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
			std::set<FMTcoordinate>collected;
			for (int period = start; period <=stop; ++period)
				{
				for (std::set<FMTevent>::const_iterator eventit : m_events.getevents(period, targetedactions))
					{
					for (const FMTcoordinate& coordinate : eventit->elements)
						{
						const Graph::FMTlinegraph* graphptr = &(mapping.find(coordinate)->second.getLineGraph());
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
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::getfromevents", __LINE__, __FILE__);
			}
		return graphs;
		}

std::map<std::string,double> FMTSpatialSchedule::getoutputfromgraph(const Graph::FMTlinegraph& linegraph, const Models::FMTmodel & model,
											 const Core::FMToutputnode& node, const double* solution, const int&period,
											const Core::FMTmask& nodemask,
											boost::unordered_map<Core::FMTmask, double>& nodecache,
											const Core::FMTtheme* p_theme,Core::FMToutputlevel level)const
	{
	std::map<std::string, double> values;
	try{
	if (!(node.isActionbased()&&linegraph.isonlygrow(period)))
	{
		boost::unordered_map<Core::FMTmask, double>::const_iterator cashit = nodecache.find(nodemask);
		if (cashit != nodecache.end() && level == Core::FMToutputlevel::totalonly)//get it from cashing
		{
			values["Total"] = cashit->second;
		}else {
			if (level == Core::FMToutputlevel::standard)//Only feel the target theme
			{
				const int themeid = node.source.getthemetarget();
				if (themeid >= 0)
					{
					p_theme = &model.themes.at(themeid);
					}
			}
			values = linegraph.getsource(model, node, period, *p_theme, solution,level);
			if (level != Core::FMToutputlevel::developpement)//No caching for developpement
			{
				nodecache[nodemask] = values.at("Total");
			}
		}
	}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTSpatialSchedule::getoutputfromgraph", __LINE__, __FILE__);
		}
	return values;
	}

void FMTSpatialSchedule::postsolve(const Core::FMTmaskfilter& p_Filter,
								const std::vector<Core::FMTaction>& p_PresolveActions,
							const Models::FMTmodel& p_OriginalBaseModel)
	{
	try {
		const std::vector<Core::FMTtheme> postsolvethemes = p_OriginalBaseModel.getthemes();
		const std::vector<Core::FMTaction> postsolveactions = p_OriginalBaseModel.getactions();
		std::vector<int>actionmapping;
		actionmapping.reserve(p_PresolveActions.size());
		for (const Core::FMTaction action : p_PresolveActions)
		{
			const int loc = static_cast<int>(std::distance(postsolveactions.begin(), std::find_if(postsolveactions.begin(), postsolveactions.end(), Core::FMTactioncomparator(action.getname()))));
			actionmapping.push_back(loc);
		}
		
		std::vector<FMTcoordinate>coordinates;
		for (std::map<FMTcoordinate, FMTVirtualLineGraph>::iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
			Graph::FMTlinegraph graphCopy = graphit->second.getLineGraph();
			graphCopy.postsolve(p_Filter, postsolvethemes, actionmapping);
			graphit->second.setLineGraph(graphCopy, m_NonSpatialSolution);
			coordinates.push_back(graphit->first);
			}
		FMTeventcontainer newevents;
		for (FMTeventcontainer::iterator eventit= m_events.begin(); eventit!=m_events.end();eventit++)
			{
			FMTevent newevent(*eventit);
			newevent.setactionid(actionmapping.at(eventit->getactionid()));
			newevents.insert(newevent);
			}
		m_events.swap(newevents);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTSpatialSchedule::postsolve", __LINE__, __FILE__);
		}
	}

FMTSpatialSchedule FMTSpatialSchedule::presolve(const Core::FMTmaskfilter& p_filter,
	FMTSpatialGraphs& p_Graphs, size_t p_ReserveSize) const
	{
	FMTSpatialSchedule presolvedSchedule;
	try {
		if (actperiod() != 1)//just presolve if no solution
			{
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Cannot presolve a schedule with more than 1 period",
				"FMTSpatialSchedule::presolve", __LINE__, __FILE__);
			}
		presolvedSchedule.scheduletype = FMTSpatialScheduletype::FMTcomplete;
		presolvedSchedule.FMTlayer<FMTVirtualLineGraph>::operator = (copyextent<FMTVirtualLineGraph>());
		boost::unordered_map<Core::FMTdevelopment, FMTcoordinate>cacheGraph;
		std::vector<FMTcoordinate>coordinates(mapping.size());
		presolvedSchedule.m_NonSpatialSolution = p_Graphs.GetBaseSolution();
		size_t id = 0;
		for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphIt = mapping.begin(); graphIt != mapping.end(); ++graphIt)
			{
			//std::queue<Graph::FMTlinegraph::FMTvertex_descriptor> allDescriptors = graphIt->second.getLineGraph().getactiveverticies();
			const Core::FMTdevelopment& NON_PRESOLVED = graphIt->second.getLineGraph().getperiodstopdev(0);
			const Core::FMTactualdevelopment PRESOLVED_DEV = Core::FMTactualdevelopment(NON_PRESOLVED, getcellsize()).presolve(p_filter, p_Graphs.GetModel().themes);
			if (cacheGraph.find(PRESOLVED_DEV) == cacheGraph.end())
			{
				std::vector<Core::FMTactualdevelopment> actDevelopment;
				actDevelopment.push_back(PRESOLVED_DEV);
				Graph::FMTlinegraph local_graph(p_ReserveSize);
				std::queue<Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor> actives = local_graph.initialize(actDevelopment);
				presolvedSchedule.mapping[graphIt->first] = p_Graphs.GetVirtualGraph(local_graph);
				cacheGraph[PRESOLVED_DEV] = graphIt->first;
			}else {
				presolvedSchedule.mapping[graphIt->first] = presolvedSchedule.mapping[cacheGraph[PRESOLVED_DEV]];
				}
			coordinates[id] = graphIt->first;
			++id;
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTSpatialSchedule::presolve", __LINE__, __FILE__);
		}
	return presolvedSchedule;
	}


bool FMTSpatialSchedule::inscheduleoperabilities(const std::vector<boost::unordered_set<Core::FMTdevelopment>>& scheduleoperabilities,
	Core::FMTdevelopment const* dev, const int& actionid, const Core::FMTaction& action) const
{
	try {
		const boost::unordered_set<Core::FMTdevelopment>& location = scheduleoperabilities.at(actionid);
		if (!location.empty())
			{
			boost::unordered_set<Core::FMTdevelopment>::const_iterator opit;

			if (!action.dorespectlock())
			{
				const Core::FMTdevelopment target = dev->clearlock();
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
		_exhandler->raisefromcatch("", "FMTSpatialSchedule::inscheduleoperabilities", __LINE__, __FILE__);
	}
	return false;
}

FMTSpatialSchedule::actionbindings FMTSpatialSchedule::getbindingactionsbyperiod(const Models::FMTmodel& model) const
{
	std::vector<std::vector<Spatial::FMTbindingspatialaction>>allbindings;
	for (int period = 1 ; period < mapping.begin()->second.getLineGraph().getperiod(); ++period)
		{
		allbindings.push_back(getbindingactions(model,period));
		}
	return allbindings;
}


std::vector<Spatial::FMTbindingspatialaction> FMTSpatialSchedule::getbindingactions(const Models::FMTmodel& model, const int& period) const
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
	std::vector<Spatial::FMTbindingspatialaction>bindings;
	try {
		bindings.reserve(nactions);
		for (const Core::FMTconstraint& constraint : model.constraints)
		{
			if (constraint.isspatial() &&
				period >= constraint.getperiodlowerbound() &&
				period <= constraint.getperiodupperbound())
			{
				double lower = 0;
				double upper = 0;
				constraint.getbounds(lower, upper, period);
				const std::vector<int>actionids = constraint.getactionids(model.actions);
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
				if (constraint.getconstrainttype() == Core::FMTconstrainttype::FMTspatialsize)
				{
					//const Core::FMTyldbounds yldbounds = constraint.getyldsbounds().at("NSIZE");
					const Core::FMTyldbounds& yldbounds = constraint.getyieldbound("NSIZE");
					const double lowernsize = yldbounds.getlower();
					const double uppernsize = yldbounds.getupper();
					
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
				else if (constraint.getconstrainttype() == Core::FMTconstrainttype::FMTspatialadjacency)
				{
					//const Core::FMTyldbounds yldbounds = constraint.getyldsbounds().at("GUP");
					const Core::FMTyldbounds& yldbounds = constraint.getyieldbound("GUP");
					const double lowergup = yldbounds.getlower();
					const double uppergup = yldbounds.getupper();
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
		_exhandler->raisefromcatch("", "FMTSpatialSchedule::getbindingactions", __LINE__, __FILE__);
		}
	return bindings;
	}


std::map<std::string, double> FMTSpatialSchedule::referencebuild(const Core::FMTschedule& schedule,
	const Models::FMTmodel& model,
	const std::vector<boost::unordered_set<Core::FMTdevelopment>>& scheduleoperabilities,
	std::default_random_engine& p_randomEngine,
	bool schedule_only,
	bool scheduleatfirstpass)
{
	std::map<std::string, double>results;
	try {
		if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot use a non complete schedule ",
				"FMTSpatialSchedule::referencebuild", __LINE__, __FILE__);
		}
		const int period = this->actperiod();
		//this->setnewperiod();
		//std::default_random_engine generator(seed);
		double total_area = 0;
		std::vector<double>targets(model.actions.size(),0.0);
		std::vector<double>originaltargets(model.actions.size(),0.0);
		std::vector<int>actiontargets;
		int actionid = 0;
		for (const Core::FMTaction& action : model.actions)
			{
			const double actionarea = schedule.actionarea(action.getname());
			if (actionarea>0)
				{
				originaltargets[actionid]=actionarea;
				targets[actionid] = actionarea;
				actiontargets.push_back(actionid);
				total_area += actionarea;
				}
			++actionid;
			}
		std::vector<int>basetargets(actiontargets);
		double allocated_area = 0;
		const std::vector<Spatial::FMTbindingspatialaction>bindingactions = this->getbindingactions(model, period);
		if (!schedule.empty() || !schedule_only)
		{
			double pass_allocated_area = 0;
			bool schedulepass = scheduleatfirstpass;
			bool schedulechange = false;
			int pass = 0;
			boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>cachedactions;
			std::vector<std::set<Spatial::FMTcoordinate>>actions_operabilities = this->getupdatedscheduling(model, actiontargets, cachedactions, scheduleoperabilities, schedulepass);
			do {
				pass_allocated_area = 0;
				int action_id = 0;
				if (schedulechange)
				{
					cachedactions.clear();
					actions_operabilities = this->getupdatedscheduling(model, actiontargets, cachedactions, scheduleoperabilities, schedulepass);
					schedulechange = false;
				}
				std::vector<int>targetstoremove;
				for (const int& action_id : actiontargets)
				{
					const double& action_area = targets.at(action_id);
					const std::set<Spatial::FMTcoordinate>& allowable_coordinates = actions_operabilities.at(action_id);
					if (!allowable_coordinates.empty() && action_area > 0)
					{
						const std::set<Spatial::FMTcoordinate> spatialy_allowable = this->verifyspatialfeasability(action_id, bindingactions, period, allowable_coordinates);
						if (!spatialy_allowable.empty())
						{
							std::vector<Spatial::FMTcoordinate> updatedcells;
							const Spatial::FMTeventcontainer harvest = this->buildharvest(action_area, bindingactions.at(action_id), p_randomEngine, spatialy_allowable, period, action_id, updatedcells);
							if (harvest.size() > 0)
							{
								const double operatedarea = this->operateevents(harvest,model.actions.at(action_id), action_id, model.transitions[action_id], model.yields, model.themes);
								actions_operabilities = this->getupdatedscheduling(model, actiontargets, cachedactions, scheduleoperabilities, schedulepass, actions_operabilities, updatedcells);
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
			results[model.actions.at(action_id).getname()] = ((originalvalue - targets.at(action_id)) / originalvalue);
			}
	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTSpatialSchedule::referencebuild", __LINE__, __FILE__);
	}
	return results;


}

std::map<std::string, double> FMTSpatialSchedule::greedyreferencebuild(const Core::FMTschedule& schedule, const FMTSpatialGraphs& p_Graphs,
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
	const std::vector<boost::unordered_set<Core::FMTdevelopment>>scheduleoperabilities = schedule.getoperabilities(p_Graphs.GetModel().actions);
	size_t stalcount = 0;
	size_t iteration = 0;
	const unsigned int initialseed = seed;
	try {
		if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot use a non complete schedule ",
				"FMTSpatialSchedule::greedyreferencebuild", __LINE__, __FILE__);
		}
		bestresults["Total"] = 0;
		const double totaliterations = static_cast<double>(randomiterations);
		double lastprimalinf = 0;
		double lastobjective = 0;
		double lastschedulefactor = 1;
		size_t failediterations = 0;
		bool didregular = false;
		bool mergingprimal = true;
		const double objsense = p_Graphs.GetModel().constraints.at(0).sense();
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
			const Core::FMTschedule factoredschedule = schedule.getnewschedule(schedulefactor);
			const std::map<std::string, double>results = solutioncopy.referencebuild(factoredschedule, p_Graphs.GetModel(), scheduleoperabilities, generator, false, true);
			double newprimalinf = 0;
			double newobjective = 0;
			solutioncopy.getsolutionstatus(newobjective, newprimalinf, p_Graphs, true, false, false);
			const double primalgap = (newprimalinf - lastprimalinf);
			const double objgap = ((newobjective * objsense) - (lastobjective * objsense));
			if (iteration == 0 || (mergingprimal && (primalgap <= FMT_DBL_TOLERANCE)) ||
				(!mergingprimal && ((primalgap <= FMT_DBL_TOLERANCE) && (objgap <= FMT_DBL_TOLERANCE))))/*solutioncopy.isbetterbygroup(*this, model)*/
			{
				bestresults = results;
				lastschedulefactor = schedulefactor;

				if (iteration == 0)
				{
					*this = solutioncopy;
					this->getsolutionstatus(newobjective, newprimalinf, p_Graphs, true, false, false);
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
				this->logsolutionstatus(iteration, lastobjective, lastprimalinf);
			}
			solutioncopy.eraselastperiod();//clear the last period to redo a simulate and test again!
			++seed;
			++iteration;
		}
		if (log && stalcount == maxstall)
		{
			_logger->logwithlevel("Stalled after " + std::to_string(iteration) + " iterations Skipping\n", 1);
		}
		if (log && failediterations == randomiterations)
		{
			_logger->logwithlevel("Solution stuck after " + std::to_string(iteration) + " iterations Skipping\n", 1);
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
		_exhandler->printexceptions("", "FMTsesmodel::greedyreferencebuild", __LINE__, __FILE__);
	}

	return bestresults;
}

Graph::FMTgraphstats FMTSpatialSchedule::randombuild(const Models::FMTmodel& model, std::default_random_engine& generator)
	{
	Graph::FMTgraphstats periodstats;
	try {
		#ifndef NDEBUG
				assert(mapping.size() == _GetNonSpatialCellsCount());
		#endif
		boost::unordered_map<Core::FMTdevelopment,std::vector<int>>operability;
		const int period = this->mapping.begin()->second.getLineGraph().getperiod();
		const std::vector<FMTbindingspatialaction> bindings = getbindingactions(model, period);
		std::uniform_int_distribution<int> dosomethingactions(1, model.getparameter(Models::FMTintmodelparameters::LENGTH));
		for (std::map<FMTcoordinate, FMTVirtualLineGraph>::iterator graphit = mapping.begin(); graphit !=mapping.end(); ++graphit)
			{
			Graph::FMTlinegraph local_graph = graphit->second.getLineGraph();
			if (dosomethingactions(generator) == 1)//Do something with 1 chance on length of model!
			{
				size_t maxsize = 0;
				const std::vector<int>actionids = local_graph.randombuildperiod(model, generator, operability);
				if (!actionids.empty())
				{
					m_events.addactions(graphit->first, period, actionids, bindings);
				}
			}
			else { 
				local_graph.grow();
			}
			periodstats += local_graph.getstats();
			graphit->second.setLineGraph(local_graph,m_NonSpatialSolution);

			}
	#ifndef NDEBUG
		assert(mapping.size() == _GetNonSpatialCellsCount());
	#endif


	}catch (...)
		{
		_exhandler->printexceptions("", "FMTSpatialSchedule::randombuild", __LINE__, __FILE__);
		}
	return periodstats;
	}

void FMTSpatialSchedule::perturbgraph(const FMTcoordinate& coordinate,const int&period,const Models::FMTmodel& model,
	std::default_random_engine& generator, const actionbindings& bindings)
	{
	try {
		const Graph::FMTlinegraph& graph = mapping.at(coordinate).getLineGraph();
		const size_t graphsize = graph.size();
		//setgraphfromcache(graph, model,period,true);
		std::map<Core::FMTdevelopment, std::vector<bool>>tabuoperability;
		const std::vector<std::vector<bool>>actions = graph.getactions(model, period, tabuoperability);
		boost::unordered_map<Core::FMTdevelopment, std::vector<int>>operability;
		bool dontbuildgrowth = false;
		if (!actions.empty())
		{
		for (std::map<Core::FMTdevelopment, std::vector<bool>>::const_iterator it = tabuoperability.begin(); it != tabuoperability.end(); it++)
			{
				int actionid = 0;
				operability[it->first] = std::vector<int>();
				for (const Core::FMTaction& action : model.actions)
				{
					if ((!it->second.at(actionid)) && it->first.operable(action, model.yields))
					{
						operability[it->first].push_back(actionid);
					}
					++actionid;
				}
				///poor thing it can only do one action so grow!
			}
			const size_t buffer = 1;
			FMTeventcontainer newevents;
			const FMTeventcontainer eventstoerase = m_events.geteventstoerase(period, actions, coordinate, buffer, newevents);
			//remove contribution of old events to PI(eventstoerase)
			//const double oldeventsPI = getprimalinfeasibility(model.constraints, model, nullptr,true,true,false);
			//*_logger << "old event contribution " << oldeventsPI << "\n";
			newevents = m_events.addupdate(newevents, eventstoerase);
			//add contribution of new events to PI (newevents)

			//events.erasecoordinate(coordinate, period, actions);
		}else {
			dontbuildgrowth = true;
			}
		Graph::FMTlinegraph newgraph = graph.partialcopy(period);
		newgraph.clearnodecache();
		int localperiod = period;
		//their should be only one call to add action for the whole graph!!!!!
		while (graphsize != newgraph.size())
		{
			size_t maxsize = 0;
			const std::vector<int>actionids = newgraph.randombuildperiod(model, generator, operability, dontbuildgrowth);
			if (!actionids.empty())
				{
				m_events.addactions(coordinate, localperiod, actionids, bindings.at(period - 1));
				}
			++localperiod;
		}
		//setgraphfromcache(newgraph, model,period, false);
		mapping[coordinate].setLineGraph(newgraph, m_NonSpatialSolution);
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTSpatialSchedule::perturbgraph", __LINE__, __FILE__);
		}
	}



int FMTSpatialSchedule::getperiodwithmaximalevents(const std::vector<bool>& actions) const
{
	int period = 0;
	try {
		size_t maxnumferofevents = 0;
		for (int lperiod = 1; lperiod < actperiod(); ++lperiod)
		{
			const size_t periodsize = m_events.getevents(lperiod, actions).size();
			if (periodsize > maxnumferofevents)
			{
				maxnumferofevents = periodsize;
				period = lperiod;
			}
		}
	}catch (...)
	{
	_exhandler->printexceptions("", "FMTSpatialSchedule::getperiodwithmaximalevents", __LINE__, __FILE__);
	}
	return period;
}


std::vector<std::vector<Spatial::FMTcoordinate>>FMTSpatialSchedule::getareaconflictcoordinates(const actionbindings& bindingactions,const int& period, bool conflictonly) const
{
	std::vector<std::vector<Spatial::FMTcoordinate>>coordinates;
	try {
		int actionid = 0;
		for (const Spatial::FMTbindingspatialaction& actionbind : bindingactions.at(period-1))
		{
			if (actionbind.isspatialyareabinding()||!conflictonly)
			{
				for (const FMTeventcontainer::const_iterator eventit : m_events.getevents(period, actionid))
				{
					size_t eventsize = eventit->size();
					std::vector<Spatial::FMTcoordinate>eventcoordinates;
					if (eventsize < actionbind.getminimalsize()||!conflictonly)
					{
						for (const Spatial::FMTcoordinate& coordinate : eventit->elements)
						{
							eventcoordinates.push_back(coordinate);
						}
					}else if (eventsize > actionbind.getmaximalsize())
						{
						std::vector<std::set<FMTcoordinate>::const_iterator>borders = eventit->getborders();
						while (eventsize> actionbind.getmaximalsize()&&
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
			return getareaconflictcoordinates(bindingactions, period, false);
		}

	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTSpatialSchedule::getareaconflictcoordinates", __LINE__, __FILE__);
	}
	return coordinates;
}

std::vector<std::vector<Spatial::FMTcoordinate>>FMTSpatialSchedule::getadjacencyconflictcoordinates(const actionbindings& bindingactions,const int& period, bool conflictonly) const
{
	std::vector<std::vector<Spatial::FMTcoordinate>>coordinates;
	try {
			boost::unordered_set<FMTeventrelation> relations;
			for (const Spatial::FMTbindingspatialaction& actionbind : bindingactions.at(period - 1))
			{
			if (actionbind.isspatialyadjacencybinding())
				{
				std::vector<bool>actionids(bindingactions.at(period - 1).size(),false);
				for (const int& action : actionbind.getneighbors())
					{
					actionids[action] = true;
					}
				if (!conflictonly)
				{
					for (FMTeventcontainer::const_iterator cit : m_events.getevents(period, actionids))
					{
						const std::vector<Spatial::FMTcoordinate>eventscoords(cit->elements.begin(), cit->elements.end());
						coordinates.push_back(eventscoords);
					}
					return coordinates;
				}
				std::vector<FMTeventcontainer::const_iterator>conflicts;
				evaluatespatialadjacency(period, actionbind.getminimalgreenup(),
					actionbind.getminimaladjacency(), actionbind.getmaximaladjacency(), 
					actionbind.testminimaladjacency(), actionbind.testmaximaladjacency(), 
					conflicts, relations, actionids);
				for (FMTeventcontainer::const_iterator cit : conflicts)
					{
					const std::vector<Spatial::FMTcoordinate>eventscoords(cit->elements.begin(), cit->elements.end());
					coordinates.push_back(eventscoords);
					}
				}
			if (conflictonly && coordinates.empty())
			{
				return getadjacencyconflictcoordinates(bindingactions, period, false);
			}
		}
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTSpatialSchedule::getadjacencyconflictcoordinates", __LINE__, __FILE__);
		}
	return coordinates;
}



std::vector<Spatial::FMTcoordinate>FMTSpatialSchedule::getmovablecoordinates(const Models::FMTmodel& model, const int& period,
																			const std::vector<Spatial::FMTcoordinate>* statics,
																			boost::unordered_map<Core::FMTdevelopment, bool>*operability) const
{
	std::vector<Spatial::FMTcoordinate>coordinates;
	try {
		std::vector<const Core::FMTaction*>actions;
		for (const Core::FMTaction& action : model.actions)
		{
			actions.push_back(&action);
		}
		coordinates.reserve(this->mapping.size());
		if (statics!=nullptr)
		{
		for (const Spatial::FMTcoordinate& coordinate : *statics)
			{
			std::map<Spatial::FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphit = mapping.find(coordinate);
			if (graphit->second.getLineGraph().ismovable(actions, model.yields, period,operability))
				{
				coordinates.push_back(coordinate);
				}
			}
		}else {
			for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphit = mapping.begin(); graphit != mapping.end(); ++graphit)
			{
				if (graphit->second.getLineGraph().ismovable(actions, model.yields, period,operability))
				{
					coordinates.push_back(graphit->first);
				}
			}

			}


		
	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTSpatialSchedule::getgraphs", __LINE__, __FILE__);
	}
	return coordinates;
}

bool FMTSpatialSchedule::ispartial() const
{
	return (scheduletype == FMTSpatialScheduletype::FMTpartial);
}

bool FMTSpatialSchedule::emptyevents() const
{
	return m_events.empty();
}

void FMTSpatialSchedule::copyfrompartial(FMTSpatialSchedule& rhs)
{
	try {
		if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot use a non complete schedule ",
				"FMTSpatialSchedule::copyfrompartial", __LINE__, __FILE__);
		}
		if (rhs.scheduletype != FMTSpatialScheduletype::FMTpartial)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot copy from complete solution ",
				"FMTSpatialSchedule::copyfrompartial", __LINE__, __FILE__);
		}
		//m_Graphs.clear();
		for (std::map<FMTcoordinate, FMTVirtualLineGraph>::iterator graphit = rhs.mapping.begin(); graphit != rhs.mapping.end(); ++graphit)
		{
			mapping[graphit->first].setLineGraph(graphit->second.getLineGraph(), m_NonSpatialSolution);
		}
		m_events.swap(rhs.m_events);
		constraintsfactor.swap(rhs.constraintsfactor);
	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTSpatialSchedule::copyfrompartial", __LINE__, __FILE__);
	}

}

size_t FMTSpatialSchedule::_GetNonSpatialCellsCount() const
	{
	size_t totalCount = 0;
	for (size_t CELLS : m_NonSpatialSolution)
		{
		totalCount += CELLS;
		}
	return totalCount;
	}


void FMTSpatialSchedule::dorefactortorization(const FMTSpatialGraphs& p_Graphs)
{
	try {
		if (!constraintsfactor.empty())
		{
			size_t cntid = 0;
			for (const double& value : getconstraintsvalues(p_Graphs))
			{
				const double valuewfactor = constraintsfactor.at(cntid)*value;
				if (valuewfactor > 1000 || valuewfactor < -1000)
				{
					constraintsfactor[cntid] = std::abs(1000 / value);
				}
				++cntid;
			}
		}
	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTSpatialSchedule::dorefactortorization", __LINE__, __FILE__);
	}

}



bool FMTSpatialSchedule::needsrefactortorization(const FMTSpatialGraphs& p_Graphs) const
{
	try {
		if (!constraintsfactor.empty())
			{
			size_t cntid = 0;
			for (const double& value : getconstraintsvalues(p_Graphs))
				{
				const double valuewfactor = constraintsfactor.at(cntid)*value;
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
		_exhandler->printexceptions("", "FMTSpatialSchedule::needsrefactortorization", __LINE__, __FILE__);
	}
return false;
}

void FMTSpatialSchedule::setconstraintsfactor(const Models::FMTmodel& model,const std::vector<double>&factors)
	{
	try {
		if (model.constraints.size()!=factors.size())
		{
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"Cannot set factors with size different from constraints",
				"FMTSpatialSchedule::setconstraintsfactor", __LINE__, __FILE__);
		}
		constraintsfactor = factors;
		double minimalfactor = std::numeric_limits<double>::max();
		double maximalfactor = -std::numeric_limits<double>::max();
		size_t cntid = 0;
		for (const double& value : factors)
			{
			if (value<0)
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Cannot set negative factor "+std::to_string(value)+" for constraint "+std::string(model.constraints.at(cntid)),
					"FMTSpatialSchedule::setconstraintsfactor", __LINE__, __FILE__);
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
		_logger->logwithlevel("Constraints normalization Min(" + std::to_string(minimalfactor) + ") Max(" + std::to_string(maximalfactor) + ")\n", 2);
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTSpatialSchedule::setconstraintsfactor", __LINE__, __FILE__);
		}

	}


const std::vector<double>& FMTSpatialSchedule::getConstraintsFactor() const
	{
	return constraintsfactor;
	}




std::vector<Spatial::FMTcoordinate>FMTSpatialSchedule::getstaticsmovablecoordinates(const Models::FMTmodel& model) const
{
	std::vector<Spatial::FMTcoordinate>coordinates;
	boost::unordered_map<Core::FMTdevelopment, bool>operability;
	try {
		std::vector<const Core::FMTaction*>actions;
		for (const Core::FMTaction& action : model.actions)
		{
			actions.push_back(&action);
		}
		coordinates.reserve(this->mapping.size());
		const int lastperiod = this->mapping.begin()->second.getLineGraph().getperiod() - 1;
		for (std::map<FMTcoordinate, FMTVirtualLineGraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
			for (int period = 1; period <= lastperiod;++period)
				{
				if (graphit->second.getLineGraph().ismovable(actions, model.yields, period, &operability))
					{
					coordinates.push_back(graphit->first);
					break;
					}
				}
			}
		std::sort(coordinates.begin(), coordinates.end());
	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTSpatialSchedule::getstaticsmovablegraphs", __LINE__, __FILE__);
	}
	return coordinates;
}



bool FMTSpatialSchedule::isbetterbygroup(const FMTSpatialSchedule& rhs, const FMTSpatialGraphs& p_Graphs) const
	{
	try {
		if (scheduletype != FMTSpatialScheduletype::FMTcomplete)
		{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cannot use a non complete schedule ",
				"FMTSpatialSchedule::isbetterbygroup", __LINE__, __FILE__);
		}
		size_t gotbetter = 0;
		size_t groupid = 0;
		const std::vector<int>groupvalues = isbetterthan(rhs, p_Graphs);
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
		_exhandler->printexceptions("", "FMTSpatialSchedule::isbetterbygroup", __LINE__, __FILE__);
		}
	return false;
	}


}
