
#include "FMTVirtualLineGraph.h"
#include "FMTlayer.hpp"
#include "FMTlinegraph.h"
#include "FMTSpatialGraphs.h"
#include "FMTGraphInfo.h"
#include "FMTmodel.h"
#include "FMToutput.h"


namespace Spatial{


	double FMTVirtualLineGraph::GetOutput(const Models::FMTmodel& p_model, 
		const std::vector<size_t>& p_solution,
		const Core::FMToutput& p_output, int p_period) const
	{
		if (p_solution.at(m_Iterator->second.GetGraphId())>0)
		{
			const double AREA = m_Iterator->first.getbasedevelopment().getarea();
			const double* SOLUTION = &AREA;
			return m_Iterator->first.getoutput(p_model, p_output, p_period, SOLUTION, Core::FMToutputlevel::totalonly).at("Total");
		}
	return 0xd;
	}


	FMTVirtualLineGraph::FMTVirtualLineGraph(
				FMTSpatialGraphs& p_Graphs,
				std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_Iterator,
				size_t p_Family):
		m_Iterator(p_Iterator), m_GraphFamily(p_Family), m_Graphs(&p_Graphs)
		{

		}

	void FMTVirtualLineGraph::setLineGraph(const Graph::FMTlinegraph& p_LineGraph,
						std::vector<size_t>& p_solution)
		{
		_insertInto(p_LineGraph, p_solution);
		}

	void FMTVirtualLineGraph::SetBaseGraph(std::vector<size_t>& p_solution)
		{
		_remove(p_solution);
		m_Iterator = m_Graphs->GetBaseIterator(m_GraphFamily);
		_add(p_solution);
		}

	void FMTVirtualLineGraph::SetLastPeriod(std::vector<size_t>& p_solution)
		{
		_remove(p_solution);
		m_Iterator = m_Graphs->GetLastPeriodIterator(m_GraphFamily, m_Iterator);
		_add(p_solution);
		}

	FMTVirtualLineGraph FMTVirtualLineGraph::PostSolve(const Core::FMTmaskfilter& p_Filter,
		const std::vector<int>& p_actionMapping,
		FMTSpatialGraphs& p_Graphs,
		std::vector<size_t>& p_solution) const
	{
		Graph::FMTlinegraph graphCopy = getLineGraph();
		graphCopy.postsolve(p_Filter, 
			p_Graphs.GetModel().getthemes(), p_actionMapping);
		FMTVirtualLineGraph postSolved = p_Graphs.SetVirtualGraph(graphCopy);
		postSolved._add(p_solution);
		return postSolved;
	}

	void FMTVirtualLineGraph::_insertInto(const Graph::FMTlinegraph& p_LineGraph,
		std::vector<size_t>& p_solution)
	{
		_remove(p_solution);
		m_Iterator =  m_Graphs->SetIterator(p_LineGraph,m_GraphFamily);
		_add(p_solution);

	}

	FMTVirtualLineGraph::FMTVirtualLineGraph(const FMTVirtualLineGraph& p_LineGraph):
		m_Iterator(p_LineGraph.m_Iterator), m_GraphFamily(p_LineGraph.m_GraphFamily), m_Graphs(p_LineGraph.m_Graphs)
	{

	}

	FMTVirtualLineGraph::FMTVirtualLineGraph(const FMTVirtualLineGraph& p_LineGraph, FMTSpatialGraphs& p_Graphs):
		m_Iterator(), m_GraphFamily(p_LineGraph.m_GraphFamily), m_Graphs(&p_Graphs)
	{
		m_Iterator = m_Graphs->GetIterator(p_LineGraph.getLineGraph(), m_GraphFamily);
	}

	FMTVirtualLineGraph::~FMTVirtualLineGraph()
	{

	}

	FMTVirtualLineGraph& FMTVirtualLineGraph::operator = (const FMTVirtualLineGraph& p_LineGraph)
	{
		if (this!=&p_LineGraph)
			{
			m_Iterator = p_LineGraph.m_Iterator;
			m_Graphs = p_LineGraph.m_Graphs;
			m_GraphFamily = p_LineGraph.m_GraphFamily;
			}
		return *this;
	}

	const Graph::FMTlinegraph& FMTVirtualLineGraph::getLineGraph() const
		{
		return m_Iterator->first;
		}

	bool FMTVirtualLineGraph::operator == (const  FMTVirtualLineGraph& p_Element) const
		{
		if (notNull() && p_Element.notNull())
			{
			return (m_GraphFamily == p_Element.m_GraphFamily &&
				m_Iterator == p_Element.m_Iterator);
			}
		return (notNull() != p_Element.notNull());
		}
	bool FMTVirtualLineGraph::operator < (const  FMTVirtualLineGraph& p_Element) const
		{
		if (notNull() && p_Element.notNull())
			{
			if (m_GraphFamily < p_Element.m_GraphFamily)
				return true;
			if (m_GraphFamily > p_Element.m_GraphFamily)
				return false;
			if (m_Iterator->first < p_Element.m_Iterator->first)
				return true;
			if (p_Element.m_Iterator->first < m_Iterator->first)
				return false;
		}else if (notNull() && !p_Element.notNull())
			{
			return true;
		}else if (p_Element.notNull() && !notNull())
			{
			return false;
			}
		return false;
		}

	size_t FMTVirtualLineGraph::hash() const
		{
		return m_Iterator->first.hash();
		}

	bool FMTVirtualLineGraph::notNull() const
		{
		return m_Graphs->IsNotNull(m_GraphFamily, m_Iterator);
		}

	void FMTVirtualLineGraph::_add(std::vector<size_t>& p_solution)
		{
		m_Graphs->AddToSolution(p_solution, m_GraphFamily, m_Iterator);
		}
	void FMTVirtualLineGraph::_remove(std::vector<size_t>& p_solution)
		{
		m_Graphs->RemoveToSolution(p_solution, m_GraphFamily, m_Iterator);
		}
	
}

