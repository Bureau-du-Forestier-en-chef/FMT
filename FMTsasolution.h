#ifndef FMTSASOLUTION_H
#define FMTSASOLUTION_H

#include "FMTlayer.h"
#include "FMTforest.h"
#include "FMTgraphdescription.h"
#include "FMTgraph.h"
#include "FMTgraphstats.h"

using namespace Spatial;
using namespace Graph;

namespace Models{class FMTsamodel;}//early declaration for samodel to pass it to
namespace Spatial
{
enum class FMTsamovetype
    {
        shotgun,
        cluster
    };
class FMTsasolution : public FMTlayer<FMTgraph>
    {
    private:
        bool initial_map;
    protected:
        FMTgraphstats solution_stats;
        vector<vector<vector<FMTevent<FMTgraph>>>> events;//v1 period v2 action id v3 FMTevent<FMTgraph>
    public:
        FMTsasolution(){};
        FMTsasolution(const FMTforest& initialmap);
        virtual ~FMTsasolution()=default;
        FMTsasolution(const FMTsasolution& rhs);
        FMTsasolution& operator = (const FMTsasolution& rhs);
        bool operator == (const FMTsasolution& rhs)const;
        bool operator != (const FMTsasolution& rhs)const;
        FMTgraphstats buildperiod(const FMTmodel& model,default_random_engine& generator);
        FMTsasolution perturb( FMTsamodel& model, default_random_engine& generator,
                               FMTsamovetype movetype = FMTsamovetype::shotgun) const;
        const FMTgraphstats& getsolution_stats() const;
        const vector<vector<vector<FMTevent<FMTgraph>>>>& getevents() const;
        double getgraphsconstraint(const FMTmodel& model, const FMTconstraint& constraint)const;
        double geteventsconstraint(const FMTsamodel& model) const;
        double evaluatey(const FMTconstraint& constraint, const double& xvalue) const;
    };
}
#endif // FMTSASOLUTION_H
