/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTVIRTUALLINEGRAPH_H
#define FMTVIRTUALLINEGRAPH_H


#include "FMTutility.h"
#include <vector>
#include <map>
#include <memory>
#include "FMTGraphInfo.h"
#include "FMTSpatialGraphs.h"

namespace Graph
{
	class FMTLineGraph;
}

namespace Models
{
	class FMTModel;
}

namespace Core
{
	class FMTOutput;
	class FMTTheme;
	class FMTMaskFilter;
}


namespace Spatial
	{
	template <typename T>
	class FMTLayer;
	class FMTSolutionTracker;
	// DocString: FMTVirtualLineGraph
	/**
	@brief Lightweight handle to a line graph stored in a FMTSpatialGraphs, forwarding output and solution operations to the underlying graph.
	*/
	class FMTEXPORT FMTVirtualLineGraph
		{
		public:
			// DocString: FMTVirtualLineGraph()
			/**
			@brief Default constructor for FMTVirtualLineGraph.
			*/
			FMTVirtualLineGraph()=default;
			// DocString: FMTVirtualLineGraph(FMTSpatialGraphs&,FMTSpatialGraphs::const_iterator,size_t)
			/**
			@brief Construct a FMTVirtualLineGraph pointing to a graph of a family in a FMTSpatialGraphs.
			@param[in] p_Graphs the spatial graphs.
			@param[in] p_Iterator the graph iterator.
			@param[in] p_Family the constraint family.
			*/
			FMTVirtualLineGraph(FMTSpatialGraphs& p_Graphs,
							FMTSpatialGraphs::const_iterator p_Iterator,
							size_t p_Family);
			// DocString: FMTVirtualLineGraph(const FMTVirtualLineGraph&,FMTSpatialGraphs&)
			/**
			@brief Copy a FMTVirtualLineGraph, rebinding it to another FMTSpatialGraphs.
			@param[in] p_LineGraph the FMTVirtualLineGraph to copy.
			@param[in] p_Graphs the spatial graphs.
			*/
			FMTVirtualLineGraph(const FMTVirtualLineGraph& p_LineGraph, FMTSpatialGraphs& p_Graphs);
			// DocString: FMTVirtualLineGraph(const FMTVirtualLineGraph&)
			/**
			@brief Copy constructor for FMTVirtualLineGraph.
			@param[in] p_LineGraph the FMTVirtualLineGraph to copy.
			*/
			FMTVirtualLineGraph(const FMTVirtualLineGraph& p_LineGraph);
			// DocString: FMTVirtualLineGraph::operator=
			/**
			@brief Copy assignment operator for FMTVirtualLineGraph.
			@param[in] p_LineGraph the FMTVirtualLineGraph to copy.
			@return a reference to this FMTVirtualLineGraph.
			*/
			FMTVirtualLineGraph& operator = (const FMTVirtualLineGraph& p_LineGraph);
			// DocString: ~FMTVirtualLineGraph()
			/**
			@brief Destructor for FMTVirtualLineGraph.
			*/
			~FMTVirtualLineGraph();
			// DocString: FMTVirtualLineGraph::operator==
			/**
			@brief Comparison operator for FMTVirtualLineGraph.
			@param[in] p_Element the FMTVirtualLineGraph to compare to.
			@return true if both virtual line graphs are equal else false.
			*/
			bool operator == (const  FMTVirtualLineGraph& p_Element) const;
			// DocString: FMTVirtualLineGraph::operator<
			/**
			@brief Less than operator for FMTVirtualLineGraph.
			@param[in] p_Element the FMTVirtualLineGraph to compare to.
			@return true if this virtual line graph is less than the other else false.
			*/
			bool operator < (const  FMTVirtualLineGraph& p_Element) const;
			// DocString: FMTVirtualLineGraph::hash
			/**
			@brief Hash the virtual line graph.
			@return the hash of the virtual line graph.
			*/
			size_t hash() const;
			// DocString: FMTVirtualLineGraph::getLineGraph
			/**
			@brief Return the underlying line graph.
			@return the line graph.
			*/
			const Graph::FMTLineGraph& getLineGraph() const;
			// DocString: FMTVirtualLineGraph::setLineGraph
			/**
			@brief Set the underlying line graph and update the solution tracker.
			@param[in] p_LineGraph the line graph.
			@param[in,out] p_solution the solution tracker.
			*/
			void setLineGraph(const Graph::FMTLineGraph& p_LineGraph,
							FMTSolutionTracker& p_solution);
			// DocString: FMTVirtualLineGraph::getOutput
			/**
			@brief Return the output value of the graph for a period.
			@param[in] p_model the model.
			@param[in] p_solution the solution tracker.
			@param[in] p_output the output.
			@param[in] p_period the period.
			@return the output value.
			*/
			double getOutput(const Models::FMTModel& p_model,
				const FMTSolutionTracker& p_solution,
				const Core::FMTOutput& p_output, int p_period) const;
			// DocString: FMTVirtualLineGraph::getGraphFamily
			/**
			@brief Return the constraint family of the graph.
			@return the graph family.
			*/
			size_t getGraphFamily() const;
			// DocString: FMTVirtualLineGraph::setBaseGraph
			/**
			@brief Set the graph to its base version and update the solution tracker.
			@param[in,out] p_solution the solution tracker.
			*/
			void setBaseGraph(FMTSolutionTracker& p_solution);
			// DocString: FMTVirtualLineGraph::setLastPeriod
			/**
			@brief Set the graph to its last period version and update the solution tracker.
			@param[in,out] p_solution the solution tracker.
			*/
			void setLastPeriod(FMTSolutionTracker& p_solution);
			// DocString: FMTVirtualLineGraph::setNaturalGrowth
			/**
			@brief Set the graph to its natural growth version and update the solution tracker.
			@param[in,out] p_solution the solution tracker.
			*/
			void setNaturalGrowth(FMTSolutionTracker& p_solution);
			// DocString: FMTVirtualLineGraph::postSolve
			/**
			@brief Return a postsolved copy of the virtual line graph.
			@param[in] p_Filter the mask filter.
			@param[in] p_actionMapping the action mapping.
			@param[in,out] p_Graphs the spatial graphs.
			@param[in,out] p_solution the solution tracker.
			@return the postsolved virtual line graph.
			*/
			FMTVirtualLineGraph postSolve(const Core::FMTMaskFilter& p_Filter,
					const std::vector<int>& p_actionMapping,
					FMTSpatialGraphs& p_Graphs,
				FMTSolutionTracker& p_solution) const;
		private:
			FMTSpatialGraphs::const_iterator m_Iterator;
			size_t m_GraphFamily;
			FMTSpatialGraphs* m_Graphs;
			// DocString: FMTVirtualLineGraph::_insertInto
			/**
			@brief Insert a line graph into the spatial graphs and update the solution tracker.
			@param[in] p_LineGraph the line graph.
			@param[in,out] p_solution the solution tracker.
			*/
			void _insertInto(const Graph::FMTLineGraph& p_LineGraph,
					FMTSolutionTracker& p_solution);
			// DocString: FMTVirtualLineGraph::_add
			/**
			@brief Add the graph contribution to the solution tracker.
			@param[in,out] p_solution the solution tracker.
			*/
			void _add(FMTSolutionTracker& p_solution);
			// DocString: FMTVirtualLineGraph::_remove
			/**
			@brief Remove the graph contribution from the solution tracker.
			@param[in,out] p_solution the solution tracker.
			*/
			void _remove(FMTSolutionTracker& p_solution);
			// DocString: FMTVirtualLineGraph::notNull
			/**
			@brief Return true if the virtual line graph points to a valid graph.
			@return true if the graph is not null else false.
			*/
			bool notNull() const;
		};
	}

namespace std {
	template <>
	struct hash<Spatial::FMTVirtualLineGraph>
	{
		std::size_t operator()(const Spatial::FMTVirtualLineGraph& p_object) const
		{
			return (p_object.hash());
		}
	};

}


#endif 
