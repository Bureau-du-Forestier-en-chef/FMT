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

#include "FMTlpmodel.h"

namespace Models
{

	void FMTlpmodel::buildsolverinterface()
	{
		switch (solvertype)
		{
		case FMTsolverinterface::CLP:
			solverinterface = unique_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface);
			break;
			/*case FMTsolverinterface::MOSEK:
				solverinterface = unique_ptr<OsiMskSolverInterface>(new OsiMskSolverInterface);
			break;
			case FMTsolverinterface::CPLEX:
				solverinterface = unique_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface);
			break;
			case FMTsolverinterface::GUROBI:
				solverinterface = unique_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface);
			break;*/
		default:
			solverinterface = unique_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface);
			break;
		}
	solverinterface->passInMessageHandler(&*this->_logger);
	}
	void FMTlpmodel::copysolverinterface(const unique_ptr<OsiSolverInterface>& solver_ptr)
	{
		switch (solvertype)
		{
		case FMTsolverinterface::CLP:
			solverinterface = unique_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface(*dynamic_cast<OsiClpSolverInterface*>(solver_ptr.get())));
			break;
			/*case FMTsolverinterface::MOSEK:
				solverinterface = unique_ptr<OsiMskSolverInterface>(new OsiMskSolverInterface(*dynamic_cast<OsiMskSolverInterface*>(solver_ptr.get())));
			break;
			case FMTsolverinterface::CPLEX:
				solverinterface = unique_ptr<OsiCpxSolverInterface>(new OsiCpxSolverInterface(*dynamic_cast<OsiCpxSolverInterface*>(solver_ptr.get())));
			break;
			case FMTsolverinterface::GUROBI:
				solverinterface = unique_ptr<OsiGrbSolverInterface>(new OsiGrbSolverInterface(*dynamic_cast<OsiGrbSolverInterface*>(solver_ptr.get())));
			break;*/
		default:
			solverinterface = unique_ptr<OsiClpSolverInterface>(new OsiClpSolverInterface(*dynamic_cast<OsiClpSolverInterface*>(solver_ptr.get())));
			break;
		}
	solverinterface->passInMessageHandler(&*this->_logger);
	}
	/*FMTlpmodel::FMTlpmodel() : FMTobject(), solvertype(), data(), solverinterface(), developments(), stats()
	{
		//this->buildsolverinterface();
	}

	FMTlpmodel::FMTlpmodel(FMTsolverinterface lsolvertype) : FMTobject(), solvertype(lsolvertype), data(), solverinterface(), developments(), stats()
	{
		this->buildsolverinterface();
	}*/

	unique_ptr<OsiSolverInterface>& FMTlpmodel::getsolverinterface()
	{
		return solverinterface;
	}

	/*FMTlpmodel::FMTlpmodel(const FMTlpmodel& rhs) :FMTobject(rhs), solvertype(rhs.solvertype), data(rhs.data), solverinterface(),
		developments(rhs.developments), stats(rhs.stats)
	{
		copysolverinterface(rhs.solverinterface);
	}*/

	/*FMTlpmodel& FMTlpmodel::operator = (const FMTlpmodel& rhs)
	{
		if (this != &rhs)
		{
			FMTobject::operator=(rhs);
			solvertype = rhs.solvertype;
			data = rhs.data;
			developments = rhs.developments;
			stats = rhs.stats;
			copysolverinterface(rhs.solverinterface);
		}
		return *this;
	}*/

	/*bool FMTlpmodel::containsdevelopment(const FMTdevelopment& developement) const
	{
		return ((developments.size() > (developement.period)) &&
			developments[(developement.period)].find(boost::hash<FMTdevelopment>()(developement)) != developments[(developement.period)].end());
	}*/

	/*map<string, double> FMTlpmodel::getsource(const FMToutputnode& node,
											int period,
											const FMTtheme& theme) const
		{
		map<string, double>values; ///start here
		for (auto attribute_id : theme.getvaluenames())
		{
			values[attribute_id.first] = 0;
		}
		//int graph_period = (period);//for inventory?


		if (developments.size() > period)//
		{
			//FMTotar target = source.gettarget();
			vector<int>action_IDS = node.source.targets(actions, action_aggregates);
			if (!(period==0 && !action_IDS.empty()) &&!node.source.isnull(yields) && !node.factor.isnull(yields) && (!action_IDS.empty() && !node.source.getaction().empty() || node.source.getaction().empty()))
			{
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "getting period " <<period << "\n";
				vector<FMTaction>selected;
				if (!action_IDS.empty())
				{
					selected.resize(action_IDS.size());
					int aid = 0;
					for (const int & actid : action_IDS)
					{
						selected[aid] = actions[actid];
						++aid;
					}
				}

				const double* solution = solverinterface->getColSolution();
				//const double* upper_bounds = solverinterface->getRowUpper();

				for (boost::unordered_map<size_t, FMTvertex_descriptor>::const_iterator it = developments[period].begin();
					it != developments[period].end(); it++)
				{
					const FMTvertex_descriptor& vertex_descriptor = it->second;
					const FMTdevelopment& development = data[vertex_descriptor].get();
					if (node.source.use(development, yields))
					{

						if (node.source.useinedges()) //in edges
						{
							if ((period == 0 || periodstart(vertex_descriptor)) &&
								(action_IDS.empty() || (!action_IDS.empty() &&
								(((_buildtype == FMTlpbuild::schedulebuild) && development.anyoperable(selected, yields)) || anyoperables(vertex_descriptor, action_IDS)))))
							{
								const string value = development.mask.get(theme);
								double coef = 1;
								coef = node.source.getcoef(development, yields) * node.factor.getcoef(development, yields) * node.constant;
								double area = 0;
								if (period == 0)
									{
									area = outarea(vertex_descriptor, -1, solution);
								}else {
									area = inarea(vertex_descriptor, solution, true);
									}
								values[value] += coef * area;
							}
						}
						else if (out_degree(vertex_descriptor, data) > 1 && (anyoperables(vertex_descriptor, action_IDS))) //out edges
						{
							const string value = development.mask.get(theme);
							int actionID = 0;
							for (const FMTaction& act : selected)
							{
								const string value = development.mask.get(theme);
								double action_value = 0;
								vector<FMTdevelopmentpath>paths = getpaths(vertex_descriptor, action_IDS[actionID]);
								double action_coef = node.source.getcoef(development, yields, act, paths) * node.factor.getcoef(development, yields) * node.constant;
								action_value = action_coef * (outarea(vertex_descriptor, action_IDS[actionID], solution));
								values[value] += action_value;
								++actionID;
							}
						}
					}

				}
			}
			double total = 0;
			for (auto valit : values)
			{
				total += valit.second;
			}
			values["Total"] = total;
		}
		return values;

		}*/

	/*vector<FMTaction>FMTlpmodel::selectedactions(const vector<int>& action_IDS) const
		{
		vector<FMTaction>selected;
		if (!action_IDS.empty())
		{
			selected.resize(action_IDS.size());
			int aid = 0;
			for (const int & actid : action_IDS)
			{
				selected[aid] = actions[actid];
				++aid;
			}
		}
		return selected;
		}*/

	/*map<string, double> FMTlpmodel::getsource(const FMToutputnode& node,
		int period,
		const FMTtheme& theme) const
	{
		map<string, double>values; ///start here
		for (auto attribute_id : theme.getvaluenames())
		{
			values[attribute_id.first] = 0;
		}
		vector<int>action_IDS;
		if (validouputnode(node, action_IDS, period))
			{
			vector<FMTaction>selected = selectedactions(action_IDS);
			const double* solution = solverinterface->getColSolution();
			bool inedges = false;
			for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator it = developments[period].begin();
				it != developments[period].end(); it++)
				{
				if (validgraphnode(inedges,it->second,node,action_IDS,selected))
					{
					const FMTdevelopment& development = data[it->second].get();
					const string value = development.mask.get(theme);
					if (inedges)
						{
						double coef = 1;
						coef = node.source.getcoef(development, yields) * node.factor.getcoef(development, yields) * node.constant;
						double area = 0;
						if (period == 0)
							{
							area = outarea(it->second, -1, solution);
							}
							else {
							area = inarea(it->second, solution, true);
							}
						values[value] += coef * area;
						}else {
						int actionID = 0;
						for (const FMTaction& act : selected)
							{
							double action_value = 0;
							vector<FMTdevelopmentpath>paths = getpaths(it->second, action_IDS[actionID]);
							double action_coef = node.source.getcoef(development, yields, act, paths) * node.factor.getcoef(development, yields) * node.constant;
							action_value = action_coef * (outarea(it->second, action_IDS[actionID], solution));
							values[value] += action_value;
							++actionID;
							}
						}

					}

				}
			double total = 0;
			for (auto valit : values)
			{
				total += valit.second;
			}
			values["Total"] = total;
			}
		return values;
	}*/


	/*bool FMTlpmodel::anyoperables(const FMTvertex_descriptor& descriptor, const vector<int>& action_ids) const
	{
		map<int, int>variables = getoutvariables(descriptor);
		for (const int& id : action_ids)
		{
			if (variables.find(id) != variables.end())
			{
				return true;
			}
		}
		return false;
	}*/

	/*FMTgraphstats FMTlpmodel::getstats() const
	{
		return stats;
	}*/

	/*void FMTlpmodel::setstats(const FMTgraphstats& newstats)
	{
		stats = newstats;
	}*/



	/*std::queue<FMTvertex_descriptor>FMTlpmodel::initialize(const vector<FMTactualdevelopment>& actdevelopments)
	{
        std::queue<FMTvertex_descriptor>actives;
		developments.clear();
		//P0 useless stuff with bounds on variables
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		developments.back().reserve(actdevelopments.size());
		//P1 part of the queue
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		developments.back().reserve(actdevelopments.size()); //should be exponential here...
        int constraint_id = -1;
        int edge_id = -1;
        double proportion = 100;
		stats.edges = 0;
		stats.vertices = 0;
		for (const FMTactualdevelopment& development : actdevelopments)
		{
		    if (!this->containsdevelopment(development))
                {
                //P0
				const FMTvertexproperties properties(development, constraint_id);
				FMTvertex_descriptor newvertex = add_vertex(properties, data);
				developments[0][boost::hash<FMTdevelopment>()(development)] = newvertex;
				++stats.vertices;
				//P1
                FMTfuturdevelopment P1dev(development);
                P1dev.period = 1;
                const FMTvertexproperties P1properties(P1dev, constraint_id);
                FMTvertex_descriptor P1vertex = add_vertex(P1properties, data);
                developments[1][boost::hash<FMTdevelopment>()(P1dev)] = P1vertex;
				++stats.vertices;
                actives.push(P1vertex);
                //Now set the edge!!
				const FMTedgeproperties newedge(edge_id, stats.edges, proportion);
				add_edge(newvertex,P1vertex, newedge, data);
				++stats.edges;
                }

		}
		return actives;
	}*/

	FMTgraphstats FMTlpmodel::initializematrix()
	{
		int ncols = static_cast<int>(graph.nedges());
        vector<int>column_Starts(ncols + 1, 0);
		vector<int>targetrows(ncols, 0);
		vector<double>nelements(ncols, 0.0);
		vector<double>lower_bounds(ncols, 0.0);
		vector<double>upper_bounds(ncols, 0.0);
		vector<double>objectives(ncols, 0.0);
		graph.getinitialbounds(lower_bounds, upper_bounds);
        solverinterface->addCols(ncols, &column_Starts[0], &targetrows[0],
			&nelements[0], &lower_bounds[0],
			&upper_bounds[0], &objectives[0]);
		solverinterface->setColSolution(&lower_bounds[0]);
		FMTgraphstats stats = graph.getstats();
		stats.cols = ncols;
        return stats;
	}


	/*vector<FMTactualdevelopment>FMTlpmodel::actuals(int location) const
	{
		vector<FMTactualdevelopment>all_devs;
		const double* actual_solution = solverinterface->getColSolution();
		for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator devit = developments[location].begin();
			devit != developments[location].end(); devit++)
		{

			double area = graph.inarea(devit->second, actual_solution);
			const FMTdevelopment& development = data[devit->second].get();
			all_devs.push_back(FMTactualdevelopment(development, area));
		}
		return all_devs;
	}*/

	bool FMTlpmodel::boundsolution(int period)
	{
    if (graph.size() > period)
		{
        const double* actual_solution = solverinterface->getColSolution();
		vector<int>variable_index;
		vector<double>bounds;
        for (const auto& descriptors : graph.getperiodverticies(period))
            {
            const FMTvertex_descriptor vertex_descriptor = descriptors.second;
			map<int,int>variables = graph.getoutvariables(vertex_descriptor);
			for (map<int, int>::const_iterator varit = variables.begin(); varit != variables.end();varit++)
				{
				if (std::find(variable_index.begin(), variable_index.end(), varit->second)== variable_index.end())
					{
					variable_index.push_back(varit->second);
					bounds.push_back(*(actual_solution + varit->second));
					bounds.push_back(*(actual_solution + varit->second));
					}
				}
            }
		solverinterface->setColSetBounds(&variable_index[0], &variable_index.back() - 1, &bounds[0]);
        return true;
        }
    return false;
	}

	bool FMTlpmodel::unboundsolution(int period)
		{
		if (graph.size() > period && period > 0)//period >0 to not select actual developments!
			{
			vector<int>variable_index;
			vector<double>bounds;
			for (const auto& descriptors : graph.getperiodverticies(period))
				{
				const FMTvertex_descriptor vertex_descriptor = descriptors.second;
				map<int, int>variables = graph.getoutvariables(vertex_descriptor);
				for (map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
					if (std::find(variable_index.begin(), variable_index.end(), varit->second) == variable_index.end())
						{
						variable_index.push_back(varit->second);
						bounds.push_back(-COIN_DBL_MAX);
						bounds.push_back(COIN_DBL_MAX);
						}
					}
				}
			solverinterface->setColSetBounds(&variable_index[0], &variable_index.back() - 1, &bounds[0]);
			return true;
			}
		return false;
		}

	bool FMTlpmodel::setsolution(int period, const FMTschedule& schedule)
	{
		if (graph.size() > period && period > 0)
		{
			vector<double>new_solution(solverinterface->getNumCols(), 0);
			const double* actual_solution = solverinterface->getColSolution();
			const double* upper_bounds = solverinterface->getRowUpper();
			int colid = 0;
			for (double& nsol : new_solution)
			{
				new_solution[colid] = *(actual_solution + colid);
				++colid;
			}
			FMTschedule locked_schedule;
			for (const auto actionit : schedule.elements)
			{
				int actionid = int(std::distance(actions.begin(), find_if(actions.begin(), actions.end(), FMTactioncomparator(actionit.first.name))));
				//iterating on basis variables action variables
				size_t allocated = 0;
				for (auto devit : actionit.second)
				{
					if (actionit.first.lock && graph.containsdevelopment(devit.first))
					{
						const FMTvertex_descriptor vdescriptor = graph.getdevelopment(devit.first);
						int variable = graph.getoutvariables(vdescriptor)[actionid];
						/*double total_value = 0;
						for (const double& val : devit.second)//can have multiple developments?!?!?!?! wtf...
							{
							total_value += val;
							}*/
						new_solution[variable] = devit.second.at(0);
						++allocated;
					}
					else if (!actionit.first.lock)
					{
						//the dev is suppose to be there so look while vey wacky...potential cluster fuck
						FMTdevelopment locked(devit.first);
						int area_id = 0;
						int last_lock = 0;
						while (area_id < devit.second.size())
						{
							//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "testing" << string(locked) <<" area of "<< devit.second.at(area_id) << "\n";
							
							if (graph.containsdevelopment(locked))
							{
								const FMTvertex_descriptor vdescriptor = graph.getdevelopment(locked);
								int variable = graph.getoutvariables(vdescriptor)[actionid];
								new_solution[variable] += devit.second.at(area_id);
								last_lock = locked.lock;
								++allocated;
								++area_id;
								++locked.lock;
							}
							if (area_id < devit.second.size())//continue looking for a developpement!
							{
								size_t checked = 0;
								size_t allocated_bound = (graph.getperiodverticies(period).size() - allocated);

								while (!graph.containsdevelopment(locked) &&
									checked < allocated_bound)
								{
									++locked.lock;
									++checked;
								}
								//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "allocated " << checked <<" "<< allocated_bound << "\n";
								if (checked == allocated_bound)//no lock available so get back to regular
								{
									locked.lock = last_lock; //go back to last good hit!
								}
							}

						}
					}
					else {
						_exhandler->raise(FMTexc::FMTmissingdevelopement,
							FMTwssect::Empty, string(devit.first) + " at period " + to_string(period) + " operated by " + actionit.first.name,
							__LINE__, __FILE__);
						return false;
					}
				}
			}
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "filling natural evolution "<< "\n";
			//Fill up natural evolution
			std::unordered_map<size_t, FMTvertex_descriptor>processed;
			std::queue<FMTvertex_descriptor>descriptors;

			//int growth_set = 0;
			for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator devit = graph.getperiodverticies(period).begin();
				devit != graph.getperiodverticies(period).end(); devit++)
			{
				if (graph.periodstart(devit->second))//get inperiod
				{
					const double* solution = &new_solution[0];
					double rest = graph.inarea(devit->second, solution);
					map<int, int>variables = graph.getoutvariables(devit->second);
					int growth = variables[-1];
					variables.erase(-1);
					for (map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
					{
						vector<FMTdevelopmentpath> paths = graph.getpaths(devit->second, varit->first);
						for (const FMTdevelopmentpath path : paths)
						{
							if (path.development->period == period && processed.find(boost::hash<FMTdevelopment>()(*path.development)) == processed.end())
							{
								//processed[boost::hash<FMTdevelopment>()(*path.development)] = developments[period][boost::hash<FMTdevelopment>()(*path.development)];
								processed[boost::hash<FMTdevelopment>()(*path.development)] = graph.getdevelopment(*path.development);

								descriptors.push(graph.getdevelopment(*path.development));
							}
						}
						rest -= new_solution[varit->second];
					}
					//++growth_set;
					new_solution[growth] = rest;

				}
			}

			while (!descriptors.empty())
			{
				FMTvertex_descriptor first = descriptors.front();
				map<int, int>variables = graph.getoutvariables(first);
				const double* solution = &new_solution[0];
				double rest = graph.inarea(first, solution);
				int growth = variables[-1];
				variables.erase(-1);
				for (map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
				{
					vector<FMTdevelopmentpath> paths = graph.getpaths(first, varit->first);
					for (const FMTdevelopmentpath path : paths)
					{
						if (path.development->period == period && processed.find(boost::hash<FMTdevelopment>()(*path.development)) == processed.end())
						{
							processed[boost::hash<FMTdevelopment>()(*path.development)] = graph.getdevelopment(*path.development);
							descriptors.push(graph.getdevelopment(*path.development));
						}
					}
					rest -= new_solution[varit->second];
				}
				//++growth_set;
				new_solution[growth] = rest;
				descriptors.pop();
			}
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << growth_set << " growth set " << "\n";
			solverinterface->setColSolution(&new_solution[0]);

		}
		return true;
	}

	FMTschedule FMTlpmodel::getsolution(int period) const
	{
		if (graph.size() > period && period > 0)
		{

			map<FMTaction, map<FMTdevelopment, map<int, double>>>schedule_solution;
			const double* actual_solution = solverinterface->getColSolution();
			for (const auto deviterator : graph.getperiodverticies(period))
			{
				const FMTvertex_descriptor vertex = deviterator.second;
				map<int, int>variables = graph.getoutvariables(vertex);
				variables.erase(-1);
				if (!variables.empty())
				{
					for (const auto variable_iterator : variables)
					{
						if (*(actual_solution + variable_iterator.second) > 0) //basis solution only!!!
						{
							if (schedule_solution.find(actions[variable_iterator.first]) == schedule_solution.end())
							{
								schedule_solution[actions[variable_iterator.first]] = map<FMTdevelopment, map<int, double>>();
							}
							//FMTvertexproperties properties = data[deviterator.second];
							const FMTdevelopment& basedev = graph.getdevelopment(deviterator.second);
							FMTdevelopment lockout = basedev.clearlock();
							//FMTdevelopment lockout = properties.get().clearlock(); //fix for lock!?!?!??!?!
							if (schedule_solution[actions[variable_iterator.first]].find(lockout) == schedule_solution[actions[variable_iterator.first]].end())
							{
								schedule_solution[actions[variable_iterator.first]][lockout] = map<int, double>();
							}
							schedule_solution[actions[variable_iterator.first]][lockout][basedev.lock] = (*(actual_solution + variable_iterator.second));
						}
					}

				}
			}
			return FMTschedule(period, schedule_solution);
		}
		return FMTschedule();
	}

	/*FMTgraphstats FMTlpmodel::buildschedule(std::queue<FMTvertex_descriptor> actives,
		const FMTschedule& schedule)
	{
		FMTgraphstats statsdiff(stats);
		//const double* bounds = solverinterface->getColUpper();
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		developments.back().reserve(actives.size()); //realy minimal here!?!?!
		//int queue_size = actives.size();
		//int op_hit = 0;
		while (!actives.empty())
		{
			const FMTvertex_descriptor front_vertex = actives.front();
			actives.pop();
			FMTvertexproperties front_properties = data[front_vertex];
			const FMTdevelopment active_development = front_properties.get();
			//boost::dynamic_bitset<> action_bits(actions.size(), false);
			int action_id = 0;
			for (const FMTaction& action : actions)
			{
				if (schedule.elements.find(action) != schedule.elements.end() &&
					(((schedule.elements.at(action)).find(active_development) != (schedule.elements.at(action)).end()) ||
						(!action.lock && active_development.lock != 0 && //lock exempt stuff...
						(schedule.elements.at(action)).find(active_development.clearlock()) != (schedule.elements.at(action)).end())))
				{
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " founda " << action.name <<" transition on "<< transitions[action_id].name<< "\n";
					const vector<FMTdevelopmentpath> paths = active_development.operate(action, transitions[action_id], yields, themes);
					addaction(action_id, statsdiff, actives,
						front_vertex, paths);
					//action_bits[action_id] = true;
				}
				++action_id;
			}
			FMTfuturdevelopment grown_up = active_development.grow();
			FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
			int variable_id = statsdiff.cols;
			++statsdiff.cols;
			int id = -1;
			double proportion = 100;
			const FMTedgeproperties newedge(id, variable_id, proportion);
			add_edge(front_vertex, next_period, newedge, data);
		}

		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " op hit  " << op_hit << "\n";
		return (statsdiff - stats);
	}*/

	/*FMTgraphstats FMTlpmodel::build(std::queue<FMTvertex_descriptor> actives)
	{
		FMTgraphstats statsdiff(stats);
		developments.push_back(std::unordered_map<size_t, FMTvertex_descriptor>());
		while (!actives.empty())
		{
			const FMTvertex_descriptor front_vertex = actives.front();
			actives.pop();
			FMTvertexproperties front_properties = data[front_vertex];
			const FMTdevelopment active_development = front_properties.get();

			int action_id = 0;
			for (const FMTaction& action : actions)
			{
				if (active_development.operable(action, yields))
				{
					const vector<FMTdevelopmentpath> paths = active_development.operate(action, transitions[action_id], yields, themes);
					addaction(action_id, statsdiff, actives, front_vertex, paths);
				}
				++action_id;
			}
			FMTfuturdevelopment grown_up = active_development.grow();
			FMTvertex_descriptor next_period = this->adddevelopment(grown_up); //getset
			const FMTedgeproperties newedge(-1, statsdiff.cols, 100);
			++statsdiff.cols;
			add_edge(front_vertex, next_period, newedge, data);
		}

		return (statsdiff - stats);
	}*/

	FMTgraphstats FMTlpmodel::updatematrix(const std::unordered_map<size_t, FMTvertex_descriptor>& targets,
		const FMTgraphstats& newstats)
	{

		//columns
		vector<int>column_Starts(newstats.cols+1, 0);
		vector<int>targetrows(newstats.cols, 0);
		vector<double>nelements(newstats.cols, 0.0);
		vector<double>lower_bounds(newstats.cols, 0.0);
		vector<double>upper_bounds(newstats.cols, COIN_DBL_MAX);
		vector<double>objectives(newstats.cols, 0.0);

		solverinterface->addCols(newstats.cols, &column_Starts[0], &targetrows[0],
			&nelements[0], &lower_bounds[0],
			&upper_bounds[0], &objectives[0]);

		//rows
		vector<int>row_Starts;
		row_Starts.reserve(targets.size());
		vector<int>targetcols;
		vector<double>elements;
		/*vector<double>row_lower;
		row_lower.reserve(targets.size());
		vector<double>row_upper;
		row_upper.reserve(targets.size());*/

		vector<double>row_lower(targets.size(), 0.0);
		vector<double>row_upper(targets.size(), 0.0);

		//Need to reset a new constraint ID!
		FMTgraphstats oldstats = graph.getstats();
		int newconstraintID = solverinterface->getNumRows();
		for (auto& vertex_it : targets)
		{
			/*FMTvertexproperties front_properties = data[vertex_it.second];
			front_properties.setconstraintID(newconstraintID);*/
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "Graph in! " << string(data[vertex_it.second].get()) << "\n";
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "CONST ID" << newconstraintID << "\n";
			graph.setconstraintID(vertex_it.second, newconstraintID);
			if (graph.gettransferrow(vertex_it.second, row_Starts, targetcols,elements))
			{

			}
			++oldstats.transfer_rows;
			++newconstraintID;
		}
		int nrows = (newconstraintID - solverinterface->getNumRows());
		row_Starts.push_back(int(targetcols.size()));
		/*Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " trsrow " << stats.transfer_rows << "\n";
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " srows " << solverinterface->getNumRows() << "\n";
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " nrows " << nrows << "\n";
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " target size " << targets.size() << "\n";*/
		solverinterface->addRows(nrows, &row_Starts[0], &targetcols[0],
			&elements[0], &row_lower[0], &row_upper[0]);
		oldstats.cols = solverinterface->getNumCols();
		oldstats.rows = solverinterface->getNumRows();
		return oldstats;
	}

	/*FMTvertex_descriptor FMTlpmodel::getdevelopment(const FMTdevelopment& developement) const
	{
		return developments[developement.period].at(boost::hash<FMTdevelopment>()(developement));
	}
	FMTvertex_descriptor FMTlpmodel::adddevelopment(const FMTfuturdevelopment& futurdevelopement)
	{
		if (!this->containsdevelopment(futurdevelopement))
		{
			int constraint_id = -1;//stats.rows;//getnumedges();
			const FMTvertexproperties properties(futurdevelopement, constraint_id);
			FMTvertex_descriptor newvertex = add_vertex(properties, data);
			developments[futurdevelopement.period][boost::hash<FMTdevelopment>()(futurdevelopement)] = newvertex;
			//++stats.rows;
			return newvertex;
		}
		return getdevelopment(futurdevelopement);
	}*/

	/*bool FMTlpmodel::gettransferrow(const FMTvertex_descriptor& vertex_descriptor,
		vector<int>&row_starts,
		vector<int>& cols,
		vector<double>& cols_value) const
	{
		//FMTvertexproperties front_properties = data[vertex_it.second];

		FMTinedge_iterator inedge_iterator, inedge_end;

		FMTvertexproperties vertex_property = data[vertex_descriptor];
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "transfer row of " <<string(vertex_property.get())<<" ID "<< vertex_property.getconstraintID() <<"\n";
		row_starts.push_back(int(cols.size()));

		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "setting " << vertex_property.get().mask.getstr() << "\n";
		for (tie(inedge_iterator, inedge_end) = in_edges(vertex_descriptor, data); inedge_iterator != inedge_end; ++inedge_iterator)
		{
			FMTedgeproperties edgeprop = data[*inedge_iterator];
			cols.push_back(edgeprop.getvariable());
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " FROM " << edgeprop.getvariable() << "\n";
			cols_value.push_back((edgeprop.getproportion() / 100));
		}
		FMTvertexproperties properties = data[vertex_descriptor];

		vector<int>locals;
		FMToutedge_iterator outedge_iterator, outedge_end;
		for (tie(outedge_iterator, outedge_end) = out_edges(vertex_descriptor, data); outedge_iterator != outedge_end; ++outedge_iterator)
		{
			FMTedgeproperties edgeprop = data[*outedge_iterator];
			int edgevar = edgeprop.getvariable();
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " STO " << edgevar << "\n";
			if (find(locals.begin(), locals.end(), edgevar) == locals.end())
			{
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " TO " << edgevar << "\n";
				cols.push_back(edgevar);
				locals.push_back(edgevar);
				cols_value.push_back(-1);
			}
		}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "Adding Row! " <<vertex_property.get().mask.getstr()<<" AGE "<<
			//vertex_property.get().age<<" PERIOD "<< vertex_property.get().period <<" LOCK "<< vertex_property.get().lock<< "\n";
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << cols.size() << " LOWER " << row_lower << " UPPER " << row_upper << "\n";
		
		//++stats.transfer_rows;
		return true;
	}*/

	/*void FMTlpmodel::addaction(const int& actionID,
		FMTgraphstats& statsdiff,
		std::queue<FMTvertex_descriptor>& actives,
		const FMTvertex_descriptor& out_vertex,
		const vector<FMTdevelopmentpath>& paths)
	{
		int variable_id = statsdiff.cols;//solverinterface->getNumCols();
		++statsdiff.cols;
		vector<FMTvertex_descriptor>active_vertex;
		//bool addcol = false;
		//bool newdev = false;
		for (const FMTdevelopmentpath& devpath : paths)
		{
			const FMTedgeproperties newedge(actionID, variable_id, devpath.proportion);
			
			FMTvertex_descriptor tovertex;
			//if not operable for any actions
			//then directly grow and add it to the newactive for next period!
			
			if (!this->containsdevelopment(*devpath.development))
			{
				tovertex = this->adddevelopment(*devpath.development);
				actives.push(tovertex);
			}
			else {
				tovertex = this->adddevelopment(*devpath.development);
			}
			//}
			add_edge(out_vertex, tovertex, newedge, data);


		}
	}*/

	/*double FMTlpmodel::outarea(const FMTvertex_descriptor& out_vertex,
		const int& actionID, const double*&  solution) const
	{
		FMToutedge_iterator outedge_iterator, outedge_end;
		//const double* solution = solverinterface->getColSolution();
		double value = 0;
		for (tie(outedge_iterator, outedge_end) = out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
		{
			FMTedgeproperties edgeprop = data[*outedge_iterator];
			if (edgeprop.getactionID() == actionID)
			{
				value += *(solution + edgeprop.getvariable()) * (edgeprop.getproportion() / 100);
			}
		}
		return value;
	}*/

	/*double FMTlpmodel::getRHS(const FMTvertex_descriptor& out_vertex, const double*& bounds) const
	{
		FMTvertexproperties properties = data[out_vertex];
		if (properties.getconstraintID() > 0)//last period no update matrix done!
		{
			return *(bounds + properties.getconstraintID());
		}
		return 0.0;
	}*/

	/*double FMTlpmodel::inarea(const FMTvertex_descriptor& out_vertex, const double*&  solution, bool growth) const
	{
		FMTinedge_iterator inedge_iterator, inedge_end;
		FMTvertexproperties properties = data[out_vertex];
		double area = 0;// = abs(getRHS(out_vertex, bounds));
		//if (area == 0)
		//{
			//const double* solution = solverinterface->getColSolution();
			for (tie(inedge_iterator, inedge_end) = in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				FMTedgeproperties edgeprop = data[*inedge_iterator];
				
				if (edgeprop.getactionID() < 0 || !growth) //only from gorwth!!!!!! not from the operated!!!!
				{
					area += *(solution + edgeprop.getvariable()) * (edgeprop.getproportion() / 100);
					//break;
				}
			}
		//}
		return area;
	}*/

	/*bool FMTlpmodel::periodstart(const FMTvertex_descriptor& out_vertex) const
	{
		FMTvertexproperties properties = data[out_vertex];

			FMTinedge_iterator inedge_iterator, inedge_end;
			for (tie(inedge_iterator, inedge_end) = in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
			{
				FMTedgeproperties edgeprop = data[*inedge_iterator];
				FMTvertexproperties source_vecterx = data[source(*inedge_iterator, data)];
				if (source_vecterx.get().period < properties.get().period)
				{
					return true;
				}
			}
	return false;
	}*/

	/*bool FMTlpmodel::periodstop(const FMTvertex_descriptor& out_vertex) const
	{
		FMToutedge_iterator outedge_iterator, outedge_end;
		FMTvertexproperties source_properties = data[out_vertex];
		if (source_properties.get().period==0)
			{
			return true;
			}
		for (tie(outedge_iterator, outedge_end) = out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
		{
			FMTedgeproperties edgeprop = data[*outedge_iterator];
			FMTvertexproperties target_vecterx = data[target(*outedge_iterator, data)];
			if (source_properties.get().period < target_vecterx.get().period)
			{
				return true;
			}
		}
		return false;
	}*/

	/*vector<FMTdevelopmentpath>FMTlpmodel::getpaths(const FMTvertex_descriptor& out_vertex,
		const int& actionID) const
	{
		vector<FMTdevelopmentpath>paths;
		for (FMToutedge_pair edge_pair = out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
		{
			FMTedgeproperties edgeprop = data[*edge_pair.first];
			if (edgeprop.getactionID() == actionID)
			{
				FMTvertexproperties vertex_target = data[target(*edge_pair.first, data)];
				paths.push_back(FMTdevelopmentpath(vertex_target.get(), edgeprop.getproportion()));
			}
		}
		return paths;
	}*/

	/*map<int, int> FMTlpmodel::getoutvariables(const FMTvertex_descriptor& out_vertex) const
	{
		map<int, int> mapping;
		FMToutedge_pair edge_pair;
		for (edge_pair = out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
		{
			FMTedgeproperties edgeprop = data[*edge_pair.first];
			int actionid = edgeprop.getactionID();
			mapping[actionid] = edgeprop.getvariable();
		}
		return mapping;
	}*/


	FMTlpmodel::FMTlpmodel(const FMTmodel& base, FMTsolverinterface lsolvertype) :
		FMTmodel(base),
		solvertype(lsolvertype),
		graph(FMTgraphbuild::nobuild),
		solverinterface(),
		elements()
	{
		buildsolverinterface();
	}

	FMTlpmodel::FMTlpmodel() :
		FMTmodel(),
		solvertype(),
		graph(FMTgraphbuild::nobuild),
		solverinterface(),
		elements()
	{

	}

	FMTlpmodel::FMTlpmodel(const FMTlpmodel& rhs) :
		FMTmodel(rhs),
		solvertype(rhs.solvertype),
		graph(rhs.graph),
		solverinterface(),
		elements(rhs.elements)
	{
		copysolverinterface(rhs.solverinterface);
	}

	/*bool FMTlpmodel::samegraph(const FMTlpmodel& rhs) const
	{
		return (area == rhs.area &&
			actions == rhs.actions &&
			transitions == rhs.transitions &&
			yields == rhs.yields);
	}*/

	/*void FMTlpmodel::setgraph(const FMTlpmodel& rhs)
	{
		solvertype = rhs.solvertype;
		graph = rhs.graph;
		copysolverinterface(rhs.solverinterface);
	}*/

	bool FMTlpmodel::operator == (const FMTlpmodel& rhs) const
	{
		return (FMTmodel::operator == (rhs) &&
			graph == rhs.graph &&
			solvertype == rhs.solvertype &&
			solverinterface == rhs.solverinterface &&
			elements == rhs.elements);
	}

	bool FMTlpmodel::operator != (const FMTlpmodel& rhs) const
	{
		return !(*this == rhs);
	}

	/*vector<FMTactualdevelopment>FMTlpmodel::getarea(int period) const
		{
		if (period == 0)
		{
			return area;
		}
		return actuals(period);
		//return actuals(period);
		}*/

	map<string, double> FMTlpmodel::getoutput(const FMToutput& output,int period, FMToutputlevel level)
	{
		const double* solution = solverinterface->getColSolution();
		return graph.getoutput(*this, output, period, solution,level);
	}

	FMTtheme FMTlpmodel::locatestatictheme() const
		{
		FMTtheme besttheme;
		vector<FMTtheme>bestthemes = getthemes();
		for (const FMTtransition& transition : gettransitions())
			{
			bestthemes = transition.getstaticthemes(bestthemes);
			}
		size_t themesize = 0;
		for (const FMTtheme& theme : bestthemes)
			{
			if (themesize < theme.size())
				{
				besttheme = theme;
				themesize = theme.size();
				}
			}
		return besttheme;
		}

	FMTgraphstats FMTlpmodel::buildperiod(FMTschedule schedule, bool forcepartialbuild)
	{
		/*if ((developments.size() == (period) || (period == 1)))
		{*/

			/*if (!developments.empty())
				{
				--location;
				}*/
			std::queue<FMTvertex_descriptor> actives;
			FMTgraphstats buildstats;
			if (graph.empty())
			{
				actives = graph.initialize(area);
				buildstats = initializematrix();
				graph.setstats(buildstats);
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "initialize done" << "\n";
			}
			else {
				actives = graph.getactiveverticies();
			}
			if (!forcepartialbuild && schedule.empty()) // full build
			{
				buildstats = graph.build(*this,actives);
				graph.setbuildtype(FMTgraphbuild::fullbuild);
			}
			else {//partial build for result ?
				graph.setbuildtype(FMTgraphbuild::schedulebuild);
				buildstats = graph.buildschedule(*this,actives, schedule);
			}

			//FMTgraph<solvertype>::hashlast(actions, yields);
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "build done" << "\n";
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "build stats: " << string(buildstats) << "\n";
			int location = static_cast<int>(graph.size() - 2);
			FMTgraphstats newstats = this->updatematrix(graph.getperiodverticies(location), buildstats);
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "update matrix done" << "\n";
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "new stats: " << string(newstats) << "\n";
			graph.setstats(newstats);
		//}
		return graph.getstats();
	}

	/*FMTgraphstats FMTlpmodel::eraseperiod()
		{
		vector<int>columns_id;
		columns_id.reserve(developments.begin()->size()*5);
		vector<int>rows_id;
		rows_id.reserve(developments.begin()->size());
		for (std::unordered_map<size_t, FMTvertex_descriptor>::iterator it = developments[0].begin();
			it != developments[0].end(); it++)
            {
            FMTvertex_descriptor vertex_location = it->second;
			FMTvertexproperties vertex_properties = data[vertex_location];
			int constraint_id = vertex_properties.getconstraintID();
			if (constraint_id >= 0)
				{
				if (std::find(rows_id.begin(), rows_id.end(), constraint_id) == rows_id.end())
					{
					rows_id.push_back(constraint_id);
					--stats.rows;
					--stats.transfer_rows;
					}
				}
			FMTinedge_iterator inedge_iterator, inedge_end;
			for (tie(inedge_iterator, inedge_end) = in_edges(vertex_location, data); inedge_iterator != inedge_end; ++inedge_iterator)
				{
				FMTedgeproperties edgeprop = data[*inedge_iterator];
				int variable = edgeprop.getvariable();
				if (std::find(columns_id.begin(), columns_id.end(), variable)==columns_id.end())
					{
					columns_id.push_back(variable);
					--stats.cols;
					}
				remove_edge(*inedge_iterator, data);
				--stats.edges;
				}
			if (!periodstop(vertex_location))//remove in edges only!
				{
				FMToutedge_iterator outedge_iterator, outedge_end;
				for (tie(outedge_iterator, outedge_end) = out_edges(vertex_location, data); outedge_iterator != outedge_end; ++outedge_iterator)
					{
					FMTedgeproperties edgeprop = data[*outedge_iterator];
					remove_edge(*outedge_iterator, data);
					--stats.edges;
					}
				}
			remove_vertex(vertex_location, data);
			--stats.vertices;
            }
		//Now clean the matrix!!!
		solverinterface->deleteRows(int(rows_id.size()), &rows_id[0]);
		solverinterface->deleteCols(int(columns_id.size()), &columns_id[0]);
		return this->getstats();
		}*/

	FMTlpmodel& FMTlpmodel::operator = (const FMTlpmodel& rhs)
	{
		if (this != &rhs)
		{
			FMTmodel::operator = (rhs);
			solvertype = rhs.solvertype;
			graph = rhs.graph;
			copysolverinterface(rhs.solverinterface);
		}
		return *this;
	}


	/*bool FMTlpmodel::validouputnode(const FMToutputnode& node, vector<int>& action_IDS, int period) const
		{
		if (developments.size() > period)//
			{
			action_IDS = node.source.targets(actions, action_aggregates);
			if (!(period == 0 && !action_IDS.empty()) && !node.source.isnull(yields) && !node.factor.isnull(yields) && (!action_IDS.empty() && !node.source.getaction().empty() || node.source.getaction().empty()))
				{
				return true;
				}
			}
		return false;
		}*/

	/*bool FMTlpmodel::validgraphnode(bool& inedges, const FMTvertex_descriptor& vertex_descriptor,
		const FMToutputnode& node, const vector<int>& action_IDS, const vector<FMTaction>& selected) const
		{
		inedges = false;
		const FMTdevelopment& development = data[vertex_descriptor].get();
		if (node.source.use(development, yields))
			{
			if (node.source.useinedges()) //in edges
				{
					if ((development.period == 0 || periodstart(vertex_descriptor)) &&
						(action_IDS.empty() || (!action_IDS.empty() &&
						(((_buildtype == FMTlpbuild::schedulebuild) && development.anyoperable(selected, yields)) || anyoperables(vertex_descriptor, action_IDS)))))
					{
					inedges = true;
					return true;
					}
				}else if (out_degree(vertex_descriptor, data) > 1 && (anyoperables(vertex_descriptor, action_IDS))) //out edges
				{
				return true;
				}
			}
		return false;
		}*/

	/*void FMTlpmodel::locatenodes(const vector<FMToutputnode>& nodes, int period,
		vector<int>& variables, vector<double>& coefs,double multiplier) const
		{
		vector<int> action_IDS;
		for (FMToutputnode output_node : nodes)
			{

			//change the period if the node is single well a other potential cluster fuck
			int node_period = period;

			if (output_node.source.useinedges())//evaluate at the begining of the other period if inventory! what a major fuck
                {
                ++node_period;
                }

			if (output_node.singleperiod())
                {
                 //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "node location: " << output_node.source.getperiodlowerbound() << "\n";
                if (output_node.ispastperiod())
                    {
                     
                    if ((output_node.source.getperiodlowerbound() + period) >= 0)
                        {
                       // node_period  = (output_node.source.getperiodlowerbound() + period);
                       // Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " BOUNDING on "<<node_period << "\n";
                        FMTperbounds perbound(FMTwssect::Optimize,node_period,node_period);
                        output_node.source.setbounds(perbound);
                        output_node.factor.setbounds(perbound);
                        }else{
                        continue;//dont need that node...
                        }
                    }else{
                    node_period  = output_node.source.getperiodlowerbound();
                    if (output_node.source.useinedges())
                        {
                        ++node_period;
                        }
                    }
                }

            if (!output_node.source.isvariablelevel())
                {
                if (validouputnode(output_node,action_IDS, node_period ))
                    {
                    vector<FMTaction>selected = selectedactions(action_IDS);
                    bool inedges = false;
                    for (std::unordered_map<size_t, FMTvertex_descriptor>::const_iterator it = developments[node_period ].begin();
                        it != developments[node_period ].end(); it++)
                        {
                        if (validgraphnode(inedges, it->second, output_node, action_IDS, selected))
                            {
                            const FMTdevelopment& development = data[it->second].get();
                            //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " got  "<<string(development) << "\n";
                            if (inedges)
                                {
                                //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " in edges!  " << "\n";
                                double coef = 1;
                                coef = output_node.source.getcoef(development, yields) * output_node.factor.getcoef(development, yields) * output_node.constant;
                                if (node_period  == 0)
                                    {

                                    map<int, int>vars = getoutvariables(it->second);
                                    variables.push_back(vars.at(-1));
                                    coefs.push_back(coef*multiplier);
                                }else {
                                    FMTinedge_iterator inedge_iterator, inedge_end;
                                    for (tie(inedge_iterator, inedge_end) = in_edges(it->second, data); inedge_iterator != inedge_end; ++inedge_iterator)
                                        {
                                        FMTedgeproperties edgeprop = data[*inedge_iterator];
                                        variables.push_back(edgeprop.getvariable());
                                        coefs.push_back((edgeprop.getproportion() / 100)*coef*multiplier);
                                        }
                                    }
                            }else {
                                int actionID = 0;
                                map<int,int>outvars = getoutvariables(it->second);
                                for (const FMTaction& act : selected)
                                    {
                                    if (outvars.find(action_IDS.at(actionID))!=outvars.end())
                                        {
                                        double action_value = 0;
                                        vector<FMTdevelopmentpath>paths = getpaths(it->second, action_IDS.at(actionID));
                                        double action_coef = output_node.source.getcoef(development, yields, act, paths) * output_node.factor.getcoef(development, yields) * output_node.constant;
                                        //variables.push_back(outvars.at(actionID));
                                        variables.push_back(outvars.at(action_IDS.at(actionID)));
                                        coefs.push_back(action_coef*multiplier);
                                        }
                                    ++actionID;
                                    }
                                }
                            }
                        }
                    }
                }
			}
		}*/

bool FMTlpmodel::locatenodes(const vector<FMToutputnode>& nodes, int period, 
	map<int, double>& variables, double multiplier) const
	{
	bool cashhit = false;
	for (const FMToutputnode& node : nodes)
		{
		map<int, double>node_map;
		node_map = graph.locatenode(*this, node, period);//go into the graph
		for (map<int, double>::const_iterator node_it = node_map.begin(); node_it != node_map.end(); node_it++)
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



	FMTgraphstats FMTlpmodel::setconstraint(const FMTconstraint& constraint)
		{
		if (!constraint.isobjective())
			{
           /* for (const FMToutputsource& src : constraint.getsources())
                    {
                    Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<string(src)<<"\n";
                    Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "targetss " <<src.emptyperiod()<<" "<<src.getperiodlowerbound()<<" "<<src.getperiodupperbound()<< "\n";
                    }*/
			int first_period = 0;
            int last_period = 0;
			if (graph.constraintlenght(constraint, first_period, last_period))
				{
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << first_period << " " << last_period << "\n";
				FMTconstrainttype constraint_type = constraint.getconstrainttype();
				double averagefactor = 1;
				if (last_period != first_period)
					{
					averagefactor = (1 / (last_period - first_period));
					}
				vector<FMToutputnode>all_nodes = constraint.getnodes(averagefactor); //
				double lowerbound = 0;
				double upperbound = 0;
				double coef_multiplier_lower = 1;
				double coef_multiplier_upper = 1;
				double lower_variation = 0;
				double upper_variation = 0;
				constraint.getvariations(lower_variation,upper_variation);
				/*coef_multiplier_lower += 1;
				coef_multiplier_upper += 1;*/
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " start period :"<<first_period << "\n";
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " stop period :"<<last_period << "\n";
				//double uppersense = -1; // 1 is MODIFY RHS and -1 MODIFY LHS
				
				int upper_even_variable = -1;
				int lower_even_variable = -1;

				if (constraint_type == FMTconstrainttype::FMTevenflow && lower_variation != 0)
					{
					++last_period;
					}


				for (int period = first_period; period <= last_period; ++period)
					{
					map<int, double>all_variables;
					int goal_variable = -1;
					if (constraint.isgoal())
						{
						goal_variable = addmatrixelement(constraint, FMTmatrixelement::goalvariable, all_variables,period);
						all_variables[goal_variable] = 1;
						}

					if (constraint_type == FMTconstrainttype::FMTevenflow)
						{
						coef_multiplier_lower = 1 - lower_variation;
						coef_multiplier_upper = 1 + lower_variation;
						lowerbound = 0;
						upperbound = 0;
						if (lower_variation !=0)//create a variable for each periods!
							{
							if (upper_even_variable < 0 && lower_even_variable < 0)
								{
								map<int, double>localvariables;
								upper_even_variable = addmatrixelement(constraint, FMTmatrixelement::objectivevariable, localvariables);
								lower_even_variable = addmatrixelement(constraint, FMTmatrixelement::objectivevariable, localvariables);
								localvariables[upper_even_variable] = coef_multiplier_lower;
								localvariables[lower_even_variable] = -1;
								//Aplly to all constraint periods association!
								lowerbound = numeric_limits<double>::lowest();
								upperbound = 0;
								int lower_constraint_id = addmatrixelement(constraint, FMTmatrixelement::constraint, localvariables,-1, lowerbound, upperbound);
								}
							//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "on period " << period << "\n";
							locatenodes(all_nodes, period, all_variables, 1);
							all_variables[lower_even_variable] = -1;
							lowerbound = 0;
							upperbound = numeric_limits<double>::max();
							int lowervalue = addmatrixelement(constraint, FMTmatrixelement::constraint, all_variables,
									period, lowerbound,upperbound);
							all_variables.erase(lower_even_variable);
							all_variables[upper_even_variable] = -1;
							lowerbound = numeric_limits<double>::lowest();
							upperbound = 0;
							int uppervalue = addmatrixelement(constraint, FMTmatrixelement::constraint, all_variables,
								period, lowerbound, upperbound);
							if (period == last_period)
								{
								return graph.getstats();
								}
							continue;
							}


						}else if(constraint_type == FMTconstrainttype::FMTnondeclining)
							{
							lowerbound = numeric_limits<double>::lowest();
							upperbound = 0;
							//RHSperiodsneeded.push_back(period + 1); //just the next period
							}else if(constraint_type == FMTconstrainttype::FMTsequence)
								{
								//uppersense = 1;
								coef_multiplier_upper = 1 + lower_variation;//upper_variation;//1 + lower_variation; // 1 + upper_variation; 
								coef_multiplier_lower = 1 - upper_variation;
								lowerbound = 0;
								upperbound = 0;
								//RHSperiodsneeded.push_back(period + 1); // just the next period
								}else{
								constraint.getbounds(lowerbound,upperbound,period);
								}
						//vector<int>all_variables;
					   // vector<double>all_coefs;
						
						size_t left_location = 0;
						//locatenodes(all_nodes, period, all_variables, coef_multiplier_lower);
						locatenodes(all_nodes, period, all_variables, 1);
						
						//graph.locatenodes(*this,all_nodes, period, all_variables, coef_multiplier_lower);
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "1. got  n variables :"<< all_variables.size()<<" PEriod "<<period << "\n";

						//Check for goals!!!
						


						if (constraint.acrossperiod())
							{
							//if (!(constraint_type == FMTconstrainttype::FMTevenflow && coef_multiplier_lower != 1))
								//{
								locatenodes(all_nodes, (period + 1), all_variables, -1);
								//}
							if (coef_multiplier_lower != 1)
								{
								lowerbound = numeric_limits<double>::lowest();
								upperbound = 0;
								}
							}

						//level part
						locatelevels(all_nodes, period, all_variables, /*all_coefs,*/constraint);

							map<int, double>lowervars = all_variables;
							if (coef_multiplier_lower!=1)
								{
								/*if (constraint_type == FMTconstrainttype::FMTevenflow)
									{
									lowervars[upper_even_variable] = -1;
									}else{*/
									for (map<int, double>::iterator varit = lowervars.begin(); varit != lowervars.end(); varit++)
										{
											if (varit->second> 0 && varit->first != goal_variable)
											{
												varit->second *= coef_multiplier_lower;
											}
										}
									//}
								}
							int lower_constraint_id = addmatrixelement(constraint,FMTmatrixelement::constraint, lowervars,
													 period,lowerbound, upperbound);

							//ismultiple
							if (constraint.acrossperiod() && coef_multiplier_lower != 1 && coef_multiplier_upper!= 0)
								{
								map<int, double>uppervars = all_variables;
								if (coef_multiplier_upper != 1)
									{
									for (map<int, double>::iterator varit = uppervars.begin(); varit != uppervars.end(); varit++)
										{
										if (varit->second > 0 && varit->first != goal_variable)
											{
											varit->second *= coef_multiplier_upper;
											}
										}
									}
								lowerbound = 0;
								upperbound = numeric_limits<double>::max();
								int upper_constraint_id = addmatrixelement(constraint,FMTmatrixelement::constraint, uppervars,
													period,lowerbound, upperbound);

								}

						}
				}
			}
		return graph.getstats();
		}

	FMTgraphstats FMTlpmodel::removeconstraint(const FMTconstraint& constraint, int period)
		{
		//clear node variables caching 
		if (elements.size() > period && elements.at(period).find(constraint.hash())!= elements.at(period).end())
			{
			vector<vector<int>>all_elements = elements.at(period).at(constraint.hash());
			elements.at(period).erase(elements.at(period).find(constraint.hash()));
			int maxrowid = -1;
			int removedrow = -1;
			int maxcolid = -1;
			vector<int>colstoremove;
			if (!all_elements.at(FMTmatrixelement::constraint).empty())
				{
				maxrowid = *max_element(all_elements.at(FMTmatrixelement::constraint).begin(), all_elements.at(FMTmatrixelement::constraint).end());
				removedrow = static_cast<int>(all_elements.at(FMTmatrixelement::constraint).size());
				solverinterface->deleteRows(removedrow, &all_elements.at(FMTmatrixelement::constraint)[0]);
				graph.getstatsptr()->rows -= removedrow;
				graph.getstatsptr()->output_rows -= removedrow;
				}
			

			if (!all_elements.at(FMTmatrixelement::goalvariable).empty())
				{
				maxcolid = *max_element(all_elements.at(FMTmatrixelement::goalvariable).begin(), all_elements.at(FMTmatrixelement::goalvariable).end());
				colstoremove = all_elements.at(FMTmatrixelement::goalvariable);
				}

			if (!all_elements.at(FMTmatrixelement::levelvariable).empty())
				{
				//check if the level is still use somewhere else
				bool candelete = true;
				for (const int& levelid : all_elements.at(FMTmatrixelement::levelvariable))
					{
					for (std::unordered_map<size_t, vector<vector<int>>>::iterator elit = elements.at(period).begin(); elit != elements.at(period).end(); elit++)
						{
						if (std::find(elit->second.at(FMTmatrixelement::levelvariable).begin(), elit->second.at(FMTmatrixelement::levelvariable).end(),levelid)!= elit->second.at(FMTmatrixelement::levelvariable).end())
							{
							candelete = false;
							break;
							}	
						}
					if (candelete)
						{
						colstoremove.push_back(levelid);
						if (levelid> maxcolid)
							{
							maxcolid = levelid;
							}
						}
					}
				}

			if (!colstoremove.empty())
				{
				int colssize = static_cast<int>(colstoremove.size());
				solverinterface->deleteCols(colssize, &colstoremove[0]);
				graph.getstatsptr()->cols-= colssize;
				graph.getstatsptr()->output_cols -= colssize;
				}


			for (size_t locperiod = period; locperiod < elements.size(); ++locperiod)
					{
					for (std::unordered_map<size_t, vector<vector<int>>>::iterator elit = elements.at(locperiod).begin(); elit != elements.at(locperiod).end(); elit++)
						{
						if (maxrowid!=-1)
							{
							for (int& rowid : elit->second.at(FMTmatrixelement::constraint))
								{
									if (rowid > maxrowid)
									{
										rowid -= removedrow;
									}
								}
							}
						if (maxcolid!=-1)
							{
							int rcolsize = static_cast<int>(colstoremove.size());
							for (int& colid : elit->second.at(FMTmatrixelement::levelvariable))
								{
								if (colid > maxcolid)
									{
									colid -= rcolsize;
									}
								}
							for (int& colid : elit->second.at(FMTmatrixelement::objectivevariable))
								{
									if (colid > maxcolid)
									{
										colid -= rcolsize;
									}
								}
							for (int& colid : elit->second.at(FMTmatrixelement::goalvariable))
								{
									if (colid > maxcolid)
									{
										colid -= rcolsize;
									}
								}

							}
				
						}
					}
			}
		return graph.getstats();
		}

     void FMTlpmodel::locatelevels(const vector<FMToutputnode>& nodes,int period,
            /*vector<int>& variables,vector<double>& coefs*/map<int,double>& variables,const FMTconstraint& constraint)
            {
            vector<string>level_names = constraint.getvariablelevels();
            if (!level_names.empty())
                {
                for (const FMToutputnode& node : nodes)
                    {
                    if (node.source.isvariablelevel())
                        {
                        string level_name = level_names.front();
                        level_names.erase(level_names.begin());
                        int level_index = getsetlevel(constraint,level_name,period);
						variables[level_index] = node.constant;
                        //variables.push_back(level_index);
                       // coefs.push_back(node.constant);
                        }
                    }
                }
            }

	/*void FMTlpmodel::constraintlenght(const FMTconstraint& constraint,int& start, int& stop) const
		{
		start = constraint.getperiodlowerbound();
		stop = (constraint.getperiodupperbound() > (developments.size() - 2)) ? (developments.size() - 2) : constraint.getperiodupperbound();
		if (constraint.acrossperiod())
            {
            --stop;
            }
		}*/

    int FMTlpmodel::getsetlevel(const FMTconstraint& constraint,const string& variable_level,int period)
        {
        //first need to check if other contraints at the period as this level!
        for (const FMTconstraint& model_constraint : constraints)
            {
            if (model_constraint != constraint)
                {
                vector<string>level_names = model_constraint.getvariablelevels();
                if (!level_names.empty())
                    {
                    const vector<vector<int>>constraint_elements = getmatrixelement(model_constraint,period);
                    vector<string>::const_iterator name_it = find(level_names.begin(),level_names.end(),variable_level);
                    if (!constraint_elements.at(FMTmatrixelement::levelvariable).empty() &&
                        (name_it!= level_names.end())) // caught a constriant with level!
                        {
                        //Is it the same level?
                        size_t variable_location = std::distance(level_names.cbegin(),name_it);
                        return constraint_elements.at(FMTmatrixelement::levelvariable).at(variable_location);
                        }
                    }
                }
            }
        //If no conresponding level found create one!
        //vector<int>no_index;
        //vector<double>no_coefs;
		map<int, double>no_index;
        return addmatrixelement(constraint,FMTmatrixelement::levelvariable,no_index/*,no_coefs*/,period);
        }

	bool FMTlpmodel::summarize(/*vector<int> variables, vector<double> coefficiants*/const map<int, double>& variables,
		vector<int>& sumvariables, vector<double>& sumcoefficiants) const
		{
		if (!variables.empty())
			{
			sumvariables.clear();
			sumcoefficiants.clear();
			sumvariables.reserve(variables.size());
			sumcoefficiants.reserve(variables.size());
			for (map<int,double>::const_iterator varit = variables.begin();varit!=variables.end();varit++)
				{
				sumvariables.push_back(varit->first);
				sumcoefficiants.push_back(varit->second);
				}
			return true;
			}
		return false;
		/*if (!variables.empty())
			{
			sumvariables.clear();
			sumvariables.push_back(variables.back());
			variables.pop_back();
			sumcoefficiants.clear();
			sumcoefficiants.push_back(coefficiants.back());
			coefficiants.pop_back();
			while (!variables.empty())
			{
				vector<int>::iterator var_it = std::find(sumvariables.begin(), sumvariables.end(), variables.back());
				//if (!std::isnan(coefficiants.back()))
				//{
					if (var_it != sumvariables.end())
					{
						size_t location = std::distance(sumvariables.begin(), var_it);
					

						
						sumcoefficiants[location] += coefficiants.back();
					}
					else {
						sumvariables.push_back(variables.back());
						sumcoefficiants.push_back(coefficiants.back());
					}
				//}
				variables.pop_back();
				coefficiants.pop_back();
			}
			return true;
			}
		return false;*/
		}

	FMTgraphstats FMTlpmodel::setobjective(const FMTconstraint& objective)
		{
		int first_period = 0;
		int last_period = 0;

		graph.constraintlenght(objective,first_period, last_period);

		/*Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "first period " <<first_period << "\n";
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "last period " <<last_period << "\n";*/
		double averagefactor = 1;
		if (last_period != first_period)
			{
			averagefactor = (1 / (last_period - first_period));
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "average factors! " << averagefactor << "\n";
		vector<FMToutputnode>all_nodes = objective.getnodes(averagefactor);
		//vector<int>all_variables;
		//vector<double>all_coefs;
		map<int, double>all_variables;
		if (!objective.extravariables())//simple to
			{
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "NO EXTRAAAA "<< first_period<<" "<< last_period << "\n";
			for (int period = first_period; period <= last_period; ++period)
				{
				//graph.locatenodes(*this,all_nodes, period, all_variables/*, all_coefs*/);
				locatenodes(all_nodes, period, all_variables);
				locatelevels(all_nodes, period, all_variables/*, all_coefs*/,objective);
				}
		}else {
			double lowerbound = 0;
			double upperbound = 0;
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "WIT EXTRA " << first_period << " " << last_period << "\n";
			if (objective.getconstrainttype() == FMTconstrainttype::FMTMAXMINobjective)
				{
				upperbound = numeric_limits<double>::max();
			}else {
				lowerbound = numeric_limits<double>::lowest();
				}
			int variable_id = addmatrixelement(objective,FMTmatrixelement::objectivevariable,all_variables/*,all_coefs*/);
			for (int period = first_period; period <= last_period; ++period)
				{
				//vector<int>period_variables;
				//vector<double>period_coefs;
				map<int, double>period_variables;
				//graph.locatenodes(*this,all_nodes, period, period_variables/*, period_coefs*/);
				locatenodes(all_nodes, period, period_variables);
				locatelevels(all_nodes, period, all_variables,/* all_coefs,*/objective);
				//period_variables.push_back(variable_id);
				//period_coefs.push_back(-1);
				period_variables[variable_id] = -1;
				int constraint_id = addmatrixelement(objective,FMTmatrixelement::constraint,period_variables/*,
                                         period_coefs*/,period,lowerbound, upperbound);

				}
			all_variables[variable_id] = 1.0;
            //all_variables.push_back(variable_id);
           // all_coefs.push_back(1.0);
			}
        if (objective.isgoal()) //process goal system...other constraints add to be added first!!
            {
            double penalty_sense = 1;
            vector<string>targets = objective.getpenalties(penalty_sense);
            if (!getgoals(targets,all_variables,/*all_coefs,*/penalty_sense))
                {
               // Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "nothing found!! " <<penalty_sense << "\n";
                //No goals set...
                }
            }
		vector<int>objective_variables;
		vector<double>objective_coefficiants;
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "NODES SIZE "<< all_variables .size()<< "\n";
		if (summarize(all_variables, /*all_coefs,*/ objective_variables, objective_coefficiants))
			{
			solverinterface->setObjCoeffSet(&objective_variables[0], &objective_variables[objective_variables.size()], &objective_coefficiants[0]);
			}
       //&all_variables[all_variables.size() - 1]
		solverinterface->setObjSense(objective.sense());
		return graph.getstats();
		}

     bool FMTlpmodel::resolve()
        {
        //solverinterface->initialSolve();
		 solverinterface->resolve();
		/*const double* solve = solverinterface->getColSolution();
		for (size_t id = 0 ; id < solverinterface->getNumCols();++id)
			{
			Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "var  " << *(solve+id) << "\n";
			}*/
		return solverinterface->isProvenOptimal();
        }

	 size_t FMTlpmodel::buildoutputscache(const vector<FMToutput>& outputs)
		{
		 vector<const FMToutput*>outs;
		 for (const FMToutput& out : outputs)
			{
			 outs.push_back(&out);
			}
		return graph.buildoutputscache(*this, outs);
		}

	 size_t FMTlpmodel::buildconstraintscache(const vector<FMTconstraint>& constraints)
		{
		vector<const FMToutput*>outs;
		for (const FMTconstraint& out : constraints)
			{
			 outs.push_back(&out);
			}
		return graph.buildoutputscache(*this, outs);
		}

     bool FMTlpmodel::getgoals(const vector<string>& goalsnames, map<int, double>& index
                               /*vector<int>& index,vector<double>& coefs*/,const double& sense)const
        {
        bool found_something = false;
        for (const FMTconstraint& constraint : constraints)
            {
            if (!constraint.isobjective() && constraint.isgoal())
                {
                string name;
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
                        vector<vector<int>>constraint_elements = getmatrixelement(constraint,period);//for each periods?!?!?!?!?!
                        if (!constraint_elements.at(FMTmatrixelement::goalvariable).empty())
                            {
                            found_something = true;
                            //index.push_back(constraint_elements.at(FMTmatrixelement::goalvariable).at(0));
                            //coefs.push_back(value);
							index[constraint_elements.at(FMTmatrixelement::goalvariable).at(0)] = value;
                            }
                        }
                    }
                }
            }
        return found_something;
        }

    int FMTlpmodel::addmatrixelement(const FMTconstraint& constraint,
                     const FMTmatrixelement& element_type,const map<int,double>& indexes,/*const vector<int>& indexes,const vector<double>& coefs,*/
                     int period,double lowerbound,double upperbound)
        {
        //matrix
        int element_id = 0;
		vector<int>sumvariables;
		vector<double>sumcoefficiants;
		summarize(indexes,/* coefs,*/ sumvariables, sumcoefficiants);
		//FMTgraphstats oldstats = graph.getstats();
		FMTgraphstats* stats = graph.getstatsptr();
		if (element_type == FMTmatrixelement::constraint)
			{
				element_id = stats->rows;
				solverinterface->addRow(int(sumvariables.size()), &sumvariables[0], &sumcoefficiants[0], lowerbound, upperbound);
				++stats->rows;
				++stats->output_rows;
			}else {
				element_id = stats->cols;
				solverinterface->addCol(0, &sumvariables[0], &sumcoefficiants[0], lowerbound, upperbound, 0);
				++stats->cols;
				++stats->output_cols;
			}
			//graph.setstats(oldstats);
			//listing
			//if period = -1 then add it to all periods!
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
				size_t to_resize = location - elements.size();
				/*Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "period: " << period << "\n";
				Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "location: " << location << "\n";
				Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "elements: " << elements.size() << "\n";
				Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "Resize size: " << to_resize << "\n";*/
				if (to_resize > 0)
				{
					for (size_t id = 0; id < to_resize; ++id)
					{
						elements.push_back(std::unordered_map<size_t, vector<vector<int>>>());
						//elements.push_back(vector<std::unordered_map<size_t,int>>(FMTmatrixelement::nr_items));
					}
				}

			}
			int starting = period;
			int stoping = period + 1;
			if (period < 0) //add to all reference!!
			{
				starting = 0;
				stoping = static_cast<int>(elements.size());
			}
			for (int locid = starting; locid < stoping; ++locid)
			{
				if (elements[locid].find(constraint.hash()) == elements[locid].end())
				{
					elements[locid][constraint.hash()] = vector<vector<int>>(FMTmatrixelement::nr_items);
				}
				elements[locid][constraint.hash()][element_type].push_back(element_id);
			}
        return element_id;
        }

    vector<vector<int>>FMTlpmodel::getmatrixelement(const FMTconstraint& constraint,int period) const
        {
        if ((period < elements.size()) && (elements.at(period).find(constraint.hash()) != elements.at(period).end()))
            {
            return elements.at(period).at(constraint.hash());
            }
        return vector<vector<int>>(FMTmatrixelement::nr_items);
        }

	FMTgraphstats FMTlpmodel::getstats() const
		{
		return graph.getstats();
		}



	bool FMTlpmodel::initialsolve()
		{
		//solverinterface->writeLp("C:/Users/cyrgu3/source/repos/FMT/x64/Release/test");
		//solverinterface->readMps("T:/Donnees/Courant/Projets/Carbone/models/Modele_Prov/20190823/Data/v01_20190826/PC_PROV.mps");
		//solverinterface->writeLp("C:/Users/cyrgu3/source/repos/FMT/x64/Release/test2");
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
				/*case FMTsolverinterface::MOSEK:
					solverinterface = unique_ptr<OsiMskSolverInterface>(new OsiMskSolverInterface);
				break;
				case FMTsolverinterface::CPLEX:
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
