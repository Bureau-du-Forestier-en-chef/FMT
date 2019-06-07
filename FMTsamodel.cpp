#include "FMTsamodel.h"

namespace Models

{
    FMTsamodel::FMTsamodel():
        FMTmodel(),
        generator(),
        spactions(),
        cooling_schedule(),
        current_solution(),
        new_solution(),
        best_solution()
    {

    }
    FMTsamodel::FMTsamodel(const FMTsamodel& rhs):
        FMTmodel(rhs),
        generator(rhs.generator),
        spactions(rhs.spactions),
        cooling_schedule(rhs.cooling_schedule->Clone()),
        current_solution(rhs.current_solution),
        new_solution(rhs.new_solution),
        best_solution(rhs.best_solution)
    {

    }
    FMTsamodel::FMTsamodel(const FMTmodel& rhs):
        FMTmodel(rhs),
        generator(),
        spactions(),
        cooling_schedule(),
        current_solution(),
        new_solution(),
        best_solution()
    {

    }
    FMTsamodel& FMTsamodel::operator = (const FMTsamodel& rhs)
    {
        if (this!=&rhs)
            {
            FMTmodel::operator = (rhs);
            generator = rhs.generator;
            spactions = rhs.spactions;
            cooling_schedule = rhs.cooling_schedule->Clone();
            current_solution = rhs.current_solution;
            new_solution = rhs.new_solution;
            best_solution = rhs.best_solution;
            }
        return *this;
    }

    bool FMTsamodel::setschedule(const FMTlinearschedule& schedule)
    {
        cooling_schedule = unique_ptr<FMTlinearschedule>(new FMTlinearschedule(schedule));
        return true;
    }
    /*bool FMTsamodel::setschedule(FMTexponentialschedule schedule) const;
        {
        cooling_schedule = schedule
        }*///Need to build this class

    FMTsasolution FMTsamodel::get_current_solution()const
    {
            return current_solution;
    }
     bool FMTsamodel::setspactions(const vector<FMTspatialaction>& lspactions)
    {
		vector<FMTtransition>newtransitions;
		vector<FMTspatialaction>newspatials;
		vector<FMTaction>newbaseactions;
		for (const FMTspatialaction& spaction : lspactions)
			{
			vector<FMTtransition>::const_iterator trn_iterator = find_if(transitions.begin(), transitions.end(), FMTtransitioncomparator(spaction.name));
			if (trn_iterator!= transitions.end())
				{
				newtransitions.push_back(*trn_iterator);
				newspatials.push_back(spaction);
				newbaseactions.push_back(spaction);
			}else{
				_exhandler->raise(FMTexc::WSinvalid_transition,
					FMTwssect::Transition, "Missing transition case for action : " + spaction.name,__LINE__, __FILE__);
				return false;
				}
			}
        spactions = lspactions;
		actions = newbaseactions;
		transitions = newtransitions;
		return true;
    }
    bool FMTsamodel::setinitial_mapping(const FMTforest& forest)
    {
        current_solution = FMTsasolution(forest);
		return true;
    }
    double FMTsamodel::evaluate() const
    {
        return 0.0;
    }
    FMTgraphstats FMTsamodel::buildperiod()
    {
        return current_solution.buildperiod(*this,generator);
    }

    void FMTsamodel::move(FMTsamovetype movetype)
    {
        new_solution = current_solution.perturb(*this,generator,movetype);
    }

    bool FMTsamodel::comparesolutions() const
    {
        if (current_solution.getsolution_stats() == new_solution.getsolution_stats() && current_solution.getevents() == new_solution.getevents())
        {
            for (vector<size_t>::const_iterator it = mapidmodified.begin(); it != mapidmodified.end(); it++)
            {
                 map<FMTcoordinate,FMTgraph>::const_iterator current_solutionit =  current_solution.mapping.begin();
                 std::advance(current_solutionit,*it);
                 if(current_solutionit->second != new_solution.mapping.at(current_solutionit->first))
                 {
                     return false;
                 }
            }
            return true;
        }
        return false;
    }

    bool FMTsamodel::setmapidmodified(const vector<size_t>& id)
    {
        mapidmodified = id;
        return true;
    }
}
