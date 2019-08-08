#ifndef FMTGRAPH_H
#define FMTGRAPH_H

#include "FMTgraphstats.h"
#include "FMTgraphdescription.h"

#include "FMToutputnode.h"
#include "FMTdevelopment.h"
#include "FMTdevelopmentpath.h"
#include "FMTschedule.h"
#include "FMToutputnode.h"
#include "FMTconstraint.h"
#include "FMTevent.h"
#include "FMTcoordinate.h"

#include <boost/serialization/split_member.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>

#include <memory>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <queue>
#include <random>

namespace Models
{class FMTmodel;}//Forward declaration for friend class

using namespace Models;
using namespace Spatial;

namespace Graph
{

enum class FMTgraphbuild
	{
		schedulebuild = 1,
		fullbuild = 2,
		nobuild = 3
	};

enum FMToutputlevel
	{
	standard = 1,
	totalonly = 2,
	developpement = 3
	};

class FMTgraph
    {
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		ar & BOOST_SERIALIZATION_NVP(data);
		ar & BOOST_SERIALIZATION_NVP(nodescache);
		ar & BOOST_SERIALIZATION_NVP(stats);
		ar & BOOST_SERIALIZATION_NVP(buildtype);
	}
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(data);
		generatedevelopments();
		ar & BOOST_SERIALIZATION_NVP(nodescache);
		ar & BOOST_SERIALIZATION_NVP(stats);
		ar & BOOST_SERIALIZATION_NVP(buildtype);
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
    protected:
		FMTadjacency_list data;
        FMTgraphbuild buildtype;
        vector<std::unordered_map<size_t,FMTvertex_descriptor>> developments;
		mutable std::unordered_map<size_t, map<string, double>>nodescache; //Quand le graph change faire nodescache.clear()
        FMTgraphstats stats;
		void updatevarsmap(map<int,double>& variables,const int& var,const double& coef) const;
    public:
        FMTgraph();
        ~FMTgraph()=default;
        FMTgraph(const FMTgraphbuild lbuildtype);
        FMTgraph(const FMTgraph& rhs);
        FMTgraph& operator = (const FMTgraph& rhs);
        bool operator == (const FMTgraph& rhs) const;
        bool operator != (const FMTgraph& rhs) const;
        FMTgraphbuild getbuildtype() const;
		void setbuildtype(const FMTgraphbuild& build);
        bool containsdevelopment(const FMTdevelopment& developement) const;
		std::queue<FMTvertex_descriptor> initialize(const vector<FMTactualdevelopment>& actdevelopments);
		FMTgraphstats build(const FMTmodel& model,
                            std::queue<FMTvertex_descriptor> actives);
        vector<pair<size_t,int>> adjacentevents(const vector<FMTevent<FMTgraph>>& events, const FMTcoordinate& localisation,const int action_id) const;
        pair<size_t,int> randomoperate(const vector<pair<size_t,int>>& operables, const FMTmodel& model, std::queue<FMTvertex_descriptor>& actives,
                                            FMTgraphstats& statsdiff, const FMTvertex_descriptor& front_vertex, default_random_engine& generator,
                                            const FMTdevelopment& active_development);
        FMTgraphstats randombuild(const FMTmodel& model,
                                    std::queue<FMTvertex_descriptor> actives, default_random_engine& generator,
                                    vector<vector<FMTevent<FMTgraph>>>& events_id, const FMTcoordinate& localisation);
        FMTgraphstats naturalgrowth(std::queue<FMTvertex_descriptor> actives);
        vector<FMTactualdevelopment> getperiodstopdev(const int location,const double* solution) const;
		map<string, double> getoutput(const FMTmodel& model, const FMToutput& output,
			int period, const double* solution,FMToutputlevel level = FMToutputlevel::standard) const;
		FMTvertex_descriptor getdevelopment(const FMTdevelopment& developement) const;
		const FMTdevelopment& getdevelopment(const FMTvertex_descriptor& descriptor) const;
		FMTvertex_descriptor adddevelopment(const FMTfuturdevelopment& futurdevelopement);
		void addaction(const int& actionID,
                        FMTgraphstats& statsdiff,
                        std::queue<FMTvertex_descriptor>& actives,
                        const FMTvertex_descriptor& out_vertex,
                        const vector<FMTdevelopmentpath>& paths);
		double outarea(const FMTvertex_descriptor& out_vertex,
                        const int& actionID, const double*&  solution) const;//hack solution to use cellsize as solution for sasolution
		double inarea(const FMTvertex_descriptor& out_vertex,
                        const double*&  solution, bool growth = false) const;//hack solution to use cellsize as solution for sasolution
		bool periodstart(const FMTvertex_descriptor& out_vertex) const;
		bool periodstop(const FMTvertex_descriptor& out_vertex) const;
		vector<FMTdevelopmentpath> getpaths(const FMTvertex_descriptor& out_vertex,
                                            const int& actionID) const;
		bool validouputnode(const FMTmodel& model, const FMToutputnode& node,vector<int>& action_IDS, int period) const;
		bool validgraphnode(const FMTmodel& model, bool& inedges, const FMTvertex_descriptor& vertex_descriptor,
                            const FMToutputnode& node, const vector<int>& action_IDS,const vector<const FMTaction*>& selected) const;
		void locatenodes(const FMTmodel& model, const vector<FMToutputnode>& nodes,int period,
                            /*vector<int>& variables,vector<double>& coefs*/
					map<int, double>& variables,double multiplier = 1) const;//to look at
        bool anyoperables(const FMTvertex_descriptor& descriptor, const vector<int>& action_ids) const;
        map<int, int> getoutvariables(const FMTvertex_descriptor& out_vertex) const;
		vector<std::pair<const int*,const int*>>getoutvariablesnactionsptr(const FMTvertex_descriptor& out_vertex) const;
        vector<const FMTaction*> selectedactions(const FMTmodel& model, const vector<int>& action_IDS) const;
		bool constraintlenght(const FMTconstraint& constraint, int& start, int& stop) const;
		FMTgraphstats getstats() const;
		FMTgraphstats* getstatsptr();
		void setstats(const FMTgraphstats& newstats);
		FMTgraphstats buildschedule(const FMTmodel& model, std::queue<FMTvertex_descriptor> actives,
			const FMTschedule& schedule);
		FMTgraphstats eraseperiod();
		bool empty() const;
		std::queue<FMTvertex_descriptor> getactiveverticies() const;
		const std::unordered_map<size_t, FMTvertex_descriptor>& getperiodverticies(int period) const;
		size_t size() const;
		void setconstraintID(const FMTvertex_descriptor& vertex, const int& id);
		bool gettransferrow(const FMTvertex_descriptor& vertex_descriptor,
			vector<int>&row_starts,
			vector<int>& cols,
			vector<double>& cols_value) const;
		void getinitialbounds(vector<double>& lower_bounds, vector<double>& upper_bounds) const;
		size_t nedges() const;
		map<string, double> getsource(const FMTmodel& model,
			const FMToutputnode& node,
			int period, const FMTtheme& theme,
			const double* solution, FMToutputlevel level = FMToutputlevel::standard) const;
        bool splittedevent(const FMTevent<FMTgraph>& event, vector<FMTevent<FMTgraph>>& splittedevents) const;
        void cleanevents(vector<FMTevent<FMTgraph>>& events_id, const FMTcoordinate& localisation) const;
        FMTgraphstats clearfromperiod(const int& period, vector<vector<vector<FMTevent<FMTgraph>>>>& events,
                                        const FMTcoordinate& localisation);
        FMTgraph partialcopy(const int& period, vector<vector<vector<FMTevent<FMTgraph>>>>& events,
                                const FMTcoordinate& localisation) const;
        void generatedevelopments();
        FMTgraph perturbgraph(const FMTmodel& model,default_random_engine& generator,
                              vector<vector<vector<FMTevent<FMTgraph>>>>& events,
                              const FMTcoordinate& localisation, const int period) const;

    };
}
#endif // FMTGRAPH_H
