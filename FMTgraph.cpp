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

#include "FMTgraph.h"

#include "FMTmodel.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTtransition.h"
#include "FMTtheme.h"
#include <assert.h>

namespace Graph
{

FMTgraph::FMTgraph():
    buildtype(FMTgraphbuild::nobuild),
    developments(),
	nodescache(),
    stats(),
    data()
    {

    }

FMTgraph::FMTgraph(const FMTgraphbuild lbuildtype):
    buildtype(lbuildtype),
    developments(),
	nodescache(),
    stats(),
    data()
    {

    }

FMTgraph::FMTgraph(const FMTgraph& rhs):
    buildtype(rhs.buildtype),
    developments(),
	nodescache(),
    stats(rhs.stats),
    data(rhs.data)
    {
        generatedevelopments();
    }

void FMTgraph::swap(FMTgraph& rhs)
	{
	buildtype = rhs.buildtype;
	nodescache.swap(rhs.nodescache);
	stats = rhs.stats;
	data.swap(rhs.data);
	generatedevelopments();
	}

FMTgraph& FMTgraph::operator = (const FMTgraph& rhs)
    {
        if(this!=&rhs)
            {
            buildtype=rhs.buildtype;
			nodescache = rhs.nodescache;
            stats=rhs.stats;
            data=rhs.data;
            generatedevelopments();
            }
        return *this;
    }

bool FMTgraph::operator == (const FMTgraph& rhs) const
       {
       if (buildtype == rhs.buildtype &&
             stats == rhs.stats)
             {
             for (size_t location = 0 ; location < developments.size(); ++location)
                    {
                    for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator it = developments.at(location).begin() ; it != developments.at(location).end(); it++)
                           {
                           if (rhs.developments.at(location).find(it->first) == rhs.developments.at(location).end())
                                  {
                                  return false;
                           }else {
                                  const FMTvertex_descriptor vertex_location = it->second;
                                  const FMTvertex_descriptor rhsvertex_location = rhs.developments.at(location).find(it->first)->second;
                                  FMTinedge_iterator inedge_iterator, inedge_end;
                                  vector<FMTedgeproperties>edges;
                                  vector<FMTedgeproperties>rhsedges;
                                  for (tie(inedge_iterator, inedge_end) = in_edges(vertex_location, data); inedge_iterator != inedge_end; ++inedge_iterator)
                                        {
                                        edges.push_back(data[*inedge_iterator]);
                                        }
                                  for (tie(inedge_iterator, inedge_end) = in_edges(rhsvertex_location, rhs.data); inedge_iterator != inedge_end; ++inedge_iterator)
                                        {
                                        rhsedges.push_back(rhs.data[*inedge_iterator]);
                                        }
                                  sort(edges.begin(), edges.end());
                                  sort(rhsedges.begin(), rhsedges.end());
                                  if (edges!= rhsedges)
                                        {
                                        return false;
                                        }
                                  }
                           }
                    }
             return true;
             }
       return false;
       }

bool FMTgraph::operator != (const FMTgraph& rhs) const
       {
       return (!(*this==rhs));
       }


FMTgraphbuild FMTgraph::getbuildtype() const
    {
        return buildtype;
    }

bool FMTgraph::containsdevelopment(const FMTdevelopment& developement) const
	{
		return ((developments.size() > (developement.period-stats.erasedperiods)) &&
			developments.at((developement.period-stats.erasedperiods)).find(boost::hash<FMTdevelopment>()(developement)) != developments.at((developement.period-stats.erasedperiods)).end());
	}

std::queue<FMTvertex_descriptor> FMTgraph::initialize(const vector<FMTactualdevelopment>& actdevelopments)
    {
        std::queue<FMTvertex_descriptor>actives;
		developments.clear();
		//P0 useless stuff with bounds on variables
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		developments.back().reserve(actdevelopments.size());
		//P1 part of the queue
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		developments.back().reserve(actdevelopments.size()); //should be exponential here...
        int constraint_id = -1;
        int edge_id = -1;
        double proportion = 100;
		stats.edges = 0;
		stats.vertices = 0;
		for (const FMTactualdevelopment& development : actdevelopments)
		{
            //P0
			const FMTvertexproperties properties(development, constraint_id);
			FMTvertex_descriptor newvertex = add_vertex(properties, data);
			developments[0][boost::hash<FMTdevelopment>()(development)] = newvertex;
			++stats.vertices;
            //P1
            FMTfuturdevelopment P1dev(development);
            P1dev.period = 1;
            FMTvertex_descriptor tovertex;
            tovertex = adddevelopment(P1dev);
            /*const FMTvertexproperties P1properties(P1dev, constraint_id);
            FMTvertex_descriptor P1vertex = add_vertex(P1properties, data);
            developments[1][boost::hash<FMTdevelopment>()(P1dev)] = P1vertex;
			++stats.vertices;*/
            actives.push(tovertex);
            //Now set the edge!!
			const FMTedgeproperties newedge(edge_id, stats.edges, proportion);
			add_edge(newvertex,tovertex, newedge, data);
			++stats.edges;
		}
		return actives;
	}

FMTgraphstats FMTgraph::build(const FMTmodel& model,std::queue<FMTvertex_descriptor> actives)
	{
		FMTgraphstats statsdiff(stats);
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		while (!actives.empty())
		{
			const FMTvertex_descriptor front_vertex = actives.front();
			actives.pop();
			FMTvertexproperties front_properties = data[front_vertex];
			const FMTdevelopment active_development = front_properties.get();

			int action_id = 0;
			for (const FMTaction& action : model.actions)
			{
				if (active_development.operable(action, model.yields))
				{
					const vector<FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
					addaction(action_id, statsdiff, actives, front_vertex, paths);
				}
				++action_id;
			}
			FMTfuturdevelopment grown_up = active_development.grow();
			FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
			const FMTedgeproperties newedge(-1, statsdiff.cols, 100);
			++statsdiff.cols;
			add_edge(front_vertex, next_period, newedge, data);
			++stats.edges;
		}

		return (statsdiff - stats);
    }

