/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#ifdef FMTWITHOSI

#ifndef FMTsrmodel_H_INCLUDED
#define FMTsrmodel_H_INCLUDED

#include "FMTmodel.hpp"
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>
#include "FMTlpsolver.hpp"
#include "FMTvertexproperties.hpp"
#include "FMTedgeproperties.hpp"
#include "FMTgraph.hpp"

#if defined FMTWITHR
	#include <Rcpp.h>
#endif 

namespace Models 
	{
	// DocString: FMTsrmodel
	/**
	FMTsrmodel hold the attributes and methods shared between the spatially referenced models(FTMlpmodel and FMTnssmodel). 
	*/
	class FMTEXPORT FMTsrmodel : public FMTmodel
	{
		// DocString: FMTsrmodel::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
			ar & BOOST_SERIALIZATION_NVP(solver);
			ar & BOOST_SERIALIZATION_NVP(graph);
		}
		// DocString: FMTsrmodel::load
		/**
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
			ar & BOOST_SERIALIZATION_NVP(solver);
			ar & BOOST_SERIALIZATION_NVP(graph);
			solver.passinmessagehandler(*this->_logger);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
	protected:
		// DocString: FMTsrmodel::graph
		///graph holding the FMTdevelopments for all the periods.
		Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>graph;
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
		// DocString: FMTsrmodel::passinobjecttomembers
		/**
		Pass FMTobject to members data.
		*/
		void passinobjecttomembers(const Core::FMTobject& rhs);
		// DocString: FMTsrmodel::getgraphlength
		/**
		Return the size of the graph.
		*/
		size_t getgraphsize() const;
	public:
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
		FMTsrmodel(	const FMTmodel& base,const Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>& lgraph,
					const FMTlpsolver& lsolver);
		// DocString: FMTsrmodel::operator=
		/**
		Copy assignment of FMTsrmodel
		*/
		FMTsrmodel& operator = (const FMTsrmodel& rhs)=default;
		// DocString: FMTsrmodel(FMTsrmodel&&)
		/**
		Default move constructor for FMTsrmodel.
		*/
		FMTsrmodel(FMTsrmodel&& rhs)=default;
		// DocString: FMTsrmodel::operator=(FMTsrmodel&& rhs) 
		/**
		Default move assignment for FMTsrmodel.
		*/
		FMTsrmodel& operator =(FMTsrmodel&& rhs) =default;
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
		// DocString: FMTsrmodel::getsolution
		/**
		Get the standard solution for a given period (FMTschedule dont have natural growth solution included).
		If with lock is true then the schedule will contain locked developement.
		*/
		Core::FMTschedule getsolution(int period, bool withlock = false) const final;
		// DocString: FMTlpsrmodel::getstats
		/**
		Get the graph stats of the graph and matrix (number of columns/rows/edges/verticies...)
		*/
		Graph::FMTgraphstats getstats() const;
		// DocString: FMTsrmodel::getoutput
		/**
		Get the output value of a output for a given period using the solution of the matrix.
		the map key returned consist of output name
		if level == FMToutputlevel::standard || level == FMToutputlevel::totalonly,
		or developement name if level == FMToutputlevel::developpement
		*/
		virtual std::map<std::string, double> getoutput(const Core::FMToutput& output,
			int period, Core::FMToutputlevel level = Core::FMToutputlevel::standard) const final;
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
		// DocString: FMTsrmodel::passinlogger
		/**
		It's sometime usefull to pass in the logger of an other FMTobject.
		*/
		virtual void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger) override;
		// DocString: FMTlsrmodel::passinexceptionhandler
		/**
		It's sometime usefull to pass in the exception handler of an other FMTobject.
		*/
		virtual void passinexceptionhandler(const std::shared_ptr<Exception::FMTexceptionhandler>& exhandler) override;
		// DocString: FMTsrmodel::passinobject
		/**
		It's sometime usefull to pass in the exception handler and the logger  of an other FMTobject to
		a FMTobject.
		*/
		virtual void passinobject(const Core::FMTobject& rhs) override;
		// DocString: FMTsrmodel::clone
		/**
		Get a clone of the FMTsrmodel
		*/
		virtual std::unique_ptr<FMTmodel>clone() const override;
	};

}

BOOST_CLASS_EXPORT_KEY(Models::FMTsrmodel)

#endif 
#endif