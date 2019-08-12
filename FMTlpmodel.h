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
	FMTgraph graph; //The Type 3 Graph
	FMTsolverinterface solvertype; //Solvertype used
	unique_ptr<OsiSolverInterface>solverinterface;//The osisolver interface Abstract class (constraints/objectives/matrix ....LP)
	vector<std::unordered_map<size_t,
		vector<vector<int>>>>elements;//Locations of the constraints and variables in the matrix for the constraints / objective
	mutable std::unordered_map<size_t,
		map<int, double>>nodevariables;//Only for cashing variables location related to nodes...
	void buildsolverinterface();
	void copysolverinterface(const unique_ptr<OsiSolverInterface>& solver_ptr);
	bool summarize(/*vector<int> variables,vector<double> coefficiants*/const map<int, double>& variables ,
					vector<int>& sumvariables, vector<double>& sumcoefficiants) const;
	FMTgraphstats initializematrix();
	FMTgraphstats updatematrix(const std::unordered_map<size_t, FMTvertex_descriptor>& targets,
			const FMTgraphstats& newstats);
		int addmatrixelement(const FMTconstraint& constraint,
                     const FMTmatrixelement& element_type, const map<int, double>& indexes,/*const vector<int>& indexes,const vector<double>& coefs,*/
                     int period = -1,
                     double lowerbound = COIN_DBL_MIN,double upperbound = COIN_DBL_MAX);
        bool getgoals(const vector<string>& goalsnames,map<int,double>& index
                      /*vector<int>& index,vector<double>& coefs*/,const double& sense) const;
        int getsetlevel(const FMTconstraint& constraint,const string& variable_level,int period);

        vector<vector<int>>getmatrixelement(const FMTconstraint& constraint,int period) const;
		unique_ptr<OsiSolverInterface>& getsolverinterface();
        void locatelevels(const vector<FMToutputnode>& nodes,int period,map<int, double>& variables,const FMTconstraint& constraint);
		bool locatenodes(const vector<FMToutputnode>& nodes, int period, map<int, double>& variables,double multiplier = 1) const;
		FMTtheme locatestatictheme() const;
	public:
		FMTlpmodel(const FMTmodel& base, FMTsolverinterface lsolvertype);
		FMTlpmodel();
		FMTlpmodel(const FMTlpmodel& rhs);
		bool specificinitialsolve();
		bool setsolution(int period, const FMTschedule& schedule);
		bool boundsolution(int period);
		bool unboundsolution(int period);
		FMTschedule getsolution(int period) const;
		FMTgraphstats getstats() const;
		bool operator == (const FMTlpmodel& rhs) const;
		bool operator != (const FMTlpmodel& rhs) const;
		map<string, double> getoutput(const FMToutput& output,
			int period, FMToutputlevel level = FMToutputlevel::standard);
		FMTgraphstats buildperiod(FMTschedule schedule = FMTschedule(),
			bool forcepartialbuild = false);
		FMTgraphstats setobjective(const FMTconstraint& objective);
		FMTgraphstats setconstraint(const FMTconstraint& constraint);
		FMTgraphstats removeconstraint(const FMTconstraint& constraint, int period);
		/*bool unboundconstraint(const FMTconstraint& constraint, int period);
		bool boundconstraint(const FMTconstraint& constraint, int period);*/
		bool solve();
		FMTlpmodel& operator = (const FMTlpmodel& rhs);
		~FMTlpmodel() = default;
	};

}


#endif
