/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
