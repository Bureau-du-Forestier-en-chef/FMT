#ifndef FMTlpmodel_H_INCLUDED
#define FMTlpmodel_H_INCLUDED

#include "FMTgraphdescription.h"
#include "FMTdevelopment.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMTyields.h"
#include "FMTtheme.h"
#include "FMTschedule.h"



#include "FMToutputproperties.h"
#include "FMTdevelopmentpath.h"

#include "OsiClpSolverInterface.hpp"
#include "FMTserializablematrix.h"


/*
#include "OsiCpxSolverInterface.hpp"
#include "OsiGrbSolverInterface.hpp"
#include "OsiMskSolverInterface.hpp"
*/
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


using namespace Graph;
using namespace boost;
using namespace std;

namespace Models
{


enum class FMTsolverinterface
	{
	CLP = 1,
	MOSEK = 2,
	CPLEX = 3,
	GUROBI = 4,
	};

enum FMTmatrixelement
	{
	goalvariable=0,//period base
	levelvariable=1,//general
	objectivevariable=2,//general
	constraint=3,//period base...
	nr_items=4
	};

class FMTlpmodel : public FMTmodel
	{
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
		{
		ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
		ar & BOOST_SERIALIZATION_NVP(solvertype);
		ar & BOOST_SERIALIZATION_NVP(graph);
		ar & BOOST_SERIALIZATION_NVP(elements);
		//Save the matrix columnes / rows / column solution / row solution / objective
		FMTserializablematrix matrix(solverinterface);
		ar & BOOST_SERIALIZATION_NVP(matrix);
		}
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("model", boost::serialization::base_object<FMTmodel>(*this));
		ar & BOOST_SERIALIZATION_NVP(solvertype);
		ar & BOOST_SERIALIZATION_NVP(graph);
		ar & BOOST_SERIALIZATION_NVP(elements);
		//load the matrix
		FMTserializablematrix matrix;
		ar & BOOST_SERIALIZATION_NVP(matrix);
		buildsolverinterface();
		matrix.setmatrix(solverinterface);
		}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	FMTgraph graph;
	FMTsolverinterface solvertype;
	unique_ptr<OsiSolverInterface>solverinterface;
	/*vector<std::unordered_map<size_t,
		FMTvertex_descriptor>>developments;*/
	vector<std::unordered_map<size_t,
		vector<vector<int>>>>elements;
	//mutable std::unordered_map<size_t, map<string, double>>_source_cache;
	void buildsolverinterface();
	void copysolverinterface(const unique_ptr<OsiSolverInterface>& solver_ptr);
	bool summarize(vector<int> variables,vector<double> coefficiants,
					vector<int>& sumvariables, vector<double>& sumcoefficiants) const;
		//bool containsdevelopment(const FMTdevelopment& developement) const;
		/*map<string, double> getsource(const FMToutputnode& node,
			 int period,
			const FMTtheme& theme) const;*/
		//bool anyoperables(const FMTvertex_descriptor& descriptor, const vector<int>& action_ids) const;
		//void setstats(const FMTgraphstats& newstats);
		FMTgraphstats initializematrix();
		//std::queue<FMTvertex_descriptor>initialize(const vector<FMTactualdevelopment>& actdevelopments);
		//vector<FMTactualdevelopment>actuals(int location) const;
		/*FMTgraphstats buildschedule(std::queue<FMTvertex_descriptor> actives,
			const FMTschedule& schedule);*/
		//FMTgraphstats build(std::queue<FMTvertex_descriptor> actives);
		FMTgraphstats updatematrix(const std::unordered_map<size_t, FMTvertex_descriptor>& targets,
			const FMTgraphstats& newstats);
		//FMTvertex_descriptor getdevelopment(const FMTdevelopment& developement) const;
		//FMTvertex_descriptor adddevelopment(const FMTfuturdevelopment& futurdevelopement);
		int addmatrixelement(const FMTconstraint& constraint,
                     const FMTmatrixelement& element_type,const vector<int>& indexes,const vector<double>& coefs,
                     int period = -1,
                     double lowerbound = COIN_DBL_MIN,double upperbound = COIN_DBL_MAX);
        bool getgoals(const vector<string>& goalsnames,
                      vector<int>& index,vector<double>& coefs,const double& sense) const;
        int getsetlevel(const FMTconstraint& constraint,const string& variable_level,int period);

        vector<vector<int>>getmatrixelement(const FMTconstraint& constraint,int period) const;
		/*bool gettransferrow(const FMTvertex_descriptor& vertex_descriptor,
			vector<int>&row_starts,
			vector<int>& cols,
			vector<double>& cols_value) const;*/
		/*void addaction(const int& actionID,
			FMTgraphstats& statsdiff,
			std::queue<FMTvertex_descriptor>& actives,
			const FMTvertex_descriptor& out_vertex,
			const vector<FMTdevelopmentpath>& paths);*/
		/*double outarea(const FMTvertex_descriptor& out_vertex,
			const int& actionID, const double*&  solution) const;*/
		//double inarea(const FMTvertex_descriptor& out_vertex, const double*&  solution, bool growth = false) const;
		/*bool periodstart(const FMTvertex_descriptor& out_vertex) const;
		bool periodstop(const FMTvertex_descriptor& out_vertex) const;*/
		/*vector<FMTdevelopmentpath>getpaths(const FMTvertex_descriptor& out_vertex,
			const int& actionID) const;
		map<int, int> getoutvariables(const FMTvertex_descriptor& out_vertex) const;*/
		unique_ptr<OsiSolverInterface>& getsolverinterface();
		//vector<FMTaction>selectedactions(const vector<int>& action_IDS) const;
		//bool validouputnode(const FMToutputnode& node,vector<int>& action_IDS, int period) const;
		/*bool validgraphnode(bool& inedges, const FMTvertex_descriptor& vertex_descriptor,
			const FMToutputnode& node, const vector<int>& action_IDS,const vector<FMTaction>& selected) const;*/
		/*void locatenodes(const vector<FMToutputnode>& nodes,int period,
			vector<int>& variables,vector<double>& coefs,double multiplier = 1) const;*/
        void locatelevels(const vector<FMToutputnode>& nodes,int period,
            vector<int>& variables,vector<double>& coefs,const FMTconstraint& constraint);
		//void constraintlenght(const FMTconstraint& constraint, int& start, int& stop) const;
	public:
		FMTlpmodel(const FMTmodel& base, FMTsolverinterface lsolvertype);
		FMTlpmodel();
		FMTlpmodel(const FMTlpmodel& rhs);
		bool setsolution(int period, const FMTschedule& schedule);
		bool boundsolution(int period);
		FMTschedule getsolution(int period) const;
		//bool samegraph(const FMTlpmodel& rhs) const;
		FMTgraphstats getstats() const;
		//void setgraph(const FMTlpmodel& rhs);
		bool operator == (const FMTlpmodel& rhs) const;
		bool operator != (const FMTlpmodel& rhs) const;
		//vector<FMTactualdevelopment>getarea(int period) const override;
		map<string, double> getoutput(const FMToutput& output,
			int period);
		FMTgraphstats buildperiod(FMTschedule schedule = FMTschedule());
		//FMTgraphstats eraseperiod();
		FMTgraphstats setobjective(const FMTconstraint& objective);
		FMTgraphstats setconstraint(const FMTconstraint& constraint);
		bool solve();
		FMTlpmodel& operator = (const FMTlpmodel& rhs);
		~FMTlpmodel() = default;
	};

}


#endif
