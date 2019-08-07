#include "FMTareaparser.h"
#include "FMTsasolution.h"
#include "FMTsamodel.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <math.h>
#include <cmath>
#include <cfloat>

namespace Spatial
{

    FMTsasolution::FMTsasolution():FMTlayer<FMTgraph>(),solution_stats(),events(),objectivefunctionvalue(),events_meansize(),constraint_outputs_penalties()
    {

    }

    FMTsasolution::FMTsasolution(const FMTforest& initialmap):events(),objectivefunctionvalue(),events_meansize(),constraint_outputs_penalties()
    {
        FMTlayer<FMTgraph>::operator = (initialmap.copyextent<FMTgraph>());//Setting layer information
        for(map<FMTcoordinate,FMTdevelopment>::const_iterator devit = initialmap.mapping.begin(); devit != initialmap.mapping.end(); ++devit)
        {
            vector<FMTactualdevelopment> actdevelopment;
            actdevelopment.push_back(FMTactualdevelopment (devit->second,initialmap.getcellsize()));
            FMTgraph local_graph(FMTgraphbuild::schedulebuild);
            std::queue<FMTvertex_descriptor> actives = local_graph.initialize(actdevelopment);
            mapping[devit->first] = local_graph;
            solution_stats += local_graph.getstats();
        }
    }

    FMTsasolution::FMTsasolution(const FMTsasolution& rhs):
            FMTlayer<FMTgraph>(rhs),
            solution_stats(rhs.solution_stats),
            events(rhs.events),
            objectivefunctionvalue(rhs.objectivefunctionvalue),
            events_meansize(rhs.events_meansize),
            constraint_outputs_penalties(rhs.constraint_outputs_penalties)
    {

    }

    FMTsasolution& FMTsasolution::operator = (const FMTsasolution& rhs)
    {
    if (this!=&rhs)
        {
        FMTlayer<FMTgraph>::operator = (rhs);
        solution_stats = rhs.solution_stats;
        events = rhs.events;
        objectivefunctionvalue = rhs.objectivefunctionvalue;
        events_meansize = rhs.events_meansize;
        constraint_outputs_penalties = rhs.constraint_outputs_penalties;
        }
    return *this;
    }

    bool FMTsasolution::operator == (const FMTsasolution& rhs) const
    {
    for (map<FMTcoordinate,FMTgraph>::const_iterator mainsolutionit = this->mapping.begin();
                                                     mainsolutionit != this->mapping.end(); ++mainsolutionit)
        {
            if (mainsolutionit->second != rhs.mapping.at(mainsolutionit->first))
            {
                return false;
            }
        }
    return true;
    }

    bool FMTsasolution::operator != (const FMTsasolution& rhs) const
    {
        return (!(*this==rhs));
    }

// ++++++++++++++++++++++++++++++ Function to get info on the solution +++++++++++++++++++++++++++++++++++

    map<string,double> FMTsasolution::geteventmeansize(const FMTsamodel& model)
    {
            /*size_t action_id = 0;
            vector<FMTspatialaction>spatialactions = model.getspatialactions();
            for (const FMTspatialaction& spaction : spatialactions)
                double size_sum = 0;
                size_t num_event = 0;
                for (const vector<vector<FMTevent<FMTgraph>>>& period_actions : events)
                {
                    const vector<FMTevent<FMTgraph>>& action_events = period_actions.at(action_id);
                    for(const FMTevent<FMTgraph>& event : action_events)
                    {
                        if (!event.empty())
                        {
                            size_sum += static_cast<double>(event.elements.size());
                            ++num_event;
                        }
                    }
                double mean = size_sum/num_event;
                events_meansize[spaction.name]=mean;
                ++action_id;
                }
        return events_meansize;*/
    }

    double FMTsasolution::getobjfvalue()const
    {
        return objectivefunctionvalue;
    }

    FMTgraphstats FMTsasolution::getsolution_stats() const
    {
        return solution_stats;
    }
    const vector<vector<vector<FMTevent<FMTgraph>>>>& FMTsasolution::getevents() const
    {
        return events;
    }

    void FMTsasolution::getstartstop(const FMTconstraint& constraint,int& periodstart,int& periodstop) const
    {
        map<FMTcoordinate,FMTgraph>::const_iterator graphit = this->mapping.begin();
        const FMTgraph* local_graph = &graphit->second;
        local_graph->constraintlenght(constraint,periodstart,periodstop);
    }

