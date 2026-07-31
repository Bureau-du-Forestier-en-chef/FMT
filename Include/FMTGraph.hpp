/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTGRAPH_H
#define FMTGRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/thread.hpp>
#include <functional>

#include "FMTVertexProperties.h"
#include "FMTEdgeProperties.h"
#include "FMTGraphStats.h"

#include "FMTOutputNode.h"
#include "FMTDevelopment.h"
#include "FMTActualDevelopment.h"
#include "FMTFuturDevelopment.h"
#include "FMTDevelopmentPath.h"
#include "FMTSchedule.h"
#include "FMTConstraint.h"
#include "FMTOutputNodeCache.h"
#include <boost\unordered_set.hpp>
#include "FMTPredictor.h"
#include "FMTGraphVertexToYield.h"



#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/export.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include <memory>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <queue>
#include <random>

#include "FMTModel.h"
#include "FMTAction.h"
#include "FMTTheme.h"
#include "FMTObject.h"
#include <tuple>
#include <assert.h>
#include "FMTLookup.h"
#include "boost/graph/graphviz.hpp"
#include "FMTExceptionHandler.h"
#include "FMTSerie.h"
#include <memory>
#include <type_traits>


/// Namespace for using/building unidirectional graphs in FMT
namespace Graph
{

enum class FMTgraphbuild
	{
		schedulebuild = 1,
		fullbuild = 2,
		nobuild = 3
	};

#define FMT_COMMA ,


// DocString: FMTGraph
/**
@brief Directed graph holding the FMTDevelopment of every period as vertices and the actions and natural growth as edges.
@details The graph is divided per period and is the backbone used to build the matrix of the LP models and the line graphs of the spatially explicit models.
@tparam tvertexproperties the vertex properties type.
@tparam tedgeproperties the edge properties type.
*/
template <class tvertexproperties,class tedgeproperties>
class FMTEXPORT FMTGraph : public Core::FMTObject
    {
	public:
		typedef boost::adjacency_list<boost::vecS,
			boost::vecS,
			boost::bidirectionalS,
			tvertexproperties,
			tedgeproperties,
			boost::no_property,
			boost::listS>FMTadjacency_list;
		typedef typename boost::graph_traits<FMTadjacency_list>::vertex_descriptor FMTvertex_descriptor;
		typedef typename boost::graph_traits<FMTadjacency_list>::edge_descriptor FMTedge_descriptor;
		typedef typename boost::graph_traits<FMTadjacency_list>::in_edge_iterator FMTinedge_iterator;
		typedef typename boost::graph_traits<FMTadjacency_list>::out_edge_iterator FMToutedge_iterator;
		typedef typename boost::graph_traits<FMTadjacency_list>::vertex_iterator FMTvertex_iterator;
		typedef typename boost::graph_traits<FMTadjacency_list>::edge_iterator FMTedge_iterator;
		typedef typename std::pair<FMToutedge_iterator, FMToutedge_iterator> FMToutedge_pair;
		typedef typename std::pair<FMTinedge_iterator, FMTinedge_iterator> FMTinedge_pair;
		typedef typename std::pair<FMTvertex_iterator, FMTvertex_iterator> FMTvertex_pair;
		// DocString: FMTGraph()
		/**
		@brief Default constructor for FMTGraph.
		*/
		FMTGraph() :
			Core::FMTObject(),
			data(),
			m_gotDeath(),
			buildtype(FMTgraphbuild::nobuild),
			developments(),
			nodescache(),
			stats(),
			m_allocator(),
			m_reserve(),
			m_selectedVertices(m_allocator)
		{

		}
		// DocString: ~FMTGraph()
		/**
		@brief Default virtual destructor for FMTGraph.
		*/
		virtual ~FMTGraph() = default;

		// DocString: FMTGraph(const FMTgraphbuild)
		/**
		@brief Construct a FMTGraph from a build type.
		@param[in] lbuildtype the build type.
		*/
		FMTGraph(const FMTgraphbuild lbuildtype) :
			Core::FMTObject(),
			data(),
			m_gotDeath(),
			buildtype(lbuildtype),
			developments(),
			nodescache(),
			stats(),
			m_allocator(),
			m_reserve(),
			m_selectedVertices(m_allocator)
		{

		}

		// DocString: FMTGraph(const FMTGraph&)
		/**
		@brief Copy constructor for FMTGraph.
		@param[in] rhs the FMTGraph to copy.
		*/
		FMTGraph(const FMTGraph& rhs) :
			Core::FMTObject(rhs),
			data(rhs.data),
			m_gotDeath(rhs.m_gotDeath),
			buildtype(rhs.buildtype),
			developments(),
			nodescache(),
			stats(rhs.stats),
			m_allocator(rhs.m_allocator),
			m_reserve(rhs.m_reserve),
			m_selectedVertices(rhs.m_allocator)
		{
			_generateDevelopments();
		}

		// DocString: FMTGraph::swap
		/**
		@brief Swap this FMTGraph with another one.
		@param[in,out] rhs the FMTGraph to swap with.
		*/
		void swap(FMTGraph& rhs)
		{
			/*std::swap(buildtype, rhs.buildtype);
			std::swap(m_gotDeath, rhs.m_gotDeath);
			nodescache.swap(rhs.nodescache);
			std::swap(stats,rhs.stats);
			//data.swap(rhs.data);
			data = rhs.data;
			developments.swap(rhs.developments);
			std::swap(m_reserve, rhs.m_reserve);
			m_allocator = rhs.m_allocator;*/
			//std::swap(developments, rhs.developments);
		}
		// DocString: FMTGraph::reserveVertices
		/**
		@brief Reserve memory for a number of vertices.
		@param[in] p_reserve the number of vertices to reserve.
		*/
		void reserveVertices(size_t p_reserve)
		{
			data.m_vertices.reserve(p_reserve);
		}
		// DocString: FMTGraph::operator=
		/**
		@brief Copy assignment operator for FMTGraph.
		@param[in] rhs the FMTGraph to copy.
		@return a reference to this FMTGraph.
		*/
		FMTGraph& operator = (const FMTGraph& rhs)
		{
			if (this != &rhs)
			{
				Core::FMTObject::operator=(rhs);
				m_gotDeath = rhs.m_gotDeath;
				buildtype = rhs.buildtype;
				nodescache = rhs.nodescache;
				stats = rhs.stats;
				data = rhs.data;
				m_reserve = rhs.m_reserve;
				m_allocator = rhs.m_allocator;
				m_selectedVertices = rhs.m_selectedVertices;
				_generateDevelopments();
			}
			return *this;

		}

		// DocString: FMTGraph::copyToPeriod
		/**
		@brief Copy the graph, dropping the vertices with a period greater than the given one.
		@param[in] p_period the maximal period.
		@return the copied graph.
		*/
		FMTGraph copyToPeriod(int p_period) const
		{
			FMTGraph newGraph;
			try {
				newGraph.m_gotDeath = m_gotDeath;
				newGraph.buildtype = buildtype;
				newGraph.m_allocator = m_allocator;
				newGraph.m_reserve = size();
				newGraph.reserveVertices(newGraph.m_reserve);
				boost::filtered_graph<FMTadjacency_list, EdgePeriodicFilter,VertexPeriodicFilter>filterdGraph(data,
					EdgePeriodicFilter(data, newGraph.stats, p_period),
					VertexPeriodicFilter(data, p_period));
				boost::copy_graph(filterdGraph, newGraph.data);
				newGraph.stats.vertices = static_cast<int>(boost::num_vertices(newGraph.data));
				newGraph.stats.edges = static_cast<int>(boost::num_edges(newGraph.data));
				newGraph._generateDevelopments();
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::copyToPeriod", __LINE__, __FILE__);
			}
			return newGraph;
		}


