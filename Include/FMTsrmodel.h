/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#ifdef FMTWITHOSI

#ifndef FMTsrmodel_H_INCLUDED
#define FMTsrmodel_H_INCLUDED

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
	// DocString: FMTsrmodel
	/**
	FMTsrmodel hold the attributes and methods shared between the spatially referenced models(FTMlpmodel and FMTnssmodel). 
	*/
	class FMTEXPORT FMTsrmodel : public FMTmodel
	{
	public:
		// DocString: FMTsrmodel::setNodeCacheSize
		/*
		@brief building constraints or outut sometime takes alot of memory. This is the size of reserved memory
		taken when caching output node during build process ans output request.
		@param[in] p_size the size to reserve 
		*/
		void setNodeCacheSize(const size_t& p_size);
		// DocString: FMTsrmodel::getGraphVertexToYield
		/*
		If you want to do a generic yieldrequest for the model you may need to have a graphvertextoyield to deal with yields
		that depand on the full graph and model (like FMTyieldmodel).
		the function return a filled Graphvertexyoyield without a specific vertex selected.
		*/
		Graph::FMTgraphvertextoyield getGraphVertexToYield() const;
		// DocString: FMTsrmodel::postsolve
		/*
		This function is for postsolving the presolved model into the original model. In this case, the FMTgraph of the FMTsrmodel is also postsolved.
		*/
		virtual void postsolve(const FMTmodel& originalbasemodel);
		// DocString: FMTsrmodel::setparallellogger
		/**
		Solver's logger cannot work in parallel so you need to pass a logger owned
		by the thead to the solver to make sure it does not work in concurrency.
		*/
		void setparallellogger(Logging::FMTlogger& logger) override;
		// DocString: FMTsrmodel::getavailablesolverinterface
		/**
		Return a vector of solverinterface available
		*/
		static std::vector<Models::FMTsolverinterface> getavailablesolverinterface();
		// DocString: FMTsrmodel(const FMTmodel,FMTsolverinterface)
		/**
		Main constructor used to build FMTsrmodel using it's base class and to let the user choose the solvertype
		(CLP,MOSEK,GUROBI,CPLEX) see FMTsolverinterface.h for more information about the supported solvertype.
		*/
		FMTsrmodel(const FMTmodel& base, FMTsolverinterface lsolvertype);
		// DocString: FMTsrmodel()
		/**
		Default constructor of FMTsrmodel
		*/
		FMTsrmodel();
		// DocString: FMTsrmodel(const FMTsrmodel)
		/**
		Copy constructor of FMTsrmodel
		*/
		FMTsrmodel(const FMTsrmodel& rhs);
		// DocString: FMTsrmodel(const FMTmodel, const Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>,	const FMTlpsolver)
		/**
		Constructor of FMTsrmodel mainly use in postsolve.
		*/
		FMTsrmodel(FMTmodel&& base,const Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>& lgraph,
					const FMTlpsolver& lsolver);
		// DocString: FMTsrmodel::operator=
		/**
		Copy assignment of FMTsrmodel
		*/
		FMTsrmodel& operator = (const FMTsrmodel& rhs);
		// DocString: FMTsrmodel(FMTsrmodel&&)
		/**
		Default move constructor for FMTsrmodel.
		*/
		FMTsrmodel(FMTsrmodel&& rhs) noexcept;
		// DocString: FMTsrmodel::operator=(FMTsrmodel&& rhs) 
		/**
		Default move assignment for FMTsrmodel.
		*/
		FMTsrmodel& operator =(FMTsrmodel&& rhs) noexcept;
		// DocString: ~FMTsrmodel()
		/**
		Default destructor of FMTsrmodel
		*/
		~FMTsrmodel() = default;
		// DocString: FMTsrmodel::operator==
		/**
		Comparison operator of FMTsrmodel
		*/
		bool operator == (const FMTsrmodel& rhs) const;
		// DocString: FMTsrmodel::operator!=
		/**
		Comparison operator of FMTsrmodel
		*/
		bool operator != (const FMTsrmodel& rhs) const;
		// DocString: FMTsrmodel::unboundsolution
		/**
		Unbound the primal bounds of a given period.
		*/
		bool unboundsolution(int period);
		// DocString: FMTsrmodel::isperiodbounded
		/**
		Check if FMTdevelopment area are bounded on there primal variables for a given period.
		*/
		bool isperiodbounded(int period) const;
		// DocString: FMTsrmodel::cleargraphdevelopements
		/**
		Clear developments location by periods in the graph
		*/
		void cleargraphdevelopements();
		// DocString: FMTsrmodel::cleargraphcache
		/**
		Clear the node caching in the graph
		*/
		void cleargraphcache();
		// DocString: FMTsrmodel::setsolution
		/**
		If the user wants to set a solution for a given period for warmstarting the model or prepare to
		bound the model to that solution.
		*/
		bool setsolution(int period, const Core::FMTschedule& schedule, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTsrmodel::setsolutionbylp
		/**
		In some cases if you avec a lot of _lockexcempt actions comming from a Ws model it might be easier to
		use the setsolutionbylp to set the solution on a partial graph. This function will change the objective function,
		constraints and variables bounds so juste use it in a "Get results" context ( only valid for partial graph).
		*/
		bool setsolutionbylp(int period, const Core::FMTschedule& schedule, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTsrmodel::forcesolution
		/**
		Force a solution by using a schedule based on proportions instead of area. So if there is modifications to the model, it will
		place only the parts of the solution that are possible considering the modifications. 
		The schedule must be with lock indicators if locks are used in the model. 
		*/
		bool forcesolution(int period, const Core::FMTschedule& proportionschedulewithlock);
		// DocString: FMTsrmodel::getsolution
		/**
		Get the standard solution for a given period (FMTschedule dont have natural growth solution included).
		If with lock is true then the schedule will contain locked developement.
		*/
		Core::FMTschedule getsolution(int period, bool withlock = false) const final;
		// DocString: FMTsrmodel::getscheduleproportions
		/**
		Same as getsolution but the schedule area represent the percentage of area of the developement.
		*/
		Core::FMTschedule getscheduleproportions(int period, bool withlock) const;
		// DocString: FMTsrmodel::getstats
		/**
		Get the graph stats of the graph and matrix (number of columns/rows/edges/verticies...)
		*/
		Graph::FMTgraphstats getstats() const;
		// DocString: FMTsrmodel::getcopy
		/**
		This function returns a copy of the FMTmodel of the selected period.
		If period = 0 it returns the FMTmodel::getcopy if period > 0 then it returns
		a copy of the FMTmodel based on the developements of the FMTgraph of the FMTlpmodel.
		Need to have a builded graph with a solution to use this function.
		*/
		virtual std::unique_ptr<FMTmodel> getcopy(int period = 0) const;
		// DocString: FMTsrmodel::getoutput
		/**
		Get the output value of a output for a given period using the solution of the matrix.
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or developement name if level == FMToutputlevel::developpement
		*/
		virtual std::map<std::string, double> getoutput(const Core::FMToutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const;
		#if defined FMTWITHR
		// DocString: FMTsrmodel::getoutputsdataframe
		/**
		Returns a dataframe filled up with outputs from first period to last period at the developement level.
		For multiple outputs.
		*/
		Rcpp::DataFrame getoutputsdataframe(const std::vector<Core::FMToutput>& outputsdata, int firstperiod, int lastperiod) const;
		#endif 
		// DocString: FMTsrmodel::buildperiod
		/**
		This function is the main function used to build the graph and the matrix.
		A call to that function add a period within the graph and the matrix of the FMTsrmodel.
		If the schedule is not empty than the model is going to be generated in partialbuild mode and
		not fullbuild mode. Partialbuild will only build the graph linked to the solution. make it perfect to
		reinterpret outputs for a given solution without rebuilding the whole graph.
		*/
		Graph::FMTgraphstats buildperiod(Core::FMTschedule schedule = Core::FMTschedule(),
				bool forcepartialbuild = false, int compressageclassoperability = 1);
		// DocString: FMTsrmodel::getfirstactiveperiod
		/**
		Return the first active period should be always 0 in case or planning.
		But when eraseperiod is called the first active period is going to move to 1 and so on.
		*/
		int getfirstactiveperiod() const;
		// DocString: FMTsrmodel::getarea
		/**
		This function returns an area for a given period for a FMTsrmodel.
		If period = 0 the area is the same has FMTmodel::getarea().
		For period > 0 the area returned is the FMTdevelopement of the graph
		that can be (actual of futur) existing at the beginning of the period.
		Need to have a builded graph with a solution to use this function.
		*/
		virtual std::vector<Core::FMTactualdevelopment>getarea(int period = 0, bool beforegrowanddeath = false) const override;
		// DocString: FMTsrmodel::getsolverptr()
		/**
		Get a pointer to the solver behind the model.
		*/
		FMTlpsolver* getsolverptr();
		// DocString: FMTsrmodel::getconstsolverptr()
		/**
		Get a pointer to the const solver behind the model.
		*/
		const FMTlpsolver* getconstsolverptr() const;
		// DocString: FMTsrmodel::clone
		/**
		Get a clone of the FMTsrmodel
		*/
		virtual std::unique_ptr<FMTmodel>clone() const override;
		// DocString: FMTsrmodel::passinlogger
		/**
		We need to override the passinlogger for the osisolverinterface
		*/
		void passinlogger(const std::unique_ptr<Logging::FMTlogger>& logger) override;
		// DocString: FMTsrmodel::presolve
		/**
		Presolve a FMTsrmodel.
		*/
		virtual std::unique_ptr<FMTmodel>presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments = std::vector<Core::FMTactualdevelopment>()) const override;
		// DocString: FMTsrmodel::boundsolution
		/**
		This function bounds the primal variables to the primal solution present within the matrix for
		a given period and tolerance. Perfect function to update a FMTlpmodel or get ready for replanning.
		*/
		bool boundsolution(int period, double tolerance = FMT_DBL_TOLERANCE);
		// DocString: FMTsrmodel::setparameter(const FMTintmodelparameters, const int)
		/**
		Override setter for intmodelparameters.
		*/
		bool setparameter(const FMTintmodelparameters& key, const int& value) override;
		// DocString: FMTsrmodel::setparameter(const FMTboolmodelparameters, const bool)
		/**
		Override setter for boolmodelparameters.
		*/
		bool setparameter(const FMTboolmodelparameters& key, const bool& value) override;
		// DocString: FMTsrmodel::getgraphstats
		/**
		Return the statistics of the actual graph (number of cols,rows,edges,verticies)
		*/
		Graph::FMTgraphstats getgraphstats() const;
		// DocString: FMTsrmodel::getRotations
		/**
		Based on a mask and an action get all the unique rotations (in period) taken to complete a serie of action. A serie can contain a subserie for an
		aggregate of actions. It will only keep series that actions are part of the aggregate.
		Args:
			mask : the FMTmask of the rotations
			aggregate : std::string on which the last action is considered to be in
		Returns: Unique FMTSerie (where the serie is in string action1-action2-action3... string is the serie and int is the number of periods taken to complete the serie.
		*/
		std::set<Core::FMTSerie>getRotations(const Core::FMTmask& mask, const std::string& aggregate) const;
		// DocString: FMTsrmodel::isoptimal
		/**
		Return true if the solver consider the solution optimal.
		*/
		virtual bool isoptimal() const;
		// DocString: FMTsrmodel::getobjectivevalue
		/**
		Return the value of the solver objective.
		*/
		virtual double getobjectivevalue() const;
		// DocString: FMTsrmodel::getnochoice
		/**
		Giving a base_mask returns period 0 developpements that have no actions assigned for the whole planning horizon.
		The developpements mask have to be a subset of the base_mask.
		*/
		std::vector<const Core::FMTdevelopment*> getnochoice(const Core::FMTmask& base_mask) const;
		// DocString: FMTsrmodel::getAllMasks
		/*
		@brief Get all masks in string format for a complete grapoh only for selected themes
		@param[in] p_selectedThemes
		@return a set of masks
		*/
		std::set<std::string> getAllMasks(const std::vector<Core::FMTtheme>& p_selectedThemes) const;
	protected:
		// DocString: FMTsrmodel::graph
		///graph holding the FMTdevelopments for all the periods.
		std::unique_ptr<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>>m_graph;
		// DocString: FMTsrmodel::solver
		///The lpsolver
		FMTlpsolver solver;
		// DocString: FMTsrmodel::summarize
		/**
		Simple function to summarize constraints that are un a map structure key = variables, element = coefficiant
		to a array structure (vector) for osisolverinterface. map structure is easier to deal with thant two vectors.
		*/
		bool summarize(const std::map<int, double>& variables,
			std::vector<int>& sumvariables, std::vector<double>& sumcoefficiants) const;
		// DocString: FMTsrmodel::initializematrix
		/**
		Initialize the solverinterface called once when the FMTgraph was empty after the first call of buildperiod.
		*/
		Graph::FMTgraphstats initializematrix();
		// DocString: FMTsrmodel::updatematrix
		/**
		During a call to build period after the graph has been updated with nes developments type the solverinterface matrix
		need to be updated. Variables and constraints related to each of those new developements will be added to the matrix.
		So area transfer row and natural growth plus action variables.
		*/
		Graph::FMTgraphstats updatematrix(const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_pair& targets,
			const Graph::FMTgraphstats& newstats);
		// DocString: FMTsrmodel::getgraphlength
		/**
		Return the size of the graph.
		*/
		size_t getgraphsize() const;
		// DocString: FMTsrmodel::postsolvegraph
		/**
		Post solve this graph and return a presolved graph for each vertex and edges based on the original model.
		*/
		void postsolvegraph(const FMTmodel& originalbasemodel);
		// DocString: FMTsrmodel::getActives
		/**
		@brief If the graph is not initialize then initialise the graph based on the area else return the actives verticies.
		@return a queue of actives vertices of the graph.
		*/
		std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> getActives();
	private:
		// DocString: FMTsrmodel::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
			ar& BOOST_SERIALIZATION_NVP(solver);
			ar& BOOST_SERIALIZATION_NVP(m_graph);
		}
		// DocString: FMTsrmodel::load
		/**
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
			ar& BOOST_SERIALIZATION_NVP(solver);
			ar& BOOST_SERIALIZATION_NVP(m_graph);
			solver.passinmessagehandler(*_logger);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()

	};

}

BOOST_CLASS_EXPORT_KEY(Models::FMTsrmodel)

#endif 
#endif