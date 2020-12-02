/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlinegraph.h"
#include "FMTexceptionhandler.h"
#include "FMTmodel.h"
#include "FMTlogger.h"
#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_oarchive.hpp>


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

	void FMTlinegraph::newperiod()
	{
		try {
			std::queue<FMTvertex_descriptor> actives = getactiveverticies();
			developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
			periodstatsdiff = getstats();
			if (actives.size() > 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than one active at the begining of the period", "FMTlinegraph::newperiod()", __LINE__, __FILE__);
			}
			if (actives.size() < 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "0 actives at the begining of the period", "FMTlinegraph::newperiod()", __LINE__, __FILE__);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlinegraph::newperiod", __LINE__, __FILE__);
			}
	}

	void FMTlinegraph::setaction(FMTvertex_descriptor active,const int& actionID,
		const std::vector<Core::FMTdevelopmentpath>& paths)
	{
		try {
			for (const Core::FMTdevelopmentpath& devpath : paths)
			{
				const FMTedgeproperties newedge(actionID, 0, devpath.proportion);
				FMTvertex_descriptor tovertex;
				tovertex = this->adddevelopment(*devpath.development);
				boost::add_edge(active, tovertex, newedge, data);
				++stats.edges;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::setaction", __LINE__, __FILE__);
		}
	}

	std::vector<Core::FMTdevelopmentpath> FMTlinegraph::operate(const Core::FMTaction& action, 
																const int& action_id,
																const Core::FMTtransition& transition,
																const Core::FMTyields& ylds,
																const std::vector<Core::FMTtheme>& themes)
	{
		std::vector<Core::FMTdevelopmentpath> paths;
		try {
			FMTvertex_descriptor activev = getactivevertex();
			const Core::FMTdevelopment& active_development = getdevelopment(activev);
			paths = active_development.operate(action, transition, ylds, themes);
			setaction(activev, action_id, paths);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::operate", __LINE__, __FILE__);
		}
		return paths;
	}

	void FMTlinegraph::grow()
	{
		try {
			FMTvertex_descriptor active = getactivevertex();
			const Core::FMTdevelopment& active_development = getdevelopment(active);
			const Core::FMTfuturdevelopment grown_up = active_development.grow();
			FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::FMTvertex_descriptor next_period = adddevelopment(grown_up);
			const Graph::FMTedgeproperties newedge(-1, 0, 100);
			boost::add_edge(active, next_period, newedge, data);
			++stats.edges;
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlinegraph::grow", __LINE__, __FILE__);
			}
	}
	FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::FMTvertex_descriptor FMTlinegraph::getactivevertex() const
	{
		FMTvertex_descriptor active;
		try {
			FMTvertex_iterator vertex, vend;
			boost::tie(vertex, vend) = vertices(data);
			--vend;
			active = *vend;
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getactivevertex", __LINE__, __FILE__);
		}
		return active;
	}

	int FMTlinegraph::getlastactionid(const int& period) const
	{
		std::vector<int> ids;
		try {
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
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than in egde for last development at period " + std::to_string(period), "FMTlinegraph::getlastactionid()", __LINE__, __FILE__);
			}
			if (ids.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "No inedge for  " + std::string(*this) + " at period " + std::to_string(period), "FMTlinegraph::getlastactionid()", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getlastactionid", __LINE__, __FILE__);
		}
		return ids.at(0);
	}

	const Core::FMTdevelopment& FMTlinegraph::getperiodstartdev(const int& period) const
	{
		try {
			for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator devit = developments.at(period).begin();
				devit != developments.at(period).end(); devit++)
			{
				if (periodstart(devit->second))
				{
					return data[devit->second].get();
				}
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getperiodstartdev", __LINE__, __FILE__);
		}
	return data[developments.at(period).begin()->second].get();
	}

	const Core::FMTdevelopment& FMTlinegraph::getperiodstopdev(const int & period) const
	{
		try {
			for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator devit = developments.at(period).begin();
				devit != developments.at(period).end(); devit++)
			{
				if (periodstop(devit->second))
				{
					return data[devit->second].get();
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getperiodstopdev", __LINE__, __FILE__);
		}
		return data[developments.at(period).begin()->second].get();
	}
	

    int FMTlinegraph::randomoperate(const std::vector<int>& operables, const Models::FMTmodel& model, std::queue<FMTvertex_descriptor>& actives,
                                            FMTgraphstats& statsdiff, const FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
                                            const Core::FMTdevelopment& active_development, bool dontchoosegrow)
    {
		int distribution_select = -1;//natural growth
		try {
			if (!operables.empty())
			{
				const int size_op = static_cast<int>(operables.size()) - 1;
				//Logging::FMTlogger() << "size of op " << size_op<<" "<< operables.at(0) << "\n";
				int noaction = -(size_op);
				if (!operables.empty() && dontchoosegrow)
				{
					noaction = 0;
				}
				std::uniform_int_distribution<int>distribution(noaction, size_op);//50 50 to actionate something
				distribution_select = distribution(generator);

			}
			if (distribution_select >= 0)//Take care ok _DEATH hereeeeee ... I think it's implicitly done &!&!Validate
			{
				const int action_id = operables.at(distribution_select);
				//Logging::FMTlogger() << "selected " << std::string(model.actions.at(action_id)) << "\n";
				const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(model.actions.at(action_id), model.transitions.at(action_id), model.yields, model.themes);
				//FIX safunctions with the new small funcionts
				//addaction(action_id, statsdiff, actives, front_vertex, paths);
				addaction(action_id, statsdiff, actives, front_vertex, paths);
				//addaction(front_vertex, action_id, paths);
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
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlinegraph::randomoperate", __LINE__, __FILE__);
			}
		return -1;
    }

    FMTgraphstats FMTlinegraph::randombuildperiod(const Models::FMTmodel& model,std::queue<FMTvertex_descriptor> actives, std::default_random_engine& generator,
                                            Spatial::FMTeventcontainer& events, const Spatial::FMTcoordinate& localisation,
											std::unordered_map<size_t, std::vector<int>>* operability,
											const std::vector<Spatial::FMTbindingspatialaction>* bindings,bool dontchoosegrow)
	{
		FMTgraphstats statsdiff(stats);
		try {
			developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
			while (!actives.empty())
			{
				const FMTvertex_descriptor front_vertex = actives.front();
				actives.pop();
				const FMTbasevertexproperties& front_properties = data[front_vertex];
				const Core::FMTdevelopment& active_development = front_properties.get();
				const size_t devhash = active_development.hash();
				std::vector<int> operables;
				if (operability != nullptr && operability->find(devhash) != operability->end())
				{
					operables = operability->at(devhash);
				}
				else {
					int action_id = 0;
					for (const Core::FMTaction& action : model.actions)
					{
						if (active_development.operable(action, model.yields))
						{
							operables.push_back(action_id);
						}
						++action_id;
					}
					if (operability != nullptr)
					{
						(*operability)[devhash] = operables;
					}

				}


				const int selectedaction = randomoperate(operables, model, actives, statsdiff, front_vertex, generator, active_development, dontchoosegrow);

				if (selectedaction > -1)
				{
					//deleted need to be removed from spatial infeasibility
					//changes need to be added to spatial infeasibility

					if (bindings != nullptr)
					{
						events.addaction(localisation, active_development.period, selectedaction, bindings->at(selectedaction).getmaximalsize());
					}
					else {
						events.addaction(localisation, active_development.period, selectedaction);
					}

				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlinegraph::randombuildperiod", __LINE__, __FILE__);
			}
        return (statsdiff - stats);
	}

    FMTgraphstats FMTlinegraph::clearfromperiod(const int& period,bool updatedevelopments)
    {
		try {
			const int basesize = static_cast<int>(this->size() - 1);
			for (int location = basesize; location >= period; --location)
			{
				std::vector<std::unordered_map<size_t, FMTvertex_descriptor>::iterator>removed;
				for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = developments.at(location).begin();
					it != developments.at(location).end(); it++)
				{
					FMTvertex_descriptor vertex_location = it->second;
					if (!(location == period && periodstart(vertex_location)))
					{
						--stats.edges;
						boost::clear_in_edges(vertex_location, data);
						removed.push_back(it);
					}
				}
				for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it : removed)
				{
					boost::remove_vertex(it->second, data);
					--stats.vertices;
					if (updatedevelopments)
					{
						developments.at(location).erase(it);
					}
				}
				if (updatedevelopments && developments.back().empty())
				{
					developments.pop_back();
				}

			}
			if (updatedevelopments)
			{
				nodescache.clear();
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlinegraph::clearfromperiod", __LINE__, __FILE__);
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
        //events.erasecoordinate(localisation,period);
		std::map<Core::FMTdevelopment,std::vector<bool>>tabuoperability;
		const std::vector<std::vector<bool>>actions = getactions(model, period,tabuoperability);
		std::unordered_map<size_t,std::vector<int>>operability;
		bool dontbuildgrowth = false;
		if (!actions.empty())
			{
			//Logging::FMTlogger() << "got action! " << "\n";
			for (std::map<Core::FMTdevelopment, std::vector<bool>>::const_iterator it = tabuoperability.begin(); it != tabuoperability.end(); it++)
			{
				int actionid = 0;
				operability[it->first.hash()] = std::vector<int>();
				//Logging::FMTlogger() << "setting empty vector "  << "\n";
				for (const Core::FMTaction& action : model.actions)
				{
					if ((!it->second.at(actionid)) && it->first.operable(action, model.yields))
					{
						operability[it->first.hash()].push_back(actionid);
						//Logging::FMTlogger() << "can do " << action.getname() << " at period " << it->first.period << "\n";
						
					}
					++actionid;
				}
			///poor thing it can only do one action so grow!
			}
			events.erasecoordinate(localisation, period,actions);
			//Remove the contribution to the infeasibility of the erased and add the contribution of the changes...

		}else {
			//Logging::FMTlogger() << "got gorwth" << "\n";
			dontbuildgrowth = true;
		}
		


        FMTlinegraph newgraph = partialcopy(period);
        newgraph.nodescache.clear();
        while(this->size() != newgraph.size())
        {
            std::queue<FMTvertex_descriptor> actives = newgraph.getactiveverticies();
            newgraph.randombuildperiod(model,actives,generator,events,localisation,&operability,nullptr, dontbuildgrowth);
        }

    return newgraph;
    }

	bool FMTlinegraph::isonlygrow() const
		{
		return (size()-1) == boost::num_edges(data);
		}

	void FMTlinegraph::clearnodecache()
		{
		nodescache.clear();
		}

	bool FMTlinegraph::ismovable(const std::vector<const Core::FMTaction*>& actions,const Core::FMTyields& yields, const int& period,
		std::unordered_map<size_t,bool>*operability) const
		{
		const int lastperiod = getperiod();
		for (int localperiod = period; localperiod < lastperiod;++localperiod)
			{
			const Core::FMTdevelopment& startingdev = getperiodstartdev(localperiod);
			const size_t devhash = startingdev.hash();
			bool operable = false;
			if (operability!=nullptr&&operability->find(devhash)!=operability->end())
				{
				operable = operability->at(devhash);
			}else {
				if (startingdev.anyoperable(actions, yields))
					{
					operable = true;
					}
				if (operability != nullptr)
					{
					(*operability)[devhash] = operable;
					}
				}
			if (operable)
				{
				return true;
				}
			}
		return false;
		}

	bool FMTlinegraph::operator == (const FMTlinegraph& rhs) const
		{
		return FMTgraph::operator==(rhs);
		}

	const Core::FMTdevelopment& FMTlinegraph::getbasedevelopment() const
		{
		FMTvertex_iterator vertex_iterator, vertex_iterator_end;
		boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
		return getdevelopment(*vertex_iterator);
		}

	size_t FMTlinegraph::getbasehash(const Core::FMTmask& dynamicmask) const
		{
		size_t hashvalue = 0;
		const Core::FMTdevelopment& development = getbasedevelopment();
		boost::hash_combine(hashvalue, development.mask.getintersect(dynamicmask));
		boost::hash_combine(hashvalue, development.age);
		return hashvalue;
		}

	std::vector<std::vector<bool>>FMTlinegraph::getactions(const Models::FMTmodel& model, const int& fromperiod,
		std::map<Core::FMTdevelopment, std::vector<bool>>& operability) const
		{
		std::vector<std::vector<bool>>allactions;
		if (!isonlygrow())
		{
			FMTedge_iterator edge_iterator, edge_iterator_end;
			boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
			const int lastperiod = getperiod() - 1;
			allactions = std::vector<std::vector<bool>>((lastperiod - fromperiod)+1, std::vector<bool>(model.actions.size(), false));
			//Logging::FMTlogger() << "size " << allactions.size() << "\n";
			while (edge_iterator != edge_iterator_end)
			{
				const FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
				const int actionid = edgeprop.getactionID();
				if (actionid >= 0)
				{
					const FMTvertex_descriptor descriptor = boost::source(*edge_iterator, data);
					const FMTbasevertexproperties& vertexprop = data[descriptor];
					const Core::FMTdevelopment& dev = data[descriptor].get();
					const int& period = dev.period;
					if (period>=fromperiod&&period<=lastperiod)
						{
						if (operability.find(dev)== operability.end())
							{
							operability[dev] = std::vector<bool>(model.actions.size(),false);
							}
						operability[dev][actionid] = true;
						allactions[period - fromperiod][actionid] = true;
						}
				}
				++edge_iterator;
			}
		for (std::vector<bool>& actions : allactions)
			{
			std::vector<bool>::const_iterator it = actions.begin();
			while (it!=actions.end()&&(!*it))
				{
				++it;
				}
			if (it == actions.end())
				{
				actions.clear();
				}
			}
		}
		
		return allactions;
		}

	size_t FMTlinegraph::getedgeshash(const int& maximalperiod, bool& gotthewhole) const
		{
		const int actperiod = getperiod() - 1;
		std::string hashstr;
		if (!isonlygrow())
		{
			FMTedge_iterator edge_iterator, edge_iterator_end;
			boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
			int periodcount = 0;
			while (edge_iterator != edge_iterator_end && periodcount <= maximalperiod)
			{
				const FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
				const int actionid = edgeprop.getactionID();
				hashstr += std::to_string(actionid);
				if (actionid < 0)
				{
					++periodcount;
				}
				++edge_iterator;
			}

		}
		else {
			for (int period = 0; period <= std::min(actperiod, maximalperiod); ++period)
			{
				hashstr += "-1";
			}
		}
		gotthewhole = (maximalperiod <= actperiod);
		return boost::hash<std::string>{}(hashstr);
		}

	void FMTlinegraph::addfromevents(const Spatial::FMTcoordinate& localisation, const Models::FMTmodel& model, Spatial::FMTeventcontainer& events) const
		{
		FMTedge_iterator edge_iterator,edge_iterator_end;
		boost::tie(edge_iterator,edge_iterator_end) = boost::edges(data);
		while (edge_iterator!= edge_iterator_end)
			{
			const FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
			const int actionid = edgeprop.getactionID();
			if (actionid>=0)
				{
				const FMTvertex_descriptor descriptor = boost::source(*edge_iterator,data);
				const FMTbasevertexproperties& vertexprop = data[descriptor];
				events.addaction(localisation, vertexprop.get().period, actionid);
				}
			++edge_iterator;
			}

		}

	bool FMTlinegraph::hashforconstraint(size_t& hashvalue,const int& stop, const Core::FMTmask& dynamicmask) const
	{
		boost::hash_combine(hashvalue,getbasehash(dynamicmask));
		bool gotthewholegraph = false;
		boost::hash_combine(hashvalue,getedgeshash(stop,gotthewholegraph));
		return gotthewholegraph;
	}

	


}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTlinegraph)
