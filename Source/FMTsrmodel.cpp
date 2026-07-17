/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTsrmodel.h"
#include <unordered_map>
#include <map>
#include <set>
#include<algorithm>
#include <utility>
#include <vector>
#include <queue>
#include "FMTexceptionhandler.h"
#include "FMTgraphvertextoyield.h"
#include "FMTSerie.h"

#if defined FMTWITHR
	#include <Rcpp.h>
#endif 

namespace Models
{

	void FMTSrModel::setNodeCacheSize(const size_t& p_size)
	{
		_logger->logWithLevel("Node Cache size for " + getName() + " of " + std::to_string(p_size) + "\n", 1);
		m_graph->setNodeSize(p_size);
	}

	Graph::FMTGraphVertexToYield FMTSrModel::getGraphVertexToYield() const
	{
		try {
			return Graph::FMTGraphVertexToYield(*this,*m_graph,nullptr);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSrModel::getGraphVertexToYield", __LINE__, __FILE__);
			}
		return Graph::FMTGraphVertexToYield();
	}


	Graph::FMTGraphStats FMTSrModel::initializeMatrix()
	{
		Graph::FMTGraphStats stats;
		try {
			const int ncols = static_cast<int>(m_graph->nEdges());
			const std::vector<int>column_Starts(static_cast<size_t>(ncols) + 1, 0);
			const std::vector<int>targetrows(static_cast<size_t>(ncols), 0);
			const std::vector<double>nelements(ncols, 0.0);
			std::vector<double>lower_bounds(ncols, 0.0);
			std::vector<double>upper_bounds(ncols, 0.0);
			m_graph->getInitialBounds(lower_bounds, upper_bounds);
			solver.addCols(ncols, &column_Starts[0], &targetrows[0],
				&nelements[0], &lower_bounds[0],
				&upper_bounds[0], &nelements[0]);
			solver.setColSolution(&lower_bounds[0]);
			stats = m_graph->getStats();
			stats.cols = ncols;
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSrModel::initializeMatrix", __LINE__, __FILE__);
		}
		return stats;
	}

