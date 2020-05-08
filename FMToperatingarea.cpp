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
#include "FMToperatingarea.h"
#include "CoinPackedVector.hpp"
#include "FMTmatrixbuild.h"

namespace Heuristics
{

double FMToperatingarea::getarea(const double* primalsolution, const Graph::FMTgraph& maingraph, const std::vector<Graph::FMTvertex_descriptor>& verticies) const//Get the area of the operating area base on a solution
	{
	double area = 0;
	for (const Graph::FMTvertex_descriptor& descriptor : verticies)
		{
		if (maingraph.periodstart(descriptor))
			{
			area += maingraph.inarea(descriptor, primalsolution);
			}
		}
	return area;
	}

double FMToperatingarea::getarea() const
	{
	return _area;
	}

size_t FMToperatingarea::getbestschemeid(const double* primalsolution) const//Get the best possible scheme looking at the primal solution
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

std::vector<std::vector<std::vector<Graph::FMTvertex_descriptor>>> FMToperatingarea::generateschemes(const std::vector<std::vector<Graph::FMTvertex_descriptor>>& verticies)// Generate unique schemes base on parameters
	{
	std::vector<std::vector<std::vector<Graph::FMTvertex_descriptor>>>schemes;
	for (size_t id = 0 ; id < verticies.size();++id)
		{
		std::vector<std::vector<Graph::FMTvertex_descriptor>> newscheme;
		size_t localid = id;
		bool validscheme = true;
		while (localid< verticies.size())
			{
			size_t opening = 0;
			while (localid < verticies.size() && opening < openingtime && !verticies.at(localid).empty())
				{
				newscheme.push_back(verticies.at(localid));
				++opening;
				++localid;
				}
			if (newscheme.size()< openingtime)//need a complete pattern
				{
				validscheme = false;
				}
			size_t closing = 0;
			while (localid < verticies.size() && closing < returntime)
				{
				++localid;
				++closing;
				}
			}
		if (validscheme)
			{
			schemes.push_back(newscheme);
			validscheme = true;
			}
		}
	return schemes;
	}

	void FMToperatingarea::schemestoLP(const std::vector<std::vector<std::vector<Graph::FMTvertex_descriptor>>>& schemes,
		const std::vector<std::vector<Graph::FMTvertex_descriptor>>& periodics,
		const std::vector<Graph::FMTvertex_descriptor>& totalareaverticies,
		Models::FMTlpsolver& solver,
		const double* primalsolution,
		const Graph::FMTgraph& maingraph, const std::vector<int>& actionIDS)
	{
		//int binaryid = matrixbuild.getlastcolindex()+1;
		int binaryid = solver.getNumCols();
		_area = 0;
		//std::vector<std::vector<Graph::FMTvertex_descriptor>>::const_iterator perit = periodics.begin();
		if (!totalareaverticies.empty())
			{
			_area = this->getarea(primalsolution, maingraph, totalareaverticies);
			}
		std::map<int, std::vector<int>>periodicsblocksvariables;
		std::vector<size_t>selectedschemes;
		size_t schemeid = 0;
		for (const std::vector<std::vector<Graph::FMTvertex_descriptor>>& scheme : schemes)
		{
			bool shemehasactions = false;
			size_t blockid = 0;
			for (const std::vector<Graph::FMTvertex_descriptor>& localblock : scheme)
			{
				if (!localblock.empty())
				{
					const int period = schemesperiods.at(schemeid).at(blockid);
					if (periodicsblocksvariables.find(period) == periodicsblocksvariables.end())
					{
						periodicsblocksvariables[period] = std::vector<int>();
						for (const Graph::FMTvertex_descriptor& descriptor : localblock)
						{
							const std::map<int, int>actions = maingraph.getoutvariables(descriptor);
							if (actions.size() > 1)
							{
								for (const int& action : actionIDS)
								{
									std::map<int, int>::const_iterator actit = actions.find(action);
									if (actit != actions.end())
									{
										periodicsblocksvariables[period].push_back(actit->second);
										shemehasactions = true;
									}
								}
							}
						}
					}
				}
				++blockid;
			}
			if (shemehasactions)
			{
				openingbinaries.push_back(binaryid);
				selectedschemes.push_back(schemeid);
				++binaryid;
			}
			++schemeid;
		}
		//int constraintid = matrixbuild.getlastrowindex()+1;
		int constraintid = solver.getNumRows();
		std::map<int, std::vector<int>>constraintsmap;
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
							elements.push_back(-_area);
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
					//matrixbuild.addRow(static_cast<int>(targetedvariables.size()), &targetedvariables[0], &elements[0], std::numeric_limits<double>::lowest(), 0);
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
				//matrixbuild.addCol(0, nullptr, nullptr, 0, 1);
				solver.addCol(0, nullptr, nullptr, 0, 1);
				}
			//matrixbuild.setlastcolindex(openingbinaries.back());
			maximalschemesconstraint = constraintid;
			std::vector<double>maxelements(openingbinaries.size(), 1.0);
			//matrixbuild.addRow(static_cast<int>(openingbinaries.size()), &openingbinaries[0], &maxelements[0], std::numeric_limits<double>::lowest(), 1);
			//matrixbuild.setlastrowindex(maximalschemesconstraint);
			solver.addRow(static_cast<int>(openingbinaries.size()), &openingbinaries[0], &maxelements[0], std::numeric_limits<double>::lowest(), 1);
			}

	}




