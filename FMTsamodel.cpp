#include "FMTsamodel.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <time.h>

namespace Models

{
    FMTsamodel::FMTsamodel():
        FMTmodel(),
        write_outputs(false),
        outputs_write_location(),
        number_of_moves(0),
        outputs_stream(),
        generator(),
        spactions(),
        bests_solutions(),
        mapidmodified(),
        cooling_schedule(),
        current_solution(),
        new_solution()
    {

    }

    FMTsamodel::FMTsamodel(const FMTsamodel& rhs):
        FMTmodel(rhs),
        write_outputs(rhs.write_outputs),
        outputs_write_location(rhs.outputs_write_location),
        number_of_moves(0),
        outputs_stream(),
        generator(rhs.generator),
        spactions(rhs.spactions),
        bests_solutions(rhs.bests_solutions),
        mapidmodified(),
        cooling_schedule(rhs.cooling_schedule->Clone()),
        current_solution(rhs.current_solution),
        new_solution(rhs.new_solution)
    {
        outputs_stream << rhs.outputs_stream.rdbuf();
    }

    FMTsamodel::FMTsamodel(const FMTmodel& rhs):
        FMTmodel(rhs),
        write_outputs(false),
        outputs_write_location(),
        number_of_moves(0),
        outputs_stream(),
        generator(),
        spactions(),
        bests_solutions(),
        mapidmodified(),
        cooling_schedule(),
        current_solution(),
        new_solution()
    {

    }

    FMTsamodel& FMTsamodel::operator = (const FMTsamodel& rhs)
    {
        if (this!=&rhs)
            {
            FMTmodel::operator = (rhs);
            write_outputs = rhs.write_outputs;
            outputs_write_location = rhs.outputs_write_location;
            number_of_moves = rhs.number_of_moves;
            outputs_stream << rhs.outputs_stream.rdbuf();
            generator = rhs.generator;
            spactions = rhs.spactions;
            bests_solutions = rhs.bests_solutions;
            mapidmodified = rhs.mapidmodified;
            cooling_schedule = rhs.cooling_schedule->Clone();
            current_solution = rhs.current_solution;
            new_solution = rhs.new_solution;
            }
        return *this;
    }

