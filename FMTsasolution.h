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
#include <unordered_map>
#include <vector>
#include <map>


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
class FMTsasolution : public FMTlayer<Graph::FMTgraph>
    {
	mutable std::unordered_map<size_t, std::vector<double>>outputscache;
    protected:
        Graph::FMTgraphstats solution_stats;
		std::vector<std::vector<std::vector<FMTevent<Graph::FMTgraph>>>> events;//v1 period v2 action id v3 FMTevent<FMTgraph>
        double objectivefunctionvalue;//Sum of all penalties
    public:
		std::map<std::string, std::pair<std::vector<double>, std::vector<double>>> constraint_outputs_penalties;
        FMTsasolution();
        FMTsasolution(const FMTforest& initialmap);
        virtual ~FMTsasolution()=default;
        FMTsasolution(const FMTsasolution& rhs);
        FMTsasolution& operator = (const FMTsasolution& rhs);
		bool copyfromselected(const FMTsasolution& rhs, const std::vector<size_t>& selected);
		bool swapfromselected(FMTsasolution& rhs, const std::vector<size_t>& selected);
        bool empty() const {return mapping.empty();};
        bool operator == (const FMTsasolution& rhs)const;
        bool operator != (const FMTsasolution& rhs)const;
        //Function to get info on the solution
        double getobjfvalue()const;
        Graph::FMTgraphstats getsolution_stats() const;
        const std::vector<std::vector<std::vector<FMTevent<Graph::FMTgraph>>>>& getevents() const;
        void getstartstop(const Core::FMTconstraint& constraint, int& periodstart,int& periodstop) const;
		std::vector<double> getgraphsoutputs(const Models::FMTmodel& model, const Core::FMTconstraint& constraint,
                                    const int& periodstart,const int& periodstop) const;
        double getgraphspenalties(const Models::FMTsamodel& model, const Core::FMTconstraint& constraint,
                                    const double& coef, std::vector<double>& output_vals, std::vector<double>& penalties_vals);
        double getspatialpenalties(const Models::FMTsamodel& model, const Core::FMTconstraint& constraint,
                                    const double& coef, std::vector<double>& output_vals, std::vector<double>& penalties_vals) const;
        FMTforest getforestperiod(const int& period) const;

        //Function to manipulate solution
        Graph::FMTgraphstats buildperiod(const Models::FMTmodel& model, std::default_random_engine& generator);
        FMTsasolution perturb( Models::FMTsamodel& model, std::default_random_engine& generator,
                               FMTsamovetype movetype = FMTsamovetype::shotgun,
                               const double min_ratio=0, const double max_ratio=1) const;
        double exponentialpenalty( const double& xvalue, const double& xlowerbound, const double& xupperbound,
                                    const double& maxpenalty,
                                    double curvelength = 0,
                                    double slope = 1) const;
        double applypenalty(const double& upper,const double& lower,
                            const double& value, const double& coef, const FMTsapenaltytype penalty_type) const;
        double evaluate(const Models::FMTsamodel& model);
        void write_events(const std::vector<Core::FMTaction> model_actions, const std::string out_path, const std::string addon = "") const;
    };
}
#endif // FMTSASOLUTION_H
