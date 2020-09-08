/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlinegraph.h"
#include "FMTexceptionhandler.h"
#include "FMTmodel.h"
#include "FMTlogger.h"


namespace Graph
{
    FMTlinegraph::FMTlinegraph():
		FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>(FMTgraphbuild::nobuild),periodstatsdiff()
    {

    }

    FMTlinegraph::FMTlinegraph(const FMTgraphbuild lbuildtype):
		FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>(lbuildtype),periodstatsdiff()
    {

    }

    FMTlinegraph::FMTlinegraph(const FMTlinegraph& rhs):
		FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>(rhs),periodstatsdiff(rhs.periodstatsdiff)
    {

    }

    FMTlinegraph::FMTlinegraph(const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>& rhs):
		FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>(rhs),periodstatsdiff()
    {

    }

    FMTlinegraph& FMTlinegraph::operator = (const FMTlinegraph& rhs)
    {
        if(this!=&rhs)
            {
			FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::operator = (rhs);
			periodstatsdiff = rhs.periodstatsdiff;
            }
        return *this;
    }

    FMTlinegraph& FMTlinegraph::operator = (const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>& rhs)
    {
        if(this!=&rhs)
            {
			FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::operator = (rhs);
			periodstatsdiff = FMTgraphstats();
            }
        return *this;
    }

    int FMTlinegraph::getperiod() const
    {
        std::queue<FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::FMTvertex_descriptor> actives = getactiveverticies();
		FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::FMTvertex_descriptor front_vertex = actives.front();
        const Graph::FMTbasevertexproperties& front_properties = data[front_vertex];
        const int period = front_properties.get().period;
        return period;
    }

	void FMTlinegraph::newperiod()
	{
		Exception::FMTexceptionhandler handler;
		std::queue<FMTvertex_descriptor> actives = getactiveverticies();
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		periodstatsdiff = getstats();
		if (actives.size() > 1)
		{
			handler.raise(Exception::FMTexc::FMTnotlinegraph, "More than one active at the begining of the period", "FMTlinegraph::newperiod()", __LINE__, __FILE__);
		}
		if (actives.size() < 1)
		{
			handler.raise(Exception::FMTexc::FMTnotlinegraph, "0 actives at the begining of the period", "FMTlinegraph::newperiod()", __LINE__, __FILE__);
		}
	}

	void FMTlinegraph::addaction(FMTvertex_descriptor active,const int& actionID,
		const std::vector<Core::FMTdevelopmentpath>& paths)
	{
		for (const Core::FMTdevelopmentpath& devpath : paths)
		{
			const FMTedgeproperties newedge(actionID, 0, devpath.proportion);
			FMTvertex_descriptor tovertex;
			tovertex = this->adddevelopment(*devpath.development);
			boost::add_edge(active, tovertex, newedge, data);
			++stats.edges;
		}
	}

	std::vector<Core::FMTdevelopmentpath> FMTlinegraph::operate(const Core::FMTaction& action, 
																const int& action_id,
																const Core::FMTtransition& transition,
																const Core::FMTyields& ylds,
																const std::vector<Core::FMTtheme>& themes)
	{
		FMTvertex_descriptor activev = getactivevertex();
		const Core::FMTdevelopment& active_development = getdevelopment(activev);
		const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(action, transition, ylds, themes);
		this->addaction(activev,action_id, paths);
		return paths;
	}

	void FMTlinegraph::grow()
	{
		FMTvertex_descriptor active = getactivevertex();
		const Core::FMTdevelopment& active_development = getdevelopment(active);
		const Core::FMTfuturdevelopment grown_up = active_development.grow();
		FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::FMTvertex_descriptor next_period = adddevelopment(grown_up);
		const Graph::FMTedgeproperties newedge(-1, 0, 100);
		boost::add_edge(active, next_period, newedge, data);
		++stats.edges;
	}
	FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::FMTvertex_descriptor FMTlinegraph::getactivevertex() const
	{
		FMTvertex_iterator vertex, vend;
		boost::tie(vertex, vend) = vertices(data);
		--vend;
		const FMTvertex_descriptor active = *vend;
		return active;
	}

	int FMTlinegraph::getlastactionid(const int& period) const
	{
		Exception::FMTexceptionhandler handler;
		std::vector<int> ids;
		for (const auto& devit : getperiodverticies(period))
		{
			const FMTvertex_descriptor& outv = devit.second;
			if (periodstop(outv))
			{
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(outv, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
					const int id = edgeprop.getactionID();
					ids.push_back(id);
				}
			}
		}
		if (ids.size() > 1)
		{
			handler.raise(Exception::FMTexc::FMTnotlinegraph, "More than in egde for last development at period "+std::to_string(period), "FMTlinegraph::getlastactionid()", __LINE__, __FILE__);
		}
		if (ids.empty())
		{
			handler.raise(Exception::FMTexc::FMTnotlinegraph, "No inedge for  " +std::string(*this)+" at period "+ std::to_string(period), "FMTlinegraph::getlastactionid()", __LINE__, __FILE__);
		}
		return ids.at(0);
	}

