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
#include <tuple>
#include <assert.h>

namespace Graph
{

FMTgraph::FMTgraph():
	data(),
    buildtype(FMTgraphbuild::nobuild),
    developments(),
	nodescache(),
    stats()
    {

    }

FMTgraph::FMTgraph(const FMTgraphbuild lbuildtype):
	data(),
    buildtype(lbuildtype),
    developments(),
	nodescache(),
    stats()
    {

    }

FMTgraph::FMTgraph(const FMTgraph& rhs):
	data(rhs.data),
    buildtype(rhs.buildtype),
    developments(),
	nodescache(),
    stats(rhs.stats)
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
		   std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::const_iterator devsit = this->getfirstconstblock();
		   size_t location = 0;
            while (devsit!= developments.end())
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
								  std::vector<FMTedgeproperties>edges;
								  std::vector<FMTedgeproperties>rhsedges;
                                  for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_location, data); inedge_iterator != inedge_end; ++inedge_iterator)
                                        {
                                        edges.push_back(data[*inedge_iterator]);
                                        }
                                  for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(rhsvertex_location, rhs.data); inedge_iterator != inedge_end; ++inedge_iterator)
                                        {
                                        rhsedges.push_back(rhs.data[*inedge_iterator]);
                                        }
                                  std::sort(edges.begin(), edges.end());
                                  std::sort(rhsedges.begin(), rhsedges.end());
                                  if (edges!= rhsedges)
                                        {
                                        return false;
                                        }
                                  }
                           }
					++location;
					++devsit;
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

bool FMTgraph::containsdevelopment(const Core::FMTdevelopment& developement) const
	{
		return ((developments.size() > (developement.period)) &&
			developments.at((developement.period)).find(boost::hash<Core::FMTdevelopment>()(developement)) != developments.at((developement.period)).end());
	}

std::queue<FMTvertex_descriptor> FMTgraph::initialize(const std::vector<Core::FMTactualdevelopment>& actdevelopments)
    {
        std::queue<FMTvertex_descriptor>actives;
		developments.clear();
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		developments.back().reserve(actdevelopments.size());
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		developments.back().reserve(actdevelopments.size());
        const int constraint_id = -1;
        int edge_id = -1;
        const double proportion = 100;
		stats.edges = 0;
		stats.vertices = 0;
		for (const Core::FMTactualdevelopment& development : actdevelopments)
		{
            //P0
			const FMTvertexproperties properties(development, constraint_id);
			FMTvertex_descriptor newvertex = boost::add_vertex(properties, data);
			developments[0][boost::hash<Core::FMTdevelopment>()(development)] = newvertex;
			++stats.vertices;
            //P1
			Core::FMTfuturdevelopment P1dev(development);
            P1dev.period = 1;
            FMTvertex_descriptor tovertex;
            tovertex = adddevelopment(P1dev);
            actives.push(tovertex);
            //Now set the edge!!
			const FMTedgeproperties newedge(edge_id, stats.edges, proportion);
			boost::add_edge(newvertex,tovertex, newedge, data);
			++stats.edges;
		}

		return actives;
	}

FMTgraphstats FMTgraph::build(const Models::FMTmodel& model,std::queue<FMTvertex_descriptor> actives)
	{
		FMTgraphstats statsdiff(stats);
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		while (!actives.empty())
		{
			const FMTvertex_descriptor front_vertex = actives.front();
			actives.pop();
			FMTvertexproperties front_properties = data[front_vertex];
			const Core::FMTdevelopment active_development = front_properties.get();
			int action_id = 0;
			bool death = false;
			for (const Core::FMTaction& action : model.actions)
			{
				if (active_development.operable(action, model.yields))
				{
					if (action.getname() == "_DEATH")
						{
						death = true;
						}
					const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
					addaction(action_id, statsdiff, actives, front_vertex, paths);
				}
				++action_id;
			}
			if (!death)
				{
				const Core::FMTfuturdevelopment grown_up = active_development.grow();
				FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
				const FMTedgeproperties newedge(-1, statsdiff.cols, 100);
				++statsdiff.cols;
				boost::add_edge(front_vertex, next_period, newedge, data);
				++stats.edges;
				}
			
		}

		return (statsdiff - stats);
    }

