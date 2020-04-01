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

#include "FMToperatingareaheuristic.h"
#include <algorithm>
#include <random>
#include "OsiClpSolverInterface.hpp"
#ifdef  FMTWITHMOSEK
	#include "OsiMskSolverInterface.hpp"
	#include "mosek.h"
#endif
#include "FMTmatrixbuild.h"

namespace Heuristics
{
	void FMToperatingareaheuristic::clearrowcache()
		{
		#ifdef  FMTWITHMOSEK
			if (!useprimal && solvertype == Models::FMTsolverinterface::MOSEK)
				{
				OsiMskSolverInterface* msksolver = dynamic_cast<OsiMskSolverInterface*>(solverinterface.get());
				msksolver->freeCachedRowRim();
				}
		#endif
		}


	void FMToperatingareaheuristic::unboundall(bool atprimal)
		{
		this->clearrowcache();
		std::vector<int>targeteditems;
		std::vector<double>bounds;
		for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
			{
			if (useprimal || atprimal)
				{
				operatingareait->unboundallprimalschemes(targeteditems, bounds);
			}
			else { // dual
				operatingareait->unboundalldualschemes(targeteditems, bounds);
			}
			
			}
			if (useprimal || atprimal)
			{
				solverinterface->setColSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
			}else {
				solverinterface->setRowSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
				}
		}

	void FMToperatingareaheuristic::closeprimalbounds()
		{
		std::vector<int>variables;
		std::vector<double>bounds;
		for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
				{
				operatingareait->boundallprimalschemes(variables,bounds,0);
				}
		solverinterface->setColSetBounds(&variables[0], &variables.back() + 1, &bounds[0]);
		}

