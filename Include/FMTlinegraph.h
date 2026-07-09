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
		@brief Constructor for the linegraph, can be scheudle build or full build.
		@param[in] p_LengthReserve the size to reserve
		*/
        FMTlinegraph(size_t p_LengthReserve);
		// DocString: FMTlinegraph(const Core::FMTactualdevelopment&,size_t )
		/**
		@brief Construct line graph
		@param[in] p_LengthReserve the size to reserve
		@param[in] p_dev the base dev.
		*/
		FMTlinegraph(size_t p_LengthReserve, const Core::FMTactualdevelopment& p_dev);
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
		// DocString: FMTlinegraph::setAction
		/**
		Add an edge action based on paths on an active vertex.
		*/
		void setAction(	FMTvertex_descriptor active,const int& actionID,
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
		void grow(int p_Length=1);
		// DocString: FMTlinegraph::getActiveVertex
		/**
		Returns the last active vertex of the graph.
		*/
		FMTvertex_descriptor getActiveVertex() const;
		// DocString: FMTlinegraph::getLastActionId
		/**
		Get the last action id of the last edge of the (period) of the graph.
		*/
		int getLastActionId(const int& period)const;
		// DocString: FMTlinegraph::getPeriodActionIds
		/**
		Get the action id of every action done in period in order.
		*/
		std::vector<int> getPeriodActionIds(const int& period, bool p_ordered = true)const;
		// DocString: FMTlinegraph::getPeriodPredictors
		/**
		Returns a vector of predictors for a given (period), (actionsmap), (yieldnames) and yields.
		*/
		std::vector<FMTpredictor>getPeriodPredictors(const int& period, const Models::FMTmodel& model, const std::vector<std::string>& yieldnames,bool periodonevalues =false,bool withGCBMid = true) const;
		// DocString: FMTlinegraph::getInEdgeActionId
		/**
		Get the in edge action id of a vertex.
		*/
		int getInEdgeActionId(const FMTvertex_descriptor& vdesc)const;
		// DocString: FMTlinegraph::getPeriodStartDev
		/**
		Get a reference to the first development(vertex) seen in the (period).
		*/
		const Core::FMTdevelopment& getPeriodStartDev(const int& period) const;
		// DocString: FMTlinegraph::getPeriodStopDev
		/**
		Get a reference to the last development(vertex) seen in the (period).
		*/
		const Core::FMTdevelopment& getPeriodStopDev(const int& period) const;
		// DocString: FMTlinegraph::randomBuildPeriod
		/**
		Randomly operate the active vertex to completybuild the active period of the graph.
		Returns the action id of the new edges generated.
		*/
		std::vector<int> randomBuildPeriod(const Models::FMTmodel& model,std::default_random_engine& generator,
									boost::unordered_map<Core::FMTdevelopment, std::vector<int>>& operability,
									bool dontchoosegrow=false);
		// DocString: FMTlinegraph::getactions
		/**
		Starting (fromperiod) the function will return the a vector of of vector of bool if true the action is operable
		at the specified period of the vector.
		*/
		std::vector<std::vector<bool>>getactions(const Models::FMTmodel& model,const int& fromperiod,
			std::map<Core::FMTdevelopment, std::vector<bool>>& operability) const;
		// DocString: FMTlinegraph::getBaseDevelopment
		/**
		Returns a reference to the first developement of the graph.
		*/
		const Core::FMTdevelopment& getBaseDevelopment() const;
		// DocString: FMTlinegraph::getBaseHash
		/**
		Returnsa base hash of the starting developement of the linegraph based on a dynamicmask filter.
		*/
		size_t getBaseHash(const Core::FMTmask& dynamicmask) const;
		// DocString:FMTlinegraph::getBaseStr
		/**
		Returns a base string of the starting developement of the linegraph based on a dynamicmask filter.
		*/
		std::string getBaseStr(const Core::FMTmask& dynamicmask) const;
		// DocString: FMTlinegraph::getBaseMask
		/**
		Returns a filtered mask of the starting developement mask of the linegraph based on a dynamicmask filter.
		*/
		Core::FMTmask getBaseMask(const Core::FMTmask& dynamicmask) const;
		// DocString: FMTlinegraph::setBaseMask
		/**
		@brief set the base mask to the dynamic mask by intersect.
		@param[in] p_dynamicmask the dynamic mask to intersect.
		@param[in] p_period the period to which we want to get a dynamic mask.
		*/
		void setBaseMask(Core::FMTmask& p_dynamicmask,const int& p_period) const;
		// DocString: FMTlinegraph::getBaseMask
		/**
		Returns a filtered mask of the starting developement mask of the linegraph based on a dynamicmask filter.
		*/
		size_t getEdgesHash(const int& maximalperiod,bool& gotthewhole) const;
		// DocString: FMTlinegraph::getEdgesStr
		/**
		Returns a string representing the edges of the linegraph from 0 to period (maximalperiod)
		*/
		std::string getEdgesStr(const int& maximalperiod, bool& gotthewhole) const;
		// DocString: FMTlinegraph::fillEdgesMask
		/**
		Fill up a unique mask based on the edges from period 0 to (maximalperiod).
		*/
		void fillEdgesMask(Core::FMTmask& mask, const int& maximalperiod) const;
		// DocString: FMTlinegraph::stringForConstraint
		/**
		Fill a string for uniqueness of the FMTlinegraph.
		*/
		bool stringForConstraint(std::string& value, const int& stop, const Core::FMTmask& dynamicmask) const;
		// DocString: FMTlinegraph::hashForConstraint
		/**
		Fill a hash for uniqueness of the FMTlinegraph.
		*/
		bool hashForConstraint(size_t& hashvalue,const int& stop,const Core::FMTmask& dynamicmask) const;
		// DocString: FMTlinegraph::isOnlyGrow
		/**
		Returns true if the graph is never operated by any actions.
		*/
		bool isOnlyGrow(int period=0) const;
		// DocString:FMTlinegraph::addfromevents
		/**
		Add actions (edges) + (verticies) to the graph based an event.
		*/
		//void addfromevents(const Spatial::FMTcoordinate& localisation,const Models::FMTmodel& model, Spatial::FMTeventcontainer& events) const;
		// DocString: FMTlinegraph::ismovable
		/**
		Returns true if the graph can be modified at (period) and operated by different action or can be turned into natural grow.
		*/
		bool isMovable(const Models::FMTmodel& p_model, const int& period,
			boost::unordered_map<Core::FMTdevelopment, std::vector<int>>&p_operability) const;
		// DocString:  FMTlinegraph::operator==
		/**
		Comparison operator of FMTlinegraph
		*/
		bool operator == (const FMTlinegraph& rhs) const;
		// DocString:  FMTlinegraph::operator==
		/**
		Comparison operator of FMTlinegraph
		*/
		bool operator < (const FMTlinegraph& rhs) const;
		// DocString:  FMTlinegraph::isLessPeriod
		/**
		@brief check if rhs is the same graph with one less period
		@return true if same graph with one less period.
		*/
		bool isLessPeriod(const FMTlinegraph& rhs) const;
		// DocString:  FMTlinegraph::isSameBase
		/**
		@brief Check if same base dev
		@return true if same
		*/
		bool isSameBase(const FMTlinegraph& rhs) const;
		// DocString:  FMTlinegraph::isNotDead
		/**
		@brief Check if descriptor got the _death action
		@param[in] vertex descriptor  p_Descriptor
		@param[in] p_DeathId death action id
		@return true if not dead
		*/
		bool isNotDead(FMTvertex_descriptor p_Descriptor,int8_t p_DeathId) const;
		// DocString:  FMTlinegraph::clearNodeCache
		/**
		Clear the node cache when getting outpus of the linegraph.
		*/
		void clearNodeCache();
		// DocString:  FMTlinegraph::anyUsageOf
		/**
		Returns the period at which the vertex fall within the outputnode description. 
		*/
		std::vector<int> anyUsageOf(Core::FMToutputnode output_node, const Models::FMTmodel& model, const int& startingperiod) const;
		static const std::vector<int>& getSetOperability(
			const Core::FMTdevelopment& p_development,
			const Models::FMTmodel& p_model,
			boost::unordered_map<Core::FMTdevelopment, std::vector<int>>& p_Cache);
		private:
			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& boost::serialization::make_nvp("Graph::FMTgraph<Graph::FMTbasevertexproperties,Graph::FMTbaseedgeproperties>", boost::serialization::base_object<FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>>(*this));

			}
			// DocString: FMTlinegraph::isAnyActionOfEdge
			/**
			Returns true if the action of the edgte is in the unordered map.
			*/
			bool isAnyActionOfEdge(const FMTbaseedgeproperties& edgeproperty, const std::unordered_set<int>& actions) const;
			// DocString: FMTlinegraph::isAnyVertexUsage
			/**
			Returns true The vertex is in the outputsource based on (yields).
			*/
			bool isAnyVertexUsage(const FMTbasevertexproperties& vertexproperty, const Core::FMToutputsource& source, const Core::FMTyields& yields) const;
			// DocString: FMTlinegraph::_randomOperate
			/**
			Randomly operate an (active _developement) (front_vertex) and returns the action id.
			*/
			int _randomOperate(const std::vector<int>& operables, const Models::FMTmodel& model,
				FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
				const Core::FMTdevelopment& active_development, bool dontchoosegrow = false);
			
		
};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTlinegraph)

#endif // FMTLINEGRAPH_H
