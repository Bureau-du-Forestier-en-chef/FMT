/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include <memory>
#include <tuple>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <queue>

#ifdef FMTWITHOSI
#include "OsiSolverInterface.hpp"
#include "FMTlpmodel.h"
#ifdef FMTWITHMOSEK
	#include "OsiMskSolverInterface.hpp"
	#include "mosek.h"
#endif
#include "OsiClpSolverInterface.hpp"


namespace Models
{

	Heuristics::FMToperatingareaclusterer FMTlpmodel::getclusterer(
		const std::vector<Heuristics::FMToperatingareacluster>& initialcluster,
		const Core::FMToutput& areaoutput,
		const Core::FMToutput& statisticoutput,
		const int& period, int minimalnumberofclusters, int maximalnumberofclusters) const
	{
		Heuristics::FMToperatingareaclusterer newclusterer;
		try {
			std::vector<Heuristics::FMToperatingareacluster>newclusters;
			double minimalstatistic, minimalarea = COIN_DBL_MAX;
			double maximalstatistic, maximalarea = 0;
			const double minimalstatisticvalue = FMT_DBL_TOLERANCE*10000;
			std::map<Core::FMTmask,std::pair<double,double>>outputcaching;
			for (const Heuristics::FMToperatingareacluster& originalcluster : initialcluster)
				{
				Heuristics::FMToperatingareaclusterbinary centroid = originalcluster.getcentroid();
				std::vector<Heuristics::FMToperatingareaclusterbinary>allbinaries = originalcluster.getbinaries();
				const Core::FMTmask centroidmask = centroid.getmask();
				double cstatistic = 0;
				double carea = 0;
				if (outputcaching.find(centroidmask)!= outputcaching.end())
				{
					cstatistic = outputcaching.at(centroidmask).first;
					carea = outputcaching.at(centroidmask).second;
				}
				else {
					const Core::FMToutput centroidoutput = centroid.getoutputintersect(statisticoutput, themes);
					const Core::FMToutput centroidareaoutput = centroid.getoutputintersect(areaoutput, themes);
					const std::map<std::string, double> centroidvalue = this->getoutput(centroidoutput, period, Core::FMToutputlevel::totalonly);
					const std::map<std::string, double> centroidarea = this->getoutput(centroidareaoutput, period, Core::FMToutputlevel::totalonly);
					cstatistic = centroidvalue.at("Total");
					carea = centroidarea.at("Total");
					outputcaching[centroidmask] = std::pair<double, double>(cstatistic, carea);
					if (cstatistic < 0)
					{
						_exhandler->raise(Exception::FMTexc::FMTignore,
							"Negative stats value rounded to " + std::to_string(minimalstatisticvalue) +
							" for " + std::string(centroidmask),
							"FMTlpmodel::getclusterer", __LINE__, __FILE__);
					}
					
				}
				cstatistic = std::max(minimalstatisticvalue,cstatistic);
				minimalstatistic = std::min(minimalstatistic, cstatistic);
				minimalarea = std::min(minimalarea, carea);
				maximalstatistic = std::max(maximalstatistic, cstatistic);
				maximalarea = std::max(maximalarea, carea);
				centroid.setstatistic(cstatistic);
				centroid.setarea(carea);
				for (Heuristics::FMToperatingareaclusterbinary& binary : allbinaries)
					{
					const Core::FMTmask binarymask = binary.getmask();
					double statistic = 0;
					double area = 0;
					if (outputcaching.find(binarymask) != outputcaching.end())
					{
						statistic = outputcaching.at(binarymask).first;
						area = outputcaching.at(binarymask).second;
					}
					else {
						const Core::FMToutput binaryoutput = binary.getoutputintersect(statisticoutput, themes);
						const Core::FMToutput binaryareaoutput = binary.getoutputintersect(areaoutput, themes);
						const std::map<std::string, double> binaryvalue = this->getoutput(binaryoutput, period, Core::FMToutputlevel::totalonly);
						const std::map<std::string, double> binaryarea = this->getoutput(binaryareaoutput, period, Core::FMToutputlevel::totalonly);
						statistic = binaryvalue.at("Total");
						area = binaryarea.at("Total");
						outputcaching[binarymask] = std::pair<double, double>(statistic,area);
						//double statistic(binaryvalue.at("Total"));
						if (statistic < 0)
						{
							_exhandler->raise(Exception::FMTexc::FMTignore,
								"Negative stats value rounded to " + std::to_string(minimalstatisticvalue) +
								" for " + std::string(binarymask),
								"FMTlpmodel::getclusterer", __LINE__, __FILE__);
						}
					}
					statistic = std::max(minimalstatisticvalue, statistic);
					//const double area(binaryarea.at("Total"));
					minimalstatistic = std::min(minimalstatistic,statistic);
					minimalarea = std::min(minimalarea,area);
					maximalstatistic = std::max(maximalstatistic,statistic);
					maximalarea = std::max(maximalarea,area);
					binary.setstatistic(statistic);
					binary.setarea(area);
					}
				
                const Heuristics::FMToperatingareacluster newopcluster(Heuristics::FMToperatingareacluster(centroid,allbinaries),originalcluster.getrealminimalarea(),originalcluster.getrealmaximalarea());
                if (!newopcluster.isvalidareabounds())
                    {
                    _exhandler->raise(Exception::FMTexc::FMTignore,
									"Operating area cluster "+std::string(centroid.getmask())+" wont reach its minimal size",
									"FMTlpmodel::getclusterer",__LINE__, __FILE__);
                    }
				newclusters.push_back(newopcluster);
				
				}
			newclusterer = Heuristics::FMToperatingareaclusterer(solver.getsolvertype(),0,newclusters,minimalnumberofclusters,maximalnumberofclusters);
			_logger->logwithlevel("Units: ("+std::to_string(newclusterer.getbinariescount() )+")\nStats: min(" + std::to_string(minimalstatistic) + ")" +
				" max(" + std::to_string(maximalstatistic) + ") \nArea: min(" + std::to_string(minimalarea) + ") max(" + std::to_string(maximalarea) + ")\n", 0);
			newclusterer.passinobject(*this);
			newclusterer.buildproblem();
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
				Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator it,itend;

				for (boost::tie(it, itend)=graph.getperiodverticies(period);it!=itend;++it)
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
				return this->resolve();
				//return solver.stockresolve();
			}
		}catch (...)
		{
			_exhandler->printexceptions("at period " + std::to_string(period), "FMTlpmodel::boundsolution", __LINE__, __FILE__);
		}



