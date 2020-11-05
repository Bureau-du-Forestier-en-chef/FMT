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


	bool FMTspatialschedule::allow_action(const int& targetaction, const std::vector<Spatial::FMTbindingspatialaction>&bindingactions, const FMTcoordinate& location, const int& period) const
	{
		try
		{
			const size_t targetmaximalsize = bindingactions.at(targetaction).getmaximalsize();
			const int lowergup = static_cast<int>(bindingactions.at(targetaction).getminimalgreenup());
			const size_t loweradjacency = bindingactions.at(targetaction).getminimaladjacency();
			for (int green_up = std::max(1,period- lowergup); green_up <= period; ++green_up)
				{
				for (const int& mact : bindingactions.at(targetaction).getneighbors())
							{
							const size_t distance = loweradjacency + std::max(targetmaximalsize, bindingactions.at(mact).getmaximalsize());
							const size_t minx = distance > location.getx() ? 0 : location.getx() - distance;
							const size_t miny = distance > location.gety() ? 0 : location.gety() - distance;
							const size_t maxofx = (distance + location.getx()) > maxx ? maxx : (distance + location.getx());
							const size_t maxofy = (distance + location.gety()) > maxy ? maxy : (distance + location.gety());
							const FMTcoordinate minimallocation(minx, miny);
							const FMTcoordinate maximallocation(maxofx, maxofy);
							for (const FMTeventcontainer::const_iterator eventit : events.getevents(green_up, mact, minimallocation, maximallocation))
								{
								if (eventit->within(loweradjacency, location))
									{
									return false;

									}
								}
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
																	const std::vector<Core::FMTaction>& actions,
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
				original.resize(actions.size());
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
					std::pair<boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>::iterator,bool>insertedpair = cachedactions.insert(std::make_pair(active_development, std::vector<bool>(actions.size(), false)));
					cacheit = insertedpair.first;
					size_t actionid = 0;
					for (const Core::FMTaction& action : actions)
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
				for (const Core::FMTaction& action : actions)
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



	std::set<FMTcoordinate> FMTspatialschedule::verifyspatialfeasability(const int& targetaction,
		const std::vector<Spatial::FMTbindingspatialaction>& bindingactions,
		const int& period, const std::set<FMTcoordinate>& operables) const
	{
		std::set<FMTcoordinate> spatialyallowable;
		try
		{
			for (std::set<FMTcoordinate>::const_iterator itc = operables.begin(); itc != operables.end(); ++itc)
			{
				if (allow_action(targetaction, bindingactions,*itc, period))
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



	FMTeventcontainer FMTspatialschedule::buildharvest(const double & target, const Spatial::FMTbindingspatialaction& targetaction,
		std::default_random_engine & generator, std::set<FMTcoordinate> mapping_pass,
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
			const bool check_adjacency = (std::find(targetaction.getneighbors().begin(), targetaction.getneighbors().end(), actionid) != targetaction.getneighbors().end());
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
					if (newcut.ignit(targetaction.getmaximalsize(), *randomit, actionid, period))
					{
						++initdone;
						if (newcut.spread(targetaction.getminimalsize(), targetaction.getmaximalsize(), targetaction.getminimalneighborsize(), mapping_pass))
						{
							++spreaddone;
							bool tooclose = false;
							if (check_adjacency)
							{
								const size_t adjacency = targetaction.getminimaladjacency();
								const size_t maximaldistance = adjacency + targetaction.getmaximalsize();
								for (const std::set<FMTevent>::const_iterator cutit : cuts.getevents(period, newcut.getterritory(maximaldistance)))
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
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTspatialschedule::buildharvest", __LINE__, __FILE__);
		}
		return cuts;
	}


	double FMTspatialschedule::operate(const FMTeventcontainer& cuts, const Core::FMTaction& action, const int& action_id, const Core::FMTtransition& Transition,
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

	double FMTspatialschedule::evaluatespatialconstraint(const Core::FMTconstraint& spatialconstraint, const Models::FMTmodel& model) const
	{
	double returnvalue = 0;
	try {
		int periodstart = 0;
		int periodstop = 0;
		if (this->mapping.begin()->second.constraintlenght(spatialconstraint, periodstart, periodstop))
		{
			const std::vector<int>actionids = spatialconstraint.getactionids(model.actions);
			const Core::FMTconstrainttype spatialconstrainttype = spatialconstraint.getconstrainttype();
			double lower = 0;
			double upper = 0;
			for (int period = periodstart; period <= periodstop; ++period)
			{
				spatialconstraint.getbounds(lower, upper,period);
				for (const FMTeventcontainer::const_iterator& eventit : events.getevents(period, actionids))
				{
					double event_objective = 0;
					switch (spatialconstrainttype)
					{
					case Core::FMTconstrainttype::FMTspatialsize:
					{
						const double event_val = static_cast<double>(eventit->size());
						if (event_val < lower)
						{
							event_objective = lower - event_val;
						}
						else if (event_val > upper)
						{
							event_objective = event_val - upper;
						}
						break;
					}
					case Core::FMTconstrainttype::FMTspatialadjacency:
					{

						const Core::FMTyldbounds boundsyld = spatialconstraint.getyldsbounds().at("GUP");
						const int lowergup = static_cast<int>(boundsyld.getlower());
						for (int gupperiod = std::max(1, period - lowergup); gupperiod <= period; ++gupperiod)
						{
							event_objective += events.evaluatedistance(*eventit, lower, upper, gupperiod, actionids);
						}
						break;
					}
					default:
					{
						break;
					}
					}
					returnvalue += event_objective;
				}
			}
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTspatialschedule::evaluatespatialconstraint", __LINE__, __FILE__);
		}
	return returnvalue;
	}


	double FMTspatialschedule::getconstraintevaluation(const Core::FMTconstraint&constraint,
		const Models::FMTmodel& model,const FMTspatialschedule*	friendlysolution) const
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
				value = this->evaluatespatialconstraint(constraint,model);
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getconstraintevaluation", __LINE__, __FILE__);
		}
	return value;
	}

	double FMTspatialschedule::getprimalinfeasibility(const std::vector<Core::FMTconstraint>& constraints, const Models::FMTmodel& model,
		const FMTspatialschedule*	friendlysolution) const
	{
		double value = 0;
		try {
			for (const Core::FMTconstraint& constraint: constraints)
				{
				value += getconstraintevaluation(constraint, model,friendlysolution);
				}

		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getprimalinfeasibility", __LINE__, __FILE__);
			}
		return value;
	}

	double FMTspatialschedule::getobjectivevalue(const Core::FMTconstraint& constraint, const Models::FMTmodel& model,
		const FMTspatialschedule*	friendlysolution,bool withsense) const
	{
		double value = 0;
		try {
			value = getconstraintevaluation(constraint, model,friendlysolution)*(withsense ? constraint.sense() : 1);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getobjectivevalue", __LINE__, __FILE__);
		}
		return value;
	}


	std::vector<int> FMTspatialschedule::isbetterthan(const FMTspatialschedule& newsolution,
													const Models::FMTmodel& model) const
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
				const double oldvalue = this->getconstraintevaluation(constraint, model);
				const double newvalue= newsolution.getconstraintevaluation(constraint, model,this);
				if (!(newvalue==0 && oldvalue==0))
					{
					const size_t groupid = constraint.getgroup();
					const double constraintdif = (oldvalue - newvalue);
					groupsprimalinfeasibilitygap[groupid] += constraintdif;
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
		const FMTspatialschedule*	friendlysolution, bool withsense) const
	{
		try {
			std::vector<Core::FMTconstraint>constraints = model.getconstraints();
			objective = this->getobjectivevalue(constraints.at(0), model,friendlysolution, withsense);
			constraints.erase(constraints.begin());
			primalinfeasibility = this->getprimalinfeasibility(constraints, model,friendlysolution);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getsolutionstatus", __LINE__, __FILE__);
		}
	}

	double FMTspatialschedule::getglobalobjective(const Models::FMTmodel& model, const FMTspatialschedule*	friendlysolution) const
	{
		double global = 0;
		try {
			double objective = 0;
			double infeasibilities = 0;
			getsolutionstatus(objective, infeasibilities, model, friendlysolution,false);
			global = (objective + infeasibilities);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTspatialschedule::getglobalobjective", __LINE__, __FILE__);
			}
		return global;
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


std::vector<Spatial::FMTbindingspatialaction> FMTspatialschedule::getbindingactions(const Models::FMTmodel& model, const int& period) const
	{
	const size_t nactions = model.actions.size();
	std::vector<double>minimalsizes(nactions,0);
	std::vector<double>maximalsizes(nactions,std::numeric_limits<double>::max());
	std::vector<double>minimalgreenups(nactions,0);
	std::vector<double>maximalgreenups(nactions, std::numeric_limits<double>::max());
	std::vector<double>minimalnsizes(nactions,0);
	std::vector<double>maximalnsizes(nactions,std::numeric_limits<double>::max());
	std::vector<double>minimaladjacencys(nactions, 0);
	std::vector<double>maximaladjacencys(nactions, std::numeric_limits<double>::max());
	std::vector<std::vector<int>>neighboring(nactions, std::vector<int>());
	std::vector<Spatial::FMTbindingspatialaction>bindings;
	try {
		bindings.reserve(nactions);
		for (const Core::FMTconstraint& constraint : model.constraints)
		{
			if (constraint.isspatial() &&
				period >= constraint.getperiodlowerbound() &&
				period <= constraint.getperiodupperbound())
			{
				double lower = 0;
				double upper = 0;
				constraint.getbounds(lower, upper, period);
				const std::vector<int>actionids = constraint.getactionids(model.actions);
				for (const int& actionid : actionids)
					{

					for (const int& subid : actionids)
						{
						if (std::find(neighboring.at(actionid).begin(), neighboring.at(actionid).end(), subid) == neighboring.at(actionid).end())
							{
							neighboring.at(actionid).push_back(subid);
							}
						}
					}
				if (constraint.getconstrainttype() == Core::FMTconstrainttype::FMTspatialsize)
				{
					const Core::FMTyldbounds yldbounds = constraint.getyldsbounds().at("NSIZE");
					const double lowernsize = yldbounds.getlower();
					const double uppernsize = yldbounds.getupper();
					for (const int& actionid : actionids)
					{
						if (upper < maximalsizes.at(actionid))
						{
							maximalsizes[actionid] = upper;
						}
						if (lower > minimalsizes.at(actionid))
						{
							minimalsizes[actionid] = lower;
						}
						if (uppernsize < maximalnsizes.at(actionid))
						{
							maximalnsizes[actionid] = uppernsize;
						}
						if (lowernsize > minimalnsizes.at(actionid))
						{
							minimalnsizes[actionid] = lowernsize;
						}
					}
				}
				else if (constraint.getconstrainttype() == Core::FMTconstrainttype::FMTspatialadjacency)
				{
					const Core::FMTyldbounds yldbounds = constraint.getyldsbounds().at("GUP");
					const double lowergup = yldbounds.getlower();
					const double uppergup = yldbounds.getupper();
					for (const int& actionid : actionids)
					{
						if (upper < maximaladjacencys.at(actionid))
						{
							maximaladjacencys[actionid] = upper;
						}
						if (lower > minimaladjacencys.at(actionid))
						{
							minimaladjacencys[actionid] = lower;
						}
						if (uppergup < maximalgreenups.at(actionid))
						{
							maximalgreenups[actionid] = uppergup;
						}
						if (lowergup > minimalgreenups.at(actionid))
						{
							minimalgreenups[actionid] = lowergup;
						}
					}
				}

			}


		}
		for (size_t actionid = 0; actionid < nactions;++actionid)
			{
			const size_t minimalsize = static_cast<size_t>(minimalsizes.at(actionid));
			const size_t maximalsize = (maximalsizes.at(actionid) == std::numeric_limits<double>::max()) ? std::numeric_limits<size_t>::max() : static_cast<size_t>(maximalsizes.at(actionid));
			const size_t minimalgreenup = static_cast<size_t>(minimalgreenups.at(actionid));
			const size_t maximalgreenup = (maximalgreenups.at(actionid) == std::numeric_limits<double>::max()) ? std::numeric_limits<size_t>::max() : static_cast<size_t>(maximalgreenups.at(actionid));
			const size_t minimalnsize = static_cast<size_t>(minimalnsizes.at(actionid));
			const size_t maximalnsize = (maximalnsizes.at(actionid) == std::numeric_limits<double>::max()) ? std::numeric_limits<size_t>::max() : static_cast<size_t>(maximalnsizes.at(actionid));
			const size_t minimaladjacency = static_cast<size_t>(minimaladjacencys.at(actionid));
			const size_t maximaladjacency = (maximaladjacencys.at(actionid) == std::numeric_limits<double>::max()) ? std::numeric_limits<size_t>::max() : static_cast<size_t>(maximaladjacencys.at(actionid));
			/*if (actionid==2||actionid==14)
			{
				*_logger << "minimalsize " << minimalsize << "\n";
				*_logger << "maximalsize " << maximalsize << "\n";
				*_logger << "minimalgreenup " << minimalgreenup << "\n";
				*_logger << "maximalgreenup " << maximalgreenup << "\n";
				*_logger << "minimalnsize " << minimalnsize << "\n";
				*_logger << "maximalnsize " << maximalnsize << "\n";
				*_logger << "minimaladjacency " << minimaladjacency << "\n";
				*_logger << "maximaladjacenc " << maximaladjacency << "\n";
			}*/


			bindings.emplace_back(neighboring.at(actionid), minimalgreenup, maximalgreenup, minimaladjacency, maximaladjacency,minimalsize, maximalsize, minimalnsize, maximalnsize);
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTspatialschedule::getbindingactions", __LINE__, __FILE__);
		}
	return bindings;
	}




void FMTspatialschedule::removegraphfromcache(const Graph::FMTlinegraph& graph,const Models::FMTmodel& model)
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

void FMTspatialschedule::addgraphtocache(const Graph::FMTlinegraph& graph,const Models::FMTmodel& model)
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

std::map<std::string, double> FMTspatialschedule::referencebuild(const Core::FMTschedule& schedule, const Models::FMTmodel& model,
	bool schedule_only,
	bool scheduleatfirstpass,
	unsigned int seed)
{
	std::map<std::string, double>results;
	try {
		const int period = this->actperiod();
		this->setnewperiod();
		std::default_random_engine generator(seed);
		const double total_area = schedule.area();
		std::map<std::string, double>targets;
		for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator ait = schedule.begin(); ait != schedule.end(); ait++)
		{
			targets[ait->first.getname()] = schedule.actionarea(ait->first);
		}
		double allocated_area = 0;
		const std::vector<Spatial::FMTbindingspatialaction>bindingactions = this->getbindingactions(model, period);
		if (!schedule.empty() || !schedule_only)
		{
			double pass_allocated_area = 0;
			bool schedulepass = scheduleatfirstpass;
			bool schedulechange = false;
			int pass = 0;
			boost::unordered_map<Core::FMTdevelopment, std::vector<bool>>cachedactions;
			std::vector<std::set<Spatial::FMTcoordinate>>actions_operabilities = this->getupdatedscheduling(model.actions, schedule, cachedactions, model.yields, schedulepass);
			do {
				pass_allocated_area = 0;
				int action_id = 0;
				if (schedulechange)
				{
					cachedactions.clear();
					actions_operabilities = this->getupdatedscheduling(model.actions, schedule, cachedactions, model.yields, schedulepass);
					schedulechange = false;
				}
				for (const Core::FMTaction& action : model.actions)
				{
					const double action_area = targets[action.getname()];
					const std::set<Spatial::FMTcoordinate> allowable_coordinates = actions_operabilities.at(action_id);
					if (!allowable_coordinates.empty() && action_area > 0)
					{
						const std::set<Spatial::FMTcoordinate> spatialy_allowable = this->verifyspatialfeasability(action_id, bindingactions, period, allowable_coordinates);
						if (!spatialy_allowable.empty())
						{
							std::vector<Spatial::FMTcoordinate> updatedcells;
							const Spatial::FMTeventcontainer harvest = this->buildharvest(action_area, bindingactions.at(action_id), generator, spatialy_allowable, period, action_id, updatedcells);
							if (harvest.size() > 0)
							{
								const double operatedarea = this->operate(harvest, action, action_id, model.transitions[action_id], model.yields, model.themes);
								this->addevents(harvest);
								actions_operabilities = this->getupdatedscheduling(model.actions, schedule, cachedactions, model.yields, schedulepass, actions_operabilities, updatedcells);
								targets[action.getname()] -= operatedarea;
								pass_allocated_area += operatedarea;
							}
						}
					}
					++action_id;
				}
				allocated_area += pass_allocated_area;
				++pass;
				if (!schedule_only && pass_allocated_area == 0)
				{
					if (schedulepass)
					{
						schedulechange = true;
						schedulepass = false;
					}
					else {
						schedule_only = true;
					}
				}
			} while (allocated_area < total_area && (pass_allocated_area != 0 || (!schedule_only)));
		}
		this->grow();
		results["Total"] = allocated_area / total_area;
		for (std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>::const_iterator ait = schedule.begin(); ait != schedule.end(); ait++)
		{
			double total_action_area = schedule.actionarea(ait->first);
			results[ait->first.getname()] = ((total_action_area - targets[ait->first.getname()]) / total_action_area);
		}
	}
	catch (...)
	{
		_exhandler->printexceptions("", "FMTspatialschedule::newsimulate", __LINE__, __FILE__);
	}
	return results;


}

std::map<std::string, double> FMTspatialschedule::greedyreferencebuild(const Core::FMTschedule& schedule, const Models::FMTmodel& model,
	const size_t& randomiterations,
	unsigned int seed,
	double tolerance)
	{
		std::map<std::string, double>bestresults;
		FMTspatialschedule solutioncopy(*this);
		const size_t maxstall = 5;
		std::default_random_engine generator(seed);
		const double factorgap = 0.5;
		std::uniform_real_distribution<double>scheduledistribution(1 - factorgap, 1);
		size_t stalcount = 0;
		size_t iteration = 0;
		const unsigned int initialseed = seed;
		try {
			bestresults["Total"] = 0;
			const double totaliterations = static_cast<double>(randomiterations);
			double lastprimalinf = 0;
			double lastobjective = 0;
			double lastschedulefactor = 1;
			size_t failediterations = 0;
			bool didregular = false;
			while ((stalcount < maxstall && failediterations < randomiterations) && ((randomiterations > 1) || (randomiterations == 1) && iteration < 1))//&& iteration < randomiterations
			{
				double factorit = (static_cast<double>(iteration) / totaliterations);
				if (factorit > 1)
				{
					factorit = scheduledistribution(generator);
				}
				double schedulefactor = (randomiterations == 1) ? 1 : 1 - ((1 - factorit) * factorgap);//bottom up
				if (failediterations == (randomiterations - 1) && !didregular)//The last try will be the regular stuff
				{
					seed = initialseed;
					schedulefactor = 1;
					didregular = true;
				}
				bool scheduleonly = false;
				const Core::FMTschedule factoredschedule = schedule.getnewschedule(schedulefactor);
				const std::map<std::string, double>results = solutioncopy.referencebuild(factoredschedule,model, false, true, seed);
				if (iteration == 0 || solutioncopy.isbetterbygroup(*this,model))
				{
					bestresults = results;
					lastschedulefactor = schedulefactor;
					*this = solutioncopy;
					double newprimalinf = 0;
					double newobjective = 0;
					this->getsolutionstatus(newobjective, newprimalinf,model);
					this->logsolutionstatus(iteration, newobjective, newprimalinf);
					if (std::abs(lastprimalinf - newprimalinf) <= FMT_DBL_TOLERANCE &&
						std::abs(lastobjective - newobjective) <= FMT_DBL_TOLERANCE)
					{
						++stalcount;
					}
					else {
						stalcount = 0;
					}
					lastprimalinf = newprimalinf;
					lastobjective = newobjective;
					failediterations = 0;
				}
				else {
					++failediterations;
				}
				solutioncopy.eraselastperiod();//clear the last period to redo a simulate and test again!
				++seed;
				++iteration;
			}
			if (stalcount == maxstall)
			{
				_logger->logwithlevel("Stalled after " + std::to_string(stalcount) + " iterations Skipping\n", 1);
			}
			if (failediterations == randomiterations)
			{
				_logger->logwithlevel("Solution stuck after " + std::to_string(iteration) + " iterations Skipping\n", 1);
			}
			//Need the remove the incomplete stuff from the cash before going to the next step.
			for (std::map<std::string, double>::iterator facit = bestresults.begin(); facit != bestresults.end(); facit++)
			{
				facit->second *= (lastschedulefactor < 1 ? 1 + (1 - lastschedulefactor) : 1 - (lastschedulefactor - 1));
			}
			bestresults["Primalinfeasibility"] = lastprimalinf;
			bestresults["Objective"] = lastobjective;
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::montecarlosimulate", __LINE__, __FILE__);
		}

		return bestresults;
	}

Graph::FMTgraphstats FMTspatialschedule::randombuild(const Models::FMTmodel& model, std::default_random_engine& generator)
	{
	Graph::FMTgraphstats periodstats;
	try {
		 periodstats = Graph::FMTgraphstats();
		for (std::map<FMTcoordinate, Graph::FMTlinegraph>::iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
			Graph::FMTlinegraph* local_graph = &graphit->second;
			std::queue<Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>::FMTvertex_descriptor> actives = local_graph->getactiveverticies();
			Graph::FMTgraphstats stats = local_graph->randombuildperiod(model, actives, generator, events, graphit->first);
			periodstats += local_graph->getstats();
			}
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTspatialschedule::randombuild", __LINE__, __FILE__);
		}
	return periodstats;
	}

void FMTspatialschedule::perturbgraph(const FMTcoordinate& coordinate, const Graph::FMTlinegraph& graph,const int&period, const Models::FMTmodel& model, std::default_random_engine& generator)
	{
	try {
		removegraphfromcache(graph, model);
		const Graph::FMTlinegraph newgraph = graph.perturbgraph(model, generator, events,coordinate,period);
		mapping[coordinate] = newgraph;
		addgraphtocache(newgraph, model);
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTspatialschedule::perturbgraph", __LINE__, __FILE__);
		}
	}

std::vector<std::map<Spatial::FMTcoordinate, Graph::FMTlinegraph>::const_iterator>FMTspatialschedule::getgraphs() const
	{
	std::vector<std::map<Spatial::FMTcoordinate, Graph::FMTlinegraph>::const_iterator>graphs;
	try {
		graphs.reserve(this->mapping.size());
		for (std::map<FMTcoordinate, Graph::FMTlinegraph>::const_iterator graphit = this->mapping.begin(); graphit != this->mapping.end(); ++graphit)
			{
			graphs.push_back(graphit);
			}
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTspatialschedule::getgraphs", __LINE__, __FILE__);
		}
	return graphs;
	}

bool FMTspatialschedule::isbetterbygroup(const FMTspatialschedule& rhs,const Models::FMTmodel& model) const
	{
	try {
		size_t gotbetter = 0;
		size_t groupid = 0;
		const std::vector<int>groupvalues = isbetterthan(rhs, model);
		for (const int& value : groupvalues)
		{
			if (value >= 0)
			{
				++gotbetter;
			}
			++groupid;
		}
		return (gotbetter == groupvalues.size());
	}catch (...)
		{
		_exhandler->printexceptions("", "FMTspatialschedule::isbetterbygroup", __LINE__, __FILE__);
		}
	return false;
	}


}
