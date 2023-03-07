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

#include "FMTvertexproperties.hpp"
#include "FMTedgeproperties.hpp"
#include "FMTgraphstats.hpp"

#include "FMToutputnode.hpp"
#include "FMTdevelopment.hpp"
#include "FMTactualdevelopment.hpp"
#include "FMTfuturdevelopment.hpp"
#include "FMTdevelopmentpath.hpp"
#include "FMTschedule.hpp"
#include "FMTconstraint.hpp"
#include "FMToutputnodecache.hpp"
#include <boost\unordered_set.hpp>
#include "FMTpredictor.hpp"
#include "FMTgraphvertextoyield.hpp"



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

#include "FMTmodel.hpp"
#include "FMTaction.hpp"
#include "FMTtheme.hpp"
#include "FMTobject.hpp"
#include <tuple>
#include <assert.h>
#include "FMTlookup.hpp"
#include "boost/graph/graphviz.hpp"
#include "FMTexceptionhandler.hpp"


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
	protected:
        FMTgraphbuild buildtype;
		std::vector<FMTvertex_pair>developments;
		mutable std::vector<FMToutputnodecache<FMTvertex_descriptor FMT_COMMA FMTvertex_iterator>>nodescache;
		typedef typename std::vector<FMToutputnodecache<FMTvertex_descriptor FMT_COMMA FMTvertex_iterator>>::reverse_iterator reversecachenodeit;
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
		typename std::vector<FMTvertex_pair>::iterator getfirstblock()
		{
			typename std::vector<FMTvertex_pair>::iterator periodit = developments.begin();
			try {
				if (!developments.empty())
				{
					FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
					while (periodit!= developments.end()&&periodit->second==vertex_iterator_end)
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

		typename std::vector<FMTvertex_pair>::const_iterator getfirstconstblock() const
		{
			typename std::vector<FMTvertex_pair>::const_iterator periodit = developments.begin();
			try {
				if (!developments.empty())
				{
					FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
					while (periodit != developments.end() && periodit->second == vertex_iterator_end)
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
		bool isdependant(const FMTvertex_descriptor& descriptor,
			const int& theactionid,bool& newedge) const
		{
			try {
				newedge = true;
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
									"Action recursivity " + std::to_string(theactionid)+" ", "FMTgraph::isdependant", __LINE__, __FILE__);
							newedge = false;
							}
						}
					}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::isdependant", __LINE__, __FILE__);
			}
			return false;
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
			generatedevelopments();
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
					typename std::vector<FMTvertex_pair>::const_iterator devsit = this->getfirstconstblock();
					size_t location = 0;
					while (devsit != developments.end())
					{
						//for (typename std::vector<FMTvertex_pair>::const_iterator it = developments.at(location).begin(); it != developments.at(location).end(); it++)
						//{
							if(((data[*developments.at(location).first].get())!= (rhs.data[*rhs.developments.at(location).first].get()) ||
								(data[*developments.at(location).second].get()) != (rhs.data[*rhs.developments.at(location).second].get())))
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
        		std::vector<FMToutputnodecache<FMTvertex_descriptor FMT_COMMA FMTvertex_iterator>>().swap(nodescache);
			}
        void cleardevelopments()
			{
        		std::vector<FMTvertex_pair>().swap(developments);
			}
		FMTgraphbuild getbuildtype() const
			{
			return buildtype;
			}
		void setbuildtype(const FMTgraphbuild& build)
			{
			buildtype = build;
			}
		boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>getdevsset(const int& period) const
			{
			boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>> basedevs;
			try {
				FMTvertex_iterator vertex_iterator,vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = developments.at(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
					basedevs.insert(Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>(*vertex_iterator, data[*vertex_iterator].get()));
					}
			}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getdevsset", __LINE__, __FILE__);
			}
			return basedevs;
			}

		bool containsdevelopment(const Core::FMTdevelopment& development,
								 const boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>& alldevs) const
			{
			return (alldevs.find(Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>(development)) != alldevs.end());
			}

		std::queue<FMTvertex_descriptor> initialize(const std::vector<Core::FMTactualdevelopment>& actdevelopments)
			{
				std::queue<FMTvertex_descriptor>actives;
				try {
					developments.clear();
					const int startingperiod = actdevelopments.begin()->getperiod();
					const int actualperiod = startingperiod+1;
					FMTvertex_iterator vertex_iterator, vertex_iterator_end;
					//End always stay the same use .end() for non valid period
					boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data);
					for (int period = 0 ; period <= actualperiod;++period)
						{
						developments.push_back(FMTvertex_pair(vertex_iterator_end,vertex_iterator_end));
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
						Core::FMTfuturdevelopment P1dev(development);
						P1dev.setperiod(actualperiod);
						const FMTvertex_descriptor tovertex = adddevelopment(P1dev, devsets);
						actives.push(tovertex);
						//Now set the edge!!
						const FMTedgeproperties newedge(edge_id, stats.edges, proportion);
						boost::add_edge(P0descriptors.at(poid), tovertex, newedge, data);
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
					std::queue<FMTvertex_descriptor> actives,
					int compressageoperability=1)
				{
					FMTgraphstats newstats;
					try {
						FMTgraphstats statsdiff(stats);
						const int actualperiod = getperiod();
						const bool gotseries = model.useactionserie();
						boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>> actualdevs = getdevsset(actualperiod);
						int action_id = 0;
						for (const Core::FMTaction& action : model.actions)
						{
							std::queue<FMTvertex_descriptor> new_actives;
							while (!actives.empty())
							{
								const FMTvertex_descriptor front_vertex = actives.front();
								actives.pop();
								const Graph::FMTgraphvertextoyield vertexinfo = getvertextoyieldinfo(model,front_vertex);
								const FMTbasevertexproperties& front_properties = data[front_vertex];
								const Core::FMTdevelopment& active_development = front_properties.get();					
								bool doesnotgrow = false;
								if (active_development.operable(action,model.yields,&vertexinfo) &&
									active_development.getage()%compressageoperability==0)
								{
									const bool gotaserie = (gotseries && stayonserie(front_vertex, action, model));
									if (gotaserie||action.getname() == "_DEATH")
									{
										if (gotaserie && boost::out_degree(front_vertex, data) > 0) // If you are on a serie and you can be operated by other action just throw...
											{
											_exhandler->raise(Exception::FMTexc::FMTinvalid_action,
												std::string(front_properties.get())+
												" is on a serie for action "+action.getname() + " and have been already operated",
												"FMTgraph::build", __LINE__, __FILE__);
											}
										doesnotgrow = true;
									}
									const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
									addaction(action_id, statsdiff, new_actives, front_vertex, paths, actualdevs, gotseries);
								}
								if (!doesnotgrow)
								{
									new_actives.push(front_vertex);
								}

							}
							actives = new_actives;
							++action_id;
						}
						const bool typeIIforestmodel = (model.getparameter(Models::FMTintmodelparameters::MATRIX_TYPE) == 2);
						newstats = naturalgrowth(actives, statsdiff, typeIIforestmodel, gotseries);
					}
					catch (...)
					{
						_exhandler->raisefromcatch("", "FMTgraph::build", __LINE__, __FILE__);
					}
					
					return newstats;
				}

		bool isnotransfer(const FMTvertex_descriptor& descriptor,size_t outcount = 0) const
			{
			try {
				if (boost::in_degree(descriptor, data) == 1 &&
					boost::out_degree(descriptor, data) == outcount)
					{
					if (outcount<1)
						{
						return true;
					}else {
						//No Death action please
						const std::map<int, int>outsvar = getoutvariables(descriptor);
						return (outsvar.find(-1) != outsvar.end());
						}
					}
					
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::isnotransfer", __LINE__, __FILE__);
				}
			return false;
			}

		

		double getinproportion(const FMTvertex_descriptor& vertex_descriptor) const
		{
			return 1;
		}
		
		FMTgraphstats naturalgrowth(std::queue<FMTvertex_descriptor> actives, FMTgraphstats statsdiff,bool typeIImatrix = false,bool splitgrowth = false)
		{
			try {
				boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>> nextperiods;
				FMTvertex_iterator vertex_iterator, vertex_iterator_end,lastoperated;
				boost::tie(vertex_iterator, vertex_iterator_end) = developments.back();
				lastoperated = vertex_iterator_end;
				--lastoperated;
				while (!actives.empty())
				{
					const FMTvertex_descriptor front_vertex = actives.front();
					actives.pop();
					FMTbasevertexproperties front_properties = data[front_vertex];
					const Core::FMTdevelopment active_development = front_properties.get();
					const Core::FMTfuturdevelopment grown_up = active_development.grow();
					FMTvertex_descriptor next_period = this->adddevelopment(grown_up, nextperiods,splitgrowth); //getset
					int variableindex = statsdiff.cols;
					double proportion = 100;
					if (!splitgrowth&&(typeIImatrix && isnotransfer(front_vertex)))//do a type II dont need new variable
					{
						variableindex = getinvariables(front_vertex).at(0);
						proportion = getinproportion(front_vertex);
					}else{ //We need a new variable
						++statsdiff.cols;
						}
					const FMTedgeproperties newedge(-1, variableindex,proportion);
					boost::add_edge(front_vertex, next_period, newedge, data);
					++stats.edges;
				}
				++lastoperated;
				developments.back() = FMTvertex_pair(vertex_iterator, lastoperated);
				developments.push_back(FMTvertex_pair(lastoperated, vertex_iterator_end));
				rebasecache();
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
						if (colnames.at(variableid).empty())
							{
							const int actionid = data[*outit].getactionID();
							if (actionid >=0)
								{
								actionname = actions.at(actionid).getname();
								}
							colnames[variableid] = basename + actionname;
							}
						}

					}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getvariablenames", __LINE__, __FILE__);
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
		}*/

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
				FMTvertex_iterator vertexit, vertexend;
				for (boost::tie(vertexit, vertexend) = developments.at(location); vertexit != vertexend; ++vertexit)
				{
					if (periodstop(*vertexit))
					{
						const double area = inarea(*vertexit, actual_solution);
						const FMTbasevertexproperties& vetexprop = data[*vertexit];
						all_period_stop_devs.push_back(Core::FMTactualdevelopment(vetexprop.get(),area));
					}
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getperiodstopdev", __LINE__, __FILE__);
			}
			return all_period_stop_devs;

		}
		std::map<std::string, double> getoutput(const Models::FMTmodel& model, const Core::FMToutput& output,
			int period, const double* solution, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const
		{
			Core::FMTtheme targettheme;
			std::vector<std::string>target_attributes;
			std::map<std::string, double>results;
			try {
				if(output.targetthemeid() < 0 && !(level == Core::FMToutputlevel::developpement))
				{
					level = Core::FMToutputlevel::totalonly;
				}
				if (level != Core::FMToutputlevel::developpement)
				{
					if (level == Core::FMToutputlevel::standard)
					{
						target_attributes = output.getdecomposition(model.themes);
						if (!target_attributes.empty() && level == Core::FMToutputlevel::standard)
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
					std::vector<std::string> equation;
					if (output.canbenodesonly())
					{
						for (const Core::FMToutputnode& output_node : output.getnodes(equation))
						{
							const std::map<std::string, double> srcvalues = getsource(model, output_node, period, targettheme, solution, level);
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
					}else {
						std::map<std::string, std::vector<std::string>>allequations;
						const std::vector<Core::FMToutputnode> allnodes = output.getnodes(equation,1, false,period);
						
						
						if (allnodes.empty())
						{
							if (level==Core::FMToutputlevel::developpement)
								{
								_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
									"Cannot get level values by developement",
									"FMTgraph::getoutput", __LINE__, __FILE__);
								}
							allequations["Total"] = equation;

						}else {
							size_t outid = 0;
							for (const Core::FMToutputnode& output_node : allnodes)
							{
								const std::map<std::string, double> srcvalues = getsource(model, output_node, period, targettheme, solution, level);
								output_node.fillupequation(allequations, srcvalues, equation, outid);
								++outid;
							}
						}
						output.fillfromshuntingyard(equation,results, allnodes, allequations);
						}

			}
			catch (...)
			{
				_exhandler->raisefromcatch("For output: "+std::string(output), "FMTgraph::getoutput", __LINE__, __FILE__);
			}

			return results;

		}
		FMTvertex_descriptor getdevelopment(const Core::FMTdevelopment& developement,
			const boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>& alldevs) const
		{
			const Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment> tofind(developement);
			return alldevs.find(tofind)->memoryobject;
		}
		const Core::FMTdevelopment& getdevelopment(const FMTvertex_descriptor& descriptor) const
			{
			return data[descriptor].get();
			}
		FMTvertex_descriptor adddevelopment(const Core::FMTfuturdevelopment& futurdevelopement,
											boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>>& alldevs,bool forcenewone=false)
		{
			try {
				if (!this->containsdevelopment(futurdevelopement, alldevs)||forcenewone)
				{
					const int constraint_id = -1;
					const FMTvertexproperties properties(futurdevelopement, constraint_id);
					FMTvertex_descriptor newvertex = boost::add_vertex(properties, data);
					alldevs.insert(Core::FMTlookup<FMTvertex_descriptor,Core::FMTdevelopment>(newvertex,data[newvertex].get()));
					++stats.vertices;
					return newvertex;
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::adddevelopment", __LINE__, __FILE__);
			}
			return getdevelopment(futurdevelopement, alldevs);

		}
		FMTvertex_descriptor adddevelopment(const Core::FMTfuturdevelopment& futurdevelopement)
		{
			FMTvertex_descriptor newvertex = data.null_vertex();
			try {
					const int constraint_id = -1;
					const FMTvertexproperties properties(futurdevelopement, constraint_id);
					newvertex = boost::add_vertex(properties, data);
					++stats.vertices;
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::adddevelopment", __LINE__, __FILE__);
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
				_exhandler->raisefromcatch("", "FMTgraph::hash", __LINE__, __FILE__);
			}
			return seed;

		}
		void addaction(const int& actionID,
			FMTgraphstats& statsdiff,
			std::queue<FMTvertex_descriptor>& actives,
			const FMTvertex_descriptor& out_vertex,
			const std::vector<Core::FMTdevelopmentpath>& paths,
			boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>& devsets,
			bool inserie = false)
		{
			try {
				//int variable_id = statsdiff.cols;
				//++statsdiff.cols;
				//std::vector<FMTvertex_descriptor>active_vertex;
				bool newchoice = false;
				for (const Core::FMTdevelopmentpath& devpath : paths)
				{
					FMTvertex_descriptor tovertex;
					bool newedge = false;
					if (!this->containsdevelopment(*devpath.development,devsets))
					{
						
						tovertex = this->adddevelopment(*devpath.development,devsets);
						actives.push(tovertex);
						newedge = true;
					}else {
						tovertex = this->adddevelopment(*devpath.development, devsets);
						if (inserie||isdependant(tovertex, actionID, newedge))
							{
							tovertex = this->adddevelopment(*devpath.development, devsets, true);
							actives.push(tovertex);
							}
					}
					if (newedge)
					{
						const FMTedgeproperties newedge(actionID, statsdiff.cols, devpath.proportion);
						boost::add_edge(out_vertex, tovertex, newedge, data);
						++stats.edges;
						newchoice = true;
					}
					
				}
				if (newchoice)
				{
					++statsdiff.cols;
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::addaction", __LINE__, __FILE__);
			}
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
					const FMTvertex_descriptor tovertex = this->adddevelopment(*devpath.development);
					actives.push(tovertex);
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

		std::vector<FMTvertex_descriptor> getactionsources(const FMTvertex_descriptor& out_vertex,const int& actionid) const
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
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::getactionsources", __LINE__, __FILE__);
				}
			return vsources;
		}

		double inarea(const FMTvertex_descriptor& out_vertex,
                        const double*&  solution,int actionid = -1, bool growth = false) const
		{
			double area = 0;
			try{
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
				if (edgeprop.getactionID() == actionid || !growth)
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
				FMTvertex_iterator vertexit, vertexend;
				for (boost::tie(vertexit, vertexend) = developments.at(period);vertexit!=vertexend;++vertexit)
					{
					const Core::FMTdevelopment& dev = data[*vertexit].get();
					if (dev.getlock()>lock)
						{
						lock = dev.getlock();
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
				if ((source_properties.get().getperiod()) == 0)
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
						return ((development.getperiod() == 0 || node.source.isaction() || periodstart(vertex_descriptor)) && ((selected.empty() && (node.source.isnextperiod() || !node.source.emptylock())) ||
							(((buildtype == FMTgraphbuild::schedulebuild) && development.anyoperable(selected, model.yields)) ||
								isanyoperables(vertex_descriptor, development.isanyworthtestingoperability(selected, model.actions)))));
					}
					else //out edges
					{
						return isanyoperables(vertex_descriptor, development.isanyworthtestingoperability(selected, model.actions));
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
				Core::FMToutputnode tempnode(output_node);
				const std::vector<FMTvertex_descriptor>verticies = getnode(model, tempnode, period);
				return getvariables(model, tempnode, verticies);
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
								const std::string value = dev.getmask().get(model.themes.at(themetarget));
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
		std::vector<FMTvertex_descriptor> getnode(const Models::FMTmodel& model, Core::FMToutputnode& output_node, int period) const
		{
			std::vector<FMTvertex_descriptor>locations;
			try {
				std::vector<int>targetedperiods;
				const int maxperiod = static_cast<int>(developments.size() - 2);
				const int node_period = output_node.settograph(targetedperiods, period, maxperiod);
				//*_logger << "node of " << node_period<<"dev size "<< developments.size() << "\n";
				if (node_period < 0)
				{
					return locations;
				}
				if (!output_node.source.isvariablelevel())
				{
					//constexpr size_t minimalcachedrop = 25;//10 %
					std::vector<const Core::FMTaction*> selected;
					//*_logger << "node " << std::string(output_node) << " period " << node_period<<" "<< targetedperiods.size() << "\n";
					if (isvalidouputnode(model, output_node, selected, node_period))
					{
						//*_logger << "valid node! " << "\n";
						
						if (nodescache.empty())
							{
							nodescache.reserve(developments.size());
							}
						//cleannodecaching();
						const bool gotstaticnode = model.isstaticnode(output_node);
						for (const int& localnodeperiod : targetedperiods)
						{
							std::vector<FMTvertex_descriptor> const* descriptors = nullptr;
							std::vector<FMTvertex_descriptor>staticdescriptors;
							bool exactverticies = false;
							
							if (gotstaticnode)
							{
								staticdescriptors = getnodebystaticmask(model, output_node, localnodeperiod);
								descriptors = &staticdescriptors;
							}else {
								while (nodescache.size() != developments.size())
								{
									const size_t devindex = nodescache.size();
									nodescache.emplace_back(developments.at(devindex).first, developments.at(devindex).second);
								}
								descriptors = &nodescache.at(localnodeperiod).getverticies(output_node, model.actions, model.themes, exactverticies);
							}
							//*_logger << "test? " << gotstaticnode << " " << exactverticies << "\n";
							if (exactverticies)
							{
								locations.reserve(locations.size() + descriptors->size());
								locations.insert(locations.end(), descriptors->begin(), descriptors->end());
							}else {
								std::vector<FMTvertex_descriptor>periodlocations;
								//*_logger << "testing on "<< descriptors->size()<<" "<< selected .size()<< "\n";
								
								for (const FMTvertex_descriptor& potential : *descriptors)
								{
									if (isvalidgraphnode(model, potential, output_node, selected))
									{
										locations.push_back(potential);
										periodlocations.push_back(potential);
									}
								}
								//*_logger << "done on " << periodlocations.size() << "\n";
							std::sort(periodlocations.begin(), periodlocations.end());
							if (!gotstaticnode)
								{
								nodescache.at(localnodeperiod).setvalidverticies(output_node, periodlocations);
								}
							}
						}


					}

				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getnode", __LINE__, __FILE__);
			}
			std::sort(locations.begin(), locations.end());
			//*_logger << "Output of getnode!" << "\n";
			return locations;
		}
		std::map<int, double> getvariables(const Models::FMTmodel& model, const Core::FMToutputnode& output_node,const std::vector<FMTvertex_descriptor>& verticies) const
		{
			return std::map<int, double>();	
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
		std::vector<int>getinvariables(const FMTvertex_descriptor& out_vertex) const
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
				_exhandler->raisefromcatch("", "FMTgraph::getinvariables", __LINE__, __FILE__);
			}
			return invars;
		}

		std::vector<double>getinproportions(const FMTvertex_descriptor& out_vertex) const
		{
			std::vector<double>inprops;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					const FMTbaseedgeproperties& edgeprop = data[*inedge_iterator];
					inprops.push_back(edgeprop.getproportion());

				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getinproportions", __LINE__, __FILE__);
			}
			return inprops;
		}

		std::map<int, int>getinidsvariables(const FMTvertex_descriptor& out_vertex) const
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
				_exhandler->raisefromcatch("", "FMTgraph::getinidsvariables", __LINE__, __FILE__);
			}
			return mapping;
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
				//*_logger << "get on" << getfirstperiod() << "\n";
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

		FMTgraphstats buildschedule(const Models::FMTmodel& model,
			std::queue<FMTvertex_descriptor> actives,
			const Core::FMTschedule& schedule)
		{
			FMTgraphstats statsdiff(stats);
			try {
				//developments.push_back(boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>());
				//developments.back().reserve(actives.size());
				const int actualperiod = getperiod();
				boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>> actualdevs = getdevsset(actualperiod);
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
								(!action.dorespectlock() && active_development.getlock() != 0 &&
								(schedule.at(action)).find(active_development.clearlock()) != (schedule.at(action)).end())))
							{
								const Graph::FMTgraphvertextoyield vertexinfo = getvertextoyieldinfo(model,front_vertex);
								if (active_development.operable(action, model.yields, &vertexinfo))
								{
									if (action.getname() == "_DEATH")
									{
										death = true;
									}
									const std::vector<Core::FMTdevelopmentpath> paths = active_development.operate(action, model.transitions[action_id], model.yields, model.themes);
									addaction(action_id, statsdiff, new_actives, front_vertex, paths, actualdevs);
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
				_exhandler->raisefromcatch("", "FMTgraph::buildschedule", __LINE__, __FILE__);
			}
			const bool typeIIforestmodel = (model.getparameter(Models::FMTintmodelparameters::MATRIX_TYPE) == 2);
			return naturalgrowth(actives, statsdiff,typeIIforestmodel);
		}



		FMTgraphstats eraseperiod(std::vector<int>& deletedconstraints,
			std::vector<int>&deletedvariables,
			bool keepbounded = false)
		{
			
			return getstats();
		}
		bool empty() const
		{
			return developments.empty();
		}

		std::queue<FMTvertex_descriptor> getactiveverticies() const
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
				_exhandler->raisefromcatch("", "FMTgraph::getactiveverticies", __LINE__, __FILE__);
			}
			return actives;
		}


		const FMTvertex_pair& getperiodverticies(int period) const
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
		void setconstraintID(const FMTvertex_descriptor& vertex, const int& id)
		{
			
		}
		bool gettransferrow(const FMTvertex_descriptor& vertex_descriptor,
			std::vector<int>&row_starts,
			std::vector<int>& cols,
			std::vector<double>& cols_value) const
		{
			return false;
		}
		void getinitialbounds(std::vector<double>& lower_bounds, std::vector<double>& upper_bounds) const
		{
			try {

				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = developments.at(getfirstactiveperiod()); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const FMTvertex_descriptor descriptor = *vertex_iterator;
					const FMTbasevertexproperties& vproperty = data[descriptor];
					const std::map<int, int>outs = getoutvariables(descriptor);
					lower_bounds[outs.at(-1)] = vproperty.getbaseRHS();
					upper_bounds[outs.at(-1)] = vproperty.getbaseRHS();
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

		std::vector<FMTvertex_descriptor>getnodebystaticmask(
			const Models::FMTmodel& model,
			const Core::FMToutputnode& node, int period) const
		{
			std::vector<FMTvertex_descriptor>descriptors;
			try {
				const Core::FMTmask staticmask = model.getstaticmask(node, true);
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end)=getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
					const Core::FMTdevelopment& dev = data[*vertex_iterator].get();
					if (dev.getmask().issubsetof(staticmask))
						{
						descriptors.push_back(*vertex_iterator);
						}
					}
				/*std::queue<FMTvertex_descriptor>activedescriptors;
				boost::unordered_set<FMTvertex_descriptor>explored;
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				const typename std::vector<FMTvertex_pair>::const_iterator firstconst = getfirstconstblock();
				vertex_iterator_end = firstconst->second;
				for (vertex_iterator = firstconst->first; vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const Core::FMTdevelopment& dev = data[*vertex_iterator].get();
					if (dev.getmask().issubsetof(staticmask))
						{
						activedescriptors.push(*vertex_iterator);
						explored.insert(*vertex_iterator);
						}
					}
				while (!activedescriptors.empty())
				{
					const FMTvertex_descriptor& active = activedescriptors.front();
					FMToutedge_iterator outit, outend;
					for (boost::tie(outit, outend) = boost::out_edges(active, data); outit != outend; ++outit)
						{
						const FMTvertex_descriptor gtarget = boost::target(*outit, data);
						if (explored.find(gtarget)==explored.end()&&data[gtarget].get().getperiod()<=period)
							{
							activedescriptors.push(gtarget);
							explored.insert(gtarget);
							}
						}
					const Core::FMTdevelopment& actdev = data[active].get();
					if (actdev.getperiod()==period)
						{
						descriptors.push_back(active);
						}
					activedescriptors.pop();
				}*/
			}catch (...)
				{
				_exhandler->raisefromcatch("For node: " + std::string(node), "FMTgraph::getnodebystaticmask", __LINE__, __FILE__);
				}
			return descriptors;
		}


		std::map<std::string, double> getsource(const Models::FMTmodel& model,
			const Core::FMToutputnode& node,
			int period, const Core::FMTtheme& theme,
			const double* solution, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const
		{
			std::map<std::string, double>emptyreturn;
			try{
			Core::FMToutputnode tempnode(node);
			const std::vector<FMTvertex_descriptor>verticies = getnode(model, tempnode, period);
			return getvalues(model, verticies, tempnode, theme, solution, level);
			}catch (...)
				{
				_exhandler->raisefromcatch("For node: "+std::string(node), "FMTgraph::getsource", __LINE__, __FILE__);
				}
			return emptyreturn;
		}

		Graph::FMTgraphvertextoyield getvertextoyieldinfo(const Models::FMTmodel& model,const FMTvertex_descriptor& descriptor) const
			{
			try {
				return Graph::FMTgraphvertextoyield(model,*this, reinterpret_cast<const void*>(&descriptor));
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::getvertextoyieldinfo", __LINE__, __FILE__);
				}
			return Graph::FMTgraphvertextoyield();
			}

		const FMTvertex_descriptor* getvertexfromvertexinfo(const Graph::FMTgraphvertextoyield* info) const
		{
			try {
				return reinterpret_cast<const FMTvertex_descriptor*>(info->getvertexptr());
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::getvertexfromvertexinfo", __LINE__, __FILE__);
			}
			return nullptr;
		}




		std::map<std::string, double> getvalues(const Models::FMTmodel& model, const std::vector<FMTvertex_descriptor>& verticies,
			const Core::FMToutputnode& node, const Core::FMTtheme& theme,
			const double* solution, Core::FMToutputlevel level) const
		{
			std::map<std::string, double>values;
			try {
				if (level == Core::FMToutputlevel::standard)
				{
					for (const std::string& attribute : theme.getbaseattributes())
					{
						values[attribute] = 0;
					}
				}
				else if (level == Core::FMToutputlevel::totalonly)
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
						const Graph::FMTgraphvertextoyield vertexinfo = getvertextoyieldinfo(model,vertex);
						std::string value;
						if (level == Core::FMToutputlevel::standard)
						{
							value = development.getmask().get(theme);
						}
						else if (level == Core::FMToutputlevel::developpement)
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
							Core::FMTdevelopment newdev(development);
							newdev.setperiod(newdev.getperiod() - 1);
							const double coef = node.source.getcoef(newdev, model.yields, &vertexinfo) * node.factor.getcoef(newdev, model.yields, &vertexinfo) * node.constant;
							double area = 0;
							if (development.getperiod() == 0)
							{
								area = outarea(vertex, -1, solution);
							}
							else {
								area = inarea(vertex, solution,!node.source.isaction());
							}
							values[value] += coef * area;
						}
						else {
							for (const Core::FMTaction* act : selected)
							{
								double action_value = 0;
								const int actionID = static_cast<int>(std::distance(&(*model.actions.begin()), act));
								const std::vector<Core::FMTdevelopmentpath>paths = getpaths(vertex, actionID);
								const double action_coef = node.source.getcoef(development, model.yields, &vertexinfo,&paths,act) * node.factor.getcoef(development, model.yields, &vertexinfo,&paths,act) * node.constant;
								action_value = action_coef * (outarea(vertex, actionID, solution));
								values[value] += action_value;
							}
						}

					}
				}
				if (level == Core::FMToutputlevel::standard)
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
				return (data[*vend].get().getperiod());
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::getperiod", __LINE__, __FILE__);
				}
			return 0;
		}

		void rebasecache()
		{
			try {
				for (size_t nodecacheid = 0; nodecacheid < nodescache.size(); ++nodecacheid)
				{
					nodescache[nodecacheid].rebase(developments.at(nodecacheid).first,developments.at(nodecacheid).second);
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::rebasecache", __LINE__, __FILE__);
			}
		}


        void generatedevelopments()
		{
			try {
				nodescache.clear();
				developments.clear();
				FMTvertex_iterator base_iterator, base_iterator_end;
				//End always stay the same use .end() for non valid period
				boost::tie(base_iterator, base_iterator_end) = boost::vertices(data);
				if (base_iterator!= base_iterator_end)
				{
					developments.resize(getperiod() + 1, FMTvertex_pair(base_iterator_end, base_iterator_end));
					FMTvertex_iterator vertex, vend;
					int actualperiod = 0;
					size_t vertexid = 0;
					FMTvertex_iterator firstvertex;
					for (boost::tie(vertex, vend) = boost::vertices(data); vertex != vend; ++vertex)
					{
						const FMTbasevertexproperties& properties = data[*vertex];
						const Core::FMTdevelopment& dev = properties.get();
						const size_t period_location = (dev.getperiod());
						if (vertexid == 0)
						{
							firstvertex = vertex;
							actualperiod = dev.getperiod();
						}
						if (actualperiod != dev.getperiod())//periodchanges!
						{
							developments[period_location - 1] = FMTvertex_pair(firstvertex, vertex);
							firstvertex = vertex;
							actualperiod = dev.getperiod();
						}
						++vertexid;
					}
					developments.back() = FMTvertex_pair(firstvertex, base_iterator_end);
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
					const FMTbaseedgeproperties& thisedgeprop = data[*thisedge_iterator];
					const FMTbaseedgeproperties& rhsedgeprop = rhs.data[*rhsedge_iterator];
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
						periodtolastdisturbance = data[vertexsource].get().getperiod();
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
						const int sourceperiod = data[sourcevertex].get().getperiod();
						if (inedgeproperties.getactionID()>=0)
							{
							lastactions.push_back(&inedgeproperties);
							distances.push_back(targetperiod - sourceperiod);
							}
						if (sourceperiod == 0 && lastactions.size() <= depth)
						{
							lastactions.push_back(nullptr);
							distances.push_back(targetperiod-1 + data[sourcevertex].get().getage());//targetperiod-1 because distance between age20 at p1 and p2 == 21, not 22
						}
					}
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::filluplastactions", __LINE__, __FILE__);
			}

		}



		size_t getamountofpaths(const Core::FMTdevelopment& dev, const int& actionid,const Models::FMTmodel& model,
								const boost::unordered_set<Core::FMTlookup<FMTvertex_descriptor, Core::FMTdevelopment>>& actualperioddevs) const
		{
			size_t amount = 0;
			try {
				std::vector<FMTvertex_descriptor>paths;
				if (actionid >= 0)
				{
					for (const Core::FMTdevelopmentpath& path : dev.operate(model.actions.at(actionid), model.transitions.at(actionid), model.yields, model.themes))
						{
						if (containsdevelopment(*path.development, actualperioddevs))
							{
							paths.push_back(getdevelopment(*path.development, actualperioddevs));
							}
						}
				}
				else {
					const FMTvertex_descriptor& act = getdevelopment(dev, actualperioddevs);
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
					int period = getdevelopment(path).getperiod();
					FMTvertex_descriptor vdescriptor = path;
					while (period < size())
					{
						std::map<int, int>vars = getoutvariables(vdescriptor);
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
				_exhandler->printexceptions("for " + std::string(dev), "FMTgraph::getamountofpaths", __LINE__, __FILE__);
			}
			return amount;
		}
		bool stayonserie(const FMTvertex_descriptor& targetdescriptor, const Core::FMTaction& action, const Models::FMTmodel& model) const
		{
			try {
				if (action.ispartofaserie())
					{
					const std::vector<std::string> targetserie = getactionserie(targetdescriptor, action, model);
					return action.isallowedinserie(targetserie);
					}
			}
			catch (...)
			{
				_exhandler->printexceptions("", "FMTgraph::stayonserie", __LINE__, __FILE__);
			}
			return false;
		}


		std::vector<std::string> getactionserie(FMTvertex_descriptor targetdescriptor,const Core::FMTaction& action, const Models::FMTmodel& model) const
			{
			std::vector<std::string>theserie;
			try {
				size_t inedgessize = boost::in_degree(targetdescriptor,data);
				const size_t maxactions = action.getlargestseriesize();
				theserie.reserve(maxactions);
				while (inedgessize>0 && theserie.size()<maxactions)
					{
					if (inedgessize > 0)
						{
						if (inedgessize > 1)
							{
							FMTinedge_iterator inedge_iterator, inedge_end;
							std::string actions;
							for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
							{
								const FMTbaseedgeproperties& inedgeproperties = data[*inedge_iterator];
								actions += std::to_string(inedgeproperties.getactionID())+" ";
							}
								const FMTbasevertexproperties& targetproperties = data[targetdescriptor];
								_exhandler->raise(Exception::FMTexc::FMTrangeerror,
									"Developement " + std::string(targetproperties.get()) + " operated by "+action.getname() + " has " + std::to_string(inedgessize) + " in edges ("+actions+")",
									"FMTgraph::getactionserie", __LINE__, __FILE__);
							}
						FMTinedge_iterator inedge_iterator, inedge_end;
						boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data);
						const FMTbaseedgeproperties& inedgeproperties = data[*inedge_iterator];
						const int actionid = inedgeproperties.getactionID();
						if (actionid >=0)
							{
							theserie.insert(theserie.begin(), model.actions.at(actionid).getname());
							}
						targetdescriptor = boost::source(*inedge_iterator, data);
						inedgessize = boost::in_degree(targetdescriptor, data);
						}
					}
			}catch (...)
				{
				_exhandler->printexceptions("", "FMTgraph::getactionserie", __LINE__, __FILE__);
				}
			return theserie;
			}


		std::vector<FMTpredictor> getpredictors(const FMTvertex_descriptor& targetdescriptor,const Models::FMTmodel& model,
			const std::vector<std::string>& yieldnames,const size_t& depth,bool periodonevalues= false, bool withGCBMid =true) const
			{
			std::vector<FMTpredictor> predictors;
			try {
				FMTinedge_iterator inedge_iterator, inedge_end;
				const FMTbasevertexproperties& targetproperties = data[targetdescriptor];
				const int targetperiod = targetproperties.get().getperiod();
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(targetdescriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
					std::vector<const FMTbaseedgeproperties*>lastactions;
					std::vector<int>distances;
					const FMTvertex_descriptor& sourcevertex = boost::source(*inedge_iterator, data);
					const FMTbasevertexproperties& sourceproperties = data[sourcevertex];
					const int sourceperiod = sourceproperties.get().getperiod();
					if (sourceperiod>0||periodonevalues)
					{
						lastactions.push_back(&data[*inedge_iterator]);
						distances.push_back(targetperiod-sourceperiod);
					}else{
						break;//if nothing happen in period one, it's not a predictor, because there is no change in yields and other things because P0 = begining of P1
					}
					filluplastactions(targetperiod, sourcevertex, lastactions, distances, depth);
					while (lastactions.size()<= depth)
						{
							lastactions.push_back(nullptr);
							distances.push_back(-1);
						}
					predictors.emplace_back(model.actions, yieldnames,model.yields, sourceproperties, targetproperties, lastactions, distances,withGCBMid);
				}
			predictors.shrink_to_fit();
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTgraph::getpredictors", __LINE__, __FILE__);
				}
			return predictors;
			}

		int getfirstperiod() const
			{
			return data[*developments.at(getfirstactiveperiod() + 1).first].get().getperiod();
			}
		void postsolve(const Core::FMTmaskfilter& filter,
			const std::vector<Core::FMTtheme>&originalbasethemes,
			const std::vector<int>& actionmapconnection)
		{
			try {
				developments.clear();
				nodescache.clear();//Some postsolve can be done here to keep some usefull information but for now just clear
				//start by remapping the actions
				FMTedge_iterator edge_iterator, edge_iterator_end;
				for (boost::tie(edge_iterator, edge_iterator_end) = boost::edges(data); edge_iterator != edge_iterator_end; ++edge_iterator)
				{
					FMTbaseedgeproperties& edgeprop = data[*edge_iterator];
					if (edgeprop.getactionID()>=0)
						{
						edgeprop.setactionID(actionmapconnection.at(edgeprop.getactionID()));
						}
				}
				boost::unordered_map<Core::FMTmask,Core::FMTmask>presolvetopostsolve;
				FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = boost::vertices(data); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					FMTbasevertexproperties& vertexprop = data[*vertex_iterator];
					const Core::FMTmask& presolvemask = vertexprop.get().getmask();
					boost::unordered_map<Core::FMTmask, Core::FMTmask>::const_iterator mskit = presolvetopostsolve.find(presolvemask);
					if (mskit != presolvetopostsolve.end())
						{
						vertexprop.setdevlopementmask(mskit->second);
					}else {
						const Core::FMTmask postsolvedmask = presolvemask.postsolve(filter, originalbasethemes);
						presolvetopostsolve[presolvemask] = postsolvedmask;
						vertexprop.setdevlopementmask(postsolvedmask);
						}
				}
				generatedevelopments();
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTgraph::postsolve", __LINE__, __FILE__);
			}
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
					FMTvertex_iterator vertex_iterator,vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = getperiodverticies(lperiod); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const FMTvertex_descriptor vertex = *vertex_iterator;
						std::map<int, int>variables = getoutvariables(vertex);
						variables.erase(-1);
						if (!variables.empty())
						{
							const Core::FMTdevelopment& dev = data[*vertex_iterator].get();
							for (const auto variable_iterator : variables)
							{
								if (*(actual_solution + variable_iterator.second) > FMT_DBL_TOLERANCE) //basis solution only!!!
								{
									
									/*if (schedule_solution.find(actions[variable_iterator.first]) == schedule_solution.end())
									{
										schedule_solution[actions[variable_iterator.first]] = std::map<Core::FMTdevelopment, std::map<int, double>>();
									}
									const Core::FMTdevelopment& basedev = getdevelopment(deviterator.memoryobject);*/
									newschedule.addevent(dev, *(actual_solution + variable_iterator.second), actions.at(variable_iterator.first));
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
		Core::FMTschedule getoutvariablesproportions(const std::vector<Core::FMTaction>& actions, const double* actual_solution, const int& lperiod,bool withlock = false) const
		{
			Core::FMTschedule newschedule(lperiod,*this, withlock);
			try {
				if (static_cast<int>(size()) > lperiod && lperiod > 0)
				{
					FMTvertex_iterator vertex_iterator,vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = getperiodverticies(lperiod); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const FMTvertex_descriptor vertex = *vertex_iterator;
						std::map<int, int>variables = getoutvariables(vertex);
						const Core::FMTdevelopment& dev = data[*vertex_iterator].get();
						double outarea = 0;
						std::map<int,double>variablesarea;
						for (const auto variable_iterator : variables)
						{
							const double vout = *(actual_solution + variable_iterator.second);
							/*if(vout>0)
							{
								std::cout<<std::string(dev)+" Variable : "+std::to_string(variable_iterator.first)+" Outarea : "+std::to_string(vout)<<std::endl;
							}*/
							if (vout > FMT_DBL_TOLERANCE) //basis solution only!!!
							{
								if(variable_iterator.first>=0)
								{
									variablesarea[variable_iterator.first]=vout;
								}
								outarea+=vout;
							}
						}
						//std::cout<<"Total area : "+std::to_string(outarea)<<std::endl;
						for (const auto variable_iterator : variablesarea) 
						{
							newschedule.addevent(dev, variable_iterator.second/outarea, actions.at(variable_iterator.first));
							//std::cout<<std::string(dev)+" Variable : "+std::to_string(variable_iterator.first)+" Proportion : "+std::to_string(variable_iterator.second/outarea)<<std::endl;
						}
					}
					newschedule.clean();
				}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("at period " + std::to_string(lperiod), "FMTgraph::getproportionsschedule", __LINE__, __FILE__);
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



template<> inline double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::inarea(const FMTvertex_descriptor& out_vertex, const double*&  solution, int actionid , bool growth) const
	{
		
		double area = 0;
		try {
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				const FMTedgeproperties& edgeprop = data[*inedge_iterator];
				if (edgeprop.getactionID() == actionid || !growth)
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

template<> inline std::map<int, double> FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getvariables(const Models::FMTmodel& model, const Core::FMToutputnode& output_node, const std::vector<FMTvertex_descriptor>& verticies) const
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
				const Graph::FMTgraphvertextoyield vertexinfo = getvertextoyieldinfo(model,vertex);
				if (output_node.source.useinedges())
				{
					Core::FMTdevelopment newdev(development);
					newdev.setperiod(newdev.getperiod() - 1);
					const double coef = output_node.source.getcoef(newdev, model.yields, &vertexinfo) * output_node.factor.getcoef(newdev, model.yields, &vertexinfo) * output_node.constant;
					if (development.getperiod() == 0)
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
							if (actionid < 0 || output_node.source.isaction())
							{
								updatevarsmap(variables, edgeprop.getvariableID(), (edgeprop.getproportion() / 100)*coef);
								continue;
							}
							const FMTvertex_descriptor sourceverex = boost::source(*inedge_iterator, data);
							const FMTvertexproperties& sourceproperties = data[sourceverex];
							if ((sourceproperties.get().getperiod() == development.getperiod() && !periodstart(sourceverex)))
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
							const double action_coef = output_node.source.getcoef(development, model.yields, &vertexinfo, &paths, act) * output_node.factor.getcoef(development, model.yields, &vertexinfo, &paths, act) * output_node.constant;
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

template<> inline FMTgraphstats FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::eraseperiod(std::vector<int>& deletedconstraints,
	std::vector<int>&deletedvariables,
	bool keepbounded)
{
	try {
		typename std::vector<FMTvertex_pair>::iterator periodit = this->getfirstblock();
		FMTvertex_iterator vertexit, vertexend;
		for (boost::tie(vertexit, vertexend) = *periodit; vertexit != vertexend; ++vertexit)
		{
			const FMTvertex_descriptor& vertex_location = *vertexit;
			FMTinedge_iterator inedge_iterator, inedge_end;
			bool gotinedges = false;
			const bool nottransferrow = isnotransfer(vertex_location, 1);

			if (!nottransferrow)
			{
				for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_location, data); inedge_iterator != inedge_end; ++inedge_iterator)
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
				}
				boost::clear_out_edges(vertex_location, data);
			}
			boost::clear_in_edges(vertex_location, data);
		}
		FMTvertex_iterator firstvertex;
		FMTvertex_iterator lastvertex;
		bool assigned = false;
		for (boost::tie(vertexit, vertexend) = *periodit; vertexit != vertexend; ++vertexit)
		{
			const FMTvertex_descriptor& vertex_location = *vertexit;
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
		rebasecache();
		const int firstperiod = this->getfirstactiveperiod();
		/*if (static_cast<size_t>(firstperiod) < nodescache.size())
		{
			nodescache[firstperiod].clear();
		}*/
		nodescache.clear();

	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTgraph::eraseperiod", __LINE__, __FILE__);
	}
	return stats;
}

template<> inline void FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::setconstraintID(const FMTvertex_descriptor& vertex, const int& id)
{
	data[vertex].setconstraintID(id);
}

template<> inline bool FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::gettransferrow(const FMTvertex_descriptor& vertex_descriptor,
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

template<> inline double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getinproportion(const FMTvertex_descriptor& vertex_descriptor) const
{
	try {
		FMTinedge_iterator inedge_iterator, inedge_end;
		FMTvertexproperties vertex_property = data[vertex_descriptor];
		for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(vertex_descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
		{
			const FMTedgeproperties& edgeprop = data[*inedge_iterator];
			return edgeprop.getproportion();
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTgraph::getinproportion", __LINE__, __FILE__);
	}
	return 1;
}



template<> inline void FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::updatematrixindex(
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

