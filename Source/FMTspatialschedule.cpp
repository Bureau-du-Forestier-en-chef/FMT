#include "FMTspatialschedule.h"
#include "FMTmodel.h"
#include <numeric>
#include <algorithm>
#include <set>
#include <iterator>
#include "FMTspatialnodescache.h"

namespace Spatial
{
    FMTspatialschedule::FMTspatialschedule():FMTlayer<Graph::FMTlinegraph>(),cache(),events()
    {
        //ctor
    }

    FMTspatialschedule::FMTspatialschedule(const FMTforest& initialmap) : FMTlayer<Graph::FMTlinegraph>(), cache(), events()
    {
        FMTlayer<Graph::FMTlinegraph>::operator = (initialmap.copyextent<Graph::FMTlinegraph>());//Setting layer information
		std::vector<FMTcoordinate>coordinates;
		coordinates.reserve(initialmap.mapping.size());
        for(std::map<FMTcoordinate,Core::FMTdevelopment>::const_iterator devit = initialmap.mapping.begin(); devit != initialmap.mapping.end(); ++devit)
        {
			std::vector<Core::FMTactualdevelopment> actdevelopment;
            actdevelopment.push_back(Core::FMTactualdevelopment (devit->second,initialmap.getcellsize()));
            Graph::FMTlinegraph local_graph(Graph::FMTgraphbuild::schedulebuild);
            std::queue<Graph::FMTgraph<Graph::FMTbasevertexproperties,Graph ::FMTbaseedgeproperties>::FMTvertex_descriptor> actives = local_graph.initialize(actdevelopment);
            mapping[devit->first] = local_graph;
			coordinates.push_back(devit->first);
        }
		cache = FMTspatialnodescache(coordinates);
    }

