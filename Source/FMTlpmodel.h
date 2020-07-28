/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#ifndef FMTlpmodel_H_INCLUDED
#define FMTlpmodel_H_INCLUDED

#include "FMTgraphdescription.h"
#include "FMTdevelopment.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMTyields.h"
#include "FMTtheme.h"
#include "FMTschedule.h"
#include "FMTutility.h"
#include "FMTdevelopmentpath.h"
#include "FMTserializablematrix.h"

#ifdef FMTWITHMOSEK
	/*
	#include "OsiCpxSolverInterface.hpp"
	#include "OsiGrbSolverInterface.hpp"*/
	#include "OsiMskSolverInterface.hpp"
#endif

#include "OsiSolverInterface.hpp"
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include "FMTgraphstats.h"
#include <memory>
#include <tuple>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <queue>
#include "FMTgraph.h"
#include "FMTmodel.h"
#include "FMTlpsolver.h"
#include "FMTsolverinterface.h"
#include "FMToperatingareascheduler.h"
#include "FMToperatingareaclusterer.h"
#include <boost/serialization/export.hpp>


namespace Models
{
// DocString: FMTmatrixelement
/**
enum for matrix element type  present within the matrix
elements private member of the FMTlpmodel uses those enumerators to
locate constraints/objectives/levels in the matrix for each period.
*/
enum FMTmatrixelement
	{
	goalvariable=0,//period base
	levelvariable=1,//general
	objectivevariable=2,//general
	constraint=3,//period base...
	nr_items=4
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

class FMTlpmodel : public FMTmodel, public FMTlpsolver
	{
	// DocString: FMTlpmodel::save
	/**
	Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
		{
		ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
		ar & boost::serialization::make_nvp("lpsolve", boost::serialization::base_object<FMTlpsolver>(*this));
		ar & BOOST_SERIALIZATION_NVP(graph);
		ar & BOOST_SERIALIZATION_NVP(elements);
		//ar & BOOST_SERIALIZATION_NVP(deletedconstraints);
		//ar & BOOST_SERIALIZATION_NVP(deletedvariables);
		}
	// DocString: FMTlpmodel::load
	/**
	Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
		ar & boost::serialization::make_nvp("lpsolve", boost::serialization::base_object<FMTlpsolver>(*this));
		ar & BOOST_SERIALIZATION_NVP(graph);
		ar & BOOST_SERIALIZATION_NVP(elements);
		this->passinmessagehandler(*this->_logger);
		//ar & BOOST_SERIALIZATION_NVP(deletedconstraints);
		//ar & BOOST_SERIALIZATION_NVP(deletedvariables);
		}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	// DocString: FMTlpmodel::graph
	///graph holding the FMTdevelopments for all the periods.
	Graph::FMTgraph graph;
	// DocString: FMTlpmodel::elements
	///Locations of the constraints and variables in the matrix for the constraints / objective.
	std::vector<std::unordered_map<size_t,
		std::vector<std::vector<int>>>>elements;
	// DocString: FMTlpmodel::deletedconstraints
	///Deleted constraints used in replanning context when the constraints indexes need to be updated.
	//std::vector<int>deletedconstraints;
	// DocString: FMTlpmodel::deletedvariables
	///Deleted variables used in replanning context when the variables indexes need to be updated.
	//std::vector<int>deletedvariables;
	// DocString: FMTlpmodel::summarize
	/**
	Simple function to summarize constraints that are un a map structure key = variables, element = coefficiant
	to a array structure (vector) for osisolverinterface. map structure is easier to deal with thant two vectors.
	*/
	bool summarize(const std::map<int, double>& variables ,
		std::vector<int>& sumvariables, std::vector<double>& sumcoefficiants) const;
	// DocString: FMTlpmodel::getclusterer
	/**
	Using an inventory output (areaoutput) and an (statisticoutput) at (period) this function returns operating area cluster filled with statistic double comming from
	the output for a given period.
	*/
	Heuristics::FMToperatingareaclusterer getclusterer(
		const std::vector<Heuristics::FMToperatingareacluster>& initialcluster,
		const Core::FMToutput& areaoutput,
		const Core::FMToutput& statisticoutput,
		const int& period) const;
	// DocString: FMTlpmodel::initializematrix
	/**
	Initialize the solverinterface called once when the FMTgraph was empty after the first call of buildperiod.
	*/
	Graph::FMTgraphstats initializematrix();
	// DocString: FMTlpmodel::updatematrix
	/**
	During a call to build period after the graph has been updated with nes developments type the solverinterface matrix
	need to be updated. Variables and constraints related to each of those new developements will be added to the matrix.
	So area transfer row and natural growth plus action variables.
	*/
	Graph::FMTgraphstats updatematrix(const std::unordered_map<size_t, Graph::FMTvertex_descriptor>& targets,
			const Graph::FMTgraphstats& newstats);
	// DocString: FMTlpmodel::getsetmatrixelement
	/**
	When the user add constraints using the setconstraint function or the setobjective function the model needs to had
	variables and/or constraints to the matrix to satisfy the FMTconstraint. Each type a variable or constraint need to be added
	to the matrix the function is called and return the index of the element if it exists (already in matrix) or not (new element).
	*/
	int getsetmatrixelement(const Core::FMTconstraint& constraint,
                     const FMTmatrixelement& element_type, const std::map<int, double>& indexes,
                     int period = -1,
                     double lowerbound = COIN_DBL_MIN,double upperbound = COIN_DBL_MAX);
	// DocString: FMTlpmodel::getgoals
	/**
	Return goals (index) if it already exist within the other constraints (goals (goalsnames) can be used across multiple FMTconstraints.
	*/
    bool getgoals(const std::vector<std::string>& goalsnames, std::map<int,double>& index,const double& sense) const;
	// DocString: FMTlpmodel::getsetlevel
	/**
	Will check if the level (variable_level) already exist within the matrix for other constraints than the (constraint)
	for a given period.
	*/
    int getsetlevel(const Core::FMTconstraint& constraint,const std::string& variable_level,int period);
	// DocString: FMTlpmodel::getmatrixelement
	/**
	Return all the elements (level / constraint / variable) related to a given (constraint) for a period
	for a given period.
	*/
	std::vector<std::vector<int>>getmatrixelement(const Core::FMTconstraint& constraint,int period) const;
	// DocString: FMTlpmodel::locatelevels
	/**
	For a given period lookup in the graph to fill the variables map (variables) for a given level (nodes).
	*/
    void locatelevels(const std::vector<Core::FMToutputnode>& nodes,int period, std::map<int, double>& variables,const Core::FMTconstraint& constraint);
	// DocString: FMTlpmodel::locatenodes
	/**
	For a given period lookup in the graph to fill the variables map (variables) for a given FMTconstraints (nodes).
	Also apply the multiplier to coefficiants of the map the map<variableindex,coefficiants>.
	*/
	bool locatenodes(const std::vector<Core::FMToutputnode>& nodes, int period, std::map<int, double>& variables,double multiplier = 1) const;
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
	void updateconstraintsmapping(const std::vector<int>& Dvariables,const std::vector<int>& Dconstraints);
	// DocString: FMTlpmodel::updateconstraintsmapping
	/**
	When the eraseperiod function is called the matrix size is shrinked and the variables/constraints indexes have
	to be updated. This function update the indexes of all the FMTconstraints of the elements
	and also the FMTdevelopement constraints and variables of in the graph and delete those variables and constraints
	from the solverinterface matrix.
	*/
	bool updatematrixngraph();
	// DocString: FMTlpmodel::ismatrixelement
	/**
	Check if the FMTconstraint had a element of (element_type) located in the matrix for a given period.
	*/
	bool ismatrixelement(const Core::FMTconstraint& constraint,
			const FMTmatrixelement& element_type, int period) const;
	// DocString: FMTlpmodel::issamematrixelement
	/**
	Check if the requested matrix element (matrixindex) is the same as the found one (LB/UB + variables).
	*/
	bool issamematrixelement(const int& matrixindex, const FMTmatrixelement& element_type,
			const double& lowerb, const double& upperb, const std::map<int, double>& variables) const;
	// DocString: FMTlpmodel::eraseallconstraint
	/**
	Erase all constraints / variables related to a FMTconstraint for all the planning horizon (graph length).
	*/
	Graph::FMTgraphstats eraseallconstraint(const Core::FMTconstraint& constraint);
	// DocString: FMTlpmodel::getobjectivebounds
	/**
	Returns constraints indexes of the lower and upper bounds of the constraints set for the objective.
	*/
	std::vector<int>setobjectivebounds(bool dolower = true, bool doupper = true, double tolerance = FMT_DBL_TOLERANCE);
	public:
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
		bool initialsolve() final;
		// DocString: FMTlpmodel::setsolution
		/**
		If the user wants to set a solution for a given period for warmstarting the model or prepare to
		bound the model to that solution.
		*/
		bool setsolution(int period, const Core::FMTschedule& schedule);
		// DocString: FMTlpmodel::boundsolution
		/**
		This function bounds the primal variables to the primal solution present within the matrix for
		a given period and tolerance. Perfect function to update a FMTlpmodel or get ready for replanning.
		*/
		bool boundsolution(int period,double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTlpmodel::unboundsolution
		/**
		Unbound the primal bounds of a given period.
		*/
		bool unboundsolution(int period);
		// DocString: FMTlpmodel::isperiodbounded
		/**
		Check if FMTdevelopment area are bounded on there primal variables for a given period.
		*/
		bool isperiodbounded(int period) const;
		// DocString: FMTlpmodel::getsolution
		/**
		Get the standard solution for a given period (FMTschedule dont have natural growth solution included).
		*/
		Core::FMTschedule getsolution(int period) const;
		// DocString: FMTlpmodel::getstats
		/**
		Get the graph stats of the graph and matrix (number of columns/rows/edges/verticies...)
		*/
		Graph::FMTgraphstats getstats() const;
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
		// DocString: FMTlpmodel::getoutput
		/**
		Get the output value of a output for a given period using the solution of the matrix.
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or developement name if level == FMToutputlevel::developpement
		*/
		std::map<std::string, double> getoutput(const Core::FMToutput& output,
			int period, Graph::FMToutputlevel level = Graph::FMToutputlevel::standard) const;
		// DocString: FMTlpmodel::buildperiod
		/**
		This function is the main function used to build the graph and the matrix.
		A call to that function add a period within the graph and the matrix of the FMTlpmodel.
		If the schedule is not empty than the model is going to be generated in partialbuild mode and
		not fullbuild mode. Partialbuild will only build the graph linked to the solution. make it perfect to
		reinterpret outputs for a given solution without rebuilding the whole graph.
		*/
		Graph::FMTgraphstats buildperiod(Core::FMTschedule schedule = Core::FMTschedule(),
			bool forcepartialbuild = false);
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
		Erase a constraint for a given period in the matrix and in the model elements.
		*/
		Graph::FMTgraphstats eraseconstraint(const Core::FMTconstraint& constraint, int period);
		// DocString: FMTlpmodel::getvariabilities
		/**
		Get the variability of multiple outputs for a given tolerance across the planning horizon.
		Need to call this function after initialsolve.
		The actual objective function is transform to a constraint and then each output
		are minimized and maximized to get their actual range for each period.
		the returned map key are the name of the output plus UB for upper bound (maximization) and
		LB for lower bound (minimization). This function  do alot of resolve.
		*/
		std::map<std::string, std::vector<double>>getvariabilities(const std::vector<Core::FMToutput>& outputs,
																double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTlpmodel::getareavariabilities
		/**
		Sometime it is usefull to know what is the impact on some outputs of changing a set of developements
		of the area of the model for a given global mask. This function uses the globalmask has target and returns
		the variabilities between the given tolerance of changing the area.
		*/
		std::map<std::string, std::vector<double>>getareavariabilities(const std::vector<Core::FMToutput>& localoutputs,
																			const Core::FMTmask& globalmask,
																			double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTlpmodel::eraseperiod
		/**
		When doing replanning or simply model update the user may want to delete the first period (front)
		of the graph and the matrix to get a FMTmodel - first period. The planning length will be shrinked to
		originalsize - 1.
		*/
		Graph::FMTgraphstats eraseperiod(bool constraintsonly = false);
		// DocString: FMTlpmodel::getfirstactiveperiod
		/**
		Return the first active period should be always 0 in case or planning.
		But when eraseperiod is called the first active period is going to move to 1 and so on.
		*/
		int getfirstactiveperiod() const;
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
																				size_t numberofheuristics=1) const;
		// DocString: FMTlpmodel::resolve
		/**
		By default call solverinterface->resolve() when some changes are done to the model.
		The user dont necessery need the call initialsolve every time the matrix has changed a call to resolve maybe enought.
		*/
		bool resolve() final;
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
		// DocString: FMTlpmodel::getarea
		/**
		This function returns an area for a given period for a FMTlpmodel.
		If period = 0 the area is the same has FMTmodel::getarea().
		For period > 0 the area returned is the FMTdevelopement of the graph
		that can be (actual of futur) existing at the beginning of the period.
		Need to have a builded graph with a solution to use this function.
		*/
		std::vector<Core::FMTactualdevelopment>getarea(int period = 0) const override;
		// DocString: FMTlpmodel::getmodel
		/**
		This function returns a copy of the FMTmodel of the selected period.
		If period = 0 it returns the FMTmodel::getcopy if period > 0 then it returns
		a copy of the FMTmodel based on the developements of the FMTgraph of the FMTlpmodel.
		Need to have a builded graph with a solution to use this function.
		*/
		FMTmodel getcopy(int period = 0) const override;
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

	};

}

BOOST_CLASS_EXPORT_KEY(Models::FMTlpmodel)

#endif
#endif
