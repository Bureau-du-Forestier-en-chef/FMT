#include "FMTspatialschedule.h"

namespace Spatial
{
    FMTspatialschedule::FMTspatialschedule():FMTlayer<Graph::FMTlinegraph>(),outputscache(),events()
    {
        //ctor
    }

    FMTspatialschedule::FMTspatialschedule(const FMTforest& initialmap):outputscache(),events()
    {
        FMTlayer<Graph::FMTlinegraph>::operator = (initialmap.copyextent<Graph::FMTlinegraph>());//Setting layer information
        for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator devit = initialmap.mapping.begin(); devit != initialmap.mapping.end(); ++devit)
        {
			std::vector<Core::FMTactualdevelopment> actdevelopment;
            actdevelopment.push_back(Core::FMTactualdevelopment (devit->second,initialmap.getcellsize()));
            Graph::FMTlinegraph local_graph(Graph::FMTgraphbuild::schedulebuild);
            std::queue<Graph::FMTvertex_descriptor> actives = local_graph.initialize(actdevelopment);
            mapping[devit->first] = local_graph;
        }
    }

    FMTspatialschedule::FMTspatialschedule(const FMTspatialschedule& other):
            FMTlayer<Graph::FMTlinegraph>(other),
            outputscache(other.outputscache),
            events(other.events)
    {
        //copy ctor
    }

    FMTspatialschedule& FMTspatialschedule::operator=(const FMTspatialschedule& rhs)
    {
        if (this == &rhs) return *this; // handle self assignment
        //assignment operator
        FMTlayer<Graph::FMTlinegraph>::operator = (rhs);
        events = rhs.events;
        if (outputscache.size() < rhs.outputscache.size())
			{
			outputscache = rhs.outputscache;
			}
        return *this;
    }

    bool FMTspatialschedule::operator == (const FMTspatialschedule& rhs)const
    {
        if (!(events==rhs.events)) return false;
        for (std::map<FMTcoordinate,Graph::FMTlinegraph>::const_iterator coordit = this->mapping.begin();
                                                                         coordit!= this->mapping.end(); ++coordit)
        {
            if (coordit->second != rhs.mapping.at(coordit->first))
            {
                return false;
            }
        }
    return true;
    }

    bool FMTspatialschedule::operator != (const FMTspatialschedule& rhs)const
    {
        return (!(*this==rhs));
    }

    int FMTspatialschedule::actperiod() const
    {
        Graph::FMTlinegraph flgraph = mapping.begin()->second;
        return flgraph.getperiod();
    }

    bool FMTspatialschedule::copyfromselected(const FMTspatialschedule& rhs, const std::vector<size_t>& selected)
    {
		std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator baseit;
		std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator newvalueit;
		//No location check sooo make sure you copy the same kind of solution...
		events = rhs.events;
		if (outputscache.size()<rhs.outputscache.size())
			{
			outputscache = rhs.outputscache;
			}
		if (this->size() == rhs.size())
			{
			for (const size_t& selection : selected)
				{
				baseit = std::next(this->mapping.begin(), selection);
				newvalueit = std::next(rhs.mapping.begin(), selection);
				baseit->second = newvalueit->second;
				}
			return true;
			}
		return false;
    }

	bool FMTspatialschedule::swapfromselected(FMTspatialschedule& rhs, const std::vector<size_t>& selected)
    {
		std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator baseit;
		std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator newvalueit;
		//No location check sooo make sure you copy the same kind of solution...
		events.swap(rhs.events);
		if (outputscache.size() < rhs.outputscache.size())
		{
			outputscache.swap(rhs.outputscache);
		}
		if (this->size() == rhs.size())
		{
			for (const size_t& selection : selected)
			{
				baseit = std::next(this->mapping.begin(), selection);
				newvalueit = std::next(rhs.mapping.begin(), selection);
				baseit->second.swap(newvalueit->second);
			}
			return true;
		}
		return false;
    }

    FMTforest FMTspatialschedule::getforestperiod(const int& period) const
    {
        FMTforest forest(this->copyextent<Core::FMTdevelopment>());//Setting layer information
        for(std::map<FMTcoordinate,Graph::FMTlinegraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
        {
            const Graph::FMTlinegraph* local_graph = &graphit->second;
            const std::vector<double> solutions(1,this->getcellsize());
			std::vector<Core::FMTactualdevelopment> actdev = local_graph->getperiodstopdev(period,&solutions[0]);//
            forest.mapping[graphit->first]=Core::FMTdevelopment(actdev.front());
        }
        return forest;
    }

    bool FMTspatialschedule::allow_action(const FMTspatialaction& targetaction,const std::vector<Core::FMTaction>& modelactions,
                                          const FMTcoordinate& location, const int& period) const
    {
        int MINGU = static_cast<int>((period - targetaction.green_up));
        for(size_t green_up = std::max(0,MINGU); green_up < static_cast<size_t>(period); ++green_up)
        {
            int naction_id = 0;
            for (const Core::FMTaction& mact : modelactions)
            {
                if (std::find(targetaction.neighbors.begin(), targetaction.neighbors.end(), mact.getname()) != targetaction.neighbors.end())
                {
                    std::vector<FMTeventcontainer::const_iterator> eventsatgupaid = events.getevents(green_up,naction_id);
                    if(!eventsatgupaid.empty())
                    {
                        for(const auto eventit : eventsatgupaid)
                        {
                            if(eventit->withinc(static_cast<unsigned int>(targetaction.adjacency),location))
                            {
                            return false;
                            }
                        }
                    }
                }
                ++naction_id;
            }
        }
        return true;
    }

    std::map<Core::FMTaction,std::set<FMTcoordinate>> FMTspatialschedule::getscheduling(const Core::FMTschedule& selection,
                                                                                        boost::unordered_map<Core::FMTdevelopment,std::vector<bool>>& cached_operability,
                                                                                        const Core::FMTyields& yields,
                                                                                        bool schedule_only) const
    {
        std::map<Core::FMTaction,std::set<FMTcoordinate>>scheduling;
        for(std::map<FMTcoordinate,Graph::FMTlinegraph>::const_iterator itc = mapping.begin(); itc != mapping.end(); ++itc)
        {
			Graph::FMTvertex_descriptor active = itc->second.getactivevertex();
			const Core::FMTdevelopment& active_development = itc->second.getdevelopment(active);
            if (cached_operability.find(active_development) == cached_operability.end())
            {
                if (!schedule_only)
                    {
                    cached_operability[active_development] = std::vector<bool>(selection.size(), false);
                    }
                int action_id = 0;
                for(std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator it = selection.begin();it != selection.end();it++)
                {

                    if  (selection.operated(it->first, active_development)||
                        (!schedule_only && active_development.operable(it->first,yields)))
                        {
                        if (scheduling.find(it->first)==scheduling.end())
                            {
                                scheduling[it->first] = std::set<FMTcoordinate>();
                            }
                        scheduling[it->first].insert(itc->first);
                        if (!schedule_only)
                            {
                                cached_operability[active_development][action_id] = true;
                            }
                        }
                    ++action_id;
                }
            }else
            {
				int action_id = 0;
				for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator it = selection.begin(); it != selection.end(); it++)
				{
					if (cached_operability[active_development][action_id])
					{
                        if(scheduling.find(it->first) == scheduling.end())
                        {
                            scheduling[it->first] = std::set<FMTcoordinate>();
                        }
                        scheduling[it->first].insert(itc->first);
                    }
                    ++action_id;
                }
            }
        }
        return scheduling;
    }

    std::set<FMTcoordinate> FMTspatialschedule::verifyspatialfeasability(const FMTspatialaction& targetaction,
                                                                         const std::vector<Core::FMTaction>& modelactions,
                                                                         const int& period,
                                                                         const std::set<FMTcoordinate>& operables) const
    {
    std::set<FMTcoordinate> spatialyallowable;
    for(std::set<FMTcoordinate>::const_iterator itc = operables.begin();itc != operables.end();++itc)
        {
        if (allow_action(targetaction,modelactions,*itc,period))
            {
            spatialyallowable.insert(*itc);
            }
        }
    return spatialyallowable;
    }

	FMTeventcontainer FMTspatialschedule::buildharvest(	const double & target, const FMTspatialaction & targetaction, 
														std::default_random_engine & generator, const std::set<FMTcoordinate>& lmapping, 
														const int& period, const int& actionid) const
	{
		double harvested_area = 0;
		FMTeventcontainer cuts;
		std::vector<int> locations(lmapping.size());
		unsigned int locid = 0;
		std::iota(locations.begin(), locations.end(), 0);
		int tooclosecall = 0;
		int initdone = 0;
		int spreaddone = 0;
		bool check_adjacency = (std::find(targetaction.neighbors.begin(), targetaction.neighbors.end(), targetaction.getname()) != targetaction.neighbors.end());
		if (!lmapping.empty())
		{
			std::shuffle(locations.begin(), locations.end(), generator);
			std::set<FMTcoordinate>::const_iterator randomit;
			while (harvested_area < target&&locid < locations.size())
			{
				randomit = lmapping.begin();
				std::advance(randomit, locations[locid]);
				FMTevent newcut;
				if (newcut.ignit(targetaction, *randomit, actionid, period))
				{
					++initdone;
					if (newcut.spread(targetaction,lmapping))
					{
						++spreaddone;
						bool tooclose = false;
						if (check_adjacency)
						{
							for (const FMTevent& cut :cuts)
							{
								if (cut.within(static_cast<unsigned int>(targetaction.adjacency), newcut))
								{
									tooclose = true;
									++tooclosecall;
									break;
								}
							}
						}
						if (!tooclose)
						{
							cuts.insert(newcut);
							harvested_area += (static_cast<double>(newcut.elements.size())*cellsize);
						}
					}
				}
				++locid;
			}
		}
		return cuts;
	}

	double FMTspatialschedule::operate(const FMTeventcontainer& cuts, const FMTspatialaction& action, const int& action_id, const Core::FMTtransition& Transition,
									 const Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes)
	{
		double operatedarea = 0;
		for (const FMTevent& cut : cuts)
		{
			for (std::set<FMTcoordinate>::const_iterator coordit = cut.elements.begin(); coordit != cut.elements.end(); coordit++)
			{
				Graph::FMTlinegraph* lg = &mapping.at(*coordit);
				const std::vector<Core::FMTdevelopmentpath> paths = lg->operate(action, action_id, Transition, ylds, themes);
				if (paths.size() > 1)
				{
					_exhandler->raise(Exception::FMTexc::FMTnotlinegraph, "More than one verticies for the graph after operate ... See if you have multiple transitions. Coord at " + std::string(*coordit),
						"FMTspatialschedule::operate", __LINE__, __FILE__);
				}
				operatedarea += cellsize;
			}
		}
		return operatedarea;
	}

	void FMTspatialschedule::addevents(const FMTeventcontainer& newevents)
	{
		events.merge(newevents);
	}

	void FMTspatialschedule::grow()
	{
		for (std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
		{
			Graph::FMTlinegraph* local_graph = &graphit->second;
			local_graph->grow();
		}
	}

	void FMTspatialschedule::setnewperiod()
	{
		for (std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
		{
			Graph::FMTlinegraph* local_graph = &graphit->second;
			local_graph->newperiod();
		}
	}

	std::vector<Core::FMTschedule> FMTspatialschedule::getschedules(const std::vector<Core::FMTaction>& modelactions) const
	{
		std::vector<Core::FMTschedule> operatedschedules;
		operatedschedules.reserve(actperiod() - 1);
		for (int period = 1; period < actperiod(); ++period)
		{
			operatedschedules.emplace_back(period, std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>());
		}
		std::vector<double> solution(1, getcellsize());
		for (std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator git = this->mapping.begin(); git != this->mapping.end(); ++git)
		{
			for (int period = 1; period < actperiod(); ++period)
			{
				Core::FMTschedule schedule = git->second.getschedule(modelactions,&solution[0],period);
				operatedschedules[period - 1] += schedule;
			}
		}
		return operatedschedules;
	}

	std::vector<double> FMTspatialschedule::getgraphsoutputs(	const Models::FMTmodel & model, const Core::FMTconstraint & constraint, 
																const int & periodstart, const int & periodstop) const
	{
		std::vector<double>periods_values(periodstop - periodstart + 1, 0);
		const std::vector<double> solutions(1, this->getcellsize());
		for (std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
		{
			const Graph::FMTlinegraph* local_graph = &graphit->second;
			const size_t hashvalue = local_graph->hash(constraint.Core::FMToutput::hash());
			std::vector<double>graphvalues(periodstop - periodstart + 1, 0);
			if (outputscache.find(hashvalue) != outputscache.end())
			{
				graphvalues = outputscache.at(hashvalue);
			}
			else {
				for (int period = 1; period < static_cast<int>(local_graph->size() - 1); ++period)
				{
					const std::map<std::string, double> output = local_graph->getoutput(model, constraint, period, &solutions[0], Graph::FMToutputlevel::totalonly);
					const double totalperiod = output.at("Total");
					graphvalues[period - 1] += totalperiod;
				}
				outputscache[hashvalue] = graphvalues;
			}
			for (int period = periodstart; period <= periodstop; ++period)
			{
				periods_values[period - 1] += graphvalues[period - 1];
			}

		}
		return periods_values;
	}
	std::string FMTspatialschedule::getpatchstats(const std::vector<Core::FMTaction>& actions) const
	{
		std::string result = "";
		for (int period = events.firstperiod(); period <= events.lastperiod(); ++period)
		{
			for (std::size_t action_id = 0; action_id < actions.size(); ++action_id)
			{
				std::vector<FMTeventcontainer::const_iterator> evsit = events.getevents(period, action_id);
				for (auto eventit : evsit)
				{
					result += std::to_string(period) + " " + actions.at(action_id).getname() + " " + eventit->getstats() + "\n";
				}
			}
		}
		return result;
	}
}