std::vector<std::pair<size_t,int>> FMTgraph::adjacentevents(const std::vector<Spatial::FMTevent<FMTgraph>>& events, const Spatial::FMTcoordinate& localisation,const int action_id) const
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
    }

std::pair<size_t,int> FMTgraph::randomoperate(const std::vector<std::pair<size_t,int>>& operables, const Models::FMTmodel& model, std::queue<FMTvertex_descriptor>& actives,
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
    }

FMTgraphstats FMTgraph::randombuild(const Models::FMTmodel& model,std::queue<FMTvertex_descriptor> actives, std::default_random_engine& generator,
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
			const Core::FMTdevelopment active_development = front_properties.get();
            const Core::FMTfuturdevelopment grown_up = active_development.grow();
            FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
            const FMTedgeproperties newedge(-1, 0, 100);
            boost::add_edge(front_vertex, next_period, newedge, data);
            ++stats.edges;
		}

		return (statsdiff - stats);
    }

std::vector<Core::FMTactualdevelopment> FMTgraph::getperiodstopdev(const int location,const double* actual_solution) const
	{
		std::vector<Core::FMTactualdevelopment>all_period_stop_devs;
		for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator devit = developments.at(location).begin();
			devit != developments.at(location).end(); devit++)
		{
            if (periodstop(devit->second))
            {
                const double area = inarea(devit->second, actual_solution);
                const Core::FMTdevelopment& development = data[devit->second].get();
                all_period_stop_devs.push_back(Core::FMTactualdevelopment(development, area));
            }
		}
		all_period_stop_devs.size();
		return all_period_stop_devs;
	}

FMTvertex_descriptor FMTgraph::getdevelopment(const Core::FMTdevelopment& developement) const
	{
		return developments.at(developement.period).at(boost::hash<Core::FMTdevelopment>()(developement));
	}

const Core::FMTdevelopment& FMTgraph::getdevelopment(const FMTvertex_descriptor& descriptor) const
	{
	return data[descriptor].get();
	}

FMTvertex_descriptor FMTgraph::adddevelopment(const Core::FMTfuturdevelopment& futurdevelopement)
	{
		if (!this->containsdevelopment(futurdevelopement))
		{
			const int constraint_id = -1;
			const FMTvertexproperties properties(futurdevelopement, constraint_id);
			FMTvertex_descriptor newvertex = boost::add_vertex(properties, data);
			developments[futurdevelopement.period][boost::hash<Core::FMTdevelopment>()(futurdevelopement)] = newvertex;
			++stats.vertices;
			return newvertex;
		}
		return getdevelopment(futurdevelopement);
	}

void FMTgraph::addaction(const int& actionID,
                            FMTgraphstats& statsdiff,
                            std::queue<FMTvertex_descriptor>& actives,
                            const FMTvertex_descriptor& out_vertex,
                            const std::vector<Core::FMTdevelopmentpath>& paths)
	{
		int variable_id = statsdiff.cols;
		++statsdiff.cols;
		std::vector<FMTvertex_descriptor>active_vertex;
		for (const Core::FMTdevelopmentpath& devpath : paths)
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
			boost::add_edge(out_vertex, tovertex, newedge, data);
            ++stats.edges;
		}
	}

double FMTgraph::outarea(const FMTvertex_descriptor& out_vertex, const int& actionID, const double*&  solution) const
	{
		FMToutedge_iterator outedge_iterator, outedge_end;
		double value = 0;
		for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
		{
			const FMTedgeproperties& edgeprop = data[*outedge_iterator];
			if (edgeprop.getactionID() == actionID)
			{
				value += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
			}
		}
		return value;
	}

