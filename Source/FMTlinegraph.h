/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTLINEGRAPH_H
#define FMTLINEGRAPH_H

#include "FMTgraph.h"

#include "FMTsaeventcontainer.h"

namespace Graph
{
class FMTlinegraph : public FMTgraph
{
    public:
        FMTlinegraph();
        ~FMTlinegraph()=default;
        FMTlinegraph(const FMTgraphbuild lbuildtype);
        FMTlinegraph(const FMTlinegraph& rhs);
        FMTlinegraph(const FMTgraph& rhs);
        FMTlinegraph& operator = (const FMTlinegraph& rhs);
        FMTlinegraph& operator = (const FMTgraph& rhs);
        int getlastperiod() const;
		int randomoperate(const std::vector<int>& operables, const Models::FMTmodel& model, std::queue<FMTvertex_descriptor>& actives,
                                            FMTgraphstats& statsdiff, const FMTvertex_descriptor& front_vertex, std::default_random_engine& generator,
                                            const Core::FMTdevelopment& active_development);
        FMTgraphstats randombuild(const Models::FMTmodel& model,std::queue<FMTvertex_descriptor> actives, std::default_random_engine& generator,
                                  Spatial::FMTsaeventcontainer& events, const Spatial::FMTcoordinate& localisation);
        FMTgraphstats clearfromperiod(const int& period);
        FMTlinegraph partialcopy(const int& period) const;
        FMTlinegraph perturbgraph(  const Models::FMTmodel& model, std::default_random_engine& generator,
                                Spatial::FMTsaeventcontainer& events,
                                const Spatial::FMTcoordinate& localisation, const int period) const;
};
}

#endif // FMTLINEGRAPH_H
