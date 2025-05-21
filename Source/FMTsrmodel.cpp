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

	void FMTsrmodel::setNodeCacheSize(const size_t& p_size)
	{
		_logger->logwithlevel("Node Cache size for " + getname() + " of " + std::to_string(p_size) + "\n", 1);
		m_graph->setNodeSize(p_size);
	}

	Graph::FMTgraphvertextoyield FMTsrmodel::getGraphVertexToYield() const
	{
		try {
			return Graph::FMTgraphvertextoyield(*this,*m_graph,nullptr);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsrmodel::getGraphVertexToYield", __LINE__, __FILE__);
			}
		return Graph::FMTgraphvertextoyield();
	}


	Graph::FMTgraphstats FMTsrmodel::initializematrix()
	{
		Graph::FMTgraphstats stats;
		try {
			const int ncols = static_cast<int>(m_graph->nedges());
			const std::vector<int>column_Starts(static_cast<size_t>(ncols) + 1, 0);
			const std::vector<int>targetrows(static_cast<size_t>(ncols), 0);
			const std::vector<double>nelements(ncols, 0.0);
			std::vector<double>lower_bounds(ncols, 0.0);
			std::vector<double>upper_bounds(ncols, 0.0);
			m_graph->getinitialbounds(lower_bounds, upper_bounds);
			solver.addCols(ncols, &column_Starts[0], &targetrows[0],
				&nelements[0], &lower_bounds[0],
				&upper_bounds[0], &nelements[0]);
			solver.setColSolution(&lower_bounds[0]);
			stats = m_graph->getstats();
			stats.cols = ncols;
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTsrmodel::initializematrix", __LINE__, __FILE__);
		}
		return stats;
	}

	bool FMTsrmodel::isperiodbounded(int period) const
	{
		try {
			if (static_cast<int>(m_graph->size()) > period)
			{
				size_t totaledges = 0;
				size_t boundededges = 0;
				const double* columnsupperbounds = solver.getColUpper();
				const double* columnslowerbounds = solver.getColLower();
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator it, itend;
				for (boost::tie(it, itend) = m_graph->getperiodverticies(period); it != itend; ++it)
				{
					const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor& vertex_descriptor = *it;
					const std::map<int, int>variables = m_graph->getoutvariables(vertex_descriptor);
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
			_exhandler->raisefromcatch("at period " + std::to_string(period),
				"FMTsrmodel::isperiodbounded", __LINE__, __FILE__);
		}
		return false;
	}

	bool FMTsrmodel::unboundsolution(int period)
	{
		try {
			if (static_cast<int>(m_graph->size()) > period && period > 0)//period >0 to not select actual developments!
			{
				std::vector<int>variable_index;
				std::vector<double>bounds;
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator it, itend;
				for (boost::tie(it, itend) = m_graph->getperiodverticies(period); it != itend; ++it)
				{
					const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vertex_descriptor = *it;
					std::map<int, int>variables = m_graph->getoutvariables(vertex_descriptor);
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
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTsrmodel::unboundsolution", __LINE__, __FILE__);
		}


		return false;
	}

	std::unique_ptr<FMTmodel>FMTsrmodel::getcopy(int period) const
	{
		try {
			return std::unique_ptr<FMTmodel>(new FMTsrmodel(*this, solver.getsolvertype()));
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::getcopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
	}


	bool FMTsrmodel::forcesolution(int period, const Core::FMTschedule& proportionschedulewithlock)
	{
		try
		{
			const double tolerance = getparameter(FMTdblmodelparameters::TOLERANCE);
			if(!proportionschedulewithlock.douselock())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
												"This function can only be used with schedules using lock", 
												"FMTsrmodel::forcesolution", __LINE__, __FILE__);
			}
			if (static_cast<int>(m_graph->size()) > period && period > 0)
			{
				std::vector<Core::FMTaction>::const_iterator cit = std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator("_DEATH"));
				const int deathid = static_cast<int>(std::distance(actions.cbegin(), cit));
				const double* actual_solution = solver.getColSolution();
				const boost::unordered_set<Core::FMTlookup<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor, Core::FMTdevelopment>> lookup = m_graph->getdevsset(period);
				//Copy de la solution donc les p�riodes pr�c�dents car c'est elle qu'on veut... Le reste on le scrap ?!
				std::vector<double>new_solution(actual_solution, actual_solution + solver.getNumCols());
				//Contient la proportion d'area qui rentre dans le vertex qui doit resortir dans la variable... Donc 200 ha dans le vertex rentre.. 222,0.1 il y a 20 ha qui ressort dans la variable 222
				boost::unordered_map<int,double> varproportions;
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				const bool typeII = (getparameter(Models::FMTintmodelparameters::MATRIX_TYPE) == 2);
				for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					if (typeII&&m_graph->isnotransfer(*vertex_iterator, 1))//Dont need to set to 0 global growth variable.
						{
						continue;
						}
					const std::map<int, int>variables = m_graph->getoutvariables(*vertex_iterator);

					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						new_solution[varit->second] = 0;
					}
				}
				//Getoutvariables proportions for schedule...
				//Container pour deduire le growht � partir de ce qui est plac� dans les actions par dev
				std::map<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor,double> growthordeathdeductor;
				for (int actionid = 0; actionid < static_cast<int>(actions.size()); ++actionid)
				{
					const auto& actionit = proportionschedulewithlock.find(actions.at(actionid));
					if (actionit != proportionschedulewithlock.end())
					{
						for (const auto& devit : actionit->second)
						{
							if(m_graph->containsdevelopment(devit.first, lookup))
							{
								const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = m_graph->getdevelopment(devit.first, lookup);
								if(growthordeathdeductor.find(vdescriptor)==growthordeathdeductor.end())
								{
									growthordeathdeductor.emplace(vdescriptor,1-devit.second.at(0));
								}else{
									growthordeathdeductor[vdescriptor]-=devit.second.at(0);
								}
								std::map<int, int> outvariables = m_graph->getoutvariables(vdescriptor);
								std::map<int, int>::const_iterator varit = outvariables.find(actionid);
								if (varit == outvariables.cend())
								{
									_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
												"Developement " + std::string(devit.first) + " is not operable "
												" for action " + actionit->first.getname(), 
												"FMTsrmodel::forcesolution", __LINE__, __FILE__);
								}
								//std::cout<<std::string(devit.first)<<" "+std::to_string(devit.second.at(0))<<" "+std::to_string(actionid)<<std::endl;//" "+this->getactions().at(actionid).getname()<<std::endl;
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
					const std::map<int, int> outvariables = m_graph->getoutvariables(vdescriptor_props.first);
					std::map<int, int>::const_iterator varit = outvariables.find(-1);
					if (varit == outvariables.cend())
					{
						varit = outvariables.find(deathid);
						if(varit == outvariables.cend())
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
									"Developement "+std::string(m_graph->getdevelopment(vdescriptor_props.first))+" cannot grow or die ...", 
									"FMTsrmodel::forcesolution", __LINE__, __FILE__);
						}
					}
					//Pas besoin de setter de growth ou death a 0%
					if(vdescriptor_props.second>0)
					{
						//std::cout<<vdescriptor_props.second<<std::endl;
						varproportions.emplace(varit->second,vdescriptor_props.second);
					}
				}
				//setsolution by proportions
				//keep track of variables setted 
				std::set<int>processedvariables;
				boost::unordered_set<Core::FMTdevelopment> processed;
				//store all out vertex out of variables set to process in queue
				std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>descriptors;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					//since it's comming from growth and nothing else we know the area
					if (m_graph->periodstart(*vertex_iterator))//get inperiod
					{
						const std::map<int,int> inidsvars = m_graph->getinidsvariables(*vertex_iterator);
						const bool setrest = !(typeII&&m_graph->isnotransfer(*vertex_iterator, 1));
						if(inidsvars.size()>1)
						{
							//Because what comes from previous period must have inarea ... will be set later 
							processedvariables.emplace(inidsvars.at(-1));
							descriptors.push(*vertex_iterator);
						}else{
							const double* solution = &new_solution[0];
							const double inarea = m_graph->inarea(*vertex_iterator, solution);
							if (inarea >0)
							{
								double outarea = 0;
								std::map<int, int>variables = m_graph->getoutvariables(*vertex_iterator);
								bool foundoutvar = false;
								for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
								{
									if(varproportions.find(varit->second)!=varproportions.end())
									{
										foundoutvar = true;
										std::vector<Core::FMTdevelopmentpath> paths = m_graph->getpaths(*vertex_iterator, varit->first);
										for (const Core::FMTdevelopmentpath path : paths)
										{
											if (path.getDevelopment().getperiod() == period && processed.find(path.getDevelopment()) == processed.end())
											{
												processed.emplace(path.getDevelopment());
												Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor nextdescriptor = m_graph->getdevelopment(path.getDevelopment(), lookup);
												descriptors.push(nextdescriptor);
												
											}
										}
										//std::cout<<std::string(m_graph->getdevelopment(*vertex_iterator)) << " "+std::to_string(varproportions[varit->second])<<" "+std::to_string(inarea)<<" "+std::to_string(varit->first)<<std::endl;//<<" "+this->getactions().at(varit->first).getname()<<std::endl;
										double proportion = 1;
										if (setrest)
											{
											proportion = varproportions[varit->second];
											}
										const double outvararea = proportion*inarea;
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
										new_solution[variables.at(targetaction)] = inarea;
									}
									outarea+=inarea;
								}
								//Valider ?! pas sur ...
								if (inarea - tolerance > outarea )
								{
									_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
										"Negative growth solution for " +
										std::string(m_graph->getdevelopment(*vertex_iterator)) +" in area of " + std::to_string(inarea) + 
										" and out area of " + std::to_string(outarea) ,
										"FMTsrmodel::forcesolution", __LINE__, __FILE__);
								}
								if (inarea + tolerance < outarea )
								{
									_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
										"Positive growth solution for " +
										std::string(m_graph->getdevelopment(*vertex_iterator)) +" in area of " + std::to_string(inarea) + 
										" and out area of " + std::to_string(outarea) ,
										"FMTsrmodel::forcesolution", __LINE__, __FILE__);
								}
							}
						}
					}
				}
				size_t passwithoutprocess=0;
				while(!descriptors.empty())
				{
					const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor first = descriptors.front();
					//Cette portion est pour valider que toutes les variables qui rentrent dans le vertex on �t� setter par la fonction
					//sinon on le remet a la fin de la queue 
					const std::vector<int>invariables = m_graph->getinvariables(first);
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
					const bool setrest = !(typeII&&m_graph->isnotransfer(first, 1));
					if	(canprocess)
					{
						passwithoutprocess=0;
						//Check ce bout la ... 
						const double* solution = &new_solution[0];
						const double inarea = m_graph->inarea(first, solution);
						double outarea = 0;
						std::map<int, int>variables = m_graph->getoutvariables(first);
						bool foundoutvar = false;
						for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
						{
							if(varproportions.find(varit->second)!=varproportions.end())
							{
								foundoutvar = true;
								std::vector<Core::FMTdevelopmentpath> paths = m_graph->getpaths(first, varit->first);
								for (const Core::FMTdevelopmentpath path : paths)
								{
									if (path.getDevelopment().getperiod() == period && processed.find(path.getDevelopment()) == processed.end())
									{
										processed.emplace(path.getDevelopment());
										descriptors.push(m_graph->getdevelopment(path.getDevelopment(), lookup));
									}
								}
								//std::cout<<std::string(m_graph->getdevelopment(first)) << " "+std::to_string(varproportions[varit->second])<<" "+std::to_string(inarea)<<" "+std::to_string(varit->first)<<std::endl;//<<" "+this->getactions().at(varit->first).getname()<<std::endl;
								double proportion = 1;
								if (setrest)
								{
									proportion = varproportions[varit->second];
								}
								const double outvararea = proportion *inarea;
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
								new_solution[variables.at(targetaction)] = inarea;
							}
							outarea+=inarea;
						}
						//Valider ?! pas sur ...
						if (inarea - tolerance > outarea )
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
								"Negative growth solution for " +
								std::string(m_graph->getdevelopment(first)) +" in area of " + std::to_string(inarea) +
								" and out area of " + std::to_string(outarea) ,
								"FMTsrmodel::forcesolution", __LINE__, __FILE__);
						}
						if (inarea + tolerance < outarea )
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
								"Positive growth solution for " +
								std::string(m_graph->getdevelopment(first)) +" in area of " + std::to_string(inarea) +
								" and out area of " + std::to_string(outarea) ,
								"FMTsrmodel::forcesolution", __LINE__, __FILE__);
						}
						descriptors.pop();
					}else{
						++passwithoutprocess;
						if(passwithoutprocess == descriptors.size())
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
								"Wrong variable",
								"FMTsrmodel::forcesolution", __LINE__, __FILE__);
						}
						descriptors.pop();
						descriptors.push(first);
						//handle pour ne pas avoir de boucle infini et passer ceux qui sont r�cursif sur la meme action genre boucle infini ... valider aussi comment on g�re �a dans FMTgraph
					}
				}
				solver.setColSolution(&new_solution[0]);

			}
		}catch(...){
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTsrmodel::forcesolution", __LINE__, __FILE__);
		}
		return true;

	}


	bool FMTsrmodel::setsolution(int period, const Core::FMTschedule& schedule, double tolerance)
	{
		try {
			const bool WILL_THROW = parameters.getboolparameter(FMTboolmodelparameters::SETSOLUTION_THROW);
			if (static_cast<int>(m_graph->size()) > period && period > 0)
			{
				std::vector<Core::FMTaction>::const_iterator cit = std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator("_DEATH"));
				const int deathid = static_cast<int>(std::distance(actions.cbegin(), cit));
				const double* actual_solution = solver.getColSolution();
				const boost::unordered_set<Core::FMTlookup<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor, Core::FMTdevelopment>> lookup = m_graph->getdevsset(period);
				std::vector<double>new_solution(actual_solution, actual_solution + solver.getNumCols());
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				const bool typeII = (getparameter(Models::FMTintmodelparameters::MATRIX_TYPE)==2);
				for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					if (typeII&&m_graph->isnotransfer(*vertex_iterator,1))//Dont need to set to 0 global growth variable.
						{
						continue;
						}
					const std::map<int, int>variables = m_graph->getoutvariables(*vertex_iterator);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						new_solution[varit->second] = 0;
					}
				}
				int maximallock = -1;
				for (int actionid = 0; actionid < static_cast<int>(actions.size()); ++actionid/*const auto& actionit : schedule*/)
				{

					//int actionid = int(std::distance(actions.begin(), std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(actionit.first.getname()))));
					const auto& actionit = schedule.find(actions.at(actionid));
					if (actionit != schedule.end())
					{
						//*_logger << "processinh " << actionit->first.getname() << "\n";
						size_t allocated = 0;
						for (const auto& devit : actionit->second)
						{
							if (((schedule.douselock() || actionit->first.dorespectlock()) && m_graph->containsdevelopment(devit.first, lookup)))
							{
								//*_logger << "t1 " << actionit->first.getname() << "\n";
								const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = m_graph->getdevelopment(devit.first, lookup);
								//const int variable = m_graph->getoutvariables(vdescriptor)[actionid];
								std::map<int, int> outvariables = m_graph->getoutvariables(vdescriptor);
								std::map<int, int>::const_iterator varit = outvariables.find(actionid);
								if (varit == outvariables.cend())
								{
									if (WILL_THROW)
									{
									_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
										"Developement " + std::string(devit.first) + " is not operable "
										" for action " + actionit->first.getname(),
										"FMTsrmodel::setsolution", __LINE__, __FILE__);
									}
									else 
									{
										return false;
									}
								}
								const int variable = varit->second;
								new_solution[variable] = devit.second.at(0);
								++allocated;
							}
							else if (!schedule.douselock() && !actionit->first.dorespectlock())
							{
								//*_logger << "t2 " << actionit->first.getname() << "\n";
								if (maximallock == -1)
								{
									maximallock = m_graph->getmaximalock(period);
								}
								std::vector<double>lockstoadress(devit.second);
								std::vector<std::pair<Core::FMTdevelopment, double>>locksfound;
								std::vector<std::pair<int, size_t>>locksorter;
								Core::FMTdevelopment locked(devit.first);
								for (int lockid = 0; lockid <= maximallock; ++lockid)
								{
									locked.setlock(lockid);
									if (m_graph->containsdevelopment(locked, lookup))
									{
										const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = m_graph->getdevelopment(locked, lookup);
										const double* newsolutionptr = &new_solution[0];
										double originalinarea = m_graph->inarea(vdescriptor, newsolutionptr, true);
										if (originalinarea == 0)
										{
											originalinarea = std::numeric_limits<double>::max();
										}
										if (!(m_graph->onlypertiodstart(vdescriptor) && originalinarea == std::numeric_limits<double>::max()))
										{
											locksorter.push_back(std::pair<size_t, size_t>(locksfound.size(), m_graph->getamountofpaths(locked, -1, *this, lookup)));
											locksfound.push_back(std::pair<Core::FMTdevelopment, double>(locked, originalinarea));

										}
									}
								}
								//*_logger << "got " << locksfound.size() << " for " << actionit->first.getname() << "\n";
								std::sort(locksorter.begin(),
									locksorter.end(),
									[](const std::pair<size_t, size_t>& a,
										const std::pair<size_t, size_t>& b) {return a.second < b.second; });

								std::vector<std::pair<Core::FMTdevelopment, double>>sortedlocksfound;
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
										for (const std::pair<Core::FMTdevelopment, double>& element : locksfound)
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
											for (const std::pair<Core::FMTdevelopment, double>& element : locksfound)
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
											const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = m_graph->getdevelopment(locksfound.at(id).first, lookup);
											//const int variable = m_graph->getoutvariables(vdescriptor)[actionid];
											std::map<int, int> outvariables = m_graph->getoutvariables(vdescriptor);
											std::map<int, int>::const_iterator varit = outvariables.find(actionid);
											if (varit == outvariables.cend())
											{
												if (WILL_THROW)
												{
												_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
													"Developement " + std::string(devit.first) + " is not operable "
													" for action " + actionit->first.getname(),
													"FMTsrmodel::setsolution", __LINE__, __FILE__);
												}
												else
												{
													return false;
												}
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
											if (WILL_THROW)
											{
												_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
													"Cannot allocate area of " + std::to_string(areatoput) + " to " +
													std::string(devit.first) + " for action " + actionit->first.getname(), "FMTsrmodel::setsolution", __LINE__, __FILE__);
											}
											else
											{
												return false;
											}
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
								_exhandler->raise(Exception::FMTexc::FMTmissingdevelopement, std::string(devit.first) + " at period " + std::to_string(period) + " operated by " + actionit->first.getname(),
									"FMTsrmodel::setsolution", __LINE__, __FILE__);
								return false;
							}
						}
					}
				}
				//Fill up natural evolution
				boost::unordered_map<Core::FMTdevelopment, Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>processed;
				std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>descriptors;
				//Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					if (m_graph->periodstart(*vertex_iterator))//get inperiod
					{
						const double* solution = &new_solution[0];
						double rest = m_graph->inarea(*vertex_iterator, solution);
						//double rest = m_graph->inarea(devit->second, actual_solution);
						std::map<int, int>variables = m_graph->getoutvariables(*vertex_iterator);
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
						const bool setrest = !(typeII&&m_graph->isnotransfer(*vertex_iterator, 1));
						for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
						{
							std::vector<Core::FMTdevelopmentpath> paths = m_graph->getpaths(*vertex_iterator, varit->first);
							for (const Core::FMTdevelopmentpath path : paths)
							{
								if (path.getDevelopment().getperiod() == period && processed.find(path.getDevelopment()) == processed.end())
								{
									processed[path.getDevelopment()] = m_graph->getdevelopment(path.getDevelopment(), lookup);
									descriptors.push(m_graph->getdevelopment(path.getDevelopment(), lookup));
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
								actionnames += actions.at(varit->first).getname() + ",";
							}
							actionnames.pop_back();
							const Core::FMTdevelopment dev(m_graph->getdevelopment(*vertex_iterator));
							const double* solution = &new_solution[0];
							const double inarea = m_graph->inarea(*vertex_iterator, solution);
							std::string locking;
							if (dev.getlock() > 0)
							{
								Core::FMTdevelopment locked(dev);
								locking += " lock(";
								for (int locklevel = 0; locklevel < 30; ++locklevel)
								{
									locked.setlock(locklevel);
									if (m_graph->containsdevelopment(locked, lookup))
									{
										locking += std::to_string(locklevel) + ",";
									}
								}
								locking.pop_back();
								locking += ")";
							}
							if (WILL_THROW)
							{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
								std::to_string(rest) + " negative growth solution for " +
								std::string(dev) + " operated by " + actionnames + locking + " in area " + std::to_string(inarea),
								"FMTsrmodel::setsolution", __LINE__, __FILE__);
							}
							else {
								return false;
							}
						}
						if ((targetaction < 0) && setrest)//Ajust only natural growth and not _DEATH
						{
							new_solution[growth] = rest;
						}

					}
				}
				while (!descriptors.empty())
				{
					Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor first = descriptors.front();
					std::map<int, int>variables = m_graph->getoutvariables(first);
					const double* solution = &new_solution[0];
					double rest = m_graph->inarea(first, solution);
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
						std::vector<Core::FMTdevelopmentpath> paths = m_graph->getpaths(first, varit->first);
						for (const Core::FMTdevelopmentpath path : paths)
						{
							if (path.getDevelopment().getperiod() == period && processed.find(path.getDevelopment()) == processed.end())
							{
								processed[path.getDevelopment()] = m_graph->getdevelopment(path.getDevelopment(), lookup);
								descriptors.push(m_graph->getdevelopment(path.getDevelopment(), lookup));
							}
						}

						rest -= new_solution[varit->second];
					}
					if ((rest + tolerance) < 0)
					{
						if (WILL_THROW)
						{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
							std::to_string(rest) + " negative growth solution for " +
							std::string(m_graph->getdevelopment(first)),
							"FMTsrmodel::setsolution", __LINE__, __FILE__);
						}
						else
						{
							return false;
						}
					}
					const bool setrest = !(typeII&&m_graph->isnotransfer(first, 1));
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
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTsrmodel::setsolution", __LINE__, __FILE__);
		}
		return true;
	}

	std::set<std::string> FMTsrmodel::getAllMasks(const std::vector<Core::FMTtheme>& p_selectedThemes) const {
		std::set<std::string> masks;
		try {
			std::vector<Core::FMTtheme> toIgnore;
			for (const Core::FMTtheme& THEME : themes) {
				if (std::find_if(p_selectedThemes.begin(), p_selectedThemes.end(), Core::FMTthemecomparator(THEME)) == p_selectedThemes.end()) {
					toIgnore.push_back(THEME);
				}
			}
			masks = m_graph->getAllDevelopmentsMask(toIgnore);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::getAllMasks", __LINE__, __FILE__);
		}
		return masks;
	}

	bool FMTsrmodel::setsolutionbylp(int period, const Core::FMTschedule& schedule, double tolerance)
	{
		try {
			if (Graph::FMTgraphbuild::schedulebuild != m_graph->getbuildtype())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot set solution by lp on a non partial graph",
					"FMTsrmodel::setsolutionbylp", __LINE__, __FILE__);
			}
			if (static_cast<int>(m_graph->size()) > period && period > 0)
			{
				const boost::unordered_set<Core::FMTlookup<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor, Core::FMTdevelopment>> lookup = m_graph->getdevsset(period);
				std::map<int, std::pair<double, double>>bounds;
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const std::map<int, int>variables = m_graph->getoutvariables(*vertex_iterator);
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
							if ((schedule.douselock() || actionit->first.dorespectlock()) && m_graph->containsdevelopment(devit.first, lookup))
							{
								const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = m_graph->getdevelopment(devit.first, lookup);
								//const int variable = m_graph->getoutvariables(vdescriptor)[actionid];
								std::map<int, int> outvariables = m_graph->getoutvariables(vdescriptor);
								std::map<int, int>::const_iterator varit = outvariables.find(actionid);
								if (varit == outvariables.cend())
								{
									_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
												"Developement " + std::string(devit.first) + " is not operable "
												" for action " + actionit->first.getname(), 
												"FMTsrmodel::setsolutionbylp", __LINE__, __FILE__);
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
							else if (!schedule.douselock() && !actionit->first.dorespectlock())
							{
								if (maximallock == -1)
								{
									maximallock = m_graph->getmaximalock(period);
								}
								Core::FMTdevelopment locked(devit.first);
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
									locked.setlock(lockid);
									if (m_graph->containsdevelopment(locked, lookup))
									{
										const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = m_graph->getdevelopment(locked, lookup);
										//const int variable = m_graph->getoutvariables(vdescriptor)[actionid];
										std::map<int, int> outvariables = m_graph->getoutvariables(vdescriptor);
										std::map<int, int>::const_iterator varit = outvariables.find(actionid);
										if (varit == outvariables.cend())
										{
											_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
														"Developement " + std::string(devit.first) + " is not operable "
														" for action " + actionit->first.getname(), 
														"FMTsrmodel::setsolutionbylp", __LINE__, __FILE__);
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
										"Cannot allocate any developements for action " + std::string(actionit->first.getname()) +
										" at period " + std::to_string(period),
										"FMTsrmodel::setsolutionbylp", __LINE__, __FILE__);
								}
							}
							else {
								_exhandler->raise(Exception::FMTexc::FMTmissingdevelopement, std::string(devit.first) + " at period " + std::to_string(period) + " operated by " + actionit->first.getname(),
									"FMTsrmodel::setsolutionbylp", __LINE__, __FILE__);
								return false;
							}
						}
						//*_logger << "area set of " << actionareaset  <<" set for "<< actionit->first.getname() << "\n";

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
					solver.stockresolve();
				}
				else {
					solver.initialsolve();
				}

				if (!solver.isProvenOptimal())
				{
					/*for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const std::map<int, int>variables = m_graph->getoutvariables(*vertex_iterator);
						for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
						{

						}
					}*/
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Infeasible schedule at period " + std::to_string(period),
						"FMTsrmodel::setsolutionbylp", __LINE__, __FILE__);
				}
				const double* solution = solver.getColSolution();
				double scheduleobjective = 0;
				for (const int& var : variables)
				{
					scheduleobjective += *(solution + var);
				}
				const std::string areacomparison = "schedule area of " + std::to_string(schedule.area()) + " vs " + std::to_string(scheduleobjective)+"\n";
				_logger->logwithlevel(areacomparison, 1);
				std::vector<double>varsconstraint(bounds.size(), 1.0);
				solver.addRow(static_cast<int>(varsconstraint.size()), &variables[0],
					&varsconstraint[0], std::min(scheduleobjective - tolerance, schedule.area()));
				if (!solver.stockresolve())
				{
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Infeasible on resolve " + std::to_string(period),
						"FMTlsrmodel::setsolutionbylp", __LINE__, __FILE__);
				}

			}
		}
		catch (...)
		{
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTsrmodel::setsolutionbylp", __LINE__, __FILE__);
		}


		return true;
	}

	FMTsrmodel::FMTsrmodel(FMTmodel&& base, const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& lgraph,
		const FMTlpsolver& lsolver) :
		FMTmodel(base),
		m_graph(new Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>(lgraph)),
		solver(lsolver)
	{
		//solver.passinobject(base);
		//m_graph->passinobject(base);
	}

	FMTsrmodel::FMTsrmodel(FMTsrmodel&& rhs) noexcept :
		FMTmodel(),
		m_graph(new Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>()),
		solver()
	{
		*this = std::move(rhs);
	}

	FMTsrmodel& FMTsrmodel::operator = (const FMTsrmodel& rhs)
	{
		if (this != &rhs)
		{
			FMTmodel::operator=(rhs);
			m_graph = std::move(std::unique_ptr<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>>(new 
								Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>(*rhs.m_graph)));
			solver=rhs.solver;
		}
		return *this;
	}

	FMTsrmodel& FMTsrmodel::operator =(FMTsrmodel&& rhs) noexcept
	{
		if (this!=&rhs)
		{
			FMTmodel::operator=(std::move(rhs));
			//graph=rhs.graph;//Boost graph does not provide real swap so...
			m_graph.swap(rhs.m_graph);
			solver.swap(rhs.solver);
		}
		return *this;
	}


	void FMTsrmodel::postsolve(const FMTmodel& originalbasemodel)
	{
		try {
			postsolvegraph(originalbasemodel);
			FMTmodel::postsolve(originalbasemodel);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::postsolve", __LINE__, __FILE__);
		}

	}

	bool FMTsrmodel::isoptimal() const
	{
		try {
			if (m_graph->getbuildtype() == Graph::FMTgraphbuild::fullbuild)//Only global Graph
			{
				return solver.isProvenOptimal();
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::isoptimal", __LINE__, __FILE__);
		}
		return false;
	}
	
	double FMTsrmodel::getobjectivevalue() const
	{
		double value = std::numeric_limits<double>::quiet_NaN();
		try {
			value = solver.getObjValue();
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsrmodel::getobjectivevalue", __LINE__, __FILE__);
			}
		return value;
	}



	std::unique_ptr<FMTmodel>FMTsrmodel::presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
	{
		try{
			if (!m_graph->empty())
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Cannot presolve a srmodel with period(s) builded in m_graph->",
					"FMTsrmodel::presolve", __LINE__, __FILE__);
				}
			return std::unique_ptr<FMTmodel>(new FMTsrmodel(std::move(*FMTmodel::presolve(optionaldevelopments)),*m_graph,solver));
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::presolve", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
	}


	Core::FMTschedule FMTsrmodel::getsolution(int period, bool withlock) const
	{
		Core::FMTschedule newschedule;
		try
		{
			//setparameter(SHOW_LOCK_IN_SCHEDULES,withlock);
			const double* actual_solution = solver.getColSolution();
			newschedule = m_graph->getschedule(actions, actual_solution, period, withlock);//getparameter(SHOW_LOCK_IN_SCHEDULES));

		}
		catch (...)
		{
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTsrmodel::getsolution", __LINE__, __FILE__);
		}
		return newschedule;
	}

	Core::FMTschedule FMTsrmodel::getscheduleproportions(int period, bool withlock) const
	{
		Core::FMTschedule newschedule;
		try
		{
			const double* actual_solution = solver.getColSolution();
			newschedule = m_graph->getoutvariablesproportions(actions, actual_solution, period, withlock);//getparameter(SHOW_LOCK_IN_SCHEDULES));
		}
		catch (...)
		{
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTsrmodel::getscheduleproportions", __LINE__, __FILE__);
		}
		return newschedule;
	}

	Graph::FMTgraphstats FMTsrmodel::updatematrix(const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_pair& targets,
		const Graph::FMTgraphstats& newstats)
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
			Graph::FMTgraphstats oldstats = m_graph->getstats();
			int newconstraintID = solver.getNumRows();
			size_t periodsize = 0;
			Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator it, itend;
			const bool typeII = (getparameter(Models::FMTintmodelparameters::MATRIX_TYPE) == 2);
			for (boost::tie(it, itend) = targets; it != itend; ++it)
			{
				if (typeII&&m_graph->isnotransfer(*it,1))//We dont need a transfer Row here
				{
					//Type II behavior
				}else {//Else add a new transfer row.
					m_graph->setconstraintID(*it, newconstraintID);
					if (m_graph->gettransferrow(*it, row_Starts, targetcols, elements))
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
			_exhandler->raisefromcatch("", "FMTsrmodel::updatematrix", __LINE__, __FILE__);
		}
		return this->getstats();
	}


	FMTsrmodel::FMTsrmodel(const FMTmodel& base, FMTsolverinterface lsolvertype) :
		FMTmodel(base),
		m_graph(new Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>(Graph::FMTgraphbuild::nobuild)),
		solver(lsolvertype,
			base.getparameter(FMTstrmodelparameters::SOLVER_COLD_START),
			base.getparameter(FMTstrmodelparameters::SOLVER_WARM_START),
			base.getname())
	{
		//solver.passinobject(base);
		//m_graph->passinobject(base);

	}

	FMTsrmodel::FMTsrmodel(const FMTsrmodel& rhs):
		FMTmodel(rhs),
		m_graph(new Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>(*rhs.m_graph)),
		solver(rhs.solver)
	{
		//solver.passinobject(rhs);
		//m_graph->passinobject(rhs);
	}

	FMTsrmodel::FMTsrmodel() :
		FMTmodel(),
		m_graph(new Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>(Graph::FMTgraphbuild::nobuild)),
		solver()
	{

	}



	bool FMTsrmodel::operator == (const FMTsrmodel& rhs) const
	{
		return (FMTmodel::operator == (rhs) &&
			solver == (rhs.solver) &&
			*m_graph == *rhs.m_graph);
	}

	bool FMTsrmodel::operator != (const FMTsrmodel& rhs) const
	{
		return !(*this == rhs);
	}

	std::map<std::string, double> FMTsrmodel::getoutput(const Core::FMToutput& output, int period, Core::FMToutputlevel level) const
	{
		try {
			const double* solution = solver.getColSolution();
			if (!output.isvariablesizeof(area.begin()->getmask().size()))
				{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
					"For output " + std::string(output.getname()),
					"FMTsrmodel::getoutput", __LINE__, __FILE__);
				}
			return m_graph->getoutput(*this, output, period, solution, level);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::getoutput", __LINE__, __FILE__);
		}

		return std::map<std::string, double>();
	}

	std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> FMTsrmodel::getActives()
	{
		std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> actives;
		try {
			if (m_graph->empty())
			{
				actives = m_graph->initialize(area);
				Graph::FMTgraphstats BUILD_STATS = initializematrix();
				m_graph->setstats(BUILD_STATS);
			}
			else {
				actives = m_graph->getactiveverticies();
			}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::getActives", __LINE__, __FILE__);
		}
		return actives;
	}



	Graph::FMTgraphstats FMTsrmodel::buildperiod(Core::FMTschedule schedule, bool forcepartialbuild, int compressageclassoperability)
	{
		try {
			std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> actives = getActives();
			Graph::FMTgraphstats buildstats;
			if (!forcepartialbuild && schedule.empty()) // full build
			{
				buildstats = m_graph->build(*this, actives, compressageclassoperability);
				m_graph->setbuildtype(Graph::FMTgraphbuild::fullbuild);
			}
			else {//partial build for result
				m_graph->setbuildtype(Graph::FMTgraphbuild::schedulebuild);
				buildstats = m_graph->buildschedule(*this, actives, schedule);
			}
			const int location = static_cast<int>(m_graph->size() - 2);
			const Graph::FMTgraphstats newstats = this->updatematrix(m_graph->getperiodverticies(location), buildstats);
			m_graph->setstats(newstats);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::buildperiod", __LINE__, __FILE__);
		}

		return m_graph->getstats();
	}



	size_t FMTsrmodel::getgraphsize() const
	{
		return m_graph->size();
	}

	void FMTsrmodel::cleargraphdevelopements()
	{
		m_graph->cleardevelopments();
	}
	void FMTsrmodel::cleargraphcache()
	{
		m_graph->clearcache();
	}


	bool FMTsrmodel::summarize(const std::map<int, double>& variables,
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


	Graph::FMTgraphstats FMTsrmodel::getstats() const
	{
		return m_graph->getstats();
	}

	int FMTsrmodel::getfirstactiveperiod() const
	{
		return m_graph->getfirstactiveperiod();
	}

	void FMTsrmodel::postsolvegraph(const FMTmodel& originalbasemodel)
	{
		try {
			const std::vector<Core::FMTtheme>& postsolvethemes = dynamic_cast<const FMTsrmodel*>(&originalbasemodel)->themes;
			const std::vector<Core::FMTaction>& postsolveactions = dynamic_cast<const FMTsrmodel*>(&originalbasemodel)->actions;
			const Core::FMTmaskfilter postsolvefilter = this->getpostsolvefilter(originalbasemodel.getthemes(),originalbasemodel.getarea().begin()->getmask());
			const std::vector<Core::FMTaction>& presolveactions = this->actions;
			std::vector<int>actionmapping;
			actionmapping.reserve(presolveactions.size());
			for (const Core::FMTaction action : presolveactions)
			{
				const int loc = static_cast<int>(std::distance(postsolveactions.begin(), std::find_if(postsolveactions.begin(), postsolveactions.end(), Core::FMTactioncomparator(action.getname()))));
				actionmapping.push_back(loc);
			}
			this->m_graph->postsolve(postsolvefilter,postsolvethemes, actionmapping);
			
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::postsolvegraph", __LINE__, __FILE__);
		}
	}



	std::vector<Core::FMTactualdevelopment>FMTsrmodel::getarea(int period, bool beforegrowanddeath) const
	{
		std::vector<Core::FMTactualdevelopment>returnedarea;
		try {
			returnedarea.reserve(area.size());//Reserve at least the size of the initial area.
			if (period == 0)
			{
				return FMTmodel::getarea();
			};
			const double* modelsolution = solver.getColSolution();
			const int deathactionid = static_cast<int>(actions.size()-1);
			Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			if (beforegrowanddeath)
			{
				--period;
			}
			for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
				if ((!beforegrowanddeath&&m_graph->periodstart(*vertex_iterator)))
				{
					const Core::FMTdevelopment& graphdevelopement = m_graph->getdevelopment(*vertex_iterator);
					const double areaofdevelopement = m_graph->inarea(*vertex_iterator, modelsolution,-1,true);
					if (areaofdevelopement > FMT_DBL_TOLERANCE)
					{
						returnedarea.push_back(Core::FMTactualdevelopment(graphdevelopement, areaofdevelopement));
					}
				}
				else if (beforegrowanddeath)
				{
					const Core::FMTdevelopment& graphdevelopement = m_graph->getdevelopment(*vertex_iterator);
					const double indeatharea = m_graph->inarea(*vertex_iterator, modelsolution, deathactionid, true);
					const double areaofdevelopement = m_graph->outarea(*vertex_iterator, -1, modelsolution) +m_graph->outarea(*vertex_iterator, deathactionid, modelsolution) -indeatharea;
					if (areaofdevelopement > FMT_DBL_TOLERANCE)
					{
						returnedarea.push_back(Core::FMTactualdevelopment(graphdevelopement, areaofdevelopement));
					}

				}
				
			}
			std::sort(returnedarea.begin(), returnedarea.end());
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::getarea", __LINE__, __FILE__);
		}

		return returnedarea;
	}


	FMTlpsolver* FMTsrmodel::getsolverptr()
	{
		return &solver;
	}

	const FMTlpsolver* FMTsrmodel::getconstsolverptr() const
	{
		return &solver;
	}

	void FMTsrmodel::setparallellogger(Logging::FMTlogger& logger)
	{
		try {
			solver.passinmessagehandler(logger);
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsrmodel::setparallellogger", __LINE__, __FILE__);
			}
	}

	void FMTsrmodel::passinlogger(const std::unique_ptr<Logging::FMTlogger>& logger)
	{
		solver.passinlogger(logger);
	}

	bool FMTsrmodel::boundsolution(int period, double tolerance)
	{
		try {
			if (static_cast<int>(m_graph->size()) > period)
			{
				const double* actual_solution = solver.getColSolution();
				std::vector<int>variable_index;
				std::vector<double>bounds;
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator it, itend;

				for (boost::tie(it, itend) = m_graph->getperiodverticies(period); it != itend; ++it)
				{
					const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vertex_descriptor = *it;
					const std::map<int, int>variables = m_graph->getoutvariables(vertex_descriptor);
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
				//return solver.stockresolve();
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTlpmodel::boundsolution", __LINE__, __FILE__);
		}

		return false;
	}


#if defined FMTWITHR
	Rcpp::DataFrame FMTsrmodel::getoutputsdataframe(const std::vector<Core::FMToutput>& outputsdata, int firstperiod, int lastperiod) const
	{
		Rcpp::DataFrame data = Rcpp::DataFrame();
		try {
			std::map<std::string, std::vector<double>>generalcatch;
			const double* solution = solver.getColSolution();
			for (int period = firstperiod; period <= lastperiod; ++period)
			{
				size_t outputid = 0;
				for (const Core::FMToutput& output : outputsdata)
				{
					const std::map<std::string, double> values = m_graph->getoutput(*this, output, period, solution, Core::FMToutputlevel::developpement);
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
					const std::string scenarioname = getname();
					size_t outid = 0;
					for (const Core::FMToutput& output : outputsdata)
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
						outputsvariables[totalid] = output.getname();
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
			_exhandler->printexceptions("", "FMTsrmodel::getoutputsdataframe", __LINE__, __FILE__);
		}
		return data;
	}

#endif 

	std::unique_ptr<FMTmodel>FMTsrmodel::clone() const
	{
		return std::unique_ptr<FMTmodel>(new FMTsrmodel(*this));
	}

	std::vector<Models::FMTsolverinterface> FMTsrmodel::getavailablesolverinterface()
	{
		std::vector<Models::FMTsolverinterface> interfaces;
		interfaces.push_back(Models::FMTsolverinterface::CLP);
		interfaces.push_back(Models::FMTsolverinterface::MOSEK);
		return interfaces;
	}

	bool FMTsrmodel::setparameter(const FMTboolmodelparameters& key, const bool& value)
	{
		try {
			FMTmodel::setparameter(key, value);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTsrmodel::setparameter", __LINE__, __FILE__);
		}
		return true;
	}

	bool FMTsrmodel::setparameter(const FMTintmodelparameters& key, const int& value)
	{
		try {
			FMTmodel::setparameter(key, value);
			if (key == NUMBER_OF_THREADS)
			{
				solver.setnumberofthreads(parameters.getintparameter(NUMBER_OF_THREADS));
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTsrmodel::setparameter", __LINE__, __FILE__);
		}
		return true;
	}

	Graph::FMTgraphstats FMTsrmodel::getgraphstats() const
	{
		return m_graph->getstats();
	}

	std::set<Core::FMTSerie>FMTsrmodel::getRotations(const Core::FMTmask& mask, const std::string& aggregate) const
	{
		std::set<Core::FMTSerie>rotations;
		try {
			rotations =  m_graph->getRotations(*this, mask, aggregate);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTsrmodel::getrorations", __LINE__, __FILE__);
		}
		return rotations;
	}

	std::vector<const Core::FMTdevelopment*> FMTsrmodel::getnochoice(const Core::FMTmask& base_mask) const
	{
		std::vector<const Core::FMTdevelopment*>devs;
		try {
			const int death_id = static_cast<int>(actions.size() - 1);
			devs = m_graph->nochoice(base_mask, death_id);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsrmodel::getnochoice", __LINE__, __FILE__);
			}
		return devs;
	}


}



BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsrmodel)
#endif
