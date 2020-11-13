/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTLINEGRAPH_H
#define FMTLINEGRAPH_H

#include "FMTgraph.h"

#include "FMTeventcontainer.h"
#include "FMTspatialaction.h"
#include "FMTbindingspatialaction.h"
#include <boost/serialization/export.hpp>

namespace Graph
{
class FMTlinegraph : public FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("Graph::FMTgraph<Graph::FMTbasevertexproperties,Graph::FMTbaseedgeproperties>",boost::serialization::base_object<FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>>(*this));
		ar & BOOST_SERIALIZATION_NVP(periodstatsdiff);

	}
	protected:
		FMTgraphstats periodstatsdiff;
    public:
        FMTlinegraph();
        ~FMTlinegraph()=default;
        FMTlinegraph(const FMTgraphbuild lbuildtype);
        FMTlinegraph(const FMTlinegraph& rhs);
        FMTlinegraph(const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>& rhs);
        FMTlinegraph& operator = (const FMTlinegraph& rhs);
        FMTlinegraph& operator = (const FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>& rhs);
		void newperiod();
		void setaction(	FMTvertex_descriptor active,const int& actionID,
						const std::vector<Core::FMTdevelopmentpath>& paths);
		std::vector<Core::FMTdevelopmentpath> operate(	const Core::FMTaction& action, const int& action_id, const Core::FMTtransition& transition, 
														const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes);
		void grow();
		FMTvertex_descriptor getactivevertex() const;
		int getlastactionid(const int& period)const;
		const Core::FMTdevelopment& getperiodstartdev(const int& period) const;
		const Core::FMTdevelopment& getperiodstopdev(const int& period) const;
		int randomoperate(const std::vector<int>& operables, const Models::FMTmodel& model, std::queue<FMTvertex_descriptor>& actives,
                                            FMTgraphstats& statsdiff, const FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
                                            const Core::FMTdevelopment& active_development,bool dontchoosegrow=false);
        FMTgraphstats randombuildperiod(const Models::FMTmodel& model, std::queue<FMTvertex_descriptor> actives, std::default_random_engine& generator,
									Spatial::FMTeventcontainer& events, const Spatial::FMTcoordinate& localisation,
									std::unordered_map<size_t, std::vector<int>>* operability=nullptr,
									const std::vector<Spatial::FMTbindingspatialaction>* bindings = nullptr,
									bool dontchoosegrow=false);
        FMTgraphstats clearfromperiod(const int& period,bool updatedevelopments=false);
        FMTlinegraph partialcopy(const int& period) const;
        FMTlinegraph perturbgraph(  const Models::FMTmodel& model, std::default_random_engine& generator,
									Spatial::FMTeventcontainer& events,
									const Spatial::FMTcoordinate& localisation, const int period) const;
		std::vector<std::vector<bool>>getactions(const Models::FMTmodel& model,const int& fromperiod,
			std::map<Core::FMTdevelopment, std::vector<bool>>& operability) const;
		const Core::FMTdevelopment& getbasedevelopment() const;
		size_t getbasehash(const Core::FMTmask& dynamicmask) const;
		size_t getedgeshash(const int& maximalperiod,bool& gotthewhole) const;
		bool hashforconstraint(size_t& hashvalue,const int& stop,const Core::FMTmask& dynamicmask) const;
		bool isonlygrow() const;
		void addfromevents(const Spatial::FMTcoordinate& localisation,const Models::FMTmodel& model, Spatial::FMTeventcontainer& events) const;
		bool ismovable(const std::vector<const Core::FMTaction*>& actions,
			const Core::FMTyields& yields, const int& period,std::unordered_map<size_t,bool>*operability=nullptr) const;
		bool operator == (const FMTlinegraph& rhs) const;
		
};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTlinegraph)

#endif // FMTLINEGRAPH_H