vector<pair<size_t,int>> FMTgraph::adjacentevents(const vector<FMTevent<FMTgraph>>& events, const FMTcoordinate& localisation,const int action_id) const
    {
        vector<pair<size_t,int>> selectedevents;
        size_t indice = 0;
        for (const FMTevent<FMTgraph>& event:events)
        {
            if(event.whithinelements(1,localisation))
            {
                selectedevents.push_back(pair<size_t,int>(indice,action_id));
            }
            ++indice;
        }
        return selectedevents;
    }

pair<size_t,int> FMTgraph::randomoperate(const vector<pair<size_t,int>>& operables, const FMTmodel& model, std::queue<FMTvertex_descriptor>& actives,
                                            FMTgraphstats& statsdiff, const FMTvertex_descriptor& front_vertex, default_random_engine& generator,
                                            const FMTdevelopment& active_development)
    {
        int size_op =  static_cast<int>(operables.size());
        uniform_int_distribution<int> distribution(0,size_op);
        int distribution_select = distribution(generator);
        if(!operables.empty() && distribution_select != size_op)//If size_op grow
            {
                statsdiff.cols = 0;
                pair<size_t,int> selection = operables.at(distribution_select);
                int action_id = selection.second;
                const vector<FMTdevelopmentpath> paths = active_development.operate(model.actions.at(action_id), model.transitions.at(action_id), model.yields, model.themes);
                size_t active_size = actives.size();
                addaction(action_id, statsdiff, actives, front_vertex, paths);
                assert(actives.size()==active_size);//Action returning on the same action == mega bug
                return selection;
            }
         else
            {
                FMTfuturdevelopment grown_up = active_development.grow();
                FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
                const FMTedgeproperties newedge(-1, 0, 100);
                add_edge(front_vertex, next_period, newedge, data);
                ++stats.edges;
                return pair<size_t,int>(0,-1);
            }
    }

FMTgraphstats FMTgraph::randombuild(const FMTmodel& model,std::queue<FMTvertex_descriptor> actives, default_random_engine& generator,
                                    vector<vector<FMTevent<FMTgraph>>>& events_id, const FMTcoordinate& localisation)
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
			const FMTdevelopment& active_development = front_properties.get();
			vector<pair<size_t,int>> operables;//size_t == event_id in the vector of events // int == action_id
			vector<pair<size_t,int>> allpotentialevents;//vector of events with action_id in a pair to only pick one random
			vector<vector<size_t>> action_adj_events(events_id.size());//Event index in case two adjacents events for one action
			int id = 0;
			for (const FMTaction& action : model.actions)
			{
			    //Remove, see FMTsasolution line 180
                /*if (events_id.size()<id+1)//Add vector of event in events_id for each action id if it does not exist
				{
				    vector<FMTevent<FMTgraph>> emptyvofevents;
                    events_id.push_back(emptyvofevents);
				}*/
			    if (active_development.operable(action, model.yields))
				{
                    operables.push_back(pair<size_t,int>(0,id));
                    if (!events_id.at(id).empty())
                    {
                        vector<pair<size_t,int>> potentialevents = adjacentevents(events_id.at(id),localisation,id);
                        allpotentialevents.insert(allpotentialevents.end(),potentialevents.begin(),potentialevents.end());
                        if (potentialevents.size()>1)
                        {
                            for (const pair<size_t,int>& event : potentialevents)
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
                    pair<size_t,int> selectedevent = randomoperate(allpotentialevents, model, actives, statsdiff, front_vertex, generator,active_development);
                    if (selectedevent.second != -1)
                        {

                            //To merge events that are in distance of 1 if there is more than 1
                            if (!action_adj_events.at(selectedevent.second).empty())//Cause we only populate if there is more than 2
                            {
                                FMTevent<FMTgraph> mergearoundevents(localisation);
                                mergearoundevents.insert(localisation,nullptr);//We dont do iginition or spread... So we need to add the FMTcoordinate to the elements
                                vector<FMTevent<FMTgraph>>& events= events_id.at(selectedevent.second);
                                vector<size_t>& events_indexes = action_adj_events.at(selectedevent.second);
                                for (const size_t& event_index : events_indexes)
                                {
                                    FMTevent<FMTgraph>& event = events.at(event_index);
                                    for (map<FMTcoordinate,const FMTgraph*>::const_iterator coordit = event.elements.begin(); coordit != event.elements.end(); ++coordit)
                                    {
                                        mergearoundevents.insert(coordit->first,nullptr);
                                    }
                                }
                                for (map<FMTcoordinate,const FMTgraph*>::const_iterator coordit = mergearoundevents.elements.begin(); coordit != mergearoundevents.elements.end(); ++coordit)
                                {
                                    cleanevents(events,coordit->first);
                                }
                                events.push_back(mergearoundevents);
                            }
                            else
                            {
                                FMTevent<FMTgraph>& luckyevent = events_id.at(selectedevent.second).at(selectedevent.first);
                                luckyevent.insert(localisation,nullptr);
                            }
                        }
                }
            else// New event
                {
                    pair<size_t,int> selectedevent = randomoperate(operables, model, actives, statsdiff, front_vertex,generator,active_development);
                    if (selectedevent.second != -1)
                        {
                            FMTevent<FMTgraph> newluckyevent(localisation);
                            newluckyevent.insert(localisation,nullptr);//We dont do iginition or spread... So we need to add the FMTcoordinate to the elements
                            vector<FMTevent<FMTgraph>>& events= events_id.at(selectedevent.second);
                            events.push_back(newluckyevent);
                        }
                }
            }
		return (statsdiff - stats);
    }

FMTgraphstats FMTgraph::naturalgrowth(std::queue<FMTvertex_descriptor> actives)
	{
		FMTgraphstats statsdiff(stats);
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		while (!actives.empty())
		{
			const FMTvertex_descriptor front_vertex = actives.front();
			actives.pop();
			FMTvertexproperties front_properties = data[front_vertex];
			const FMTdevelopment active_development = front_properties.get();
            FMTfuturdevelopment grown_up = active_development.grow();
            FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
            const FMTedgeproperties newedge(-1, 0, 100);
            add_edge(front_vertex, next_period, newedge, data);
            ++stats.edges;
		}

		return (statsdiff - stats);
    }

vector<FMTactualdevelopment> FMTgraph::getperiodstopdev(const int location,const double* actual_solution) const
	{
		vector<FMTactualdevelopment>all_period_stop_devs;
		for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator devit = developments.at(location).begin();
			devit != developments.at(location).end(); devit++)
		{
            if (periodstop(devit->second))
            {
                double area = inarea(devit->second, actual_solution);
                const FMTdevelopment& development = data[devit->second].get();
                all_period_stop_devs.push_back(FMTactualdevelopment(development, area));
            }
		}
		all_period_stop_devs.size();
		return all_period_stop_devs;
	}

