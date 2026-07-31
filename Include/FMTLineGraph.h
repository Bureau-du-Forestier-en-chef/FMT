/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTLINEGRAPH_H
#define FMTLINEGRAPH_H

#include "FMTGraph.hpp"
#include <boost/serialization/export.hpp>

namespace Spatial
{
	class FMTEventContainer;
}

namespace Graph
{
// DocString: FMTLineGraph
/**
@brief Simple graph where each action generates a single new vertex (0----0----0----0), used with FMTLayer to simulate the growth and harvest of a single forest stand.
@details Heavily used in FMTSesModel and FMTSaModel and normally used with spatially explicit models.
*/
class FMTEXPORT FMTLineGraph : public FMTGraph<FMTBaseVertexProperties,FMTBaseEdgeProperties>
{
    public:
		// DocString: FMTLineGraph()
		/**
		@brief Default constructor for FMTLineGraph.
		*/
        FMTLineGraph();
		// DocString: ~FMTLineGraph()
		/**
		@brief Default destructor for FMTLineGraph.
		*/
        ~FMTLineGraph()=default;
		// DocString: FMTLineGraph(const FMTgraphbuild)
		/**
		@brief Construct a FMTLineGraph reserving memory for a given length.
		@param[in] p_LengthReserve the size to reserve.
		*/
        FMTLineGraph(size_t p_LengthReserve);
		// DocString: FMTLineGraph(const Core::FMTActualDevelopment&,size_t )
		/**
		@brief Construct a FMTLineGraph from a base development, reserving memory for a given length.
		@param[in] p_LengthReserve the size to reserve.
		@param[in] p_dev the base development.
		*/
		FMTLineGraph(size_t p_LengthReserve, const Core::FMTActualDevelopment& p_dev);
		// DocString: FMTLineGraph(const FMTLineGraph&)
		/**
		@brief Copy constructor for FMTLineGraph.
		@param[in] rhs the FMTLineGraph to copy.
		*/
        FMTLineGraph(const FMTLineGraph& rhs);
		// DocString: FMTLineGraph(const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>&)
		/**
		@brief Construct a FMTLineGraph from its parent FMTGraph.
		@param[in] rhs the parent graph.
		*/
        FMTLineGraph(const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>& rhs);
		// DocString: FMTLineGraph::operator=
		/**
		@brief Copy assignment operator for FMTLineGraph.
		@param[in] rhs the FMTLineGraph to copy.
		@return a reference to this FMTLineGraph.
		*/
        FMTLineGraph& operator = (const FMTLineGraph& rhs);
		// DocString: FMTLineGraph::operator=
		/**
		@brief Copy assignment operator from the parent FMTGraph.
		@param[in] rhs the parent graph.
		@return a reference to this FMTLineGraph.
		*/
        FMTLineGraph& operator = (const FMTGraph<FMTBaseVertexProperties,FMTBaseEdgeProperties>& rhs);
		// DocString: FMTLineGraph::newPeriod
		/**
		@brief Generate a new period container at the end of the line graph.
		*/
		//void newPeriod();
		// DocString: FMTLineGraph::setAction
		/**
		@brief Add an action edge based on paths on an active vertex.
		@param[in] active the active vertex.
		@param[in] actionID the action id.
		@param[in] paths the development paths.
		*/
		void setAction(	FMTvertex_descriptor active,const int& actionID,
						const std::vector<Core::FMTDevelopmentPath>& paths);
		// DocString: FMTLineGraph::operate
		/**
		@brief Operate the last active vertex and return the length of the generated development path.
		@param[in] action the action.
		@param[in] action_id the action id.
		@param[in] transition the transition.
		@param[in] ylds the yields.
		@param[in] themes the themes.
		@return the length of the generated development path.
		*/
		size_t operate(	const Core::FMTAction& action, const int& action_id, const Core::FMTTransition& transition, 
														const Core::FMTYields& ylds, const std::vector<Core::FMTTheme>& themes);
		// DocString: FMTLineGraph::grow
		/**
		@brief Grow the last active vertex of the line graph, generating a new edge and vertex.
		@param[in] p_Length the number of periods to grow.
		*/
		void grow(int p_Length=1);
		// DocString: FMTLineGraph::getActiveVertex
		/**
		@brief Return the last active vertex of the graph.
		@return the active vertex.
		*/
		FMTvertex_descriptor getActiveVertex() const;
		// DocString: FMTLineGraph::getLastActionId
		/**
		@brief Return the action id of the last edge of a period.
		@param[in] period the period.
		@return the last action id.
		*/
		int getLastActionId(const int& period)const;
		// DocString: FMTLineGraph::getPeriodActionIds
		/**
		@brief Return the action ids of every action done in a period, in order.
		@param[in] period the period.
		@param[in] p_ordered if true returns them ordered.
		@return the action ids.
		*/
		std::vector<int> getPeriodActionIds(const int& period, bool p_ordered = true)const;
		// DocString: FMTLineGraph::getPeriodPredictors
		/**
		@brief Return the predictors of a period.
		@param[in] period the period.
		@param[in] model the model.
		@param[in] yieldnames the yield names.
		@param[in] periodonevalues if true uses the period one values.
		@param[in] withGCBMid if true uses the GCBM id.
		@return the predictors.
		*/
		std::vector<FMTPredictor>getPeriodPredictors(const int& period, const Models::FMTModel& model, const std::vector<std::string>& yieldnames,bool periodonevalues =false,bool withGCBMid = true) const;
		// DocString: FMTLineGraph::getInEdgeActionId
		/**
		@brief Return the in edge action id of a vertex.
		@param[in] vdesc the vertex descriptor.
		@return the in edge action id.
		*/
		int getInEdgeActionId(const FMTvertex_descriptor& vdesc)const;
		// DocString: FMTLineGraph::getPeriodStartDev
		/**
		@brief Return a reference to the first development (vertex) of a period.
		@param[in] period the period.
		@return the first development.
		*/
		const Core::FMTDevelopment& getPeriodStartDev(const int& period) const;
		// DocString: FMTLineGraph::getPeriodStopDev
		/**
		@brief Return a reference to the last development (vertex) of a period.
		@param[in] period the period.
		@return the last development.
		*/
		const Core::FMTDevelopment& getPeriodStopDev(const int& period) const;
		// DocString: FMTLineGraph::randomBuildPeriod
		/**
		@brief Randomly operate the active vertex to completely build the active period of the graph.
		@param[in] model the model.
		@param[in,out] generator the random generator.
		@param[in,out] operability the operability cache.
		@param[in] dontchoosegrow if true does not choose the grow action.
		@return the action ids of the new edges.
		*/
		std::vector<int> randomBuildPeriod(const Models::FMTModel& model,std::default_random_engine& generator,
									boost::unordered_map<Core::FMTDevelopment, std::vector<int>>& operability,
									bool dontchoosegrow=false);
		// DocString: FMTLineGraph::getActions
		/**
		@brief Return, starting from a period, a vector of booleans per period indicating whether each action is operable.
		@param[in] model the model.
		@param[in] fromperiod the first period.
		@param[in,out] operability the operability cache.
		@return the operable actions per period.
		*/
		std::vector<std::vector<bool>>getActions(const Models::FMTModel& model,const int& fromperiod,
			std::map<Core::FMTDevelopment, std::vector<bool>>& operability) const;
		// DocString: FMTLineGraph::getBaseDevelopment
		/**
		@brief Return a reference to the first development of the graph.
		@return the base development.
		*/
		const Core::FMTDevelopment& getBaseDevelopment() const;
		// DocString: FMTLineGraph::getBaseHash
		/**
		@brief Return a hash of the starting development of the line graph based on a dynamic mask filter.
		@param[in] dynamicmask the dynamic mask.
		@return the base hash.
		*/
		size_t getBaseHash(const Core::FMTMask& dynamicmask) const;
		// DocString: FMTLineGraph::getBaseStr
		/**
		@brief Return a string of the starting development of the line graph based on a dynamic mask filter.
		@param[in] dynamicmask the dynamic mask.
		@return the base string.
		*/
		std::string getBaseStr(const Core::FMTMask& dynamicmask) const;
		// DocString: FMTLineGraph::getBaseMask
		/**
		@brief Return a filtered mask of the starting development mask of the line graph based on a dynamic mask filter.
		@param[in] dynamicmask the dynamic mask.
		@return the base mask.
		*/
		Core::FMTMask getBaseMask(const Core::FMTMask& dynamicmask) const;
		// DocString: FMTLineGraph::setBaseMask
		/**
		@brief Set the base mask to the dynamic mask by intersection for a period.
		@param[in,out] p_dynamicmask the dynamic mask to intersect.
		@param[in] p_period the period.
		*/
		void setBaseMask(Core::FMTMask& p_dynamicmask,const int& p_period) const;
		// DocString: FMTLineGraph::getEdgesHash
		/**
		@brief Return a hash of the edges of the line graph from period 0 to a maximal period.
		@param[in] maximalperiod the maximal period.
		@param[out] gotthewhole true if the whole graph was hashed.
		@return the edges hash.
		*/
		size_t getEdgesHash(const int& maximalperiod,bool& gotthewhole) const;
		// DocString: FMTLineGraph::getEdgesStr
		/**
		@brief Return a string representing the edges of the line graph from period 0 to a maximal period.
		@param[in] maximalperiod the maximal period.
		@param[out] gotthewhole true if the whole graph was represented.
		@return the edges string.
		*/
		std::string getEdgesStr(const int& maximalperiod, bool& gotthewhole) const;
		// DocString: FMTLineGraph::fillEdgesMask
		/**
		@brief Fill a unique mask based on the edges from period 0 to a maximal period.
		@param[in,out] mask the mask to fill.
		@param[in] maximalperiod the maximal period.
		*/
		void fillEdgesMask(Core::FMTMask& mask, const int& maximalperiod) const;
		// DocString: FMTLineGraph::stringForConstraint
		/**
		@brief Fill a string used for the uniqueness of the line graph.
		@param[in,out] value the string to fill.
		@param[in] stop the stop period.
		@param[in] dynamicmask the dynamic mask.
		@return true if the whole graph was represented else false.
		*/
		bool stringForConstraint(std::string& value, const int& stop, const Core::FMTMask& dynamicmask) const;
		// DocString: FMTLineGraph::hashForConstraint
		/**
		@brief Fill a hash used for the uniqueness of the line graph.
		@param[in,out] hashvalue the hash to fill.
		@param[in] stop the stop period.
		@param[in] dynamicmask the dynamic mask.
		@return true if the whole graph was hashed else false.
		*/
		bool hashForConstraint(size_t& hashvalue,const int& stop,const Core::FMTMask& dynamicmask) const;
		// DocString: FMTLineGraph::isOnlyGrow
		/**
		@brief Return true if the graph is never operated by any action.
		@param[in] period the period.
		@return true if the graph only grows else false.
		*/
		bool isOnlyGrow(int period=0) const;
		// DocString: FMTLineGraph::addFromEvents
		/**
		@brief Add actions (edges) and vertices to the graph based on an event.
		*/
		//void addFromEvents(const Spatial::FMTCoordinate& localisation,const Models::FMTModel& model, Spatial::FMTEventContainer& events) const;
		// DocString: FMTLineGraph::ismovable
		/**
		@brief Return true if the graph can be modified at a period, operated by a different action or turned into natural growth.
		@param[in] p_model the model.
		@param[in] period the period.
		@param[in,out] p_operability the operability cache.
		@return true if the graph is movable else false.
		*/
		bool isMovable(const Models::FMTModel& p_model, const int& period,
			boost::unordered_map<Core::FMTDevelopment, std::vector<int>>&p_operability) const;
		// DocString:  FMTLineGraph::operator==
		/**
		@brief Comparison operator for FMTLineGraph.
		@param[in] rhs the FMTLineGraph to compare to.
		@return true if both line graphs are equal else false.
		*/
		bool operator == (const FMTLineGraph& rhs) const;
		// DocString:  FMTLineGraph::operator<
		/**
		@brief Less than operator for FMTLineGraph.
		@param[in] rhs the FMTLineGraph to compare to.
		@return true if this line graph is less than the other else false.
		*/
		bool operator < (const FMTLineGraph& rhs) const;
		// DocString:  FMTLineGraph::isLessPeriod
		/**
		@brief Return true if the other graph is the same graph with one less period.
		@param[in] rhs the other graph.
		@return true if it is the same graph with one less period else false.
		*/
		bool isLessPeriod(const FMTLineGraph& rhs) const;
		// DocString:  FMTLineGraph::isSameBase
		/**
		@brief Return true if the other graph has the same base development.
		@param[in] rhs the other graph.
		@return true if the base developments are the same else false.
		*/
		bool isSameBase(const FMTLineGraph& rhs) const;
		// DocString:  FMTLineGraph::isNotDead
		/**
		@brief Return true if a vertex descriptor did not get the death action.
		@param[in] p_Descriptor the vertex descriptor.
		@param[in] p_DeathId the death action id.
		@return true if the vertex is not dead else false.
		*/
		bool isNotDead(FMTvertex_descriptor p_Descriptor,int8_t p_DeathId) const;
		// DocString:  FMTLineGraph::clearNodeCache
		/**
		@brief Clear the node cache used when getting outputs of the line graph.
		*/
		void clearNodeCache();
		// DocString:  FMTLineGraph::anyUsageOf
		/**
		@brief Return the periods at which the vertices fall within an output node description.
		@param[in] output_node the output node.
		@param[in] model the model.
		@param[in] startingperiod the starting period.
		@return the periods of usage.
		*/
		std::vector<int> anyUsageOf(Core::FMTOutputNode output_node, const Models::FMTModel& model, const int& startingperiod) const;
		// DocString: FMTLineGraph::getSetOperability
		/**
		@brief Return the operable action ids of a development, computing and caching them if needed.
		@param[in] p_development the development.
		@param[in] p_model the model.
		@param[in,out] p_Cache the operability cache.
		@return the operable action ids.
		*/
		static const std::vector<int>& getSetOperability(
			const Core::FMTDevelopment& p_development,
			const Models::FMTModel& p_model,
			boost::unordered_map<Core::FMTDevelopment, std::vector<int>>& p_Cache);
		private:
			friend class boost::serialization::access;
			// DocString: FMTLineGraph::serialize
			/**
			@brief Serialize the FMTLineGraph through its base FMTGraph for multiprocessing across multiple cpus (pickle in Python).
			@tparam Archive the archive type.
			@param[in,out] ar the archive to serialize to or from.
			@param[in] version the serialization version.
			*/
			template<class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& boost::serialization::make_nvp("Graph::FMTgraph<Graph::FMTbasevertexproperties,Graph::FMTbaseedgeproperties>", boost::serialization::base_object<FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>>(*this));

			}
			// DocString: FMTLineGraph::isAnyActionOfEdge
			/**
			@brief Return true if the action of an edge is in a set of actions.
			@param[in] edgeproperty the edge properties.
			@param[in] actions the set of actions.
			@return true if the edge action is in the set else false.
			*/
			bool isAnyActionOfEdge(const FMTBaseEdgeProperties& edgeproperty, const std::unordered_set<int>& actions) const;
			// DocString: FMTLineGraph::isAnyVertexUsage
			/**
			@brief Return true if a vertex is in an output source based on the yields.
			@param[in] vertexproperty the vertex properties.
			@param[in] source the output source.
			@param[in] yields the yields.
			@return true if the vertex is used else false.
			*/
			bool isAnyVertexUsage(const FMTBaseVertexProperties& vertexproperty, const Core::FMTOutputSource& source, const Core::FMTYields& yields) const;
			// DocString: FMTLineGraph::_randomOperate
			/**
			@brief Randomly operate an active development at the front vertex and return the action id.
			@param[in] operables the operable action ids.
			@param[in] model the model.
			@param[in,out] front_vertex the front vertex.
			@param[in,out] generator the random generator.
			@param[in] active_development the active development.
			@param[in] dontchoosegrow if true does not choose the grow action.
			@return the operated action id.
			*/
			int _randomOperate(const std::vector<int>& operables, const Models::FMTModel& model,
				FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
				const Core::FMTDevelopment& active_development, bool dontchoosegrow = false);
			
		
};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTLineGraph)

#endif // FMTLINEGRAPH_H
