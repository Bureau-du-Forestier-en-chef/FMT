/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTBindingSpatialAction.h"

namespace Spatial
{
	FMTBindingSpatialAction::FMTBindingSpatialAction(const std::vector<int>& lneighbors,
		const size_t& mingreenup, const size_t& maxgreenup,
		const size_t& minadjacency, const size_t& maxadjacency,
		const size_t& minsize, const size_t& maxsize,
		const size_t& minnsize, const size_t& maxnsize):
		m_neighbors(lneighbors),
		m_minimal_green_up(mingreenup),
		m_maximal_green_up(maxgreenup),
		m_minimal_adjacency(minadjacency),
		m_maximal_adjacency(maxadjacency),
		m_minimal_size(minsize),
		m_maximal_size(maxsize),
		m_minimal_neighbors_size(minnsize),
		m_maximal_neighbors_size(maxnsize)
	{

	}
	bool FMTBindingSpatialAction::isSpatiallyBinding() const
	{
		return (isSpatiallyAdjacencyBinding()|| isSpatiallyAreaBinding());
	}

	bool FMTBindingSpatialAction::isSpatiallyAdjacencyBinding() const
	{
		return (!m_neighbors.empty() && (m_minimal_adjacency > 0 ||
			m_maximal_adjacency != std::numeric_limits<size_t>::max()));
	}

	bool FMTBindingSpatialAction::isSpatiallyAreaBinding() const
		{
		return (!m_neighbors.empty() && (m_minimal_size > 0 ||
				m_maximal_size != std::numeric_limits<size_t>::max()));
		}

	bool FMTBindingSpatialAction::testMinimalAdjacency() const
	{
		return (!m_neighbors.empty() &&  m_minimal_adjacency > 0);
	}
	bool FMTBindingSpatialAction::testMaximalAdjacency() const
	{
		return (!m_neighbors.empty() &&  m_maximal_adjacency != std::numeric_limits<size_t>::max());
	}

}