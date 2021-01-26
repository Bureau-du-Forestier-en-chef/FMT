/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTlpmodel.h"
#ifdef FMTWITHMOSEK
	#include "OsiMskSolverInterface.hpp"
	#include "mosek.h"
#endif
#include "OsiClpSolverInterface.hpp"


namespace Models
{



	Graph::FMTgraphstats FMTlpmodel::initializematrix()
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
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTlpmodel::initializematrix", __LINE__, __FILE__);
			}
        return stats;
	}

	Heuristics::FMToperatingareaclusterer FMTlpmodel::getclusterer(
		const std::vector<Heuristics::FMToperatingareacluster>& initialcluster,
		const Core::FMToutput& areaoutput,
		const Core::FMToutput& statisticoutput,
		const int& period) const
	{
		Heuristics::FMToperatingareaclusterer newclusterer;
		try {
			std::vector<Heuristics::FMToperatingareacluster>newclusters;
			for (const Heuristics::FMToperatingareacluster& originalcluster : initialcluster)
				{
				Heuristics::FMToperatingareaclusterbinary centroid = originalcluster.getcentroid();
				std::vector<Heuristics::FMToperatingareaclusterbinary>allbinaries = originalcluster.getbinaries();
				const Core::FMToutput& centroidoutput = centroid.getoutputintersect(statisticoutput);
				const Core::FMToutput& centroidareaoutput = centroid.getoutputintersect(areaoutput);
				const std::map<std::string, double> centroidvalue = this->getoutput(centroidoutput,period);
				const std::map<std::string, double> centroidarea = this->getoutput(centroidareaoutput,period);
				centroid.setstatistic(centroidvalue.at("Total"));
				centroid.setarea(centroidarea.at("Total"));
				for (Heuristics::FMToperatingareaclusterbinary& binary : allbinaries)
					{
					const Core::FMToutput& binaryoutput = binary.getoutputintersect(statisticoutput);
					const Core::FMToutput& binaryareaoutput = binary.getoutputintersect(areaoutput);
					const std::map<std::string, double> binaryvalue = this->getoutput(binaryoutput,period);
					const std::map<std::string, double> binaryarea = this->getoutput(binaryareaoutput,period);
					binary.setstatistic(binaryvalue.at("Total"));
					binary.setarea(binaryarea.at("Total"));
					}
                const Heuristics::FMToperatingareacluster newopcluster(Heuristics::FMToperatingareacluster(centroid,allbinaries),originalcluster.getrealminimalarea(),originalcluster.getrealmaximalarea());
                if (!newopcluster.isvalidareabounds())
                    {
                    _exhandler->raise(Exception::FMTexc::FMTignore,
									"Operating area cluster "+std::string(centroid.getmask())+" wont reach its minimal size",
									"FMTlpmodel::getclusterer",__LINE__, __FILE__, _section);
                    }
				newclusters.push_back(newopcluster);
				}
			newclusterer = Heuristics::FMToperatingareaclusterer(solver.getsolvertype(),0,newclusters);
			newclusterer.passinobject(*this);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpmodel::getclusterer", __LINE__, __FILE__);
			}
		return newclusterer;
	}

	bool FMTlpmodel::boundsolution(int period,double tolerance)
	{
		try {
			if (static_cast<int>(graph.size()) > period)
			{
				const double* actual_solution = solver.getColSolution();
				std::vector<int>variable_index;
				std::vector<double>bounds;
				int maximalvariableset = 0;//This is for clp (bug on setcolsetbounds)
				for (const auto& descriptors : graph.getperiodverticies(period))
				{
					const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vertex_descriptor = descriptors.memoryobject;
					const std::map<int, int>variables = graph.getoutvariables(vertex_descriptor);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						if (std::find(variable_index.begin(), variable_index.end(), varit->second) == variable_index.end())
						{
							variable_index.push_back(varit->second);
							if (varit->second>maximalvariableset)
								{
								maximalvariableset = varit->second;
								}
							//Had tolerance on primal infeasibilities with FMT_DBL_TOLERANCE ...
							bounds.push_back(*(actual_solution + varit->second)*(1 - tolerance));
							bounds.push_back(*(actual_solution + varit->second)*(1 + tolerance));
						}
					}
				}
				maximalvariableset += 1;
				solver.setColSetBounds(&variable_index[0], &variable_index.back() + 1, &bounds[0],maximalvariableset);
				return this->resolve();
			}
		}catch (...)
		{
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTlpmodel::boundsolution", __LINE__, __FILE__);
		}



    return false;
	}

	bool FMTlpmodel::isperiodbounded(int period) const
		{
		try {
			if (static_cast<int>(graph.size()) > period)
			{
				size_t totaledges = 0;
				size_t boundededges = 0;
				const double* columnsupperbounds = solver.getColUpper();
				const double* columnslowerbounds = solver.getColLower();
				for (const auto& descriptors : graph.getperiodverticies(period))
				{
					const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor& vertex_descriptor = descriptors.memoryobject;
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
		}catch (...)
			{
			_exhandler->raisefromcatch("at period " + std::to_string(period),
				"FMTlpmodel::isperiodbounded", __LINE__, __FILE__);
			}
		return false;
		}

	bool FMTlpmodel::unboundsolution(int period)
		{
		try {
			if (static_cast<int>(graph.size()) > period && period > 0)//period >0 to not select actual developments!
			{
				std::vector<int>variable_index;
				std::vector<double>bounds;
				for (const auto& descriptors : graph.getperiodverticies(period))
				{
					const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vertex_descriptor = descriptors.memoryobject;
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
		}catch (...)
		{
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTlpmodel::unboundsolution", __LINE__, __FILE__);
		}


		return false;
		}

	size_t FMTlpmodel::getamountofpaths(const Core::FMTdevelopment& dev, const int& actionid) const
		{
		size_t amount = 0;
		try {
			std::vector<Core::FMTdevelopmentpath> paths;
			if (actionid>=0)
			{
				paths = dev.operate(actions.at(actionid), transitions.at(actionid), yields, themes);
			}else {
				paths.push_back(Core::FMTdevelopmentpath(dev.grow(), 1.0));
			}
			for (const Core::FMTdevelopmentpath& path : paths)
				{
				int period = path.development->period;
				Core::FMTdevelopment dev(*path.development);
				while (period < graph.size())
					{
					if (graph.containsdevelopment(dev))
						{
						const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = graph.getdevelopment(dev);
						std::map<int, int>vars = graph.getoutvariables(vdescriptor);
						vars.erase(-1);
						amount += vars.size();
						}
					dev = dev.grow();
					++period;
					}
				}
		}catch (...)
			{
			_exhandler->printexceptions("for "+std::string(dev), "FMTlpmodel::getamountofpaths", __LINE__, __FILE__);
			}
		return amount;
		}

	bool FMTlpmodel::setsolution(int period,const Core::FMTschedule& schedule,double tolerance)
	{
		try {
			if (static_cast<int>(graph.size()) > period && period > 0)
			{
				const double* actual_solution = solver.getColSolution();
				std::vector<double>new_solution(actual_solution, actual_solution + solver.getNumCols());
				for (Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::lookconst_iterator devit = graph.begin(period);
					devit != graph.end(period); devit++)
				{
					const std::map<int, int>variables = graph.getoutvariables(devit->memoryobject);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						new_solution[varit->second] = 0;
					}
				}

				int maximallock = -1;
				/*std::vector<Core::FMTschedule::const_iterator>orderedschedule;
				for (Core::FMTschedule::const_iterator actionit = schedule.begin(); actionit != schedule.end();actionit++)
				{
					if (actionit->first.dorespectlock())
					{
						orderedschedule.insert(orderedschedule.begin(), actionit);
					}else {
						orderedschedule.push_back(actionit);
					}

				}*/

				//Core::FMTschedule locked_schedule;
				for (const auto& actionit : schedule)
				{
					int actionid = int(std::distance(actions.begin(), find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(actionit.first.getname()))));
					size_t allocated = 0;
					for (const auto& devit : actionit.second)
					{
						if ((schedule.douselock()||actionit.first.dorespectlock()) && graph.containsdevelopment(devit.first))
						{
							const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = graph.getdevelopment(devit.first);
							const int variable = graph.getoutvariables(vdescriptor)[actionid];
							new_solution[variable] = devit.second.at(0);
							++allocated;
						}
						else if (!schedule.douselock()&&!actionit.first.dorespectlock())
						{
							if (maximallock==-1)
								{
								maximallock = graph.getmaximalock(period);
								}
							std::vector<double>lockstoadress(devit.second);
							//std::sort(lockstoadress.begin(), lockstoadress.end());
							//std::reverse(lockstoadress.begin(), lockstoadress.end());
							std::vector<std::pair<Core::FMTdevelopment,double>>locksfound;
							std::vector<std::pair<int,size_t>>locksorter;
							Core::FMTdevelopment locked(devit.first);
							for (int lockid = 0; lockid <= maximallock;++lockid)
								{
								locked.lock = lockid;
								if (graph.containsdevelopment(locked))
									{
									const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = graph.getdevelopment(locked);
									
									double originalinarea = graph.inarea(vdescriptor, actual_solution, true);
									if (originalinarea == 0)
										{
										originalinarea = std::numeric_limits<double>::max();
										}
									if (!(graph.onlypertiodstart(vdescriptor) && originalinarea == std::numeric_limits<double>::max()))
										{
										/*if (std::string(locked).find("15+")!=std::string::npos&&locked.period==2&&locked.age==10&&
											std::string(locked).find("S27034") != std::string::npos)
											{
											*_logger << "found " << std::string(locked) << " amount of " << getamountofpaths(locked, actionid) << "\n";
											*_logger << "no action " << getamountofpaths(locked,-1) << "\n";
											}*/
										locksorter.push_back(std::pair<size_t, size_t>(locksfound.size(), getamountofpaths(locked, -1)));
										locksfound.push_back(std::pair<Core::FMTdevelopment, double>(locked, originalinarea));
										
										}
									}
								}
	
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
								//*_logger << "allocatin " << areatoput << "\n";
								if (tolerance < areatoput)
								{
									size_t id = 0;
									bool found = false;
									bool exact = false;
									for (const std::pair<Core::FMTdevelopment, double>& element : locksfound)
									{
										if (std::abs(areatoput - element.second)<tolerance)
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
											if (areatoput <= (element.second+tolerance))
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
										const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = graph.getdevelopment(locksfound.at(id).first);
										const int variable = graph.getoutvariables(vdescriptor)[actionid];
										new_solution[variable] += areatoput;
										if (locksfound.at(id).second<std::numeric_limits<double>::max())
											{
											locksfound.at(id).second -= areatoput;
											if (exact|| locksfound.at(id).second< tolerance)
												{
												//*_logger << "Removing " << std::string(locksfound.at(id).first)<<" "<< locksfound.at(id).second << "\n";
												locksfound.erase(locksfound.begin() + id);
												}
											}
										
										/*if (exact)
											{
											locksfound.erase(locksfound.begin() + id);
										}else if (locksfound.at(id).second<std::numeric_limits<double>::max())
											{
											locksfound.at(id).second -= areatoput;

											//to set
											}*/
										lockstoadress.erase(lockstoadress.begin());
										++allocated;
									}
									else if(secondpass) 
										{
										_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
											"Cannot allocate area of " + std::to_string(areatoput) + " to " +
											std::string(devit.first) + " for action " + actionit.first.getname(), "FMTlpmodel::setsolution", __LINE__, __FILE__);
									}else {
										lockstoadress.push_back(areatoput);
										lockstoadress.erase(lockstoadress.begin());
									}
								}else {
									lockstoadress.erase(lockstoadress.begin());
									}
								if (iteration == initialsize)
									{
									secondpass = true;
									}
								++iteration;
								}


							/*int area_id = 0;
							int last_lock = 0;
							while (area_id < static_cast<int>(devit.second.size()))
							{
								if (graph.containsdevelopment(locked))
								{
									//const double originalinarea = graph.inarea(devit->second, solution);
									const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor vdescriptor = graph.getdevelopment(locked);
									const int variable = graph.getoutvariables(vdescriptor)[actionid];
									new_solution[variable] += devit.second.at(area_id);
									last_lock = locked.lock;
									++allocated;
									++area_id;
									++locked.lock;
								}
								if (area_id < static_cast<int>(devit.second.size()))//continue looking for a developpement!
								{
									size_t checked = 0;
									const size_t allocated_bound = (graph.getperiodverticies(period).size() - allocated);
									while (!graph.containsdevelopment(locked) &&
										checked < allocated_bound)
									{
										++locked.lock;
										++checked;
									}
									if (checked == allocated_bound)//no lock available so get back to regular
									{
										locked.lock = last_lock; //go back to last good hit!
									}
								}

							}*/
						}
						else {
							_exhandler->raise(Exception::FMTexc::FMTmissingdevelopement,std::string(devit.first) + " at period " + std::to_string(period) + " operated by " + actionit.first.getname(),
								"FMTlpmodel::setsolution",__LINE__, __FILE__);
							return false;
						}
					}
				}
				//Fill up natural evolution
				boost::unordered_map<Core::FMTdevelopment, Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>processed;
				std::queue<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>descriptors;
				for (Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::lookconst_iterator devit = graph.begin(period);
					devit != graph.end(period); devit++)
				{
					if (graph.periodstart(devit->memoryobject))//get inperiod
					{
						const double* solution = &new_solution[0];
						double rest = graph.inarea(devit->memoryobject, solution);
						//double rest = graph.inarea(devit->second, actual_solution);
						std::map<int, int>variables = graph.getoutvariables(devit->memoryobject);
						int growth = variables[-1];
						variables.erase(-1);
						for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
						{
							std::vector<Core::FMTdevelopmentpath> paths = graph.getpaths(devit->memoryobject, varit->first);
							for (const Core::FMTdevelopmentpath path : paths)
							{
								if (path.development->period == period && processed.find(*path.development) == processed.end())
								{
									processed[*path.development] = graph.getdevelopment(*path.development);
									descriptors.push(graph.getdevelopment(*path.development));
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
							const Core::FMTdevelopment dev(graph.getdevelopment(devit->memoryobject));
							const double* solution = &new_solution[0];
							const double inarea = graph.inarea(devit->memoryobject,solution);
							std::string locking;
							if (dev.lock>0)
								{
								Core::FMTdevelopment locked(dev);
								locking += " lock(";
								for (int locklevel = 0; locklevel < 30;++locklevel)
									{
									locked.lock = locklevel;
									if (graph.containsdevelopment(locked))
										{
										locking += std::to_string(locklevel)+",";
										}
									}
								locking.pop_back();
								locking += ")";
								}
							_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
								 std::to_string(rest) + " negative growth solution for " +
								std::string(dev)+" operated by "+ actionnames + locking+" in area "+ std::to_string(inarea),
								"FMTlpmodel::setsolution",__LINE__, __FILE__);
						}
						new_solution[growth] = rest;

					}
				}

				while (!descriptors.empty())
				{
					Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor first = descriptors.front();
					std::map<int, int>variables = graph.getoutvariables(first);
					const double* solution = &new_solution[0];
					double rest = graph.inarea(first, solution);
					int growth = variables[-1];
					variables.erase(-1);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						std::vector<Core::FMTdevelopmentpath> paths = graph.getpaths(first, varit->first);
						for (const Core::FMTdevelopmentpath path : paths)
						{
							if (path.development->period == period && processed.find(*path.development) == processed.end())
							{
								processed[*path.development] = graph.getdevelopment(*path.development);
								descriptors.push(graph.getdevelopment(*path.development));
							}
						}
						rest -= new_solution[varit->second];
					}
					if ((rest + tolerance) < 0)
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_number,
							 std::to_string(rest) + " negative growth solution for " +
							std::string(graph.getdevelopment(first)),
							"FMTlpmodel::setsolution",__LINE__, __FILE__);
					}
					new_solution[growth] = rest;
					descriptors.pop();
				}
				solver.setColSolution(&new_solution[0]);

			}
		}catch (...)
		{
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTlpmodel::setsolution", __LINE__, __FILE__);
		}


		return true;
	}

	Core::FMTschedule FMTlpmodel::getsolution(int period, bool withlock) const
	{
		Core::FMTschedule newschedule;
		try
		{
			const double* actual_solution = solver.getColSolution();
			newschedule = graph.getschedule(actions,actual_solution,period,withlock);
			newschedule.passinobject(*this);

		}catch (...)
		{
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTlpmodel::getsolution", __LINE__, __FILE__);
		}
		return newschedule;
	}

	Graph::FMTgraphstats FMTlpmodel::updatematrix(const boost::unordered_set<Core::FMTlookup<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor, Core::FMTdevelopment>>& targets,
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
			row_Starts.reserve(targets.size());
			std::vector<int>targetcols;
			std::vector<double>elements;
			const std::vector<double>row_bounds(targets.size(), 0.0);
			//Need to reset a new constraint ID!
			Graph::FMTgraphstats oldstats = graph.getstats();
			int newconstraintID = solver.getNumRows();
			for (auto& vertex_it : targets)
			{
				graph.setconstraintID(vertex_it.memoryobject, newconstraintID);
				if (graph.gettransferrow(vertex_it.memoryobject, row_Starts, targetcols, elements))
				{

				}
				++oldstats.transfer_rows;
				++newconstraintID;
			}
			const int nrows = (newconstraintID - solver.getNumRows());
			row_Starts.push_back(static_cast<int>(targetcols.size()));
			solver.addRows(nrows, &row_Starts[0], &targetcols[0],
				&elements[0], &row_bounds[0], &row_bounds[0]);
			oldstats.cols = solver.getNumCols();
			oldstats.rows = solver.getNumRows();
			return oldstats;
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTlpmodel::updatematrix", __LINE__, __FILE__);
			}
		return this->getstats();
	}


	FMTlpmodel::FMTlpmodel(const FMTmodel& base, FMTsolverinterface lsolvertype) :
		FMTmodel(base),
		graph(Graph::FMTgraphbuild::nobuild),
		elements(),
		solver(lsolvertype, *this->_logger)
	{
		solver.passinobject(base);
		graph.passinobject(base);
	
	}

	FMTlpmodel::FMTlpmodel() :
		FMTmodel(),
		graph(Graph::FMTgraphbuild::nobuild),
		elements(),
		solver()
	{

	}

	FMTlpmodel::FMTlpmodel(const FMTlpmodel& rhs) :
		FMTmodel(rhs),
		graph(rhs.graph),
		elements(rhs.elements),
		solver(rhs.solver)
	{

	}


	bool FMTlpmodel::operator == (const FMTlpmodel& rhs) const
	{
		return (FMTmodel::operator == (rhs) &&
			solver == (rhs.solver) &&
			graph == rhs.graph &&
			elements == rhs.elements);
	}

	bool FMTlpmodel::operator != (const FMTlpmodel& rhs) const
	{
		return !(*this == rhs);
	}



	std::map<std::string, double> FMTlpmodel::getoutput(const Core::FMToutput& output,int period, Graph::FMToutputlevel level) const
	{
		try {
			const double* solution = solver.getColSolution();
			return graph.getoutput(*this, output, period, solution, level);
		}catch (...)
			{
				_exhandler->printexceptions("", "FMTlpmodel::getoutput", __LINE__, __FILE__);
			}

		return std::map<std::string, double>();
	}



	Graph::FMTgraphstats FMTlpmodel::buildperiod(Core::FMTschedule schedule, bool forcepartialbuild)
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
				buildstats = graph.build(*this,actives);
				graph.setbuildtype(Graph::FMTgraphbuild::fullbuild);
			}
			else {//partial build for result
				graph.setbuildtype(Graph::FMTgraphbuild::schedulebuild);   
				buildstats = graph.buildschedule(*this,actives, schedule);
			}
			const int location = static_cast<int>(graph.size() - 2);
			const Graph::FMTgraphstats newstats = this->updatematrix(graph.getperiodverticies(location), buildstats);
			graph.setstats(newstats);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTlpmodel::buildperiod", __LINE__, __FILE__);
		}

		return graph.getstats();
	}


	FMTlpmodel& FMTlpmodel::operator = (const FMTlpmodel& rhs)
	{
		if (this != &rhs)
		{
			FMTmodel::operator = (rhs);
			solver = (rhs.solver);
			graph = rhs.graph;
		}
		return *this;
	}


