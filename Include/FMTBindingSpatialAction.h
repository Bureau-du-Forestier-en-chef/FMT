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
	// DocString: FMTBindingSpatialAction
	/**
	@brief Spatial binding of an action, holding the green up, adjacency, size and neighbors constraints used to test spatial operability.
	*/
	class FMTBindingSpatialAction
	{
	public:
		// DocString: FMTBindingSpatialAction()
		/**
		@brief Default constructor for FMTBindingSpatialAction.
		*/
		FMTBindingSpatialAction()=default;
		// DocString: FMTBindingSpatialAction(const FMTBindingSpatialAction)
		/**
		@brief Copy constructor for FMTBindingSpatialAction.
		@param[in] rhs the FMTBindingSpatialAction to copy.
		*/
		FMTBindingSpatialAction(const FMTBindingSpatialAction& rhs) = default;
		// DocString: FMTBindingSpatialAction::operator=
		/**
		@brief Copy assignment operator for FMTBindingSpatialAction.
		@param[in] rhs the FMTBindingSpatialAction to copy.
		@return a reference to this FMTBindingSpatialAction.
		*/
		FMTBindingSpatialAction& operator = (const FMTBindingSpatialAction& rhs)=default;
		// DocString: FMTBindingSpatialAction(const std::vector<int>&,...)
		/**
		@brief Construct a FMTBindingSpatialAction from its neighbors and its green up, adjacency, size and neighbor size bounds.
		@param[in] lneighbors the neighbors.
		@param[in] mingreenup the minimal green up.
		@param[in] maxgreenup the maximal green up.
		@param[in] minadjacency the minimal adjacency.
		@param[in] maxadjacency the maximal adjacency.
		@param[in] minsize the minimal size.
		@param[in] maxsize the maximal size.
		@param[in] minnsize the minimal neighbors size.
		@param[in] maxnsize the maximal neighbors size.
		*/
		FMTBindingSpatialAction(const std::vector<int>& lneighbors,
			const size_t& mingreenup, const size_t& maxgreenup,
			const size_t& minadjacency, const size_t& maxadjacency,
			const size_t& minsize, const size_t& maxsize,
			const size_t& minnsize, const size_t& maxnsize);
		// DocString: FMTBindingSpatialAction::isSpatiallyBinding
		/**
		@brief Return true if the action is spatially binding.
		@return true if the action is spatially binding else false.
		*/
		bool isSpatiallyBinding() const;
		// DocString: FMTBindingSpatialAction::isSpatiallyAreaBinding
		/**
		@brief Return true if the action is spatially binding on the area (size).
		@return true if the action is area binding else false.
		*/
		bool isSpatiallyAreaBinding() const;
		// DocString: FMTBindingSpatialAction::isSpatiallyAdjacencyBinding
		/**
		@brief Return true if the action is spatially binding on the adjacency.
		@return true if the action is adjacency binding else false.
		*/
		bool isSpatiallyAdjacencyBinding() const;
		// DocString: FMTBindingSpatialAction::testMinimalAdjacency
		/**
		@brief Return true if a minimal adjacency is set.
		@return true if a minimal adjacency is set else false.
		*/
		bool testMinimalAdjacency() const;
		// DocString: FMTBindingSpatialAction::testMaximalAdjacency
		/**
		@brief Return true if a maximal adjacency is set.
		@return true if a maximal adjacency is set else false.
		*/
		bool testMaximalAdjacency() const;
		// DocString: FMTBindingSpatialAction::getNeighbors
		/**
		@brief Return the neighbors of the action.
		@return the neighbors.
		*/
		inline const std::vector<int>& getNeighbors() const
			{
			return m_neighbors;
			}
		// DocString: FMTBindingSpatialAction::getMinimalGreenUp
		/**
		@brief Return the minimal green up.
		@return the minimal green up.
		*/
		inline const size_t& getMinimalGreenUp() const
			{
			return m_minimal_green_up;
			}
		// DocString: FMTBindingSpatialAction::getMaximalGreenUp
		/**
		@brief Return the maximal green up.
		@return the maximal green up.
		*/
		inline const size_t& getMaximalGreenUp() const
			{
			return m_maximal_green_up;
			}
		// DocString: FMTBindingSpatialAction::getMinimalAdjacency
		/**
		@brief Return the minimal adjacency.
		@return the minimal adjacency.
		*/
		inline const size_t& getMinimalAdjacency() const
			{
			return m_minimal_adjacency;
			}
		// DocString: FMTBindingSpatialAction::getMaximalAdjacency
		/**
		@brief Return the maximal adjacency.
		@return the maximal adjacency.
		*/
		inline const size_t& getMaximalAdjacency() const
			{
			return m_maximal_adjacency;
			}
		// DocString: FMTBindingSpatialAction::getMinimalSize
		/**
		@brief Return the minimal size.
		@return the minimal size.
		*/
		inline const size_t& getMinimalSize() const
			{
			return m_minimal_size;
			}
		// DocString: FMTBindingSpatialAction::getMaximalSize
		/**
		@brief Return the maximal size.
		@return the maximal size.
		*/
		inline const size_t& getMaximalSize() const
			{
			return m_maximal_size;
			}
		// DocString: FMTBindingSpatialAction::getMinimalNeighborSize
		/**
		@brief Return the minimal neighbors size.
		@return the minimal neighbors size.
		*/
		inline const size_t& getMinimalNeighborSize() const
			{
			return m_minimal_neighbors_size;
			}
		// DocString: FMTBindingSpatialAction::getMaximalNeighborSize
		/**
		@brief Return the maximal neighbors size.
		@return the maximal neighbors size.
		*/
		inline const size_t& getMaximalNeighborSize() const
			{
			return m_maximal_neighbors_size;
			}
	private:
		std::vector<int>m_neighbors;
		size_t m_minimal_green_up;
		size_t m_maximal_green_up;
		size_t m_minimal_adjacency;
		size_t m_maximal_adjacency;
		size_t m_minimal_size;
		size_t m_maximal_size;
		size_t m_minimal_neighbors_size;
		size_t m_maximal_neighbors_size;
	};



}
#endif
