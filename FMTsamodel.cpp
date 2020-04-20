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

#include "FMTsamodel.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <boost/filesystem.hpp>
#include <cstring>

namespace Models

{
    Graph::FMTgraphstats FMTsamodel::g_move_solution(const double min_ratio,const double max_ratio)
    {
        number_of_moves ++;
        new_solution = current_solution.perturb(*this,generator,movetype,min_ratio,max_ratio);
        return new_solution.getsolution_stats();
    }

    bool FMTsamodel::comparesolutions() const
    {
        if (current_solution.getsolution_stats() == new_solution.getsolution_stats() && current_solution.getevents() == new_solution.getevents())
        {
            for (std::vector<size_t>::const_iterator it = mapidmodified.begin(); it != mapidmodified.end(); it++)
            {
				std::map<Spatial::FMTcoordinate,Graph::FMTlinegraph>::const_iterator current_solutionit =  current_solution.mapping.begin();
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

    FMTsamodel::FMTsamodel():
        FMTmodel(),
        movetype(Spatial::FMTsamovetype::opt1),
        min_ratio_moves(0),
        max_ratio_moves(1),
        outputs_write_location(),
        number_of_moves(0),
        constraints_values_penalties(),
        generator(),
        spactions(),
        accepted_solutions(),
        mapidmodified(),
        probabs(),
        cooling_schedule(),
        best_solution(),
        current_solution(),
        new_solution()
    {

    }

    FMTsamodel::FMTsamodel(const FMTsamodel& rhs):
        FMTmodel(rhs),
        movetype(rhs.movetype),
        min_ratio_moves(rhs.min_ratio_moves),
        max_ratio_moves(rhs.max_ratio_moves),
        outputs_write_location(rhs.outputs_write_location),
        number_of_moves(0),
        constraints_values_penalties(rhs.constraints_values_penalties),
        generator(rhs.generator),
        spactions(rhs.spactions),
        accepted_solutions(rhs.accepted_solutions),
        mapidmodified(),
        probabs(rhs.probabs),
        cooling_schedule(rhs.cooling_schedule->Clone()),
        best_solution(rhs.best_solution),
        current_solution(rhs.current_solution),
        new_solution(rhs.new_solution)
    {

    }

    FMTsamodel::FMTsamodel(const FMTmodel& rhs):
        FMTmodel(rhs),
        movetype(Spatial::FMTsamovetype::opt1),
        min_ratio_moves(0),
        max_ratio_moves(1),
        outputs_write_location(),
        number_of_moves(0),
        constraints_values_penalties(),
        generator(),
        spactions(),
        accepted_solutions(),
        mapidmodified(),
        probabs(),
        cooling_schedule(),
        best_solution(),
        current_solution(),
        new_solution()
    {

    }

    FMTsamodel& FMTsamodel::operator = (const FMTsamodel& rhs)
    {
        if (this!=&rhs)
            {
            FMTmodel::operator = (rhs);
            movetype = rhs.movetype;
            min_ratio_moves = rhs.min_ratio_moves;
            max_ratio_moves = rhs.max_ratio_moves;
            outputs_write_location = rhs.outputs_write_location;
            number_of_moves = rhs.number_of_moves;
            constraints_values_penalties = rhs.constraints_values_penalties;
            generator = rhs.generator;
            spactions = rhs.spactions;
            accepted_solutions = rhs.accepted_solutions;
            mapidmodified = rhs.mapidmodified;
            probabs = rhs.probabs;
            cooling_schedule = rhs.cooling_schedule->Clone();
            best_solution=rhs.best_solution;
            current_solution = rhs.current_solution;
            new_solution = rhs.new_solution;
            }
        return *this;
    }

    double FMTsamodel::warmup(const double initprob, const size_t iterations,bool keep_best,FMTsawarmuptype type)
    {
        const Spatial::FMTsasolution first=current_solution;//Put it as current at the end
        double temp = 0;
        const  double mean_ratio = (max_ratio_moves - min_ratio_moves)/2;
        size_t iter = iterations;
        switch(type)
        {
        case FMTsawarmuptype::log :
            {
                double sum_delta = 0;
                size_t num_delta = 0;
                while (iter>0)
                {
                    move_solution();
                    if(evaluate(0))//The temp need to be 0 to do only greedy to track what is upgraded because we accept every solution
                    {
                        const double delta = current_solution.getobjfvalue()-new_solution.getobjfvalue();
                        sum_delta += delta;
                        ++num_delta;
                    }
                    acceptnew();
                    --iter;
                }
                temp = -(sum_delta/num_delta)/log(initprob);
                break;
            }
        case FMTsawarmuptype::logmax :
            {
                double maxdelta = 0;
                while (iter>0)
                {
                    move_solution();
                    if(evaluate(0))//The temp need to be 0 to do only greedy to track what is upgraded because we accept every solution
                    {
                        const double delta = fabs(new_solution.getobjfvalue()-current_solution.getobjfvalue());
                        if (delta>maxdelta)
                        {
                            maxdelta = delta;
                        }
                    }
                    acceptnew();
                    --iter;
                    *_logger<<maxdelta<<" : delta at iter num "<<number_of_moves<<"\n";
                }
                temp = -(maxdelta)/log(initprob);
                break;
            }
        case FMTsawarmuptype::bigdelta :
            {
                double maxdelta = 0;
                while (iter>0)
                {
                    g_move_solution(mean_ratio,mean_ratio);//Systematicaly evaluate the mean_ratio
                    evaluate(0);//The temp need to be 0 to do only greedy to track what is upgraded because we accept every solution
                    const double delta = fabs(current_solution.getobjfvalue()-new_solution.getobjfvalue());
                    if (delta>maxdelta)
                    {
                            maxdelta = delta;
                    }
                    acceptnew();
                    --iter;
                }
                temp = maxdelta;
                break;
            }
        case FMTsawarmuptype::bootstrapmagic :
            {
				std::vector<double> penalties;
                while(iter > 0)
                {
                    move_solution();
                    evaluate(0);
                    if (number_of_moves == 1)
                    {
                        penalties.push_back(current_solution.getobjfvalue());
                    }
                    penalties.push_back(new_solution.getobjfvalue());
                    acceptnew();
                    --iter;
                }
                //Bootstrap
                int sample_num = 0;
				std::uniform_int_distribution<int> penalties_distribution(0,penalties.size()-1);
				std::vector<std::pair<double,double>> pmin_max;
                while(sample_num<100)
                {
                    int sample_size = 0;
					std::vector<double> sample;
                    while (sample_size<100)
                    {
                        const int lucky_penalty = penalties_distribution(generator);
                        sample.push_back(penalties.at(lucky_penalty));
                        sample_size++;
                    }
                    pmin_max.push_back(std::pair<double,double>(*std::min_element(sample.begin(),sample.end()),*std::max_element(sample.begin(),sample.end())));
                    sample_num++;
                }
                //Magie
                const double tempmin = *std::max_element(penalties.begin(),penalties.end());
                double templ = tempmin;
                double pcalculate = 0;
                while (pcalculate<=initprob)
                {
                    double totemin=0;
                    double totemax=0;
                    for (const std::pair<double,double>& min_max : pmin_max)
                    {
                        totemin+=exp(-min_max.first/templ);
                        totemax+=exp(-min_max.second/templ);
                    }
                    pcalculate = totemax/totemin;
                    *_logger<<pcalculate<<"\n";
                    if (pcalculate<initprob)
                    {
                        templ+=tempmin*0.05;
                    }
                }
                temp = templ;
                break;
            }
        default :
            break;

        }
        if (!keep_best)
        {
            current_solution=first;
        }
        else
        {
            current_solution=best_solution;
        }
        get_outputs("warmup_");
        //Only to reset samodel parameters to default
        number_of_moves = 0;
        new_solution = Spatial::FMTsasolution();
        best_solution = Spatial::FMTsasolution();
        mapidmodified = std::vector<size_t>();
        return temp;
    }

    void FMTsamodel::write_outputs_at(std::string path)
    {
        outputs_write_location = path;
        if (boost::filesystem::exists(path+"outputs.csv"))
        {
            const std::string outputs= path+"outputs.csv";
            char *cstr = new char[outputs.size() + 1];
            strcpy(cstr, outputs.c_str());
            remove(cstr);
            delete[]cstr;
        }

    }

    bool FMTsamodel::setschedule(const Spatial::FMTexponentialschedule& schedule)
    {
        cooling_schedule = std::unique_ptr<Spatial::FMTexponentialschedule>(new Spatial::FMTexponentialschedule(schedule));
        return true;
    }

    bool FMTsamodel::setspactions(const std::vector<Spatial::FMTspatialaction>& lspactions)
    {
		std::vector<Core::FMTtransition>newtransitions;
		std::vector<Spatial::FMTspatialaction>newspatials;
		std::vector<Core::FMTaction>newbaseactions;
		for (const Spatial::FMTspatialaction& spaction : lspactions)
			{
			std::vector<Core::FMTtransition>::const_iterator trn_iterator = std::find_if(transitions.begin(), transitions.end(), Core::FMTtransitioncomparator(spaction.getname()));
			if (trn_iterator!= transitions.end())
				{
				newtransitions.push_back(*trn_iterator);
				newspatials.push_back(spaction);
				newbaseactions.push_back(spaction);
				if (spaction.simulated_annealing_valid())
                {
					std::vector<Core::FMTconstraint> newconst = spaction.to_constraints();
                    for (Core::FMTconstraint& constraint : newconst)
                    {
                        constraints.push_back(constraint);
                    }
                }
			}else{
				_exhandler->raise(Exception::FMTexc::WSinvalid_transition, Core::FMTwssect::Transition, "Missing transition case for action : " + spaction.getname(),__LINE__, __FILE__);
				return false;
				}
			}
        spactions = lspactions;
		actions = newbaseactions;
		transitions = newtransitions;
		return true;
    }

    bool FMTsamodel::set_min_max_moves(const double min_r,const double max_r)
    {
        min_ratio_moves = min_r;
        max_ratio_moves = max_r;
        return true;
    }

    bool FMTsamodel::set_movetype(const Spatial::FMTsamovetype movet)
    {
        movetype = movet;
        if (movetype == movet)
        {
             return true;
        }
        return false;
    }

    int FMTsamodel::get_number_moves()const
    {
        return number_of_moves;
    }

    void FMTsamodel::get_outputs(std::string addon)
    {
        if (outputs_write_location.empty())
        {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path, Core::FMTwssect::Empty, "No path given to the function write_outputs_at ",__LINE__, __FILE__);
        }
        else
        {
			std::fstream outputFile;
			const std::string filename = outputs_write_location+addon+"outputs.csv";
			const std::string headers = "Move,Constraint,Period,Output,Penalty,P";
            outputFile.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
			std::vector<int>::const_iterator move_num=accepted_solutions.begin();
			std::vector<double>::const_iterator probs=probabs.begin();
            if (*move_num==0)
            {
                outputFile<<headers<<std::endl;
            }
            for (const std::map<std::string, std::pair<std::vector<double>, std::vector<double>>>& move_info : constraints_values_penalties)
            {
                for (std::map<std::string, std::pair<std::vector<double>, std::vector<double>>>::const_iterator mapit = move_info.begin(); mapit != move_info.end(); ++mapit)
                {
                    const std::string& cname = mapit->first;
                    const std::vector<double>& outputs = mapit->second.first;
                    const std::vector<double>& penalties = mapit->second.second;
                    for (size_t i = 0 ; i < outputs.size() ; ++i )
                    {
                        outputFile<<*move_num<<","<<cname<<","<<i+1<<","<<outputs.at(i)<<","<<penalties.at(i)<<","<<*probs<<std::endl;
                    }
                }
                ++move_num;
                ++probs;
            }
            constraints_values_penalties.clear();
            accepted_solutions.clear();
            probabs.clear();
            outputFile.close();
        }
    }

    Spatial::FMTsasolution FMTsamodel::get_current_solution()const
    {
            return current_solution;
    }

	Spatial::FMTsasolution FMTsamodel::get_new_solution()const
    {
            return new_solution;
    }

    void FMTsamodel::write_solutions_events(std::string out_path)const
    {
        if (number_of_moves>0)
        {
            current_solution.write_events(actions,out_path,"Current_solution");
            new_solution.write_events(actions,out_path,"New_solution");
            best_solution.write_events(actions,out_path,"Best_solution");
        }
        else
        {
            current_solution.write_events(actions,out_path,"Current_solution");
        }
    }

   std::vector<Spatial::FMTspatialaction> FMTsamodel::getspatialactions()const
    {
        return spactions;
    }

    bool FMTsamodel::setinitial_mapping(const Spatial::FMTforest& forest)
    {
        current_solution = Spatial::FMTsasolution(forest);
		return true;
    }

    void FMTsamodel::acceptnew()
    {
		current_solution.copyfromselected(new_solution, mapidmodified);
		new_solution = Spatial::FMTsasolution();
    }

    bool FMTsamodel::testprobability(const double& p) //Metropolis criterion
    {
        std::uniform_real_distribution<double> r_dist(0.0,1);
        const double r = r_dist(generator);
        if (p > r)
        {
            return true;
        }
        return false;
    }

    bool FMTsamodel::evaluate(const double temp,bool all_data)
    //Get the output, evaluate the penalties and compare solutions
    {
        if (!comparesolutions())// if compare solution return false ... which means they are different
        {
            double cur_obj = 0;
            double best_obj = 0;
            double p = 0;
            if ( number_of_moves == 1)//Evaluate initial solution and write results
            {
                cur_obj = current_solution.evaluate(*this);
                best_solution = current_solution;
                best_obj = best_solution.getobjfvalue();
                accepted_solutions.push_back(0);
                constraints_values_penalties.push_back(current_solution.constraint_outputs_penalties);
            }
            else
            {
                cur_obj = current_solution.getobjfvalue();
                best_obj = best_solution.getobjfvalue();
            }
            //Evaluate new solution
            const double new_obj = new_solution.evaluate(*this);
            if (best_obj>new_obj)
            {
				best_solution.copyfromselected(new_solution, mapidmodified);
            }
            if ( cur_obj>new_obj )//If new is better than the last_best
            {
                p = 1;
            }
            else
                {
                    p = std::exp((-1*(cur_obj)-new_obj)/temp);
                }
            if (testprobability(p))
            {
                accepted_solutions.push_back(number_of_moves);
                probabs.push_back(p);
                constraints_values_penalties.push_back(new_solution.constraint_outputs_penalties);
                return true;
            }
        }
        else
        {
            if ( number_of_moves == 1)//Evaluate initial solution and write results
            {
                current_solution.evaluate(*this);
                best_solution = current_solution;
            }
        }
        return false;
    }

    Graph::FMTgraphstats FMTsamodel::buildperiod()
    {
        return current_solution.buildperiod(*this,generator);
    }

    Graph::FMTgraphstats FMTsamodel::move_solution()
    {
        return g_move_solution(min_ratio_moves,max_ratio_moves);
    }

    bool FMTsamodel::setmapidmodified(const std::vector<size_t>& id)
    {
        mapidmodified = id;
        return true;
    }
}