    return false;
	}


	std::vector<Core::FMTconstraint> FMTlpmodel::getlocalconstraints(const std::vector<Core::FMTconstraint>& localconstraints, const int& period) const
	{
		std::vector<Core::FMTconstraint>newconstraints(localconstraints.begin(), localconstraints.end());
		try
		{
			if (!solver.isProvenOptimal())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot getlocalconstraints from a non optimal global",
					"FMTlpmodel::getlocalconstraints", __LINE__, __FILE__);
				}
			return FMTmodel::getlocalconstraints(localconstraints,period);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTlpmodel::getlocalconstraints", __LINE__, __FILE__);
		}
		return newconstraints;
	}


	void FMTlpmodel::addscheduletoobjective(const Core::FMTschedule& schedule, double weight)
	{
		try
		{
			const int period = schedule.getperiod();
			const double* actualobjective = solver.getObjCoefficients();
			const double sense = solver.getObjSense();
			std::vector<double>newobjective(solver.getNumCols(), 0.0);
			for (int colid = 0 ; colid < solver.getNumCols();++colid)
				{
				newobjective[colid] = *(actualobjective + colid);
				}
		
			Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			for (boost::tie(vertex_iterator, vertex_iterator_end) = graph.getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
				std::map<int, int>variables = graph.getoutvariables(*vertex_iterator);
				variables.erase(-1);
				for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
				{
					if (schedule.find(actions.at(varit->first))!=schedule.end()&&
						schedule.at(actions.at(varit->first)).find(graph.getdevelopment(*vertex_iterator))!= schedule.at(actions.at(varit->first)).end())
					{
						newobjective[varit->second] = (weight*(sense*-1.0))+newobjective.at(varit->second);
					}

				}
			}
			solver.setObjective(&newobjective.at(0));
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTlpmodel::addscheduletoobjective", __LINE__, __FILE__);
		}
	}


	FMTlpmodel::FMTlpmodel(const FMTmodel& base, FMTsolverinterface lsolvertype) :
		FMTsrmodel(base,lsolvertype),
		strictlypositivesoutputsmatrix(false),
		elements()
	{
	
	}

	FMTlpmodel::FMTlpmodel() :
		FMTsrmodel(),
		strictlypositivesoutputsmatrix(false),
		elements()
	{

	}

	FMTlpmodel::FMTlpmodel(const FMTlpmodel& rhs) :
		FMTsrmodel(rhs),
		strictlypositivesoutputsmatrix(rhs.strictlypositivesoutputsmatrix),
		elements(rhs.elements)
	{

	}


	bool FMTlpmodel::operator == (const FMTlpmodel& rhs) const
	{
		return (FMTsrmodel::operator == (rhs) &&
			strictlypositivesoutputsmatrix == rhs.strictlypositivesoutputsmatrix &&
			elements == rhs.elements);
	}

	bool FMTlpmodel::operator != (const FMTlpmodel& rhs) const
	{
		return !(*this == rhs);
	}

	void FMTlpmodel::setstrictlypositivesoutputsmatrix()
	{
		strictlypositivesoutputsmatrix=true;
	}



	FMTlpmodel& FMTlpmodel::operator = (const FMTlpmodel& rhs)
	{
		if (this != &rhs)
		{
			FMTsrmodel::operator = (rhs);
			strictlypositivesoutputsmatrix = (rhs.strictlypositivesoutputsmatrix);
			elements = rhs.elements;
		}
		return *this;
	}


