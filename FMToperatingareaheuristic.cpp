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


#include "FMToperatingareaheuristic.h"
#include <algorithm>
#include <random>

namespace Heuristics
{
	void FMToperatingareaheuristic::unboundall()
		{
		for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
			{
			operatingareait->unboundallschemes(solverinterface);
			}
		}

	void FMToperatingareaheuristic::setallinteger()
		{
		for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
			{
			operatingareait->binarize(solverinterface);
			}
		}

	void FMToperatingareaheuristic::initialsolve()
		{
		if (solverinterface->isProvenOptimal())
			{
			vector<std::vector<FMToperatingarea>::const_iterator> selected;
			do {
				selected = this->setdraw();
				size_t setssize = this->setbounds(selected);
				solverinterface->resolve();
				if (solverinterface->isProvenPrimalInfeasible() || 
					solverinterface->isProvenDualInfeasible())
					{
					userandomness = true; //Switch to random now
					this->unboundall(); //release everything
					solverinterface->resolve();
					}
			} while (!selected.empty() && solverinterface->isProvenOptimal());
			}
		}

	void FMToperatingareaheuristic::branchnboundsolve()
		{
		if (solverinterface->isProvenOptimal())
			{
			this->unboundall();
			this->setallinteger();
			solverinterface->branchAndBound();
			}
		}

	void FMToperatingareaheuristic::setoperatingareasconstraints(const Graph::FMTgraph& maingraph,
																const Models::FMTmodel& model,
																const Core::FMToutputnode& target)
		{
		Core::FMToutputnode specifictarget(target);
		const vector<FMTaction>modelactions=model.getactions();
		vector<const Core::FMTaction*>actions = specifictarget.source.targets(model.getactions(), model.getactionaggregates());
		vector<int>actionids;
		for (const Core::FMTaction* actptr : actions)
			{
			actionids.push_back(std::distance(&modelactions[0], actptr));
			}
		for (std::vector<FMToperatingarea>::iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
			{
			specifictarget.source.setmask(operatingareait->getmask());
			vector<vector<Graph::FMTvertex_descriptor>>descriptors;
			for (int period = maingraph.getfirstactiveperiod();period < maingraph.size();++period)
				{
				descriptors.push_back(maingraph.getnode(model, specifictarget, period));
				}
			if (!descriptors.empty())
				{
				operatingareait->setconstraints(descriptors, maingraph, solverinterface, actionids);
				}
			}
		}

	void FMToperatingareaheuristic::setadjacencyconstraints()
		{
		vector<int>columns;
		vector<int>rowstarts;
		int constraintsid = solverinterface->getNumRows();
		for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
			{
			const vector<FMTmask>neighbors = operatingareait->getneighbors();
			for (const FMTmask& neighbor : neighbors)
				{
				std::pair<FMTmask, FMTmask>simple(operatingareait->getmask(), neighbor);
				std::pair<FMTmask, FMTmask>reverse(neighbor, operatingareait->getmask());
				if (adjacencyconstraints.find(simple)== adjacencyconstraints.end() &&
					adjacencyconstraints.find(reverse) == adjacencyconstraints.end())
					{
					std::vector<FMToperatingarea>::const_iterator opneighbor = std::find_if(operatingareas.begin(), operatingareas.end(), FMToperatingareacomparator(neighbor));
					map<int, vector<int>> neighborsbin;
					if (opneighbor!= operatingareas.end())
						{
						neighborsbin  = operatingareait->getcommonbinairies(*opneighbor);
						}
					vector<int>constraintindexes;
					for (map<int,vector<int>>::const_iterator binit = neighborsbin.begin();binit!=neighborsbin.end();++binit)
						{
						for (const int& index : binit->second)
							{
							rowstarts.push_back(columns.size());
							columns.push_back(binit->first);
							columns.push_back(index);
							constraintindexes.push_back(constraintsid);
							++constraintsid;
							}
						}
					adjacencyconstraints[simple] = constraintindexes;
					}
				}
			}

		if (!rowstarts.empty())
			{
			vector<double>rowlbs(rowstarts.size(), 0);
			vector<double>rowubs(rowstarts.size(), 1);
			vector<double>elements(rowstarts.size(), 1);
			solverinterface->addRows(int(rowstarts.size()), &rowstarts[0], &columns[0], &elements[0], &rowlbs[0], &rowubs[0]);
			}
		}

	FMToperatingareaheuristic::FMToperatingareaheuristic(const FMToperatingareaheuristic& rhs) :
		FMTobject(rhs),operatingareas(rhs.operatingareas), adjacencyconstraints(rhs.adjacencyconstraints),
		solverinterface(rhs.solverinterface), generator(rhs.generator),seed(rhs.seed), proportionofset(rhs.proportionofset), 
		userandomness(rhs.userandomness), solvertype(rhs.solvertype)
		{

		}
	FMToperatingareaheuristic& FMToperatingareaheuristic::operator = (const FMToperatingareaheuristic& rhs)
		{
		if (this!=&rhs)
			{
			FMTobject::operator = (rhs);
			operatingareas=rhs.operatingareas;
			adjacencyconstraints=rhs.adjacencyconstraints;
			generator=rhs.generator;
			seed = rhs.seed;
			proportionofset = rhs.proportionofset;
			userandomness = rhs.userandomness;
			solvertype=rhs.solvertype;
			solverinterface = rhs.solverinterface;
			}
		return *this;
		}
	FMToperatingareaheuristic::~FMToperatingareaheuristic()
		{
			//Will need a clean matrix to fit with FMTlpmodel!
			vector<int>rowstodelete;
			vector<int>columnstodelete;
			for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
					operatingareait != operatingareas.end(); ++operatingareait)
					{
					operatingareait->getressourcestodelete(columnstodelete, rowstodelete);
					}
			for (std::map<std::pair<FMTmask, FMTmask>, vector<int>>::const_iterator it = adjacencyconstraints.begin();it!= adjacencyconstraints.end(); it++)
					{
					rowstodelete.insert(rowstodelete.end(), it->second.begin(), it->second.end());
					}
			if (!rowstodelete.empty())
				{
				solverinterface->deleteRows(rowstodelete.size(), &rowstodelete[0]);
				}
			if (!columnstodelete.empty())
				{ 
				solverinterface->deleteCols(columnstodelete.size(), &columnstodelete[0]);
				}
			if (!rowstodelete.empty() || !columnstodelete.empty())
				{
				solverinterface->resolve();
				}
		operatingareas.clear();
		adjacencyconstraints.clear();
		}