	bool FMTSrModel::isPeriodBounded(int period) const
	{
		try {
			if (static_cast<int>(m_graph->size()) > period)
			{
				size_t totaledges = 0;
				size_t boundededges = 0;
				const double* columnsupperbounds = solver.getColUpper();
				const double* columnslowerbounds = solver.getColLower();
				Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_iterator it, itend;
				for (boost::tie(it, itend) = m_graph->getPeriodVertices(period); it != itend; ++it)
				{
					const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor& vertex_descriptor = *it;
					const std::map<int, int>variables = m_graph->getOutVariables(vertex_descriptor);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						if (*(columnsupperbounds + varit->second) == *(columnslowerbounds + varit->second))
						{
							++boundededges;
							++totaledges;
						}
					}
				}
				if (boundededges == totaledges)
				{
					return true;
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("at period " + std::to_string(period),
				"FMTSrModel::isPeriodBounded", __LINE__, __FILE__);
		}
		return false;
	}

	bool FMTSrModel::unboundSolution(int period)
	{
		try {
			if (static_cast<int>(m_graph->size()) > period && period > 0)//period >0 to not select actual developments!
			{
				std::vector<int>variable_index;
				std::vector<double>bounds;
				Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_iterator it, itend;
				for (boost::tie(it, itend) = m_graph->getPeriodVertices(period); it != itend; ++it)
				{
					const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor vertex_descriptor = *it;
					std::map<int, int>variables = m_graph->getOutVariables(vertex_descriptor);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						if (std::find(variable_index.begin(), variable_index.end(), varit->second) == variable_index.end())
						{
							variable_index.push_back(varit->second);
							bounds.push_back(-std::numeric_limits< double >::max());
							bounds.push_back(std::numeric_limits< double >::max());
						}
					}
				}
				solver.setColSetBounds(&variable_index[0], &variable_index.back() + 1, &bounds[0]);
				return true;
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("at period " + std::to_string(period), "FMTSrModel::unboundSolution", __LINE__, __FILE__);
		}


		return false;
	}

	std::unique_ptr<FMTModel>FMTSrModel::getCopy(int period) const
	{
		try {
			return std::unique_ptr<FMTModel>(new FMTSrModel(*this, solver.getSolverType()));
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::getCopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTModel>(nullptr);
	}


	bool FMTSrModel::forceSolution(int period, const Core::FMTSchedule& proportionschedulewithlock)
	{
		try
		{
			const double tolerance = getParameter(FMTdblmodelparameters::TOLERANCE);
			if(!proportionschedulewithlock.doUseLock())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
												"This function can only be used with schedules using lock", 
												"FMTSrModel::forceSolution", __LINE__, __FILE__);
			}
			if (static_cast<int>(m_graph->size()) > period && period > 0)
			{
				std::vector<Core::FMTAction>::const_iterator cit = std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator("_DEATH"));
				const int deathid = static_cast<int>(std::distance(actions.cbegin(), cit));
				const double* actual_solution = solver.getColSolution();
				const boost::unordered_set<Core::FMTLookup<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor, Core::FMTDevelopment>> lookup = m_graph->getDevsSet(period);
				//Copy de la solution donc les p�riodes pr�c�dents car c'est elle qu'on veut... Le reste on le scrap ?!
				std::vector<double>new_solution(actual_solution, actual_solution + solver.getNumCols());
				//Contient la proportion d'area qui rentre dans le vertex qui doit resortir dans la variable... Donc 200 ha dans le vertex rentre.. 222,0.1 il y a 20 ha qui ressort dans la variable 222
				boost::unordered_map<int,double> varproportions;
				Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				const bool typeII = (getParameter(Models::FMTintmodelparameters::MATRIX_TYPE) == 2);
				for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getPeriodVertices(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					if (typeII&&m_graph->isNoTransfer(*vertex_iterator, 1))//Dont need to set to 0 global growth variable.
						{
						continue;
						}
					const std::map<int, int>variables = m_graph->getOutVariables(*vertex_iterator);

					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						new_solution[varit->second] = 0;
					}
				}
				//Getoutvariables proportions for schedule...
				//Container pour deduire le growht � partir de ce qui est plac� dans les actions par dev
				std::map<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor,double> growthordeathdeductor;
				for (int actionid = 0; actionid < static_cast<int>(actions.size()); ++actionid)
				{
					const auto& actionit = proportionschedulewithlock.find(actions.at(actionid));
					if (actionit != proportionschedulewithlock.end())
					{
						for (const auto& devit : actionit->second)
						{
							if(m_graph->containsDevelopment(devit.first, lookup))
							{
								const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor vdescriptor = m_graph->getDevelopment(devit.first, lookup);
								if(growthordeathdeductor.find(vdescriptor)==growthordeathdeductor.end())
								{
									growthordeathdeductor.emplace(vdescriptor,1-devit.second.at(0));
								}else{
									growthordeathdeductor[vdescriptor]-=devit.second.at(0);
								}
								std::map<int, int> outvariables = m_graph->getOutVariables(vdescriptor);
								std::map<int, int>::const_iterator varit = outvariables.find(actionid);
								if (varit == outvariables.cend())
								{
									_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
												"Developement " + std::string(devit.first) + " is not operable "
												" for action " + actionit->first.getName(), 
												"FMTSrModel::forceSolution", __LINE__, __FILE__);
								}
								//std::cout<<std::string(devit.first)<<" "+std::to_string(devit.second.at(0))<<" "+std::to_string(actionid)<<std::endl;//" "+this->getactions().at(actionid).getName()<<std::endl;
								varproportions.emplace(varit->second,devit.second.at(0));
								
							}
							else{
								//message de warning pour les d�velopement initiaux qui n'existe plus ppour la schedule.
								//std::cout<<std::string(devit.first)<<" "+std::to_string(devit.second.at(0))<<" missing"<<std::endl;
							}
						}
					}
				}
				for(const auto& vdescriptor_props : growthordeathdeductor)
				{
					const std::map<int, int> outvariables = m_graph->getOutVariables(vdescriptor_props.first);
					std::map<int, int>::const_iterator varit = outvariables.find(-1);
					if (varit == outvariables.cend())
					{
						varit = outvariables.find(deathid);
						if(varit == outvariables.cend())
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
									"Developement "+std::string(m_graph->getDevelopment(vdescriptor_props.first))+" cannot grow or die ...", 
									"FMTSrModel::forceSolution", __LINE__, __FILE__);
						}
					}
					//Pas besoin de setter de growth ou death a 0%
					if(vdescriptor_props.second>0)
					{
						//std::cout<<vdescriptor_props.second<<std::endl;
						varproportions.emplace(varit->second,vdescriptor_props.second);
					}
				}
				//setSolution by proportions
				//keep track of variables setted 
				std::set<int>processedvariables;
				boost::unordered_set<Core::FMTDevelopment> processed;
				//store all out vertex out of variables set to process in queue
				std::queue<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>descriptors;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getPeriodVertices(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					//since it's comming from growth and nothing else we know the area
					if (m_graph->periodStart(*vertex_iterator))//get inperiod
					{
						const std::map<int,int> inidsvars = m_graph->getInIdsVariables(*vertex_iterator);
						const bool setrest = !(typeII&&m_graph->isNoTransfer(*vertex_iterator, 1));
						if(inidsvars.size()>1)
						{
							//Because what comes from previous period must have inArea ... will be set later 
							processedvariables.emplace(inidsvars.at(-1));
							descriptors.push(*vertex_iterator);
						}else{
							const double* solution = &new_solution[0];
							const double inArea = m_graph->inArea(*vertex_iterator, solution);
							if (inArea >0)
							{
								double outarea = 0;
								std::map<int, int>variables = m_graph->getOutVariables(*vertex_iterator);
								bool foundoutvar = false;
								for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
								{
									if(varproportions.find(varit->second)!=varproportions.end())
									{
										foundoutvar = true;
										std::vector<Core::FMTDevelopmentPath> paths = m_graph->getPaths(*vertex_iterator, varit->first);
										for (const Core::FMTDevelopmentPath path : paths)
										{
											if (path.getDevelopment().getPeriod() == period && processed.find(path.getDevelopment()) == processed.end())
											{
												processed.emplace(path.getDevelopment());
												Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor nextdescriptor = m_graph->getDevelopment(path.getDevelopment(), lookup);
												descriptors.push(nextdescriptor);
												
											}
										}
										//std::cout<<std::string(m_graph->getDevelopment(*vertex_iterator)) << " "+std::to_string(varproportions[varit->second])<<" "+std::to_string(inArea)<<" "+std::to_string(varit->first)<<std::endl;//<<" "+this->getactions().at(varit->first).getName()<<std::endl;
										double proportion = 1;
										if (setrest)
											{
											proportion = varproportions[varit->second];
											}
										const double outvararea = proportion*inArea;
										if (setrest||varit->first>=0)
										{
											new_solution[varit->second] = outvararea;
										}
										outarea+=outvararea;
									}
									processedvariables.emplace(varit->second);
									
								}
								if(!foundoutvar){
									//Check to put everythin on growth or death .. 
									//No need to put in descriptors, because it goes to the next period
									int targetaction = -1;
									if ((variables.find(-1) == variables.end()))//process only if you have evolution
									{
										targetaction = deathid;
									}
									if (setrest)
									{
										new_solution[variables.at(targetaction)] = inArea;
									}
									outarea+=inArea;
								}
								//Valider ?! pas sur ...
								if (inArea - tolerance > outarea )
								{
									_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
										"Negative growth solution for " +
										std::string(m_graph->getDevelopment(*vertex_iterator)) +" in area of " + std::to_string(inArea) + 
										" and out area of " + std::to_string(outarea) ,
										"FMTSrModel::forceSolution", __LINE__, __FILE__);
								}
								if (inArea + tolerance < outarea )
								{
									_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
										"Positive growth solution for " +
										std::string(m_graph->getDevelopment(*vertex_iterator)) +" in area of " + std::to_string(inArea) + 
										" and out area of " + std::to_string(outarea) ,
										"FMTSrModel::forceSolution", __LINE__, __FILE__);
								}
							}
						}
					}
				}
				size_t passwithoutprocess=0;
				while(!descriptors.empty())
				{
					const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor first = descriptors.front();
					//Cette portion est pour valider que toutes les variables qui rentrent dans le vertex on �t� setter par la fonction
					//sinon on le remet a la fin de la queue 
					const std::vector<int>invariables = m_graph->getInVariables(first);
					const std::set<int> invariablesset(invariables.begin(), invariables.end());
					std::set<int> notprocessedvariables;
					std::set_difference(invariablesset.begin(), invariablesset.end(), processedvariables.begin(), processedvariables.end(), std::inserter(notprocessedvariables, notprocessedvariables.end()));
					bool canprocess = true;
					for(std::set<int>::iterator it = notprocessedvariables.begin(); it!=notprocessedvariables.end(); ++it)
					{	
						//In this case a variables comming in the vertex with a proportion in schedule as not been setted in solution.
						if(varproportions.find(*it)!=varproportions.end())
						{
							canprocess = false;
						}
					}
					const bool setrest = !(typeII&&m_graph->isNoTransfer(first, 1));
					if	(canprocess)
					{
						passwithoutprocess=0;
						//Check ce bout la ... 
						const double* solution = &new_solution[0];
						const double inArea = m_graph->inArea(first, solution);
						double outarea = 0;
						std::map<int, int>variables = m_graph->getOutVariables(first);
						bool foundoutvar = false;
						for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
						{
							if(varproportions.find(varit->second)!=varproportions.end())
							{
								foundoutvar = true;
								std::vector<Core::FMTDevelopmentPath> paths = m_graph->getPaths(first, varit->first);
								for (const Core::FMTDevelopmentPath path : paths)
								{
									if (path.getDevelopment().getPeriod() == period && processed.find(path.getDevelopment()) == processed.end())
									{
										processed.emplace(path.getDevelopment());
										descriptors.push(m_graph->getDevelopment(path.getDevelopment(), lookup));
									}
								}
								//std::cout<<std::string(m_graph->getDevelopment(first)) << " "+std::to_string(varproportions[varit->second])<<" "+std::to_string(inArea)<<" "+std::to_string(varit->first)<<std::endl;//<<" "+this->getactions().at(varit->first).getName()<<std::endl;
								double proportion = 1;
								if (setrest)
								{
									proportion = varproportions[varit->second];
								}
								const double outvararea = proportion *inArea;
								if (setrest||varit->first>=0)
								{
									new_solution[varit->second] = outvararea;
								}
								outarea+=outvararea;
							}
							processedvariables.emplace(varit->second);
							
						}
						if(!foundoutvar){
							//Check to put everythin on growth or death .. 
							int targetaction = -1;
							if ((variables.find(-1) == variables.end()))//process only if you have evolution
							{
								targetaction = deathid;
							}
							if (setrest)
							{
								new_solution[variables.at(targetaction)] = inArea;
							}
							outarea+=inArea;
						}
						//Valider ?! pas sur ...
						if (inArea - tolerance > outarea )
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
								"Negative growth solution for " +
								std::string(m_graph->getDevelopment(first)) +" in area of " + std::to_string(inArea) +
								" and out area of " + std::to_string(outarea) ,
								"FMTSrModel::forceSolution", __LINE__, __FILE__);
						}
						if (inArea + tolerance < outarea )
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
								"Positive growth solution for " +
								std::string(m_graph->getDevelopment(first)) +" in area of " + std::to_string(inArea) +
								" and out area of " + std::to_string(outarea) ,
								"FMTSrModel::forceSolution", __LINE__, __FILE__);
						}
						descriptors.pop();
					}else{
						++passwithoutprocess;
						if(passwithoutprocess == descriptors.size())
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
								"Wrong variable",
								"FMTSrModel::forceSolution", __LINE__, __FILE__);
						}
						descriptors.pop();
						descriptors.push(first);
						//handle pour ne pas avoir de boucle infini et passer ceux qui sont r�cursif sur la meme action genre boucle infini ... valider aussi comment on g�re �a dans FMTGraph
					}
				}
				solver.setColSolution(&new_solution[0]);

			}
		}catch(...){
			_exhandler->printExceptions("at period " + std::to_string(period), "FMTSrModel::forceSolution", __LINE__, __FILE__);
		}
		return true;

	}


	bool FMTSrModel::setSolution(int period, const Core::FMTSchedule& schedule, double tolerance)
	{
		try {
			const bool WILL_THROW = parameters.getBoolParameter(FMTboolmodelparameters::SETSOLUTION_THROW);
			if (static_cast<int>(m_graph->size()) > period && period > 0)
			{
				std::vector<Core::FMTAction>::const_iterator cit = std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator("_DEATH"));
				const int deathid = static_cast<int>(std::distance(actions.cbegin(), cit));
				const double* actual_solution = solver.getColSolution();
				const boost::unordered_set<Core::FMTLookup<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor, Core::FMTDevelopment>> lookup = m_graph->getDevsSet(period);
				std::vector<double>new_solution(actual_solution, actual_solution + solver.getNumCols());
				Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				const bool typeII = (getParameter(Models::FMTintmodelparameters::MATRIX_TYPE)==2);
				for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getPeriodVertices(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					if (typeII&&m_graph->isNoTransfer(*vertex_iterator,1))//Dont need to set to 0 global growth variable.
						{
						continue;
						}
					const std::map<int, int>variables = m_graph->getOutVariables(*vertex_iterator);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						new_solution[varit->second] = 0;
					}
				}
				int maximallock = -1;
				for (int actionid = 0; actionid < static_cast<int>(actions.size()); ++actionid/*const auto& actionit : schedule*/)
				{

					//int actionid = int(std::distance(actions.begin(), std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator(actionit.first.getName()))));
					const auto& actionit = schedule.find(actions.at(actionid));
					if (actionit != schedule.end())
					{
						//*_logger << "processinh " << actionit->first.getName() << "\n";
						size_t allocated = 0;
						for (const auto& devit : actionit->second)
						{
							if (((schedule.doUseLock() || actionit->first.doRespectLock()) && m_graph->containsDevelopment(devit.first, lookup)))
							{
								//*_logger << "t1 " << actionit->first.getName() << "\n";
								const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor vdescriptor = m_graph->getDevelopment(devit.first, lookup);
								//const int variable = m_graph->getOutVariables(vdescriptor)[actionid];
								std::map<int, int> outvariables = m_graph->getOutVariables(vdescriptor);
								std::map<int, int>::const_iterator varit = outvariables.find(actionid);
								if (varit == outvariables.cend())
								{
									const Exception::FMTexc EXCEPTION = WILL_THROW ? Exception::FMTexc::FMTinvalid_number : Exception::FMTexc::FMTignore;
									_exhandler->raise(EXCEPTION,
										"Developement " + std::string(devit.first) + " is not operable "
										" for action " + actionit->first.getName(),
										"FMTSrModel::setSolution", __LINE__, __FILE__);
									return false;
								}
								const int variable = varit->second;
								new_solution[variable] = devit.second.at(0);
								++allocated;
							}
							else if (!schedule.doUseLock() && !actionit->first.doRespectLock())
							{
								//*_logger << "t2 " << actionit->first.getName() << "\n";
								if (maximallock == -1)
								{
									maximallock = m_graph->getMaximalLock(period);
								}
								std::vector<double>lockstoadress(devit.second);
								std::vector<std::pair<Core::FMTDevelopment, double>>locksfound;
								std::vector<std::pair<int, size_t>>locksorter;
								Core::FMTDevelopment locked(devit.first);
								for (int lockid = 0; lockid <= maximallock; ++lockid)
								{
									locked.setLock(lockid);
									if (m_graph->containsDevelopment(locked, lookup))
									{
										const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor vdescriptor = m_graph->getDevelopment(locked, lookup);
										const double* newsolutionptr = &new_solution[0];
										double originalinarea = m_graph->inArea(vdescriptor, newsolutionptr, true);
										if (originalinarea == 0)
										{
											originalinarea = std::numeric_limits<double>::max();
										}
										if (!(m_graph->onlyPeriodStart(vdescriptor) && originalinarea == std::numeric_limits<double>::max()))
										{
											locksorter.push_back(std::pair<size_t, size_t>(locksfound.size(), m_graph->getAmountOfPaths(locked, -1, *this, lookup)));
											locksfound.push_back(std::pair<Core::FMTDevelopment, double>(locked, originalinarea));

										}
									}
								}
								//*_logger << "got " << locksfound.size() << " for " << actionit->first.getName() << "\n";
								std::sort(locksorter.begin(),
									locksorter.end(),
									[](const std::pair<size_t, size_t>& a,
										const std::pair<size_t, size_t>& b) {return a.second < b.second; });

								std::vector<std::pair<Core::FMTDevelopment, double>>sortedlocksfound;
								for (const std::pair<size_t, size_t>& id : locksorter)
								{
									sortedlocksfound.push_back(locksfound.at(id.first));
								}
								locksfound = sortedlocksfound;
								bool secondpass = false;
								const size_t initialsize = lockstoadress.size();
								size_t iteration = 0;
								while (!lockstoadress.empty())
								{
									const double areatoput = *lockstoadress.begin();
									if (tolerance < areatoput)
									{
										size_t id = 0;
										bool found = false;
										bool exact = false;
										for (const std::pair<Core::FMTDevelopment, double>& element : locksfound)
										{
											//*_logger << "testing " << std::string(element.first) << " " << element.second<<" for "<< areatoput << "\n";
											if (std::abs(areatoput - element.second) < tolerance)
											{
												found = true;
												exact = true;
												//*_logger << "exact " << std::string(element.first) << " " << element.second << "\n";
												break;
											}
											//*_logger << "op "<< areatoput <<" " << std::string(element.first) << " " << element.second << "\n";
											++id;
										}
										if (secondpass && !found)
										{
											id = 0;
											for (const std::pair<Core::FMTDevelopment, double>& element : locksfound)
											{
												//*_logger << "ON second passt "<< std::string(element.first) <<" "<< areatoput<<" "<< (element.second + tolerance) << "\n";
												if (areatoput <= (element.second + tolerance))
												{
													found = true;
													//*_logger << "non exact " << std::string(element.first) << " " << element.second << "\n";
													break;
												}
												++id;
											}
										}
										if (found)
										{
											const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor vdescriptor = m_graph->getDevelopment(locksfound.at(id).first, lookup);
											//const int variable = m_graph->getOutVariables(vdescriptor)[actionid];
											std::map<int, int> outvariables = m_graph->getOutVariables(vdescriptor);
											std::map<int, int>::const_iterator varit = outvariables.find(actionid);
											if (varit == outvariables.cend())
											{
												const Exception::FMTexc EXCEPTION = WILL_THROW ? Exception::FMTexc::FMTinvalid_number : Exception::FMTexc::FMTignore;
												_exhandler->raise(EXCEPTION,
													"Developement " + std::string(devit.first) + " is not operable "
													" for action " + actionit->first.getName(),
													"FMTSrModel::setSolution", __LINE__, __FILE__);
												return false;
											}
											const int variable = varit->second;
											new_solution[variable] += areatoput;
											if (locksfound.at(id).second < std::numeric_limits<double>::max())
											{
												locksfound.at(id).second -= areatoput;
												if (exact || locksfound.at(id).second < tolerance)
												{
													//*_logger << "Removing " << std::string(locksfound.at(id).first)<<" "<< locksfound.at(id).second << "\n";
													locksfound.erase(locksfound.begin() + id);
												}
											}
											lockstoadress.erase(lockstoadress.begin());
											++allocated;
										}
										else if (secondpass)
										{
											const Exception::FMTexc EXCEPTION = WILL_THROW ? Exception::FMTexc::FMTinvalid_number : Exception::FMTexc::FMTignore;
											_exhandler->raise(EXCEPTION,
												"Cannot allocate area of " + std::to_string(areatoput) + " to " +
												std::string(devit.first) + " for action " + actionit->first.getName(), "FMTSrModel::setSolution", __LINE__, __FILE__);
											return false;
										}
										else {
											lockstoadress.push_back(areatoput);
											lockstoadress.erase(lockstoadress.begin());
										}
									}
									else {
										lockstoadress.erase(lockstoadress.begin());
									}
									if (iteration == initialsize)
									{
										secondpass = true;
									}
									++iteration;
								}
							}
							else {
								_exhandler->raise(Exception::FMTexc::FMTmissingdevelopment, std::string(devit.first) + " at period " + std::to_string(period) + " operated by " + actionit->first.getName(),
									"FMTSrModel::setSolution", __LINE__, __FILE__);
								return false;
							}
						}
					}
				}
				//Fill up natural evolution
				boost::unordered_map<Core::FMTDevelopment, Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>processed;
				std::queue<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>descriptors;
				//Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getPeriodVertices(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					if (m_graph->periodStart(*vertex_iterator))//get inperiod
					{
						const double* solution = &new_solution[0];
						double rest = m_graph->inArea(*vertex_iterator, solution);
						//double rest = m_graph->inArea(devit->second, actual_solution);
						std::map<int, int>variables = m_graph->getOutVariables(*vertex_iterator);
						int targetaction = -1;
						if ((variables.find(-1) == variables.end()))//process only if you have evolution
						{
							targetaction = deathid;
						}
						const int growth = variables[targetaction];
						if (targetaction < 0)
						{
							variables.erase(targetaction);
						}
						const bool setrest = !(typeII&&m_graph->isNoTransfer(*vertex_iterator, 1));
						for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
						{
							std::vector<Core::FMTDevelopmentPath> paths = m_graph->getPaths(*vertex_iterator, varit->first);
							for (const Core::FMTDevelopmentPath path : paths)
							{
								if (path.getDevelopment().getPeriod() == period && processed.find(path.getDevelopment()) == processed.end())
								{
									processed[path.getDevelopment()] = m_graph->getDevelopment(path.getDevelopment(), lookup);
									descriptors.push(m_graph->getDevelopment(path.getDevelopment(), lookup));
								}
							}
							//rest -= *(actual_solution + varit->second);
							rest -= new_solution[varit->second];
						}
						if ((rest + tolerance) < 0)
						{
							std::string actionnames;
							for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
							{
								actionnames += actions.at(varit->first).getName() + ",";
							}
							actionnames.pop_back();
							const Core::FMTDevelopment dev(m_graph->getDevelopment(*vertex_iterator));
							const double* solution = &new_solution[0];
							const double inArea = m_graph->inArea(*vertex_iterator, solution);
							std::string locking;
							if (dev.getLock() > 0)
							{
								Core::FMTDevelopment locked(dev);
								locking += " lock(";
								for (int locklevel = 0; locklevel < 30; ++locklevel)
								{
									locked.setLock(locklevel);
									if (m_graph->containsDevelopment(locked, lookup))
									{
										locking += std::to_string(locklevel) + ",";
									}
								}
								locking.pop_back();
								locking += ")";
							}

							const Exception::FMTexc EXCEPTION = WILL_THROW ? Exception::FMTexc::FMTinvalid_number : Exception::FMTexc::FMTignore;
							_exhandler->raise(EXCEPTION,
								std::to_string(rest) + " negative growth solution for " +
								std::string(dev) + " operated by " + actionnames + locking + " in area " + std::to_string(inArea),
								"FMTSrModel::setSolution", __LINE__, __FILE__);
							return false;
						}
						if ((targetaction < 0) && setrest)//Ajust only natural growth and not _DEATH
						{
							new_solution[growth] = rest;
						}

					}
				}
				while (!descriptors.empty())
				{
					Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor first = descriptors.front();
					std::map<int, int>variables = m_graph->getOutVariables(first);
					const double* solution = &new_solution[0];
					double rest = m_graph->inArea(first, solution);
					int targetaction = -1;
					if ((variables.find(-1) == variables.end()))//Dont need to fill up if you dont have natural evolution
					{
						targetaction = deathid;
					}
					const int growth = variables[targetaction];
					if (targetaction < 0)
					{
						variables.erase(targetaction);
					}
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						std::vector<Core::FMTDevelopmentPath> paths = m_graph->getPaths(first, varit->first);
						for (const Core::FMTDevelopmentPath path : paths)
						{
							if (path.getDevelopment().getPeriod() == period && processed.find(path.getDevelopment()) == processed.end())
							{
								processed[path.getDevelopment()] = m_graph->getDevelopment(path.getDevelopment(), lookup);
								descriptors.push(m_graph->getDevelopment(path.getDevelopment(), lookup));
							}
						}

						rest -= new_solution[varit->second];
					}
					if ((rest + tolerance) < 0)
					{
						const Exception::FMTexc EXCEPTION = WILL_THROW ? Exception::FMTexc::FMTinvalid_number : Exception::FMTexc::FMTignore;
						_exhandler->raise(EXCEPTION,
							std::to_string(rest) + " negative growth solution for " +
							std::string(m_graph->getDevelopment(first)),
							"FMTSrModel::setSolution", __LINE__, __FILE__);
						return false;
					}
					const bool setrest = !(typeII&&m_graph->isNoTransfer(first, 1));
					if ((targetaction < 0) && setrest)
					{

						new_solution[growth] = rest;
					}
					descriptors.pop();
				}
				solver.setColSolution(&new_solution[0]);
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("at period " + std::to_string(period), "FMTSrModel::setSolution", __LINE__, __FILE__);
		}
		return true;
	}

	std::set<std::string> FMTSrModel::getAllMasks(const std::vector<Core::FMTTheme>& p_selectedThemes) const {
		std::set<std::string> masks;
		try {
			std::vector<Core::FMTTheme> toIgnore;
			for (const Core::FMTTheme& THEME : themes) {
				if (std::find_if(p_selectedThemes.begin(), p_selectedThemes.end(), Core::FMTThemeComparator(THEME)) == p_selectedThemes.end()) {
					toIgnore.push_back(THEME);
				}
			}
			masks = m_graph->getAllDevelopmentsMask(toIgnore);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::getAllMasks", __LINE__, __FILE__);
		}
		return masks;
	}

	bool FMTSrModel::setSolutionByLp(int period, const Core::FMTSchedule& schedule, double tolerance)
	{
		try {
			if (Graph::FMTgraphbuild::schedulebuild != m_graph->getBuildType())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot set solution by lp on a non partial graph",
					"FMTSrModel::setSolutionByLp", __LINE__, __FILE__);
			}
			if (static_cast<int>(m_graph->size()) > period && period > 0)
			{
				const boost::unordered_set<Core::FMTLookup<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor, Core::FMTDevelopment>> lookup = m_graph->getDevsSet(period);
				std::map<int, std::pair<double, double>>bounds;
				Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getPeriodVertices(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const std::map<int, int>variables = m_graph->getOutVariables(*vertex_iterator);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						if (varit->first != -1)
						{
								bounds[varit->second] = std::pair<double, double>(0.0, 0.0);
						}
					}
				}
				std::vector<double>objcoefs(solver.getObjCoefficients(), solver.getNumCols() + solver.getObjCoefficients());
				const bool usetobeoptimal = solver.isProvenOptimal();
				int maximallock = -1;
				for (int actionid = 0; actionid < static_cast<int>(actions.size()); ++actionid)
				{
					const auto& actionit = schedule.find(actions.at(actionid));
					if (actionit != schedule.end())
					{
						double actionareaset = 0;
						for (const auto& devit : actionit->second)
						{
							//(Lock in schedule or action not LOCKEXEMPT) and dev in graph 
							if ((schedule.doUseLock() || actionit->first.doRespectLock()) && m_graph->containsDevelopment(devit.first, lookup))
							{
								const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor vdescriptor = m_graph->getDevelopment(devit.first, lookup);
								//const int variable = m_graph->getOutVariables(vdescriptor)[actionid];
								std::map<int, int> outvariables = m_graph->getOutVariables(vdescriptor);
								std::map<int, int>::const_iterator varit = outvariables.find(actionid);
								if (varit == outvariables.cend())
								{
									_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
												"Developement " + std::string(devit.first) + " is not operable "
												" for action " + actionit->first.getName(), 
												"FMTSrModel::setSolutionByLp", __LINE__, __FILE__);
								}
								const int variable = varit->second;
								double devarea = devit.second.at(0);
								if(devit.second.size()>1)
								{
									//In case the schedule contain multiple line for the same dev with different lock ...
									devarea = 0;
									for(const auto& a : devit.second)
									{
										devarea+=a;
									}
								}
								bounds[variable] = std::pair<double, double>(devarea - tolerance, devarea + tolerance);
								objcoefs[variable] = 1.0;
								actionareaset += devarea + tolerance;
							}
							//LOCKEXEMPT and no lock in schedule
							else if (!schedule.doUseLock() && !actionit->first.doRespectLock())
							{
								if (maximallock == -1)
								{
									maximallock = m_graph->getMaximalLock(period);
								}
								Core::FMTDevelopment locked(devit.first);
								bool gotsomething = false;
								double totalareaofdevs = 0;
								for (const double& value : devit.second)
								{
									totalareaofdevs += value;
								}
								//we can add a constraint here to force a given level!
								std::vector<int>mixedvariables;
								for (int lockid = 0; lockid <= maximallock; ++lockid)
								{
									locked.setLock(lockid);
									if (m_graph->containsDevelopment(locked, lookup))
									{
										const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor vdescriptor = m_graph->getDevelopment(locked, lookup);
										//const int variable = m_graph->getOutVariables(vdescriptor)[actionid];
										std::map<int, int> outvariables = m_graph->getOutVariables(vdescriptor);
										std::map<int, int>::const_iterator varit = outvariables.find(actionid);
										if (varit == outvariables.cend())
										{
											_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
														"Developement " + std::string(devit.first) + " is not operable "
														" for action " + actionit->first.getName(), 
														"FMTSrModel::setSolutionByLp", __LINE__, __FILE__);
										}
										const int variable = varit->second;
										gotsomething = true;
										bounds[variable] = std::pair<double, double>(0.0, totalareaofdevs+tolerance);
										actionareaset += totalareaofdevs + tolerance;
										objcoefs[variable] = 1.0;
										mixedvariables.push_back(variable);
										
									}
								}
								std::vector<double>rowcoefs(mixedvariables.size(), 1.0);
								solver.addRow(static_cast<int>(rowcoefs.size()), &mixedvariables[0],
										&rowcoefs[0], totalareaofdevs - tolerance, totalareaofdevs);

								if (!gotsomething)
								{
									_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
										"Cannot allocate any developements for action " + std::string(actionit->first.getName()) +
										" at period " + std::to_string(period),
										"FMTSrModel::setSolutionByLp", __LINE__, __FILE__);
								}
							}
							else {
								_exhandler->raise(Exception::FMTexc::FMTmissingdevelopment, std::string(devit.first) + " at period " + std::to_string(period) + " operated by " + actionit->first.getName(),
									"FMTSrModel::setSolutionByLp", __LINE__, __FILE__);
								return false;
							}
						}
						//*_logger << "area set of " << actionareaset  <<" set for "<< actionit->first.getName() << "\n";

					}
				}

				std::vector<double>varsbounds(bounds.size() * 2);
				std::vector<int>variables(bounds.size());
				size_t id = 0;
				for (std::map<int, std::pair<double, double>>::const_iterator cit = bounds.begin(); cit != bounds.end(); ++cit)
				{
					variables[id] = cit->first;
					varsbounds[(id * 2)] = cit->second.first;
					varsbounds[(id * 2) + 1] = cit->second.second;
					++id;
				}
				//solver.setColSetBounds(&*(variables.cbegin()), &*(variables.cend()), &varsbounds[0]);
				solver.setColSetBounds(&variables[0], &variables.back() + 1, &varsbounds[0]);
				solver.setObjective(&objcoefs[0]);
				solver.setObjSense(-1);
				if (usetobeoptimal)
				{
					solver.stockResolve();
				}
				else {
					solver.initialSolve();
				}

				if (!solver.isProvenOptimal())
				{
					/*for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getPeriodVertices(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const std::map<int, int>variables = m_graph->getOutVariables(*vertex_iterator);
						for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
						{

						}
					}*/
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Infeasible schedule at period " + std::to_string(period),
						"FMTSrModel::setSolutionByLp", __LINE__, __FILE__);
				}
				const double* solution = solver.getColSolution();
				double scheduleobjective = 0;
				for (const int& var : variables)
				{
					scheduleobjective += *(solution + var);
				}
				const std::string areacomparison = "schedule area of " + std::to_string(schedule.area()) + " vs " + std::to_string(scheduleobjective)+"\n";
				_logger->logWithLevel(areacomparison, 1);
				std::vector<double>varsconstraint(bounds.size(), 1.0);
				solver.addRow(static_cast<int>(varsconstraint.size()), &variables[0],
					&varsconstraint[0], std::min(scheduleobjective - tolerance, schedule.area()));
				if (!solver.stockResolve())
				{
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Infeasible on resolve " + std::to_string(period),
						"FMTlsrmodel::setSolutionByLp", __LINE__, __FILE__);
				}

			}
		}
		catch (...)
		{
			_exhandler->printExceptions("at period " + std::to_string(period), "FMTSrModel::setSolutionByLp", __LINE__, __FILE__);
		}


		return true;
	}

	FMTSrModel::FMTSrModel(FMTModel&& base, const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& lgraph,
		const FMTLpSolver& lsolver) :
		FMTModel(base),
		m_graph(new Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>(lgraph)),
		solver(lsolver)
	{
		//solver.passinobject(base);
		//m_graph->passinobject(base);
	}

	FMTSrModel::FMTSrModel(FMTSrModel&& rhs) noexcept :
		FMTModel(),
		m_graph(new Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>()),
		solver()
	{
		*this = std::move(rhs);
	}

	FMTSrModel& FMTSrModel::operator = (const FMTSrModel& rhs)
	{
		if (this != &rhs)
		{
			FMTModel::operator=(rhs);
			m_graph = std::move(std::unique_ptr<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>>(new 
								Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>(*rhs.m_graph)));
			solver=rhs.solver;
		}
		return *this;
	}

	FMTSrModel& FMTSrModel::operator =(FMTSrModel&& rhs) noexcept
	{
		if (this!=&rhs)
		{
			FMTModel::operator=(std::move(rhs));
			//graph=rhs.graph;//Boost graph does not provide real swap so...
			m_graph.swap(rhs.m_graph);
			solver.swap(rhs.solver);
		}
		return *this;
	}


	void FMTSrModel::postSolve(const FMTModel& originalbasemodel)
	{
		try {
			postsolveGraph(originalbasemodel);
			FMTModel::postSolve(originalbasemodel);
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::postSolve", __LINE__, __FILE__);
		}

	}

	bool FMTSrModel::isOptimal() const
	{
		try {
			if (m_graph->getBuildType() == Graph::FMTgraphbuild::fullbuild)//Only global Graph
			{
				return solver.isProvenOptimal();
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::isOptimal", __LINE__, __FILE__);
		}
		return false;
	}
	
	double FMTSrModel::getObjectiveValue() const
	{
		double value = std::numeric_limits<double>::quiet_NaN();
		try {
			value = solver.getObjValue();
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTSrModel::getObjectiveValue", __LINE__, __FILE__);
			}
		return value;
	}



	std::unique_ptr<FMTModel>FMTSrModel::presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments) const
	{
		try{
			if (!m_graph->empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Cannot presolve a srmodel with period(s) builded in m_graph->",
					"FMTSrModel::presolve", __LINE__, __FILE__);
				}
			return std::unique_ptr<FMTModel>(new FMTSrModel(std::move(*FMTModel::presolve(optionaldevelopments)),*m_graph,solver));
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::presolve", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTModel>(nullptr);
	}


	Core::FMTSchedule FMTSrModel::getSolution(int period, bool withlock) const
	{
		Core::FMTSchedule newSchedule;
		try
		{
			//setParameter(SHOW_LOCK_IN_SCHEDULES,withlock);
			const double* actual_solution = solver.getColSolution();
			newSchedule = m_graph->getSchedule(actions, actual_solution, period, withlock);//getParameter(SHOW_LOCK_IN_SCHEDULES));

		}
		catch (...)
		{
			_exhandler->printExceptions("at period " + std::to_string(period), "FMTSrModel::getSolution", __LINE__, __FILE__);
		}
		return newSchedule;
	}

	Core::FMTSchedule FMTSrModel::getScheduleProportions(int period, bool withlock) const
	{
		Core::FMTSchedule newSchedule;
		try
		{
			const double* actual_solution = solver.getColSolution();
			newSchedule = m_graph->getOutVariablesProportions(actions, actual_solution, period, withlock);//getParameter(SHOW_LOCK_IN_SCHEDULES));
		}
		catch (...)
		{
			_exhandler->printExceptions("at period " + std::to_string(period), "FMTSrModel::getScheduleProportions", __LINE__, __FILE__);
		}
		return newSchedule;
	}

	Graph::FMTGraphStats FMTSrModel::updateMatrix(const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_pair& targets,
		const Graph::FMTGraphStats& newstats)
	{

		try {
			//columns
			const std::vector<int>column_Starts(static_cast<size_t>(newstats.cols) + 1, 0);
			const std::vector<int>targetrows(newstats.cols, 0);
			const std::vector<double>nelements(newstats.cols, 0.0);
			const std::vector<double>lower_bounds(newstats.cols, 0.0);
			const std::vector<double>upper_bounds(newstats.cols, std::numeric_limits< double >::max());
			if (newstats.cols>0)
				{
				solver.addCols(newstats.cols, &column_Starts[0], &targetrows[0],
					&nelements[0], &lower_bounds[0],
					&upper_bounds[0], &nelements[0]);
				}
			//rows
			std::vector<int>row_Starts;
			//row_Starts.reserve(targets.size());
			std::vector<int>targetcols;
			std::vector<double>elements;

			//Need to reset a new constraint ID!
			Graph::FMTGraphStats oldstats = m_graph->getStats();
			int newconstraintID = solver.getNumRows();
			size_t periodsize = 0;
			Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_iterator it, itend;
			const bool typeII = (getParameter(Models::FMTintmodelparameters::MATRIX_TYPE) == 2);
			for (boost::tie(it, itend) = targets; it != itend; ++it)
			{
				if (typeII&&m_graph->isNoTransfer(*it,1))//We dont need a transfer Row here
				{
					//Type II behavior
				}else {//Else add a new transfer row.
					m_graph->setConstraintID(*it, newconstraintID);
					if (m_graph->getTransferRow(*it, row_Starts, targetcols, elements))
					{

					}
					++oldstats.transfer_rows;
					++newconstraintID;
					++periodsize;
				}
				
				
			}
			const std::vector<double>row_bounds(periodsize, 0.0);
			const int nrows = (newconstraintID - solver.getNumRows());
			row_Starts.push_back(static_cast<int>(targetcols.size()));
			if (nrows>0)
				{
				solver.addRows(nrows, &row_Starts[0], &targetcols[0],
					&elements[0], &row_bounds[0], &row_bounds[0]);
				}
			oldstats.cols = solver.getNumCols();
			oldstats.rows = solver.getNumRows();
			return oldstats;
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSrModel::updateMatrix", __LINE__, __FILE__);
		}
		return this->getStats();
	}


	FMTSrModel::FMTSrModel(const FMTModel& base, FMTsolverinterface lsolvertype) :
		FMTModel(base),
		m_graph(new Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>(Graph::FMTgraphbuild::nobuild)),
		solver(lsolvertype,
			base.getParameter(FMTstrmodelparameters::SOLVER_COLD_START),
			base.getParameter(FMTstrmodelparameters::SOLVER_WARM_START),
			base.getName())
	{
		//solver.passinobject(base);
		//m_graph->passinobject(base);

	}

	FMTSrModel::FMTSrModel(const FMTSrModel& rhs):
		FMTModel(rhs),
		m_graph(new Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>(*rhs.m_graph)),
		solver(rhs.solver)
	{
		//solver.passinobject(rhs);
		//m_graph->passinobject(rhs);
	}

	FMTSrModel::FMTSrModel() :
		FMTModel(),
		m_graph(new Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>(Graph::FMTgraphbuild::nobuild)),
		solver()
	{

	}



	bool FMTSrModel::operator == (const FMTSrModel& rhs) const
	{
		return (FMTModel::operator == (rhs) &&
			solver == (rhs.solver) &&
			*m_graph == *rhs.m_graph);
	}

	bool FMTSrModel::operator != (const FMTSrModel& rhs) const
	{
		return !(*this == rhs);
	}

	std::map<std::string, double> FMTSrModel::getOutput(const Core::FMTOutput& output, int period, Core::FMToutputlevel level) const
	{
		try {
			const double* solution = solver.getColSolution();
			if (!output.isVariableSizeOf(area.begin()->getMask().size()))
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
					"For output " + std::string(output.getName()),
					"FMTSrModel::getOutput", __LINE__, __FILE__);
				}
			return m_graph->getOutput(*this, output, period, solution, level);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::getOutput", __LINE__, __FILE__);
		}

		return std::map<std::string, double>();
	}

	std::queue<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor> FMTSrModel::getActives()
	{
		std::queue<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor> actives;
		try {
			if (m_graph->empty())
			{
				actives = m_graph->initialize(area);
				Graph::FMTGraphStats BUILD_STATS = initializeMatrix();
				m_graph->setStats(BUILD_STATS);
			}
			else {
				actives = m_graph->getActiveVertices();
			}
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::getActives", __LINE__, __FILE__);
		}
		return actives;
	}



	Graph::FMTGraphStats FMTSrModel::buildPeriod(Core::FMTSchedule schedule, bool forcepartialbuild, int compressageclassoperability)
	{
		try {
			std::queue<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor> actives = getActives();
			Graph::FMTGraphStats buildstats;
			if (!forcepartialbuild && schedule.empty()) // full build
			{
				buildstats = m_graph->build(*this, actives, compressageclassoperability);
				m_graph->setBuildType(Graph::FMTgraphbuild::fullbuild);
			}
			else {//partial build for result
				m_graph->setBuildType(Graph::FMTgraphbuild::schedulebuild);
				buildstats = m_graph->buildSchedule(*this, actives, schedule);
			}
			const int location = static_cast<int>(m_graph->size() - 2);
			const Graph::FMTGraphStats newstats = this->updateMatrix(m_graph->getPeriodVertices(location), buildstats);
			m_graph->setStats(newstats);
		}
		catch (...)
		{
			const int PERIOD = static_cast<int>(m_graph->size() - 3);
			_exhandler->printExceptions("At Period "+std::to_string(PERIOD), 
								"FMTSrModel::buildPeriod", __LINE__, __FILE__);
		}

		return m_graph->getStats();
	}



	size_t FMTSrModel::getGraphSize() const
	{
		return m_graph->size();
	}

	void FMTSrModel::clearGraphDevelopments()
	{
		m_graph->clearDevelopments();
	}
	void FMTSrModel::clearGraphCache()
	{
		m_graph->clearCache();
	}


	bool FMTSrModel::summarize(const std::map<int, double>& variables,
		std::vector<int>& sumvariables, std::vector<double>& sumcoefficiants) const
	{
		if (!variables.empty())
		{
			sumvariables.clear();
			sumcoefficiants.clear();
			sumvariables.reserve(variables.size());
			sumcoefficiants.reserve(variables.size());
			for (std::map<int, double>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
			{
				if (varit->second != 0)
				{
					sumvariables.push_back(varit->first);
					sumcoefficiants.push_back(varit->second);
				}
			}
			return true;
		}
		return false;
	}


	Graph::FMTGraphStats FMTSrModel::getStats() const
	{
		return m_graph->getStats();
	}

	Graph::FMTGraphStats FMTSrModel::getGraphStats(const Core::FMTMask& p_Subset) const
	{
		Graph::FMTGraphStats TheStats;
		try {
			TheStats = m_graph->getStats(p_Subset);
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::getGraphStats", __LINE__, __FILE__);
		}
		return TheStats;
	}

	int FMTSrModel::getFirstActivePeriod() const
	{
		return m_graph->getFirstActivePeriod();
	}

	void FMTSrModel::postsolveGraph(const FMTModel& originalbasemodel)
	{
		try {
			const std::vector<Core::FMTTheme>& postsolvethemes = dynamic_cast<const FMTSrModel*>(&originalbasemodel)->themes;
			const std::vector<Core::FMTAction>& postsolveactions = dynamic_cast<const FMTSrModel*>(&originalbasemodel)->actions;
			const Core::FMTMaskFilter postsolvefilter = this->getPostsolveFilter(originalbasemodel.getThemes(),originalbasemodel.getArea().begin()->getMask());
			const std::vector<Core::FMTAction>& presolveactions = this->actions;
			std::vector<int>actionmapping;
			actionmapping.reserve(presolveactions.size());
			for (const Core::FMTAction action : presolveactions)
			{
				const int loc = static_cast<int>(std::distance(postsolveactions.begin(), std::find_if(postsolveactions.begin(), postsolveactions.end(), Core::FMTActionComparator(action.getName()))));
				actionmapping.push_back(loc);
			}
			this->m_graph->postSolve(postsolvefilter,postsolvethemes, actionmapping);
			
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::postsolveGraph", __LINE__, __FILE__);
		}
	}



	std::vector<Core::FMTActualDevelopment>FMTSrModel::getArea(int period, bool beforegrowanddeath) const
	{
		std::vector<Core::FMTActualDevelopment>returnedarea;
		try {
			returnedarea.reserve(area.size());//Reserve at least the size of the initial area.
			if (period == 0)
			{
				return FMTModel::getArea();
			};
			const double* modelsolution = solver.getColSolution();
			const int deathactionid = static_cast<int>(actions.size()-1);
			Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			if (beforegrowanddeath)
			{
				--period;
			}
			for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getPeriodVertices(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
				if ((!beforegrowanddeath&&m_graph->periodStart(*vertex_iterator)))
				{
					const Core::FMTDevelopment& graphdevelopment = m_graph->getDevelopment(*vertex_iterator);
					const double areaofdevelopment = m_graph->inArea(*vertex_iterator, modelsolution,-1,true);
					if (areaofdevelopment > FMT_DBL_TOLERANCE)
					{
						returnedarea.push_back(Core::FMTActualDevelopment(graphdevelopment, areaofdevelopment));
					}
				}
				else if (beforegrowanddeath)
				{
					const Core::FMTDevelopment& graphdevelopment = m_graph->getDevelopment(*vertex_iterator);
					const double indeatharea = m_graph->inArea(*vertex_iterator, modelsolution, deathactionid, true);
					const double areaofdevelopment = m_graph->outArea(*vertex_iterator, -1, modelsolution) +m_graph->outArea(*vertex_iterator, deathactionid, modelsolution) -indeatharea;
					if (areaofdevelopment > FMT_DBL_TOLERANCE)
					{
						returnedarea.push_back(Core::FMTActualDevelopment(graphdevelopment, areaofdevelopment));
					}

				}
				
			}
			std::sort(returnedarea.begin(), returnedarea.end());
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::getArea", __LINE__, __FILE__);
		}

		return returnedarea;
	}


	std::vector<Core::FMTActualDevelopment> FMTSrModel::getPotentialArea(int p_Period, bool p_BeforeGrowAndDeath) const
	{
		std::vector<Core::FMTActualDevelopment> devArea;
		try {
			const std::vector<double> POTENTIAL_SOLUTION(solver.getNumCols(), 1.0);
			FMTSrModel newModel(*this);
			newModel.solver.setColSolution(&*POTENTIAL_SOLUTION.cbegin());
			devArea = newModel.getArea(p_Period, p_BeforeGrowAndDeath);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::getPotentialArea", __LINE__, __FILE__);
		}
		return devArea;
	}

	FMTLpSolver* FMTSrModel::getSolverPtr()
	{
		return &solver;
	}

	const FMTLpSolver* FMTSrModel::getConstSolverPtr() const
	{
		return &solver;
	}

	void FMTSrModel::setParallelLogger(Logging::FMTLogger& logger)
	{
		try {
			solver.passInMessageHandler(logger);
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTSrModel::setParallelLogger", __LINE__, __FILE__);
			}
	}

	void FMTSrModel::passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger)
	{
		solver.passInLogger(logger);
	}

	bool FMTSrModel::boundSolution(int period, double tolerance)
	{
		try {
			if (static_cast<int>(m_graph->size()) > period)
			{
				const double* actual_solution = solver.getColSolution();
				std::vector<int>variable_index;
				std::vector<double>bounds;
				Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_iterator it, itend;

				for (boost::tie(it, itend) = m_graph->getPeriodVertices(period); it != itend; ++it)
				{
					const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor vertex_descriptor = *it;
					const std::map<int, int>variables = m_graph->getOutVariables(vertex_descriptor);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						if (std::find(variable_index.begin(), variable_index.end(), varit->second) == variable_index.end())
						{
							variable_index.push_back(varit->second);
							//Had tolerance on primal infeasibilities with FMT_DBL_TOLERANCE ...
							bounds.push_back(*(actual_solution + varit->second)*(1 - tolerance));
							bounds.push_back(*(actual_solution + varit->second)*(1 + tolerance));
						}
					}
				}
				solver.setColSetBounds(&variable_index[0], &variable_index.back() + 1, &bounds[0]);
				return solver.resolve();
				//return solver.stockResolve();
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("at period " + std::to_string(period), "FMTLpModel::boundSolution", __LINE__, __FILE__);
		}

		return false;
	}


