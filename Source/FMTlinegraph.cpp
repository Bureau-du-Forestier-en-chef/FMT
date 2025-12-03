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
#include "FMTeventcontainer.h"


namespace Graph
{
    FMTlinegraph::FMTlinegraph():
		FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>(FMTgraphbuild::nobuild)
    {

    }

    FMTlinegraph::FMTlinegraph(size_t p_LengthReserve):
		FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>(FMTgraphbuild::schedulebuild)
    {
		reserveVerticies(p_LengthReserve);
    }

    FMTlinegraph::FMTlinegraph(const FMTlinegraph& rhs):
		FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>(rhs)
    {

    }

    FMTlinegraph::FMTlinegraph(const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>& rhs):
		FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>(rhs)
    {

    }

    FMTlinegraph& FMTlinegraph::operator = (const FMTlinegraph& rhs)
    {
        if(this!=&rhs)
            {
			FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::operator = (rhs);
            }
        return *this;
    }

    FMTlinegraph& FMTlinegraph::operator = (const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>& rhs)
    {
        if(this!=&rhs)
            {
			FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::operator = (rhs);
            }
        return *this;
    }

	/*void FMTlinegraph::newperiod()
	{
		try {
			size_t activessize = 0;
			FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			for (boost::tie(vertex_iterator, vertex_iterator_end) = developments.back(); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
				++activessize;
				}
			const FMTvertex_iterator firstiterator = vertex_iterator;
			boost::tie(vertex_iterator, vertex_iterator_end) = vertices(data);
			//close the last period
			developments.back() = FMTvertex_pair(developments.back().first, vertex_iterator);
			developments.push_back(FMTvertex_pair(firstiterator, vertex_iterator_end));
			//developments.push_back(boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>());
			rebasecache();
			if (activessize > 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than one active at the begining of the period", "FMTlinegraph::newperiod()", __LINE__, __FILE__);
			}
			if (activessize < 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "0 actives at the begining of the period", "FMTlinegraph::newperiod()", __LINE__, __FILE__);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlinegraph::newperiod", __LINE__, __FILE__);
			}
	}*/

