/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#ifndef FMTlpmodel_Hm_included
#define FMTlpmodel_Hm_included

#include "FMTSrModel.h"
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lock_guard.hpp> 
#include <limits>
#include <map>

namespace Heuristics
{
	class FMTOperatingAreaScheduler;
	class FMTOperatingAreaClusterer;
	class FMTOperatingAreaCluster;
	class FMTOperatingAreaScheme;
}

namespace Models
{
// DocString: FMTmatrixelement
/**
@brief Enum for the matrix element types present within the matrix.
@details The private members of the FMTLpModel use these enumerators to locate constraints, objectives and levels in the matrix for each period.
*/
enum FMTmatrixelement
	{
	goalvariable=0,//period base
	levelvariable=1,//general
	objectivevariable=2,//general
	constraint=3,//period base...
	strictlypositive=4,//when an output have negative coef, be sure that the value of the output in the graph is strictly positive (like in woodstock)
	nr_items=5
	};

// DocString: FMTLpModel
/**
@brief Type III LP forest planning model, with the graph divided per period.
@details Made for replanning and simple forest planning LP optimization, it makes heavy use of FMTGraph to build the matrix. Being based on a list structure, the model can easily be extended (by the back) using buildPeriod or shrinked (by the front) using erasePeriod. The matrix is held within the solver interface pointer.
*/

class FMTEXPORT FMTLpModel : public FMTSrModel
	{
	
