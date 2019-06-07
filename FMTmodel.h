#ifndef FMTMODEL_H_INCLUDED
#define FMTMODEL_H_INCLUDED

#include <vector>
#include <string>

#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMTyields.h"
#include "FMTlifespans.h"
#include "FMTactualdevelopment.h"
#include "FMTconstants.h"
#include "FMToutput.h"
#include "FMTconstraint.h"
#include <memory>
#include "FMTgraph.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>

/*
#include <lemon/list_graph.h>
#include <lemon/concepts/digraph.h>
using namespace lemon;
*/

using namespace Core;
//using namespace Spatial;



namespace Models
{


class FMTmodel : public FMTobject
    {
    friend class Graph::FMTgraph;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(area);
		ar & BOOST_SERIALIZATION_NVP(themes);
		ar & BOOST_SERIALIZATION_NVP(actions);
		ar & BOOST_SERIALIZATION_NVP(action_aggregates);
		ar & BOOST_SERIALIZATION_NVP(transitions);
		ar & BOOST_SERIALIZATION_NVP(yields);
		ar & BOOST_SERIALIZATION_NVP(lifespan);
		ar & BOOST_SERIALIZATION_NVP(outputs);
		ar & BOOST_SERIALIZATION_NVP(constraints);
		ar & BOOST_SERIALIZATION_NVP(name);
	}
    protected:
        vector<FMTactualdevelopment>area;
        vector<FMTtheme>themes;
        vector<FMTaction>actions;
		//vector<unique_ptr<FMTaction>>actions;
		map<string, vector<string>>action_aggregates;
        vector<FMTtransition>transitions;
        FMTyields yields;
        FMTlifespans lifespan;
		vector<FMToutput> outputs;
		vector<FMTconstraint>constraints;
		void setdefaultobjects();
    public:
        string name;
		FMTmodel(const vector<FMTactualdevelopment>& larea, const vector<FMTtheme>& lthemes,
			const vector<FMTaction>& lactions, const map<string, vector<string>>& laction_aggregates,
             const vector<FMTtransition>& ltransitions,const FMTyields& lyields,const FMTlifespans& llifespan,
			const string& lname,const vector<FMToutput>& loutputs);
		FMTmodel(const vector<FMTactualdevelopment>& larea, const vector<FMTtheme>& lthemes,
			const vector<FMTaction>& lactions, const map<string, vector<string>>& laction_aggregates,
			const vector<FMTtransition>& ltransitions, const FMTyields& lyields, const FMTlifespans& llifespan,
			const string& lname, const vector<FMToutput>& loutputs, const vector<FMTconstraint>& lconstraints);
    FMTmodel();
    virtual ~FMTmodel()=default;
    virtual vector<FMTactualdevelopment>getarea(int period = 0) const;
	void cleanactionsntransitions();
    vector<FMTtheme>getthemes() const;
    vector<FMTaction>getactions() const;
	map<string,vector<string>> getactionaggregates() const;
    vector<FMTtransition>gettransitions() const;
    FMTyields getyields() const;
    FMTlifespans getlifespan() const;
	vector<FMToutput> getoutputs() const;
	FMTaction defaultdeathaction() const;
	FMTtransition defaultdeathtransition() const;
	vector<FMTconstraint>getconstraints() const;
	void setconstraints(const vector<FMTconstraint>& lconstraint);
	bool operator == (const FMTmodel& rhs) const;
    bool setarea(const vector<FMTactualdevelopment>& ldevs);
    bool setthemes(const vector<FMTtheme>& lthemes);
    bool setactions(const vector<FMTaction>& lactions);
    bool settransitions(const vector<FMTtransition>& ltransitions);
    bool setyields(const FMTyields& lylds);
    bool setlifespan(const FMTlifespans& llifespan);
    bool valid();
    FMTmodel(const FMTmodel& rhs);
    FMTmodel& operator = (const FMTmodel& rhs);
    };

class FMTmodelcomparator
{
	string model_name;
public:
	FMTmodelcomparator(string name);
	bool operator()(const FMTmodel& model) const;

};

}

#endif // FMTMODEL_H_INCLUDED