    vector<double> FMTsasolution::getgraphsoutputs(const FMTmodel& model, const FMTconstraint& constraint,
                                           const int& periodstart,const int& periodstop) const
    // Return sum of all graphs outputs related to constraint
    {
        vector<double> periods_values(periodstop-periodstart+1);
        const vector<double> solutions(1,this->getcellsize());
        for(map<FMTcoordinate,FMTgraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
            {
                const FMTgraph* local_graph = &graphit->second;
                for (int period = periodstart ; period<=periodstop ; ++period)
                    {
                        map<string, double> output = local_graph->getoutput(model,constraint,period,&solutions[0],FMToutputlevel::totalonly);
                        double totalperiod = output.at("Total");
                        periods_values[period-1]+=totalperiod;
                        //cout<<"Period "<<period<<"\nOutput "<<totalperiod<<endl;
                        //cout<<"Vector value "<<periods_values.at(period-1)<<endl;
                    }
            }
        return periods_values;
    }

    double FMTsasolution::getgraphspenalties(const FMTsamodel& model, const FMTconstraint& constraint,
                                         const double& coef, vector<double>& output_vals, vector<double>& penalties_vals)

    //Find output by period and evaluate penalties for each. Finally sum penalties
    {
        int periodstart=0;
        int periodstop=0;
        getstartstop(constraint,periodstart,periodstop);
        double sumpenalties=0;//Sum of penalties for each period
        output_vals = getgraphsoutputs(model,constraint,periodstart,periodstop);
        int period = periodstart;
        for  (const double& value : output_vals)
        {
            double lower = 0;
            double upper = 0;
            constraint.getbounds(lower,upper,period);
            double penalties = this->applypenalty(upper,lower,value,coef,FMTsapenaltytype::exponential);
            //Debug
            /*cout<<constraint.name<<" Upper : "<<upper<<" Lower : "<<lower<<endl;
            cout<<value<<" "<<penalties<<endl;
            cin.get();*/
            penalties_vals.push_back(penalties);
            sumpenalties+=penalties;
            period++;
        }
        return sumpenalties;
    }

    double FMTsasolution::getspatialpenalties(const FMTsamodel& model, const FMTconstraint& constraint,
                                            const double& coef, vector<double>& output_vals, vector<double>& penalties_vals) const //Use of spatial actions as objectives
    // See to add greenup eventualy and set by bounds and period the shit
    {
        vector<FMTspatialaction> spatialactions = model.getspatialactions();
        double spatialpenalties = 0;
        double lower=0;
        double upper=0;
        vector<FMTspatialaction>::iterator actionit = find_if(spatialactions.begin(),spatialactions.end(),[constraint] (const FMTspatialaction& spaction) {return spaction.name == constraint.name;});
        int action_id = std::distance(spatialactions.begin(), actionit);
        FMTspatialaction spaction = spatialactions.at(action_id);
        size_t period = 1;
        for (const vector<vector<FMTevent<FMTgraph>>>& period_actions : events)
        {
            if (!period_actions.empty())
            {
                constraint.getbounds(lower,upper,period);
                vector<FMTevent<FMTgraph>> action_events = period_actions.at(action_id);
                if(!action_events.empty())
                {
                    if (constraint.getconstrainttype()==FMTconstrainttype::FMTspatialadjacency)
                    {
                        vector<FMTevent<FMTgraph>> potential_neigbors;
                        //Getting events that can be considerate as neighbors
                        for (vector<vector<FMTevent<FMTgraph>>>::const_iterator action_events_it = period_actions.begin();action_events_it != period_actions.end();++action_events_it) //To navigate through neighbors
                        {
                            for (vector<string>::const_iterator neighbors_it = spaction.neighbors.begin();neighbors_it!=spaction.neighbors.end();++neighbors_it)
                            {
                                if (spatialactions.at(action_events_it-period_actions.begin()).name == *neighbors_it)//Where iterator is - begin == index so we compare the name of the action and neighbors in neighbors list
                                {
                                    const vector<FMTevent<FMTgraph>> value = *action_events_it;
                                    potential_neigbors.insert(potential_neigbors.end(),value.begin(),value.end());//Insert FMTevent<FMTgraph> of each action in neighbors
                                }
                            }
                        }
                        double output_val = 0;
                        double penalties_val = 0;
                        for (const FMTevent<FMTgraph>& event : action_events)
                        {
                            if (!event.empty())
                            {
                                double event_val = event.minimaldistance(potential_neigbors,lower);
                                output_val+=event_val;
                                //Fix FMTconstraint and change upper lower for constraint
                                penalties_val += applypenalty(numeric_limits<double>::infinity(),lower,event_val,coef,FMTsapenaltytype::linear);
                            }
                        }
                        output_vals.push_back(output_val);
                        penalties_vals.push_back(penalties_val);
                        spatialpenalties+=penalties_val;
                    }
                    if (constraint.getconstrainttype()==FMTconstrainttype::FMTspatialsize)
                    {
                        const double maxsize = static_cast<double>(upper);
                        const double minsize = static_cast<double>(lower);
                        double output_val = 0;
                        double penalties_val = 0;
                        for (const FMTevent<FMTgraph>& event : action_events)
                        {
                            if (!event.empty())
                            {
                                double event_val = static_cast<double>(event.elements.size());
                                output_val+=event_val;
                                //Fix FMTconstraint and change upper lower for constraint and coef
                                penalties_val += applypenalty(maxsize,minsize,event_val,coef,FMTsapenaltytype::linear);
                            }
                        }
                        output_vals.push_back(output_val);
                        penalties_vals.push_back(penalties_val);
                        spatialpenalties+=penalties_val;
                    }
                }
            }
            period++;
        }
        return spatialpenalties;
    }

    FMTforest FMTsasolution::getforestperiod(const int& period) const
    {
        FMTforest forest(this->copyextent<FMTdevelopment>());//Setting layer information
        for(map<FMTcoordinate,FMTgraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
        {
            const FMTgraph* local_graph = &graphit->second;
            const vector<double> solutions(1,this->getcellsize());
            vector<FMTactualdevelopment> actdev = local_graph->getperiodstopdev(period,&solutions[0]);//
            forest.mapping[graphit->first]=FMTdevelopment(actdev.front());
        }
        return forest;
    }

// ++++++++++++++++++++++++++++++ Function to manipulate solution +++++++++++++++++++++++++++++++++++

    FMTgraphstats FMTsasolution::buildperiod(const FMTmodel& model,default_random_engine& generator)
        {
            FMTgraphstats periodstats = FMTgraphstats();
            vector<vector<FMTevent<FMTgraph>>> events_id(model.getactions().size());
            for(map<FMTcoordinate,FMTgraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)//change const_iterator to iterator because graph is modified
            {
                FMTgraph* local_graph = &graphit->second;
                std::queue<FMTvertex_descriptor> actives = local_graph->getactiveverticies();
                FMTgraphstats stats = local_graph->randombuild(model,actives,generator,events_id,graphit->first);
                periodstats += local_graph->getstats();
            }
            events.push_back(events_id);
            solution_stats += periodstats;
            return periodstats;
        }

    FMTsasolution FMTsasolution::perturb(FMTsamodel& model, default_random_engine& generator,FMTsamovetype movetype,const double min_ratio, const double max_ratio) const
    {
        FMTsasolution newsolution(*this);
        newsolution.events_meansize.clear();
        switch(movetype)
        {
        case FMTsamovetype::shotgun ://Create function shotgun move
        {
            vector<size_t> map_lenght(this->mapping.size());
            size_t i = 0;
            for (size_t& lsize : map_lenght)
            {
                lsize = i;
                ++i;
            }
            int min_cells = max(int((map_lenght.size()-1)*min_ratio),1);
            int max_cells = max(int((map_lenght.size()-1)*max_ratio),1);
            shuffle(map_lenght.begin(),map_lenght.end(),generator);//shuffle all indices in the map
            uniform_int_distribution<int> celldistribution(min_cells,max_cells);
            int numbercells = celldistribution(generator);//Get number of cell to perturb
            vector<size_t> ChangedId;
            //cout<< "Map size "<< map_lenght.size()<<endl;
            //cout<< "Number of cells to modify "<<numbercells<<endl;
            for (int id = 0; id<numbercells; ++id)
            {
                //cout<<"+++++++++++++ NEW CELL ++++++++++++++"<<endl;
                map<FMTcoordinate,FMTgraph>::const_iterator luckygraph = this ->mapping.begin();
                std::advance(luckygraph,map_lenght.at(id));
                uniform_int_distribution<int> perioddistribution(1,luckygraph->second.size()-2);//period to change
                int period = perioddistribution(generator);

                //cout<<"Random period : "<<period<<endl;

                newsolution.solution_stats -= luckygraph->second.getstats();

                //cout<<"Iteration : "<<id<<endl;
                //cout<<"Cell : "<<map_lenght.at(id)<<endl;

                FMTgraph newgraph = luckygraph -> second.perturbgraph(model,generator,newsolution.events,luckygraph->first,period);//perturb cell

                //cout<<"Newgraph generated"<<endl;

                newsolution.solution_stats += newgraph.getstats();
                newsolution.mapping[luckygraph->first] = newgraph;
                ChangedId.push_back(map_lenght.at(id));
            }
            //cout<<"Setting mapid" <<endl;
            bool mapid = model.setmapidmodified(ChangedId);
            break;
        }
        case FMTsamovetype::cluster :
            break;//to do list before 2025

        default :
            break;
        }
        return newsolution;
    }

    double FMTsasolution::exponentialpenalty(const double& xvalue,
                        const double& xlowerbound, const double& xupperbound,
                        const double& maxpenalty,
                        double curvelength,
                        double slope) const
    {
    //https://en.wikipedia.org/wiki/Exponential_distribution
    //change slope between 1 to 2 (1  = more linear)
    //If no curvelength find one
    if (!curvelength)
        {
         if (xlowerbound != -DBL_MAX)
            {
            curvelength = abs(xlowerbound);
            }else if(xupperbound != DBL_MAX)
                {
                curvelength =  abs(xupperbound);
                }
        }
    if (curvelength != 0)
        {
        double basefive = 0;
        if (xvalue < xlowerbound)
            {
            basefive=(5.0/curvelength)*xvalue;
            if (xvalue < 0)
                {
                basefive=5-(5.0/curvelength)*(abs(xvalue)-abs(xlowerbound));
                }
            }else if(xvalue > xupperbound)
                {
                basefive = 5-((5.0/curvelength)*(xvalue-xupperbound));
                }else{
                    return 0;
                }
        return (slope*exp(-slope*(basefive)))*maxpenalty;
        //return min((slope*exp(-slope*(basefive)))*maxpenalty,maxpenalty);
        }
    return 0;
    }

    double FMTsasolution::applypenalty(const double& upper,const double& lower, const double& value, const double& coef, const FMTsapenaltytype penalty_type)const
    {
        switch(penalty_type)
        {
            case FMTsapenaltytype::linear :
                {
                    double penaltyvalue = 0;
                    //Put penalty value
                    //Linear -x+lower bound
                    //x - upper bound if not infinite
                    //if between == 0
                    if (!isinf(lower))//If lower is not infinite
                    {
                        if(value<lower)//If value under lower bound penalize
                        {
                            penaltyvalue = -value+lower;
                        }
                    }
                    if (!isinf(upper))//If upper is not infinite
                    {
                        if (value>upper)//If value higher than upper bound penalize
                        {
                            penaltyvalue = value-upper;
                        }
                    }
                    return penaltyvalue*coef;
                }

            case FMTsapenaltytype::exponential:
                {
                    double lowerl = lower;
                    double upperl = upper;
                    double maxpenalty = 0;
                    if (isinf(lower))//If lower is infinite
                    {
                        lowerl = -DBL_MAX;
                        maxpenalty = upper*coef;

                    }
                        if (isinf(upper))//If upper is infinite
                    {
                        upperl = DBL_MAX;
                        maxpenalty = lower*coef;
                    }
                    else
                    {
                        maxpenalty = (max(lower,upper))*coef;
                    }
                    return exponentialpenalty(value,lowerl,upperl,maxpenalty);
                }

            default :
                break;
        }
    return 0;
    }

    double FMTsasolution::evaluate(const FMTsamodel& model)
    {
        vector<FMTconstraint> constraints = model.getconstraints();
        FMTconstraint objective = constraints.front();
        double penalty_sense = 1;//objective.sense();
        vector<string>penalties = objective.getpenalties(penalty_sense);//Return values in penalty
        if (penalties.empty())
            {
                _exhandler->raise(FMTexc::FMTunsupported_objective,FMTwssect::Optimize,"No penalties",__LINE__, __FILE__);
            }
        else
            {
                constraints.erase(constraints.begin());
                vector<string>::const_iterator all_it = find(penalties.begin(), penalties.end(), "ALL");
                double penalty_value = 0;
                for (const FMTconstraint& constraint : constraints)
                {
                    FMTconstrainttype type = constraint.getconstrainttype();
                    double value = 0;
                    string name;
                    string cname = boost::replace_all_copy(string(constraint),"\n","");
                    boost::replace_all(cname,","," ");
                    double coef = 1;
                    constraint.getgoal(name,coef);
                    if (!name.empty())
                    {
                        vector<string>::const_iterator name_it = find(penalties.begin(), penalties.end(), name);
                        if (all_it != penalties.end() || name_it != penalties.end())
                        {
                            vector<double> output_vals;
                            vector<double> penalties_vals;
                            if ( type == FMTconstrainttype::FMTspatialadjacency  || type == FMTspatialsize || type == FMTspatialgreenup )//Case spatial are in the section optimize
                            {
                                value = this->getspatialpenalties(model,constraint,coef, output_vals, penalties_vals);
                            }
                            else
                            {
                                value = this->getgraphspenalties(model,constraint,coef, output_vals, penalties_vals);//apply weight in applypenalty
                            }
                            constraint_outputs_penalties[cname+to_string(constraint.getconstrainttype())] = pair<vector<double>,vector<double>>(output_vals,penalties_vals);
                        }
                        else//Case spatialobjectives are set with spatial action
                            {
                                vector<double> output_vals;
                                vector<double> penalties_vals;
                                if ( type == FMTconstrainttype::FMTspatialadjacency  || type == FMTspatialsize || type == FMTspatialgreenup )
                                {
                                    value = this->getspatialpenalties(model,constraint,coef, output_vals, penalties_vals);
                                }
                                constraint_outputs_penalties[cname+to_string(constraint.getconstrainttype())] = pair<vector<double>,vector<double>>(output_vals,penalties_vals);
                            }
                    }
                    penalty_value += value;
                }
                objectivefunctionvalue = penalty_value;
                return penalty_value;
            }

                /*map<string,vector<double>> action_period_penalties;
                objectivefunctionvalue = graphpenaltyvalue + this->getspatialpenalties(model,action_period_penalties);
                for (map<string,vector<double>>::const_iterator action_period_penalties_it = action_period_penalties.begin();action_period_penalties_it!=action_period_penalties.end();++action_period_penalties_it)
                {
                    vector<double> penalties_vals = action_period_penalties_it->second;
                    constraintvaluespenalties[pair<int,string>(move_num,action_period_penalties_it->first)].push_back(vector<double>(penalties_vals.size(),0));//To keep the same structure as graph
                    constraintvaluespenalties[pair<int,string>(move_num,action_period_penalties_it->first)].push_back(penalties_vals);
                }*/
        return 0;
        }

    void FMTsasolution::write_events(const vector<FMTaction> model_actions,const string out_path,const string addon) const
    //Add to parser
    {
      //One layer per action per period
        if (!events.empty())
        {
            int period = 1;//No events on period 0
            for (vector<vector<FMTevent<FMTgraph>>> period_v : events)
            {
                if (!period_v.empty())
                {
                    //cout<<"Period : "<<period<<endl;
                    int action_id = 0;
                    for (vector<FMTevent<FMTgraph>> action_id_v : period_v)//Vector of events in action_id
                    {
                        if (!action_id_v.empty())
                        {
                            FMTlayer<int> action_layer(this->copyextent<int>());//Setting layer information
                            int event_id = 1; //To write in the map
                            map<int,string> event_map;
                            for (FMTevent<FMTgraph> event : action_id_v)//The event
                            {
                                if (!event.empty())
                                {
                                    //cout<<"Event_id : "<<event_id<<endl;
                                    for (map<FMTcoordinate,const FMTgraph*>::const_iterator coordit = event.elements.begin(); coordit != event.elements.end(); ++coordit)
                                    {

                                        action_layer.mapping[coordit->first]=event_id;
                                    }
                                    event_map[event_id] = "Event_"+to_string(event_id);
                                    event_id++;
                                }
                            }
                            if (action_layer.mapping.size()>0)
                            {
                                string action_name = model_actions.at(action_id).name;
                                string out_location = out_path+action_name+"_"+addon+"_events_period_"+std::to_string(period)+".tif";
                                WSParser::FMTareaparser parser;
                                parser.writelayer(action_layer,out_location,event_map);
                            }
                        }
                        action_id++;
                    }
                }
                period++;
            }
        }
    }

}
