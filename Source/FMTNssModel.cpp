/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTNssModel.h"
#include "FMTFuturDevelopment.h"
#include "FMTActualDevelopment.h"
#include "FMTExceptionHandler.h"

namespace Models
{
	FMTNssModel::FMTNssModel():
		FMTSrModel(FMTModel(),
		#ifdef FMTWITHMOSEK
				Models::FMTsolverinterface::MOSEK
		#else
				Models::FMTsolverinterface::CLP
		#endif
			)
	{
	
	}

	FMTNssModel::FMTNssModel(const FMTNssModel& rhs) :
		FMTSrModel(rhs)
	{

	}

	FMTNssModel::FMTNssModel(const FMTModel& rhs, unsigned int seed):
		FMTSrModel(rhs,
		#ifdef FMTWITHMOSEK
					Models::FMTsolverinterface::MOSEK
		#else
					Models::FMTsolverinterface::CLP
		#endif
		)
	{
		FMTModel::setParameter(SEED,seed);
		FMTModel::setSeed(seed);
	}

	std::vector<const Core::FMTOutput*> FMTNssModel::constraintsToTarget(std::vector<double>& p_targets, const int& p_period)
		{
		std::vector<const Core::FMTOutput*>targetedoutputs;
		try {
			p_targets.clear();
			std::vector<double>lowers;
			std::vector<double>uppers;
			for (const Core::FMTConstraint& constraint : constraints)
				{
				if (constraint.isRandomAction() &&
					p_period>=constraint.getPeriodLowerBound() &&
					p_period<=constraint.getPeriodUpperBound())
					{
				
					if (!constraint.doSupportRandom())
						{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Constraint "+std::string(constraint)+" does not support _RANDOM keyword", "FMTNssModel::constraintsToTarget", __LINE__, __FILE__);
						}
					
					double lower = 0;
					double upper = 0;
					constraint.getBounds(lower, upper, p_period);
					Core::FMTOutput inventOut(constraint);
					std::vector<Core::FMTOutputSource> sources = inventOut.getSources();
					sources[0].setTarget(Core::FMTotar::inventory);
					sources[0].setAction("");
					inventOut.setSources(sources);
					const double MAXIMAL_VALUE = getOutput(inventOut, getAreaPeriod(), Core::FMToutputlevel::totalonly).at("Total");
					upper = std::min(MAXIMAL_VALUE, upper);
					size_t location = 0;
					bool added = false;
					for (const Core::FMTOutput* doneit : targetedoutputs)
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
				if (uppers.at(outid)!= std::numeric_limits<double>::infinity()&&
					(uppers.at(outid)-lowerbound) > FMT_DBL_TOLERANCE)
					{
					const double upperbound = uppers.at(outid);
					std::uniform_real_distribution<double>udist(lowerbound, upperbound);
					value = udist(m_generator);
					}
				
				p_targets.push_back(value);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTNssModel::constraintsToTarget", __LINE__, __FILE__);
			}
		return targetedoutputs;
		}

	std::vector<std::set<size_t>> FMTNssModel::getActionsTargets(const std::vector<const Core::FMTOutput*>& p_allOutputs) const
		{
		std::vector<std::set<size_t>>outputActions(actions.size());
		try {
			size_t Id = 0;
			for (const Core::FMTOutput* output : p_allOutputs)
			{
				for (const Core::FMTAction* actionPtr : output->getSourcesReference().begin()->targets(actions))
				{
					outputActions[std::distance(&(*actions.cbegin()), actionPtr)].insert(Id);
				}
				++Id;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTNssModel::getActionsTargets", __LINE__, __FILE__);
		}
		return outputActions;
	}


	std::pair<size_t, const Core::FMTAction*> FMTNssModel::getFirstOperable(const Core::FMTDevelopment& development,
		std::vector<std::vector<const Core::FMTAction*>> targets,
		const std::vector<const Core::FMTOutput*>& alloutputs) const
	{
		std::pair<size_t, const Core::FMTAction*> result(0, nullptr);
		try {
				size_t location = 0;
				for (const Core::FMTOutput* output : alloutputs)
				{
					if (output->getSourcesReference().begin()->use(development, yields))
					{
						std::vector<const Core::FMTAction*>::const_iterator actit = targets.at(location).begin();
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
									result = std::pair<size_t, const Core::FMTAction*>(location, *actit);
								}

							}else {
								result = std::pair<size_t, const Core::FMTAction*>(location, *actit);
							}
						}
					}
					++location;
				}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTNssModel::getFirstOperable", __LINE__, __FILE__);
		}
	return result;
	}

