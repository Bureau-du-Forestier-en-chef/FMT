/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

	
	std::shared_ptr<OsiSolverInterface>& FMTlpmodel::getsolverinterface()
	{
		return solverinterface;
	}

	

	Graph::FMTgraphstats FMTlpmodel::initializematrix()
	{
		const int ncols = static_cast<int>(graph.nedges());
        const std::vector<int>column_Starts(static_cast<size_t>(ncols) + 1, 0);
		const std::vector<int>targetrows(static_cast<size_t>(ncols), 0);
		const std::vector<double>nelements(ncols, 0.0);
		std::vector<double>lower_bounds(ncols, 0.0);
		std::vector<double>upper_bounds(ncols, 0.0);
		graph.getinitialbounds(lower_bounds, upper_bounds);
        solverinterface->addCols(ncols, &column_Starts[0], &targetrows[0],
			&nelements[0], &lower_bounds[0],
			&upper_bounds[0], &nelements[0]);
		solverinterface->setColSolution(&lower_bounds[0]);
		Graph::FMTgraphstats stats = graph.getstats();
		stats.cols = ncols;
        return stats;
	}

	bool FMTlpmodel::boundsolution(int period,double tolerance)
	{
    if (graph.size() > period)
		{
        const double* actual_solution = solverinterface->getColSolution();
		std::vector<int>variable_index;
		std::vector<double>bounds;
        for (const auto& descriptors : graph.getperiodverticies(period))
            {
            const Graph::FMTvertex_descriptor vertex_descriptor = descriptors.second;
			const std::map<int,int>variables = graph.getoutvariables(vertex_descriptor);
			for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end();varit++)
				{
				if (std::find(variable_index.begin(), variable_index.end(), varit->second)== variable_index.end())
					{
					variable_index.push_back(varit->second);
					//Had tolerance on primal infeasibilities with FMT_DBL_TOLERANCE ...
					bounds.push_back(*(actual_solution + varit->second)*(1-tolerance));
					bounds.push_back(*(actual_solution + varit->second)*(1+tolerance));
					}
				}
            }
		solverinterface->setColSetBounds(&variable_index[0], &variable_index.back()+1, &bounds[0]);
		this->resolve();
        return solverinterface->isProvenOptimal();
        }
    return false;
	}

	bool FMTlpmodel::isperiodbounded(int period) const
		{
		if (graph.size() > period)
			{
			size_t totaledges = 0;
			size_t boundededges = 0;
			const double* columnsupperbounds = solverinterface->getColUpper();
			const double* columnslowerbounds = solverinterface->getColLower();
			for (const auto& descriptors : graph.getperiodverticies(period))
				{
				const Graph::FMTvertex_descriptor& vertex_descriptor = descriptors.second;
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
		return false;
		}

	bool FMTlpmodel::unboundsolution(int period)
		{
		if (graph.size() > period && period > 0)//period >0 to not select actual developments!
			{
			std::vector<int>variable_index;
			std::vector<double>bounds;
			for (const auto& descriptors : graph.getperiodverticies(period))
				{
				const Graph::FMTvertex_descriptor vertex_descriptor = descriptors.second;
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
			solverinterface->setColSetBounds(&variable_index[0], &variable_index.back() + 1, &bounds[0]);
			return true;
			}
		return false;
		}

	bool FMTlpmodel::setsolution(int period, const Core::FMTschedule& schedule)
	{
		if (graph.size() > period && period > 0)
		{
			std::vector<double>new_solution(solverinterface->getNumCols(), 0);
			const double* actual_solution = solverinterface->getColSolution();
			const double* upper_bounds = solverinterface->getRowUpper();
			int colid = 0;
			for (double& nsol : new_solution)
			{
				new_solution[colid] = *(actual_solution + colid);
				++colid;
			}
			Core::FMTschedule locked_schedule;
			for (const auto actionit : schedule.elements)
			{
				int actionid = int(std::distance(actions.begin(), find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(actionit.first.getname()))));
				size_t allocated = 0;
				for (auto devit : actionit.second)
				{
					if (actionit.first.dorespectlock() && graph.containsdevelopment(devit.first))
					{
						const Graph::FMTvertex_descriptor vdescriptor = graph.getdevelopment(devit.first);
						const int variable = graph.getoutvariables(vdescriptor)[actionid];
						new_solution[variable] = devit.second.at(0);
						++allocated;
					}
					else if (!actionit.first.dorespectlock())
					{
						Core::FMTdevelopment locked(devit.first);
						int area_id = 0;
						int last_lock = 0;
						while (area_id < devit.second.size())
						{
							if (graph.containsdevelopment(locked))
							{
								const Graph::FMTvertex_descriptor vdescriptor = graph.getdevelopment(locked);
								const int variable = graph.getoutvariables(vdescriptor)[actionid];
								new_solution[variable] += devit.second.at(area_id);
								last_lock = locked.lock;
								++allocated;
								++area_id;
								++locked.lock;
							}
							if (area_id < devit.second.size())//continue looking for a developpement!
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

						}
					}
					else {
						_exhandler->raise(Exception::FMTexc::FMTmissingdevelopement,
							FMTwssect::Empty, std::string(devit.first) + " at period " + std::to_string(period) + " operated by " + actionit.first.getname(),
							__LINE__, __FILE__);
						return false;
					}
				}
			}
			//Fill up natural evolution
			std::unordered_map<size_t, Graph::FMTvertex_descriptor>processed;
			std::queue<Graph::FMTvertex_descriptor>descriptors;
			for (std::unordered_map<size_t, Graph::FMTvertex_descriptor>::const_iterator devit = graph.getperiodverticies(period).begin();
				devit != graph.getperiodverticies(period).end(); devit++)
			{
				if (graph.periodstart(devit->second))//get inperiod
				{
					const double* solution = &new_solution[0];
					double rest = graph.inarea(devit->second, solution);
					std::map<int, int>variables = graph.getoutvariables(devit->second);
					int growth = variables[-1];
					variables.erase(-1);
					for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						std::vector<Core::FMTdevelopmentpath> paths = graph.getpaths(devit->second, varit->first);
						for (const Core::FMTdevelopmentpath path : paths)
						{
							if (path.development->period == period && processed.find(boost::hash<Core::FMTdevelopment>()(*path.development)) == processed.end())
							{
								processed[boost::hash<Core::FMTdevelopment>()(*path.development)] = graph.getdevelopment(*path.development);

								descriptors.push(graph.getdevelopment(*path.development));
							}
						}
						rest -= new_solution[varit->second];
					}
					new_solution[growth] = rest;

				}
			}

			while (!descriptors.empty())
			{
				Graph::FMTvertex_descriptor first = descriptors.front();
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
						if (path.development->period == period && processed.find(boost::hash<Core::FMTdevelopment>()(*path.development)) == processed.end())
						{
							processed[boost::hash<Core::FMTdevelopment>()(*path.development)] = graph.getdevelopment(*path.development);
							descriptors.push(graph.getdevelopment(*path.development));
						}
					}
					rest -= new_solution[varit->second];
				}
				new_solution[growth] = rest;
				descriptors.pop();
			}
			solverinterface->setColSolution(&new_solution[0]);

		}
		return true;
	}

	Core::FMTschedule FMTlpmodel::getsolution(int period) const
	{
		if (graph.size() > period && period > 0)
		{

			std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::map<int, double>>>schedule_solution;
			const double* actual_solution = solverinterface->getColSolution();
			for (const auto deviterator : graph.getperiodverticies(period))
			{
				const Graph::FMTvertex_descriptor vertex = deviterator.second;
				std::map<int, int>variables = graph.getoutvariables(vertex);
				variables.erase(-1);
				if (!variables.empty())
				{
					for (const auto variable_iterator : variables)
					{
						if (*(actual_solution + variable_iterator.second) > 0) //basis solution only!!!
						{
							if (schedule_solution.find(actions[variable_iterator.first]) == schedule_solution.end())
							{
								schedule_solution[actions[variable_iterator.first]] = std::map<Core::FMTdevelopment, std::map<int, double>>();
							}
							const Core::FMTdevelopment& basedev = graph.getdevelopment(deviterator.second);
							Core::FMTdevelopment lockout = basedev.clearlock();
							if (schedule_solution[actions[variable_iterator.first]].find(lockout) == schedule_solution[actions[variable_iterator.first]].end())
							{
								schedule_solution[actions[variable_iterator.first]][lockout] = std::map<int, double>();
							}
							schedule_solution[actions[variable_iterator.first]][lockout][basedev.lock] = (*(actual_solution + variable_iterator.second));
						}
					}

				}
			}
			return Core::FMTschedule(period, schedule_solution);
		}
		return Core::FMTschedule();
	}

	Graph::FMTgraphstats FMTlpmodel::updatematrix(const std::unordered_map<size_t, Graph::FMTvertex_descriptor>& targets,
		const Graph::FMTgraphstats& newstats)
	{

		//columns
		const std::vector<int>column_Starts(static_cast<size_t>(newstats.cols)+1, 0);
		const std::vector<int>targetrows(newstats.cols, 0);
		const std::vector<double>nelements(newstats.cols, 0.0);
		const std::vector<double>lower_bounds(newstats.cols, 0.0);
		const std::vector<double>upper_bounds(newstats.cols, COIN_DBL_MAX);
		solverinterface->addCols(newstats.cols, &column_Starts[0], &targetrows[0],
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
		int newconstraintID = solverinterface->getNumRows();
		for (auto& vertex_it : targets)
		{
			graph.setconstraintID(vertex_it.second, newconstraintID);
			if (graph.gettransferrow(vertex_it.second, row_Starts, targetcols,elements))
			{

			}
			++oldstats.transfer_rows;
			++newconstraintID;
		}
		const int nrows = (newconstraintID - solverinterface->getNumRows());
		row_Starts.push_back(static_cast<int>(targetcols.size()));
		solverinterface->addRows(nrows, &row_Starts[0], &targetcols[0],
			&elements[0], &row_bounds[0], &row_bounds[0]);
		oldstats.cols = solverinterface->getNumCols();
		oldstats.rows = solverinterface->getNumRows();
		return oldstats;
	}


	FMTlpmodel::FMTlpmodel(const FMTmodel& base, FMTsolverinterface lsolvertype) :
		FMTmodel(base),
		solvertype(lsolvertype),
		graph(Graph::FMTgraphbuild::nobuild),
		solverinterface(),
		elements(),
		deletedconstraints(),
		deletedvariables()
	{

		solverinterface = FMTserializablematrix().buildsolverinterface(solvertype,&*this->_logger);
	}

	FMTlpmodel::FMTlpmodel() :
		FMTmodel(),
		solvertype(),
		graph(Graph::FMTgraphbuild::nobuild),
		solverinterface(),
		elements(),
		deletedconstraints(),
		deletedvariables()
	{

	}

	FMTlpmodel::FMTlpmodel(const FMTlpmodel& rhs) :
		FMTmodel(rhs),
		solvertype(rhs.solvertype),
		graph(rhs.graph),
		solverinterface(),
		elements(rhs.elements),
		deletedconstraints(rhs.deletedconstraints),
		deletedvariables(rhs.deletedvariables)
	{
		solverinterface = FMTserializablematrix().copysolverinterface(rhs.solverinterface,solvertype,&*this->_logger);
	}


	bool FMTlpmodel::operator == (const FMTlpmodel& rhs) const
	{
		return (FMTmodel::operator == (rhs) &&
			graph == rhs.graph &&
			solvertype == rhs.solvertype &&
			solverinterface == rhs.solverinterface &&
			elements == rhs.elements && 
			deletedconstraints == rhs.deletedconstraints &&
			deletedvariables == rhs.deletedvariables);
	}

	bool FMTlpmodel::operator != (const FMTlpmodel& rhs) const
	{
		return !(*this == rhs);
	}



	std::map<std::string, double> FMTlpmodel::getoutput(const Core::FMToutput& output,int period, Graph::FMToutputlevel level)
	{
		const double* solution = solverinterface->getColSolution();
		return graph.getoutput(*this, output, period, solution,level);
	}

	

	Graph::FMTgraphstats FMTlpmodel::buildperiod(Core::FMTschedule schedule, bool forcepartialbuild)
	{
			std::queue<Graph::FMTvertex_descriptor> actives;
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
		return graph.getstats();
	}


	FMTlpmodel& FMTlpmodel::operator = (const FMTlpmodel& rhs)
	{
		if (this != &rhs)
		{
			FMTmodel::operator = (rhs);
			solvertype = rhs.solvertype;
			graph = rhs.graph;
			deletedconstraints = rhs.deletedconstraints;
			deletedvariables = rhs.deletedvariables;
			solverinterface = FMTserializablematrix().copysolverinterface(rhs.solverinterface, solvertype, &*this->_logger);
		}
		return *this;
	}


bool FMTlpmodel::locatenodes(const std::vector<Core::FMToutputnode>& nodes, int period, 
	std::map<int, double>& variables, double multiplier) const
	{
	bool cashhit = false;
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
	return cashhit;
	}



	Graph::FMTgraphstats FMTlpmodel::setconstraint(const Core::FMTconstraint& constraint)
		{
		if (!constraint.isobjective())
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
				std::vector<Core::FMToutputnode>all_nodes = constraint.getnodes(averagefactor); 
				double lowerbound = 0;
				double upperbound = 0;
				double coef_multiplier_lower = 1;
				double coef_multiplier_upper = 1;
				double lower_variation = 0;
				double upper_variation = 0;
				constraint.getvariations(lower_variation,upper_variation);
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
						goal_variable = getsetmatrixelement(constraint, FMTmatrixelement::goalvariable, all_variables,period);
						all_variables[goal_variable] = 1;
						}

					if (constraint_type == Core::FMTconstrainttype::FMTevenflow)
						{
						coef_multiplier_lower = 1 - lower_variation;
						coef_multiplier_upper = 1 + lower_variation;
						lowerbound = 0;
						upperbound = 0;
						if (lower_variation !=0)
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
								const int lower_constraint_id = getsetmatrixelement(constraint, FMTmatrixelement::constraint, localvariables,-1, lowerbound, upperbound);
								}
							locatenodes(all_nodes, period, all_variables, 1);
							all_variables[lower_even_variable] = -1;
							lowerbound = 0;
							upperbound = std::numeric_limits<double>::max();
							if (all_variables.size()==1)
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


						}else if(constraint_type == Core::FMTconstrainttype::FMTnondeclining)
							{
							lowerbound = std::numeric_limits<double>::lowest();
							upperbound = 0;
							}else if(constraint_type == Core::FMTconstrainttype::FMTsequence)
								{
								coef_multiplier_upper = 1 + lower_variation;
								coef_multiplier_lower = 1 - upper_variation;
								lowerbound = 0;
								upperbound = 0;
								}else{
								constraint.getbounds(lowerbound,upperbound,period);
								}
						size_t left_location = 0;
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
						locatelevels(all_nodes, period, all_variables,constraint);
						std::map<int, double>lowervars = all_variables;
							if (coef_multiplier_lower!=1)
								{
									for (std::map<int, double>::iterator varit = lowervars.begin(); varit != lowervars.end(); varit++)
										{
											if (varit->second> 0 && varit->first != goal_variable)
											{
												varit->second *= coef_multiplier_lower;
											}
										}
								}
							const int lower_constraint_id = getsetmatrixelement(constraint, FMTmatrixelement::constraint, lowervars,
									period, lowerbound, upperbound);
							//ismultiple
							if (constraint.acrossperiod() && (coef_multiplier_lower != 1 || constraint_type == Core::FMTconstrainttype::FMTsequence) && coef_multiplier_upper!= 0)
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
		return graph.getstats();
		}

	std::map<std::string, std::vector<double>>FMTlpmodel::getvariabilities(const std::vector<Core::FMToutput>& outputs,double tolerance)
		{
		std::vector<double>originalcoefficients(solverinterface->getNumCols(),0.0);
		const double* originalcoef = solverinterface->getObjCoefficients();
		const double originalsense = solverinterface->getObjSense();
		std::vector<int>indexes;
		std::vector<double>coefficients;
		for (int colid = 0; colid < solverinterface->getNumCols();++colid)
			{
			originalcoefficients[colid] = *(originalcoef + colid);
			if (*(originalcoef + colid)!=0)
				{
				indexes.push_back(colid);
				coefficients.push_back(*(originalcoef + colid));
				}
			}
		const double originalvalue = solverinterface->getObjValue();
		const double lowerbound = originalvalue - tolerance;
		const double upperbound = originalvalue + tolerance;
		solverinterface->addRow(static_cast<int>(indexes.size()), &indexes[0], &coefficients[0], lowerbound, std::numeric_limits<double>::infinity());
		solverinterface->addRow(static_cast<int>(indexes.size()), &indexes[0], &coefficients[0], std::numeric_limits<double>::lowest(), upperbound);
		std::map<std::string, std::vector<double>>uppernlower;
		for (const Core::FMToutput& output : outputs)
			{
			Core::FMTconstraint maxconstraint(Core::FMTconstrainttype::FMTMAXobjective, output);
			maxconstraint.setlength();
			this->setobjective(maxconstraint);
			this->initialsolve();
			int first_period = 0;
			int last_period = 0;
			std::vector<double>uppervalues;
			graph.constraintlenght(maxconstraint, first_period, last_period);
			for (int period = first_period; period <= last_period; ++period)
				{
				uppervalues.push_back(this->getoutput(output, period).begin()->second);
				}
			uppernlower["U" + output.name] = uppervalues;
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
			uppernlower["L" + output.name] = lowervalues;
			this->eraseallconstraint(minconstraint);
			}
		solverinterface->setObjective(&originalcoefficients[0]);
		solverinterface->setObjSense(originalsense);
		this->initialsolve();
		return uppernlower;
		}

	Graph::FMTgraphstats FMTlpmodel::eraseallconstraint(const Core::FMTconstraint& constraint)
		{
		int first_period = 0;
		int last_period = 0;
		if (graph.constraintlenght(constraint, first_period, last_period))
			{
			for (int period = first_period; period<=last_period;++period)
				{
				this->eraseconstraint(constraint,period);
				}
			}
		return this->getstats();
		}

	Graph::FMTgraphstats FMTlpmodel::eraseconstraint(const Core::FMTconstraint& constraint, int period)
		{
		if (elements.size() > period && elements.at(period).find(constraint.hash())!= elements.at(period).end())
			{
			std::vector<std::vector<int>>all_elements = elements.at(period).at(constraint.hash());
			elements.at(period).erase(elements.at(period).find(constraint.hash()));
			int maxrowid = -1;
			int removedrow = -1;
			int maxcolid = -1;
			std::vector<int>colstoremove;
			std::vector<int>constraintstoremove;
			if (!all_elements.at(FMTmatrixelement::constraint).empty())
				{
				for (const int& levelid : all_elements.at(FMTmatrixelement::constraint))
				{
					bool removeconstraint = true;
					for (int locator = (period + 1); locator < elements.size(); ++locator)
					{
						for (std::unordered_map<size_t, std::vector<std::vector<int>>>::iterator elit = elements.at(locator).begin(); elit != elements.at(locator).end(); elit++)
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
						constraintstoremove.push_back(levelid);
						}
				}
				removedrow = static_cast<int>(constraintstoremove.size());
				deletedconstraints.insert(deletedconstraints.end(), constraintstoremove.begin(), constraintstoremove.end());
				graph.getstatsptr()->rows -= removedrow;
				graph.getstatsptr()->output_rows -= removedrow;
				}
			if (!all_elements.at(FMTmatrixelement::goalvariable).empty())
				{
				colstoremove.insert(colstoremove.end(),all_elements.at(FMTmatrixelement::goalvariable).begin(), all_elements.at(FMTmatrixelement::goalvariable).end());
				}

			if (!all_elements.at(FMTmatrixelement::levelvariable).empty() || !all_elements.at(FMTmatrixelement::objectivevariable).empty())
				{
				std::vector<int>potentialcols;
				potentialcols.insert(potentialcols.end(), all_elements.at(FMTmatrixelement::objectivevariable).begin(), all_elements.at(FMTmatrixelement::objectivevariable).end());
				potentialcols.insert(potentialcols.end(), all_elements.at(FMTmatrixelement::levelvariable).begin(), all_elements.at(FMTmatrixelement::levelvariable).end());
				bool candelete = true;
				for (const int& levelid : potentialcols)
					{
					for (int locator = (period+1); locator < elements.size();++locator)
						{
						for (std::unordered_map<size_t, std::vector<std::vector<int>>>::iterator elit = elements.at(locator).begin(); elit != elements.at(locator).end(); elit++)
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
						colstoremove.push_back(levelid);
						}
					}
				}

			if (!colstoremove.empty())
				{
				const int colssize = static_cast<int>(colstoremove.size());
				deletedvariables.insert(deletedvariables.end(), colstoremove.begin(), colstoremove.end());
				graph.getstatsptr()->cols-= colssize;
				graph.getstatsptr()->output_cols -= colssize;
				}
			}
		return graph.getstats();
		}

	void FMTlpmodel::updateconstraintsmapping(const std::vector<int>& Dvariables, const std::vector<int>& Dconstraints)
		{
		if (!Dvariables.empty() || !Dconstraints.empty())
			{
			std::vector<std::unordered_map<size_t, std::vector<std::vector<int>>>>::iterator periodit = elements.begin();
			while (periodit!= elements.end())
				{
				for (std::unordered_map<size_t, std::vector<std::vector<int>>>::iterator constraintit = periodit->begin(); constraintit != periodit->end(); constraintit++)
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
		}

	void FMTlpmodel::updatematrixelements(std::vector<int>& matrixelements, const std::vector<int>& deletedelements) const
		{
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
		}

	bool FMTlpmodel::updatematrixngraph()
		{
		if (!deletedconstraints.empty() || !deletedvariables.empty())
			{
			sort(deletedconstraints.begin(), deletedconstraints.end());
			sort(deletedvariables.begin(), deletedvariables.end());
			deletedconstraints.erase(unique(deletedconstraints.begin(), deletedconstraints.end()), deletedconstraints.end());
			deletedvariables.erase(unique(deletedvariables.begin(), deletedvariables.end()), deletedvariables.end());
			graph.updatematrixindex(deletedvariables, deletedconstraints);
			updateconstraintsmapping(deletedvariables, deletedconstraints);
			solverinterface->deleteRows(static_cast<int>(deletedconstraints.size()), &deletedconstraints[0]);
			//Cols
			solverinterface->deleteCols(static_cast<int>(deletedvariables.size()), &deletedvariables[0]);
			//Now clear the deleted
			deletedconstraints.clear();
			deletedvariables.clear();
			return true;
			}
		return false;
		}

	Graph::FMTgraphstats FMTlpmodel::eraseperiod()
		{
		int firstperiod = graph.getfirstactiveperiod();
		if (isperiodbounded(firstperiod))
			{
				graph.eraseperiod(deletedconstraints, deletedvariables);
				++firstperiod;
			for (const Core::FMTconstraint& constraint : constraints)
				{
				this->eraseconstraint(constraint, firstperiod);
				}
			graph.eraseperiod(deletedconstraints, deletedvariables,true);
			this->updatematrixngraph(); 
		}else {
			const int badperiod = std::max(firstperiod,1);
			_exhandler->raise(Exception::FMTexc::FMTunboundedperiod, FMTwssect::Empty, std::to_string(badperiod), __LINE__, __FILE__);
			}
		 return graph.getstats();
		 }

     void FMTlpmodel::locatelevels(const std::vector<Core::FMToutputnode>& nodes,int period,
		 std::map<int,double>& variables,const Core::FMTconstraint& constraint)
            {
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
            }


    int FMTlpmodel::getsetlevel(const Core::FMTconstraint& constraint,const std::string& variable_level,int period)
        {
        for (const Core::FMTconstraint& model_constraint : constraints)
            {
            if (model_constraint != constraint)
                {
				const std::vector<std::string>level_names = model_constraint.getvariablelevels();
                if (!level_names.empty())
                    {
                    const std::vector<std::vector<int>>constraint_elements = getmatrixelement(model_constraint,period);
					std::vector<std::string>::const_iterator name_it = find(level_names.begin(),level_names.end(),variable_level);
                    if (!constraint_elements.at(FMTmatrixelement::levelvariable).empty() &&
                        (name_it!= level_names.end())) // caught a constriant with level!
                        {
                        const size_t variable_location = std::distance(level_names.cbegin(),name_it);
                        return constraint_elements.at(FMTmatrixelement::levelvariable).at(variable_location);
                        }
                    }
                }
            }
		std::map<int, double>no_index;
        return getsetmatrixelement(constraint,FMTmatrixelement::levelvariable,no_index,period);
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
		return((period < elements.size() &&
			(elements.at(period).find(constraint.hash()) != elements.at(period).end()) &&
			!elements.at(period).at(constraint.hash()).at(element_type).empty()));
	}

	bool FMTlpmodel::issamematrixelement(const int& matrixindex, const FMTmatrixelement& element_type,
		const double& lowerb, const double& upperb, const std::map<int, double>& variables) const
		{
		if (element_type== FMTmatrixelement::constraint)
			{
			const double* upperbr = solverinterface->getRowUpper();
			const double* lowerbr = solverinterface->getRowLower();
			if (lowerb != *(lowerbr+ matrixindex) && upperb != *(lowerbr + matrixindex))
				{
				return false;
				}
			const CoinPackedMatrix* rowpacked = solverinterface->getMatrixByRow();
			const int vectorsize = rowpacked->getVectorSize(matrixindex);
			if (vectorsize == static_cast<int>(variables.size()))
				{
				const int* indicies = rowpacked->getIndices();
				const int* vectorstarts = rowpacked->getVectorStarts();
				const double* elements = rowpacked->getElements();
				for (int index = *(vectorstarts + matrixindex); index < (*(vectorstarts + matrixindex) + vectorsize);++index)
					{
					const std::map<int, double>::const_iterator itindex = variables.find(*(indicies+index));
					if (itindex==variables.end() || itindex->second != *(elements + index))
						{
						return false;
						}
					}
				}
		}else {
			const double* upperbcols = solverinterface->getColUpper();
			const double* lowerbcols = solverinterface->getColLower();
			if (lowerb != *(lowerbcols + matrixindex) || upperb != *(upperbcols + matrixindex))
				{
				return false;
				}
			const CoinPackedMatrix* colpacked = solverinterface->getMatrixByCol();
			const int vectorsize = colpacked->getVectorSize(matrixindex);
			if (vectorsize == static_cast<int>(variables.size()))
				{
				const int* indicies = colpacked->getIndices();
				const int* vectorstarts = colpacked->getVectorStarts();
				const double* elements = colpacked->getElements();
				for (int index = *(vectorstarts + matrixindex); index < (*(vectorstarts + matrixindex) + vectorsize); ++index)
					{
						const std::map<int, double>::const_iterator itindex = variables.find(*(indicies + index));
						if (itindex == variables.end() || itindex->second != *(elements + index))
						{
							return false;
						}
					}
				}
			}
		return true;
		}

	Graph::FMTgraphstats FMTlpmodel::setobjective(const Core::FMTconstraint& objective)
		{
		int first_period = 0;
		int last_period = 0;
		graph.constraintlenght(objective,first_period, last_period);
		double averagefactor = 1;
		if (last_period != first_period)
			{
			averagefactor = (1 / (last_period - first_period));
			}
		const std::vector<Core::FMToutputnode>all_nodes = objective.getnodes(averagefactor);
		std::map<int, double>all_variables;
		if (!objective.extravariables())
			{
			for (int period = first_period; period <= last_period; ++period)
				{
				locatenodes(all_nodes, period, all_variables);
				locatelevels(all_nodes, period, all_variables,objective);
				}
		}else {
			double lowerbound = 0;
			double upperbound = 0;
			if (objective.getconstrainttype() == Core::FMTconstrainttype::FMTMAXMINobjective)
				{
				upperbound = std::numeric_limits<double>::max();
			}else {
				lowerbound = std::numeric_limits<double>::lowest();
				}
			const int variable_id = getsetmatrixelement(objective,FMTmatrixelement::objectivevariable,all_variables);
			bool gotvariables = false;
			for (int period = first_period; period <= last_period; ++period)
				{
				std::map<int, double>period_variables;
				locatenodes(all_nodes, period, period_variables);
				locatelevels(all_nodes, period, all_variables,objective);
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
            if (!getgoals(targets,all_variables,penalty_sense))
                {
                //No goals set...
                }
            }
			std::vector<double>finalobj(static_cast<size_t>(solverinterface->getNumCols()),0.0);
			for (int colid = 0; colid < solverinterface->getNumCols();++colid)
				{
				std::map<int, double>::const_iterator cit = all_variables.find(colid);
				if (cit != all_variables.end())
					{
					finalobj[cit->first] = cit->second;
					}
				}
			solverinterface->setObjective(&finalobj[0]);
		if (all_variables.empty())
			{
			std::string cname = std::string(objective);
			cname.erase(std::remove(cname.begin(), cname.end(), '\n'), cname.end());
			_exhandler->raise(Exception::FMTexc::FMTnonaddedconstraint, FMTwssect::Empty, std::string(cname), __LINE__, __FILE__);
			}
		solverinterface->setObjSense(objective.sense());
		return graph.getstats();
		}

	double FMTlpmodel::getObjValue() const
		{
		return solverinterface->getObjValue();
		}

     bool FMTlpmodel::resolve()
        {
		solverinterface->resolve();
		return solverinterface->isProvenOptimal();
        }

	 void FMTlpmodel::writeLP(const std::string& location) const
		{
		solverinterface->writeLp(location.c_str());
		}
	 void FMTlpmodel::writeMPS(const std::string& location) const
		{
		solverinterface->writeMps(location.c_str());
		}

     bool FMTlpmodel::getgoals(const std::vector<std::string>& goalsnames, std::map<int, double>& index,const double& sense)const
        {
        bool found_something = false;
        for (const Core::FMTconstraint& constraint : constraints)
            {
            if (!constraint.isobjective() && constraint.isgoal())
                {
				std::string name;
                double value = 1;
                constraint.getgoal(name,value);
                value *= sense;
                if (goalsnames.at(0)== "ALL" || (std::find(goalsnames.begin(),goalsnames.end(),name)!=goalsnames.end()))
                    {
                    int first_period = 0;
                    int last_period = 0;
                    graph.constraintlenght(constraint,first_period, last_period);
                     for (int period = first_period; period <= last_period;++period)
                        {
						const std::vector<std::vector<int>>constraint_elements = getmatrixelement(constraint,period);
                        if (!constraint_elements.at(FMTmatrixelement::goalvariable).empty())
                            {
                            found_something = true;
							index[constraint_elements.at(FMTmatrixelement::goalvariable).at(0)] = value;
                            }
                        }
                    }
                }
            }
        return found_something;
        }

    int FMTlpmodel::getsetmatrixelement(const Core::FMTconstraint& constraint,
                     const FMTmatrixelement& element_type,const std::map<int,double>& indexes,
                     int period,double lowerbound,double upperbound)
        {
		if (ismatrixelement(constraint,element_type,period))
			{
			int foundperiod = period;
			if (period==-1)
				{
				foundperiod = graph.getfirstactiveperiod() + 1;
				}
			const std::vector<int>& alllocalelements = elements.at(foundperiod).at(constraint.hash()).at(element_type);
			for (const int& elid : alllocalelements)
				{
				if (issamematrixelement(elid, element_type, lowerbound, upperbound, indexes))
					{
					return elid;
					}
				}
		}else if (element_type == FMTmatrixelement::constraint && indexes.empty())
			{
			std::string cname = std::string(constraint);
			cname.erase(std::remove(cname.begin(), cname.end(), '\n'), cname.end());
			_exhandler->raise(Exception::FMTexc::FMTnonaddedconstraint, FMTwssect::Empty, std::string(cname), __LINE__, __FILE__);
			return -1;
			}
			int element_id = 0;
			std::vector<int>sumvariables;
			std::vector<double>sumcoefficiants;
			summarize(indexes, sumvariables, sumcoefficiants);
			Graph::FMTgraphstats* stats = graph.getstatsptr();
			if (element_type == FMTmatrixelement::constraint)
			{
				element_id = stats->rows;
				solverinterface->addRow(static_cast<int>(sumvariables.size()), &sumvariables[0], &sumcoefficiants[0], lowerbound, upperbound);
				++stats->rows;
				++stats->output_rows;
			}
			else {
				element_id = stats->cols;
				solverinterface->addCol(0, &sumvariables[0], &sumcoefficiants[0], lowerbound, upperbound, 0);
				++stats->cols;
				++stats->output_cols;
			}
			if (elements.size() != (graph.size() - 1) && period >= elements.size()) //just for resizing!
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
						elements.push_back(std::unordered_map<size_t, std::vector<std::vector<int>>>());
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
				if (elements[locid].find(constraint.hash()) == elements[locid].end())
				{
					elements[locid][constraint.hash()] = std::vector<std::vector<int>>(FMTmatrixelement::nr_items);
				}
				elements[locid][constraint.hash()][element_type].push_back(element_id);
			}
		return element_id;
        }

	std::vector<std::vector<int>>FMTlpmodel::getmatrixelement(const Core::FMTconstraint& constraint,int period) const
        {
        if ((period < elements.size()) && (elements.at(period).find(constraint.hash()) != elements.at(period).end()))
            {
            return elements.at(period).at(constraint.hash());
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

	std::vector<Heuristics::FMToperatingareaheuristic>FMTlpmodel::getoperatingareaheuristics(const std::vector<Heuristics::FMToperatingarea>& opareas,
																						const Core::FMToutputnode& node,
																						size_t numberofheuristics,
																						bool copysolver)
		{
		bool userandomness = false;
		size_t seedof = 0;
		double proportionofset = 0.25;
		std::vector<Heuristics::FMToperatingareaheuristic>allheuristics;
		allheuristics.emplace_back(opareas, graph, *this, node, solverinterface, solvertype, seedof, proportionofset, userandomness, copysolver);
		for (size_t heuristicid = 1 ; heuristicid < numberofheuristics; ++heuristicid)
			{
			allheuristics.emplace_back(*allheuristics.begin());
			allheuristics.back().setasrandom();
			allheuristics.back().setgeneratorseed(seedof);
			seedof += 1;
			}
		return allheuristics;
		}



	bool FMTlpmodel::initialsolve()
		{
		switch (solvertype)
		{
		case FMTsolverinterface::CLP:
				{
				//options.setSpecialOption(which,value1,value2)
				/** which translation is:
						 which:
						 0 - startup in Dual  (nothing if basis exists).:
									  0 - no basis
						   1 - crash
						   2 - use initiative about idiot! but no crash
						 1 - startup in Primal (nothing if basis exists):
									  0 - use initiative
						   1 - use crash
						   2 - use idiot and look at further info
						   3 - use sprint and look at further info
						   4 - use all slack
						   5 - use initiative but no idiot
						   6 - use initiative but no sprint
						   7 - use initiative but no crash
									  8 - do allslack or idiot
									  9 - do allslack or sprint
						   10 - slp before
						   11 - no nothing and primal(0)
						 2 - interrupt handling - 0 yes, 1 no (for threadsafe)
						 3 - whether to make +- 1matrix - 0 yes, 1 no
						 4 - for barrier
									  0 - dense cholesky
						   1 - Wssmp allowing some long columns
						   2 - Wssmp not allowing long columns
						   3 - Wssmp using KKT
									  4 - Using Florida ordering
						   8 - bit set to do scaling
						   16 - set to be aggressive with gamma/delta?
									  32 - Use KKT
						 5 - for presolve
									  1 - switch off dual stuff
						 6 - extra switches
					 */
				OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
				ClpSolve options;
				options.setSolveType(ClpSolve::useBarrier);
				//options.setSolveType(ClpSolve::useBarrierNoCross);
				//Do no cross over then when you get optimal switch to primal crossover!!!!
				//options.setSolveType(ClpSolve::tryDantzigWolfe);
				//options.setSolveType(ClpSolve::usePrimalorSprint);
				//options.setSolveType(ClpSolve::tryBenders);
				options.setPresolveType(ClpSolve::presolveOn);
				//options.setSpecialOption(1, 1);
				//options.setSpecialOption(1, 2);
				//options.setSpecialOption(4, 3, 4); //WSMP Florida
				//options.setSpecialOption(4, 0); //dense cholesky
				clpsolver->setSolveOptions(options);
				clpsolver->initialSolve();
				//ClpSolve simplexoptions;
				//simplexoptions.setSolveType(ClpSolve::usePrimal); //Or sprint?
				//simplexoptions.setSolveType(ClpSolve::usePrimalorSprint);
				//simplexoptions.setPresolveType(ClpSolve::presolveOn);
				//clpsolver->setSolveOptions(simplexoptions);
				//clpsolver->resolve();
				}
				break;
				#ifdef FMTWITHMOSEK
				case FMTsolverinterface::MOSEK:
					{
					OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
					msksolver->freeCachedData();
					MSKtask_t task = msksolver->getMutableLpPtr();
					MSK_putintparam(task, MSK_IPAR_OPTIMIZER, MSK_OPTIMIZER_INTPNT);
					MSK_putintparam(task, MSK_IPAR_INTPNT_BASIS, MSK_BI_IF_FEASIBLE);
					MSK_putintparam(task, MSK_IPAR_SIM_HOTSTART, MSK_SIM_HOTSTART_NONE);
					MSK_putintparam(task, MSK_IPAR_PRESOLVE_USE, MSK_ON);
					MSK_putintparam(task, MSK_IPAR_INTPNT_STARTING_POINT, MSK_STARTING_POINT_CONSTANT);
					MSK_putintparam(task, MSK_IPAR_BI_CLEAN_OPTIMIZER, MSK_OPTIMIZER_PRIMAL_SIMPLEX);
					MSK_putdouparam(task, MSK_DPAR_INTPNT_TOL_PSAFE, 100.0);
					MSK_putdouparam(task, MSK_DPAR_INTPNT_TOL_PATH, 1.0e-2);
					MSKrescodee error = MSK_optimize(task);
					//msksolver->initialSolve();
					if (msksolver->isLicenseError())
						{
						_exhandler->raise(Exception::FMTexc::FMTmissinglicense,
							FMTwssect::Empty, " Missing Mosek License ",
							__LINE__, __FILE__);
						}
					}
				#endif
				break;
				/*case FMTsolverinterface::CPLEX:
					solverinterface = unique_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
				break;
				case FMTsolverinterface::GUROBI:
					solverinterface = unique_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface);
				break;*/
				default:
					{
					solverinterface->initialSolve();
					}
				break;
			}
		return solverinterface->isProvenOptimal();
		}

}
#endif