    FMTspatialschedule::FMTspatialschedule(const FMTspatialschedule& other):
            FMTlayer<Graph::FMTlinegraph>(other),
            cache(other.cache),
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
		cache = rhs.cache;
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
		if (cache.size()<rhs.cache.size())
			{
			cache = rhs.cache;
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
		/*if (cache.size() < rhs.cache.size())
		{
			cache.swap(rhs.cache);
		}*/
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
		try {
			forest.passinobject(*this);
			for(std::map<FMTcoordinate,Graph::FMTlinegraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
				const Graph::FMTlinegraph* local_graph = &graphit->second;
				const std::vector<double> solutions(1,this->getcellsize());
				forest.mapping[graphit->first] = local_graph->getperiodstopdev(period);
				/*std::vector<Core::FMTactualdevelopment> actdev = local_graph->getperiodstopdev(period,&solutions[0]);//
				forest.mapping[graphit->first]=Core::FMTdevelopment(actdev.front());*/
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("For period "+std::to_string(period), "FMTspatialschedule::getforestperiod", __LINE__, __FILE__);
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
				if (active_development.period != selection.getperiod())
					{
					_exhandler->raise(Exception::FMTexc::FMTrangeerror,
						"Wrong developement/schedule period " + std::to_string(active_development.period),
						"FMTspatialschedule::getupdatedscheduling", __LINE__, __FILE__);
					}
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
														std::default_random_engine & generator,std::set<FMTcoordinate> mapping_pass,
														const int& period, const int& actionid, std::vector<FMTcoordinate>& operated) const
	{
		//To gain efficiency, maybe tracking cell that have been ignit actually, we are suposing that we are trying every cell, but its not true because of the random generator
		double harvested_area = 0;
		FMTeventcontainer cuts;
		try {
			size_t count = mapping_pass.size();
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
								const size_t adjacency = static_cast<size_t>(targetaction.adjacency);
								const size_t maximaldistance = adjacency + static_cast<size_t>(targetaction.maximal_size);
								for (const std::set<FMTevent>::const_iterator cutit :cuts.getevents(period,newcut.getterritory(maximaldistance)))
								{
									if (cutit->within(adjacency, newcut))
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

	double FMTspatialschedule::getconstraintevaluation(const Core::FMTconstraint&constraint,
		const Models::FMTmodel& model, const std::vector<Spatial::FMTspatialaction>& spactions,
		const FMTspatialschedule*	friendlysolution) const
	{
		double value = 0;
		try {
			if (!constraint.isspatial())
			{
				const std::vector<double>outputvalues = this->getgraphsoutputs(model, constraint, friendlysolution);
				if (!outputvalues.empty())
				{
					value = constraint.evaluate(outputvalues);
				}

			}
			else {//evaluate spatial stuff
				value = this->evaluatespatialconstraint(constraint, spactions);
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getconstraintevaluation", __LINE__, __FILE__);
		}
	return value;
	}

	double FMTspatialschedule::getprimalinfeasibility(const std::vector<Core::FMTconstraint>& constraints, const Models::FMTmodel& model,
		const std::vector<Spatial::FMTspatialaction>& spactions,
		const FMTspatialschedule*	friendlysolution) const
	{
		double value = 0;
		try {
			for (const Core::FMTconstraint& constraint: constraints)
				{
				value += getconstraintevaluation(constraint, model, spactions, friendlysolution);
				}
			
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getprimalinfeasibility", __LINE__, __FILE__);
			}
		return value;
	}

	double FMTspatialschedule::getobjectivevalue(const Core::FMTconstraint& constraint, const Models::FMTmodel& model,
		const std::vector<Spatial::FMTspatialaction>& spactions,
		const FMTspatialschedule*	friendlysolution) const
	{
		double value = 0;
		try {
			value = getconstraintevaluation(constraint, model, spactions, friendlysolution)*constraint.sense();
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getobjectivevalue", __LINE__, __FILE__);
		}
		return value;
	}


	std::vector<int> FMTspatialschedule::isbetterthan(const FMTspatialschedule& newsolution,
													const Models::FMTmodel& model,
													const std::vector<Spatial::FMTspatialaction>& spactions) const
		{
		std::vector<int> groupevaluation;
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
			groupevaluation = std::vector<int>(maximalgroup + 1, 0);
			std::vector<double>groupsprimalinfeasibilitygap(maximalgroup + 1, 0);
			for (const Core::FMTconstraint& constraint : constraints)
				{
				const double oldvalue = this->getconstraintevaluation(constraint, model, spactions);
				const double newvalue= newsolution.getconstraintevaluation(constraint, model, spactions,this);
				if (!(newvalue==0 && oldvalue==0))
					{
					const size_t groupid = constraint.getgroup();
					const double constraintdif = (oldvalue - newvalue);
					groupsprimalinfeasibilitygap[groupid] += constraintdif;
					/*if (constraintdif < 0)
					{
						++groupevaluation[groupid];
					}else if (constraintdif > FMT_DBL_TOLERANCE)
						{
						--groupevaluation[groupid];
						}*/
					}
				}
			for (size_t groupid = 0 ;groupid < groupevaluation.size();++groupid)
				{
				if (groupsprimalinfeasibilitygap[groupid]>0)//Make sure the primalinfeasibility get better for the group!
					{
					groupevaluation[groupid] = -1;
					}
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::isbetterthan", __LINE__, __FILE__);
			}
		return groupevaluation;
		}

	void FMTspatialschedule::logsolutionstatus(const size_t& iteration,const double& objective, const double& primalinfeasibility) const
	{
	try {
		_logger->logwithlevel("Iteration "+std::to_string(iteration)+" Primal Inf(" + std::to_string(primalinfeasibility) + ") Obj(" + std::to_string(objective) + ")\n", 1);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTspatialschedule::logsolutionstatus", __LINE__, __FILE__);
		}
	}

	void FMTspatialschedule::getsolutionstatus(double& objective, double& primalinfeasibility, const Models::FMTmodel& model,
		const std::vector<Spatial::FMTspatialaction>& spactions,
		const FMTspatialschedule*	friendlysolution) const
	{
		try {
			std::vector<Core::FMTconstraint>constraints = model.getconstraints();
			objective = this->getobjectivevalue(constraints.at(0), model, spactions, friendlysolution);
			constraints.erase(constraints.begin());
			primalinfeasibility = this->getprimalinfeasibility(constraints, model, spactions, friendlysolution);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getsolutionstatus", __LINE__, __FILE__);
		}
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
					&& !friendlysolution->cache.empty()
					&& cache.size()!=friendlysolution->cache.size())
					{
					cache.insert(friendlysolution->cache);
					oldcachesize = cache.size();
					}
				if (constraint.acrossperiod())
					{
					++periodstop;
					}
				periods_values = std::vector<double>(periodstop - periodstart + 1, 0);
				const int constraintupperbound = constraint.getperiodupperbound();
				const std::vector<double> solutions(1, this->getcellsize());
				if (!(periodstart == periodstop && constraint.acrossperiod()))
				{
					const std::vector<Core::FMTtheme> statictransitionsthemes = model.locatestatictransitionsthemes();
					for (const Core::FMToutputnode& node : constraint.getnodes(model.area, model.actions, model.yields))
					{
						if (node.source.isvariable())
							{
							bool exactnode = false;
							const std::vector<FMTcoordinate>& nodescoordinates = cache.getnode(node, model, exactnode);//starting point to simplification
							std::vector<std::pair<size_t,int>>periodstolookfor;
							size_t periodid = 0;
							for (int period = periodstart; period <= periodstop; ++period)
								{
								if (!cache.getactualnodecache()->gotcachevalue(period))
									{
									periodstolookfor.push_back(std::pair<size_t, int>(periodid,period));
								}else {
									periods_values[periodid] = cache.getactualnodecache()->getcachevalue(period);
									}
								++periodid;
								}
							if (!periodstolookfor.empty())
								{
								if (!exactnode&&cache.getactualnodecache()->worthintersecting)
									{
									setgraphcachebystatic(nodescoordinates, node); //Needs to be fixed !!!
									}
								const std::vector<FMTcoordinate>& staticcoordinates = cache.getnode(node, model, exactnode);//Will possibility return the whole map...
								const Core::FMTmask dynamicmask = cache.getactualnodecache()->dynamicmask;
								if (node.isactionbased() && !node.source.isinventory())
								{
									for (const std::pair<size_t, int>& periodpair : periodstolookfor)
									{
										const std::vector<FMTcoordinate>eventscoordinate = getfromevents(node, model.getactions(), periodpair.second);
										std::vector<FMTcoordinate>selection;
										if (cache.getactualnodecache()->worthintersecting)
											{
											std::set_intersection(staticcoordinates.begin(), staticcoordinates.end(),
												eventscoordinate.begin(), eventscoordinate.end(),
												std::back_inserter(selection));//filter from the static selection and the events selected....
										}else {
											selection = eventscoordinate;
											}
										if (cache.getactualnodecache()->worthintersecting &&
											selection.size()== eventscoordinate.size())//Then the intersection was useless...say it to the cache
											{
											cache.getactualnodecache()->worthintersecting = false;
											//*_logger << "intersect not usefull for " << std::string(node) << "\n";
										}
										/*else if (cache.getactualnodecache()->worthintersecting && selection.size() != eventscoordinate.size())
											{
											*_logger << "intersect proved usefull for " <<std::string(node) <<"\n";
											}*/
										for (const FMTcoordinate& coordinate: selection)
										{
											size_t patternhash = 0;
											const Graph::FMTlinegraph* graph = &mapping.at(coordinate);
											graph->hashforconstraint(patternhash, periodpair.second, dynamicmask);
											periods_values[periodpair.first] += getoutputfromgraph(*graph, model, node, &solutions[0], periodpair.second, patternhash, cache.getactualnodecache()->patternvalues);
										}
									}
								}else {
									bool useless = false;
									for (const FMTcoordinate& coordinate : staticcoordinates)
										{
											const Graph::FMTlinegraph* graph = &mapping.at(coordinate);
											const size_t basegraphhash = graph->getbasehash(dynamicmask);
											for (const std::pair<size_t, int>& periodpair : periodstolookfor)
											{
												size_t patternhash = graph->getedgeshash(periodpair.second, useless);
												boost::hash_combine(patternhash, basegraphhash);
												periods_values[periodpair.first] += getoutputfromgraph(*graph, model, node, &solutions[0], periodpair.second, patternhash, cache.getactualnodecache()->patternvalues);
											}
										}
								}
								for (const std::pair<size_t, int>& periodpair : periodstolookfor)
								{
									
									/*if (cache.getactualnodecache()->gotcachevalue(periodpair.second))
									{
										double cash = cache.getactualnodecache()->getcachevalue(periodpair.second);
										double notcash = periods_values[periodpair.first];
										if (cash > 0 && notcash > 0 && cash != notcash)
										{
											*_logger << std::string(node) << " cash " << cash << " not cash " << notcash << "\n";
										}
									}*/
									cache.getactualnodecache()->setvalue(periodpair.second, periods_values[periodpair.first]);
								}
								
								
								
								}
						}
						
					}
				}
			if (friendlysolution != nullptr && 
				oldcachesize!=cache.size())
				{
				friendlysolution->cache.insert(cache);
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
		try {
			const int lastperiod = this->mapping.begin()->second.getperiod() - 1;
			cache.removeperiod(lastperiod);
			for (std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
				{
				graphit->second.clearfromperiod(lastperiod, true);
				}
			std::set<FMTevent>::const_iterator periodit = events.getbounds(lastperiod).first;
			while (periodit!=events.end())
				{
				periodit = events.erase(periodit);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::eraselastperiod", __LINE__, __FILE__);
			}

		}

	/*void FMTspatialschedule::cleanincompleteconstraintscash(const Models::FMTmodel& model)
		{
		try {
			std::unordered_map<size_t, std::vector<double>> newcashing;
			const std::vector<Core::FMTconstraint> constraints = model.getconstraints();
			newcashing.reserve(outputscache.size());
			for (const Core::FMTconstraint& constraint : constraints)
				{
				int periodstart = 0;
				int periodstop = 0;
				if (!this->mapping.empty() && this->mapping.begin()->second.constraintlenght(constraint, periodstart, periodstop))
					{
					
					const int contraintupper = constraint.getperiodupperbound();
					for (Core::FMToutputnode& node : constraint.getnodes(model.area,model.actions,model.yields))
						{
						size_t nodehash = node.hash();
						Core::FMTmask dynamicmask;
						std::unordered_map<size_t, Core::FMTmask>::const_iterator maskit = nodesmaskcache.find(nodehash);
						if (maskit != nodesmaskcache.end())
						{
							dynamicmask = maskit->second;
						}
						else {
							dynamicmask = model.getdynamicmask(node);
							nodesmaskcache[nodehash] = dynamicmask;
						}
						boost::hash_combine(nodehash, periodstop);
						for (std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
						{
							size_t graphhash = nodehash;
							if (!(node.isactionbased() && graphit->second.isonlygrow()) && graphit->second.hashforconstraint(graphhash, contraintupper, dynamicmask))
							{
								std::unordered_map<size_t, std::vector<double>>::const_iterator cacheit = outputscache.find(graphhash);
								if (cacheit != outputscache.end())
								{
									newcashing[graphhash] = cacheit->second;
								}

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

		} */

	std::vector<FMTcoordinate>FMTspatialschedule::getfromevents(const Core::FMToutputnode& node, const std::vector<Core::FMTaction>& actions, const int& period) const
	{
		//std::vector<const Graph::FMTlinegraph*>graphs;
		std::vector<FMTcoordinate>coordinates;
		try {
			if (node.isactionbased())
			{
				std::vector<int>targetedactions;
				if (node.source.isvariable())
				{
					for (const Core::FMTaction* actionptr : node.source.targets(actions))
					{
						const int actionid = static_cast<int>(std::distance(&(*actions.cbegin()), actionptr));
						if (std::find(targetedactions.begin(), targetedactions.end(), actionid) == targetedactions.end())
						{
							targetedactions.push_back(actionid);
						}

					}
				}
				std::sort(targetedactions.begin(), targetedactions.end());
				for (std::set<FMTevent>::const_iterator eventit : events.getevents(period, targetedactions))
					{
						for (const FMTcoordinate& coordinate : eventit->elements)
						{
							//const Graph::FMTlinegraph* graphptr = &(mapping.find(coordinate)->second);
							//graphs.push_back(graphptr);
							coordinates.push_back(coordinate);
						}
					}
				std::sort(coordinates.begin(), coordinates.end());
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getfromevents", __LINE__, __FILE__);
		}
		return coordinates;

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
			std::set<FMTcoordinate>collected;
			for (int period = start; period <=stop; ++period)
				{
				for (std::set<FMTevent>::const_iterator eventit : events.getevents(period, targetedactions))
					{
					for (const FMTcoordinate& coordinate : eventit->elements)
						{
						const Graph::FMTlinegraph* graphptr = &(mapping.find(coordinate)->second);
						if (collected.find(coordinate)==collected.end())
							{
							collected.insert(coordinate);
							graphs.push_back(graphptr);
							}
						/*if (std::find(graphs.begin(), graphs.end(), graphptr) == graphs.end())
							{
							graphs.push_back(graphptr);
							}*/
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

double FMTspatialschedule::getoutputfromgraph(const Graph::FMTlinegraph& linegraph, const Models::FMTmodel & model,
											 const Core::FMToutputnode& node, const double* solution, const int&period, const size_t& hashvalue,
											 std::unordered_map<size_t, double>& nodecache) const
	{
	double value = 0;
	try{
	if (!(node.isactionbased()&&linegraph.isonlygrow()))
	{
		//const Graph::FMTlinegraph* local_graph = &linegraph;
		//linegraph.hashforconstraint(hashvalue,constraintupperbound, dynamicmask);
		Core::FMTtheme targettheme;
		bool complete = false;
		//boost::hash_combine(hashvalue,period);
		std::unordered_map<size_t,double>::const_iterator cashit = nodecache.find(hashvalue);
		if (cashit != nodecache.end())//get it from cashing
		{
			value = cashit->second;
		}else {//get it and add to cashing
			const std::map<std::string, double> output = linegraph.getsource(model, node, period, targettheme, solution, Graph::FMToutputlevel::totalonly);
			value = output.at("Total");
			nodecache[hashvalue] = value;

		}
	}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTspatialschedule::getoutputfromgraph", __LINE__, __FILE__);
		}
	return value;
	}

/*void FMTspatialschedule::setoutputfromgraph(const Graph::FMTlinegraph& linegraph, const Models::FMTmodel & model, std::vector<double>& periods_values,
											const Core::FMTconstraint & constraint, const double* solution, const int& start, const int& stop,size_t hashvalue,
											const Core::FMTmask& dynamicmask) const
	{
	try {
	if (!(constraint.isactionbased() && linegraph.isonlygrow()))
		{
			const Graph::FMTlinegraph* local_graph = &linegraph;
			local_graph->hashforconstraint(hashvalue,constraint.getperiodupperbound(),dynamicmask);
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
	}*/

void FMTspatialschedule::setgraphcachebystatic(const std::vector<FMTcoordinate>& coordinates,const Core::FMToutputnode& node) const
	{
	//we should use a static  output node cache here
	std::vector<FMTcoordinate>goodcoordinates;
	try {
			//double totalsize = static_cast<double>(this->mapping.size());
			for (const FMTcoordinate& coordinate : coordinates)
				{
				const Graph::FMTlinegraph* linegraph = &mapping.at(coordinate);
				if (linegraph->getbasedevelopment().mask.issubsetof(cache.getactualnodecache()->staticmask))
					{
					goodcoordinates.push_back(coordinate);
					}
				}
			cache.setnode(node, goodcoordinates);
			//const double efficiency = (1 - (static_cast<double>(graphs.size()) / totalsize)) * 100;
			//*_logger << "Efficiency of " << efficiency << "\n";
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTspatialschedule::setgraphcachebystatic", __LINE__, __FILE__);
		}
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


void FMTspatialschedule::removegraphfromcache(const Graph::FMTlinegraph& graph,
	const Models::FMTmodel& model, const std::vector<Spatial::FMTspatialaction>& spactions)
{
	const std::vector<double> solutions(1, this->getcellsize());
	std::unordered_set<size_t>nodesnperiodremoved;
	for (const Core::FMTconstraint& constraint : model.constraints)
		{
		int periodstart = 0;
		int periodstop = 0;
		bool exact = false;
		if (graph.constraintlenght(constraint, periodstart, periodstop))
			{
			for (const Core::FMToutputnode& node : constraint.getnodes(model.area, model.actions, model.yields))
				{
				if (node.source.isvariable())
					{
						const size_t completenodehash = node.hashforvalue();
						cache.getnode(node, model, exact);
						const Core::FMTmask dynamicmask = cache.getactualnodecache()->dynamicmask;
						const size_t basegraphhash = graph.getbasehash(dynamicmask);
						for (int period = periodstart; period <= periodstop; ++period)
						{
							size_t nodenperiodhash = 0;
							boost::hash_combine(nodenperiodhash, completenodehash);
							if (nodesnperiodremoved.find(nodenperiodhash) == nodesnperiodremoved.end())
							{
								size_t patternhash = graph.getedgeshash(period, exact);
								const double graphvalue = getoutputfromgraph(graph, model, node, &solutions[0], period, patternhash, cache.getactualnodecache()->patternvalues);
								cache.getactualnodecache()->removevaluefromperiod(period, graphvalue);
								nodesnperiodremoved.insert(nodenperiodhash);
							}
						}
					}
					}
				}
		}

}

void FMTspatialschedule::addgraphtocache(const Graph::FMTlinegraph& graph,
	const Models::FMTmodel& model, const std::vector<Spatial::FMTspatialaction>& spactions)
{
	const std::vector<double> solutions(1, this->getcellsize());
	std::unordered_set<size_t>nodesnperiodremoved;
	for (const Core::FMTconstraint& constraint : model.constraints)
	{
		int periodstart = 0;
		int periodstop = 0;
		bool exact = false;
		if (graph.constraintlenght(constraint, periodstart, periodstop))
		{
			for (const Core::FMToutputnode& node : constraint.getnodes(model.area, model.actions, model.yields))
			{
				if (node.source.isvariable())
				{
					const size_t completenodehash = node.hashforvalue();
					cache.getnode(node, model, exact);
					const Core::FMTmask dynamicmask = cache.getactualnodecache()->dynamicmask;
					const size_t basegraphhash = graph.getbasehash(dynamicmask);
					for (int period = periodstart; period <= periodstop; ++period)
					{
						size_t nodenperiodhash = 0;
						boost::hash_combine(nodenperiodhash, completenodehash);
						if (nodesnperiodremoved.find(nodenperiodhash) == nodesnperiodremoved.end())
						{
							size_t patternhash = graph.getedgeshash(period, exact);
							const double graphvalue = getoutputfromgraph(graph, model, node, &solutions[0], period, patternhash, cache.getactualnodecache()->patternvalues);
							cache.getactualnodecache()->addvaluefromperiod(period, graphvalue);
							nodesnperiodremoved.insert(nodenperiodhash);
						}
					}
				}
			}
		}
	}

}


}