double FMTgraph::inarea(const FMTvertex_descriptor& out_vertex, const double*&  solution, bool growth) const
	{
		FMTinedge_iterator inedge_iterator, inedge_end;
		double area = 0;
		for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTedgeproperties& edgeprop = data[*inedge_iterator];
				if (edgeprop.getactionID() < 0 || !growth)
				{
					area += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
				}
			}
		return area;
	}

bool FMTgraph::periodstart(const FMTvertex_descriptor& out_vertex) const
	{
	const FMTvertexproperties& properties = data[out_vertex];
	FMTinedge_iterator inedge_iterator, inedge_end;
	for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
		{
		const FMTedgeproperties& edgeprop = data[*inedge_iterator];
		if (edgeprop.getactionID()==-1)
			{
			return true;
			}
		}
	return false;
	}

bool FMTgraph::periodstop(const FMTvertex_descriptor& out_vertex) const
	{
		FMToutedge_iterator outedge_iterator, outedge_end;
		const FMTvertexproperties& source_properties = data[out_vertex];
		if ((source_properties.get().period)==0)
			{
			return true;
			}
		for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
		{
			const FMTedgeproperties& edgeprop = data[*outedge_iterator];
			if (edgeprop.getactionID() == -1)
				{
				return true;
				}
		}
		return false;
	}

