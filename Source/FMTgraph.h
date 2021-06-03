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
#include <boost\unordered_set.hpp>
#include "FMTcarbonpredictor.h"



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
#include "FMTyields.h"
#include "FMTtransition.h"
#include "FMTtheme.h"
#include "FMTobject.h"
#include <tuple>
#include <assert.h>
#include "FMTexceptionhandler.h"
#include "FMTlookup.h"
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
class FMTgraph : public Core::FMTobject
    {
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
		FMTobject::forcesave(ar, version);
		ar & BOOST_SERIALIZATION_NVP(data);
		ar & BOOST_SERIALIZATION_NVP(stats);
		ar & BOOST_SERIALIZATION_NVP(buildtype);
	}
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("FMTobject", boost::serialization::base_object<FMTobject>(*this));
		FMTobject::forcesave(ar, version);
		ar & BOOST_SERIALIZATION_NVP(data);
		generatedevelopments();
		ar & BOOST_SERIALIZATION_NVP(stats);
		ar & BOOST_SERIALIZATION_NVP(buildtype);
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	protected:
		typedef boost::adjacency_list< boost::listS,
			boost::listS,
			boost::bidirectionalS,
			tvertexproperties,
			tedgeproperties,
			boost::no_property,
			boost::listS>FMTadjacency_list;
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
		typedef typename boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>::iterator lookiterator;
		typedef typename boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>::const_iterator lookconst_iterator;
	protected:
		class lookupsorter
		{
			public:
			inline bool operator() (const Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>& lookup1,
				const Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>& lookup2)
			{
				return (*lookup1.pointerobject < *lookup2.pointerobject);
			}
		};
        FMTgraphbuild buildtype;
		std::vector<boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>>developments;
		mutable std::vector<FMToutputnodecache<FMTvertex_descriptor>>nodescache;
		typedef typename std::vector<FMToutputnodecache<FMTvertex_descriptor>>::reverse_iterator reversecachenodeit;
        FMTgraphstats stats;
		void updatevarsmap(std::map<int,double>& variables,const int& var,const double& coef) const
		{
			try {
				std::map<int, double>::iterator varit = variables.find(var);
				if (varit == variables.end())
				{
					variables[var] = coef;
				}
				else {
					varit->second += coef;
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::updatevarsmap", __LINE__, __FILE__);
			}
		}
		typename std::vector<boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>>::iterator getfirstblock()
		{
			typename std::vector<boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>>::iterator periodit = developments.begin();
			try {
				if (!developments.empty())
				{
					while (periodit->empty())
					{
						++periodit;
					}
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"", "FMTgraph::getfirstblock", __LINE__, __FILE__);
				}
			}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getfirstblock", __LINE__, __FILE__);
			}
			return periodit;
		}

		typename std::vector<boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>>::const_iterator getfirstconstblock() const
		{
			typename std::vector<boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>>::const_iterator periodit = developments.begin();
			try {
				if (!developments.empty())
				{
					while (periodit->empty())
					{
						++periodit;
					}
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Empty graph", "FMTgraph::getfirstconstblock", __LINE__, __FILE__);
				}
			}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getfirstconstblock", __LINE__, __FILE__);
			}
			return periodit;
		}
    public:
        FMTgraph() :
			Core::FMTobject(),
			data(),
			buildtype(FMTgraphbuild::nobuild),
			developments(),
			nodescache(),
			stats()
		{

		}
        virtual ~FMTgraph()=default;

		FMTgraph(const FMTgraphbuild lbuildtype) :
			Core::FMTobject(),
			data(),
			buildtype(lbuildtype),
			developments(),
			nodescache(),
			stats()
		{

		}

		FMTgraph(const FMTgraph& rhs) :
			Core::FMTobject(rhs),
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
			std::swap(buildtype,rhs.buildtype);
			nodescache.swap(rhs.nodescache);
			std::swap(stats,rhs.stats);
			data.swap(rhs.data);
			std::swap(developments, rhs.developments);
			//generatedevelopments();
		}

		void passinobject(const Core::FMTobject& rhs) override
			{
			FMTobject::passinobject(rhs);
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
				while(vertex_iterator!= vertex_iterator_end)
					{
					data[*vertex_iterator].passinobject(rhs);
					++vertex_iterator;
					}
			}catch (...)
				{
					_exhandler->raisefromcatch("", "FMTgraph::passinobject", __LINE__, __FILE__);
				}

			}	


		FMTgraph& operator = (const FMTgraph& rhs)
		{
			if (this != &rhs)
			{
				Core::FMTobject::operator=(rhs);
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
			try {
				if (buildtype == rhs.buildtype &&
					stats == rhs.stats)
				{
					typename std::vector<boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>>::const_iterator devsit = this->getfirstconstblock();
					size_t location = 0;
					while (devsit != developments.end())
					{
						for (typename boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>::const_iterator it = developments.at(location).begin(); it != developments.at(location).end(); it++)
						{
							if (rhs.developments.at(location).find(*it) == rhs.developments.at(location).end())
							{
								return false;
							}
							else {
								const FMTvertex_descriptor vertex_location = it->memoryobject;
								const FMTvertex_descriptor rhsvertex_location = rhs.developments.at(location).find(*it)->memoryobject;
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
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::operator==", __LINE__, __FILE__);
			}
			return false;
		}
        bool operator != (const FMTgraph& rhs) const
			{
			return (!(*this == rhs));
			}
        void clearcache()
			{
        		std::vector<FMToutputnodecache<FMTvertex_descriptor>>().swap(nodescache);
			}
        void cleardevelopments()
			{
        		std::vector<boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>>().swap(developments);
			}
		FMTgraphbuild getbuildtype() const
			{
			return buildtype;
			}
		void setbuildtype(const FMTgraphbuild& build)
			{
			buildtype = build;
			}
		bool containsdevelopment(const Core::FMTdevelopment& development) const
			{
			return ((static_cast<int>(developments.size()) > (development.period)) &&
				developments.at((development.period)).find(Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>(development)) != developments.at((development.period)).end());
			}

		std::queue<FMTvertex_descriptor> initialize(const std::vector<Core::FMTactualdevelopment>& actdevelopments)
			{
				std::queue<FMTvertex_descriptor>actives;
				try {
					developments.clear();
					/*developments.push_back(boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>());
					developments.back().reserve(actdevelopments.size());
					developments.push_back(boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>());
					developments.back().reserve(actdevelopments.size());*/
					const int startingperiod = actdevelopments.begin()->period;
					const int actualperiod = startingperiod+1;
					for (int period = 0 ; period <= actualperiod;++period)
						{
						developments.push_back(boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>());
						}
					for (int period = startingperiod; period <= actualperiod; ++period)
						{
						developments[period].reserve(actdevelopments.size());
						}

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
						developments[startingperiod].insert(Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>(newvertex,data[newvertex].get()));
						++stats.vertices;
						//P1
						Core::FMTfuturdevelopment P1dev(development);
						P1dev.period = actualperiod;
						FMTvertex_descriptor tovertex;
						tovertex = adddevelopment(P1dev);
						actives.push(tovertex);
						//Now set the edge!!
						const FMTedgeproperties newedge(edge_id, stats.edges, proportion);
						boost::add_edge(newvertex, tovertex, newedge, data);
						++stats.edges;
					}
					//*_logger << "verticies " << stats.vertices << "\n";
				}
				catch (...)
				{
					_exhandler->raisefromcatch("", "FMTgraph::initialize", __LINE__, __FILE__);
				}
				return actives;

			}
		// DocString: FMTgraph::build(const Models::FMTmodel&,std::queue<FMTvertex_descriptor>)
		/**
		This function build one period at the end of the graph. Like in Woodstock, future developements of a given action
		can not be operate by an action that is before in the section action. The actions in the model must be
		ordered to take that in account.
		*/
		FMTgraphstats build(const Models::FMTmodel& model,
					std::queue<FMTvertex_descriptor> actives)
				{
					FMTgraphstats statsdiff(stats);
					try {
						developments.push_back(boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>());
						//developments.back().max_load_factor(0.5);
						int action_id = 0;
						for (const Core::FMTaction& action : model.actions)
						{
							std::queue<FMTvertex_descriptor> new_actives;
							while (!actives.empty())
							{
								const FMTvertex_descriptor front_vertex = actives.front();
								actives.pop();
								FMTvertexproperties front_properties = data[front_vertex];
								const Core::FMTdevelopment& active_development = front_properties.get();
								bool death = false;
								if (active_development.operable(action, model.yields))
								{
									if (action.getname() == "_DEATH")
									{
										death = true;
									}
									const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
									addaction(action_id, statsdiff, new_actives, front_vertex, paths);
								}
								if (!death)
								{
									new_actives.push(front_vertex);
								}

							}
							actives = new_actives;
							++action_id;
						}

					}
					catch (...)
					{
						_exhandler->raisefromcatch("", "FMTgraph::build", __LINE__, __FILE__);
					}
					return naturalgrowth(actives,statsdiff);
				}

		/*FMTgraphstats build(const Models::FMTmodel& model,
			std::queue<FMTvertex_descriptor> actives)
		{
			FMTgraphstats statsdiff(stats);
			try {
				developments.push_back(boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>());
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
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::build", __LINE__, __FILE__);
			}
			return (statsdiff - stats);

		}*/
		FMTgraphstats naturalgrowth(std::queue<FMTvertex_descriptor> actives,FMTgraphstats statsdiff)
		{
			//FMTgraphstats statsdiff(stats);
			try {
				//developments.push_back(boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>());
				while (!actives.empty())
				{
					const FMTvertex_descriptor front_vertex = actives.front();
					actives.pop();
					FMTvertexproperties front_properties = data[front_vertex];
					const Core::FMTdevelopment active_development = front_properties.get();
					const Core::FMTfuturdevelopment grown_up = active_development.grow();
					FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
					const FMTedgeproperties newedge(-1, statsdiff.cols, 100);
					++statsdiff.cols;
					boost::add_edge(front_vertex, next_period, newedge, data);
					++stats.edges;
					/*const Core::FMTfuturdevelopment grown_up = active_development.grow();
					FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
					const FMTedgeproperties newedge(-1, 0, 100);
					boost::add_edge(front_vertex, next_period, newedge, data);
					++stats.edges;*/
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::naturalgrowth", __LINE__, __FILE__);
			}
			return (statsdiff - stats);

		}
		void getvariablenames(const std::vector<Core::FMTaction>& actions, std::vector<std::string>& colnames) const
		{
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				const std::string toremove = "+-/*";
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end;++vertex_iterator)
					{
					std::string basename=std::string(Core::FMTdevelopment(data[*vertex_iterator].get()));
					boost::remove_erase_if(basename, boost::is_any_of(toremove));
					FMToutedge_iterator outit, outend;
					for (boost::tie(outit, outend) = boost::out_edges(*vertex_iterator,data);outit!=outend;++outit)
						{
						std::string actionname = "EVO";
						const int variableid = data[*outit].getvariableID();
						const int actionid = data[*outit].getactionID();
						if (actionid >=0)
							{
							actionname = actions.at(actionid).getname();
							}
						colnames[variableid] = basename + actionname;
						}

					}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getvariablenames", __LINE__, __FILE__);
			}
		}

		void cleannodecaching(unsigned long long minbytes= 10000000000) const//5 Go max
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
						unsigned long long avmemory = FMTobject::getavailablememory();
						if (avmemory < minbytes)
						{
							unsigned long long rest = (minbytes - avmemory);
							unsigned long long totalclean = 1;
							while (totalclean >= 1 && rest > 0)
							{
								totalclean = 0;
								for (reversecachenodeit cacheit = nodescache.rbegin(); cacheit != nodescache.rend(); ++cacheit)
								{
									const unsigned long long lastclean = cacheit->removelargest();
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
		}

		void gettransferrownames(std::vector<std::string>& rownames) const
		{
			try {
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				const std::string toremove = "+-/*";
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const int rowid = data[*vertex_iterator].getconstraintID();
					if (rowid>=0)
						{
						std::string name = std::string(Core::FMTdevelopment(data[*vertex_iterator].get()));
						boost::remove_erase_if(name, boost::is_any_of(toremove));
						rownames[rowid] = name;
						}
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::gettransferrownames", __LINE__, __FILE__);
			}
		}


		std::vector<Core::FMTactualdevelopment> getperiodstopdev(const int location, const double* actual_solution) const
		{
			std::vector<Core::FMTactualdevelopment>all_period_stop_devs;
			try {
				for (typename boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>::const_iterator devit = developments.at(location).begin();
					devit != developments.at(location).end(); devit++)
				{
					if (periodstop(devit->descriptor))
					{
						const double area = inarea(devit->descriptor, actual_solution);
						all_period_stop_devs.push_back(Core::FMTactualdevelopment(*devit->development, area));
					}
				}
				all_period_stop_devs.size();
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getperiodstopdev", __LINE__, __FILE__);
			}
			return all_period_stop_devs;

		}
		std::map<std::string, double> getoutput(const Models::FMTmodel& model, const Core::FMToutput& output,
			int period, const double* solution, FMToutputlevel level = FMToutputlevel::standard) const
		{
			Core::FMTtheme targettheme;
			std::vector<std::string>target_attributes;
			std::map<std::string, double>results;
			try {
				if(output.targetthemeid() < 0 && !(level == FMToutputlevel::developpement))
				{
					level = FMToutputlevel::totalonly;
				}
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
				
					//const int localperiod = getfirstactiveperiod() + period;//Normal planning first active period is 0, in replanning it wont be 0!.
					/*if (getfirstactiveperiod()>0)
						{
						++period;
						}*/
					for (const Core::FMToutputnode& output_node : output.getnodes(/*model.area, model.actions, model.yields*/))
					{
						const std::map<std::string, double> srcvalues = getsource(model, output_node,period, targettheme, solution, level);
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
				_exhandler->raisefromcatch("For output: "+std::string(output), "FMTgraph::getoutput", __LINE__, __FILE__);
			}

			return results;

		}
		FMTvertex_descriptor getdevelopment(const Core::FMTdevelopment& developement) const
		{
			const Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment> tofind(developement);
			return developments.at(developement.period).find(tofind)->memoryobject;
		}
		const Core::FMTdevelopment& getdevelopment(const FMTvertex_descriptor& descriptor) const
			{
			return data[descriptor].get();
			}
		FMTvertex_descriptor adddevelopment(const Core::FMTfuturdevelopment& futurdevelopement)
		{
			try {
				if (!this->containsdevelopment(futurdevelopement))
				{
					const int constraint_id = -1;
					const FMTvertexproperties properties(futurdevelopement, constraint_id);
					FMTvertex_descriptor newvertex = boost::add_vertex(properties, data);
					developments[futurdevelopement.period].insert(Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>(newvertex,data[newvertex].get()));
					++stats.vertices;
					return newvertex;
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::adddevelopment", __LINE__, __FILE__);
			}
			return getdevelopment(futurdevelopement);

		}
		size_t hash(size_t seed = 0) const
		{
			try {
				boost::hash_combine(seed, boost::hash<Core::FMTdevelopment>()(*developments.at(0).begin()->pointerobject));
				FMTedge_iterator edge_iterator, edge_iterator_end;
				for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
					boost::hash_combine(seed, edgeprop.getactionID());
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::hash", __LINE__, __FILE__);
			}
			return seed;

		}
		void addaction(const int& actionID,
                        FMTgraphstats& statsdiff,
                        std::queue<FMTvertex_descriptor>& actives,
                        const FMTvertex_descriptor& out_vertex,
                        const std::vector<Core::FMTdevelopmentpath>& paths)
		{
			try {
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
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::addaction", __LINE__, __FILE__);
			}
		}
		double outarea(const FMTvertex_descriptor& out_vertex,
			const int& actionID, const double*&  solution) const
		{
			double value = 0;
			try{
				FMToutedge_iterator outedge_iterator, outedge_end;
				
				for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*outedge_iterator];
					if (edgeprop.getactionID() == actionID)
					{
						
						value += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
					}
				}
				
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::outarea", __LINE__, __FILE__);
			}
			return value;
		}

		FMTvertex_descriptor getgrowthsource(const FMTvertex_descriptor& out_vertex) const
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
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::getgrowthsource", __LINE__, __FILE__);
				}
			return FMTvertex_descriptor();
		}

		double inarea(const FMTvertex_descriptor& out_vertex,
                        const double*&  solution, bool growth = false) const
		{
			double area = 0;
			try{
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
				if (edgeprop.getactionID() < 0 || !growth)
				{
					area += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
				}
			}
			}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::inarea", __LINE__, __FILE__);
			}
			return area;
		}
		bool periodstart(const FMTvertex_descriptor& out_vertex) const
		{
			try {
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
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::periodstart", __LINE__, __FILE__);
			}
			return false;
		}

		bool onlypertiodstart(const FMTvertex_descriptor& out_vertex) const
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
				_exhandler->raisefromcatch("", "FMTgraph::onlypertiodstart", __LINE__, __FILE__);
			}
			return false;

		}

		int getmaximalock(const int& period)
		{
			int lock = 0;
			try {
				for (typename boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>::const_iterator devit = developments.at(period).begin();
					devit != developments.at(period).end(); devit++)
					{
					if (devit->pointerobject->lock>lock)
						{
						lock = devit->pointerobject->lock;
						}
					}
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::getmaximalock", __LINE__, __FILE__);
				}
			return lock;
		}

		bool periodstop(const FMTvertex_descriptor& out_vertex) const
		{
			try {
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
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::periodstop", __LINE__, __FILE__);
			}
			return false;
		}
		std::vector<Core::FMTdevelopmentpath> getpaths(const FMTvertex_descriptor& out_vertex,
                                            const int& actionID) const
		{
			std::vector<Core::FMTdevelopmentpath>paths;
			try {
				for (FMToutedge_pair edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
				{
					const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
					if (edgeprop.getactionID() == actionID)
					{
						const FMTbasevertexproperties& vertex_target = data[target(*edge_pair.first, data)];
						paths.push_back(Core::FMTdevelopmentpath(vertex_target.get(), edgeprop.getproportion()));
					}
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getpaths", __LINE__, __FILE__);
			}
			return paths;
		}
		bool isvalidouputnode(const Models::FMTmodel& model, const Core::FMToutputnode& node, std::vector<const Core::FMTaction*>& action_IDS, int period) const
		{
			try {
				if (static_cast<int>(developments.size()) > period)
				{
					action_IDS = node.source.targets(model.actions);
					if (!(period == 0 && !action_IDS.empty()) && !node.source.isnull(model.yields) && !node.factor.isnull(model.yields) && ((!action_IDS.empty() && !node.source.getaction().empty()) || node.source.getaction().empty()))
					{
						return true;
					}
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::isvalidouputnode", __LINE__, __FILE__);
			}
			return false;
		}
		bool isvalidgraphnode(const Models::FMTmodel& model, const FMTvertex_descriptor& vertex_descriptor,
                            const Core::FMToutputnode& node,const std::vector<const Core::FMTaction*>& selected) const
		{
			try {
				const Core::FMTdevelopment& development = data[vertex_descriptor].get();
				if (node.source.use(development, model.yields))
				{
					if (node.source.useinedges())
					{
						if ((development.period == 0 || periodstart(vertex_descriptor)) && ((selected.empty() && (node.source.isnextperiod() || !node.source.emptylock())) ||
							(((buildtype == FMTgraphbuild::schedulebuild) && development.anyoperable(selected, model.yields)) ||
								isanyoperables(vertex_descriptor, development.isanyworthtestingoperability(selected, model.actions)))))
								//anyoperables(vertex_descriptor, development.anyworthtestingoperability(selected, *model.actions.begin())))))
						{
							/*isanyoperables(vertex_descriptor, development.isanyworthtestingoperability(selected,model.actions)))))*/
							return true;
						}
					}
					else if (isanyoperables(vertex_descriptor, development.isanyworthtestingoperability(selected, model.actions))) //out edges
					{
						//(anyoperables(vertex_descriptor, development.anyworthtestingoperability(selected, *model.actions.begin())))
						/*isanyoperables(vertex_descriptor, development.isanyworthtestingoperability(selected, model.actions))*/
						return true;
					}
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::isvalidgraphnode", __LINE__, __FILE__);
			}
			return false;
		}
		std::map<int,double> locatenode(const Models::FMTmodel& model,Core::FMToutputnode output_node, int period) const
		{
			std::map<int, double>emptyreturn;
			try {
				const std::vector<FMTvertex_descriptor>verticies = getnode(model, output_node, period);
				return getvariables(model, output_node, verticies);
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::locatenode", __LINE__, __FILE__);
			}
			return emptyreturn;

		}
		std::map<std::string,std::map<int,double>>  locatenodebytheme(const Models::FMTmodel& model,Core::FMToutputnode output_node, int period) const
				{
				std::map<std::string,std::map<int,double>> variablesreturn;
				//std::vector<std::map<int,double>> variablesreturn;
					try {
						const std::vector<FMTvertex_descriptor>verticies = getnode(model, output_node, period);
						int themetarget = output_node.source.getthemetarget();
						if(themetarget<0)
						{
							variablesreturn["~nothemetargetid~"]=getvariables(model, output_node, verticies);
							//variablesreturn.push_back(getvariables(model, output_node, verticies));
						}else{
							std::map<std::string,std::vector<FMTvertex_descriptor>> orderedtarget;
							for (const auto& vert : verticies)
							{
								const Core::FMTdevelopment& dev = getdevelopment(vert);
								const std::string value = dev.mask.get(model.themes.at(themetarget));
								if(orderedtarget.find(value)==orderedtarget.end())
								{
									orderedtarget[value]=std::vector<FMTvertex_descriptor>();
								}
								orderedtarget[value].push_back(vert);
							}
							for (const auto& odtar:orderedtarget)
							{
								variablesreturn[odtar.first]=getvariables(model,output_node,odtar.second);
								//variablesreturn.push_back(getvariables(model,output_node,odtar.second));
							}
						}
					}
					catch (...)
					{
						_exhandler->raisefromcatch("", "FMTgraph::locatenodebytheme", __LINE__, __FILE__);
					}
					return variablesreturn;

				}
		std::vector<FMTvertex_descriptor> getnode(const Models::FMTmodel& model, Core::FMToutputnode output_node, int period) const
		{
			std::vector<FMTvertex_descriptor>locations;
			try {
				std::vector<int>targetedperiods;
				const int maxperiod = static_cast<int>(developments.size() - 2);
				const int node_period = output_node.settograph(targetedperiods, period, maxperiod);
				if (node_period < 0)
				{
					return locations;
				}
				if (!output_node.source.isvariablelevel())
				{
					//constexpr size_t minimalcachedrop = 25;//10 %
					std::vector<const Core::FMTaction*> selected;
					if (isvalidouputnode(model, output_node, selected, node_period))
					{
						//cleannodecaching();
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
								//*_logger << "got exact for " << std::string(output_node) << "\n";
								locations.reserve(locations.size() + descriptors.size());
								locations.insert(locations.end(), descriptors.begin(), descriptors.end());
							}
							else {
								//std::vector<FMTvertex_descriptor>revisited;
								//revisited.reserve(descriptors.size());
								locations.reserve(descriptors.size());
								for (const FMTvertex_descriptor& potential : descriptors)
								{
									if (isvalidgraphnode(model, potential, output_node, selected))
									{
										//revisited.push_back(potential);
										locations.push_back(potential);
									}
								}
								//if (revisited.size()<=((descriptors.size()/100)*(100-minimalcachedrop)))//Dont add too much
								//	{
									//std::sort(revisited.begin(), revisited.end());
								    std::sort(locations.begin(), locations.end());
									//nodescache.at(localnodeperiod).setvalidverticies(output_node, revisited);
									nodescache.at(localnodeperiod).setvalidverticies(output_node, locations);
								/*}
								else {
									nodescache[localnodeperiod].erasenode(output_node);
								}*/
									//*_logger << "pushed size "<<std::string(output_node)<<" "<< locations.size()<<" " <<descriptors.size() <<"\n";
								
							}
						}


					}

				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getnode", __LINE__, __FILE__);
			}
			return locations;
		}
		std::map<int, double> getvariables(const Models::FMTmodel& model, const Core::FMToutputnode& output_node,const std::vector<FMTvertex_descriptor>& verticies) const
		{
			std::map<int, double>variables;
			try {
				if (!verticies.empty())
				{
					//std::vector<Core::FMTdevelopmentpath>paths;
					//Core::FMTaction optimization_action;
					const std::vector<const Core::FMTaction*> selected = output_node.source.targets(model.actions);
					for (const FMTvertex_descriptor& vertex : verticies)
					{
						const Core::FMTdevelopment& development = data[vertex].get();
						if (output_node.source.useinedges())
						{
							const double coef = output_node.source.getcoef(development, model.yields) * output_node.factor.getcoef(development, model.yields) * output_node.constant;
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
									const std::vector<Core::FMTdevelopmentpath>paths = getpaths(vertex, actionID);
									const double action_coef = output_node.source.getcoef(development, model.yields,&paths,act) * output_node.factor.getcoef(development, model.yields,&paths,act) * output_node.constant;
									updatevarsmap(variables, outvars.at(actionID), action_coef);
								}
							}
						}
					}
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getvariables", __LINE__, __FILE__);
			}
			return variables;
		}
		bool isanyoperables(const FMTvertex_descriptor& descriptor, const std::vector<bool>& actionsop) const noexcept
		{
			//try {
				if (!actionsop.empty())
				{
					FMToutedge_pair edge_pair;
					for (edge_pair = boost::out_edges(descriptor, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
					{
						const FMTbaseedgeproperties& edgeprop = data[*edge_pair.first];
						const short int* actionid = edgeprop.getactionptr();
						if (*actionid >= 0 && actionsop.at(*actionid))
						{
							return true;
						}
					}
				}
				
			/*}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::isanyoperables", __LINE__, __FILE__);
			}*/
			return false;
		}



        bool anyoperables(const FMTvertex_descriptor& descriptor, const std::vector<int>& action_ids) const
		{
			try {
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
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::anyoperables", __LINE__, __FILE__);
			}
			return false;
		}
		std::map<int, int> getoutvariables(const FMTvertex_descriptor& out_vertex) const
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
				_exhandler->raisefromcatch("", "FMTgraph::getoutvariables", __LINE__, __FILE__);
			}
			return mapping;
		}
		std::vector<int>getoutactions(const FMTvertex_descriptor& out_vertex) const
		{
			std::vector<int>actions;
			try {
				const size_t outsize = boost::out_degree(out_vertex, data);
				if (outsize > 1)
				{
					actions.reserve(outsize - 1);
					FMToutedge_pair edge_pair;
					for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
					{
						const FMTedgeproperties& edgeprop = data[*edge_pair.first];
						const short int* actionid = edgeprop.getactionptr();
						if ((*actionid) >= 0)
						{
							actions.emplace_back(static_cast<int>(*actionid));
						}
					}
					std::sort(actions.begin(), actions.end());
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getoutactions", __LINE__, __FILE__);
			}
			return actions;
		}
		std::vector<const Core::FMTaction*> selectedactions(const Models::FMTmodel& model, const std::vector<int>& action_IDS) const
		{
			std::vector<const Core::FMTaction*>selected;
			try {
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
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::selectedactions", __LINE__, __FILE__);
				}
			return selected;
		}
		bool constraintlenght(const Core::FMTconstraint& constraint, int& start, int& stop) const
		{
			try {
				int constraintlower = constraint.getperiodlowerbound();
				if (constraintlower == std::numeric_limits<int>::max())
				{
					constraintlower = static_cast<int>((developments.size() - 2));
				}
				//start = std::max(constraintlower, getfirstactiveperiod() + 1);
				start = std::max(constraintlower, getfirstperiod());
				stop = static_cast<int>((constraint.getperiodupperbound() > static_cast<int>((developments.size() - 2))) ? (developments.size() - 2) : constraint.getperiodupperbound());
				if (constraint.acrossperiod())
				{
					--stop;
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::constraintlenght", __LINE__, __FILE__);
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
			try {
				developments.push_back(boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>());
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
			}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTgraph::buildschedule", __LINE__, __FILE__);
		}
			return (statsdiff - stats);
		}


		FMTgraphstats eraseperiod(std::vector<int>& deletedconstraints,
			std::vector<int>&deletedvariables,
			bool keepbounded = false)
		{
			try {
				typename std::vector<boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>>::iterator periodit = this->getfirstblock();
				for (typename boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>::iterator it = periodit->begin();
					it != periodit->end(); it++)
				{
					const FMTvertex_descriptor& vertex_location = it->memoryobject;
					FMTinedge_iterator inedge_iterator, inedge_end;
					bool gotinedges = false;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges((*it).memoryobject, data); inedge_iterator != inedge_end; ++inedge_iterator)
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
				boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>restingdevelopments;
				for (typename boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>::iterator it = periodit->begin();
					it != periodit->end(); it++)
				{
					const FMTvertex_descriptor& vertex_location = (*it).memoryobject;
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
						restingdevelopments.insert(*it);
					}
				}
				*periodit = restingdevelopments;
				const int firstperiod = this->getfirstactiveperiod();
				if (static_cast<size_t>(firstperiod) < nodescache.size())
				{
					nodescache[firstperiod].clear();
				}
				
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::eraseperiod", __LINE__, __FILE__);
				}
			return stats;
		}
		bool empty() const
		{
			return developments.empty();
		}
		std::vector<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>getsortedverticies(const boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>& uset) const
		{
			std::vector<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>sorted;
			try {
				sorted = std::vector<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>(uset.begin(), uset.end());
				std::sort(sorted.begin(), sorted.end(),lookupsorter());
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getsortedverticies", __LINE__, __FILE__);
			}
			return sorted;
		}


		std::queue<FMTvertex_descriptor> getactiveverticies() const
		{
			std::queue<FMTvertex_descriptor>actives;
			try {
				for (auto& lastperiod : /*getsortedverticies(*/developments.back()/*)*/)
				{
					actives.push(lastperiod.memoryobject);
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getactiveverticies", __LINE__, __FILE__);
			}
			return actives;
		}
		std::vector<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>getsortedperiodverticies(int period) const
		{
			std::vector<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>sorted;
			try {
				sorted = getsortedverticies(getperiodverticies(period));
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getsortedperiodverticies", __LINE__, __FILE__);
			}
			return sorted;
		}

		const boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>& getperiodverticies(int period) const
			{
			return developments.at(period);
			}

		lookconst_iterator begin(int period) const
		{
			return developments.at(period).cbegin();
		}

		lookconst_iterator end(int period) const
		{
			return developments.at(period).cend();
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
			try {
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
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::gettransferrow", __LINE__, __FILE__);
			}
			return true;
		}
		void getinitialbounds(std::vector<double>& lower_bounds, std::vector<double>& upper_bounds) const
		{
			try {
				
				for (const auto& descriptors : developments.at(getfirstactiveperiod()))
				{
					const FMTvertex_descriptor descriptor = descriptors.memoryobject;
					const FMTvertexproperties& property = data[descriptor];
					const std::map<int, int>outs = getoutvariables(descriptor);
					lower_bounds[outs.at(-1)] = property.getbaseRHS();
					upper_bounds[outs.at(-1)] = property.getbaseRHS();
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getinitialbounds", __LINE__, __FILE__);
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
			std::map<std::string, double>emptyreturn;
			try{
			const std::vector<FMTvertex_descriptor>verticies = getnode(model, node, period);
			/**_logger << "size of " << verticies.size() << "\n";
			for (FMTvertex_descriptor ver : verticies)
				{
				*_logger << "verticies found " << std::string(data[ver].get()) << "\n";
				}*/
			
			return getvalues(model, verticies, node, theme, solution, level);
			}catch (...)
				{
				_exhandler->raisefromcatch("For node: "+std::string(node), "FMTgraph::getsource", __LINE__, __FILE__);
				}
			return emptyreturn;
		}
		std::map<std::string, double> getvalues(const Models::FMTmodel& model, const std::vector<FMTvertex_descriptor>& verticies,
			const Core::FMToutputnode& node, const Core::FMTtheme& theme,
			const double* solution, FMToutputlevel level) const
		{
			std::map<std::string, double>values;
			try {
				if (level == FMToutputlevel::standard)
				{
					for (const std::string& attribute : theme.getbaseattributes())
					{
						values[attribute] = 0;
					}
				}
				else if (level == FMToutputlevel::totalonly)
				{
					values["Total"] = 0;
				}
				if (!verticies.empty())
				{
					const std::vector<const Core::FMTaction*> selected = node.source.targets(model.actions);
					
					//Core::FMTaction optimization_action;
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
							if (node.source.isnextperiod())//If it looks at next period make sure to show the right dev...
								{
								value = std::string(data[getgrowthsource(vertex)].get());
							}else {
								value = std::string(development);
								}
							
							values[value] = 0;
						}
						else {
							value = "Total";
						}

						if (node.source.useinedges())
						{
							const double coef = node.source.getcoef(development, model.yields) * node.factor.getcoef(development, model.yields) * node.constant;
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
								const std::vector<Core::FMTdevelopmentpath>paths = getpaths(vertex, actionID);
								const double action_coef = node.source.getcoef(development, model.yields,&paths,act) * node.factor.getcoef(development, model.yields,&paths,act) * node.constant;
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
				_exhandler->raisefromcatch("For node: "+std::string(node)+ " on theme "+std::string(theme), "FMTgraph::getvalues", __LINE__, __FILE__);
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
				_exhandler->raisefromcatch("", "FMTgraph::getperiod", __LINE__, __FILE__);
				}
			return 0;
		}


        void generatedevelopments()
		{
			try {
				nodescache.clear();
				developments.clear();
				const size_t max_period = static_cast<size_t>(getperiod());
				FMTvertex_iterator vertex, vend;
				developments.resize(max_period + 1);
				for (boost::tie(vertex, vend) = vertices(data); vertex != vend; ++vertex)
				{
					const FMTbasevertexproperties& properties = data[*vertex];
					const Core::FMTdevelopment& dev = properties.get();
					const size_t period_location = (dev.period);
					developments[period_location].insert(Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>(*vertex,dev));
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::generatedevelopments", __LINE__, __FILE__);
			}
		}
		bool sameedgesas(const FMTgraph& rhs) const
		{
			bool different = false;
			try {
				FMTedge_iterator thisedge_iterator, thisedge_iterator_end;
				FMTedge_iterator rhsedge_iterator, rhsedge_iterator_end;
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
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::sameedgesas", __LINE__, __FILE__);
				}
			return different;
		}
		void updatematrixindex(const std::vector<int>& removedvariables,
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
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::updatematrixindex", __LINE__, __FILE__);
			}
		}
		int getfirstactiveperiod() const
		{
			return static_cast<int>(std::distance(developments.begin(), getfirstconstblock()));
		}

		FMTinedge_iterator getlastdisturbance(FMTinedge_iterator activeedge,int& periodtolastdisturbance) const
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
						periodtolastdisturbance = data[vertexsource].get().period;
						return activeedge;
						}
					FMTinedge_iterator inedge_iterator, inedge_end;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertexsource, data); inedge_iterator != inedge_end; ++inedge_iterator)
						{
						actives.push(inedge_iterator);
						}
					}
				lastedge = activeedge;
			}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getlastdisturbance", __LINE__, __FILE__);
			}
			
			return lastedge;
		}

		void filluplastactions(const int& targetperiod,const FMTvertex_descriptor& targetdescriptor,std::vector<const FMTbaseedgeproperties*>& lastactions, std::vector<int>& distances,const size_t& depth) const
		{
			try {
				std::queue<FMTvertex_descriptor>activevertex;
				activevertex.push(targetdescriptor);
				while (!activevertex.empty()&&lastactions.size() <= depth)
				{
					FMTvertex_descriptor descriptor = activevertex.front();
					activevertex.pop();
					FMTinedge_iterator inedge_iterator, inedge_end;
					for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
					{
						const FMTbaseedgeproperties& inedgeproperties = data[*inedge_iterator];
						const FMTvertex_descriptor& sourcevertex = boost::source(*inedge_iterator, data);
						activevertex.push(sourcevertex);
						if (inedgeproperties.getactionID()>=0)
							{
							lastactions.push_back(&inedgeproperties);
							distances.push_back(targetperiod - data[sourcevertex].get().period);
							}
						
					}
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::filluplastactions", __LINE__, __FILE__);
			}

		}



		std::vector<FMTcarbonpredictor> getcarbonpredictors(const FMTvertex_descriptor& targetdescriptor, const std::map<int, int>& actionsindex,
			const std::vector<std::string>& yieldnames, const Core::FMTyields&yields,const size_t& depth) const
			{
			std::vector<FMTcarbonpredictor> predictors;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				const FMTbasevertexproperties& targetproperties = data[targetdescriptor];
				const int targetperiod = targetproperties.get().period;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					std::vector<const FMTbaseedgeproperties*>lastactions;
					std::vector<int>distances;
					const FMTvertex_descriptor& sourcevertex = boost::source(*inedge_iterator, data);
					const FMTbasevertexproperties& sourceproperties = data[sourcevertex];
					lastactions.push_back(&data[*inedge_iterator]);
					distances.push_back(targetperiod-sourceproperties.get().period);
					filluplastactions(targetperiod, sourcevertex, lastactions, distances, depth);
					while (lastactions.size()<= depth)
						{
						lastactions.push_back(nullptr);
						distances.push_back(-1);
						}

					predictors.emplace_back(actionsindex, yieldnames, yields, sourceproperties, targetproperties, lastactions, distances);
				}
			predictors.shrink_to_fit();
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::getcarbonpredictors", __LINE__, __FILE__);
				}
			return predictors;
			}

		int getfirstperiod() const
			{
			return developments.at(getfirstactiveperiod() +1).begin()->pointerobject->period;
			}
		FMTgraph postsolve(const Core::FMTmask& selectedmask,
			const std::vector<Core::FMTtheme>&originalbasethemes,
			const std::map<int,int>& actionmapconnection) const
		{
			FMTgraph newgraph(*this);
			try {
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
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::postsolve", __LINE__, __FILE__);
			}
			return newgraph;
		}
		Core::FMTschedule getschedule(const std::vector<Core::FMTaction>& actions, const double* actual_solution, const int& lperiod,bool withlock = false) const
		{
			Core::FMTschedule newschedule(lperiod,*this, withlock);
			try {
				if (static_cast<int>(size()) > lperiod && lperiod > 0)
				{
					//newschedule.setperiod(lperiod);
					//std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::map<int, double>>>schedule_solution;
					//const double* actual_solution = this->getColSolution();
					for (const auto deviterator : getperiodverticies(lperiod))
					{
						const FMTvertex_descriptor vertex = deviterator.memoryobject;
						std::map<int, int>variables = getoutvariables(vertex);
						variables.erase(-1);
						if (!variables.empty())
						{
							for (const auto variable_iterator : variables)
							{
								if (*(actual_solution + variable_iterator.second) > FMT_DBL_TOLERANCE) //basis solution only!!!
								{
									
									/*if (schedule_solution.find(actions[variable_iterator.first]) == schedule_solution.end())
									{
										schedule_solution[actions[variable_iterator.first]] = std::map<Core::FMTdevelopment, std::map<int, double>>();
									}
									const Core::FMTdevelopment& basedev = getdevelopment(deviterator.memoryobject);*/									
									newschedule.addevent(*deviterator.pointerobject, *(actual_solution + variable_iterator.second), actions.at(variable_iterator.first));
									/*Core::FMTdevelopment lockout = basedev.clearlock();
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
					newschedule.clean();
					/*Core::FMTschedule newschedule(lperiod, schedule_solution);
					newschedule.passinobject(*this);
					if (withlock)
						{
						newschedule.setuselock(true);
						}*/
					//return newschedule;
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("at period " + std::to_string(lperiod), "FMTgraph::getschedule", __LINE__, __FILE__);
			}
			
			return newschedule;
		}
		operator std::string() const
		{
			std::ostringstream stream;
			try {
				//write_graphviz(stream, data);
			}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::std::string()", __LINE__, __FILE__);
			}
			return stream.str();
		}

    };

template<> inline std::map<int, int> FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getoutvariables(const FMTvertex_descriptor& out_vertex) const
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
			_exhandler->raisefromcatch("", "FMTgraph::getoutvariables", __LINE__, __FILE__);
		}
		return mapping;
	}


template<> inline std::vector<Core::FMTdevelopmentpath> FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getpaths(const FMTvertex_descriptor& out_vertex, const int& actionID) const
	{
		std::vector<Core::FMTdevelopmentpath>paths;
		try {
			for (FMToutedge_pair edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
			{
				const FMTedgeproperties& edgeprop = data[*edge_pair.first];
				if (edgeprop.getactionID() == actionID)
				{
					const FMTbasevertexproperties& vertex_target = data[target(*edge_pair.first, data)];
					paths.push_back(Core::FMTdevelopmentpath(vertex_target.get(), edgeprop.getproportion()));
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTgraph::getpaths", __LINE__, __FILE__);
			}
		return paths;
	}



template<> inline double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::inarea(const FMTvertex_descriptor& out_vertex, const double*&  solution, bool growth) const
	{
		
		double area = 0;
		try {
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTedgeproperties& edgeprop = data[*inedge_iterator];
				if (edgeprop.getactionID() < 0 || !growth)
				{
					area += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTgraph::inarea", __LINE__, __FILE__);
			}
		return area;
	}

template<> inline double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::outarea(const FMTvertex_descriptor& out_vertex, const int& actionID, const double*&  solution) const
	{
		double value = 0;
		FMToutedge_iterator outedge_iterator, outedge_end;
		try {
			for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
			{
				const FMTedgeproperties& edgeprop = data[*outedge_iterator];
				if (edgeprop.getactionID() == actionID)
				{
					value += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
				}
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTgraph::outarea", __LINE__, __FILE__);
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