FMTvertex_descriptor FMTgraph::getdevelopment(const FMTdevelopment& developement) const
	{
		return developments.at(developement.period-stats.erasedperiods).at(boost::hash<FMTdevelopment>()(developement));
	}

const FMTdevelopment& FMTgraph::getdevelopment(const FMTvertex_descriptor& descriptor) const
	{
	return data[descriptor].get();
	}

FMTvertex_descriptor FMTgraph::adddevelopment(const FMTfuturdevelopment& futurdevelopement)
	{
		if (!this->containsdevelopment(futurdevelopement))
		{
			int constraint_id = -1;//stats.rows;//getnumedges();
			const FMTvertexproperties properties(futurdevelopement, constraint_id);
			FMTvertex_descriptor newvertex = add_vertex(properties, data);
			developments[futurdevelopement.period - stats.erasedperiods][boost::hash<FMTdevelopment>()(futurdevelopement)] = newvertex;
			++stats.vertices;
			return newvertex;
		}
		return getdevelopment(futurdevelopement);
	}

void FMTgraph::addaction(const int& actionID,
                            FMTgraphstats& statsdiff,
                            std::queue<FMTvertex_descriptor>& actives,
                            const FMTvertex_descriptor& out_vertex,
                            const vector<FMTdevelopmentpath>& paths)
	{
		int variable_id = statsdiff.cols;//solverinterface->getNumCols();
		++statsdiff.cols;
		vector<FMTvertex_descriptor>active_vertex;
		for (const FMTdevelopmentpath& devpath : paths)
		{
			const FMTedgeproperties newedge(actionID, variable_id, devpath.proportion);
	
			FMTvertex_descriptor tovertex;
			if (!this->containsdevelopment(*devpath.development))
			{
				tovertex = this->adddevelopment(*devpath.development);
				actives.push(tovertex);

			}
			else {
				tovertex = this->adddevelopment(*devpath.development);
			}
			add_edge(out_vertex, tovertex, newedge, data);
            ++stats.edges;
		}
	}

