/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsamodel.hpp"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <boost/filesystem.hpp>
#include <cstring>
#include "FMTforest.hpp"
#include "FMTexceptionhandler.hpp"
#include "FMTsaschedule.hpp"

namespace Models

{

    std::string FMTsamodel::getcoolingscheduletype() const 
        { 
        return cooling_schedule->get_schedule_type(); 
        }

    double  FMTsamodel::cool_down(double temp)const
        {
        return cooling_schedule->reduce_temp(temp);
        }

    /*
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
				 Spatial::FMTlayer<Graph::FMTlinegraph>::const_iterator current_solutionit =  current_solution.begin();
                 std::advance(current_solutionit,*it);
                 if(current_solutionit->second != new_solution.at(current_solutionit->first))
                 {
                     return false;
                 }
            }
            return true;
        }
        return false;
    }*/

    FMTsamodel::~FMTsamodel() = default;

    FMTsamodel::FMTsamodel():
        FMTsemodel(),
       // movetype(Spatial::FMTsamovetype::opt1),
        min_ratio_moves(0),
        max_ratio_moves(1),
        outputs_write_location(),
        number_of_moves(0),
        constraints_values_penalties(),
        generator(),
        //spactions(),
        accepted_solutions(),
        mapidmodified(),
        probabs(),
        cooling_schedule()//,
        //best_solution(),
        //current_solution(),
       // new_solution()
    {

    }

    FMTsamodel::FMTsamodel(const FMTsamodel& rhs):
        FMTsemodel(rhs),
        //movetype(rhs.movetype),
        min_ratio_moves(rhs.min_ratio_moves),
        max_ratio_moves(rhs.max_ratio_moves),
        outputs_write_location(rhs.outputs_write_location),
        number_of_moves(0),
        constraints_values_penalties(rhs.constraints_values_penalties),
        generator(rhs.generator),
       // spactions(rhs.spactions),
        accepted_solutions(rhs.accepted_solutions),
        mapidmodified(),
        probabs(rhs.probabs),
        cooling_schedule(rhs.cooling_schedule->Clone())//,
        //best_solution(rhs.best_solution),
        //current_solution(rhs.current_solution),
       // new_solution(rhs.new_solution)
    {

    }

    FMTsamodel::FMTsamodel(const FMTmodel& rhs):
        FMTsemodel(rhs),
        //movetype(Spatial::FMTsamovetype::opt1),
        min_ratio_moves(0),
        max_ratio_moves(1),
        outputs_write_location(),
        number_of_moves(0),
        constraints_values_penalties(),
        generator(),
        //spactions(),
        accepted_solutions(),
        mapidmodified(),
        probabs(),
        cooling_schedule(std::unique_ptr<Spatial::FMTexponentialschedule>(new Spatial::FMTexponentialschedule()))//,
        //best_solution(),
        //current_solution(),
       // new_solution()
    {

    }

    FMTsamodel::FMTsamodel(const FMTmodel& rhs, const Spatial::FMTforest& forest):
        FMTsemodel(rhs,forest),
        //movetype(Spatial::FMTsamovetype::opt1),
        min_ratio_moves(0),
        max_ratio_moves(1),
        outputs_write_location(),
        number_of_moves(0),
        constraints_values_penalties(),
        generator(),
        //spactions(),
        accepted_solutions(),
        mapidmodified(),
        probabs(),
        cooling_schedule(std::unique_ptr<Spatial::FMTexponentialschedule>(new Spatial::FMTexponentialschedule()))//,
    {

    }

    FMTsamodel::FMTsamodel(const FMTsemodel& rhs):
        FMTsemodel(rhs),
        //movetype(Spatial::FMTsamovetype::opt1),
        min_ratio_moves(0),
        max_ratio_moves(1),
        outputs_write_location(),
        number_of_moves(0),
        constraints_values_penalties(),
        generator(),
        //spactions(),
        accepted_solutions(),
        mapidmodified(),
        probabs(),
        cooling_schedule(std::unique_ptr<Spatial::FMTexponentialschedule>(new Spatial::FMTexponentialschedule()))//,
    {

    }


