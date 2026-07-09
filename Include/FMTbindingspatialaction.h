/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTBINDINGSPATIALACTION_Hm_included
#define FMTBINDINGSPATIALACTION_Hm_included

#include <vector>
#include <limits>
#include <cstddef>

namespace Spatial

{
	class FMTbindingspatialaction
	{
		std::vector<int>neighbors;
		size_t minimal_green_up;
		size_t maximal_green_up;
		size_t minimal_adjacency;
		size_t maximal_adjacency;
		size_t minimal_size;
		size_t maximal_size;
		size_t minimal_neighbors_size;
		size_t maximal_neighbors_size;
	public:
		FMTbindingspatialaction()=default;
		FMTbindingspatialaction(const FMTbindingspatialaction& rhs) = default;
		FMTbindingspatialaction& operator = (const FMTbindingspatialaction& rhs)=default;
		FMTbindingspatialaction(const std::vector<int>& lneighbors,
			const size_t& mingreenup, const size_t& maxgreenup,
			const size_t& minadjacency, const size_t& maxadjacency,
			const size_t& minsize, const size_t& maxsize,
			const size_t& minnsize, const size_t& maxnsize);
		bool isSpatialyBinding() const;
		bool isSpatialyAreaBinding() const;
		bool isSpatialyAdjacencyBinding() const;
		bool testMinimalAdjacency() const;
		bool testMaximalAdjacency() const;
		inline const std::vector<int>& getNeighbors() const
			{
			return neighbors;
			}
		inline const size_t& getMinimalGreenUp() const
			{
			return minimal_green_up;
			}
		inline const size_t& getMaximalGreenUp() const
			{
			return maximal_green_up;
			}
		inline const size_t& getMinimalAdjacency() const
			{
			return minimal_adjacency;
			}
		inline const size_t& getMaximalAdjacency() const
			{
			return maximal_adjacency;
			}
		inline const size_t& getMinimalSize() const
			{
			return minimal_size;
			}
		inline const size_t& getMaximalSize() const
			{
			return maximal_size;
			}
		inline const size_t& getMinimalNeighborSize() const
			{
			return minimal_neighbors_size;
			}
		inline const size_t& getMaximalNeighborSize() const
			{
			return maximal_neighbors_size;
			}
	};



}
#endif