	Core::FMTdevelopment FMTlinegraph::getperiodstartdev(const int& period) const
	{
		Exception::FMTexceptionhandler handler;
		std::vector<Core::FMTdevelopment>all_period_start_devs;
		for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator devit = developments.at(period).begin();
			devit != developments.at(period).end(); devit++)
		{
			if (periodstart(devit->second))
			{
				const Core::FMTdevelopment& development = data[devit->second].get();
				all_period_start_devs.push_back(development);
			}
		}
		if (all_period_start_devs.size() > 1)
		{
			handler.raise(Exception::FMTexc::FMTnotlinegraph, "More than one development at the beginning of period " + std::to_string(period), "FMTlinegraph::getperiodstartdev()", __LINE__, __FILE__);
		}
		return all_period_start_devs.at(0);
	}

	Core::FMTdevelopment FMTlinegraph::getperiodstopdev(const int & period) const
	{
		Exception::FMTexceptionhandler handler;
		std::vector<Core::FMTdevelopment>all_period_stop_devs;
		for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator devit = developments.at(period).begin();
			devit != developments.at(period).end(); devit++)
		{
			if (periodstop(devit->second))
			{
				const Core::FMTdevelopment& development = data[devit->second].get();
				all_period_stop_devs.push_back(development);
			}
		}
		if (all_period_stop_devs.size() > 1)
		{
			handler.raise(Exception::FMTexc::FMTnotlinegraph, "More than one development at the end of period " + std::to_string(period), "FMTlinegraph::getperiodstopdev()", __LINE__, __FILE__);
		}
		return all_period_stop_devs.at(0);
	}
	

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
				//FIX safunctions with the new small funcionts
                //addaction(action_id, statsdiff, actives, front_vertex, paths);
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

    FMTgraphstats FMTlinegraph::randombuildperiod(const Models::FMTmodel& model,std::queue<FMTvertex_descriptor> actives, std::default_random_engine& generator,
                                            Spatial::FMTeventcontainer& events, const Spatial::FMTcoordinate& localisation)
	{
		FMTgraphstats statsdiff(stats);
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		while (!actives.empty())
        {
			const FMTvertex_descriptor front_vertex = actives.front();
			actives.pop();
			const FMTbasevertexproperties& front_properties = data[front_vertex];
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

    FMTlinegraph FMTlinegraph::partialcopy(const int& period) const
    {
        FMTlinegraph newgraph(*this);
        const FMTgraphstats delstats = newgraph.clearfromperiod(period);
        newgraph.generatedevelopments();
        return newgraph;
    }

    FMTlinegraph FMTlinegraph::perturbgraph(const Models::FMTmodel& model, std::default_random_engine& generator,
                                            Spatial::FMTeventcontainer& events,
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
            newgraph.randombuildperiod(model,actives,generator,events,localisation);
        }

    return newgraph;
    }

	bool FMTlinegraph::isonlygrow() const
		{
		return size() == boost::num_edges(data);
		}

	bool FMTlinegraph::hashforconstraint(size_t& hashvalue, const int&start, const int& stop) const
	{
		FMTvertex_iterator vertex_iterator, vertex_iterator_end;
		boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
		boost::hash_combine(hashvalue, boost::hash<Core::FMTdevelopment>()(getdevelopment(*vertex_iterator)));
		FMTedge_iterator edge_iterator, edge_iterator_end;
		boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
		//the first edge is suppose to be natural evolution so
		int periodcount = 0;
		int cashin = 0;
		while (edge_iterator!=edge_iterator_end)
			{
			if (periodcount<=stop)
				{
				const FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
				const int actionid = edgeprop.getactionID();
				if (periodcount>=start)
					{
					boost::hash_combine(hashvalue, edgeprop.getactionID());
					++cashin;
					}
				if (!(stop == std::numeric_limits<int>::max()) && periodcount == stop)
					{
					return true;
					}
				if (actionid < 0)
				{
					++periodcount;
				}
				}
			++edge_iterator;
			}
		/*for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
		{
			const FMTvertex_descriptor& sourcevertex = boost::source(*edge_iterator, data);
			const int sourceperiod = getdevelopment(sourcevertex).period;
			if (sourceperiod<=stop)
				{
				const FMTedgeproperties& edgeprop = data[*edge_iterator];
				boost::hash_combine(hashvalue, edgeprop.getactionID());
				if (!(stop==std::numeric_limits<int>::max())&&sourceperiod==stop)
					{
					return true;
					}
				
				}
		}*/
		return false;
	}

	


}
