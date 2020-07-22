/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTGRAPH_H
#define FMTGRAPH_H

#include "FMTgraphstats.h"
#include "FMTgraphdescription.h"

#include "FMToutputnode.h"
#include "FMTdevelopment.h"
#include "FMTactualdevelopment.h"
#include "FMTfuturdevelopment.h"
#include "FMTdevelopmentpath.h"
#include "FMTschedule.h"
#include "FMToutputnode.h"
#include "FMTconstraint.h"
#include "FMTevent.h"
#include "FMTcoordinate.h"
#include "FMToutputnodecache.h"

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
{class FMTmodel;}

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
		ar & BOOST_SERIALIZATION_NVP(stats);
		ar & BOOST_SERIALIZATION_NVP(buildtype);
	}
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(data);
		generatedevelopments();
		ar & BOOST_SERIALIZATION_NVP(stats);
		ar & BOOST_SERIALIZATION_NVP(buildtype);
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
    protected:
		FMTadjacency_list data;
        FMTgraphbuild buildtype;
        std::vector<std::unordered_map<size_t,FMTvertex_descriptor>> developments;
		mutable std::vector<FMToutputnodecache>nodescache;
        FMTgraphstats stats;
		void updatevarsmap(std::map<int,double>& variables,const int& var,const double& coef) const;
		std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::iterator getfirstblock();
		std::vector<std::unordered_map<size_t, FMTvertex_descriptor>>::const_iterator getfirstconstblock() const;
    public:
        FMTgraph();
        virtual ~FMTgraph()=default;
        FMTgraph(const FMTgraphbuild lbuildtype);
        FMTgraph(const FMTgraph& rhs);
		void swap(FMTgraph& rhs);
        FMTgraph& operator = (const FMTgraph& rhs);
        bool operator == (const FMTgraph& rhs) const;
        bool operator != (const FMTgraph& rhs) const;
        FMTgraphbuild getbuildtype() const;
		void setbuildtype(const FMTgraphbuild& build);
        bool containsdevelopment(const Core::FMTdevelopment& developement) const;
		std::queue<FMTvertex_descriptor> initialize(const std::vector<Core::FMTactualdevelopment>& actdevelopments);
		FMTgraphstats build(const Models::FMTmodel& model,
                            std::queue<FMTvertex_descriptor> actives);
        FMTgraphstats naturalgrowth(std::queue<FMTvertex_descriptor> actives);
		std::vector<Core::FMTactualdevelopment> getperiodstopdev(const int location,const double* solution) const;
		std::map<std::string, double> getoutput(const Models::FMTmodel& model, const Core::FMToutput& output,
			int period, const double* solution,FMToutputlevel level = FMToutputlevel::standard) const;
		FMTvertex_descriptor getdevelopment(const Core::FMTdevelopment& developement) const;
		const Core::FMTdevelopment& getdevelopment(const FMTvertex_descriptor& descriptor) const;
		FMTvertex_descriptor adddevelopment(const Core::FMTfuturdevelopment& futurdevelopement);
		size_t hash(size_t seed=0) const;
		void addaction(const int& actionID,
                        FMTgraphstats& statsdiff,
                        std::queue<FMTvertex_descriptor>& actives,
                        const FMTvertex_descriptor& out_vertex,
                        const std::vector<Core::FMTdevelopmentpath>& paths);
		double outarea(const FMTvertex_descriptor& out_vertex,
                        const int& actionID, const double*&  solution) const;
		double inarea(const FMTvertex_descriptor& out_vertex,
                        const double*&  solution, bool growth = false) const;
		bool periodstart(const FMTvertex_descriptor& out_vertex) const;
		bool periodstop(const FMTvertex_descriptor& out_vertex) const;
		std::vector<Core::FMTdevelopmentpath> getpaths(const FMTvertex_descriptor& out_vertex,
                                            const int& actionID) const;
		bool isvalidouputnode(const Models::FMTmodel& model, const Core::FMToutputnode& node, std::vector<const Core::FMTaction*>& action_IDS, int period) const;
		bool isvalidgraphnode(const Models::FMTmodel& model, const FMTvertex_descriptor& vertex_descriptor,
                            const Core::FMToutputnode& node,const std::vector<const Core::FMTaction*>& selected) const;
		std::map<int,double> locatenode(const Models::FMTmodel& model,Core::FMToutputnode output_node, int period) const;
		std::vector<FMTvertex_descriptor> getnode(const Models::FMTmodel& model, Core::FMToutputnode output_node,int period) const;
		std::map<int, double> getvariables(const Models::FMTmodel& model, const Core::FMToutputnode& output_node,const std::vector<FMTvertex_descriptor>& verticies) const;
        bool anyoperables(const FMTvertex_descriptor& descriptor, const std::vector<int>& action_ids) const;
		std::map<int, int> getoutvariables(const FMTvertex_descriptor& out_vertex) const;
		std::vector<int>getoutactions(const FMTvertex_descriptor& out_vertex) const;
		std::vector<const Core::FMTaction*> selectedactions(const Models::FMTmodel& model, const std::vector<int>& action_IDS) const;
		bool constraintlenght(const Core::FMTconstraint& constraint, int& start, int& stop) const;
		FMTgraphstats getstats() const;
		FMTgraphstats* getstatsptr();
		void setstats(const FMTgraphstats& newstats);
		FMTgraphstats buildschedule(const Models::FMTmodel& model, std::queue<FMTvertex_descriptor> actives,
			const Core::FMTschedule& schedule);
		FMTgraphstats eraseperiod(std::vector<int>& deletedconstraints,
								std::vector<int>&deletedvariables,
								bool keepbounded=false);
		bool empty() const;
		std::queue<FMTvertex_descriptor> getactiveverticies() const;
		const std::unordered_map<size_t, FMTvertex_descriptor>& getperiodverticies(int period) const;
		size_t size() const;
		void setconstraintID(const FMTvertex_descriptor& vertex, const int& id);
		bool gettransferrow(const FMTvertex_descriptor& vertex_descriptor,
			std::vector<int>&row_starts,
			std::vector<int>& cols,
			std::vector<double>& cols_value) const;
		void getinitialbounds(std::vector<double>& lower_bounds, std::vector<double>& upper_bounds) const;
		size_t nedges() const;
		std::map<std::string, double> getsource(const Models::FMTmodel& model,
			const Core::FMToutputnode& node,
			int period, const Core::FMTtheme& theme,
			const double* solution, FMToutputlevel level = FMToutputlevel::standard) const;
		std::map<std::string, double> getvalues(const Models::FMTmodel& model, const std::vector<FMTvertex_descriptor>& verticies,
			const Core::FMToutputnode& node, const Core::FMTtheme& theme,
			const double* solution, FMToutputlevel level) const;
        void generatedevelopments();
		bool sameedgesas(const FMTgraph& rhs) const;
		void updatematrixindex(const std::vector<int>& removedvariables,
			const std::vector<int>& removedconstraints);
		int getfirstactiveperiod() const;
		FMTgraph postsolve(const Core::FMTmask& selectedmask,
			const std::vector<Core::FMTtheme>&originalbasethemes,
			const std::map<int,int>& actionmapconnection) const;
    };
}
#endif // FMTGRAPH_H