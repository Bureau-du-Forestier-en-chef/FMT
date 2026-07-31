/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTLineGraph.h"
#include "FMTExceptionHandler.h"
#include "FMTModel.h"
#include "FMTLogger.h"
#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "FMTEventContainer.h"


namespace Graph
{
    FMTLineGraph::FMTLineGraph():
		FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>(FMTgraphbuild::nobuild)
    {

    }

    FMTLineGraph::FMTLineGraph(size_t p_LengthReserve):
		FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>(FMTgraphbuild::schedulebuild)
    {
		reserveVertices(p_LengthReserve);
    }

	FMTLineGraph::FMTLineGraph(
		size_t p_LengthReserve, const Core::FMTActualDevelopment& p_dev):
		FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>(FMTgraphbuild::schedulebuild)
	{
		reserveVertices(p_LengthReserve);
		const std::vector<Core::FMTActualDevelopment> BASES(1, p_dev);
		initialize(BASES);
	}


    FMTLineGraph::FMTLineGraph(const FMTLineGraph& rhs):
		FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>(rhs)
    {

    }

    FMTLineGraph::FMTLineGraph(const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>& rhs):
		FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>(rhs)
    {

    }

    FMTLineGraph& FMTLineGraph::operator = (const FMTLineGraph& rhs)
    {
        if(this!=&rhs)
            {
			FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>::operator = (rhs);
            }
        return *this;
    }

    FMTLineGraph& FMTLineGraph::operator = (const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>& rhs)
    {
        if(this!=&rhs)
            {
			FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>::operator = (rhs);
            }
        return *this;
    }

	/*void FMTLineGraph::newPeriod()
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
			//developments.push_back(boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor,Core::FMTDevelopment>>());
			rebaseCache();
			if (activessize > 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than one active at the begining of the period", "FMTLineGraph::newPeriod()", __LINE__, __FILE__);
			}
			if (activessize < 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "0 actives at the begining of the period", "FMTLineGraph::newPeriod()", __LINE__, __FILE__);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTLineGraph::newPeriod", __LINE__, __FILE__);
			}
	}*/

