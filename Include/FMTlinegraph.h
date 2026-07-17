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
	class FMTEventContainer;
}

namespace Graph
{
// DocString: FMTLineGraph
/**
FMTLineGraph is a simple graph  where edges - vertices -1. Each actions occuring in the graph
generate only one new vertex like that 0----0----0----0. This graph is used with FMTLayer to
simulate the preocess of growth and havest of a single forest stand where each action produce only
one new stade. This class is heavely used in the FMTSesModel and FMTSaModel and is normaly used with
spatially explicit models.
*/
class FMTEXPORT FMTLineGraph : public FMTGraph<FMTBaseVertexProperties,FMTBaseEdgeProperties>
{
    public:
		// DocString: FMTLineGraph()
		/**
			Default constructor of FMTLineGraph.
		*/
        FMTLineGraph();
		// DocString: ~FMTLineGraph()
		/**
			Default desctructor of FMTLineGraph.
		*/
        ~FMTLineGraph()=default;
		// DocString: FMTLineGraph(const FMTgraphbuild)
		/**
		@brief Constructor for the linegraph, can be scheudle build or full build.
		@param[in] p_LengthReserve the size to reserve
		*/
        FMTLineGraph(size_t p_LengthReserve);
		// DocString: FMTLineGraph(const Core::FMTActualDevelopment&,size_t )
		/**
		@brief Construct line graph
		@param[in] p_LengthReserve the size to reserve
		@param[in] p_dev the base dev.
		*/
		FMTLineGraph(size_t p_LengthReserve, const Core::FMTActualDevelopment& p_dev);
		// DocString: FMTLineGraph(const FMTLineGraph&)
		/**
			Copy constructor of the FMTLineGraph.
		*/
        FMTLineGraph(const FMTLineGraph& rhs);
		// DocString: FMTLineGraph(const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>&)
		/**
			Copy constructor with the parent class FMTGraph.
		*/
        FMTLineGraph(const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>& rhs);
		// DocString: FMTLineGraph::operator=
		/**
		Copy assignment of FMTLineGraph
		*/
        FMTLineGraph& operator = (const FMTLineGraph& rhs);
		// DocString: FMTLineGraph::operator=
		/**
		Copy assignment with parent FMTGraph
		*/
        FMTLineGraph& operator = (const FMTGraph<FMTBaseVertexProperties,FMTBaseEdgeProperties>& rhs);
		// DocString: FMTLineGraph::newperiod
		/**
		Generate a new period container at the end of the linegraph.
		*/
		//void newperiod();
		// DocString: FMTLineGraph::setAction
		/**
		Add an edge action based on paths on an active vertex.
		*/
		void setAction(	FMTvertex_descriptor active,const int& actionID,
						const std::vector<Core::FMTDevelopmentPath>& paths);
		// DocString: FMTLineGraph::operate
		/**
		Operate the last active vertex and return the length of the generated developpementpath.
		*/
		size_t operate(	const Core::FMTAction& action, const int& action_id, const Core::FMTTransition& transition, 
														const Core::FMTYields& ylds, const std::vector<Core::FMTTheme>& themes);
		// DocString: FMTLineGraph::grow
		/**
		Grow the last active vertex of the linegraph generating new edge and vertex.
		*/
		void grow(int p_Length=1);
		// DocString: FMTLineGraph::getActiveVertex
		/**
		Returns the last active vertex of the graph.
		*/
		FMTvertex_descriptor getActiveVertex() const;
		// DocString: FMTLineGraph::getLastActionId
		/**
		Get the last action id of the last edge of the (period) of the graph.
		*/
		int getLastActionId(const int& period)const;
		// DocString: FMTLineGraph::getPeriodActionIds
		/**
		Get the action id of every action done in period in order.
		*/
		std::vector<int> getPeriodActionIds(const int& period, bool p_ordered = true)const;
		// DocString: FMTLineGraph::getPeriodPredictors
		/**
		Returns a vector of predictors for a given (period), (actionsmap), (yieldnames) and yields.
		*/
		std::vector<FMTPredictor>getPeriodPredictors(const int& period, const Models::FMTModel& model, const std::vector<std::string>& yieldnames,bool periodonevalues =false,bool withGCBMid = true) const;
		// DocString: FMTLineGraph::getInEdgeActionId
		/**
		Get the in edge action id of a vertex.
		*/
		int getInEdgeActionId(const FMTvertex_descriptor& vdesc)const;
		// DocString: FMTLineGraph::getPeriodStartDev
		/**
		Get a reference to the first development(vertex) seen in the (period).
		*/
		const Core::FMTDevelopment& getPeriodStartDev(const int& period) const;
		// DocString: FMTLineGraph::getPeriodStopDev
		/**
		Get a reference to the last development(vertex) seen in the (period).
		*/
		const Core::FMTDevelopment& getPeriodStopDev(const int& period) const;
		// DocString: FMTLineGraph::randomBuildPeriod
		/**
		Randomly operate the active vertex to completybuild the active period of the graph.
		Returns the action id of the new edges generated.
		*/
		std::vector<int> randomBuildPeriod(const Models::FMTModel& model,std::default_random_engine& generator,
									boost::unordered_map<Core::FMTDevelopment, std::vector<int>>& operability,
									bool dontchoosegrow=false);
		// DocString: FMTLineGraph::getactions
		/**
		Starting (fromperiod) the function will return the a vector of of vector of bool if true the action is operable
		at the specified period of the vector.
		*/
		std::vector<std::vector<bool>>getactions(const Models::FMTModel& model,const int& fromperiod,
			std::map<Core::FMTDevelopment, std::vector<bool>>& operability) const;
		// DocString: FMTLineGraph::getBaseDevelopment
		/**
		Returns a reference to the first development of the graph.
		*/
		const Core::FMTDevelopment& getBaseDevelopment() const;
		// DocString: FMTLineGraph::getBaseHash
		/**
		Returnsa base hash of the starting development of the linegraph based on a dynamicmask filter.
		*/
		size_t getBaseHash(const Core::FMTMask& dynamicmask) const;
		// DocString:FMTLineGraph::getBaseStr
		/**
		Returns a base string of the starting development of the linegraph based on a dynamicmask filter.
		*/
		std::string getBaseStr(const Core::FMTMask& dynamicmask) const;
		// DocString: FMTLineGraph::getBaseMask
		/**
		Returns a filtered mask of the starting development mask of the linegraph based on a dynamicmask filter.
		*/
		Core::FMTMask getBaseMask(const Core::FMTMask& dynamicmask) const;
		// DocString: FMTLineGraph::setBaseMask
		/**
		@brief set the base mask to the dynamic mask by intersect.
		@param[in] p_dynamicmask the dynamic mask to intersect.
		@param[in] p_period the period to which we want to get a dynamic mask.
		*/
		void setBaseMask(Core::FMTMask& p_dynamicmask,const int& p_period) const;
		// DocString: FMTLineGraph::getBaseMask
		/**
		Returns a filtered mask of the starting development mask of the linegraph based on a dynamicmask filter.
		*/
		size_t getEdgesHash(const int& maximalperiod,bool& gotthewhole) const;
		// DocString: FMTLineGraph::getEdgesStr
		/**
		Returns a string representing the edges of the linegraph from 0 to period (maximalperiod)
		*/
		std::string getEdgesStr(const int& maximalperiod, bool& gotthewhole) const;
		// DocString: FMTLineGraph::fillEdgesMask
		/**
		Fill up a unique mask based on the edges from period 0 to (maximalperiod).
		*/
		void fillEdgesMask(Core::FMTMask& mask, const int& maximalperiod) const;
		// DocString: FMTLineGraph::stringForConstraint
		/**
		Fill a string for uniqueness of the FMTLineGraph.
		*/
		bool stringForConstraint(std::string& value, const int& stop, const Core::FMTMask& dynamicmask) const;
		// DocString: FMTLineGraph::hashForConstraint
		/**
		Fill a hash for uniqueness of the FMTLineGraph.
		*/
		bool hashForConstraint(size_t& hashvalue,const int& stop,const Core::FMTMask& dynamicmask) const;
		// DocString: FMTLineGraph::isOnlyGrow
		/**
		Returns true if the graph is never operated by any actions.
		*/
		bool isOnlyGrow(int period=0) const;
		// DocString:FMTLineGraph::addfromevents
		/**
		Add actions (edges) + (vertices) to the graph based an event.
		*/
		//void addfromevents(const Spatial::FMTCoordinate& localisation,const Models::FMTModel& model, Spatial::FMTEventContainer& events) const;
		// DocString: FMTLineGraph::ismovable
		/**
		Returns true if the graph can be modified at (period) and operated by different action or can be turned into natural grow.
		*/
		bool isMovable(const Models::FMTModel& p_model, const int& period,
			boost::unordered_map<Core::FMTDevelopment, std::vector<int>>&p_operability) const;
		// DocString:  FMTLineGraph::operator==
		/**
		Comparison operator of FMTLineGraph
		*/
		bool operator == (const FMTLineGraph& rhs) const;
		// DocString:  FMTLineGraph::operator==
		/**
		Comparison operator of FMTLineGraph
		*/
		bool operator < (const FMTLineGraph& rhs) const;
		// DocString:  FMTLineGraph::isLessPeriod
		/**
		@brief check if rhs is the same graph with one less period
		@return true if same graph with one less period.
		*/
		bool isLessPeriod(const FMTLineGraph& rhs) const;
		// DocString:  FMTLineGraph::isSameBase
		/**
		@brief Check if same base dev
		@return true if same
		*/
		bool isSameBase(const FMTLineGraph& rhs) const;
		// DocString:  FMTLineGraph::isNotDead
		/**
		@brief Check if descriptor got the _death action
		@param[in] vertex descriptor  p_Descriptor
		@param[in] p_DeathId death action id
		@return true if not dead
		*/
		bool isNotDead(FMTvertex_descriptor p_Descriptor,int8_t p_DeathId) const;
		// DocString:  FMTLineGraph::clearNodeCache
		/**
		Clear the node cache when getting outpus of the linegraph.
		*/
		void clearNodeCache();
		// DocString:  FMTLineGraph::anyUsageOf
		/**
		Returns the period at which the vertex fall within the outputnode description. 
		*/
		std::vector<int> anyUsageOf(Core::FMTOutputNode output_node, const Models::FMTModel& model, const int& startingperiod) const;
		static const std::vector<int>& getSetOperability(
			const Core::FMTDevelopment& p_development,
			const Models::FMTModel& p_model,
			boost::unordered_map<Core::FMTDevelopment, std::vector<int>>& p_Cache);
		private:
			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& boost::serialization::make_nvp("Graph::FMTgraph<Graph::FMTbasevertexproperties,Graph::FMTbaseedgeproperties>", boost::serialization::base_object<FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>>(*this));

			}
			// DocString: FMTLineGraph::isAnyActionOfEdge
			/**
			Returns true if the action of the edgte is in the unordered map.
			*/
			bool isAnyActionOfEdge(const FMTBaseEdgeProperties& edgeproperty, const std::unordered_set<int>& actions) const;
			// DocString: FMTLineGraph::isAnyVertexUsage
			/**
			Returns true The vertex is in the outputsource based on (yields).
			*/
			bool isAnyVertexUsage(const FMTBaseVertexProperties& vertexproperty, const Core::FMTOutputSource& source, const Core::FMTYields& yields) const;
			// DocString: FMTLineGraph::_randomOperate
			/**
			Randomly operate an (active _development) (front_vertex) and returns the action id.
			*/
			int _randomOperate(const std::vector<int>& operables, const Models::FMTModel& model,
				FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
				const Core::FMTDevelopment& active_development, bool dontchoosegrow = false);
			
		
};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTLineGraph)

#endif // FMTLINEGRAPH_H
