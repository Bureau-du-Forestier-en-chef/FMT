/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#ifdef FMTWITHOSI

#ifndef FMTsrmodel_Hm_included
#define FMTsrmodel_Hm_included

#include "FMTModel.h"
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>
#include "FMTLpSolver.h"
#include "FMTVertexProperties.h"
#include "FMTEdgeProperties.h"
#include "FMTGraph.hpp"
#include <memory>

#if defined FMTWITHR
	#include <Rcpp.h>
#endif

namespace Core
{
	class FMTSerie;
}


namespace Graph
{
	class FMTGraphVertexToYield;

}

namespace Models 
	{
	// DocString: FMTSrModel
	/**
	@brief Base class holding the attributes and methods shared between the spatially referenced models (FMTLpModel and FMTNssModel).
	*/
	class FMTEXPORT FMTSrModel : public FMTModel
	{
	public:
		// DocString: FMTSrModel::setNodeCacheSize
		/**
		@brief Set the size of reserved memory used when caching output nodes during the build process and output requests.
		@param[in] p_size the size to reserve.
		*/
		void setNodeCacheSize(const size_t& p_size);
		// DocString: FMTSrModel::getGraphVertexToYield
		/**
		@brief Return a filled FMTGraphVertexToYield without a specific vertex selected.
		@details Useful for a generic yield request depending on the full graph and model (like FMTYieldModel).
		@return the graph vertex to yield.
		*/
		Graph::FMTGraphVertexToYield getGraphVertexToYield() const;
		// DocString: FMTSrModel::postSolve
		/**
		@brief Postsolve the presolved model back into the original model, also postsolving the FMTGraph of the FMTSrModel.
		@param[in] originalbasemodel the original base model.
		*/
		virtual void postSolve(const FMTModel& originalbasemodel);
		// DocString: FMTSrModel::setParallelLogger
		/**
		@brief Pass a logger owned by the thread to the solver so it does not work in concurrency.
		@details The solver logger cannot work in parallel.
		@param[in] logger the logger.
		*/
		void setParallelLogger(Logging::FMTLogger& logger) override;
		// DocString: FMTSrModel::getAvailableSolverInterface
		/**
		@brief Return the available solver interfaces.
		@return the available solver interfaces.
		*/
		static std::vector<Models::FMTSolverInterface> getAvailableSolverInterface();
		// DocString: FMTSrModel(const FMTModel,FMTSolverInterface)
		/**
		@brief Main constructor building a FMTSrModel from its base class and letting the user choose the solver type.
		@details See FMTSolverInterface.h for the supported solver types (CLP, MOSEK, GUROBI, CPLEX).
		@param[in] base the base model.
		@param[in] lsolvertype the solver type.
		*/
		FMTSrModel(const FMTModel& base, FMTSolverInterface lsolvertype);
		// DocString: FMTSrModel()
		/**
		@brief Default constructor for FMTSrModel.
		*/
		FMTSrModel();
		// DocString: FMTSrModel(const FMTSrModel)
		/**
		@brief Copy constructor for FMTSrModel.
		@param[in] rhs the FMTSrModel to copy.
		*/
		FMTSrModel(const FMTSrModel& rhs);
		// DocString: FMTSrModel(const FMTModel, const Graph::FMTGraph<Graph::FMTVertexProperties,Graph::FMTEdgeProperties>,	const FMTLpSolver)
		/**
		@brief Constructor for FMTSrModel mainly used in postSolve.
		@param[in] base the base model.
		@param[in] lgraph the graph.
		@param[in] lsolver the solver.
		*/
		FMTSrModel(FMTModel&& base,const Graph::FMTGraph<Graph::FMTVertexProperties,Graph::FMTEdgeProperties>& lgraph,
					const FMTLpSolver& lsolver);
		// DocString: FMTSrModel::operator=
		/**
		@brief Copy assignment operator for FMTSrModel.
		@param[in] rhs the FMTSrModel to copy.
		@return a reference to this FMTSrModel.
		*/
		FMTSrModel& operator = (const FMTSrModel& rhs);
		// DocString: FMTSrModel(FMTSrModel&&)
		/**
		@brief Default move constructor for FMTSrModel.
		@param[in,out] rhs the model to move from.
		*/
		FMTSrModel(FMTSrModel&& rhs) noexcept;
		// DocString: FMTSrModel::operator=(FMTSrModel&& rhs) 
		/**
		@brief Default move assignment for FMTSrModel.
		@param[in,out] rhs the model to move from.
		@return a reference to this FMTSrModel.
		*/
		FMTSrModel& operator =(FMTSrModel&& rhs) noexcept;
		// DocString: ~FMTSrModel()
		/**
		@brief Default destructor for FMTSrModel.
		*/
		~FMTSrModel() = default;
		// DocString: FMTSrModel::operator==
		/**
		@brief Comparison operator for FMTSrModel.
		@param[in] rhs the FMTSrModel to compare to.
		@return true if both models are equal else false.
		*/
		bool operator == (const FMTSrModel& rhs) const;
		// DocString: FMTSrModel::operator!=
		/**
		@brief Comparison operator for FMTSrModel.
		@param[in] rhs the FMTSrModel to compare to.
		@return true if both models are different else false.
		*/
		bool operator != (const FMTSrModel& rhs) const;
		// DocString: FMTSrModel::unboundSolution
		/**
		@brief Unbound the primal bounds of a given period.
		@param[in] period the period.
		@return true if the solution is unbounded else false.
		*/
		bool unboundSolution(int period);
		// DocString: FMTSrModel::isPeriodBounded
		/**
		@brief Check if the FMTDevelopment areas are bounded on their primal variables for a given period.
		@param[in] period the period.
		@return true if the period is bounded else false.
		*/
		bool isPeriodBounded(int period) const;
		// DocString: FMTSrModel::clearGraphDevelopments
		/**
		@brief Clear the developments location by periods in the graph.
		*/
		void clearGraphDevelopments();
		// DocString: FMTSrModel::clearGraphCache
		/**
		@brief Clear the node caching in the graph.
		*/
		void clearGraphCache();
		// DocString: FMTSrModel::setSolution
		/**
		@brief Set a solution for a given period to warmstart the model or to prepare to bound the model to that solution.
		@param[in] period the period.
		@param[in] schedule the schedule.
		@param[in] tolerance the tolerance.
		@return true if the solution is set else false.
		*/
		bool setSolution(int period, const Core::FMTSchedule& schedule, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTSrModel::setSolutionByLp
		/**
		@brief Set the solution on a partial graph using a schedule, changing the objective function, constraints and variable bounds.
		@details Useful with many lock exempt actions coming from a Ws model; only valid in a get results context on a partial graph.
		@param[in] period the period.
		@param[in] schedule the schedule.
		@param[in] tolerance the tolerance.
		@return true if the solution is set else false.
		*/
		bool setSolutionByLp(int period, const Core::FMTSchedule& schedule, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTSrModel::forceSolution
		/**
		@brief Force a solution using a schedule based on proportions instead of area, placing only the parts of the solution possible with the model modifications.
		@details The schedule must contain lock indicators if locks are used in the model.
		@param[in] period the period.
		@param[in] proportionschedulewithlock the proportion schedule with lock.
		@return true if the solution is forced else false.
		*/
		bool forceSolution(int period, const Core::FMTSchedule& proportionschedulewithlock);
		// DocString: FMTSrModel::getSolution
		/**
		@brief Get the standard solution for a given period, without the natural growth solution.
		@param[in] period the period.
		@param[in] withlock if true the schedule contains locked developments.
		@return the solution schedule.
		*/
		Core::FMTSchedule getSolution(int period, bool withlock = false) const final;
		// DocString: FMTSrModel::getScheduleProportions
		/**
		@brief Same as getSolution but the schedule area represents the percentage of area of the development.
		@param[in] period the period.
		@param[in] withlock if true the schedule contains locked developments.
		@return the schedule proportions.
		*/
		Core::FMTSchedule getScheduleProportions(int period, bool withlock) const;
		// DocString: FMTSrModel::getStats
		/**
		@brief Get the graph stats of the graph and matrix (number of columns, rows, edges, vertices).
		@return the full stats.
		*/
		Graph::FMTGraphStats getStats() const;
		// DocString: FMTSrModel::getGraphStats
		/**
		@brief Get the graph stats of the graph and matrix (number of columns, rows, edges, vertices) for a mask subset.
		@param[in] p_Subset the mask subset.
		@return the graph stats for the graph.
		*/
		Graph::FMTGraphStats getGraphStats(const Core::FMTMask& p_Subset) const;
		// DocString: FMTSrModel::getCopy
		/**
		@brief Return a copy of the FMTModel for the selected period.
		@details For period 0 returns FMTModel::getCopy; for period greater than 0 returns a copy based on the developments of the FMTGraph. Needs a built graph with a solution.
		@param[in] period the period.
		@return the copied model.
		*/
		virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
		// DocString: FMTSrModel::getOutput
		/**
		@brief Get the output value for a period using the matrix solution.
		@details The map key is the output name when level is standard or totalonly, or the development name when level is developpement.
		@param[in] output the output.
		@param[in] period the period.
		@param[in] level the output level.
		@return a map of names to values.
		*/
		virtual std::map<std::string, double> getOutput(const Core::FMTOutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		#if defined FMTWITHR
		// DocString: FMTSrModel::getOutputsDataFrame
		/**
		@brief Return a dataframe filled with outputs from the first to the last period at the development level, for multiple outputs.
		@param[in] outputsdata the outputs.
		@param[in] firstPeriod the first period.
		@param[in] lastPeriod the last period.
		@return the outputs dataframe.
		*/
		Rcpp::DataFrame getOutputsDataFrame(const std::vector<Core::FMTOutput>& outputsdata, int firstPeriod, int lastPeriod) const;
		#endif 
		// DocString: FMTSrModel::buildPeriod
		/**
		@brief Main function used to build the graph and the matrix, adding a period within the graph and the matrix.
		@details If the schedule is not empty the model is generated in partial build mode, building only the graph linked to the solution.
		@param[in] schedule the schedule.
		@param[in] forcepartialbuild if true forces a partial build.
		@param[in] compressageclassoperability the age class operability compression.
		@return the graph stats.
		*/
		Graph::FMTGraphStats buildPeriod(Core::FMTSchedule schedule = Core::FMTSchedule(),
				bool forcepartialbuild = false, int compressageclassoperability = 1);
		// DocString: FMTSrModel::getFirstActivePeriod
		/**
		@brief Return the first active period.
		@details Should be 0 for planning, but moves to 1 and so on when erasePeriod is called.
		@return the first active period.
		*/
		int getFirstActivePeriod() const;
		// DocString: FMTSrModel::getArea
		/**
		@brief Return the area for a given period for a FMTSrModel.
		@details For period 0 the area is the same as FMTModel::getarea; for period greater than 0 the area is the FMTDevelopment of the graph existing at the beginning of the period. Needs a built graph with a solution.
		@param[in] period the period.
		@param[in] beforegrowanddeath if true returns before growth and death.
		@return the actual developments.
		*/
		virtual std::vector<Core::FMTActualDevelopment>getArea(int period = 0, bool beforegrowanddeath = false) const override;
		// DocString: FMTSrModel::getPotentialArea
		/**
		@brief Compute the potential (maximum) development area for a period by forcing all solver decision variables to 1.0.
		@details A temporary copy of the model is used so the current model is not modified.
		@param[in] p_Period the planning period.
		@param[in] p_BeforeGrowAndDeath if true computes the area before growth and mortality, otherwise after.
		@return the potential development area for the period.
		*/
		std::vector<Core::FMTActualDevelopment>getPotentialArea(int p_Period = 0, bool p_BeforeGrowAndDeath = false) const;
		
		
		// DocString: FMTSrModel::getSolverPtr()
		/**
		@brief Get a pointer to the solver behind the model.
		@return a pointer to the solver.
		*/
		FMTLpSolver* getSolverPtr();
		// DocString: FMTSrModel::getConstSolverPtr()
		/**
		@brief Get a pointer to the const solver behind the model.
		@return a const pointer to the solver.
		*/
		const FMTLpSolver* getConstSolverPtr() const;
		// DocString: FMTSrModel::clone
		/**
		@brief Get a clone of the FMTSrModel.
		@return a unique pointer to the cloned model.
		*/
		virtual std::unique_ptr<FMTModel>clone() const override;
		// DocString: FMTSrModel::passInLogger
		/**
		@brief Pass in the logger, overridden for the osisolverinterface.
		@param[in] logger the logger.
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger) override;
		// DocString: FMTSrModel::preSolve
		/**
		@brief Return a presolved copy of the FMTSrModel.
		@param[in] optionaldevelopments the optional developments.
		@return the presolved model.
		*/
		virtual std::unique_ptr<FMTModel>preSolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const override;
		// DocString: FMTSrModel::boundSolution
		/**
		@brief Bound the primal variables to the primal solution present in the matrix for a given period and tolerance.
		@details Useful to update a FMTLpModel or to get ready for replanning.
		@param[in] period the period.
		@param[in] tolerance the tolerance.
		@return true if the solution is bounded else false.
		*/
		bool boundSolution(int period, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTSrModel::setParameter(const FMTintmodelparameters, const int)
		/**
		@brief Override setter for int model parameters.
		@param[in] key the parameter key.
		@param[in] value the value to set.
		@return true if the parameter is set else false.
		*/
		bool setParameter(const FMTintmodelparameters& key, const int& value) override;
		// DocString: FMTSrModel::setParameter(const FMTboolmodelparameters, const bool)
		/**
		@brief Override setter for bool model parameters.
		@param[in] key the parameter key.
		@param[in] value the value to set.
		@return true if the parameter is set else false.
		*/
		bool setParameter(const FMTboolmodelparameters& key, const bool& value) override;
		// DocString: FMTSrModel::getRotations
		/**
		@brief Get all the unique rotations (in periods) taken to complete a serie of actions for a mask and an action aggregate.
		@details A serie can contain a subserie for an aggregate of actions, keeping only series whose actions are part of the aggregate.
		@param[in] mask the mask of the rotations.
		@param[in] aggregate the aggregate the last action is considered to be in.
		@return the unique series.
		*/
		std::set<Core::FMTSerie>getRotations(const Core::FMTMask& mask, const std::string& aggregate) const;
		// DocString: FMTSrModel::isOptimal
		/**
		@brief Return true if the solver considers the solution optimal.
		@return true if the solution is optimal else false.
		*/
		virtual bool isOptimal() const;
		// DocString: FMTSrModel::getObjectiveValue
		/**
		@brief Return the value of the solver objective.
		@return the objective value.
		*/
		virtual double getObjectiveValue() const;
		// DocString: FMTSrModel::getNoChoice
		/**
		@brief Return the period 0 developments of a base mask that have no actions assigned for the whole planning horizon.
		@details The developments mask has to be a subset of the base mask.
		@param[in] base_mask the base mask.
		@return the developments with no choice.
		*/
		std::vector<const Core::FMTDevelopment*> getNoChoice(const Core::FMTMask& base_mask) const;
		// DocString: FMTSrModel::getAllMasks
		/**
		@brief Get all masks in string format for a complete graph, only for the selected themes.
		@param[in] p_selectedThemes the selected themes.
		@return a set of masks.
		*/
		std::set<std::string> getAllMasks(const std::vector<Core::FMTTheme>& p_selectedThemes) const;
	protected:
		// DocString: FMTSrModel::graph
		///graph holding the FMTdevelopments for all the periods.
		std::unique_ptr<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>>m_graph;
		// DocString: FMTSrModel::solver
		///The lpsolver
		FMTLpSolver solver;
		// DocString: FMTSrModel::summarize
		/**
		@brief Summarize constraints stored in a map structure (key is the variable, element is the coefficient) into a vector structure for the osisolverinterface.
		@param[in] variables the variables map.
		@param[in,out] sumvariables the summarized variables.
		@param[in,out] sumcoefficiants the summarized coefficients.
		@return true if the summary succeeded else false.
		*/
		bool summarize(const std::map<int, double>& variables,
			std::vector<int>& sumvariables, std::vector<double>& sumcoefficiants) const;
		// DocString: FMTSrModel::initializeMatrix
		/**
		@brief Initialize the solver interface, called once when the FMTGraph was empty after the first call of buildPeriod.
		@return the graph stats.
		*/
		Graph::FMTGraphStats initializeMatrix();
		// DocString: FMTSrModel::updateMatrix
		/**
		@brief Update the solver interface matrix after the graph has been updated with new development types during buildPeriod.
		@details Variables and constraints related to each new development are added to the matrix (area transfer row, natural growth and action variables).
		@param[in] targets the target vertices.
		@param[in] newstats the new stats.
		@return the graph stats.
		*/
		Graph::FMTGraphStats updateMatrix(const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_pair& targets,
			const Graph::FMTGraphStats& newstats);
		// DocString: FMTSrModel::getgraphlength
		/**
		@brief Return the size of the graph.
		@return the graph size.
		*/
		size_t getGraphSize() const;
		// DocString: FMTSrModel::postSolveGraph
		/**
		@brief Postsolve the graph, rebuilding each vertex and edge based on the original model.
		@param[in] originalbasemodel the original base model.
		*/
		void postSolveGraph(const FMTModel& originalbasemodel);
		// DocString: FMTSrModel::getActives
		/**
		@brief Initialize the graph based on the area if it is not initialized, otherwise return the active vertices.
		@return a queue of active vertices of the graph.
		*/
		std::queue<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor> getActives();
	private:
		// DocString: FMTSrModel::save
		/**
		@brief Save function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to save to.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTModel>(*this));
			ar& BOOST_SERIALIZATION_NVP(solver);
			ar& BOOST_SERIALIZATION_NVP(m_graph);
		}
		// DocString: FMTSrModel::load
		/**
		@brief Load function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to load from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTModel>(*this));
			ar& BOOST_SERIALIZATION_NVP(solver);
			ar& BOOST_SERIALIZATION_NVP(m_graph);
			solver.passInMessageHandler(*_logger);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()

	};

}

BOOST_CLASS_EXPORT_KEY(Models::FMTSrModel)

#endif 
#endif