bool FMTlpmodel::locatenodes(const std::vector<Core::FMToutputnode>& nodes, int period,
	std::map<int, double>& variables, double multiplier) const
	{
	bool cashhit = false;
	try {
		for (const Core::FMToutputnode& node : nodes)
			{
			const std::map<int, double>node_map = graph.locatenode(*this, node, period);
			for (std::map<int, double>::const_iterator node_it = node_map.begin(); node_it != node_map.end(); node_it++)
				{
				if (variables.find(node_it->first) == variables.end())
					{
					variables[node_it->first] = 0;
					}
				variables[node_it->first] += node_it->second*multiplier;
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpmodel::locatenodes", __LINE__, __FILE__);
			}
	return cashhit;
	}



	Graph::FMTgraphstats FMTlpmodel::setconstraint(const Core::FMTconstraint& constraint)
		{
		try {
			if (!constraint.isobjective()&&!constraint.isspatial())
			{
				int first_period = 0;
				int last_period = 0;
				if (graph.constraintlenght(constraint, first_period, last_period))
				{
					Core::FMTconstrainttype constraint_type = constraint.getconstrainttype();
					double averagefactor = 1;
					if (last_period != first_period)
					{
						averagefactor = (1 / (last_period - first_period));
					}
					std::vector<Core::FMToutputnode>all_nodes = constraint.getnodes(area,actions,yields,averagefactor);
					double lowerbound = 0;
					double upperbound = 0;
					double coef_multiplier_lower = 1;
					double coef_multiplier_upper = 1;
					double lower_variation = 0;
					double upper_variation = 0;
					constraint.getvariations(lower_variation, upper_variation);
					int upper_even_variable = -1;
					int lower_even_variable = -1;
					if (constraint_type == Core::FMTconstrainttype::FMTevenflow && lower_variation != 0)
					{
						++last_period;
					}
					for (int period = first_period; period <= last_period; ++period)
					{
						std::map<int, double>all_variables;
						int goal_variable = -1;
						if (constraint.isgoal())
						{
							goal_variable = getsetmatrixelement(constraint, FMTmatrixelement::goalvariable, all_variables, period);
							all_variables[goal_variable] = 1;
						}

						if (constraint_type == Core::FMTconstrainttype::FMTevenflow)
						{
							coef_multiplier_lower = 1 - lower_variation;
							coef_multiplier_upper = 1 + lower_variation;
							lowerbound = 0;
							upperbound = 0;
							if (lower_variation != 0)
							{
								if (upper_even_variable < 0 && lower_even_variable < 0)
								{
									std::map<int, double>localvariables;
									upper_even_variable = getsetmatrixelement(constraint, FMTmatrixelement::levelvariable, localvariables);
									lower_even_variable = getsetmatrixelement(constraint, FMTmatrixelement::objectivevariable, localvariables);
									localvariables[upper_even_variable] = coef_multiplier_lower;
									localvariables[lower_even_variable] = -1;
									lowerbound = std::numeric_limits<double>::lowest();
									upperbound = 0;
									const int lower_constraint_id = getsetmatrixelement(constraint, FMTmatrixelement::constraint, localvariables, -1, lowerbound, upperbound);
								}
								locatenodes(all_nodes, period, all_variables, 1);
								all_variables[lower_even_variable] = -1;
								lowerbound = 0;
								upperbound = std::numeric_limits<double>::max();
								if (all_variables.size() == 1)
								{
									all_variables.clear();
								}
								const int lowervalue = getsetmatrixelement(constraint, FMTmatrixelement::constraint, all_variables,
									period, lowerbound, upperbound);
								all_variables.erase(lower_even_variable);
								all_variables[upper_even_variable] = -1;
								lowerbound = std::numeric_limits<double>::lowest();
								upperbound = 0;
								if (all_variables.size() == 1)
								{
									all_variables.clear();
								}
								const int uppervalue = getsetmatrixelement(constraint, FMTmatrixelement::constraint, all_variables,
									period, lowerbound, upperbound);
								if (period == last_period)
								{
									return graph.getstats();
								}
								continue;
							}


						}
						else if (constraint_type == Core::FMTconstrainttype::FMTnondeclining)
						{
							lowerbound = std::numeric_limits<double>::lowest();
							upperbound = 0;
						}
						else if (constraint_type == Core::FMTconstrainttype::FMTsequence)
						{
							coef_multiplier_upper = 1 + lower_variation;
							coef_multiplier_lower = 1 - upper_variation;
							lowerbound = 0;
							upperbound = 0;
						}
						else {
							constraint.getbounds(lowerbound, upperbound, period);
						}
						//size_t left_location = 0;
						locatenodes(all_nodes, period, all_variables, 1);
						if (constraint.acrossperiod() && !all_variables.empty())
						{
							const size_t sizebeforecrossing = all_variables.size();
							locatenodes(all_nodes, (period + 1), all_variables, -1);
							size_t sizeaftercrossing = all_variables.size();
							if (sizebeforecrossing == sizeaftercrossing)//dont want empty periods!
							{
								all_variables.clear();
							}
							if (coef_multiplier_lower != 1 || constraint_type == Core::FMTconstrainttype::FMTsequence)
							{
								lowerbound = std::numeric_limits<double>::lowest();
								upperbound = 0;
							}
						}

						//level part
						locatelevels(all_nodes, period, all_variables, constraint);
						std::map<int, double>lowervars = all_variables;
						if (coef_multiplier_lower != 1)
						{
							for (std::map<int, double>::iterator varit = lowervars.begin(); varit != lowervars.end(); varit++)
							{
								if (varit->second > 0 && varit->first != goal_variable)
								{
									varit->second *= coef_multiplier_lower;
								}
							}
						}
						const int lower_constraint_id = getsetmatrixelement(constraint, FMTmatrixelement::constraint, lowervars,
							period, lowerbound, upperbound);
						//ismultiple
						if (constraint.acrossperiod() && (coef_multiplier_lower != 1 || constraint_type == Core::FMTconstrainttype::FMTsequence) && coef_multiplier_upper != 0)
						{
							std::map<int, double>uppervars = all_variables;
							if (coef_multiplier_upper != 1)
							{
								for (std::map<int, double>::iterator varit = uppervars.begin(); varit != uppervars.end(); varit++)
								{
									if (varit->second > 0 && varit->first != goal_variable)
									{
										varit->second *= coef_multiplier_upper;
									}
								}
							}
							lowerbound = 0;
							upperbound = std::numeric_limits<double>::max();
							const int upper_constraint_id = getsetmatrixelement(constraint, FMTmatrixelement::constraint, uppervars,
								period, lowerbound, upperbound);
						}

					}
				}
			}
		}catch (...)
			{
				_exhandler->printexceptions("for " + std::string(constraint), "FMTlpmodel::setconstraint", __LINE__, __FILE__);
			}
		return graph.getstats();
		}

	std::vector<int>FMTlpmodel::setobjectivebounds(bool dolower, bool doupper, double tolerance)
		{
		std::vector<int>bounding;
		try {
			const double originalvalue = solver.getObjValue();
			const double* originalcoef = solver.getObjCoefficients();
			const int numberofcols = solver.getNumCols();
			std::vector<int>indexes;
			indexes.reserve(numberofcols);
			std::vector<double>coefficients;
			coefficients.reserve(numberofcols);
			for (int colid = 0; colid < numberofcols; ++colid)
			{
				if (*(originalcoef + colid) != 0)
				{
					indexes.push_back(colid);
					coefficients.push_back(*(originalcoef + colid));
				}
			}
			if (dolower)
			{
				const double lowerbound = originalvalue - tolerance;
				solver.addRow(static_cast<int>(indexes.size()), &indexes[0], &coefficients[0], lowerbound, std::numeric_limits<double>::infinity());
				bounding.push_back(solver.getNumRows() - 1);
			}
			if (doupper)
			{
				const double upperbound = originalvalue + tolerance;
				solver.addRow(static_cast<int>(indexes.size()), &indexes[0], &coefficients[0], std::numeric_limits<double>::lowest(), upperbound);
				bounding.push_back(solver.getNumRows() - 1);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpmodel::setobjectivebounds", __LINE__, __FILE__);
			}
		return bounding;
		}

	std::map<std::string, std::vector<double>>FMTlpmodel::getareavariabilities(const std::vector<Core::FMToutput>& localoutputs,
		const std::vector<Core::FMTmask>& globalmasks,
		std::vector<double> tolerances)
	{
		std::map<std::string, std::vector<double>>uppernlower;
		try {
			if (tolerances.empty())
				{
				tolerances = std::vector<double>(FMT_DBL_TOLERANCE,globalmasks.size());
				}
			if (tolerances.size()!=globalmasks.size())
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Global masks and tolerances are not the same length", "FMTlpmodel::getareavariabilities", __LINE__, __FILE__);
				}
			const boost::unordered_set<Core::FMTlookup<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor,Core::FMTdevelopment>>& initialperiod = graph.getperiodverticies(0);
			std::vector<int>colstarget;
			std::vector<double>originalbounds;
			std::vector<double>newbounds;
			const double* collowerbounds = solver.getColLower();
			const double* colupperbounds = solver.getColUpper();
			std::vector<bool>foundcorresponding(globalmasks.size(), false);
			const int firstfutrecolumn = static_cast<int>(initialperiod.size());
				for (boost::unordered_set<Core::FMTlookup<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor, Core::FMTdevelopment>>::const_iterator vertexit = initialperiod.begin();
					vertexit != initialperiod.end(); vertexit++)
				{
					size_t maskid = 0;
					size_t gotvariables = 0;
					for (const Core::FMTmask& globalmask : globalmasks)
					{
						if (vertexit->pointerobject->mask.issubsetof(globalmask))
						{
							const int varindex = graph.getoutvariables(vertexit->memoryobject).at(-1);
							if (*(colupperbounds + varindex) == COIN_DBL_MAX)
							{
								_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
									"Changing a bound on a non initial variable for developmenets" + std::string(*(vertexit->pointerobject)),
									"FMTlpmodel::getareavariabilities", __LINE__, __FILE__);
							}
							colstarget.push_back(varindex);
							const double originallowerboundvalue = *(collowerbounds + varindex);
							const double originalupperboundvalue = *(colupperbounds + varindex);
							originalbounds.push_back(originallowerboundvalue);
							originalbounds.push_back(originalupperboundvalue);
							foundcorresponding[maskid] = true;
							const double newlowerboundvalue = originallowerboundvalue + (originallowerboundvalue * tolerances.at(maskid));
							const double newupperboundvalue = originalupperboundvalue + (originalupperboundvalue * tolerances.at(maskid));
							newbounds.push_back(newlowerboundvalue);
							newbounds.push_back(newupperboundvalue);
							++gotvariables;
						}
						++maskid;
					}
					if (gotvariables > 1)
					{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
							"Got more than one global mask for " + std::string((vertexit->pointerobject->mask)),
							"FMTlpmodel::getareavariabilities", __LINE__, __FILE__);
					}
				}
				size_t mskid = 0;
				for (const bool& gotone : foundcorresponding)
				{
					if (!gotone)
					{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"No corresponding development found for " + std::string(globalmasks.at(mskid)),
							"FMTlpmodel::getareavariabilities", __LINE__, __FILE__);
					}
					++mskid;
				}
			if (solver.isProvenOptimal())
			{
				for (const Core::FMToutput& output : localoutputs)
				{
					std::vector<double>outputvalues;
					for (int period = 0; period <= static_cast<int>(graph.size() - 2); ++period)
					{
						outputvalues.push_back(this->getoutput(output, period).at("Total"));
					}
					uppernlower["OLD_" + output.getname()] = outputvalues;
				}
			}
			//this->writeLP("T:/Donnees/Usagers/FORBR3/Pentes LIDAR/Nouvelle méthode/06251/a1");
			solver.setColSetBounds(&(*colstarget.cbegin()), &(*colstarget.cend()), &newbounds[0], firstfutrecolumn);
			if (this->resolve())
			{
				for (const Core::FMToutput& output : localoutputs)
				{
					std::vector<double>outputvalues;
					for (int period = 0; period <= static_cast<int>(graph.size() - 2); ++period)
					{
						outputvalues.push_back(this->getoutput(output, period).at("Total"));
					}
					uppernlower["NEW_" + output.getname()] = outputvalues;
				}
			}
			//this->writeLP("T:/Donnees/Usagers/FORBR3/Pentes LIDAR/Nouvelle méthode/06251/a2");
			solver.setColSetBounds(&(*colstarget.cbegin()), &(*colstarget.cend()), &originalbounds[0], firstfutrecolumn);
			//this->writeLP("T:/Donnees/Usagers/FORBR3/Pentes LIDAR/Nouvelle méthode/06251/a3");
			if (!this->resolve())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot regenerate initial optimal solution",
					"FMTlpmodel::getareavariabilities", __LINE__, __FILE__);
				}
		}catch (...)
			{
			_exhandler->printexceptions("for " + name, "FMTlpmodel::getareavariabilities", __LINE__, __FILE__);
			}
	return uppernlower;
	}



	std::map<std::string, std::vector<double>>FMTlpmodel::getvariabilities(const std::vector<Core::FMToutput>& outputs,double tolerance)
		{
		std::map<std::string, std::vector<double>>uppernlower;
		try {
			const double* originalcoef = solver.getObjCoefficients();
			const std::vector<double>originalcoefficients(originalcoef, (originalcoef + solver.getNumCols()));
			const double originalsense = solver.getObjSense();
			const std::vector<int>objectivebounds = setobjectivebounds(true, true, tolerance);
			Core::FMToutput outtest;
			for (const Core::FMToutput& output : outputs)
			{
				int first_period = 0;
				int last_period = 0;
				Core::FMTconstraint maxconstraint(Core::FMTconstrainttype::FMTMAXobjective, output);
				maxconstraint.setlength();
				graph.constraintlenght(maxconstraint, first_period, last_period);
				std::vector<double>medianvalues;
				for (int period = first_period; period <= last_period; ++period)
				{
					medianvalues.push_back(this->getoutput(output, period).begin()->second);
				}
				uppernlower["M" + output.getname()] = medianvalues;
				this->setobjective(maxconstraint);
				this->initialsolve();
				std::vector<double>uppervalues;
				for (int period = first_period; period <= last_period; ++period)
				{
					uppervalues.push_back(this->getoutput(output, period).begin()->second);
				}
				uppernlower["U" + output.getname()] = uppervalues;
				this->eraseallconstraint(maxconstraint);
				Core::FMTconstraint minconstraint(Core::FMTconstrainttype::FMTMINobjective, output);
				minconstraint.setlength();
				this->setobjective(minconstraint);
				this->initialsolve();
				std::vector<double>lowervalues;
				for (int period = first_period; period <= last_period; ++period)
				{
					lowervalues.push_back(this->getoutput(output, period).begin()->second);
				}
				uppernlower["L" + output.getname()] = lowervalues;
				this->eraseallconstraint(minconstraint);
			}
			solver.setObjective(&originalcoefficients[0]);
			solver.setObjSense(originalsense);
			solver.deleteRows(static_cast<int>(objectivebounds.size()), &objectivebounds[0]);
			solver.initialsolve();
		}catch (...)
			{
			_exhandler->raisefromcatch("for "+name,
				"FMTlpmodel::getvariabilities", __LINE__, __FILE__);
			}
		return uppernlower;
		}

	Graph::FMTgraphstats FMTlpmodel::eraseallconstraint(const Core::FMTconstraint& constraint)
		{
		try {
			int first_period = 0;
			int last_period = 0;
			if (graph.constraintlenght(constraint, first_period, last_period))
			{
				for (int period = first_period; period <= last_period; ++period)
				{
					this->eraseconstraint(constraint, period);
				}
			}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTlpmodel::eraseallconstraint", __LINE__, __FILE__);
		}


		return this->getstats();
		}

	Graph::FMTgraphstats FMTlpmodel::eraseconstraint(const Core::FMTconstraint& constraint, int period)
		{
		try {
		if (static_cast<int>(elements.size()) > period && elements.at(period).find(std::string(constraint))!= elements.at(period).end())
			{
			std::vector<std::vector<int>>all_elements = elements.at(period).at(std::string(constraint));
			elements.at(period).erase(elements.at(period).find(std::string(constraint)));
			int removedrow = -1;
			if (!all_elements.at(FMTmatrixelement::constraint).empty())
				{
				for (const int& levelid : all_elements.at(FMTmatrixelement::constraint))
				{
					bool removeconstraint = true;
					for (int locator = (period + 1); locator < static_cast<int>(elements.size()); ++locator)
					{
						for (std::unordered_map<std::string, std::vector<std::vector<int>>>::iterator elit = elements.at(locator).begin(); elit != elements.at(locator).end(); elit++)
						{
							if (std::find(elit->second.at(FMTmatrixelement::constraint).begin(), elit->second.at(FMTmatrixelement::constraint).end(), levelid) != elit->second.at(FMTmatrixelement::constraint).end())
							{
								removeconstraint = false;
								break;
							}
						}
						if (!removeconstraint)
							{
							break;
							}
					}
					if (removeconstraint)
						{
						solver.deleteRow(levelid);
						--graph.getstatsptr()->rows;
						--graph.getstatsptr()->output_rows;
						}
				}
				}
			if (!all_elements.at(FMTmatrixelement::goalvariable).empty())
				{
				for (const int& colid : all_elements.at(FMTmatrixelement::goalvariable))
					{
					solver.deleteCol(colid);
					--graph.getstatsptr()->cols;
					--graph.getstatsptr()->output_cols;
					}
				}

			if (!all_elements.at(FMTmatrixelement::levelvariable).empty() || !all_elements.at(FMTmatrixelement::objectivevariable).empty())
				{
				std::vector<int>potentialcols;
				potentialcols.insert(potentialcols.end(), all_elements.at(FMTmatrixelement::objectivevariable).begin(), all_elements.at(FMTmatrixelement::objectivevariable).end());
				potentialcols.insert(potentialcols.end(), all_elements.at(FMTmatrixelement::levelvariable).begin(), all_elements.at(FMTmatrixelement::levelvariable).end());
				bool candelete = true;
				for (const int& levelid : potentialcols)
					{
					for (int locator = (period+1); locator < static_cast<int>(elements.size());++locator)
						{
						for (std::unordered_map<std::string, std::vector<std::vector<int>>>::iterator elit = elements.at(locator).begin(); elit != elements.at(locator).end(); elit++)
							{
							if (std::find(elit->second.at(FMTmatrixelement::levelvariable).begin(), elit->second.at(FMTmatrixelement::levelvariable).end(), levelid) != elit->second.at(FMTmatrixelement::levelvariable).end() ||
								std::find(elit->second.at(FMTmatrixelement::objectivevariable).begin(), elit->second.at(FMTmatrixelement::objectivevariable).end(), levelid) != elit->second.at(FMTmatrixelement::objectivevariable).end())
								{
								candelete = false;
								break;
								}
							}
						if (!candelete)
							{
							break;
							}
						}
					if (candelete)
						{
						solver.deleteCol(levelid);
						--graph.getstatsptr()->cols;
						--graph.getstatsptr()->output_cols;
						//colstoremove.push_back(levelid);
						}
					}
				}
			}
			}catch (...)
				{
				_exhandler->raisefromcatch("for "+std::string(constraint),"FMTlpmodel::eraseconstraint", __LINE__, __FILE__);
				}
		return graph.getstats();
		}

	void FMTlpmodel::updateconstraintsmapping(const std::vector<int>& Dvariables, const std::vector<int>& Dconstraints)
		{
		try {
			if (!Dvariables.empty() || !Dconstraints.empty())
			{
				std::vector<std::unordered_map<std::string, std::vector<std::vector<int>>>>::iterator periodit = elements.begin();
				while (periodit != elements.end())
				{
					for (std::unordered_map<std::string, std::vector<std::vector<int>>>::iterator constraintit = periodit->begin(); constraintit != periodit->end(); constraintit++)
					{
						if (!Dconstraints.empty())
						{
							updatematrixelements(constraintit->second.at(FMTmatrixelement::constraint), Dconstraints);
						}
						if (!Dvariables.empty())
						{
							updatematrixelements(constraintit->second.at(FMTmatrixelement::levelvariable), Dvariables);
							updatematrixelements(constraintit->second.at(FMTmatrixelement::objectivevariable), Dvariables);
							updatematrixelements(constraintit->second.at(FMTmatrixelement::goalvariable), Dvariables);
						}
					}
					++periodit;
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpmodel::updateconstraintsmapping", __LINE__, __FILE__);
			}
		}

	void FMTlpmodel::updatematrixelements(std::vector<int>& matrixelements, const std::vector<int>& deletedelements) const
		{
		try {
			for (int& elementvalue : matrixelements)
			{
				int value = 0;
				std::vector<int>::const_iterator constraintitdelete = deletedelements.begin();
				while (constraintitdelete != deletedelements.end() && elementvalue > *constraintitdelete)
				{
					++value;
					++constraintitdelete;
				}
				elementvalue -= value;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpmodel::updatematrixelements", __LINE__, __FILE__);
			}
		}

	bool FMTlpmodel::updatematrixngraph()
		{
		solver.sortdeletedcache();
		const std::vector<int>& deletedconstraints = solver.getcachedeletedconstraints();
		const std::vector<int>& deletedvariables = solver.getcachedeletedvariables();
		if (!deletedconstraints.empty() || !deletedvariables.empty())
			{
			try {
				graph.updatematrixindex(deletedvariables, deletedconstraints);
				updateconstraintsmapping(deletedvariables, deletedconstraints);
				solver.synchronize();
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTlpmodel::updatematrixngraph", __LINE__, __FILE__);
				}
			return true;
			}
		return false;
		}

	Graph::FMTgraphstats FMTlpmodel::eraseperiod(bool constraintsonly)
	{
		try{
			int firstperiod = graph.getfirstactiveperiod();
			if (isperiodbounded(firstperiod))
				{
				std::vector<int>deletedconstraints;
				std::vector<int>deletedvariables;
				if (!constraintsonly)
					{
						graph.eraseperiod(deletedconstraints, deletedvariables);
					}
					++firstperiod;
					for (const Core::FMTconstraint& constraint : constraints)
					{
						this->eraseconstraint(constraint, firstperiod);
					}
					if (!constraintsonly)
					{
						graph.eraseperiod(deletedconstraints, deletedvariables, true);
					}
					for (const int& constraintid : deletedconstraints)
						{
						solver.deleteRow(constraintid);
						}
					for (const int& variableid : deletedvariables)
						{
						solver.deleteCol(variableid);
						}
					this->updatematrixngraph();
				}else {
					const int badperiod = std::max(firstperiod, 1);
					_exhandler->raise(Exception::FMTexc::FMTunboundedperiod,
						std::to_string(badperiod),"FMTlpmodel::eraseperiod", __LINE__, __FILE__);
					}

		}catch (...)
			{
				_exhandler->printexceptions("", "FMTlpmodel::eraseperiod", __LINE__, __FILE__);
			}

		 return graph.getstats();
		 }

     void FMTlpmodel::locatelevels(const std::vector<Core::FMToutputnode>& nodes,int period,
		 std::map<int,double>& variables,const Core::FMTconstraint& constraint)
            {
			try {
				std::vector<std::string>level_names = constraint.getvariablelevels();
				if (!level_names.empty())
					{
					for (const Core::FMToutputnode& node : nodes)
						{
						if (node.source.isvariablelevel())
							{
							std::string level_name = level_names.front();
							level_names.erase(level_names.begin());
							const int level_index = getsetlevel(constraint,level_name,period);
							variables[level_index] = node.constant;
							}
						}
					}
			}catch (...)
				{
				_exhandler->raisefromcatch(std::string(constraint),"FMTlpmodel::locatelevels", __LINE__, __FILE__);
				}
            }


    int FMTlpmodel::getsetlevel(const Core::FMTconstraint& constraint,const std::string& variable_level,int period)
        {
		try {
			for (const Core::FMTconstraint& model_constraint : constraints)
			{
				if (model_constraint != constraint)
				{
					const std::vector<std::string>level_names = model_constraint.getvariablelevels();
					if (!level_names.empty())
					{
						const std::vector<std::vector<int>>constraint_elements = getmatrixelement(model_constraint, period);
						std::vector<std::string>::const_iterator name_it = find(level_names.begin(), level_names.end(), variable_level);
						if (!constraint_elements.at(FMTmatrixelement::levelvariable).empty() &&
							(name_it != level_names.end())) // caught a constriant with level!
						{
							const size_t variable_location = std::distance(level_names.cbegin(), name_it);
							return constraint_elements.at(FMTmatrixelement::levelvariable).at(variable_location);
						}
					}
				}
			}
			std::map<int, double>no_index;
			return getsetmatrixelement(constraint, FMTmatrixelement::levelvariable, no_index, period);
		}catch (...)
			{
			_exhandler->raisefromcatch("for " + std::string(constraint),
				"FMTlpmodel::getsetlevel", __LINE__, __FILE__);
			}
		return -1;
        }

	bool FMTlpmodel::summarize(const std::map<int, double>& variables,
		std::vector<int>& sumvariables, std::vector<double>& sumcoefficiants) const
		{
		if (!variables.empty())
			{
			sumvariables.clear();
			sumcoefficiants.clear();
			sumvariables.reserve(variables.size());
			sumcoefficiants.reserve(variables.size());
			for (std::map<int,double>::const_iterator varit = variables.begin();varit!=variables.end();varit++)
				{
				sumvariables.push_back(varit->first);
				sumcoefficiants.push_back(varit->second);
				}
			return true;
			}
		return false;
		}

	bool FMTlpmodel::ismatrixelement(const Core::FMTconstraint& constraint,
		const FMTmatrixelement& element_type, int period) const
	{
		if (period == -1)
			{
			period = graph.getfirstactiveperiod()+1;
			}
		return((period < static_cast<int>(elements.size()) &&
			(elements.at(period).find(std::string(constraint)) != elements.at(period).end()) &&
			!elements.at(period).at(std::string(constraint)).at(element_type).empty()));
	}

	bool FMTlpmodel::issamematrixelement(const int& matrixindex, const FMTmatrixelement& element_type,
		const double& lowerb, const double& upperb, const std::map<int, double>& variables) const
		{
		try {
			double lower;
			double upper;
			double objective;
			std::vector<int>indicies;
			std::vector<double>elements;
		if (element_type == FMTmatrixelement::constraint)
			{
			const int rownelement = solver.getrow(matrixindex, lower, upper, indicies, elements);
		}else {
			const int colnelement = solver.getcol(matrixindex, lower, upper, objective, indicies, elements);
			}
		if (lowerb != lower && upperb != upper)
			{
			return false;
			}
		if (elements.size() == variables.size())
		{
			for (size_t elid = 0; elid < elements.size(); ++elid)
			{
				const std::map<int, double>::const_iterator itindex = variables.find(indicies.at(elid));
				if (itindex == variables.end() || itindex->second != elements.at(elid))
					{
					return false;
					}
			}
		}
		}catch (...)
			{
			_exhandler->raisefromcatch("for " + std::to_string(matrixindex)+
				solver.lowernuppertostr(lowerb,upperb)+
				std::to_string(element_type),"FMTlpmodel::issamematrixelement", __LINE__, __FILE__);
			}
		return true;
		}

	Graph::FMTgraphstats FMTlpmodel::setobjective(const Core::FMTconstraint& objective)
		{
		try {
			int first_period = 0;
			int last_period = 0;
			graph.constraintlenght(objective, first_period, last_period);
			double averagefactor = 1;
			if (last_period != first_period)
			{
				averagefactor = (1 / (last_period - first_period));
			}
			const std::vector<Core::FMToutputnode>all_nodes = objective.getnodes(area,actions,yields,averagefactor);
			std::map<int, double>all_variables;
			if (!objective.extravariables())
			{
				for (int period = first_period; period <= last_period; ++period)
				{
					locatenodes(all_nodes, period, all_variables);
					locatelevels(all_nodes, period, all_variables, objective);
				}
			}
			else {
				double lowerbound = 0;
				double upperbound = 0;
				if (objective.getconstrainttype() == Core::FMTconstrainttype::FMTMAXMINobjective)
				{
					upperbound = std::numeric_limits<double>::max();
				}
				else {
					lowerbound = std::numeric_limits<double>::lowest();
				}
				const int variable_id = getsetmatrixelement(objective, FMTmatrixelement::objectivevariable, all_variables);
				bool gotvariables = false;
				for (int period = first_period; period <= last_period; ++period)
				{
					std::map<int, double>period_variables;
					locatenodes(all_nodes, period, period_variables);
					locatelevels(all_nodes, period, all_variables, objective);
					if (!period_variables.empty())
					{
						period_variables[variable_id] = -1;
						const int constraint_id = getsetmatrixelement(objective, FMTmatrixelement::constraint, period_variables,
							period, lowerbound, upperbound);
						gotvariables = true;
					}
				}
				if (gotvariables)
				{
					all_variables[variable_id] = 1.0;
				}
			}
			if (objective.isgoal()) //process goal system...other constraints add to be added first!!
			{
				double penalty_sense = 1;
				const std::vector<std::string>targets = objective.getpenalties(penalty_sense);
				if (!getgoals(targets, all_variables, penalty_sense))
				{
					//No goals set...
				}
			}
			std::vector<double>finalobj(static_cast<size_t>(solver.getNumCols()), 0.0);
			for (int colid = 0; colid < solver.getNumCols(); ++colid)
			{
				std::map<int, double>::const_iterator cit = all_variables.find(colid);
				if (cit != all_variables.end())
				{
					finalobj[cit->first] = cit->second;
				}
			}
			try {
				solver.setObjective(&finalobj[0]);
			}catch (...)
				{
				_exhandler->raisefromcatch("","FMTlpmodel::setobjective", __LINE__, __FILE__);
				}
			if (all_variables.empty())
				{
				std::string cname = std::string(objective);
				cname.erase(std::remove(cname.begin(), cname.end(), '\n'), cname.end());
				_exhandler->raise(Exception::FMTexc::FMTnonaddedconstraint,
					std::string(cname),"FMTlpmodel::setobjective", __LINE__, __FILE__);
				}
			solver.setObjSense(objective.sense());
			}catch (...)
			{
				_exhandler->printexceptions(std::string(objective), "FMTlpmodel::setobjective", __LINE__, __FILE__);
			}

		return graph.getstats();
		}

     bool FMTlpmodel::resolve()
        {
		 try {
		return solver.resolve();
		 }catch (...)
		 {
			 _exhandler->printexceptions("", "FMTlpmodel::resolve", __LINE__, __FILE__);
		 }


		 return false;
        }


     bool FMTlpmodel::getgoals(const std::vector<std::string>& goalsnames, std::map<int, double>& index,const double& sense)const
        {
        bool found_something = false;
		try {
			for (const Core::FMTconstraint& constraint : constraints)
			{
				if (!constraint.isobjective() && constraint.isgoal())
				{
					std::string name;
					double value = 1;
					constraint.getgoal(name, value);
					value *= sense;
					if (goalsnames.at(0) == "ALL" || (std::find(goalsnames.begin(), goalsnames.end(), name) != goalsnames.end()))
					{
						int first_period = 0;
						int last_period = 0;
						graph.constraintlenght(constraint, first_period, last_period);
						for (int period = first_period; period <= last_period; ++period)
						{
							const std::vector<std::vector<int>>constraint_elements = getmatrixelement(constraint, period);
							if (!constraint_elements.at(FMTmatrixelement::goalvariable).empty())
							{
								found_something = true;
								index[constraint_elements.at(FMTmatrixelement::goalvariable).at(0)] = value;
							}
						}
					}
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTlpmodel::getgoals", __LINE__, __FILE__);
			}
        return found_something;
        }

    int FMTlpmodel::getsetmatrixelement(const Core::FMTconstraint& constraint,
                     const FMTmatrixelement& element_type,const std::map<int,double>& indexes,
                     int period,double lowerbound,double upperbound)
        {
		int element_id = 0;
		try {
			if (ismatrixelement(constraint, element_type, period))
			{
				int foundperiod = period;
				if (period == -1)
				{
					foundperiod = graph.getfirstactiveperiod() + 1;
				}
				const std::vector<int>& alllocalelements = elements.at(foundperiod).at(std::string(constraint)).at(element_type);
				for (const int& elid : alllocalelements)
				{
					if (issamematrixelement(elid, element_type, lowerbound, upperbound, indexes))
					{
						return elid;
					}
				}
			}
			else if (element_type == FMTmatrixelement::constraint && indexes.empty())
			{
				std::string cname = std::string(constraint);
				cname.erase(std::remove(cname.begin(), cname.end(), '\n'), cname.end());
				_exhandler->raise(Exception::FMTexc::FMTnonaddedconstraint,
					std::string(cname) + " at period " + std::to_string(period),
					"FMTlpmodel::getsetmatrixelement",__LINE__, __FILE__);
				return -1;
			}
			std::vector<int>sumvariables;
			std::vector<double>sumcoefficiants;
			summarize(indexes, sumvariables, sumcoefficiants);
			Graph::FMTgraphstats* stats = graph.getstatsptr();
			if (element_type == FMTmatrixelement::constraint)
			{
				element_id = stats->rows;
				solver.addRow(static_cast<int>(sumvariables.size()), &sumvariables[0], &sumcoefficiants[0], lowerbound, upperbound);
				++stats->rows;
				++stats->output_rows;
			}
			else {
				element_id = stats->cols;
				solver.addCol(0, &sumvariables[0], &sumcoefficiants[0], lowerbound, upperbound, 0);
				++stats->cols;
				++stats->output_cols;
			}
			if (elements.size() != (graph.size() - 1) && period >= static_cast<int>(elements.size())) //just for resizing!
			{
				size_t location = 0;
				if (period < 0)
				{
					location = graph.size();
				}
				else {
					location = period + 1;
				}
				const size_t to_resize = location - elements.size();
				if (to_resize > 0)
				{
					for (size_t id = 0; id < to_resize; ++id)
					{
						elements.push_back(std::unordered_map<std::string, std::vector<std::vector<int>>>());
					}
				}

			}
			int starting = period;
			int stoping = period + 1;
			if (period < 0)
			{
				starting = 0;
				stoping = static_cast<int>(elements.size());
			}
			for (int locid = starting; locid < stoping; ++locid)
			{
				if (elements[locid].find(std::string(constraint)) == elements[locid].end())
				{
					elements[locid][std::string(constraint)] = std::vector<std::vector<int>>(FMTmatrixelement::nr_items);
				}
				elements[locid][std::string(constraint)][element_type].push_back(element_id);
			}
		}catch (...)
			{
			_logger->logwithlevel(solver.getcacheelements(), 4);
			_exhandler->raisefromcatch(std::string(constraint)+" at period "+std::to_string(period)+
				solver.lowernuppertostr(lowerbound,upperbound),"FMTlpmodel::getsetmatrixelement", __LINE__, __FILE__);
			}
		return element_id;
        }

	std::vector<std::vector<int>>FMTlpmodel::getmatrixelement(const Core::FMTconstraint& constraint,int period) const
        {
        if ((period < static_cast<int>(elements.size())) && (elements.at(period).find(std::string(constraint)) != elements.at(period).end()))
            {
            return elements.at(period).at(std::string(constraint));
            }
        return std::vector<std::vector<int>>(FMTmatrixelement::nr_items);
        }

	Graph::FMTgraphstats FMTlpmodel::getstats() const
		{
		return graph.getstats();
		}

	int FMTlpmodel::getfirstactiveperiod() const
		{
		return graph.getfirstactiveperiod();
		}

	std::vector<Heuristics::FMToperatingareascheduler>FMTlpmodel::getoperatingareaschedulerheuristics(const std::vector<Heuristics::FMToperatingareascheme>& opareas,
																						const Core::FMToutputnode& node,
																						size_t numberofheuristics,
																						bool copysolver)
		{
		bool userandomness = false;
		size_t seedof = 0;
		double proportionofset = 0.25;
		std::vector<Heuristics::FMToperatingareascheduler>allheuristics;
		try {
			allheuristics.emplace_back(opareas, graph, *this, node,*this->getsolverptr(), seedof, proportionofset, userandomness, copysolver);
			for (size_t heuristicid = 1 ; heuristicid < numberofheuristics; ++heuristicid)
				{
				allheuristics.emplace_back(*allheuristics.begin());
				allheuristics.back().setasrandom();
				allheuristics.back().setgeneratorseed(seedof);
				allheuristics.back().passinobject(*this);
				seedof += 1;
				}
		}catch (...)
			{
				_exhandler->printexceptions("", "FMTlpmodel::getoperatingareaschedulerheuristics", __LINE__, __FILE__);
			}
		return allheuristics;
		}

    std::vector<Heuristics::FMToperatingareaclusterer>FMTlpmodel::getoperatingareaclustererheuristics(const std::vector<Heuristics::FMToperatingareacluster>& clusters,
																				const Core::FMToutput& statisticoutput,
                                                                                const Core::FMToutput& areaoutput,
                                                                                const int& period,
																				size_t numberofheuristics) const
        {
        size_t seedof = 0;
        std::vector<Heuristics::FMToperatingareaclusterer>allheuristics;
        Heuristics::FMToperatingareaclusterer baseclusterer=this->getclusterer(clusters,areaoutput,statisticoutput,period);
        try{
            for (size_t heuristicid = 1 ; heuristicid < numberofheuristics; ++heuristicid)
                {
                allheuristics.push_back(baseclusterer);
                allheuristics.back().passinobject(*this);
                allheuristics.back().setgeneratorseed(seedof);
                ++seedof;
                }
        }catch(...)
            {
            _exhandler->printexceptions("", "FMTlpmodel::getoperatingareaclustererheuristics", __LINE__, __FILE__);
            }
        return allheuristics;
        }



	bool FMTlpmodel::initialsolve()
		{
		try {
			return solver.initialsolve();
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTlpmodel::initialsolve", __LINE__, __FILE__);
		}

		return false;
		}

	std::vector<Core::FMTactualdevelopment>FMTlpmodel::getarea(int period) const
		{
		std::vector<Core::FMTactualdevelopment>returnedarea;
		try {
			if (period == 0)
			{
				return FMTmodel::getarea();
			};
			const double* modelsolution = solver.getColSolution();
			for (Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::lookconst_iterator vertexit = graph.begin(period);
				vertexit != graph.end(period); vertexit++)
			{
				if (graph.periodstart(vertexit->memoryobject))
				{
					const Core::FMTdevelopment& graphdevelopement = *vertexit->pointerobject;
					const double areaofdevelopement = graph.inarea(vertexit->memoryobject, modelsolution, true);
					if (areaofdevelopement > 0)
					{
						returnedarea.push_back(Core::FMTactualdevelopment(graphdevelopement, areaofdevelopement));
					}
				}
			}
		}catch (...)
			{
				_exhandler->printexceptions("", "FMTlpmodel::getarea", __LINE__, __FILE__);
			}

		return returnedarea;
		}

	FMTmodel FMTlpmodel::getcopy(int period) const
		{
		FMTmodel newmodel = FMTmodel::getcopy(period);
		newmodel.setarea(FMTlpmodel::getarea(period));
		return newmodel;
		}

	FMTlpmodel FMTlpmodel::getlocalmodel(FMTmodel localmodel, int period) const
		{
		if (!localmodel.empty())
			{
			//localmodel.setarea(std::vector<Core::FMTactualdevelopment>());
			//localmodel.push_back(FMTlpmodel::getcopy(period));
			localmodel.setarea(FMTlpmodel::getarea(period));
			}
		//Need to add a constraint over the global objective
		return FMTlpmodel (localmodel, solver.getsolvertype());
		}

	FMTlpsolver* FMTlpmodel::getsolverptr()
		{
		return &solver;
		}
	
	#if defined FMTWITHR
	Rcpp::DataFrame FMTlpmodel::getoutputsdataframe(const std::vector<Core::FMToutput>& outputsdata, int firstperiod, int lastperiod) const
		{
		Rcpp::DataFrame data = Rcpp::DataFrame();
		try {
			std::map<std::string, std::vector<double>>generalcatch;
			const double* solution = solver.getColSolution();
			for (int period  = firstperiod; period <= lastperiod;++period)
				{
				size_t outputid = 0;
				for (const Core::FMToutput& output : outputsdata)
					{
					const std::map<std::string,double> values  = graph.getoutput(*this, output, period, solution, Graph::FMToutputlevel::developpement);
					for (std::map<std::string, double>::const_iterator it = values.begin(); it!= values.end();++it)
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
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTlpmodel::getoutputsdataframe", __LINE__, __FILE__);
			}
		return data;
		}

	#endif 

	void FMTlpmodel::passinobjecttomembers(const Core::FMTobject& rhs)
	{
		try {
			graph.passinobject(rhs);
			solver.passinobject(rhs);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::passinobjecttomembers", __LINE__, __FILE__);
		}
	}

	void FMTlpmodel::passinobject(const Core::FMTobject& rhs)
	{
		try{
		FMTmodel::passinobject(rhs);
		passinobjecttomembers(rhs);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::passinobject", __LINE__, __FILE__);
		}

	}

	void FMTlpmodel::setdefaultlogger()
	{
		try{
		FMTmodel::setdefaultlogger();
		passinobjecttomembers(*this);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::setdefaultlogger", __LINE__, __FILE__);
		}
	}

	void FMTlpmodel::setquietlogger()
	{
		try{
		FMTmodel::setquietlogger();
		passinobjecttomembers(*this);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::setquietlogger", __LINE__, __FILE__);
		}
	}
	void FMTlpmodel::setdebuglogger()
	{
		try{
		FMTmodel::setdebuglogger();
		passinobjecttomembers(*this);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::setdebuglogger", __LINE__, __FILE__);
		}
	}
	void FMTlpmodel::setdefaultexceptionhandler()
	{
		try{
		FMTmodel::setdefaultexceptionhandler();
		passinobjecttomembers(*this);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::setdefaultexceptionhandler", __LINE__, __FILE__);
		}

	}
	void FMTlpmodel::setquietexceptionhandler()
	{
		try{
		FMTmodel::setquietexceptionhandler();
		passinobjecttomembers(*this);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::setquietexceptionhandler", __LINE__, __FILE__);
		}
	}
	void FMTlpmodel::setdebugexceptionhandler()
	{
		try{
		FMTmodel::setdebugexceptionhandler();
		passinobjecttomembers(*this);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::setdebugexceptionhandler", __LINE__, __FILE__);
		}
	}
	void FMTlpmodel::setfreeexceptionhandler()
	{
		try{
		FMTmodel::setfreeexceptionhandler();
		passinobjecttomembers(*this);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::setfreeexceptionhandler", __LINE__, __FILE__);
		}
	}
	void FMTlpmodel::disablenestedexceptions()
	{
		try{
		FMTmodel::disablenestedexceptions();
		passinobjecttomembers(*this);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::disablenestedexceptions", __LINE__, __FILE__);
		}
	}
	void FMTlpmodel::enablenestedexceptions()
	{
		try{
		FMTmodel::enablenestedexceptions();
		passinobjecttomembers(*this);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::enablenestedexceptions", __LINE__, __FILE__);
		}
	}
}



BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTlpmodel)
#endif
