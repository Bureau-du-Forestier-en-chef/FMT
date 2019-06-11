#include "FMTsasolution.h"
#include "FMTsamodel.h"
#include <algorithm>
#include <iostream>

namespace Spatial
{

FMTsasolution::FMTsasolution(const FMTforest& initialmap):events()
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
    initial_map = true;
}

FMTsasolution::FMTsasolution(const FMTsasolution& rhs):
        FMTlayer<FMTgraph>(rhs),
        initial_map(rhs.initial_map),
        solution_stats(rhs.solution_stats),
        events(rhs.events)
{

}

FMTsasolution& FMTsasolution::operator = (const FMTsasolution& rhs)
    {
    if (this!=&rhs)
        {
        FMTlayer<FMTgraph>::operator = (rhs);
        initial_map = rhs.initial_map;
        solution_stats = rhs.solution_stats;
        events = rhs.events;
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

FMTgraphstats FMTsasolution::buildperiod(const FMTmodel& model,default_random_engine& generator)
    {
    if (!initial_map)
        {
            vector<vector<FMTevent<FMTgraph>>> events_id;
            for(map<FMTcoordinate,FMTgraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)//change const_iterator to iterator because graph is modified
            {
                FMTgraph* local_graph = &graphit->second;
                std::queue<FMTvertex_descriptor> actives = local_graph->getactiveverticies();
                FMTgraphstats stats = local_graph->randombuild(model,actives,generator,events_id,graphit->first);
                cout<<string(local_graph->getstats())<<endl;
                cin.get();
                solution_stats += local_graph->getstats();
            }
            events.push_back(events_id);
        }
    else {initial_map=false;}//Keep it, its just to recreate the same mecanic as in lp model when we build period
    return solution_stats;
    }

FMTsasolution FMTsasolution::perturb(FMTsamodel& model, default_random_engine& generator,FMTsamovetype movetype) const
    {
        FMTsasolution newsolution(*this);
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
            shuffle(map_lenght.begin(),map_lenght.end(),generator);//shuffle all indices in the map
            uniform_int_distribution<int> celldistribution(0,map_lenght.size()-1);
            int numbercells = celldistribution(generator);//Get number of cell to perturb
            vector<size_t> ChangedId;
            for (int id = 0; id<numbercells; ++id)
            {
                map<FMTcoordinate,FMTgraph>::const_iterator luckygraph = this ->mapping.begin();
                std::advance(luckygraph,map_lenght.at(id));
                uniform_int_distribution<int> perioddistribution(1,this->mapping.size()-1);
                int period = perioddistribution(generator);
                newsolution.solution_stats -= luckygraph->second.getstats();
                FMTgraph newgraph = luckygraph -> second.perturbgraph(model,generator,newsolution.events,luckygraph->first,period);//perturb cell
                newsolution.solution_stats += newgraph.getstats();
                newsolution.mapping[luckygraph->first] = newgraph;
                ChangedId.push_back(map_lenght.at(id));
            }
            model.setmapidmodified(ChangedId);
            break;
        }
        case FMTsamovetype::cluster :
            break;//to do list before 2025

        default :
            break;
        }
        return newsolution;
    }
const FMTgraphstats& FMTsasolution::getsolution_stats() const
    {
        return solution_stats;
    }
const vector<vector<vector<FMTevent<FMTgraph>>>>& FMTsasolution::getevents() const
    {
        return events;
    }

double FMTsasolution::getgraphsconstraint(const FMTmodel& model, const FMTconstraint& constraint)const
    //Add all output and evaluate y value for each
    {
        double xvalue=0;
        for(map<FMTcoordinate,FMTgraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
            {
                int periodstart=0;
                int periodstop=0;
                const FMTgraph* local_graph = &graphit->second;
                local_graph->constraintlenght(constraint,periodstart,periodstop);
                const vector<double> solutions(1,this->getcellsize());
                for (int period = periodstart ; period<=periodstop ; ++period)
                    {
                        map<string, double> output = local_graph->getoutput(model,constraint,period,&solutions[0]);
                        double totalperiod = output.at("Total");
                        xvalue += totalperiod;
                    }

            }
        return this->evaluatey(constraint,xvalue);

    }

double FMTsasolution::geteventsconstraint(const FMTsamodel& model) const //Hardcoded because we need to put keyword in optimize section
    {
        //Return yvalue for each event
        return 0.0;
    }

double FMTsasolution::evaluatey(const FMTconstraint& constraint, const double& xvalue) const
    {
        return xvalue;
    }
}
