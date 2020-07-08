/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlinegraph.h"

#include "FMTmodel.h"

namespace Graph
{
    FMTlinegraph::FMTlinegraph():
    FMTgraph(FMTgraphbuild::nobuild)
    {

    }

    FMTlinegraph::FMTlinegraph(const FMTgraphbuild lbuildtype):
    FMTgraph(lbuildtype)
    {

    }

    FMTlinegraph::FMTlinegraph(const FMTlinegraph& rhs):
    FMTgraph(rhs)
    {

    }

    FMTlinegraph::FMTlinegraph(const FMTgraph& rhs):
    FMTgraph(rhs)
    {

    }

    FMTlinegraph& FMTlinegraph::operator = (const FMTlinegraph& rhs)
    {
        if(this!=&rhs)
            {
            FMTgraph::operator = (rhs);
            }
        return *this;
    }

    FMTlinegraph& FMTlinegraph::operator = (const FMTgraph& rhs)
    {
        if(this!=&rhs)
            {
            FMTgraph::operator = (rhs);
            }
        return *this;
    }

    int FMTlinegraph::getlastperiod() const
    {
        std::queue<Graph::FMTvertex_descriptor> actives = getactiveverticies();
        Graph::FMTvertex_descriptor front_vertex = actives.front();
        const Graph::FMTvertexproperties& front_properties = data[front_vertex];
        const int period = front_properties.get().period;
        return period;
    }

    //Was used before
    /*std::vector<std::pair<size_t,int>> FMTgraph::adjacentevents(const std::vector<Spatial::FMTevent<FMTgraph>>& events, const Spatial::FMTcoordinate& localisation,const int action_id) const
    {
		std::vector<std::pair<size_t,int>> selectedevents;
        size_t indice = 0;
        for (const Spatial::FMTevent<FMTgraph>& event:events)
        {
            if(event.whithinelements(1,localisation))
            {
                selectedevents.push_back(std::pair<size_t,int>(indice,action_id));
            }
            ++indice;
        }
        return selectedevents;
    }*/

    int FMTlinegraph::randomoperate(const std::vector<int>& operables, const Models::FMTmodel& model, std::queue<FMTvertex_descriptor>& actives,
                                            FMTgraphstats& statsdiff, const FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
                                            const Core::FMTdevelopment& active_development)
    {
        const int size_op =  static_cast<int>(operables.size());
		std::uniform_int_distribution<int> distribution(0,size_op);
        int distribution_select = distribution(generator);
        if(!operables.empty() && distribution_select != size_op)//Take care ok _DEATH hereeeeee ... I think it's implicitly done &!&!Validate
            {
                const int action_id = operables.at(distribution_select);
                const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(model.getactions().at(action_id), model.gettransitions().at(action_id), model.getyields(), model.getthemes());
                const size_t active_size = actives.size();
                addaction(action_id, statsdiff, actives, front_vertex, paths);
                assert(actives.size()==active_size);//Action returning on the same action == mega bug
                return action_id;
            }
         else
            {
                const Core::FMTfuturdevelopment grown_up = active_development.grow();
                FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
                const FMTedgeproperties newedge(-1, 0, 100);
                boost::add_edge(front_vertex, next_period, newedge, data);
                ++stats.edges;
                return -1;
            }
    }

    /*std::pair<size_t,int> FMTgraph::randomoperate(const std::vector<std::pair<size_t,int>>& operables, const Models::FMTmodel& model, std::queue<FMTvertex_descriptor>& actives,
                                            FMTgraphstats& statsdiff, const FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
                                            const Core::FMTdevelopment& active_development)
    {
        const int size_op =  static_cast<int>(operables.size());
		std::uniform_int_distribution<int> distribution(0,size_op);
        int distribution_select = distribution(generator);
        if(!operables.empty() && distribution_select != size_op)//If size_op grow //Take care ok _DEATH hereeeeee
            {
                statsdiff.cols = 0;
				const std::pair<size_t,int> selection = operables.at(distribution_select);
                const int action_id = selection.second;
                const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(model.actions.at(action_id), model.transitions.at(action_id), model.yields, model.themes);
                const size_t active_size = actives.size();
                addaction(action_id, statsdiff, actives, front_vertex, paths);
                assert(actives.size()==active_size);//Action returning on the same action == mega bug
                return selection;
            }
         else
            {
                const Core::FMTfuturdevelopment grown_up = active_development.grow();
                FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
                const FMTedgeproperties newedge(-1, 0, 100);
                boost::add_edge(front_vertex, next_period, newedge, data);
                ++stats.edges;
                return std::pair<size_t,int>(0,-1);
            }
    }*/

