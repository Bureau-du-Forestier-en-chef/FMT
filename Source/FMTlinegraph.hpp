/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTLINEGRAPH_H
#define FMTLINEGRAPH_H

#include "FMTgraph.hpp"
#include <boost/serialization/export.hpp>

namespace Spatial
{
	class FMTeventcontainer;
}

namespace Graph
{
// DocString: FMTlinegraph
/**
FMTlinegraph is a simple graph  where edges - verticies -1. Each actions occuring in the graph
generate only one new vertex like that 0----0----0----0. This graph is used with FMTlayer to
simulate the preocess of growth and havest of a single forest stand where each action produce only
one new stade. This class is heavely used in the FMTsesmodel and FMTsamodel and is normaly used with
spatialy explicit models.
*/
class FMTEXPORT FMTlinegraph : public FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("Graph::FMTgraph<Graph::FMTbasevertexproperties,Graph::FMTbaseedgeproperties>",boost::serialization::base_object<FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>>(*this));

	}
	// DocString: FMTlinegraph::isanyactionofedge
	/**
	Returns true if the action of the edgte is in the unordered map.
	*/
	bool isanyactionofedge(const FMTbaseedgeproperties& edgeproperty,const std::unordered_set<int>& actions) const;
	// DocString: FMTlinegraph::isanyvertexusage
	/**
	Returns true The vertex is in the outputsource based on (yields).
	*/
	bool isanyvertexusage(const FMTbasevertexproperties& vertexproperty, const Core::FMToutputsource& source, const Core::FMTyields& yields) const;
    public:
		// DocString: FMTlinegraph()
		/**
			Default constructor of FMTlinegraph.
		*/
        FMTlinegraph();
		// DocString: ~FMTlinegraph()
		/**
			Default desctructor of FMTlinegraph.
		*/
        ~FMTlinegraph()=default;
		// DocString: FMTlinegraph(const FMTgraphbuild)
		/**
			Constructor for the linegraph, can be scheudle build or full build.
		*/
        FMTlinegraph(const FMTgraphbuild lbuildtype);
		// DocString: FMTlinegraph(const FMTlinegraph&)
		/**
			Copy constructor of the FMTlinegraph.
		*/
        FMTlinegraph(const FMTlinegraph& rhs);
		// DocString: FMTlinegraph(const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>&)
		/**
			Copy constructor with the parent class FMTgraph.
		*/
        FMTlinegraph(const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>& rhs);
		// DocString: FMTlinegraph::operator=
		/**
		Copy assignment of FMTlinegraph
		*/
        FMTlinegraph& operator = (const FMTlinegraph& rhs);
		// DocString: FMTlinegraph::operator=
		/**
		Copy assignment with parent FMTgraph
		*/
        FMTlinegraph& operator = (const FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>& rhs);
		// DocString: FMTlinegraph::newperiod
		/**
		Generate a new period container at the end of the linegraph.
		*/
		//void newperiod();
		// DocString: FMTlinegraph::setaction
		/**
		Add an edge action based on paths on an active vertex.
		*/
		void setaction(	FMTvertex_descriptor active,const int& actionID,
						const std::vector<Core::FMTdevelopmentpath>& paths);
		// DocString: FMTlinegraph::operate
		/**
		Operate the last active vertex and return the length of the generated developpementpath.
		*/
		size_t operate(	const Core::FMTaction& action, const int& action_id, const Core::FMTtransition& transition, 
														const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes);
		// DocString: FMTlinegraph::grow
		/**
		Grow the last active vertex of the linegraph generating new edge and vertex.
		*/
		void grow();
		// DocString: FMTlinegraph::getactivevertex
		/**
		Returns the last active vertex of the graph.
		*/
		FMTvertex_descriptor getactivevertex() const;
		// DocString: FMTlinegraph::getlastactionid
		/**
		Get the last action id of the last edge of the (period) of the graph.
		*/
		int getlastactionid(const int& period)const;
		// DocString: FMTlinegraph::getperiodcarbonpredictors
		/**
		Returns a vector of carbon predictors for a given (period), (actionsmap), (yieldnames) and yields.
		*/
		std::vector<FMTcarbonpredictor>getperiodcarbonpredictors(const int& period, const Models::FMTmodel& model, const std::vector<std::string>& yieldnames,bool withGCBMid = true) const;
		// DocString: FMTlinegraph::getinedgeactionid
		/**
		Get the in edge action id of a vertex.
		*/
		int getinedgeactionid(const FMTvertex_descriptor& vdesc)const;
		// DocString: FMTlinegraph::getperiodstartdev
		/**
		Get a reference to the first development(vertex) seen in the (period).
		*/
		const Core::FMTdevelopment& getperiodstartdev(const int& period) const;
		// DocString: FMTlinegraph::getperiodstopdev
		/**
		Get a reference to the last development(vertex) seen in the (period).
		*/
		const Core::FMTdevelopment& getperiodstopdev(const int& period) const;
		// DocString: FMTlinegraph::randomoperate
		/**
		Randomly operate an (active _developement) (front_vertex) and returns the action id.
		*/
		int randomoperate(const std::vector<int>& operables, const Models::FMTmodel& model,
                                            FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
                                            const Core::FMTdevelopment& active_development,bool dontchoosegrow=false);
		// DocString: FMTlinegraph::randombuildperiod
		/**
		Randomly operate the active vertex to completybuild the active period of the graph.
		Returns the action id of the new edges generated.
		*/
		std::vector<int> randombuildperiod(const Models::FMTmodel& model,std::default_random_engine& generator,
									boost::unordered_map<Core::FMTdevelopment, std::vector<int>>& operability,
									bool dontchoosegrow=false);
		// DocString: FMTlinegraph::clearfromperiod
		/**
		This function will remove every vertex and edge present in the graph starting from this (period).
		if updatedevelopements is true the map holding the developements location will be updated.
		*/
        FMTgraphstats clearfromperiod(const int& period,bool updatedevelopments=false);
		// DocString: FMTlinegraph::partialcopy
		/**
		This function will copy all verticies and edges of the graph from period = 0 to (period).
		*/
        FMTlinegraph partialcopy(const int& period) const;
		// DocString: FMTlinegraph::getactions
		/**
		Starting (fromperiod) the function will return the a vector of of vector of bool if true the action is operable
		at the specified period of the vector.
		*/
		std::vector<std::vector<bool>>getactions(const Models::FMTmodel& model,const int& fromperiod,
			std::map<Core::FMTdevelopment, std::vector<bool>>& operability) const;
		// DocString: FMTlinegraph::getbasedevelopment
		/**
		Returns a reference to the first developement of the graph.
		*/
		const Core::FMTdevelopment& getbasedevelopment() const;
		// DocString: FMTlinegraph::getbasehash
		/**
		Returnsa base hash of the starting developement of the linegraph based on a dynamicmask filter.
		*/
		size_t getbasehash(const Core::FMTmask& dynamicmask) const;
		// DocString:FMTlinegraph::getbasestr
		/**
		Returns a base string of the starting developement of the linegraph based on a dynamicmask filter.
		*/
		std::string getbasestr(const Core::FMTmask& dynamicmask) const;
		// DocString: FMTlinegraph::getbasemask
		/**
		Returns a filtered mask of the starting developement mask of the linegraph based on a dynamicmask filter.
		*/
		Core::FMTmask getbasemask(const Core::FMTmask& dynamicmask) const;
		// DocString: FMTlinegraph::getbasemask
		/**
		Returns a filtered mask of the starting developement mask of the linegraph based on a dynamicmask filter.
		*/
		size_t getedgeshash(const int& maximalperiod,bool& gotthewhole) const;
		// DocString: FMTlinegraph::getedgesstr
		/**
		Returns a string representing the edges of the linegraph from 0 to period (maximalperiod)
		*/
		std::string getedgesstr(const int& maximalperiod, bool& gotthewhole) const;
		// DocString: FMTlinegraph::filledgesmask
		/**
		Fill up a unique mask based on the edges from period 0 to (maximalperiod).
		*/
		void filledgesmask(Core::FMTmask& mask, const int& maximalperiod) const;
		// DocString: FMTlinegraph::stringforconstraint
		/**
		Fill a string for uniqueness of the FMTlinegraph.
		*/
		bool stringforconstraint(std::string& value, const int& stop, const Core::FMTmask& dynamicmask) const;
		// DocString: FMTlinegraph::hashforconstraint
		/**
		Fill a hash for uniqueness of the FMTlinegraph.
		*/
		bool hashforconstraint(size_t& hashvalue,const int& stop,const Core::FMTmask& dynamicmask) const;
		// DocString: FMTlinegraph::isonlygrow
		/**
		Returns true if the graph is never operated by any actions.
		*/
		bool isonlygrow(int period=0) const;
		// DocString:FMTlinegraph::addfromevents
		/**
		Add actions (edges) + (verticies) to the graph based an event.
		*/
		void addfromevents(const Spatial::FMTcoordinate& localisation,const Models::FMTmodel& model, Spatial::FMTeventcontainer& events) const;
		// DocString: FMTlinegraph::ismovable
		/**
		Returns true if the graph can be modified at (period) and operated by different action or can be turned into natural grow.
		*/
		bool ismovable(const std::vector<const Core::FMTaction*>& actions,
			const Core::FMTyields& yields, const int& period, boost::unordered_map<Core::FMTdevelopment, bool>*operability =nullptr) const;
		// DocString:  FMTlinegraph::operator==
		/**
		Comparison operator of FMTlinegraph
		*/
		bool operator == (const FMTlinegraph& rhs) const;
		// DocString:  FMTlinegraph::clearnodecache
		/**
		Clear the node cache when getting outpus of the linegraph.
		*/
		void clearnodecache();
		// DocString:  FMTlinegraph::anyusageof
		/**
		Returns the period at which the vertex fall within the outputnode description. 
		*/
		std::vector<int> anyusageof(Core::FMToutputnode output_node, const Models::FMTmodel& model, const int& startingperiod) const;
		
		
};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTlinegraph)

#endif // FMTLINEGRAPH_H
