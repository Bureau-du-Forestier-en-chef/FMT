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

namespace Graph
{
class FMTlinegraph : public FMTgraph
{
	protected:
		FMTgraphstats periodstatsdiff;
    public:
        FMTlinegraph();
        ~FMTlinegraph()=default;
        FMTlinegraph(const FMTgraphbuild lbuildtype);
        FMTlinegraph(const FMTlinegraph& rhs);
        FMTlinegraph(const FMTgraph& rhs);
        FMTlinegraph& operator = (const FMTlinegraph& rhs);
        FMTlinegraph& operator = (const FMTgraph& rhs);
        int getperiod() const;
		void newperiod();
		void addaction(	const int& actionID,
						const std::vector<Core::FMTdevelopmentpath>& paths);
		std::vector<Core::FMTdevelopmentpath> operate(	const Core::FMTaction& action, const int& action_id, const Core::FMTtransition& transition, 
														const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes);
		void grow();
		FMTvertex_descriptor getactivevertex() const;
		int randomoperate(const std::vector<int>& operables, const Models::FMTmodel& model, std::queue<FMTvertex_descriptor>& actives,
                                            FMTgraphstats& statsdiff, const FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
                                            const Core::FMTdevelopment& active_development);
        FMTgraphstats randombuildperiod(const Models::FMTmodel& model,std::queue<FMTvertex_descriptor> actives, std::default_random_engine& generator,
                                  Spatial::FMTeventcontainer& events, const Spatial::FMTcoordinate& localisation);
        FMTgraphstats clearfromperiod(const int& period);
        FMTlinegraph partialcopy(const int& period) const;
        FMTlinegraph perturbgraph(  const Models::FMTmodel& model, std::default_random_engine& generator,
									Spatial::FMTeventcontainer& events,
									const Spatial::FMTcoordinate& localisation, const int period) const;
};
}

#endif // FMTLINEGRAPH_H