	vector<std::vector<FMToperatingarea>::const_iterator> FMToperatingareaheuristic::setdraw()
		{
		vector<std::vector<FMToperatingarea>::const_iterator>potentials;
		const double* upperbounds = solverinterface->getColUpper();
		const double* lowerbounds = solverinterface->getColLower();
		const double* primalsolution = solverinterface->getColSolution();
		std::vector<FMToperatingarea>::const_iterator areait = operatingareas.begin();
		while (areait != operatingareas.end())
			{
			if (!areait->isbounded(lowerbounds, upperbounds) && !areait->isallbounded(lowerbounds, upperbounds))
				{
				//Make sure it's sorted!
				double value = areait->getbinariessum(primalsolution);
				if (!potentials.empty())
					{
					vector<std::vector<FMToperatingarea>::const_iterator>::iterator vit = potentials.begin();
					size_t oldsize = potentials.size();
					while (potentials.size() == oldsize)
						{
							if (value > (*vit)->getbinariessum(primalsolution))
							{
								potentials.insert(vit, areait);
							}
							++vit;
						}
					}else {
						potentials.push_back(areait);
						}
				}
			}
		const size_t maxareatopick = static_cast<size_t>(double(operatingareas.size()) * proportionofset);
		if (userandomness)
			{
			std::shuffle(potentials.begin(), potentials.end(), this->generator);
			}
		vector<std::vector<FMToperatingarea>::const_iterator>selected;
		vector<std::vector<FMToperatingarea>::const_iterator>::iterator randomit = potentials.begin();
		while ((selected.size() < maxareatopick) && randomit != potentials.end())
			{
			selected.push_back(*randomit);
			++randomit;
			}
		return selected;
		}

	size_t FMToperatingareaheuristic::setbounds(const vector<std::vector<FMToperatingarea>::const_iterator>& tobound)
		{
		size_t gotschedule = 0;
		const double* primalsolution = solverinterface->getColSolution();
		for (std::vector<FMToperatingarea>::const_iterator opit : tobound)
			{
			vector<FMToperatingarea>allneighbors;
			for (const FMTmask& neighbormask : opit->getneighbors())
				{
				std::vector<FMToperatingarea>::const_iterator opneighbor = std::find_if(operatingareas.begin(), operatingareas.end(), FMToperatingareacomparator(neighbormask));
				if (opneighbor != operatingareas.end())
					{
					allneighbors.push_back(*opneighbor);
					}
				
				}
			vector<size_t>potentialschemes = opit->getpotentialschemes(primalsolution,allneighbors);
			if (!potentialschemes.empty())
				{
				if (userandomness)
					{
					std::shuffle(potentialschemes.begin(), potentialschemes.end(), this->generator);
					}
				++gotschedule;
				opit->boundscheme(solverinterface, *potentialschemes.begin());
			}else {
				opit->boundallschemes(solverinterface);
				}
			}
		return gotschedule;
		}

	std::vector<FMTyieldhandler> FMToperatingareaheuristic::getsolution(const string& yldname) const
		{
		std::vector<FMTyieldhandler>allhandlers;
		const double* primalsolution = solverinterface->getColSolution();
		for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
			{
			vector<double>data = operatingareait->getsolution(primalsolution);
			vector<string>source;
			FMTyieldhandler handler(FMTyldwstype::FMTtimeyld, operatingareait->getmask());
			handler.push_base(1);
			handler.push_data(yldname,FMTdata(data, FMTyieldparserop::FMTwsnone, source));
			allhandlers.push_back(handler);
			}
		return allhandlers;
		}

	FMToperatingareaheuristic::FMToperatingareaheuristic(const std::vector<FMToperatingarea>& loperatingareas,
		const Graph::FMTgraph& maingraph,
		const Models::FMTmodel& model,
		const Core::FMToutputnode& target,
		std::shared_ptr<OsiSolverInterface> initialsolver,
		const Models::FMTsolverinterface& lsolvertype, size_t lseed,
		double proportionofset, bool userandomness, bool copysolver):
		FMTobject(),operatingareas(loperatingareas),adjacencyconstraints(),
		solverinterface(nullptr), generator(seed), proportionofset(proportionofset),
		userandomness(userandomness), solvertype(lsolvertype)
		{
		if (copysolver)
			{
			solverinterface = FMTserializablematrix().copysolverinterface(initialsolver, solvertype,&*this->_logger);
		}else {
			solverinterface = initialsolver;
			}
		this->setoperatingareasconstraints(maingraph, model, target);
		this->setadjacencyconstraints();
		//dont forget to pass in exeception and logger!
		}

	bool FMToperatingareaheuristic::isfeasible() const
		{
		return solverinterface->isProvenOptimal();
		}
	double FMToperatingareaheuristic::getobjective() const
		{
		return solverinterface->getObjValue();
		}

}