    FMTsamodel& FMTsamodel::operator = (const FMTsamodel& rhs)
    {
        if (this!=&rhs)
            {
            FMTsemodel::operator = (rhs);
			solution = rhs.solution;
           // movetype = rhs.movetype;
            min_ratio_moves = rhs.min_ratio_moves;
            max_ratio_moves = rhs.max_ratio_moves;
            outputs_write_location = rhs.outputs_write_location;
            number_of_moves = rhs.number_of_moves;
            constraints_values_penalties = rhs.constraints_values_penalties;
            generator = rhs.generator;
            //spactions = rhs.spactions;
            accepted_solutions = rhs.accepted_solutions;
            mapidmodified = rhs.mapidmodified;
            probabs = rhs.probabs;
            cooling_schedule = rhs.cooling_schedule->Clone();
            //best_solution=rhs.best_solution;
            //current_solution = rhs.current_solution;
            //new_solution = rhs.new_solution;
            }
        return *this;
    }

    void FMTsamodel::swap_ptr(const std::unique_ptr<FMTmodel>& rhs)
    {
        *this = std::move(*dynamic_cast<FMTsamodel*>(rhs.get()));
    }

	std::unique_ptr<FMTmodel>FMTsamodel::clone() const
		{
		return std::unique_ptr<FMTmodel>(new FMTsamodel(*this));
		}

    /*double FMTsamodel::warmup(const double initprob, const size_t iterations, bool keep_best, FMTsawarmuptype type)
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
    }*/

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

    /*bool FMTsamodel::setspactions(const std::vector<Spatial::FMTspatialaction>& lspactions)
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
				_exhandler->raise(Exception::FMTexc::FMTinvalid_transition,
					"Missing transition case for action : " + spaction.getname(),
					"FMTsamodel::setspactions",__LINE__, __FILE__, Core::FMTsection::Transition);
				return false;
				}
			}
        spactions = lspactions;
		actions = newbaseactions;
		transitions = newtransitions;
		return true;
    }*/

    bool FMTsamodel::set_min_max_moves(const double min_r,const double max_r)
    {
        min_ratio_moves = min_r;
        max_ratio_moves = max_r;
        return true;
    }
    /*
    bool FMTsamodel::set_movetype(const Spatial::FMTsamovetype movet)
    {
        movetype = movet;
        if (movetype == movet)
        {
             return true;
        }
        return false;
    }
    */
    int FMTsamodel::get_number_moves()const
    {
        return number_of_moves;
    }

