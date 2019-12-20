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

#include "FMToperatingarea.h"

namespace Heuristics
{

double FMToperatingarea::getarea(const double* primalsolution, const Graph::FMTgraph& maingraph, const vector<Graph::FMTvertex_descriptor>& verticies) const//Get the area of the operating area base on a solution
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

size_t FMToperatingarea::getbestschemeid(const double* primalsolution) const//Get the best possible scheme looking at the primal solution
	{
	size_t id = 0;
	size_t bestid = 0;
	double bestvalue = 0;
	for (const int& binary : openingbinaries)
		{
		if (*(primalsolution + openingbinaries.at(id)) > bestvalue)
			{
			bestvalue = *(primalsolution + openingbinaries.at(id));
			bestid = id;
			}
		++id;
		}
	return bestid;
	}

vector<vector<vector<Graph::FMTvertex_descriptor>>> FMToperatingarea::generateschemes(const vector<vector<Graph::FMTvertex_descriptor>>& verticies)// Generate unique schemes base on parameters
	{
	vector<vector<vector<Graph::FMTvertex_descriptor>>>schemes;
	for (size_t id = 0 ; id < verticies.size();++id)
		{
		vector<vector<Graph::FMTvertex_descriptor>> newscheme;
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
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "haft generated: " << newscheme.size() << "\n";
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
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "new schemes " << newscheme.size() << "\n";
		if (validscheme)
			{
			schemes.push_back(newscheme);
			validscheme = true;
			}
		
		}
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "generated schesmes size: " << schemes.size() << "\n";
	return schemes;
	}

void FMToperatingarea::schemestoLP(const vector<vector<vector<Graph::FMTvertex_descriptor>>>& schemes,
								const vector<vector<Graph::FMTvertex_descriptor>>& periodics,
								std::shared_ptr<OsiSolverInterface> solverinterface, const Graph::FMTgraph& maingraph, const vector<int>& actionIDS) //Fill opening constraints and opening binairies in the LP and in the OParea
	{
	int binaryid = solverinterface->getNumCols();
	const double* primalsolution = solverinterface->getColSolution();
	_area = 0;
	vector<vector<Graph::FMTvertex_descriptor>>::const_iterator perit = periodics.begin();
	if (!periodics.empty())
		{
		_area = this->getarea(primalsolution, maingraph, *perit);
		}
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "area of " << area << "\n";
	map<int, vector<int>>periodicsblocksvariables;
	vector<size_t>selectedschemes;
	size_t schemeid = 0;
	for (const vector<vector<Graph::FMTvertex_descriptor>>& scheme : schemes)
		{
		//if (scheme.size()==openingtime)
			//{ 
			bool shemehasactions = false;
			size_t blockid = 0;
			for (const vector<Graph::FMTvertex_descriptor>& localblock : scheme)
				{
				if (!localblock.empty())
					{
					int period = schemesperiods.at(schemeid).at(blockid);
					if (periodicsblocksvariables.find(period) == periodicsblocksvariables.end())
						{
						periodicsblocksvariables[period] = vector<int>();
						for (const Graph::FMTvertex_descriptor& descriptor : localblock)
							{
								map<int, int>actions = maingraph.getoutvariables(descriptor);
								if (actions.size() > 1)
								{
									for (const int& action : actionIDS)
									{
										map<int, int>::const_iterator actit = actions.find(action);
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
			//}
		}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got schemes " << periodicsblocksvariables.size() << "\n";
		int constraintid = solverinterface->getNumRows();
		vector<int>targetedvariables;
		vector<double>elements;
		vector<int>rowstarts;
		//openingconstraints=vector<vector<int>>(schemesperiods.size(),vector<int>());
		map<int, vector<int>>constraintsmap;
		for (map<int, vector<int>>::const_iterator periodics = periodicsblocksvariables.begin(); 
			periodics != periodicsblocksvariables.end();++periodics)
			{
			if (!periodics->second.empty())
				{
				rowstarts.push_back(targetedvariables.size());
				targetedvariables.insert(targetedvariables.end(), periodics->second.begin(), periodics->second.end());
				elements.insert(elements.end(), periodics->second.size(), 1.0);
				size_t validscheme = 0;
				for (const size_t& shemeid : selectedschemes)
					{
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "schemeid " << shemeid << "\n";
					if (!schemesperiods.at(shemeid).empty() && 
						std::find(schemesperiods.at(shemeid).begin(), schemesperiods.at(shemeid).end(), periodics->first) != schemesperiods.at(shemeid).end())
						{
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "adding up " << shemeid << "\n";
						targetedvariables.push_back(openingbinaries.at(validscheme));
						elements.push_back(-_area);
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "adding up 2 " << shemeid<<" "<< openingconstraints.size() << "\n";
						if (constraintsmap.find(schemeid) == constraintsmap.end())
							{
							constraintsmap[schemeid] = vector<int>();
							}
						constraintsmap[schemeid].push_back(constraintid);
						//openingconstraints[schemeid].push_back(constraintid);
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "adding up 3 " << shemeid << "\n";
						}
					++validscheme;
					}
				++constraintid;
				}
			}
		openingconstraints.resize(openingbinaries.size());
		size_t tid = 0;
		for (const size_t& shemeid : selectedschemes)
			{
			if (constraintsmap.find(shemeid) != constraintsmap.end())
				{
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got const of size: " << constraintsmap.at(shemeid).size() << "\n";
				openingconstraints[tid] = constraintsmap.at(shemeid);
				}
			++tid;
			}
	if (!elements.empty())
		{
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got opconstraint size " << openingconstraints.size() << "\n";
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "adding up " << elements.size()<< "\n";
		//Add all binary cols
		vector<double>colslower(openingbinaries.size(),0);
		vector<double>colsupper(openingbinaries.size(),1);
		vector<double>colsobj(openingbinaries.size(),0);
		vector<int>column_Starts(openingbinaries.size()+1, 0);
		vector<int>targetrows(openingbinaries.size(), 0);
		vector<double>nelements(openingbinaries.size(), 0.0);
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "adding up cols "<< openingbinaries.size() << "\n";
		solverinterface->addCols(int(openingbinaries.size()), &column_Starts[0], &targetrows[0], &nelements[0], &colslower[0], &colsupper[0], &colsobj[0]);
		//Add all Rows
		vector<double>rowlowers(rowstarts.size(), numeric_limits<double>::lowest());
		vector<double>rowuppers(rowstarts.size(), 0);
		//Also add the maximal constraint
		maximalschemesconstraint = constraintid;
		rowlowers.push_back(numeric_limits<double>::lowest());//Can decide to do no scheme = no harvest
		rowuppers.push_back(1);
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "targeted variable size "<< targetedvariables.size() << "\n";
		rowstarts.push_back(static_cast<int>(targetedvariables.size()));
		for (const int& binary : openingbinaries)
			{
			targetedvariables.push_back(binary);
			elements.push_back(1);
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "opening binaries size1 " << rowlowers.size() << "\n";
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "opening binaries size2 " << rowstarts.size() << "\n";
		/*for (int rowsi : rowstarts)
			{
			Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "starts at " << rowsi << "\n";
			}
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "opening binaries size3 " << targetedvariables.size() << "\n";
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "opening binaries size4 " << elements.size() << "\n";
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "opening binaries size5 " << rowuppers.size() << "\n";
		Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "adding up rows" << "\n";*/
		rowstarts.push_back(static_cast<int>(targetedvariables.size()));
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "before row " << solverinterface->getNumRows() << "\n";
		solverinterface->addRows(int(rowlowers.size()), &rowstarts[0], &targetedvariables[0], &elements[0], &rowlowers[0], &rowuppers[0]);
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "added " << rowlowers.size() << "\n";
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "after row " << solverinterface->getNumRows() << "\n";
		//solverinterface->resolve();
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "IS IT OPTIMAL??? " << solverinterface->isProvenOptimal() << "\n";
		}
	}


void FMToperatingarea::getressourcestodelete(vector<int>& colstodelete, vector<int>& rowstodelete) const
	{
	colstodelete.insert(colstodelete.end(), openingbinaries.begin(), openingbinaries.end());
	for (const vector<int>& blockconstraint : openingconstraints)
		{
		rowstodelete.insert(rowstodelete.end(), blockconstraint.begin(), blockconstraint.end());
		}
	if (!openingconstraints.empty())
		{
		rowstodelete.push_back(maximalschemesconstraint);
		}
	}

size_t FMToperatingarea::binarize(std::shared_ptr<OsiSolverInterface> solverinterface) const //Set all opening binairies to integer variable to get ready for branch and bound
	{
	solverinterface->setInteger(&openingbinaries[0], int(openingbinaries.size()));
	return openingbinaries.size();
	}
size_t FMToperatingarea::unboundallprimalschemes(std::shared_ptr<OsiSolverInterface> solverinterface) const //Unbound all binairies to 0<=B<=1
	{
	vector<double>allbounds;
	for (const int& bintounbound : openingbinaries)
		{
		allbounds.push_back(0);
		allbounds.push_back(1);
		}
	solverinterface->setColSetBounds(&openingbinaries[0], &openingbinaries.back() + 1, &allbounds[0]);
	return !allbounds.empty();
	}

size_t FMToperatingarea::unboundalldualschemes(std::shared_ptr<OsiSolverInterface> solverinterface) const
	{
	for (const vector<int>& constraints : openingconstraints)
		{
		if (!constraints.empty())
			{
			vector<double>bounds(constraints.size(), _area);
			solverinterface->setRowSetBounds(&constraints[0], &constraints.back() + 1, &bounds[0]);
			}
		}
	}

bool FMToperatingarea::boundprimalscheme(std::shared_ptr<OsiSolverInterface> solverinterface, const size_t& schemeid) const //Looking at the primal solution set the best scheme to the solverinterface 1<=B<=1 and check optimality
	{
	solverinterface->setColBounds(openingbinaries.at(schemeid), 1.0, 1.0);
	return !openingbinaries.empty();
	}

bool FMToperatingarea::unbounddualscheme(std::shared_ptr<OsiSolverInterface> solverinterface, const size_t& schemeid) const //Looking at the primal solution set the best scheme to the solverinterface 1<=B<=1 and check optimality
{
	vector<double>bounds(openingconstraints.at(schemeid).size(), _area);
	solverinterface->setRowSetBounds(&openingconstraints.at(schemeid)[0], &openingconstraints.at(schemeid).back()+1,&bounds[0]);
	return !openingconstraints.empty();
}

vector<double>FMToperatingarea::fillpattern(const vector<double>& pattern, const int& startat) const
	{
	vector<double>values;
	for (size_t repid = 0; repid < repetition; ++repid)
		{
			values.insert(values.end(), pattern.begin(), pattern.end());
		}
	for (size_t period = 1; period < startat; ++period)
		{
			values.emplace(values.begin(), 0);
		}
	values.emplace(values.begin(), 1);
	values.emplace(values.begin(), 0);
	for (size_t vloc = 2; vloc < (startat + 2); ++vloc)
		{
			int period = vloc - 1;
			if (period < startingperiod)
			{
				values[vloc] = 1;
			}
		}
	return values;
	}

vector<double> FMToperatingarea::getdualsolution(const double* upperbounds) const
	{
	vector<string>sources;
	vector<double>pattern(openingtime + returntime, 0);
	size_t id = 0;
	int startingat = *(schemesperiods.at(id).begin());
	size_t solutionid = getdualsolutionindex(upperbounds);
	for (const int& period : schemesperiods.at(solutionid))
		{
		pattern[(period - startingat)] = 1;
		}
	return fillpattern(pattern, startingat);
	}

vector<double>FMToperatingarea::getprimalsolution(const double* primalsolution) const //Get the solution into yield
	{
	vector<string>sources;
	vector<double>pattern(openingtime+returntime,0);
	size_t id = 0;
	int startingat = 0;
	for (const int& binary : openingbinaries)
		{
			if (*(primalsolution + binary) > 0) // got a the scheme if non integer going to fill everything!
				{
				startingat = *(schemesperiods.at(id).begin());
				for (const int& period : schemesperiods.at(id))
					{
					pattern[(period- startingat)] = 1;
					}
				}
			++id;
		}
	return fillpattern(pattern, startingat);
	}

vector<vector<int>> FMToperatingarea::schemestoperiods(const vector<vector<vector<Graph::FMTvertex_descriptor>>>& schemes,
														const Graph::FMTgraph& maingraph) const
	{
	vector<vector<int>>periods;
	for (const vector<vector<Graph::FMTvertex_descriptor>> scheme : schemes)
		{
		vector<int>periodids;
		for (const vector<Graph::FMTvertex_descriptor>& block : scheme)
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

vector<FMTmask>FMToperatingarea::getneighbors() const
	{
	return neighbors;
	}

bool FMToperatingarea::empty() const
	{
	return (schemesperiods.empty() || openingbinaries.empty() || openingconstraints.empty());
	}

const vector<int>& FMToperatingarea::getopeningbinaries() const
	{
	return openingbinaries;
	}

FMToperatingarea::FMToperatingarea(const FMTmask& lmask, const size_t& lopeningtime, const size_t& lreturntime,
	const size_t& lrepetition,const size_t& lgreenup, const size_t& lstartingperiod,
	const double& lneihgborsperimeter):
	mask(lmask),
	neighbors(),
	openingconstraints(),
	openingbinaries(),
	maximalschemesconstraint(),
	schemesperiods(),
	openingtime(lopeningtime),returntime(lreturntime),repetition(lrepetition),startingperiod(lstartingperiod),
	greenup(lgreenup), neihgborsperimeter(lneihgborsperimeter),_area()
	{

	}

void FMToperatingarea::setneighbors(const vector<FMTmask>& lneighbors)
	{
	neighbors = lneighbors;
	}

void FMToperatingarea::setconstraints(const vector<vector<Graph::FMTvertex_descriptor>>& verticies,
	const Graph::FMTgraph& graph, std::shared_ptr<OsiSolverInterface> solverinterface,
	const vector<int>& actionIDS)
	{
	vector<vector<vector<Graph::FMTvertex_descriptor>>> schemes = this->generateschemes(verticies);
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got verticies " << schemes.size() << "\n";
	schemesperiods = schemestoperiods(schemes, graph);
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got schemes " << schemesperiods.size() << "\n";
	schemestoLP(schemes, verticies, solverinterface, graph, actionIDS);
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "to lp done schemes " << "\n";
	}


map<int, vector<int>> FMToperatingarea::getcommonbinairies(const FMToperatingarea& neighbor) const
	{
	map<int, vector<int>>neighboringscheme;
	int greenupuse = static_cast<int>(max(greenup,neighbor.greenup));
	for (size_t schemeid = 0; schemeid < schemesperiods.size();++schemeid)
		{
		int scheme_binary = openingbinaries.at(schemeid);
		vector<int>neighborsbinaries;
		for (const int& period : schemesperiods.at(schemeid))
			{
			int upper = (period + greenupuse);
			int lower = (period - greenupuse);
			for (size_t schemeneighborsid = 0; schemeneighborsid < neighbor.schemesperiods.size(); ++schemeneighborsid)
				{
				int scheme_neighbors_binary = neighbor.openingbinaries.at(schemeneighborsid);
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

size_t FMToperatingarea::getdualsolutionindex(const double* upperbound) const
	{
	size_t locid = 0;
	for (const vector<int>& constraints : openingconstraints)
		{
			size_t countop = 0;
			for (const int& constid : constraints)
				{
					if (*(upperbound + constid) == _area)
					{
						countop += 1;
					}
				}
			if (!constraints.empty() && constraints.size() == countop)
				{
				return locid;
				}
		++locid;
		}
	return 0;
	}

bool FMToperatingarea::havebinarysolution(const double* primalsolution) const
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
	for (const vector<int>& constraints : openingconstraints)
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



vector<size_t>FMToperatingarea::getpotentialprimalschemes(const double* primalsolution, const double* lowerbounds, const double* upperbounds, const vector<FMToperatingarea>& neighbors) const
	{
	vector<size_t>potentialindexes;
	if (havebinarysolution(primalsolution))//Got something more than zero...
		{
		vector<int>potentials = this->openingbinaries;
		for (const FMToperatingarea& neighbor : neighbors)
			{
			if (neighbor.isprimalbounded(lowerbounds, upperbounds))
				{
				const size_t neighborsolution = neighbor.getprimalsolutionindex(primalsolution);
				const map<int, vector<int>>commons = neighbor.getcommonbinairies(*this);
				for (const int& binary : commons.at(neighbor.openingbinaries.at(neighborsolution)))
					{
					vector<int>::iterator binit = std::find(potentials.begin(), potentials.end(), binary);
					if (binit != potentials.end())
						{
						potentials.erase(binit);
						}
					}
				}
			}
		for (const int& binary : potentials)
			{
			vector<int>::const_iterator binit = std::find(this->openingbinaries.begin(), this->openingbinaries.end(), binary);
			double actualvalue = *(primalsolution + binary);
			if (!potentialindexes.empty())
				{
				const size_t oldsize = potentialindexes.size();
				vector<size_t>::iterator indexid = potentialindexes.begin();
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

double FMToperatingarea::getrowsactivitysum(const vector<int>& rows, const double* dualsolution) const
	{
	double value = 0;
	for (const int& constid : rows)
		{
		value += *(constid + dualsolution);
		}
	return value;
	}

vector<size_t>FMToperatingarea::getpotentialdualschemes(const double* dualsolution, const double* upperbound, const vector<FMToperatingarea>& neighbors) const
	{
	vector<size_t>potentialindexes;
	if (haveactivitysolution(dualsolution))//Got something more than zero...
		{
			Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got activity "  << "\n";
			vector<int>potentials = this->openingbinaries;
			for (const FMToperatingarea& neighbor : neighbors)
			{
				if (neighbor.isdualbounded(upperbound))
				{
					const size_t neighborsolution = neighbor.getdualsolutionindex(upperbound);
					const map<int, vector<int>>commons = neighbor.getcommonbinairies(*this);
					for (const int& binary : commons.at(neighbor.openingbinaries.at(neighborsolution)))
					{
						vector<int>::iterator binit = std::find(potentials.begin(), potentials.end(), binary);
						if (binit != potentials.end())
						{
							potentials.erase(binit);
						}
					}
				}
			}
			for (const int& binary : potentials)
			{
				vector<int>::const_iterator binit = std::find(this->openingbinaries.begin(), this->openingbinaries.end(), binary);
				size_t indexlocation = std::distance(this->openingbinaries.begin(), binit);
				double actualvalue = getrowsactivitysum(openingconstraints.at(indexlocation),dualsolution);
				if (!potentialindexes.empty())
				{
					const size_t oldsize = potentialindexes.size();
					vector<size_t>::iterator indexid = potentialindexes.begin();
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


size_t FMToperatingarea::boundallprimalschemes(std::shared_ptr<OsiSolverInterface> solverinterface) const
	{
	vector<double>bounds(openingbinaries.size(),1.0);
	solverinterface->setColSetBounds(&(*openingbinaries.begin()), &(*(openingbinaries.end()-1)), &bounds[0]);
	return openingbinaries.size();
	}

size_t FMToperatingarea::boundalldualschemes(std::shared_ptr<OsiSolverInterface> solverinterface) const
	{
	size_t unbounded = 0;
	for (const vector<int>& constraints : openingconstraints)
		{
			if (!constraints.empty())
			{
				vector<double>bounds(constraints.size(), 0);
				solverinterface->setRowSetBounds(&constraints[0], &constraints.back() + 1, &bounds[0]);
				unbounded += constraints.size();
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
	for (const vector<int>& constraint : openingconstraints)
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
	vector<int>counted;
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << string(mask) << "\n";
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << openingbinaries.empty() << "\n";
	for (const vector<int>& constraints : openingconstraints)
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
	for (const vector<int>& constraints : openingconstraints)
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

FMTmask FMToperatingarea::getmask() const
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

FMToperatingareacomparator::FMToperatingareacomparator(const FMTmask& lmask):mask(lmask)
	{

	}
bool FMToperatingareacomparator::operator()(const FMToperatingarea& oparea) const
	{
	return (oparea.getmask() == mask);
	}


}
