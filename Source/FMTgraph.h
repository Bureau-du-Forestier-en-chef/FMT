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

#include "FMTvertexproperties.h"
#include "FMTedgeproperties.h"
#include "FMTgraphstats.h"

#include "FMToutputnode.h"
#include "FMTdevelopment.h"
#include "FMTactualdevelopment.h"
#include "FMTfuturdevelopment.h"
#include "FMTdevelopmentpath.h"
#include "FMTschedule.h"
#include "FMToutputnode.h"
#include "FMTconstraint.h"
#include "FMTevent.h"
#include "FMTcoordinate.h"
#include "FMToutputnodecache.h"



#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/export.hpp>

#include <memory>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <queue>
#include <random>

#include "FMTmodel.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTtransition.h"
#include "FMTtheme.h"
#include <tuple>
#include <assert.h>
#include "FMTexceptionhandler.h"
#include "boost/graph/graphviz.hpp"

//namespace Models
//{class FMTmodel;}

namespace Graph
{

enum class FMTgraphbuild
	{
		schedulebuild = 1,
		fullbuild = 2,
		nobuild = 3
	};

enum FMToutputlevel
	{
	standard = 1,
	totalonly = 2,
	developpement = 3
	};


template <class tvertexproperties,class tedgeproperties>
class FMTgraph
    {
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		ar & BOOST_SERIALIZATION_NVP(data);
		ar & BOOST_SERIALIZATION_NVP(stats);
		ar & BOOST_SERIALIZATION_NVP(buildtype);
	}
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(data);
		generatedevelopments();
		ar & BOOST_SERIALIZATION_NVP(stats);
		ar & BOOST_SERIALIZATION_NVP(buildtype);
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
		typedef boost::adjacency_list< boost::listS,
			boost::listS,
			boost::bidirectionalS,
			tvertexproperties,
			tedgeproperties,
			boost::no_property,
			boost::listS>FMTadjacency_list;
	protected:
		FMTadjacency_list data;
	public:
		typedef typename boost::graph_traits<FMTadjacency_list>::vertex_descriptor FMTvertex_descriptor;
		typedef typename boost::graph_traits<FMTadjacency_list>::edge_descriptor FMTedge_descriptor;
		typedef typename boost::graph_traits<FMTadjacency_list>::in_edge_iterator FMTinedge_iterator;
		typedef typename boost::graph_traits<FMTadjacency_list>::out_edge_iterator FMToutedge_iterator;
		typedef typename boost::graph_traits<FMTadjacency_list>::vertex_iterator FMTvertex_iterator;
		typedef typename boost::graph_traits<FMTadjacency_list>::edge_iterator FMTedge_iterator;
		typedef typename std::pair<FMToutedge_iterator, FMToutedge_iterator> FMToutedge_pair;
		typedef typename std::pair<FMTvertex_iterator, FMTvertex_iterator> FMTvertex_pair;
	protected:
        FMTgraphbuild buildtype;
        std::vector<std::unordered_map<size_t,FMTvertex_descriptor>> developments;
		mutable std::vector<FMToutputnodecache<FMTvertex_descriptor>>nodescache;
        FMTgraphstats stats;
		void updatevarsmap(std::map<int,double>& variables,const int& var,const double& coef) const
		{
			std::map<int, double>::iterator varit = variables.find(var);
			if (varit == variables.end())
			{
				variables[var] = coef;
			}
			else {
				varit->second += coef;
			}
		}
		typename std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::iterator getfirstblock()
		{
			typename std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::iterator periodit = developments.begin();
			if (!developments.empty())
			{
				while (periodit->empty())
				{
					++periodit;
				}
			}
			else {
				Exception::FMTexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
					"", "FMTgraph::getfirstblock", __LINE__, __FILE__);
			}
			return periodit;
		}

		typename std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::const_iterator getfirstconstblock() const
		{
			typename std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::const_iterator periodit = developments.begin();
			if (!developments.empty())
			{
				while (periodit->empty())
				{
					++periodit;
				}
			}
			else {
				Exception::FMTexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
					"Empty graph", "FMTgraph::getfirstconstblock", __LINE__, __FILE__);
			}
			return periodit;
		}
    public:
        FMTgraph() :
			data(),
			buildtype(FMTgraphbuild::nobuild),
			developments(),
			nodescache(),
			stats()
		{

		}
        virtual ~FMTgraph()=default;

		FMTgraph(const FMTgraphbuild lbuildtype) :
			data(),
			buildtype(lbuildtype),
			developments(),
			nodescache(),
			stats()
		{

		}

		FMTgraph(const FMTgraph& rhs) :
			data(rhs.data),
			buildtype(rhs.buildtype),
			developments(),
			nodescache(),
			stats(rhs.stats)
		{
			generatedevelopments();
		}

		void swap(FMTgraph& rhs)
		{
			buildtype = rhs.buildtype;
			nodescache.swap(rhs.nodescache);
			stats = rhs.stats;
			data.swap(rhs.data);
			generatedevelopments();
		}


		FMTgraph& operator = (const FMTgraph& rhs)
		{
			if (this != &rhs)
			{
				buildtype = rhs.buildtype;
				nodescache = rhs.nodescache;
				stats = rhs.stats;
				data = rhs.data;
				generatedevelopments();
			}
			return *this;

		}
		bool operator == (const FMTgraph& rhs) const
		{
			if (buildtype == rhs.buildtype &&
				stats == rhs.stats)
			{
				typename std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::const_iterator devsit = this->getfirstconstblock();
				size_t location = 0;
				while (devsit != developments.end())
				{
					for (typename std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator it = developments.at(location).begin(); it != developments.at(location).end(); it++)
					{
						if (rhs.developments.at(location).find(it->first) == rhs.developments.at(location).end())
						{
							return false;
						}
						else {
							const FMTvertex_descriptor vertex_location = it->second;
							const FMTvertex_descriptor rhsvertex_location = rhs.developments.at(location).find(it->first)->second;
							FMTinedge_iterator inedge_iterator, inedge_end;
							std::vector<FMTbaseedgeproperties>edges;
							std::vector<FMTbaseedgeproperties>rhsedges;
							for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_location, data); inedge_iterator != inedge_end; ++inedge_iterator)
							{
								edges.push_back(data[*inedge_iterator]);
							}
							for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(rhsvertex_location, rhs.data); inedge_iterator != inedge_end; ++inedge_iterator)
							{
								rhsedges.push_back(rhs.data[*inedge_iterator]);
							}
							std::sort(edges.begin(), edges.end());
							std::sort(rhsedges.begin(), rhsedges.end());
							if (edges != rhsedges)
							{
								return false;
							}
						}
					}
					++location;
					++devsit;
				}
				return true;
			}
			return false;
		}
        bool operator != (const FMTgraph& rhs) const
			{
			return (!(*this == rhs));
			}
		FMTgraphbuild getbuildtype() const
			{
			return buildtype;
			}
		void setbuildtype(const FMTgraphbuild& build)
			{
			buildtype = build;
			}
		bool containsdevelopment(const Core::FMTdevelopment& developement) const
			{
			return ((static_cast<int>(developments.size()) > (developement.period)) &&
				developments.at((developement.period)).find(boost::hash<Core::FMTdevelopment>()(developement)) != developments.at((developement.period)).end());
			}
		std::queue<FMTvertex_descriptor> initialize(const std::vector<Core::FMTactualdevelopment>& actdevelopments)
			{
				std::queue<FMTvertex_descriptor>actives;
				developments.clear();
				developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
				developments.back().reserve(actdevelopments.size());
				developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
				developments.back().reserve(actdevelopments.size());
				const int constraint_id = -1;
				int edge_id = -1;
				const double proportion = 100;
				stats.edges = 0;
				stats.vertices = 0;
				for (const Core::FMTactualdevelopment& development : actdevelopments)
				{
					//P0
					const FMTvertexproperties properties(development, constraint_id);
					FMTvertex_descriptor newvertex = boost::add_vertex(properties, data);
					developments[0][boost::hash<Core::FMTdevelopment>()(development)] = newvertex;
					++stats.vertices;
					//P1
					Core::FMTfuturdevelopment P1dev(development);
					P1dev.period = 1;
					FMTvertex_descriptor tovertex;
					tovertex = adddevelopment(P1dev);
					actives.push(tovertex);
					//Now set the edge!!
					const FMTedgeproperties newedge(edge_id, stats.edges, proportion);
					boost::add_edge(newvertex, tovertex, newedge, data);
					++stats.edges;
				}

				return actives;

			}
		FMTgraphstats build(const Models::FMTmodel& model,
			std::queue<FMTvertex_descriptor> actives)
		{
			FMTgraphstats statsdiff(stats);
			developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
			while (!actives.empty())
			{
				const FMTvertex_descriptor front_vertex = actives.front();
				actives.pop();
				FMTvertexproperties front_properties = data[front_vertex];
				const Core::FMTdevelopment active_development = front_properties.get();
				int action_id = 0;
				bool death = false;
				for (const Core::FMTaction& action : model.actions)
				{
					if (active_development.operable(action, model.yields))
					{
						if (action.getname() == "_DEATH")
						{
							death = true;
						}
						const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
						addaction(action_id, statsdiff, actives, front_vertex, paths);
					}
					++action_id;
				}
				if (!death)
				{
					const Core::FMTfuturdevelopment grown_up = active_development.grow();
					FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
					const FMTedgeproperties newedge(-1, statsdiff.cols, 100);
					++statsdiff.cols;
					boost::add_edge(front_vertex, next_period, newedge, data);
					++stats.edges;
				}

			}

			return (statsdiff - stats);

		}
		FMTgraphstats naturalgrowth(std::queue<FMTvertex_descriptor> actives)
		{
			FMTgraphstats statsdiff(stats);
			developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
			while (!actives.empty())
			{
				const FMTvertex_descriptor front_vertex = actives.front();
				actives.pop();
				FMTvertexproperties front_properties = data[front_vertex];
				const Core::FMTdevelopment active_development = front_properties.get();
				const Core::FMTfuturdevelopment grown_up = active_development.grow();
				FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
				const FMTedgeproperties newedge(-1, 0, 100);
				boost::add_edge(front_vertex, next_period, newedge, data);
				++stats.edges;
			}

			return (statsdiff - stats);

		}
		std::vector<Core::FMTactualdevelopment> getperiodstopdev(const int location, const double* actual_solution) const
		{
			std::vector<Core::FMTactualdevelopment>all_period_stop_devs;
			for (typename std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator devit = developments.at(location).begin();
				devit != developments.at(location).end(); devit++)
			{
				if (periodstop(devit->second))
				{
					const double area = inarea(devit->second, actual_solution);
					const Core::FMTdevelopment& development = data[devit->second].get();
					all_period_stop_devs.push_back(Core::FMTactualdevelopment(development, area));
				}
			}
			all_period_stop_devs.size();
			return all_period_stop_devs;

		}
		std::map<std::string, double> getoutput(const Models::FMTmodel& model, const Core::FMToutput& output,
			int period, const double* solution, FMToutputlevel level = FMToutputlevel::standard) const
		{
			Core::FMTtheme targettheme;
			std::vector<std::string>target_attributes;
			std::map<std::string, double>results;
			try {
				if (level != FMToutputlevel::developpement)
				{
					if (level == FMToutputlevel::standard)
					{
						target_attributes = output.getdecomposition(model.themes);
						if (!target_attributes.empty() && level == FMToutputlevel::standard)
						{
							targettheme = output.targettheme(model.themes);
						}
					}
					target_attributes.push_back("Total");
					for (const std::string& attribute : target_attributes)
					{
						results[attribute] = 0;
					}
				}
				if (!output.islevel())
				{
					for (const Core::FMToutputnode& output_node : output.getnodes(model.area, model.actions, model.yields))
					{
						const std::map<std::string, double> srcvalues = getsource(model, output_node, period, targettheme, solution, level);
						if (level == FMToutputlevel::developpement)
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
				}

			}
			catch (...)
			{
				Exception::FMTexceptionhandler().raisefromcatch("", "FMTgraph::getoutput", __LINE__, __FILE__);
			}

			return results;

		}
		FMTvertex_descriptor getdevelopment(const Core::FMTdevelopment& developement) const
		{
			return developments.at(developement.period).at(boost::hash<Core::FMTdevelopment>()(developement));
		}
		const Core::FMTdevelopment& getdevelopment(const FMTvertex_descriptor& descriptor) const
		{
				return data[descriptor].get();

		}
		FMTvertex_descriptor adddevelopment(const Core::FMTfuturdevelopment& futurdevelopement)
		{
			if (!this->containsdevelopment(futurdevelopement))
			{
				const int constraint_id = -1;
				const FMTvertexproperties properties(futurdevelopement, constraint_id);
				FMTvertex_descriptor newvertex = boost::add_vertex(properties, data);
				developments[futurdevelopement.period][boost::hash<Core::FMTdevelopment>()(futurdevelopement)] = newvertex;
				++stats.vertices;
				return newvertex;
			}
			return getdevelopment(futurdevelopement);

		}
		size_t hash(size_t seed = 0) const
		{
			boost::hash_combine(seed, boost::hash<Core::FMTdevelopment>()(data[developments.at(0).begin()->second].get()));
			FMTedge_iterator edge_iterator, edge_iterator_end;
			for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
			{
				const FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
				boost::hash_combine(seed, edgeprop.getactionID());
			}
			return seed;

		}
		void addaction(const int& actionID,
                        FMTgraphstats& statsdiff,
                        std::queue<FMTvertex_descriptor>& actives,
                        const FMTvertex_descriptor& out_vertex,
                        const std::vector<Core::FMTdevelopmentpath>& paths)
		{
			int variable_id = statsdiff.cols;
			++statsdiff.cols;
			std::vector<FMTvertex_descriptor>active_vertex;
			for (const Core::FMTdevelopmentpath& devpath : paths)
			{

				const FMTedgeproperties newedge(actionID, variable_id, devpath.proportion);

				FMTvertex_descriptor tovertex;
				if (!this->containsdevelopment(*devpath.development))
				{
					tovertex = this->adddevelopment(*devpath.development);
					actives.push(tovertex);

				}
				else {
					tovertex = this->adddevelopment(*devpath.development);
				}
				boost::add_edge(out_vertex, tovertex, newedge, data);
				++stats.edges;
			}
		}
		double outarea(const FMTvertex_descriptor& out_vertex,
			const int& actionID, const double*&  solution) const
		{
			{
				FMToutedge_iterator outedge_iterator, outedge_end;
				double value = 0;
				for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*outedge_iterator];
					if (edgeprop.getactionID() == actionID)
					{
						value += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
					}
				}
				return value;
			}

		}
		double inarea(const FMTvertex_descriptor& out_vertex,
                        const double*&  solution, bool growth = false) const
		{
			FMTinedge_iterator inedge_iterator, inedge_end;
			double area = 0;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
				if (edgeprop.getactionID() < 0 || !growth)
				{
					area += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
				}
			}
			return area;
		}
		bool periodstart(const FMTvertex_descriptor& out_vertex) const
		{
			//const FMTvertexproperties& properties = data[out_vertex];
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
				if (edgeprop.getactionID() == -1)
				{
					return true;
				}
			}
			return false;
		}
		bool periodstop(const FMTvertex_descriptor& out_vertex) const
		{
			FMToutedge_iterator outedge_iterator, outedge_end;
			const FMTbasevertexproperties& source_properties = data[out_vertex];
			if ((source_properties.get().period) == 0)
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
			return false;
		}
		std::vector<Core::FMTdevelopmentpath> getpaths(const FMTvertex_descriptor& out_vertex,
                                            const int& actionID) const
		{
			std::vector<Core::FMTdevelopmentpath>paths;
			for (FMToutedge_pair edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
			{
				const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
				if (edgeprop.getactionID() == actionID)
				{
					const FMTbasevertexproperties& vertex_target = data[target(*edge_pair.first, data)];
					paths.push_back(Core::FMTdevelopmentpath(vertex_target.get(), edgeprop.getproportion()));
				}
			}
			return paths;
		}
		bool isvalidouputnode(const Models::FMTmodel& model, const Core::FMToutputnode& node, std::vector<const Core::FMTaction*>& action_IDS, int period) const
		{
			if (static_cast<int>(developments.size()) > period)
			{
				action_IDS = node.source.targets(model.actions);
				if (!(period == 0 && !action_IDS.empty()) && !node.source.isnull(model.yields) && !node.factor.isnull(model.yields) && ((!action_IDS.empty() && !node.source.getaction().empty()) || node.source.getaction().empty()))
				{
					return true;
				}
			}
			return false;
		}
		bool isvalidgraphnode(const Models::FMTmodel& model, const FMTvertex_descriptor& vertex_descriptor,
                            const Core::FMToutputnode& node,const std::vector<const Core::FMTaction*>& selected) const
		{
			const Core::FMTdevelopment& development = data[vertex_descriptor].get();
			if (node.source.use(development, model.yields))
			{
				if (node.source.useinedges())
				{
					if ((development.period == 0 || periodstart(vertex_descriptor)) && ((selected.empty() && (node.source.isnextperiod() || !node.source.emptylock())) ||
						(((buildtype == FMTgraphbuild::schedulebuild) && development.anyoperable(selected, model.yields)) ||
							anyoperables(vertex_descriptor, development.anyworthtestingoperability(selected, *model.actions.begin())))))
					{
						return true;
					}
				}
				else if ((anyoperables(vertex_descriptor, development.anyworthtestingoperability(selected, *model.actions.begin())))) //out edges
				{
					return true;
				}
			}
			return false;
		}
		std::map<int,double> locatenode(const Models::FMTmodel& model,Core::FMToutputnode output_node, int period) const
		{
			const std::vector<FMTvertex_descriptor>verticies = getnode(model, output_node, period);
			return getvariables(model, output_node, verticies);

		}
		std::vector<FMTvertex_descriptor> getnode(const Models::FMTmodel& model, Core::FMToutputnode output_node, int period) const
		{
			std::vector<FMTvertex_descriptor>locations;
			std::vector<int>targetedperiods;
			const int maxperiod = static_cast<int>(developments.size() - 2);
			const int node_period = output_node.settograph(targetedperiods, period, maxperiod);
			if (node_period < 0)
			{
				return locations;
			}
			//size_t lookedat_size = 0;
			if (!output_node.source.isvariablelevel())
			{
				std::vector<const Core::FMTaction*> selected;
				if (isvalidouputnode(model, output_node, selected, node_period))
				{
					for (const int& localnodeperiod : targetedperiods)
					{
						while (nodescache.size() != developments.size())
						{
							const size_t devindex = nodescache.size();
							nodescache.emplace_back(developments.at(devindex));
						}
						bool exactverticies = false;
						const std::vector<FMTvertex_descriptor>& descriptors = nodescache.at(localnodeperiod).getverticies(output_node, model.actions, model.themes, exactverticies);
						if (exactverticies)
						{
							locations.reserve(locations.size() + descriptors.size());
							locations.insert(locations.end(), descriptors.begin(), descriptors.end());
						}
						else {
							std::vector<FMTvertex_descriptor>revisited;
							for (const FMTvertex_descriptor& potential : descriptors)
							{
								if (isvalidgraphnode(model, potential, output_node, selected))
								{
									revisited.push_back(potential);
									locations.push_back(potential);
								}
							}
							std::sort(revisited.begin(), revisited.end());
							nodescache.at(localnodeperiod).setvalidverticies(output_node, revisited);
						}
					}


				}

			}
			return locations;
		}
		std::map<int, double> getvariables(const Models::FMTmodel& model, const Core::FMToutputnode& output_node,const std::vector<FMTvertex_descriptor>& verticies) const
		{
			std::map<int, double>variables;
			if (!verticies.empty())
			{
				std::vector<Core::FMTdevelopmentpath>paths;
				Core::FMTaction optimization_action;
				const std::vector<const Core::FMTaction*> selected = output_node.source.targets(model.actions);
				for (const FMTvertex_descriptor& vertex : verticies)
				{
					const Core::FMTdevelopment& development = data[vertex].get();
					if (output_node.source.useinedges())
					{
						const double coef = output_node.source.getcoef(development, model.yields, optimization_action, paths) * output_node.factor.getcoef(development, model.yields, optimization_action, paths) * output_node.constant;
						if (development.period == 0)
						{
							const std::map<int, int>vars = getoutvariables(vertex);
							updatevarsmap(variables, vars.at(-1), coef);
						}
						else {
							FMTinedge_iterator inedge_iterator, inedge_end;
							for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
							{
								const FMTedgeproperties& edgeprop = data[*inedge_iterator];
								const int actionid = edgeprop.getactionID();
								if (actionid < 0)
								{
									updatevarsmap(variables, edgeprop.getvariableID(), (edgeprop.getproportion() / 100)*coef);
									continue;
								}
								const FMTvertex_descriptor sourceverex = boost::source(*inedge_iterator, data);
								const FMTvertexproperties& sourceproperties = data[sourceverex];
								if ((sourceproperties.get().period == development.period && !periodstart(sourceverex)))
								{
									updatevarsmap(variables, edgeprop.getvariableID(), (edgeprop.getproportion() / 100)*coef);
								}
							}
						}

					}
					else {
						const std::map<int, int>outvars = getoutvariables(vertex);
						for (const Core::FMTaction* act : selected)
						{
							const int actionID = static_cast<int>(std::distance(&(*model.actions.begin()), act));
							if (outvars.find(actionID) != outvars.end())
							{
								paths = getpaths(vertex, actionID);
								const double action_coef = output_node.source.getcoef(development, model.yields, *act, paths) * output_node.factor.getcoef(development, model.yields, *act, paths) * output_node.constant;
								updatevarsmap(variables, outvars.at(actionID), action_coef);
							}
						}
					}
				}
			}
			return variables;
		}
        bool anyoperables(const FMTvertex_descriptor& descriptor, const std::vector<int>& action_ids) const
		{
			if (!action_ids.empty())
			{
				std::map<int, int> variables = getoutvariables(descriptor);
				for (const int& id : action_ids)
				{
					if (variables.find(id) != variables.end())
					{
						return true;
					}
				}
			}
			return false;
		}
		std::map<int, int> getoutvariables(const FMTvertex_descriptor& out_vertex) const
		{
			std::map<int, int> mapping;
			FMToutedge_pair edge_pair;
			for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
			{
				const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
				int actionid = edgeprop.getactionID();
				mapping[actionid] = edgeprop.getvariableID();
			}
			return mapping;
		}
		std::vector<int>getoutactions(const FMTvertex_descriptor& out_vertex) const
		{
			std::vector<int>actions;
			const size_t outsize = boost::out_degree(out_vertex, data);
			if (outsize > 1)
			{
				actions.reserve(outsize - 1);
				FMToutedge_pair edge_pair;
				for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
				{
					const FMTedgeproperties& edgeprop = data[*edge_pair.first];
					const int* actionid = edgeprop.getactionptr();
					if ((*actionid) >= 0)
					{
						actions.emplace_back(*actionid);
					}
				}
				std::sort(actions.begin(), actions.end());
			}
			return actions;
		}
		std::vector<const Core::FMTaction*> selectedactions(const Models::FMTmodel& model, const std::vector<int>& action_IDS) const
		{
			std::vector<const Core::FMTaction*>selected;
			if (!action_IDS.empty())
			{
				selected.resize(action_IDS.size());
				int aid = 0;
				for (const int & actid : action_IDS)
				{
					selected[aid] = &model.actions[actid];
					++aid;
				}
			}
			return selected;
		}
		bool constraintlenght(const Core::FMTconstraint& constraint, int& start, int& stop) const
		{
			start = std::max(constraint.getperiodlowerbound(), getfirstactiveperiod() + 1);
			stop = static_cast<int>((constraint.getperiodupperbound() > static_cast<int>((developments.size() - 2))) ? (developments.size() - 2) : constraint.getperiodupperbound());
			if (constraint.acrossperiod())
			{
				--stop;
			}
			return (start < static_cast<int>(developments.size()) && start <= stop);
		}
		FMTgraphstats getstats() const
		{
			return stats;
		}
		FMTgraphstats* getstatsptr()
		{
			return &stats;
		}
		void setstats(const FMTgraphstats& newstats)
		{
			stats = newstats;
		}
		FMTgraphstats buildschedule(const Models::FMTmodel& model, std::queue<FMTvertex_descriptor> actives,
			const Core::FMTschedule& schedule)
		{
			FMTgraphstats statsdiff(stats);
			developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
			developments.back().reserve(actives.size());
			while (!actives.empty())
			{
				const FMTvertex_descriptor front_vertex = actives.front();
				actives.pop();
				FMTvertexproperties front_properties = data[front_vertex];
				const Core::FMTdevelopment active_development = front_properties.get();
				bool death = false;
				int action_id = 0;
				for (const Core::FMTaction& action : model.actions)
				{
					if (schedule.find(action) != schedule.end() &&
						(((schedule.at(action)).find(active_development) != (schedule.at(action)).end()) ||
						(!action.dorespectlock() && active_development.lock != 0 &&
							(schedule.at(action)).find(active_development.clearlock()) != (schedule.at(action)).end())))
					{
						if (action.getname() == "_DEATH")
						{
							death = true;
						}
						const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
						addaction(action_id, statsdiff, actives,
							front_vertex, paths);
					}
					++action_id;
				}
				if (!death)
				{
					const Core::FMTfuturdevelopment grown_up = active_development.grow();
					FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
					const int variable_id = statsdiff.cols;
					++statsdiff.cols;
					const int id = -1;
					const double proportion = 100;
					const FMTedgeproperties newedge(id, variable_id, proportion);
					boost::add_edge(front_vertex, next_period, newedge, data);
					++stats.edges;
				}

			}
			return (statsdiff - stats);
		}


		FMTgraphstats eraseperiod(std::vector<int>& deletedconstraints,
			std::vector<int>&deletedvariables,
			bool keepbounded = false)
		{
			typename std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::iterator periodit = this->getfirstblock();
			for (typename std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = periodit->begin();
				it != periodit->end(); it++)
			{
				FMTvertex_descriptor& vertex_location = it->second;
				FMTinedge_iterator inedge_iterator, inedge_end;
				bool gotinedges = false;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(it->second, data); inedge_iterator != inedge_end; ++inedge_iterator)
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
				if (!keepbounded)
				{
					const std::map<int, int>outvars = this->getoutvariables(vertex_location);
					for (std::map<int, int>::const_iterator varit = outvars.begin(); varit != outvars.end(); varit++)
					{
						if (std::find(deletedvariables.begin(), deletedvariables.end(), varit->second) == deletedvariables.end())
						{
							--stats.cols;
							deletedvariables.push_back(varit->second);
						}
						--stats.edges;
					}
					boost::clear_out_edges(vertex_location, data);
				}
				boost::clear_in_edges(vertex_location, data);
			}
			std::unordered_map<size_t, FMTvertex_descriptor>restingdevelopments;
			for (typename std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = periodit->begin();
				it != periodit->end(); it++)
			{
				const FMTvertex_descriptor& vertex_location = it->second;
				FMTvertexproperties& vertexproperty = data[vertex_location];
				const int constvalue = vertexproperty.getconstraintID();
				if (constvalue >= 0)
				{
					--stats.rows;
					--stats.transfer_rows;
					deletedconstraints.push_back(constvalue);
					vertexproperty.setconstraintID(-1);
				}
				if (!keepbounded || out_degree(vertex_location, data) == 0)
				{
					boost::remove_vertex(vertex_location, data);
					--stats.vertices;
				}
				else {
					restingdevelopments[boost::hash<Core::FMTdevelopment>()(vertexproperty.get())] = it->second;
				}
			}
			*periodit = restingdevelopments;
			const int firstperiod = this->getfirstactiveperiod();
			if (static_cast<size_t>(firstperiod) < nodescache.size())
			{
				nodescache[firstperiod].clear();
			}
			return stats;
		}
		bool empty() const
		{
			return developments.empty();
		}
		std::queue<FMTvertex_descriptor> getactiveverticies() const
		{
			std::queue<FMTvertex_descriptor>actives;
			for (auto& lastperiod : developments.back())
			{
				actives.push(lastperiod.second);
			}
			return actives;
		}
		const std::unordered_map<size_t, FMTvertex_descriptor>& getperiodverticies(int period) const
		{
			return developments.at(period);
		}
		size_t size() const
		{
			return developments.size();
		}
		void setconstraintID(const FMTvertex_descriptor& vertex, const int& id)
		{
			data[vertex].setconstraintID(id);
		}
		bool gettransferrow(const FMTvertex_descriptor& vertex_descriptor,
			std::vector<int>&row_starts,
			std::vector<int>& cols,
			std::vector<double>& cols_value) const
		{
			FMTinedge_iterator inedge_iterator, inedge_end;
			FMTvertexproperties vertex_property = data[vertex_descriptor];
			row_starts.push_back(static_cast<int>(cols.size()));
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTedgeproperties& edgeprop = data[*inedge_iterator];
				cols.push_back(edgeprop.getvariableID());
				cols_value.push_back((edgeprop.getproportion() / 100));
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
			return true;

		}
		void getinitialbounds(std::vector<double>& lower_bounds, std::vector<double>& upper_bounds) const
		{
			try {
				for (const auto& descriptors : developments.at(0))
				{
					const FMTvertex_descriptor descriptor = descriptors.second;
					const FMTvertexproperties& property = data[descriptor];
					const std::map<int, int>outs = getoutvariables(descriptor);
					lower_bounds[outs.at(-1)] = property.getbaseRHS();
					upper_bounds[outs.at(-1)] = property.getbaseRHS();
				}
			}
			catch (...)
			{
				Exception::FMTexceptionhandler().raisefromcatch("", "FMTgraph::getinitialbounds", __LINE__, __FILE__);
			}
		}
		size_t nedges() const
		{
			return boost::num_edges(data);
		}
		std::map<std::string, double> getsource(const Models::FMTmodel& model,
			const Core::FMToutputnode& node,
			int period, const Core::FMTtheme& theme,
			const double* solution, FMToutputlevel level = FMToutputlevel::standard) const
		{
			std::vector<FMTvertex_descriptor>verticies = getnode(model, node, period);
			return getvalues(model, verticies, node, theme, solution, level);
		}
		std::map<std::string, double> getvalues(const Models::FMTmodel& model, const std::vector<FMTvertex_descriptor>& verticies,
			const Core::FMToutputnode& node, const Core::FMTtheme& theme,
			const double* solution, FMToutputlevel level) const
		{
			std::map<std::string, double>values;
			try {
				if (level == FMToutputlevel::standard)
				{
					for (auto attribute_id : theme.getvaluenames())
					{
						values[attribute_id.first] = 0;
					}
				}
				else if (level == FMToutputlevel::totalonly)
				{
					values["Total"] = 0;
				}
				if (!verticies.empty())
				{
					const std::vector<const Core::FMTaction*> selected = node.source.targets(model.actions);
					std::vector<Core::FMTdevelopmentpath>paths;
					Core::FMTaction optimization_action;
					for (const FMTvertex_descriptor& vertex : verticies)
					{
						const Core::FMTdevelopment& development = data[vertex].get();
						std::string value;
						if (level == FMToutputlevel::standard)
						{
							value = development.mask.get(theme);
						}
						else if (level == FMToutputlevel::developpement)
						{
							value = std::string(development);
							values[value] = 0;
						}
						else {
							value = "Total";
						}

						if (node.source.useinedges())
						{
							const double coef = node.source.getcoef(development, model.yields, optimization_action, paths) * node.factor.getcoef(development, model.yields, optimization_action, paths) * node.constant;
							double area = 0;
							if (development.period == 0)
							{
								area = outarea(vertex, -1, solution);
							}
							else {
								area = inarea(vertex, solution, true);
							}
							values[value] += coef * area;
						}
						else {
							for (const Core::FMTaction* act : selected)
							{
								double action_value = 0;
								const int actionID = static_cast<int>(std::distance(&(*model.actions.begin()), act));
								paths = getpaths(vertex, actionID);
								const double action_coef = node.source.getcoef(development, model.yields, *act, paths) * node.factor.getcoef(development, model.yields, *act, paths) * node.constant;
								action_value = action_coef * (outarea(vertex, actionID, solution));
								values[value] += action_value;
							}
						}

					}
				}
				if (level == FMToutputlevel::standard)
				{
					double total = 0;
					for (auto valit : values)
					{
						total += valit.second;
					}
					values["Total"] = total;
				}
			}
			catch (...)
			{
				Exception::FMTexceptionhandler().raisefromcatch("", "FMTgraph::getvalues", __LINE__, __FILE__);
			}
			return values;
		}
		int getperiod() const
		{
			try {
				FMTvertex_iterator vertex, vend;
				boost::tie(vertex, vend) = vertices(data);
				--vend;
				return (data[*vend].get().period);
			}catch (...)
				{
				Exception::FMTexceptionhandler().raisefromcatch("", "FMTgraph::getperiod", __LINE__, __FILE__);
				}
			return 0;
		}


        void generatedevelopments()
		{
			try {
				developments.clear();
				const size_t max_period = static_cast<size_t>(getperiod());
				FMTvertex_iterator vertex, vend;
				developments.resize(max_period + 1);
				for (boost::tie(vertex, vend) = vertices(data); vertex != vend; ++vertex)
				{
					const FMTbasevertexproperties& properties = data[*vertex];
					const Core::FMTdevelopment& dev = properties.get();
					const size_t period_location = (dev.period);
					developments[period_location][boost::hash<Core::FMTdevelopment>()(dev)] = *vertex;
				}
			}
			catch (...)
			{
				Exception::FMTexceptionhandler().raisefromcatch("", "FMTgraph::generatedevelopments", __LINE__, __FILE__);
			}
		}
		bool sameedgesas(const FMTgraph& rhs) const
		{
			FMTedge_iterator thisedge_iterator, thisedge_iterator_end;
			FMTedge_iterator rhsedge_iterator, rhsedge_iterator_end;
			bool different = false;
			boost::tie(thisedge_iterator, thisedge_iterator_end) = boost::edges(data);
			boost::tie(rhsedge_iterator, rhsedge_iterator_end) = boost::edges(rhs.data);
			while (!different && thisedge_iterator != thisedge_iterator_end && rhsedge_iterator != rhsedge_iterator_end)
			{
				const FMTedgeproperties& thisedgeprop = data[*thisedge_iterator];
				const FMTedgeproperties& rhsedgeprop = rhs.data[*rhsedge_iterator];
				if (thisedgeprop.getactionID() != rhsedgeprop.getactionID())
				{
					different = true;
				}
				++thisedge_iterator;
				++rhsedge_iterator;
			}
			return different;
		}
		void updatematrixindex(const std::vector<int>& removedvariables,
			const std::vector<int>& removedconstraints)
		{
			//Everything is sorted
			if (!removedconstraints.empty())
			{
				const int& maxconstraint = removedconstraints.back();
				const int& minconstraint = removedconstraints.front();
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					FMTvertexproperties& vertexproperty = data[*vertex_iterator];
					const int actualconstraint = vertexproperty.getconstraintID();
					if (actualconstraint >= 0)
					{
						int toremove = 0;
						if (actualconstraint > minconstraint && actualconstraint < maxconstraint)
						{
							std::vector<int>::const_iterator removeditconstraint = removedconstraints.begin();
							while (removeditconstraint != removedconstraints.end() && actualconstraint > *removeditconstraint)
							{
								++toremove;
								++removeditconstraint;
							}
						}
						else if (actualconstraint > maxconstraint)
						{
							toremove = static_cast<int>(removedconstraints.size());
						}
						vertexproperty.setconstraintID(actualconstraint - toremove);
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
						int toremove = 0;
						if (actualvariable > minvariable && actualvariable < maxvariable)
						{
							std::vector<int>::const_iterator removeditvariable = removedvariables.begin();
							while (removeditvariable != removedvariables.end() && actualvariable > *removeditvariable)
							{
								++toremove;
								++removeditvariable;
							}
						}
						else if (actualvariable > maxvariable)
						{
							toremove = static_cast<int>(removedvariables.size());
						}
						edgeproperty.setvariableID(actualvariable - toremove);
					}

				}
			}

		}
		int getfirstactiveperiod() const
		{
			return static_cast<int>(std::distance(developments.begin(), getfirstconstblock()));
		}
		FMTgraph postsolve(const Core::FMTmask& selectedmask,
			const std::vector<Core::FMTtheme>&originalbasethemes,
			const std::map<int,int>& actionmapconnection) const
		{
			FMTgraph newgraph(*this);
			newgraph.developments.clear();
			newgraph.nodescache.clear();//Some postsolve can be done here to keep some usefull information but for now just clear
			//start by remapping the actions
			FMTedge_iterator edge_iterator, edge_iterator_end;
			for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
			{
				FMTedgeproperties& edgeprop = newgraph.data[*edge_iterator];
				edgeprop.setactionID(actionmapconnection.at(edgeprop.getactionID()));
			}
			FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
				FMTvertexproperties& vertexprop = newgraph.data[*vertex_iterator];
				vertexprop.setdevlopementmask(vertexprop.get().mask.postsolve(selectedmask, originalbasethemes));
			}
			newgraph.generatedevelopments();
			return newgraph;
		}
		Core::FMTschedule getschedule(const std::vector<Core::FMTaction>& actions, const double* actual_solution, const int& lperiod) const
		{
			try {
				if (static_cast<int>(size()) > lperiod && lperiod > 0)
				{
					std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::map<int, double>>>schedule_solution;
					//const double* actual_solution = this->getColSolution();
					for (const auto deviterator : getperiodverticies(lperiod))
					{
						const FMTvertex_descriptor vertex = deviterator.second;
						std::map<int, int>variables = getoutvariables(vertex);
						variables.erase(-1);
						if (!variables.empty())
						{
							for (const auto variable_iterator : variables)
							{
								if (*(actual_solution + variable_iterator.second) > 0) //basis solution only!!!
								{
									if (schedule_solution.find(actions[variable_iterator.first]) == schedule_solution.end())
									{
										schedule_solution[actions[variable_iterator.first]] = std::map<Core::FMTdevelopment, std::map<int, double>>();
									}
									const Core::FMTdevelopment& basedev = getdevelopment(deviterator.second);
									Core::FMTdevelopment lockout = basedev.clearlock();
									if (schedule_solution[actions[variable_iterator.first]].find(lockout) == schedule_solution[actions[variable_iterator.first]].end())
									{
										schedule_solution[actions[variable_iterator.first]][lockout] = std::map<int, double>();
									}
									schedule_solution[actions[variable_iterator.first]][lockout][basedev.lock] = (*(actual_solution + variable_iterator.second));
								}
							}

						}
					}
					Core::FMTschedule newschedule(lperiod, schedule_solution);
					return newschedule;
				}
			}
			catch (...)
			{
				Exception::FMTexceptionhandler handler;
				handler.raisefromcatch("at period " + std::to_string(lperiod), "FMTgraph::getschedule", __LINE__, __FILE__);
			}
			return Core::FMTschedule();
		}
		operator std::string() const
		{
			std::ostringstream stream;
			//write_graphviz(stream, data);
			return stream.str();
		}

    };

