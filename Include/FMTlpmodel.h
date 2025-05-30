/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#ifndef FMTlpmodel_H_INCLUDED
#define FMTlpmodel_H_INCLUDED

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
	class FMToperatingareascheduler;
	class FMToperatingareaclusterer;
	class FMToperatingareacluster;
	class FMToperatingareascheme;
}

namespace Models
{
// DocString: FMTmatrixelement
/**
Enum for matrix element type present within the matrix.
Elements private member of the FMTlpmodel uses those enumerators to
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

// DocString: FMTlpmodel
/**
This model is a type III LP forest planning model. The graph
is divided per period. This model is made for replanning and simple
forest planning LP optimization. It makes heavy use of FMTgraph
for building the matrix. FMTgraph being based on a list structure
FMTlpmodel can easely be extended (by the back) using the function buildperiod() or
shrinked (by the front) using the function eraseperiod.
The matrix is held within the solverinterface pointer.
*/

class FMTEXPORT FMTlpmodel : public FMTsrmodel
	{
	
	public:
		// DocString: FMTlpmodel::clearcache
		/**
		Remove cache to release memory.
		*/
		void clearcache();
		// DocString: FMTlpmodel::clearconstraintlocation
		/**
		Clear the localisation in the matrix...if you do that you break the relation between the FMTmodel constraints and the matrix contraints.
		*/
		void clearconstraintlocation();
		// DocString: FMTlpmodel(const FMTmodel,FMTsolverinterface)
		/**
		Main constructor used to build FMTlpmodel using it's base class and to let the user choose the solvertype
		(CLP,MOSEK,GUROBI,CPLEX) see FMTsolverinterface.h for more information about the supported solvertype.
		*/
		FMTlpmodel(const FMTmodel& base, FMTsolverinterface lsolvertype);
		// DocString: FMTlpmodel()
		/**
		Default constructor of FMTlpmodel
		*/
		FMTlpmodel();
		// DocString: FMTlpmodel(const FMTmodel, const Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>, const FMTlpsolver, const std::vector<std::unordered_map<std::string,std::vector<std::vector<int>>>>)
		/**
		Constructor of FMTlpmodel mainly use in postsolve to pass each attributes of the class.
		*/
		/*FMTlpmodel(const FMTmodel& base, const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& lgraph,
					const FMTlpsolver& lsolver,const std::vector<std::unordered_map<std::string,std::vector<std::vector<int>>>>& lelements);*/
		// DocString: FMTlpmodel(const FMTlpmodel)
		/**
		Copy constructor of FMTlpmodel
		*/
		FMTlpmodel(const FMTlpmodel& rhs);
		// DocString: FMTlpmodel::initialsolve
		/**
		Cold start of the LPsolve of a simple LP model.
		By default initialsolve will call solverinterface->initialsolve() but using the FMTsolverinterface enum
		this function will try to use the best solver parameters for a Type III Forest planning model.
		For all solvers interior point is considered the best algorith.
		*/
		bool initialsolve();
		// DocString: FMTlpmodel(FMTlpmodel&&)
		/**
		Default move constructor for FMTlpmodel.
		*/
		FMTlpmodel(FMTlpmodel&& rhs);
		// DocString: FMTlpmodel::operator=(FMTlpmodel&& rhs) 
		/**
		Default move assignment for FMTlpmodel.
		*/
		FMTlpmodel& operator =(FMTlpmodel&& rhs);
		// DocString: FMTlpmodel::setstrictlypositivesoutputsmatrix
		/**
		To set the model parameter STRICTLY_POSITIVE at true.
		*/
		void setstrictlypositivesoutputsmatrix();
		// DocString: FMTlpmodel::getoutput
		/**
		Get the output value of a output for a given period using the solution of the matrix.
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or developement name if level == FMToutputlevel::developpement, this function can also return the value of
		a variable output.
		*/
		virtual std::map<std::string, double> getoutput(const Core::FMToutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		// DocString: FMTlpmodel::getreplanningconstraints
		/**
		During replaning some local/global constraints need to be ajusted to the global model.
		The function will take a vector of local/global constraint and ajust the bounds using the solution of this global/local model.
		*/
		virtual std::vector<Core::FMTconstraint> getreplanningconstraints(const std::string& modeltype, const std::vector<Core::FMTconstraint>& localconstraints, const int& period) const;
		// DocString: FMTlpmodel::addscheduletoobjective
		/**
		This function will addup weight to the objective function for a given schedule.
		*/
		void addscheduletoobjective(const Core::FMTschedule& schedule,double weight = 1000);
		// DocString: FMTlpmodel::operator==
		/**
		Comparison operator of FMTlpmodel
		*/
		bool operator == (const FMTlpmodel& rhs) const;
		// DocString: FMTlpmodel::operator!=
		/**
		Comparison operator of FMTlpmodel
		*/
		bool operator != (const FMTlpmodel& rhs) const;
		// DocString: FMTlpmodel::setobjective
		/**
		This function set the objective of the matrix for the whole planning horizon (graph length).
		Note that the objective function is always the firts constraint in the constraints vector of a
		FMTmodel. If a objective was already set before it will replace it when calling this function.
		*/
		Graph::FMTgraphstats setobjective(const Core::FMTconstraint& objective);
		// DocString: FMTlpmodel::setconstraint
		/**
		This function set a constraint in the matrix for the whole planning horizon (graph length).
		If the function is recalled if the constraint already exist in the matrix in wont be replaced.
		Can be called after calling builperiod in replanning.
		*/
		Graph::FMTgraphstats setconstraint(const Core::FMTconstraint& constraint);
		// DocString: FMTlpmodel::eraseconstraint
		/**
		Erase a constraint for a given period in the matrix and in the model elements. If period == -1 then it will erase
		the constraint for the whole horizon else it will only erase the constraint for the specified period.
		*/
		Graph::FMTgraphstats eraseconstraint(const Core::FMTconstraint& constraint,int period=-1);
		// DocString: FMTlpmodel::getvariabilities
		/**
		Get the variability of multiple outputs for a given tolerance(see modelparameters) for each period between (periodstart) and (periodstop).
		Need to call this function after initialsolve.
		The actual objective function is transform to a constraint and then each output
		are minimized and maximized to get their actual range for each period.
		the returned map key are the name of the output plus UB for upper bound (maximization) and
		LB for lower bound (minimization). This function  do a lot of resolve.
		*/
		std::map<std::string, std::vector<double>>getvariabilities(const std::vector<Core::FMToutput>& outputs,
																 const int& periodstart,const int& periodstop);
		// DocString: FMTlpmodel::getmodelfromproportions
		/**
		Will returns a new FMTlpmodel with changes in the initial area based on the tolerances of each globalmasks
		provided in the tolerances vector tolerance > 0 = increase in area, tolerance < 0 = decrease in area.
		*/
		FMTlpmodel getmodelfromproportions(const std::vector<Core::FMTmask>& globalmasks,
											std::vector<double> tolerances) const;
		// DocString: FMTlpmodel::getareavariabilities
		/**
		Sometime it is usefull to know what is the impact on some outputs of changing a set of developements
		of the area of the model for a given global mask. This function uses the globalmasks has target and returns
		the change in outputs resulting from a variation > 0 or < 0, will generate no map key if infeasible.
		*/
		std::map<std::string, std::vector<double>>getareavariabilities(const std::vector<Core::FMToutput>& localoutputs,
				const std::vector<Core::FMTmask>& globalmasks,
				std::vector<double> tolerances = std::vector<double>()) const;
		// DocString: FMTlpmodel::eraseperiod
		/**
		When doing replanning or simply model update the user may want to delete the first period (front)
		of the graph and the matrix to get a FMTmodel - first period. The planning length will be shrinked to
		originalsize - 1.
		*/
		Graph::FMTgraphstats eraseperiod(bool constraintsonly = false);
		// DocString: FMTlpmodel::getoperatingareaschedulerheuristics
		/**
		Using multiple operating areas and a simple output node a MIP formulation (using the BFECopt heuristic) is done using the matrix of
		the FMTlpmodel. The matrix can be copied within the operatingareaheuristic or directly uses the matrix
		of the FMTlpmodel (if numberofheuristics == 1 and copysolver == true). The user can than decide to change
		some parameters of heuristics that are in the vectors.
		*/
		std::vector<Heuristics::FMToperatingareascheduler>getoperatingareaschedulerheuristics(const std::vector<Heuristics::FMToperatingareascheme>& opareas,
																				const Core::FMToutputnode& node,
																				size_t numberofheuristics=1,
																				bool copysolver=true);
        // DocString: FMTlpmodel::getoperatingareaclustererheuristics
		/**
		Using multiple operating areas this function generates multiples heuristics for solving the operating area clustering problem.
        The user can decide to change some parameters of heuristics that are in the vectors.The FMTlpmodel has to be solved so the heuristic can use
        the value of the (output) of a given (period) has a statistic to perform clustering.
		*/
		std::vector<Heuristics::FMToperatingareaclusterer>getoperatingareaclustererheuristics(const std::vector<Heuristics::FMToperatingareacluster>& clusters,
																				const Core::FMToutput& statisticoutput,
                                                                                const Core::FMToutput& areaoutput,
                                                                                const int& period,
																				size_t numberofheuristics=1,
																				int minimalnumberofclusters = -1,
																				int maximalnumberofclusters = -1) const;
		// DocString: FMTlpmodel::resolve
		/**
		By default call solverinterface->resolve() when some changes are done to the model.
		The user dont necessery need the call initialsolve every time the matrix has changed a call to resolve maybe enought.
		*/
		bool resolve();
		// DocString: FMTlpmodel::operator=
		/**
		Copy assignment of FMTlpmodel
		*/
		FMTlpmodel& operator = (const FMTlpmodel& rhs);
		// DocString: ~FMTlpmodel()
		/**
		Default destructor of FMTlpmodel
		*/
		~FMTlpmodel() = default;
		// DocString: FMTlpmodel::getlpmodel
		/**
		This fonction is based on model hierarchy. The *this FMTlpmodel is considered has the
		general parent model the localmodel if not empty is considered one part of the child model.
		The function is going to append the child model to the subset of the general model
		generated by the selected period.
		Need to have a builded graph with a solution to use this function.
		The returned model wont be solved nor builded.
		*/
		FMTlpmodel getlocalmodel(FMTmodel localmodel = FMTmodel(),int period = 0) const;
		// DocString: FMTlpmodel::getObjValue
		/**
		Get the objective value of the solved matrix.
		*/
		double getObjValue() const;
		// DocString: FMTlpmodel::writeLP
		/**
		Write the solverinterface matrix to a file (location) using the lp formulation.
		*/
		void writeLP(const std::string& location);
		// DocString: FMTlpmodel::writeMPS
		/**
		Write the solverinterface matrix to a file (location) using the MPS formulation.
		*/
		void writeMPS(const std::string& location);
		// DocString: FMTlpmodel::clone
		/**
		Get a clone of the FMTlpmodel
		*/
		virtual std::unique_ptr<FMTmodel>clone() const;
		// DocString: FMTlpmodel::presolve
		/**
		This function use a vector of developments and the actual transitions of the model and return new unique pointer to presolved FMTmodel.
		The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data if the model is badly formulated.
		*/
		virtual std::unique_ptr<FMTmodel>presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments = std::vector<Core::FMTactualdevelopment>()) const;
		// DocString: FMTmodel::build
		/**
		This function build the graph and setsolution if (schedules) are passed to the function. If FORCE_PARTIAL_BUILD is set at true 
		and (schedules) are passed to the function, only a partial graph will be build for the number of period corresponding to schedules passed.

		It will be possible in the futur to have a mix in fullgraph and schedulegraph, but now its not implemented
		*/
		virtual bool build(std::vector<Core::FMTschedule> schedules=std::vector<Core::FMTschedule>());
		// DocString: FMTmodel::solve
		/**
		This function call initialsolve on the solver.
		*/
		virtual bool solve();
		// DocString: FMTlpmodel::getcopy
		/**
		This function returns a copy of the FMTmodel of the selected period.
		If period = 0 it returns the FMTmodel::getcopy if period > 0 then it returns
		a copy of the FMTmodel based on the developements of the FMTgraph of the FMTlpmodel.
		Need to have a builded graph with a solution to use this function.
		*/
		virtual std::unique_ptr<FMTmodel> getcopy(int period = 0) const;
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
		// DocString: FMTlpmodel::m_rowsAllocator
		///The int allocator for the constraintIndex
		mutable std::allocator<int>m_rowsAllocator;
		// DocString: FMTlpmodel::m_indexAllocator
		///the constraints index allocator
		mutable std::allocator<ConstraintIndex>m_indexAllocator;
		// DocString: FMTlpmodel::m_indexes
		///Locations of the constraints and variables in the matrix for the constraints / objective.
		std::map<ConstraintIndex,std::vector<int>>m_indexes;
		// DocString: FMTlpmodel::_getConstraintIndex
		/**
		@brief Get the constraint index iterator of a constraint.
		@param[in] p_constraint the constraint we want the iterator.
		@return the const iterator of the constraint.
		*/
		std::vector<Core::FMTconstraint>::const_iterator _getConstraintIndex(const Core::FMTconstraint& p_constraint) const;
		// DocString: FMTlpmodel::_getsetConstraintIndex
		/**
		@brief Get or set a constraint and get it's iterator
		@param[in] p_constraint the constraint to getset
		@return the const iterator of the constraint.
		*/
		std::vector<Core::FMTconstraint>::const_iterator _getsetConstraintIndex(const Core::FMTconstraint& p_constraint);
		// DocString: FMTlpmodel::_getIndex
		/**
		@brief get the index of a const iterator of constraint
		@param[in] p_it the constraint iterator.
		@return the index of the constraint.
		*/
		int _getIndex(const std::vector<Core::FMTconstraint>::const_iterator& p_it) const;
		// DocString: FMTlpmodel::_setGraphCache
		/**
		@brief reserve memory for verticies in the graph.
		@param[in] p_noLength if true wont use the length of the actual model (base guess).
		*/
		void _setGraphCache(bool p_noLength = false);
		// DocString: FMTlpmodel::_setConstraintsCache
		/**
		@brief set the size of the cache of the constraints.
		*/
		void _setConstraintsCache();
		// DocString: FMTlpmodel::serialize
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTsrmodel>(*this));
			ar& BOOST_SERIALIZATION_NVP(m_indexes);
		}
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTsrmodel>(*this));
			ar& BOOST_SERIALIZATION_NVP(m_indexes);
		}
		// DocString: FMTlpmodel::elements
		///Locations of the constraints and variables in the matrix for the constraints / objective.
		//std::vector<std::unordered_map<std::string,
		//	std::vector<std::vector<int>>>>elements;
		// DocString: FMTlpmodel::getclusterer
		/**
		Using an inventory output (areaoutput) and an (statisticoutput) at (period) this function returns
		operating area cluster filled with statistic double comming from the output for a given period.
		*/
		Heuristics::FMToperatingareaclusterer getclusterer(
			const std::vector<Heuristics::FMToperatingareacluster>& initialcluster,
			const Core::FMToutput& areaoutput,
			const Core::FMToutput& statisticoutput,
			const int& period, int minimalnumberofclusters = -1, int maximalnumberofclusters = -1) const;
		// DocString: FMTlpmodel::getsetMatrixElement
		/**
		When the user add constraints using the setconstraint function or the setobjective function the model needs to had
		variables and/or constraints to the matrix to satisfy the FMTconstraint. Each type a variable or constraint need to be added
		to the matrix the function is called and return the index of the element if it exists (already in matrix) or not (new element).
		*/
		int getsetMatrixElement(const std::vector<Core::FMTconstraint>::const_iterator& p_constraintId,
			const FMTmatrixelement& element_type, const std::map<int, double>& indexes,
			int period = -1,
			double lowerbound = std::numeric_limits<double>::min(),
			double upperbound = std::numeric_limits<double>::max());
		// DocString: FMTlpmodel::getgoals
		/**
		Return goals (index) if it already exist within the other constraints (goals (goalsnames) can be used across multiple FMTconstraints.
		*/
		bool getgoals(const std::vector<std::string>& goalsnames,
			std::map<int, double>& index, const double& sense) const;
		// DocString: FMTlpmodel::getlevelfromlevelname
		/**
		Will return the level index if non empty constraint it wont check for this constraint.
		*/
		int getlevelfromlevelname(const std::string& variable_level, int period,
			const std::vector<Core::FMTconstraint>::const_iterator& p_it) const;
		// DocString: FMTlpmodel::getsetlevel
		/**
		Will check if the level (variable_level) already exist within the matrix for other constraints than the (constraint)
		for a given period.
		*/
		int getsetlevel(const std::vector<Core::FMTconstraint>::const_iterator& p_it,
						const std::string& variable_level, int period);
		// DocString: FMTlpmodel::getMatrixElement
		/**
		Return all the elements (level / constraint / variable) related to a given (constraint) for a period
		for a given period.
		*/
		const std::vector<int> getMatrixElement(const std::vector<Core::FMTconstraint>::const_iterator& it,
												int p_period,
												FMTmatrixelement p_element) const;
		std::vector<int>& getMatrixElementRef(const std::vector<Core::FMTconstraint>::const_iterator& it,
			int p_period,
			FMTmatrixelement p_element);
		// DocString: FMTlpmodel::locatelevels
		/**
		For a given period lookup in the graph to fill the variables map (variables) for a given level (nodes).
		*/
		void locatelevels(const std::vector<Core::FMToutputnode>& nodes, int period,
			std::map<int, double>& variables, const std::vector<Core::FMTconstraint>::const_iterator& p_it,
			double multiplier = 1);
		// DocString: FMTlpmodel::locatenodes
		/**
		For a given period lookup in the graph to fill the variables map (variables) for a given FMTconstraints (nodes).
		Also apply the multiplier to coefficiants of the map the map<variableindex,coefficiants>. If there is some outputs with negative coef,
		return a vector containing a map (variables for each outputs) if the model parameter STRICTLY_POSITIVE is true.
		*/
		std::vector<std::map<int, double>> locatenodes(const std::vector<Core::FMToutputnode>& nodes, int period, std::map<int, double>& variables, double multiplier = 1) const;
		// DocString: FMTlpmodel::updatematrixelements
		/**
		When the eraseperiod function is called the matrix size is shrinked and the variables/constraints indexes have
		to be updated. This function use the deletedconstraints and deletedvariables private member to update the indexes of
		a given matrix elements.
		*/
		void updatematrixelements(std::vector<int>& matrixelements, const std::vector<int>& deletedelements) const;
		// DocString: FMTlpmodel::updateconstraintsmapping
		/**
		When the eraseperiod function is called the matrix size is shrinked and the variables/constraints indexes have
		to be updated. This function update the indexes of all the FMTconstraints of the elements calling updatematrixelements().
		*/
		void updateconstraintsmapping(const std::vector<int>& Dvariables, const std::vector<int>& Dconstraints);
		// DocString: FMTlpmodel::updateconstraintsmapping
		/**
		When the eraseperiod function is called the matrix size is shrinked and the variables/constraints indexes have
		to be updated. This function update the indexes of all the FMTconstraints of the elements
		and also the FMTdevelopement constraints and variables of in the graph and delete those variables and constraints
		from the solverinterface matrix.
		*/
		bool updatematrixngraph(bool updategraph = true);
		// DocString: FMTlpmodel::getamountofpaths
		/**
		Get the number of possible paths if an action is commited on a development.
		*/
		//size_t getamountofpaths(const Core::FMTdevelopment& dev, const int& actionid) const;
		// DocString: FMTlpmodel::isMatrixElement
		/**
		Check if the FMTconstraint had a element of (element_type) located in the matrix for a given period.
		*/
		bool isMatrixElement(const std::vector<Core::FMTconstraint>::const_iterator& p_constraintId,
			const FMTmatrixelement& element_type, int period) const;
		// DocString: FMTlpmodel::containsMatrixElements
		/**
		Check if the FMTconstraint have elements related to it.
		*/
		bool containsMatrixElements(const std::vector<Core::FMTconstraint>::const_iterator& p_constraintId, int period) const;
		// DocString: FMTlpmodel::issamematrixelement
		/**
		Check if the requested matrix element (matrixindex) is the same as the found one (LB/UB + variables).
		*/
		bool issamematrixelement(const int& matrixindex, const FMTmatrixelement& element_type,
			const double& lowerb, const double& upperb, const std::map<int, double>& variables) const;

		// DocString: FMTlpmodel::getobjectivebounds
		/**
		Returns constraints indexes of the lower and upper bounds of the constraints set for the objective.
		*/
		std::vector<int>setobjectivebounds(bool dolower = true, bool doupper = true, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTlpmodel::updatematrixnaming
		/**
		Update the row and variables name based on graph.
		*/
		void updatematrixnaming();
		// DocString: FMTlpmodel::updategeneralconstraintsnaming
		/**
		Update the row and variables using the general constraints of the model.
		*/
		void updategeneralconstraintsnaming(std::vector<std::string>& colnames,
			std::vector<std::string>& rownames) const;
		// DocString: FMTlpmodel::setpositiveoutputsinmatrix
		/**
		Set the variables in the map (strictlypositivesoutputs) to be >=0 int the matrix.
		*/
		bool setpositiveoutputsinmatrix(const std::vector<Core::FMTconstraint>::const_iterator& p_it,
			const std::vector<std::map<int, double>>& strictlypositivesoutputs, int period);
		virtual void swap_ptr(std::unique_ptr<FMTmodel>& rhs);
		// DocString: FMTlpmodel::FMTlpmodel(const FMTsrmodel&)
		/**
		Constructor to presolve FMTlpmodel.
		*/
		FMTlpmodel(const FMTsrmodel& rhs);
		// DocString: FMTlpmodel::trysetsolution(const std::vector<Core::FMTschedule>&)
		/**
		Try to setsolution, if not possible try setsolutionbylp
		*/
		bool trysetsolution(const std::vector<Core::FMTschedule>& schedules);
		
	};

}

BOOST_CLASS_EXPORT_KEY(Models::FMTlpmodel)

#endif
#endif