	public:
		// DocString: FMTLpModel::clearCache
		/**
		@brief Remove the cache to release memory.
		*/
		void clearCache();
		// DocString: FMTLpModel::clearConstraintLocation
		/**
		@brief Clear the constraint localisation in the matrix.
		@details This breaks the relation between the FMTModel constraints and the matrix constraints.
		*/
		void clearConstraintLocation();
		// DocString: FMTLpModel(const FMTModel,FMTsolverinterface)
		/**
		@brief Main constructor building a FMTLpModel from its base class and letting the user choose the solver type.
		@details See FMTsolverinterface.h for the supported solver types (CLP, MOSEK, GUROBI, CPLEX).
		@param[in] base the base model.
		@param[in] lsolvertype the solver type.
		*/
		FMTLpModel(const FMTModel& base, FMTsolverinterface lsolvertype);
		// DocString: FMTLpModel()
		/**
		@brief Default constructor for FMTLpModel.
		*/
		FMTLpModel();
		// DocString: FMTLpModel(const FMTModel, const Graph::FMTGraph<Graph::FMTVertexProperties,Graph::FMTEdgeProperties>, const FMTLpSolver, const std::vector<std::unordered_map<std::string,std::vector<std::vector<int>>>>)
		/**
		@brief Constructor for FMTLpModel mainly used in postSolve to pass each attribute of the class.
		*/
		/*FMTLpModel(const FMTModel& base, const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& lgraph,
					const FMTLpSolver& lsolver,const std::vector<std::unordered_map<std::string,std::vector<std::vector<int>>>>& lelements);*/
		// DocString: FMTLpModel(const FMTLpModel)
		/**
		@brief Copy constructor for FMTLpModel.
		@param[in] rhs the FMTLpModel to copy.
		*/
		FMTLpModel(const FMTLpModel& rhs);
		// DocString: FMTLpModel::initialSolve
		/**
		@brief Cold start of the LP solve of a simple LP model.
		@details By default calls solverinterface->initialSolve, but based on the FMTsolverinterface it tries to use the best solver parameters for a type III forest planning model; interior point is considered the best algorithm for all solvers.
		@return true if the initial solve succeeded else false.
		*/
		bool initialSolve();
		// DocString: FMTLpModel(FMTLpModel&&)
		/**
		@brief Default move constructor for FMTLpModel.
		@param[in,out] rhs the model to move from.
		*/
		FMTLpModel(FMTLpModel&& rhs);
		// DocString: FMTLpModel::operator=(FMTLpModel&& rhs) 
		/**
		@brief Default move assignment for FMTLpModel.
		@param[in,out] rhs the model to move from.
		@return a reference to this FMTLpModel.
		*/
		FMTLpModel& operator =(FMTLpModel&& rhs);
		// DocString: FMTLpModel::setStrictlyPositivesOutputsMatrix
		/**
		@brief Set the model parameter STRICTLY_POSITIVE to true.
		*/
		void setStrictlyPositivesOutputsMatrix();
		// DocString: FMTLpModel::getOutput
		/**
		@brief Get the output value for a period using the matrix solution.
		@details The map key is the output name when level is standard or totalonly, or the development name when level is developpement; this function can also return the value of a variable output.
		@param[in] output the output.
		@param[in] period the period.
		@param[in] level the output level.
		@return a map of names to values.
		*/
		virtual std::map<std::string, double> getOutput(const Core::FMTOutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTLpModel::getReplanningConstraints
		/**
		@brief Adjust local or global constraints to the global model during replanning, using the solution of this model to adjust the bounds.
		@param[in] modeltype the model type.
		@param[in] localconstraints the local constraints.
		@param[in] period the period.
		@return the adjusted constraints.
		*/
		virtual std::vector<Core::FMTConstraint> getReplanningConstraints(const std::string& modeltype, const std::vector<Core::FMTConstraint>& localconstraints, const int& period) const;
		// DocString: FMTLpModel::addScheduleToObjective
		/**
		@brief Add weight to the objective function for a given schedule.
		@param[in] schedule the schedule.
		@param[in] weight the weight to add.
		*/
		void addScheduleToObjective(const Core::FMTSchedule& schedule,double weight = 1000);
		// DocString: FMTLpModel::operator==
		/**
		@brief Comparison operator for FMTLpModel.
		@param[in] rhs the FMTLpModel to compare to.
		@return true if both models are equal else false.
		*/
		bool operator == (const FMTLpModel& rhs) const;
		// DocString: FMTLpModel::operator!=
		/**
		@brief Comparison operator for FMTLpModel.
		@param[in] rhs the FMTLpModel to compare to.
		@return true if both models are different else false.
		*/
		bool operator != (const FMTLpModel& rhs) const;
		// DocString: FMTLpModel::setObjective
		/**
		@brief Set the objective of the matrix for the whole planning horizon (graph length).
		@details The objective function is always the first constraint in the constraints vector of a FMTModel; a previously set objective is replaced.
		@param[in] objective the objective constraint.
		@return the graph stats.
		*/
		Graph::FMTGraphStats setObjective(const Core::FMTConstraint& objective);
		// DocString: FMTLpModel::setConstraint
		/**
		@brief Set a constraint in the matrix for the whole planning horizon (graph length).
		@details If the constraint already exists in the matrix it is not replaced; can be called after buildPeriod in replanning.
		@param[in] constraint the constraint.
		@return the graph stats.
		*/
		Graph::FMTGraphStats setConstraint(const Core::FMTConstraint& constraint);
		// DocString: FMTLpModel::eraseConstraint
		/**
		@brief Erase a constraint in the matrix and in the model elements.
		@details If period is -1 the constraint is erased for the whole horizon, otherwise only for the specified period.
		@param[in] constraint the constraint.
		@param[in] period the period.
		@return the graph stats.
		*/
		Graph::FMTGraphStats eraseConstraint(const Core::FMTConstraint& constraint,int period=-1);
		// DocString: FMTLpModel::getVariabilities
		/**
		@brief Get the variability of multiple outputs for a given tolerance for each period between periodStart and periodStop.
		@details Must be called after initialSolve; the actual objective function is transformed into a constraint and each output is minimized and maximized to get its range. The map keys are the output name plus UB for the upper bound and LB for the lower bound.
		@param[in] outputs the outputs.
		@param[in] periodStart the first period.
		@param[in] periodStop the last period.
		@return a map of output names to value ranges.
		*/
		std::map<std::string, std::vector<double>>getVariabilities(const std::vector<Core::FMTOutput>& outputs,
																 const int& periodStart,const int& periodStop);
		// DocString: FMTLpModel::getModelFromProportions
		/**
		@brief Return a new FMTLpModel with changes in the initial area based on the tolerances of each global mask.
		@details A tolerance greater than 0 increases the area, a tolerance lower than 0 decreases it.
		@param[in] globalmasks the global masks.
		@param[in] tolerances the tolerances.
		@return the new model.
		*/
		FMTLpModel getModelFromProportions(const std::vector<Core::FMTMask>& globalmasks,
											std::vector<double> tolerances) const;
		// DocString: FMTLpModel::getAreaVariabilities
		/**
		@brief Return the change in outputs resulting from a variation of the area of the model for a given global mask.
		@details Uses the global masks as target; no map key is generated for infeasible variations.
		@param[in] localoutputs the local outputs.
		@param[in] globalmasks the global masks.
		@param[in] tolerances the tolerances.
		@return a map of output names to value ranges.
		*/
		std::map<std::string, std::vector<double>>getAreaVariabilities(const std::vector<Core::FMTOutput>& localoutputs,
				const std::vector<Core::FMTMask>& globalmasks,
				std::vector<double> tolerances = std::vector<double>()) const;
		// DocString: FMTLpModel::erasePeriod
		/**
		@brief Delete the first period (front) of the graph and the matrix, shrinking the planning length to the original size minus one.
		@param[in] constraintsonly if true only erases the constraints.
		@return the graph stats.
		*/
		Graph::FMTGraphStats erasePeriod(bool constraintsonly = false);
		// DocString: FMTLpModel::getOperatingAreaSchedulerHeuristics
		/**
		@brief Return operating area scheduler heuristics using multiple operating areas and an output node with a MIP formulation on the matrix of the FMTLpModel.
		@details The matrix can be copied within the heuristic or used directly (when numberofheuristics is 1 and copysolver is true).
		@param[in] opareas the operating area schemes.
		@param[in] node the output node.
		@param[in] numberofheuristics the number of heuristics.
		@param[in] copysolver if true copies the solver.
		@return the operating area scheduler heuristics.
		*/
		std::vector<Heuristics::FMTOperatingAreaScheduler>getOperatingAreaSchedulerHeuristics(const std::vector<Heuristics::FMTOperatingAreaScheme>& opareas,
																				const Core::FMTOutputNode& node,
																				size_t numberofheuristics=1,
																				bool copysolver=true);
        // DocString: FMTLpModel::getOperatingAreaClustererHeuristics
        /**
        @brief Return operating area clusterer heuristics for solving the operating area clustering problem using multiple operating areas.
        @details The FMTLpModel has to be solved so the heuristic can use the value of the output for a given period as a statistic to perform clustering.
        @param[in] clusters the operating area clusters.
        @param[in] statisticoutput the statistic output.
        @param[in] areaoutput the area output.
        @param[in] period the period.
        @param[in] numberofheuristics the number of heuristics.
        @param[in] minimalnumberofclusters the minimal number of clusters.
        @param[in] maximalnumberofclusters the maximal number of clusters.
        @return the operating area clusterer heuristics.
        */
		std::vector<Heuristics::FMTOperatingAreaClusterer>getOperatingAreaClustererHeuristics(const std::vector<Heuristics::FMTOperatingAreaCluster>& clusters,
																				const Core::FMTOutput& statisticoutput,
                                                                                const Core::FMTOutput& areaoutput,
                                                                                const int& period,
																				size_t numberofheuristics=1,
																				int minimalnumberofclusters = -1,
																				int maximalnumberofclusters = -1) const;
		// DocString: FMTLpModel::resolve
		/**
		@brief Call solverinterface->resolve when some changes are done to the model, avoiding a full initialSolve.
		@return true if the resolve succeeded else false.
		*/
		bool resolve();
		// DocString: FMTLpModel::operator=
		/**
		@brief Copy assignment operator for FMTLpModel.
		@param[in] rhs the FMTLpModel to copy.
		@return a reference to this FMTLpModel.
		*/
		FMTLpModel& operator = (const FMTLpModel& rhs);
		// DocString: ~FMTLpModel()
		/**
		@brief Default destructor for FMTLpModel.
		*/
		~FMTLpModel() = default;
		// DocString: FMTLpModel::getlpmodel
		/**
		@brief Append a child local model to the subset of this general model generated by the selected period, based on the model hierarchy.
		@details Needs a built graph with a solution; the returned model is neither solved nor built.
		@param[in] localmodel the local model.
		@param[in] period the period.
		@return the local model.
		*/
		FMTLpModel getLocalModel(FMTModel localmodel = FMTModel(),int period = 0) const;
		// DocString: FMTLpModel::getObjValue
		/**
		@brief Get the objective value of the solved matrix.
		@return the objective value.
		*/
		double getObjValue() const;
		// DocString: FMTLpModel::writeLP
		/**
		@brief Write the solver interface matrix to a file using the LP formulation.
		@param[in] location the file location.
		*/
		void writeLP(const std::string& location);
		// DocString: FMTLpModel::writeMPS
		/**
		@brief Write the solver interface matrix to a file using the MPS formulation.
		@param[in] location the file location.
		*/
		void writeMPS(const std::string& location);
		// DocString: FMTLpModel::clone
		/**
		@brief Get a clone of the FMTLpModel.
		@return a unique pointer to the cloned model.
		*/
		virtual std::unique_ptr<FMTModel>clone() const;
		// DocString: FMTLpModel::presolve
		/**
		@brief Return a presolved copy of the model using a vector of developments and the actual transitions, reducing the model data if it is badly formulated.
		@param[in] optionaldevelopments the optional developments.
		@return the presolved model.
		*/
		virtual std::unique_ptr<FMTModel>presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
		// DocString: FMTModel::build
		/**
		@brief Build the graph and set the solution if schedules are passed.
		@details If FORCE_PARTIAL_BUILD is true and schedules are passed, only a partial graph is built for the number of periods corresponding to the schedules.
		@param[in] schedules the schedules.
		@return true if the build succeeded else false.
		*/
		virtual bool build(std::vector<Core::FMTSchedule> schedules=std::vector<Core::FMTSchedule>());
		// DocString: FMTModel::solve
		/**
		@brief Call initialSolve on the solver.
		@return true if the solve succeeded else false.
		*/
		virtual bool solve();
		// DocString: FMTLpModel::getCopy
		/**
		@brief Return a copy of the FMTModel for the selected period.
		@details For period 0 returns FMTModel::getCopy; for period greater than 0 returns a copy based on the developments of the FMTGraph. Needs a built graph with a solution.
		@param[in] period the period.
		@return the copied model.
		*/
		virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
	private:
		class ConstraintIndex
			{
			public:
				ConstraintIndex(int p_constraintId, int p_period,FMTmatrixelement p_type);
				/*ConstraintIndex(int p_constraintId, int p_period,
							FMTmatrixelement p_type,
							std::allocator<int>& p_allocator,size_t p_allocation=0);*/
				//void push_back(const int& p_row);
				bool operator < (const ConstraintIndex& p_rhs) const;
				bool operator == (const ConstraintIndex& p_rhs) const;
				//const std::vector<int>& getRows() const;
				//std::vector<int>& getRowsRef();
				size_t getHash() const;
			private:
				int m_constraintId;
				int m_period;
				FMTmatrixelement m_type;
				//std::vector<int>m_rows;
			};
		// DocString: FMTLpModel::m_rowsAllocator
		///The int allocator for the constraintIndex
		mutable std::allocator<int>m_rowsAllocator;
		// DocString: FMTLpModel::m_indexAllocator
		///the constraints index allocator
		mutable std::allocator<ConstraintIndex>m_indexAllocator;
		// DocString: FMTLpModel::m_indexes
		///Locations of the constraints and variables in the matrix for the constraints / objective.
		std::map<ConstraintIndex,std::vector<int>>m_indexes;
		// DocString: FMTLpModel::_getConstraintIndex
		/**
		@brief Get the constraint index iterator of a constraint.
		@param[in] p_constraint the constraint we want the iterator of.
		@return the const iterator of the constraint.
		*/
		std::vector<Core::FMTConstraint>::const_iterator _getConstraintIndex(const Core::FMTConstraint& p_constraint) const;
		// DocString: FMTLpModel::_getsetConstraintIndex
		/**
		@brief Get or set a constraint and get its iterator.
		@param[in] p_constraint the constraint to get or set.
		@return the const iterator of the constraint.
		*/
		std::vector<Core::FMTConstraint>::const_iterator _getsetConstraintIndex(const Core::FMTConstraint& p_constraint);
		// DocString: FMTLpModel::_getIndex
		/**
		@brief Get the index of a const iterator of constraint.
		@param[in] p_it the constraint iterator.
		@return the index of the constraint.
		*/
		int _getIndex(const std::vector<Core::FMTConstraint>::const_iterator& p_it) const;
		// DocString: FMTLpModel::_setGraphCache
		/**
		@brief Reserve memory for the vertices in the graph.
		@param[in] p_noLength if true does not use the length of the actual model as a base guess.
		*/
		void _setGraphCache(bool p_noLength = false);
		// DocString: FMTLpModel::_setConstraintsCache
		/**
		@brief Set the size of the cache of the constraints.
		*/
		void _setConstraintsCache();
		// DocString: FMTLpModel::serialize
		/**
		@brief Save and load functions used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTSrModel>(*this));
			ar& BOOST_SERIALIZATION_NVP(m_indexes);
		}
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTSrModel>(*this));
			ar& BOOST_SERIALIZATION_NVP(m_indexes);
		}
		// DocString: FMTLpModel::elements
		///Locations of the constraints and variables in the matrix for the constraints / objective.
		//std::vector<std::unordered_map<std::string,
		//	std::vector<std::vector<int>>>>elements;
		// DocString: FMTLpModel::getClusterer
		/**
		@brief Return an operating area clusterer filled with statistic doubles coming from the output for a given period, using an area output and a statistic output.
		@param[in] initialcluster the initial clusters.
		@param[in] areaoutput the area output.
		@param[in] statisticoutput the statistic output.
		@param[in] period the period.
		@param[in] minimalnumberofclusters the minimal number of clusters.
		@param[in] maximalnumberofclusters the maximal number of clusters.
		@return the operating area clusterer.
		*/
		Heuristics::FMTOperatingAreaClusterer getClusterer(
			const std::vector<Heuristics::FMTOperatingAreaCluster>& initialcluster,
			const Core::FMTOutput& areaoutput,
			const Core::FMTOutput& statisticoutput,
			const int& period, int minimalnumberofclusters = -1, int maximalnumberofclusters = -1) const;
		// DocString: FMTLpModel::getsetMatrixElement
		/**
		@brief Add variables or constraints to the matrix to satisfy a FMTConstraint, returning the index of the element whether it already exists or is new.
		@param[in] p_constraintId the constraint iterator.
		@param[in] element_type the matrix element type.
		@param[in] indexes the variable indexes and coefficients.
		@param[in] period the period.
		@param[in] lowerbound the lower bound.
		@param[in] upperbound the upper bound.
		@return the index of the matrix element.
		*/
		int getsetMatrixElement(const std::vector<Core::FMTConstraint>::const_iterator& p_constraintId,
			const FMTmatrixelement& element_type, const std::map<int, double>& indexes,
			int period = -1,
			double lowerbound = std::numeric_limits<double>::min(),
			double upperbound = std::numeric_limits<double>::max());
		// DocString: FMTLpModel::getGoals
		/**
		@brief Return the goals index if it already exists within the other constraints.
		@details Goals names can be used across multiple FMTConstraints.
		@param[in] goalsnames the goals names.
		@param[in,out] index the variable indexes and coefficients.
		@param[in] sense the sense.
		@return true if the goals exist else false.
		*/
		bool getGoals(const std::vector<std::string>& goalsnames,
			std::map<int, double>& index, const double& sense) const;
		// DocString: FMTLpModel::getLevelFromLevelName
		/**
		@brief Return the level index of a non empty constraint.
		@param[in] variable_level the level name.
		@param[in] period the period.
		@param[in] p_it the constraint iterator.
		@return the level index.
		*/
		int getLevelFromLevelName(const std::string& variable_level, int period,
			const std::vector<Core::FMTConstraint>::const_iterator& p_it) const;
		// DocString: FMTLpModel::getSetLevel
		/**
		@brief Check if the level already exists within the matrix for constraints other than the given one for a period, and set it otherwise.
		@param[in] p_it the constraint iterator.
		@param[in] variable_level the level name.
		@param[in] period the period.
		@return the level index.
		*/
		int getSetLevel(const std::vector<Core::FMTConstraint>::const_iterator& p_it,
						const std::string& variable_level, int period);
		// DocString: FMTLpModel::getMatrixElement
		/**
		@brief Return all the elements (level, constraint or variable) related to a given constraint for a period.
		@param[in] it the constraint iterator.
		@param[in] p_period the period.
		@param[in] p_element the matrix element type.
		@return the matrix elements.
		*/
		const std::vector<int> getMatrixElement(const std::vector<Core::FMTConstraint>::const_iterator& it,
												int p_period,
												FMTmatrixelement p_element) const;
		// DocString: FMTLpModel::getMatrixElementRef
		/**
		@brief Return a reference to all the elements (level, constraint or variable) related to a given constraint for a period.
		@param[in] it the constraint iterator.
		@param[in] p_period the period.
		@param[in] p_element the matrix element type.
		@return a reference to the matrix elements.
		*/
		std::vector<int>& getMatrixElementRef(const std::vector<Core::FMTConstraint>::const_iterator& it,
			int p_period,
			FMTmatrixelement p_element);
		// DocString: FMTLpModel::locateLevels
		/**
		@brief Fill the variables map for a given level by looking up the graph for a period.
		@param[in] nodes the output nodes.
		@param[in] period the period.
		@param[in,out] variables the variables map.
		@param[in] p_it the constraint iterator.
		@param[in] multiplier the multiplier.
		*/
		void locateLevels(const std::vector<Core::FMTOutputNode>& nodes, int period,
			std::map<int, double>& variables, const std::vector<Core::FMTConstraint>::const_iterator& p_it,
			double multiplier = 1);
		// DocString: FMTLpModel::locateNodes
		/**
		@brief Fill the variables map for given FMTConstraints by looking up the graph for a period, applying the multiplier to the coefficients.
		@details If there are outputs with negative coefficients, returns a vector containing a variables map for each output when the model parameter STRICTLY_POSITIVE is true.
		@param[in] nodes the output nodes.
		@param[in] period the period.
		@param[in,out] variables the variables map.
		@param[in] multiplier the multiplier.
		@return a vector of variable maps.
		*/
		std::vector<std::map<int, double>> locateNodes(const std::vector<Core::FMTOutputNode>& nodes, int period, std::map<int, double>& variables, double multiplier = 1) const;
		// DocString: FMTLpModel::updateMatrixElements
		/**
		@brief Update the indexes of matrix elements after erasePeriod shrinks the matrix, using the deleted elements.
		@param[in,out] matrixelements the matrix elements to update.
		@param[in] deletedelements the deleted elements.
		*/
		void updateMatrixElements(std::vector<int>& matrixelements, const std::vector<int>& deletedelements) const;
		// DocString: FMTLpModel::updateConstraintsMapping
		/**
		@brief Update the indexes of all the FMTConstraints of the elements after erasePeriod shrinks the matrix, calling updateMatrixElements.
		@param[in] Dvariables the deleted variables.
		@param[in] Dconstraints the deleted constraints.
		*/
		void updateConstraintsMapping(const std::vector<int>& Dvariables, const std::vector<int>& Dconstraints);
		// DocString: FMTLpModel::updateMatrixNGraph
		/**
		@brief Update the indexes of all the FMTConstraints, the FMTDevelopment constraints and variables in the graph, and delete those variables and constraints from the solver interface matrix after erasePeriod.
		@param[in] updategraph if true also updates the graph.
		@return true if the update succeeded else false.
		*/
		bool updateMatrixNGraph(bool updategraph = true);
		// DocString: FMTLpModel::getAmountOfPaths
		/**
		@brief Get the number of possible paths if an action is committed on a development.
		*/
		//size_t getAmountOfPaths(const Core::FMTDevelopment& dev, const int& actionid) const;
		// DocString: FMTLpModel::isMatrixElement
		/**
		@brief Check if the FMTConstraint has an element of the given type located in the matrix for a period.
		@param[in] p_constraintId the constraint iterator.
		@param[in] element_type the matrix element type.
		@param[in] period the period.
		@return true if the matrix element exists else false.
		*/
		bool isMatrixElement(const std::vector<Core::FMTConstraint>::const_iterator& p_constraintId,
			const FMTmatrixelement& element_type, int period) const;
		// DocString: FMTLpModel::containsMatrixElements
		/**
		@brief Check if the FMTConstraint has elements related to it.
		@param[in] p_constraintId the constraint iterator.
		@param[in] period the period.
		@return true if the constraint has matrix elements else false.
		*/
		bool containsMatrixElements(const std::vector<Core::FMTConstraint>::const_iterator& p_constraintId, int period) const;
		// DocString: FMTLpModel::isSameMatrixElement
		/**
		@brief Check if the requested matrix element is the same as the found one (bounds and variables).
		@param[in] matrixindex the matrix index.
		@param[in] element_type the matrix element type.
		@param[in] lowerb the lower bound.
		@param[in] upperb the upper bound.
		@param[in] variables the variables map.
		@return true if it is the same matrix element else false.
		*/
		bool isSameMatrixElement(const int& matrixindex, const FMTmatrixelement& element_type,
			const double& lowerb, const double& upperb, const std::map<int, double>& variables) const;

		// DocString: FMTLpModel::getobjectivebounds
		/**
		@brief Return the constraint indexes of the lower and upper bounds of the constraints set for the objective.
		@param[in] dolower if true does the lower bound.
		@param[in] doupper if true does the upper bound.
		@param[in] tolerance the tolerance.
		@return the constraint indexes of the objective bounds.
		*/
		std::vector<int>setObjectiveBounds(bool dolower = true, bool doupper = true, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTLpModel::updateMatrixNaming
		/**
		@brief Update the row and variable names based on the graph.
		*/
		void updateMatrixNaming();
		// DocString: FMTLpModel::updateGeneralConstraintsNaming
		/**
		@brief Update the row and variable names using the general constraints of the model.
		@param[in,out] colnames the column names.
		@param[in,out] rownames the row names.
		*/
		void updateGeneralConstraintsNaming(std::vector<std::string>& colnames,
			std::vector<std::string>& rownames) const;
		// DocString: FMTLpModel::setPositiveOutputsInMatrix
		/**
		@brief Set the variables in the strictly positive outputs map to be greater than or equal to 0 in the matrix.
		@param[in] p_it the constraint iterator.
		@param[in] strictlypositivesoutputs the strictly positive outputs.
		@param[in] period the period.
		@return true if the outputs were set else false.
		*/
		bool setPositiveOutputsInMatrix(const std::vector<Core::FMTConstraint>::const_iterator& p_it,
			const std::vector<std::map<int, double>>& strictlypositivesoutputs, int period);
		// DocString: FMTLpModel::swapPtr
		/**
		@brief Swap this model for the element at the end of the unique pointer.
		@param[in,out] rhs the unique pointer to swap with.
		*/
		virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);
		// DocString: FMTLpModel::FMTLpModel(const FMTSrModel&)
		/**
		@brief Construct a FMTLpModel from a FMTSrModel, for presolve use.
		@param[in] rhs the FMTSrModel.
		*/
		FMTLpModel(const FMTSrModel& rhs);
		// DocString: FMTLpModel::trySetSolution(const std::vector<Core::FMTSchedule>&)
		/**
		@brief Try setSolution and fall back to setSolutionByLp if it is not possible.
		@param[in] schedules the schedules.
		@return true if the solution was set else false.
		*/
		bool trySetSolution(const std::vector<Core::FMTSchedule>& schedules);
		
	};

}

BOOST_CLASS_EXPORT_KEY(Models::FMTLpModel)

#endif
#endif
