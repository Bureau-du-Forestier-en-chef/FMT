/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTOperatingAreaScheme.h"
#include "CoinPackedVector.hpp"
#include "FMTMatrixBuild.h"
#include "FMTGraph.hpp"
#include "FMTMask.h"
#include "FMTLpSolver.h"
#include <algorithm>
#include <numeric>

namespace Heuristics
{

double FMTOperatingAreaScheme::getPrimalArea(const double* primalsolution, const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& maingraph, const std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>& vertices) const//Get the area of the operating area base on a solution
	{
	double area = 0;
	for (const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor& descriptor : vertices)
		{
		if (maingraph.periodStart(descriptor))
			{
			area += maingraph.inArea(descriptor, primalsolution);
			}
		}
	return area;
	}

size_t FMTOperatingAreaScheme::getBestSchemeId(const double* primalsolution) const//Get the best possible scheme looking at the primal solution
	{
	size_t bestid = 0;
	double bestvalue = 0;
	for (size_t id = 0; id < openingbinaries.size();++id)
		{
		if (*(primalsolution + openingbinaries.at(id)) > bestvalue)
			{
			bestvalue = *(primalsolution + openingbinaries.at(id));
			bestid = id;
			}
		}
	return bestid;
	}



std::vector<std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>> FMTOperatingAreaScheme::generateSchemes(const std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>& vertices)// Generate unique schemes base on parameters
{
	std::vector<std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>>schemes;
		std::map<std::pair<size_t, size_t>, std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>nodes;
		std::vector<size_t>returntimeof;
		for (size_t returnid = returntime; returnid <= maxreturntime; ++returnid)
			{
			returntimeof.push_back(returnid);
			}
		std::sort(returntimeof.begin(), returntimeof.end());
		do {
			size_t acceptedscheme = 0;
			for (size_t id = 0; id < vertices.size(); ++id)
			{
				size_t returntimeid = 0;
				std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>> newscheme;
				std::unordered_set<std::string>enumdone;
				size_t localid = id;
				bool validscheme = true;
				std::string newenum;
				while (localid < vertices.size())
				{
					const size_t localreturntime = returntimeof.at(std::min(returntimeof.size() - 1, returntimeid));
					newenum += "_" + std::to_string(localreturntime);
					++returntimeid;
					size_t opening = 0;
					while (localid < vertices.size() && opening < openingtime && !vertices.at(localid).empty())
					{
							newscheme.push_back(vertices.at(localid));
							++opening;
							++localid;
					}
					if (opening < openingtime)//need a complete pattern
					{
						validscheme = false;
						break;
					}
					size_t closing = 0;
					while (localid < vertices.size() && closing < localreturntime)
					{
						++localid;
						++closing;
					}
				}


				if (validscheme && !newscheme.empty() && enumdone.find(newenum) == enumdone.end())
				{
					enumdone.insert(newenum);
					schemes.push_back(newscheme);
					++acceptedscheme;
				}
			}
			}while (std::next_permutation(returntimeof.begin(), returntimeof.end()));
	return schemes;
	}

int FMTOperatingAreaScheme::getMaxPeriod() const
{
	int maxperiod = 0;
	for (const std::vector<int>& periods : schemesperiods)
		{
		for (const int& period : periods)
			{
			if (period>maxperiod)
				{
				maxperiod = period;
				}
			}
		}
	return maxperiod;
}

std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor> FMTOperatingAreaScheme::getIgnoredVertices(const std::vector<std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>>& schemes,
	const std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>& targetedperiodsvertices) const
	{
		std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor> ignored;
		std::unordered_set<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor> schemesvertices;
		for (const auto& scheme : schemes)
		{
			for (const auto& periodv : scheme)
			{
				for (const auto& vertices : periodv)
				{
					schemesvertices.insert(vertices);
				}
			}
		}
		for (const auto& periodv : targetedperiodsvertices)
		{
			for (const auto& vertices : periodv)
			{
				if (schemesvertices.find(vertices) == schemesvertices.end())
				{
					ignored.push_back(vertices);
				}
			}
		}
		return ignored;
	}