	void FMTLineGraph::setAction(FMTvertex_descriptor active,const int& actionID,
		const std::vector<Core::FMTDevelopmentPath>& paths)
	{
		try {
			for (const Core::FMTDevelopmentPath& devpath : paths)
			{
				const FMTEdgeProperties newEdge(actionID, 0, devpath.getProportion());
				FMTvertex_descriptor tovertex;
				tovertex = this->addDevelopment(devpath.getDevelopment());
				boost::add_edge(active, tovertex, newEdge, data);
				++stats.edges;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::setAction", __LINE__, __FILE__);
		}
	}

	size_t FMTLineGraph::operate(const Core::FMTAction& action, 
																const int& action_id,
																const Core::FMTTransition& transition,
																const Core::FMTYields& ylds,
																const std::vector<Core::FMTTheme>& themes)
	{
		std::vector<Core::FMTDevelopmentPath> paths;
		try {
			FMTvertex_descriptor activev = getActiveVertex();
			const Core::FMTDevelopment& active_development = getDevelopment(activev);
			paths = active_development.operate(action, transition, ylds, themes);
			setAction(activev, action_id, paths);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::operate", __LINE__, __FILE__);
		}
		return paths.size();
	}

	void FMTLineGraph::grow(int p_Length)
	{
		try {
			while (p_Length>0)
			{
				FMTvertex_descriptor active = getActiveVertex();
				const Core::FMTDevelopment& active_development = getDevelopment(active);
				const Core::FMTFuturDevelopment grown_up = active_development.grow();
				FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>::FMTvertex_descriptor next_period = addDevelopment(grown_up);
				const Graph::FMTBaseEdgeProperties newEdge(-1);
				boost::add_edge(active, next_period, newEdge, data);
				++stats.edges;
				//close The last period
				FMTvertex_iterator vertex, vend, firstof;
				boost::tie(vertex, vend) = boost::vertices(data);
				firstof = vend;
				--firstof;

				developments.back() = FMTvertex_pair(developments.back().first, firstof);
				//Open the new period
				developments.push_back(FMTvertex_pair(firstof, vend));
				//nodescache.clear();
				rebaseCache();
				--p_Length;
			}
			

		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLineGraph::grow", __LINE__, __FILE__);
			}
	}
	FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>::FMTvertex_descriptor FMTLineGraph::getActiveVertex() const
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
			_exhandler->raiseFromCatch("", "FMTLineGraph::getActiveVertex", __LINE__, __FILE__);
		}
		return active;
	}

	int FMTLineGraph::getInEdgeActionId(const FMTvertex_descriptor& vdesc)const
	{
		std::vector<int> ids;
		try
		{
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vdesc, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTBaseEdgeProperties& edgeprop = data[*inedge_iterator];
				const int id = edgeprop.getActionID();
				ids.push_back(id);
			}
			if (ids.size() > 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph,
						"More than in egde for development " + std::string(getDevelopment(vdesc)),
						"FMTLineGraph::getInEdgeActionId()", __LINE__, __FILE__);
			}
			if (ids.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph,
						"No inedge for  " + std::string(getDevelopment(vdesc)),
						"FMTLineGraph::getInEdgeActionId()", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::getInEdgeActionId()", __LINE__, __FILE__);
		}
		return ids.at(0);

	}

	std::vector<FMTPredictor>FMTLineGraph::getPeriodPredictors(const int& period, const Models::FMTModel& model, const std::vector<std::string>& yieldnames,bool periodonevalues, bool withGCBMid) const
	{
		std::vector<FMTPredictor>allpredictors;
		try {
			FMTvertex_iterator vertexit;
			FMTvertex_iterator vertexend;
			for (boost::tie(vertexit, vertexend) = getPeriodVertices(period); vertexit != vertexend; ++vertexit)
			{
				const FMTvertex_descriptor& outv = *vertexit;
				if (periodStop(outv) &&
					(data[outv].get().getPeriod() != 0)
					/*!(devit.pointerobject->period==1 && getInEdgeActionId(outv)==-1)*/)
				{
					const std::vector<FMTPredictor>devpredictor = FMTGraph::getPredictors(outv, model, yieldnames, 3,periodonevalues,withGCBMid);
					allpredictors.insert(allpredictors.end(), devpredictor.begin(), devpredictor.end());
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::getPeriodPredictors", __LINE__, __FILE__);
		}
		return allpredictors;
	}


	int FMTLineGraph::getLastActionId(const int& period) const
	{
		//std::vector<int> ids;
		int id = -1;
		int perstcount=0;
		try {
			FMTvertex_iterator vertexit;
			FMTvertex_iterator vertexend;
			for (boost::tie(vertexit, vertexend) = getPeriodVertices(period); vertexit != vertexend; ++vertexit)
			{
				const FMTvertex_descriptor& outv = *vertexit;
				if (periodStop(outv))
				{
					perstcount++;
					id = getInEdgeActionId(outv);
					/*FMTinedge_iterator inedge_iterator, inedge_end;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(outv, data); inedge_iterator != inedge_end; ++inedge_iterator)
					{
						const FMTBaseEdgeProperties& edgeprop = data[*inedge_iterator];
						const int id = edgeprop.getActionID();
						ids.push_back(id);
					}*/
				}
			}
			if (perstcount > 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than development at the end of period " + std::to_string(period), "FMTLineGraph::getLastActionId()", __LINE__, __FILE__);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::getLastActionId", __LINE__, __FILE__);
		}
		//return ids.at(0);
		return id;
	}

	std::vector<int> FMTLineGraph::getPeriodActionIds(const int& period, bool p_ordered) const
	{
		std::vector<int> ids;
		try {
			FMTvertex_iterator vertexit;
			FMTvertex_iterator vertexend;
			for (boost::tie(vertexit, vertexend) = getPeriodVertices(period); vertexit != vertexend; ++vertexit)
			{
				const FMTvertex_descriptor& outv = *vertexit;
				if (!periodStart(outv))
				{
					ids.push_back(getInEdgeActionId(outv));
				}
			}
			if(ids.size()>1 && p_ordered)
			{
				std::sort(ids.begin(),ids.end());
				//Because we operate in order of action and its not recursive, so the first action cannot be triger after the last action. But the last action can be triger after the first action.
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::getPeriodActionIds", __LINE__, __FILE__);
		}
		return ids;
	}

	const Core::FMTDevelopment& FMTLineGraph::getPeriodStartDev(const int& period) const
	{
		FMTvertex_iterator vertexit;
		try {
			FMTvertex_iterator vertexend;
			for (boost::tie(vertexit, vertexend) = developments.at(period); vertexit != vertexend; ++vertexit)
			{
				if (periodStart(*vertexit))
				{
					return getDevelopment(*vertexit);
				}
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::getPeriodStartDev", __LINE__, __FILE__);
		}
	return getDevelopment(*vertexit);
	}

	const Core::FMTDevelopment& FMTLineGraph::getPeriodStopDev(const int & period) const
	{
		FMTvertex_iterator vertexit;
		try {
			FMTvertex_iterator vertexend;
			for (boost::tie(vertexit, vertexend) = developments.at(period); vertexit != vertexend; ++vertexit)
			{
				if (periodStop(*vertexit))
				{
					return getDevelopment(*vertexit);
				}
			}
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, 
							"The period for which the developement is asked is not complete so there is no period stop developement.", 
							"FMTLineGraph::getPeriodStopDev()", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::getPeriodStopDev", __LINE__, __FILE__);
		}
		return getDevelopment(*vertexit);
	}


    int FMTLineGraph::_randomOperate(const std::vector<int>& operables, const Models::FMTModel& model,
                                            FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
                                            const Core::FMTDevelopment& active_development, bool dontchoosegrow)
    {
		int distribution_select = -1;//natural growth
		try {
			const FMTvertex_descriptor vertex = front_vertex;
			front_vertex = boost::graph_traits<FMTadjacency_list>::null_vertex();
			if (!operables.empty())
			{
				const int size_op = static_cast<int>(operables.size()) - 1;
				//Logging::FMTLogger() << "size of op " << size_op<<" "<< operables.at(0) << "\n";
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
				const std::vector<Core::FMTDevelopmentPath> paths = active_development.operate(model.actions.at(action_id), model.transitions.at(action_id), model.yields, model.themes);
				std::queue<FMTvertex_descriptor>actives;
				addAction(action_id,stats,actives,vertex, paths);
				if (!actives.empty() && model.actions.at(action_id).getName() != "_DEATH")
					{
					front_vertex = actives.front();
					}
				return action_id;
			}
			else
			{
				grow();
				return -1;
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLineGraph::_randomOperate", __LINE__, __FILE__);
			}
		return -1;
    }

	const std::vector<int>& FMTLineGraph::getSetOperability(
		const Core::FMTDevelopment& p_development,
		const Models::FMTModel& p_model,
		boost::unordered_map<Core::FMTDevelopment, std::vector<int>>& p_Cache)
	{
		boost::unordered_map<Core::FMTDevelopment, std::vector<int>>::iterator OpIt = p_Cache.end();
		try {
			OpIt = p_Cache.find(p_development);
			if (OpIt == p_Cache.end())
				{
				std::vector<int> operables;
				int action_id = 0;
				for (const Core::FMTAction& action : p_model.actions)
				{
					if (p_development.operable(action, p_model.yields))
					{
						operables.push_back(action_id);
					}
					++action_id;
				}
				OpIt = p_Cache.insert(
					std::pair<Core::FMTDevelopment, std::vector<int>>(p_development, operables)).first;
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch(
				"", "FMTLineGraph::getSetOperability", __LINE__, __FILE__);
			}
		return OpIt->second;
	}

    std::vector<int> FMTLineGraph::randomBuildPeriod(const Models::FMTModel& model, std::default_random_engine& generator,
											boost::unordered_map<Core::FMTDevelopment, std::vector<int>>& operability,bool dontchoosegrow)
	{
		std::vector<int>actioned;
		try {
			Graph::FMTLineGraph::FMTvertex_descriptor active = getActiveVertex();
			while (active!= boost::graph_traits<FMTadjacency_list>::null_vertex())
			{
					const FMTBaseVertexProperties& front_properties = data[active];
					const Core::FMTDevelopment& active_development = front_properties.get();
					const std::vector<int>& DEV_OP = getSetOperability(active_development, model, operability);

					const int SELECTED_ACTION = _randomOperate(DEV_OP, model,
							active, generator, active_development, dontchoosegrow);

						if (SELECTED_ACTION > -1)
						{
							actioned.push_back(SELECTED_ACTION);
						}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLineGraph::randomBuildPeriod", __LINE__, __FILE__);
			}
        return actioned;
	}

	bool FMTLineGraph::isAnyVertexUsage(const FMTBaseVertexProperties& vertexproperty, const Core::FMTOutputSource& source, const Core::FMTYields& yields) const
	{
		try {
			const Core::FMTDevelopment& dev = vertexproperty.get();
			return  (source.use(dev, yields));
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::isAnyVertexUsage", __LINE__, __FILE__);
		}
		return false;

	}

	std::vector<int> FMTLineGraph::anyUsageOf(Core::FMTOutputNode output_node, const Models::FMTModel& model,const int& startingperiod) const
	{
		std::vector<int>periods;
		try {
			for (int period = startingperiod; period < getPeriod(); ++period)
			{
				const std::vector<FMTvertex_descriptor>vertices = getNode(model, output_node, period);
				if (!vertices.empty())
					{
					periods.push_back(period);
					}
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::anyUsageOf", __LINE__, __FILE__);
		}
		return periods;
	}


	bool FMTLineGraph::isAnyActionOfEdge(const FMTBaseEdgeProperties& edgeproperty, const std::unordered_set<int>& actions) const
	{
		try {
			
			const int id = edgeproperty.getActionID();
			return  (id >=0 && actions.find(id) != actions.end());
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::isAnyActionOfEdge", __LINE__, __FILE__);
		}
		return false;
	}



    

	bool FMTLineGraph::isOnlyGrow(int period) const
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

	void FMTLineGraph::clearNodeCache()
		{
		nodescache.clear();
		}

	bool FMTLineGraph::isMovable(const Models::FMTModel& p_model, const int& period,
		boost::unordered_map<Core::FMTDevelopment, std::vector<int>>& p_operability) const
		{
		try{
			const int lastPeriod = getPeriod();
			for (int localperiod = period; localperiod < lastPeriod;++localperiod)
				{
				const Core::FMTDevelopment& startingdev = getPeriodStartDev(localperiod);
				const std::vector<int>& DEV_OP = getSetOperability(startingdev, 
																p_model, p_operability);
				return !DEV_OP.empty();
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLineGraph::isMovable", __LINE__, __FILE__);
			}
		return false;
		}

	bool FMTLineGraph::isSameBase(const FMTLineGraph& rhs) const
	{
		return (getBaseDevelopment() == rhs.getBaseDevelopment());
	}

	bool FMTLineGraph::isNotDead(FMTvertex_descriptor p_Descriptor,int8_t p_DeathId) const
		{
		bool returned = true;
		/*if (boost::num_vertices(data) != size())//to be death you need more then just grow
			{
			const FMTBaseVertexProperties& VERTEX_PROPERTIES = data[p_Descriptor];
			const Core::FMTDevelopment& ACTIVE = VERTEX_PROPERTIES.get();
			FMTvertex_iterator VertexIt,VertexEnd;
			boost::tie(VertexIt, VertexEnd) = boost::vertices(data);
			if (std::distance(developments.at(ACTIVE.getPeriod()).first, VertexEnd)>1)
				{*/
					FMTinedge_iterator inedge_iterator, inedge_end;
					boost::tie(inedge_iterator, inedge_end) = boost::in_edges(p_Descriptor, data);
					if (inedge_iterator != inedge_end)
					{
						const FMTBaseEdgeProperties& EDGE_PROPERTIES = data[*inedge_iterator];
						returned = (EDGE_PROPERTIES.getShortActionID() < p_DeathId);
					}
			/* }
			}*/
		return returned;
		}

	bool  FMTLineGraph::isLessPeriod(const FMTLineGraph& rhs) const
	{
		if (isSameBase(rhs) &&
			rhs.getPeriod() == getPeriod()-1)
		{
			FMTedge_iterator edge_iterator, edge_iterator_end, rhs_edge_iterator, rhs_edge_iterator_end;
			boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
			boost::tie(rhs_edge_iterator, rhs_edge_iterator_end) = boost::edges(rhs.data);
			while (edge_iterator != edge_iterator_end &&
				rhs_edge_iterator != rhs_edge_iterator_end)
			{
				const FMTBaseEdgeProperties& THIS_PROPERTIES = data[*edge_iterator];
				const FMTBaseEdgeProperties& RHS_PROPERTIES = data[*rhs_edge_iterator];
				if (THIS_PROPERTIES.getShortActionID() !=
					RHS_PROPERTIES.getShortActionID())
				{
					return false;
				}
				++edge_iterator;
				++rhs_edge_iterator;
			}
			return true;
		}
		return false;
	}

	bool FMTLineGraph::operator == (const FMTLineGraph& rhs) const
		{
		if (isSameBase(rhs))
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
				const FMTBaseEdgeProperties& THIS_PROPERTIES = data[*edge_iterator];
				const FMTBaseEdgeProperties& RHS_PROPERTIES = data[*rhs_edge_iterator];
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

	bool FMTLineGraph::operator < (const FMTLineGraph& rhs) const
		{
		const Core::FMTDevelopment& THIS_BASE = getBaseDevelopment();
		const Core::FMTDevelopment& RHS_BASE = rhs.getBaseDevelopment();
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
			const FMTBaseEdgeProperties& THIS_PROPERTIES = data[*edge_iterator];
			const FMTBaseEdgeProperties& RHS_PROPERTIES = data[*rhs_edge_iterator];
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

	const Core::FMTDevelopment& FMTLineGraph::getBaseDevelopment() const
		{
		FMTvertex_iterator vertex_iterator, vertex_iterator_end;
		boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
		return getDevelopment(*vertex_iterator);
		}

	size_t FMTLineGraph::getBaseHash(const Core::FMTMask& dynamicmask) const
		{
		size_t hashvalue = 0;
		try{
		const Core::FMTDevelopment& development = getBaseDevelopment();
		boost::hash_combine(hashvalue, development.getMask().getIntersect(dynamicmask));
		boost::hash_combine(hashvalue, development.getAge());
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::getBaseHash", __LINE__, __FILE__);
		}
		return hashvalue;
		}

	std::string FMTLineGraph::getBaseStr(const Core::FMTMask& dynamicmask) const
		{
		std::string value;
		try{
		const Core::FMTDevelopment& development = getBaseDevelopment();
		value += development.getMask().getIntersect(dynamicmask).getBitsString();
		value += std::to_string(development.getAge());
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::getBaseStr", __LINE__, __FILE__);
		}
		return value;
		}

	void FMTLineGraph::setBaseMask(Core::FMTMask& p_dynamicmask, const int& p_period) const
		{
		const Core::FMTDevelopment& development = getBaseDevelopment();
		p_dynamicmask.setIntersect(development.getMask());
		p_dynamicmask.binarizedAppend<int8_t>(development.getShortAge());
		fillEdgesMask(p_dynamicmask, p_period);
		}

	Core::FMTMask FMTLineGraph::getBaseMask(const Core::FMTMask& dynamicmask) const
		{
		const Core::FMTDevelopment& development = getBaseDevelopment();
		Core::FMTMask mask = development.getMask().getIntersect(dynamicmask);
		mask.binarizedAppend<int8_t>(development.getShortAge());
		return mask;
		}

	void FMTLineGraph::fillEdgesMask(Core::FMTMask& mask, const int& maximalperiod) const
		{
		try{
		if (!isOnlyGrow())
			{
			FMTedge_iterator edge_iterator, edge_iterator_end;
			boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
			int periodcount = 0;
			while (edge_iterator != edge_iterator_end && periodcount <= maximalperiod)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*edge_iterator];
					const int8_t ACTION_ID = edgeprop.getShortActionID();
					mask.binarizedAppend<int8_t>(ACTION_ID);
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
			_exhandler->raiseFromCatch("", "FMTLineGraph::fillEdgesMask", __LINE__, __FILE__);
		}
		}


	std::string FMTLineGraph::getEdgesStr(const int& maximalperiod, bool& gotthewhole) const
		{
		
		std::string hashstr;
		try {
			const int actPeriod = getPeriod() - 1;
		if (!isOnlyGrow())
		{
			FMTedge_iterator edge_iterator, edge_iterator_end;
			boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
			int periodcount = 0;
			while (edge_iterator != edge_iterator_end && periodcount <= maximalperiod)
			{
				const FMTBaseEdgeProperties& edgeprop = data[*edge_iterator];
				const int actionid = edgeprop.getActionID();
				hashstr += std::to_string(actionid);
				if (actionid < 0)
				{
					++periodcount;
				}
				++edge_iterator;
			}

		}
		else {
			for (int period = 0; period <= std::min(actPeriod, maximalperiod); ++period)
			{
				hashstr += "-1";
			}
		}
		gotthewhole = (maximalperiod <= actPeriod);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::getEdgesStr", __LINE__, __FILE__);
		}
		return hashstr;
		}

	std::vector<std::vector<bool>>FMTLineGraph::getActions(const Models::FMTModel& model, const int& fromperiod,
		std::map<Core::FMTDevelopment, std::vector<bool>>& operability) const
		{
		std::vector<std::vector<bool>>allactions;
		try{
		if (!isOnlyGrow())
		{
			FMTedge_iterator edge_iterator, edge_iterator_end;
			boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data);
			const int lastPeriod = getPeriod() - 1;
			allactions = std::vector<std::vector<bool>>((lastPeriod - fromperiod)+1, std::vector<bool>(model.actions.size(), false));
			while (edge_iterator != edge_iterator_end)
			{
				const FMTBaseEdgeProperties& edgeprop = data[*edge_iterator];
				const int actionid = edgeprop.getActionID();
				if (actionid >= 0)
				{
					const FMTvertex_descriptor descriptor = boost::source(*edge_iterator, data);
					const FMTBaseVertexProperties& vertexprop = data[descriptor];
					const Core::FMTDevelopment& dev = data[descriptor].get();
					const int period = dev.getPeriod();
					if (period>=fromperiod&&period<=lastPeriod)
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
			_exhandler->raiseFromCatch("", "FMTLineGraph::getActions", __LINE__, __FILE__);
		}
		return allactions;
		}

	size_t FMTLineGraph::getEdgesHash(const int& maximalperiod, bool& gotthewhole) const
		{
		return boost::hash<std::string>{}(getEdgesStr(maximalperiod, gotthewhole));
		}

	/*void FMTLineGraph::addFromEvents(const Spatial::FMTCoordinate& localisation, const Models::FMTModel& model, Spatial::FMTEventContainer& events) const
		{
		try{
		FMTedge_iterator edge_iterator,edge_iterator_end;
		boost::tie(edge_iterator,edge_iterator_end) = boost::edges(data);
		while (edge_iterator!= edge_iterator_end)
			{
			const FMTBaseEdgeProperties& edgeprop = data[*edge_iterator];
			const int actionid = edgeprop.getActionID();
			if (actionid>=0)
				{
				const FMTvertex_descriptor descriptor = boost::source(*edge_iterator,data);
				const FMTBaseVertexProperties& vertexprop = data[descriptor];
				events.addAction(localisation, vertexprop.get().getPeriod(), actionid);
				}
			++edge_iterator;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTLineGraph::addFromEvents", __LINE__, __FILE__);
		}
		}*/

	bool FMTLineGraph::hashForConstraint(size_t& hashvalue,const int& stop, const Core::FMTMask& dynamicmask) const
	{
		bool gotthewholegraph = false;
		try{
		boost::hash_combine(hashvalue,getBaseHash(dynamicmask));
		boost::hash_combine(hashvalue,getEdgesHash(stop,gotthewholegraph));
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::hashForConstraint", __LINE__, __FILE__);
		}
		return gotthewholegraph;
	}

	bool FMTLineGraph::stringForConstraint(std::string& value, const int& stop, const Core::FMTMask& dynamicmask) const
	{
		bool gotthewholegraph = false;
		try{
		value += getBaseStr(dynamicmask);
		value += getEdgesStr(stop, gotthewholegraph);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTLineGraph::stringForConstraint", __LINE__, __FILE__);
		}
		return gotthewholegraph;
	}

	


}

BOOST_CLASS_EXPORT_IMPLEMENT(Graph::FMTLineGraph)