    FMTgraphstats FMTlinegraph::randombuild(const Models::FMTmodel& model,std::queue<FMTvertex_descriptor> actives, std::default_random_engine& generator,
                                            Spatial::FMTsaeventcontainer& events, const Spatial::FMTcoordinate& localisation)
	{
		FMTgraphstats statsdiff(stats);
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		while (!actives.empty())
        {
			const FMTvertex_descriptor front_vertex = actives.front();
			actives.pop();
			const FMTvertexproperties& front_properties = data[front_vertex];
			const Core::FMTdevelopment& active_development = front_properties.get();
            std::vector<Core::FMTaction> actions = model.getactions();
			int action_id = 0;
			std::vector<int> operables;
			for (const Core::FMTaction& action : actions)
			{
				if (active_development.operable(action, model.getyields()))
				{
                    {
                        operables.push_back(action_id);
                    }
				}
				++action_id;
            }
            const int selectedaction = randomoperate(operables, model, actives, statsdiff, front_vertex, generator, active_development);
            if (selectedaction>-1)
            {
                events.addaction(localisation,active_development.period,selectedaction);
            }
        }
        return (statsdiff - stats);
	}

    /*FMTgraphstats FMTgraph::randombuild(const Models::FMTmodel& model,std::queue<FMTvertex_descriptor> actives, std::default_random_engine& generator,
	std::vector<std::vector<Spatial::FMTevent<FMTgraph>>>& events_id, const Spatial::FMTcoordinate& localisation)
	{
	    //ok
		FMTgraphstats statsdiff(stats);
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		while (!actives.empty())
		{
		    //ok
			const FMTvertex_descriptor front_vertex = actives.front();
			actives.pop();
			const FMTvertexproperties& front_properties = data[front_vertex];
			const Core::FMTdevelopment& active_development = front_properties.get();
			std::vector<std::pair<size_t,int>> operables;//size_t == event_id in the vector of events // int == action_id
			std::vector<std::pair<size_t,int>> allpotentialevents;//vector of events with action_id in a pair to only pick one random
			std::vector<std::vector<size_t>> action_adj_events(events_id.size());//Event index in case two adjacents events for one action
			int id = 0;
			for (const Core::FMTaction& action : model.actions)
			{
			    if (active_development.operable(action, model.yields))
				{
                    operables.push_back(std::pair<size_t,int>(0,id));
                    if (!events_id.at(id).empty())
                    {
						const std::vector<std::pair<size_t,int>> potentialevents = adjacentevents(events_id.at(id),localisation,id);
                        allpotentialevents.insert(allpotentialevents.end(),potentialevents.begin(),potentialevents.end());
                        if (potentialevents.size()>1)
                        {
                            for (const std::pair<size_t,int>& event : potentialevents)
                            {
                                action_adj_events[id].push_back(event.first);
                            }
                        }
                    }
				}
				++id;
			}
			if (!allpotentialevents.empty() && !operables.empty())
                {
				std::pair<size_t,int> selectedevent = randomoperate(allpotentialevents, model, actives, statsdiff, front_vertex, generator,active_development);
                    if (selectedevent.second != -1)
                        {

                            //To merge events that are in distance of 1 if there is more than 1
                            if (!action_adj_events.at(selectedevent.second).empty())//Cause we only populate if there is more than 2
                            {
                                Spatial::FMTevent<FMTgraph> mergearoundevents(localisation);
                                mergearoundevents.insert(localisation,nullptr);//We dont do iginition or spread... So we need to add the FMTcoordinate to the elements
								std::vector<Spatial::FMTevent<FMTgraph>>& events= events_id.at(selectedevent.second);
								const std::vector<size_t>& events_indexes = action_adj_events.at(selectedevent.second);
                                for (const size_t& event_index : events_indexes)
                                {
                                    Spatial::FMTevent<FMTgraph>& eventf = events.at(event_index);
                                    for (std::map<Spatial::FMTcoordinate,const FMTgraph*>::const_iterator coordit = eventf.elements.begin(); coordit != eventf.elements.end(); ++coordit)
                                    {
                                        mergearoundevents.insert(coordit->first,nullptr);
                                    }
                                }
                                for (std::map<Spatial::FMTcoordinate,const FMTgraph*>::const_iterator coordit = mergearoundevents.elements.begin(); coordit != mergearoundevents.elements.end(); ++coordit)
                                {
                                    cleanevents(events,coordit->first);
                                }
                                events.push_back(mergearoundevents);
                            }
                            else
                            {
                                Spatial::FMTevent<FMTgraph>& luckyevent = events_id.at(selectedevent.second).at(selectedevent.first);
                                luckyevent.insert(localisation,nullptr);
                            }
                        }
                }
            else// New event
                {
				std::pair<size_t,int> selectedevent = randomoperate(operables, model, actives, statsdiff, front_vertex,generator,active_development);
                    if (selectedevent.second != -1)
                        {
                            Spatial::FMTevent<FMTgraph> newluckyevent(localisation);
                            newluckyevent.insert(localisation,nullptr);//We dont do iginition or spread... So we need to add the FMTcoordinate to the elements
							std::vector<Spatial::FMTevent<FMTgraph>>& events= events_id.at(selectedevent.second);
                            events.push_back(newluckyevent);
                        }
                }
            }
		return (statsdiff - stats);
    }*/
    //Used before
    /*void FMTgraph::cleanevents(std::vector<Spatial::FMTevent<FMTgraph>>& events_id, const Spatial::FMTcoordinate& localisation) const
    {
		std::vector<Spatial::FMTevent<FMTgraph>> clean_events_id;
        for (Spatial::FMTevent<FMTgraph>& event:events_id)
        {
            if(event.contain(localisation))
            {
                event.erase(localisation);
                if(!event.empty())
                {
					std::vector<Spatial::FMTevent<FMTgraph>> potentialysplittedevents;
                    if (event.splittedevent(1,potentialysplittedevents))
                    {
                        clean_events_id.insert(clean_events_id.end(),potentialysplittedevents.begin(),potentialysplittedevents.end());
                    }
                    else{clean_events_id.push_back(event);}
                }
            }
            else
            {
                clean_events_id.push_back(event);
            }
        }
        events_id = clean_events_id;
    }*/