std::vector<Core::FMTdevelopmentpath> FMTgraph::getpaths(const FMTvertex_descriptor& out_vertex, const int& actionID) const
	{
		std::vector<Core::FMTdevelopmentpath>paths;
		for (FMToutedge_pair edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
		{
			const FMTedgeproperties& edgeprop = data[*edge_pair.first];
			if (edgeprop.getactionID() == actionID)
			{
				const FMTvertexproperties& vertex_target = data[target(*edge_pair.first, data)];
				paths.push_back(Core::FMTdevelopmentpath(vertex_target.get(), edgeprop.getproportion()));
			}
		}
		return paths;
	}

bool FMTgraph::isvalidouputnode(const Models::FMTmodel& model,const Core::FMToutputnode& node, std::vector<const Core::FMTaction*>& action_IDS, int period) const
    {
	if (developments.size() > period)
		{
		action_IDS = node.source.targets(model.actions);
		if (!(period == 0 && !action_IDS.empty()) && !node.source.isnull(model.yields) && !node.factor.isnull(model.yields) && (!action_IDS.empty() && !node.source.getaction().empty() || node.source.getaction().empty()))
			{
			return true;
			}
		}
	return false;
	}

bool FMTgraph::isvalidgraphnode(const Models::FMTmodel& model, const FMTvertex_descriptor& vertex_descriptor,
                                const Core::FMToutputnode& node,const std::vector<const Core::FMTaction*>& selected) const
	{
	const Core::FMTdevelopment& development = data[vertex_descriptor].get();
	if (node.source.use(development, model.yields))
		{
		if (node.source.useinedges())
		{
			if ((development.period == 0 || periodstart(vertex_descriptor))&&(selected.empty() && (node.source.isnextperiod() || !node.source.emptylock())) ||
				(((buildtype == FMTgraphbuild::schedulebuild) && development.anyoperable(selected, model.yields)) ||
					anyoperables(vertex_descriptor, development.anyworthtestingoperability(selected, *model.actions.begin()))))
			{
				return true;
			}
		}
		else if ((anyoperables(vertex_descriptor, development.anyworthtestingoperability(selected, *model.actions.begin())))) //out edges
		{
			return true;
		}
		}
	return false;
	}

std::map<int, double> FMTgraph::locatenode(const Models::FMTmodel& model, Core::FMToutputnode output_node,int period) const
	{
		const std::vector<FMTvertex_descriptor>verticies = getnode(model, output_node, period);
		return getvariables(model, output_node, verticies);

	}


std::vector<FMTvertex_descriptor> FMTgraph::getnode(const Models::FMTmodel& model, Core::FMToutputnode output_node,
	int period) const
	{
			std::vector<FMTvertex_descriptor>locations;
			std::vector<int>targetedperiods;
			const int maxperiod = static_cast<int>(developments.size() - 2);
			const int node_period = output_node.settograph(targetedperiods, period, maxperiod);
			if (node_period<0)
				{
				return locations;
				}
			size_t lookedat_size = 0;
			if (!output_node.source.isvariablelevel())
			{
				std::vector<const Core::FMTaction*> selected;
				if (isvalidouputnode(model, output_node, selected, node_period))
				{
					for (const int localnodeperiod : targetedperiods)
						{
						while (nodescache.size()!=developments.size())
							{
							const size_t devindex = nodescache.size();
							nodescache.push_back(FMToutputnodecache(developments.at(devindex)));
							}
						bool exactverticies = false;
						const std::vector<FMTvertex_descriptor>& descriptors = nodescache.at(localnodeperiod).getverticies(output_node,model.actions,model.themes, exactverticies);
						if (exactverticies)
							{
							locations.reserve(locations.size() + descriptors.size());
							locations.insert(locations.end(),descriptors.begin(), descriptors.end());
						}else {
							std::vector<FMTvertex_descriptor>revisited;
							for (const FMTvertex_descriptor& potential : descriptors)
								{
									if (isvalidgraphnode(model, potential, output_node, selected))
									{
										revisited.push_back(potential);
										locations.push_back(potential);
									}
								}
							std::sort(revisited.begin(), revisited.end());
							nodescache.at(localnodeperiod).setvalidverticies(output_node, revisited);
							}
						}
					
					
				}

			}
		return locations;
	}

std::map<int, double> FMTgraph::getvariables(const Models::FMTmodel& model, const Core::FMToutputnode& output_node,
	const std::vector<FMTvertex_descriptor>& verticies) const
	{
	std::map<int, double>variables;
	if (!verticies.empty())
		{
		std::vector<Core::FMTdevelopmentpath>paths;
		Core::FMTaction optimization_action;
		const std::vector<const Core::FMTaction*> selected = output_node.source.targets(model.actions);
		for (const FMTvertex_descriptor& vertex : verticies)
			{
			const Core::FMTdevelopment& development = data[vertex].get();
			if (output_node.source.useinedges())
				{
				const double coef = output_node.source.getcoef(development, model.yields, optimization_action, paths) * output_node.factor.getcoef(development, model.yields, optimization_action, paths) * output_node.constant;
				if (development.period == 0)
					{
						const std::map<int, int>vars = getoutvariables(vertex);
						updatevarsmap(variables, vars.at(-1), coef);
					}else{
						FMTinedge_iterator inedge_iterator, inedge_end;
						for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
						{
							const FMTedgeproperties& edgeprop = data[*inedge_iterator];
							const int actionid = edgeprop.getactionID();
							if (actionid < 0)
								{
								updatevarsmap(variables, edgeprop.getvariableID(), (edgeprop.getproportion() / 100)*coef);
								continue;
								}
							const FMTvertex_descriptor sourceverex = boost::source(*inedge_iterator, data);
							const FMTvertexproperties& sourceproperties = data[sourceverex];
							if ((sourceproperties.get().period == development.period && !periodstart(sourceverex)))
								{
								updatevarsmap(variables, edgeprop.getvariableID(), (edgeprop.getproportion() / 100)*coef);
								}
						}
					}
				
			}else {
				const std::map<int, int>outvars = getoutvariables(vertex);
				for (const Core::FMTaction* act : selected)
				{
					const int actionID = static_cast<int>(std::distance(&(*model.actions.begin()), act));
					if (outvars.find(actionID) != outvars.end())
					{
						paths = getpaths(vertex, actionID);
						const double action_coef = output_node.source.getcoef(development, model.yields, *act, paths) * output_node.factor.getcoef(development, model.yields, *act, paths) * output_node.constant;
						updatevarsmap(variables, outvars.at(actionID), action_coef);
					}
				}
			}
			}
		}
	return variables;
	}



bool FMTgraph::anyoperables(const FMTvertex_descriptor& descriptor, const std::vector<int>& action_ids) const
	{
	if (!action_ids.empty())
		{
		std::map<int, int> variables = getoutvariables(descriptor);
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

std::map<int, int> FMTgraph::getoutvariables(const FMTvertex_descriptor& out_vertex) const
	{
		std::map<int, int> mapping;
		FMToutedge_pair edge_pair;
		for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
		{
			const FMTedgeproperties& edgeprop = data[*edge_pair.first];
			int actionid = edgeprop.getactionID();
			mapping[actionid] = edgeprop.getvariableID();
		}
		return mapping;
	}

std::vector<int>FMTgraph::getoutactions(const FMTvertex_descriptor& out_vertex) const
	{
	std::vector<int>actions;
	const size_t outsize = boost::out_degree(out_vertex, data);
	if (outsize > 1)
		{
		actions.reserve(outsize-1);
		FMToutedge_pair edge_pair;
		for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
			{
			const FMTedgeproperties& edgeprop = data[*edge_pair.first];
			const int* actionid = edgeprop.getactionptr();
			if ((*actionid) >= 0)
				{
				actions.emplace_back(*actionid);
				}
			}
		std::sort(actions.begin(), actions.end());
		}
	return actions;
	}

std::vector<const Core::FMTaction*>FMTgraph::selectedactions(const Models::FMTmodel& model, const std::vector<int>& action_IDS) const
		{
		std::vector<const Core::FMTaction*>selected;
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

bool FMTgraph::constraintlenght(const Core::FMTconstraint& constraint, int& start, int& stop) const
	{
		start = std::max(constraint.getperiodlowerbound(),getfirstactiveperiod());
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

FMTgraphstats FMTgraph::buildschedule(const Models::FMTmodel& model, std::queue<FMTvertex_descriptor> actives,
	const Core::FMTschedule& schedule)
{
	FMTgraphstats statsdiff(stats);
	developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
	developments.back().reserve(actives.size()); 
	while (!actives.empty())
	{
		const FMTvertex_descriptor front_vertex = actives.front();
		actives.pop();
		FMTvertexproperties front_properties = data[front_vertex];
		const Core::FMTdevelopment active_development = front_properties.get();
		bool death = false;
		int action_id = 0;
		for (const Core::FMTaction& action : model.actions)
		{
			if (schedule.find(action) != schedule.end() &&
				(((schedule.at(action)).find(active_development) != (schedule.at(action)).end()) ||
				(!action.dorespectlock() && active_development.lock != 0 &&
					(schedule.at(action)).find(active_development.clearlock()) != (schedule.at(action)).end())))
			{
				if (action.getname() == "_DEATH")
					{
					death = true;
					}
				const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
				addaction(action_id, statsdiff, actives,
					front_vertex, paths);
			}
			++action_id;
		}
		if (!death)
			{
			const Core::FMTfuturdevelopment grown_up = active_development.grow();
			FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
			const int variable_id = statsdiff.cols;
			++statsdiff.cols;
			const int id = -1;
			const double proportion = 100;
			const FMTedgeproperties newedge(id, variable_id, proportion);
			boost::add_edge(front_vertex, next_period, newedge, data);
			++stats.edges;
			}
		
	}
	return (statsdiff - stats);
}

FMTgraphstats FMTgraph::eraseperiod(std::vector<int>&deletedconstraints,
									std::vector<int>&deletedvariables,
									bool keepbounded)
	{
	std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::iterator periodit = this->getfirstblock();
	for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = periodit->begin();
			it != periodit->end(); it++)
			{
		    FMTvertex_descriptor& vertex_location = it->second;
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(it->second, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTedgeproperties& edgeproperty = data[*inedge_iterator];
					int varvalue = edgeproperty.getvariableID();
					if (std::find(deletedvariables.begin(), deletedvariables.end(), varvalue)== deletedvariables.end())
						{
						--stats.cols;
						deletedvariables.push_back(varvalue);
						}
					--stats.edges;
				}
			boost::clear_in_edges(vertex_location, data);
			}
		std::unordered_map<size_t, FMTvertex_descriptor>restingdevelopments;
		for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = periodit->begin();
				it != periodit->end(); it++)
				{
				const FMTvertex_descriptor& vertex_location = it->second;
				FMTvertexproperties& vertexproperty = data[vertex_location];
				const int constvalue = vertexproperty.getconstraintID();
				if (constvalue >= 0)
					{
					--stats.rows;
					--stats.transfer_rows;
					deletedconstraints.push_back(constvalue);
					vertexproperty.setconstraintID(-1);
					}
				if (!keepbounded || out_degree(vertex_location,data)==0)
					{
					boost::remove_vertex(vertex_location, data);
					--stats.vertices;
				}else {
					restingdevelopments[boost::hash<Core::FMTdevelopment>()(vertexproperty.get())] = it->second;
					}
				}
	*periodit = restingdevelopments;
	const int firstperiod = this->getfirstactiveperiod();
	if (static_cast<size_t>(firstperiod)< nodescache.size())
		{
		nodescache[firstperiod].clear();
		}
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
	for (auto& lastperiod : developments.back())
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
	std::vector<int>&row_starts,
	std::vector<int>& cols,
	std::vector<double>& cols_value) const
	{
	FMTinedge_iterator inedge_iterator, inedge_end;
	FMTvertexproperties vertex_property = data[vertex_descriptor];
	row_starts.push_back(static_cast<int>(cols.size()));
	for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
	{
		const FMTedgeproperties& edgeprop = data[*inedge_iterator];
		cols.push_back(edgeprop.getvariableID());
		cols_value.push_back((edgeprop.getproportion() / 100));
	}
	std::vector<int>locals;
	FMToutedge_iterator outedge_iterator, outedge_end;
	for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(vertex_descriptor, data); outedge_iterator != outedge_end; ++outedge_iterator)
	{
		const FMTedgeproperties& edgeprop = data[*outedge_iterator];
		const int edgevar = edgeprop.getvariableID();
		if (std::find(locals.begin(), locals.end(), edgevar) == locals.end())
		{
			cols.push_back(edgevar);
			locals.push_back(edgevar);
			cols_value.push_back(-1);
		}
	}
	return true;

	}

void FMTgraph::getinitialbounds(std::vector<double>& lower_bounds, std::vector<double>& upper_bounds) const
	{
	for (const auto& descriptors : developments.at(0))
		{
		const FMTvertex_descriptor descriptor = descriptors.second;
		const FMTvertexproperties& property = data[descriptor];
		const std::map<int, int>outs = getoutvariables(descriptor);
		lower_bounds[outs.at(-1)] = property.getbaseRHS();
		upper_bounds[outs.at(-1)] = property.getbaseRHS();
		}
	}

size_t FMTgraph::nedges() const
	{
	return boost::num_edges(data);
	}

std::map<std::string, double> FMTgraph::getoutput(const Models::FMTmodel& model,const Core::FMToutput& output,
	int period,const double* solution,FMToutputlevel level) const
	{
	Core::FMTtheme targettheme;
	std::vector<std::string>target_attributes;
	std::map<std::string, double>results;
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
			for (const std::string& attribute : target_attributes)
				{
				results[attribute] = 0;
				}
			}
		if (!output.islevel())
		{
			for (const Core::FMToutputnode& output_node : output.getnodes())
			{
				std::map<std::string, double> srcvalues = getsource(model, output_node, period, targettheme, solution, level);
				if (level == FMToutputlevel::developpement)
					{
					for (std::map<std::string, double>::const_iterator mit = srcvalues.begin(); mit!= srcvalues.end();mit++)
						{
						if (results.find(mit->first)==results.end())
							{
							results[mit->first] = 0;
							}
						results[mit->first] += mit->second;
						}
				}else {
					for (const std::string& attribute : target_attributes)
						{
						results[attribute] += srcvalues[attribute];
						}
				}

			}
		}
	return results;
	}


std::map<std::string, double> FMTgraph::getsource(const Models::FMTmodel& model,
	const Core::FMToutputnode& node,
	int period, const Core::FMTtheme& theme,const double* solution, FMToutputlevel level) const
{
	std::vector<FMTvertex_descriptor>verticies = getnode(model, node, period);
	return getvalues(model, verticies, node, theme, solution, level);
}

std::map<std::string, double> FMTgraph::getvalues(const Models::FMTmodel& model, const std::vector<FMTvertex_descriptor>& verticies,
	const Core::FMToutputnode& node, const Core::FMTtheme& theme,
	const double* solution, FMToutputlevel level) const
{
	std::map<std::string, double>values;
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
		const std::vector<const Core::FMTaction*> selected = node.source.targets(model.actions);
		std::vector<Core::FMTdevelopmentpath>paths;
		Core::FMTaction optimization_action;
		for (const FMTvertex_descriptor& vertex : verticies)
			{
			const Core::FMTdevelopment& development = data[vertex].get();
			std::string value;
			if (level == FMToutputlevel::standard)
				{
				value = development.mask.get(theme);
				}
				else if (level == FMToutputlevel::developpement)
				{
				value = std::string(development);
				values[value] = 0;
				}else {
				value = "Total";
				}

				if (node.source.useinedges())
				{
					const double coef = node.source.getcoef(development, model.yields, optimization_action, paths) * node.factor.getcoef(development, model.yields, optimization_action, paths) * node.constant;
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
					for (const Core::FMTaction* act : selected)
					{
						double action_value = 0;
						const int actionID = static_cast<int>(std::distance(&(*model.actions.begin()),act));
						paths = getpaths(vertex, actionID);
						const double action_coef = node.source.getcoef(development, model.yields, *act, paths) * node.factor.getcoef(development, model.yields, *act, paths) * node.constant;
						action_value = action_coef * (outarea(vertex, actionID, solution));
						values[value] += action_value;
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


void FMTgraph::cleanevents(std::vector<Spatial::FMTevent<FMTgraph>>& events_id, const Spatial::FMTcoordinate& localisation) const
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
    }

FMTgraphstats FMTgraph::clearfromperiod(const int& period, std::vector<std::vector<std::vector<Spatial::FMTevent<FMTgraph>>>>& events,
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
    }

FMTgraph FMTgraph::partialcopy(const int& period, std::vector<std::vector<std::vector<Spatial::FMTevent<FMTgraph>>>>& events,
                               const Spatial::FMTcoordinate& localisation) const
    {
        FMTgraph newgraph(*this);
        const FMTgraphstats delstats = newgraph.clearfromperiod(period,events,localisation);
        newgraph.generatedevelopments();
        return newgraph;
    }

void FMTgraph::generatedevelopments()
    	{
			
            developments.clear();
			FMTvertex_iterator vertex, vend;
			boost::tie(vertex, vend) = vertices(data);
			--vend;
			const size_t max_period = (data[*vend].get().period);
			developments.resize(max_period+1);
            for (boost::tie(vertex, vend) = vertices(data); vertex != vend; ++vertex)
            {
                const FMTvertexproperties& properties = data[*vertex];
                const Core::FMTdevelopment& dev = properties.get();
				const size_t period_location = (dev.period);
                developments[period_location][boost::hash<Core::FMTdevelopment>()(dev)] = *vertex;
            }
        }

FMTgraph FMTgraph::perturbgraph(const Models::FMTmodel& model, std::default_random_engine& generator,
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
    }

void FMTgraph::updatevarsmap(std::map<int, double>& variables, const int& var, const double& coef) const
	{
	std::map<int, double>::iterator varit = variables.find(var);
	if (varit == variables.end())
		{
			variables[var] = coef;
		}
		else {
			varit->second += coef;
		}
	}


size_t FMTgraph::hash(size_t seed) const
	{
	boost::hash_combine(seed, boost::hash<Core::FMTdevelopment>()(data[developments.at(0).begin()->second].get()));
	FMTedge_iterator edge_iterator, edge_iterator_end;
	for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
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
	boost::tie(thisedge_iterator, thisedge_iterator_end) = boost::edges(data);
	boost::tie(rhsedge_iterator, rhsedge_iterator_end) = boost::edges(rhs.data);
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

void FMTgraph::updatematrixindex(const std::vector<int>& removedvariables,const std::vector<int>& removedconstraints)
	//When the solverinterface delete a constraint or a variable
	//You need to update the whole graph...
	//Call this function once ...alot of computing dones
	{
	std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::iterator perioddevsit = this->getfirstblock();
	while (perioddevsit!= developments.end())
			{
			for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = perioddevsit->begin();
				it != perioddevsit->end(); it++)
				{
				if (!removedconstraints.empty())
					{
					FMTvertexproperties& vertexproperty = data[it->second];
					int actualconstraint = vertexproperty.getconstraintID();
					if (actualconstraint >= 0)
						{
						std::vector<int>::const_iterator removeditconstraint = removedconstraints.begin();
						int toremove = 0;
						while (removeditconstraint != removedconstraints.end() && actualconstraint > *removeditconstraint)
							{
							++toremove;
							++removeditconstraint;
							}
						vertexproperty.setconstraintID(actualconstraint - toremove);
						}
					}
				
				if (!removedvariables.empty())
					{
					FMTinedge_iterator inedge_iterator, inedge_end;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(it->second, data); inedge_iterator != inedge_end; ++inedge_iterator)
						{
						FMTedgeproperties& edgeproperty = data[*inedge_iterator];
						std::vector<int>::const_iterator removeditvariable = removedvariables.begin();
						const int actualvariable = edgeproperty.getvariableID();
						int toremove = 0;
						while (removeditvariable != removedvariables.end() && actualvariable > *removeditvariable)
						{
							++toremove;
							++removeditvariable;
						}
						edgeproperty.setvariableID(actualvariable - toremove);
						}
					}
				
				}
			++perioddevsit;
			}
	}

std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::iterator FMTgraph::getfirstblock()
	{
	std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::iterator periodit = developments.begin();
	while (periodit->empty())
		{
		++periodit;
		}
	return periodit;
	}
std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::const_iterator FMTgraph::getfirstconstblock() const
	{
	std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::const_iterator periodit = developments.begin();
	while (periodit->empty())
		{
		++periodit;
		}
	return periodit;
	}

int FMTgraph::getfirstactiveperiod() const
	{
	return static_cast<int>(std::distance(developments.begin(), getfirstconstblock()));
	}

FMTgraph FMTgraph::postsolve(const Core::FMTmask& selectedmask,
	const std::vector<Core::FMTtheme>&originalbasethemes,
	const std::map<int, int>& actionmapconnection) const
	{
	FMTgraph newgraph(*this);
	newgraph.developments.clear();
	newgraph.nodescache.clear();//Some postsolve can be done here to keep some usefull information but for now just clear
	//start by remapping the actions
	FMTedge_iterator edge_iterator, edge_iterator_end;
	for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
		{
		FMTedgeproperties& edgeprop = newgraph.data[*edge_iterator];
		edgeprop.setactionID(actionmapconnection.at(edgeprop.getactionID()));
		}
	FMTvertex_iterator vertex_iterator, vertex_iterator_end;
	for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
		{
		FMTvertexproperties& vertexprop = newgraph.data[*vertex_iterator];
		vertexprop.setdevlopementmask(vertexprop.get().mask.postsolve(selectedmask, originalbasethemes));
		}
	newgraph.generatedevelopments();
	return newgraph;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTgraph);
