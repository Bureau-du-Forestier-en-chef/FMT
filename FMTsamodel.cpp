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
    FMTsamodel::FMTsamodel():
        FMTmodel(),
        outputs_write_location(),
        number_of_moves(0),
        last_write(0),
        constraints_values_penalties(),
        generator(),
        spactions(),
        bests_solutions(),
        accepted_solutions(),
        mapidmodified(),
        cooling_schedule(),
        best_solution(),
        current_solution(),
        new_solution()
    {

    }

    FMTsamodel::FMTsamodel(const FMTsamodel& rhs):
        FMTmodel(rhs),
        outputs_write_location(rhs.outputs_write_location),
        number_of_moves(0),
        last_write(0),
        constraints_values_penalties(rhs.constraints_values_penalties),
        generator(rhs.generator),
        spactions(rhs.spactions),
        bests_solutions(rhs.bests_solutions),
        accepted_solutions(rhs.accepted_solutions),
        mapidmodified(),
        cooling_schedule(rhs.cooling_schedule->Clone()),
        best_solution(rhs.best_solution),
        current_solution(rhs.current_solution),
        new_solution(rhs.new_solution)
    {

    }

    FMTsamodel::FMTsamodel(const FMTmodel& rhs):
        FMTmodel(rhs),
        outputs_write_location(),
        number_of_moves(0),
        last_write(0),
        constraints_values_penalties(),
        generator(),
        spactions(),
        bests_solutions(),
        accepted_solutions(),
        mapidmodified(),
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
            outputs_write_location = rhs.outputs_write_location;
            number_of_moves = rhs.number_of_moves;
            last_write = rhs.last_write;
            constraints_values_penalties = rhs.constraints_values_penalties;
            generator = rhs.generator;
            spactions = rhs.spactions;
            bests_solutions = rhs.bests_solutions;
            accepted_solutions = rhs.accepted_solutions;
            mapidmodified = rhs.mapidmodified;
            cooling_schedule = rhs.cooling_schedule->Clone();
            best_solution=rhs.best_solution;
            current_solution = rhs.current_solution;
            new_solution = rhs.new_solution;
            }
        return *this;
    }

    double FMTsamodel::warmup(const double initprob,bool keep_best,int iternum)
    {
        FMTsasolution best=current_solution;//Put it as current at the end
        int iter = iternum;
        double sum_delta = 0;
        size_t num_delta = 0;
        while (iter>0)
        {
            move_solution();
            if(evaluate(0,false))//The temp need to be 0 to do only greedy to track what is upgraded because we accept every solution
            {
                double delta = current_solution.getobjfvalue()-new_solution.getobjfvalue();
                sum_delta += delta;
                ++num_delta;
                if (keep_best)
                {
                    if (best.getobjfvalue()>new_solution.getobjfvalue())
                    {
                        best = new_solution;
                    }
                }
            }
            acceptnew();
            --iter;
        }
        current_solution=best;
        number_of_moves = 0;
        new_solution = FMTsasolution();
        mapidmodified = vector<size_t>();
        cout<<sum_delta<<" : "<<num_delta<<endl;
        return -(sum_delta/num_delta)/log(initprob);
    }

    void FMTsamodel::write_outputs_at(string path)
    {
        outputs_write_location = path;
        if (boost::filesystem::exists(path+"outputs.csv"))
        {
            const string outputs= path+"outputs.csv";
            char *cstr = new char[outputs.size() + 1];
            strcpy(cstr, outputs.c_str());
            remove(cstr);
            delete[]cstr;
        }

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
				if (spaction.simulated_annealing_valid())
                {
                    vector<FMTconstraint> newconst = spaction.to_constraints();
                    for (FMTconstraint& constraint : newconst)
                    {
                        constraints.push_back(constraint);
                    }
                }
			}else{
				_exhandler->raise(FMTexc::WSinvalid_transition,FMTwssect::Transition, "Missing transition case for action : " + spaction.name,__LINE__, __FILE__);
				return false;
				}
			}
        spactions = lspactions;
		actions = newbaseactions;
		transitions = newtransitions;
		//Test spatial actions to constraints because bounds are not set ???
		/*for(FMTconstraint& constraint : constraints){
                                                        cout<<constraint.name<<" : "<<constraint.getconstrainttype()<<endl;
                                                        double upper = 0;
                                                        double lower = 0;
                                                        double coef = 0;
                                                        string name;
                                                        constraint.getbounds(lower,upper,0);
                                                        constraint.getgoal(name,coef);
                                                        cout<<upper<<" : "<<lower<<"      "<<name<<" : "<<coef<<endl;
                                                    }*/
		return true;
    }

    int FMTsamodel::get_number_moves()const
    {
        return number_of_moves;
    }

    void FMTsamodel::get_outputs(const bool only_accepted)
    {
        if (outputs_write_location.empty())
        {
            _exhandler->raise(FMTexc::FMTinvalid_path,FMTwssect::Empty, "No path given to the function write_outputs_at ",__LINE__, __FILE__);
        }
        else
        {
            fstream outputFile;
            string filename = outputs_write_location+"outputs.csv";
            string headers = "Move,Constraint,Period,Output,Penalty,Best solution,Last accepted";
            outputFile.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
            int move_num = last_write;
            if (move_num==0)
            {
                outputFile<<headers<<endl;
            }
            for (const map<string,pair<vector<double>,vector<double>>>& move_info : constraints_values_penalties)
            {
                for (map<string,pair<vector<double>,vector<double>>>::const_iterator mapit = move_info.begin(); mapit != move_info.end(); ++mapit)
                {
                    const string& cname = mapit->first;
                    const vector<double>& outputs = mapit->second.first;
                    const vector<double>& penalties = mapit->second.second;
                    for (int i = 0 ; i < outputs.size() ; ++i )
                    {
                        if (move_num==0)
                        {
                            outputFile<<move_num<<","<<cname<<","<<i+1<<","<<outputs.at(i)<<","<<penalties.at(i)<<","<<0<<","<<0<<endl;
                        }
                        else
                        {
                            const int& best = bests_solutions.at(move_num-1);
                            const int& last_accepted = accepted_solutions.at(move_num-1);
                            if (only_accepted)
                            {
                                if ( best == move_num || last_accepted == move_num )
                                {
                                outputFile<<move_num<<","<<cname<<","<<i+1<<","<<outputs.at(i)<<","<<penalties.at(i)<<","<<best<<","<<last_accepted<<endl;
                                }
                            }
                            else
                            {
                                outputFile<<move_num<<","<<cname<<","<<i+1<<","<<outputs.at(i)<<","<<penalties.at(i)<<","<<best<<","<<last_accepted<<endl;
                            }
                        }
                    }
                }
            ++move_num;
            }
            constraints_values_penalties.clear();
            outputFile.close();
            last_write = move_num;
        }
    }
            /*for (map<pair<int,string>,vector<vector<double>>>::const_iterator mapit = constraints_values_penalties.begin(); mapit != constraints_values_penalties.end(); ++mapit)
            {
                const int& move_num = mapit->first.first;
                const string& constraint_name = mapit->first.second;
                const vector<vector<double>>& outputs = mapit->second;
                for (int i = 0 ; i < outputs.at(0).size() ; ++i )
                {
                    const double& output = outputs.at(0).at(i);
                    const double& penalty = outputs.at(1).at(i);
                    if (move_num==0)
                    {
                        if (i==0 && mapit==constraints_values_penalties.begin())
                        {
                            outputFile<<headers<<endl;
                        }
                        outputFile<<move_num<<","<<constraint_name<<","<<i+1<<","<<output<<","<<penalty<<","<<0<<","<<0<<endl;
                    }
                    else
                    {
                        const int& best = bests_solutions.at(move_num-1);
                        const int& last_accepted = accepted_solutions.at(move_num-1);
                        if (only_accepted)
                        {
                            if ( best == move_num || last_accepted == move_num)
                            {
                            outputFile<<move_num<<","<<constraint_name<<","<<i+1<<","<<outputs.at(0).at(i)<<","<<outputs.at(1).at(i)<<","<<best<<","<<last_accepted<<endl;
                            }
                        }
                        else
                        {
                            outputFile<<move_num<<","<<constraint_name<<","<<i+1<<","<<outputs.at(0).at(i)<<","<<outputs.at(1).at(i)<<","<<best<<","<<last_accepted<<endl;
                        }

                    }
                }
            }
            constraints_values_penalties.clear();
            outputFile.close();
        }
    }*/

    FMTsasolution FMTsamodel::get_current_solution()const
    {
            return current_solution;
    }

    FMTsasolution FMTsamodel::get_new_solution()const
    {
            return new_solution;
    }

    void FMTsamodel::write_solutions_events(string out_path)const
    {
        if (number_of_moves>0)
        {
            current_solution.write_events(actions,out_path,"Current_solution");
            new_solution.write_events(actions,out_path,"New_solution");
        }
        else
        {
            current_solution.write_events(actions,out_path,"Current_solution");
        }
    }

    void FMTsamodel::get_events_mean_size(string path)
    {
        fstream outputFile;
        string filename = path+"events_mean_size.csv";
        outputFile.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
        outputFile<<"solution,constraint,events_mean_size"<<endl;
        map<string,double> c = current_solution.geteventmeansize(*this);
        for(map<string,double>::const_iterator mapit = c.begin();mapit!=c.end();++mapit)
        {
            const string& cname = mapit->first;
            const double& mean = mapit->second;
            outputFile<<"Current"<<","<<cname<<","<<mean<<endl;
        }
        if (number_of_moves>0)
        {
            map<string,double> n = new_solution.geteventmeansize(*this);
            for(map<string,double>::const_iterator mapit = n.begin();mapit!=n.end();++mapit)
            {
                const string& cname = mapit->first;
                const double& mean = mapit->second;
                outputFile<<"New"<<","<<cname<<","<<mean<<endl;
            }
        }
        outputFile.close();
    }

    vector<FMTspatialaction> FMTsamodel::getspatialactions()const
    {
        return spactions;
    }

    bool FMTsamodel::setinitial_mapping(const FMTforest& forest)
    {
        current_solution = FMTsasolution(forest);
		return true;
    }

    void FMTsamodel::acceptnew()
    {
        current_solution = new_solution;
        new_solution = FMTsasolution();
    }

    bool FMTsamodel::testprobability(const double temp,const double cur_obj, const double new_obj) //Metropolis criterion
    {
        uniform_real_distribution<double> r_dist(0.0,1.0);
        const double r = r_dist(generator);
        const double p = exp(((-1*cur_obj)-new_obj)/temp);
        if (p > r)
        {
            return true;
        }
        return false;
    }

    bool FMTsamodel::evaluate(const double temp,bool keep_track)
    //Get the output, evaluate the penalties and compare solutions
    {
        /*//Time checking
        clock_t start, end;
        double cpu_time_used;
        start = clock();
        //*/
        if (!comparesolutions())// if compare solution return false ... which means they are different
        {
            double cur_obj = 0;
            if ( number_of_moves == 1)//Evaluate initial solution and write results
            {
                cur_obj = current_solution.evaluate(*this);
                if (keep_track){constraints_values_penalties.push_back(current_solution.constraint_outputs_penalties);}
            }
            else
            {
                cur_obj = current_solution.getobjfvalue();
            }
            //Evaluate new solution
            const double new_obj = new_solution.evaluate(*this);
            if (keep_track){constraints_values_penalties.push_back(new_solution.constraint_outputs_penalties);}
            if (new_obj<cur_obj)
            {
                if (keep_track)
                {
                    bests_solutions.push_back(number_of_moves);//To keep track of best moves
                    accepted_solutions.push_back(number_of_moves);
                }
                return true;
            }
            else
            {
                if (testprobability(temp,cur_obj,new_obj))
                {
                    if (keep_track)
                    {
                        int last_best = bests_solutions.back();
                        bests_solutions.push_back(last_best);//Keep the last best move
                        accepted_solutions.push_back(number_of_moves);
                    }
                    return true;
                }
                if (!bests_solutions.empty())
                {
                    if(keep_track)
                    {
                        int last_best = bests_solutions.back();
                        bests_solutions.push_back(last_best);//Keep the last best move
                        int last_accepted = accepted_solutions.back();
                        accepted_solutions.push_back(last_accepted);
                    }
                    return false;
                }
                else//Case the initial solution is better
                {
                    if (keep_track)
                    {
                        bests_solutions.push_back(0);
                        accepted_solutions.push_back(0);
                    }
                    return false;
                }
            }
        }
        else
        {
            cout<<"No difference in move "+to_string(number_of_moves)<<endl;
            return false;
        }
        /*//Time checking
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        cout<<"Time used to evaluate: " <<cpu_time_used<<endl;
        //*/

    }

    FMTgraphstats FMTsamodel::buildperiod()
    {
        return current_solution.buildperiod(*this,generator);
    }

    FMTgraphstats FMTsamodel::move_solution(FMTsamovetype movetype)
    {
        number_of_moves ++;
        new_solution = current_solution.perturb(*this,generator,movetype);
        return new_solution.getsolution_stats();
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