void FMToperatingarea::getressourcestodelete(std::vector<int>& colstodelete, std::vector<int>& rowstodelete) const
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

void FMToperatingarea::pushbinaries(std::vector<int>& targets) const 
	{
	targets.insert(targets.end(), openingbinaries.begin(), openingbinaries.end());
	}
size_t FMToperatingarea::unboundallprimalschemes(std::vector<int>& targets, std::vector<double>& bounds) const //Unbound all binairies to 0<=B<=1
	{
	for (const int& bintounbound : openingbinaries)
		{
		targets.push_back(bintounbound);
		bounds.push_back(0);
		bounds.push_back(1);
		}
	return !openingbinaries.empty();
	}

size_t FMToperatingarea::unboundalldualschemes(std::vector<int>& targets, std::vector<double>& bounds) const
	{
	size_t processed = 0;
	for (const std::vector<int>& constraints : openingconstraints)
		{
		if (!constraints.empty())
			{
			for (const int& newconstraint : constraints)
				{
				if (std::find(targets.begin(), targets.end(), newconstraint)== targets.end())
					{
					targets.push_back(newconstraint);
					bounds.push_back(std::numeric_limits<double>::lowest());
					bounds.push_back(_area);
					}
				}
			processed += 1;
			}
		}
	return processed;
	}

bool FMToperatingarea::boundprimalscheme(std::vector<int>& targets, std::vector<double>& bounds, const size_t& schemeid) const //Looking at the primal solution set the best scheme to the solverinterface 1<=B<=1 and check optimality
	{
	targets.push_back(openingbinaries.at(schemeid));
	bounds.push_back(1.0);
	bounds.push_back(1.0);
	return !openingbinaries.empty();
	}

bool FMToperatingarea::unbounddualscheme(std::vector<int>& targets, std::vector<double>& bounds, const size_t& schemeid) const //Looking at the primal solution set the best scheme to the solverinterface 1<=B<=1 and check optimality
{
	size_t cid = 0;
	for (const std::vector<int>& constraints : openingconstraints)
		{
		if (!constraints.empty())
			{
				for (const int& cit : constraints)
					{
					std::vector<int>::iterator targetit = std::find(targets.begin(), targets.end(), cit);
					if (targetit == targets.end())
						{
							if (cid == schemeid)
							{
								bounds.push_back(std::numeric_limits<double>::lowest());
								bounds.push_back(_area);
							}
							else {
								bounds.push_back(0);
								bounds.push_back(0);
								}
						targets.push_back(cit);
					}else if (cid == schemeid && targetit != targets.end())
						{
						size_t location = std::distance(targets.begin(), targetit);
						bounds[location * 2] = std::numeric_limits<double>::lowest();
						bounds[location * 2 + 1] = _area;
						}
					}
			}
		++cid;
		}
	return !openingconstraints.empty();
}

std::vector<double>FMToperatingarea::fillpattern(const std::vector<double>& pattern, const int& startat) const
	{
	std::vector<double>values;
	for (size_t repid = 0; repid < repetition; ++repid)
		{
		for (const double& value : pattern)
			{
			values.push_back(value);
			}
		}
	if (startat>=0)
		{
		for (size_t period = 1; period < static_cast<size_t>(startat); ++period)
		{
			values.emplace(values.begin(), 0);
		}
		values.emplace(values.begin(), 1);
		values.emplace(values.begin(), 0);
		for (size_t vloc = 2; vloc < (static_cast<size_t>(startat) + 2); ++vloc)
		{
			int period = static_cast<int>(vloc - 1);
			if (period < static_cast<int>(startingperiod))
			{
				values[vloc] = 1;
			}
		}
		}
	return values;
	}