	bool FMTNssModel::gotOutputForDev(const Core::FMTDevelopment& p_development,
		const std::vector<const Core::FMTOutput*>& p_outputs,
		const std::set<size_t>& p_outputIds) const
	{
		try {
			for (const size_t& ID : p_outputIds)
				{
				if (p_outputs.at(ID)->getSourcesReference().begin()->use(p_development, yields))
					{
					return true;
					}
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTNssModel::gotOutputForDev", __LINE__, __FILE__);
			}
		return false;
	}


	std::unique_ptr<FMTModel>FMTNssModel::clone() const
		{
		return std::unique_ptr<FMTModel>(new FMTNssModel(*this));
		}

	FMTNssModel::FMTNssModel(const FMTSrModel& rhs, unsigned int seed) :
		FMTSrModel(rhs)
	{
		FMTModel::setSeed(seed);
	}


	std::unique_ptr<FMTModel>FMTNssModel::presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments) const
		{
		try {
			return std::unique_ptr<FMTModel>(new FMTNssModel(*(dynamic_cast<FMTSrModel*>(FMTSrModel::presolve(optionaldevelopments).get())),this->getParameter(FMTintmodelparameters::SEED)));
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTNssModel::presolve", __LINE__, __FILE__);
			}
		return std::unique_ptr<FMTModel>(nullptr);
		}