    void FMTsamodel::get_outputs(std::string addon)
    {
        if (outputs_write_location.empty())
        {
            _exhandler->raise(Exception::FMTexc::FMTinvalid_path,
				"No path given to the function write_outputs_at ","FMTsamodel::get_outputs",__LINE__, __FILE__);
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
    /*
    Spatial::FMTsasolution FMTsamodel::get_current_solution()const
    {
            return current_solution;
    }

	Spatial::FMTsasolution FMTsamodel::get_new_solution()const
    {
            return new_solution;
    }
    */
    /*void FMTsamodel::write_solutions_events(std::string out_path)const
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
    }*/
/*
   std::vector<Spatial::FMTspatialaction> FMTsamodel::getspatialactions()const
    {
        return spactions;
    }
   */

    /*void FMTsamodel::acceptnew()
    {
		current_solution.copyfromselected(new_solution, mapidmodified);
		new_solution = Spatial::FMTsasolution();
    }*/

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

    /*bool FMTsamodel::evaluate(const double temp, bool all_data)
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
    }*/

    Graph::FMTgraphstats FMTsamodel::buildperiod()
    {
        //return current_solution.buildperiod(*this,generator);
		return solution.randombuild(*this,generator);
    }
    /*
    Graph::FMTgraphstats FMTsamodel::move_solution()
    {
        return g_move_solution(min_ratio_moves,max_ratio_moves);
    }*/

    bool FMTsamodel::setmapidmodified(const std::vector<size_t>& id)
    {
        mapidmodified = id;
        return true;
    }


	bool FMTsamodel::evaluate(const double& temp, const Spatial::FMTspatialschedule& actual,const Spatial::FMTspatialschedule& candidat) const
		{
		try {
			const double actualglobalobjective = actual.getglobalobjective(*this);
			const double candidatglobalobjective = candidat.getglobalobjective(*this);
			//*_logger << "actual " << actualglobalobjective << " new " << candidatglobalobjective << "\n";
			double probability = 1;
			if (candidatglobalobjective>actualglobalobjective)
				{
				probability =  std::exp((actualglobalobjective - candidatglobalobjective) / temp);
				}
			//*_logger << "probability "<< probability << "\n";
			std::uniform_real_distribution<double>random_distribution(0.0,1.0);
			const double random_probability = random_distribution(generator);
			return  (probability > random_probability);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsamodel::evaluate", __LINE__, __FILE__);
		}
		return false;
		}

	Spatial::FMTspatialschedule FMTsamodel::move(const Spatial::FMTspatialschedule& actual,
										const Spatial::FMTspatialschedule::actionbindings& bindings,
										const std::vector<Spatial::FMTcoordinate>*movable,
										boost::unordered_map<Core::FMTdevelopment, bool>*operability) const
		{
		//Spatial::FMTspatialschedule newsolution(actual);
		try {
			const size_t movamaximalsize = std::max(size_t(1),solution.size()/10);//10 % of the map
			std::uniform_int_distribution<int> mosizedist(1, static_cast<int>(movamaximalsize));
			const size_t movesize = mosizedist(generator);
			std::uniform_int_distribution<int> perioddistribution(1, actual.actperiod() - 1);//period to change
			std::vector<Spatial::FMTcoordinate> selectionpool;
			int period = 0;
			while (selectionpool.empty())
				{
				period = perioddistribution(generator);
				selectionpool = actual.getmovablecoordinates(*this, period, movable,operability);
				}
			if (selectionpool.empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Empty solution ", "FMTsamodel::move()", __LINE__, __FILE__);
				}
			std::shuffle(selectionpool.begin(), selectionpool.end(), generator);
			std::vector<Spatial::FMTcoordinate>::const_iterator luckycoordinateit = selectionpool.begin();
			size_t perturbationdone = 0;
			const size_t selected = std::min(movesize, selectionpool.size());
			Spatial::FMTspatialschedule newsolution(actual, selectionpool.begin(), selectionpool.begin() + selected);
			//Spatial::FMTspatialschedule newsolution(actual);
			while (luckycoordinateit !=selectionpool.end()&& perturbationdone<movesize)
				{
				newsolution.perturbgraph(*luckycoordinateit, period, *this, generator, bindings);
				++perturbationdone;
				}
			return newsolution;
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsamodel::move", __LINE__, __FILE__);
			}
		//return newsolution;
		return actual;
		}

    void FMTsamodel::initialgrow()
    {
        try {
            int modellength = getparameter(Models::FMTintmodelparameters::LENGTH);
            while (modellength>0)
                {
                solution.grow();
                --modellength;
                }
        }catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::initialgrow", __LINE__, __FILE__);
        }
    }

    void FMTsamodel::randombuild()
    {
        try {
            int modellength = getparameter(Models::FMTintmodelparameters::LENGTH);
            while (modellength > 0)
            {
                solution.randombuild(*this, generator);
                --modellength;
            }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::randombuild", __LINE__, __FILE__);
        }
    }

    void FMTsamodel::schedulesbuild(const std::vector<Core::FMTschedule>& schedules)
    {
        try {
            const size_t alliterations = static_cast<size_t>(getparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS));
            for (const Core::FMTschedule& schedule : schedules)
                {
                solution.greedyreferencebuild(schedule,*this, alliterations);
                }
        }
        catch (...)
        {
            _exhandler->raisefromcatch("", "FMTsamodel::schedulesbuild", __LINE__, __FILE__);
        }

    }



	double FMTsamodel::warmup(const Spatial::FMTspatialschedule& actual,
		const Spatial::FMTspatialschedule::actionbindings& bindings,
		const std::vector<Spatial::FMTcoordinate>*movable,
		boost::unordered_map<Core::FMTdevelopment, bool>*operability,
		double initprobability, size_t iterations)
		{
		double temperature = 0;
		try {
			//const double actualobjective = actual.getglobalobjective(*this);
			const std::vector<double>actuals = actual.getconstraintsvalues(*this);
			std::vector<double>maximals = actuals;
			std::vector<double>deltasums(constraints.size(),0);
			const double normalizationfactor = 2;
			const double totalits = static_cast<double>(iterations);
			//double deltasum = 0;
			while (iterations>0)
				{
				const Spatial::FMTspatialschedule newsolution = move(actual,bindings, movable,operability);
				size_t cntid = 0;
				for (const double& value : newsolution.getconstraintsvalues(*this))
					{
					//*_logger << "Value " << value << "\n";
					if (value!=0 && (maximals.at(cntid) > 0 && maximals.at(cntid)<value ||
									maximals.at(cntid) < 0 && maximals.at(cntid)>value))
						{
						maximals[cntid] = value;
						}
					deltasums[cntid]+=(std::abs(actuals.at(cntid) - value));
					++cntid;
					}
				//const double newobjective = newsolution.getglobalobjective(*this);
				//*_logger << "old " << actualobjective << " new " << newobjective << "\n";
				//deltasum += std::abs(actualobjective - newobjective);
				--iterations;
				}
			double deltasum = 0;
			size_t cntid = 0;
			for (double& value : maximals)//Need to normalize the calculated delta
				{
				if (value==0)
					{
					value = 1;
					}
				value = (1000 / (std::abs(value)*normalizationfactor));
				deltasum += (deltasums[cntid] * value);
				++cntid;
				}
			temperature = -(deltasum / totalits) / std::log(initprobability);
			solution.setconstraintsfactor(*this, maximals);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsamodel::warmup", __LINE__, __FILE__);
			}
		return temperature;
		}

