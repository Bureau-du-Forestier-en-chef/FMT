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

#include "FMTvertexproperties.h"
#include "FMTedgeproperties.h"
#include "FMTgraphstats.h"

#include "FMToutputnode.h"
#include "FMTdevelopment.h"
#include "FMTactualdevelopment.h"
#include "FMTfuturdevelopment.h"
#include "FMTdevelopmentpath.h"
#include "FMTschedule.h"
#include "FMTconstraint.h"
#include "FMToutputnodecache.h"
#include <boost\unordered_set.hpp>
#include "FMTpredictor.h"
#include "FMTgraphvertextoyield.h"



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

#include "FMTmodel.h"
#include "FMTaction.h"
#include "FMTtheme.h"
#include "FMTobject.h"
#include <tuple>
#include <assert.h>
#include "FMTlookup.h"
#include "boost/graph/graphviz.hpp"
#include "FMTexceptionhandler.h"
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


template <class tvertexproperties,class tedgeproperties>
class FMTEXPORT FMTgraph : public Core::FMTobject
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
		FMTgraph() :
			Core::FMTobject(),
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
		virtual ~FMTgraph() = default;

		FMTgraph(const FMTgraphbuild lbuildtype) :
			Core::FMTobject(),
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

		FMTgraph(const FMTgraph& rhs) :
			Core::FMTobject(rhs),
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

		void swap(FMTgraph& rhs)
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
		void reserveVertices(size_t p_reserve)
		{
			data.m_vertices.reserve(p_reserve);
		}
		FMTgraph& operator = (const FMTgraph& rhs)
		{
			if (this != &rhs)
			{
				Core::FMTobject::operator=(rhs);
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

		/**
		* @brief Copy the graph but dont copy vertex with period > p_period
		* @param[in] p_period maximal period
		* @return a new graph
		*/
		FMTgraph copyToPeriod(int p_period) const
		{
			FMTgraph newGraph;
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
				_exhandler->raiseFromCatch("", "FMTgraph::copyToPeriod", __LINE__, __FILE__);
			}
			return newGraph;
		}


		bool operator == (const FMTgraph& rhs) const
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
				_exhandler->raiseFromCatch("", "FMTgraph::operator==", __LINE__, __FILE__);
			}
			return false;
		}
		bool operator != (const FMTgraph& rhs) const
		{
			return (!(*this == rhs));
		}
		void clearCache()
		{
			std::vector<FMToutputnodecache<FMTvertex_descriptor FMT_COMMA FMTvertex_iterator>>().swap(nodescache);
		}
		void clearDevelopments()
		{
			std::vector<FMTvertex_pair>().swap(developments);
		}
		FMTgraphbuild getBuildType() const
		{
			return buildtype;
		}
		void setBuildType(const FMTgraphbuild& build)
		{
			buildtype = build;
		}
		boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>getDevsSet(const int& period) const
		{
			boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>> basedevs;
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = developments.at(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					basedevs.insert(Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>(*vertex_iterator, data[*vertex_iterator].get()));
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getDevsSet", __LINE__, __FILE__);
			}
			return basedevs;
		}

		bool containsDevelopment(
			const Core::FMTdevelopment& development,
			const boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, 
			Core::FMTdevelopment>>& alldevs) const
		{
			return (alldevs.find(Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>(development)) != alldevs.end());
		}

		std::queue<FMTvertex_descriptor> initialize(const std::vector<Core::FMTactualdevelopment>& actdevelopments)
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
				for (const Core::FMTactualdevelopment& development : actdevelopments)
				{
					const FMTvertexproperties properties(development, constraint_id);
					const FMTvertex_descriptor newvertex = boost::add_vertex(properties, data);
					P0descriptors.push_back(newvertex);
					//++vertex_iterator;
					++stats.vertices;
				}
				size_t poid = 0;
				boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>> devsets;
				for (const Core::FMTactualdevelopment& development : actdevelopments)
				{
					//P1
					Core::FMTfuturdevelopment p1dev(development);
					p1dev.setPeriod(actualperiod);
					const FMTvertex_descriptor tovertex = addDevelopment(p1dev, devsets);
					actives.push(tovertex);
					//Now set the edge!!
					const FMTedgeproperties newEdge(edge_id, stats.edges, proportion);
					boost::add_edge(P0descriptors.at(poid), tovertex, newEdge, data);
					++stats.edges;
					++poid;
				}
				FMTvertex_iterator firstp0;
				FMTvertex_iterator lastone;
				boost::tie(firstp0, vertex_iterator_end) = boost::vertices(data);
				lastone = firstp0;
				for (const Core::FMTactualdevelopment& development : actdevelopments)
				{
					++lastone;
				}
				developments[startingperiod] = FMTvertex_pair(firstp0, lastone);
				developments[actualperiod] = FMTvertex_pair(lastone, vertex_iterator_end);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::initialize", __LINE__, __FILE__);
			}
			return actives;

		}
		// DocString: FMTgraph::build(const Models::FMTmodel&,std::queue<FMTvertex_descriptor>)
		/**
		This function build one period at the end of the graph. Like in Woodstock, future developments of a given action
		can not be operate by an action that is before in the section action. The actions in the model must be
		ordered to take that in account.
		*/
		FMTgraphstats build(
			const Models::FMTmodel& model,
			std::queue<FMTvertex_descriptor> actives,
			int compressageoperability = 1)
		{
			FMTgraphstats newstats;
			try {
				FMTgraphstats statsDiff(stats);
				const int actualperiod = getPeriod();
				const bool GOT_SERIES = model.useActionSerie();
				const size_t maxseriesize = model.getSeriesMaxSize();
				boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>> actualdevs = getDevsSet(actualperiod);
				int action_id = 0;
				for (const Core::FMTaction& action : model.actions)
				{
					std::queue<FMTvertex_descriptor> new_actives;
					while (!actives.empty())
					{
						const FMTvertex_descriptor front_vertex = actives.front();
						actives.pop();
						const Graph::FMTgraphvertextoyield vertexinfo = getVertexToYieldInfo(model, front_vertex);
						const FMTbasevertexproperties& front_properties = data[front_vertex];
						const Core::FMTdevelopment& active_development = front_properties.get();
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
										"FMTgraph::build", __LINE__, __FILE__);
								}
								_setSerieOperabilityNDeath(front_vertex, maxseriesize,
										(model.actions.cbegin() + action_id), model, 
									DoGrow, operateStand);
							}
							if (operateStand)
								{
								const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
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
				_exhandler->raiseFromCatch("", "FMTgraph::build", __LINE__, __FILE__);
			}

			return newstats;
		}

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
				_exhandler->raiseFromCatch("", "FMTgraph::isNoTransfer", __LINE__, __FILE__);
			}
			return false;
		}



		double getInProportion(const FMTvertex_descriptor& vertex_descriptor) const
		{
			return 1;
		}

		FMTgraphstats naturalGrowth(
			std::queue<FMTvertex_descriptor> actives, 
			FMTgraphstats statsDiff, 
			bool typeIImatrix = false, 
			bool splitgrowth = false)
		{
			try {
				boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>> nextperiods;
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
					FMTbasevertexproperties front_properties = data[front_vertex];
					const Core::FMTdevelopment active_development = front_properties.get();
					const Core::FMTfuturdevelopment grown_up = active_development.grow();
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
					const FMTedgeproperties newEdge(-1, variableindex, proportion);
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
				_exhandler->raiseFromCatch("", "FMTgraph::naturalGrowth", __LINE__, __FILE__);
			}
			return (statsDiff - stats);

		}

		std::vector<const Core::FMTdevelopment*> noChoice(
			const Core::FMTmask& baseMask, 
			const int& death_id) const
		{
			std::vector<const Core::FMTdevelopment*>noactions;
			try {
				FMTvertex_iterator vertexit, vertexend;
				for (boost::tie(vertexit, vertexend) = developments.at(getFirstPeriod()); vertexit != vertexend; ++vertexit)
				{
					const Core::FMTdevelopment& base_dev = data[*vertexit].get();
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
								const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
								const int action_id = edgeprop.getactionID();
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
				_exhandler->raiseFromCatch("", "FMTgraph::noChoice", __LINE__, __FILE__);
			}
			return noactions;

		}



		void getVariableNames(
			const std::vector<Core::FMTaction>& actions,
			std::vector<std::string>& colnames) const
		{
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				const std::string toRemove = "+-/*";
				size_t vertices_id = 0;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					std::string basename = std::string(Core::FMTdevelopment(data[*vertex_iterator].get()));
					const std::string vertexid = "_V" + std::to_string(vertices_id);
					boost::remove_erase_if(basename, boost::is_any_of(toRemove));
					FMToutedge_iterator outit, outend;
					for (boost::tie(outit, outend) = boost::out_edges(*vertex_iterator, data); outit != outend; ++outit)
					{
						std::string actionname = "EVO";
						const int variableid = data[*outit].getvariableID();
						if (colnames.at(variableid).empty())
						{
							const int actionid = data[*outit].getactionID();
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
				_exhandler->raiseFromCatch("", "FMTgraph::getVariableNames", __LINE__, __FILE__);
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
						unsigned long long avmemory = FMTobject::getAvailableMemory();
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
				_exhandler->raisefromcatch("", "FMTgraph::cleannodecaching", __LINE__, __FILE__);
			}
		}*/

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
						std::string name = std::string(Core::FMTdevelopment(data[*vertex_iterator].get()));
						boost::remove_erase_if(name, boost::is_any_of(toRemove));
						rownames[rowid] = name + vertexid;
					}
					++vertices_id;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getTransferRowNames", __LINE__, __FILE__);
			}
		}


		std::vector<Core::FMTactualdevelopment> getPeriodStopDev(
			const int location, 
			const double* actual_solution) const
		{
			std::vector<Core::FMTactualdevelopment>all_period_stop_devs;
			try {
				FMTvertex_iterator vertexit, vertexend;
				for (boost::tie(vertexit, vertexend) = developments.at(location); vertexit != vertexend; ++vertexit)
				{
					if (periodStop(*vertexit))
					{
						const double area = inArea(*vertexit, actual_solution);
						const FMTbasevertexproperties& vetexprop = data[*vertexit];
						all_period_stop_devs.push_back(Core::FMTactualdevelopment(vetexprop.get(), area));
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getPeriodStopDev", __LINE__, __FILE__);
			}
			return all_period_stop_devs;

		}
		std::map<std::string, double> getOutput(
			const Models::FMTmodel& model, 
			const Core::FMToutput& output,
			int period, 
			const double* solution, 
			Core::FMToutputlevel level = Core::FMToutputlevel::standard) const
		{
			Core::FMTtheme targetTheme;
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
					for (const Core::FMToutputnode& output_node : output.getNodes(equation))
					{
						bool checkGraph = true;
						if (output_node.source.isAction())
						{
							const std::vector<const Core::FMTaction*> ACTIONS = Core::FMTActionComparator(output_node.source.getAction()).getAllAggregates(model.actions);
							std::vector<const Core::FMTaction*>::const_iterator AIt = ACTIONS.begin();
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
					const std::vector<Core::FMToutputnode> allnodes = output.getNodes(equation, 1, false, period);


					if (allnodes.empty())
					{
						if (level == Core::FMToutputlevel::developpement)
						{
							_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
								"Cannot get level values by developement",
								"FMTgraph::getOutput", __LINE__, __FILE__);
						}
						allequations["Total"] = equation;

					}
					else {
						size_t outid = 0;
						for (const Core::FMToutputnode& output_node : allnodes)
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
				_exhandler->raiseFromCatch("For output: " + std::string(output), "FMTgraph::getOutput", __LINE__, __FILE__);
			}

			return results;

		}

		FMTvertex_descriptor getDevelopment(
			const Core::FMTdevelopment& development,
			const boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, 
			Core::FMTdevelopment>>& alldevs) const
		{
			const Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment> tofind(development);
			return alldevs.find(tofind)->memoryobject;
		}

		const Core::FMTdevelopment& getDevelopment(const FMTvertex_descriptor& descriptor) const
		{
			return data[descriptor].get();
		}

		FMTvertex_descriptor addDevelopment(
			const Core::FMTfuturdevelopment& futurdevelopment,
			boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>& alldevs, 
			bool forcenewone = false)
		{
			try {
				if (!this->containsDevelopment(futurdevelopment, alldevs) || forcenewone)
				{
					const int constraint_id = -1;
					const FMTvertexproperties properties(futurdevelopment, constraint_id);
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
							const Core::FMTdevelopment& DEV = getDevelopment(*base_iterator);
							if (DEV.getPeriod() == futurdevelopment.getPeriod())
							{
								alldevs.insert(Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>(*base_iterator, data[*base_iterator].get()));
							}
							++base_iterator;
						}

					}
					FMTvertex_descriptor newvertex = boost::add_vertex(properties, data);
					alldevs.insert(Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>(newvertex, data[newvertex].get()));
					++stats.vertices;
					return newvertex;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::addDevelopment", __LINE__, __FILE__);
			}
			return getDevelopment(futurdevelopment, alldevs);

		}

		FMTvertex_descriptor addDevelopment(const Core::FMTfuturdevelopment& futurdevelopment)
		{
			FMTvertex_descriptor newvertex = data.null_vertex();
			try {
				const int constraint_id = -1;
				const FMTvertexproperties properties(futurdevelopment, constraint_id);
				newvertex = boost::add_vertex(properties, data);
				++stats.vertices;
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::addDevelopment", __LINE__, __FILE__);
			}
			return newvertex;
		}

		size_t hash(size_t seed = 0) const
		{
			try {
				boost::hash_combine(seed, boost::hash<Core::FMTdevelopment>()(data[*developments.at(0).first].get()));
				FMTedge_iterator edge_iterator, edge_iterator_end;
				for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
					boost::hash_combine(seed, edgeprop.getactionID());
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::hash", __LINE__, __FILE__);
			}
			return seed;

		}
		void addAction(const int& actionID,
			FMTgraphstats& statsDiff,
			std::queue<FMTvertex_descriptor>& actives,
			const FMTvertex_descriptor& out_vertex,
			const std::vector<Core::FMTdevelopmentpath>& paths,
			boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>& devsets,
			bool inserie = false)
		{
			try {
				//int variable_id = statsDiff.cols;
				//++statsDiff.cols;
				//std::vector<FMTvertex_descriptor>active_vertex;
				bool newchoice = false;
				for (const Core::FMTdevelopmentpath& devpath : paths)
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
						const FMTedgeproperties newEdge(actionID, statsDiff.cols, devpath.getProportion());
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
				_exhandler->raiseFromCatch("On action Id " + std::to_string(actionID), "FMTgraph::addAction", __LINE__, __FILE__);
			}
		}

		void addAction(const int& actionID,
			FMTgraphstats& statsDiff,
			std::queue<FMTvertex_descriptor>& actives,
			const FMTvertex_descriptor& out_vertex,
			const std::vector<Core::FMTdevelopmentpath>& paths)
		{
			try {
				int variable_id = statsDiff.cols;
				++statsDiff.cols;
				std::vector<FMTvertex_descriptor>active_vertex;
				for (const Core::FMTdevelopmentpath& devpath : paths)
				{
					const FMTedgeproperties newEdge(actionID, variable_id, devpath.getProportion());
					const FMTvertex_descriptor tovertex = this->addDevelopment(devpath.getDevelopment());
					actives.push(tovertex);
					boost::add_edge(out_vertex, tovertex, newEdge, data);
					++stats.edges;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::addAction", __LINE__, __FILE__);
			}
		}

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
					const FMTbaseedgeproperties& edgeprop = data[*outedge_iterator];
					if (edgeprop.getactionID() == actionID)
					{

						value += *(solution + edgeprop.getvariableID()) * (edgeprop.getProportion() / 100);
					}
				}

			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::outArea", __LINE__, __FILE__);
			}
			return value;
		}

		FMTvertex_descriptor getGrowthSource(const FMTvertex_descriptor& out_vertex) const
		{
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
					if (edgeprop.getactionID() < 0)
					{
						return boost::source(*inedge_iterator, data);
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getGrowthSource", __LINE__, __FILE__);
			}
			return FMTvertex_descriptor();
		}

		std::vector<FMTvertex_descriptor> getActionSources(
			const FMTvertex_descriptor& out_vertex, 
			const int& actionid) const
		{
			std::vector<FMTvertex_descriptor> vsources;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
					if (edgeprop.getactionID() == actionid)
					{
						vsources.push_back(boost::source(*inedge_iterator, data));
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getActionSources", __LINE__, __FILE__);
			}
			return vsources;
		}

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
					const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
					if (edgeprop.getactionID() == actionid || !growth)
					{
						area += *(solution + edgeprop.getvariableID()) * (edgeprop.getProportion() / 100);
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::inArea", __LINE__, __FILE__);
			}
			return area;
		}

		bool periodStart(const FMTvertex_descriptor& out_vertex) const
		{
			try {
				return isPeriodStart(out_vertex, data);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::periodStart", __LINE__, __FILE__);
			}
			return false;
		}

		bool onlyPeriodStart(const FMTvertex_descriptor& out_vertex) const
		{
			try {
				if (boost::in_degree(out_vertex, data) == 1)
				{
					FMTinedge_iterator inedge_iterator, inedge_end;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
					{
						const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
						if (edgeprop.getactionID() == -1)
						{
							return true;
						}
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::onlyPeriodStart", __LINE__, __FILE__);
			}
			return false;

		}

		int getMaximalLock(const int& period)
		{
			int lock = 0;
			try {
				FMTvertex_iterator vertexit, vertexend;
				for (boost::tie(vertexit, vertexend) = developments.at(period); vertexit != vertexend; ++vertexit)
				{
					const Core::FMTdevelopment& dev = data[*vertexit].get();
					if (dev.getLock() > lock)
					{
						lock = dev.getLock();
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getMaximalLock", __LINE__, __FILE__);
			}
			return lock;
		}

		bool periodStop(const FMTvertex_descriptor& out_vertex) const
		{
			try {
				FMToutedge_iterator outedge_iterator, outedge_end;
				const FMTbasevertexproperties& source_properties = data[out_vertex];
				if ((source_properties.get().getPeriod()) == 0)
				{
					return true;
				}
				for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*outedge_iterator];
					if (edgeprop.getactionID() == -1)
					{
						return true;
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::periodStop", __LINE__, __FILE__);
			}
			return false;
		}

		std::vector<Core::FMTdevelopmentpath> getPaths(
			const FMTvertex_descriptor& out_vertex,
			const int& actionID) const
		{
			std::vector<Core::FMTdevelopmentpath>paths;
			try {
				paths.reserve(boost::out_degree(out_vertex, data));
				for (FMToutedge_pair edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
				{
					const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
					if (edgeprop.getactionID() == actionID)
					{
						const FMTbasevertexproperties& vertex_target = data[target(*edge_pair.first, data)];
						paths.push_back(Core::FMTdevelopmentpath(vertex_target.get().getMask(),
							vertex_target.get().getAge(),
							vertex_target.get().getLock(),
							vertex_target.get().getPeriod(),
							edgeprop.getProportion()));
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getPaths", __LINE__, __FILE__);
			}
			return paths;
		}

		bool isValidOuputNode(
			const Models::FMTmodel& model, 
			const Core::FMToutputnode& node, 
			std::vector<const Core::FMTaction*>& action_IDS, 
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
				_exhandler->raiseFromCatch("", "FMTgraph::isValidOuputNode", __LINE__, __FILE__);
			}
			return false;
		}


		bool isValidGraphNode(
			const Models::FMTmodel& p_model, 
			const FMTvertex_descriptor& p_vertex_descriptor,
			const Core::FMToutputnode& p_node, 
			const std::vector<const Core::FMTaction*>& p_selected, 
			const std::vector<size_t>& p_blocksSubset) const
		{
			try {
				const Core::FMTdevelopment& p_development = data[p_vertex_descriptor].get();
				const Core::FMTmask& DEV_MASK = p_development.getMask();
				const Core::FMTmask& SOURCE_MASK = p_node.source.getMask();
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
								const Graph::FMTgraphvertextoyield YIELD_INFO = getVertexToYieldInfo(p_model, p_vertex_descriptor);
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
				_exhandler->raiseFromCatch("", "FMTgraph::isValidGraphNode", __LINE__, __FILE__);
			}
			return false;
		}

		std::map<int, double> locateNode(
			const Models::FMTmodel& model,
			Core::FMToutputnode output_node,
			int period) const
		{
			try {
				Core::FMToutputnode tempNode(output_node);
				const std::vector<FMTvertex_descriptor>& vertices = getNode(model, tempNode, period);
				return getVariables(model, tempNode, vertices);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("Getting node at period "+std::to_string(period)+" for node "+
					std::string(output_node)+" for model "+model.getName(), "FMTgraph::locateNode", __LINE__, __FILE__);
			}
			return std::map<int, double>();

		}
		std::map<std::string, std::map<int, double>> locateNodeByTheme(
			const Models::FMTmodel& model, 
			Core::FMToutputnode output_node, 
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
						const Core::FMTdevelopment& dev = getDevelopment(vert);
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
				_exhandler->raiseFromCatch("", "FMTgraph::locateNodeByTheme", __LINE__, __FILE__);
			}
			return variablesreturn;

		}


		void setNodeSize(const size_t& p_size)
		{
			m_reserve = p_size;
			m_selectedVertices.reserve(m_reserve);
		}

		std::vector<FMTvertex_descriptor> getNode(
			const Models::FMTmodel& model, 
			Core::FMToutputnode& output_node, 
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
					std::vector<const Core::FMTaction*> selected(m_allocator);
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
				_exhandler->raiseFromCatch("", "FMTgraph::getNode", __LINE__, __FILE__);
			}
			//std::sort(locations.begin(), locations.end());
			//return locations;
			std::sort(m_selectedVertices.begin(), m_selectedVertices.end());
			return m_selectedVertices;
		}

		std::map<int, double> getVariables(
			const Models::FMTmodel& model, 
			const Core::FMToutputnode& output_node, 
			const std::vector<FMTvertex_descriptor>& vertices) const
		{
			return std::map<int, double>();
		}

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
					const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
					const int actionid = edgeprop.getactionID();
					if (actionid >= 0 && actionsop.at(actionid))
					{
						return true;
					}
				}
			}

			/*}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::isAnyOperables", __LINE__, __FILE__);
			}*/
			return false;
		}

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
				_exhandler->raiseFromCatch("", "FMTgraph::anyOperables", __LINE__, __FILE__);
			}
			return false;
		}

		std::vector<int>getInVariables(const FMTvertex_descriptor& out_vertex) const
		{
			std::vector<int>invars;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
					invars.push_back(edgeprop.getvariableID());

				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getInVariables", __LINE__, __FILE__);
			}
			return invars;
		}

		std::vector<double>getInProportions(const FMTvertex_descriptor& out_vertex) const
		{
			std::vector<double>inprops;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
					inprops.push_back(edgeprop.getProportion());

				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getInProportions", __LINE__, __FILE__);
			}
			return inprops;
		}

		std::map<int, int>getInIdsVariables(const FMTvertex_descriptor& out_vertex) const
		{
			std::map<int, int> mapping;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
					int actionid = edgeprop.getactionID();
					//*_logger << actionid << " test "<< edgeprop.getvariableID() <<"\n";
					mapping[actionid] = edgeprop.getvariableID();
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getInIdsVariables", __LINE__, __FILE__);
			}
			return mapping;
		}

		std::map<int, int> getOutVariables(const FMTvertex_descriptor& out_vertex) const
		{
			std::map<int, int> mapping;
			try {
				FMToutedge_pair edge_pair;
				for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
				{
					const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
					int actionid = edgeprop.getactionID();
					mapping[actionid] = edgeprop.getvariableID();
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getOutVariables", __LINE__, __FILE__);
			}
			return mapping;
		}

		std::vector<const FMTbaseedgeproperties*> getActionEdges(
			const FMTvertex_descriptor& pOutVertex,
			const size_t& p_actionSize) const
		{
			std::vector<const FMTbaseedgeproperties*>output(p_actionSize, nullptr);
			try {
				FMToutedge_pair edge_pair;
				for (edge_pair = boost::out_edges(pOutVertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
				{
					const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
					const int actionId = edgeprop.getactionID();
					if (actionId >= 0)
					{
						output[edgeprop.getactionID()] = &edgeprop;
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getOutVariables", __LINE__, __FILE__);
			}
			return output;
		}

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
						const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
						const int actionid = edgeprop.getactionID();
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
				_exhandler->raiseFromCatch("", "FMTgraph::getOutActions", __LINE__, __FILE__);
			}
			return actions;
		}

		std::vector<const Core::FMTaction*> selectedActions(
			const Models::FMTmodel& model, 
			const std::vector<int>& action_IDS) const
		{
			std::vector<const Core::FMTaction*>selected;
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
				_exhandler->raiseFromCatch("", "FMTgraph::selectedActions", __LINE__, __FILE__);
			}
			return selected;
		}

		bool constraintLenght(
			const Core::FMTconstraint& constraint, 
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
				_exhandler->raiseFromCatch("", "FMTgraph::constraintLenght", __LINE__, __FILE__);
			}
			return (start < static_cast<int>(developments.size()) && start <= stop);
		}

		FMTgraphstats getStats(const Core::FMTmask& p_Subset)
		{
			FMTgraphstats SubsetStats;
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				std::unordered_set<int>selectedVariables;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); 
					vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const FMTvertex_descriptor DESCRIPTOR = *vertex_iterator;
					const FMTbasevertexproperties& VERTEX_PROPERTIES = data[DESCRIPTOR];
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
				_exhandler->raiseFromCatch("", "FMTgraph::getStats", __LINE__, __FILE__);
				}
			return SubsetStats;
		}

		FMTgraphstats getStats() const
		{
			return stats;
		}

		FMTgraphstats* getStatsPtr()
		{
			return &stats;
		}

		void setStats(const FMTgraphstats& newstats)
		{
			stats = newstats;
		}

		FMTgraphstats buildSchedule(
			const Models::FMTmodel& model,
			std::queue<FMTvertex_descriptor> actives,
			const Core::FMTschedule& schedule)
		{
			FMTgraphstats statsDiff(stats);
			try {
				//developments.push_back(boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>());
				//developments.back().reserve(actives.size());
				const int actualperiod = getPeriod();
				boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>> actualdevs = getDevsSet(actualperiod);
				int action_id = 0;
				for (const Core::FMTaction& action : model.actions)
				{
					if (schedule.find(action) != schedule.end())
					{
						std::queue<FMTvertex_descriptor> new_actives;
						while (!actives.empty())
						{
							const FMTvertex_descriptor front_vertex = actives.front();
							actives.pop();
							FMTbasevertexproperties front_properties = data[front_vertex];
							const Core::FMTdevelopment& active_development = front_properties.get();
							bool death = false;
							if ((((schedule.at(action)).find(active_development) != (schedule.at(action)).end()) ||
								(!action.doRespectLock() && active_development.getLock() != 0 &&
									(schedule.at(action)).find(active_development.clearLock()) != (schedule.at(action)).end())))
							{
								const Graph::FMTgraphvertextoyield vertexinfo = getVertexToYieldInfo(model, front_vertex);
								if (active_development.operable(action, model.yields, &vertexinfo))
								{
									if (action.getName() == "_DEATH")
									{
										death = true;
									}
									const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
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
				_exhandler->raiseFromCatch("", "FMTgraph::buildSchedule", __LINE__, __FILE__);
			}
			const bool typeIIforestmodel = (model.getParameter(Models::FMTintmodelparameters::MATRIX_TYPE) == 2);
			return naturalGrowth(actives, statsDiff, typeIIforestmodel);
		}

		FMTgraphstats erasePeriod(
			std::vector<int>& deletedconstraints,
			std::vector<int>& deletedvariables,
			bool keepbounded = false)
		{

			return getStats();
		}

		bool empty() const
		{
			return developments.empty();
		}

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
				_exhandler->raiseFromCatch("", "FMTgraph::getActiveVertices", __LINE__, __FILE__);
			}
			return actives;
		}


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
		size_t size() const
		{
			return developments.size();
		}

		void setConstraintID(const FMTvertex_descriptor& vertex, const int& id)
		{

		}

		bool getTransferRow(
			const FMTvertex_descriptor& vertex_descriptor,
			std::vector<int>& row_starts,
			std::vector<int>& cols,
			std::vector<double>& cols_value) const
		{
			return false;
		}

		void getInitialBounds(
			std::vector<double>& lower_bounds, 
			std::vector<double>& upper_bounds) const
		{
			try {

				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = developments.at(getFirstActivePeriod()); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const FMTvertex_descriptor descriptor = *vertex_iterator;
					const FMTbasevertexproperties& vproperty = data[descriptor];
					const std::map<int, int>outs = getOutVariables(descriptor);
					lower_bounds[outs.at(-1)] = vproperty.getBaseRhs();
					upper_bounds[outs.at(-1)] = vproperty.getBaseRhs();
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getInitialBounds", __LINE__, __FILE__);
			}
		}
		size_t nEdges() const
		{
			return boost::num_edges(data);
		}

		std::map<std::string, double> getSource(
			const Models::FMTmodel& model,
			const Core::FMToutputnode& node,
			int period, const Core::FMTtheme& theme,
			const double* solution, 
			Core::FMToutputlevel level = Core::FMToutputlevel::standard) const
		{
			try {
				Core::FMToutputnode tempNode(node);
				const std::vector<FMTvertex_descriptor>& vertices = getNode(model, tempNode, period);
				return getValues(model, vertices, tempNode, theme, solution, level);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("For node: " + std::string(node), "FMTgraph::getSource", __LINE__, __FILE__);
			}
			return std::map<std::string, double>();
		}

		Graph::FMTgraphvertextoyield getVertexToYieldInfo(
			const Models::FMTmodel& model, 
			const FMTvertex_descriptor& descriptor) const
		{
			try {
				return Graph::FMTgraphvertextoyield(model, *this, reinterpret_cast<const void*>(&descriptor));
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getVertexToYieldInfo", __LINE__, __FILE__);
			}
			return Graph::FMTgraphvertextoyield();
		}

		const FMTvertex_descriptor* getVertexFromVertexInfo(
			const Graph::FMTgraphvertextoyield* info) const
		{
			try {
				return reinterpret_cast<const FMTvertex_descriptor*>(info->getVertexPtr());
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getVertexFromVertexInfo", __LINE__, __FILE__);
			}
			return nullptr;
		}

		std::map<std::string, double> getValues(
			const Models::FMTmodel& model, 
			const std::vector<FMTvertex_descriptor>& vertices,
			const Core::FMToutputnode& node, 
			const Core::FMTtheme& theme,
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
					const std::vector<const Core::FMTaction*> selected = node.source.targets(model.actions);

					//Core::FMTaction optimization_action;
					for (const FMTvertex_descriptor& vertex : vertices)
					{
						const Core::FMTdevelopment& development = data[vertex].get();
						const Graph::FMTgraphvertextoyield vertexinfo = getVertexToYieldInfo(model, vertex);
						if (level == Core::FMToutputlevel::standard)
						{
							toFill = values.find(development.getMask().getAttribute(theme));
						}
						else if (level == Core::FMToutputlevel::developpement)
						{
							std::string value;
							if (node.source.isNextPeriod())//If it looks at next period make sure to show the right dev...
							{
								value = std::string(Core::FMTdevelopment(data[getGrowthSource(vertex)].get()));
							}
							else {
								value = std::string(Core::FMTdevelopment(development));
							}
							std::pair<std::map<std::string, double>::iterator, bool> target = values.insert(std::pair<std::string, double>(value, 0));
							toFill = target.first;
						}
						if (node.source.useInEdges())
						{
							Core::FMTdevelopment newDev(development);
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
							for (const Core::FMTaction* act : selected)
							{
								double action_value = 0;
								const int actionID = static_cast<int>(std::distance(&(*model.actions.begin()), act));
								const std::vector<Core::FMTdevelopmentpath>paths = getPaths(vertex, actionID);
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
				_exhandler->raiseFromCatch("For node: " + std::string(node) + " on theme " + std::string(theme), "FMTgraph::getValues", __LINE__, __FILE__);
			}
			return values;
		}

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
				_exhandler->raiseFromCatch("", "FMTgraph::getPeriod", __LINE__, __FILE__);
			}
			return 0;
		}

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
				_exhandler->raiseFromCatch("", "FMTgraph::rebaseCache", __LINE__, __FILE__);
			}
		}
		
		bool sameEdgesAs(const FMTgraph& rhs) const
		{
			bool different = false;
			try {
				FMTedge_iterator thisedge_iterator, thisedge_iterator_end;
				FMTedge_iterator rhsedge_iterator, rhsedge_iterator_end;
				boost::tie(thisedge_iterator, thisedge_iterator_end) = boost::edges(data);
				boost::tie(rhsedge_iterator, rhsedge_iterator_end) = boost::edges(rhs.data);
				while (!different && thisedge_iterator != thisedge_iterator_end && rhsedge_iterator != rhsedge_iterator_end)
				{
					const FMTbaseedgeproperties& thisedgeprop = data[*thisedge_iterator];
					const FMTbaseedgeproperties& rhsedgeprop = rhs.data[*rhsedge_iterator];
					if (thisedgeprop.getactionID() != rhsedgeprop.getactionID())
					{
						different = true;
					}
					++thisedge_iterator;
					++rhsedge_iterator;
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::sameEdgesAs", __LINE__, __FILE__);
			}
			return different;
		}

		void updateMatrixIndex(
			const std::vector<int>& removedvariables,
			const std::vector<int>& removedconstraints)
		{

		}
		int getFirstActivePeriod() const
		{
			return static_cast<int>(std::distance(developments.begin(), getFirstConstBlock()));
		}

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
					if (data[*activeedge].getactionID() >= 0)
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
				_exhandler->raiseFromCatch("", "FMTgraph::getLastDisturbance", __LINE__, __FILE__);
			}

			return lastedge;
		}

		void fillUpLastActions(
			const int& targetperiod,
			const FMTvertex_descriptor& targetdescriptor, 
			std::vector<const FMTbaseedgeproperties*>& lastactions,
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
						const FMTbaseedgeproperties& inedgeproperties = data[*inedge_iterator];
						const FMTvertex_descriptor& sourcevertex = boost::source(*inedge_iterator, data);
						activevertex.push(sourcevertex);
						const int sourceperiod = data[sourcevertex].get().getPeriod();
						if (inedgeproperties.getactionID() >= 0)
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
				_exhandler->raiseFromCatch("", "FMTgraph::fillUpLastActions", __LINE__, __FILE__);
			}

		}

		size_t getAmountOfPaths(
			const Core::FMTdevelopment& dev, 
			const int& actionid,
			const Models::FMTmodel& model,
			const boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, 
			Core::FMTdevelopment>>& actualperioddevs) const
		{
			size_t amount = 0;
			try {
				std::vector<FMTvertex_descriptor>paths;
				if (actionid >= 0)
				{
					for (const Core::FMTdevelopmentpath& path : dev.operate(model.actions.at(actionid), model.transitions.at(actionid), model.yields, model.themes))
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
						const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
						if (edgeprop.getactionID() < 0)
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
							const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
							if (edgeprop.getactionID() < 0)
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
				_exhandler->printExceptions("for " + std::string(dev), "FMTgraph::getAmountOfPaths", __LINE__, __FILE__);
			}
			return amount;
		}

		std::set<Core::FMTSerie> getAllSeries(
			FMTvertex_descriptor targetdescriptor, 
			const std::vector<std::string>& p_series,
			const std::vector<Core::FMTaction>& actions, 
			const std::unordered_set<int>& actionselected, 
			const Core::FMTmask& mask) const
		{
			std::set<Core::FMTSerie>theseries;
			try {
				size_t inedgessize = boost::in_degree(targetdescriptor, data);
				const FMTbasevertexproperties& mainproperties = data[targetdescriptor];
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
							const FMTbasevertexproperties& targetproperties = data[subdescriptor];
							if (targetproperties.get().getMask().isSubsetOf(mask))//natural growth or action in the aggregate
							{
								const FMTbaseedgeproperties& inedgeproperties = data[*inedge_iterator];
								const int actionid = inedgeproperties.getactionID();
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
							const FMTbasevertexproperties& targetproperties = data[targetdescriptor];
							//*_logger << "in for out " << std::string(targetproperties.get()) << " in degree " << inedgessize << "\n";
							if (targetproperties.get().getMask().isSubsetOf(mask))//natural growth or action in the aggregate
							{
								const FMTbaseedgeproperties& inedgeproperties = data[*inedge_iterator];
								const int actionid = inedgeproperties.getactionID();
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
				_exhandler->printExceptions("", "FMTgraph::getAllSeries", __LINE__, __FILE__);
			}
			return theseries;
		}

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
						const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
						if (edgeprop.getactionID() != -1)
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
				_exhandler->raiseFromCatch("", "FMTgraph::timeSinceLastAction", __LINE__, __FILE__);
			}
			return std::numeric_limits<size_t>::max();
		}

		std::vector<FMTpredictor> getPredictors(
			const FMTvertex_descriptor& targetdescriptor, 
			const Models::FMTmodel& model,
			const std::vector<std::string>& yieldnames,
			const size_t& depth, 
			bool periodonevalues = false, 
			bool withGCBMid = true) const
		{
			std::vector<FMTpredictor> predictors;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				const FMTbasevertexproperties& targetproperties = data[targetdescriptor];
				const int targetperiod = targetproperties.get().getPeriod();
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					std::vector<const FMTbaseedgeproperties*>lastactions;
					std::vector<int>distances;
					const FMTvertex_descriptor& sourcevertex = boost::source(*inedge_iterator, data);
					const FMTbasevertexproperties& sourceproperties = data[sourcevertex];
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
				_exhandler->raiseFromCatch("", "FMTgraph::getPredictors", __LINE__, __FILE__);
			}
			return predictors;
		}

		int getFirstPeriod() const
		{
			return data[*developments.at(getFirstActivePeriod() + 1).first].get().getPeriod();
		}

		void postSolve(
			const Core::FMTmaskfilter& filter,
			const std::vector<Core::FMTtheme>& originalbasethemes,
			const std::vector<int>& actionmapconnection)
		{
			try {
				developments.clear();
				nodescache.clear();//Some postSolve can be done here to keep some usefull information but for now just clear
				//start by remapping the actions
				FMTedge_iterator edge_iterator, edge_iterator_end;
				for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
				{
					FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
					if (edgeprop.getactionID() >= 0)
					{
						edgeprop.setactionID(actionmapconnection.at(edgeprop.getactionID()));
					}
				}
				boost::unordered_map<Core::FMTmask, Core::FMTmask>presolvetopostsolve;
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					FMTbasevertexproperties& vertexprop = data[*vertex_iterator];
					const Core::FMTmask& presolvemask = vertexprop.get().getMask();
					boost::unordered_map<Core::FMTmask, Core::FMTmask>::const_iterator mskit = presolvetopostsolve.find(presolvemask);
					if (mskit != presolvetopostsolve.end())
					{
						vertexprop.setDevlopementMask(mskit->second);
					}
					else {
						const Core::FMTmask postsolvedmask = presolvemask.postSolve(filter, originalbasethemes);
						presolvetopostsolve[presolvemask] = postsolvedmask;
						vertexprop.setDevlopementMask(postsolvedmask);
					}
				}
				_generateDevelopments();
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::postSolve", __LINE__, __FILE__);
			}
		}

		Core::FMTschedule getSchedule(
			const std::vector<Core::FMTaction>& actions,
			const double* actual_solution, 
			const int& lperiod, 
			bool withlock = false) const
		{
			Core::FMTschedule newSchedule(lperiod, *this, withlock);
			try {
				if (static_cast<int>(size()) > lperiod && lperiod > 0)
				{
					//newSchedule.setPeriod(lperiod);
					//std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::map<int, double>>>schedule_solution;
					//const double* actual_solution = this->getColSolution();
					FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(lperiod); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const FMTvertex_descriptor vertex = *vertex_iterator;
						std::map<int, int>variables = getOutVariables(vertex);
						variables.erase(-1);
						if (!variables.empty())
						{
							const Core::FMTdevelopment& dev = data[*vertex_iterator].get();
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
										schedule_solution[actions[variable_iterator.first]] = std::map<Core::FMTdevelopment, std::map<int, double>>();
									}
									const Core::FMTdevelopment& basedev = getDevelopment(deviterator.memoryobject);*/
									newSchedule.addEvent(dev, *(actual_solution + variable_iterator.second), actions.at(variable_iterator.first));
									/*Core::FMTdevelopment lockout = basedev.clearLock();
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
					/*Core::FMTschedule newSchedule(lperiod, schedule_solution);
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
				_exhandler->raiseFromCatch("at period " + std::to_string(lperiod), "FMTgraph::getSchedule", __LINE__, __FILE__);
			}

			return newSchedule;
		}
		// DocString: FMTgraph::getRotations
		/**
		Based on a mask and an action get the distribution of the rotations (in period) taken to complete a serie of action. A serie can contain a subserie for an
		aggregate of actions.
		Args:
			model FMTmodel: the model
			mask : the FMTmask of the rotations
			aggregate : std::string on which the last action is considered to be in
		Returns: Distribution of rotation period taken per series (where the serie is in string action1-action2-action3... string is the serie and int is the number of periods taken
		*/
		std::set<Core::FMTSerie> getRotations(
			const Models::FMTmodel& model, 
			const Core::FMTmask& mask, 
			const std::string& aggregate) const
		{
			std::set<Core::FMTSerie>theseries;
			try {
				const Core::FMToutputsource baseSource(Core::FMTspec(), mask,
					Core::FMTotar::actual, "", aggregate);
				const Core::FMToutputsource baseFactor(Core::FMTotar::val, 1.0);
				const Core::FMToutputnode baseNode(baseSource, baseFactor, 1.0);
				const int minperiod = 1;
				const int maxperiod = static_cast<int>(developments.size() - 2);
				const std::unordered_set<int> actionsets = Core::FMTActionComparator(aggregate).getAllAggregatesSet(model.actions, true);
				for (int period = minperiod; period <= maxperiod; ++period)
				{
					Core::FMToutputnode periodNode(baseNode);
					for (const FMTvertex_descriptor& targetdescriptor : getNode(model, periodNode, period))
					{
						FMToutedge_pair edge_pair;
						for (edge_pair = boost::out_edges(targetdescriptor, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
						{
							const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
							int actionid = edgeprop.getactionID();
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
				_exhandler->raiseFromCatch("", "FMTgraph::getRotations", __LINE__, __FILE__);
			}
			return theseries;
		}

		std::set<std::string> getAllDevelopmentsMask(
			const std::vector<Core::FMTtheme>& p_themesToIgnore) const 
		{
			std::set<std::string> masks;
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const Core::FMTdevelopment& DEV = data[*vertex_iterator].get();
					Core::FMTmask maskObj(DEV.getMask());
					for (const Core::FMTtheme& THEME : p_themesToIgnore) {
						maskObj.set(THEME, "?");
					}
					masks.insert(std::string(maskObj));

				}
			}
			catch (...){
				_exhandler->raiseFromCatch("", "FMTgraph::getAllDevelopmentsMask", __LINE__, __FILE__);
			}
			return masks;
		}

		Core::FMTschedule getOutVariablesProportions(
			const std::vector<Core::FMTaction>& actions, 
			const double* actual_solution, 
			const int& lperiod, 
			bool withlock = false) const
		{
			Core::FMTschedule newSchedule(lperiod, *this, withlock);
			try {
				if (static_cast<int>(size()) > lperiod && lperiod > 0)
				{
					FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(lperiod); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const FMTvertex_descriptor vertex = *vertex_iterator;
						std::map<int, int>variables = getOutVariables(vertex);
						const Core::FMTdevelopment& dev = data[*vertex_iterator].get();
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
				_exhandler->raiseFromCatch("at period " + std::to_string(lperiod), "FMTgraph::getOutVariablesProportions", __LINE__, __FILE__);
			}

			return newSchedule;
		}

		operator std::string() const
		{
			std::ostringstream stream;
			try {
				//write_graphviz(stream, data);
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::std::string()", __LINE__, __FILE__);
			}
			return stream.str();
		}
	protected:
		FMTadjacency_list data;
		bool m_gotDeath;
        FMTgraphbuild buildtype;
		std::vector<FMTvertex_pair>developments;
		mutable std::vector<FMToutputnodecache<FMTvertex_descriptor FMT_COMMA FMTvertex_iterator>>nodescache;
		typedef typename std::vector<FMToutputnodecache<FMTvertex_descriptor FMT_COMMA FMTvertex_iterator>>::reverse_iterator reversecachenodeit;
        FMTgraphstats stats;

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
				_exhandler->raiseFromCatch("", "FMTgraph::updateVarsMap", __LINE__, __FILE__);
			}
		}

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
						"", "FMTgraph::getFirstBlock", __LINE__, __FILE__);
				}
			}catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getFirstBlock", __LINE__, __FILE__);
			}
			return periodit;
		}

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
						"Empty graph", "FMTgraph::getFirstConstBlock", __LINE__, __FILE__);
				}
			}catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::getFirstConstBlock", __LINE__, __FILE__);
			}
			return periodit;
		}

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
						const FMTbaseedgeproperties& edgeprop = data[*outit];
						if (edgeprop.getactionID() >= 0)
							{
							if (edgeprop.getactionID() < theactionid)
								{
								return true;
								}
							_exhandler->raise(Exception::FMTexc::FMTsourcetotarget_transition,
									"Action recursivity " + std::to_string(theactionid)+" from "+
								std::string(data[descriptor].get()), "FMTgraph::isDependant", __LINE__, __FILE__);
							newEdge = false;
							}
						}
					}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTgraph::isDependant", __LINE__, __FILE__);
			}
			return false;
		}
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, unsigned int version) const
		{
			ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
			FMTobject::forceSave(ar, version);
			ar& BOOST_SERIALIZATION_NVP(m_gotDeath);
			ar& BOOST_SERIALIZATION_NVP(data);
			ar& BOOST_SERIALIZATION_NVP(stats);
			ar& BOOST_SERIALIZATION_NVP(buildtype);
			ar& BOOST_SERIALIZATION_NVP(m_reserve);
		}
		template<class Archive>
		void load(Archive& ar, unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
			FMTobject::forceSave(ar, version);
			ar& BOOST_SERIALIZATION_NVP(m_gotDeath);
			ar& BOOST_SERIALIZATION_NVP(data);
			_generateDevelopments();
			ar& BOOST_SERIALIZATION_NVP(stats);
			ar& BOOST_SERIALIZATION_NVP(buildtype);
			ar& BOOST_SERIALIZATION_NVP(m_reserve);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()

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
						const FMTbasevertexproperties& properties = data[*vertex];
						const Core::FMTdevelopment& dev = properties.get();
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
				_exhandler->raiseFromCatch("", "FMTgraph::_generateDevelopments", __LINE__, __FILE__);
			}
		}
		/**
		 * @Get the vertex descriptors for one period
		 * @param[in] p_period
		 * @param[out] p_masks
		 * @param[in] p_reserve
		 * @return vector of descriptors
		 */
		std::vector<FMTvertex_descriptor> getDescriptors(
			const int& p_period,
			std::vector<const Core::FMTmask*>& p_masks, 
			const size_t& p_reserve) const
		{
			std::vector<FMTvertex_descriptor> p_descriptors;
			p_descriptors.reserve(p_reserve);
			p_masks.reserve(p_reserve);
			FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(p_period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
				const Core::FMTdevelopment& DEV = data[*vertex_iterator].get();
				p_masks.push_back(&DEV.getMask());
				p_descriptors.push_back(*vertex_iterator);
			}
			return p_descriptors;
		}
		/**
		 * @Test if vectors of descriptors is subset.
		 * @param[in] p_period
		 * @param[in] p_reserve
		 */
		void getSubset(
			const Core::FMTmask& p_staticMask,
			std::vector<const Core::FMTmask*>& p_masks,
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
		/**
		 * @Test if vectors of descriptors is subset.
		 * @param[in] p_node
		 * @param[in] p_model
		 * @param[in] p_period
		 * @param[in] p_reserve
		 */
		std::vector<FMTvertex_descriptor> threadedStaticSearch(
			const Core::FMToutputnode& p_node,
			const Models::FMTmodel& p_model,
			const int& p_period, 
			const size_t& p_reserve) const
		{
			const Core::FMTmask theStaticMask = p_model.getStaticMask(p_node, true);
			std::vector<const Core::FMTmask*>masks;
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
					workers.push_back(boost::thread(std::bind(&FMTgraph::getSubset,
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
				const FMTbaseedgeproperties& Edge = data[*edge_pair.first];
				const int& EdgeId = Edge.getactionID();
				
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

		FMTvertex_descriptor getNextPeriod(const FMTvertex_descriptor& p_vertex) const
		{
			FMTvertex_descriptor NextPeriod = data.null_vertex();
			FMToutedge_pair edge_pair = boost::out_edges(p_vertex, data);
			while (edge_pair.first != edge_pair.second &&
				NextPeriod == data.null_vertex())
			{
				const FMTbaseedgeproperties& Edge = data[*edge_pair.first];
				const int& EdgeId = Edge.getactionID();
				if (EdgeId < 0) // évolution naturelle si actionid < 0
				{
					NextPeriod = boost::target(*edge_pair.first, data);
				}
				++edge_pair.first;
			}
			return NextPeriod;
		}

		/**
		 * @brief select the vertices based on the static nodes. If p_descriptors is not empty it will presume same node but other period.
		 * @param[in] p_model the optimization model.
		 * @param[in] p_node the output node.
		 * @param[in] p_period the period of the node.
		 * @param[in] p_useCache
		 * @return vector of cached vertices
		 */
		std::vector<FMTvertex_descriptor> setNodeByStaticMask(
			const Models::FMTmodel& p_model,
			const Core::FMToutputnode& p_node, 
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
						const Core::FMTmask THE_STATIC_MASK = p_model.getStaticMask(p_node, true);
						const std::vector<size_t> BLOCKS_SUBSET = THE_STATIC_MASK.getNonFullBlocks();
						FMTvertex_iterator vertex_iterator, vertex_iterator_end;
						for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(p_period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
						{
							const Core::FMTdevelopment& DEV = data[*vertex_iterator].get();
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
												const FMTbaseedgeproperties& Edge = data[*edge_pair.first];
												const int ACTION_ID = Edge.getactionID();
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
					const Core::FMTmask THE_STATIC_MASK = p_model.getStaticMask(p_node, true);
					const std::vector<size_t> BLOCKS_SUBSET = THE_STATIC_MASK.getNonFullBlocks();
					FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					for (boost::tie(vertex_iterator, vertex_iterator_end) = getPeriodVertices(p_period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const Core::FMTdevelopment& DEV = data[*vertex_iterator].get();
						if (DEV.getMask().isSubsetOf(THE_STATIC_MASK, BLOCKS_SUBSET))//DEV.getMask().isSubsetOf(THE_STATIC_MASK))
						{
							p_descriptors.push_back(*vertex_iterator);
						}
					}
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("For node: " + std::string(p_node), "FMTgraph::getNodeByStaticMask", __LINE__, __FILE__);
			}
			return p_descriptors;
		}

		bool _keepForSerie(
			const FMTvertex_descriptor& targetdescriptor,
			std::vector<Core::FMTaction>::const_iterator theaction,
			const size_t& seriemaxsize,
			const Models::FMTmodel& p_model,
			bool& onserie) const
		{
			try {
				std::vector<Core::FMTaction>::const_iterator acit = _getActionOfFirstSerie(targetdescriptor, 
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
				_exhandler->printExceptions("", "FMTgraph::_keepForSerie", __LINE__, __FILE__);
			}
			return false;
		}

		std::vector<std::string> _getActionSerie(
			FMTvertex_descriptor targetdescriptor,
			const size_t& maxactions, 
			const std::vector<Core::FMTaction>& actions) const

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
								const FMTbaseedgeproperties& inedgeproperties = data[*inedge_iterator];
								actions += std::to_string(inedgeproperties.getactionID()) + " ";
							}
							const FMTbasevertexproperties& targetproperties = data[targetdescriptor];
							_exhandler->raise(Exception::FMTexc::FMTrangeerror,
								"Development " + std::string(targetproperties.get()) + " has " + std::to_string(inedgessize) + " in edges (" + actions + ")",
								"FMTgraph::_getActionSerie", __LINE__, __FILE__);*/
							break;
						}
						FMTinedge_iterator inedge_iterator, inedge_end;
						boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data);
						const FMTbaseedgeproperties& inedgeproperties = data[*inedge_iterator];
						const int actionid = inedgeproperties.getactionID();
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
				_exhandler->printExceptions("", "FMTgraph::_getActionSerie", __LINE__, __FILE__);
			}
			return theserie;
		}

		std::vector<Core::FMTaction>::const_iterator _getActionOfFirstSerie(
			const FMTvertex_descriptor& p_target,
			size_t p_SerieMaxSize, 
			const Models::FMTmodel& p_model) const
		{
			try {
				const std::vector<std::string> TARGET_SERIE = _getActionSerie(p_target,
																		p_SerieMaxSize, p_model.actions);
				if (!TARGET_SERIE.empty())
				{
					for (std::vector<Core::FMTaction>::const_iterator acit = p_model.actions.cbegin();
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
				_exhandler->printExceptions("", "FMTgraph::_getActionOfFirstSerie", __LINE__, __FILE__);
			}
			return p_model.actions.cend();
		}

		bool _isValidSeriePath(
			const FMTvertex_descriptor& p_target,
			std::vector<Core::FMTaction>::const_iterator p_ActionSerie,
			const Models::FMTmodel& p_model) const
		{
			try {
				const int MODEL_LENGTH = p_model.getParameter(Models::FMTintmodelparameters::LENGTH);
				Core::FMTdevelopment stand = data[p_target].get();
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
				_exhandler->printExceptions("", "FMTgraph::_isValidSeriePath", __LINE__, __FILE__);
				}
			return false;
		}

		void _setSerieOperabilityNDeath(
			const FMTvertex_descriptor& p_target,
			const size_t& p_MaxSerieSize,
			std::vector<Core::FMTaction>::const_iterator p_ActionSerie,
			const Models::FMTmodel& p_model,
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
				Core::FMTdevelopment stand = data[p_target].get().grow();
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
				_exhandler->printExceptions("", "FMTgraph::_setSerieOperabilityNDeath", __LINE__, __FILE__);
			}
		}

		static bool isPeriodStart(
			const FMTvertex_descriptor& out_vertex,
			const FMTadjacency_list& p_graph)
		{
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, p_graph);
				inedge_iterator != inedge_end; ++inedge_iterator)
				{
				const FMTbaseedgeproperties& edgeprop = p_graph[*inedge_iterator];
				if (edgeprop.getactionID() == -1)
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
			FMTgraphstats* m_stats;
			int m_period;
		};

		struct EdgePeriodicFilter
		{
			EdgePeriodicFilter(const  FMTadjacency_list& p_graph,
				FMTgraphstats& p_stats, int p_period) :
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
					const int ACTION_ID = (*m_from)[p_descriptor].getactionID();
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
			FMTgraphstats* m_stats;
			int m_period;
		};

		mutable std::allocator<FMTvertex_descriptor> m_allocator;
		size_t m_reserve;
		mutable  std::vector<FMTvertex_descriptor> m_selectedVertices;

    };


template<> inline std::map<int, int> FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getOutVariables(const FMTvertex_descriptor& out_vertex) const
	{
		std::map<int, int> mapping;
		try {
			FMToutedge_pair edge_pair;
			for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
			{
				const FMTedgeproperties& edgeprop = data[*edge_pair.first];
				int actionid = edgeprop.getactionID();
				mapping[actionid] = edgeprop.getvariableID();
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTgraph::getOutVariables", __LINE__, __FILE__);
		}
		return mapping;
	}

template<> inline std::vector<Core::FMTdevelopmentpath> FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getPaths(
	const FMTvertex_descriptor& out_vertex, 
	const int& actionID) const
	{
		std::vector<Core::FMTdevelopmentpath>paths;
		paths.reserve(boost::out_degree(out_vertex,data));
		try {
			for (FMToutedge_pair edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
			{
				const FMTedgeproperties& edgeprop = data[*edge_pair.first];
				if (edgeprop.getactionID() == actionID)
				{
					const FMTbasevertexproperties& vertex_target = data[target(*edge_pair.first, data)];
					paths.push_back(Core::FMTdevelopmentpath(vertex_target.get().getMask(),
						vertex_target.get().getAge(), 
						vertex_target.get().getLock(),
						vertex_target.get().getPeriod(),
						edgeprop.getProportion()));
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTgraph::getPaths", __LINE__, __FILE__);
			}
		return paths;
	}

template<> inline double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::inArea(
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
				const FMTedgeproperties& edgeprop = data[*inedge_iterator];
				if (edgeprop.getactionID() == actionid || !growth)
				{
					area += *(solution + edgeprop.getvariableID()) * (edgeprop.getProportion() / 100);
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTgraph::inArea", __LINE__, __FILE__);
			}
		return area;
	}

template<> inline double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::outArea(
	const FMTvertex_descriptor& out_vertex, 
	const int& actionID, 
	const double*& solution) const
	{
		double value = 0;
		FMToutedge_iterator outedge_iterator, outedge_end;
		try {
			for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
			{
				const FMTedgeproperties& edgeprop = data[*outedge_iterator];
				if (edgeprop.getactionID() == actionID)
				{
					value += *(solution + edgeprop.getvariableID()) * (edgeprop.getProportion() / 100);
				}
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTgraph::outArea", __LINE__, __FILE__);
		}
		return value;
	}

template<> inline std::map<int, double> FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getVariables(
	const Models::FMTmodel& model, 
	const Core::FMToutputnode& output_node, 
	const std::vector<FMTvertex_descriptor>& vertices) const
{
	std::map<int, double>variables;
	try {
		if (!vertices.empty())
		{
			//std::vector<Core::FMTdevelopmentpath>paths;
			//Core::FMTaction optimization_action;
			const std::vector<const Core::FMTaction*> selected = output_node.source.targets(model.actions);
			for (const FMTvertex_descriptor& vertex : vertices)
			{
				const Core::FMTdevelopment& development = data[vertex].get();
				const Graph::FMTgraphvertextoyield vertexinfo = getVertexToYieldInfo(model,vertex);
				if (output_node.source.useInEdges())
				{
					
					Core::FMTdevelopment newDev(development);
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
							const FMTedgeproperties& edgeprop = data[*inedge_iterator];
							const int actionid = edgeprop.getactionID();
							if (actionid < 0 || output_node.source.isAction())
							{
								updateVarsMap(variables, edgeprop.getvariableID(), (edgeprop.getProportion() / 100)*coef);
								continue;
							}
							const FMTvertex_descriptor sourceverex = boost::source(*inedge_iterator, data);
							const FMTvertexproperties& sourceproperties = data[sourceverex];
							if ((sourceproperties.get().getPeriod() == development.getPeriod() && !periodStart(sourceverex)))
							{
								updateVarsMap(variables, edgeprop.getvariableID(), (edgeprop.getProportion() / 100)*coef);
							}
						}
					}
				}
				else {
					//const std::map<int, int>outvars = getOutVariables(vertex);
					const std::vector<const FMTbaseedgeproperties*> ACTION_EDGES = getActionEdges(vertex, model.actions.size());
					for (const Core::FMTaction* act : selected)
					{
						const int actionID = static_cast<int>(std::distance(&(*model.actions.begin()), act));
						if (ACTION_EDGES.at(actionID))
						{
							const std::vector<Core::FMTdevelopmentpath>paths = getPaths(vertex, actionID);
							const double action_coef = output_node.source.getCoef(development, model.yields, &vertexinfo, &paths, act) * output_node.factor.getCoef(development, model.yields, &vertexinfo, &paths, act) * output_node.constant;
							updateVarsMap(variables, ACTION_EDGES.at(actionID)->getvariableID(), action_coef);
						}
					}
				}
			}
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTgraph::getVariables", __LINE__, __FILE__);
	}
	return variables;
}

template<> inline FMTgraphstats FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::erasePeriod(
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
					const FMTedgeproperties& edgeproperty = data[*inedge_iterator];
					int varvalue = edgeproperty.getvariableID();
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
			FMTvertexproperties& vertexproperty = data[vertexm_location];
			const int constvalue = vertexproperty.getConstraintId();
			if (constvalue >= 0)
			{
				--stats.rows;
				--stats.transfer_rows;
				deletedconstraints.push_back(constvalue);
				vertexproperty.setConstraintID(-1);
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
		_exhandler->raiseFromCatch("", "FMTgraph::erasePeriod", __LINE__, __FILE__);
	}
	return stats;
}

template<> inline void FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::setConstraintID(
	const FMTvertex_descriptor& vertex,
	const int& id)
{
	data[vertex].setConstraintID(id);
}

template<> inline bool FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getTransferRow(
	const FMTvertex_descriptor& vertex_descriptor,
	std::vector<int>&row_starts,
	std::vector<int>& cols,
	std::vector<double>& cols_value) const
{
	try {
		FMTinedge_iterator inedge_iterator, inedge_end;
		FMTvertexproperties vertex_property = data[vertex_descriptor];
		row_starts.push_back(static_cast<int>(cols.size()));
		bool gotin = false;
		for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
		{
			const FMTedgeproperties& edgeprop = data[*inedge_iterator];
			cols.push_back(edgeprop.getvariableID());
			cols_value.push_back((edgeprop.getProportion() / 100));
			gotin = true;
		}
		std::vector<int>locals;
		FMToutedge_iterator outedge_iterator, outedge_end;
		for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(vertex_descriptor, data); outedge_iterator != outedge_end; ++outedge_iterator)
		{
			const FMTedgeproperties& edgeprop = data[*outedge_iterator];
			const int edgevar = edgeprop.getvariableID();
			if (std::find(locals.begin(), locals.end(), edgevar) == locals.end())
			{
				cols.push_back(edgevar);
				locals.push_back(edgevar);
				cols_value.push_back(-1);
			}
		}
		if (!gotin)
		{
			const Core::FMTdevelopment& dev = vertex_property.get();
			_exhandler->raise(Exception::FMTexc::FMTrangeerror,
				"No in variables for development "+std::string(dev), "FMTgraph::getTransferRow", __LINE__, __FILE__);
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTgraph::getTransferRow", __LINE__, __FILE__);
	}
	return true;
}

template<> inline double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getInProportion(
	const FMTvertex_descriptor& vertex_descriptor) const
{
	try {
		FMTinedge_iterator inedge_iterator, inedge_end;
		FMTvertexproperties vertex_property = data[vertex_descriptor];
		for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
		{
			const FMTedgeproperties& edgeprop = data[*inedge_iterator];
			return edgeprop.getProportion();
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTgraph::getInProportion", __LINE__, __FILE__);
	}
	return 1;
}

template<> inline void FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::updateMatrixIndex(
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
				FMTvertexproperties& vertexproperty = data[*vertex_iterator];
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
					vertexproperty.setConstraintID(actualconstraint - toRemove);
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
				FMTedgeproperties& edgeproperty = data[*edge_iterator];
				const int actualvariable = edgeproperty.getvariableID();
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
					edgeproperty.setvariableID(actualvariable - toRemove);
				}

			}
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTgraph::updateMatrixIndex", __LINE__, __FILE__);
	}
}

}

namespace boost
	{
	namespace serialization
		{
		template<> struct guid_defined<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>> : boost::mpl::true_
			{

			};
		template<> struct guid_defined<Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>> : boost::mpl::true_
			{

			};
		template<> inline const char * guid<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>>()
			{
			return "Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>";
			}
		template<> inline const char * guid<Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>>()
			{
			return "Graph::FMTgraph<Graph::FMTbasevertexproperties,Graph::FMTbaseedgeproperties>";
			}
		}
	}

#endif // FMTGRAPH_H