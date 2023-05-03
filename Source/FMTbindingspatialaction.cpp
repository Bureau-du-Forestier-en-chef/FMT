/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTbindingspatialaction.hpp"

namespace Spatial
{
	FMTbindingspatialaction::FMTbindingspatialaction(const std::vector<int>& lneighbors,
		const size_t& mingreenup, const size_t& maxgreenup,
		const size_t& minadjacency, const size_t& maxadjacency,
		const size_t& minsize, const size_t& maxsize,
		const size_t& minnsize, const size_t& maxnsize):
		neighbors(lneighbors),
		minimal_green_up(mingreenup),
		maximal_green_up(maxgreenup),
		minimal_adjacency(minadjacency),
		maximal_adjacency(maxadjacency),
		minimal_size(minsize),
		maximal_size(maxsize),
		minimal_neighbors_size(minnsize),
		maximal_neighbors_size(maxnsize)
	{

	}
	bool FMTbindingspatialaction::isspatialybinding() const
	{
		return (isspatialyadjacencybinding()|| isspatialyareabinding());
	}

	bool FMTbindingspatialaction::isspatialyadjacencybinding() const
	{
		return (!neighbors.empty() && (minimal_adjacency > 0 ||
			maximal_adjacency != std::numeric_limits<size_t>::max()));
	}

	bool FMTbindingspatialaction::isspatialyareabinding() const
		{
		return (!neighbors.empty() && (minimal_size > 0 ||
				maximal_size != std::numeric_limits<size_t>::max()));
		}

	bool FMTbindingspatialaction::testminimaladjacency() const
	{
		return (!neighbors.empty() &&  minimal_adjacency > 0);
	}
	bool FMTbindingspatialaction::testmaximaladjacency() const
	{
		return (!neighbors.empty() &&  maximal_adjacency != std::numeric_limits<size_t>::max());
	}

}