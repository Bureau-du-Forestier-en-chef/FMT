/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTnssmodel.hpp"
#include "FMTfuturdevelopment.hpp"
#include "FMTactualdevelopment.hpp"
#include "FMTexceptionhandler.hpp"

namespace Models
{
	FMTnssmodel::FMTnssmodel():
		FMTsrmodel(FMTmodel(), Models::FMTsolverinterface::CLP),
		generator() 
	{
	
	}

	FMTnssmodel::FMTnssmodel(const FMTnssmodel& rhs) :
		FMTsrmodel(rhs),
		generator(rhs.generator)
	{

	}

	FMTnssmodel::FMTnssmodel(const FMTmodel& rhs, unsigned int seed):
		FMTsrmodel(rhs,Models::FMTsolverinterface::CLP),
		generator(seed) 
	{
		FMTmodel::setparameter(SEED,seed);
	}

	std::vector<const Core::FMToutput*> FMTnssmodel::constraintstotarget(std::vector<double>& targets, const int& period)
		{
		std::vector<const Core::FMToutput*>targetedoutputs;
		try {
			targets.clear();
			std::vector<double>lowers;
			std::vector<double>uppers;
			for (const Core::FMTconstraint& constraint : constraints)
				{
				if (constraint.israndomaction() &&
					period>=constraint.getperiodlowerbound() &&
					period<=constraint.getperiodupperbound())
					{
				
					if (!constraint.dosupportrandom())
						{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Constraint "+std::string(constraint)+" does not support _RANDOM keyword", "FMTnssmodel::constraintstotarget", __LINE__, __FILE__);
						}
					
					double lower = 0;
					double upper = 0;
					constraint.getbounds(lower, upper,period);
					size_t location = 0;
					bool added = false;
					for (const Core::FMToutput* doneit : targetedoutputs)
						{
						if ((*doneit) == constraint)
							{
							if (lower>lowers.at(location))
								{
								lowers[location] = lower;
								}
							if (upper<uppers.at(location))
								{
								uppers[location] = upper;
								}
							added = true;
							}
						++location;
						}
					if (!added)
						{
						targetedoutputs.push_back(&constraint);
						lowers.push_back(lower);
						uppers.push_back(upper);
						}
					}
				}
			for (size_t outid = 0 ; outid < targetedoutputs.size();++outid)
				{
				const double lowerbound = std::max(lowers.at(outid),0.0);
				double value = lowerbound;
				if (uppers.at(outid)!= std::numeric_limits<double>::infinity())
					{
					const double upperbound = uppers.at(outid);
					std::uniform_real_distribution<double>udist(lowerbound, upperbound);
					
					value = udist(generator);
					}
				
				targets.push_back(value);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTnssmodel::constraintstotarget", __LINE__, __FILE__);
			}
		return targetedoutputs;
		}

	std::vector<std::set<size_t>> FMTnssmodel::getActionsTargets(const std::vector<const Core::FMToutput*>& p_allOutputs) const
		{
		std::vector<std::set<size_t>>outputActions(actions.size());
		try {
			size_t Id = 0;
			for (const Core::FMToutput* output : p_allOutputs)
			{
				for (const Core::FMTaction* actionPtr : output->getsourcesreference().begin()->targets(actions))
				{
					outputActions[std::distance(&(*actions.cbegin()), actionPtr)].insert(Id);
				}
				++Id;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTnssmodel::getActionsTargets", __LINE__, __FILE__);
		}
		return outputActions;
	}
	/*std::vector<std::vector<const Core::FMTaction*>> FMTnssmodel::getactionstargets(const std::vector<const Core::FMToutput*>& alloutputs) const
		{
		std::vector<std::vector<const Core::FMTaction*>>outputactions;
		try {
			for (const Core::FMToutput* output : alloutputs)
			{
				outputactions.push_back(output->getsourcesreference().begin()->targets(actions));
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTnssmodel::getactionstargets", __LINE__, __FILE__);
		}
		return outputactions;
		}*/

	std::vector<Core::FMTdevelopmentpath> FMTnssmodel::operate(const Core::FMTactualdevelopment& development, const double& areatarget,const Core::FMTaction* target, Core::FMTschedule& schedule) const
	{
	std::vector<Core::FMTdevelopmentpath>paths;
	try {
			const size_t location = std::distance(&*actions.begin(), target);
			paths = development.operate(actions.at(location), transitions.at(location), yields, themes);
			schedule.addevent(development, areatarget, *target);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTnssmodel::operate", __LINE__, __FILE__);
			}
		return paths;
	}

	std::pair<size_t, const Core::FMTaction*> FMTnssmodel::getFirstOperable(const Core::FMTdevelopment& development,
		std::vector<std::vector<const Core::FMTaction*>> targets,
		const std::vector<const Core::FMToutput*>& alloutputs) const
	{
		std::pair<size_t, const Core::FMTaction*> result(0, nullptr);
		try {
				size_t location = 0;
				for (const Core::FMToutput* output : alloutputs)
				{
					if (output->getsourcesreference().begin()->use(development, yields))
					{
						std::vector<const Core::FMTaction*>::const_iterator actit = targets.at(location).begin();
						while (actit != targets.at(location).end() && !development.operable(**actit, yields))
						{
							++actit;
						}
						if (actit != targets.at(location).end())
						{
							if (result.second!=nullptr)
							{
								const size_t NEW_DISTANCE = std::distance(&(*actions.cbegin()), *actit);
								const size_t BASE_DISTANCE = std::distance(&(*actions.cbegin()), result.second);
								if (NEW_DISTANCE< BASE_DISTANCE)//Only the action that is at the top of the action section!
								{
									result = std::pair<size_t, const Core::FMTaction*>(location, *actit);
								}

							}else {
								result = std::pair<size_t, const Core::FMTaction*>(location, *actit);
							}
						}
					}
					++location;
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTnssmodel::getFirstOperable", __LINE__, __FILE__);
		}
	return result;
	}

	std::vector<std::pair<size_t, const Core::FMTaction*>> FMTnssmodel::getoperabilities(const Core::FMTdevelopment& development,
		std::vector<std::vector<const Core::FMTaction*>> targets,
		const std::vector<const Core::FMToutput*>& alloutputs) const
	{
		std::vector<std::pair<size_t, const Core::FMTaction*>>selection;
		try {
			size_t location = 0;
			for (const Core::FMToutput* output : alloutputs)
				{
				if (output->getsourcesreference().begin()->use(development,yields))
					{
					std::vector<const Core::FMTaction*>::const_iterator actit = targets.at(location).begin();
					while (actit!= targets.at(location).end() && !development.operable(**actit,yields))
						{
						++actit;
						}
					if (actit != targets.at(location).end())
						{
						selection.push_back(std::pair<size_t, const Core::FMTaction*>(location, *actit));
						}
					}
				++location;
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTnssmodel::getoperabilities", __LINE__, __FILE__);
			}
		return selection;
	}

	void FMTnssmodel::updatearea(std::vector<Core::FMTactualdevelopment>& basearea, std::vector<Core::FMTactualdevelopment>::iterator developmentit, const std::vector<Core::FMTdevelopmentpath>& paths, const double& operatedarea)
	{
		try {
			if (operatedarea>= developmentit->getarea())
				{
				basearea.erase(developmentit);
			}else {
				developmentit->setarea(developmentit->getarea() - operatedarea);
				}
			for (const Core::FMTdevelopmentpath& path : paths)
				{
				const double newareavalue = operatedarea * (path.proportion / 100);
				std::vector<Core::FMTactualdevelopment>::iterator restdev = std::find_if(basearea.begin(), basearea.end(), Core::FMTactualdevelopmentcomparator(&(*path.development)));
				if (restdev != basearea.end())
				{
					restdev->setarea(restdev->getarea() + newareavalue);
				}else {
					basearea.emplace_back(*path.development, newareavalue);
					}
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTnssmodel::updatearea", __LINE__, __FILE__);
			}
	}

	void FMTnssmodel::updateareatargets(const double& areaoperated, const size_t& outtarget,
		std::vector<const Core::FMToutput*>& alloutputs, std::vector<double>& targets,
		std::vector<std::vector<const Core::FMTaction*>>& actiontargets) const
	{
		try {
			if (targets.at(outtarget)<= areaoperated)
				{
				alloutputs.erase(alloutputs.begin()+ outtarget);
				targets.erase(targets.begin() + outtarget);
				actiontargets.erase(actiontargets.begin() + outtarget);
			}else {
				targets[outtarget] -= areaoperated;
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTnssmodel::updateareatargets", __LINE__, __FILE__);
			}
	}


	std::unique_ptr<FMTmodel>FMTnssmodel::clone() const
		{
		return std::unique_ptr<FMTmodel>(new FMTnssmodel(*this));
		}

	FMTnssmodel::FMTnssmodel(const FMTsrmodel& rhs, unsigned int seed) :
		FMTsrmodel(rhs),
		generator(seed)
	{

	}


	std::unique_ptr<FMTmodel>FMTnssmodel::presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
		{
		try {
			return std::unique_ptr<FMTmodel>(new FMTnssmodel(*(dynamic_cast<FMTsrmodel*>(FMTsrmodel::presolve(optionaldevelopments).get())),this->getparameter(FMTintmodelparameters::SEED)));
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTnssmodel::presolve", __LINE__, __FILE__);
			}
		return std::unique_ptr<FMTmodel>(nullptr);
		}

	std::unique_ptr<FMTmodel> FMTnssmodel::getcopy(int period) const
	{
		try {
			return std::unique_ptr<FMTmodel>(new FMTnssmodel(*FMTsrmodel::getcopy(period).get(),getparameter(FMTintmodelparameters::SEED)));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTnssmodel::getcopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
	}

	double FMTnssmodel::UpdateArea(const int& p_action, const double& p_devArea,
		std::vector<double>& p_targetedArea, std::vector<std::set<size_t>>& p_actionsoutputs)
	{
		double harvestedArea = 0;
		try {
			double areaToGet = 0;
			for (const size_t& OUTPUT_ID : p_actionsoutputs.at(p_action))
			{
				areaToGet += p_targetedArea.at(OUTPUT_ID);
			}
			harvestedArea = std::min(areaToGet, p_devArea);
			for (const size_t& OUTPUT_ID : p_actionsoutputs.at(p_action))
			{
				p_targetedArea.at(OUTPUT_ID) = std::max(p_targetedArea.at(OUTPUT_ID) - harvestedArea, 0.0);
				if (p_targetedArea.at(OUTPUT_ID) <= FMT_DBL_TOLERANCE)
				{
					for (std::set<size_t>& Ids : p_actionsoutputs)
					{
						Ids.erase(OUTPUT_ID);
					}
				}

			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTnssmodel::UpdateArea", __LINE__, __FILE__);
		}
		return harvestedArea;
	}

	void FMTnssmodel::simulate()
	{
		try {
			generator = std::default_random_engine(getparameter(Models::FMTintmodelparameters::SEED));
			//First make some noise
			std::shuffle(area.begin(), area.end(), generator);
			std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> actives = getActives();
			const int GRAPH_SIZE = getgraphsize();
			int period = static_cast<int>(GRAPH_SIZE -1);
			if (GRAPH_SIZE == 0)
			{
				period = area.begin()->getperiod() + 1;
			}
			std::vector<double>targetedArea;
			const std::vector<const Core::FMToutput*> TARGETED_OUTPUTS = constraintstotarget(targetedArea, period);
			std::vector<std::set<size_t>> actionsOutputs = getActionsTargets(TARGETED_OUTPUTS);
			if (targetedArea.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"No area to simulate at period " + std::to_string(period)+" for "+getname(), "FMTnssmodel::simulate", __LINE__, __FILE__);
			}

			if (targetedArea.size() != TARGETED_OUTPUTS.size() ||
				actionsOutputs.size() != actions.size())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Area target not the same size has output or actions target for " + getname(), "FMTnssmodel::simulate", __LINE__, __FILE__);
			}
			if (area.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Simulation model has no area to simulate for " + getname(), "FMTnssmodel::simulate", __LINE__, __FILE__);
			}
			graph.setbuildtype(Graph::FMTgraphbuild::schedulebuild);
			setparameter(Models::FMTintmodelparameters::MATRIX_TYPE, 3);
			bool allocatedArea = false;
			double totalOperatedArea = 0;
			Graph::FMTgraphstats GraphStats = getgraphstats();
			const double* ColSolution = solver.getColSolution();
			std::vector<double>newSolution(ColSolution, ColSolution+solver.getNumCols());
			int actionId = 0;
			for (const Core::FMTaction& ACTION : actions)
			{
				if (!actionsOutputs.at(actionId).empty())
				{
					std::queue< Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>newActives;
					while (!actives.empty())
					{
						bool doesNotGrow = false;
						Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor& frontVertex = actives.front();
						if (!actionsOutputs.at(actionId).empty())
						{
							const Core::FMTdevelopment& DEVELOPPEMENT = graph.getdevelopment(frontVertex);
							if (DEVELOPPEMENT.operable(ACTION, yields))
							{
								const double* actualSolution = &newSolution[0];
								double DEV_AREA = graph.inarea(frontVertex, actualSolution);
								for (const int& actionId : graph.getoutactions(frontVertex))
								{
									DEV_AREA -= graph.outarea(frontVertex, actionId, actualSolution);
								}
								if (ACTION.getname() == "_DEATH")
								{
									doesNotGrow = true;
								}
								const std::vector<Core::FMTdevelopmentpath> PATHS = DEVELOPPEMENT.operate(ACTION, transitions[actionId], yields, themes);
								graph.addaction(actionId, GraphStats, newActives, frontVertex, PATHS);
								const double OPERATED_AREA = UpdateArea(actionId, DEV_AREA, targetedArea, actionsOutputs);
								newSolution.push_back(OPERATED_AREA);
								totalOperatedArea += OPERATED_AREA;
							}
						}
						if (!doesNotGrow)
							{
							newActives.push(frontVertex);
							}
						actives.pop();
					}
					actives = newActives;
				}
				++actionId;
			}
			std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>toGrowWithSolution(actives);
			GraphStats = graph.naturalgrowth(actives, GraphStats, false);
			while (!toGrowWithSolution.empty())
			{
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor& GrowVertex = toGrowWithSolution.front();
				const double* actualSolution = &newSolution[0];
				double DEV_AREA = graph.inarea(GrowVertex, actualSolution);
				for (const int& Id : graph.getoutactions(GrowVertex))
				{
					DEV_AREA -= graph.outarea(GrowVertex, Id, actualSolution);
				}
				newSolution.push_back(DEV_AREA);
				toGrowWithSolution.pop();
			}
			
			const int location = static_cast<int>(graph.size() - 2);
			const Graph::FMTgraphstats newStats = this->updatematrix(graph.getperiodverticies(location), GraphStats);
			if (solver.getNumCols() != static_cast<int>(newSolution.size()))
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Solver cols of " + std::to_string(solver.getNumCols()) +
					" vs Solution cols of " + std::to_string(newSolution.size()),
					"FMTnssmodel::simulate", __LINE__, __FILE__);
			}
			graph.setstats(newStats);
			solver.setColSolution(&newSolution[0]);
			this->boundsolution(period);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTnssmodel::simulate", __LINE__, __FILE__);
		}
	}


	/*void FMTnssmodel::simulate()
	{
		try {
			//generator.seed(getparameter(Models::FMTintmodelparameters::SEED));
			generator = std::default_random_engine(getparameter(Models::FMTintmodelparameters::SEED));
			Core::FMTschedule schedule;
			schedule.setuselock(true);
			//schedule.passinobject(*this);
			const int actualgraphlength = static_cast<int>(getgraphsize());
			std::vector<Core::FMTactualdevelopment> actualarea;
			int simulatedperiod = (actualgraphlength-1);
			//First make some noise
			std::shuffle(area.begin(), area.end(), generator);
			
			if (actualgraphlength ==0)
			{
				actualarea = area;
				for (Core::FMTactualdevelopment& actdev : actualarea)
					{
					simulatedperiod =actdev.getperiod() + 1;
					actdev.setperiod(actdev.getperiod()+1);
					}
			}else {
				actualarea = getarea(actualgraphlength - 1);
				}
			std::vector<double>targetedarea;
			std::vector<const Core::FMToutput*> targetedoutputs = constraintstotarget(targetedarea, simulatedperiod);
			std::vector<std::vector<const Core::FMTaction*>> targetedactions = getactionstargets(targetedoutputs);
			schedule.setperiod(simulatedperiod);
			if (targetedarea.empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"No area to simulate at period "+std::to_string(simulatedperiod), "FMTnssmodel::simulate", __LINE__, __FILE__);
			}

			if (targetedarea.size()!= targetedoutputs.size() ||
				targetedactions.size()!= targetedarea.size())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Area target not the same size has output or actions target", "FMTnssmodel::simulate", __LINE__, __FILE__);
				}
			if (actualarea.empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Simulation model has no area to simulate", "FMTnssmodel::simulate", __LINE__, __FILE__);
				}
			//Shuffle the area...
			//std::shuffle(actualarea.begin(), actualarea.end(), generator);
			bool allocatedarea = true;
			bool anyallocation = false;
			double totaloperatedarea = 0;
			while (allocatedarea&&!targetedarea.empty())
				{
				std::vector<Core::FMTactualdevelopment>::iterator devit = actualarea.begin();
				std::vector<std::pair<size_t, const Core::FMTaction*>>operables;
				allocatedarea = false;
				
				while (devit!= actualarea.end()&&operables.empty())
					{
					operables = getoperabilities(*devit, targetedactions, targetedoutputs);
					
					if (operables.empty())
						{
						++devit;
						}
					}
				if (!operables.empty())
					{
					const double operatedarea = std::min(targetedarea.at(operables.begin()->first), devit->getarea());
					totaloperatedarea += operatedarea;
					const std::vector<Core::FMTdevelopmentpath> paths = operate(*devit, operatedarea, operables.begin()->second, schedule);
					updatearea(actualarea,devit, paths, operatedarea);
					updateareatargets(operatedarea, operables.begin()->first, targetedoutputs, targetedarea, targetedactions);
					allocatedarea = true;
					anyallocation = true;
					}
				}
			//Need to take care of the _DEATH!
			const Core::FMTaction& deathaction = actions.back();
			for (const Core::FMTactualdevelopment& developement : actualarea)
				{
				if (developement.operable(deathaction,yields))
					{
					schedule.addevent(developement, developement.getarea(),deathaction);
					}
				}				
			if (!anyallocation)
				{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"No area simulated at period " + std::to_string(simulatedperiod), "FMTnssmodel::simulate", __LINE__, __FILE__);
				}
			schedule.clean();
			this->buildperiod(schedule, true);
			this->setsolution(simulatedperiod,schedule);
			this->boundsolution(simulatedperiod);
			const double* ColSolution2 = solver.getColSolution();
			std::vector<double>newSolution2(ColSolution2, ColSolution2 + solver.getNumCols());
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTnssmodel::simulate", __LINE__, __FILE__);
		}
	}*/

	bool FMTnssmodel::build(std::vector<Core::FMTschedule> schedules)
	{
		bool simulationdone = false;
		try{
			if(!schedules.empty())
			{
				//warning
			}
			const int length = getparameter(LENGTH);
			for (int period = 0; period<length;++period)
			{
				simulate();
			}
			simulationdone = true;
		}catch(...)
		{
			_exhandler->raisefromcatch("", "FMTnssmodel::solve", __LINE__, __FILE__);
		}
		return simulationdone;
	}

	bool FMTnssmodel::setparameter(const FMTboolmodelparameters& key, const bool& value)
	{
		try {
			return FMTmodel::setparameter(key, value);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTnssmodel::setparameter", __LINE__, __FILE__);
			}
		return false;
	}

	bool FMTnssmodel::setparameter(const FMTintmodelparameters& key, const int& value)
	{
		bool parametersetted = false;
		try{
			FMTmodel::setparameter(key,value);
			if(key==SEED)
			{
				generator=std::default_random_engine(value);
				yields.setModel(this);
			}
			parametersetted=true;
		}catch(...)
		{
			_exhandler->raisefromcatch("", "FMTnssmodel::setparameter", __LINE__, __FILE__);
		}
		return parametersetted;
	}

	void FMTnssmodel::swap_ptr(const std::unique_ptr<FMTmodel>& rhs)
	{
		*this = std::move(*dynamic_cast<FMTnssmodel*>(rhs.get()));
	}
}

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTnssmodel)

#endif