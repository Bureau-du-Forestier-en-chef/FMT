/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTnssmodel.h"
#include "FMTfuturdevelopment.h"
#include "FMTactualdevelopment.h"
#include "FMTexceptionhandler.h"

namespace Models
{
	FMTnssmodel::FMTnssmodel():
		FMTsrmodel(FMTmodel(),
		#ifdef FMTWITHMOSEK
				Models::FMTsolverinterface::MOSEK
		#else
				Models::FMTsolverinterface::CLP
		#endif
			)
	{
	
	}

	FMTnssmodel::FMTnssmodel(const FMTnssmodel& rhs) :
		FMTsrmodel(rhs)
	{

	}

	FMTnssmodel::FMTnssmodel(const FMTmodel& rhs, unsigned int seed):
		FMTsrmodel(rhs,
		#ifdef FMTWITHMOSEK
					Models::FMTsolverinterface::MOSEK
		#else
					Models::FMTsolverinterface::CLP
		#endif
		)
	{
		FMTmodel::setparameter(SEED,seed);
		FMTmodel::setSeed(seed);
	}

	std::vector<const Core::FMToutput*> FMTnssmodel::constraintsToTarget(std::vector<double>& p_targets, const int& p_period)
		{
		std::vector<const Core::FMToutput*>targetedoutputs;
		try {
			p_targets.clear();
			std::vector<double>lowers;
			std::vector<double>uppers;
			for (const Core::FMTconstraint& constraint : constraints)
				{
				if (constraint.israndomaction() &&
					p_period>=constraint.getperiodlowerbound() &&
					p_period<=constraint.getperiodupperbound())
					{
				
					if (!constraint.dosupportrandom())
						{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Constraint "+std::string(constraint)+" does not support _RANDOM keyword", "FMTnssmodel::constraintsToTarget", __LINE__, __FILE__);
						}
					
					double lower = 0;
					double upper = 0;
					constraint.getbounds(lower, upper, p_period);
					Core::FMToutput inventOut(constraint);
					std::vector<Core::FMToutputsource> sources = inventOut.getsources();
					sources[0].settarget(Core::FMTotar::inventory);
					sources[0].setaction("");
					inventOut.setsources(sources);
					const double MAXIMAL_VALUE = getoutput(inventOut, getAreaPeriod(), Core::FMToutputlevel::totalonly).at("Total");
					upper = std::min(MAXIMAL_VALUE, upper);
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
					value = udist(m_generator);
					}
				
				p_targets.push_back(value);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTnssmodel::constraintsToTarget", __LINE__, __FILE__);
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

	bool FMTnssmodel::gotOutputForDev(const Core::FMTdevelopment& p_development,
		const std::vector<const Core::FMToutput*>& p_outputs,
		const std::set<size_t>& p_outputIds) const
	{
		try {
			for (const size_t& ID : p_outputIds)
				{
				if (p_outputs.at(ID)->getsourcesreference().begin()->use(p_development, yields))
					{
					return true;
					}
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTnssmodel::gotOutputForDev", __LINE__, __FILE__);
			}
		return false;
	}


	std::unique_ptr<FMTmodel>FMTnssmodel::clone() const
		{
		return std::unique_ptr<FMTmodel>(new FMTnssmodel(*this));
		}

	FMTnssmodel::FMTnssmodel(const FMTsrmodel& rhs, unsigned int seed) :
		FMTsrmodel(rhs)
	{
		FMTmodel::setSeed(seed);
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

	double FMTnssmodel::UpdateOutputs(const Core::FMTdevelopment& p_development, 
								    const std::vector<Core::FMTdevelopmentpath>& p_paths,
									const int& p_action, const double& p_devArea,
									std::vector<double>& p_targets, std::vector<std::set<size_t>>& p_actionsoutputs,
									const std::vector<const Core::FMToutput*>& p_allOutput) const
	{
		double harvestedArea = 0;
		try {
			double valueToGet = 0;
			double devValue = 0;
			std::vector<double>COEFFICIENTS(p_targets.size());
			double maxArea = 0;
			for (const size_t& OUTPUT_ID : p_actionsoutputs.at(p_action))
			{
				const Core::FMToutputsource& SOURCE = *p_allOutput.at(OUTPUT_ID)->getsourcesreference().begin();
				if (SOURCE.use(p_development, yields))//Only select the output for this dev.
				{
					const std::string& YIELD = SOURCE.getyield();
					double COEFFICIENT = 1;
					if (!YIELD.empty())
						{
						COEFFICIENT = p_development.getharvestcoef(p_paths, actions.at(p_action), yields, YIELD);
						}
					maxArea = std::max(p_targets.at(OUTPUT_ID) / COEFFICIENT, maxArea);
					COEFFICIENTS[OUTPUT_ID] = COEFFICIENT;
				}
			}
			harvestedArea =  std::min(maxArea, p_devArea);
			for (const size_t& OUTPUT_ID : p_actionsoutputs.at(p_action))
			{
				p_targets.at(OUTPUT_ID) = std::max(p_targets.at(OUTPUT_ID) - harvestedArea * COEFFICIENTS.at(OUTPUT_ID), 0.0);
				if (p_targets.at(OUTPUT_ID) <= FMT_DBL_TOLERANCE)
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
			m_generator = std::default_random_engine(getparameter(Models::FMTintmodelparameters::SEED));
			//First make some noise
			std::shuffle(area.begin(), area.end(), m_generator);
			m_graph->setbuildtype(Graph::FMTgraphbuild::schedulebuild);
			std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> actives = getActives();
			const int GRAPH_SIZE = static_cast<int>(getgraphsize());
			int period = static_cast<int>(GRAPH_SIZE -1);
			if (GRAPH_SIZE == 0)
			{
				period = getAreaPeriod() + 1;
			}
			std::vector<double>targetedValues;
			const std::vector<const Core::FMToutput*> TARGETED_OUTPUTS = constraintsToTarget(targetedValues, period);
			std::vector<std::set<size_t>> actionsOutputs = getActionsTargets(TARGETED_OUTPUTS);
			if (targetedValues.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"No area to simulate at period " + std::to_string(period)+" for "+getname(), "FMTnssmodel::simulate", __LINE__, __FILE__);
			}

			if (targetedValues.size() != TARGETED_OUTPUTS.size() ||
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
			setparameter(Models::FMTintmodelparameters::MATRIX_TYPE, 3);
			bool allocatedArea = false;
			double totalOperatedArea = 0;
			Graph::FMTgraphstats GraphStats = getgraphstats();
			const double* ColSolution = solver.getColSolution();
			std::vector<double>newSolution(ColSolution, ColSolution+solver.getNumCols());
			int actionId = 0;
			for (const Core::FMTaction& ACTION : actions)
			{
				const bool DOES_NOT_GROW = (ACTION.getname() == "_DEATH");
				std::queue< Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>toGrow;
				const size_t MAX_SPIN = 10; //area.size() * 2;
				size_t visit = 0;
				while (!actionsOutputs.at(actionId).empty() && visit < MAX_SPIN)//Keep on spinning if you havent reach the target?
				{
					std::queue< Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>revisitedActives;
					while (!actives.empty())
					{
						Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor& frontVertex = actives.front();
						bool inOutput = false;
						if (!actionsOutputs.at(actionId).empty())
						{
							const Core::FMTdevelopment& DEVELOPPEMENT = m_graph->getdevelopment(frontVertex);
							const bool GOT_OUTPUT = gotOutputForDev(DEVELOPPEMENT, TARGETED_OUTPUTS, actionsOutputs.at(actionId));
							if (GOT_OUTPUT)
							{
								inOutput = true;
								if (DEVELOPPEMENT.operable(ACTION, yields))
									{
									const double* actualSolution = &newSolution[0];
									double DEV_AREA = m_graph->inarea(frontVertex, actualSolution);
									for (const int& actionId : m_graph->getoutactions(frontVertex))
										{
										DEV_AREA -= m_graph->outarea(frontVertex, actionId, actualSolution);
										}
									const std::vector<Core::FMTdevelopmentpath> PATHS = DEVELOPPEMENT.operate(ACTION, transitions[actionId], yields, themes);
									m_graph->addaction(actionId, GraphStats, toGrow, frontVertex, PATHS);
									const double OPERATED_AREA = UpdateOutputs(DEVELOPPEMENT, PATHS, actionId, DEV_AREA,
										targetedValues, actionsOutputs,
										TARGETED_OUTPUTS);
									newSolution.push_back(OPERATED_AREA);
									totalOperatedArea += OPERATED_AREA;
									if (!DOES_NOT_GROW)
										{
										toGrow.push(frontVertex);
										}
									}else if (!DOES_NOT_GROW)
										{
										revisitedActives.push(frontVertex);
										}
							}
						}
						if (!inOutput && !DOES_NOT_GROW)
							{
							toGrow.push(frontVertex);
							}
						actives.pop();
					}
					actives = revisitedActives;
					++visit;
				}
				if (visit == MAX_SPIN)
				{
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"For action "+ ACTION.getname()+" at period " + std::to_string(period) + " reached max spin ",
						"FMTnssmodel::simulate", __LINE__, __FILE__);
				}
				while (!toGrow.empty())
					{
					actives.push(toGrow.front());
					toGrow.pop();
					}
				++actionId;
			}
			std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>toGrowWithSolution(actives);
			GraphStats = m_graph->naturalgrowth(actives, GraphStats, false);
			while (!toGrowWithSolution.empty())
			{
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor& GrowVertex = toGrowWithSolution.front();
				const double* actualSolution = &newSolution[0];
				double DEV_AREA = m_graph->inarea(GrowVertex, actualSolution);
				for (const int& Id : m_graph->getoutactions(GrowVertex))
				{
					DEV_AREA -= m_graph->outarea(GrowVertex, Id, actualSolution);
				}
				newSolution.push_back(DEV_AREA);
				toGrowWithSolution.pop();
			}
			
			const int location = static_cast<int>(m_graph->size() - 2);
			const Graph::FMTgraphstats newStats = this->updatematrix(m_graph->getperiodverticies(location), GraphStats);
			if (solver.getNumCols() != static_cast<int>(newSolution.size()))
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Solver cols of " + std::to_string(solver.getNumCols()) +
					" vs Solution cols of " + std::to_string(newSolution.size()),
					"FMTnssmodel::simulate", __LINE__, __FILE__);
			}
			m_graph->setstats(newStats);
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
			const size_t LENGTH = static_cast<size_t>(getparameter(FMTintmodelparameters::LENGTH));
			const size_t AREA = area.size();
			const size_t ACTIONS = actions.size();
			const size_t EXPO_FACTOR = 12;
			m_graph->reserveVerticies(LENGTH * AREA * ACTIONS * EXPO_FACTOR);
			for (int period = 0; period< getparameter(FMTintmodelparameters::LENGTH);++period)
			{
				simulate();
			}
			simulationdone = true;
		}catch(...)
		{
			_exhandler->raisefromcatch(getname(), "FMTnssmodel::build", __LINE__, __FILE__);
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
				m_generator=std::default_random_engine(value);
				yields.setModel(this);
			}
			parametersetted=true;
		}catch(...)
		{
			_exhandler->raisefromcatch("", "FMTnssmodel::setparameter", __LINE__, __FILE__);
		}
		return parametersetted;
	}

	void FMTnssmodel::swap_ptr(std::unique_ptr<FMTmodel>& rhs)
	{
		*this = std::move(*dynamic_cast<FMTnssmodel*>(rhs.get()));
	}
}

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTnssmodel)

#endif