std::vector<double> FMToperatingarea::getdualsolution(const double* upperbounds) const
	{
	const size_t patternsize = (this->openingtime + this->returntime);
	std::vector<double>pattern(patternsize,0.0);
	size_t solutionid = 0;
	int startingat = -1;
	if (getdualsolutionindex(upperbounds, solutionid))
		{
		startingat = *(schemesperiods.at(solutionid).begin());
		pattern[0] = 1;
		}
	return this->fillpattern(pattern, startingat);
	}

std::vector<double>FMToperatingarea::getprimalsolution(const double* primalsolution) const //Get the solution into yield
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
				pattern[0] = 1;
				}
			++id;
		}
	return this->fillpattern(pattern, startingat);
	}

std::vector<std::vector<int>> FMToperatingarea::schemestoperiods(const std::vector<std::vector<std::vector<Graph::FMTvertex_descriptor>>>& schemes,
														const Graph::FMTgraph& maingraph) const
	{
	std::vector<std::vector<int>>periods;
	for (const std::vector<std::vector<Graph::FMTvertex_descriptor>> scheme : schemes)
		{
		std::vector<int>periodids;
		for (const std::vector<Graph::FMTvertex_descriptor>& block : scheme)
			{
			if (!block.empty())
				{
				periodids.push_back(maingraph.getdevelopment(*(block.begin())).period);
				}
			}
		if (!periodids.empty())
			{
			periods.push_back(periodids);
			}
		
		}
	return periods;
	}

std::vector<Core::FMTmask>FMToperatingarea::getneighbors() const
	{
	return neighbors;
	}

bool FMToperatingarea::empty() const
	{
	return (schemesperiods.empty() || openingbinaries.empty() || openingconstraints.empty());
	}

const std::vector<int>& FMToperatingarea::getopeningbinaries() const
	{
	return openingbinaries;
	}

FMToperatingarea::FMToperatingarea(const Core::FMTmask& lmask, const size_t& lopeningtime, const size_t& lreturntime,
	const size_t& lrepetition,const size_t& lgreenup, const size_t& lstartingperiod,
	const double& lneihgborsperimeter):
	mask(lmask),
	neighbors(),
	openingconstraints(),
	openingbinaries(),
	maximalschemesconstraint(),
	schemesperiods(),
	openingtime(lopeningtime),returntime(lreturntime),repetition(lrepetition), greenup(lgreenup),startingperiod(lstartingperiod),
	neihgborsperimeter(lneihgborsperimeter),_area()
	{

	}

void FMToperatingarea::setneighbors(const std::vector<Core::FMTmask>& lneighbors)
	{
	neighbors = lneighbors;
	}


void FMToperatingarea::setconstraints(const std::vector<std::vector<Graph::FMTvertex_descriptor>>& verticies,
	const std::vector<Graph::FMTvertex_descriptor>& totalareaverticies,
	const Graph::FMTgraph& graph, Models::FMTlpsolver& solver,
	const double* primalsolution,
	const std::vector<int>& actionIDS)
	{
	const std::vector<std::vector<std::vector<Graph::FMTvertex_descriptor>>> schemes = this->generateschemes(verticies);
	schemesperiods = schemestoperiods(schemes, graph);
	schemestoLP(schemes, verticies, totalareaverticies, solver, primalsolution, graph, actionIDS);
	}


std::map<int, std::vector<int>> FMToperatingarea::getcommonbinairies(const FMToperatingarea& neighbor) const
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

size_t FMToperatingarea::getprimalsolutionindex(const double* primalsolution) const 
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

bool FMToperatingarea::getdualsolutionindex(const double* upperbound, size_t& locid) const
	{
	locid = 0;
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
	return false;
	}

bool FMToperatingarea::havepotentialsolution(const double* primalsolution) const
	{
	for (const int& binary : openingbinaries)
		{
			if (*(primalsolution + binary) > 0)
			{
				return true;
			}
		}
	return false;
	}

bool FMToperatingarea::haveactivitysolution(const double* dualsolution) const
{
	for (const std::vector<int>& constraints : openingconstraints)
	{
		for (const int& constid : constraints)
			{
			if (*(dualsolution + constid) > 0)
				{
				return true;
				}
			}
	}
	return false;
}



std::vector<size_t>FMToperatingarea::getpotentialprimalschemes(const double* primalsolution, const double* lowerbounds, const double* upperbounds, const std::vector<FMToperatingarea>& neighbors) const
	{
	std::vector<size_t>potentialindexes;
	if (havepotentialsolution(primalsolution))//Got something more than zero...
		{
		std::vector<int>potentials = this->openingbinaries;
		for (const FMToperatingarea& neighbor : neighbors)
			{
			if (neighbor.isprimalbounded(lowerbounds, upperbounds))
				{
				const size_t neighborsolution = neighbor.getprimalsolutionindex(primalsolution);
				const std::map<int, std::vector<int>>commons = neighbor.getcommonbinairies(*this);
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
			}else if(actualvalue>0)
				{
				potentialindexes.push_back(binit - this->openingbinaries.begin());
				}
			}
		}
	return potentialindexes;
	}

