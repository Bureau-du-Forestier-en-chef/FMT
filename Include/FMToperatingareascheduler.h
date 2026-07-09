/*
Copyright (c) 2019 Gouvernement du Qubec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#ifndef FMToperatingareascheduler_Hm_included
#define FMToperatingareascheduler_Hm_included

#include "FMTmask.h"
#include "FMToperatingareascheme.h"
#include "FMTlpheuristic.h"




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
	class FMTgraph;
}

namespace Models
{
	class FMTmodel;
}

namespace Core
{
	class FMToutputnode;
	class FMTtimeyieldhandler;
	class FMToutput;
}


namespace Heuristics
{
	// DocString: FMToperatingareascheduler
	/**
	FMToperatingareaheuristic is a heuristics made to solve spatialization problem
	across landscape for multiple operating areas. MIP Forest planning problem tend to be
	symmetrical and tought to solve using a regular MIP solver. This heuristics is made to quickly
	generate good enought starting solution for those kind of Forest management problem.
	Before using this class the user must have sets all parameters of a vector of FMToperatingarea
	so that the heuristic can sets the constraints and variables of each operating area into
	the matrix. It's up to the user to decide to just generate a good initialsolution or
	generate a good initialsolution and then try to find the optimaly using a BnB solver.
	*/
	class FMTEXPORT FMToperatingareascheduler : public FMTlpheuristic
	{
		// DocString: FMToperatingareaheuristic::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("lpsolve", boost::serialization::base_object<FMTlpheuristic>(*this));
			ar & BOOST_SERIALIZATION_NVP(operatingareas);
			ar & BOOST_SERIALIZATION_NVP(adjacencyconstraints);
			ar & BOOST_SERIALIZATION_NVP(proportionofset);
			ar & BOOST_SERIALIZATION_NVP(userandomness);
			ar & BOOST_SERIALIZATION_NVP(useprimal);
		}
		// DocString: FMToperatingareascheduler::operatingareas
		///Vector of FMToperatingarea holding all informations about each operating area coming from the user
		std::vector<FMToperatingareascheme>operatingareas;
		// DocString: FMToperatingareascheduler::adjacencyconstraints
		///Adjacency constraints indexes of all operating area the pair is used to not duplicate neighboring constraints
		std::map<std::pair<Core::FMTmask, Core::FMTmask>, std::vector<int>>adjacencyconstraints;
		// DocString: FMToperatingareascheduler::proportionofset
		///This data member gives the information about the proportion of operating area status change allowed before resolving the problem
		double proportionofset;
		// DocString: FMToperatingareascheduler::userandomness
		///If true the heuristic is going to shuffle the potential oprating area schemes and area schemes before fixing the bounds.
		bool userandomness;
		// DocString: FMToperatingareascheduler::useprimal
		///If true the heuristic will solve try to solve the problem by the primal variables else by the dual constraints.
		bool useprimal;
		// DocString: FMToperatingareascheduler::setOperatingAreasConstraints
		/**
		The function sets all the operating area constraints (changes the stade of the operating area) and adding all constraints
		and variables to the solverinterface using the FMTlpmodel graph (maingraph), it's parent class (model) and finaly using
		an output node (the output node needs to be linked to a least one action of the FMTmodel).
		*/
		void setOperatingAreasConstraints(const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& maingraph,
			const Models::FMTmodel& model,
			const Core::FMToutputnode& target);
		// DocString: FMToperatingareascheduler::setAdjacencyConstraints
		/**
		The function fills up the adjacencyconstraints data member and add these constraints to the solverinterface.
		Adjacency constraints cannot be added from each operating area thats why the constraints are directly linked to
		the FMToperatingareaheuristic and the FMToperatingarea.
		*/
		bool setAdjacencyConstraints();
		// DocString: FMToperatingareascheduler::setDraw
		/**
		Returns a vector of potential oeprating areas that have no schedule sets yet. Based on the dual or primal solution if useprimal == true.
		Depending if userandomness == false the draw is going to be sorted starting by the best operating area to set schedule
		looking at the actual primal or dual solution. If userrandomness == true the draw is going to be shuffled
		*/
		std::vector<std::vector<FMToperatingareascheme>::const_iterator> setDraw();
		// DocString: FMToperatingareascheduler::draw
		/**
		Returns a vector selected oeprating areas based on the vector passed.
		Depending if userandomness == false the draw will select the first elements until the max number of operatingarea to select is reach.
		If userrandomness == true the draw is going to be shuffled
		*/
		std::vector<std::vector<FMToperatingareascheme>::const_iterator> draw(std::vector<std::vector<FMToperatingareascheme>::const_iterator>& oparea);
		// DocString: FMToperatingareascheduler::setBounds
		/**
		Depending if we solve the dual or the primal the this function will set a  random schedule if userrandomness == true or
		simply set the best known schedule to a set of operating areas (tobound).
		If schemetoskip is not empty, for each operatingareascheme at the same index in tobound, it will try to use another schemeid.(schemestoskip) need to be the same size as (tobound) or be empty.
		*/
		size_t setBounds(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& tobound,const std::vector<int>& schemestoskip=std::vector<int>(), bool keeploose=true);
		// DocString: FMToperatingareascheduler::selectScheme
		/**
		Select a scheme in potentialscheme that is different from the schemeid to skip
		*/
		size_t selectScheme(std::vector<size_t>& potentialschemes, const int& schemetoskip) const;
		// DocString: FMToperatingareascheduler::getBounds
		/**
		Fill targeteditems and bounds according to the scheme id. If boundall == true, no scheme id is needed.
		If boundall == false, the default schemeid is 0, the user must pass the good one.
		*/
		bool getBounds(const std::vector<FMToperatingareascheme>::const_iterator& operatingareaiterator,
			std::vector<int>& targeteditems,std::vector<double>& bounds, const bool& boundall, const size_t& schemeid=0,bool keeploose=true) const;
		// DocString: FMToperatingareascheduler::unboundAll
		/**
		This function is like a reset button, it will unselect all schedules already selected for each management units
		and unbound all variables/constraints related to all operating areas in the solverinterface.
		*/
		void unboundAll(bool atprimal = false);
		// DocString: FMToperatingareascheduler::unbound
		/**
		This function is like a reset button, it will unselect all schedules already selected for management units to unbound
		and unbound all variables/constraints related to all operating areas in the solverinterface.
		*/
		void unbound(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& tounbound,bool atprimal = false);
		// DocString: FMToperatingareascheduler::closePrimalBounds
		/**
		Sets all binary variables of all operating area to 0 by fixing theirs primal bounds.
		*/
		void closePrimalBounds();
		// DocString: FMToperatingareascheduler::setAllInteger
		/**
		After solving the heuristic with initialSolve() the user may want to try to get a optimal solution or to
		valide the actual solution using BnB. This function sets all schemes variables or each operating area
		as integer variables within the solverinterface and prepare the matrix for Bnb if the solve was in dual.
		*/
		void setAllInteger();
		// DocString: FMToperatingareascheduler::resolveModel
		/**
		The function call solverinterface->resolve() by default but depending of the solvertype used it
		may use different algorithm or parameters to get a fast resolve of the problem.
		*/
		int resolveModel();
		// DocString: FMToperatingareascheduler::clearRowCache
		/**
		Function only usefull for when solvertype == mosek the row cache doesn't seems to handle realy well the heuristic.
		So the row memory cache needs to be cleared.
		*/
		void clearRowCache();
		// DocString: FMToperatingareascheduler::getSolutionIndexes
		/*
		Return the scheme index which is bounded for each operatingareascheme pass to the function in the same order.
		*/
		std::vector<int> getSolutionIndexes(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& opareaits) const;
		// DocString: FMToperatingareascheduler::getThreadId
		/*
		Return thread id to log it into functions greedyPass and initialsove
		*/
		std::string getThreadId() const;
		// DocString: FMToperatingareascheduler::completeInitialSolution
		/**
		Iterate on each FMToperatingareascheme and get the binaries (variables) and the constraints for the operatingscheme and the adjacency.
		Then get the old solution and complete the solution with binaries = 0 and complete the row price of each constraints added...
		completeInitialSolution will return 0 if something is not valid then you will need to recall initialSolve ... Need to check the return from setConstraint
		of each FMToperatingarea to know about it...After calling this function you can safely call resolve...
		*/
		bool completeInitialSolution();
		// DocString: FMToperatingareascheduler::gotRejectedNodes
		/**
		Validate if you have rejected nodes...
		*/
		bool gotRejectedNodes() const;
		// DocString: FMToperatingareascheduler::resetBaseGreedySolution
		/**
		After a greedy iteration if there's no gain replace the newly generated solution by the old one and get back to the
		base stade of the scheduler...
		*/
		void resetBaseGreedySolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected,
									const std::vector<int>& oldschemeid,
									const std::vector<int>& oldconstraints,
									const std::vector<double>& oldbounds);
		// DocString: FMToperatingareascheduler::releaseGreedySolution
		/**
		Before the greedy move you need to get the bounds and constraints informations from the selected OA and keep it in vectors
		if you want to get back to the original solution and release the bounds and resolve the model if optimal will return true,
		if not will return false
		*/
		bool releaseGreedySolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected,
			std::vector<int>& oldconstraints,
			std::vector<double>& oldbounds);
		// DocString: FMToperatingareascheduler::greedyMoveSolution
		/**
		Using the selected OA (selected) and the tabou schemeid to select (oldschemeid) place a new solution and solve the model
		return true if optimal else false.
		*/
		bool greedyMoveSolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected,
								const std::vector<int>& oldschemeid);
		// DocString: FMToperatingareascheduler::setForDualInitialSolve
		/**
		Make sure rhs are right need to be released, Need to close primal bound to get some activities and then solve to make it clean
		for a fresh initialSolve.
		*/
		void setForDualInitialSolve();
		// DocString: FMToperatingareascheduler::initialMoveSolution
		/**
		Bounds the newly selected OAs and resolve the model and return the number of iterations took to do so.
		*/
		int initialMoveSolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected);
		// DocString: FMToperatingareascheduler::resetBaseInitialSolution
		/**
		If setting the initial solution fail then you need to get back to the base form of the formulation by releasing
		all bounds and resolving.
		*/
		void resetBaseInitialSolution();
	public:
		// DocString: FMToperatingareascheduler::generateInitialProportionOfSet
		/*
		Try to calculate a good proportion of set for the initialSolve...if anything goes wrong it will be 0.25 If more complexity is
		detected using the FMToperatingareascheme getNumberofscheme vs the getNumberofsimplescheme it will get the difference and use a factor of 0.00025
		to decrease in a linear fashion the propotion of set to a minimum of 0.01.
		*/
		double generateInitialProportionOfSet() const;
		// DocString: FMToperatingareascheduler::empty
		/**
		Return true if operatingareas is empty.
		*/
		bool empty() const;
		// DocString: FMToperatingareascheduler::initialSolve
		/**
		Solve the heuristic problem using the original heuristic resolving the problem till finding a initial solution
		for each operating area. The user can use the function getSolution to first yield solution.
		*/
		bool initialSolve() final;
		// DocString: FMToperatingareascheduler::branchNBoundSolve
		/**
		Solve problem using Branch and bound on the primal formulation. If the function is called after a call to initialSolve()
		it's going to use the heuristic solution has a starting MIP solution, if not it's going to directly use the BnB on the formulated problem.
		*/
		bool branchNBoundSolve() final;
		// DocString: FMToperatingareascheduler::greedyPass
		/**

		*/
		bool greedyPass(const double& initsol, const unsigned int& iteration) final;
		// DocString: FMToperatingareascheduler::setAsRandom
		/**
		Sets True the userandomness data member
		*/
		void setAsRandom();
		// DocString: FMToperatingareascheduler::setAsPrimal
		/**
		Sets True the useprimal data member
		*/
		void setAsPrimal();
		// DocString: FMToperatingareascheduler::setProportionOfSet
		/**
		Set the allowed proportion of operating area status change allowed before resolving the problem. Default = 0.25
		*/
		void setProportionOfSet(const double& proportion);
		// DocString: FMToperatingareascheduler::getSolution
		/**
		Gets the actual solution into a FMTyieldhandler format (time yield)
		The user can decide the (yldname) to use. Each operating area will have its own
		yieldhandler. If (selectedmask) and (basethemes) are given, the yieldhandler return
		will be postsolve. To get (selectedmask), you must call the function FMTmodel::getSelectedMask()
		after presolving the model. (basethemes) are the themes of the original model. 
		*/
		std::vector<Core::FMTtimeyieldhandler> getSolution(	const std::string& yldname) const;
		// DocString: FMToperatingareascheduler::getLevelSolution
		/**
		Gets the lower bounds of the solution for each operating area get 2 outputs:
		the first one is the level and the second one is the varaible outputs that needs to be bound to.
		*/
		std::vector<Core::FMToutput>getLevelSolution(const std::string& outputname,const std::string& aggregate,int outputid) const;
		// DocString: FMToperatingareascheduler(const std::vector<FMToperatingarea>,const Graph::FMTgraph,const Models::FMTmodel,const Core::FMToutputnode,std::shared_ptr<OsiSolverInterface>,const Models::FMTsolverinterface,size_t lseed,double proportionofset,bool userandomness,bool copysolver)
		/**
		Main constructor used to initialize a FMToperatingareaheuristic, the constructor needs
		alot of information comming from a FMTlpmodel. Also constructing a FMToperatingareaheuristic is
		not a small task if copysolver = true because the solverinterface beging the FMTlpmodel needs
		to be copied. After constructing the object the operatingareaheuristic doesn't need any information
		about the FMTlpmodel.
		*/
		FMToperatingareascheduler(const std::vector<FMToperatingareascheme>& loperatingareas,
			const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& maingraph,
			const Models::FMTmodel& model,
			const Core::FMToutputnode& target,
			Models::FMTlpsolver& basesolve, size_t lseed = 0,
			double proportionofset = 0.25, bool userandomness = false, bool copysolver = true);
		// DocString: FMToperatingareascheduler()
		/**
		Default FMToperatingareaheuristic constructor
		*/
		FMToperatingareascheduler()=default;
		// DocString: FMToperatingareascheduler(const FMToperatingareaheuristic&)
		/**
		FMToperatingareaheuristic copy constructor
		*/
		FMToperatingareascheduler(const FMToperatingareascheduler& rhs);
		// DocString: FMToperatingareascheduler(const FMToperatingareaheuristic&&)
		/**
		FMToperatingareaheuristic move constructor
		*/
		FMToperatingareascheduler(FMToperatingareascheduler&& rhs)=default;
		// DocString: FMToperatingareascheduler::operator=
		/**
		FMToperatingareaheuristic copy assignment
		*/
		FMToperatingareascheduler& operator = (const FMToperatingareascheduler& rhs);
		// DocString: FMToperatingareascheduler::isfeasible
		/**
		Return true if the actual solution of the heuristic is feasible.
		*/
		~FMToperatingareascheduler();
	};
}
BOOST_CLASS_EXPORT_KEY(Heuristics::FMToperatingareascheduler)
#endif
#endif