    FMTgraphstats FMTlinegraph::clearfromperiod(const int& period)
    {
        for (int location = static_cast<int>(this->size()-1) ; location>=period ; --location)
        {
            for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = developments.at(location).begin();
                it != developments.at(location).end(); it++)
                {
                    FMTvertex_descriptor vertex_location = it->second;
                    if (!(location == period && periodstart(vertex_location)))
                        {
                            //Removing inedge
                            /*FMTinedge_iterator inedge_iterator, inedge_end;
                            for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_location, data); inedge_iterator != inedge_end; ++inedge_iterator)
                            {
                                const FMTedgeproperties& edgeprop = data[*inedge_iterator];
                                int action_id = edgeprop.getactionID();
                                if (action_id >= 0 && !events.empty())
                                {
                                    cleanevents(events.at(period-1).at(action_id),localisation);
                                }

                            }*/
                            --stats.edges;
							boost::clear_in_edges(vertex_location,data);
                        }
                }
                   for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = developments[location].begin();
                    it != developments[location].end(); it++)
                    {
                        FMTvertex_descriptor vertex_location = it->second;
                        if (!(location == period && periodstart(vertex_location)))
                            {
                                boost::remove_vertex(vertex_location, data);
                                --stats.vertices;
                            }
                    }
        }
    return stats;
    }

    /*FMTgraphstats FMTgraph::clearfromperiod(const int& period, std::vector<std::vector<std::vector<Spatial::FMTevent<FMTgraph>>>>& events,
                                        const Spatial::FMTcoordinate& localisation)
    {
        for (int location = static_cast<int>(this->size()-1) ; location>=period ; --location)
        {
            for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = developments.at(location).begin();
                it != developments.at(location).end(); it++)
                {
                    FMTvertex_descriptor vertex_location = it->second;
                    if (!(location == period && periodstart(vertex_location)))
                        {
                            //Removing inedge
                            FMTinedge_iterator inedge_iterator, inedge_end;
                            for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_location, data); inedge_iterator != inedge_end; ++inedge_iterator)
                            {
                                const FMTedgeproperties& edgeprop = data[*inedge_iterator];
                                int action_id = edgeprop.getactionID();
                                if (action_id >= 0 && !events.empty())
                                {
                                    cleanevents(events.at(period-1).at(action_id),localisation);
                                }

                            }
                            --stats.edges;
							boost::clear_in_edges(vertex_location,data);
                        }
                }
                   for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = developments[location].begin();
                    it != developments[location].end(); it++)
                    {
                        FMTvertex_descriptor vertex_location = it->second;
                        if (!(location == period && periodstart(vertex_location)))
                            {
                                boost::remove_vertex(vertex_location, data);
                                --stats.vertices;
                            }
                    }
        }
    return stats;
    }*/

    FMTlinegraph FMTlinegraph::partialcopy(const int& period) const
    {
        FMTlinegraph newgraph(*this);
        const FMTgraphstats delstats = newgraph.clearfromperiod(period);
        newgraph.generatedevelopments();
        return newgraph;
    }

    /*FMTgraph FMTgraph::partialcopy(const int& period, std::vector<std::vector<std::vector<Spatial::FMTevent<FMTgraph>>>>& events,
                               const Spatial::FMTcoordinate& localisation) const
    {
        FMTgraph newgraph(*this);
        const FMTgraphstats delstats = newgraph.clearfromperiod(period,events,localisation);
        newgraph.generatedevelopments();
        return newgraph;
    }*/

    FMTlinegraph FMTlinegraph::perturbgraph(const Models::FMTmodel& model, std::default_random_engine& generator,
                                            Spatial::FMTsaeventcontainer& events,
                                            const Spatial::FMTcoordinate& localisation, const int period) const
    {
        events.erasecoordinate(localisation,period);
        FMTlinegraph newgraph = partialcopy(period);
        newgraph.nodescache.clear();
        while(this->size() != newgraph.size())
        {
            std::queue<FMTvertex_descriptor> actives = newgraph.getactiveverticies();
            //FMTvertex_descriptor front_vertex = actives.front();
			//const FMTvertexproperties& front_properties = data[front_vertex];
            newgraph.randombuild(model,actives,generator,events,localisation);
        }

    return newgraph;
    }

    /*FMTgraph FMTgraph::perturbgraph(const Models::FMTmodel& model, std::default_random_engine& generator,
	std::vector<std::vector<std::vector<Spatial::FMTevent<FMTgraph>>>>& events,
                                const Spatial::FMTcoordinate& localisation, const int period) const
    {
        FMTgraph newgraph = partialcopy(period,events,localisation);
        newgraph.nodescache.clear();
        while(this->size() != newgraph.size())
        {
            std::queue<FMTvertex_descriptor> actives = newgraph.getactiveverticies();
            FMTvertex_descriptor front_vertex = actives.front();
			const FMTvertexproperties& front_properties = data[front_vertex];
			const int periodbuildevents = front_properties.get().period-1;
            newgraph.randombuild(model,actives,generator,events.at(periodbuildevents),localisation);
        }

    return newgraph;
    }*/
}