double FMToperatingarea::getrowsactivitysum(const std::vector<int>& rows, const double* dualsolution) const
	{
	double value = 0;
	for (const int& constid : rows)
		{
		value += *(constid + dualsolution);
		}
	return value;
	}

std::vector<size_t>FMToperatingarea::getpotentialdualschemes(const double* dualsolution, const double* upperbound, const std::vector<FMToperatingarea>& neighbors) const
	{
	std::vector<size_t>potentialindexes;
	if (haveactivitysolution(dualsolution))//Got something more than zero...
		{
			std::vector<int>potentials = this->openingbinaries;
			for (const FMToperatingarea& neighbor : neighbors)
			{
				if (neighbor.isdualbounded(upperbound))
				{
					size_t neighborsolution = 0;
					if (neighbor.getdualsolutionindex(upperbound, neighborsolution))
						{
						const std::map<int, std::vector<int>>commons = neighbor.getcommonbinairies(*this);
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
			for (const int& binary : potentials)
			{
				std::vector<int>::const_iterator binit = std::find(this->openingbinaries.begin(), this->openingbinaries.end(), binary);
				size_t indexlocation = std::distance(this->openingbinaries.begin(), binit);
				double actualvalue = getrowsactivitysum(openingconstraints.at(indexlocation),dualsolution);
				if (!potentialindexes.empty())
				{
					const size_t oldsize = potentialindexes.size();
					std::vector<size_t>::iterator indexid = potentialindexes.begin();
					while (oldsize == potentialindexes.size())
					{
					if (indexid == potentialindexes.end() || 
						actualvalue > getrowsactivitysum(openingconstraints.at(*indexid), dualsolution))
						{
							potentialindexes.insert(indexid, binit - this->openingbinaries.begin());
						}
						++indexid;
					}
				}
				else if (actualvalue > 0)
				{
					potentialindexes.push_back(binit - this->openingbinaries.begin());
				}
			}
		}
	return potentialindexes;
	}


size_t FMToperatingarea::boundallprimalschemes(std::vector<int>& targets, std::vector<double>& bounds, double boundvalue) const
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

size_t FMToperatingarea::boundalldualschemes(std::vector<int>& targets, std::vector<double>& bounds) const
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

bool FMToperatingarea::isallprimalbounded(const double* lowerbounds, const double* upperbounds) const
	{
	for (const int& binary : openingbinaries)
		{
		if (*(binary + lowerbounds)!= 1.0 || *(binary + upperbounds) != 1.0)
			{
			return false;
			}
		}
	return true;
	}

bool FMToperatingarea::isalldualbounded(const double* upperbounds) const
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

double FMToperatingarea::getbinariessum(const double* primalsolution) const
	{
	double total = 0;
	for (const int& binary : openingbinaries)
		{
		total += *(primalsolution + binary);
		}
	return total;
	}

double FMToperatingarea::getactivitysum(const double* dualsolution) const
	{
	double total = 0;
	std::vector<int>counted;
	for (const std::vector<int>& constraints : openingconstraints)
		{
		for (const int& constraint : constraints)
			{
			if (std::find(counted.begin(), counted.end(), constraint)== counted.end())
				{
				total += *(dualsolution + constraint);
				counted.push_back(constraint);
				}
			}
		}
	return total;
	}


bool FMToperatingarea::isprimalbounded(const double* lowerbounds, const double* upperbounds) const
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

bool FMToperatingarea::isdualbounded(const double* upperbounds) const
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
	return false;
	}

double FMToperatingarea::getneihgborsperimeter() const
	{
	return neihgborsperimeter;
	}

Core::FMTmask FMToperatingarea::getmask() const
	{
	return mask;
	}

size_t FMToperatingarea::getstartingperiod() const
	{
	return startingperiod;
	}

bool FMToperatingarea::operator == (const FMToperatingarea& rhs) const
	{
	return (mask == rhs.mask);
	}
bool FMToperatingarea::operator != (const FMToperatingarea& rhs) const
	{
	return (*this != rhs);
	}

FMToperatingareacomparator::FMToperatingareacomparator(const Core::FMTmask& lmask):mask(lmask)
	{

	}
bool FMToperatingareacomparator::operator()(const FMToperatingarea& oparea) const
	{
	return (oparea.getmask() == mask);
	}


}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMToperatingarea);
#endif