	void FMToperatingareaheuristic::setallinteger()
		{
		if (!useprimal)//need to turn back the formulation to primal to make it work ith MIP
			{
				this->clearrowcache();
				const double* rowupper = solverinterface->getRowUpper();
				std::vector<int>rowsitems;
				std::vector<double>rowsbounds;
				std::vector<int>colsitems;
				std::vector<double>colsbounds;
				for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
					operatingareait != operatingareas.end(); ++operatingareait)
					{
					size_t selected = 0;
					if (operatingareait->getdualsolutionindex(rowupper, selected))
						{
						operatingareait->boundprimalscheme(colsitems, colsbounds, selected);
						}
					operatingareait->unboundalldualschemes(rowsitems,rowsbounds);
					}
				//Need to bring it back to initial state!
				for (double& bound : rowsbounds)
					{
					if (bound!= std::numeric_limits<double>::lowest())
						{
						bound = 0;
						}
					}
				solverinterface->setRowSetBounds(&rowsitems[0], &rowsitems.back() + 1, &rowsbounds[0]);
				solverinterface->setColSetBounds(&colsitems[0], &colsitems.back() + 1, &colsbounds[0]);
				this->resolvemodel();
			}
		std::vector<int>integervariables;
		for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
				{
				operatingareait->pushbinaries(integervariables);
				}
		solverinterface->setInteger(&integervariables[0], static_cast<int>(integervariables.size()));
		}

	int FMToperatingareaheuristic::resolvemodel()
		{
		int numberofiterations = 0;
		if (solvertype == Models::FMTsolverinterface::CLP && !useprimal)//clp with dual simplex
			{
			OsiClpSolverInterface* clpsolver = dynamic_cast<OsiClpSolverInterface*>(solverinterface.get());
			ClpSimplex* splexmodel = clpsolver->getModelPtr();
			splexmodel->setPerturbation(-6);
			splexmodel->setSpecialOptions(64 | 128 | 1024 | 2048 | 4096 | 32768 | 262144 | 0x01000000);
			splexmodel->tightenPrimalBounds();
			splexmodel->dual();
			numberofiterations = splexmodel->numberIterations();
		}
		#ifdef  FMTWITHMOSEK
		else if(solvertype == Models::FMTsolverinterface::MOSEK) //Mosek with interior point
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
			MSK_putintparam(task, MSK_IPAR_LOG, 10);
			MSK_putintparam(task, MSK_IPAR_LOG_INTPNT, 4);
			MSKrescodee error = MSK_optimize(task);
			MSK_getintinf(task, MSK_IINF_INTPNT_ITER, &numberofiterations);
			}
		#endif
		else{//default
				solverinterface->resolve();
				numberofiterations = solverinterface->getIterationCount();
				}
		return numberofiterations;
		}

	void FMToperatingareaheuristic::initialsolve()
		{
		if (solverinterface->isProvenOptimal())
			{
			double initialobjectivevalue = solverinterface->getObjValue();
			size_t opareaprocessed = 0;
			std::string problemsolved = "primal";
			if (!useprimal)
				{
				this->unboundall(); //Make sure rhs are right need to be released
				this->closeprimalbounds(); //Need that to get some activities
				this->resolvemodel();
				problemsolved = "dual";
				}
			std::vector<std::vector<FMToperatingarea>::const_iterator> selected;
			do {
				this->clearrowcache();
				selected = this->setdraw();
				const size_t setssize = this->setbounds(selected);
				const int iterations = this->resolvemodel();
				opareaprocessed += selected.size();
				if (!selected.empty())
					{
					int setratio = ((static_cast<double>(opareaprocessed)) / (static_cast<double>(this->operatingareas.size()))) * 100;
					(*_logger) << "Solution generation phase (" + std::to_string(setratio) + "%) took " + std::to_string(iterations) +" iterations on "+ problemsolved << "\n";
					}
				if (!solverinterface->isProvenOptimal())
					{
					userandomness = true; //Switch to random now
					this->unboundall(); //release everything
					if (!useprimal)
						{
						this->closeprimalbounds();
						}
					this->resolvemodel();
					opareaprocessed = 0;
					}
			} while (!selected.empty() && solverinterface->isProvenOptimal());
			if (solverinterface->isProvenOptimal())
				{
				const double newobjective = solverinterface->getObjValue();
				const double dblgap = (100 - (round((newobjective / initialobjectivevalue) * 1000) / 10));
				(*_logger) << "Feasible solution found objective: " + std::to_string(round(newobjective)) + " (" + std::to_string(dblgap) + "%)" << "\n";
				this->clearrowcache();
				}
			}
		}

	void FMToperatingareaheuristic::branchnboundsolve()
		{
		if (solverinterface->isProvenOptimal())
			{
			//In that order it seems to work...
			this->setallinteger();
			solverinterface->branchAndBound();
			this->unboundall(true);
			solverinterface->branchAndBound();
			}
		}


	void FMToperatingareaheuristic::setoperatingareasconstraints(const Graph::FMTgraph& maingraph,
																const Models::FMTmodel& model,
																const Core::FMToutputnode& target)
		{
		Core::FMToutputnode specifictarget(target);
		const std::vector<Core::FMTaction>modelactions=model.getactions();
		const std::unordered_map<size_t, Graph::FMTvertex_descriptor> basedescriptors = maingraph.getperiodverticies(maingraph.getfirstactiveperiod());
		Core::FMToutputnode areatarget(specifictarget);
		Core::FMTmask areamask = areatarget.source.getmask();
		for (const Core::FMTtheme& theme : model.getthemes())
			{
			areamask.set(theme, "?");
			}
		areatarget.source = Core::FMToutputsource(Core::FMTspec(), areamask, Core::FMTotar::inventory);
		const std::vector<const Core::FMTaction*>actions = specifictarget.source.targets(modelactions);
		std::vector<int>actionids;
		for (const Core::FMTaction* actptr : actions)
			{
			actionids.push_back(std::distance(&modelactions[0], actptr));
			}
		const double* primalsolution = solverinterface->getColSolution();
		Models::FMTmatrixbuild matrixbuild;
		matrixbuild.setlastcolindex(solverinterface->getNumCols()-1);
		matrixbuild.setlastrowindex(solverinterface->getNumRows()-1);
		for (std::vector<FMToperatingarea>::iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
			{
			specifictarget.source.setmask(operatingareait->getmask());
			areatarget.source.setmask(operatingareait->getmask());
			std::vector<std::vector<Graph::FMTvertex_descriptor>>descriptors;
			std::vector<Graph::FMTvertex_descriptor>totalareadescriptors;
			for (int period = (maingraph.getfirstactiveperiod()+ operatingareait->getstartingperiod());period < static_cast<int>((maingraph.size()-1));++period)
				{
				if (descriptors.empty())
					{
					totalareadescriptors = maingraph.getnode(model, areatarget, period);
					}
				std::vector<Graph::FMTvertex_descriptor> perioddescriptors;
				if (!totalareadescriptors.empty())
					{
					perioddescriptors = maingraph.getnode(model, specifictarget, period);
					}
				descriptors.push_back(perioddescriptors);
				}
			if (!descriptors.empty())
				{
				operatingareait->setconstraints(descriptors, totalareadescriptors, maingraph,matrixbuild,primalsolution,actionids);
				}
			}
		matrixbuild.synchronize(solverinterface);
		}

	void FMToperatingareaheuristic::setadjacencyconstraints()
		{
		Models::FMTmatrixbuild matrixbuild;
		const std::vector<double>elements(2, 1.0);
		std::vector<int>columns(2, 0);
		int constraintsid = solverinterface->getNumRows();
		for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
			{
			const std::vector<Core::FMTmask>neighbors = operatingareait->getneighbors();
			for (const Core::FMTmask& neighbor : neighbors)
				{
				std::pair<Core::FMTmask, Core::FMTmask>simple(operatingareait->getmask(), neighbor);
				std::pair<Core::FMTmask, Core::FMTmask>reverse(neighbor, operatingareait->getmask());
				if (adjacencyconstraints.find(simple)== adjacencyconstraints.end() &&
					adjacencyconstraints.find(reverse) == adjacencyconstraints.end())
					{
					std::vector<FMToperatingarea>::const_iterator opneighbor = std::find_if(operatingareas.begin(), operatingareas.end(), FMToperatingareacomparator(neighbor));
					std::map<int, std::vector<int>> neighborsbin;
					if (opneighbor!= operatingareas.end())
						{
						neighborsbin  = operatingareait->getcommonbinairies(*opneighbor);
						}
					std::vector<int>constraintindexes;
					for (std::map<int, std::vector<int>>::const_iterator binit = neighborsbin.begin();binit!=neighborsbin.end();++binit)
						{
						for (const int& index : binit->second)
							{
							constraintindexes.push_back(constraintsid);
							++constraintsid;
							columns[0] = binit->first;
							columns[1] = index;
							matrixbuild.addRow(2, &columns[0], &elements[0], 0, 1);
							}
						}
					if (!constraintindexes.empty())
						{
						adjacencyconstraints[simple] = constraintindexes;
						}
					
					}
				}
			}
		matrixbuild.synchronize(solverinterface);
		}

	FMToperatingareaheuristic::FMToperatingareaheuristic(const FMToperatingareaheuristic& rhs) :
		Core::FMTobject(rhs),operatingareas(rhs.operatingareas), adjacencyconstraints(rhs.adjacencyconstraints),
		solverinterface(), generator(rhs.generator),seed(rhs.seed), proportionofset(rhs.proportionofset), 
		userandomness(rhs.userandomness), usingsolvercopy(true),useprimal(false), solvertype(rhs.solvertype)
		{
		solverinterface = Models::FMTserializablematrix().copysolverinterface(rhs.solverinterface, rhs.solvertype, &*(this->_logger));
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
			usingsolvercopy = true;
			useprimal = false;
			solvertype=rhs.solvertype;
			solverinterface = Models::FMTserializablematrix().copysolverinterface(rhs.solverinterface, rhs.solvertype, &*(this->_logger));
			}
		return *this;
		}
	FMToperatingareaheuristic::~FMToperatingareaheuristic()
		{
			//Will need a clean matrix to fit with FMTlpmodel!
			std::vector<int>rowstodelete;
			std::vector<int>columnstodelete;
			for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
					operatingareait != operatingareas.end(); ++operatingareait)
					{
					operatingareait->getressourcestodelete(columnstodelete, rowstodelete);
					}
			for (std::map<std::pair<Core::FMTmask,Core::FMTmask>, std::vector<int>>::const_iterator it = adjacencyconstraints.begin();it!= adjacencyconstraints.end(); it++)
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
				this->resolvemodel();
				}
		operatingareas.clear();
		adjacencyconstraints.clear();
		}

	std::vector<std::vector<FMToperatingarea>::const_iterator> FMToperatingareaheuristic::setdraw()
		{
		std::vector<std::vector<FMToperatingarea>::const_iterator>potentials;
		const double* upperbounds = solverinterface->getColUpper();
		const double* lowerbounds = solverinterface->getColLower();
		const double* primalsolution = solverinterface->getColSolution();
		const double* dualsolution = solverinterface->getRowActivity();
		const double* rhsupper = solverinterface->getRowUpper();
		std::vector<FMToperatingarea>::const_iterator areait = operatingareas.begin();
		while (areait != operatingareas.end())
			{
			if (!areait->empty() && ((useprimal && !areait->isprimalbounded(lowerbounds, upperbounds) && !areait->isallprimalbounded(lowerbounds, upperbounds))||
				(!useprimal && !areait->isdualbounded(rhsupper))))
				{
				//Make sure it's sorted!
				double value = 0;
				if (useprimal)
					{
					value = areait->getbinariessum(primalsolution);
				}else {
					value = areait->getactivitysum(dualsolution);
					}
				if (!potentials.empty())
					{
					std::vector<std::vector<FMToperatingarea>::const_iterator>::iterator vit = potentials.begin();
					size_t oldsize = potentials.size();
					while (potentials.size() == oldsize)
						{
						double potentialvalue = 0;
						if (vit != potentials.end())
							{ 
							if (useprimal)
								{
									potentialvalue = (*vit)->getbinariessum(primalsolution);
								}
								else {
									potentialvalue = (*vit)->getactivitysum(dualsolution);
								}
							}
						if (vit == potentials.end() || value > potentialvalue)
							{
							potentials.insert(vit, areait);
							}
							++vit;
						}
					}else {
						potentials.push_back(areait);
						}
				}
			++areait;
			}
		const size_t maxareatopick = static_cast<size_t>(static_cast<double>(operatingareas.size()) * proportionofset);
		if (userandomness)
			{
			std::shuffle(potentials.begin(), potentials.end(), this->generator);
			}
		std::vector<std::vector<FMToperatingarea>::const_iterator>selected;
		std::vector<std::vector<FMToperatingarea>::const_iterator>::iterator randomit = potentials.begin();
		while ((selected.size() < maxareatopick) && randomit != potentials.end())
			{
			selected.push_back(*randomit);
			++randomit;
			}
		return selected;
		}

	size_t FMToperatingareaheuristic::setbounds(const std::vector<std::vector<FMToperatingarea>::const_iterator>& tobound)
	{
		size_t gotschedule = 0;
		const double* primalsolution = solverinterface->getColSolution();
		const double* dualsolution = solverinterface->getRowActivity();
		const double* lowerprimalbounds = solverinterface->getColLower();
		const double* upperprimalbounds = solverinterface->getColUpper();
		const double* rowupperbound = solverinterface->getRowUpper();
		std::vector<int>targeteditems;
		std::vector<double>bounds;
		for (std::vector<FMToperatingarea>::const_iterator opit : tobound)
		{
			std::vector<FMToperatingarea>allneighbors;
			for (const Core::FMTmask& neighbormask : opit->getneighbors())
			{
				std::vector<FMToperatingarea>::const_iterator opneighbor = std::find_if(operatingareas.begin(), operatingareas.end(), FMToperatingareacomparator(neighbormask));
				if (opneighbor != operatingareas.end())
				{
					allneighbors.push_back(*opneighbor);
				}

			}
			std::vector<size_t>potentialschemes;
			if (useprimal)
			{
				potentialschemes = opit->getpotentialprimalschemes(primalsolution, lowerprimalbounds, upperprimalbounds, allneighbors);
			}
			else {
				potentialschemes = opit->getpotentialdualschemes(dualsolution, rowupperbound, allneighbors);
			}
			if (!potentialschemes.empty())
			{
				if (userandomness)
				{
					std::shuffle(potentialschemes.begin(), potentialschemes.end(), this->generator);
				}
				++gotschedule;
				if (useprimal)
				{
					opit->boundprimalscheme(targeteditems, bounds, *potentialschemes.begin());
				}
				else {
					opit->unbounddualscheme(targeteditems, bounds, *potentialschemes.begin());
				}
			}
			else {
				if (useprimal)
				{
					opit->boundallprimalschemes(targeteditems, bounds);
				}
				else {
					opit->boundalldualschemes(targeteditems, bounds);
				}
			}
		}
		if (useprimal)
		{
			solverinterface->setColSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
		}else {
			solverinterface->setRowSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
			this->clearrowcache();
			}
		return gotschedule;
		}

	std::vector<Core::FMTyieldhandler> FMToperatingareaheuristic::getsolution(const std::string& yldname) const
		{
		std::vector<Core::FMTyieldhandler>allhandlers;
		const double* primalsolution = solverinterface->getColSolution();
		const double* rowupperbound = solverinterface->getRowUpper();
		for (std::vector<FMToperatingarea>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
			{
			std::vector<double>data;
			if (useprimal)
				{
				data=operatingareait->getprimalsolution(primalsolution);
			}else {
				data=operatingareait->getdualsolution(rowupperbound);
				}
			std::vector<std::string>source;
			Core::FMTyieldhandler handler(Core::FMTyldwstype::FMTtimeyld, operatingareait->getmask());
			handler.push_data(yldname,Core::FMTdata(data, Core::FMTyieldparserop::FMTwsnone, source));
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
		Core::FMTobject(),operatingareas(loperatingareas),adjacencyconstraints(),
		solverinterface(nullptr), generator(lseed),seed(lseed), proportionofset(proportionofset),
		userandomness(userandomness), usingsolvercopy(copysolver), useprimal(false), solvertype(lsolvertype)
		{
		if (copysolver)
			{
			solverinterface = Models::FMTserializablematrix().copysolverinterface(initialsolver, solvertype,&*this->_logger);
		}else {
			solverinterface = initialsolver;
			}
		this->setoperatingareasconstraints(maingraph, model, target);
		this->setadjacencyconstraints();
		this->resolvemodel();
		}

	void FMToperatingareaheuristic::setasrandom()
		{
		userandomness = true;
		}

	void FMToperatingareaheuristic::setasprimal()
		{
		useprimal = true;
		}

	void FMToperatingareaheuristic::setgeneratorseed(const size_t& lseed)
		{
		seed = lseed;
		generator.seed(lseed);
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
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMToperatingareaheuristic);
#endif