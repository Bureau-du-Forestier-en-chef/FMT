/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#ifndef FMTlpmodel_Hm_included
#define FMTlpmodel_Hm_included

#include "FMTsrmodel.h"
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
Enum for matrix element type present within the matrix.
Elements private member of the FMTLpModel uses those enumerators to
locate constraints/objectives/levels in the matrix for each period.
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
This model is a type III LP forest planning model. The graph
is divided per period. This model is made for replanning and simple
forest planning LP optimization. It makes heavy use of FMTGraph
for building the matrix. FMTGraph being based on a list structure
FMTLpModel can easely be extended (by the back) using the function buildperiod() or
shrinked (by the front) using the function erasePeriod.
The matrix is held within the solverinterface pointer.
*/

class FMTEXPORT FMTLpModel : public FMTSrModel
	{
	
	public:
		// DocString: FMTLpModel::clearCache
		/**
		Remove cache to release memory.
		*/
		void clearCache();
		// DocString: FMTLpModel::clearConstraintLocation
		/**
		Clear the localisation in the matrix...if you do that you break the relation between the FMTModel constraints and the matrix contraints.
		*/
		void clearConstraintLocation();
		// DocString: FMTLpModel(const FMTModel,FMTsolverinterface)
		/**
		Main constructor used to build FMTLpModel using it's base class and to let the user choose the solvertype
		(CLP,MOSEK,GUROBI,CPLEX) see FMTsolverinterface.h for more information about the supported solvertype.
		*/
		FMTLpModel(const FMTModel& base, FMTsolverinterface lsolvertype);
		// DocString: FMTLpModel()
		/**
		Default constructor of FMTLpModel
		*/
		FMTLpModel();
		// DocString: FMTLpModel(const FMTModel, const Graph::FMTGraph<Graph::FMTVertexProperties,Graph::FMTEdgeProperties>, const FMTLpSolver, const std::vector<std::unordered_map<std::string,std::vector<std::vector<int>>>>)
		/**
		Constructor of FMTLpModel mainly use in postSolve to pass each attributes of the class.
		*/
		/*FMTLpModel(const FMTModel& base, const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& lgraph,
					const FMTLpSolver& lsolver,const std::vector<std::unordered_map<std::string,std::vector<std::vector<int>>>>& lelements);*/
		// DocString: FMTLpModel(const FMTLpModel)
		/**
		Copy constructor of FMTLpModel
		*/
		FMTLpModel(const FMTLpModel& rhs);
		// DocString: FMTLpModel::initialSolve
		/**
		Cold start of the LPsolve of a simple LP model.
		By default initialSolve will call solverinterface->initialSolve() but using the FMTsolverinterface enum
		this function will try to use the best solver parameters for a Type III Forest planning model.
		For all solvers interior point is considered the best algorith.
		*/
		bool initialSolve();
		// DocString: FMTLpModel(FMTLpModel&&)
		/**
		Default move constructor for FMTLpModel.
		*/
		FMTLpModel(FMTLpModel&& rhs);
		// DocString: FMTLpModel::operator=(FMTLpModel&& rhs) 
		/**
		Default move assignment for FMTLpModel.
		*/
		FMTLpModel& operator =(FMTLpModel&& rhs);
		// DocString: FMTLpModel::setStrictlyPositivesOutputsMatrix
		/**
		To set the model parameter STRICTLY_POSITIVE at true.
		*/
		void setStrictlyPositivesOutputsMatrix();
		// DocString: FMTLpModel::getOutput
		/**
		Get the output value of a output for a given period using the solution of the matrix.
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or development name if level == FMToutputlevel::developpement, this function can also return the value of
		a variable output.
		*/
		virtual std::map<std::string, double> getOutput(const Core::FMTOutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTLpModel::getReplanningConstraints
		/**
		During replaning some local/global constraints need to be ajusted to the global model.
		The function will take a vector of local/global constraint and ajust the bounds using the solution of this global/local model.
		*/
		virtual std::vector<Core::FMTConstraint> getReplanningConstraints(const std::string& modeltype, const std::vector<Core::FMTConstraint>& localconstraints, const int& period) const;
		// DocString: FMTLpModel::addScheduleToObjective
		/**
		This function will addup weight to the objective function for a given schedule.
		*/
		void addScheduleToObjective(const Core::FMTSchedule& schedule,double weight = 1000);
		// DocString: FMTLpModel::operator==
		/**
		Comparison operator of FMTLpModel
		*/
		bool operator == (const FMTLpModel& rhs) const;
		// DocString: FMTLpModel::operator!=
		/**
		Comparison operator of FMTLpModel
		*/
		bool operator != (const FMTLpModel& rhs) const;
		// DocString: FMTLpModel::setObjective
		/**
		This function set the objective of the matrix for the whole planning horizon (graph length).
		Note that the objective function is always the firts constraint in the constraints vector of a
		FMTModel. If a objective was already set before it will replace it when calling this function.
		*/
		Graph::FMTGraphStats setObjective(const Core::FMTConstraint& objective);
		// DocString: FMTLpModel::setConstraint
		/**
		This function set a constraint in the matrix for the whole planning horizon (graph length).
		If the function is recalled if the constraint already exist in the matrix in wont be replaced.
		Can be called after calling builperiod in replanning.
		*/
		Graph::FMTGraphStats setConstraint(const Core::FMTConstraint& constraint);
		// DocString: FMTLpModel::eraseConstraint
		/**
		Erase a constraint for a given period in the matrix and in the model elements. If period == -1 then it will erase
		the constraint for the whole horizon else it will only erase the constraint for the specified period.
		*/
		Graph::FMTGraphStats eraseConstraint(const Core::FMTConstraint& constraint,int period=-1);
		// DocString: FMTLpModel::getVariabilities
		/**
		Get the variability of multiple outputs for a given tolerance(see modelparameters) for each period between (periodStart) and (periodStop).
		Need to call this function after initialSolve.
		The actual objective function is transform to a constraint and then each output
		are minimized and maximized to get their actual range for each period.
		the returned map key are the name of the output plus UB for upper bound (maximization) and
		LB for lower bound (minimization). This function  do a lot of resolve.
		*/
		std::map<std::string, std::vector<double>>getVariabilities(const std::vector<Core::FMTOutput>& outputs,
																 const int& periodStart,const int& periodStop);
		// DocString: FMTLpModel::getModelFromProportions
		/**
		Will returns a new FMTLpModel with changes in the initial area based on the tolerances of each globalmasks
		provided in the tolerances vector tolerance > 0 = increase in area, tolerance < 0 = decrease in area.
		*/
		FMTLpModel getModelFromProportions(const std::vector<Core::FMTMask>& globalmasks,
											std::vector<double> tolerances) const;
		// DocString: FMTLpModel::getAreaVariabilities
		/**
		Sometime it is usefull to know what is the impact on some outputs of changing a set of developments
		of the area of the model for a given global mask. This function uses the globalmasks has target and returns
		the change in outputs resulting from a variation > 0 or < 0, will generate no map key if infeasible.
		*/
		std::map<std::string, std::vector<double>>getAreaVariabilities(const std::vector<Core::FMTOutput>& localoutputs,
				const std::vector<Core::FMTMask>& globalmasks,
				std::vector<double> tolerances = std::vector<double>()) const;
		// DocString: FMTLpModel::erasePeriod
		/**
		When doing replanning or simply model update the user may want to delete the first period (front)
		of the graph and the matrix to get a FMTModel - first period. The planning length will be shrinked to
		originalsize - 1.
		*/
		Graph::FMTGraphStats erasePeriod(bool constraintsonly = false);
		// DocString: FMTLpModel::getOperatingAreaSchedulerHeuristics
		/**
		Using multiple operating areas and a simple output node a MIP formulation (using the BFECopt heuristic) is done using the matrix of
		the FMTLpModel. The matrix can be copied within the operatingareaheuristic or directly uses the matrix
		of the FMTLpModel (if numberofheuristics == 1 and copysolver == true). The user can than decide to change
		some parameters of heuristics that are in the vectors.
		*/
		std::vector<Heuristics::FMTOperatingAreaScheduler>getOperatingAreaSchedulerHeuristics(const std::vector<Heuristics::FMTOperatingAreaScheme>& opareas,
																				const Core::FMTOutputNode& node,
																				size_t numberofheuristics=1,
																				bool copysolver=true);
        // DocString: FMTLpModel::getOperatingAreaClustererHeuristics
		/**
		Using multiple operating areas this function generates multiples heuristics for solving the operating area clustering problem.
        The user can decide to change some parameters of heuristics that are in the vectors.The FMTLpModel has to be solved so the heuristic can use
        the value of the (output) of a given (period) has a statistic to perform clustering.
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
		By default call solverinterface->resolve() when some changes are done to the model.
		The user dont necessery need the call initialSolve every time the matrix has changed a call to resolve maybe enought.
		*/
		bool resolve();
		// DocString: FMTLpModel::operator=
		/**
		Copy assignment of FMTLpModel
		*/
		FMTLpModel& operator = (const FMTLpModel& rhs);
		// DocString: ~FMTLpModel()
		/**
		Default destructor of FMTLpModel
		*/
		~FMTLpModel() = default;
		// DocString: FMTLpModel::getlpmodel
		/**
		This fonction is based on model hierarchy. The *this FMTLpModel is considered has the
		general parent model the localmodel if not empty is considered one part of the child model.
		The function is going to append the child model to the subset of the general model
		generated by the selected period.
		Need to have a builded graph with a solution to use this function.
		The returned model wont be solved nor builded.
		*/
		FMTLpModel getLocalModel(FMTModel localmodel = FMTModel(),int period = 0) const;
		// DocString: FMTLpModel::getObjValue
		/**
		Get the objective value of the solved matrix.
		*/
		double getObjValue() const;
		// DocString: FMTLpModel::writeLP
		/**
		Write the solverinterface matrix to a file (location) using the lp formulation.
		*/
		void writeLP(const std::string& location);
		// DocString: FMTLpModel::writeMPS
		/**
		Write the solverinterface matrix to a file (location) using the MPS formulation.
		*/
		void writeMPS(const std::string& location);
		// DocString: FMTLpModel::clone
		/**
		Get a clone of the FMTLpModel
		*/
		virtual std::unique_ptr<FMTModel>clone() const;
		// DocString: FMTLpModel::presolve
		/**
		This function use a vector of developments and the actual transitions of the model and return new unique pointer to presolved FMTModel.
		The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data if the model is badly formulated.
		*/
		virtual std::unique_ptr<FMTModel>presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
		// DocString: FMTModel::build
		/**
		This function build the graph and setSolution if (schedules) are passed to the function. If FORCE_PARTIAL_BUILD is set at true 
		and (schedules) are passed to the function, only a partial graph will be build for the number of period corresponding to schedules passed.

		It will be possible in the futur to have a mix in fullgraph and schedulegraph, but now its not implemented
		*/
		virtual bool build(std::vector<Core::FMTSchedule> schedules=std::vector<Core::FMTSchedule>());
		// DocString: FMTModel::solve
		/**
		This function call initialSolve on the solver.
		*/
		virtual bool solve();
		// DocString: FMTLpModel::getCopy
		/**
		This function returns a copy of the FMTModel of the selected period.
		If period = 0 it returns the FMTModel::getCopy if period > 0 then it returns
		a copy of the FMTModel based on the developments of the FMTGraph of the FMTLpModel.
		Need to have a builded graph with a solution to use this function.
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
		@param[in] p_constraint the constraint we want the iterator.
		@return the const iterator of the constraint.
		*/
		std::vector<Core::FMTConstraint>::const_iterator _getConstraintIndex(const Core::FMTConstraint& p_constraint) const;
		// DocString: FMTLpModel::_getsetConstraintIndex
		/**
		@brief Get or set a constraint and get it's iterator
		@param[in] p_constraint the constraint to getset
		@return the const iterator of the constraint.
		*/
		std::vector<Core::FMTConstraint>::const_iterator _getsetConstraintIndex(const Core::FMTConstraint& p_constraint);
		// DocString: FMTLpModel::_getIndex
		/**
		@brief get the index of a const iterator of constraint
		@param[in] p_it the constraint iterator.
		@return the index of the constraint.
		*/
		int _getIndex(const std::vector<Core::FMTConstraint>::const_iterator& p_it) const;
		// DocString: FMTLpModel::_setGraphCache
		/**
		@brief reserve memory for vertices in the graph.
		@param[in] p_noLength if true wont use the length of the actual model (base guess).
		*/
		void _setGraphCache(bool p_noLength = false);
		// DocString: FMTLpModel::_setConstraintsCache
		/**
		@brief set the size of the cache of the constraints.
		*/
		void _setConstraintsCache();
		// DocString: FMTLpModel::serialize
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		Using an inventory output (areaoutput) and an (statisticoutput) at (period) this function returns
		operating area cluster filled with statistic double comming from the output for a given period.
		*/
		Heuristics::FMTOperatingAreaClusterer getClusterer(
			const std::vector<Heuristics::FMTOperatingAreaCluster>& initialcluster,
			const Core::FMTOutput& areaoutput,
			const Core::FMTOutput& statisticoutput,
			const int& period, int minimalnumberofclusters = -1, int maximalnumberofclusters = -1) const;
		// DocString: FMTLpModel::getsetMatrixElement
		/**
		When the user add constraints using the setConstraint function or the setObjective function the model needs to had
		variables and/or constraints to the matrix to satisfy the FMTConstraint. Each type a variable or constraint need to be added
		to the matrix the function is called and return the index of the element if it exists (already in matrix) or not (new element).
		*/
		int getsetMatrixElement(const std::vector<Core::FMTConstraint>::const_iterator& p_constraintId,
			const FMTmatrixelement& element_type, const std::map<int, double>& indexes,
			int period = -1,
			double lowerbound = std::numeric_limits<double>::min(),
			double upperbound = std::numeric_limits<double>::max());
		// DocString: FMTLpModel::getGoals
		/**
		Return goals (index) if it already exist within the other constraints (goals (goalsnames) can be used across multiple FMTconstraints.
		*/
		bool getGoals(const std::vector<std::string>& goalsnames,
			std::map<int, double>& index, const double& sense) const;
		// DocString: FMTLpModel::getLevelFromLevelName
		/**
		Will return the level index if non empty constraint it wont check for this constraint.
		*/
		int getLevelFromLevelName(const std::string& variable_level, int period,
			const std::vector<Core::FMTConstraint>::const_iterator& p_it) const;
		// DocString: FMTLpModel::getSetLevel
		/**
		Will check if the level (variable_level) already exist within the matrix for other constraints than the (constraint)
		for a given period.
		*/
		int getSetLevel(const std::vector<Core::FMTConstraint>::const_iterator& p_it,
						const std::string& variable_level, int period);
		// DocString: FMTLpModel::getMatrixElement
		/**
		Return all the elements (level / constraint / variable) related to a given (constraint) for a period
		for a given period.
		*/
		const std::vector<int> getMatrixElement(const std::vector<Core::FMTConstraint>::const_iterator& it,
												int p_period,
												FMTmatrixelement p_element) const;
		std::vector<int>& getMatrixElementRef(const std::vector<Core::FMTConstraint>::const_iterator& it,
			int p_period,
			FMTmatrixelement p_element);
		// DocString: FMTLpModel::locateLevels
		/**
		For a given period lookup in the graph to fill the variables map (variables) for a given level (nodes).
		*/
		void locateLevels(const std::vector<Core::FMTOutputNode>& nodes, int period,
			std::map<int, double>& variables, const std::vector<Core::FMTConstraint>::const_iterator& p_it,
			double multiplier = 1);
		// DocString: FMTLpModel::locateNodes
		/**
		For a given period lookup in the graph to fill the variables map (variables) for a given FMTconstraints (nodes).
		Also apply the multiplier to coefficiants of the map the map<variableindex,coefficiants>. If there is some outputs with negative coef,
		return a vector containing a map (variables for each outputs) if the model parameter STRICTLY_POSITIVE is true.
		*/
		std::vector<std::map<int, double>> locateNodes(const std::vector<Core::FMTOutputNode>& nodes, int period, std::map<int, double>& variables, double multiplier = 1) const;
		// DocString: FMTLpModel::updateMatrixElements
		/**
		When the erasePeriod function is called the matrix size is shrinked and the variables/constraints indexes have
		to be updated. This function use the deletedconstraints and deletedvariables private member to update the indexes of
		a given matrix elements.
		*/
		void updateMatrixElements(std::vector<int>& matrixelements, const std::vector<int>& deletedelements) const;
		// DocString: FMTLpModel::updateConstraintsMapping
		/**
		When the erasePeriod function is called the matrix size is shrinked and the variables/constraints indexes have
		to be updated. This function update the indexes of all the FMTconstraints of the elements calling updateMatrixElements().
		*/
		void updateConstraintsMapping(const std::vector<int>& Dvariables, const std::vector<int>& Dconstraints);
		// DocString: FMTLpModel::updateConstraintsMapping
		/**
		When the erasePeriod function is called the matrix size is shrinked and the variables/constraints indexes have
		to be updated. This function update the indexes of all the FMTconstraints of the elements
		and also the FMTDevelopment constraints and variables of in the graph and delete those variables and constraints
		from the solverinterface matrix.
		*/
		bool updateMatrixNGraph(bool updategraph = true);
		// DocString: FMTLpModel::getAmountOfPaths
		/**
		Get the number of possible paths if an action is commited on a development.
		*/
		//size_t getAmountOfPaths(const Core::FMTDevelopment& dev, const int& actionid) const;
		// DocString: FMTLpModel::isMatrixElement
		/**
		Check if the FMTConstraint had a element of (element_type) located in the matrix for a given period.
		*/
		bool isMatrixElement(const std::vector<Core::FMTConstraint>::const_iterator& p_constraintId,
			const FMTmatrixelement& element_type, int period) const;
		// DocString: FMTLpModel::containsMatrixElements
		/**
		Check if the FMTConstraint have elements related to it.
		*/
		bool containsMatrixElements(const std::vector<Core::FMTConstraint>::const_iterator& p_constraintId, int period) const;
		// DocString: FMTLpModel::isSameMatrixElement
		/**
		Check if the requested matrix element (matrixindex) is the same as the found one (LB/UB + variables).
		*/
		bool isSameMatrixElement(const int& matrixindex, const FMTmatrixelement& element_type,
			const double& lowerb, const double& upperb, const std::map<int, double>& variables) const;

		// DocString: FMTLpModel::getobjectivebounds
		/**
		Returns constraints indexes of the lower and upper bounds of the constraints set for the objective.
		*/
		std::vector<int>setObjectiveBounds(bool dolower = true, bool doupper = true, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTLpModel::updateMatrixNaming
		/**
		Update the row and variables name based on graph.
		*/
		void updateMatrixNaming();
		// DocString: FMTLpModel::updateGeneralConstraintsNaming
		/**
		Update the row and variables using the general constraints of the model.
		*/
		void updateGeneralConstraintsNaming(std::vector<std::string>& colnames,
			std::vector<std::string>& rownames) const;
		// DocString: FMTLpModel::setPositiveOutputsInMatrix
		/**
		Set the variables in the map (strictlypositivesoutputs) to be >=0 int the matrix.
		*/
		bool setPositiveOutputsInMatrix(const std::vector<Core::FMTConstraint>::const_iterator& p_it,
			const std::vector<std::map<int, double>>& strictlypositivesoutputs, int period);
		virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);
		// DocString: FMTLpModel::FMTLpModel(const FMTSrModel&)
		/**
		Constructor to presolve FMTLpModel.
		*/
		FMTLpModel(const FMTSrModel& rhs);
		// DocString: FMTLpModel::trySetSolution(const std::vector<Core::FMTSchedule>&)
		/**
		Try to setSolution, if not possible try setSolutionByLp
		*/
		bool trySetSolution(const std::vector<Core::FMTSchedule>& schedules);
		
	};

}

BOOST_CLASS_EXPORT_KEY(Models::FMTLpModel)

#endif
#endif