	bool FMTsamodel::initialsolve()
		{
		try {
			double primalinf = 0;
			double objective = 0;
			const std::vector<Spatial::FMTcoordinate>movables = solution.getstaticsmovablecoordinates(*this);
			const Spatial::FMTspatialschedule::actionbindings actionsbinding = solution.getbindingactionsbyperiod(*this);
			boost::unordered_map<Core::FMTdevelopment, bool>operability;
			double temperature = warmup(solution, actionsbinding,&movables,&operability);
			_logger->logwithlevel("Annealer Temp("+std::to_string(temperature)+")\n", 1);
            const size_t alliterations = static_cast<size_t>(getparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS));
			//solution.getsolutionstatus(objective, primalinf, *this);
			//solution.logsolutionstatus(0, objective, primalinf);
			size_t notaccepted = 0;
			size_t temperaturelevel = 0;
			size_t totaliteration = 0;
			while (notaccepted<100 && totaliteration<alliterations)
				{
				size_t iteration = 0;
				size_t accepted = 0;
				while (notaccepted < 100 && iteration <1000)
					{
					const Spatial::FMTspatialschedule newsolution = move(solution, actionsbinding,&movables,&operability);
					if (evaluate(temperature,solution,newsolution))
						{
						if (newsolution.ispartial())
							{
							solution.copyfrompartial(newsolution);
						}else {
							solution = newsolution;
							}
						++accepted;
						notaccepted = 0;
					}else {
						++notaccepted;
						}
					if (totaliteration % 100 == 0)
						{
						double primalinf = 0;
						double objective = 0;
						solution.getsolutionstatus(objective, primalinf,*this);
						solution.logsolutionstatus(totaliteration, objective,primalinf);
						solution.dorefactortorization(*this);
						}
					++iteration;
					++totaliteration;
					}
				++temperaturelevel;
				const double acceptanceratio = (static_cast<double>(accepted) / static_cast<double>(iteration)) * 100;
				_logger->logwithlevel("Annealer Temp(" + std::to_string(temperature) + ") Ratio("+std::to_string(acceptanceratio)+")\n", 1);
				temperature = cool_down(temperature);
				}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsamodel::initialsolve", __LINE__, __FILE__);
			}
        return true;
		}

    bool FMTsamodel::build(std::vector<Core::FMTschedule> schedules)
    {
        try {
            if (schedules.empty()) //From no solution
            {
                const bool forcepartialbuild = parameters.getboolparameter(FORCE_PARTIAL_BUILD);
                if (forcepartialbuild)
                {
                    initialgrow();
                }else {
                    randombuild();
                }
            }else {
                schedulesbuild(schedules);
                }
        }catch (...)
        {
            _exhandler->printexceptions("", "FMTsamodel::build", __LINE__, __FILE__);
        }
        return true;
    }

 
    bool FMTsamodel::solve()
    {
        try {
            return initialsolve();
        }catch (...)
        {
            _exhandler->printexceptions("", "FMTsamodel::solve", __LINE__, __FILE__);
        }
        return false;
    }

    std::unique_ptr<FMTmodel>FMTsamodel::presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
    {
        try {
            return std::unique_ptr<FMTmodel>(new FMTsamodel(*(dynamic_cast<FMTsemodel*>(FMTsemodel::presolve(optionaldevelopments).get()))));
        }
        catch (...)
        {
            _exhandler->printexceptions("", "FMTsamodel::presolve", __LINE__, __FILE__);
        }
        return std::unique_ptr<FMTmodel>(nullptr);
    }



}

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsamodel)