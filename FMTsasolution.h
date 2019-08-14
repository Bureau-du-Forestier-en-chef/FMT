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

#ifndef FMTSASOLUTION_H
#define FMTSASOLUTION_H

#include "FMTlayer.h"
#include "FMTforest.h"
#include "FMTgraphdescription.h"
#include "FMTgraph.h"
#include "FMTgraphstats.h"

using namespace Spatial;
using namespace Graph;

namespace Models{class FMTsamodel;}//early declaration for samodel to pass it to sasolution
namespace Spatial
{
enum class FMTsamovetype
    {
        shotgun,
        cluster
    };
enum class FMTsapenaltytype
    {
        linear,
        exponential,
    };
class FMTsasolution : public FMTlayer<FMTgraph>
    {
    protected:
        FMTgraphstats solution_stats;
        vector<vector<vector<FMTevent<FMTgraph>>>> events;//v1 period v2 action id v3 FMTevent<FMTgraph>
        double objectivefunctionvalue;//Sum of all penalties
        map<string,double> events_meansize;

    public:
        map<string,pair<vector<double>,vector<double>>> constraint_outputs_penalties;
        FMTsasolution();
        FMTsasolution(const FMTforest& initialmap);
        virtual ~FMTsasolution()=default;
        FMTsasolution(const FMTsasolution& rhs);
        FMTsasolution& operator = (const FMTsasolution& rhs);
        bool empty() const {return mapping.empty();};
        bool operator == (const FMTsasolution& rhs)const;
        bool operator != (const FMTsasolution& rhs)const;

        //Function to get info on the solution

        map<string,double> geteventmeansize(const FMTsamodel& model);
        double getobjfvalue()const;
        FMTgraphstats getsolution_stats() const;
        const vector<vector<vector<FMTevent<FMTgraph>>>>& getevents() const;
        void getstartstop(const FMTconstraint& constraint, int& periodstart,int& periodstop) const;
        vector<double> getgraphsoutputs(const FMTmodel& model, const FMTconstraint& constraint,
                                    const int& periodstart,const int& periodstop) const;
        double getgraphspenalties(const FMTsamodel& model, const FMTconstraint& constraint,
                                    const double& coef, vector<double>& output_vals, vector<double>& penalties_vals);
        double getspatialpenalties(const FMTsamodel& model, const FMTconstraint& constraint,
                                    const double& coef, vector<double>& output_vals, vector<double>& penalties_vals) const;
        FMTforest getforestperiod(const int& period) const;

        //Function to manipulate solution
        FMTgraphstats buildperiod(const FMTmodel& model,default_random_engine& generator);
        FMTsasolution perturb( FMTsamodel& model, default_random_engine& generator,
                               FMTsamovetype movetype = FMTsamovetype::shotgun,
                               const double min_ratio=0, const double max_ratio=1) const;
        double exponentialpenalty( const double& xvalue, const double& xlowerbound, const double& xupperbound,
                                    const double& maxpenalty,
                                    double curvelength = 0,
                                    double slope = 1) const;
        double applypenalty(const double& upper,const double& lower,
                            const double& value, const double& coef, const FMTsapenaltytype penalty_type) const;
        double evaluate(const FMTsamodel& model);
        void write_events(const vector<FMTaction> model_actions, const string out_path, const string addon = "") const;
    };
}
#endif // FMTSASOLUTION_H