	void FMTlinegraph::setaction(FMTvertex_descriptor active,const int& actionID,
		const std::vector<Core::FMTdevelopmentpath>& paths)
	{
		try {
			for (const Core::FMTdevelopmentpath& devpath : paths)
			{
				const FMTedgeproperties newedge(actionID, 0, devpath.getProportion());
				FMTvertex_descriptor tovertex;
				tovertex = this->adddevelopment(devpath.getDevelopment());
				boost::add_edge(active, tovertex, newedge, data);
				++stats.edges;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::setaction", __LINE__, __FILE__);
		}
	}

	size_t FMTlinegraph::operate(const Core::FMTaction& action, 
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
		return paths.size();
	}

	void FMTlinegraph::grow()
	{
		try {
			FMTvertex_descriptor active = getactivevertex();
			const Core::FMTdevelopment& active_development = getdevelopment(active);
			const Core::FMTfuturdevelopment grown_up = active_development.grow();
			FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>::FMTvertex_descriptor next_period = adddevelopment(grown_up);
			const Graph::FMTbaseedgeproperties newedge(-1);
			boost::add_edge(active, next_period, newedge, data);
			++stats.edges;
			//close The last period
			FMTvertex_iterator vertex, vend,firstof;
			boost::tie(vertex, vend) = boost::vertices(data);
			firstof = vend;
			--firstof;
			
			developments.back() = FMTvertex_pair(developments.back().first, firstof);
			//Open the new period
			developments.push_back(FMTvertex_pair(firstof, vend));
			//nodescache.clear();
			rebasecache();

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

	int FMTlinegraph::getinedgeactionid(const FMTvertex_descriptor& vdesc)const
	{
		std::vector<int> ids;
		try
		{
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vdesc, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
				const int id = edgeprop.getactionID();
				ids.push_back(id);
			}
			if (ids.size() > 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph,
						"More than in egde for development " + std::string(getdevelopment(vdesc)),
						"FMTlinegraph::getinedgeactionid()", __LINE__, __FILE__);
			}
			if (ids.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph,
						"No inedge for  " + std::string(getdevelopment(vdesc)),
						"FMTlinegraph::getinedgeactionid()", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getinedgeactionid()", __LINE__, __FILE__);
		}
		return ids.at(0);

	}

	std::vector<FMTpredictor>FMTlinegraph::getperiodpredictors(const int& period, const Models::FMTmodel& model, const std::vector<std::string>& yieldnames,bool periodonevalues, bool withGCBMid) const
	{
		std::vector<FMTpredictor>allpredictors;
		try {
			FMTvertex_iterator vertexit;
			FMTvertex_iterator vertexend;
			for (boost::tie(vertexit, vertexend) = getperiodverticies(period); vertexit != vertexend; ++vertexit)
			{
				const FMTvertex_descriptor& outv = *vertexit;
				if (periodstop(outv) &&
					(data[outv].get().getperiod() != 0)
					/*!(devit.pointerobject->period==1 && getinedgeactionid(outv)==-1)*/)
				{
					const std::vector<FMTpredictor>devpredictor = FMTgraph::getpredictors(outv, model, yieldnames, 3,periodonevalues,withGCBMid);
					allpredictors.insert(allpredictors.end(), devpredictor.begin(), devpredictor.end());
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getperiodpredictors", __LINE__, __FILE__);
		}
		return allpredictors;
	}


	int FMTlinegraph::getlastactionid(const int& period) const
	{
		//std::vector<int> ids;
		int id = -1;
		int perstcount=0;
		try {
			FMTvertex_iterator vertexit;
			FMTvertex_iterator vertexend;
			for (boost::tie(vertexit, vertexend) = getperiodverticies(period); vertexit != vertexend; ++vertexit)
			{
				const FMTvertex_descriptor& outv = *vertexit;
				if (periodstop(outv))
				{
					perstcount++;
					id = getinedgeactionid(outv);
					/*FMTinedge_iterator inedge_iterator, inedge_end;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(outv, data); inedge_iterator != inedge_end; ++inedge_iterator)
					{
						const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
						const int id = edgeprop.getactionID();
						ids.push_back(id);
					}*/
				}
			}
			if (perstcount > 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than development at the end of period " + std::to_string(period), "FMTlinegraph::getlastactionid()", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getlastactionid", __LINE__, __FILE__);
		}
		//return ids.at(0);
		return id;
	}

	std::vector<int> FMTlinegraph::getperiodactionids(const int& period) const
	{
		std::vector<int> ids;
		try {
			FMTvertex_iterator vertexit;
			FMTvertex_iterator vertexend;
			for (boost::tie(vertexit, vertexend) = getperiodverticies(period); vertexit != vertexend; ++vertexit)
			{
				const FMTvertex_descriptor& outv = *vertexit;
				if (!periodstart(outv))
				{
					ids.push_back(getinedgeactionid(outv));
				}
			}
			if(ids.size()>1)
			{
				std::sort(ids.begin(),ids.end());
				//Because we operate in order of action and its not recursive, so the first action cannot be triger after the last action. But the last action can be triger after the first action.
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getperiodactionids", __LINE__, __FILE__);
		}
		return ids;
	}

	const Core::FMTdevelopment& FMTlinegraph::getperiodstartdev(const int& period) const
	{
		FMTvertex_iterator vertexit;
		try {
			FMTvertex_iterator vertexend;
			for (boost::tie(vertexit, vertexend) = developments.at(period); vertexit != vertexend; ++vertexit)
			{
				if (periodstart(*vertexit))
				{
					return getdevelopment(*vertexit);
				}
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getperiodstartdev", __LINE__, __FILE__);
		}
	return getdevelopment(*vertexit);
	}

	const Core::FMTdevelopment& FMTlinegraph::getperiodstopdev(const int & period) const
	{
		FMTvertex_iterator vertexit;
		try {
			FMTvertex_iterator vertexend;
			for (boost::tie(vertexit, vertexend) = developments.at(period); vertexit != vertexend; ++vertexit)
			{
				if (periodstop(*vertexit))
				{
					return getdevelopment(*vertexit);
				}
			}
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, 
							"The period for which the developement is asked is not complete so there is no period stop developement.", 
							"FMTlinegraph::getperiodstopdev()", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getperiodstopdev", __LINE__, __FILE__);
		}
		return getdevelopment(*vertexit);
	}
	

    int FMTlinegraph::randomoperate(const std::vector<int>& operables, const Models::FMTmodel& model,
                                            FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
                                            const Core::FMTdevelopment& active_development, bool dontchoosegrow)
    {
		int distribution_select = -1;//natural growth
		try {
			const FMTvertex_descriptor vertex = front_vertex;
			front_vertex = boost::graph_traits<FMTadjacency_list>::null_vertex();
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
				const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(model.actions.at(action_id), model.transitions.at(action_id), model.yields, model.themes);
				std::queue<FMTvertex_descriptor>actives;
				addaction(action_id,stats,actives,vertex, paths);
				if (!actives.empty())
					{
					front_vertex = actives.front();
					}
				return action_id;
			}
			else
			{
				grow();
				/*const Core::FMTfuturdevelopment grown_up = active_development.grow();
				FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
				const FMTedgeproperties newedge(-1, 0, 100);
				boost::add_edge(vertex, next_period, newedge, data);
				++stats.edges;
				//close the last element
				FMTvertex_iterator vertex_iterator, vertex_iterator_end, first_vertex;
				boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
				first_vertex = vertex_iterator_end;
				--first_vertex;
				developments.back() = FMTvertex_pair(developments.back().first, first_vertex);
				developments.push_back(FMTvertex_pair(first_vertex, vertex_iterator_end));
				rebasecache();*/
				return -1;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlinegraph::randomoperate", __LINE__, __FILE__);
			}
		return -1;
    }

    std::vector<int> FMTlinegraph::randombuildperiod(const Models::FMTmodel& model, std::default_random_engine& generator,
											boost::unordered_map<Core::FMTdevelopment, std::vector<int>>& operability,bool dontchoosegrow)
	{
		std::vector<int>actioned;
		try {
			/*FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			boost::tie(vertex_iterator, vertex_iterator_end) = vertices(data);
			developments.push_back(FMTvertex_pair(developments.back().second, vertex_iterator_end));
			rebasecache();*/
			Graph::FMTlinegraph::FMTvertex_descriptor active = getactivevertex();
			std::vector<int>operated;
			while (active!= boost::graph_traits<FMTadjacency_list>::null_vertex())
			{
				const FMTbasevertexproperties& front_properties = data[active];
				const Core::FMTdevelopment& active_development = front_properties.get();
				std::vector<int> operables;
				if (operability.find(active_development) != operability.end())
				{
					operables = operability.at(active_development);
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
				operability[active_development] = operables;
				}


				const int selectedaction = randomoperate(operables, model,active, generator, active_development, dontchoosegrow);

				if (selectedaction > -1)
				{
					actioned.push_back(selectedaction);
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlinegraph::randombuildperiod", __LINE__, __FILE__);
			}
        return actioned;
	}

	bool FMTlinegraph::isanyvertexusage(const FMTbasevertexproperties& vertexproperty, const Core::FMToutputsource& source, const Core::FMTyields& yields) const
	{
		try {
			const Core::FMTdevelopment& dev = vertexproperty.get();
			return  (source.use(dev, yields));
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::isanyvertexusage", __LINE__, __FILE__);
		}
		return false;

	}

	std::vector<int> FMTlinegraph::anyusageof(Core::FMToutputnode output_node, const Models::FMTmodel& model,const int& startingperiod) const
	{
		std::vector<int>periods;
		try {
			for (int period = startingperiod; period < getperiod(); ++period)
			{
				const std::vector<FMTvertex_descriptor>verticies = getnode(model, output_node, period);
				if (!verticies.empty())
					{
					periods.push_back(period);
					}
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::anyusageof", __LINE__, __FILE__);
		}
		return periods;
	}


	bool FMTlinegraph::isanyactionofedge(const FMTbaseedgeproperties& edgeproperty, const std::unordered_set<int>& actions) const
	{
		try {
			
			const int id = edgeproperty.getactionID();
			return  (id >=0 && actions.find(id) != actions.end());
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::isanyactionofedge", __LINE__, __FILE__);
		}
		return false;
	}


    FMTgraphstats FMTlinegraph::clearfromperiod(const int& period,bool updatedevelopments)
    {
		try {
			const int basesize = static_cast<int>(this->size() - 1);
			std::vector<FMTvertex_descriptor>removed;
			for (int location = basesize; location >= period; --location)
			{
				
				FMTvertex_iterator vertexit, vertexend;
				for (boost::tie(vertexit, vertexend) = developments.at(location); vertexit != vertexend; ++vertexit)
				{
					const FMTvertex_descriptor vertexm_location = *vertexit;
					if (!(location == period && periodstart(vertexm_location)))
					{
						--stats.edges;
						boost::clear_in_edges(vertexm_location, data);
						removed.push_back(*vertexit);
					}
				}
				
				/*if (updatedevelopments && developments.back().empty())
				{
					developments.pop_back();
				}*/

			}
			std::sort(removed.rbegin(), removed.rend());
			for (FMTvertex_descriptor it : removed)
			{
				boost::remove_vertex(it, data);
				--stats.vertices;
				/*if (updatedevelopments)
				{
					developments.at(location).erase(it);
				}*/
			}
			if (updatedevelopments)
			{
				generatedevelopments();
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
		try {
        const FMTgraphstats delstats = newgraph.clearfromperiod(period);
        newgraph.generatedevelopments();
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::partialcopy", __LINE__, __FILE__);
		}
        return newgraph;
    }

    

	bool FMTlinegraph::isonlygrow(int period) const
		{
		//*_logger << "looking at period " << period << " dev size " << developments.size() << "\n";
		if (period > 0)
			{
			FMTvertex_iterator vertexit, vertexend;
			boost::tie(vertexit, vertexend) = developments.at(period);
			return (std::distance(vertexit, vertexend) == 1);
			}
		return (size()-1) == boost::num_edges(data);
		}

	void FMTlinegraph::clearnodecache()
		{
		nodescache.clear();
		}

	bool FMTlinegraph::ismovable(const std::vector<const Core::FMTaction*>& actions,const Core::FMTyields& yields, const int& period,
		boost::unordered_map<Core::FMTdevelopment,bool>*operability) const
		{
		try{
		const int lastperiod = getperiod();
		for (int localperiod = period; localperiod < lastperiod;++localperiod)
			{
			const Core::FMTdevelopment& startingdev = getperiodstartdev(localperiod);
			bool operable = false;
			if (operability!=nullptr&&operability->find(startingdev)!=operability->end())
				{
				operable = operability->at(startingdev);
			}else {
				if (startingdev.anyoperable(actions, yields))
					{
					operable = true;
					}
				if (operability != nullptr)
					{
					(*operability)[startingdev] = operable;
					}
				}
			if (operable)
				{
				return true;
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::ismovable", __LINE__, __FILE__);
		}
		return false;
		}

	bool FMTlinegraph::operator == (const FMTlinegraph& rhs) const
		{
		if (getbasedevelopment() == rhs.getbasedevelopment())
			{
			const size_t THIS_EDGES = boost::num_edges(data);
			const size_t RHS_EDGES = boost::num_edges(rhs.data);
			if (THIS_EDGES!= RHS_EDGES)
				{
				return false;
				}
			FMTedge_iterator edge_iterator, edge_iterator_end, rhs_edge_iterator, rhs_edge_iterator_end;
			boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
			boost::tie(rhs_edge_iterator, rhs_edge_iterator_end) = boost::edges(rhs.data);
			while (edge_iterator != edge_iterator_end && 
					rhs_edge_iterator != rhs_edge_iterator_end)
				{
				const FMTbaseedgeproperties& THIS_PROPERTIES = data[*edge_iterator];
				const FMTbaseedgeproperties& RHS_PROPERTIES = data[*rhs_edge_iterator];
				if (THIS_PROPERTIES.getShortActionID() !=
					RHS_PROPERTIES.getShortActionID())
					{
					return false;
					}
				++edge_iterator;
				++rhs_edge_iterator;
				}
			return (edge_iterator != edge_iterator_end ||
				rhs_edge_iterator != rhs_edge_iterator_end);
			}
		return false;
		}

	bool FMTlinegraph::operator < (const FMTlinegraph& rhs) const
		{
		const Core::FMTdevelopment& THIS_BASE = getbasedevelopment();
		const Core::FMTdevelopment& RHS_BASE = rhs.getbasedevelopment();
		//strict ordering
		if (THIS_BASE < RHS_BASE)
			return true;
		if (RHS_BASE < THIS_BASE)
			return false;
		const size_t THIS_EDGES = boost::num_edges(data);
		const size_t RHS_EDGES = boost::num_edges(rhs.data);
		if (THIS_EDGES < RHS_EDGES)
			return true;
		if (RHS_EDGES < THIS_EDGES)
			return false;
		FMTedge_iterator edge_iterator, edge_iterator_end, rhs_edge_iterator, rhs_edge_iterator_end;
		boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
		boost::tie(rhs_edge_iterator, rhs_edge_iterator_end) = boost::edges(rhs.data);
		while (edge_iterator != edge_iterator_end &&
			rhs_edge_iterator != rhs_edge_iterator_end)
		{
			const FMTbaseedgeproperties& THIS_PROPERTIES = data[*edge_iterator];
			const FMTbaseedgeproperties& RHS_PROPERTIES = data[*rhs_edge_iterator];
			const int8_t THIS_ACTION = THIS_PROPERTIES.getShortActionID();
			const int8_t RHS_ACTION = RHS_PROPERTIES.getShortActionID();
			if (THIS_ACTION < RHS_ACTION)
				return true;
			if (RHS_ACTION < THIS_ACTION)
				return false;
			++edge_iterator;
			++rhs_edge_iterator;
		}
		return false;
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
		try{
		const Core::FMTdevelopment& development = getbasedevelopment();
		boost::hash_combine(hashvalue, development.getmask().getintersect(dynamicmask));
		boost::hash_combine(hashvalue, development.getage());
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getbasehash", __LINE__, __FILE__);
		}
		return hashvalue;
		}

	std::string FMTlinegraph::getbasestr(const Core::FMTmask& dynamicmask) const
		{
		std::string value;
		try{
		const Core::FMTdevelopment& development = getbasedevelopment();
		value += development.getmask().getintersect(dynamicmask).getbitsstring();
		value += std::to_string(development.getage());
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getbasestr", __LINE__, __FILE__);
		}
		return value;
		}

	void FMTlinegraph::setBaseMask(Core::FMTmask& p_dynamicmask, const int& p_period) const
		{
		const Core::FMTdevelopment& development = getbasedevelopment();
		p_dynamicmask.setIntersect(development.getmask());
		p_dynamicmask.binarizedappend<int8_t>(development.getShortAge());
		filledgesmask(p_dynamicmask, p_period);
		}

	Core::FMTmask FMTlinegraph::getbasemask(const Core::FMTmask& dynamicmask) const
		{
		const Core::FMTdevelopment& development = getbasedevelopment();
		Core::FMTmask mask = development.getmask().getintersect(dynamicmask);
		mask.binarizedappend<int8_t>(development.getShortAge());
		return mask;
		}

	void FMTlinegraph::filledgesmask(Core::FMTmask& mask, const int& maximalperiod) const
		{
		try{
		if (!isonlygrow())
			{
			FMTedge_iterator edge_iterator, edge_iterator_end;
			boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
			int periodcount = 0;
			while (edge_iterator != edge_iterator_end && periodcount <= maximalperiod)
				{
					const FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
					const int8_t ACTION_ID = edgeprop.getShortActionID();
					mask.binarizedappend<int8_t>(ACTION_ID);
					if (ACTION_ID < 0)
					{
						++periodcount;
					}
					++edge_iterator;
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::filledgesmask", __LINE__, __FILE__);
		}
		}


	std::string FMTlinegraph::getedgesstr(const int& maximalperiod, bool& gotthewhole) const
		{
		
		std::string hashstr;
		try {
			const int actperiod = getperiod() - 1;
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
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getedgesstr", __LINE__, __FILE__);
		}
		return hashstr;
		}

	std::vector<std::vector<bool>>FMTlinegraph::getactions(const Models::FMTmodel& model, const int& fromperiod,
		std::map<Core::FMTdevelopment, std::vector<bool>>& operability) const
		{
		std::vector<std::vector<bool>>allactions;
		try{
		if (!isonlygrow())
		{
			FMTedge_iterator edge_iterator, edge_iterator_end;
			boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
			const int lastperiod = getperiod() - 1;
			allactions = std::vector<std::vector<bool>>((lastperiod - fromperiod)+1, std::vector<bool>(model.actions.size(), false));
			while (edge_iterator != edge_iterator_end)
			{
				const FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
				const int actionid = edgeprop.getactionID();
				if (actionid >= 0)
				{
					const FMTvertex_descriptor descriptor = boost::source(*edge_iterator, data);
					const FMTbasevertexproperties& vertexprop = data[descriptor];
					const Core::FMTdevelopment& dev = data[descriptor].get();
					const int period = dev.getperiod();
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
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::getactions", __LINE__, __FILE__);
		}
		return allactions;
		}

	size_t FMTlinegraph::getedgeshash(const int& maximalperiod, bool& gotthewhole) const
		{
		return boost::hash<std::string>{}(getedgesstr(maximalperiod, gotthewhole));
		}

	/*void FMTlinegraph::addfromevents(const Spatial::FMTcoordinate& localisation, const Models::FMTmodel& model, Spatial::FMTeventcontainer& events) const
		{
		try{
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
				events.addaction(localisation, vertexprop.get().getperiod(), actionid);
				}
			++edge_iterator;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::addfromevents", __LINE__, __FILE__);
		}
		}*/

	bool FMTlinegraph::hashforconstraint(size_t& hashvalue,const int& stop, const Core::FMTmask& dynamicmask) const
	{
		bool gotthewholegraph = false;
		try{
		boost::hash_combine(hashvalue,getbasehash(dynamicmask));
		boost::hash_combine(hashvalue,getedgeshash(stop,gotthewholegraph));
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::hashforconstraint", __LINE__, __FILE__);
		}
		return gotthewholegraph;
	}

	bool FMTlinegraph::stringforconstraint(std::string& value, const int& stop, const Core::FMTmask& dynamicmask) const
	{
		bool gotthewholegraph = false;
		try{
		value += getbasestr(dynamicmask);
		value += getedgesstr(stop, gotthewholegraph);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlinegraph::stringforconstraint", __LINE__, __FILE__);
		}
		return gotthewholegraph;
	}

	


}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTlinegraph)