#if defined FMTWITHR
	Rcpp::DataFrame FMTSrModel::getOutputsDataFrame(const std::vector<Core::FMTOutput>& outputsdata, int firstPeriod, int lastPeriod) const
	{
		Rcpp::DataFrame data = Rcpp::DataFrame();
		try {
			std::map<std::string, std::vector<double>>generalcatch;
			const double* solution = solver.getColSolution();
			for (int period = firstPeriod; period <= lastPeriod; ++period)
			{
				size_t outputid = 0;
				for (const Core::FMTOutput& output : outputsdata)
				{
					const std::map<std::string, double> values = m_graph->getOutput(*this, output, period, solution, Core::FMToutputlevel::developpement);
					for (std::map<std::string, double>::const_iterator it = values.begin(); it != values.end(); ++it)
					{
						if ((it->second <= -FMT_DBL_TOLERANCE) || (it->second >= FMT_DBL_TOLERANCE))
						{
							if (generalcatch.find(it->first) == generalcatch.end())
							{
								generalcatch[it->first] = std::vector<double>(outputsdata.size(), std::numeric_limits<double>::quiet_NaN());
							}
							generalcatch[it->first][outputid] = it->second;
						}

					}
					++outputid;
				}
			}
			if (!generalcatch.empty())
			{
				const size_t datasize = generalcatch.size() * outputsdata.size();
				std::vector<std::vector<std::string>>attributes(themes.size(), std::vector<std::string>(datasize));
				Rcpp::IntegerVector age(datasize);
				Rcpp::IntegerVector lock(datasize);
				Rcpp::IntegerVector period(datasize);
				Rcpp::StringVector scenario(datasize);
				Rcpp::StringVector outputsvariables(datasize);
				Rcpp::NumericVector outputsvalues(datasize, std::numeric_limits<double>::quiet_NaN());
				size_t devid = 0;
				size_t totalid = 0;
				for (std::map<std::string, std::vector<double>>::const_iterator it = generalcatch.begin(); it != generalcatch.end(); ++it)
				{
					std::vector<std::string>devdata;
					boost::split(devdata, it->first, boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
					devdata.pop_back();
					const int periodvalue = std::stoi(devdata.back());
					devdata.pop_back();
					const int lockvalue = std::stoi(devdata.back());
					devdata.pop_back();
					const int agevalue = std::stoi(devdata.back());
					devdata.pop_back();
					const std::string scenarioname = getName();
					size_t outid = 0;
					for (const Core::FMTOutput& output : outputsdata)
					{
						size_t atid = 0;
						for (const std::string& attribute : devdata)
						{
							attributes[atid][totalid] = attribute;
							++atid;
						}
						age[totalid] = agevalue;
						lock[totalid] = lockvalue;
						period[totalid] = periodvalue;
						scenario[totalid] = scenarioname;
						outputsvalues[totalid] = it->second.at(outid);
						outputsvariables[totalid] = output.getName();
						++outid;
						++totalid;
					}
					++devid;
				}
				generalcatch.clear();
				size_t themeid = 1;
				for (const std::vector<std::string>& attributevalues : attributes)
				{
					const std::string colname = "THEME" + std::to_string(themeid);
					Rcpp::StringVector Rattributes(attributevalues.size());
					std::copy(attributevalues.begin(), attributevalues.end(), Rattributes.begin());
					data.push_back(Rattributes, colname);
					++themeid;
				}
				data.push_back(age, "AGE");
				data.push_back(lock, "LOCK");
				data.push_back(period, "PERIOD");
				data.push_back(scenario, "SCENARIO");
				data.push_back(outputsvariables, "OUTPUT");
				data.push_back(outputsvalues, "VALUE");
				data.attr("row.names") = Rcpp::seq(1, age.size());
			}
			data.attr("class") = "data.frame";
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSrModel::getOutputsDataFrame", __LINE__, __FILE__);
		}
		return data;
	}

#endif 

	std::unique_ptr<FMTModel>FMTSrModel::clone() const
	{
		return std::unique_ptr<FMTModel>(new FMTSrModel(*this));
	}

	std::vector<Models::FMTsolverinterface> FMTSrModel::getAvailableSolverInterface()
	{
		std::vector<Models::FMTsolverinterface> interfaces;
		interfaces.push_back(Models::FMTsolverinterface::CLP);
		#ifdef FMTWITHMOSEK
			interfaces.push_back(Models::FMTsolverinterface::MOSEK);
		#endif
		return interfaces;
	}

	bool FMTSrModel::setParameter(const FMTboolmodelparameters& key, const bool& value)
	{
		try {
			FMTModel::setParameter(key, value);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSrModel::setParameter", __LINE__, __FILE__);
		}
		return true;
	}

	bool FMTSrModel::setParameter(const FMTintmodelparameters& key, const int& value)
	{
		try {
			FMTModel::setParameter(key, value);
			if (key == NUMBER_OF_THREADS)
			{
				solver.setNumberOfThreads(parameters.getIntParameter(NUMBER_OF_THREADS));
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSrModel::setParameter", __LINE__, __FILE__);
		}
		return true;
	}

	std::set<Core::FMTSerie>FMTSrModel::getRotations(const Core::FMTMask& mask, const std::string& aggregate) const
	{
		std::set<Core::FMTSerie>rotations;
		try {
			rotations =  m_graph->getRotations(*this, mask, aggregate);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSrModel::getRotations", __LINE__, __FILE__);
		}
		return rotations;
	}

	std::vector<const Core::FMTDevelopment*> FMTSrModel::getNoChoice(const Core::FMTMask& base_mask) const
	{
		std::vector<const Core::FMTDevelopment*>devs;
		try {
			const int death_id = static_cast<int>(actions.size() - 1);
			devs = m_graph->noChoice(base_mask, death_id);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSrModel::getNoChoice", __LINE__, __FILE__);
			}
		return devs;
	}


}



BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTSrModel)
#endif