template<> inline std::map<int, int> FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getoutvariables(const FMTvertex_descriptor& out_vertex) const
	{
		std::map<int, int> mapping;
		FMToutedge_pair edge_pair;
		for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
		{
			const FMTedgeproperties& edgeprop = data[*edge_pair.first];
			int actionid = edgeprop.getactionID();
			mapping[actionid] = edgeprop.getvariableID();
		}
		return mapping;
	}


template<> inline std::vector<Core::FMTdevelopmentpath> FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getpaths(const FMTvertex_descriptor& out_vertex, const int& actionID) const
	{
		std::vector<Core::FMTdevelopmentpath>paths;
		for (FMToutedge_pair edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
		{
			const FMTedgeproperties& edgeprop = data[*edge_pair.first];
			if (edgeprop.getactionID() == actionID)
			{
				const FMTbasevertexproperties& vertex_target = data[target(*edge_pair.first, data)];
				paths.push_back(Core::FMTdevelopmentpath(vertex_target.get(), edgeprop.getproportion()));
			}
		}
		return paths;
	}



template<> inline double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::inarea(const FMTvertex_descriptor& out_vertex, const double*&  solution, bool growth) const
	{
		FMTinedge_iterator inedge_iterator, inedge_end;
		double area = 0;
		for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
		{
			const FMTedgeproperties& edgeprop = data[*inedge_iterator];
			if (edgeprop.getactionID() < 0 || !growth)
			{
				area += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
			}
		}
		return area;
	}

template<> inline double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::outarea(const FMTvertex_descriptor& out_vertex, const int& actionID, const double*&  solution) const
	{
		FMToutedge_iterator outedge_iterator, outedge_end;
		double value = 0;
		for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
		{
			const FMTedgeproperties& edgeprop = data[*outedge_iterator];
			if (edgeprop.getactionID() == actionID)
			{
				value += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
			}
		}
		return value;

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

