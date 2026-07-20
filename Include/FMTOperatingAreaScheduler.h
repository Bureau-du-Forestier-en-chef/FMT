/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingareascheduler_Hm_included
#define FMToperatingareascheduler_Hm_included

#include "FMTMask.h"
#include "FMTOperatingAreaScheme.h"
#include "FMTLpHeuristic.h"




#include <random>
#include <memory>
#include <map>
#include <vector>
//#include <chrono>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>


namespace Graph
{
	template<class T1, class T2>
	class FMTGraph;
}

namespace Models
{
	class FMTModel;
}

namespace Core
{
	class FMTOutputNode;
	class FMTTimeYieldHandler;
	class FMTOutput;
}


namespace Heuristics
{
	// DocString: FMTOperatingAreaScheduler
	/**
	@brief Heuristic to solve the spatialization problem across a landscape for multiple operating areas.
	@details MIP forest planning problems tend to be symmetrical and hard to solve with a regular MIP solver; this heuristic quickly generates a good enough starting solution. Before using this class the user must have set all the parameters of a vector of FMTOperatingArea so the heuristic can set the constraints and variables of each operating area into the matrix.
	*/
	class FMTEXPORT FMTOperatingAreaScheduler : public FMTLpHeuristic
	{
		// DocString: FMToperatingareaheuristic::save
		/**
		@brief Serialize the FMTOperatingAreaScheduler for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("lpsolve", boost::serialization::base_object<FMTLpHeuristic>(*this));
			ar & BOOST_SERIALIZATION_NVP(operatingareas);
			ar & BOOST_SERIALIZATION_NVP(adjacencyconstraints);
			ar & BOOST_SERIALIZATION_NVP(proportionofset);
			ar & BOOST_SERIALIZATION_NVP(userandomness);
			ar & BOOST_SERIALIZATION_NVP(useprimal);
		}
		// DocString: FMTOperatingAreaScheduler::operatingareas
		///Vector of FMTOperatingArea holding all informations about each operating area coming from the user
		std::vector<FMTOperatingAreaScheme>operatingareas;
		// DocString: FMTOperatingAreaScheduler::adjacencyconstraints
		///Adjacency constraints indexes of all operating area the pair is used to not duplicate neighboring constraints
		std::map<std::pair<Core::FMTMask, Core::FMTMask>, std::vector<int>>adjacencyconstraints;
		// DocString: FMTOperatingAreaScheduler::proportionofset
		///This data member gives the information about the proportion of operating area status change allowed before resolving the problem
		double proportionofset;
		// DocString: FMTOperatingAreaScheduler::userandomness
		///If true the heuristic is going to shuffle the potential oprating area schemes and area schemes before fixing the bounds.
		bool userandomness;
		// DocString: FMTOperatingAreaScheduler::useprimal
		///If true the heuristic will solve try to solve the problem by the primal variables else by the dual constraints.
		bool useprimal;
		// DocString: FMTOperatingAreaScheduler::setOperatingAreasConstraints
		/**
		@brief Set all the operating area constraints and add all the constraints and variables to the solver interface using the graph, the parent model and an output node.
		@details The output node must be linked to at least one action of the model.
		@param[in] maingraph the main graph.
		@param[in] model the model.
		@param[in] target the output node.
		*/
		void setOperatingAreasConstraints(const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& maingraph,
			const Models::FMTModel& model,
			const Core::FMTOutputNode& target);
		// DocString: FMTOperatingAreaScheduler::setAdjacencyConstraints
		/**
		@brief Fill the adjacency constraints member and add these constraints to the solver interface.
		@details Adjacency constraints cannot be added from each operating area so they are linked directly to the heuristic and the operating area.
		@return true if the adjacency constraints are set else false.
		*/
		bool setAdjacencyConstraints();
		// DocString: FMTOperatingAreaScheduler::setDraw
		/**
		@brief Return the potential operating areas that have no schedule set yet, based on the dual or primal solution.
		@details When randomness is disabled the draw is sorted starting from the best operating area, otherwise it is shuffled.
		@return the potential operating areas.
		*/
		std::vector<std::vector<FMTOperatingAreaScheme>::const_iterator> setDraw();
		// DocString: FMTOperatingAreaScheduler::draw
		/**
		@brief Return the selected operating areas from a vector of potential ones.
		@details When randomness is disabled the first elements are selected up to the maximum number, otherwise the draw is shuffled.
		@param[in] oparea the potential operating areas.
		@return the selected operating areas.
		*/
		std::vector<std::vector<FMTOperatingAreaScheme>::const_iterator> draw(std::vector<std::vector<FMTOperatingAreaScheme>::const_iterator>& oparea);
		// DocString: FMTOperatingAreaScheduler::setBounds
		/**
		@brief Set a random schedule or the best known schedule to a set of operating areas, depending on whether randomness is used.
		@details If schemestoskip is not empty, for each operating area at the same index it tries to use another scheme id; it must be the same size as tobound or empty.
		@param[in] tobound the operating areas to bound.
		@param[in] schemestoskip the scheme ids to skip.
		@param[in] keeploose if true keeps the bounds loose.
		@return the number of bounded operating areas.
		*/
		size_t setBounds(const std::vector<std::vector<FMTOperatingAreaScheme>::const_iterator>& tobound,const std::vector<int>& schemestoskip=std::vector<int>(), bool keeploose=true);
		// DocString: FMTOperatingAreaScheduler::selectScheme
		/**
		@brief Select a scheme in the potential schemes that is different from the scheme id to skip.
		@param[in,out] potentialschemes the potential schemes.
		@param[in] schemetoskip the scheme id to skip.
		@return the selected scheme id.
		*/
		size_t selectScheme(std::vector<size_t>& potentialschemes, const int& schemetoskip) const;
		// DocString: FMTOperatingAreaScheduler::getBounds
		/**
		@brief Fill the targeted items and bounds according to a scheme id.
		@details If boundall is true no scheme id is needed; otherwise the default scheme id is 0 and the user must pass the correct one.
		@param[in] operatingareaiterator the operating area iterator.
		@param[in,out] targeteditems the targeted items.
		@param[in,out] bounds the bounds.
		@param[in] boundall if true bounds all the schemes.
		@param[in] schemeid the scheme id.
		@param[in] keeploose if true keeps the bounds loose.
		@return true if the bounds are filled else false.
		*/
		bool getBounds(const std::vector<FMTOperatingAreaScheme>::const_iterator& operatingareaiterator,
			std::vector<int>& targeteditems,std::vector<double>& bounds, const bool& boundall, const size_t& schemeid=0,bool keeploose=true) const;
		// DocString: FMTOperatingAreaScheduler::unboundAll
		/**
		@brief Unselect all the schedules already selected and unbound all the variables and constraints related to the operating areas in the solver interface.
		@param[in] atprimal if true unbounds at the primal.
		*/
		void unboundAll(bool atprimal = false);
		// DocString: FMTOperatingAreaScheduler::unbound
		/**
		@brief Unselect the schedules of the given operating areas and unbound their variables and constraints in the solver interface.
		@param[in] tounbound the operating areas to unbound.
		@param[in] atprimal if true unbounds at the primal.
		*/
		void unbound(const std::vector<std::vector<FMTOperatingAreaScheme>::const_iterator>& tounbound,bool atprimal = false);
		// DocString: FMTOperatingAreaScheduler::closePrimalBounds
		/**
		@brief Set all the binary variables of all the operating areas to 0 by fixing their primal bounds.
		*/
		void closePrimalBounds();
		// DocString: FMTOperatingAreaScheduler::setAllInteger
		/**
		@brief Set all the scheme variables of each operating area as integer variables in the solver interface and prepare the matrix for branch and bound when the solve was in dual.
		*/
		void setAllInteger();
		// DocString: FMTOperatingAreaScheduler::resolveModel
		/**
		@brief Call solverinterface->resolve, using different algorithms or parameters depending on the solver type for a fast resolve.
		@return the number of iterations.
		*/
		int resolveModel();
		// DocString: FMTOperatingAreaScheduler::clearRowCache
		/**
		@brief Clear the row memory cache, useful when the solver type is Mosek.
		*/
		void clearRowCache();
		// DocString: FMTOperatingAreaScheduler::getSolutionIndexes
		/**
		@brief Return the bounded scheme index of each operating area in the same order they are passed.
		@param[in] opareaits the operating area iterators.
		@return the bounded scheme indexes.
		*/
		std::vector<int> getSolutionIndexes(const std::vector<std::vector<FMTOperatingAreaScheme>::const_iterator>& opareaits) const;
		// DocString: FMTOperatingAreaScheduler::getThreadId
		/**
		@brief Return the thread id to log it in greedyPass and initialSolve.
		@return the thread id.
		*/
		std::string getThreadId() const;
		// DocString: FMTOperatingAreaScheduler::completeInitialSolution
		/**
		@brief Complete the solution by getting the binaries and constraints of each operating area and adjacency, completing the old solution with binaries of 0 and completing the row price of each added constraint.
		@return true if the solution is completed else false.
		*/
		bool completeInitialSolution();
		// DocString: FMTOperatingAreaScheduler::gotRejectedNodes
		/**
		@brief Return true if there are rejected nodes.
		@return true if there are rejected nodes else false.
		*/
		bool gotRejectedNodes() const;
		// DocString: FMTOperatingAreaScheduler::resetBaseGreedySolution
		/**
		@brief Replace the newly generated solution by the old one and get back to the base state of the scheduler after a greedy iteration with no gain.
		@param[in] selected the selected operating areas.
		@param[in] oldschemeid the old scheme ids.
		@param[in] oldconstraints the old constraints.
		@param[in] oldbounds the old bounds.
		*/
		void resetBaseGreedySolution(const std::vector<std::vector<FMTOperatingAreaScheme>::const_iterator>& selected,
									const std::vector<int>& oldschemeid,
									const std::vector<int>& oldconstraints,
									const std::vector<double>& oldbounds);
		// DocString: FMTOperatingAreaScheduler::releaseGreedySolution
		/**
		@brief Get the bounds and constraints of the selected operating areas before a greedy move, release the bounds and resolve the model.
		@param[in] selected the selected operating areas.
		@param[in,out] oldconstraints the old constraints.
		@param[in,out] oldbounds the old bounds.
		@return true if the model is optimal else false.
		*/
		bool releaseGreedySolution(const std::vector<std::vector<FMTOperatingAreaScheme>::const_iterator>& selected,
			std::vector<int>& oldconstraints,
			std::vector<double>& oldbounds);
		// DocString: FMTOperatingAreaScheduler::greedyMoveSolution
		/**
		@brief Place a new solution using the selected operating areas and the tabou scheme ids to skip, then solve the model.
		@param[in] selected the selected operating areas.
		@param[in] oldschemeid the tabou scheme ids to skip.
		@return true if the model is optimal else false.
		*/
		bool greedyMoveSolution(const std::vector<std::vector<FMTOperatingAreaScheme>::const_iterator>& selected,
								const std::vector<int>& oldschemeid);
		// DocString: FMTOperatingAreaScheduler::setForDualInitialSolve
		/**
		@brief Release the right hand sides and close the primal bounds to get some activities, then solve to prepare for a fresh initialSolve.
		*/
		void setForDualInitialSolve();
		// DocString: FMTOperatingAreaScheduler::initialMoveSolution
		/**
		@brief Bound the newly selected operating areas and resolve the model.
		@param[in] selected the selected operating areas.
		@return the number of iterations.
		*/
		int initialMoveSolution(const std::vector<std::vector<FMTOperatingAreaScheme>::const_iterator>& selected);
		// DocString: FMTOperatingAreaScheduler::resetBaseInitialSolution
		/**
		@brief Get back to the base form of the formulation by releasing all the bounds and resolving when setting the initial solution fails.
		*/
		void resetBaseInitialSolution();
	public:
		// DocString: FMTOperatingAreaScheduler::generateInitialProportionOfSet
		/**
		@brief Compute a good proportion of set for the initialSolve, defaulting to 0.25 and decreasing linearly with the scheme complexity to a minimum of 0.01.
		@return the initial proportion of set.
		*/
		double generateInitialProportionOfSet() const;
		// DocString: FMTOperatingAreaScheduler::empty
		/**
		@brief Return true if the operating areas are empty.
		@return true if the operating areas are empty else false.
		*/
		bool empty() const;
		// DocString: FMTOperatingAreaScheduler::initialSolve
		/**
		@brief Solve the heuristic problem until an initial solution is found for each operating area.
		@return true if an initial solution is found else false.
		*/
		bool initialSolve() final;
		// DocString: FMTOperatingAreaScheduler::branchNBoundSolve
		/**
		@brief Solve the problem using branch and bound on the primal formulation, using the heuristic solution as a starting MIP solution when initialSolve has been called.
		@return true if a solution is found else false.
		*/
		bool branchNBoundSolve() final;
		// DocString: FMTOperatingAreaScheduler::greedyPass
		/**
		@brief Do a greedy pass to try to improve the solution.
		@param[in] initsol the initial solution value.
		@param[in] iteration the iteration.
		@return true if the pass improved the solution else false.
		*/
		bool greedyPass(const double& initsol, const unsigned int& iteration) final;
		// DocString: FMTOperatingAreaScheduler::setAsRandom
		/**
		@brief Set the randomness member to true.
		*/
		void setAsRandom();
		// DocString: FMTOperatingAreaScheduler::setAsPrimal
		/**
		@brief Set the primal member to true.
		*/
		void setAsPrimal();
		// DocString: FMTOperatingAreaScheduler::setProportionOfSet
		/**
		@brief Set the allowed proportion of operating area status change before resolving the problem.
		@param[in] proportion the proportion of set.
		*/
		void setProportionOfSet(const double& proportion);
		// DocString: FMTOperatingAreaScheduler::getSolution
		/**
		@brief Return the actual solution as a vector of time yield handlers, one per operating area.
		@details If a selected mask and base themes are given the returned yield handlers are postsolved.
		@param[in] yldname the yield name to use.
		@return the yield handlers.
		*/
		std::vector<Core::FMTTimeYieldHandler> getSolution(	const std::string& yldname) const;
		// DocString: FMTOperatingAreaScheduler::getLevelSolution
		/**
		@brief Return the lower bounds of the solution for each operating area as two outputs: the level and the variable output to bound to.
		@param[in] outputname the output name.
		@param[in] aggregate the aggregate.
		@param[in] outputid the output id.
		@return the level solution outputs.
		*/
		std::vector<Core::FMTOutput>getLevelSolution(const std::string& outputname,const std::string& aggregate,int outputid) const;
		// DocString: FMTOperatingAreaScheduler(const std::vector<FMTOperatingArea>,const Graph::FMTGraph,const Models::FMTModel,const Core::FMTOutputNode,std::shared_ptr<OsiSolverInterface>,const Models::FMTsolverinterface,size_t lseed,double proportionofset,bool userandomness,bool copysolver)
		/**
		@brief Main constructor for FMTOperatingAreaScheduler, taking the information coming from a FMTLpModel.
		@details Constructing the scheduler copies the solver interface when copysolver is true; after construction the scheduler no longer needs the FMTLpModel.
		@param[in] loperatingareas the operating areas.
		@param[in] maingraph the main graph.
		@param[in] model the model.
		@param[in] target the output node.
		@param[in,out] basesolve the base solver.
		@param[in] lseed the seed.
		@param[in] proportionofset the proportion of set.
		@param[in] userandomness if true uses randomness.
		@param[in] copysolver if true copies the solver.
		*/
		FMTOperatingAreaScheduler(const std::vector<FMTOperatingAreaScheme>& loperatingareas,
			const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& maingraph,
			const Models::FMTModel& model,
			const Core::FMTOutputNode& target,
			Models::FMTLpSolver& basesolve, size_t lseed = 0,
			double proportionofset = 0.25, bool userandomness = false, bool copysolver = true);
		// DocString: FMTOperatingAreaScheduler()
		/**
		@brief Default constructor for FMTOperatingAreaScheduler.
		*/
		FMTOperatingAreaScheduler()=default;
		// DocString: FMTOperatingAreaScheduler(const FMToperatingareaheuristic&)
		/**
		@brief Copy constructor for FMTOperatingAreaScheduler.
		@param[in] rhs the FMTOperatingAreaScheduler to copy.
		*/
		FMTOperatingAreaScheduler(const FMTOperatingAreaScheduler& rhs);
		// DocString: FMTOperatingAreaScheduler(const FMToperatingareaheuristic&&)
		/**
		@brief Move constructor for FMTOperatingAreaScheduler.
		@param[in,out] rhs the FMTOperatingAreaScheduler to move from.
		*/
		FMTOperatingAreaScheduler(FMTOperatingAreaScheduler&& rhs)=default;
		// DocString: FMTOperatingAreaScheduler::operator=
		/**
		@brief Copy assignment operator for FMTOperatingAreaScheduler.
		@param[in] rhs the FMTOperatingAreaScheduler to copy.
		@return a reference to this FMTOperatingAreaScheduler.
		*/
		FMTOperatingAreaScheduler& operator = (const FMTOperatingAreaScheduler& rhs);
		// DocString: ~FMTOperatingAreaScheduler()
		/**
		@brief Destructor for FMTOperatingAreaScheduler.
		*/
		~FMTOperatingAreaScheduler();
	};
}
BOOST_CLASS_EXPORT_KEY(Heuristics::FMTOperatingAreaScheduler)
#endif
#endif
