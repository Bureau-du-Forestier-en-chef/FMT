/*
Copyright (c) 2019 Gouvernement du Qu�bec

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
#include "FMToperatingareaclusterer.h"

#ifdef FMTWITHOSI
#include "FMToperatingareascheduler.h"
#include "FMTlpmodel.h"
#include "FMTexceptionhandler.h"
#include "boost/filesystem.hpp"



namespace Models
{


	FMTlpmodel::ConstraintIndex::ConstraintIndex(int p_constraintId, int p_period, FMTmatrixelement p_type):
		m_constraintId(p_constraintId),
		m_period(p_period),
		m_type(p_type)/*,
		m_rows()*/
	{

	}

	/*FMTlpmodel::ConstraintIndex::ConstraintIndex(int p_constraintId, int p_period,
		FMTmatrixelement p_type, std::allocator<int>& p_allocator, size_t p_allocation):
			m_constraintId(p_constraintId),
			m_period(p_period),
			m_type(p_type),
			m_rows(p_allocator)
			{
			m_rows.reserve(p_allocation);
			}*/
	bool FMTlpmodel::ConstraintIndex::operator < (const ConstraintIndex& p_rhs) const
		{
		return std::tie(m_constraintId, m_period, m_type) <
			std::tie(p_rhs.m_constraintId, p_rhs.m_period, p_rhs.m_type);
		}
	bool FMTlpmodel::ConstraintIndex::operator == (const ConstraintIndex& p_rhs) const
		{
		return (m_constraintId == p_rhs.m_constraintId &&
			m_period == p_rhs.m_period &&
			m_type == p_rhs.m_type);
		}
	/*const std::vector<int>& FMTlpmodel::ConstraintIndex::getRows() const
		{
		return m_rows;
		}
	std::vector<int>& FMTlpmodel::ConstraintIndex::getRowsRef()
		{
		return m_rows;
		}*/

	/*void FMTlpmodel::ConstraintIndex::push_back(const int& p_row)
		{
		m_rows.push_back(p_row);
		}*/
	
	size_t FMTlpmodel::ConstraintIndex::getHash() const
		{
		return std::hash<int>()(m_constraintId) ^
			std::hash<int>()(m_period) ^
			std::hash<int>()(m_type);
		}

	std::vector<Core::FMTconstraint>::const_iterator FMTlpmodel::_getConstraintIndex(const Core::FMTconstraint& p_constraint) const
		{
		std::vector<Core::FMTconstraint>::const_iterator it = constraints.begin();
		for (const Core::FMTconstraint& CONSTRAINT : constraints)
			{
			if (p_constraint==CONSTRAINT)
				{
				return it;
				}
			++it;
			}
		return constraints.end();
		}

	std::vector<Core::FMTconstraint>::const_iterator FMTlpmodel::_getsetConstraintIndex(const Core::FMTconstraint& p_constraint)
		{
		std::vector<Core::FMTconstraint>::const_iterator it = _getConstraintIndex(p_constraint);
		if (it == constraints.end())
			{
			constraints.push_back(p_constraint);
			it = _getConstraintIndex(p_constraint);
			}
		return it;
		}

	int FMTlpmodel::_getIndex(const std::vector<Core::FMTconstraint>::const_iterator& p_it) const
		{
		return static_cast<int>(std::distance(constraints.begin(), p_it));
		}

	void FMTlpmodel::_setGraphCache(bool p_noLength)
		{
		const bool QUIET_LOG = parameters.getboolparameter(QUIET_LOGGING);
		size_t length = 5;
		if (!p_noLength)
			{
			length = static_cast<size_t>(getparameter(FMTintmodelparameters::LENGTH));
			}
		const size_t AREA = area.size();
		const size_t ACTIONS = actions.size();
		const size_t EXPO_FACTOR = 5;
		const size_t TO_RESERVE = length * AREA * ACTIONS * EXPO_FACTOR;
		m_graph->reserveVerticies(TO_RESERVE);
		if (!QUIET_LOG)
		{ 
			_logger->logwithlevel("Graph reserve of " + getname() + " (" + std::to_string(TO_RESERVE) + ") vertices\n", 1);
		}
		}

	void FMTlpmodel::_setConstraintsCache()
	{
		const size_t FACTOR = 100; //higher is less memory
		const size_t VARIABLES = static_cast<size_t>(m_graph->getstats().cols);
		const size_t THE_LENGTH = m_graph->size();
		const size_t TO_RESERVE = (VARIABLES / THE_LENGTH) / FACTOR;
		const bool QUIET_LOG = parameters.getboolparameter(QUIET_LOGGING);
		int id = 0;
		for (const Core::FMTconstraint CONSTRAINT : constraints)
		{
			for (int period = 0; period <= getparameter(FMTintmodelparameters::LENGTH); ++period)
			{
				for (int typeId = FMTmatrixelement::goalvariable; typeId != FMTmatrixelement::nr_items; typeId++)
				{
					FMTmatrixelement type = static_cast<FMTmatrixelement>(typeId);
					size_t targetSize(TO_RESERVE);
					if (type != FMTmatrixelement::constraint)
					{
						targetSize = 100;
					}
					//m_indexes.insert(ConstraintIndex(id, period, type, m_rowsAllocator, TO_RESERVE));
					auto inserted = m_indexes.insert(std::pair<ConstraintIndex,std::vector<int>>(ConstraintIndex(id, period, type),std::vector<int>(m_rowsAllocator)));
					inserted.first->second.reserve(targetSize);
				}
			}
			++id;
		}
		if (!QUIET_LOG)
		{
			_logger->logwithlevel("Constraints reserve of " + getname() + " (" + std::to_string(TO_RESERVE) + ") elements\n", 1);
		}
	}

	Heuristics::FMToperatingareaclusterer FMTlpmodel::getclusterer(
		const std::vector<Heuristics::FMToperatingareacluster>& initialcluster,
		const Core::FMToutput& areaoutput,
		const Core::FMToutput& statisticoutput,
		const int& period, int minimalnumberofclusters, int maximalnumberofclusters) const
	{
		Heuristics::FMToperatingareaclusterer newclusterer;
		try {
			std::vector<Heuristics::FMToperatingareacluster>newclusters;
			double minimalstatistic, minimalarea = std::numeric_limits<double>::max();
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
			newclusterer.buildproblem();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpmodel::getclusterer", __LINE__, __FILE__);
			}
		return newclusterer;
	}


	std::vector<Core::FMTconstraint> FMTlpmodel::getreplanningconstraints(const std::string& modeltype, const std::vector<Core::FMTconstraint>& localconstraints, const int& period) const
	{
		try
		{
			if (!solver.isProvenOptimal())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot getlocalconstraints from a non optimal global",
					"FMTlpmodel::getlocalconstraints", __LINE__, __FILE__);
				}
			return FMTmodel::getreplanningconstraints(modeltype,localconstraints,period);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTlpmodel::getreplanningconstraints", __LINE__, __FILE__);
		}
		return std::vector<Core::FMTconstraint>();
	}

	std::map<std::string, double> FMTlpmodel::getoutput(const Core::FMToutput& output,
		int period, Core::FMToutputlevel level) const
	{
		try {
			if (output.isonlylevel()&&
				output.getsourcesreference().at(0).isvariablelevel())
			{
				if (level!=Core::FMToutputlevel::totalonly)
				{
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Cannot request any other outputlevel than totalonly for level "+output.getname(),
						" FMTlpmodel::getoutputs", __LINE__, __FILE__);
				}
				const double* solution = solver.getColSolution();
				for (const Core::FMTconstraint& constraint : constraints)
				{
					const std::vector<std::string>level_names = constraint.getvariablelevels();
					std::vector<std::string>::const_iterator lit = std::find(level_names.begin(), level_names.end(), output.getname());
					if (lit != level_names.end())
						{
						const int levelindex = getlevelfromlevelname(*lit, period, constraints.end());
						std::map<std::string, double>levelmap;
						levelmap["Total"] = *(solution + levelindex);
						return levelmap;
						}
				}
			}else {
				return FMTsrmodel::getoutput(output, period, level);
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTlpmodel::getoutput", __LINE__, __FILE__);
		}
		return std::map<std::string, double>();

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
			for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getperiodverticies(period); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
				std::map<int, int>variables = m_graph->getoutvariables(*vertex_iterator);
				variables.erase(-1);
				for (std::map<int, int>::const_iterator varit = variables.begin(); varit != variables.end(); varit++)
				{
					if (schedule.find(actions.at(varit->first))!=schedule.end()&&
						schedule.at(actions.at(varit->first)).find(m_graph->getdevelopment(*vertex_iterator))!= schedule.at(actions.at(varit->first)).end())
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
		m_rowsAllocator(),
		m_indexAllocator(),
		m_indexes(m_indexAllocator)
		//elements()
	{
		solver.setnumberofthreads(getparameter(NUMBER_OF_THREADS));	
	}
	/*
	FMTlpmodel::FMTlpmodel(	const FMTmodel& base,const Graph::FMTgraph<Graph::FMTvertexproperties,Graph::FMTedgeproperties>& lgraph,
							const FMTlpsolver& lsolver,const std::vector<std::unordered_map<std::string,std::vector<std::vector<int>>>>& lelements) :
	FMTsrmodel(base,lgraph,lsolver),
	m_indexes(m_indexAllocator),
	elements(lelements)
	{
		
		solver.setnumberofthreads(getparameter(NUMBER_OF_THREADS));	
	}
	*/
	FMTlpmodel::FMTlpmodel() :
		FMTsrmodel(),
		m_rowsAllocator(),
		m_indexAllocator(),
		m_indexes(m_indexAllocator)
		//elements()
	{
		
	}

	FMTlpmodel::FMTlpmodel(const FMTlpmodel& rhs) :
		FMTsrmodel(rhs),
		m_rowsAllocator(rhs.m_rowsAllocator),
		m_indexAllocator(rhs.m_indexAllocator),
		m_indexes(rhs.m_indexes)
		//elements(rhs.elements)
	{

	}

	bool FMTlpmodel::operator == (const FMTlpmodel& rhs) const
	{
		return (FMTsrmodel::operator == (rhs) &&
			m_indexes == rhs.m_indexes /* &&
			elements == rhs.elements*/);
	}

	bool FMTlpmodel::operator != (const FMTlpmodel& rhs) const
	{
		return !(*this == rhs);
	}

	void FMTlpmodel::setstrictlypositivesoutputsmatrix()
	{
		FMTmodel::setparameter(STRICTLY_POSITIVE,true);
	}

	FMTlpmodel& FMTlpmodel::operator = (const FMTlpmodel& rhs)
	{
		if (this != &rhs)
		{
			FMTsrmodel::operator = (rhs);
			m_rowsAllocator = rhs.m_rowsAllocator;
			m_indexAllocator = rhs.m_indexAllocator;
			m_indexes = rhs.m_indexes;
			//elements = rhs.elements;
		}
		return *this;
	}

std::vector<std::map<int, double>> FMTlpmodel::locatenodes(const std::vector<Core::FMToutputnode>& nodes, int period,
	std::map<int, double>& variables, double multiplier) const
	{
	std::vector<std::map<int, double>> strictlypositivesoutputs;
	//std::unordered_set<int>test;
	const bool strictlypositivesoutputsmatrix = getparameter(STRICTLY_POSITIVE);
	try {
		std::unordered_set<int> output_negvar;
		for (const Core::FMToutputnode& node : nodes)
			{
			const std::map<int, double>node_map = m_graph->locatenode(*this, node, period);
			for (std::map<int, double>::const_iterator node_it = node_map.begin(); node_it != node_map.end(); node_it++)
				{
				std::pair<std::map<int, double>::iterator,bool> inserted = variables.insert(std::pair<int, double>(node_it->first,0.0));
				/*if (variables.find(node_it->first) == variables.end())
					{
					variables[node_it->first] = 0;
					}*/
				//on doit seulement utiliser le outputid le plus élevé obtenue dans TOUS les noeuds lue!!!!
				//output_negvar ne doit pas être une liste mais bien un seul élément unique.
				//Sur cet élément unique on doit recuellir tous les verticies par attribut du output
				//Et setter chaque attribut comme étant >= 0
				if (strictlypositivesoutputsmatrix && node_it->second<0 && !node.source.getyield().empty())
					{
						output_negvar.insert(node.getoutputid());
					}
				inserted.first->second += node_it->second * multiplier;
				//variables[node_it->first] += node_it->second*multiplier;
				}
			//test.insert(node.getoutputid());
			}
			if (strictlypositivesoutputsmatrix && !output_negvar.empty())
			{
				for (const auto& onid:output_negvar)
				{
					int outputid=-9999;
					bool _area = false;
					std::map<std::string,std::map<int,double>> node_map_theme_id;
					std::vector<std::string>equation;
					for (const Core::FMToutputnode& node : outputs.at(onid).getnodes(equation,1,true))
					{
						
						const int id = node.getoutputid();
						if (!node.source.getyield().empty())
						{
							if(_area)
							{
								_exhandler->raise(	Exception::FMTexc::FMTfunctionfailed,
													"Output id from node :"+std::string(node)+" contain different type of output", "FMTlpmodel::locatenodes", __LINE__, __FILE__);
							}
							const std::map<std::string,std::map<int,double>> node_map_theme = m_graph->locatenodebytheme(*this, node, period);
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
			_exhandler->raisefromcatch("at period "+std::to_string(period), "FMTlpmodel::locatenodes", __LINE__, __FILE__);
			}
	return strictlypositivesoutputs;
	}

	bool FMTlpmodel::setpositiveoutputsinmatrix(const std::vector<Core::FMTconstraint>::const_iterator& p_it,
		const std::vector<std::map<int, double>>& strictlypositivesoutputs,int period)
	{
		try
		{
			if (!strictlypositivesoutputs.empty())
			{
				for (const auto& outvariable:strictlypositivesoutputs)
				{
					getsetMatrixElement(p_it, FMTmatrixelement::strictlypositive, outvariable, period,0);
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
		//std::vector<std::unordered_map<std::string,std::vector<std::vector<int>>>>().swap(elements);
		m_indexes.clear();
	}

	Graph::FMTgraphstats FMTlpmodel::setconstraint(const Core::FMTconstraint& constraint)
		{
		try {
			/*if (getname() == "tactique_AllEnrqc_CC")
			{
				std::terminate();
			}*/
			if (!constraint.isobjective()&&!constraint.isspatial())
			{
				const std::vector<Core::FMTconstraint>::const_iterator CONSTRAINT_IT = _getsetConstraintIndex(constraint);
				if (!constraint.islinear())
				{
					_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
						"The constraint output " + std::string(constraint) + " cannot be deduct to output nodes",
						"FMTlpmodel::setconstraint", __LINE__, __FILE__);
				}
				int first_period = 0;
				int last_period = 0;
				if (m_graph->constraintlenght(constraint, first_period, last_period))
				{
					Core::FMTconstrainttype constraint_type = constraint.getconstrainttype();
					std::vector<std::string>equation;
					///////////////////////////////////////////////////
					const std::vector<Core::FMToutputnode>all_nodes = constraint.getnodes(equation, 1);
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
						if (containsMatrixElements(CONSTRAINT_IT,period)&&
							(!constraint.acrossperiod()||
							(constraint.acrossperiod()&& containsMatrixElements(CONSTRAINT_IT, period+1)&&
							!(lower_variation!=0 && constraint_type == Core::FMTconstrainttype::FMTevenflow))))
							{
							//If you get here your are probably making replanning if not you are not suppose to fall here!
							continue;
							}
						std::map<int, double>all_variables;
						int goal_variable = -1;
						if (constraint.isgoal())
						{

							goal_variable = getsetMatrixElement(CONSTRAINT_IT, FMTmatrixelement::goalvariable, all_variables, period);
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
									upper_even_variable = getsetMatrixElement(CONSTRAINT_IT, FMTmatrixelement::levelvariable, localvariables);
									lower_even_variable = getsetMatrixElement(CONSTRAINT_IT, FMTmatrixelement::objectivevariable, localvariables);
									localvariables[upper_even_variable] = coef_multiplier_lower;
									localvariables[lower_even_variable] = -1;
									lowerbound = std::numeric_limits<double>::lowest();
									upperbound = 0;
									const int lower_constraint_id = getsetMatrixElement(CONSTRAINT_IT, FMTmatrixelement::constraint, localvariables, -1, lowerbound, upperbound);
								}
								const std::vector<std::map<int, double>> outputvarpos = locatenodes(all_nodes, period, all_variables, 1);
								setpositiveoutputsinmatrix(CONSTRAINT_IT,outputvarpos,period);
								all_variables[lower_even_variable] = -1;
								lowerbound = 0;
								upperbound = std::numeric_limits<double>::max();
								if (all_variables.size() == 1)
								{
									all_variables.clear();
								}
								const int lowervalue = getsetMatrixElement(CONSTRAINT_IT, FMTmatrixelement::constraint, all_variables,
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
								const int uppervalue = getsetMatrixElement(CONSTRAINT_IT, FMTmatrixelement::constraint, all_variables,
									period, lowerbound, upperbound);
								if (period == last_period)
								{
									return m_graph->getstats();
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
						const std::vector<std::map<int, double>> outputvarpos = locatenodes(all_nodes, period, all_variables, 1);
						setpositiveoutputsinmatrix(CONSTRAINT_IT,outputvarpos,period);
						locatelevels(all_nodes, period, all_variables, CONSTRAINT_IT);

						if ((constraint_type == Core::FMTconstrainttype::FMTsequence || constraint_type == Core::FMTconstrainttype::FMTevenflow) ||
							(constraint_type == Core::FMTconstrainttype::FMTnondeclining && !all_variables.empty()))
						{
							const size_t sizebeforecrossing = all_variables.size();
							//*_logger<<"Enter size second "<<all_variables.size()<<"\n";
							const std::vector<std::map<int, double>> outputvarposloc = locatenodes(all_nodes, (period + 1), all_variables, -1);
							//*_logger<<"Out size second "<<all_variables.size()<<"\n";
							locatelevels(all_nodes, period+1, all_variables, CONSTRAINT_IT,-1.0);
							setpositiveoutputsinmatrix(CONSTRAINT_IT,outputvarposloc,period+1);
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
						if (!constraint.canbenodesonly())
							{
							//constraint.getRHSvalue(period, lowerbound, upperbound);
							constraint.getRHSvalue(period + 1, lowerbound, upperbound);
							}
						}

						//level part
						//locatelevels(all_nodes, period, all_variables, constraint);
						std::map<int, double>lowervars(all_variables);
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
						if (goal_variable != -1 && (lowerbound == std::numeric_limits<double>::lowest() || lowerbound == upperbound))
							{
							lowervars[goal_variable] = -1;
							}
						const int lower_constraint_id = getsetMatrixElement(CONSTRAINT_IT, FMTmatrixelement::constraint, lowervars,
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
							if (goal_variable != -1 && (lowerbound == std::numeric_limits<double>::lowest() || lowerbound == upperbound))
								{
								uppervars[goal_variable] = -1;
								}
							const int upper_constraint_id = getsetMatrixElement(CONSTRAINT_IT, FMTmatrixelement::constraint, uppervars,
								period, lowerbound, upperbound);
						}

					}
				}
			}
		}catch (...)
			{
			std::string constraintName = std::string(constraint);
			if (!constraintName.empty())
				{
				constraintName.pop_back();
				}
			_exhandler->printexceptions("for " + constraintName, "FMTlpmodel::setconstraint", __LINE__, __FILE__);
			}
		return m_graph->getstats();
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

	FMTlpmodel FMTlpmodel::getmodelfromproportions(const std::vector<Core::FMTmask>& globalmasks,
		std::vector<double> tolerances) const
	{
		FMTlpmodel modelwithproportion(*this);
		try {
			if (tolerances.size() != globalmasks.size())
			{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Global masks and tolerances are not the same length", "FMTlpmodel::getareavariabilities", __LINE__, __FILE__);
			}
			//const boost::unordered_set<Core::FMTlookup<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor,Core::FMTdevelopment>>& initialperiod = m_graph->getperiodverticies(0);
			std::vector<int>colstarget;
			std::vector<double>originalbounds;
			std::vector<double>newbounds;
			const double* collowerbounds = solver.getColLower();
			const double* colupperbounds = solver.getColUpper();
			std::vector<bool>foundcorresponding(globalmasks.size(), false);
			//const int firstfutrecolumn = static_cast<int>(initialperiod.size());
			Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_iterator vertex_iterator, vertex_iterator_end;
			for (boost::tie(vertex_iterator, vertex_iterator_end) = m_graph->getperiodverticies(0); vertex_iterator != vertex_iterator_end; ++vertex_iterator)
			{
				size_t maskid = 0;
				size_t gotvariables = 0;
				const Core::FMTdevelopment& dev = m_graph->getdevelopment(*vertex_iterator);
				for (const Core::FMTmask& globalmask : globalmasks)
				{
					if (dev.getmask().isSubsetOf(globalmask))
					{
						const int varindex = m_graph->getoutvariables(*vertex_iterator).at(-1);
						if (*(colupperbounds + varindex) == std::numeric_limits<double>::max())
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
			modelwithproportion.solver.setColSetBounds(&colstarget[0],&colstarget.back() + 1, &newbounds[0]);
			if (!modelwithproportion.solver.resolve())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot regenerate initial optimal solution",
					"FMTlpmodel::getareavariabilities", __LINE__, __FILE__);
			}
		}catch (...)
		{
			_exhandler->printexceptions("for " + name, "FMTlpmodel::getmodelfromproportions", __LINE__, __FILE__);
		}
		return modelwithproportion;
	}

	std::map<std::string, std::vector<double>>FMTlpmodel::getareavariabilities(const std::vector<Core::FMToutput>& localoutputs,
		const std::vector<Core::FMTmask>& globalmasks,
		std::vector<double> tolerances) const
	{
		std::map<std::string, std::vector<double>>uppernlower;
		try {
			if (tolerances.empty())
				{
				tolerances = std::vector<double>(globalmasks.size(),FMT_DBL_TOLERANCE);
				}
			if (solver.isProvenOptimal())
			{
				for (const Core::FMToutput& output : localoutputs)
				{
					std::vector<double>outputvalues;
					for (int period = 0; period <= static_cast<int>(m_graph->size() - 2); ++period)
					{
						outputvalues.push_back(this->getoutput(output, period).at("Total"));
					}
					uppernlower["OLD_" + output.getname()] = outputvalues;
				}
			}
			const FMTlpmodel newmodelwithproportion = getmodelfromproportions(globalmasks, tolerances);
			for (const Core::FMToutput& output : localoutputs)
			{
				std::vector<double>outputvalues;
				for (int period = 0; period <= static_cast<int>(m_graph->size() - 2); ++period)
				{
					outputvalues.push_back(newmodelwithproportion.getoutput(output, period).at("Total"));
				}
				uppernlower["NEW_" + output.getname()] = outputvalues;
			}
		}catch (...)
			{
			_exhandler->printexceptions("for " + name, "FMTlpmodel::getareavariabilities", __LINE__, __FILE__);
			}
	return uppernlower;
	}

	std::map<std::string, std::vector<double>>FMTlpmodel::getvariabilities(const std::vector<Core::FMToutput>& outputs, const int& periodstart,const int& periodstop)
		{
		std::map<std::string, std::vector<double>>uppernlower;
		try {
			const double* originalcoef = solver.getObjCoefficients();
			const std::vector<double>originalcoefficients(originalcoef, (originalcoef + solver.getNumCols()));
			const double originalsense = solver.getObjSense();
			const std::vector<int>objectivebounds = setobjectivebounds(true, true, parameters.getdblparameter(FMTdblmodelparameters::TOLERANCE));
			Core::FMToutput outtest;
			for (const Core::FMToutput& output : outputs)
			{
				std::vector<double>medianvalues;
				for (int period = periodstart; period <= periodstop; ++period)
				{
					medianvalues.push_back(this->getoutput(output, period,Core::FMToutputlevel::totalonly).begin()->second);
				}
				uppernlower["M" + output.getname()] = medianvalues;
				Core::FMTconstraint maxconstraint(Core::FMTconstrainttype::FMTMAXobjective, output);
				std::vector<double>uppervalues;
				Core::FMTconstraint minconstraint(Core::FMTconstrainttype::FMTMINobjective, output);
				std::vector<double>lowervalues;
				for (int period = periodstart ; period <= periodstop ; ++period )
				{
					maxconstraint.setlength(period,period);
					this->setobjective(maxconstraint);
					if(!this->initialsolve())
					{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Upper objectif unfeasable at period "+std::to_string(period),"FMTlpmodel::getvariabilities", __LINE__, __FILE__);
					}
					uppervalues.push_back(this->getoutput(output, period,Core::FMToutputlevel::totalonly).begin()->second);
					minconstraint.setlength(period,period);
					this->setobjective(minconstraint);
					if (!this->initialsolve())
					{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Lower objectif unfeasable at period "+std::to_string(period),"FMTlpmodel::getvariabilities", __LINE__, __FILE__);
					}
					lowervalues.push_back(this->getoutput(output, period,Core::FMToutputlevel::totalonly).begin()->second);
				}
				uppernlower["U" + output.getname()] = uppervalues;	
				uppernlower["L" + output.getname()] = lowervalues;
			}
			solver.setObjective(&originalcoefficients[0]);
			solver.setObjSense(originalsense);
			solver.deleteRows(static_cast<int>(objectivebounds.size()), &objectivebounds[0]);
			if(!solver.initialsolve())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Cannot resolve to reset initial state","FMTlpmodel::getvariabilities", __LINE__, __FILE__);
			}
		}catch (...)
			{
			_exhandler->printexceptions("for "+name,
										"FMTlpmodel::getvariabilities", __LINE__, __FILE__);
			}
		return uppernlower;
		}
	
	Graph::FMTgraphstats FMTlpmodel::eraseconstraint(const Core::FMTconstraint& constraint, int period)
		{
		try {
		const std::vector<Core::FMTconstraint>::const_iterator CONSTRAINT_IT = _getsetConstraintIndex(constraint);
		if (period==-1)
			{
				int first_period = 0;
				int last_period = 0;
				if (m_graph->constraintlenght(constraint, first_period, last_period))
				{
					for (int period = first_period; period <= last_period; ++period)
					{
						this->eraseconstraint(constraint, period);
					}
				}
		}
		else if (isMatrixElement(CONSTRAINT_IT, FMTmatrixelement::constraint, period)||
			isMatrixElement(CONSTRAINT_IT, FMTmatrixelement::strictlypositive, period))
			//(static_cast<int>(elements.size()) > period && elements.at(period).find(std::string(constraint))!= elements.at(period).end())
			{
			const std::vector<FMTmatrixelement> simpleelements = { FMTmatrixelement::constraint,FMTmatrixelement::strictlypositive };
			//std::vector<std::vector<int>>all_elements = elements.at(period).at(std::string(constraint));
			//elements.at(period).erase(elements.at(period).find(std::string(constraint)));

			int removedrow = -1;
			const int LENGTH = getparameter(FMTintmodelparameters::LENGTH);
			for (const auto& elementtype : simpleelements)
			{
				const std::vector<int> ELEMENTS = getMatrixElement(CONSTRAINT_IT, period, elementtype);
				if (!ELEMENTS.empty())
					{
					getMatrixElementRef(CONSTRAINT_IT, period, elementtype).clear();
					}
				if (!ELEMENTS.empty())
				{
					for (const int& levelid : ELEMENTS)
					{
						bool removeconstraint = true;
						for (int locator = (period + 1); locator < LENGTH + 1; ++locator)
						{
							for (std::vector<Core::FMTconstraint>::const_iterator it = constraints.begin();
								it != constraints.end();++it)
							{
								const std::vector<int> CONSTRAINT_ELEMENTS = getMatrixElement(it, locator, elementtype);
								if (std::find(CONSTRAINT_ELEMENTS.begin(), CONSTRAINT_ELEMENTS.end(), levelid)!=
									CONSTRAINT_ELEMENTS.end())
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
							--m_graph->getstatsptr()->rows;
							--m_graph->getstatsptr()->output_rows;
							}
					}
				}
			}
			
			if (isMatrixElement(CONSTRAINT_IT, FMTmatrixelement::goalvariable, period))
				{
				for (const int& colid : getMatrixElement(CONSTRAINT_IT, period, FMTmatrixelement::goalvariable))
					{
					solver.deleteCol(colid);
					--m_graph->getstatsptr()->cols;
					--m_graph->getstatsptr()->output_cols;
					}
				}
			
			if (isMatrixElement(CONSTRAINT_IT, FMTmatrixelement::levelvariable, period) ||
				isMatrixElement(CONSTRAINT_IT, FMTmatrixelement::objectivevariable, period))
				{
				const std::vector<int> OBJ = getMatrixElement(CONSTRAINT_IT, period, FMTmatrixelement::objectivevariable);
				const std::vector<int> LEVEL = getMatrixElement(CONSTRAINT_IT, period, FMTmatrixelement::levelvariable);
				std::vector<int>potentialcols;
				potentialcols.insert(potentialcols.end(), OBJ.begin(), OBJ.end());
				potentialcols.insert(potentialcols.end(), LEVEL.begin(), LEVEL.end());
				bool candelete = true;
				for (const int& levelid : potentialcols)
					{
					for (int locator = (period+1); locator < LENGTH+1;++locator)
						{
						for (std::vector<Core::FMTconstraint>::const_iterator it = constraints.begin();
							it != constraints.end(); ++it)
						{
							const std::vector<int> LEVEL_ELEMENTS = getMatrixElement(it, locator, FMTmatrixelement::levelvariable);
							const std::vector<int> OBJ_ELEMENTS = getMatrixElement(it, locator, FMTmatrixelement::objectivevariable);
							if (std::find(LEVEL_ELEMENTS.begin(), LEVEL_ELEMENTS.end(), levelid) != LEVEL_ELEMENTS.end() ||
								std::find(OBJ_ELEMENTS.begin(), OBJ_ELEMENTS.end(), levelid) != OBJ_ELEMENTS.end())
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
						--m_graph->getstatsptr()->cols;
						--m_graph->getstatsptr()->output_cols;
						//colstoremove.push_back(levelid);
						}
					}
				}
			}
			this->updatematrixngraph(false);
		}
			catch (...)
				{
				_exhandler->raisefromcatch("for "+std::string(constraint),"FMTlpmodel::eraseconstraint", __LINE__, __FILE__);
				}
		return m_graph->getstats();
		}
	
	
	void FMTlpmodel::updateconstraintsmapping(const std::vector<int>& Dvariables, const std::vector<int>& Dconstraints)
		{
		try {
			if (!Dvariables.empty() || !Dconstraints.empty())
			{
				const int LENGTH = getparameter(Models::FMTintmodelparameters::LENGTH);
				for (int period = 0; period < LENGTH + 1; ++period)
				{
					for (std::vector<Core::FMTconstraint>::const_iterator it = constraints.begin();
						it != constraints.end(); ++it)
					{

						if (!Dconstraints.empty())
						{
							if (!getMatrixElement(it, period, FMTmatrixelement::constraint).empty())
								{
								updatematrixelements(getMatrixElementRef(it, period, FMTmatrixelement::constraint), Dconstraints);
								}
							
						}
						if (!Dvariables.empty())
						{
							if (!getMatrixElement(it, period, FMTmatrixelement::levelvariable).empty())
								{
								updatematrixelements(getMatrixElementRef(it, period, FMTmatrixelement::levelvariable), Dvariables);
								}
							//updatematrixelements(getMatrixElementRef(it, period, FMTmatrixelement::levelvariable), Dvariables);
							if (!getMatrixElement(it, period, FMTmatrixelement::objectivevariable).empty())
							{
								updatematrixelements(getMatrixElementRef(it, period, FMTmatrixelement::objectivevariable), Dvariables);
							}
							//updatematrixelements(getMatrixElementRef(it, period, FMTmatrixelement::objectivevariable), Dvariables);
							if (!getMatrixElement(it, period, FMTmatrixelement::goalvariable).empty())
							{
								updatematrixelements(getMatrixElementRef(it, period, FMTmatrixelement::goalvariable), Dvariables);
							}
							//updatematrixelements(getMatrixElementRef(it, period, FMTmatrixelement::goalvariable), Dvariables);
						}
					}
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
	
	bool FMTlpmodel::updatematrixngraph(bool updategraph)
		{
		solver.sortdeletedcache();
		const std::vector<int>& deletedconstraints = solver.getcachedeletedconstraints();
		const std::vector<int>& deletedvariables = solver.getcachedeletedvariables();
		if (!deletedconstraints.empty() || !deletedvariables.empty())
			{
			try {
				if (updategraph)
				{
					m_graph->updatematrixindex(deletedvariables, deletedconstraints);
				}
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
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"This function is need to be rewrite because updatematrixngraph is now in eraseconstraint. This function is nowhere used in FMT normally.", "FMTlpmodel::eraseperiod", __LINE__, __FILE__);
			int firstperiod = m_graph->getfirstactiveperiod();
			if (isperiodbounded(firstperiod))
				{
				std::vector<int>deletedconstraints;
				std::vector<int>deletedvariables;
				if (!constraintsonly)
					{
						m_graph->eraseperiod(deletedconstraints, deletedvariables);
					}
					++firstperiod;
					
					for (const Core::FMTconstraint& constraint : constraints)
					{
						this->eraseconstraint(constraint, firstperiod);
					}
					if (!constraintsonly)
					{
						m_graph->eraseperiod(deletedconstraints, deletedvariables, true);
					}
					for (const int& constraintid : deletedconstraints)
						{
						solver.deleteRow(constraintid);
						}
					for (const int& variableid : deletedvariables)
						{
						solver.deleteCol(variableid);
						}
					this->updatematrixngraph(false);
				}else {
					const int badperiod = std::max(firstperiod, 1);
					_exhandler->raise(Exception::FMTexc::FMTunboundedperiod,
						std::to_string(badperiod),"FMTlpmodel::eraseperiod", __LINE__, __FILE__);
					}

		}catch (...)
			{
				_exhandler->printexceptions("", "FMTlpmodel::eraseperiod", __LINE__, __FILE__);
			}

		 return m_graph->getstats();
		 }
	
     void FMTlpmodel::locatelevels(const std::vector<Core::FMToutputnode>& nodes,int period,
		 std::map<int,double>& variables, const std::vector<Core::FMTconstraint>::const_iterator& p_it, double multiplier)
            {
			try {
				std::vector<std::string>level_names = p_it->getvariablelevels();
				if (!level_names.empty())
					{
					for (const Core::FMToutputnode& node : nodes)
						{
						if (node.source.isvariablelevel())
							{
							std::string level_name = level_names.front();
							level_names.erase(level_names.begin());
							if (node.ispastperiod())
								{
								period = (node.source.getperiodlowerbound() + period);
								}
							const int level_index = getsetlevel(p_it,level_name,period);
							variables[level_index] = node.constant*multiplier;
							}
						}
					}
			}catch (...)
				{
				_exhandler->raisefromcatch(std::string(*p_it),"FMTlpmodel::locatelevels", __LINE__, __FILE__);
				}
            }

	 int FMTlpmodel::getlevelfromlevelname(const std::string& variable_level,
		 int period, const std::vector<Core::FMTconstraint>::const_iterator& p_it) const
	 {
		 try {
			 std::vector<Core::FMTconstraint>::const_iterator it = constraints.begin();
			 for (const Core::FMTconstraint& model_constraint : constraints)
			 {
				 if (p_it==constraints.end() || model_constraint != *p_it)
				 {
					 const std::vector<std::string>level_names = model_constraint.getvariablelevels();
					 if (!level_names.empty())
					 {
						 const std::vector<int> ELEMENTS = getMatrixElement(it, period, FMTmatrixelement::levelvariable);
						 
						 std::vector<std::string>::const_iterator name_it = find(level_names.begin(), level_names.end(), variable_level);
						 const size_t variable_location = std::distance(level_names.cbegin(), name_it);
						 if (!ELEMENTS.empty() && ELEMENTS.size() > variable_location &&
							 (name_it != level_names.end())) // caught a constriant with level!
						 {
							 return ELEMENTS.at(variable_location);
						 }
						 /*const std::vector<std::vector<int>>constraint_elements = getMatrixElement(model_constraint, period);
						 std::vector<std::string>::const_iterator name_it = find(level_names.begin(), level_names.end(), variable_level);
						 const std::vector<int> levelconstelem = constraint_elements.at(FMTmatrixelement::levelvariable);
						 const size_t variable_location = std::distance(level_names.cbegin(), name_it);
						 if (!levelconstelem.empty() && levelconstelem.size() > variable_location &&
							 (name_it != level_names.end())) // caught a constriant with level!
						 {
							 return levelconstelem.at(variable_location);
						 }*/
					 }
				 }
				 ++it;
			 }
		 }catch (...)
		 {
			 _exhandler->raisefromcatch("for " + std::string(*p_it),
				 "FMTlpmodel::getlevelfromlevelname", __LINE__, __FILE__);
		 }
		 return -1;
	 }


    int FMTlpmodel::getsetlevel(const std::vector<Core::FMTconstraint>::const_iterator& p_it,
								const std::string& variable_level,int period)
        {
		/*for (const Core::FMTconstraint& model_constraint : constraints)
		{
			if (model_constraint != constraint)
			{
				const std::vector<std::string>level_names = model_constraint.getvariablelevels();
				if (!level_names.empty())
				{
					const std::vector<std::vector<int>>constraint_elements = getmatrixelement(model_constraint, period);
					std::vector<std::string>::const_iterator name_it = find(level_names.begin(), level_names.end(), variable_level);
					const std::vector<int> levelconstelem = constraint_elements.at(FMTmatrixelement::levelvariable);
					const size_t variable_location = std::distance(level_names.cbegin(), name_it);
					if (!levelconstelem.empty() && levelconstelem.size() > variable_location &&
						(name_it != level_names.end())) // caught a constriant with level!
					{
						return levelconstelem.at(variable_location);
					}
				}
			}
		}
		std::map<int, double>no_index;
		return getsetmatrixelement(constraint, FMTmatrixelement::levelvariable, no_index, period);*/
		int index = -1;
		try {
			index = getlevelfromlevelname(variable_level, period, p_it);
			if (index==-1)
			{
				std::map<int, double>no_index;
				return getsetMatrixElement(p_it, FMTmatrixelement::levelvariable, no_index, period);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("for " + std::string(*p_it),
				"FMTlpmodel::getsetlevel", __LINE__, __FILE__);
			}
		return index;
        }

	bool FMTlpmodel::isMatrixElement(const std::vector<Core::FMTconstraint>::const_iterator& p_constraintId,
		const FMTmatrixelement& p_element_type, int p_period) const
		{
		if (p_period == -1)
			{
			p_period = m_graph->getfirstactiveperiod()+1;
			}
		ConstraintIndex index(_getIndex(p_constraintId), p_period, p_element_type);
		std::map<ConstraintIndex,std::vector<int>>::const_iterator indexIt = m_indexes.find(index);
		return (indexIt != m_indexes.end() && !indexIt->second.empty());

		/*return((period < static_cast<int>(elements.size()) &&
			(elements.at(period).find(std::string(constraint)) != elements.at(period).end()) &&
			!elements.at(period).at(std::string(constraint)).at(element_type).empty()));*/
		}

	bool FMTlpmodel::containsMatrixElements(const std::vector<Core::FMTconstraint>::const_iterator& p_constraintId, 
											int p_period) const
		{
		for (int typeId = FMTmatrixelement::goalvariable; typeId != FMTmatrixelement::nr_items; typeId++)
			{
			FMTmatrixelement type = static_cast<FMTmatrixelement>(typeId);
			if (isMatrixElement(p_constraintId,type,p_period))
				{
				return true;
				}
			}
		return false;
		/*return(period < static_cast<int>(elements.size()) &&
			(elements.at(period).find(std::string(constraint)) != elements.at(period).end()));*/
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
			/*if (!objective.canbenodesonly())
				{
				_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
					"The objective output "+std::string(objective)+" cannot be deduct to output nodes",
					"FMTlpmodel::setobjective", __LINE__, __FILE__);
				}*/
			if (!objective.islinear())
			{
				_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
					"The objective output " + std::string(objective) + " cannot be deduct to output nodes",
					"FMTlpmodel::setobjective", __LINE__, __FILE__);
			}
			const std::vector<Core::FMTconstraint>::const_iterator OBJECTIVE_IT = _getsetConstraintIndex(objective);
			const bool strictlypositivesoutputsmatrix = getparameter(STRICTLY_POSITIVE);
			int first_period = 0;
			int last_period = 0;
			m_graph->constraintlenght(objective, first_period, last_period);
			//Its now handle in FMToutputnode settograph
			/*double averagefactor = 1;
			if (last_period != first_period)
			{
				averagefactor = (1 / (last_period - first_period));
			}*/
			std::vector<std::string>equation;
			const std::vector<Core::FMToutputnode>all_nodes = objective.getnodes(equation,1,strictlypositivesoutputsmatrix);
			std::map<int, double>all_variables;
			if (!objective.extravariables())
			{
				if (!objective.canbenodesonly())
					{
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"The objective value " + std::string(objective) + " will be different than the output value",
						"FMTlpmodel::setobjective", __LINE__, __FILE__);
					}
				for (int period = first_period; period <= last_period; ++period)
				{
					const std::vector<std::map<int, double>> outputvarpos = locatenodes(all_nodes, period, all_variables);
					setpositiveoutputsinmatrix(OBJECTIVE_IT,outputvarpos,period);
					//locatenodes(all_nodes, period, all_variables);
					locatelevels(all_nodes, period, all_variables, OBJECTIVE_IT);
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
				const int variable_id = getsetMatrixElement(OBJECTIVE_IT, FMTmatrixelement::objectivevariable, all_variables);
				//*_logger << "variable id of " << variable_id << "\n";
				bool gotvariables = false;
				for (int period = first_period; period <= last_period; ++period)
				{
					std::map<int, double>period_variables;
					const std::vector<std::map<int, double>> outputvarpos = locatenodes(all_nodes, period, period_variables);
					setpositiveoutputsinmatrix(OBJECTIVE_IT,outputvarpos,period);
					//locatenodes(all_nodes, period, period_variables);
					locatelevels(all_nodes, period, all_variables, OBJECTIVE_IT);
					if (!period_variables.empty())
					{
						period_variables[variable_id] = -1;
						const int constraint_id = getsetMatrixElement(OBJECTIVE_IT, FMTmatrixelement::constraint, period_variables,
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

		return m_graph->getstats();
		}

     bool FMTlpmodel::resolve()
        {
		 try 
		 {
			 return solver.stockresolve();
		 }
		 catch (...)
		 {
			 if (DEBUG_MATRIX)
			 {
				 writeLP((boost::filesystem::path(getruntimelocation()) += boost::filesystem::path::preferred_separator).string());
			 }
			 _exhandler->printexceptions("", "FMTlpmodel::resolve", __LINE__, __FILE__);
		 }

		 return false;
        }


     bool FMTlpmodel::getgoals(const std::vector<std::string>& goalsnames, std::map<int, double>& index,const double& sense)const
        {
        bool found_something = false;
		try {
			std::vector<Core::FMTconstraint>::const_iterator it = constraints.begin();
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
						m_graph->constraintlenght(constraint, first_period, last_period);
						for (int period = first_period; period <= last_period; ++period)
						{
							const std::vector<int> COLS = getMatrixElement(it, period, FMTmatrixelement::goalvariable);
							if (!COLS.empty())
							{
								found_something = true;
								index[COLS.at(0)] = value;
							}
							/*const std::vector<std::vector<int>>constraint_elements = getmatrixelement(constraint, period);
							if (!constraint_elements.at(FMTmatrixelement::goalvariable).empty())
							{
								found_something = true;
								index[constraint_elements.at(FMTmatrixelement::goalvariable).at(0)] = value;
							}*/
						}
					}
				}
				++it;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTlpmodel::getgoals", __LINE__, __FILE__);
			}
        return found_something;
        }

    int FMTlpmodel::getsetMatrixElement(const std::vector<Core::FMTconstraint>::const_iterator& p_constraintId,
                     const FMTmatrixelement& element_type,const std::map<int,double>& indexes,
                     int period,double lowerbound,double upperbound)
        {
		int element_id = 0;
		try {
			//*_logger << "getsetStage " + std::to_string(1) + " " + std::to_string(element_type) +  "\n";
			//*_logger<<"Enter " <<std::string(constraint)<<" "<<element_type<< " "<<period<<"\n";
			if (isMatrixElement(p_constraintId, element_type, period))
			{
				//*_logger << "getsetStage " + std::to_string(2) + "\n";
				//*_logger<<"Issamematrix " <<std::string(constraint)<<" "<<element_type<< " "<<period<<"\n";
				int foundperiod = period;
				if (period == -1)
				{
					foundperiod = m_graph->getfirstactiveperiod() + 1;
				}
				//*_logger << "getsetStage " + std::to_string(3) + "\n";
				//const std::vector<int>& alllocalelements = elements.at(foundperiod).at(std::string(p_BaseConstraint)).at(element_type);
				const std::vector<int> alllocalelements = getMatrixElement(p_constraintId, foundperiod, element_type);
				for (const int& elid : alllocalelements)
				{
					if (issamematrixelement(elid, element_type, lowerbound, upperbound, indexes))
					{
						return elid;
					}
				}
			}
			if (element_type == FMTmatrixelement::constraint  && indexes.empty())
			{
				//*_logger << "getsetStage " + std::to_string(4) + "\n";
				if (period>(m_graph->getfirstactiveperiod()) &&period<static_cast<int>(m_graph->size()) &&
					p_constraintId->getconstrainttype() == Core::FMTconstrainttype::FMTstandard &&
					(upperbound< 0 || lowerbound > 0))
					{
					std::string cname = std::string(*p_constraintId);
					cname.erase(std::remove(cname.begin(), cname.end(), '\n'), cname.end());
					_exhandler->raise(Exception::FMTexc::FMTinfeasibleconstraint,
						std::string(cname) + " at period " + std::to_string(period),
						"FMTlpmodel::getsetmatrixelement", __LINE__, __FILE__);
				}else {
					std::string cname = std::string(*p_constraintId);
					cname.erase(std::remove(cname.begin(), cname.end(), '\n'), cname.end());
					_exhandler->raise(Exception::FMTexc::FMTnonaddedconstraint,
						std::string(cname) + " at period " + std::to_string(period),
						"FMTlpmodel::getsetmatrixelement", __LINE__, __FILE__);
					}
				return -1;
			}
			//*_logger << "getsetStage " + std::to_string(5) + "\n";
			Graph::FMTgraphstats* stats = m_graph->getstatsptr();
			if (element_type == FMTmatrixelement::constraint || element_type == FMTmatrixelement::strictlypositive)
			{
				//*_logger << "getsetStage " + std::to_string(6) + "\n";
				std::vector<int>sumvariables;
				std::vector<double>sumcoefficiants;
				summarize(indexes, sumvariables, sumcoefficiants);
				//*_logger<<"Add " <<std::string(constraint)<<" "<<element_type<< " "<<period<<"\n";
				//*_logger << "getsetStage " + std::to_string(7) + "\n";
				if (sumvariables.empty() && element_type == FMTmatrixelement::strictlypositive)
				{
					return -1;
				}else{
					element_id = stats->rows;
					if (!p_constraintId->canbenodesonly()&&period!=-1)
						{
						p_constraintId->getRHSvalue(period, lowerbound, upperbound);
						}
					if (!sumvariables.empty())
						{
						solver.addRow(static_cast<int>(sumvariables.size()), &(*sumvariables.cbegin()), &(*sumcoefficiants.cbegin()), lowerbound, upperbound);
						++stats->rows;
						++stats->output_rows;
					}else {
						return -1;
						}
					
				}
			}
			else {
				//*_logger << "getsetStage " + std::to_string(8) + "\n";
				element_id = stats->cols;
				//solver.addCol(0, &(*sumvariables.cbegin()),&(*sumcoefficiants.cbegin()), lowerbound, upperbound, 0);
				solver.addCol(0,nullptr,nullptr, lowerbound, upperbound, 0);
				++stats->cols;
				++stats->output_cols;
			}
			/*if ((p_constraintId->size() != (m_graph->size() - 1)) && (period >= static_cast<int>(p_constraintId->size())) || period<0) //just for resizing!
			{
				size_t location = 0;
				if (period < 0)
				{
					location = m_graph->size();
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

			}*/
			//*_logger << "getsetStage " + std::to_string(9) + "\n";
			int starting = period;
			int stoping = period + 1;
			if (period < 0)
			{
				//*_logger << "getsetStage " + std::to_string(10) + "\n";
				starting = 0;
				stoping = getparameter(Models::FMTintmodelparameters::LENGTH) +1;
			}
			//*_logger << "getsetStage " + std::to_string(11) + "\n";
			for (int locid = starting; locid < stoping; ++locid)
				{
				ConstraintIndex index(_getIndex(p_constraintId),locid, element_type);
				std::map<ConstraintIndex,std::vector<int>>::iterator target = m_indexes.find(index);
				if (target == m_indexes.end())
					{
					ConstraintIndex CompleteIndex(_getIndex(p_constraintId), locid,element_type);
					target = m_indexes.insert(std::pair<ConstraintIndex,std::vector<int>>(CompleteIndex,std::vector<int>(m_rowsAllocator))).first;
					target->second.reserve(100);
					}
				//ConstraintIndex& toPush = const_cast<ConstraintIndex&>(*target);//wont change sorting
				//toPush.push_back(element_id);
				target->second.push_back(element_id);
				}

			/*if (period < 0)
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
			}*/
		}catch (...)
			{
			_logger->logwithlevel(solver.getcacheelements(), 4);
			_exhandler->raisefromcatch(std::string(*p_constraintId)+" at period "+std::to_string(period)+
				solver.lowernuppertostr(lowerbound,upperbound),"FMTlpmodel::getsetmatrixelement", __LINE__, __FILE__);
			}
		return element_id;
        }

		const std::vector<int> FMTlpmodel::getMatrixElement(const std::vector<Core::FMTconstraint>::const_iterator& it,
			int p_period,
			FMTmatrixelement p_element) const
        {
			std::vector<int>result;
			ConstraintIndex index(_getIndex(it), p_period, p_element);
			std::map<ConstraintIndex,std::vector<int>>::const_iterator indexIt = m_indexes.find(index);
			if (indexIt!= m_indexes.end())
				{
				result = indexIt->second;
				}
			return result;
        }

		std::vector<int>& FMTlpmodel::getMatrixElementRef(const std::vector<Core::FMTconstraint>::const_iterator& it,
			int p_period,
			FMTmatrixelement p_element)
		{
			ConstraintIndex index(_getIndex(it), p_period, p_element);
			std::map<ConstraintIndex, std::vector<int>>::iterator indexIt = m_indexes.find(index);
			//ConstraintIndex& toPush = const_cast<ConstraintIndex&>(*indexIt);//wont change sorting
			return indexIt->second;
		}

	std::vector<Heuristics::FMToperatingareascheduler>FMTlpmodel::getoperatingareaschedulerheuristics(const std::vector<Heuristics::FMToperatingareascheme>& opareas,
																						const Core::FMToutputnode& node,
																						size_t numberofheuristics,
																						bool copysolver)		
		{
		const bool userandomness = false;
		size_t seedof = 1;
		const double proportionofset = 0.25;
		std::vector<Heuristics::FMToperatingareascheduler>allheuristics;
		try {
			updatematrixnaming();
			for(int i = 0 ; i < static_cast<int>(opareas.size()) ; ++i)
			{
				const Core::FMTmask m1 = opareas.at(i).getmask();
				for(int ii = i+1 ; ii < static_cast<int>(opareas.size()) ; ++ii)
				{
					const Core::FMTmask m2 = opareas.at(ii).getmask();
					if(!(m1.isnotthemessubset(m2,themes)))
					{
						_exhandler->raise(	Exception::FMTexc::FMTfunctionfailed,
											"The masks of operating area intersects each other : \n"+std::string(m1)+"\n"+std::string(m2),
											"FMTlpmodel::getObjValue",
											__LINE__, __FILE__);
					}
				}
			}
			//Validate intersecting mask of opareas
			allheuristics.emplace_back(opareas, *m_graph, *this, node,*this->getsolverptr(), seedof, proportionofset, userandomness, copysolver);
			for (size_t heuristicid = 1 ; heuristicid < numberofheuristics; ++heuristicid)
				{
				allheuristics.emplace_back(*allheuristics.begin());
				allheuristics.back().setasrandom();
				allheuristics.back().setgeneratorseed(seedof);
				//allheuristics.back().passinobject(*this);
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
                //allheuristics.back().passinobject(*this);
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
			if (DEBUG_MATRIX)
			{
				*_logger << "Writing matrix here " + boost::filesystem::path(getruntimelocation()).string() << "\n";
				writeLP((boost::filesystem::path(getruntimelocation()) += boost::filesystem::path::preferred_separator).string());
			}
			_exhandler->printexceptions(getname(), "FMTlpmodel::initialsolve", __LINE__, __FILE__);
		}

		return false;
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
			if (DEBUG_MATRIX)
				{
				*_logger << "Writing matrix here " + name +".lp" + "\n";
				}
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

	void FMTlpmodel::updategeneralconstraintsnaming(std::vector<std::string>& colnames,
													std::vector<std::string>& rownames) const
	{
		try {
			int constraintid = 0;
			std::vector<Core::FMTconstraint>::const_iterator it = constraints.begin();
			for (const Core::FMTconstraint& constraint : constraints)
				{
				int first_period = 0;
				int last_period = 0;
				if (m_graph->constraintlenght(constraint, first_period,last_period))
					{
					if(constraint.acrossperiod())
					{
						++last_period;
					}
					const std::string constraintname("ID"+std::to_string(constraintid));
					for (int period = first_period;period <= last_period;++period)
						{
						const std::string constnperiod(constraintname + "_P" + std::to_string(period));
						for (int typeId = FMTmatrixelement::goalvariable; typeId != FMTmatrixelement::nr_items; typeId++)
						{
							FMTmatrixelement elementtype = static_cast<FMTmatrixelement>(typeId);
							const std::vector<int>  elements = getMatrixElement(it,period, elementtype);
							if (!elements.empty())
							{
								std::string* original;
								size_t elid = 0;
								for (const auto& el : elements)
								{
									if (elementtype == FMTmatrixelement::constraint ||
										elementtype == FMTmatrixelement::strictlypositive)
									{
										original = &rownames[el];
									}
									else {
										original = &colnames[el];
									}
									if (!original->empty())
									{
										if (original->at(original->size() - 2) == '_')
										{
											*original = original->substr(0, original->size() - 1) + std::to_string(period);
										}
										else {
											*original = *original + "_" + std::to_string(period);
										}
									}
									else {
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
						}
						/*const std::vector<std::vector<int>>constraintindex = getMatrixElement(constraint, period);
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
							}*/
						}
					}
				++constraintid;
				++it;
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

	FMTlpmodel::FMTlpmodel(const FMTsrmodel& rhs) :
		FMTsrmodel(rhs),
		m_rowsAllocator(),
		m_indexAllocator(),
		m_indexes(m_indexAllocator)
	{

	}

	FMTlpmodel::FMTlpmodel(FMTlpmodel&& rhs):
		FMTsrmodel(),
		m_rowsAllocator(),
		m_indexAllocator(),
		m_indexes(m_indexAllocator)
	{
		*this = std::move(rhs);

	}
	
	FMTlpmodel& FMTlpmodel::operator =(FMTlpmodel&& rhs)
	{
		if (this!=&rhs) 
			{
			FMTsrmodel::operator=(std::move(rhs));
			m_rowsAllocator = rhs.m_rowsAllocator;
			m_indexAllocator = rhs.m_indexAllocator;
			m_indexes.swap(rhs.m_indexes);
			}
		return *this;
	}


	std::unique_ptr<FMTmodel> FMTlpmodel::presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
	{
		try{
			return std::unique_ptr<FMTmodel>(new FMTlpmodel(*(dynamic_cast<FMTsrmodel*>(FMTsrmodel::presolve(optionaldevelopments).get()))));
		}catch(...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::presolve", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
	}

	void FMTlpmodel::updatematrixnaming()
	{
		try {
			std::vector<std::string>colnames(solver.getNumCols());
			std::vector<std::string>rownames(solver.getNumRows());
			m_graph->getvariablenames(actions,colnames);
			m_graph->gettransferrownames(rownames);
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

	bool FMTlpmodel::build(std::vector<Core::FMTschedule> schedules)
	{
		try{
			const bool QUIET_LOG = parameters.getboolparameter(QUIET_LOGGING);
			const int length = parameters.getintparameter(LENGTH);
			bool allempty = true;
			for(const auto& schedule : schedules)
			{
				if(!schedule.empty())
				{
					allempty = false;
					break;
				}
			}
			const bool forcepartialbuild = parameters.getboolparameter(FORCE_PARTIAL_BUILD);
			std::string addon = "";
			if(!allempty)
			{
				addon = "FMT will use schedules pass by argument for periods 1 to "+std::to_string(schedules.size());
			}
			if (!QUIET_LOG)
			{ 
				_logger->logwithlevel("Building "+getname()+" for "+std::to_string(length)+" periods. "+addon+"\n",1);
			}
			//Period start at 0 but it's the period 1 that is created first. Reason is that schedules is a vector and the first elements 
			//is the schedule for period 1
			_setGraphCache();
			for (int period = 0; period<length;++period)
				{
					if(!allempty)
					{
						if(period<schedules.size())
						{
							const Graph::FMTgraphstats PERIOD = buildperiod(schedules.at(period), forcepartialbuild, parameters.getperiodcompresstime(period));
							if (!QUIET_LOG)
							{
								_logger->logwithlevel(std::string(PERIOD)+"\n",3);
							}
						}
						else{
							_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
												"The lenght to build the graph is bigger than the number of schedule passed to the function. Cannot have mix schedulebuild and fullbuild for now",
												"FMTlpmodel::build",__LINE__,__FILE__);
						}
					}else{
						const Graph::FMTgraphstats PERIOD = buildperiod(Core::FMTschedule(), false, parameters.getperiodcompresstime(period));
						if (!QUIET_LOG)
						{
							_logger->logwithlevel(std::string(PERIOD) + "\n", 3);
						}
					}
				}
			if(!allempty)
			{
				addon = "FMT will use schedules pass by argument for periods 1 to "+std::to_string(schedules.size())+" to set the solution in the matrix.";
			}
			if (!forcepartialbuild)
				{
				if (!QUIET_LOG)
				{
					_logger->logwithlevel("Setting constraints on the " + getname() + ". " + addon + "\n", 1);
				}
				_setConstraintsCache();
				for (size_t constraintid = 1; constraintid < constraints.size(); ++constraintid)
					{
					this->setconstraint(constraints.at(constraintid));
					}
				if (!constraints.empty())
				{
					const Graph::FMTgraphstats OBJECTIVE = setobjective(constraints.at(0));
					if (!QUIET_LOG)
					{
						_logger->logwithlevel("*Graph stats with all constraints : \n" + std::string(OBJECTIVE) + "\n", 1);
					}
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"No objective to set",
						"FMTlpmodel::build", __LINE__, __FILE__);
				}
				}
			if(!allempty)
			{
				return trysetsolution(schedules);
			}
		}catch(...)
		{
			_exhandler->raisefromcatch(getname(), "FMTlpmodel::build", __LINE__, __FILE__);
		}
		return true;
	}

	bool FMTlpmodel::solve()
	{
		bool optimal = false;
		try{
			optimal = this->initialsolve();
		}catch(...)
		{
			_exhandler->raisefromcatch(getname(), "FMTlpmodel::solve", __LINE__, __FILE__);
		}
		return optimal;
	}

	std::unique_ptr<FMTmodel> FMTlpmodel::getcopy(int period) const
	{
		try {
			return std::unique_ptr<FMTmodel>(new FMTlpmodel(*dynamic_cast<FMTsrmodel*>(FMTsrmodel::getcopy(period).get())));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTlpmodel::getcopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
	}


	bool FMTlpmodel::trysetsolution(const std::vector<Core::FMTschedule>& schedules)
	{
		try
		{
			int period = 1;
			setparameter(Models::FMTboolmodelparameters::SETSOLUTION_THROW, false);
			for (const Core::FMTschedule& schedule: schedules)
			{
				const bool SOLUTION_FOUND = setsolution(period, schedule,parameters.getdblparameter(FMTdblmodelparameters::TOLERANCE));
				if (!SOLUTION_FOUND)
				{
					setsolutionbylp(period, schedule, parameters.getdblparameter(FMTdblmodelparameters::TOLERANCE));
				}
				++period;
			}
			setparameter(Models::FMTintmodelparameters::LENGTH, period);
			setparameter(Models::FMTboolmodelparameters::SETSOLUTION_THROW, true);
		}catch(...){
			setparameter(Models::FMTboolmodelparameters::SETSOLUTION_THROW, true);
			_exhandler->raisefromcatch("", "FMTlpmodel::trysetsolution", __LINE__, __FILE__);
		}
		return true;
	}

	

	void FMTlpmodel::swap_ptr(std::unique_ptr<FMTmodel>& rhs)
	{
		FMTlpmodel* ptrToModel = dynamic_cast<FMTlpmodel*>(rhs.get());
		*this = std::move(*ptrToModel);
	}

}



BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTlpmodel)
#endif
