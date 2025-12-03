
#include "FMTVirtualLineGraph.h"
#include "FMTlayer.hpp"
#include "FMTlinegraph.h"


namespace Spatial{
	FMTVirtualLineGraph::FMTVirtualLineGraph(
				const Graph::FMTlinegraph& p_LineGraph,
				std::map<Graph::FMTlinegraph, size_t>& p_mapping):
				m_iterator(), m_end()
		{
		_insertInto(p_LineGraph, p_mapping);
		}

	void FMTVirtualLineGraph::setLineGraph(const Graph::FMTlinegraph& p_LineGraph,
				std::map<Graph::FMTlinegraph, size_t>& p_mapping)
		{
		_remove();
		_deleteGraph(p_mapping);
		_insertInto(p_LineGraph, p_mapping);
		}

	void FMTVirtualLineGraph::setToGraph(std::map<Graph::FMTlinegraph, size_t>& p_mapping)
		{
		_insertInto(getLineGraph(), p_mapping);
		}

	void FMTVirtualLineGraph::_insertInto(const Graph::FMTlinegraph& p_LineGraph,
		std::map<Graph::FMTlinegraph, size_t>& p_mapping)
	{
		std::pair<std::map<Graph::FMTlinegraph, size_t>::iterator, bool> inserted = 
				p_mapping.insert(std::pair<Graph::FMTlinegraph, size_t>(p_LineGraph,0));
		m_end = p_mapping.end();
		m_iterator = inserted.first;
		++m_iterator->second;
	}

	FMTVirtualLineGraph::FMTVirtualLineGraph(const FMTVirtualLineGraph& p_LineGraph)
	{
		m_end = p_LineGraph.m_end;
		m_iterator = p_LineGraph.m_iterator;
		_add();
	}

	FMTVirtualLineGraph& FMTVirtualLineGraph::operator = (const FMTVirtualLineGraph& p_LineGraph)
	{
		if (this!=&p_LineGraph)
			{
			_remove();
			m_end = p_LineGraph.m_end;
			m_iterator = p_LineGraph.m_iterator;
			_add();
			}
		return *this;
	}

	const Graph::FMTlinegraph& FMTVirtualLineGraph::getLineGraph() const
		{
		return m_iterator->first;
		}

	bool FMTVirtualLineGraph::operator == (const  FMTVirtualLineGraph& p_Element) const
		{
		if (notNull() && p_Element.notNull())
			{
			return (*m_iterator == *p_Element.m_iterator);
			}
		return (notNull() != p_Element.notNull());
		}
	bool FMTVirtualLineGraph::operator < (const  FMTVirtualLineGraph& p_Element) const
		{
		if (notNull() && p_Element.notNull())
			{
			return (*m_iterator < *p_Element.m_iterator);
		}else if (notNull() && !p_Element.notNull())
			{
			return true;
		}else if (p_Element.notNull() && !notNull())
			{
			return false;
			}
		}

	size_t FMTVirtualLineGraph::hash() const
		{
		return m_iterator->first.hash();
		}

	bool FMTVirtualLineGraph::notNull() const
	{
		return m_iterator != m_end;
	}

	void FMTVirtualLineGraph::_add()
		{
		if (notNull())
			{
			++m_iterator->second;
			}
		}
	void FMTVirtualLineGraph::_remove()
		{
		if (notNull())
			{
			--m_iterator->second;
			}
		}
	void FMTVirtualLineGraph::_deleteGraph(
				std::map<Graph::FMTlinegraph, size_t>& p_mapping)
	{
		if (notNull() && m_iterator->second == 0)
			{
			p_mapping.erase(m_iterator);
			}
	}
	
}

