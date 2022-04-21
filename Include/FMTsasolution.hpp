/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSASOLUTION_H
#define FMTSASOLUTION_H

#include "FMTlayer.hpp"
#include "FMTeventcontainer.hpp"
#include "FMTlinegraph.hpp"
#include "FMTgraphstats.hpp"
#include <unordered_map>
#include <vector>
#include <map>


namespace Models{class FMTsamodel;}//early declaration for samodel to pass it to sasolution
namespace Spatial
{
class FMTforest;
enum class FMTsamovetype
    {
        shotgun = 1,
        cluster = 2,
        opt1 = 3
    };
enum class FMTsapenaltytype
    {
        linear = 1,
        exponential = 2
    };

// DocString: FMTsasolution
/**
This class is a map containing a linear graph for each pixel. The graph represent
the action and transition at each each period for the pixel. It's the solution of
the FMTsamodel. It's in this class that the "move" is implemented in the function
perturb graph. It's also in this class that the value of the objective function for
the solution is store.
*/
class FMTsasolution : public FMTlayer<Graph::FMTlinegraph>
    {
	mutable std::unordered_map<size_t, std::vector<double>>outputscache;
    protected:
        Graph::FMTgraphstats solution_stats;
        double objectivefunctionvalue;//Sum of all penalties
        FMTeventcontainer events;//Modifying in FMTlinegraph
    public:
		std::map<std::string, std::pair<std::vector<double>, std::vector<double>>> constraint_outputs_penalties;
        FMTsasolution();
        FMTsasolution(const FMTforest& initialmap);
        ~FMTsasolution()=default;
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
        const FMTeventcontainer& getevents() const;
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