std::vector<std::map<int, double>> FMTlpmodel::locatenodes(const std::vector<Core::FMToutputnode>& nodes, int period,
	std::map<int, double>& variables, double multiplier) const
	{
	std::vector<std::map<int, double>> strictlypositivesoutputs;
	try {
		std::unordered_set<int> output_negvar;
		for (const Core::FMToutputnode& node : nodes)
			{
			const std::map<int, double>node_map = graph.locatenode(*this, node, period);
			for (std::map<int, double>::const_iterator node_it = node_map.begin(); node_it != node_map.end(); node_it++)
				{
				if (variables.find(node_it->first) == variables.end())
					{
					variables[node_it->first] = 0;
					}
				if (strictlypositivesoutputsmatrix && node_it->second<0 && !node.source.getyield().empty())
					{
						output_negvar.insert(node.getoutputid());
					}

				variables[node_it->first] += node_it->second*multiplier;
				}
			}
			if (strictlypositivesoutputsmatrix && !output_negvar.empty())
			{
				for (const auto& onid:output_negvar)
				{
					int outputid=-9999;
					bool _area = false;
					std::map<std::string,std::map<int,double>> node_map_theme_id;
					for (const Core::FMToutputnode& node : outputs.at(onid).getnodes(1,true))
					{
						const int id = node.getoutputid();
						if (!node.source.getyield().empty())
						{
							if(_area)
							{
								_exhandler->raise(	Exception::FMTexc::FMTfunctionfailed,
													"Output id from node :"+std::string(node)+" contain different type of output", "FMTlpmodel::locatenodes", __LINE__, __FILE__);
							}
							const std::map<std::string,std::map<int,double>> node_map_theme = graph.locatenodebytheme(*this, node, period);
							if (!node_map_theme.empty())
							{
								if (id==-1)
								{
									_exhandler->raise(	Exception::FMTexc::FMTfunctionfailed,
														"Invalid output id from node :"+std::string(node), "FMTlpmodel::locatenodes", __LINE__, __FILE__);
								}
							}
							if (outputid!=id)
								{
									if(!node_map_theme_id.empty())
									{
										int counttheme = 0;
										bool foundna = false;
										for(const auto& nmti:node_map_theme_id)
										{
											if (!nmti.second.empty())
											{
												strictlypositivesoutputs.push_back(nmti.second);
												if(nmti.first=="~nothemetargetid~"){foundna=true;}
												++counttheme;
											}
										}
										if(counttheme>1 && foundna)
										{
											_exhandler->raise(	Exception::FMTexc::FMTfunctionfailed,
																"Differences in thematics of certains outputs and there sources", "FMTlpmodel::locatenodes", __LINE__, __FILE__);
										}
									}
									node_map_theme_id.clear();
								}
							for(const auto& nmt:node_map_theme)
								{
									if (node_map_theme_id.find(nmt.first)==node_map_theme_id.end())
									{
										node_map_theme_id[nmt.first]=nmt.second;
									}else{
										node_map_theme_id[nmt.first].insert(nmt.second.begin(),nmt.second.end());
									}
								}
						}else{
							_area=true;
						}
						outputid = id;
					}
					if(!node_map_theme_id.empty())
					{
						int counttheme = 0;
						bool foundna = false;
						for(const auto& nmti:node_map_theme_id)
						{
							if (!nmti.second.empty())
							{
								strictlypositivesoutputs.push_back(nmti.second);
								if(nmti.first=="~nothemetargetid~"){foundna=true;}
								++counttheme;
							}
						}
						if(counttheme>1 && foundna)
						{
							_exhandler->raise(	Exception::FMTexc::FMTfunctionfailed,
												"Differences in thematics of certains outputs and there sources", "FMTlpmodel::locatenodes", __LINE__, __FILE__);
						}
					}
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpmodel::locatenodes", __LINE__, __FILE__);
			}
	return strictlypositivesoutputs;
	}

	bool FMTlpmodel::setpositiveoutputsinmatrix(const Core::FMTconstraint& constraint, const std::vector<std::map<int, double>>& strictlypositivesoutputs,int period)
	{
		try
		{
			if (!strictlypositivesoutputs.empty())
			{
				for (const auto& outvariable:strictlypositivesoutputs)
				{
					getsetmatrixelement(constraint, FMTmatrixelement::strictlypositive, outvariable, period,0);
				}
				return true;
			}
		}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTlpmodel::setpositiveoutputsinmatrix", __LINE__, __FILE__);
		}
		return false;
	}

	void FMTlpmodel::clearcache()
	{
		FMTmodel::clearcache();
		cleargraphcache();
	}

	void FMTlpmodel::clearconstraintlocation()
	{
		std::vector<std::unordered_map<std::string,std::vector<std::vector<int>>>>().swap(elements);
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
					std::vector<Core::FMToutputnode>all_nodes = constraint.getnodes(/*area,actions,yields,*/averagefactor);
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
						if (containsmatrixelements(constraint,period)&&
							(!constraint.acrossperiod()||
							(constraint.acrossperiod()&&containsmatrixelements(constraint, period+1)&&
							!(lower_variation!=0 && constraint_type == Core::FMTconstrainttype::FMTevenflow))))
							{
							//If you get here your are probably making replanning if not you are not suppose to fall here!
							continue;
							}
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
								const std::vector<std::map<int, double>> outputvarpos = locatenodes(all_nodes, period, all_variables, 1);
								setpositiveoutputsinmatrix(constraint,outputvarpos,period);
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
								if (goal_variable != -1)
								{
									all_variables[goal_variable] = -1;
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
						//*_logger<<"Period even "<<period<<"\n";
						//*_logger<<"Enter size "<<all_variables.size()<<"\n";
						const std::vector<std::map<int, double>> outputvarpos = locatenodes(all_nodes, period, all_variables, 1);
						//*_logger<<"Out size "<<all_variables.size()<<"\n";
						setpositiveoutputsinmatrix(constraint,outputvarpos,period);
						if ((constraint_type == Core::FMTconstrainttype::FMTsequence || constraint_type == Core::FMTconstrainttype::FMTevenflow) ||
							(constraint_type == Core::FMTconstrainttype::FMTnondeclining && !all_variables.empty()))
						{
							const size_t sizebeforecrossing = all_variables.size();
							//*_logger<<"Enter size second "<<all_variables.size()<<"\n";
							const std::vector<std::map<int, double>> outputvarposloc = locatenodes(all_nodes, (period + 1), all_variables, -1);
							//*_logger<<"Out size second "<<all_variables.size()<<"\n";
							setpositiveoutputsinmatrix(constraint,outputvarposloc,period+1);
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
						if (goal_variable != -1&& lowerbound == std::numeric_limits<double>::lowest())
							{
							lowervars[goal_variable] = -1;
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
							if (goal_variable != -1 && lowerbound == std::numeric_limits<double>::lowest())
								{
								uppervars[goal_variable] = -1;
								}
							const int upper_constraint_id = getsetmatrixelement(constraint, FMTmatrixelement::constraint, uppervars,
								period, lowerbound, upperbound);
						}

					}
				}
			}
		}catch (...)
			{
			std::string constraintname = std::string(constraint);
			constraintname.pop_back();
				_exhandler->printexceptions("for " + constraintname, "FMTlpmodel::setconstraint", __LINE__, __FILE__);
			}
		//elements.clear();
		//solver.unmarkHotStart();
		//*_logger << "setting done of " << std::string(constraint) << "\n";
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
				tolerances = std::vector<double>(globalmasks.size(),FMT_DBL_TOLERANCE);
				}
			if (tolerances.size()!=globalmasks.size())
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Global masks and tolerances are not the same length", "FMTlpmodel::getareavariabilities", __LINE__, __FILE__);
				}
			//const boost::unordered_set<Core::FMTlookup<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor,Core::FMTdevelopment>>& initialperiod = graph.getperiodverticies(0);
			std::vector<int>colstarget;
			std::vector<double>originalbounds;
			std::vector<double>newbounds;
			const double* collowerbounds = solver.getColLower();
			const double* colupperbounds = solver.getColUpper();
			std::vector<bool>foundcorresponding(globalmasks.size(), false);
			//const int firstfutrecolumn = static_cast<int>(initialperiod.size());
			Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			for (boost::tie(vertex_iterator, vertex_iterator_end) = graph.getperiodverticies(0); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
					size_t maskid = 0;
					size_t gotvariables = 0;
					const Core::FMTdevelopment& dev = graph.getdevelopment(*vertex_iterator);
					for (const Core::FMTmask& globalmask : globalmasks)
					{
						if (dev.getmask().issubsetof(globalmask))
						{
							const int varindex = graph.getoutvariables(*vertex_iterator).at(-1);
							if (*(colupperbounds + varindex) == COIN_DBL_MAX)
							{
								_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
									"Changing a bound on a non initial variable for developmenets" + std::string(dev),
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
							"Got more than one global mask for " + std::string(dev.getmask()),
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
			solver.setColSetBounds(&(*colstarget.cbegin()), &(*colstarget.cend()), &newbounds[0]);
			
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
			solver.setColSetBounds(&(*colstarget.cbegin()), &(*colstarget.cend()), &originalbounds[0]);
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
			std::vector<FMTmatrixelement> simpleelements={FMTmatrixelement::constraint,FMTmatrixelement::strictlypositive};
			for (const auto& elementtype : simpleelements)
			{
				if (!all_elements.at(elementtype).empty())
				{
					for (const int& levelid : all_elements.at(elementtype))
					{
						bool removeconstraint = true;
						for (int locator = (period + 1); locator < static_cast<int>(elements.size()); ++locator)
						{
							for (std::unordered_map<std::string, std::vector<std::vector<int>>>::iterator elit = elements.at(locator).begin(); elit != elements.at(locator).end(); elit++)
							{
								if (std::find(elit->second.at(elementtype).begin(), elit->second.at(elementtype).end(), levelid) != elit->second.at(elementtype).end())
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
		}
			catch (...)
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

	bool FMTlpmodel::containsmatrixelements(const Core::FMTconstraint& constraint, int period) const
		{
		return(period < static_cast<int>(elements.size()) &&
			(elements.at(period).find(std::string(constraint)) != elements.at(period).end()));
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
		if (element_type == FMTmatrixelement::constraint || element_type == FMTmatrixelement::strictlypositive)
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
		else{return false;}
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
			const std::vector<Core::FMToutputnode>all_nodes = objective.getnodes(/*area,actions,yields,*/averagefactor,strictlypositivesoutputsmatrix);
			std::map<int, double>all_variables;
			if (!objective.extravariables())
			{
				for (int period = first_period; period <= last_period; ++period)
				{
					const std::vector<std::map<int, double>> outputvarpos = locatenodes(all_nodes, period, all_variables);
					setpositiveoutputsinmatrix(objective,outputvarpos,period);
					//locatenodes(all_nodes, period, all_variables);
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
				//*_logger << "variable id of " << variable_id << "\n";
				bool gotvariables = false;
				for (int period = first_period; period <= last_period; ++period)
				{
					std::map<int, double>period_variables;
					const std::vector<std::map<int, double>> outputvarpos = locatenodes(all_nodes, period, period_variables);
					setpositiveoutputsinmatrix(objective,outputvarpos,period);
					//locatenodes(all_nodes, period, period_variables);
					locatelevels(all_nodes, period, all_variables, objective);
					if (!period_variables.empty())
					{
						period_variables[variable_id] = -1;
						const int constraint_id = getsetmatrixelement(objective, FMTmatrixelement::constraint, period_variables,
							period, lowerbound, upperbound);
						gotvariables = true;
					}else{
						_exhandler->raise(Exception::FMTexc::FMTunsupported_objective,
											"The value of the output in the objective for period "+std::to_string(period)+" is equal to zero, so the solution of your model is zero",
											"FMTlpmodel::setobjective", __LINE__, __FILE__);
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
					_exhandler->raise(Exception::FMTexc::FMTnonaddedconstraint,
						"No goals detected", "FMTlpmodel::setobjective", __LINE__, __FILE__);
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
			//*_logger<<"Enter " <<std::string(constraint)<<" "<<element_type<< " "<<period<<"\n";
			if (ismatrixelement(constraint, element_type, period))
			{
				//*_logger<<"Issamematrix " <<std::string(constraint)<<" "<<element_type<< " "<<period<<"\n";
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
			else if (element_type == FMTmatrixelement::constraint  && indexes.empty())
			{
				std::string cname = std::string(constraint);
				cname.erase(std::remove(cname.begin(), cname.end(), '\n'), cname.end());
				if (period>(graph.getfirstactiveperiod()) &&period<static_cast<int>(graph.size()) &&
					constraint.getconstrainttype() == Core::FMTconstrainttype::FMTstandard &&
					(upperbound< 0 || lowerbound > 0))
					{
					_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
						std::string(cname) + " at period " + std::to_string(period),
						"FMTlpmodel::getsetmatrixelement", __LINE__, __FILE__);
				}else {
					_exhandler->raise(Exception::FMTexc::FMTnonaddedconstraint,
						std::string(cname) + " at period " + std::to_string(period),
						"FMTlpmodel::getsetmatrixelement", __LINE__, __FILE__);
					}
				return -1;
			}
			std::vector<int>sumvariables;
			std::vector<double>sumcoefficiants;
			summarize(indexes, sumvariables, sumcoefficiants);
			Graph::FMTgraphstats* stats = graph.getstatsptr();
			if (element_type == FMTmatrixelement::constraint || element_type == FMTmatrixelement::strictlypositive)
			{
				//*_logger<<"Add " <<std::string(constraint)<<" "<<element_type<< " "<<period<<"\n";
				if (sumvariables.empty() && element_type == FMTmatrixelement::strictlypositive)
				{
					return -1;
				}else{
					element_id = stats->rows;
					solver.addRow(static_cast<int>(sumvariables.size()), &sumvariables[0], &sumcoefficiants[0], lowerbound, upperbound);
					++stats->rows;
					++stats->output_rows;
				}
			}
			else {
				element_id = stats->cols;
				solver.addCol(0, &sumvariables[0], &sumcoefficiants[0], lowerbound, upperbound, 0);
				++stats->cols;
				++stats->output_cols;
			}
			if ((elements.size() != (graph.size() - 1)) && (period >= static_cast<int>(elements.size()))||period<0) //just for resizing!
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
			//*_logger << "starting " << starting<< " "<<stoping << "\n";
			for (int locid = starting; locid < stoping; ++locid)
			{
				if (elements[locid].find(std::string(constraint)) == elements[locid].end())
				{
					elements[locid][std::string(constraint)] = std::vector<std::vector<int>>(FMTmatrixelement::nr_items);
				}
				/*if (element_type==2)
				{
					*_logger << "pushing at " << locid << "\n";
				}*/
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

	std::vector<Heuristics::FMToperatingareascheduler>FMTlpmodel::getoperatingareaschedulerheuristics(const std::vector<Heuristics::FMToperatingareascheme>& opareas,
																						const Core::FMToutputnode& node,
																						size_t numberofheuristics,
																						bool copysolver,
																						bool updatematrixname)
		{
		bool userandomness = false;
		size_t seedof = 1;
		double proportionofset = 0.25;
		std::vector<Heuristics::FMToperatingareascheduler>allheuristics;
		try {
			if (updatematrixname)//For debugging matrix
				{
				updatematrixnaming();
				}
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
																				size_t numberofheuristics,
																				int minimalnumberofclusters,
																				int maximalnumberofclusters) const
        {
        size_t seedof = 0;
        std::vector<Heuristics::FMToperatingareaclusterer>allheuristics;
		try {
			Heuristics::FMToperatingareaclusterer baseclusterer=this->getclusterer(clusters,areaoutput,statisticoutput,period,
				minimalnumberofclusters, maximalnumberofclusters);
            for (size_t heuristicid = 0 ; heuristicid < numberofheuristics; ++heuristicid)
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

	double FMTlpmodel::getObjValue() const
		{
		if (solver.isProvenOptimal())
			{
			return solver.getObjValue();
			}else{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,"Can not get objective value because model is NOT PROVEN OPTIMAL","FMTlpmodel::getObjValue",__LINE__, __FILE__);
				return 0;
			}
		}


	void FMTlpmodel::writeLP(const std::string& location)
	{
		try {
			updatematrixnaming();
			const std::string name = location + getname();
			solver.writeLP(name);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::writeLP", __LINE__, __FILE__);
		}
	}


	void FMTlpmodel::writeMPS(const std::string& location)
	{
		try {
			updatematrixnaming();
			solver.writeMPS(location);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::writeMPS", __LINE__, __FILE__);
		}
	}

	bool FMTlpmodel::doplanning(const bool& solve,std::vector<Core::FMTschedule> schedules)
		{
			//J'utiliserais pas le presolve car ça serait à l'uytilisateur de dire si il veut que ça aille plus 
			//vite... Si on fait ca, on a pas de postsoolve sur le la matrice mais oui pour le graph, donc est-ce que le getoutput va fonctionner ?
		bool optimal = false;
		try {
			const int length = parameters.getintparameter(LENGTH);
			const int presolve_iterations = parameters.getintparameter(PRESOLVE_ITERATIONS);
			const bool postsolve = parameters.getboolparameter(POSTSOLVE);
			const bool forcepartialbuild = parameters.getboolparameter(FORCE_PARTIAL_BUILD);
			/**_logger<<"Presolving model with "+std::to_string(presolve_iterations)+" iterations"<<"\n";
			//pointeur presolve swap début fin
			std::unique_ptr<FMTmodel> presolve_model = this->presolve(presolve_iterations,area);*/
			if (parameters.getboolparameter(STRICTLY_POSITIVE))
			{
				setstrictlypositivesoutputsmatrix();
			}
			solver.setnumberofthreads(parameters.getintparameter(NUMBER_OF_THREADS));
			std::string addon = "";
			if(!schedules.empty())
			{
				addon = "FMT will use schedules pass by argument for periods 1 to "+std::to_string(schedules.size());
			}
			*_logger<<"Building model for "+std::to_string(length)+" periods. "+addon<<"\n";
			//Period start at 0 but it's the period 1 that is created first. Reason is that schedules is a vector and the first elements 
			//is the schedule for period 1 
			for (int period = 0; period<length;++period)
				{
					if((!schedules.empty()) && (period<schedules.size()))
					{
						*_logger << std::string(this->buildperiod(schedules.at(period),forcepartialbuild,parameters.getperiodcompresstime(period)))<<"\n";
					}else{
						*_logger << std::string(this->buildperiod(Core::FMTschedule(),false,parameters.getperiodcompresstime(period)))<<"\n";
					}
				}
			if(!schedules.empty())
			{
				addon = "FMT will use schedules pass by argument for periods 1 to "+std::to_string(schedules.size())+" to set the solution in the matrix.";
			}
			*_logger<<"Setting constraints on the model. "+addon<<"\n";
			for (size_t constraintid = 1; constraintid < constraints.size();++constraintid)
				{
				this->setconstraint(constraints.at(constraintid));
				}
			*_logger<<"*Graph stats with all constraints : \n"<<std::string(this->setobjective(constraints.at(0)))<<"\n";
			if( !schedules.empty() && length<=schedules.size() )
			{
				bool bylp = false;
				int period = 1;
				for (const Core::FMTschedule& schedule : schedules)
				{
					this->setsolution(period,schedule,parameters.getdblparameter(TOLERANCE));
					++period;
					//Checker pour implémenter avec Guillaume
					/*if (!bylp)
					{
						try{

						}catch(...)
						{
							if()
							{
								bylp=true;
							}
						}

					}*/
				}
			}
			if (solve) 
			{
				optimal = this->initialsolve();
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpmodel::doplanning", __LINE__, __FILE__);
			}
			return optimal;
		}

	void FMTlpmodel::updategeneralconstraintsnaming(std::vector<std::string>& colnames,
													std::vector<std::string>& rownames) const
	{
		try {
			int constraintid = 0;
			for (const Core::FMTconstraint& constraint : constraints)
				{
				int first_period = 0;
				int last_period = 0;
				if (graph.constraintlenght(constraint, first_period,last_period))
					{
					if(constraint.acrossperiod())
					{
						++last_period;
					}
					const std::string constraintname("ID"+std::to_string(constraintid));
					for (int period = first_period;period <= last_period;++period)
						{
						const std::string constnperiod(constraintname + "_P" + std::to_string(period));
						const std::vector<std::vector<int>>constraintindex = getmatrixelement(constraint, period);
						int elementid = 0;
						for (const std::vector<int>& elements : constraintindex)
							{
							if (!elements.empty())
								{
								const FMTmatrixelement elementtype = static_cast<FMTmatrixelement>(elementid);
								std::string* original; 
								size_t elid=0;
								for (const auto& el:elements)
								{
									if (elementtype== FMTmatrixelement::constraint||elementtype==FMTmatrixelement::strictlypositive)
									{
									original = &rownames[el];
								}else {
									original = &colnames[el];
									}
								if (!original->empty())
								{
									if (original->at(original->size() - 2) == '_')
									{
										*original = original->substr(0, original->size() - 1) + std::to_string(period);
									}else {
										*original = *original + "_" + std::to_string(period);
										}
								}else{
									std::string tag;
									switch (elementtype)
									{
									case FMTmatrixelement::constraint:
									{
										tag = "CON_";
										break;
									}
									case FMTmatrixelement::goalvariable:
									{
										tag = "GOAL_";
										break;
									}
									case FMTmatrixelement::levelvariable:
									{
										tag = "LEVEL_";
										break;
									}
									case FMTmatrixelement::objectivevariable:
									{
										tag = "OBJ_";
										break;
									}
									case FMTmatrixelement::strictlypositive:
									{
										tag = "POS_";
										break;
									}
									default:
										break;
									}
									*original = tag + std::to_string(elid) + constnperiod;
								}
								++elid;
								}
								}
							++elementid;
							}
						}
					}
				++constraintid;
				}


		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::updategeneralconstraintsnaming", __LINE__, __FILE__);
		}

	}

	std::unique_ptr<FMTmodel>FMTlpmodel::clone() const
		{
		return std::unique_ptr<FMTmodel>(new FMTlpmodel(*this));
		}


	void FMTlpmodel::updatematrixnaming()
	{
		try {
			std::vector<std::string>colnames(solver.getNumCols());
			std::vector<std::string>rownames(solver.getNumRows());
			graph.getvariablenames(actions,colnames);
			graph.gettransferrownames(rownames);
			updategeneralconstraintsnaming(colnames,rownames);
			int colid = 0;
			for (const std::string& name : colnames)
				{
				solver.setcolname(name, colid);
				++colid;
				}
			int rowid = 0;
			for (const std::string& name : rownames)
				{
				solver.setrowname(name, rowid);
				++rowid;
				}
			solver.updaterowsandcolsnames();

		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::updatematrixnaming", __LINE__, __FILE__);
		}
	}

}



BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTlpmodel)
#endif
