/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#ifdef FMTWITHOSI

#ifndef FMTsrmodel_Hm_included
#define FMTsrmodel_Hm_included

#include "FMTmodel.h"
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>
#include "FMTlpsolver.h"
#include "FMTvertexproperties.h"
#include "FMTedgeproperties.h"
#include "FMTgraph.hpp"
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
	class FMTgraphvertextoyield;

}

namespace Models 
	{
	// DocString: FMTSrModel
	/**
	FMTSrModel hold the attributes and methods shared between the spatially referenced models(FTMlpmodel and FMTNssModel). 
	*/
	class FMTEXPORT FMTSrModel : public FMTModel
	{
	public:
		// DocString: FMTSrModel::setNodeCacheSize
		/*
		@brief building constraints or outut sometime takes alot of memory. This is the size of reserved memory
		taken when caching output node during build process ans output request.
		@param[in] p_size the size to reserve 
		*/
		void setNodeCacheSize(const size_t& p_size);
		// DocString: FMTSrModel::getGraphVertexToYield
		/*
		If you want to do a generic yieldrequest for the model you may need to have a graphvertextoyield to deal with yields
		that depand on the full graph and model (like FMTYieldModel).
		the function return a filled Graphvertexyoyield without a specific vertex selected.
		*/
		Graph::FMTgraphvertextoyield getGraphVertexToYield() const;
		// DocString: FMTSrModel::postSolve
		/*
		This function is for postsolving the presolved model into the original model. In this case, the FMTgraph of the FMTSrModel is also postsolved.
		*/
		virtual void postSolve(const FMTModel& originalbasemodel);
		// DocString: FMTSrModel::setParallelLogger
		/**
		Solver's logger cannot work in parallel so you need to pass a logger owned
		by the thead to the solver to make sure it does not work in concurrency.
		*/
		void setParallelLogger(Logging::FMTLogger& logger) override;
		// DocString: FMTSrModel::getAvailableSolverInterface
		/**
		Return a vector of solverinterface available
		*/
		static std::vector<Models::FMTsolverinterface> getAvailableSolverInterface();
		// DocString: FMTSrModel(const FMTModel,FMTsolverinterface)
		/**
		Main constructor used to build FMTSrModel using it's base class and to let the user choose the solvertype
		(CLP,MOSEK,GUROBI,CPLEX) see FMTsolverinterface.h for more information about the supported solvertype.
		*/
		FMTSrModel(const FMTModel& base, FMTsolverinterface lsolvertype);
		// DocString: FMTSrModel()
		/**
		Default constructor of FMTSrModel
		*/
		FMTSrModel();
		// DocString: FMTSrModel(const FMTSrModel)
		/**
		Copy constructor of FMTSrModel
		*/
		FMTSrModel(const FMTSrModel& rhs);
		// DocString: FMTSrModel(const FMTModel, const Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>,	const FMTLpSolver)
		/**
		Constructor of FMTSrModel mainly use in postSolve.
		*/
		FMTSrModel(FMTModel&& base,const Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>& lgraph,
					const FMTLpSolver& lsolver);
		// DocString: FMTSrModel::operator=
		/**
		Copy assignment of FMTSrModel
		*/
		FMTSrModel& operator = (const FMTSrModel& rhs);
		// DocString: FMTSrModel(FMTSrModel&&)
		/**
		Default move constructor for FMTSrModel.
		*/
		FMTSrModel(FMTSrModel&& rhs) noexcept;
		// DocString: FMTSrModel::operator=(FMTSrModel&& rhs) 
		/**
		Default move assignment for FMTSrModel.
		*/
		FMTSrModel& operator =(FMTSrModel&& rhs) noexcept;
		// DocString: ~FMTSrModel()
		/**
		Default destructor of FMTSrModel
		*/
		~FMTSrModel() = default;
		// DocString: FMTSrModel::operator==
		/**
		Comparison operator of FMTSrModel
		*/
		bool operator == (const FMTSrModel& rhs) const;
		// DocString: FMTSrModel::operator!=
		/**
		Comparison operator of FMTSrModel
		*/
		bool operator != (const FMTSrModel& rhs) const;
		// DocString: FMTSrModel::unboundSolution
		/**
		Unbound the primal bounds of a given period.
		*/
		bool unboundSolution(int period);
		// DocString: FMTSrModel::isPeriodBounded
		/**
		Check if FMTDevelopment area are bounded on there primal variables for a given period.
		*/
		bool isPeriodBounded(int period) const;
		// DocString: FMTSrModel::clearGraphDevelopments
		/**
		Clear developments location by periods in the graph
		*/
		void clearGraphDevelopments();
		// DocString: FMTSrModel::clearGraphCache
		/**
		Clear the node caching in the graph
		*/
		void clearGraphCache();
		// DocString: FMTSrModel::setSolution
		/**
		If the user wants to set a solution for a given period for warmstarting the model or prepare to
		bound the model to that solution.
		*/
		bool setSolution(int period, const Core::FMTSchedule& schedule, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTSrModel::setSolutionByLp
		/**
		In some cases if you avec a lot of _lockexcempt actions comming from a Ws model it might be easier to
		use the setSolutionByLp to set the solution on a partial graph. This function will change the objective function,
		constraints and variables bounds so juste use it in a "Get results" context ( only valid for partial graph).
		*/
		bool setSolutionByLp(int period, const Core::FMTSchedule& schedule, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTSrModel::forceSolution
		/**
		Force a solution by using a schedule based on proportions instead of area. So if there is modifications to the model, it will
		place only the parts of the solution that are possible considering the modifications. 
		The schedule must be with lock indicators if locks are used in the model. 
		*/
		bool forceSolution(int period, const Core::FMTSchedule& proportionschedulewithlock);
		// DocString: FMTSrModel::getSolution
		/**
		Get the standard solution for a given period (FMTSchedule dont have natural growth solution included).
		If with lock is true then the schedule will contain locked development.
		*/
		Core::FMTSchedule getSolution(int period, bool withlock = false) const final;
		// DocString: FMTSrModel::getScheduleProportions
		/**
		Same as getSolution but the schedule area represent the percentage of area of the development.
		*/
		Core::FMTSchedule getScheduleProportions(int period, bool withlock) const;
		// DocString: FMTSrModel::getStats
		/**
		@brief Get the graph stats of the graph and matrix (number of columns/rows/edges/vertices...)
		@return the full stats 
		*/
		Graph::FMTgraphstats getStats() const;
		// DocString: FMTSrModel::getStats
		/**
		@brief Get the graph stats of the graph and matrix (number of columns/rows/edges/vertices...)
		@param[in] p_Subset the mask subset
		@return the graph stats for the graph.
		*/
		Graph::FMTgraphstats getGraphStats(const Core::FMTMask& p_Subset) const;
		// DocString: FMTSrModel::getCopy
		/**
		This function returns a copy of the FMTModel of the selected period.
		If period = 0 it returns the FMTModel::getCopy if period > 0 then it returns
		a copy of the FMTModel based on the developments of the FMTgraph of the FMTLpModel.
		Need to have a builded graph with a solution to use this function.
		*/
		virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
		// DocString: FMTSrModel::getOutput
		/**
		Get the output value of a output for a given period using the solution of the matrix.
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or development name if level == FMToutputlevel::developpement
		*/
		virtual std::map<std::string, double> getOutput(const Core::FMTOutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		#if defined FMTWITHR
		// DocString: FMTSrModel::getOutputsDataFrame
		/**
		Returns a dataframe filled up with outputs from first period to last period at the development level.
		For multiple outputs.
		*/
		Rcpp::DataFrame getOutputsDataFrame(const std::vector<Core::FMTOutput>& outputsdata, int firstPeriod, int lastPeriod) const;
		#endif 
		// DocString: FMTSrModel::buildPeriod
		/**
		This function is the main function used to build the graph and the matrix.
		A call to that function add a period within the graph and the matrix of the FMTSrModel.
		If the schedule is not empty than the model is going to be generated in partialbuild mode and
		not fullbuild mode. Partialbuild will only build the graph linked to the solution. make it perfect to
		reinterpret outputs for a given solution without rebuilding the whole graph.
		*/
		Graph::FMTgraphstats buildPeriod(Core::FMTSchedule schedule = Core::FMTSchedule(),
				bool forcepartialbuild = false, int compressageclassoperability = 1);
		// DocString: FMTSrModel::getFirstActivePeriod
		/**
		Return the first active period should be always 0 in case or planning.
		But when erasePeriod is called the first active period is going to move to 1 and so on.
		*/
		int getFirstActivePeriod() const;
		// DocString: FMTSrModel::getArea
		/**
		This function returns an area for a given period for a FMTSrModel.
		If period = 0 the area is the same has FMTModel::getarea().
		For period > 0 the area returned is the FMTDevelopment of the graph
		that can be (actual of futur) existing at the beginning of the period.
		Need to have a builded graph with a solution to use this function.
		*/
		virtual std::vector<Core::FMTActualDevelopment>getArea(int period = 0, bool beforegrowanddeath = false) const override;
		// DocString: FMTSrModel::getPotentialArea
		/**
		* @brief Computes the potential development area for a given period.
		*
		* This method evaluates the theoretical (maximum) potential area by forcing
		* all decision variables in the underlying solver to a value of 1.0.
		* A temporary copy of the current FMTSrModel instance is created to avoid
		* modifying the state of the original model.
		*
		* The method then calls getarea() on the temporary model using the provided
		* period and growth/death timing option.
		*
		* @param p_Period The planning period for which the potential area is evaluated.
		* @param p_BeforeGrowAndDeath If true, computes the area before growth and mortality
		* processes are applied; otherwise, computes it after these processes.
		*
		* @return A vector of Core::FMTActualDevelopment representing the potential
		* development area for the specified period.
		*
		* @note This function does not modify the current model instance.
		* @note In case of an exception, the error is handled internally and an empty
		* vector may be returned.
		*/
		std::vector<Core::FMTActualDevelopment>getPotentialArea(int p_Period = 0, bool p_BeforeGrowAndDeath = false) const;
		
		
		// DocString: FMTSrModel::getSolverPtr()
		/**
		Get a pointer to the solver behind the model.
		*/
		FMTLpSolver* getSolverPtr();
		// DocString: FMTSrModel::getConstSolverPtr()
		/**
		Get a pointer to the const solver behind the model.
		*/
		const FMTLpSolver* getConstSolverPtr() const;
		// DocString: FMTSrModel::clone
		/**
		Get a clone of the FMTSrModel
		*/
		virtual std::unique_ptr<FMTModel>clone() const override;
		// DocString: FMTSrModel::passInLogger
		/**
		We need to override the passinlogger for the osisolverinterface
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger) override;
		// DocString: FMTSrModel::presolve
		/**
		Presolve a FMTSrModel.
		*/
		virtual std::unique_ptr<FMTModel>presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const override;
		// DocString: FMTSrModel::boundSolution
		/**
		This function bounds the primal variables to the primal solution present within the matrix for
		a given period and tolerance. Perfect function to update a FMTLpModel or get ready for replanning.
		*/
		bool boundSolution(int period, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTSrModel::setParameter(const FMTintmodelparameters, const int)
		/**
		Override setter for intmodelparameters.
		*/
		bool setParameter(const FMTintmodelparameters& key, const int& value) override;
		// DocString: FMTSrModel::setParameter(const FMTboolmodelparameters, const bool)
		/**
		Override setter for boolmodelparameters.
		*/
		bool setParameter(const FMTboolmodelparameters& key, const bool& value) override;
		// DocString: FMTSrModel::getRotations
		/**
		Based on a mask and an action get all the unique rotations (in period) taken to complete a serie of action. A serie can contain a subserie for an
		aggregate of actions. It will only keep series that actions are part of the aggregate.
		Args:
			mask : the FMTMask of the rotations
			aggregate : std::string on which the last action is considered to be in
		Returns: Unique FMTSerie (where the serie is in string action1-action2-action3... string is the serie and int is the number of periods taken to complete the serie.
		*/
		std::set<Core::FMTSerie>getRotations(const Core::FMTMask& mask, const std::string& aggregate) const;
		// DocString: FMTSrModel::isOptimal
		/**
		Return true if the solver consider the solution optimal.
		*/
		virtual bool isOptimal() const;
		// DocString: FMTSrModel::getObjectiveValue
		/**
		Return the value of the solver objective.
		*/
		virtual double getObjectiveValue() const;
		// DocString: FMTSrModel::getNoChoice
		/**
		Giving a base_mask returns period 0 developpements that have no actions assigned for the whole planning horizon.
		The developpements mask have to be a subset of the base_mask.
		*/
		std::vector<const Core::FMTDevelopment*> getNoChoice(const Core::FMTMask& base_mask) const;
		// DocString: FMTSrModel::getAllMasks
		/*
		@brief Get all masks in string format for a complete grapoh only for selected themes
		@param[in] p_selectedThemes
		@return a set of masks
		*/
		std::set<std::string> getAllMasks(const std::vector<Core::FMTTheme>& p_selectedThemes) const;
	protected:
		// DocString: FMTSrModel::graph
		///graph holding the FMTdevelopments for all the periods.
		std::unique_ptr<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>>m_graph;
		// DocString: FMTSrModel::solver
		///The lpsolver
		FMTLpSolver solver;
		// DocString: FMTSrModel::summarize
		/**
		Simple function to summarize constraints that are un a map structure key = variables, element = coefficiant
		to a array structure (vector) for osisolverinterface. map structure is easier to deal with thant two vectors.
		*/
		bool summarize(const std::map<int, double>& variables,
			std::vector<int>& sumvariables, std::vector<double>& sumcoefficiants) const;
		// DocString: FMTSrModel::initializeMatrix
		/**
		Initialize the solverinterface called once when the FMTgraph was empty after the first call of buildperiod.
		*/
		Graph::FMTgraphstats initializeMatrix();
		// DocString: FMTSrModel::updateMatrix
		/**
		During a call to build period after the graph has been updated with nes developments type the solverinterface matrix
		need to be updated. Variables and constraints related to each of those new developments will be added to the matrix.
		So area transfer row and natural growth plus action variables.
		*/
		Graph::FMTgraphstats updateMatrix(const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_pair& targets,
			const Graph::FMTgraphstats& newstats);
		// DocString: FMTSrModel::getgraphlength
		/**
		Return the size of the graph.
		*/
		size_t getGraphSize() const;
		// DocString: FMTSrModel::postsolveGraph
		/**
		Post solve this graph and return a presolved graph for each vertex and edges based on the original model.
		*/
		void postsolveGraph(const FMTModel& originalbasemodel);
		// DocString: FMTSrModel::getActives
		/**
		@brief If the graph is not initialize then initialise the graph based on the area else return the actives vertices.
		@return a queue of actives vertices of the graph.
		*/
		std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> getActives();
	private:
		// DocString: FMTSrModel::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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