		// DocString: FMTGraph::operator==
		/**
		@brief Comparison operator for FMTGraph.
		@param[in] rhs the FMTGraph to compare to.
		@return true if both graphs are equal else false.
		*/
		bool operator == (const FMTGraph& rhs) const
		{
			try {
				if (buildtype == rhs.buildtype &&
					stats == rhs.stats)
				{
					typename std::vector<FMTvertex_pair>::const_iterator devsit = this->getFirstConstBlock();
					size_t location = 0;
					while (devsit != developments.end())
					{
						//for (typename std::vector<FMTvertex_pair>::const_iterator it = developments.at(location).begin(); it != developments.at(location).end(); it++)
						//{
						if (((data[*developments.at(location).first].get()) != (rhs.data[*rhs.developments.at(location).first].get()) ||
							((location < developments.size() - 1) && 
							(data[*developments.at(location).second].get()) != (rhs.data[*rhs.developments.at(location).second].get()))))
						{
							return false;
						}

						//}
						++location;
						++devsit;
					}
					return true;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::operator==", __LINE__, __FILE__);
			}
			return false;
		}
		// DocString: FMTGraph::operator!=
		/**
		@brief Comparison operator for FMTGraph.
		@param[in] rhs the FMTGraph to compare to.
		@return true if both graphs are different else false.
		*/
		bool operator != (const FMTGraph& rhs) const
		{
			return (!(*this == rhs));
		}
		// DocString: FMTGraph::clearCache
		/**
		@brief Clear the output node cache of the graph.
		*/
		void clearCache()
		{
			std::vector<FMTOutputNodeCache<FMTvertex_descriptor FMT_COMMA FMTvertex_iterator>>().swap(nodescache);
		}
		// DocString: FMTGraph::clearDevelopments
		/**
		@brief Clear the developments location by period of the graph.
		*/
		void clearDevelopments()
		{
			std::vector<FMTvertex_pair>().swap(developments);
		}
		// DocString: FMTGraph::getBuildType
		/**
		@brief Return the build type of the graph.
		@return the build type.
		*/
		FMTgraphbuild getBuildType() const
		{
			return buildtype;
		}
		// DocString: FMTGraph::setBuildType
		/**
		@brief Set the build type of the graph.
		@param[in] build the build type.
		*/
		void setBuildType(const FMTgraphbuild& build)
		{
			buildtype = build;
		}
		// DocString: FMTGraph::getDevsSet
		/**
		@brief Return the developments of a period in a lookup set.
		@param[in] period the period.
		@return the developments lookup set.
		*/
		boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>>getDevsSet(const int& period) const
		{
			boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>> basedevs;
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = developments.at(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					basedevs.insert(Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>(*vertex_iterator, data[*vertex_iterator].get()));
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getDevsSet", __LINE__, __FILE__);
			}
			return basedevs;
		}

		// DocString: FMTGraph::containsDevelopment
		/**
		@brief Return true if a development is already in the lookup set.
		@param[in] development the development.
		@param[in] devsets the developments lookup set.
		@return true if the development is present else false.
		*/
		bool containsDevelopment(
			const Core::FMTDevelopment& development,
			const boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, 
			Core::FMTDevelopment>>& alldevs) const
		{
			return (alldevs.find(Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>(development)) != alldevs.end());
		}

		// DocString: FMTGraph::initialize
		/**
		@brief Initialize the graph with the actual developments of the first period.
		@param[in] actdevelopments the actual developments.
		@return the queue of active vertices.
		*/
		std::queue<FMTvertex_descriptor> initialize(const std::vector<Core::FMTActualDevelopment>& actdevelopments)
		{
			std::queue<FMTvertex_descriptor>actives;
			try {
				developments.clear();
				const int startingperiod = actdevelopments.begin()->getPeriod();
				const int actualperiod = startingperiod + 1;
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				//End always stay the same use .end() for non valid period
				boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
				for (int period = 0; period <= actualperiod; ++period)
				{
					developments.push_back(FMTvertex_pair(vertex_iterator_end, vertex_iterator_end));
				}
				const int constraint_id = -1;
				int edge_id = -1;
				const double proportion = 100;
				stats.edges = 0;
				stats.vertices = 0;
				std::vector<FMTvertex_descriptor>P0descriptors;
				P0descriptors.reserve(actdevelopments.size());
				for (const Core::FMTActualDevelopment& development : actdevelopments)
				{
					const FMTVertexProperties properties(development, constraint_id);
					const FMTvertex_descriptor newvertex = boost::add_vertex(properties, data);
					P0descriptors.push_back(newvertex);
					//++vertex_iterator;
					++stats.vertices;
				}
				size_t poid = 0;
				boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>> devsets;
				for (const Core::FMTActualDevelopment& development : actdevelopments)
				{
					//P1
					Core::FMTFuturDevelopment p1dev(development);
					p1dev.setPeriod(actualperiod);
					const FMTvertex_descriptor tovertex = addDevelopment(p1dev, devsets);
					actives.push(tovertex);
					//Now set the edge!!
					const FMTEdgeProperties newEdge(edge_id, stats.edges, proportion);
					boost::add_edge(P0descriptors.at(poid), tovertex, newEdge, data);
					++stats.edges;
					++poid;
				}
				FMTvertex_iterator firstp0;
				FMTvertex_iterator lastone;
				boost::tie(firstp0, vertex_iterator_end) = boost::vertices(data);
				lastone = firstp0;
				for (const Core::FMTActualDevelopment& development : actdevelopments)
				{
					++lastone;
				}
				developments[startingperiod] = FMTvertex_pair(firstp0, lastone);
				developments[actualperiod] = FMTvertex_pair(lastone, vertex_iterator_end);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::initialize", __LINE__, __FILE__);
			}
			return actives;

		}
		// DocString: FMTGraph::build(const Models::FMTModel&,std::queue<FMTvertex_descriptor>)
		/**
		@brief Build the graph for one period from the active vertices, adding the operable actions and the natural growth.
		@param[in] model the model.
		@param[in] actives the active vertices.
		@param[in] compressageoperability the age class operability compression.
		@return the graph stats of the newly built elements.
		*/
		FMTGraphStats build(
			const Models::FMTModel& model,
			std::queue<FMTvertex_descriptor> actives,
			int compressageoperability = 1)
		{
			FMTGraphStats newstats;
			try {
				FMTGraphStats statsDiff(stats);
				const int actualperiod = getPeriod();
				const bool GOT_SERIES = model.useActionSerie();
				const size_t maxseriesize = model.getSeriesMaxSize();
				boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>> actualdevs = getDevsSet(actualperiod);
				int action_id = 0;
				for (const Core::FMTAction& action : model.actions)
				{
					std::queue<FMTvertex_descriptor> new_actives;
					while (!actives.empty())
					{
						const FMTvertex_descriptor front_vertex = actives.front();
						actives.pop();
						const Graph::FMTGraphVertexToYield vertexinfo = getVertexToYieldInfo(model, front_vertex);
						const FMTBaseVertexProperties& front_properties = data[front_vertex];
						const Core::FMTDevelopment& active_development = front_properties.get();
						bool DoGrow = true;
						bool standOnASerie = false;
						bool operateStand = true;
						if (active_development.operable(action, model.yields, &vertexinfo) &&
							active_development.getAge() % compressageoperability == 0)
						{
							DoGrow =  action.getName() == "_DEATH" ? false : true;
							if (GOT_SERIES && _keepForSerie(front_vertex,
								(model.actions.cbegin() + action_id),
								maxseriesize,
								model,
								standOnASerie))//If true keep it for a next operability
								//If dont need to keep and not on a serie do the usual thing
								//If gotserie = true then you are on the right action of the serie...
							{
								
								new_actives.push(front_vertex);
								continue;
							}
							if (standOnASerie)
							{
								if (boost::out_degree(front_vertex, data) > 0) // If you are on a serie and you can be operated by other action just throw...
								{
									m_gotDeath = true;
									_exhandler->raise(Exception::FMTexc::FMTinvalid_action,
										std::string(front_properties.get()) +
										" is on a serie for action " + action.getName() + " and have been already operated",
										"FMTGraph::build", __LINE__, __FILE__);
								}
								_setSerieOperabilityNDeath(front_vertex, maxseriesize,
										(model.actions.cbegin() + action_id), model, 
									DoGrow, operateStand);
							}
							if (operateStand)
								{
								const std::vector<Core::FMTDevelopmentPath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
								addAction(action_id, statsDiff, new_actives, front_vertex,
									paths, actualdevs, action.isInSeries());
								}
						}
						if (DoGrow)
							{
							new_actives.push(front_vertex);
							}

					}
					actives = new_actives;
					++action_id;
				}
				const bool typeIIforestmodel = (model.getParameter(Models::FMTintmodelparameters::MATRIX_TYPE) == 2);
				newstats = naturalGrowth(actives, statsDiff, typeIIforestmodel, GOT_SERIES);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::build", __LINE__, __FILE__);
			}

			return newstats;
		}

		// DocString: FMTGraph::isNoTransfer
		/**
		@brief Return true if a vertex has no area transfer.
		@param[in] descriptor the vertex descriptor.
		@param[in] outcount the number of out edges.
		@return true if there is no transfer else false.
		*/
		bool isNoTransfer(
			const FMTvertex_descriptor& descriptor, 
			size_t outcount = 0) const
		{
			try {
				if (boost::in_degree(descriptor, data) == 1 &&
					boost::out_degree(descriptor, data) == outcount)
				{
					if (outcount < 1)
					{
						return true;
					}
					else {
						//No Death action please
						const std::map<int, int>outsvar = getOutVariables(descriptor);
						return (outsvar.find(-1) != outsvar.end());
					}
				}

			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::isNoTransfer", __LINE__, __FILE__);
			}
			return false;
		}



		// DocString: FMTGraph::getInProportion
		/**
		@brief Return the sum of the proportions of the in edges of a vertex.
		@param[in] vertex_descriptor the vertex descriptor.
		@return the in proportion.
		*/
		double getInProportion(const FMTvertex_descriptor& vertex_descriptor) const
		{
			return 1;
		}

		// DocString: FMTGraph::naturalGrowth
		/**
		@brief Add the natural growth edges and vertices for the active vertices.
		@param[in] actives the active vertices.
		@param[in] statsDiff the stats to update.
		@param[in] typeIImatrix if true builds a type II matrix.
		@return the graph stats of the newly built elements.
		*/
		FMTGraphStats naturalGrowth(
			std::queue<FMTvertex_descriptor> actives, 
			FMTGraphStats statsDiff, 
			bool typeIImatrix = false, 
			bool splitgrowth = false)
		{
			try {
				boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>> nextperiods;
				FMTvertex_iterator vertex_iterator, vertex_iterator_end, lastoperated;
				boost::tie(vertex_iterator, vertex_iterator_end) = developments.back();
				if (!std::is_pointer<FMTvertex_descriptor>::value)
				{
					FMTvertex_iterator vertex_begin;
					boost::tie(vertex_begin, lastoperated) = boost::vertices(data);
				}
				else {
					lastoperated = vertex_iterator_end;
					--lastoperated;
				}


				while (!actives.empty())
				{
					const FMTvertex_descriptor front_vertex = actives.front();
					actives.pop();
					FMTBaseVertexProperties front_properties = data[front_vertex];
					const Core::FMTDevelopment active_development = front_properties.get();
					const Core::FMTFuturDevelopment grown_up = active_development.grow();
					FMTvertex_descriptor next_period = this->addDevelopment(grown_up, nextperiods, splitgrowth); //getset
					int variableindex = statsDiff.cols;
					double proportion = 100;
					if (/*!splitgrowth &&*/ (typeIImatrix && isNoTransfer(front_vertex)))//do a type II dont need new variable
					{
						variableindex = getInVariables(front_vertex).at(0);
						proportion = getInProportion(front_vertex);
					}
					else { //We need a new variable
						++statsDiff.cols;
					}
					const FMTEdgeProperties newEdge(-1, variableindex, proportion);
					boost::add_edge(front_vertex, next_period, newEdge, data);
					++stats.edges;
				}

				if (!std::is_pointer<FMTvertex_descriptor>::value)
				{
					FMTvertex_iterator vertex_begin;
					boost::tie(vertex_begin, vertex_iterator_end) = boost::vertices(data);
				}
				else {
					++lastoperated;
				}
				developments.back() = FMTvertex_pair(vertex_iterator, lastoperated);
				developments.push_back(FMTvertex_pair(lastoperated, vertex_iterator_end));
				rebaseCache();
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::naturalGrowth", __LINE__, __FILE__);
			}
			return (statsDiff - stats);

		}

		// DocString: FMTGraph::noChoice
		/**
		@brief Return the developments of a base mask that have no action assigned other than death.
		@param[in] baseMask the base mask.
		@param[in] death_id the death action id.
		@return the developments with no choice.
		*/
		std::vector<const Core::FMTDevelopment*> noChoice(
			const Core::FMTMask& baseMask, 
			const int& death_id) const
		{
			std::vector<const Core::FMTDevelopment*>noactions;
			try {
				FMTvertex_iterator vertexit, vertexend;
				for (boost::tie(vertexit, vertexend) = developments.at(getFirstPeriod()); vertexit != vertexend; ++vertexit)
				{
					const Core::FMTDevelopment& base_dev = data[*vertexit].get();
					if (base_dev.getMask().isSubsetOf(baseMask))
					{
						std::queue<FMTvertex_descriptor>tocheck;
						FMToutedge_pair edge_pair;
						for (edge_pair = boost::out_edges(*vertexit, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
						{
							const FMTvertex_descriptor descriptor = boost::target(*edge_pair.first, data);
							tocheck.push(descriptor);
						}
						bool got_choice = false;
						while (!tocheck.empty() && !got_choice)
						{
							const FMTvertex_descriptor& source_descritor = tocheck.front();
							for (edge_pair = boost::out_edges(source_descritor, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
							{
								const FMTBaseEdgeProperties& edgeprop = data[*edge_pair.first];
								const int action_id = edgeprop.getActionID();
								if (action_id != -1 && action_id != death_id)
								{
									got_choice = true;
								}
								else {
									const FMTvertex_descriptor target_descriptor = boost::target(*edge_pair.first, data);
									tocheck.push(target_descriptor);
								}
							}
							tocheck.pop();
						}
						if (!got_choice)
						{
							noactions.push_back(&base_dev);
						}
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::noChoice", __LINE__, __FILE__);
			}
			return noactions;

		}



		// DocString: FMTGraph::getVariableNames
		/**
		@brief Fill the column names of the matrix with the variable names of the graph.
		@param[in] actions the model actions.
		@param[in,out] colnames the column names.
		*/
		void getVariableNames(
			const std::vector<Core::FMTAction>& actions,
			std::vector<std::string>& colnames) const
		{
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				const std::string toRemove = "+-/*";
				size_t vertices_id = 0;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					std::string basename = std::string(Core::FMTDevelopment(data[*vertex_iterator].get()));
					const std::string vertexid = "_V" + std::to_string(vertices_id);
					boost::remove_erase_if(basename, boost::is_any_of(toRemove));
					FMToutedge_iterator outit, outend;
					for (boost::tie(outit, outend) = boost::out_edges(*vertex_iterator, data); outit != outend; ++outit)
					{
						std::string actionname = "EVO";
						const int variableid = data[*outit].getVariableID();
						if (colnames.at(variableid).empty())
						{
							const int actionid = data[*outit].getActionID();
							if (actionid >= 0)
							{
								actionname = actions.at(actionid).getName();
							}
							colnames[variableid] = basename + actionname + vertexid;
						}
					}
					++vertices_id;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getVariableNames", __LINE__, __FILE__);
			}
		}

		/*void cleannodecaching(unsigned long long minbytes= 10000000000) const//5 Go max
		{
			try {
				if (!nodescache.empty()&& boost::num_vertices(data)>1000000)
				{
					unsigned long long potsize = 0;
					for (reversecachenodeit cacheit = nodescache.rbegin(); cacheit != nodescache.rend(); ++cacheit)
					{
						potsize += cacheit->getpotentialsize();
					}
					if (potsize>= minbytes)
					{
						unsigned long long avmemory = FMTObject::getAvailableMemory();
						if (avmemory < minbytes)
						{
							unsigned long long rest = (minbytes - avmemory);
							unsigned long long totalclean = 1;
							while (totalclean >= 1 && rest > 0)
							{
								totalclean = 0;
								for (reversecachenodeit cacheit = nodescache.rbegin(); cacheit != nodescache.rend(); ++cacheit)
								{
									const unsigned long long lastclean = cacheit->removeLargest();
									rest -= lastclean;
									totalclean += lastclean;
									if (lastclean > 0)
									{
										break;
									}

								}
							}
						}
					}
				}
			}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTGraph::cleannodecaching", __LINE__, __FILE__);
			}
		}*/

		// DocString: FMTGraph::getTransferRowNames
		/**
		@brief Fill the row names of the matrix with the area transfer row names of the graph.
		@param[in,out] rownames the row names.
		*/
		void getTransferRowNames(std::vector<std::string>& rownames) const
		{
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				const std::string toRemove = "+-/*";
				size_t vertices_id = 0;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const int rowid = data[*vertex_iterator].getConstraintId();
					if (rowid >= 0)
					{
						const std::string vertexid = "_V" + std::to_string(vertices_id);
						std::string name = std::string(Core::FMTDevelopment(data[*vertex_iterator].get()));
						boost::remove_erase_if(name, boost::is_any_of(toRemove));
						rownames[rowid] = name + vertexid;
					}
					++vertices_id;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getTransferRowNames", __LINE__, __FILE__);
			}
		}


		// DocString: FMTGraph::getPeriodStopDev
		/**
		@brief Return the developments present at the end of a period based on the solution.
		@param[in] location the period.
		@param[in] actual_solution the solution.
		@return the actual developments.
		*/
		std::vector<Core::FMTActualDevelopment> getPeriodStopDev(
			const int location, 
			const double* actual_solution) const
		{
			std::vector<Core::FMTActualDevelopment>all_period_stop_devs;
			try {
				FMTvertex_iterator vertexit, vertexend;
				for (boost::tie(vertexit, vertexend) = developments.at(location); vertexit != vertexend; ++vertexit)
				{
					if (periodStop(*vertexit))
					{
						const double area = inArea(*vertexit, actual_solution);
						const FMTBaseVertexProperties& vetexprop = data[*vertexit];
						all_period_stop_devs.push_back(Core::FMTActualDevelopment(vetexprop.get(), area));
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getPeriodStopDev", __LINE__, __FILE__);
			}
			return all_period_stop_devs;

		}
		// DocString: FMTGraph::getOutput
		/**
		@brief Return the output value of a period using the solution.
		@param[in] model the model.
		@param[in] output the output.
		@param[in] period the period.
		@param[in] solution the solution.
		@param[in] level the output level.
		@return a map of names to values.
		*/
		std::map<std::string, double> getOutput(
			const Models::FMTModel& model, 
			const Core::FMTOutput& output,
			int period, 
			const double* solution, 
			Core::FMToutputlevel level = Core::FMToutputlevel::standard) const
		{
			Core::FMTTheme targetTheme;
			std::vector<std::string> target_attributes;
			std::map<std::string, double> results;
			try {
				if (output.targetThemeId() < 0 && !(level == Core::FMToutputlevel::developpement))
				{
					level = Core::FMToutputlevel::totalonly;
				}
				if (level != Core::FMToutputlevel::developpement)
				{
					if (level == Core::FMToutputlevel::standard)
					{
						target_attributes = output.getDecomposition(model.themes);
						if (!target_attributes.empty() && level == Core::FMToutputlevel::standard)
						{
							targetTheme = output.targetTheme(model.themes);
						}
					}
					target_attributes.push_back("Total");
					for (const std::string& attribute : target_attributes)
					{
						results[attribute] = 0;
					}
				}
				std::vector<std::string> equation;
				if (output.canBeNodesOnly())
				{
					for (const Core::FMTOutputNode& output_node : output.getNodes(equation))
					{
						bool checkGraph = true;
						if (output_node.source.isAction())
						{
							const std::vector<const Core::FMTAction*> ACTIONS = Core::FMTActionComparator(output_node.source.getAction()).getAllAggregates(model.actions);
							std::vector<const Core::FMTAction*>::const_iterator AIt = ACTIONS.begin();
							size_t count = 0;
							while (AIt != ACTIONS.end())
							{
								if ((*AIt)->notUse())
								{
									++count;
								}
								++AIt;
							}
							checkGraph = !(count == ACTIONS.size());
						}
						if (checkGraph)
						{
							const std::map<std::string, double> srcvalues = getSource(model, output_node, period, targetTheme, solution, level);
							if (level == Core::FMToutputlevel::developpement)
							{
								for (std::map<std::string, double>::const_iterator mit = srcvalues.begin(); mit != srcvalues.end(); mit++)
								{
									if (results.find(mit->first) == results.end())
									{
										results[mit->first] = 0;
									}
									results[mit->first] += mit->second;
								}
							}
							else {
								for (const std::string& attribute : target_attributes)
								{
									results[attribute] += srcvalues.at(attribute);
								}
							}
						}
						else {
							for (const std::string& attribute : target_attributes)
							{
								results[attribute] += 0.0;
							}

						}
					}
				}
				else {
					std::map<std::string, std::vector<std::string>> allequations;
					const std::vector<Core::FMTOutputNode> allnodes = output.getNodes(equation, 1, false, period);


					if (allnodes.empty())
					{
						if (level == Core::FMToutputlevel::developpement)
						{
							_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
								"Cannot get level values by developement",
								"FMTGraph::getOutput", __LINE__, __FILE__);
						}
						allequations["Total"] = equation;

					}
					else {
						size_t outid = 0;
						for (const Core::FMTOutputNode& output_node : allnodes)
						{
							const std::map<std::string, double> srcvalues = getSource(model, output_node, period, targetTheme, solution, level);
							output_node.fillUpEquation(allequations, srcvalues, equation, outid);
							++outid;
						}
					}
					output.fillFromShuntingYard(equation, results, allnodes, allequations);
				}

			}
			catch (...)
			{
				_exhandler->raiseFromCatch("For output: " + std::string(output), "FMTGraph::getOutput", __LINE__, __FILE__);
			}

			return results;

		}

		// DocString: FMTGraph::getDevelopment(const Core::FMTDevelopment&,...)
		/**
		@brief Return the vertex descriptor of a development from the lookup set.
		@param[in] development the development.
		@param[in] devsets the developments lookup set.
		@return the vertex descriptor.
		*/
		FMTvertex_descriptor getDevelopment(
			const Core::FMTDevelopment& development,
			const boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, 
			Core::FMTDevelopment>>& alldevs) const
		{
			const Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment> tofind(development);
			return alldevs.find(tofind)->memoryobject;
		}

		// DocString: FMTGraph::getDevelopment(const FMTvertex_descriptor&)
		/**
		@brief Return the development held by a vertex.
		@param[in] descriptor the vertex descriptor.
		@return the development.
		*/
		const Core::FMTDevelopment& getDevelopment(const FMTvertex_descriptor& descriptor) const
		{
			return data[descriptor].get();
		}

		// DocString: FMTGraph::addDevelopment(const Core::FMTFuturDevelopment&,...)
		/**
		@brief Add a futur development to the graph and to the lookup set, or return the existing vertex.
		@param[in] futurdevelopment the futur development.
		@param[in,out] devsets the developments lookup set.
		@return the vertex descriptor.
		*/
		FMTvertex_descriptor addDevelopment(
			const Core::FMTFuturDevelopment& futurdevelopment,
			boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>>& alldevs, 
			bool forcenewone = false)
		{
			try {
				if (!this->containsDevelopment(futurdevelopment, alldevs) || forcenewone)
				{
					const int constraint_id = -1;
					const FMTVertexProperties properties(futurdevelopment, constraint_id);
					if (data.m_vertices.capacity() < data.m_vertices.size() + 1)
					{
						if (data.m_vertices.capacity() > 1)
						{
							_logger->logWithLevel("Reallocation occurred for (" + std::to_string(data.m_vertices.capacity()) + ") vertices\n", 1);
						}
						data.m_vertices.reserve(data.m_vertices.size() * 2);
						alldevs.clear();
						FMTvertex_iterator base_iterator, base_iterator_end;
						boost::tie(base_iterator, base_iterator_end) = boost::vertices(data);
						while (base_iterator != base_iterator_end)
						{
							const Core::FMTDevelopment& DEV = getDevelopment(*base_iterator);
							if (DEV.getPeriod() == futurdevelopment.getPeriod())
							{
								alldevs.insert(Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>(*base_iterator, data[*base_iterator].get()));
							}
							++base_iterator;
						}

					}
					FMTvertex_descriptor newvertex = boost::add_vertex(properties, data);
					alldevs.insert(Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>(newvertex, data[newvertex].get()));
					++stats.vertices;
					return newvertex;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::addDevelopment", __LINE__, __FILE__);
			}
			return getDevelopment(futurdevelopment, alldevs);

		}

		// DocString: FMTGraph::addDevelopment(const Core::FMTFuturDevelopment&)
		/**
		@brief Add a futur development to the graph.
		@param[in] futurdevelopment the futur development.
		@return the vertex descriptor.
		*/
		FMTvertex_descriptor addDevelopment(const Core::FMTFuturDevelopment& futurdevelopment)
		{
			FMTvertex_descriptor newvertex = data.null_vertex();
			try {
				const int constraint_id = -1;
				const FMTVertexProperties properties(futurdevelopment, constraint_id);
				newvertex = boost::add_vertex(properties, data);
				++stats.vertices;
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::addDevelopment", __LINE__, __FILE__);
			}
			return newvertex;
		}

		// DocString: FMTGraph::hash
		/**
		@brief Hash the graph.
		@param[in] seed the seed.
		@return the hash of the graph.
		*/
		size_t hash(size_t seed = 0) const
		{
			try {
				boost::hash_combine(seed, boost::hash<Core::FMTDevelopment>()(data[*developments.at(0).first].get()));
				FMTedge_iterator edge_iterator, edge_iterator_end;
				for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*edge_iterator];
					boost::hash_combine(seed, edgeprop.getActionID());
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::hash", __LINE__, __FILE__);
			}
			return seed;

		}
		// DocString: FMTGraph::addAction(...,devsets,inserie)
		/**
		@brief Add an action edge and its target vertices to the graph, reusing the developments of the lookup set.
		@param[in] actionID the action id.
		@param[in,out] statsDiff the stats to update.
		@param[in,out] actives the active vertices.
		@param[in] out_vertex the source vertex.
		@param[in] paths the development paths.
		@param[in,out] devsets the developments lookup set.
		@param[in] inserie if true the action is part of a serie.
		*/
		void addAction(const int& actionID,
			FMTGraphStats& statsDiff,
			std::queue<FMTvertex_descriptor>& actives,
			const FMTvertex_descriptor& out_vertex,
			const std::vector<Core::FMTDevelopmentPath>& paths,
			boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>>& devsets,
			bool inserie = false)
		{
			try {
				//int variable_id = statsDiff.cols;
				//++statsDiff.cols;
				//std::vector<FMTvertex_descriptor>active_vertex;
				bool newchoice = false;
				for (const Core::FMTDevelopmentPath& devpath : paths)
				{
					FMTvertex_descriptor tovertex;
					bool newEdge = true;
					if (!this->containsDevelopment(devpath.getDevelopment(), devsets))
					{

						tovertex = this->addDevelopment(devpath.getDevelopment(), devsets);
						actives.push(tovertex);
						//newEdge = true;
					}
					else {
						tovertex = this->addDevelopment(devpath.getDevelopment(), devsets);
						if (inserie || isDependant(tovertex, actionID, newEdge))
						{
							tovertex = this->addDevelopment(devpath.getDevelopment(), devsets, true);
							actives.push(tovertex);
						}
					}
					if (newEdge)
					{
						const FMTEdgeProperties newEdge(actionID, statsDiff.cols, devpath.getProportion());
						boost::add_edge(out_vertex, tovertex, newEdge, data);
						++stats.edges;
						newchoice = true;
					}

				}
				if (newchoice)
				{
					++statsDiff.cols;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("On action Id " + std::to_string(actionID), "FMTGraph::addAction", __LINE__, __FILE__);
			}
		}

		// DocString: FMTGraph::addAction(...,paths)
		/**
		@brief Add an action edge and its target vertices to the graph.
		@param[in] actionID the action id.
		@param[in,out] statsDiff the stats to update.
		@param[in,out] actives the active vertices.
		@param[in] out_vertex the source vertex.
		@param[in] paths the development paths.
		*/
		void addAction(const int& actionID,
			FMTGraphStats& statsDiff,
			std::queue<FMTvertex_descriptor>& actives,
			const FMTvertex_descriptor& out_vertex,
			const std::vector<Core::FMTDevelopmentPath>& paths)
		{
			try {
				int variable_id = statsDiff.cols;
				++statsDiff.cols;
				std::vector<FMTvertex_descriptor>active_vertex;
				for (const Core::FMTDevelopmentPath& devpath : paths)
				{
					const FMTEdgeProperties newEdge(actionID, variable_id, devpath.getProportion());
					const FMTvertex_descriptor tovertex = this->addDevelopment(devpath.getDevelopment());
					actives.push(tovertex);
					boost::add_edge(out_vertex, tovertex, newEdge, data);
					++stats.edges;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::addAction", __LINE__, __FILE__);
			}
		}

		// DocString: FMTGraph::outArea
		/**
		@brief Return the area leaving a vertex through an action, based on the solution.
		@param[in] out_vertex the vertex descriptor.
		@param[in] actionID the action id.
		@param[in] solution the solution.
		@return the out area.
		*/
		double outArea(
			const FMTvertex_descriptor& out_vertex,
			const int& actionID,
			const double*& solution) const
		{
			double value = 0;
			try {
				FMToutedge_iterator outedge_iterator, outedge_end;

				for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*outedge_iterator];
					if (edgeprop.getActionID() == actionID)
					{

						value += *(solution + edgeprop.getVariableID()) * (edgeprop.getProportion() / 100);
					}
				}

			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::outArea", __LINE__, __FILE__);
			}
			return value;
		}

		// DocString: FMTGraph::getGrowthSource
		/**
		@brief Return the source vertex of the natural growth edge of a vertex.
		@param[in] out_vertex the vertex descriptor.
		@return the growth source vertex.
		*/
		FMTvertex_descriptor getGrowthSource(const FMTvertex_descriptor& out_vertex) const
		{
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*inedge_iterator];
					if (edgeprop.getActionID() < 0)
					{
						return boost::source(*inedge_iterator, data);
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getGrowthSource", __LINE__, __FILE__);
			}
			return FMTvertex_descriptor();
		}

		// DocString: FMTGraph::getActionSources
		/**
		@brief Return the source vertices of an action for a vertex.
		@param[in] out_vertex the vertex descriptor.
		@param[in] actionid the action id.
		@return the source vertices.
		*/
		std::vector<FMTvertex_descriptor> getActionSources(
			const FMTvertex_descriptor& out_vertex, 
			const int& actionid) const
		{
			std::vector<FMTvertex_descriptor> vsources;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*inedge_iterator];
					if (edgeprop.getActionID() == actionid)
					{
						vsources.push_back(boost::source(*inedge_iterator, data));
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getActionSources", __LINE__, __FILE__);
			}
			return vsources;
		}

		// DocString: FMTGraph::inArea
		/**
		@brief Return the area entering a vertex based on the solution.
		@param[in] out_vertex the vertex descriptor.
		@param[in] solution the solution.
		@param[in] actionid the action id, -1 for all actions.
		@param[in] growth if true only considers the natural growth.
		@return the in area.
		*/
		double inArea(
			const FMTvertex_descriptor& out_vertex,
			const double*& solution, 
			int actionid = -1, 
			bool growth = false) const
		{
			double area = 0;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*inedge_iterator];
					if (edgeprop.getActionID() == actionid || !growth)
					{
						area += *(solution + edgeprop.getVariableID()) * (edgeprop.getProportion() / 100);
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::inArea", __LINE__, __FILE__);
			}
			return area;
		}

		// DocString: FMTGraph::periodStart
		/**
		@brief Return true if a vertex is at the start of a period.
		@param[in] out_vertex the vertex descriptor.
		@return true if the vertex starts a period else false.
		*/
		bool periodStart(const FMTvertex_descriptor& out_vertex) const
		{
			try {
				return isPeriodStart(out_vertex, data);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::periodStart", __LINE__, __FILE__);
			}
			return false;
		}

		// DocString: FMTGraph::onlyPeriodStart
		/**
		@brief Return true if a vertex is only at the start of a period with no other in edge.
		@param[in] out_vertex the vertex descriptor.
		@return true if the vertex only starts a period else false.
		*/
		bool onlyPeriodStart(const FMTvertex_descriptor& out_vertex) const
		{
			try {
				if (boost::in_degree(out_vertex, data) == 1)
				{
					FMTinedge_iterator inedge_iterator, inedge_end;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
					{
						const FMTBaseEdgeProperties& edgeprop = data[*inedge_iterator];
						if (edgeprop.getActionID() == -1)
						{
							return true;
						}
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::onlyPeriodStart", __LINE__, __FILE__);
			}
			return false;

		}

		// DocString: FMTGraph::getMaximalLock
		/**
		@brief Return the maximal lock of a period.
		@param[in] period the period.
		@return the maximal lock.
		*/
		int getMaximalLock(const int& period)
		{
			int lock = 0;
			try {
				FMTvertex_iterator vertexit, vertexend;
				for (boost::tie(vertexit, vertexend) = developments.at(period); vertexit != vertexend; ++vertexit)
				{
					const Core::FMTDevelopment& dev = data[*vertexit].get();
					if (dev.getLock() > lock)
					{
						lock = dev.getLock();
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getMaximalLock", __LINE__, __FILE__);
			}
			return lock;
		}

		// DocString: FMTGraph::periodStop
		/**
		@brief Return true if a vertex is at the end of a period.
		@param[in] out_vertex the vertex descriptor.
		@return true if the vertex stops a period else false.
		*/
		bool periodStop(const FMTvertex_descriptor& out_vertex) const
		{
			try {
				FMToutedge_iterator outedge_iterator, outedge_end;
				const FMTBaseVertexProperties& source_properties = data[out_vertex];
				if ((source_properties.get().getPeriod()) == 0)
				{
					return true;
				}
				for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*outedge_iterator];
					if (edgeprop.getActionID() == -1)
					{
						return true;
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::periodStop", __LINE__, __FILE__);
			}
			return false;
		}

		// DocString: FMTGraph::getPaths
		/**
		@brief Return the development paths generated by an action on a vertex.
		@param[in] out_vertex the vertex descriptor.
		@param[in] actionID the action id.
		@return the development paths.
		*/
		std::vector<Core::FMTDevelopmentPath> getPaths(
			const FMTvertex_descriptor& out_vertex,
			const int& actionID) const
		{
			std::vector<Core::FMTDevelopmentPath>paths;
			try {
				paths.reserve(boost::out_degree(out_vertex, data));
				for (FMToutedge_pair edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*edge_pair.first];
					if (edgeprop.getActionID() == actionID)
					{
						const FMTBaseVertexProperties& vertex_target = data[target(*edge_pair.first, data)];
						paths.push_back(Core::FMTDevelopmentPath(vertex_target.get().getMask(),
							vertex_target.get().getAge(),
							vertex_target.get().getLock(),
							vertex_target.get().getPeriod(),
							edgeprop.getProportion()));
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getPaths", __LINE__, __FILE__);
			}
			return paths;
		}

		// DocString: FMTGraph::isValidOuputNode
		/**
		@brief Return true if an output node is valid for the graph.
		@param[in] model the model.
		@param[in] node the output node.
		@param[in] actions the selected actions.
		@return true if the output node is valid else false.
		*/
		bool isValidOuputNode(
			const Models::FMTModel& model, 
			const Core::FMTOutputNode& node, 
			std::vector<const Core::FMTAction*>& action_IDS, 
			int period) const
		{
			try {
				if (static_cast<int>(developments.size()) > period)
				{
					action_IDS = node.source.targets(model.actions);
					if (!(period == 0 && !action_IDS.empty()) && !node.source.isNull(model.yields) && !node.factor.isNull(model.yields) && ((!action_IDS.empty() && !node.source.getAction().empty()) || node.source.getAction().empty()))
					{
						return true;
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::isValidOuputNode", __LINE__, __FILE__);
			}
			return false;
		}


		// DocString: FMTGraph::isValidGraphNode
		/**
		@brief Return true if a vertex falls within an output node description.
		@param[in] p_model the model.
		@param[in] p_vertex_descriptor the vertex descriptor.
		@param[in] p_node the output node.
		@return true if the vertex is valid for the node else false.
		*/
		bool isValidGraphNode(
			const Models::FMTModel& p_model, 
			const FMTvertex_descriptor& p_vertex_descriptor,
			const Core::FMTOutputNode& p_node, 
			const std::vector<const Core::FMTAction*>& p_selected, 
			const std::vector<size_t>& p_blocksSubset) const
		{
			try {
				const Core::FMTDevelopment& p_development = data[p_vertex_descriptor].get();
				const Core::FMTMask& DEV_MASK = p_development.getMask();
				const Core::FMTMask& SOURCE_MASK = p_node.source.getMask();
				if (DEV_MASK.isSubsetOf(SOURCE_MASK, p_blocksSubset)&&
					p_development.is(p_node.source, p_model.yields))//node.source.use(development, model.yields))
				{
					if (p_node.source.useInEdges())
					{
						bool returnedValue = false;
						if ((p_development.getPeriod() == 0 || p_node.source.isAction() || periodStart(p_vertex_descriptor)))
						{
							if (p_selected.empty() && (p_node.source.isNextPeriod() || !p_node.source.emptyLock()))
							{
								returnedValue = true;
							}
							else if (buildtype == FMTgraphbuild::schedulebuild)
							{
								const Graph::FMTGraphVertexToYield YIELD_INFO = getVertexToYieldInfo(p_model, p_vertex_descriptor);
								returnedValue = p_development.anyOperable(p_selected, p_model.yields, &YIELD_INFO);
							}
							else if (isAnyOperables(p_vertex_descriptor, p_development.isAnyWorthTestingOperability(p_selected, p_model.actions)))
							{
								returnedValue = true;
							}
						}
						return returnedValue;
					}
					else //out edges
					{
						return isAnyOperables(p_vertex_descriptor, p_development.isAnyWorthTestingOperability(p_selected, p_model.actions));
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::isValidGraphNode", __LINE__, __FILE__);
			}
			return false;
		}

		// DocString: FMTGraph::locateNode
		/**
		@brief Return the matrix variables and coefficients matching an output node for a period.
		@param[in] model the model.
		@param[in] output_node the output node.
		@param[in] period the period.
		@return the variables and coefficients.
		*/
		std::map<int, double> locateNode(
			const Models::FMTModel& model,
			Core::FMTOutputNode output_node,
			int period) const
		{
			try {
				Core::FMTOutputNode tempNode(output_node);
				const std::vector<FMTvertex_descriptor>& vertices = getNode(model, tempNode, period);
				return getVariables(model, tempNode, vertices);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("Getting node at period "+std::to_string(period)+" for node "+
					std::string(output_node)+" for model "+model.getName(), "FMTGraph::locateNode", __LINE__, __FILE__);
			}
			return std::map<int, double>();

		}
		// DocString: FMTGraph::locateNodeByTheme
		/**
		@brief Return the matrix variables and coefficients matching an output node for a period, grouped by theme attribute.
		@param[in] model the model.
		@param[in] output_node the output node.
		@param[in] period the period.
		@return the variables and coefficients per theme attribute.
		*/
		std::map<std::string, std::map<int, double>> locateNodeByTheme(
			const Models::FMTModel& model, 
			Core::FMTOutputNode output_node, 
			int period) const
		{
			std::map<std::string, std::map<int, double>> variablesreturn;
			//std::vector<std::map<int,double>> variablesreturn;
			try {
				const std::vector<FMTvertex_descriptor> vertices = getNode(model, output_node, period);
				int themetarget = output_node.source.getThemeTarget();
				if (themetarget < 0)
				{
					variablesreturn["~nothemetargetid~"] = getVariables(model, output_node, vertices);
					//variablesreturn.push_back(getVariables(model, output_node, vertices));
				}
				else {
					std::map<std::string, std::vector<FMTvertex_descriptor>> orderedtarget;
					for (const auto& vert : vertices)
					{
						const Core::FMTDevelopment& dev = getDevelopment(vert);
						const std::string value = dev.getMask().get(model.themes.at(themetarget));
						if (orderedtarget.find(value) == orderedtarget.end())
						{
							orderedtarget[value] = std::vector<FMTvertex_descriptor>();
						}
						orderedtarget[value].push_back(vert);
					}
					for (const auto& odtar : orderedtarget)
					{
						variablesreturn[odtar.first] = getVariables(model, output_node, odtar.second);
						//variablesreturn.push_back(getVariables(model,output_node,odtar.second));
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::locateNodeByTheme", __LINE__, __FILE__);
			}
			return variablesreturn;

		}


		// DocString: FMTGraph::setNodeSize
		/**
		@brief Set the reserved size of the output node cache.
		@param[in] p_size the size to reserve.
		*/
		void setNodeSize(const size_t& p_size)
		{
			m_reserve = p_size;
			m_selectedVertices.reserve(m_reserve);
		}

		// DocString: FMTGraph::getNode
		/**
		@brief Return the vertices matching an output node for a period.
		@param[in] model the model.
		@param[in,out] output_node the output node.
		@param[in] period the period.
		@return the matching vertices.
		*/
		std::vector<FMTvertex_descriptor> getNode(
			const Models::FMTModel& model, 
			Core::FMTOutputNode& output_node, 
			int period) const
		{
			//std::vector<FMTvertex_descriptor>locations(m_allocator);
			//locations.reserve(m_reserve);
			m_selectedVertices.clear();
			try {
				std::vector<int> targetedperiods(m_allocator);

				targetedperiods.reserve(size() - 2);
	
				const int maxperiod = static_cast<int> (size() - 2);
				const int node_period = output_node.setToGraph(targetedperiods, period, maxperiod);
				//*_logger << "node of " << node_period<<"dev size "<< developments.size() << "\n";
				if (node_period < 0)
				{
					//return locations;
					return m_selectedVertices;
				}
				if (!output_node.source.isVariableLevel())
				{
					//constexpr size_t minimalcachedrop = 25;//10 %
					std::vector<const Core::FMTAction*> selected(m_allocator);
					selected.reserve(model.actions.size());
					bool useCache = true;
					//const size_t TO_RESERVE = model.area.size() * node_period * node_period * 100;

					if (isValidOuputNode(model, output_node, selected, node_period))
					{
						//locations.reserve(m_reserve);
						if (useCache)
						{
							if (nodescache.empty())
							{
								nodescache.reserve(developments.size());
							}while (nodescache.size() != developments.size())
							{
								const size_t devindex = nodescache.size();
								nodescache.emplace_back(developments.at(devindex).first, developments.at(devindex).second, m_allocator, m_reserve);
							}
						}

						//cleannodecaching();
						const bool gotstaticnode = model.isStaticNode(output_node);
						//Pre-allocate to use the same vector each time to reduce memory allocation
						//////////////////////////////////////////////////////////////////
						//std::vector<FMTvertex_descriptor>staticdescriptors(m_allocator);
						//staticdescriptors.reserve(m_reserve);
						//std::vector<FMTvertex_descriptor>periodlocations(m_allocator);
						//periodlocations.reserve(m_reserve);
						//////////////////////////////////////////////////////////////////
						const std::vector<size_t>NON_FULL_BLOCKS = output_node.source.getMask().getNonFullBlocks();
						for (const int& localnodeperiod : targetedperiods)
						{
							std::vector<FMTvertex_descriptor>staticdescriptors(m_allocator);
							
							//staticdescriptors.clear();
							std::vector<FMTvertex_descriptor> const* descriptors = nullptr;
							bool exactvertices = false;
							if (gotstaticnode)
							{
								staticdescriptors = setNodeByStaticMask(model, output_node, localnodeperiod, useCache);//, TO_RESERVE);
								descriptors = &staticdescriptors;
							}
							else if (useCache) {

								descriptors = &nodescache.at(localnodeperiod).getVertices(output_node, model.actions, model.themes, exactvertices);
							}
							else {
								FMTvertex_iterator vertex_iterator, vertex_iterator_end;
								staticdescriptors.reserve(m_reserve);
								for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(localnodeperiod); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
								{
									staticdescriptors.push_back(*vertex_iterator);
								}
								descriptors = &staticdescriptors;
							}
							if (exactvertices)
							{
								//locations.reserve(locations.size() + descriptors->size());
								//locations.insert(locations.end(), descriptors->begin(), descriptors->end());
								m_selectedVertices.reserve(m_selectedVertices.size() + descriptors->size());
								m_selectedVertices.insert(m_selectedVertices.end(), descriptors->begin(), descriptors->end());
							}
							else {
								std::vector<FMTvertex_descriptor>periodlocations(m_allocator);
								periodlocations.reserve(m_reserve);
								//periodlocations.clear();
								for (const FMTvertex_descriptor& potential : *descriptors)
								{
									if (isValidGraphNode(model, potential, output_node, selected, NON_FULL_BLOCKS))
									{
										//locations.push_back(potential);
										m_selectedVertices.push_back(potential);
										periodlocations.push_back(potential);
									}
								}
								//*_logger << "done on " << periodlocations.size() << "\n";
								std::sort(periodlocations.begin(), periodlocations.end());
								if (useCache && !gotstaticnode)
								{
									nodescache.at(localnodeperiod).setValidVertices(output_node, periodlocations);
								}
							}
						}


					}

				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getNode", __LINE__, __FILE__);
			}
			//std::sort(locations.begin(), locations.end());
			//return locations;
			std::sort(m_selectedVertices.begin(), m_selectedVertices.end());
			return m_selectedVertices;
		}

		// DocString: FMTGraph::getVariables
		/**
		@brief Return the matrix variables and coefficients of an output node for a set of vertices.
		@param[in] model the model.
		@param[in] output_node the output node.
		@param[in] verticies the vertices.
		@return the variables and coefficients.
		*/
		std::map<int, double> getVariables(
			const Models::FMTModel& model, 
			const Core::FMTOutputNode& output_node, 
			const std::vector<FMTvertex_descriptor>& vertices) const
		{
			return std::map<int, double>();
		}

		// DocString: FMTGraph::isAnyOperables
		/**
		@brief Return true if any of the given actions is operable on a vertex.
		@param[in] descriptor the vertex descriptor.
		@param[in] actionsop the operable actions.
		@return true if any action is operable else false.
		*/
		bool isAnyOperables(
			const FMTvertex_descriptor& descriptor, 
			const std::vector<bool>& actionsop) const noexcept
		{
			//try {
			if (!actionsop.empty())
			{
				FMToutedge_pair edge_pair;
				for (edge_pair = boost::out_edges(descriptor, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*edge_pair.first];
					const int actionid = edgeprop.getActionID();
					if (actionid >= 0 && actionsop.at(actionid))
					{
						return true;
					}
				}
			}

			/*}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTGraph::isAnyOperables", __LINE__, __FILE__);
			}*/
			return false;
		}

		// DocString: FMTGraph::anyOperables
		/**
		@brief Return true if any of the given action ids is operable on a vertex.
		@param[in] descriptor the vertex descriptor.
		@param[in] action_ids the action ids.
		@return true if any action is operable else false.
		*/
		bool anyOperables(
			const FMTvertex_descriptor& descriptor, 
			const std::vector<int>& action_ids) const
		{
			try {
				if (!action_ids.empty())
				{
					std::map<int, int> variables = getOutVariables(descriptor);
					for (const int& id : action_ids)
					{
						if (variables.find(id) != variables.end())
						{
							return true;
						}
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::anyOperables", __LINE__, __FILE__);
			}
			return false;
		}

		// DocString: FMTGraph::getInVariables
		/**
		@brief Return the matrix variables of the in edges of a vertex.
		@param[in] out_vertex the vertex descriptor.
		@return the in variables.
		*/
		std::vector<int>getInVariables(const FMTvertex_descriptor& out_vertex) const
		{
			std::vector<int>invars;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*inedge_iterator];
					invars.push_back(edgeprop.getVariableID());

				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getInVariables", __LINE__, __FILE__);
			}
			return invars;
		}

		// DocString: FMTGraph::getInProportions
		/**
		@brief Return the proportions of the in edges of a vertex.
		@param[in] out_vertex the vertex descriptor.
		@return the in proportions.
		*/
		std::vector<double>getInProportions(const FMTvertex_descriptor& out_vertex) const
		{
			std::vector<double>inprops;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*inedge_iterator];
					inprops.push_back(edgeprop.getProportion());

				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getInProportions", __LINE__, __FILE__);
			}
			return inprops;
		}

		// DocString: FMTGraph::getInIdsVariables
		/**
		@brief Return the action ids and matrix variables of the in edges of a vertex.
		@param[in] out_vertex the vertex descriptor.
		@return the action ids and variables.
		*/
		std::map<int, int>getInIdsVariables(const FMTvertex_descriptor& out_vertex) const
		{
			std::map<int, int> mapping;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*inedge_iterator];
					int actionid = edgeprop.getActionID();
					//*_logger << actionid << " test "<< edgeprop.getVariableID() <<"\n";
					mapping[actionid] = edgeprop.getVariableID();
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getInIdsVariables", __LINE__, __FILE__);
			}
			return mapping;
		}

		// DocString: FMTGraph::getOutVariables
		/**
		@brief Return the action ids and matrix variables of the out edges of a vertex.
		@param[in] out_vertex the vertex descriptor.
		@return the action ids and variables.
		*/
		std::map<int, int> getOutVariables(const FMTvertex_descriptor& out_vertex) const
		{
			std::map<int, int> mapping;
			try {
				FMToutedge_pair edge_pair;
				for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*edge_pair.first];
					int actionid = edgeprop.getActionID();
					mapping[actionid] = edgeprop.getVariableID();
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getOutVariables", __LINE__, __FILE__);
			}
			return mapping;
		}

		// DocString: FMTGraph::getActionEdges
		/**
		@brief Return the out edges of a vertex for an action.
		@param[in] pOutVertex the vertex descriptor.
		@param[in] p_actionId the action id.
		@return the action edges.
		*/
		std::vector<const FMTBaseEdgeProperties*> getActionEdges(
			const FMTvertex_descriptor& pOutVertex,
			const size_t& p_actionSize) const
		{
			std::vector<const FMTBaseEdgeProperties*>output(p_actionSize, nullptr);
			try {
				FMToutedge_pair edge_pair;
				for (edge_pair = boost::out_edges(pOutVertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
				{
					const FMTBaseEdgeProperties& edgeprop = data[*edge_pair.first];
					const int actionId = edgeprop.getActionID();
					if (actionId >= 0)
					{
						output[edgeprop.getActionID()] = &edgeprop;
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getOutVariables", __LINE__, __FILE__);
			}
			return output;
		}

		// DocString: FMTGraph::getOutActions
		/**
		@brief Return the action ids of the out edges of a vertex.
		@param[in] out_vertex the vertex descriptor.
		@param[in] p_multipleEdges if true keeps the duplicated actions.
		@return the out action ids.
		*/
		std::vector<int> getOutActions(
			const FMTvertex_descriptor& out_vertex, 
			bool p_multipleEdges = true) const
		{
			std::vector<int>actions;
			try {
				const size_t outsize = boost::out_degree(out_vertex, data);
				if (outsize > 1 || !p_multipleEdges)
				{
					actions.reserve(outsize);
					FMToutedge_pair edge_pair;
					for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
					{
						const FMTBaseEdgeProperties& edgeprop = data[*edge_pair.first];
						const int actionid = edgeprop.getActionID();
						if ((actionid) >= 0)
						{
							actions.emplace_back(static_cast<int>(actionid));
						}
					}
					std::sort(actions.begin(), actions.end());
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getOutActions", __LINE__, __FILE__);
			}
			return actions;
		}

		// DocString: FMTGraph::selectedActions
		/**
		@brief Return the model actions matching a set of action ids.
		@param[in] model the model.
		@param[in] action_IDS the action ids.
		@return the selected actions.
		*/
		std::vector<const Core::FMTAction*> selectedActions(
			const Models::FMTModel& model, 
			const std::vector<int>& action_IDS) const
		{
			std::vector<const Core::FMTAction*>selected;
			try {
				if (!action_IDS.empty())
				{
					selected.resize(action_IDS.size());
					int aid = 0;
					for (const int& actid : action_IDS)
					{
						selected[aid] = &model.actions[actid];
						++aid;
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::selectedActions", __LINE__, __FILE__);
			}
			return selected;
		}

		// DocString: FMTGraph::constraintLenght
		/**
		@brief Return the first and last period covered by a constraint in the graph.
		@param[in] constraint the constraint.
		@param[out] start the first period.
		@param[out] stop the last period.
		@return true if the constraint is within the graph else false.
		*/
		bool constraintLenght(
			const Core::FMTConstraint& constraint, 
			int& start, 
			int& stop) const
		{
			try {
				int constraintlower = constraint.getPeriodLowerBound();
				if (constraintlower == std::numeric_limits<int>::max())
				{
					constraintlower = static_cast<int>((developments.size() - 2));
				}
				//start = std::max(constraintlower, getFirstActivePeriod() + 1);
				//*_logger << "get on" << getFirstPeriod() << "\n";
				start = std::max(constraintlower, getFirstPeriod());
				stop = static_cast<int>((constraint.getPeriodUpperBound() > static_cast<int>((developments.size() - 2))) ? (developments.size() - 2) : constraint.getPeriodUpperBound());
				if (constraint.acrossPeriod())
				{
					--stop;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::constraintLenght", __LINE__, __FILE__);
			}
			return (start < static_cast<int>(developments.size()) && start <= stop);
		}

		// DocString: FMTGraph::getStats(const Core::FMTMask&)
		/**
		@brief Return the graph stats for a mask subset.
		@param[in] p_Subset the mask subset.
		@return the graph stats.
		*/
		FMTGraphStats getStats(const Core::FMTMask& p_Subset)
		{
			FMTGraphStats SubsetStats;
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				std::unordered_set<int>selectedVariables;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); 
					vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const FMTvertex_descriptor DESCRIPTOR = *vertex_iterator;
					const FMTBaseVertexProperties& VERTEX_PROPERTIES = data[DESCRIPTOR];
					if (VERTEX_PROPERTIES.get().getMask().isSubsetOf(p_Subset))
						{
						++SubsetStats.vertices;
						const int OUT_EDGES = static_cast<int>(boost::out_degree(DESCRIPTOR,data));
						SubsetStats.edges += OUT_EDGES;
						++SubsetStats.transfer_rows;
						++SubsetStats.rows;
						for (const int& IN_VAR : getInVariables(DESCRIPTOR))
							{
							if (selectedVariables.find(IN_VAR) == selectedVariables.end())//No double counting for variables
								{
								selectedVariables.insert(IN_VAR);
								++SubsetStats.cols;
								}
							}
						}
				}
			}catch (...)
				{
				_exhandler->raiseFromCatch("", "FMTGraph::getStats", __LINE__, __FILE__);
				}
			return SubsetStats;
		}

		// DocString: FMTGraph::getStats()
		/**
		@brief Return the graph stats.
		@return the graph stats.
		*/
		FMTGraphStats getStats() const
		{
			return stats;
		}

		// DocString: FMTGraph::getStatsPtr
		/**
		@brief Return a pointer to the graph stats.
		@return a pointer to the graph stats.
		*/
		FMTGraphStats* getStatsPtr()
		{
			return &stats;
		}

		// DocString: FMTGraph::setStats
		/**
		@brief Set the graph stats.
		@param[in] newstats the new stats.
		*/
		void setStats(const FMTGraphStats& newstats)
		{
			stats = newstats;
		}

		// DocString: FMTGraph::buildSchedule
		/**
		@brief Build the graph for one period following a schedule instead of every operable action.
		@param[in] model the model.
		@param[in] actives the active vertices.
		@param[in] schedule the schedule.
		@return the graph stats of the newly built elements.
		*/
		FMTGraphStats buildSchedule(
			const Models::FMTModel& model,
			std::queue<FMTvertex_descriptor> actives,
			const Core::FMTSchedule& schedule)
		{
			FMTGraphStats statsDiff(stats);
			try {
				//developments.push_back(boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>>());
				//developments.back().reserve(actives.size());
				const int actualperiod = getPeriod();
				boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, Core::FMTDevelopment>> actualdevs = getDevsSet(actualperiod);
				int action_id = 0;
				for (const Core::FMTAction& action : model.actions)
				{
					if (schedule.find(action) != schedule.end())
					{
						std::queue<FMTvertex_descriptor> new_actives;
						while (!actives.empty())
						{
							const FMTvertex_descriptor front_vertex = actives.front();
							actives.pop();
							FMTBaseVertexProperties front_properties = data[front_vertex];
							const Core::FMTDevelopment& active_development = front_properties.get();
							bool death = false;
							if ((((schedule.at(action)).find(active_development) != (schedule.at(action)).end()) ||
								(!action.doRespectLock() && active_development.getLock() != 0 &&
									(schedule.at(action)).find(active_development.clearLock()) != (schedule.at(action)).end())))
							{
								const Graph::FMTGraphVertexToYield vertexinfo = getVertexToYieldInfo(model, front_vertex);
								if (active_development.operable(action, model.yields, &vertexinfo))
								{
									if (action.getName() == "_DEATH")
									{
										death = true;
									}
									const std::vector<Core::FMTDevelopmentPath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
									addAction(action_id, statsDiff, new_actives, front_vertex, paths, actualdevs);
								}
							}
							if (!death)
							{
								new_actives.push(front_vertex);
							}
						}
						actives = new_actives;
					}
					++action_id;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::buildSchedule", __LINE__, __FILE__);
			}
			const bool typeIIforestmodel = (model.getParameter(Models::FMTintmodelparameters::MATRIX_TYPE) == 2);
			return naturalGrowth(actives, statsDiff, typeIIforestmodel);
		}

		// DocString: FMTGraph::erasePeriod
		/**
		@brief Erase the first period of the graph and return the deleted constraints and variables.
		@param[in,out] deletedconstraints the deleted constraints.
		@param[in,out] deletedvariables the deleted variables.
		@param[in] keepbounded if true keeps the bounded vertices.
		@return the graph stats of the deleted elements.
		*/
		FMTGraphStats erasePeriod(
			std::vector<int>& deletedconstraints,
			std::vector<int>& deletedvariables,
			bool keepbounded = false)
		{

			return getStats();
		}

		// DocString: FMTGraph::empty
		/**
		@brief Return true if the graph is empty.
		@return true if the graph is empty else false.
		*/
		bool empty() const
		{
			return developments.empty();
		}

		// DocString: FMTGraph::getActiveVertices
		/**
		@brief Return the active vertices of the graph.
		@return the queue of active vertices.
		*/
		std::queue<FMTvertex_descriptor> getActiveVertices() const
		{
			std::queue<FMTvertex_descriptor>actives;
			try {
				FMTvertex_iterator vertexit, vertexend;
				for (boost::tie(vertexit, vertexend) = developments.back(); vertexit != vertexend; ++vertexit)
				{
					actives.push(*vertexit);
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getActiveVertices", __LINE__, __FILE__);
			}
			return actives;
		}


		// DocString: FMTGraph::getPeriodVertices
		/**
		@brief Return the first and last vertex iterators of a period.
		@param[in] period the period.
		@return the vertex pair of the period.
		*/
		const FMTvertex_pair& getPeriodVertices(int period) const
		{
			return developments.at(period);
		}

		/*lookconst_iterator begin(int period) const
		{
			return developments.at(period).cbegin();
		}

		lookconst_iterator end(int period) const
		{
			return developments.at(period).cend();
		}*/
		// DocString: FMTGraph::size
		/**
		@brief Return the number of periods of the graph.
		@return the graph size.
		*/
		size_t size() const
		{
			return developments.size();
		}

		// DocString: FMTGraph::setConstraintId
		/**
		@brief Set the constraint id of a vertex.
		@param[in] vertex the vertex descriptor.
		@param[in] id the constraint id.
		*/
		void setConstraintId(const FMTvertex_descriptor& vertex, const int& id)
		{

		}

		// DocString: FMTGraph::getTransferRow
		/**
		@brief Fill the area transfer row of a vertex for the matrix.
		@param[in] vertex_descriptor the vertex descriptor.
		@param[in,out] row_starts the row starts.
		@param[in,out] cols the columns.
		@param[in,out] cols_value the column coefficients.
		@return true if the row was filled else false.
		*/
		bool getTransferRow(
			const FMTvertex_descriptor& vertex_descriptor,
			std::vector<int>& row_starts,
			std::vector<int>& cols,
			std::vector<double>& cols_value) const
		{
			return false;
		}

		// DocString: FMTGraph::getInitialBounds
		/**
		@brief Fill the lower and upper bounds of the area transfer rows of the first period.
		@param[in,out] lower_bounds the lower bounds.
		@param[in,out] upper_bounds the upper bounds.
		*/
		void getInitialBounds(
			std::vector<double>& lower_bounds, 
			std::vector<double>& upper_bounds) const
		{
			try {

				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = developments.at(getFirstActivePeriod()); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const FMTvertex_descriptor descriptor = *vertex_iterator;
					const FMTBaseVertexProperties& vproperty = data[descriptor];
					const std::map<int, int>outs = getOutVariables(descriptor);
					lower_bounds[outs.at(-1)] = vproperty.getBaseRhs();
					upper_bounds[outs.at(-1)] = vproperty.getBaseRhs();
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getInitialBounds", __LINE__, __FILE__);
			}
		}
		// DocString: FMTGraph::nEdges
		/**
		@brief Return the number of edges of the graph.
		@return the number of edges.
		*/
		size_t nEdges() const
		{
			return boost::num_edges(data);
		}

		// DocString: FMTGraph::getSource
		/**
		@brief Return the values of an output node source for a period.
		@param[in] model the model.
		@param[in] node the output node.
		@param[in] period the period.
		@param[in] solution the solution.
		@param[in] level the output level.
		@return a map of names to values.
		*/
		std::map<std::string, double> getSource(
			const Models::FMTModel& model,
			const Core::FMTOutputNode& node,
			int period, const Core::FMTTheme& theme,
			const double* solution, 
			Core::FMToutputlevel level = Core::FMToutputlevel::standard) const
		{
			try {
				Core::FMTOutputNode tempNode(node);
				const std::vector<FMTvertex_descriptor>& vertices = getNode(model, tempNode, period);
				return getValues(model, vertices, tempNode, theme, solution, level);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("For node: " + std::string(node), "FMTGraph::getSource", __LINE__, __FILE__);
			}
			return std::map<std::string, double>();
		}

		// DocString: FMTGraph::getVertexToYieldInfo
		/**
		@brief Return the vertex to yield adapter of a vertex, used for the yield requests depending on the graph.
		@param[in] model the model.
		@param[in] descriptor the vertex descriptor.
		@return the vertex to yield adapter.
		*/
		Graph::FMTGraphVertexToYield getVertexToYieldInfo(
			const Models::FMTModel& model, 
			const FMTvertex_descriptor& descriptor) const
		{
			try {
				return Graph::FMTGraphVertexToYield(model, *this, reinterpret_cast<const void*>(&descriptor));
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getVertexToYieldInfo", __LINE__, __FILE__);
			}
			return Graph::FMTGraphVertexToYield();
		}

		// DocString: FMTGraph::getVertexFromVertexInfo
		/**
		@brief Return the vertex descriptor held by a vertex to yield adapter.
		@param[in] info the vertex to yield adapter.
		@return a pointer to the vertex descriptor.
		*/
		const FMTvertex_descriptor* getVertexFromVertexInfo(
			const Graph::FMTGraphVertexToYield* info) const
		{
			try {
				return reinterpret_cast<const FMTvertex_descriptor*>(info->getVertexPtr());
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getVertexFromVertexInfo", __LINE__, __FILE__);
			}
			return nullptr;
		}

		// DocString: FMTGraph::getValues
		/**
		@brief Return the output values of a set of vertices for a period.
		@param[in] model the model.
		@param[in] verticies the vertices.
		@param[in] node the output node.
		@param[in] period the period.
		@param[in] solution the solution.
		@param[in] level the output level.
		@return a map of names to values.
		*/
		std::map<std::string, double> getValues(
			const Models::FMTModel& model, 
			const std::vector<FMTvertex_descriptor>& vertices,
			const Core::FMTOutputNode& node, 
			const Core::FMTTheme& theme,
			const double* solution, 
			Core::FMToutputlevel level) const
		{
			std::map<std::string, double>values;
			try {
				std::map<std::string, double>::iterator toFill = values.end();
				std::map<std::string, double>::iterator totalFill = values.end();
				double accumulator=0.0;
				if (level == Core::FMToutputlevel::standard)
					{
					for (const std::string& attribute : theme.getBaseAttributes())
						{
						values[attribute] = 0;
						}
					}
				if (level != Core::FMToutputlevel::developpement)
					{
					std::pair<std::map<std::string, double>::iterator, bool> result = values.insert(std::pair<std::string, double>("Total", 0.0));
					toFill = result.first;
					totalFill = result.first;
					}
				if (!vertices.empty())
				{
					const std::vector<const Core::FMTAction*> selected = node.source.targets(model.actions);

					//Core::FMTAction optimization_action;
					for (const FMTvertex_descriptor& vertex : vertices)
					{
						const Core::FMTDevelopment& development = data[vertex].get();
						const Graph::FMTGraphVertexToYield vertexinfo = getVertexToYieldInfo(model, vertex);
						if (level == Core::FMToutputlevel::standard)
						{
							toFill = values.find(development.getMask().getAttribute(theme));
						}
						else if (level == Core::FMToutputlevel::developpement)
						{
							std::string value;
							if (node.source.isNextPeriod())//If it looks at next period make sure to show the right dev...
							{
								value = std::string(Core::FMTDevelopment(data[getGrowthSource(vertex)].get()));
							}
							else {
								value = std::string(Core::FMTDevelopment(development));
							}
							std::pair<std::map<std::string, double>::iterator, bool> target = values.insert(std::pair<std::string, double>(value, 0));
							toFill = target.first;
						}
						if (node.source.useInEdges())
						{
							Core::FMTDevelopment newDev(development);
							newDev.setPeriod(newDev.getPeriod() - 1);
							const double coef = node.source.getCoef(newDev, model.yields, &vertexinfo) * node.factor.getCoef(newDev, model.yields, &vertexinfo) * node.constant;
							double area = 0;
							if (development.getPeriod() == 0)
							{
								area = outArea(vertex, -1, solution);
							}
							else {
								area = inArea(vertex, solution, -1, !node.source.isAction());
							}
							const double VALUE = coef * area;
							accumulator += VALUE;
							toFill->second += VALUE;
						}
						else {
							for (const Core::FMTAction* act : selected)
							{
								double action_value = 0;
								const int actionID = static_cast<int>(std::distance(&(*model.actions.begin()), act));
								const std::vector<Core::FMTDevelopmentPath>paths = getPaths(vertex, actionID);
								const double action_coef = node.source.getCoef(development, model.yields, &vertexinfo, &paths, act) * node.factor.getCoef(development, model.yields, &vertexinfo, &paths, act) * node.constant;
								action_value = action_coef * (outArea(vertex, actionID, solution));
								toFill->second += action_value;
								accumulator += action_value;
							}
						}

					}
				}
				if (level == Core::FMToutputlevel::standard)
					{
					totalFill->second = accumulator;
					}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("For node: " + std::string(node) + " on theme " + std::string(theme), "FMTGraph::getValues", __LINE__, __FILE__);
			}
			return values;
		}

		// DocString: FMTGraph::getPeriod
		/**
		@brief Return the last period of the graph.
		@return the last period.
		*/
		int getPeriod() const
		{
			try {
				FMTvertex_iterator vertex, vend;
				boost::tie(vertex, vend) = vertices(data);
				--vend;
				return (data[*vend].get().getPeriod());
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getPeriod", __LINE__, __FILE__);
			}
			return 0;
		}

		// DocString: FMTGraph::rebaseCache
		/**
		@brief Rebase the output node cache after the graph has been shrinked.
		*/
		void rebaseCache()
		{
			try {
				for (size_t nodecacheid = 0; nodecacheid < nodescache.size(); ++nodecacheid)
				{
					nodescache[nodecacheid].rebase(developments.at(nodecacheid).first, developments.at(nodecacheid).second);
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::rebaseCache", __LINE__, __FILE__);
			}
		}
		
		// DocString: FMTGraph::sameEdgesAs
		/**
		@brief Return true if this graph has the same edges as another one.
		@param[in] rhs the other graph.
		@return true if the edges are the same else false.
		*/
		bool sameEdgesAs(const FMTGraph& rhs) const
		{
			bool different = false;
			try {
				FMTedge_iterator thisedge_iterator, thisedge_iterator_end;
				FMTedge_iterator rhsedge_iterator, rhsedge_iterator_end;
				boost::tie(thisedge_iterator, thisedge_iterator_end) = boost::edges(data);
				boost::tie(rhsedge_iterator, rhsedge_iterator_end) = boost::edges(rhs.data);
				while (!different && thisedge_iterator != thisedge_iterator_end && rhsedge_iterator != rhsedge_iterator_end)
				{
					const FMTBaseEdgeProperties& thisedgeprop = data[*thisedge_iterator];
					const FMTBaseEdgeProperties& rhsedgeprop = rhs.data[*rhsedge_iterator];
					if (thisedgeprop.getActionID() != rhsedgeprop.getActionID())
					{
						different = true;
					}
					++thisedge_iterator;
					++rhsedge_iterator;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::sameEdgesAs", __LINE__, __FILE__);
			}
			return different;
		}

		// DocString: FMTGraph::updateMatrixIndex
		/**
		@brief Update the matrix indexes of the vertices and edges after variables and constraints have been removed.
		@param[in] removedvariables the removed variables.
		@param[in] removedconstraints the removed constraints.
		*/
		void updateMatrixIndex(
			const std::vector<int>& removedvariables,
			const std::vector<int>& removedconstraints)
		{

		}
		// DocString: FMTGraph::getFirstActivePeriod
		/**
		@brief Return the first active period of the graph.
		@return the first active period.
		*/
		int getFirstActivePeriod() const
		{
			return static_cast<int>(std::distance(developments.begin(), getFirstConstBlock()));
		}

		// DocString: FMTGraph::getLastDisturbance
		/**
		@brief Return the in edge of the last disturbance and the number of periods since it happened.
		@param[in] activeedge the active in edge.
		@param[in,out] periodtolastdisturbance the number of periods since the last disturbance.
		@return the in edge of the last disturbance.
		*/
		FMTinedge_iterator getLastDisturbance(
			FMTinedge_iterator activeedge,
			int& periodtolastdisturbance) const
		{
			FMTinedge_iterator lastedge;
			try {
				std::queue<FMTinedge_iterator>actives;
				actives.push(activeedge);
				while (!actives.empty())
				{
					activeedge = actives.front();
					actives.pop();
					const FMTvertex_descriptor vertexsource = boost::source(*activeedge, data);
					if (data[*activeedge].getActionID() >= 0)
					{
						periodtolastdisturbance = data[vertexsource].get().getPeriod();
						return activeedge;
					}
					FMTinedge_iterator inedge_iterator, inedge_end;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertexsource, data); inedge_iterator != inedge_end; ++inedge_iterator)
					{
						actives.push(inedge_iterator);
					}
				}
				lastedge = activeedge;
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getLastDisturbance", __LINE__, __FILE__);
			}

			return lastedge;
		}

		// DocString: FMTGraph::fillUpLastActions
		/**
		@brief Fill the last action edges seen before a target vertex up to a target period.
		@param[in] targetperiod the target period.
		@param[in] targetdescriptor the target vertex.
		@param[in,out] edges the action edges.
		@param[in,out] gaps the period gaps.
		*/
		void fillUpLastActions(
			const int& targetperiod,
			const FMTvertex_descriptor& targetdescriptor, 
			std::vector<const FMTBaseEdgeProperties*>& lastactions,
			std::vector<int>& distances, 
			const size_t& depth) const
		{
			try {
				std::queue<FMTvertex_descriptor>activevertex;
				activevertex.push(targetdescriptor);
				while (!activevertex.empty() && lastactions.size() <= depth)
				{
					FMTvertex_descriptor descriptor = activevertex.front();
					activevertex.pop();
					FMTinedge_iterator inedge_iterator, inedge_end;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
					{
						const FMTBaseEdgeProperties& inedgeproperties = data[*inedge_iterator];
						const FMTvertex_descriptor& sourcevertex = boost::source(*inedge_iterator, data);
						activevertex.push(sourcevertex);
						const int sourceperiod = data[sourcevertex].get().getPeriod();
						if (inedgeproperties.getActionID() >= 0)
						{
							lastactions.push_back(&inedgeproperties);
							distances.push_back(targetperiod - sourceperiod);
						}
						if (sourceperiod == 0 && lastactions.size() <= depth)
						{
							lastactions.push_back(nullptr);
							distances.push_back(targetperiod - 1 + data[sourcevertex].get().getAge());//targetperiod-1 because distance between age20 at p1 and p2 == 21, not 22
						}
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::fillUpLastActions", __LINE__, __FILE__);
			}

		}

		// DocString: FMTGraph::getAmountOfPaths
		/**
		@brief Return the number of possible paths if an action is committed on a development.
		@param[in] dev the development.
		@param[in] actionid the action id.
		@param[in] model the model.
		@param[in] devsets the developments lookup set.
		@return the number of paths.
		*/
		size_t getAmountOfPaths(
			const Core::FMTDevelopment& dev, 
			const int& actionid,
			const Models::FMTModel& model,
			const boost::unordered_set<Core::FMTLookup<FMTvertex_descriptor, 
			Core::FMTDevelopment>>& actualperioddevs) const
		{
			size_t amount = 0;
			try {
				std::vector<FMTvertex_descriptor>paths;
				if (actionid >= 0)
				{
					for (const Core::FMTDevelopmentPath& path : dev.operate(model.actions.at(actionid), model.transitions.at(actionid), model.yields, model.themes))
					{
						if (containsDevelopment(path.getDevelopment(), actualperioddevs))
						{
							paths.push_back(getDevelopment(path.getDevelopment(), actualperioddevs));
						}
					}
				}
				else {
					const FMTvertex_descriptor& act = getDevelopment(dev, actualperioddevs);
					FMToutedge_pair edge_pair;
					for (edge_pair = boost::out_edges(act, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
					{
						const FMTBaseEdgeProperties& edgeprop = data[*edge_pair.first];
						if (edgeprop.getActionID() < 0)
						{
							paths.push_back(boost::target(*edge_pair.first, data));
							break;
						}
					}

				}
				for (const FMTvertex_descriptor& path : paths)
				{
					int period = getDevelopment(path).getPeriod();
					FMTvertex_descriptor vdescriptor = path;
					while (period < size())
					{
						std::map<int, int>vars = getOutVariables(vdescriptor);
						vars.erase(-1);
						amount += vars.size();
						FMToutedge_pair edge_pair;
						bool gotit = false;
						for (edge_pair = boost::out_edges(vdescriptor, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
						{
							const FMTBaseEdgeProperties& edgeprop = data[*edge_pair.first];
							if (edgeprop.getActionID() < 0)
							{
								vdescriptor = boost::target(*edge_pair.first, data);
								gotit = true;
								break;
							}
						}
						if (!gotit)
						{
							break;
						}
						++period;
					}
				}
			}
			catch (...)
			{
				_exhandler->printExceptions("for " + std::string(dev), "FMTGraph::getAmountOfPaths", __LINE__, __FILE__);
			}
			return amount;
		}

		// DocString: FMTGraph::getAllSeries
		/**
		@brief Return all the action series starting from a vertex.
		@param[in] targetdescriptor the target vertex.
		@param[in] p_series the base serie.
		@param[in] actions the model actions.
		@param[in] actionsets the aggregate action ids.
		@param[in] mask the mask.
		@return the series.
		*/
		std::set<Core::FMTSerie> getAllSeries(
			FMTvertex_descriptor targetdescriptor, 
			const std::vector<std::string>& p_series,
			const std::vector<Core::FMTAction>& actions, 
			const std::unordered_set<int>& actionselected, 
			const Core::FMTMask& mask) const
		{
			std::set<Core::FMTSerie>theseries;
			try {
				size_t inedgessize = boost::in_degree(targetdescriptor, data);
				const FMTBaseVertexProperties& mainproperties = data[targetdescriptor];
				const int startperiod = mainproperties.get().getPeriod();
				std::vector<std::string> actualSerie(p_series);
				//The actual serie end up with the out_degree...
				//*_logger <<"in "<< std::string(mainproperties.get())<<" in degree "<< inedgessize << "\n";
				while (inedgessize > 0)
				{
					FMTinedge_iterator inedge_iterator, inedge_end;
					if (inedgessize > 1)
					{
						inedgessize = 0;
						for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
						{
							std::vector<std::string> subSerie(actualSerie);
							FMTvertex_descriptor subdescriptor = boost::source(*inedge_iterator, data);
							const FMTBaseVertexProperties& targetproperties = data[subdescriptor];
							if (targetproperties.get().getMask().isSubsetOf(mask))//natural growth or action in the aggregate
							{
								const FMTBaseEdgeProperties& inedgeproperties = data[*inedge_iterator];
								const int actionid = inedgeproperties.getActionID();
								const int perioddiff = startperiod - targetproperties.get().getPeriod();
								//bool digMore = true;
								if (/*actionid >= 0 &&*/ actionselected.find(actionid) != actionselected.end())
								{
									//if ()
									//{
									subSerie.insert(subSerie.begin(), actions.at(actionid).getName());
									theseries.insert(Core::FMTSerie(subSerie, perioddiff));//Always add the subserie
									//}else {
									//	digMore = false;
									//}

								}
								//if (digMore)
								//{
								for (const Core::FMTSerie& subofserie : getAllSeries(subdescriptor, subSerie, actions, actionselected, mask))
								{
									//const std::string fullserie = subofserie.first +"-" + subserie;
									const int perioddepth = subofserie.getLength() + perioddiff;
									theseries.insert(Core::FMTSerie(subofserie.getActions(), perioddepth));
								}
								//}

							}
						}

					}
					else {
						inedgessize = 0;
						boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data);
						if (inedge_iterator != inedge_end)
						{
							targetdescriptor = boost::source(*inedge_iterator, data);
							const FMTBaseVertexProperties& targetproperties = data[targetdescriptor];
							//*_logger << "in for out " << std::string(targetproperties.get()) << " in degree " << inedgessize << "\n";
							if (targetproperties.get().getMask().isSubsetOf(mask))//natural growth or action in the aggregate
							{
								const FMTBaseEdgeProperties& inedgeproperties = data[*inedge_iterator];
								const int actionid = inedgeproperties.getActionID();
								if (/*actionid >= 0 &&*/ actionselected.find(actionid) != actionselected.end())
								{
									actualSerie.insert(actualSerie.begin(), actions.at(actionid).getName());
									theseries.insert(Core::FMTSerie(actualSerie, startperiod - targetproperties.get().getPeriod()));//Always add the subserie
								}
								inedgessize = boost::in_degree(targetdescriptor, data);
							}
						}
					}

				}
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTGraph::getAllSeries", __LINE__, __FILE__);
			}
			return theseries;
		}

		// DocString: FMTGraph::timeSinceLastAction
		/**
		@brief Return the number of periods since the last action on a vertex.
		@param[in] targetdescriptor the vertex descriptor.
		@return the number of periods since the last action.
		*/
		size_t timeSinceLastAction(const FMTvertex_descriptor& targetdescriptor) const
		{
			try {
				std::queue<std::pair<FMTvertex_descriptor, size_t>>vertices_n_depth;
				vertices_n_depth.push(std::pair<FMTvertex_descriptor, size_t>(targetdescriptor, 0));
				while (!vertices_n_depth.empty())
				{
					const std::pair<FMTvertex_descriptor, size_t> descriptor_n_depth = vertices_n_depth.front();
					FMTinedge_iterator inedge_iterator, inedge_end;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(descriptor_n_depth.first, data); inedge_iterator != inedge_end; ++inedge_iterator)
					{
						const FMTBaseEdgeProperties& edgeprop = data[*inedge_iterator];
						if (edgeprop.getActionID() != -1)
						{
							return descriptor_n_depth.second;
						}
						else {
							const FMTvertex_descriptor sourcevertex = boost::source(*inedge_iterator, data);
							vertices_n_depth.push(std::pair<FMTvertex_descriptor, size_t>(sourcevertex, descriptor_n_depth.second + 1));
						}
					}
					vertices_n_depth.pop();
				}

			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::timeSinceLastAction", __LINE__, __FILE__);
			}
			return std::numeric_limits<size_t>::max();
		}

		// DocString: FMTGraph::getPredictors
		/**
		@brief Return the predictors of a vertex.
		@param[in] targetdescriptor the target vertex.
		@param[in] model the model.
		@param[in] yieldnames the yield names.
		@param[in] period the period.
		@param[in] periodonevalues if true uses the period one values.
		@param[in] withGCBMid if true uses the GCBM id.
		@return the predictors.
		*/
		std::vector<FMTPredictor> getPredictors(
			const FMTvertex_descriptor& targetdescriptor, 
			const Models::FMTModel& model,
			const std::vector<std::string>& yieldnames,
			const size_t& depth, 
			bool periodonevalues = false, 
			bool withGCBMid = true) const
		{
			std::vector<FMTPredictor> predictors;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				const FMTBaseVertexProperties& targetproperties = data[targetdescriptor];
				const int targetperiod = targetproperties.get().getPeriod();
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					std::vector<const FMTBaseEdgeProperties*>lastactions;
					std::vector<int>distances;
					const FMTvertex_descriptor& sourcevertex = boost::source(*inedge_iterator, data);
					const FMTBaseVertexProperties& sourceproperties = data[sourcevertex];
					const int sourceperiod = sourceproperties.get().getPeriod();
					if (sourceperiod > 0 || periodonevalues)
					{
						lastactions.push_back(&data[*inedge_iterator]);
						distances.push_back(targetperiod - sourceperiod);
					}
					else {
						break;//if nothing happen in period one, it's not a predictor, because there is no change in yields and other things because P0 = begining of P1
					}
					fillUpLastActions(targetperiod, sourcevertex, lastactions, distances, depth);
					while (lastactions.size() <= depth)
					{
						lastactions.push_back(nullptr);
						distances.push_back(-1);
					}
					predictors.emplace_back(model.actions, yieldnames, model.yields, sourceproperties, targetproperties, lastactions, distances, withGCBMid);
				}
				predictors.shrink_to_fit();
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getPredictors", __LINE__, __FILE__);
			}
			return predictors;
		}

		// DocString: FMTGraph::getFirstPeriod
		/**
		@brief Return the first period of the graph.
		@return the first period.
		*/
		int getFirstPeriod() const
		{
			return data[*developments.at(getFirstActivePeriod() + 1).first].get().getPeriod();
		}

		// DocString: FMTGraph::postSolve
		/**
		@brief Postsolve the graph back into the original themes and actions.
		@param[in] filter the mask filter used for the preSolve.
		@param[in] originalbasethemes the original themes.
		@param[in] actionmapping the action mapping.
		*/
		void postSolve(
			const Core::FMTMaskFilter& filter,
			const std::vector<Core::FMTTheme>& originalbasethemes,
			const std::vector<int>& actionmapconnection)
		{
			try {
				developments.clear();
				nodescache.clear();//Some postSolve can be done here to keep some usefull information but for now just clear
				//start by remapping the actions
				FMTedge_iterator edge_iterator, edge_iterator_end;
				for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
				{
					FMTBaseEdgeProperties& edgeprop = data[*edge_iterator];
					if (edgeprop.getActionID() >= 0)
					{
						edgeprop.setActionID(actionmapconnection.at(edgeprop.getActionID()));
					}
				}
				boost::unordered_map<Core::FMTMask, Core::FMTMask>presolvetopostsolve;
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					FMTBaseVertexProperties& vertexprop = data[*vertex_iterator];
					const Core::FMTMask& presolvemask = vertexprop.get().getMask();
					boost::unordered_map<Core::FMTMask, Core::FMTMask>::const_iterator mskit = presolvetopostsolve.find(presolvemask);
					if (mskit != presolvetopostsolve.end())
					{
						vertexprop.setDevlopementMask(mskit->second);
					}
					else {
						const Core::FMTMask postsolvedmask = presolvemask.postSolve(filter, originalbasethemes);
						presolvetopostsolve[presolvemask] = postsolvedmask;
						vertexprop.setDevlopementMask(postsolvedmask);
					}
				}
				_generateDevelopments();
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::postSolve", __LINE__, __FILE__);
			}
		}

		// DocString: FMTGraph::getSchedule
		/**
		@brief Return the operated schedule of a period based on the solution.
		@param[in] actions the model actions.
		@param[in] actual_solution the solution.
		@param[in] lperiod the period.
		@param[in] withlock if true includes the locked developments.
		@return the schedule.
		*/
		Core::FMTSchedule getSchedule(
			const std::vector<Core::FMTAction>& actions,
			const double* actual_solution, 
			const int& lperiod, 
			bool withlock = false) const
		{
			Core::FMTSchedule newSchedule(lperiod, *this, withlock);
			try {
				if (static_cast<int>(size()) > lperiod && lperiod > 0)
				{
					//newSchedule.setPeriod(lperiod);
					//std::map<Core::FMTAction, std::map<Core::FMTDevelopment, std::map<int, double>>>schedule_solution;
					//const double* actual_solution = this->getColSolution();
					FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(lperiod); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const FMTvertex_descriptor vertex = *vertex_iterator;
						std::map<int, int>variables = getOutVariables(vertex);
						variables.erase(-1);
						if (!variables.empty())
						{
							const Core::FMTDevelopment& dev = data[*vertex_iterator].get();
							for (const auto variable_iterator : variables)
							{
								
								if (*(actual_solution + variable_iterator.second) > FMT_DBL_TOLERANCE) //basis solution only!!!
								{
									//if (dev.getAge() == 9 
									//	&& dev.getPeriod() == 7 
									//	&& (actions.at(variable_iterator.first).getName() == "ATBE" 
									//	|| actions.at(variable_iterator.first).getName() == "AFEU"
									//	|| actions.at(variable_iterator.first).getName() == "_DEATH")
									//	&& *(actual_solution + variable_iterator.second) >= 74.39 
									//	&& *(actual_solution + variable_iterator.second) <= 74.41)
									//{
									//	std::cout << inArea(vertex, actual_solution) << "\n";
									//	std::cout << std::string(dev) << "\n";
									//}
									//if (dev.getAge() == 9
									//	&& dev.getPeriod() == 7
									//	&& (actions.at(variable_iterator.first).getName() == "_DEATH")
									//	&& *(actual_solution + variable_iterator.second) >= 148.79
									//	&& *(actual_solution + variable_iterator.second) <= 148.81)
									//{
									//	std::cout << inArea(vertex, actual_solution) << "\n";
									//	std::cout << std::string(dev) << "\n";
									//}
									/*if (schedule_solution.find(actions[variable_iterator.first]) == schedule_solution.end())
									{
										schedule_solution[actions[variable_iterator.first]] = std::map<Core::FMTDevelopment, std::map<int, double>>();
									}
									const Core::FMTDevelopment& basedev = getDevelopment(deviterator.memoryobject);*/
									newSchedule.addEvent(dev, *(actual_solution + variable_iterator.second), actions.at(variable_iterator.first));
									/*Core::FMTDevelopment lockout = basedev.clearLock();
									int leveltarget = basedev.lock;
									if (withlock)
										{
										lockout = basedev;
										leveltarget = 0;
										}
									if (schedule_solution[actions[variable_iterator.first]].find(lockout) == schedule_solution[actions[variable_iterator.first]].end())
									{
										schedule_solution[actions[variable_iterator.first]][lockout] = std::map<int, double>();
									}
									schedule_solution[actions[variable_iterator.first]][lockout][leveltarget] = (*(actual_solution + variable_iterator.second));*/
								}
							}

						}
					}
					newSchedule.clean();
					/*Core::FMTSchedule newSchedule(lperiod, schedule_solution);
					newSchedule.passinobject(*this);
					if (withlock)
						{
						newSchedule.setUseLock(true);
						}*/
						//return newSchedule;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("at period " + std::to_string(lperiod), "FMTGraph::getSchedule", __LINE__, __FILE__);
			}

			return newSchedule;
		}
		// DocString: FMTGraph::getRotations
		/**
		@brief Return the distribution of the rotations (in periods) taken to complete a serie of actions for a mask and an action aggregate.
		@details A serie can contain a subserie for an aggregate of actions.
		@param[in] model the model.
		@param[in] mask the mask of the rotations.
		@param[in] aggregate the aggregate the last action is considered to be in.
		@return the unique series with the number of periods taken.
		*/
		std::set<Core::FMTSerie> getRotations(
			const Models::FMTModel& model, 
			const Core::FMTMask& mask, 
			const std::string& aggregate) const
		{
			std::set<Core::FMTSerie>theseries;
			try {
				const Core::FMTOutputSource baseSource(Core::FMTSpec(), mask,
					Core::FMTotar::actual, "", aggregate);
				const Core::FMTOutputSource baseFactor(Core::FMTotar::val, 1.0);
				const Core::FMTOutputNode baseNode(baseSource, baseFactor, 1.0);
				const int minperiod = 1;
				const int maxperiod = static_cast<int>(developments.size() - 2);
				const std::unordered_set<int> actionsets = Core::FMTActionComparator(aggregate).getAllAggregatesSet(model.actions, true);
				for (int period = minperiod; period <= maxperiod; ++period)
				{
					Core::FMTOutputNode periodNode(baseNode);
					for (const FMTvertex_descriptor& targetdescriptor : getNode(model, periodNode, period))
					{
						FMToutedge_pair edge_pair;
						for (edge_pair = boost::out_edges(targetdescriptor, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
						{
							const FMTBaseEdgeProperties& edgeprop = data[*edge_pair.first];
							int actionid = edgeprop.getActionID();
							if (/*actionid >= 0 &&*/ actionsets.find(actionid) != actionsets.end())
							{
								std::set<Core::FMTSerie>subset;
								const std::vector<std::string>BASE_SERIE = { model.actions.at(actionid).getName() };
								const std::set<Core::FMTSerie>PERIOD_SERIES = getAllSeries(targetdescriptor, BASE_SERIE, model.actions, actionsets, mask);
								if (!PERIOD_SERIES.empty())
								{
									std::set_union(PERIOD_SERIES.begin(), PERIOD_SERIES.end(),
										theseries.begin(), theseries.end(),
										std::inserter(subset, std::begin(subset)));
									theseries.swap(subset);
								}

							}
						}


					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getRotations", __LINE__, __FILE__);
			}
			return theseries;
		}

		// DocString: FMTGraph::getAllDevelopmentsMask
		/**
		@brief Return the masks of all the developments of the graph, ignoring some themes.
		@param[in] p_themesToIgnore the themes to ignore.
		@return the development masks.
		*/
		std::set<std::string> getAllDevelopmentsMask(
			const std::vector<Core::FMTTheme>& p_themesToIgnore) const 
		{
			std::set<std::string> masks;
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const Core::FMTDevelopment& DEV = data[*vertex_iterator].get();
					Core::FMTMask maskObj(DEV.getMask());
					for (const Core::FMTTheme& THEME : p_themesToIgnore) {
						maskObj.set(THEME, "?");
					}
					masks.insert(std::string(maskObj));

				}
			}
			catch (...){
				_exhandler->raiseFromCatch("", "FMTGraph::getAllDevelopmentsMask", __LINE__, __FILE__);
			}
			return masks;
		}

		// DocString: FMTGraph::getOutVariablesProportions
		/**
		@brief Return the schedule of a period where the areas are expressed as proportions of the developments.
		@param[in] actions the model actions.
		@param[in] actual_solution the solution.
		@param[in] lperiod the period.
		@param[in] withlock if true includes the locked developments.
		@return the proportion schedule.
		*/
		Core::FMTSchedule getOutVariablesProportions(
			const std::vector<Core::FMTAction>& actions, 
			const double* actual_solution, 
			const int& lperiod, 
			bool withlock = false) const
		{
			Core::FMTSchedule newSchedule(lperiod, *this, withlock);
			try {
				if (static_cast<int>(size()) > lperiod && lperiod > 0)
				{
					FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(lperiod); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const FMTvertex_descriptor vertex = *vertex_iterator;
						std::map<int, int>variables = getOutVariables(vertex);
						const Core::FMTDevelopment& dev = data[*vertex_iterator].get();
						double outarea = 0;
						std::map<int, double>variablesarea;
						for (const auto variable_iterator : variables)
						{
							const double vout = *(actual_solution + variable_iterator.second);
							/*if(vout>0)
							{
								std::cout<<std::string(dev)+" Variable : "+std::to_string(variable_iterator.first)+" Outarea : "+std::to_string(vout)<<std::endl;
							}*/
							if (vout > FMT_DBL_TOLERANCE) //basis solution only!!!
							{
								if (variable_iterator.first >= 0)
								{
									variablesarea[variable_iterator.first] = vout;
								}
								outarea += vout;
							}
						}
						//std::cout<<"Total area : "+std::to_string(outarea)<<std::endl;
						for (const auto variable_iterator : variablesarea)
						{
							newSchedule.addEvent(dev, variable_iterator.second / outarea, actions.at(variable_iterator.first));
							//std::cout<<std::string(dev)+" Variable : "+std::to_string(variable_iterator.first)+" Proportion : "+std::to_string(variable_iterator.second/outarea)<<std::endl;
						}
					}
					newSchedule.clean();
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("at period " + std::to_string(lperiod), "FMTGraph::getOutVariablesProportions", __LINE__, __FILE__);
			}

			return newSchedule;
		}

		// DocString: FMTGraph::operator std::string
		/**
		@brief Return the string representation of the graph.
		@return the string representation.
		*/
		operator std::string() const
		{
			std::ostringstream stream;
			try {
				//write_graphviz(stream, data);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::std::string()", __LINE__, __FILE__);
			}
			return stream.str();
		}
	protected:
		FMTadjacency_list data;
		bool m_gotDeath;
        FMTgraphbuild buildtype;
		std::vector<FMTvertex_pair>developments;
		mutable std::vector<FMTOutputNodeCache<FMTvertex_descriptor FMT_COMMA FMTvertex_iterator>>nodescache;
		typedef typename std::vector<FMTOutputNodeCache<FMTvertex_descriptor FMT_COMMA FMTvertex_iterator>>::reverse_iterator reversecachenodeit;
        FMTGraphStats stats;

		// DocString: FMTGraph::updateVarsMap
		/**
		@brief Add a coefficient to a variable in a variables map, summing it if the variable is already present.
		@param[in,out] variables the variables map.
		@param[in] var the variable index.
		@param[in] coef the coefficient.
		*/
		void updateVarsMap(
			std::map<int,double>& variables,
			const int& var,
			const double& coef) const
		{
			try {
				std::pair<std::map<int, double>::iterator,bool>insertion = variables.insert(std::pair<int,double>(var,coef));
				if (!insertion.second)
					{
					insertion.first->second += coef;
					}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::updateVarsMap", __LINE__, __FILE__);
			}
		}

		// DocString: FMTGraph::getFirstBlock
		/**
		@brief Return an iterator to the first non empty period block of the graph.
		@return the iterator to the first block.
		*/
		typename std::vector<FMTvertex_pair>::iterator getFirstBlock()
		{
			typename std::vector<FMTvertex_pair>::iterator periodit = developments.begin();
			try {
				if (!developments.empty())
				{
					//FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					//boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
					while (periodit!= developments.end()&&
						periodit->first == periodit->second)
						{
						++periodit;
						}
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"", "FMTGraph::getFirstBlock", __LINE__, __FILE__);
				}
			}catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getFirstBlock", __LINE__, __FILE__);
			}
			return periodit;
		}

		// DocString: FMTGraph::getFirstConstBlock
		/**
		@brief Return a const iterator to the first non empty period block of the graph.
		@return the const iterator to the first block.
		*/
		typename std::vector<FMTvertex_pair>::const_iterator getFirstConstBlock() const
		{
			typename std::vector<FMTvertex_pair>::const_iterator periodit = developments.begin();
			try {
				if (!developments.empty())
				{
					//FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					//boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
					while (periodit != developments.end() && 
						periodit->first == periodit->second)
					{
						++periodit;
					}
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Empty graph", "FMTGraph::getFirstConstBlock", __LINE__, __FILE__);
				}
			}catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::getFirstConstBlock", __LINE__, __FILE__);
			}
			return periodit;
		}

		// DocString: FMTGraph::isDependant
		/**
		@brief Return true if a vertex already has an out edge of a lower action id, indicating an action dependency.
		@param[in] descriptor the vertex descriptor.
		@param[in] theactionid the action id.
		@param[out] newEdge true if a new edge can be added.
		@return true if the vertex is dependant else false.
		*/
		bool isDependant(const FMTvertex_descriptor& descriptor,
			const int& theactionid,bool& newEdge) const
		{
			try {
				newEdge = true;
				if (boost::out_degree(descriptor, data) > 0)
					{
					FMToutedge_iterator outit, outend;
					for (boost::tie(outit, outend) = boost::out_edges(descriptor, data); outit != outend; ++outit)
					{
						const FMTBaseEdgeProperties& edgeprop = data[*outit];
						if (edgeprop.getActionID() >= 0)
							{
							if (edgeprop.getActionID() < theactionid)
								{
								return true;
								}
							_exhandler->raise(Exception::FMTexc::FMTsourcetotarget_transition,
									"Action recursivity " + std::to_string(theactionid)+" from "+
								std::string(data[descriptor].get()), "FMTGraph::isDependant", __LINE__, __FILE__);
							newEdge = false;
							}
						}
					}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::isDependant", __LINE__, __FILE__);
			}
			return false;
		}
	private:
		friend class boost::serialization::access;
		template<class Archive>
		// DocString: FMTGraph::save
		/**
		@brief Save function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to save to.
		@param[in] version the serialization version.
		*/
		void save(Archive& ar, unsigned int version) const
		{
			ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
			FMTObject::forceSave(ar, version);
			ar& BOOST_SERIALIZATION_NVP(m_gotDeath);
			ar& BOOST_SERIALIZATION_NVP(data);
			ar& BOOST_SERIALIZATION_NVP(stats);
			ar& BOOST_SERIALIZATION_NVP(buildtype);
			ar& BOOST_SERIALIZATION_NVP(m_reserve);
		}
		template<class Archive>
		// DocString: FMTGraph::load
		/**
		@brief Load function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to load from.
		@param[in] version the serialization version.
		*/
		void load(Archive& ar, unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTObject>(*this));
			FMTObject::forceSave(ar, version);
			ar& BOOST_SERIALIZATION_NVP(m_gotDeath);
			ar& BOOST_SERIALIZATION_NVP(data);
			_generateDevelopments();
			ar& BOOST_SERIALIZATION_NVP(stats);
			ar& BOOST_SERIALIZATION_NVP(buildtype);
			ar& BOOST_SERIALIZATION_NVP(m_reserve);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()

		// DocString: FMTGraph::_generateDevelopments
		/**
		@brief Regenerate the developments location by period from the graph vertices.
		*/
		void _generateDevelopments()
		{
			try {
				nodescache.clear();
				developments.clear();
				FMTvertex_iterator base_iterator, base_iterator_end;
				//End always stay the same use .end() for non valid period
				boost::tie(base_iterator, base_iterator_end) = boost::vertices(data);
				if (base_iterator != base_iterator_end)
				{
					developments.resize(getPeriod() + 1, FMTvertex_pair(base_iterator_end, base_iterator_end));
					FMTvertex_iterator vertex, vend;
					int actualperiod = 0;
					size_t vertexid = 0;
					FMTvertex_iterator firstvertex;
					for (boost::tie(vertex, vend) = boost::vertices(data); vertex != vend; ++vertex)
					{
						const FMTBaseVertexProperties& properties = data[*vertex];
						const Core::FMTDevelopment& dev = properties.get();
						const size_t periodm_location = (dev.getPeriod());
						if (vertexid == 0)
						{
							firstvertex = vertex;
							actualperiod = dev.getPeriod();
						}
						if (actualperiod != dev.getPeriod())//periodchanges!
						{
							developments[periodm_location - 1] = FMTvertex_pair(firstvertex, vertex);
							firstvertex = vertex;
							actualperiod = dev.getPeriod();
						}
						++vertexid;
					}
					developments.back() = FMTvertex_pair(firstvertex, base_iterator_end);
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTGraph::_generateDevelopments", __LINE__, __FILE__);
			}
		}
		// DocString: FMTGraph::getDescriptors
		/**
		@brief Return the vertex descriptors of one period matching a set of masks.
		@param[in] p_period the period.
		@param[in,out] p_masks the masks to match.
		@param[in] p_reserve the number of descriptors to reserve.
		@return the vertex descriptors.
		*/
		std::vector<FMTvertex_descriptor> getDescriptors(
			const int& p_period,
			std::vector<const Core::FMTMask*>& p_masks, 
			const size_t& p_reserve) const
		{
			std::vector<FMTvertex_descriptor> p_descriptors;
			p_descriptors.reserve(p_reserve);
			p_masks.reserve(p_reserve);
			FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(p_period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
				const Core::FMTDevelopment& DEV = data[*vertex_iterator].get();
				p_masks.push_back(&DEV.getMask());
				p_descriptors.push_back(*vertex_iterator);
			}
			return p_descriptors;
		}
		// DocString: FMTGraph::getSubset
		/**
		@brief Fill the masks that are a subset of a static mask.
		@param[in] p_staticMask the static mask.
		@param[in,out] p_masks the masks to test.
		@param[in] p_period the period.
		@param[in] p_reserve the number of masks to reserve.
		*/
		void getSubset(
			const Core::FMTMask& p_staticMask,
			std::vector<const Core::FMTMask*>& p_masks,
			std::vector<bool>& p_results,
			size_t p_start, size_t p_stop) const
		{
			for (size_t i = p_start; i < p_stop; ++i)
			{
				if (p_masks[i]->isSubsetOf(p_staticMask))
				{
					p_results[i] = true;
				}
			}
		}
		// DocString: FMTGraph::threadedStaticSearch
		/**
		@brief Search in parallel the vertices matching a static output node for a period.
		@param[in] p_node the output node.
		@param[in] p_model the model.
		@param[in] p_period the period.
		@param[in] p_reserve the number of descriptors to reserve.
		@return the matching vertex descriptors.
		*/
		std::vector<FMTvertex_descriptor> threadedStaticSearch(
			const Core::FMTOutputNode& p_node,
			const Models::FMTModel& p_model,
			const int& p_period, 
			const size_t& p_reserve) const
		{
			const Core::FMTMask theStaticMask = p_model.getStaticMask(p_node, true);
			std::vector<const Core::FMTMask*>masks;
			const std::vector<FMTvertex_descriptor>BASE_DESCRIPTORS = getDescriptors(p_period, masks, p_reserve);
			std::vector<bool>results(BASE_DESCRIPTORS.size());
			const size_t NUMBER_THREADS = 10;
			const size_t BASE_SIZE = BASE_DESCRIPTORS.size();
			const size_t PER_THREAD = BASE_SIZE / NUMBER_THREADS;
			size_t start = 0;
			std::vector<boost::thread>workers;
			workers.reserve(NUMBER_THREADS);
			for (size_t threadId = 0; threadId < NUMBER_THREADS;++threadId)
				{
					size_t stop = start + PER_THREAD;
					if (threadId == (NUMBER_THREADS - 1))
					{
						stop = BASE_DESCRIPTORS.size();
					}
					stop = std::min(stop, BASE_DESCRIPTORS.size());
					workers.push_back(boost::thread(std::bind(&FMTGraph::getSubset,
						this,
						std::ref(theStaticMask),
						std::ref(masks),
						std::ref(results),
						start,
						stop)));
					start += PER_THREAD;
				}
			for (boost::thread& worker : workers)
			{
				worker.join();
			}
			std::vector<FMTvertex_descriptor>cleaned;
			cleaned.reserve(p_reserve);
			size_t i = 0;
			for (const bool& valid : results)
			{
				if (valid)
				{
					cleaned.push_back(BASE_DESCRIPTORS[i]);
				}
				i++;
			}
			return cleaned;
		}

		// DocString: FMTGraph::fillNextPeriod
		/**
		@brief Fill the vertices of the next periods starting from a vertex.
		@param[in] p_period the period.
		@param[in] p_LastPeriod the last period.
		@param[in] p_vertex the vertex descriptor.
		@param[in,out] p_descriptors the vertex descriptors.
		*/
		void fillNextPeriod(
			int p_period, 
			int p_LastPeriod, 
			const FMTvertex_descriptor& p_vertex, 
			std::queue<FMTvertex_descriptor>&p_actives) const
		{
			FMTvertex_descriptor nextDev = data.null_vertex();
			FMToutedge_pair edge_pair = boost::out_edges(p_vertex, data);
			while (edge_pair.first != edge_pair.second &&
				nextDev == data.null_vertex())
			{
				const FMTBaseEdgeProperties& Edge = data[*edge_pair.first];
				const int& EdgeId = Edge.getActionID();
				
				FMTvertex_descriptor nextDev = boost::target(*edge_pair.first, data);
				if (EdgeId < 0) // évolution naturelle si actionid < 0
				{
					if (p_LastPeriod == p_period - 1 &&
						nextDev != data.null_vertex())
					{ 
						p_actives.push(nextDev);
					}
					else if (p_period > p_LastPeriod)
					{
						fillNextPeriod(p_period, ++p_LastPeriod, nextDev, p_actives);
					}
				}
				else
				{
					fillNextPeriod(p_period, p_LastPeriod, nextDev, p_actives);
				}
				++edge_pair.first;
			}
		}

		// DocString: FMTGraph::getNextPeriod
		/**
		@brief Return the vertex of the next period reached by natural growth from a vertex.
		@param[in] p_vertex the vertex descriptor.
		@return the next period vertex.
		*/
		FMTvertex_descriptor getNextPeriod(const FMTvertex_descriptor& p_vertex) const
		{
			FMTvertex_descriptor NextPeriod = data.null_vertex();
			FMToutedge_pair edge_pair = boost::out_edges(p_vertex, data);
			while (edge_pair.first != edge_pair.second &&
				NextPeriod == data.null_vertex())
			{
				const FMTBaseEdgeProperties& Edge = data[*edge_pair.first];
				const int& EdgeId = Edge.getActionID();
				if (EdgeId < 0) // évolution naturelle si actionid < 0
				{
					NextPeriod = boost::target(*edge_pair.first, data);
				}
				++edge_pair.first;
			}
			return NextPeriod;
		}

		// DocString: FMTGraph::setNodeByStaticMask
		/**
		@brief Select the vertices based on the static nodes; if the descriptors are not empty it presumes the same node at another period.
		@param[in] p_model the optimization model.
		@param[in] p_node the output node.
		@param[in] p_period the period of the node.
		@param[in] p_useCache if true uses the cache.
		@return the selected vertices.
		*/
		std::vector<FMTvertex_descriptor> setNodeByStaticMask(
			const Models::FMTModel& p_model,
			const Core::FMTOutputNode& p_node, 
			int p_period, 
			bool useCache) const
		{
			std::vector<FMTvertex_descriptor> p_descriptors(m_allocator);
			try {
				p_descriptors.reserve(m_reserve);
				if (useCache)
				{
					int last_period = p_period - 1;
					bool contains = false;
					while (!contains && last_period >= 0)
					{
						contains = nodescache.at(last_period).contains(p_node);
						if (!contains)
						{
							--last_period;
						}
					}
					const int POTENTIAL_LAST_PERIOD = last_period;
					if (!contains)
					{
						//p_descriptors = threadedStaticSearch(p_node,p_model, p_period, p_reserve);
						const Core::FMTMask THE_STATIC_MASK = p_model.getStaticMask(p_node, true);
						const std::vector<size_t> BLOCKS_SUBSET = THE_STATIC_MASK.getNonFullBlocks();
						FMTvertex_iterator vertex_iterator, vertex_iterator_end;
						for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(p_period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
						{
							const Core::FMTDevelopment& DEV = data[*vertex_iterator].get();
							if (DEV.getMask().isSubsetOf(THE_STATIC_MASK, BLOCKS_SUBSET))//DEV.getMask().isSubsetOf(THE_STATIC_MASK))
							{
								p_descriptors.push_back(*vertex_iterator);
							}
						}
					}
					else {//Go to the period asked for by traveling on the graph
						//*_logger << "Node evo " << p_node << " " << p_period << "\n";
						bool exact = false;
						const std::vector<FMTvertex_descriptor>& PAST_DESCRIPTORS = nodescache.at(POTENTIAL_LAST_PERIOD).getVertices(p_node, p_model.actions, p_model.themes, exact);
						std::queue<FMTvertex_descriptor>actives(m_allocator);

						//for (FMTvertex_descriptor PAST_DESCRIPTOR : PAST_DESCRIPTORS)
						//	{
						//		int BasePeriod = POTENTIAL_LAST_PERIOD;
						//		while (BasePeriod != p_period && PAST_DESCRIPTOR != data.null_vertex())
						//		{
						//			PAST_DESCRIPTOR = getNextPeriod(PAST_DESCRIPTOR);
						//			++BasePeriod;
						//		}
						//		if (BasePeriod == p_period &&
						//			PAST_DESCRIPTOR != data.null_vertex())
						//		{
						//			actives.push(PAST_DESCRIPTOR);
						//		}
						//	}
						
						for (const FMTvertex_descriptor& PAST_DESCRIPTOR : PAST_DESCRIPTORS)
						{
							fillNextPeriod(p_period, POTENTIAL_LAST_PERIOD, PAST_DESCRIPTOR, actives);
						}

						//const size_t INITITAL_COUNT = actives.size();
						if (POTENTIAL_LAST_PERIOD != p_period)
						{
							nodescache.at(POTENTIAL_LAST_PERIOD).eraseNode(p_node);//Dont make a mess in the cache and delete the last period...
						}
						
						//std::allocator<FMTvertex_descriptor> treeAllocator;
						//treeAllocator.allocate(PAST_DESCRIPTORS.size() * 2);
						std::unordered_set<FMTvertex_descriptor>right_period(m_allocator);
						right_period.reserve(actives.size()*2);
						while (!actives.empty())
						{
							const FMTvertex_descriptor& DESCRIPTOR = actives.front();
							//const FMTvertex_descriptor DESCRIPTOR = actives.back();
							//actives.pop_back();
							auto inserted = right_period.insert(DESCRIPTOR);
							if (inserted.second)
							{
								p_descriptors.push_back(DESCRIPTOR);
								if (m_gotDeath||boost::out_degree(DESCRIPTOR, data)>1)
									{
									FMToutedge_pair edge_pair;
									for (edge_pair = boost::out_edges(DESCRIPTOR, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
										{
											const FMTvertex_descriptor NEXT_DESCRIPTOR = boost::target(*edge_pair.first, data);
											if (right_period.find(NEXT_DESCRIPTOR) == right_period.end())
											{
												const FMTBaseEdgeProperties& Edge = data[*edge_pair.first];
												const int ACTION_ID = Edge.getActionID();
												if (ACTION_ID >= 0)
												{
													//const FMTvertex_descriptor NEXT_DESCRIPTOR = boost::target(*edge_pair.first, data);
													actives.push(NEXT_DESCRIPTOR);
													//actives.push_back(NEXT_DESCRIPTOR);
												}
											}
										}
									}
							}
							actives.pop();
						}

					}
					if (p_period <= p_model.getParameter(Models::FMTintmodelparameters::LENGTH))
					{
						std::sort(p_descriptors.begin(), p_descriptors.end());
						nodescache.at(p_period).setValidVertices(p_node, p_descriptors);//always cache everything with a mask matching!
					}
				}
				else {
					const Core::FMTMask THE_STATIC_MASK = p_model.getStaticMask(p_node, true);
					const std::vector<size_t> BLOCKS_SUBSET = THE_STATIC_MASK.getNonFullBlocks();
					FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(p_period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const Core::FMTDevelopment& DEV = data[*vertex_iterator].get();
						if (DEV.getMask().isSubsetOf(THE_STATIC_MASK, BLOCKS_SUBSET))//DEV.getMask().isSubsetOf(THE_STATIC_MASK))
						{
							p_descriptors.push_back(*vertex_iterator);
						}
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("For node: " + std::string(p_node), "FMTGraph::getNodeByStaticMask", __LINE__, __FILE__);
			}
			return p_descriptors;
		}

		// DocString: FMTGraph::_keepForSerie
		/**
		@brief Return true if a vertex must be kept while building an action serie.
		@param[in] targetdescriptor the target vertex.
		@param[in] theaction the action iterator.
		@param[in] actions the model actions.
		@param[in] mask the mask.
		@return true if the vertex is kept else false.
		*/
		bool _keepForSerie(
			const FMTvertex_descriptor& targetdescriptor,
			std::vector<Core::FMTAction>::const_iterator theaction,
			const size_t& seriemaxsize,
			const Models::FMTModel& p_model,
			bool& onserie) const
		{
			try {
				std::vector<Core::FMTAction>::const_iterator acit = _getActionOfFirstSerie(targetdescriptor, 
																			seriemaxsize, p_model);
				if (acit == p_model.actions.end())
				{
					onserie = false;
				}
				else if (acit == theaction)
				{
					onserie = true; 
				}else {
					return _isValidSeriePath(targetdescriptor, acit, p_model);
				}
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTGraph::_keepForSerie", __LINE__, __FILE__);
			}
			return false;
		}

		// DocString: FMTGraph::_getActionSerie
		/**
		@brief Return the action serie starting from a vertex.
		@param[in] targetdescriptor the target vertex.
		@param[in] maxactions the maximal number of actions.
		@param[in] actions the model actions.
		@return the action serie.
		*/
		std::vector<std::string> _getActionSerie(
			FMTvertex_descriptor targetdescriptor,
			const size_t& maxactions, 
			const std::vector<Core::FMTAction>& actions) const

		{
			std::vector<std::string>theserie;
			try {
				size_t inedgessize = boost::in_degree(targetdescriptor, data);
				while (inedgessize > 0 && theserie.size() < maxactions)
				{
					if (inedgessize > 0)
					{
						if (inedgessize > 1)
						{
							/*FMTinedge_iterator inedge_iterator, inedge_end;
							std::string actions;
							for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
							{
								const FMTBaseEdgeProperties& inedgeproperties = data[*inedge_iterator];
								actions += std::to_string(inedgeproperties.getActionID()) + " ";
							}
							const FMTBaseVertexProperties& targetproperties = data[targetdescriptor];
							_exhandler->raise(Exception::FMTexc::FMTrangeerror,
								"Development " + std::string(targetproperties.get()) + " has " + std::to_string(inedgessize) + " in edges (" + actions + ")",
								"FMTGraph::_getActionSerie", __LINE__, __FILE__);*/
							break;
						}
						FMTinedge_iterator inedge_iterator, inedge_end;
						boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data);
						const FMTBaseEdgeProperties& inedgeproperties = data[*inedge_iterator];
						const int actionid = inedgeproperties.getActionID();
						if (actionid >= 0)
						{
							theserie.insert(theserie.begin(), actions.at(actionid).getName());
						}
						targetdescriptor = boost::source(*inedge_iterator, data);
						inedgessize = boost::in_degree(targetdescriptor, data);
					}
				}
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTGraph::_getActionSerie", __LINE__, __FILE__);
			}
			return theserie;
		}

		// DocString: FMTGraph::_getActionOfFirstSerie
		/**
		@brief Return the action of the first serie of a vertex.
		@param[in] p_target the target vertex.
		@param[in] p_SerieSize the serie size.
		@param[in] p_actions the model actions.
		@return the action iterator.
		*/
		std::vector<Core::FMTAction>::const_iterator _getActionOfFirstSerie(
			const FMTvertex_descriptor& p_target,
			size_t p_SerieMaxSize, 
			const Models::FMTModel& p_model) const
		{
			try {
				const std::vector<std::string> TARGET_SERIE = _getActionSerie(p_target,
																		p_SerieMaxSize, p_model.actions);
				if (!TARGET_SERIE.empty())
				{
					for (std::vector<Core::FMTAction>::const_iterator acit = p_model.actions.cbegin();
						acit != p_model.actions.cend(); ++acit)
					{
						if (acit->isPartOfASerie() && acit->isAllowedInSerie(TARGET_SERIE))
						{
							return acit;
						}
					}
				}
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTGraph::_getActionOfFirstSerie", __LINE__, __FILE__);
			}
			return p_model.actions.cend();
		}

		// DocString: FMTGraph::_isValidSeriePath
		/**
		@brief Return true if the path from a vertex is a valid action serie.
		@param[in] p_target the target vertex.
		@param[in] p_ActionSerie the action serie iterator.
		@param[in] p_actions the model actions.
		@return true if the serie path is valid else false.
		*/
		bool _isValidSeriePath(
			const FMTvertex_descriptor& p_target,
			std::vector<Core::FMTAction>::const_iterator p_ActionSerie,
			const Models::FMTModel& p_model) const
		{
			try {
				const int MODEL_LENGTH = p_model.getParameter(Models::FMTintmodelparameters::LENGTH);
				Core::FMTDevelopment stand = data[p_target].get();
				while (stand.getPeriod() <= MODEL_LENGTH)
					{
					if (stand.operable(*p_ActionSerie,p_model.yields))
						{
						return true;
						}
					stand = stand.grow();
					}
			}catch (...)
				{
				_exhandler->printExceptions("", "FMTGraph::_isValidSeriePath", __LINE__, __FILE__);
				}
			return false;
		}

		// DocString: FMTGraph::_setSerieOperabilityNDeath
		/**
		@brief Set the operability and the death of the actions of a serie for a vertex.
		@param[in] p_target the target vertex.
		@param[in] p_MaxSerieSize the maximal serie size.
		@param[in,out] p_actions the model actions.
		*/
		void _setSerieOperabilityNDeath(
			const FMTvertex_descriptor& p_target,
			const size_t& p_MaxSerieSize,
			std::vector<Core::FMTAction>::const_iterator p_ActionSerie,
			const Models::FMTModel& p_model,
			bool& p_grow,
			bool& p_Operable) const
		{
			//valide the _asap and _alap here
			try {
				const std::vector<std::string> TARGET_SERIE = _getActionSerie(p_target,
																p_MaxSerieSize, p_model.actions);
				const Core::FMTSerie* SERIE = p_ActionSerie->getSerie(TARGET_SERIE);
				p_Operable = true;
				const int MODEL_LENGTH = p_model.getParameter(Models::FMTintmodelparameters::LENGTH);
				Core::FMTDevelopment stand = data[p_target].get().grow();
				if (stand.getPeriod() <= MODEL_LENGTH &&
					stand.operable(*p_ActionSerie, p_model.yields))//Stand is still operable next period
					{
					p_grow = true;
					if (SERIE->isALAP())
						{
						p_Operable = false;
						}
				}else {
					p_grow = false;
					}
				if (SERIE->isASAP())
					{
					p_grow = false;
					p_Operable = true;
					}

			}catch (...)
			{
				_exhandler->printExceptions("", "FMTGraph::_setSerieOperabilityNDeath", __LINE__, __FILE__);
			}
		}

		// DocString: FMTGraph::isPeriodStart
		/**
		@brief Return true if a vertex is at the start of a period in a given adjacency list.
		@param[in] out_vertex the vertex descriptor.
		@param[in] p_graph the adjacency list.
		@return true if the vertex starts a period else false.
		*/
		static bool isPeriodStart(
			const FMTvertex_descriptor& out_vertex,
			const FMTadjacency_list& p_graph)
		{
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, p_graph);
				inedge_iterator != inedge_end; ++inedge_iterator)
				{
				const FMTBaseEdgeProperties& edgeprop = p_graph[*inedge_iterator];
				if (edgeprop.getActionID() == -1)
					{
						return true;
					}
				}
			return false;
		}

		struct VertexPeriodicFilter
		{
			VertexPeriodicFilter(const  FMTadjacency_list&  p_graph, int p_period) :
				m_from(&p_graph), 
				m_period(p_period){ }
			VertexPeriodicFilter() = default;

			bool operator()(const FMTvertex_descriptor& p_descriptor) const
				{
				const int VERTEX_PERIOD = (*m_from)[p_descriptor].get().getPeriod();
				return  (VERTEX_PERIOD < m_period ||
					(VERTEX_PERIOD == m_period && isPeriodStart(p_descriptor, (*m_from))));
				}
		private:
			FMTadjacency_list const* m_from;
			FMTGraphStats* m_stats;
			int m_period;
		};

		struct EdgePeriodicFilter
		{
			EdgePeriodicFilter(const  FMTadjacency_list& p_graph,
				FMTGraphStats& p_stats, int p_period) :
				m_from(&p_graph), m_stats(&p_stats),
				m_period(p_period) {
			}
			EdgePeriodicFilter() = default;
			bool operator()(const FMTedge_descriptor& p_descriptor) const
			{
				bool returned = false;
				FMTvertex_descriptor sourceVertex = boost::source(p_descriptor, (*m_from));
				const int VERTEX_PERIOD = (*m_from)[sourceVertex].get().getPeriod();
				if (VERTEX_PERIOD < m_period)
				{
					const int ACTION_ID = (*m_from)[p_descriptor].getActionID();
					if (ACTION_ID >= 0)
					{
						++m_stats->cols;
					}
					returned = true;
				}
				return returned;
			}
		private:
			FMTadjacency_list const* m_from;
			FMTGraphStats* m_stats;
			int m_period;
		};

		mutable std::allocator<FMTvertex_descriptor> m_allocator;
		size_t m_reserve;
		mutable  std::vector<FMTvertex_descriptor> m_selectedVertices;

    };


template<> inline std::map<int, int> FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::getOutVariables(const FMTvertex_descriptor& out_vertex) const
	{
		std::map<int, int> mapping;
		try {
			FMToutedge_pair edge_pair;
			for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
			{
				const FMTEdgeProperties& edgeprop = data[*edge_pair.first];
				int actionid = edgeprop.getActionID();
				mapping[actionid] = edgeprop.getVariableID();
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTGraph::getOutVariables", __LINE__, __FILE__);
		}
		return mapping;
	}

template<> inline std::vector<Core::FMTDevelopmentPath> FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::getPaths(
	const FMTvertex_descriptor& out_vertex, 
	const int& actionID) const
	{
		std::vector<Core::FMTDevelopmentPath>paths;
		paths.reserve(boost::out_degree(out_vertex,data));
		try {
			for (FMToutedge_pair edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
			{
				const FMTEdgeProperties& edgeprop = data[*edge_pair.first];
				if (edgeprop.getActionID() == actionID)
				{
					const FMTBaseVertexProperties& vertex_target = data[target(*edge_pair.first, data)];
					paths.push_back(Core::FMTDevelopmentPath(vertex_target.get().getMask(),
						vertex_target.get().getAge(), 
						vertex_target.get().getLock(),
						vertex_target.get().getPeriod(),
						edgeprop.getProportion()));
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTGraph::getPaths", __LINE__, __FILE__);
			}
		return paths;
	}

template<> inline double FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::inArea(
	const FMTvertex_descriptor& out_vertex, 
	const double*& solution, 
	int actionid , 
	bool growth) const
	{
		double area = 0;
		try {
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTEdgeProperties& edgeprop = data[*inedge_iterator];
				if (edgeprop.getActionID() == actionid || !growth)
				{
					area += *(solution + edgeprop.getVariableID()) * (edgeprop.getProportion() / 100);
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTGraph::inArea", __LINE__, __FILE__);
			}
		return area;
	}

template<> inline double FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::outArea(
	const FMTvertex_descriptor& out_vertex, 
	const int& actionID, 
	const double*& solution) const
	{
		double value = 0;
		FMToutedge_iterator outedge_iterator, outedge_end;
		try {
			for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
			{
				const FMTEdgeProperties& edgeprop = data[*outedge_iterator];
				if (edgeprop.getActionID() == actionID)
				{
					value += *(solution + edgeprop.getVariableID()) * (edgeprop.getProportion() / 100);
				}
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTGraph::outArea", __LINE__, __FILE__);
		}
		return value;
	}

template<> inline std::map<int, double> FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::getVariables(
	const Models::FMTModel& model, 
	const Core::FMTOutputNode& output_node, 
	const std::vector<FMTvertex_descriptor>& vertices) const
{
	std::map<int, double>variables;
	try {
		if (!vertices.empty())
		{
			//std::vector<Core::FMTDevelopmentPath>paths;
			//Core::FMTAction optimization_action;
			const std::vector<const Core::FMTAction*> selected = output_node.source.targets(model.actions);
			for (const FMTvertex_descriptor& vertex : vertices)
			{
				const Core::FMTDevelopment& development = data[vertex].get();
				const Graph::FMTGraphVertexToYield vertexinfo = getVertexToYieldInfo(model,vertex);
				if (output_node.source.useInEdges())
				{
					
					Core::FMTDevelopment newDev(development);
					newDev.setPeriod(newDev.getPeriod() - 1);
					const double coef = output_node.source.getCoef(newDev, model.yields, &vertexinfo) * output_node.factor.getCoef(newDev, model.yields, &vertexinfo) * output_node.constant;
					
					if (development.getPeriod() == 0)
					{
						const std::map<int, int>vars = getOutVariables(vertex);
						updateVarsMap(variables, vars.at(-1), coef);
					}
					else {
						FMTinedge_iterator inedge_iterator, inedge_end;
						for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
						{
							const FMTEdgeProperties& edgeprop = data[*inedge_iterator];
							const int actionid = edgeprop.getActionID();
							if (actionid < 0 || output_node.source.isAction())
							{
								updateVarsMap(variables, edgeprop.getVariableID(), (edgeprop.getProportion() / 100)*coef);
								continue;
							}
							const FMTvertex_descriptor sourceverex = boost::source(*inedge_iterator, data);
							const FMTVertexProperties& sourceproperties = data[sourceverex];
							if ((sourceproperties.get().getPeriod() == development.getPeriod() && !periodStart(sourceverex)))
							{
								updateVarsMap(variables, edgeprop.getVariableID(), (edgeprop.getProportion() / 100)*coef);
							}
						}
					}
				}
				else {
					//const std::map<int, int>outvars = getOutVariables(vertex);
					const std::vector<const FMTBaseEdgeProperties*> ACTION_EDGES = getActionEdges(vertex, model.actions.size());
					for (const Core::FMTAction* act : selected)
					{
						const int actionID = static_cast<int>(std::distance(&(*model.actions.begin()), act));
						if (ACTION_EDGES.at(actionID))
						{
							const std::vector<Core::FMTDevelopmentPath>paths = getPaths(vertex, actionID);
							const double action_coef = output_node.source.getCoef(development, model.yields, &vertexinfo, &paths, act) * output_node.factor.getCoef(development, model.yields, &vertexinfo, &paths, act) * output_node.constant;
							updateVarsMap(variables, ACTION_EDGES.at(actionID)->getVariableID(), action_coef);
						}
					}
				}
			}
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTGraph::getVariables", __LINE__, __FILE__);
	}
	return variables;
}

template<> inline FMTGraphStats FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::erasePeriod(
	std::vector<int>& deletedconstraints,
	std::vector<int>&deletedvariables,
	bool keepbounded)
{
	try {
		typename std::vector<FMTvertex_pair>::iterator periodit = this->getFirstBlock();
		FMTvertex_iterator vertexit, vertexend;
		for (boost::tie(vertexit, vertexend) = *periodit; vertexit != vertexend; ++vertexit)
		{
			const FMTvertex_descriptor& vertexm_location = *vertexit;
			FMTinedge_iterator inedge_iterator, inedge_end;
			bool gotinedges = false;
			const bool nottransferrow = isNoTransfer(vertexm_location, 1);

			if (!nottransferrow)
			{
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertexm_location, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					gotinedges = true;
					const FMTEdgeProperties& edgeproperty = data[*inedge_iterator];
					int varvalue = edgeproperty.getVariableID();
					if (std::find(deletedvariables.begin(), deletedvariables.end(), varvalue) == deletedvariables.end())
					{
						--stats.cols;
						deletedvariables.push_back(varvalue);
					}
					--stats.edges;
				}
			}
			
			if (!keepbounded)
			{
				if (!nottransferrow)
				{
					const std::map<int, int>outvars = this->getOutVariables(vertexm_location);
					for (std::map<int, int>::const_iterator varit = outvars.begin(); varit != outvars.end(); varit++)
					{
						if (std::find(deletedvariables.begin(), deletedvariables.end(), varit->second) == deletedvariables.end())
						{
							--stats.cols;
							deletedvariables.push_back(varit->second);
						}
						--stats.edges;
					}
				}
				boost::clear_out_edges(vertexm_location, data);
			}
			boost::clear_in_edges(vertexm_location, data);
		}
		FMTvertex_iterator firstvertex;
		FMTvertex_iterator lastvertex;
		bool assigned = false;
		for (boost::tie(vertexit, vertexend) = *periodit; vertexit != vertexend; ++vertexit)
		{
			const FMTvertex_descriptor& vertexm_location = *vertexit;
			FMTVertexProperties& vertexproperty = data[vertexm_location];
			const int constvalue = vertexproperty.getConstraintId();
			if (constvalue >= 0)
			{
				--stats.rows;
				--stats.transfer_rows;
				deletedconstraints.push_back(constvalue);
				vertexproperty.setConstraintId(-1);
			}
			if (!keepbounded || out_degree(vertexm_location, data) == 0)
			{
				boost::remove_vertex(vertexm_location, data);
				--stats.vertices;
			}
			else if (!assigned)
			{
				firstvertex = vertexit;
				lastvertex = vertexit;
				assigned = true;
			}
			else {
				lastvertex = vertexit;
			}
		}
		++lastvertex;
		*periodit = FMTvertex_pair(firstvertex, lastvertex);
		rebaseCache();
		const int firstPeriod = this->getFirstActivePeriod();
		/*if (static_cast<size_t>(firstPeriod) < nodescache.size())
		{
			nodescache[firstPeriod].clear();
		}*/
		nodescache.clear();

	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTGraph::erasePeriod", __LINE__, __FILE__);
	}
	return stats;
}

template<> inline void FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::setConstraintId(
	const FMTvertex_descriptor& vertex,
	const int& id)
{
	data[vertex].setConstraintId(id);
}

template<> inline bool FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::getTransferRow(
	const FMTvertex_descriptor& vertex_descriptor,
	std::vector<int>&row_starts,
	std::vector<int>& cols,
	std::vector<double>& cols_value) const
{
	try {
		FMTinedge_iterator inedge_iterator, inedge_end;
		FMTVertexProperties vertex_property = data[vertex_descriptor];
		row_starts.push_back(static_cast<int>(cols.size()));
		bool gotin = false;
		for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
		{
			const FMTEdgeProperties& edgeprop = data[*inedge_iterator];
			cols.push_back(edgeprop.getVariableID());
			cols_value.push_back((edgeprop.getProportion() / 100));
			gotin = true;
		}
		std::vector<int>locals;
		FMToutedge_iterator outedge_iterator, outedge_end;
		for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(vertex_descriptor, data); outedge_iterator != outedge_end; ++outedge_iterator)
		{
			const FMTEdgeProperties& edgeprop = data[*outedge_iterator];
			const int edgevar = edgeprop.getVariableID();
			if (std::find(locals.begin(), locals.end(), edgevar) == locals.end())
			{
				cols.push_back(edgevar);
				locals.push_back(edgevar);
				cols_value.push_back(-1);
			}
		}
		if (!gotin)
		{
			const Core::FMTDevelopment& dev = vertex_property.get();
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"No in variables for development "+std::string(dev), "FMTGraph::getTransferRow", __LINE__, __FILE__);
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTGraph::getTransferRow", __LINE__, __FILE__);
	}
	return true;
}

template<> inline double FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::getInProportion(
	const FMTvertex_descriptor& vertex_descriptor) const
{
	try {
		FMTinedge_iterator inedge_iterator, inedge_end;
		FMTVertexProperties vertex_property = data[vertex_descriptor];
		for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
		{
			const FMTEdgeProperties& edgeprop = data[*inedge_iterator];
			return edgeprop.getProportion();
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTGraph::getInProportion", __LINE__, __FILE__);
	}
	return 1;
}

template<> inline void FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::updateMatrixIndex(
	const std::vector<int>& removedvariables,
	const std::vector<int>& removedconstraints)
{
	try {
		if (!removedconstraints.empty())
		{
			const int& maxconstraint = removedconstraints.back();
			const int& minconstraint = removedconstraints.front();
			FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
				FMTVertexProperties& vertexproperty = data[*vertex_iterator];
				const int actualconstraint = vertexproperty.getConstraintId();
				if (actualconstraint >= 0)
				{
					int toRemove = 0;
					if (actualconstraint > minconstraint && actualconstraint < maxconstraint)
					{
						std::vector<int>::const_iterator removeditconstraint = removedconstraints.begin();
						while (removeditconstraint != removedconstraints.end() && actualconstraint > *removeditconstraint)
						{
							++toRemove;
							++removeditconstraint;
						}
					}
					else if (actualconstraint > maxconstraint)
					{
						toRemove = static_cast<int>(removedconstraints.size());
					}
					vertexproperty.setConstraintId(actualconstraint - toRemove);
				}
			}
		}

		if (!removedvariables.empty())
		{
			const int& maxvariable = removedvariables.back();
			const int& minvariable = removedvariables.front();
			FMTedge_iterator edge_iterator, edge_iterator_end;
			for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
			{
				FMTEdgeProperties& edgeproperty = data[*edge_iterator];
				const int actualvariable = edgeproperty.getVariableID();
				if (actualvariable >= 0)
				{
					int toRemove = 0;
					if (actualvariable > minvariable && actualvariable < maxvariable)
					{
						std::vector<int>::const_iterator removeditvariable = removedvariables.begin();
						while (removeditvariable != removedvariables.end() && actualvariable > *removeditvariable)
						{
							++toRemove;
							++removeditvariable;
						}
					}
					else if (actualvariable > maxvariable)
					{
						toRemove = static_cast<int>(removedvariables.size());
					}
					edgeproperty.setVariableID(actualvariable - toRemove);
				}

			}
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTGraph::updateMatrixIndex", __LINE__, __FILE__);
	}
}

}

namespace boost
	{
	namespace serialization
		{
		template<> struct guid_defined<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>> : boost::mpl::true_
			{

			};
		template<> struct guid_defined<Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>> : boost::mpl::true_
			{

			};
		template<> inline const char * guid<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>>()
			{
			return "Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>";
			}
		template<> inline const char * guid<Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>>()
			{
			return "Graph::FMTgraph<Graph::FMTbasevertexproperties,Graph::FMTbaseedgeproperties>";
			}
		}
	}

#endif // FMTGRAPH_H