/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTBINDINGSPATIALACTION_H_INCLUDED
#define FMTBINDINGSPATIALACTION_H_INCLUDED

#include <vector>
#include <limits>

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
		bool isspatialybinding() const;
		bool isspatialyareabinding() const;
		inline const std::vector<int>& getneighbors() const
			{
			return neighbors;
			}
		inline const size_t& getminimalgreenup() const
			{
			return minimal_green_up;
			}
		inline const size_t& getmaximalgreenup() const
			{
			return maximal_green_up;
			}
		inline const size_t& getminimaladjacency() const
			{
			return minimal_adjacency;
			}
		inline const size_t& getmaximaladjacency() const
			{
			return maximal_adjacency;
			}
		inline const size_t& getminimalsize() const
			{
			return minimal_size;
			}
		inline const size_t& getmaximalsize() const
			{
			return maximal_size;
			}
		inline const size_t& getminimalneighborsize() const
			{
			return minimal_neighbors_size;
			}
		inline const size_t& getmaximalneighborsize() const
			{
			return maximal_neighbors_size;
			}
	};



}
#endif