double FMTgraph::outarea(const FMTvertex_descriptor& out_vertex, const int& actionID, const double*&  solution) const
	{
		FMToutedge_iterator outedge_iterator, outedge_end;
		double value = 0;
		for (tie(outedge_iterator, outedge_end) = out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
		{
			const FMTedgeproperties& edgeprop = data[*outedge_iterator];
			if (edgeprop.getactionID() == actionID)
			{
				value += *(solution + edgeprop.getvariable()) * (edgeprop.getproportion() / 100);
			}
		}
		return value;
	}

double FMTgraph::inarea(const FMTvertex_descriptor& out_vertex, const double*&  solution, bool growth) const
	{
		FMTinedge_iterator inedge_iterator, inedge_end;
		//FMTvertexproperties properties = data[out_vertex];
		double area = 0;// = abs(getRHS(out_vertex, bounds));
		//if (area == 0)
		//{
			//const double* solution = solverinterface->getColSolution();
			for (tie(inedge_iterator, inedge_end) = in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTedgeproperties& edgeprop = data[*inedge_iterator];
				/*if (edgeprop.getactionID() >= 0 )
					{
					FMTvertexproperties source_vecterx = data[source(*inedge_iterator, data)];
					Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << source_vecterx.get().period << " " << properties.get().period << "\n";
					}*/
				if (edgeprop.getactionID() < 0 || !growth) //only from gorwth!!!!!! not from the operated!!!!
				{
					area += *(solution + edgeprop.getvariable()) * (edgeprop.getproportion() / 100);
					//break;
				}
			}
		//}
		return area;
	}

bool FMTgraph::periodstart(const FMTvertex_descriptor& out_vertex) const
	{
	const FMTvertexproperties& properties = data[out_vertex];
	FMTinedge_iterator inedge_iterator, inedge_end;
	for (tie(inedge_iterator, inedge_end) = in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
		{
		const FMTedgeproperties& edgeprop = data[*inedge_iterator];
		if (edgeprop.getactionID()==-1)
			{
			return true;
			}
			/*const FMTvertexproperties& source_vecterx = data[source(*inedge_iterator, data)];
			if (source_vecterx.get().period < properties.get().period)
				{
					return true;
				}*/
		}
	return false;
	}

bool FMTgraph::periodstop(const FMTvertex_descriptor& out_vertex) const
	{
		FMToutedge_iterator outedge_iterator, outedge_end;
		const FMTvertexproperties& source_properties = data[out_vertex];
		if ((source_properties.get().period-stats.erasedperiods)==0)
			{
			return true;
			}
		for (tie(outedge_iterator, outedge_end) = out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
		{
			const FMTedgeproperties& edgeprop = data[*outedge_iterator];
			if (edgeprop.getactionID() == -1)
				{
				return true;
				}
			/*const FMTvertexproperties& target_vecterx = data[target(*outedge_iterator, data)];
			if (source_properties.get().period < target_vecterx.get().period)
			{
				return true;
			}*/
		}
		return false;
	}

vector<FMTdevelopmentpath> FMTgraph::getpaths(const FMTvertex_descriptor& out_vertex, const int& actionID) const
	{
		vector<FMTdevelopmentpath>paths;
		for (FMToutedge_pair edge_pair = out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
		{
			const FMTedgeproperties& edgeprop = data[*edge_pair.first];
			if (edgeprop.getactionID() == actionID)
			{
				const FMTvertexproperties& vertex_target = data[target(*edge_pair.first, data)];
				paths.push_back(FMTdevelopmentpath(vertex_target.get(), edgeprop.getproportion()));
			}
		}
		return paths;
	}

bool FMTgraph::validouputnode(const FMTmodel& model,const FMToutputnode& node, vector<int>& action_IDS, int period) const
    {
	if (developments.size() > period-stats.erasedperiods)//
		{
		action_IDS = node.source.targets(model.actions, model.action_aggregates);
		if (!(period == stats.erasedperiods && !action_IDS.empty()) && !node.source.isnull(model.yields) && !node.factor.isnull(model.yields) && (!action_IDS.empty() && !node.source.getaction().empty() || node.source.getaction().empty()))
			{
			return true;
			}
		}
	return false;
	}

bool FMTgraph::validgraphnode(const FMTmodel& model, bool& inedges, const FMTvertex_descriptor& vertex_descriptor,
                                const FMToutputnode& node, const vector<int>& action_IDS, const vector<const FMTaction*>& selected) const
	{
	inedges = false;
	const FMTdevelopment& development = data[vertex_descriptor].get();
	if (node.source.use(development, model.yields))
		{
		if (node.source.useinedges()) //in edges
			{
				if ((development.period == stats.erasedperiods || periodstart(vertex_descriptor)) &&
					(action_IDS.empty() || (!action_IDS.empty() &&
					(((buildtype == FMTgraphbuild::schedulebuild) && development.anyoperable(selected, model.yields)) || anyoperables(vertex_descriptor, development.anyworthtestingoperability(selected,action_IDS))))))
				{
				inedges = true;
				return true;
				}
			}else if ((anyoperables(vertex_descriptor, development.anyworthtestingoperability(selected, action_IDS)))) //out edges
			{
			return true;
			}
		}
	return false;
	}

map<int, double> FMTgraph::locatenode(const FMTmodel& model, FMToutputnode output_node,int period) const
	{
		vector<int>action_IDS;
		vector<const FMTaction*> selected;
		vector<FMTvertex_descriptor>verticies = getnode(model, output_node, period, action_IDS, selected);
		return getvariables(model, output_node, verticies, action_IDS, selected);

	}



vector<FMTvertex_descriptor> FMTgraph::getnode(const FMTmodel& model, FMToutputnode output_node,
	int period, vector<int>& action_IDS, vector<const FMTaction*>& selected) const
	{
			vector<FMTvertex_descriptor>locations;
			//change the period if the node is single well a other potential cluster fuck
			int node_period = period;
			if (output_node.source.useinedges())//evaluate at the begining of the other period if inventory! what a major fuck
			{
				++node_period;
			}
			if (output_node.singleperiod())
			{
				if (output_node.ispastperiod())
				{
					if ((output_node.source.getperiodlowerbound() + period) >= 0)
					{
						FMTperbounds perbound(FMTwssect::Optimize, node_period, node_period);
						output_node.source.setbounds(perbound);
						output_node.factor.setbounds(perbound);
					}
					else {
						return locations;//dont need that node...
					}
				}
				else {
					node_period = output_node.source.getperiodlowerbound();
					if (output_node.source.useinedges())
					{
						++node_period;
					}
				}
			}
			vector<int>targetedperiods;
			if (output_node.multiperiod())
			{
				int minperiod = max(output_node.source.getperiodlowerbound(), 1);
				int maxperiod = min(output_node.source.getperiodupperbound(), static_cast<int>(developments.size() - 2));
				for (int periodid = minperiod; periodid <= maxperiod; ++periodid)
				{
					int local_period = periodid;
					if (output_node.source.useinedges())
					{
						++local_period;
					}
					targetedperiods.push_back(local_period);
				}
			}
			else {
				targetedperiods.push_back(node_period);
			}

			size_t lookedat_size = 0;
			if (!output_node.source.isvariablelevel())
			{
				//vector<int> action_IDS;
				if (validouputnode(model, output_node, action_IDS, node_period))
				{
					selected = selectedactions(model, action_IDS);
					if (nodescache.find(output_node.hash(period)) != nodescache.end())
					{
						return nodescache.at(output_node.hash(period));
					}else{
						bool inedges = false;
						vector<FMTdevelopmentpath>paths;
						FMTaction optimization_action;
						for (const int localnodeperiod : targetedperiods)
						{
							for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator it = developments.at(localnodeperiod).begin();
								it != developments.at(localnodeperiod).end(); it++)
							{
								if (validgraphnode(model, inedges, it->second, output_node, action_IDS, selected))
									{
									locations.push_back(it->second);
									}
							}
							lookedat_size+=developments.at(localnodeperiod).size();
						}
					}
				}
			}
			//just to make sure the cash doesnt get too big
			double locationssize = static_cast<double>(locations.size());
			double lookedat = static_cast<double>(lookedat_size)*0.95;
			if (locationssize < lookedat)
				{
				nodescache[output_node.hash(period)] = locations;
				}
		return locations;
	}

map<int, double> FMTgraph::getvariables(const FMTmodel& model, const FMToutputnode& output_node,
	const vector<FMTvertex_descriptor>& verticies, const vector<int>& action_IDS, const vector<const FMTaction*>& selected) const
	{
	map<int, double>variables;
	if (!verticies.empty())
		{
		vector<FMTdevelopmentpath>paths;
		FMTaction optimization_action;
		for (const FMTvertex_descriptor& vertex : verticies)
			{
			const FMTdevelopment& development = data[vertex].get();
			if (output_node.source.useinedges())
			{
				double coef = 1;
				coef = output_node.source.getcoef(development, model.yields, optimization_action, paths) * output_node.factor.getcoef(development, model.yields, optimization_action, paths) * output_node.constant;
				if (development.period == stats.erasedperiods)
				{

					map<int, int>vars = getoutvariables(vertex);
					updatevarsmap(variables, vars.at(-1), coef);
				}
				else {
					FMTinedge_iterator inedge_iterator, inedge_end;
					for (tie(inedge_iterator, inedge_end) = in_edges(vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
					{
						const FMTedgeproperties& edgeprop = data[*inedge_iterator];
						updatevarsmap(variables, edgeprop.getvariable(), (edgeprop.getproportion() / 100)*coef);
					}
				}
			}
			else {
				int actionID = 0;
				map<int, int>outvars = getoutvariables(vertex);
				for (const FMTaction* act : selected)
				{
					if (outvars.find(action_IDS.at(actionID)) != outvars.end())
					{
						double action_value = 0;
						paths = getpaths(vertex, action_IDS.at(actionID));
						double action_coef = output_node.source.getcoef(development, model.yields, *act, paths) * output_node.factor.getcoef(development, model.yields, *act, paths) * output_node.constant;
						updatevarsmap(variables, outvars.at(action_IDS.at(actionID)), action_coef);
					}
					++actionID;
				}
			}
			}
		}
	return variables;
	}



bool FMTgraph::anyoperables(const FMTvertex_descriptor& descriptor, const vector<int>& action_ids) const
	{
	if (!action_ids.empty())
		{
		map<int, int> variables = getoutvariables(descriptor);
		for (const int& id : action_ids)
			{
			if (variables.find(id) != variables.end())
				{
				return true;
				}
			}
		}
	return false;
	}

map<int, int> FMTgraph::getoutvariables(const FMTvertex_descriptor& out_vertex) const
	{
		map<int, int> mapping;
		FMToutedge_pair edge_pair;
		for (edge_pair = out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
		{
			const FMTedgeproperties& edgeprop = data[*edge_pair.first];
			int actionid = edgeprop.getactionID();
			mapping[actionid] = edgeprop.getvariable();
		}
		return mapping;
	}

vector<std::pair<const int*, const int*>>FMTgraph::getoutvariablesnactionsptr(const FMTvertex_descriptor& out_vertex) const
	{
	vector<std::pair<const int*, const int*>>pointers;
	FMToutedge_pair edge_pair;
	for (edge_pair = out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
		{
		FMTedgeproperties edgeprop = data[*edge_pair.first];
		int actionid = edgeprop.getactionID();
		pointers.push_back(std::pair<const int*, const int*>(edgeprop.getactionptr(), edgeprop.getvariableptr()));
		}
	return pointers;
	}

vector<const FMTaction*>FMTgraph::selectedactions(const FMTmodel& model, const vector<int>& action_IDS) const
		{
		vector<const FMTaction*>selected;
		if (!action_IDS.empty())
		{
			selected.resize(action_IDS.size());
			int aid = 0;
			for (const int & actid : action_IDS)
			{
				selected[aid] = &model.actions[actid];
				++aid;
			}
		}
		return selected;
		}

bool FMTgraph::constraintlenght(const FMTconstraint& constraint, int& start, int& stop) const
	{
		start = constraint.getperiodlowerbound();
		stop = static_cast<int>((constraint.getperiodupperbound() > (developments.size() - 2)) ? (developments.size() - 2) : constraint.getperiodupperbound());
		if (constraint.acrossperiod())
		{
			--stop;
		}
		return (start < developments.size());
	}

FMTgraphstats FMTgraph::getstats() const
	{
	return stats;
	}

FMTgraphstats* FMTgraph::getstatsptr()
	{
	return &stats;
	}

void FMTgraph::setstats(const FMTgraphstats& newstats)
	{
	stats = newstats;
	}

FMTgraphstats FMTgraph::buildschedule(const FMTmodel& model, std::queue<FMTvertex_descriptor> actives,
	const FMTschedule& schedule)
{
	FMTgraphstats statsdiff(stats);
	//const double* bounds = solverinterface->getColUpper();
	developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
	developments.back().reserve(actives.size()); //realy minimal here!?!?!
	//int queue_size = actives.size();
	//int op_hit = 0;
	while (!actives.empty())
	{
		const FMTvertex_descriptor front_vertex = actives.front();
		actives.pop();
		FMTvertexproperties front_properties = data[front_vertex];
		const FMTdevelopment active_development = front_properties.get();
		//boost::dynamic_bitset<> action_bits(actions.size(), false);
		int action_id = 0;
		for (const FMTaction& action : model.actions)
		{
			if (schedule.elements.find(action) != schedule.elements.end() &&
				(((schedule.elements.at(action)).find(active_development) != (schedule.elements.at(action)).end()) ||
				(!action.lock && active_development.lock != 0 && //lock exempt stuff...
					(schedule.elements.at(action)).find(active_development.clearlock()) != (schedule.elements.at(action)).end())))
			{
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " founda " << action.name <<" transition on "<< transitions[action_id].name<< "\n";
				const vector<FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
				addaction(action_id, statsdiff, actives,
					front_vertex, paths/*, actions, yields*/);
				//action_bits[action_id] = true;
			}/*else if (active_development.operable(action,yields))
				{
				//action_bits[action_id] = true;
				++op_hit;
				}*/
			++action_id;
		}
		FMTfuturdevelopment grown_up = active_development.grow();
		FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
		int variable_id = statsdiff.cols;
		++statsdiff.cols;
		int id = -1;
		double proportion = 100;
		const FMTedgeproperties newedge(id, variable_id, proportion);
		add_edge(front_vertex, next_period, newedge, data);
		++stats.edges;
	}

	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " op hit  " << op_hit << "\n";
	return (statsdiff - stats);
}

FMTgraphstats FMTgraph::eraseperiod()
	{
	for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = developments.at(0).begin();
			it != developments.at(0).end(); it++)
			{
			FMTvertex_descriptor vertex_location = it->second;
			stats.edges-=static_cast<int>(in_degree(vertex_location, data));
			clear_in_edges(vertex_location, data);
			}
	for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = developments.at(0).begin();
			it != developments.at(0).end(); it++)
			{
			FMTvertex_descriptor vertex_location = it->second;
			remove_vertex(vertex_location, data);
			--stats.vertices;
			}
	nodescache.clear();
	developments.erase(developments.begin());
	stats.erasedperiods += 1;
	return stats;
	}

void FMTgraph::setbuildtype(const FMTgraphbuild& build)
	{
	buildtype = build;
	}

bool FMTgraph::empty() const
	{
	return developments.empty();
	}

std::queue<FMTvertex_descriptor> FMTgraph::getactiveverticies() const
	{
	std::queue<FMTvertex_descriptor>actives;
	for (auto& lastperiod : developments[developments.size() - 1])
		{
		actives.push(lastperiod.second);
		}
	return actives;
	}

const std::unordered_map<size_t, FMTvertex_descriptor>& FMTgraph::getperiodverticies(int period) const
	{
	return developments.at(period);
	}


size_t FMTgraph::size() const
	{
	return developments.size();
	}

void FMTgraph::setconstraintID(const FMTvertex_descriptor& vertex, const int& id)
	{
	data[vertex].setconstraintID(id);
	}

bool FMTgraph::gettransferrow(const FMTvertex_descriptor& vertex_descriptor,
	vector<int>&row_starts,
	vector<int>& cols,
	vector<double>& cols_value) const
	{
	//FMTvertexproperties front_properties = data[vertex_it.second];

	FMTinedge_iterator inedge_iterator, inedge_end;
	/*vector<int>cols;
	vector<double>cols_value;
	double row_upper = 0;
	double row_lower = 0;*/
	FMTvertexproperties vertex_property = data[vertex_descriptor];
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "transfer row of " <<string(vertex_property.get())<<" ID "<< vertex_property.getconstraintID() <<"\n";
	row_starts.push_back(int(cols.size()));

	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "setting " << vertex_property.get().mask.getstr() << "\n";
	for (tie(inedge_iterator, inedge_end) = in_edges(vertex_descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
	{
		FMTedgeproperties edgeprop = data[*inedge_iterator];
		cols.push_back(edgeprop.getvariable());
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " FROM " << edgeprop.getvariable() << "\n";
		cols_value.push_back((edgeprop.getproportion() / 100));
	}
	FMTvertexproperties properties = data[vertex_descriptor];
	/*double area = properties.getbaseRHS();
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " area !" << area << "\n";
	if (area > 0)
	{
		row_upper[vertex_property.getconstraintID()] = -area;
		row_lower[vertex_property.getconstraintID()] = -area;
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " ACTUAL !" << row_upper[vertex_property.getconstraintID()] << "\n";
	}*/
	vector<int>locals;
	FMToutedge_iterator outedge_iterator, outedge_end;
	for (tie(outedge_iterator, outedge_end) = out_edges(vertex_descriptor, data); outedge_iterator != outedge_end; ++outedge_iterator)
	{
		FMTedgeproperties edgeprop = data[*outedge_iterator];
		int edgevar = edgeprop.getvariable();
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " STO " << edgevar << "\n";
		if (find(locals.begin(), locals.end(), edgevar) == locals.end())
		{
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " TO " << edgevar << "\n";
			cols.push_back(edgevar);
			locals.push_back(edgevar);
			cols_value.push_back(-1);
		}
	}
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "Adding Row! " <<vertex_property.get().mask.getstr()<<" AGE "<<
		//vertex_property.get().age<<" PERIOD "<< vertex_property.get().period <<" LOCK "<< vertex_property.get().lock<< "\n";
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << cols.size() << " LOWER " << row_lower << " UPPER " << row_upper << "\n";
	/*int CID = solverinterface->getNumRows();
	solverinterface->addRow(int(cols.size()), &cols[0], &cols_value[0], row_lower, row_upper);
	properties.setconstraintID(CID);
	solverinterface->setRowName(properties.getconstraintID(), properties.constraintname());
	solverinterface->setRowType(properties.getconstraintID(), 'E', row_lower, row_upper);*/
	//++stats.transfer_rows;
	return true;

	}

void FMTgraph::getinitialbounds(vector<double>& lower_bounds, vector<double>& upper_bounds) const
	{
	for (const auto& descriptors : developments.at(0))
		{
		const FMTvertex_descriptor descriptor = descriptors.second;
		const FMTvertexproperties property = data[descriptor];
		const map<int, int>outs = getoutvariables(descriptor);
		lower_bounds[outs.at(-1)] = property.getbaseRHS();
		upper_bounds[outs.at(-1)] = property.getbaseRHS();
		}
	}

size_t FMTgraph::nedges() const
	{
	return num_edges(data);
	}

map<string, double> FMTgraph::getoutput(const FMTmodel& model,const FMToutput& output,
	int period,const double* solution,FMToutputlevel level) const
	{
	FMTtheme targettheme;
	vector<string>target_attributes; 
		map<string, double>results;
		if (level != FMToutputlevel::developpement)
			{
			if (level== FMToutputlevel::standard)
				{
				target_attributes = output.getdecomposition(model.themes);
				if (!target_attributes.empty() && level == FMToutputlevel::standard)
					{
					targettheme = output.targettheme(model.themes);
					}
				}
			target_attributes.push_back("Total");
			for (const string& attribute : target_attributes)
				{
				results[attribute] = 0;
				}
			}
		if (!output.islevel())
		{
			for (const FMToutputnode& output_node : output.getnodes())
			{
				//caching with FMToutputnode ?
				map<string, double> srcvalues = getsource(model, output_node, period, targettheme, solution, level);
				//get the level of output to get decent hashing
				/*size_t outid = static_cast<int>(level);
				if (nodescache.find(output_node.source.hash(period, outid)) != nodescache.end())//hash on node source only!
				{
					//Multiply by the factor and constant before usage!!!
					srcvalues = nodescache.at(output_node.hash(period, outid));
				}
				else {
					srcvalues = getsource(model, output_node, period, targettheme, solution,level);
					//if nodes allow hashing like * constant not like * ytimeyield then cash it!!
					//before cashing the node turn it into non multiplied form to allow other nodes to use it
					nodescache[output_node.hash(period, outid)] = srcvalues;
				}*/
				if (level == FMToutputlevel::developpement)
					{
					for (map<string, double>::const_iterator mit = srcvalues.begin(); mit!= srcvalues.end();mit++)
						{
						if (results.find(mit->first)==results.end())
							{
							results[mit->first] = 0;
							}
						results[mit->first] += mit->second;
						}
				}else {
					for (const string& attribute : target_attributes)
						{
						results[attribute] += srcvalues[attribute];
						}
				}

			}
		}
	return results;
	}


map<string, double> FMTgraph::getsource(const FMTmodel& model,
	const FMToutputnode& node,
	int period, const FMTtheme& theme,const double* solution, FMToutputlevel level) const
{
	vector<int>action_IDS;
	vector<const FMTaction*> selected;
	vector<FMTvertex_descriptor>verticies = getnode(model, node, period, action_IDS, selected);
	return getvalues(model, verticies, node, theme, action_IDS, selected, solution, level);

	/*
	map<string, double>values;
	if (level== FMToutputlevel::standard)
		{
		for (auto attribute_id : theme.getvaluenames())
			{
			values[attribute_id.first] = 0;
			}
		}
	if (node.source.useinedges())//evaluate at the begining of the other period if inventory! what a major fuck
		{
		if (!node.source.getaction().empty())
			{
			if (period == 0)
				{
				return values;
				}
		}else {
			++period;
			}
		}*/

	/*if (validouputnode(model,node, action_IDS, period))
	{
		vector<const FMTaction*>selected = selectedactions(model,action_IDS);
		bool inedges = false;
		vector<FMTdevelopmentpath>paths;
		FMTaction optimization_action;
		for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator it = developments.at(period).begin();
			it != developments.at(period).end(); it++)
		{
			if (validgraphnode(model,inedges, it->second, node, action_IDS, selected))
			{
				const FMTdevelopment& development = data[it->second].get();
				string value;
				if (level == FMToutputlevel::standard)
					{
					value = development.mask.get(theme);
				}else if(level == FMToutputlevel::developpement)
					{
					value = string(development);//It's unique so yeah!
					values[value] = 0;
				}else {
					value = "Total";
					}
				if (inedges)
				{
					double coef = 1;
					coef = node.source.getcoef(development, model.yields, optimization_action,paths) * node.factor.getcoef(development, model.yields, optimization_action, paths) * node.constant;
					double area = 0;
					if (period == 0)
					{
						area = outarea(it->second, -1, solution);
					}
					else {
						area = inarea(it->second, solution, true);
					}
					values[value] += coef * area;
				}
				else {
					int actionID = 0;
					for (const FMTaction*& act : selected)
					{
						double action_value = 0;
						paths = getpaths(it->second, action_IDS[actionID]);
						double action_coef = node.source.getcoef(development, model.yields, *act, paths) * node.factor.getcoef(development, model.yields, *act, paths) * node.constant;
						action_value = action_coef * (outarea(it->second, action_IDS[actionID], solution));
						values[value] += action_value;
						++actionID;
					}
				}

			}

		}
		if (level == FMToutputlevel::standard)
			{
			double total = 0;
			for (auto valit : values)
				{
				total += valit.second;
				}	
			values["Total"] = total;
			}
	}
	return values;*/
}

map<string, double> FMTgraph::getvalues(const FMTmodel& model, const vector<FMTvertex_descriptor>& verticies,
	const FMToutputnode& node, const FMTtheme& theme,const vector<int>& action_IDS, const vector<const FMTaction*>& selected,
	const double* solution, FMToutputlevel level) const
{
	map<string, double>values; ///start here
	if (level == FMToutputlevel::standard)
		{
			for (auto attribute_id : theme.getvaluenames())
				{
				values[attribute_id.first] = 0;
				}
		}
	else if (level == FMToutputlevel::totalonly)
		{
		values["Total"] = 0;
		}
	if (!verticies.empty())
		{
		vector<FMTdevelopmentpath>paths;
		FMTaction optimization_action;
		for (const FMTvertex_descriptor& vertex : verticies)
			{
			const FMTdevelopment& development = data[vertex].get();
			string value;
			if (level == FMToutputlevel::standard)
				{
				value = development.mask.get(theme);
				}
				else if (level == FMToutputlevel::developpement)
				{
				value = string(development);//It's unique so yeah!
				values[value] = 0;
				}else {
				value = "Total";
				}

				if (node.source.useinedges())
				{
					double coef = 1;
					coef = node.source.getcoef(development, model.yields, optimization_action, paths) * node.factor.getcoef(development, model.yields, optimization_action, paths) * node.constant;
					double area = 0;
					if (development.period == 0)
					{
						area = outarea(vertex, -1, solution);
					}
					else {
						area = inarea(vertex, solution, true);
					}
					values[value] += coef * area;
				}
				else {
					int actionID = 0;
					for (const FMTaction* act : selected)
					{
						double action_value = 0;
						paths = getpaths(vertex, action_IDS[actionID]);
						double action_coef = node.source.getcoef(development, model.yields, *act, paths) * node.factor.getcoef(development, model.yields, *act, paths) * node.constant;
						action_value = action_coef * (outarea(vertex, action_IDS[actionID], solution));
						values[value] += action_value;
						++actionID;
					}
				}

			}
		}
		if (level == FMToutputlevel::standard)
			{
				double total = 0;
				for (auto valit : values)
				{
					total += valit.second;
				}
				values["Total"] = total;
			}
		return values;
	}


void FMTgraph::cleanevents(vector<FMTevent<FMTgraph>>& events_id, const FMTcoordinate& localisation) const
    {
        vector<FMTevent<FMTgraph>> clean_events_id;
        for (FMTevent<FMTgraph>& event:events_id)
        {
            if(event.contain(localisation))
            {
                event.erase(localisation);
                if(!event.empty())
                {
                    vector<FMTevent<FMTgraph>> potentialysplittedevents;
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
    }

FMTgraphstats FMTgraph::clearfromperiod(const int& period, vector<vector<vector<FMTevent<FMTgraph>>>>& events,
                                        const FMTcoordinate& localisation)
    {
        for (int location = static_cast<int>(this->size()-1) ; location>=period ; --location)
        {
            for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = developments.at(location).begin();
                it != developments.at(location).end(); it++)
                {
                    FMTvertex_descriptor vertex_location = it->second;
                    if (!(location == period && periodstart(vertex_location)))//Condition to keep periodstart at period
                        {
                            //Removing inedge
                            FMTinedge_iterator inedge_iterator, inedge_end;
                            for (tie(inedge_iterator, inedge_end) = in_edges(vertex_location, data); inedge_iterator != inedge_end; ++inedge_iterator)
                            {
                                FMTedgeproperties edgeprop = data[*inedge_iterator];
                                int action_id = edgeprop.getactionID();
                                if (action_id >= 0 && !events.empty())
                                {
                                    //cout<<"Deleting period : "<<location <<"\nAction id : "<<action_id<<endl;
                                    cleanevents(events.at(period-1).at(action_id),localisation);//Period -1 to have the good period in index of events
                                    //cout<<"cleanevents pass"<<endl;
                                }

                            }
                            --stats.edges;
                            //cout<<"Clearing edges"<<endl;
                            clear_in_edges(vertex_location,data);
                        }
                }
                   for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = developments[location].begin();
                    it != developments[location].end(); it++)
                    {
                        FMTvertex_descriptor vertex_location = it->second;
                        if (!(location == period && periodstart(vertex_location)))//Condition to keep periodstart at period
                            {
                                //cout<<"Clearing vertex"<<endl;
                                remove_vertex(vertex_location, data);
                                --stats.vertices;
                            }
                    }
        }
    return stats;
    }

FMTgraph FMTgraph::partialcopy(const int& period, vector<vector<vector<FMTevent<FMTgraph>>>>& events,
                               const FMTcoordinate& localisation) const
    {
        FMTgraph newgraph(*this);
        FMTgraphstats delstats = newgraph.clearfromperiod(period,events,localisation);
        newgraph.generatedevelopments();
        return newgraph;
    }

void FMTgraph::generatedevelopments()
    	{
			
            developments.clear();
			FMTvertex_iterator vertex, vend;
			tie(vertex, vend) = vertices(data);
			--vend;//get to last vertex...
			size_t max_period = (data[*vend].get().period - stats.erasedperiods);
			developments.resize(max_period+1);
            for (boost::tie(vertex, vend) = vertices(data); vertex != vend; ++vertex)
            {
                const FMTvertexproperties& properties = data[*vertex];
                const FMTdevelopment& dev = properties.get();
				size_t period_location = (dev.period-stats.erasedperiods);
				/*if (period_location == developments.size())
                {
                    developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
                }*/
                developments[period_location][boost::hash<FMTdevelopment>()(dev)] = *vertex;
            }
        }

FMTgraph FMTgraph::perturbgraph(const FMTmodel& model,default_random_engine& generator,
                                vector<vector<vector<FMTevent<FMTgraph>>>>& events,
                                const FMTcoordinate& localisation, const int period) const
    {
        FMTgraph newgraph = partialcopy(period,events,localisation);
        newgraph.nodescache.clear();
        while(this->size() != newgraph.size())
        {
            std::queue<FMTvertex_descriptor> actives = newgraph.getactiveverticies();
            FMTvertex_descriptor front_vertex = actives.front();
			const FMTvertexproperties& front_properties = data[front_vertex];
			int periodbuildevents = front_properties.get().period-1;//To have the good index in events
            newgraph.randombuild(model,actives,generator,events.at(periodbuildevents),localisation);
        }

    return newgraph;
    }

void FMTgraph::updatevarsmap(map<int, double>& variables, const int& var, const double& coef) const
	{
	if (variables.find(var) == variables.end())
		{
			variables[var] = coef;
		}
		else {
			variables[var] += coef;
		}
	}

size_t FMTgraph::geterasedperiods() const
	{
	size_t eperiod = 0;
	if (!developments.empty() && !developments.at(0).empty())
		{
		eperiod = data[developments.at(0).begin()->second].get().period;
		}
	return eperiod;
	}

size_t FMTgraph::hash(size_t seed) const
	{
	boost::hash_combine(seed, boost::hash<Core::FMTdevelopment>()(data[developments.at(0).begin()->second].get()));
	FMTedge_iterator edge_iterator, edge_iterator_end;
	for (tie(edge_iterator, edge_iterator_end) = edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
		{
		const FMTedgeproperties& edgeprop = data[*edge_iterator];
		boost::hash_combine(seed, edgeprop.getactionID());
		}
	return seed;
	}

bool FMTgraph::sameedgesas(const FMTgraph& rhs) const
	{
	FMTedge_iterator thisedge_iterator, thisedge_iterator_end;
	FMTedge_iterator rhsedge_iterator,  rhsedge_iterator_end;
	bool different = false;
	tie(thisedge_iterator, thisedge_iterator_end) = edges(data);
	tie(rhsedge_iterator, rhsedge_iterator_end) = edges(rhs.data);
	while (!different && thisedge_iterator != thisedge_iterator_end && rhsedge_iterator != rhsedge_iterator_end)
		{
		const FMTedgeproperties& thisedgeprop = data[*thisedge_iterator];
		const FMTedgeproperties& rhsedgeprop = rhs.data[*rhsedge_iterator];
		if (thisedgeprop.getactionID() != rhsedgeprop.getactionID())
			{
			different = true;
			}
		++thisedge_iterator;
		++rhsedge_iterator;
		}
	return different;
	}


}
