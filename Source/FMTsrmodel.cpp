/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTsrmodel.hpp"
#ifdef FMTWITHMOSEK
	#include "OsiMskSolverInterface.hpp"
	#include "mosek.h"
#endif
#include "OsiClpSolverInterface.hpp"
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <queue>

namespace Models
{
	Graph::FMTgraphstats FMTsrmodel::initializematrix()
	{
		Graph::FMTgraphstats stats;
		try {
			const int ncols = static_cast<int>(graph.nedges());
			const std::vector<int>column_Starts(static_cast<size_t>(ncols) + 1, 0);
			const std::vector<int>targetrows(static_cast<size_t>(ncols), 0);
			const std::vector<double>nelements(ncols, 0.0);
			std::vector<double>lower_bounds(ncols, 0.0);
			std::vector<double>upper_bounds(ncols, 0.0);
			graph.getinitialbounds(lower_bounds, upper_bounds);
			solver.addCols(ncols, &column_Starts[0], &targetrows[0],
				&nelements[0], &lower_bounds[0],
				&upper_bounds[0], &nelements[0]);
			solver.setColSolution(&lower_bounds[0]);
			stats = graph.getstats();
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
			if (static_cast<int>(graph.size()) > period)
			{
				size_t totaledges = 0;
				size_t boundededges = 0;
				const double* columnsupperbounds = solver.getColUpper();
				const double* columnslowerbounds = solver.getColLower();
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator it, itend;
				for (boost::tie(it, itend) = graph.getperiodverticies(period); it != itend; ++it)
				{
					const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor& vertex_descriptor = *it;
					const std::map<int, int>variables = graph.getoutvariables(vertex_descriptor);
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
			if (static_cast<int>(graph.size()) > period && period > 0)//period >0 to not select actual developments!
			{
				std::vector<int>variable_index;
				std::vector<double>bounds;
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator it, itend;
				for (boost::tie(it, itend) = graph.getperiodverticies(period); it != itend; ++it)
				{
					const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vertex_descriptor = *it;
					std::map<int, int>variables = graph.getoutvariables(vertex_descriptor);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						if (std::find(variable_index.begin(), variable_index.end(), varit->second) == variable_index.end())
						{
							variable_index.push_back(varit->second);
							bounds.push_back(-COIN_DBL_MAX);
							bounds.push_back(COIN_DBL_MAX);
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


	bool FMTsrmodel::setsolution(int period, const Core::FMTschedule& schedule, double tolerance)
	{
		try {
			//const double tolerance = getparameter(TOLERANCE); not ready yet
			if (static_cast<int>(graph.size()) > period && period > 0)
			{
				std::vector<Core::FMTaction>::const_iterator cit = std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator("_DEATH"));
				const int deathid = static_cast<int>(std::distance(actions.cbegin(), cit));
				const double* actual_solution = solver.getColSolution();
				const boost::unordered_set<Core::FMTlookup<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor, Core::FMTdevelopment>> lookup = graph.getdevsset(period);
				std::vector<double>new_solution(actual_solution, actual_solution + solver.getNumCols());
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = graph.getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const std::map<int, int>variables = graph.getoutvariables(*vertex_iterator);

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
							if ((schedule.douselock() || actionit->first.dorespectlock()) && graph.containsdevelopment(devit.first, lookup))
							{
								//*_logger << "t1 " << actionit->first.getname() << "\n";
								const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = graph.getdevelopment(devit.first, lookup);
								//const int variable = graph.getoutvariables(vdescriptor)[actionid];
								std::map<int, int> outvariables = graph.getoutvariables(vdescriptor);
								std::map<int, int>::const_iterator varit = outvariables.find(actionid);
								if (varit == outvariables.cend())
								{
									_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
												"Developement " + std::string(devit.first) + " is not operable "
												" for action " + actionit->first.getname(), 
												"FMTsrmodel::setsolution", __LINE__, __FILE__);
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
									maximallock = graph.getmaximalock(period);
								}
								std::vector<double>lockstoadress(devit.second);
								std::vector<std::pair<Core::FMTdevelopment, double>>locksfound;
								std::vector<std::pair<int, size_t>>locksorter;
								Core::FMTdevelopment locked(devit.first);
								for (int lockid = 0; lockid <= maximallock; ++lockid)
								{
									locked.setlock(lockid);
									if (graph.containsdevelopment(locked, lookup))
									{
										const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = graph.getdevelopment(locked, lookup);
										const double* newsolutionptr = &new_solution[0];
										double originalinarea = graph.inarea(vdescriptor, newsolutionptr, true);
										if (originalinarea == 0)
										{
											originalinarea = std::numeric_limits<double>::max();
										}
										if (!(graph.onlypertiodstart(vdescriptor) && originalinarea == std::numeric_limits<double>::max()))
										{
											locksorter.push_back(std::pair<size_t, size_t>(locksfound.size(), graph.getamountofpaths(locked, -1, *this, lookup)));
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
											const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = graph.getdevelopment(locksfound.at(id).first, lookup);
											//const int variable = graph.getoutvariables(vdescriptor)[actionid];
											std::map<int, int> outvariables = graph.getoutvariables(vdescriptor);
											std::map<int, int>::const_iterator varit = outvariables.find(actionid);
											if (varit == outvariables.cend())
											{
												_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
															"Developement " + std::string(devit.first) + " is not operable "
															" for action " + actionit->first.getname(), 
															"FMTsrmodel::setsolution", __LINE__, __FILE__);
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
											
											_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
												"Cannot allocate area of " + std::to_string(areatoput) + " to " +
												std::string(devit.first) + " for action " + actionit->first.getname(), "FMTsrmodel::setsolution", __LINE__, __FILE__);
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
				for (boost::tie(vertex_iterator, vertex_iterator_end) = graph.getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					if (graph.periodstart(*vertex_iterator))//get inperiod
					{
						const double* solution = &new_solution[0];
						double rest = graph.inarea(*vertex_iterator, solution);
						//double rest = graph.inarea(devit->second, actual_solution);
						std::map<int, int>variables = graph.getoutvariables(*vertex_iterator);
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

						for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
						{
							std::vector<Core::FMTdevelopmentpath> paths = graph.getpaths(*vertex_iterator, varit->first);
							for (const Core::FMTdevelopmentpath path : paths)
							{
								if (path.development->getperiod() == period && processed.find(*path.development) == processed.end())
								{
									processed[*path.development] = graph.getdevelopment(*path.development, lookup);
									descriptors.push(graph.getdevelopment(*path.development, lookup));
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
							const Core::FMTdevelopment dev(graph.getdevelopment(*vertex_iterator));
							const double* solution = &new_solution[0];
							const double inarea = graph.inarea(*vertex_iterator, solution);
							std::string locking;
							if (dev.getlock() > 0)
							{
								Core::FMTdevelopment locked(dev);
								locking += " lock(";
								for (int locklevel = 0; locklevel < 30; ++locklevel)
								{
									locked.setlock(locklevel);
									if (graph.containsdevelopment(locked, lookup))
									{
										locking += std::to_string(locklevel) + ",";
									}
								}
								locking.pop_back();
								locking += ")";
							}
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
								std::to_string(rest) + " negative growth solution for " +
								std::string(dev) + " operated by " + actionnames + locking + " in area " + std::to_string(inarea),
								"FMTsrmodel::setsolution", __LINE__, __FILE__);
						}
						if (targetaction < 0)//Ajust only natural growth and not _DEATH
						{
							new_solution[growth] = rest;
						}

					}
				}

				while (!descriptors.empty())
				{
					Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor first = descriptors.front();
					std::map<int, int>variables = graph.getoutvariables(first);
					const double* solution = &new_solution[0];
					double rest = graph.inarea(first, solution);
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
						std::vector<Core::FMTdevelopmentpath> paths = graph.getpaths(first, varit->first);
						for (const Core::FMTdevelopmentpath path : paths)
						{
							if (path.development->getperiod() == period && processed.find(*path.development) == processed.end())
							{
								processed[*path.development] = graph.getdevelopment(*path.development, lookup);
								descriptors.push(graph.getdevelopment(*path.development, lookup));
							}
						}

						rest -= new_solution[varit->second];
					}
					if ((rest + tolerance) < 0)
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
							std::to_string(rest) + " negative growth solution for " +
							std::string(graph.getdevelopment(first)),
							"FMTsrmodel::setsolution", __LINE__, __FILE__);
					}
					if (targetaction < 0)
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


	bool FMTsrmodel::setsolutionbylp(int period, const Core::FMTschedule& schedule, double tolerance)
	{
		try {
			if (Graph::FMTgraphbuild::schedulebuild != graph.getbuildtype())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot set solution by lp on a non partial graph",
					"FMTsrmodel::setsolutionbylp", __LINE__, __FILE__);
			}
			if (static_cast<int>(graph.size()) > period && period > 0)
			{
				const boost::unordered_set<Core::FMTlookup<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor, Core::FMTdevelopment>> lookup = graph.getdevsset(period);
				std::map<int, std::pair<double, double>>bounds;
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
				for (boost::tie(vertex_iterator, vertex_iterator_end) = graph.getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
				{
					const std::map<int, int>variables = graph.getoutvariables(*vertex_iterator);
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
							if ((schedule.douselock() || actionit->first.dorespectlock()) && graph.containsdevelopment(devit.first, lookup))
							{
								const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = graph.getdevelopment(devit.first, lookup);
								//const int variable = graph.getoutvariables(vdescriptor)[actionid];
								std::map<int, int> outvariables = graph.getoutvariables(vdescriptor);
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
									maximallock = graph.getmaximalock(period);
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
									if (graph.containsdevelopment(locked, lookup))
									{
										const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = graph.getdevelopment(locked, lookup);
										//const int variable = graph.getoutvariables(vdescriptor)[actionid];
										std::map<int, int> outvariables = graph.getoutvariables(vdescriptor);
										std::map<int, int>::const_iterator varit = outvariables.find(actionid);
										if (varit == outvariables.cend())
										{
											_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
														"Developement " + std::string(devit.first) + " is not operable "
														" for action " + actionit->first.getname(), 
														"FMTsrmodel::setsolution", __LINE__, __FILE__);
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
				solver.setColSetBounds(&*(variables.cbegin()), &*(variables.cend()), &varsbounds[0]);
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
					/*for (boost::tie(vertex_iterator, vertex_iterator_end) = graph.getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
					{
						const std::map<int, int>variables = graph.getoutvariables(*vertex_iterator);
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

	FMTsrmodel::FMTsrmodel(const FMTmodel& base, const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& lgraph,
		const FMTlpsolver& lsolver) :
		FMTmodel(base),
		graph(lgraph),
		solver(lsolver)
	{
		//solver.passinobject(base);
		//graph.passinobject(base);
	}


	Core::FMTschedule FMTsrmodel::getsolution(int period, bool withlock) const
	{
		Core::FMTschedule newschedule;
		try
		{
			//setparameter(SHOW_LOCK_IN_SCHEDULES,withlock);
			const double* actual_solution = solver.getColSolution();
			newschedule = graph.getschedule(actions, actual_solution, period, withlock);//getparameter(SHOW_LOCK_IN_SCHEDULES));

		}
		catch (...)
		{
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTsrmodel::getsolution", __LINE__, __FILE__);
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
			const std::vector<double>upper_bounds(newstats.cols, COIN_DBL_MAX);
			solver.addCols(newstats.cols, &column_Starts[0], &targetrows[0],
				&nelements[0], &lower_bounds[0],
				&upper_bounds[0], &nelements[0]);

			//rows
			std::vector<int>row_Starts;
			//row_Starts.reserve(targets.size());
			std::vector<int>targetcols;
			std::vector<double>elements;

			//Need to reset a new constraint ID!
			Graph::FMTgraphstats oldstats = graph.getstats();
			int newconstraintID = solver.getNumRows();
			size_t periodsize = 0;
			Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator it, itend;
			for (boost::tie(it, itend) = targets; it != itend; ++it)
			{
				graph.setconstraintID(*it, newconstraintID);
				if (graph.gettransferrow(*it, row_Starts, targetcols, elements))
				{

				}
				++oldstats.transfer_rows;
				++newconstraintID;
				++periodsize;
			}
			const std::vector<double>row_bounds(periodsize, 0.0);
			const int nrows = (newconstraintID - solver.getNumRows());
			row_Starts.push_back(static_cast<int>(targetcols.size()));
			solver.addRows(nrows, &row_Starts[0], &targetcols[0],
				&elements[0], &row_bounds[0], &row_bounds[0]);
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
		graph(Graph::FMTgraphbuild::nobuild),
		solver(lsolvertype)
	{
		//solver.passinobject(base);
		//graph.passinobject(base);

	}

	FMTsrmodel::FMTsrmodel(const FMTsrmodel& rhs):
		FMTmodel(rhs),
		graph(rhs.graph),
		solver(rhs.solver)
	{
		//solver.passinobject(rhs);
		//graph.passinobject(rhs);
	}

	FMTsrmodel::FMTsrmodel() :
		FMTmodel(),
		graph(Graph::FMTgraphbuild::nobuild),
		solver()
	{

	}



	bool FMTsrmodel::operator == (const FMTsrmodel& rhs) const
	{
		return (FMTmodel::operator == (rhs) &&
			solver == (rhs.solver) &&
			graph == rhs.graph);
	}

	bool FMTsrmodel::operator != (const FMTsrmodel& rhs) const
	{
		return !(*this == rhs);
	}

	std::map<std::string, double> FMTsrmodel::getoutput(const Core::FMToutput& output, int period, Core::FMToutputlevel level) const
	{
		try {
			const double* solution = solver.getColSolution();
			return graph.getoutput(*this, output, period, solution, level);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::getoutput", __LINE__, __FILE__);
		}

		return std::map<std::string, double>();
	}



	Graph::FMTgraphstats FMTsrmodel::buildperiod(Core::FMTschedule schedule, bool forcepartialbuild, int compressageclassoperability)
	{
		try {
			std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> actives;
			Graph::FMTgraphstats buildstats;
			if (graph.empty())
			{
				actives = graph.initialize(area);
				buildstats = initializematrix();
				graph.setstats(buildstats);
			}
			else {
				actives = graph.getactiveverticies();
			}
			if (!forcepartialbuild && schedule.empty()) // full build
			{
				buildstats = graph.build(*this, actives, compressageclassoperability);
				graph.setbuildtype(Graph::FMTgraphbuild::fullbuild);
			}
			else {//partial build for result
				graph.setbuildtype(Graph::FMTgraphbuild::schedulebuild);
				buildstats = graph.buildschedule(*this, actives, schedule);
			}
			const int location = static_cast<int>(graph.size() - 2);
			const Graph::FMTgraphstats newstats = this->updatematrix(graph.getperiodverticies(location), buildstats);
			graph.setstats(newstats);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::buildperiod", __LINE__, __FILE__);
		}

		return graph.getstats();
	}



	size_t FMTsrmodel::getgraphsize() const
	{
		return graph.size();
	}

	void FMTsrmodel::cleargraphdevelopements()
	{
		graph.cleardevelopments();
	}
	void FMTsrmodel::cleargraphcache()
	{
		graph.clearcache();
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
		return graph.getstats();
	}

	int FMTsrmodel::getfirstactiveperiod() const
	{
		return graph.getfirstactiveperiod();
	}

	Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties> FMTsrmodel::postsolvegraph(const FMTmodel& originalbasemodel) const
	{
		try {
			std::vector<Core::FMTtheme> postsolvethemes = originalbasemodel.getthemes();
			std::vector<Core::FMTaction> postsolveactions = originalbasemodel.getactions();
			Core::FMTmask selectedmask = this->getselectedmask(postsolvethemes);
			std::vector<Core::FMTaction> presolveactions = this->getactions();
			std::map<int, int>actionmapping;
			int preactionid = 0;
			for (const Core::FMTaction action : presolveactions)
			{
				const int loc = static_cast<int>(std::distance(postsolveactions.begin(), std::find_if(postsolveactions.begin(), postsolveactions.end(), Core::FMTactioncomparator(action.getname()))));
				actionmapping[preactionid] = loc;
				++preactionid;
			}
			actionmapping[-1] = -1;
			return this->graph.postsolve(selectedmask, postsolvethemes, actionmapping);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsrmodel::postsolvegraph", __LINE__, __FILE__);
		}
		return Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>();
	}


	std::vector<Core::FMTactualdevelopment>FMTsrmodel::getarea(int period, bool beforegrowanddeath) const
	{
		std::vector<Core::FMTactualdevelopment>returnedarea;
		try {
			if (period == 0)
			{
				return FMTmodel::getarea();
			};
			const double* modelsolution = solver.getColSolution();
			const int deathactionid = static_cast<int>(actions.size()-1);
			Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			for (boost::tie(vertex_iterator, vertex_iterator_end) = graph.getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
				if ((!beforegrowanddeath&&graph.periodstart(*vertex_iterator)))
				{
					const Core::FMTdevelopment& graphdevelopement = graph.getdevelopment(*vertex_iterator);
					const double areaofdevelopement = graph.inarea(*vertex_iterator, modelsolution, true);
					if (areaofdevelopement > 0)
					{
						returnedarea.push_back(Core::FMTactualdevelopment(graphdevelopement, areaofdevelopement));
					}
				}
				else if (beforegrowanddeath)
				{
					const Core::FMTdevelopment& graphdevelopement = graph.getdevelopment(*vertex_iterator);
					const double indeatharea = graph.inarea(*vertex_iterator, modelsolution, deathactionid, true);
					const double areaofdevelopement = graph.outarea(*vertex_iterator, -1, modelsolution) +graph.outarea(*vertex_iterator, deathactionid, modelsolution) -indeatharea;
					if (areaofdevelopement > 0)
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

	void FMTsrmodel::passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger)
	{
		solver.passinlogger(logger);
	}

	bool FMTsrmodel::boundsolution(int period, double tolerance)
	{
		try {
			if (static_cast<int>(graph.size()) > period)
			{
				const double* actual_solution = solver.getColSolution();
				std::vector<int>variable_index;
				std::vector<double>bounds;
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator it, itend;

				for (boost::tie(it, itend) = graph.getperiodverticies(period); it != itend; ++it)
				{
					const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vertex_descriptor = *it;
					const std::map<int, int>variables = graph.getoutvariables(vertex_descriptor);
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
					const std::map<std::string, double> values = graph.getoutput(*this, output, period, solution, Core::FMToutputlevel::developpement);
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

}



BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsrmodel)
#endif
