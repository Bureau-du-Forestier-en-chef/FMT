#include "FMTspatialschedule.h"
#include "FMTmodel.h"
#include <numeric>
#include <algorithm>
#include <set>
#include <iterator>

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
            std::queue<Graph::FMTgraph<Graph::FMTbasevertexproperties,Graph ::FMTbaseedgeproperties>::FMTvertex_descriptor> actives = local_graph.initialize(actdevelopment);
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
        this->events = rhs.events;
		this->outputscache = rhs.outputscache;
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
		forest.passinobject(*this);
        for(std::map<FMTcoordinate,Graph::FMTlinegraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
        {
            const Graph::FMTlinegraph* local_graph = &graphit->second;
            const std::vector<double> solutions(1,this->getcellsize());
			forest.mapping[graphit->first] = local_graph->getperiodstopdev(period);
			/*std::vector<Core::FMTactualdevelopment> actdev = local_graph->getperiodstopdev(period,&solutions[0]);//
            forest.mapping[graphit->first]=Core::FMTdevelopment(actdev.front());*/
        }
        return forest;
    }

    bool FMTspatialschedule::allow_action(const FMTspatialaction& targetaction, const std::vector<FMTspatialaction>& modelactions,
                                          const FMTcoordinate& location, const int& period, const std::vector<size_t>& maximalpatchsizes) const
    {
		try
		{
			int MINGU = static_cast<int>((period - targetaction.green_up));
			
			for (size_t green_up = std::max(0, MINGU); green_up < static_cast<size_t>(period); ++green_up)
			{
				int naction_id = 0;
				for (const Core::FMTaction& mact : modelactions)
				{
					if (std::find(targetaction.neighbors.begin(), targetaction.neighbors.end(), mact.getname()) != targetaction.neighbors.end())
					{
						const unsigned int distance = static_cast<unsigned int>(targetaction.adjacency) + std::max(targetaction.maximal_size, maximalpatchsizes.at(naction_id));
						const unsigned int minx = distance > location.getx() ? 0 : location.getx() - distance;
						const unsigned int miny = distance > location.gety() ? 0 : location.gety() - distance;
						const unsigned int maxofx = (distance + location.getx()) > maxx ? maxx : (distance + location.getx());
						const unsigned int maxofy = (distance + location.gety()) > maxy ? maxy : (distance + location.gety());
						const FMTcoordinate minimallocation(minx, miny);
						const FMTcoordinate maximallocation(maxofx, maxofy);;
						for (const FMTeventcontainer::const_iterator eventit : events.getevents(static_cast<int>(green_up), naction_id, minimallocation, maximallocation))
							{
							if (eventit->withinc(static_cast<unsigned int>(targetaction.adjacency), location))
								{
								return false;
								}
							}

					}
					++naction_id;
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTspatialschedule::allow_action", __LINE__, __FILE__);
		}
        return true;
    }


	std::vector<std::set<Spatial::FMTcoordinate>> FMTspatialschedule::getupdatedscheduling(
																	const std::vector<Spatial::FMTspatialaction>& spactions,
																	const Core::FMTschedule& selection,
																	boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>& cachedactions,
																	const Core::FMTyields& yields,
																	bool schedule_only,
																	std::vector<std::set<Spatial::FMTcoordinate>> original,
																	std::vector<FMTcoordinate> updatedcoordinate) const
		{
		try {
			if (original.empty())
				{
				original.resize(spactions.size());
				updatedcoordinate.reserve(mapping.size());
				for (std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator itc = mapping.begin(); itc != mapping.end(); ++itc)
					{
					updatedcoordinate.push_back(itc->first);
					}
				}
			//boost::unordered_map<Core::FMTdevelopment,std::vector<bool>>cachedactions;
			//cachedactions.reserve(updatedcoordinate.size());
			for (const FMTcoordinate& updated : updatedcoordinate)
				{
				const Graph::FMTlinegraph& lg = mapping.at(updated);
				const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor& active = lg.getactivevertex();
				const Core::FMTdevelopment& active_development = lg.getdevelopment(active);
				boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>::iterator cacheit = cachedactions.find(active_development);
				if (cacheit == cachedactions.end())
					{
					std::pair<boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>::iterator,bool>insertedpair = cachedactions.insert(std::make_pair(active_development, std::vector<bool>(spactions.size(), false)));
					cacheit = insertedpair.first;
					size_t actionid = 0;
					for (const Spatial::FMTspatialaction& action : spactions)
					{
						if ((schedule_only && selection.operated(action, active_development)) ||
							(!schedule_only && active_development.operable(action, yields)))
						{
							cacheit->second[actionid] = true;
						}else {
							cacheit->second[actionid] = false;
						}
					++actionid;
					}
					}
				size_t actionid = 0;
				for (const Spatial::FMTspatialaction& action : spactions)
					{
					if (cacheit->second.at(actionid))
					{
						original[actionid].insert(updated);
					}else{
						original[actionid].erase(updated);
						}
					++actionid;
					}

				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getupdatedscheduling", __LINE__, __FILE__);
			}
		return original;
		}

   std::set<FMTcoordinate>FMTspatialschedule::getscheduling(	const Spatial::FMTspatialaction& action,
																const Core::FMTschedule& selection,
																const Core::FMTyields& yields,
																bool schedule_only) const
    {
        std::set<FMTcoordinate>scheduling;
		try
		{
			for (std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator itc = mapping.begin(); itc != mapping.end(); ++itc)
			{
				const Graph::FMTlinegraph& lg = itc->second;
				const Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor& active = lg.getactivevertex();
				const Core::FMTdevelopment& active_development = lg.getdevelopment(active);
				if (selection.operated(action, active_development) ||
						(!schedule_only && active_development.operable(action, yields)))
					{
						scheduling.insert(itc->first);
					}
			}
		}
		catch(...)
		{
			_exhandler->raisefromcatch("","FMTspatialschedule::getscheduling", __LINE__, __FILE__);
		}
        return scheduling;
    }

    std::set<FMTcoordinate> FMTspatialschedule::verifyspatialfeasability(const FMTspatialaction& targetaction,
                                                                         const std::vector<FMTspatialaction>& modelactions,
                                                                         const int& period,
                                                                         const std::set<FMTcoordinate>& operables) const
    {
    std::set<FMTcoordinate> spatialyallowable;
	try
	{
		const std::vector<size_t>maxsizes = getmaximalpatchsizes(modelactions);
		for (std::set<FMTcoordinate>::const_iterator itc = operables.begin(); itc != operables.end(); ++itc)
		{
			if (allow_action(targetaction, modelactions, *itc, period, maxsizes))
			{
				spatialyallowable.insert(*itc);
			}
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTspatialschedule::verifyspatialfeasability", __LINE__, __FILE__);
	}
    return spatialyallowable;
    }

	FMTeventcontainer FMTspatialschedule::buildharvest(	const double & target, const FMTspatialaction & targetaction, 
														std::default_random_engine & generator, const std::set<FMTcoordinate>& lmapping, 
														const int& period, const int& actionid, std::vector<FMTcoordinate>& operated) const
	{
		//To gain efficiency, maybe tracking cell that have been ignit actually, we are suposing that we are trying every cell, but its not true because of the random generator
		double harvested_area = 0;
		FMTeventcontainer cuts;
		try {
		std::set<FMTcoordinate> mapping_pass = lmapping;
		size_t count = lmapping.size();
		int tooclosecall = 0;
		int initdone = 0;
		int spreaddone = 0;
		bool check_adjacency = (std::find(targetaction.neighbors.begin(), targetaction.neighbors.end(), targetaction.getname()) != targetaction.neighbors.end());
		if (!mapping_pass.empty())
		{
			std::set<FMTcoordinate>::const_iterator randomit;
			while (harvested_area < target && count > 0 && !mapping_pass.empty())
			{
				std::uniform_int_distribution<int> celldistribution(0, mapping_pass.size() - 1);
				const int cell = celldistribution(generator);//Get cell to ignit
				randomit = mapping_pass.begin();
				std::advance(randomit, cell);
				FMTevent newcut;
				if (newcut.ignit(targetaction, *randomit, actionid, period))
				{
					++initdone;
					if (newcut.spread(targetaction, mapping_pass))
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
							operated.reserve(newcut.elements.size());
							for (const FMTcoordinate& toremove : newcut.elements)
								{
								operated.push_back(toremove);
								mapping_pass.erase(toremove);
								}
							harvested_area += (static_cast<double>(newcut.elements.size())*cellsize);
							count = mapping_pass.size() + 1;
						}
					}
				}
				--count;
			}
		}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::buildharvest", __LINE__, __FILE__);
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
				schedule.passinobject(*this);
				operatedschedules[period - 1] += schedule;
			}
		}
		return operatedschedules;
	}

	double FMTspatialschedule::evaluatespatialconstraint(const Core::FMTconstraint& spatialconstraint,
		const std::vector<Spatial::FMTspatialaction>& spactions) const
		{
		double returnvalue = 0;
		try {
			int periodstart = 0;
			int periodstop = 0;
			if (this->mapping.empty())
			{
				this->mapping.begin()->second.constraintlenght(spatialconstraint, periodstart, periodstop);
			}
			int action_id = 0;
			for (const Spatial::FMTspatialaction& spaction : spactions)
			{
				std::vector<int> actionsid_neighbors;
				for (int actionid = 0; actionid < static_cast<int>(spactions.size()); ++actionid)
				{
					if (std::find(spaction.neighbors.begin(), spaction.neighbors.end(), spactions.at(actionid).getname()) != spaction.neighbors.end())
					{
						actionsid_neighbors.push_back(actionid);
					}
				}
				for (int period = periodstart; period <= periodstop; ++period)
				{
					double lower = 0;
					double upper = 0;
					spatialconstraint.getbounds(lower, upper, static_cast<int>(period));
					for (const FMTeventcontainer::const_iterator& eventit : events.getevents(period, action_id))
					{
						double event_objective = 0;
						if (spatialconstraint.getconstrainttype() == Core::FMTconstrainttype::FMTspatialadjacency)
							{
							const double event_val = static_cast<double>(events.minimaldistance(*eventit, static_cast<unsigned int>(lower), period, actionsid_neighbors));
							if (event_val<lower)
								{
								event_objective = (lower - event_val);
								}
						}else if (spatialconstraint.getconstrainttype() == Core::FMTconstrainttype::FMTspatialsize)
							{
							const double event_val = static_cast<double>(eventit->size());
							if (event_val<lower)
								{
								event_objective = lower - event_val;
							}else if (event_val>upper)
								{
								event_objective = event_val-upper;
								}
							}
						if ((event_objective < 0 || std::isnan(event_objective) || event_objective == std::numeric_limits<double>::max()))
						{
							_exhandler->raise(Exception::FMTexc::FMTrangeerror,
								"Got a bad spatial constraint evaluation for " + std::string(spatialconstraint) + " at " + std::to_string(event_objective),
								"FMTsasolution::evaluate", __LINE__, __FILE__);

						}

						returnvalue += event_objective;
					}
				}
				++action_id;
			}

		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::evaluatespatialconstraint", __LINE__, __FILE__);
			}
		return returnvalue;
		}


	std::vector<int>FMTspatialschedule::isbetterthan(const FMTspatialschedule& newsolution,
													const Models::FMTmodel& model,
													const std::vector<Spatial::FMTspatialaction>& spactions) const
		{
		std::vector<int>groupevaluation;
		try {
			const std::vector<Core::FMTconstraint> constraints = model.getconstraints();
			size_t maximalgroup = 0;
			for (const Core::FMTconstraint& constraint : constraints)
				{
				const size_t groupid = constraint.getgroup();
				if (groupid> maximalgroup)
					{
					maximalgroup = groupid;
					}
				}
			groupevaluation = std::vector<int>(maximalgroup+1,0);
			for (const Core::FMTconstraint& constraint : constraints)
				{
				const size_t groupid = constraint.getgroup();
				double newvalue = 0;
				double oldvalue = 0;
				if (!constraint.isspatial())
					{
					const std::vector<double>oldoutputvalues = this->getgraphsoutputs(model, constraint);
					const std::vector<double>newoutputvalues = newsolution.getgraphsoutputs(model, constraint,this);
					if (!newoutputvalues.empty() && !oldoutputvalues.empty())
						{
						newvalue = constraint.evaluate(newoutputvalues);
						oldvalue = constraint.evaluate(oldoutputvalues);
						}
					
				}else{//evaluate spatial stuff
					newvalue = newsolution.evaluatespatialconstraint(constraint, spactions);
					oldvalue = this->evaluatespatialconstraint(constraint, spactions);
					}
				if (newvalue<oldvalue)
					{
					++groupevaluation[groupid];
				}else if(oldvalue<newvalue)
					{
					--groupevaluation[groupid];
					}
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::isbetterthan", __LINE__, __FILE__);
			}
		return groupevaluation;
		}

	std::vector<double> FMTspatialschedule::getgraphsoutputs(const Models::FMTmodel & model, const Core::FMTconstraint & constraint,
															const FMTspatialschedule*	friendlysolution) const
	{
		std::vector<double>periods_values;
		try {
			int periodstart = 0;
			int periodstop = 0;
			if (!this->mapping.empty() && this->mapping.begin()->second.constraintlenght(constraint, periodstart, periodstop))
			{
				size_t oldcachesize = 0;
				if (friendlysolution!=nullptr 
					&& !friendlysolution->outputscache.empty()
					&& outputscache.size()!=friendlysolution->outputscache.size())
					{
					outputscache.insert(friendlysolution->outputscache.begin(), friendlysolution->outputscache.end());
					oldcachesize = outputscache.size();
					}
				//*_logger << "calculating with cash size of " << outputscache.size() << "\n";
				periods_values = std::vector<double>(periodstop - periodstart + 1, 0);
				const std::vector<double> solutions(1, this->getcellsize());
				if (!(periodstart==periodstop && constraint.acrossperiod()))
					{
					const size_t constrainthash = constraint.hash();
					std::vector<const Graph::FMTlinegraph*>graphs;
					/*if (constraint.isactionbased() && !constraint.isinventory())//get the graphs from the events
						{
							graphs = getgraphsfromdynamic(constraint,model,getfromevents(constraint, model.getactions(), periodstart, periodstop));
						}else {
							graphs = getgraphsfromdynamic(constraint,model);
						}
					for (const Graph::FMTlinegraph* graphptr : graphs)
						{
						setoutputfromgraph(*graphptr, model, periods_values, constraint, &solutions[0], periodstart, periodstop, constrainthash);
						}*/
					if (constraint.isactionbased() && !constraint.isinventory())//get the graphs from the events
						{
						for (const Graph::FMTlinegraph* graph : getfromevents(constraint, model.getactions(), periodstart, periodstop))
							{
							setoutputfromgraph(*graph, model, periods_values,constraint, &solutions[0], periodstart, periodstop, constrainthash);
							}

						}else{
						for (std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
							{
							setoutputfromgraph(graphit->second, model, periods_values, constraint, &solutions[0], periodstart, periodstop, constrainthash);

							}
						}
					}
				//*_logger << "cashit of " << (cashit / static_cast<double>(this->mapping.size())) * 100 << "\n";
			if (friendlysolution != nullptr && 
				oldcachesize!=outputscache.size())
				{
				//friendlysolution->outputscache.insert(outputscache.begin(), outputscache.end());
				friendlysolution->outputscache = outputscache;
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getgraphsoutputs", __LINE__, __FILE__);
			}
		return periods_values;
	}
	std::string FMTspatialschedule::getpatchstats(const std::vector<Core::FMTaction>& actions) const
	{
		std::string result = "";
		for (int period = events.firstperiod(); period <= events.lastperiod(); ++period)
		{
			for (int action_id = 0; action_id < actions.size(); ++action_id)
			{
				std::vector<FMTeventcontainer::const_iterator> evsit = events.getevents(period, action_id);
				for (const auto& eventit : evsit)
				{
					result += std::to_string(period) + " " + actions.at(action_id).getname() + " " + eventit->getstats() + "\n";
				}
			}
		}
		return result;
	}

	FMTlayer<std::string> FMTspatialschedule::lastdistlayer(const std::vector<Core::FMTaction>& modelactions, const int& period) const
	{
		FMTlayer<std::string> distlayer(this->copyextent<std::string>());
		for (std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
		{
			const int lastactid = graphit->second.getlastactionid(period);
			if (lastactid > 0)
			{
				distlayer.mapping[graphit->first] = modelactions.at(graphit->second.getlastactionid(period)).getname();
			}
			
		}
		return distlayer;
	}

	std::vector<Core::FMTGCBMtransition> FMTspatialschedule::getGCBMtransitions(FMTlayer<std::string>& stackedactions, const std::vector<Core::FMTaction>& modelactions, const std::vector<Core::FMTtheme>& classifiers, const int& period) const
	{
		std::vector<Core::FMTGCBMtransition>GCBM;
		std::map<std::string, std::vector<int>> ageaftercontainer;
		std::map<std::string, std::map<std::string, std::map<std::string, int>>> finalattributes;
		//Iter through spatialschedule
		for (std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
		{
			// lastaction id = -1 no action in period
			int lastactionid = graphit->second.getlastactionid(period);
			if (lastactionid >= 0)
			{
				stackedactions.mapping[graphit->first] = modelactions.at(lastactionid).getname();
				//For each classifier, append the value at the begining of the period and keep track of value at the end in finalattributes. Also keep the ageafter.
				if (!classifiers.empty())
				{
					const Core::FMTdevelopment sdev = graphit->second.getperiodstartdev(period);
					const Core::FMTdevelopment fdev = graphit->second.getperiodstopdev(period);
					const Core::FMTdevelopment snpdev = graphit->second.getperiodstartdev(period+1);
					const int fage = snpdev.age;
					std::map<std::string, std::string> themeattributes;
					for (const auto& theme : classifiers)
					{
						std::string themename = "THEME" + std::to_string(theme.getid() + 1);
						const std::string fclass = fdev.mask.get(theme);
						themeattributes[themename] = fclass;
						const std::string sclass = sdev.mask.get(theme);
						stackedactions.mapping[graphit->first] += "-" + sclass;
					}
					std::string stackname = stackedactions.mapping.at(graphit->first);
					if (ageaftercontainer.find(stackname) != ageaftercontainer.end())
					{
						ageaftercontainer[stackname].push_back(fage);
					}
					else {
						ageaftercontainer[stackname] = std::vector<int>(1, fage);
					}
					if (finalattributes.find(stackname) != finalattributes.end())
					{
						for (std::map<std::string, std::string>::const_iterator attit = themeattributes.begin(); attit != themeattributes.end(); ++attit)
						{
							finalattributes[stackname][attit->first][attit->second] = 1;
						}
					}
					else
					{
						for (std::map<std::string, std::string>::const_iterator attit = themeattributes.begin(); attit != themeattributes.end(); ++attit)
						{
							finalattributes[stackname][attit->first][attit->second] += 1;
						}
					}
				}
			}
		}
		//Iter through ageafter container where the first key is the stackname
		for (std::map<std::string, std::vector<int>>::const_iterator ageit = ageaftercontainer.begin(); ageit != ageaftercontainer.end(); ++ageit)
		{
			//Calculate average age 
			//Last argument in accumulate is the first element to add ... So we put a float and the return is a float to be able to round up
			const int ageafter = static_cast<int>(std::round(std::accumulate(ageit->second.begin(), ageit->second.end(), 0.0) / static_cast<float>(ageit->second.size())));
			std::map<std::string, std::string>theme_collection;
			//For each theme return the finalattributes that is the most present 
			for (std::map<std::string, std::map<std::string, int>>::const_iterator themeit = finalattributes.at(ageit->first).begin(); themeit != finalattributes.at(ageit->first).end(); ++themeit)
			{
				int maxhit = 0;
				std::string returntheme = "";
				for (std::map<std::string, int>::const_iterator cit = themeit->second.begin(); cit != themeit->second.end(); ++cit)
				{
					if (cit->second > maxhit)
					{
						maxhit = cit->second;
						returntheme = cit->first;
					}
				}
				theme_collection[themeit->first] = returntheme;
			}
			GCBM.push_back(Core::FMTGCBMtransition(ageafter, theme_collection, ageit->first));
		}
		return GCBM;
	}

	void FMTspatialschedule::eraselastperiod()
		{
		int lastperiod = -1;
		try {
			for (std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
				{
				lastperiod = graphit->second.getperiod()-1;
				graphit->second = graphit->second.partialcopy(lastperiod);
				}
			if (lastperiod!=-1)
				{
				std::set<FMTevent>::const_iterator periodit = events.getbounds(lastperiod).first;
				while (periodit!=events.end())
					{
					periodit = events.erase(periodit);
					}
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::eraselastperiod", __LINE__, __FILE__);
			}

		}

	void FMTspatialschedule::cleanincompleteconstraintscash(const std::vector<Core::FMTconstraint>& constraints)
		{
		try {
			std::unordered_map<size_t, std::vector<double>> newcashing;
			newcashing.reserve(outputscache.size());
			for (const Core::FMTconstraint& constraint : constraints)
				{
				int periodstart = 0;
				int periodstop = 0;
				if (!this->mapping.empty() && this->mapping.begin()->second.constraintlenght(constraint, periodstart, periodstop))
					{
					const size_t constraint_hashing = constraint.hash();
					const bool outputneedsaction = constraint.isactionbased();
					for (std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
						{
						size_t graphhash = constraint_hashing;
						if (!(outputneedsaction && graphit->second.isonlygrow())&&graphit->second.hashforconstraint(graphhash, periodstart,periodstop))
							{
							std::unordered_map<size_t, std::vector<double>>::const_iterator cacheit = outputscache.find(graphhash);
							if (cacheit!= outputscache.end())
								{
								newcashing[graphhash] = cacheit->second;
								}

							}
						}
					}
				}
			outputscache = newcashing;
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::cleanincompleteconstraintscash", __LINE__, __FILE__);
			}

		}

	std::vector<const Graph::FMTlinegraph*>FMTspatialschedule::getfromevents(const Core::FMTconstraint& constraint, const std::vector<Core::FMTaction>& actions, const int& start,const int& stop) const
		{
		std::vector<const Graph::FMTlinegraph*>graphs;
		try {
		if (constraint.isactionbased())
			{
			std::vector<int>targetedactions;
			for (const Core::FMToutputsource& osource : constraint.getsources())
				{
				if (osource.isvariable())
					{
					for (const Core::FMTaction* actionptr : osource.targets(actions))
						{
						const int actionid = static_cast<int>(std::distance(&(*actions.cbegin()), actionptr));
						if (std::find(targetedactions.begin(),targetedactions.end(),actionid)== targetedactions.end())
							{
							targetedactions.push_back(actionid);
							}

						}
					}
				}
			std::sort(targetedactions.begin(), targetedactions.end());
			for (int period = start; period <=stop; ++period)
				{
				for (std::set<FMTevent>::const_iterator eventit : events.getevents(period, targetedactions))
					{
					for (const FMTcoordinate& coordinate : eventit->elements)
						{
						const Graph::FMTlinegraph* graphptr = &(mapping.find(coordinate)->second);
						if (std::find(graphs.begin(), graphs.end(), graphptr) == graphs.end())
							{
							graphs.push_back(graphptr);
							}
						}

					}
				}
			

			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getfromevents", __LINE__, __FILE__);
			}
		return graphs;
		}

void FMTspatialschedule::setoutputfromgraph(const Graph::FMTlinegraph& linegraph, const Models::FMTmodel & model, std::vector<double>& periods_values,
												const Core::FMTconstraint & constraint, const double* solution, const int& start, const int& stop,size_t hashvalue) const
	{
	try {
	if (!(constraint.isactionbased() && linegraph.isonlygrow()))
		{
			const Graph::FMTlinegraph* local_graph = &linegraph;
			local_graph->hashforconstraint(hashvalue,start, constraint.getperiodupperbound());
			std::unordered_map<size_t, std::vector<double>>::const_iterator cashit = outputscache.find(hashvalue);
			if (cashit != outputscache.end())//get it from cashing
			{
				size_t periodid = 0;
				for (const double& cashvalue : cashit->second)
					{
					periods_values[periodid] += cashvalue;
					++periodid;
					}
			}else {//get it and add to cashing
				std::vector<double>graphvalues(stop - start + 1, 0);
				size_t periodid = 0;
				for (int period = start; period <= stop; ++period)
					{
					const std::map<std::string, double> output = local_graph->getoutput(model, constraint, period,solution, Graph::FMToutputlevel::totalonly);
					const double value = output.at("Total");
					periods_values[periodid] += value;
					graphvalues[periodid] = value;
					++periodid;
					}
				outputscache[hashvalue] = graphvalues;
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTspatialschedule::setoutputfromgraph", __LINE__, __FILE__);
		}
	}

std::vector<const Graph::FMTlinegraph*>FMTspatialschedule::getgraphsfromdynamic(const Core::FMTconstraint & constraint,
															const Models::FMTmodel& model,
															std::vector<const Graph::FMTlinegraph*> searchspace) const
	{
	std::vector<const Graph::FMTlinegraph*>graphs;
	try {
		const std::vector<Core::FMTtheme>dynamicthemes = model.locatedynamicthemes();
		const Core::FMTmask intersection = constraint.getvariableintersect();
		std::string nameofintersect;
		for (const Core::FMTtheme& theme : model.getthemes())
			{
			nameofintersect += "? ";
			}
		nameofintersect.pop_back();
		Core::FMTmask newmask(nameofintersect, intersection.getbitsetreference());
		for (const Core::FMTtheme& theme : dynamicthemes)
			{
			newmask.set(theme, "?");
			}
		double totalsize = 0;
		if (!searchspace.empty())
			{
			totalsize = static_cast<double>(searchspace.size());
			for (const Graph::FMTlinegraph* graphptr  : searchspace)
				{
				if (graphptr->getperiodstopdev(0).mask.issubsetof(newmask))
					{
					graphs.push_back(graphptr);
					}
				}
		}else {
			totalsize = static_cast<double>(this->mapping.size());
			for (std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				const Graph::FMTlinegraph* graphptr = &graphit->second;
				if (graphptr->getperiodstopdev(0).mask.issubsetof(newmask))
				{
					graphs.push_back(graphptr);
				}
			}

		}
		const double efficiency = (1 - (static_cast<double>(graphs.size()) / totalsize)) * 100;
		*_logger << "Efficiency of " << efficiency << " of "<<std::string(constraint)<<"\n";
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTspatialschedule::getgraphsfromdynamic", __LINE__, __FILE__);
		}
	return graphs;
	}

std::vector<size_t>FMTspatialschedule::getmaximalpatchsizes(const std::vector<FMTspatialaction>& spactions) const
	{
	std::vector<size_t>maxsizes;
	maxsizes.reserve(spactions.size());
	for (const FMTspatialaction& spaction : spactions)
		{
		size_t patchsize = spaction.maximal_size;
		if (spaction.maximal_size==std::numeric_limits<size_t>::max())
			{
			patchsize = 0;
			}
		maxsizes.push_back(patchsize);
		}
	return maxsizes;
	}


}