     void FMTsamodel::write_outputs_at(string path)
    {
        outputs_write_location = path;
        write_outputs = true;
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
			}else{
				_exhandler->raise(FMTexc::WSinvalid_transition,FMTwssect::Transition, "Missing transition case for action : " + spaction.name,__LINE__, __FILE__);
				return false;
				}
			}
        spactions = lspactions;
		actions = newbaseactions;
		transitions = newtransitions;
		return true;
    }

    void FMTsamodel::get_outputs()
    {
        if (!write_outputs)
        {
            _exhandler->raise(FMTexc::FMTinvalid_path,FMTwssect::Empty, "No path given to the function write_outputs_at ",__LINE__, __FILE__);
        }
        else
        {
            fstream outputFile;
            string filename = outputs_write_location+"outputs.csv";
            string headers = "Move,Constraint,Period,Output,Penalty,Best solution";
            //cout<<"Creating file"<<endl;
            outputFile.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
            outputFile<<headers<<endl;
            outputFile<<outputs_stream.rdbuf()<<endl;
            outputs_stream.str("");
        }

    }

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
            if (bests_solutions.size()==number_of_moves)
            {
                int best = bests_solutions.at(number_of_moves-1);
                if (best<number_of_moves)
                {
                    current_solution.write_events(actions,out_path,"Current_best_solution");
                    new_solution.write_events(actions,out_path,"New_less_good_solution");
                }
                else
                {
                    current_solution.write_events(actions,out_path,"Current_less_good_solution");
                    new_solution.write_events(actions,out_path,"New_best_solution");
                }
            }
            else
            {
                current_solution.write_events(actions,out_path,"Current_solution");
                new_solution.write_events(actions,out_path,"New_solution");
            }
        }
        else
        {
            current_solution.write_events(actions,out_path,"Initial_solution");
        }
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

    void FMTsamodel::testprobability(double temp)
    {
        //implement test if true current = new else reject new
    }

     void FMTsamodel::write_outputs_stream(const string& constraint_string, const vector<double>& outputs,
                                            const vector<double>& penalties,bool initial_solution)
    {
        int period = 1;//Hardcoded only if output are for every period
        for (int i = 0 ; i < outputs.size() ; ++i )
        {
            if (!initial_solution)
            {
                outputs_stream<<number_of_moves<<","<<constraint_string<<","<<period<<","<<outputs.at(i)<<","<<penalties.at(i)<<","<<bests_solutions.at(number_of_moves-1)<<"\n";
                period ++;
            }
            else
            {
                outputs_stream<<0<<","<<constraint_string<<","<<period<<","<<outputs.at(i)<<","<<penalties.at(i)<<","<<0<<"\n";
                period ++;
            }
        }
    }

    /*void FMTsamodel::write_output_to_file(const string& constraint_string, const vector<double>& outputs,
                                          const vector<double>& penalties,bool initial_solution) const
    {
        fstream outputFile;
        string filename = outputs_write_location+"outputs.csv";
        string headers = "Move,Constraint,Period,Output,Penalty,Best solution";
        struct stat buf;
        if (stat(filename.c_str(), &buf) == -1)//Check if file exist
        {
            //cout<<"Creating file"<<endl;
            outputFile.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
            outputFile<<headers<<endl;
        }
        else
        {
            outputFile.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
        }
        int period = 1;//Hardcoded only if output are for every period
        for (int i = 0 ; i < outputs.size() ; ++i )
        {
            if (!initial_solution)
            {
                outputFile<<number_of_moves<<","<<constraint_string<<","<<period<<","<<outputs.at(i)<<","<<penalties.at(i)<<","<<bests_solutions.at(number_of_moves-1)<<endl;
                period ++;
            }
            else
            {
                outputFile<<0<<","<<constraint_string<<","<<period<<","<<outputs.at(i)<<","<<penalties.at(i)<<","<<0<<endl;
                period ++;
            }
        }
        outputFile.close();
    }*/


    bool FMTsamodel::evaluate()

    //Get the output, evaluate the penalties and compare solutions
    {
        //Time checking
        clock_t start, end;
        double cpu_time_used;
        start = clock();
        //
        bool evaluation;
        if (!comparesolutions())// if compare solution return false ... which means they are different
        {
            if ( number_of_moves == 1)//Evaluate initial solution and write results
            {
                std::unordered_map<string,vector<vector<double>>> constraints_info = current_solution.evaluate(*this);
                if (write_outputs)
                {
                    std::unordered_map<string,vector<vector<double>>>::const_iterator it = constraints_info.begin();
                    while(it != constraints_info.end())
                    {
                        write_outputs_stream(it->first,it->second.at(0),it->second.at(1),true);
                        it++;
                    }
                }
            }
            //Evaluate new solution
            std::unordered_map<string,vector<vector<double>>> newconstraints_info = new_solution.evaluate(*this);
            if (new_solution.getobjfvalue()<current_solution.getobjfvalue())
            {
                bests_solutions.push_back(number_of_moves);//To keep track of best moves
                evaluation = true;
            }
            else
            {
                if (!bests_solutions.empty())
                {
                    int last_best = bests_solutions.back();
                    bests_solutions.push_back(last_best);//Keep the last best move
                }
                else//Case the initial solution is better
                {
                    bests_solutions.push_back(0);
                }
                evaluation = false;
            }
            //Write new solution outputs and penalties
            if (write_outputs)
            {
                std::unordered_map<string,vector<vector<double>>>::const_iterator it = newconstraints_info.begin();
                while(it != newconstraints_info.end())
                {
                    write_outputs_stream(it->first,it->second.at(0),it->second.at(1),false);
                    it++;
                }
            }
        }
        else
        {
            if (!bests_solutions.empty())
                {
                    int last_best = bests_solutions.back();
                    bests_solutions.push_back(last_best);//Keep the last best move
                }
            else//Case the initial solution is better
                {
                    bests_solutions.push_back(0);
                }
            evaluation =  false;
        }
        //Time checking
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        cout<<"Time used : " <<cpu_time_used<<endl;
        //
        return evaluation;
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