	std::unique_ptr<FMTModel> FMTNssModel::getCopy(int period) const
	{
		try {
			return std::unique_ptr<FMTModel>(new FMTNssModel(*FMTSrModel::getCopy(period).get(),getParameter(FMTintmodelparameters::SEED)));
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTNssModel::getCopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTModel>(nullptr);
	}

	double FMTNssModel::updateOutputs(const Core::FMTDevelopment& p_development, 
								    const std::vector<Core::FMTDevelopmentPath>& p_paths,
									const int& p_action, const double& p_devArea,
									std::vector<double>& p_targets, std::vector<std::set<size_t>>& p_actionsoutputs,
									const std::vector<const Core::FMTOutput*>& p_allOutput) const
	{
		double harvestedArea = 0;
		try {
			double valueToGet = 0;
			double devValue = 0;
			std::vector<double>COEFFICIENTS(p_targets.size());
			double maxArea = 0;
			for (const size_t& OUTPUT_ID : p_actionsoutputs.at(p_action))
			{
				const Core::FMTOutputSource& SOURCE = *p_allOutput.at(OUTPUT_ID)->getSourcesReference().begin();
				if (SOURCE.use(p_development, yields))//Only select the output for this dev.
				{
					const std::string& YIELD = SOURCE.getYield();
					double COEFFICIENT = 1;
					if (!YIELD.empty())
						{
						COEFFICIENT = p_development.getHarvestCoef(p_paths, actions.at(p_action), yields, YIELD);
						}
					maxArea = std::max(p_targets.at(OUTPUT_ID) / COEFFICIENT, maxArea);
					COEFFICIENTS[OUTPUT_ID] = COEFFICIENT;
				}
			}
			harvestedArea =  std::min(maxArea, p_devArea);
			std::vector<size_t>IdsToRemove;
			IdsToRemove.reserve(p_targets.size());
			for (const size_t& OUTPUT_ID : p_actionsoutputs.at(p_action))
			{
				p_targets.at(OUTPUT_ID) = std::max(p_targets.at(OUTPUT_ID) - harvestedArea * COEFFICIENTS.at(OUTPUT_ID), 0.0);
				if (p_targets.at(OUTPUT_ID) <= FMT_DBL_TOLERANCE)
				{
					IdsToRemove.push_back(OUTPUT_ID);
				}
			}
			for (std::set<size_t>& Ids : p_actionsoutputs)
				{
				for (const size_t& TO_REMOVE : IdsToRemove)
					{
					Ids.erase(TO_REMOVE);
					}
				}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTNssModel::updateOutputs", __LINE__, __FILE__);
		}
		return harvestedArea;
	}

	void FMTNssModel::simulate()
	{
		try {
			m_generator = std::default_random_engine(getParameter(Models::FMTintmodelparameters::SEED));
			//First make some noise
			std::shuffle(area.begin(), area.end(), m_generator);
			m_graph->setBuildType(Graph::FMTgraphbuild::schedulebuild);
			std::queue<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor> actives = getActives();
			const int GRAPH_SIZE = static_cast<int>(getGraphSize());
			int period = static_cast<int>(GRAPH_SIZE -1);
			if (GRAPH_SIZE == 0)
			{
				period = getAreaPeriod() + 1;
			}
			std::vector<double>targetedValues;
			const std::vector<const Core::FMTOutput*> TARGETED_OUTPUTS = constraintsToTarget(targetedValues, period);
			std::vector<std::set<size_t>> actionsOutputs = getActionsTargets(TARGETED_OUTPUTS);
			if (targetedValues.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"No area to simulate at period " + std::to_string(period)+" for "+getName(), "FMTNssModel::simulate", __LINE__, __FILE__);
			}

			if (targetedValues.size() != TARGETED_OUTPUTS.size() ||
				actionsOutputs.size() != actions.size())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Area target not the same size has output or actions target for " + getName(), "FMTNssModel::simulate", __LINE__, __FILE__);
			}
			if (area.empty())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Simulation model has no area to simulate for " + getName(), "FMTNssModel::simulate", __LINE__, __FILE__);
			}
			setParameter(Models::FMTintmodelparameters::MATRIX_TYPE, 3);
			bool allocatedArea = false;
			double totalOperatedArea = 0;
			Graph::FMTGraphStats GraphStats = getStats();
			const double* ColSolution = solver.getColSolution();
			std::vector<double>newSolution(ColSolution, ColSolution+solver.getNumCols());
			int actionId = 0;
			for (const Core::FMTAction& ACTION : actions)
			{
				const bool DOES_NOT_GROW = (ACTION.getName() == "_DEATH");
				std::queue< Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>toGrow;
				const size_t MAX_SPIN = 10; //area.size() * 2;
				size_t visit = 0;
				while (!actionsOutputs.at(actionId).empty() && visit < MAX_SPIN)//Keep on spinning if you havent reach the target?
				{
					std::queue< Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>revisitedActives;
					while (!actives.empty())
					{
						Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor& frontVertex = actives.front();
						bool inOutput = false;
						if (!actionsOutputs.at(actionId).empty())
						{
							const Core::FMTDevelopment& DEVELOPPEMENT = m_graph->getDevelopment(frontVertex);
							const bool GOT_OUTPUT = gotOutputForDev(DEVELOPPEMENT, TARGETED_OUTPUTS, actionsOutputs.at(actionId));
							if (GOT_OUTPUT)
							{
								inOutput = true;
								if (DEVELOPPEMENT.operable(ACTION, yields))
								{
									const double* actualSolution = &newSolution[0];
									double DEV_AREA = m_graph->inArea(frontVertex, actualSolution);
									for (const int& actionId : m_graph->getOutActions(frontVertex, false))
										{
										DEV_AREA -= m_graph->outArea(frontVertex, actionId, actualSolution);
										}
									if (DEV_AREA > FMT_DBL_TOLERANCE)
									{
										const std::vector<Core::FMTDevelopmentPath> PATHS = DEVELOPPEMENT.operate(ACTION, transitions[actionId], yields, themes);
										m_graph->addAction(actionId, GraphStats, toGrow, frontVertex, PATHS);
										const double OPERATED_AREA = updateOutputs(DEVELOPPEMENT, PATHS, actionId, DEV_AREA,
											targetedValues, actionsOutputs,
											TARGETED_OUTPUTS);
										newSolution.push_back(OPERATED_AREA);
										totalOperatedArea += OPERATED_AREA;
									}
									if (!DOES_NOT_GROW)
										{
										toGrow.push(frontVertex);
										}
								}
								else if (!DOES_NOT_GROW)
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
				if (!actionsOutputs.at(actionId).empty())
					{
					yields.clearRandomYieldsCache();
					}
				}
				if (visit == MAX_SPIN)
				{
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"For action "+ ACTION.getName()+" at period " + std::to_string(period) + " reached max spin ",
						"FMTNssModel::simulate", __LINE__, __FILE__);
				}
				while (!toGrow.empty())
					{
					actives.push(toGrow.front());
					toGrow.pop();
					}
				++actionId;
			}
			std::queue<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>toGrowWithSolution(actives);
			GraphStats = m_graph->naturalGrowth(actives, GraphStats, false);
			while (!toGrowWithSolution.empty())
			{
				Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor& GrowVertex = toGrowWithSolution.front();
				const double* actualSolution = &newSolution[0];
				double DEV_AREA = m_graph->inArea(GrowVertex, actualSolution);
				for (const int& Id : m_graph->getOutActions(GrowVertex))
				{
					DEV_AREA -= m_graph->outArea(GrowVertex, Id, actualSolution);
				}
				newSolution.push_back(DEV_AREA);
				toGrowWithSolution.pop();
			}
			const int location = static_cast<int>(m_graph->size() - 2);
			const Graph::FMTGraphStats newStats = this->updateMatrix(m_graph->getPeriodVertices(location), GraphStats);
			if (solver.getNumCols() != static_cast<int>(newSolution.size()))
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Solver cols of " + std::to_string(solver.getNumCols()) +
					" vs Solution cols of " + std::to_string(newSolution.size()),
					"FMTNssModel::simulate", __LINE__, __FILE__);
			}
			m_graph->setStats(newStats);
			solver.setColSolution(&newSolution[0]);
			this->boundSolution(period);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTNssModel::simulate", __LINE__, __FILE__);
		}
	}


	/*void FMTNssModel::simulate()
	{
		try {
			//generator.seed(getparameter(Models::FMTintmodelparameters::SEED));
			generator = std::default_random_engine(getparameter(Models::FMTintmodelparameters::SEED));
			Core::FMTSchedule schedule;
			schedule.setUseLock(true);
			//schedule.passinobject(*this);
			const int actualgraphlength = static_cast<int>(getGraphSize());
			std::vector<Core::FMTActualDevelopment> actualarea;
			int simulatedperiod = (actualgraphlength-1);
			//First make some noise
			std::shuffle(area.begin(), area.end(), generator);
			
			if (actualgraphlength ==0)
			{
				actualarea = area;
				for (Core::FMTActualDevelopment& actdev : actualarea)
					{
					simulatedperiod =actdev.getPeriod() + 1;
					actdev.setPeriod(actdev.getPeriod()+1);
					}
			}else {
				actualarea = getarea(actualgraphlength - 1);
				}
			std::vector<double>targetedarea;
			std::vector<const Core::FMTOutput*> targetedoutputs = constraintstotarget(targetedarea, simulatedperiod);
			std::vector<std::vector<const Core::FMTAction*>> targetedactions = getactionstargets(targetedoutputs);
			schedule.setPeriod(simulatedperiod);
			if (targetedarea.empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"No area to simulate at period "+std::to_string(simulatedperiod), "FMTNssModel::simulate", __LINE__, __FILE__);
			}

			if (targetedarea.size()!= targetedoutputs.size() ||
				targetedactions.size()!= targetedarea.size())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Area target not the same size has output or actions target", "FMTNssModel::simulate", __LINE__, __FILE__);
				}
			if (actualarea.empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Simulation model has no area to simulate", "FMTNssModel::simulate", __LINE__, __FILE__);
				}
			//Shuffle the area...
			//std::shuffle(actualarea.begin(), actualarea.end(), generator);
			bool allocatedarea = true;
			bool anyallocation = false;
			double totaloperatedarea = 0;
			while (allocatedarea&&!targetedarea.empty())
				{
				std::vector<Core::FMTActualDevelopment>::iterator devit = actualarea.begin();
				std::vector<std::pair<size_t, const Core::FMTAction*>>operables;
				allocatedarea = false;
				
				while (devit!= actualarea.end()&&operables.empty())
					{
					operables = getOperabilities(*devit, targetedactions, targetedoutputs);
					
					if (operables.empty())
						{
						++devit;
						}
					}
				if (!operables.empty())
					{
					const double operatedarea = std::min(targetedarea.at(operables.begin()->first), devit->getarea());
					totaloperatedarea += operatedarea;
					const std::vector<Core::FMTDevelopmentPath> paths = operate(*devit, operatedarea, operables.begin()->second, schedule);
					updatearea(actualarea,devit, paths, operatedarea);
					updateareatargets(operatedarea, operables.begin()->first, targetedoutputs, targetedarea, targetedactions);
					allocatedarea = true;
					anyallocation = true;
					}
				}
			//Need to take care of the _DEATH!
			const Core::FMTAction& deathaction = actions.back();
			for (const Core::FMTActualDevelopment& development : actualarea)
				{
				if (development.operable(deathaction,yields))
					{
					schedule.addEvent(development, development.getarea(),deathaction);
					}
				}				
			if (!anyallocation)
				{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"No area simulated at period " + std::to_string(simulatedperiod), "FMTNssModel::simulate", __LINE__, __FILE__);
				}
			schedule.clean();
			this->buildperiod(schedule, true);
			this->setSolution(simulatedperiod,schedule);
			this->boundSolution(simulatedperiod);
			const double* ColSolution2 = solver.getColSolution();
			std::vector<double>newSolution2(ColSolution2, ColSolution2 + solver.getNumCols());
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTNssModel::simulate", __LINE__, __FILE__);
		}
	}*/

	bool FMTNssModel::build(std::vector<Core::FMTSchedule> schedules)
	{
		bool simulationdone = false;
		try{
			if(!schedules.empty())
			{
				//warning
			}
			const size_t LENGTH = static_cast<size_t>(getParameter(FMTintmodelparameters::LENGTH));
			const size_t AREA = area.size();
			const size_t ACTIONS = actions.size();
			const size_t EXPO_FACTOR = 12;
			m_graph->reserveVertices(LENGTH * AREA * ACTIONS * EXPO_FACTOR);
			for (int period = 0; period< getParameter(FMTintmodelparameters::LENGTH);++period)
			{
				simulate();
			}
			simulationdone = true;
		}catch(...)
		{
			_exhandler->raiseFromCatch(getName(), "FMTNssModel::build", __LINE__, __FILE__);
		}
		return simulationdone;
	}

	bool FMTNssModel::setParameter(const FMTboolmodelparameters& key, const bool& value)
	{
		try {
			return FMTModel::setParameter(key, value);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTNssModel::setParameter", __LINE__, __FILE__);
			}
		return false;
	}

	bool FMTNssModel::setParameter(const FMTintmodelparameters& key, const int& value)
	{
		bool parametersetted = false;
		try{
			FMTModel::setParameter(key,value);
			if(key==SEED)
			{
				m_generator=std::default_random_engine(value);
				yields.setModel(this);
			}
			parametersetted=true;
		}catch(...)
		{
			_exhandler->raiseFromCatch("", "FMTNssModel::setParameter", __LINE__, __FILE__);
		}
		return parametersetted;
	}

	void FMTNssModel::swapPtr(std::unique_ptr<FMTModel>& rhs)
	{
		*this = std::move(*dynamic_cast<FMTNssModel*>(rhs.get()));
	}
}

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTNssModel)

#endif