	void FMTOperatingAreaScheme::schemestoLP(const std::vector<std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>>& schemes,
		const std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>& periodictargetednodes,
		const std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>& totalareavertices,
		Models::FMTLpSolver& solver,
		const double* primalsolution,
		const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& maingraph, const std::vector<int>& actionIDS)
	{
		int binaryid = solver.getNumCols();
		m_area = 0;
		if (!totalareavertices.empty())
			{
			const double area = this->getPrimalArea(primalsolution, maingraph, totalareavertices);
			//To remove the numeric instability from the multiplication in the graph inArea
			m_area = static_cast<double>((static_cast<int>(area/100)*100)+100);
			//old code
			//_area=this->getPrimalArea(primalsolution, maingraph, totalareavertices);
			}
		//map of period and variables from the graph link to the scheme 
		std::map<int, std::vector<int>>periodicsblocksvariables;
		std::vector<size_t>selectedschemes;
		size_t schemeid = 0;
		//To block activity in dev that are not in schemes and fit with the targeted nodes
		//They are remove in generateSchemes because we dont keep unfinished pattern
		std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor> ignoredvert = getIgnoredVertices(schemes, periodictargetednodes);
		if (!ignoredvert.empty())
		{
			const int cid = solver.getNumRows();
			std::vector<int> variablestoignore;
			for (const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor& descriptor : ignoredvert)
			{
				const std::map<int, int>actions = maingraph.getOutVariables(descriptor);
				for (const int& action : actionIDS)
				{
					std::map<int, int>::const_iterator actit = actions.find(action);
					if (actit != actions.end())
					{
						variablestoignore.push_back(actit->second);
					}
				}
			}
			if (!variablestoignore.empty())
			{
				std::vector<double>maxelements(variablestoignore.size(), 1.0);
				solver.addRow(static_cast<int>(variablestoignore.size()), &variablestoignore[0], &maxelements[0], 0, 0);
				solver.setRowName("rejectednodes_" + std::string(this->getMask()), cid);
				rejectednodescid = cid;
			}

		}
		//Iter over schemes to fill periodblocksvariables and get binary id and schemesid
		for (const std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>& scheme : schemes)
		{
			size_t blockid = 0;
			for (const std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>& localblock : scheme)
			{
				if (!localblock.empty())
				{
					const int period = schemesperiods.at(schemeid).at(blockid);
					if (periodicsblocksvariables.find(period) == periodicsblocksvariables.end())
					{
						periodicsblocksvariables[period] = std::vector<int>();
					}
					for (const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor& descriptor : localblock)
					{
						const std::map<int, int>actions = maingraph.getOutVariables(descriptor);
						for (const int& action : actionIDS)
						{
							std::map<int, int>::const_iterator actit = actions.find(action);
							if (actit != actions.end())
							{
								if (std::find(periodicsblocksvariables.at(period).begin(), periodicsblocksvariables.at(period).end(), actit->second) == periodicsblocksvariables.at(period).end())
								{
									periodicsblocksvariables[period].push_back(actit->second);
								}
							}
						}
					}
				}
				++blockid;
			}
			openingbinaries.push_back(binaryid);
			selectedschemes.push_back(schemeid);
			++binaryid;
			++schemeid;
		}
		//Iter over periodblocksvariable 
		int constraintid = solver.getNumRows();
		std::map<size_t, std::vector<int>>constraintsmap;
		for (std::map<int, std::vector<int>>::const_iterator periodics = periodicsblocksvariables.begin();
			periodics != periodicsblocksvariables.end(); ++periodics)
		{
			if (!periodics->second.empty())
			{
				std::vector<int>targetedvariables(periodics->second.begin(), periodics->second.end());
				std::vector<double>elements(periodics->second.size(), 1.0);
				size_t validscheme = 0;
				for (const size_t& shemeidselected : selectedschemes)
					{
						if (!schemesperiods.at(shemeidselected).empty() &&
							std::find(schemesperiods.at(shemeidselected).begin(), schemesperiods.at(shemeidselected).end(), periodics->first) != schemesperiods.at(shemeidselected).end())
						{
							targetedvariables.push_back(openingbinaries.at(validscheme));
							elements.push_back(-m_area);
							if (constraintsmap.find(shemeidselected) == constraintsmap.end())
							{
								constraintsmap[shemeidselected] = std::vector<int>();
							}
							constraintsmap[shemeidselected].push_back(constraintid);
						}
						++validscheme;
					}
				if (targetedvariables.size() > periodics->second.size())
				{
					solver.addRow(static_cast<int>(targetedvariables.size()), &targetedvariables[0], &elements[0], std::numeric_limits<double>::lowest(), 0);
					solver.setRowName("schemenode_"+std::string(this->getMask())+"_P"+std::to_string(periodics->first), constraintid);
				}
				++constraintid;
			}
		}
		openingconstraints.resize(openingbinaries.size());
		if (!constraintsmap.empty())
		{
			size_t tid = 0;
			for (const size_t& shemeidselected : selectedschemes)
			{
				if (constraintsmap.find(shemeidselected) != constraintsmap.end())
				{
					openingconstraints[tid] = constraintsmap.at(shemeidselected);
				}
				++tid;
			}
			for (size_t opid = 0; opid < openingbinaries.size();++opid)
			{
				solver.addCol(0, nullptr, nullptr, 0, 1);
				solver.setColName("schemebin_"+std::string(this->getMask())+"_binid_"+std::to_string(opid), openingbinaries.at(opid));
			}
			maximalschemesconstraint = constraintid;
			std::vector<double>maxelements(openingbinaries.size(), 1.0);
			solver.addRow(static_cast<int>(openingbinaries.size()), &openingbinaries[0], &maxelements[0], std::numeric_limits<double>::lowest(), 1);
			solver.setRowName("schemechoice_"+std::string(this->getMask()), constraintid);
		}
	}




void FMTOperatingAreaScheme::getRessourcesToDelete(std::vector<int>& colstodelete, std::vector<int>& rowstodelete) const
	{
	for (const int& binary : openingbinaries)
		{
		if (std::find(colstodelete.begin(), colstodelete.end(),binary)== colstodelete.end())
			{
			colstodelete.push_back(binary);
			}
		}
	for (const std::vector<int>& blockconstraint : openingconstraints)
		{
		for (const int& constraint : blockconstraint)
			{
				if (std::find(rowstodelete.begin(), rowstodelete.end(), constraint) == rowstodelete.end())
				{
					rowstodelete.push_back(constraint);
				}
			}
		}
	if (!openingconstraints.empty())
		{
		rowstodelete.push_back(maximalschemesconstraint);
		}
	}

void FMTOperatingAreaScheme::pushBinaries(std::vector<int>& targets) const
	{
	targets.insert(targets.end(), openingbinaries.begin(), openingbinaries.end());
	}
size_t FMTOperatingAreaScheme::unboundAllPrimalSchemes(std::vector<int>& targets, std::vector<double>& bounds) const //Unbound all binaries to 0<=B<=1
	{
	for (const int& bintounbound : openingbinaries)
		{
		targets.push_back(bintounbound);
		bounds.push_back(0);
		bounds.push_back(1);
		}
	return !openingbinaries.empty();
	}

void FMTOperatingAreaScheme::fillBoundsNVariables(const double* lowerb, const double* upperb, std::vector<int>& constraintstargets, std::vector<double>& bounds) const
{
	for (const std::vector<int>& constraints : openingconstraints)
	{
		if (!constraints.empty())
		{
			for (const int& newconstraint : constraints)
			{
				if (std::find(constraintstargets.begin(), constraintstargets.end(), newconstraint) == constraintstargets.end())
				{
					constraintstargets.push_back(newconstraint);
					bounds.push_back(*(lowerb+ newconstraint));
					bounds.push_back(*(upperb + newconstraint));
				}
			}
		}
	}
}

size_t FMTOperatingAreaScheme::unboundAllDualSchemes(std::vector<int>& targets, std::vector<double>& bounds) const
	{
	int processed = 0;
	for (const std::vector<int>& constraints : openingconstraints)
		{
		if (!constraints.empty())
			{
			for (const int& newconstraint : constraints)
				{
				if (std::find(targets.begin(), targets.end(), newconstraint)== targets.end())
					{
					targets.push_back(newconstraint);
					bounds.push_back(-std::numeric_limits<double>::max());
					bounds.push_back(m_area);
					}
				}
				processed += 1;
			}
		}
	return processed;
	}

bool FMTOperatingAreaScheme::boundPrimalScheme(std::vector<int>& targets, std::vector<double>& bounds, const size_t& schemeid) const //Looking at the primal solution set the best scheme to the solverinterface 1<=B<=1 and check optimality
	{
	targets.push_back(openingbinaries.at(schemeid));
	bounds.push_back(1.0);
	bounds.push_back(1.0);
	return !openingbinaries.empty();
	}

bool FMTOperatingAreaScheme::unboundDualScheme(const double* rowactivities,std::vector<int>& targets,
	std::vector<double>& bounds, const size_t& schemeid, bool looseset) const //Looking at the primal solution set the best scheme to the solverinterface 1<=B<=1 and check optimality
{
	double minimalopeningarea = std::numeric_limits<double>::lowest();
	if (threshold > FMT_DBL_TOLERANCE)
		{
		minimalopeningarea = threshold * m_area;
		}
	size_t cid = 0;
	//std::cout << std::string(getMask()) << " ";
	for (const std::vector<int>& constraints : openingconstraints)
		{
		if (!constraints.empty())
			{
			size_t constraintid = 0;
				for (const int& cit : constraints)
					{
					std::vector<int>::iterator targetit = std::find(targets.begin(), targets.end(), cit);
					double minimalcosarea = minimalopeningarea;
					if (!(constraintid<openingtime))//seulement les premiere ouverture vont conserver le threshold...
						{
						minimalcosarea = std::numeric_limits<double>::lowest();
						}

					if (targetit == targets.end())
						{
							if (cid == schemeid)
							{
								//double minimalcosarea = minimalopeningarea;
								if (looseset)
									{
									minimalcosarea = std::min(minimalcosarea, *(rowactivities + cit) - FMT_DBL_TOLERANCE);
									}
								//std::cout << "CID " << cid<<" " << *(rowactivities + cit)<<" "<< minimalcosarea <<" " << minimalopeningarea<<" "<< looseset << " "<<_area<<" ";
								//bounds.push_back(std::numeric_limits<double>::lowest());
								bounds.push_back(minimalcosarea);
								bounds.push_back(m_area);
							}
							else {
								bounds.push_back(0);
								bounds.push_back(0);
								}
						targets.push_back(cit);
					}else if (cid == schemeid && targetit != targets.end())
						{
						//double minimalcosarea = minimalopeningarea;
						if (looseset)
							{
							minimalcosarea = std::min(minimalcosarea, *(rowactivities + cit) - FMT_DBL_TOLERANCE);
							}
						size_t location = std::distance(targets.begin(), targetit);
						//std::cout << "CID " << cid<<" " << *(rowactivities + cit) << " " << minimalcosarea<<" "<< minimalopeningarea<<" "<<looseset << " " << _area<<" ";
						//bounds[location * 2] = std::numeric_limits<double>::lowest();
						bounds[location * 2] = minimalcosarea;
						bounds[location * 2 + 1] = m_area;
						}
					++constraintid;
					}
			}
		++cid;
		}
	//std::cout << "\n";
	return !openingconstraints.empty();
}

std::vector<double>FMTOperatingAreaScheme::fillPattern(const std::vector<double>& pattern, const int& startat) const
	{
	std::vector<double>values;
	for (size_t repid = 0; repid < repetition; ++repid)
		{
		for (const double& value : pattern)
			{
			values.push_back(value);
			}
		}
	if(startat>=0)//if<0 no pattern... 
	{
		// always zero based ... 
		size_t period = 0;
		//fill partern with 0 before the period where the pattern start
		while(period<static_cast<size_t>(startat))
		{
			values.emplace(values.begin(), 0);
			++period;
		}
	}
	size_t period = 0;
	//For period before the starting period of the optimization fill with 1 
	while (period < startingperiod)
	{
		values[period] = 1;
		++period;
	}
	return values;
	}

void FMTOperatingAreaScheme::closeNoActivity(std::vector<double>& filleduppattern, const size_t& selected, const double* dualsolution) const
	{
	size_t constraintid = 0;
	for (const int& period : schemesperiods.at(selected))
		{
		if (*(dualsolution + openingconstraints.at(selected).at(constraintid)) < FMT_DBL_TOLERANCE)
			{
			filleduppattern[period] = 0;
			}
		++constraintid;
		}
	}

std::vector<double> FMTOperatingAreaScheme::getDualSolution(const double* upperbounds, const double* dualsolution,bool& canbreakneighboring) const
	{
	std::vector<double>filledpattern;
	const size_t patternsize = (this->openingtime + this->returntime);
	bool foundsolution = false;
	size_t solutionid = 0;
	
	if (returntime != maxreturntime)
	{
		filledpattern.resize(patternsize * repetition, 0.0);
		std::vector<double>thepattern(patternsize, 0.0);
		for (size_t opid = 0; opid < this->openingtime;++opid)
		{
			thepattern[1] = 1.0;
		}

		if (getDualSolutionIndex(upperbounds, solutionid))
		{
			foundsolution = true;
			int lastPeriod = 0;
			for (const int& period : schemesperiods.at(solutionid))
			{
				filledpattern[period] = 1;
				lastPeriod = period;
			}
			const bool gotneighbors = !m_neighbors.empty();
			const int maxperiod = getMaxPeriod();
			if (maxperiod > 0)
			{
				int period = lastPeriod + 1;
				const int thepatternsize = static_cast<int>(filledpattern.size());
				while (period < thepatternsize)
				{
					size_t closed = 0;
					while (closed< this->returntime&&
						period < thepatternsize)
					{
						filledpattern[period] = 0;
						++period;
						++closed;
					}
					size_t opened = 0;
					while (opened < this->openingtime&&
						period < thepatternsize)
					{
						if (period > maxperiod)
						{
							filledpattern[period] = 1;
						}else if(gotneighbors)
							{
							canbreakneighboring = true;
							}
						++period;
						++opened;
					}
				}
			}
		}
		size_t period = 0;
		//For period before the starting period of the optimization fill with 1 
		while (period < startingperiod)
		{
			filledpattern[period] = 1;
			++period;
		}
	 } else {
		int startingat = -1;
		std::vector<double>pattern(patternsize, 0.0);
		if (getDualSolutionIndex(upperbounds, solutionid))
		{
			foundsolution = true;
			startingat = *(schemesperiods.at(solutionid).begin());
			for (size_t opid = 0; opid < this->openingtime; ++opid)
			{
				pattern[opid] = 1;
			}
			
		}
		filledpattern= this->fillPattern(pattern, startingat);
	}
	if (foundsolution)
	{
		closeNoActivity(filledpattern, solutionid, dualsolution);
	}
	return filledpattern;
	}

std::vector<double> FMTOperatingAreaScheme::getDualLowerBounds(const double* lowerbounds, const double* upperbounds) const
{
	const size_t patternsize = (this->openingtime + this->returntime);
	size_t solutionid = 0;
	int startingat = -1;
	std::vector<double>filledpattern(patternsize * repetition, 0.0);
	if (getDualSolutionIndex(upperbounds, solutionid))
	{
		size_t periodid = 0;
		for (const int& constraint : openingconstraints.at(solutionid))
			{
			filledpattern[schemesperiods.at(solutionid).at(periodid)-1] = std::max(*(lowerbounds + constraint),0.0);
			++periodid;
			}
	}
	return filledpattern;
}

std::vector<double>FMTOperatingAreaScheme::getPrimalSolution(const double* primalsolution) const //Get the solution into yield
	{
	std::vector<std::string>sources;
	std::vector<double>pattern(openingtime+returntime,0);
	size_t id = 0;
	int startingat = -1;
	for (const int& binary : openingbinaries)
		{
			if (*(primalsolution + binary) > 0) // got a the scheme if non integer going to fill everything!
				{
				startingat = *(schemesperiods.at(id).begin());
				for (size_t opid=0;opid<this->openingtime;++opid)
					{
						pattern[opid] = 1;
					}
				}
			++id;
		}
	return this->fillPattern(pattern, startingat);
	}

std::vector<std::vector<int>> FMTOperatingAreaScheme::schemesToPeriods(const std::vector<std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>>& schemes,
														const Graph::FMTGraph<Graph::FMTVertexProperties,Graph::FMTEdgeProperties>& maingraph) const
	{
	std::vector<std::vector<int>>periods;
	for (const std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>> scheme : schemes)
		{
		std::vector<int>periodids;
		for (const std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>& block : scheme)
			{
			if (!block.empty())
				{
				periodids.push_back(maingraph.getDevelopment(*(block.begin())).getPeriod());
				}
			}
		if (!periodids.empty())
			{
			periods.push_back(periodids);
			}
		
		}
	return periods;
	}

void  FMTOperatingAreaScheme::setReturnTime(const size_t& minimalreturntime, const size_t& maximalreturntime)
	{
	returntime = minimalreturntime;
	maxreturntime = maximalreturntime;
	}

size_t FMTOperatingAreaScheme::getOpeningTime() const
	{
	return openingtime;
	}

size_t FMTOperatingAreaScheme::getMinimalReturnTime() const
	{
	return returntime;
	}

size_t FMTOperatingAreaScheme::getMaximalReturnTime() const
	{
	return maxreturntime;
	}

size_t FMTOperatingAreaScheme::getRepetition() const
	{
	return repetition;
	}

double FMTOperatingAreaScheme::getThreshold() const
	{
	return threshold;
	}


	
bool FMTOperatingAreaScheme::empty() const
	{
	return (schemesperiods.empty() || openingbinaries.empty() || openingconstraints.empty());
	}

const std::vector<int>& FMTOperatingAreaScheme::getOpeningBinaries() const
	{
	return openingbinaries;
	}

FMTOperatingAreaScheme::FMTOperatingAreaScheme(const FMTOperatingArea& oparea,const size_t& lopeningtime, const size_t& lreturntime, const size_t& lmaxreturntime,
	const size_t& lrepetition,const size_t& lgreenup, const size_t& lstartingperiod, double minimalarearatio):FMTOperatingArea(oparea),
	openingconstraints(),
	openingbinaries(),
	maximalschemesconstraint(),
	schemesperiods(),
	openingtime(lopeningtime),returntime(lreturntime),repetition(lrepetition),
	greenup(lgreenup),startingperiod(lstartingperiod), threshold(minimalarearatio), maxreturntime(lmaxreturntime),rejectednodescid(-1)
	{

	}


void FMTOperatingAreaScheme::setConstraints(const std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>& vertices,
	const std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>& totalareavertices,
	const Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>& graph, Models::FMTLpSolver& solver,
	const double* primalsolution,
	const std::vector<int>& actionIDS)
	{
	const std::vector<std::vector<std::vector<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>::FMTvertex_descriptor>>> schemes = this->generateSchemes(vertices);
	schemesperiods = schemesToPeriods(schemes, graph);
	schemestoLP(schemes, vertices, totalareavertices, solver, primalsolution, graph, actionIDS);
	}


std::map<int, std::vector<int>>FMTOperatingAreaScheme::getCommonBinaries(const FMTOperatingAreaScheme& neighbor) const
	{
	std::map<int, std::vector<int>>neighboringscheme;
	const int greenupuse = static_cast<int>(std::max(greenup,neighbor.greenup));
	for (size_t schemeid = 0; schemeid < schemesperiods.size();++schemeid)
		{
		const int scheme_binary = openingbinaries.at(schemeid);
		std::vector<int>neighborsbinaries;
		for (const int& period : schemesperiods.at(schemeid))
			{
			const int upper = (period + greenupuse);
			const int lower = (period - greenupuse);
			for (size_t schemeneighborsid = 0; schemeneighborsid < neighbor.schemesperiods.size(); ++schemeneighborsid)
				{
				const int scheme_neighbors_binary = neighbor.openingbinaries.at(schemeneighborsid);
				if (std::find(neighborsbinaries.begin(), neighborsbinaries.end(), scheme_neighbors_binary)== neighborsbinaries.end())
					{
					for (const int& neighborperiod : neighbor.schemesperiods.at(schemeneighborsid))
						{
						if (neighborperiod >= lower && neighborperiod <= upper)
						{
							neighborsbinaries.push_back(scheme_neighbors_binary);
							break;
						}

						}
					}

				}
			}
		neighboringscheme[scheme_binary] = neighborsbinaries;
		}
	return neighboringscheme;
	}

size_t FMTOperatingAreaScheme::getPrimalSolutionIndex(const double* primalsolution) const
	{
	size_t id = 0;
	for (const int& binary : openingbinaries)
		{
		if (*(primalsolution + binary) > 0)
			{
			return id;
			}
		++id;
		}
	return 0;
	}

bool FMTOperatingAreaScheme::getDualSolutionIndex(const double* upperbound, size_t& locid) const
	{
	std::unordered_set<int>allopenedconstraintids;
	for (const std::vector<int>& constraints : openingconstraints)
	{
		for (const int& constid : constraints)
		{
			if (*(upperbound + constid) > FMT_DBL_TOLERANCE)
			{
				allopenedconstraintids.insert(constid);
			}
		}
	}
	locid = 0;
	for (const std::vector<int>& constraints : openingconstraints)
	{
		size_t localcount = 0;
		bool good = true;
		for (const int& constid : constraints)
		{
			if (*(upperbound + constid) > FMT_DBL_TOLERANCE)
			{
				++localcount;
			}
			else {
				good = false;
				break;
			}
		}
		if (good && allopenedconstraintids.size()== localcount)
		{
			return true;
		}
		++locid;
	}
	return false;


	/*locid = 0;
	for (const std::vector<int>& constraints : openingconstraints)
		{
			bool goodone = false;
			if (!constraints.empty())
				{
				goodone = true;
				for (const int& constid : constraints)
					{
						if (*(upperbound + constid) == 0.0)
						{
							goodone = false;
						}
					}
				}
			if (goodone)
				{
				return true;
				}
		++locid;
		}
	return false;*/
	}

bool FMTOperatingAreaScheme::havePotentialSolution(const double* primalsolution) const
	{
	for (const int& binary : openingbinaries)
		{
			if (*(primalsolution + binary) > FMT_DBL_TOLERANCE)
			{
				return true;
			}
		}
	return false;
	}

bool FMTOperatingAreaScheme::haveActivitySolution(const double* dualsolution) const
{
	for (const std::vector<int>& constraints : openingconstraints)
	{
		for (const int& constid : constraints)
			{
			if (*(dualsolution + constid) > FMT_DBL_TOLERANCE)
				{
				return true;
				}
			}
	}
	return false;
}



std::vector<size_t>FMTOperatingAreaScheme::getPotentialPrimalSchemes(const double* primalsolution, const double* lowerbounds, const double* upperbounds, const std::vector<FMTOperatingAreaScheme>& m_neighbors) const
	{
	std::vector<size_t>potentialindexes;
	if (havePotentialSolution(primalsolution))//Got something more than zero...
		{
		std::vector<int>potentials = this->openingbinaries;
		for (const FMTOperatingAreaScheme& neighbor : m_neighbors)
			{
			if (neighbor.isPrimalBounded(lowerbounds, upperbounds))
				{
				const size_t neighborsolution = neighbor.getPrimalSolutionIndex(primalsolution);
				const std::map<int, std::vector<int>>commons = neighbor.getCommonBinaries(*this);
				for (const int& binary : commons.at(neighbor.openingbinaries.at(neighborsolution)))
					{
					std::vector<int>::iterator binit = std::find(potentials.begin(), potentials.end(), binary);
					if (binit != potentials.end())
						{
						potentials.erase(binit);
						}
					}
				}
			}
		for (const int& binary : potentials)
			{
			std::vector<int>::const_iterator binit = std::find(this->openingbinaries.begin(), this->openingbinaries.end(), binary);
			const double actualvalue = *(primalsolution + binary);
			if (!potentialindexes.empty())
				{
				const size_t oldsize = potentialindexes.size();
				std::vector<size_t>::iterator indexid = potentialindexes.begin();
				while (oldsize == potentialindexes.size())
					{
					if (indexid == potentialindexes.end() || actualvalue > *(primalsolution + openingbinaries.at(*indexid)))
						{
						potentialindexes.insert(indexid, binit - this->openingbinaries.begin());
						}
					++indexid;
					}
			}else if(actualvalue>FMT_DBL_TOLERANCE)
				{
				potentialindexes.push_back(binit - this->openingbinaries.begin());
				}
			}
		}
	return potentialindexes;
	}

double FMTOperatingAreaScheme::getRowsActivitySum(const std::vector<int>& rows, const double* dualsolution) const
	{
	double value = 0;
	for (const int& constid : rows)
		{
		value += *(constid + dualsolution);
		}
	return value;
	}

std::vector<size_t> FMTOperatingAreaScheme::getPotentialDualSchemes(
	const double* dualsolution, 
	const double* upperbound, 
	const std::vector<FMTOperatingAreaScheme>& m_neighbors) const
	{
	std::vector<size_t> potentialIndexes;
	if (haveActivitySolution(dualsolution))//Got something more than zero...
		{
			std::vector<int> potentials = this->openingbinaries;
			for (const FMTOperatingAreaScheme& neighbor : m_neighbors)
			{
				if (neighbor.isDualBounded(upperbound))
				{
					size_t neighborsolution = 0;
					if (neighbor.getDualSolutionIndex(upperbound, neighborsolution))
					{
						const std::map<int, std::vector<int>>commons = neighbor.getCommonBinaries(*this);
						for (const int& binary : commons.at(neighbor.openingbinaries.at(neighborsolution)))
						{
							std::vector<int>::iterator binit = std::find(potentials.begin(), potentials.end(), binary);
							if (binit != potentials.end())
							{
								potentials.erase(binit);
							}
						}
					}
				}
			}
			// On itère sur les différentes cédules potentielles pour un seul COS
			std::vector<double> potentialValues;
			for (const int& binary : potentials)
			{
				size_t indexLocation = _findIndexLocation(binary);
				
				if (isThresholdActivityRows(openingconstraints.at(indexLocation), dualsolution))
				{
					_addPotentialResults(indexLocation, potentialIndexes, potentialValues, dualsolution);
				}
			}

			if (potentialValues.empty() && threshold > 0)
			{
				std::vector<double> tempThresholds;
				std::vector<int> tempBinary;
				for (const int& binary : potentials)
				{
					size_t indexLocation = _findIndexLocation(binary);
				
					if (!isThresholdActivityRows(openingconstraints.at(indexLocation), dualsolution) && threshold > 0)
					{
						tempThresholds.push_back(_maxNearThresholdActivityRows(openingconstraints.at(indexLocation), dualsolution));
						tempBinary.push_back(binary);
					}
				}
				if (!tempThresholds.empty())
				{
					std::vector<double>::iterator it = std::max_element(tempThresholds.begin(), tempThresholds.end()); 
					size_t index = std::distance(tempThresholds.begin(), it);
					size_t indexLocation = _findIndexLocation(tempBinary[index]);

					if (isThresholdActivityRows(openingconstraints.at(indexLocation), dualsolution, tempThresholds[index]))
					{
						_addPotentialResults(indexLocation, potentialIndexes, potentialValues, dualsolution);
						_exhandler->raise(Exception::FMTexc::FMTunreachable_threshold,
							"Initial threshold of " + std::to_string(threshold) +
							". New threshold set at " + std::to_string(tempThresholds[index]) +
							" for " + std::string(getMask()),
							"FMTOperatingAreaScheme::_maxNearThresholdActivityRows",
							__LINE__, __FILE__);
					}
				}
			}

			if (!potentialIndexes.empty())
			{
				std::vector<int> indices(potentialIndexes.size());
				std::iota(indices.begin(), indices.end(), 0);
				std::sort(indices.begin(), indices.end(),
					[&](int A, int B) -> bool {
					return potentialValues[A] > potentialValues[B];
				});
				std::vector<size_t>sortedPotentials;
				sortedPotentials.reserve(potentials.size());
				for (const int& INDEX : indices)
				{
					sortedPotentials.push_back(potentialIndexes[INDEX]);
				}
				potentialIndexes.swap(sortedPotentials);
			}
		}
	return potentialIndexes;
	}


size_t FMTOperatingAreaScheme::boundAllPrimalSchemes(std::vector<int>& targets, std::vector<double>& bounds, double boundvalue) const
{
	if (!openingbinaries.empty())
	{
		for (const int& opbin : openingbinaries)
		{
			bounds.push_back(boundvalue);
			bounds.push_back(boundvalue);
			targets.push_back(opbin);
		}
	}
	return openingbinaries.size();
}

size_t FMTOperatingAreaScheme::boundAllDualSchemes(std::vector<int>& targets, std::vector<double>& bounds) const
{
	size_t unbounded = 0;
	for (const std::vector<int>& constraints : openingconstraints)
	{
		if (!constraints.empty())
		{
			for (const int& cid : constraints)
			{
				if (std::find(targets.begin(), targets.end(), cid) == targets.end())
				{
					targets.push_back(cid);
					bounds.push_back(0);
					bounds.push_back(0);
				}
			}
			++unbounded;
		}
	}
	return unbounded;
}

bool FMTOperatingAreaScheme::isAllPrimalBounded(const double* lowerbounds, const double* upperbounds) const
{
	for (const int& binary : openingbinaries)
	{
		if (*(binary + lowerbounds) != 1.0 || *(binary + upperbounds) != 1.0)
		{
			return false;
		}
	}
	return true;
}

bool FMTOperatingAreaScheme::isAllDualBounded(const double* upperbounds) const
{
	for (const std::vector<int>& constraint : openingconstraints)
	{
		for (const int& constid : constraint)
		{
			if (*(constid + upperbounds) != 0)
			{
				return false;
			}
		}
	}
	return true;
}

double FMTOperatingAreaScheme::getBinariesSum(const double* primalsolution) const
{
	double total = 0;
	for (const int& binary : openingbinaries)
	{
		total += *(primalsolution + binary);
	}
	return total;
}

double FMTOperatingAreaScheme::getActivitySum(const double* dualsolution) const
{
	double total = 0;
	std::vector<int>counted;
	for (const std::vector<int>& constraints : openingconstraints)
	{
		for (const int& constraint : constraints)
		{
		if (std::find(counted.begin(), counted.end(), constraint) == counted.end())
				{
					total += *(dualsolution + constraint);
					counted.push_back(constraint);
				}
		}
	}
	return total;
}

double FMTOperatingAreaScheme::_maxNearThresholdActivityRows(const std::vector<int>& rows, const double* dualsolution) const
{
	double maxValue = 0;
	size_t idWithMaxValue = 0;
	size_t optid = 0;
	while (optid < openingtime && optid < rows.size())
	{
		const double VALUE = *(dualsolution + rows.at(optid));
		if (VALUE > maxValue)
		{
			maxValue = VALUE;
			idWithMaxValue = optid;
		}
		++optid;
	}
	
	double newThreshold = (maxValue / (m_area + FMT_DBL_TOLERANCE));

	return newThreshold;
}

size_t FMTOperatingAreaScheme::_findIndexLocation(int binary) const
{
	std::vector<int>::const_iterator binit = std::find(this->openingbinaries.begin(),
		this->openingbinaries.end(),
		binary);
	return std::distance(this->openingbinaries.begin(), binit);
}

void FMTOperatingAreaScheme::_addPotentialResults(
	size_t indexlocation, 
	std::vector<size_t>& potentialindexes, 
	std::vector<double>& potentialValues,
	const double* dualsolution) const
{
	const double VALUE = getRowsActivitySum(openingconstraints.at(indexlocation), dualsolution);
	if (VALUE > FMT_DBL_TOLERANCE)
	{
		potentialindexes.push_back(indexlocation);
		potentialValues.push_back(VALUE);
	}
}

bool FMTOperatingAreaScheme::_checkDoubleIncludes(const std::vector<std::vector<int>>& p_openingconstraints)
{
	for (const auto& OP_CONSTRAINT : p_openingconstraints)
	{
		size_t numberOfIncludes = 0;
		for (const auto& OP_INCLUDE : p_openingconstraints)
		{
			if (OP_CONSTRAINT != OP_INCLUDE)
			{
				if (std::includes(
					OP_CONSTRAINT.begin(),
					OP_CONSTRAINT.end(),
					OP_INCLUDE.begin(),
					OP_INCLUDE.end())
					)
				{
					++numberOfIncludes;
				}
			}
		}
		if (numberOfIncludes == p_openingconstraints.size() - 1)
		{
			return true;
		}
	}
	return false;
}

bool FMTOperatingAreaScheme::isThresholdActivityRows(const std::vector<int>& rows, const double* dualsolution, double tempThreshold) const
{
	/*for (const int& constraint : rows)
	{
		if (*(dualsolution + constraint) < ((threshold*m_area) - FMT_DBL_TOLERANCE))
		{
			return false;
		}
	}*/

	// Only first period + fix unreachable threshold
	double thresholdValue = (tempThreshold > 0) ? tempThreshold : threshold;
	size_t optid = 0;

	while (optid < openingtime && optid < rows.size())
	{
		if (*(dualsolution + rows.at(optid)) < ((thresholdValue * m_area) - FMT_DBL_TOLERANCE))
		{
			return false;
		}
		++optid;
	}

	return true;
}

bool FMTOperatingAreaScheme::isThresholdActivity(const double* dualsolution) const
{
	for (const std::vector<int>& constraints : openingconstraints)
	{
		if (!isThresholdActivityRows(constraints, dualsolution))
			{
			return false;
			}
	}
	return true;
}


bool FMTOperatingAreaScheme::isPrimalBounded(const double* lowerbounds, const double* upperbounds) const
	{
	for (const int& binary : openingbinaries)
		{
		if (*(binary + lowerbounds) == 1.0 && *(binary + upperbounds) == 1.0)
			{
			return true;
			}
		}
	return false;
	}

bool FMTOperatingAreaScheme::isDualBounded(const double* upperbounds) const
	{
	if (openingconstraints.size()==1)//need to considered it selected because theirs only one choice!
		{
		return true;
		}

	for (const std::vector<int>& constraints : openingconstraints)
		{
			for (const int& constraint : constraints)
			{
				if (*(constraint + upperbounds) == 0)
					{
					return true;
					}
			}
		}

	if (openingconstraints.size() > 1)
	{
		return _checkDoubleIncludes(openingconstraints);
	}

	return false;
	}


FMTOperatingAreaScheme FMTOperatingAreaScheme::presolve(const Core::FMTMask& selectedmask, const std::vector<Core::FMTTheme>& presolvedthemes) const
{
	const FMTOperatingArea presolveoparea = FMTOperatingArea::presolveOperatingArea(selectedmask,presolvedthemes);
	return FMTOperatingAreaScheme(presolveoparea,this->openingtime, this->returntime,this->maxreturntime,this->repetition,this->greenup,this->startingperiod);
}


size_t FMTOperatingAreaScheme::getStartingPeriod() const
	{
	return startingperiod;
	}


bool FMTOperatingAreaScheme::operator == (const FMTOperatingAreaScheme& rhs) const
	{
	return (m_mask == rhs.m_mask);
	}
bool FMTOperatingAreaScheme::operator != (const FMTOperatingAreaScheme& rhs) const
	{
	return (!(*this == rhs));
	}

size_t FMTOperatingAreaScheme::getNumberofscheme() const
	{
		return schemesperiods.size();
	}

size_t FMTOperatingAreaScheme::getNumberofsimplescheme() const
	{
	std::unordered_set<int>allperiods;
	for (const std::vector<int>& schemeperiod : schemesperiods)
		{
		for (const int& period : schemeperiod)
			{
			allperiods.insert(period);
			}
		}
	return allperiods.size() - (openingtime - 1);
	}

const int& FMTOperatingAreaScheme::getRejectedNodesCid() const
{
	return rejectednodescid;
}

const std::vector<std::vector<int>>& FMTOperatingAreaScheme::getOpeningConstraints() const
{
	return openingconstraints;
}

const int& FMTOperatingAreaScheme::getMaximalSchemesConstraint() const 
{
	return maximalschemesconstraint;
}


FMTOperatingAreaSchemeComparator::FMTOperatingAreaSchemeComparator(const Core::FMTMask& lmask):m_mask(lmask)
	{

	}
bool FMTOperatingAreaSchemeComparator::operator()(const FMTOperatingAreaScheme& oparea) const
	{
	return (oparea.getMask() == m_mask);
	}


}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTOperatingAreaScheme)
#endif
