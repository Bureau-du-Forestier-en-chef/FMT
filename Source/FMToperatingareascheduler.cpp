/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#include "FMToperatingareascheduler.h"
#include <algorithm>
#include <random>
//#include "FMTmatrixbuild.h"
#include "FMTtimeyieldhandler.h"
#include "FMTdata.h"
#include "FMTgraph.hpp"
#include "FMTmodel.h"
#include "FMToutputnode.h"
#include "FMTtimeyieldhandler.h"
#include "FMToutput.h"
#include <numeric>

namespace Heuristics
{
	void FMToperatingareascheduler::clearRowCache()
		{
		if (!useprimal)
			{
			FMTlpsolver::clearRowCache();
			}
		}


	void FMToperatingareascheduler::unboundAll(bool atprimal)
		{
		try {
			this->clearRowCache();
			std::vector<int>targeteditems;
			std::vector<double>bounds;
			for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
				operatingareait != operatingareas.end(); ++operatingareait)
				{
				if (useprimal || atprimal)
					{
					operatingareait->unboundAllPrimalSchemes(targeteditems, bounds);
				}
				else { // dual
					operatingareait->unboundAllDualSchemes(targeteditems, bounds);
				}

				}

				if (useprimal || atprimal)
				{
					this->setColSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
				}else {
					this->setRowSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
					}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::unboundall", __LINE__, __FILE__);
			}
		}

	void FMToperatingareascheduler::unbound(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& tounbound, bool atprimal)
		{
		try {
			this->clearRowCache();
			std::vector<int>targeteditems;
			std::vector<double>bounds;
			for (std::vector<FMToperatingareascheme>::const_iterator operatingareait : tounbound)
				{
				if (useprimal || atprimal)
					{
					operatingareait->unboundAllPrimalSchemes(targeteditems, bounds);
				}
				else { // dual
					operatingareait->unboundAllDualSchemes(targeteditems, bounds);
				}

				}

				if (useprimal || atprimal)
				{
					this->setColSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
				}else {
					this->setRowSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
					}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::unbound",__LINE__, __FILE__);
			}
		}


	void FMToperatingareascheduler::closePrimalBounds()
		{
        try{
            std::vector<int>variables;
            std::vector<double>bounds;
            for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
                operatingareait != operatingareas.end(); ++operatingareait)
                    {
                    operatingareait->boundAllPrimalSchemes(variables,bounds,0);
                    }
            this->setColSetBounds(&variables[0], &variables.back() + 1, &bounds[0]);
        }catch (...)
            {
			_exhandler->printexceptions("", "FMToperatingareascheduler::closeprimalbound",__LINE__, __FILE__);
            }
		}

	void FMToperatingareascheduler::setAllInteger()
		{
        try{
		if (!useprimal)//need to turn back the formulation to primal to make it work ith MIP
			{
				this->clearRowCache();
				const double* rowupper = this->getRowUpper();
				std::vector<int>rowsitems;
				std::vector<double>rowsbounds;
				std::vector<int>colsitems;
				std::vector<double>colsbounds;
				for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
					operatingareait != operatingareas.end(); ++operatingareait)
					{
					size_t selected = 0;
					if (operatingareait->getDualSolutionIndex(rowupper, selected))
						{
						operatingareait->boundPrimalScheme(colsitems, colsbounds, selected);
						}
					operatingareait->unboundAllDualSchemes(rowsitems,rowsbounds);
					}
				//Need to bring it back to initial state!
				for (double& bound : rowsbounds)
					{
					if (bound!= std::numeric_limits<double>::lowest())
						{
						bound = 0;
						}
					}
				this->setRowSetBounds(&rowsitems[0], &rowsitems.back() + 1, &rowsbounds[0]);
				this->setColSetBounds(&colsitems[0], &colsitems.back() + 1, &colsbounds[0]);
				this->stockResolve();
			}
		std::vector<int>integervariables;
		for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
				{
				operatingareait->pushBinaries(integervariables);
				}
		this->setInteger(&integervariables[0], static_cast<int>(integervariables.size()));
        }catch(...)
            {
            _exhandler->raisefromcatch("","FMToperatingareascheduler::setallinteger",__LINE__, __FILE__);
            }
		}

	int FMToperatingareascheduler::resolveModel()
		{
		try {
			if (!useprimal || solvertype != Models::FMTsolverinterface::CLP)
			{
				FMTlpsolver::resolve();
			}
			else {
				this->stockResolve();
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::resolvemodel", __LINE__, __FILE__);
			}
		return FMTlpsolver::getIterationCount();
		}

	bool FMToperatingareascheduler::empty() const
		{
		return operatingareas.empty();
		}

	void FMToperatingareascheduler::setForDualInitialSolve()
	{
		try {
			this->unboundAll(); //Make sure rhs are right need to be released
			this->closePrimalBounds(); //Need that to get some activities
			this->resolveModel();
		}catch (...)
			{
			_exhandler->printexceptions("Thread : " + getThreadId() + " ", "FMToperatingareascheduler::setfordualiniti__LINE__e", __LINE__, __FILE__);
			}

	}


	int FMToperatingareascheduler::initialMoveSolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected)
	{
		int iterations = 0;
		try {
			this->setBounds(selected);
			iterations = this->resolveModel();
		}catch (...)
		{
			_exhandler->printexceptions("Thread : " + getThreadId() + " ", "FMToperatingareascheduler::initialmov__LINE__ion", __LINE__, __FILE__);
		}
		return iterations;
	}

	void FMToperatingareascheduler::resetBaseInitialSolution()
	{
		try {
			this->unboundAll(); //release everything
			if (!useprimal)
			{
				this->closePrimalBounds();
			}
			this->resolveModel();
		}
		catch (...)
		{
			_exhandler->printexceptions("Thread : " + getThreadId() + " ", "FMToperatingareascheduler::resetbaseinit__LINE__ution", __LINE__, __FILE__);
		}

	}

	bool FMToperatingareascheduler::initialSolve()
		{
		bool foundsolution = false;
		try {
			if (this->isProvenOptimal())
			{
				const double initialobjectivevalue = this->getObjValue();
				size_t opareaprocessed = 0;
				std::string problemsolved = "primal";
				if (!useprimal)
				{
					setForDualInitialSolve();
					problemsolved = "dual";
				}
				std::vector<std::vector<FMToperatingareascheme>::const_iterator> selected;
				do {
					this->clearRowCache();
					selected = this->setDraw();
					const int iterations = initialMoveSolution(selected);
					opareaprocessed += selected.size();
					if (!selected.empty())
					{
						int setratio = static_cast<int>(((static_cast<double>(opareaprocessed)) / (static_cast<double>(this->operatingareas.size()))) * 100);
						(*_logger) << "Thread : " + getThreadId() + " Solution generation phase (" + std::to_string(setratio) + "%) took " + std::to_string(iterations) + " iterations on " + problemsolved + " formulation" << "\n";
					}
				} while (!selected.empty());
				if (this->isProvenOptimal())
				{
					foundsolution = true;
					const double newobjective = this->getObjValue();
					const std::string relativevalue = std::to_string(static_cast<int>(std::abs(initialobjectivevalue - newobjective) * 100 / initialobjectivevalue));
					(*_logger) << "Thread : " + getThreadId() + " Feasible solution found objective: " + std::to_string(newobjective) + " (" + relativevalue + "%) " << "\n";
					this->clearRowCache();
				}
				else {
					if (!userandomness)
					{
						_exhandler->raise(Exception::FMTexc::FMTschemefailed,
							"Thread : " + getThreadId() + " FMToperatingareascheduler failed initialsolve switching to random for next try.",
							"FMToperatingareascheduler:__LINE__alsolve", __LINE__, __FILE__);
						userandomness = true; //Switch to random now

					}
					else {
						_exhandler->raise(Exception::FMTexc::FMTschemefailed,
							"Thread : " + getThreadId() + " FMToperatingareascheduler initialsolve at random failed, trying another scheme.",
							"FMToperatingareaschedule__LINE__tialsolve", __LINE__, __FILE__);
					}
					resetBaseInitialSolution();
				}
			}else{
				{
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Thread : " + getThreadId() + " Model is not optimal at the beginning of initialsolve",
														"FMToperatingareasched__LINE__intialsolve", __LINE__, __FILE__);

				}
			}
		}catch (...)
		{
			_exhandler->printexceptions("Thread : " + getThreadId()+" ", "FMToperatingareasche__LINE__:initialsolve", __LINE__, __FILE__);
		}
		return foundsolution;
		}

	bool FMToperatingareascheduler::branchNBoundSolve()
		{
        try{
			if (this->isProvenOptimal())
			{
				//In that order it seems to work...
				this->setAllInteger();
				this->branchAndBound();
			}
			this->mipParameters();
			this->unboundAll(true);
			this->branchAndBound();
        	}catch(...)
            	{
        		_exhandler->printexceptions("", "FMToperatingareaschedul__LINE__anchnboundsolve", __LINE__, __FILE__);
            	}
        useprimal=true;
		return this->isProvenOptimal();
		}

	void FMToperatingareascheduler::resetBaseGreedySolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected,
															const std::vector<int>& oldschemeid,
															const std::vector<int>& oldconstraints,
															const std::vector<double>& oldbounds)
	{
		try {
			this->unbound(selected);
			std::vector<int>targeteditems;
			std::vector<double>bounds;
			size_t opat = 0;
			for (const auto& opit : selected)
			{
				const int schemeid = oldschemeid.at(opat);
				if (schemeid >= 0)
				{
					if (!useprimal)
					{
						targeteditems = oldconstraints;
						bounds = oldbounds;
					}
					else {
						getBounds(opit, targeteditems, bounds, false, static_cast<size_t>(schemeid));
					}
				}
				else {
					getBounds(opit, targeteditems, bounds, true);
				}
				++opat;
			}
			if (useprimal)
			{
				this->setColSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
			}
			else {
				this->setRowSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
				this->clearRowCache();
			}
			this->resolveModel();
		}catch (...)
		{
			_exhandler->printexceptions("Thread : " + getThreadId() +
				" Selected size of " + std::to_string(selected.size()) +
				" tabou size of " + std::to_string(oldschemeid.size()) +
				" Constraints size of " + std::to_string(oldconstraints.size()) +
				" bounds size of " + std::to_string(oldbounds.size()), "FMToperatingareascheduler::__LINE__asegreedysolution", __LINE__, __FILE__);
		}

	}


	bool FMToperatingareascheduler::releaseGreedySolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected,
		std::vector<int>& oldconstraints,
		std::vector<double>& oldbounds)
	{
		bool optimalsolution = false;
		try {
			if (!useprimal)
			{
				const double* rowlowers = this->getRowLower();
				const double* rowuppers = this->getRowUpper();
				for (const std::vector<FMToperatingareascheme>::const_iterator& cit : selected)
				{
					cit->fillBoundsNVariables(rowlowers, rowuppers, oldconstraints, oldbounds);
				}
			}

			this->unbound(selected);
			this->resolveModel();
			optimalsolution = this->isProvenOptimal();
		}catch (...)
		{
			_exhandler->printexceptions("Thread : " + getThreadId() +
				" Selected size of "+ std::to_string(selected.size())+
				" Constraints size of "+ std::to_string(oldconstraints.size()) +
				" bounds size of " + std::to_string(oldbounds.size()) , "FMToperatingareaschedul__LINE__leasegreedysolution", __LINE__, __FILE__);
		}
		return optimalsolution;
	}

	bool FMToperatingareascheduler::greedyMoveSolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected,
		const std::vector<int>& oldschemeid)
	{
		bool optimalsolution = false;
		try {
			this->setBounds(selected, oldschemeid, false);
			this->resolveModel();
			optimalsolution = this->isProvenOptimal();
		}catch (...)
			{
			_exhandler->printexceptions("Thread : " + getThreadId() +
				" Selected size of " + std::to_string(selected.size()) +
				" tabou size of " + std::to_string(oldschemeid.size()), "FMToperatingareasc__LINE__r::greedymovesolution", __LINE__, __FILE__);
			}
		return optimalsolution;
	}

	bool FMToperatingareascheduler::greedyPass(const double& initsol, const unsigned int& iteration)
	{
		try{
			if (this->isProvenOptimal())
			{
				const double sens = this->getObjSense();
				const double initialobjectivevalue = this->getObjValue();
				std::string problemsolved = "primal";
				if (!useprimal)
				{
					problemsolved = "dual";
					this->closePrimalBounds();
				}
				std::vector<std::vector<FMToperatingareascheme>::const_iterator> opareas;
				opareas.reserve(operatingareas.size());
				for (std::vector<FMToperatingareascheme>::const_iterator areait = operatingareas.begin() ; areait != operatingareas.end() ; ++areait)
				{
					opareas.push_back(areait);
				}
				this->clearRowCache();
				if(!userandomness)
				{
					//Switch to random for next draw
					userandomness = true;
				}
				const std::vector<std::vector<FMToperatingareascheme>::const_iterator> selected = draw(opareas);
				const std::vector<int> oldschemeid = getSolutionIndexes(selected);
				std::vector<int>oldconstraints;
				std::vector<double>oldbounds;
				bool gotoptimalsolution = false;
				if (releaseGreedySolution(selected,oldconstraints,oldbounds))
					{
					gotoptimalsolution = greedyMoveSolution(selected, oldschemeid);
					}
				const double newobjective = this->getObjValue();
				if (gotoptimalsolution && ((newobjective*sens < initialobjectivevalue*sens)))
				{
					const std::string relativevalue = std::to_string(static_cast<int>(std::abs(initsol - newobjective) * 100 / initsol));
					(*_logger) << "Thread : " + getThreadId() + " Better solution found objective: " + std::to_string(newobjective) + " (" + relativevalue + "%). "+std::to_string(iteration)+" iterations left." << "\n";
					this->clearRowCache();
				}else{
					if (iteration%10 == 0 && iteration>0)
						{
						const std::string bestgap = std::to_string(static_cast<int>(std::abs(initsol - initialobjectivevalue) * 100 / initsol));
						(*_logger) << "Thread : " + getThreadId() +" ("+bestgap+"%) "+std::to_string(iteration) + " iterations left..." << "\n";
						}
					resetBaseGreedySolution(selected,oldschemeid,oldconstraints,oldbounds);
				}
			}
		}catch (...)
		{
			_exhandler->printexceptions("Thread : " + getThreadId(), "FMToperatingareascheduler::greedypass", __LINE__, __FILE__);
		}
		return this->isProvenOptimal();
	}

	std::vector<int> FMToperatingareascheduler::getSolutionIndexes(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& opareaits) const
	{
	std::vector<int> indexes;
	try
		{
		const double* primalsolution = this->getColSolution();
		const double* rhsupper = this->getRowUpper();
		const double* upperbounds = this->getColUpper();
		const double* lowerbounds = this->getColLower();
		for (const auto& opareait : opareaits)
			{
				if (!useprimal)
				{
					size_t schemesid;
					if (opareait->getDualSolutionIndex(rhsupper,schemesid))
					{
						indexes.push_back(static_cast<int>(schemesid));
					}
					else{
						indexes.push_back(-1);
					}
				}
				else
				{
					if(opareait->isPrimalBounded(lowerbounds, upperbounds))
					{
						indexes.push_back(static_cast<int>(opareait->getPrimalSolutionIndex(primalsolution)));
					}else{
						indexes.push_back(-1);
					}
				}
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("","FMToperatingar__LINE__duler::getsolutionindexes", __LINE__, __FILE__);
		}
		return indexes;
	}

	bool FMToperatingareascheduler::gotRejectedNodes() const
	{
		try {
			for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
				operatingareait != operatingareas.end(); ++operatingareait)
			{
				if (operatingareait->getRejectedNodesCid() > 0)
				{
					return true;
				}
			}

		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMToperati__LINE__scheduler::gotrejectednodes", __LINE__, __FILE__);
		}
		return false;
	}

	bool FMToperatingareascheduler::completeInitialSolution()
	{
		try{
			if (!useprimal)//set it in the right form remove it from initialSolve....
			{
				//this->unboundAll(); //Make sure rhs are right need to be released
				//this->closePrimalBounds(); //Need that to get some activities
				const double* initialcolsolution = getColSolution();
				const double* initialrowsolution = getRowPrice();
				std::vector<double>newcolsolution(initialcolsolution, initialcolsolution + getNumCols() + 1);
				std::vector<double>newrowsolution(initialrowsolution, initialrowsolution + getNumRows() + 1);
				for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
					operatingareait != operatingareas.end(); ++operatingareait)
				{
					if (operatingareait->getRejectedNodesCid()>0)
					{
						return false;
					}
					for (const std::vector<int>& scheme : operatingareait->getOpeningConstraints())
					{
						for (const int& contraintindex : scheme)
						{
							newrowsolution[contraintindex] = 0.0;
						}
					}
					if (operatingareait->getMaximalSchemesConstraint()>0)
					{
						newrowsolution[operatingareait->getMaximalSchemesConstraint()] = 0.0;
					}
					
					for (const int& binary : operatingareait->getOpeningBinaries())
					{
						newcolsolution[binary] = 0.0;
					}
				}
				for (std::map<std::pair<Core::FMTmask, Core::FMTmask>, std::vector<int>>::const_iterator adid = adjacencyconstraints.begin(); adid != adjacencyconstraints.end(); adid++)
				{
					for (const int& constraintindex : adid->second)
					{
						newrowsolution[constraintindex] = 0.0; 
					}
				}
				this->setColSolution(&newcolsolution[0]);
				this->setRowPrice(&newrowsolution[0]);
				return true;
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMToperatingare__LINE__uler::completeinitialsolution", __LINE__, __FILE__);
		}
		return false;
	}


	void FMToperatingareascheduler::setOperatingAreasConstraints(const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& maingraph,
																const Models::FMTmodel& model,
																const Core::FMToutputnode& target)
		{
		try {
			Core::FMToutputnode specifictarget(target);
			const std::vector<Core::FMTaction>modelactions = model.getactions();
			//const std::unordered_map<size_t, Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> basedescriptors = maingraph.getperiodverticies(maingraph.getFirstActivePeriod());
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
				actionids.push_back(static_cast<int>(std::distance(&modelactions[0], actptr)));
			}
			const double* primalsolution = this->getColSolution();
			for (std::vector<FMToperatingareascheme>::iterator operatingareait = operatingareas.begin();
				operatingareait != operatingareas.end(); ++operatingareait)
			{
				specifictarget.source.setMask(operatingareait->getmask());
				areatarget.source.setMask(operatingareait->getmask());
				std::vector<std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>>descriptors;
				std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>totalareadescriptors;
				for (int period = static_cast<int>((maingraph.getFirstActivePeriod() + operatingareait->getStartingPeriod())); period < static_cast<int>((maingraph.size() - 1)); ++period)
				{
					if (descriptors.empty())
					{
						totalareadescriptors = maingraph.getnode(model, areatarget, period);
					}
					std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> perioddescriptors;
					if (!totalareadescriptors.empty())
					{
						perioddescriptors = maingraph.getnode(model, specifictarget, period);
					}
					descriptors.push_back(perioddescriptors);
				}
				if (!descriptors.empty())
				{
					bool choiceOnDescriptors = false;
					for (const auto& DESCRIPTORS : descriptors)
					{
						for (const auto& DESCRIPTOR : DESCRIPTORS)
						{
							if (!maingraph.getoutactions(DESCRIPTOR).empty())
								{
								choiceOnDescriptors = true;
								break;
								}
						}
						if (choiceOnDescriptors)
						{
							break;
						}
					}
					if (!choiceOnDescriptors)
						{
						_exhandler->raise(Exception::FMTexc::FMTEmptyOA,
							"No actions found in graph for " +
							std::string(operatingareait->getmask()),
							"FMToperatingareasc__LINE__r::setoperatingareasconstraints", __LINE__, __FILE__);
						}
					operatingareait->setconstraints(descriptors, totalareadescriptors, maingraph, *this, primalsolution, actionids);
				}else{
					_exhandler->raise(Exception::FMTexc::FMTignore,
											"No nodes found in graph for "+
											std::string(operatingareait->getmask()),
											"FMToperatingarea__LINE__ler::setoperatingareasconstraints", __LINE__, __FILE__);
				}
				if (operatingareait->getarea()==0)
					{
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"area of operating area "+
						std::string(operatingareait->getmask())+" is null",
						"FMToperatingar__LINE__duler::setoperatingareasconstraints", __LINE__, __FILE__);
					}else if (operatingareait->getOpeningBinaries().empty())
						{
						_exhandler->raise(Exception::FMTexc::FMTignore,
							std::string(operatingareait->getmask()) + " not operable",
							"FMToperating__LINE__heduler::setoperatingareasconstraints", __LINE__, __FILE__);
						}
			}
			this->synchronize();
			/*if (!this->stockResolve()){
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
													"Infeasible model",
													"FMToperati__LINE__scheduler::setOperatingAreasConstraints", __LINE__, __FILE__);

					}*/
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTopera__LINE__eascheduler::setoperatingareasconstraints", __LINE__, __FILE__);
			}
		}

	bool FMToperatingareascheduler::setAdjacencyConstraints()
		{
		bool rowadded = false;
		try {
		//Models::FMTmatrixbuild matrixbuild;
		const std::vector<double>elements(2, 1.0);
		std::vector<int>columns(2, 0);
		int constraintsid = this->getNumRows();
		for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
			{
			const std::vector<Core::FMTmask>neighbors = operatingareait->getNeighbors();
			for (const Core::FMTmask& neighbor : neighbors)
				{
				std::pair<Core::FMTmask, Core::FMTmask>simple(operatingareait->getmask(), neighbor);
				std::pair<Core::FMTmask, Core::FMTmask>reverse(neighbor, operatingareait->getmask());
				if (adjacencyconstraints.find(simple)== adjacencyconstraints.end() &&
					adjacencyconstraints.find(reverse) == adjacencyconstraints.end())
					{
					std::vector<FMToperatingareascheme>::const_iterator opneighbor = std::find_if(operatingareas.begin(), operatingareas.end(), FMToperatingareaschemecomparator(neighbor));
					std::map<int, std::vector<int>> neighborsbin;
					if (opneighbor!= operatingareas.end())
						{
						neighborsbin  = operatingareait->getCommonBinairies(*opneighbor);
						}
					std::vector<int>constraintindexes;
					for (std::map<int, std::vector<int>>::const_iterator binit = neighborsbin.begin();binit!=neighborsbin.end();++binit)
						{
						for (const int& index : binit->second)
							{
							constraintindexes.push_back(constraintsid);
							columns[0] = binit->first;
							columns[1] = index;
							this->addRow(2, &columns[0], &elements[0], 0, 1);
							this->setRowName("adj_"+std::string(operatingareait->getmask())+"_"+std::string(neighbor)+"_"+std::to_string(binit->first)+"_"+std::to_string(index),constraintsid);
							rowadded = true;
							//matrixbuild.addRow(2, &columns[0], &elements[0], 0, 1);
							++constraintsid;
							}
						}
					if (!constraintindexes.empty())
						{
						adjacencyconstraints[simple] = constraintindexes;
						}

					}
				}
			}
		this->synchronize();
		/*if (!this->stockResolve()){
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
										"Infeasible model",
										"F__LINE__atingareascheduler::setAdjacencyConstraints", __LINE__, __FILE__);

		}*/
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::setadjacencyconstraints", __LINE__, __FILE__);
			}
		return rowadded;
		}

	FMToperatingareascheduler::FMToperatingareascheduler(const FMToperatingareascheduler& rhs) :
		FMTlpheuristic(rhs),operatingareas(rhs.operatingareas), adjacencyconstraints(rhs.adjacencyconstraints),
		proportionofset(rhs.proportionofset),
		userandomness(rhs.userandomness),useprimal(rhs.useprimal)
		{
		
		
		}

	FMToperatingareascheduler& FMToperatingareascheduler::operator = (const FMToperatingareascheduler& rhs)
		{
		if (this!=&rhs)
			{
			FMTlpheuristic::operator=(rhs);
			operatingareas=rhs.operatingareas;
			adjacencyconstraints=rhs.adjacencyconstraints;
			proportionofset = rhs.proportionofset;
			userandomness = rhs.userandomness;
			useprimal = rhs.useprimal;
			}
		return *this;
		}
	FMToperatingareascheduler::~FMToperatingareascheduler()
		{
		try {
		    if (!usingsolvercopy&&canUpdateSource())
                {
                  //Will need a clean matrix to fit with FMTlpmodel!
                std::vector<int>rowstodelete;
                std::vector<int>columnstodelete;
                for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
                    operatingareait != operatingareas.end(); ++operatingareait)
                {
                    operatingareait->getRessourcesToDelete(columnstodelete, rowstodelete);
                }
                for (std::map<std::pair<Core::FMTmask, Core::FMTmask>, std::vector<int>>::const_iterator it = adjacencyconstraints.begin(); it != adjacencyconstraints.end(); it++)
                {
                    rowstodelete.insert(rowstodelete.end(), it->second.begin(), it->second.end());
                }
                if (!rowstodelete.empty())
                {
                    this->deleteRows(static_cast<int>(rowstodelete.size()), &rowstodelete[0]);
                }
                if (!columnstodelete.empty())
                {
                    this->deleteCols(static_cast<int>(columnstodelete.size()), &columnstodelete[0]);
                }
                if (!rowstodelete.empty() || !columnstodelete.empty())
                {
                    this->resolveModel();
                }
                operatingareas.clear();
                adjacencyconstraints.clear();
                }
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::~", __LINE__, __FILE__);
			}
		}

	std::vector<std::vector<FMToperatingareascheme>::const_iterator> FMToperatingareascheduler::setDraw()
		{
        std::vector<std::vector<FMToperatingareascheme>::const_iterator>selected;
        try{
            std::vector<std::vector<FMToperatingareascheme>::const_iterator>potentials;
			std::vector<double>potentialValues;
            const double* upperbounds = this->getColUpper();
            const double* lowerbounds = this->getColLower();
            const double* primalsolution = this->getColSolution();
            const double* dualsolution = this->getRowActivity();
            const double* rhsupper = this->getRowUpper();
            std::vector<FMToperatingareascheme>::const_iterator areait = operatingareas.begin();
            while (areait != operatingareas.end())
                {
                if (!areait->empty() 
					&& ((useprimal 
					&& !areait->isPrimalBounded(lowerbounds, upperbounds) 
					&& !areait->isAllPrimalBounded(lowerbounds, upperbounds))
						||
                    (!useprimal && !areait->isDualBounded(rhsupper)   
					&& areait->isThresholdActivity(rhsupper))))
                    {
                    //Make sure it's sorted!
                    double value = 0;
                    if (useprimal)
                        {
                        value = areait->getBinariesSum(primalsolution);
                    }else {
                        value = areait->getActivitySum(dualsolution);
                        }
					potentials.push_back(areait);
					potentialValues.push_back(value);
					/*if (!potentials.empty() && !userandomness)
                        {
                        std::vector<std::vector<FMToperatingareascheme>::const_iterator>::iterator vit = potentials.begin();
						size_t oldsize = potentials.size();
                        while (potentials.size() == oldsize)
                            {
                            double potentialvalue = 0;
                            if (vit != potentials.end())
                                {
                                if (useprimal)
                                    {
                                        potentialvalue = (*vit)->getBinariesSum(primalsolution);
                                    }else {
                                        potentialvalue = (*vit)->getActivitySum(dualsolution);
                                    }
                                }
                            if (vit == potentials.end()  || (value - potentialvalue)>FMT_DBL_TOLERANCE)
                                {
                                potentials.insert(vit, areait);
                                }
							++vit;
                            }
                        }else {
                            potentials.push_back(areait);
                            }*/
                    }
                ++areait;
                }
			if (!userandomness)
			{
				std::vector<int> indices(potentials.size());
				std::iota(indices.begin(), indices.end(), 0);
				std::sort(indices.begin(), indices.end(),
					[&](int A, int B) -> bool {
					return potentialValues[A]>potentialValues[B];
				});
				std::vector<std::vector<FMToperatingareascheme>::const_iterator>sortedPotentials;
				sortedPotentials.reserve(potentials.size());
				for (const int& INDEX : indices)
					{
					sortedPotentials.push_back(potentials[INDEX]);
					}
				potentials.swap(sortedPotentials);

			}
            if (proportionofset==0)
                {
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,"Proportion of selected ope__LINE__ area equal 0","FMToperatingareascheduler::setdraw",__LINE__,__FILE__);
                }
            selected = draw(potentials);
            /*const size_t maxareatopick = static_cast<size_t>(std::ceil(static_cast<double>(operatingareas.size()) * proportionofset));
            if (userandomness)
                {
                std::shuffle(potentials.begin(), potentials.end(), this->generator);
                }
			std::vector<std::vector<FMToperatingareascheme>::const_iterator>::iterator randomit = potentials.begin();
			while ((selected.size() < maxareatopick) && randomit != potentials.end())
				{
				selected.push_back(*randomit);
				++randomit;
				}*/
        }catch(...)
            {
                _exhandler->raisefromcatch("","FMToperatingareascheduler::setdraw", __LINE__, __FILE__);
            }
		return selected;
		}

	std::vector<std::vector<FMToperatingareascheme>::const_iterator> FMToperatingareascheduler::draw(std::vector<std::vector<FMToperatingareascheme>::const_iterator>& oparea)
	{
		std::vector<std::vector<FMToperatingareascheme>::const_iterator> selected;
		try{
			//Calculate the number of operating areas to pick base on the proportion of set... 5% of 50 operating areas == 3 because of the ceil
			const size_t maxopareatopick = static_cast<size_t>(std::ceil(static_cast<double>(operatingareas.size()) * proportionofset));
			if (userandomness)
				{
				std::shuffle(oparea.begin(), oparea.end(), this->generator);
				}
			std::vector<std::vector<FMToperatingareascheme>::const_iterator>::iterator randomit = oparea.begin();
			while ((selected.size() < maxopareatopick) && randomit != oparea.end())
				{
				selected.push_back(*randomit);
				++randomit;
				}
		}catch(...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::draw", __LINE__, __FILE__);
			}
		return selected;
	}

	size_t FMToperatingareascheduler::selectScheme(std::vector<size_t>& potentialschemes, const int& schemetoskip) const
	{
		try{
			for (const size_t& potentialscheme : potentialschemes)
			{
				if (potentialscheme!=static_cast<size_t>(schemetoskip))
				{
					return potentialscheme;
				}
			}
		}catch(...)
		{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::selectscheme", __LINE__, __FILE__);
		}
		//Return the first in the case that there is only one scheme and its the same as the one to skip
		return *potentialschemes.begin();
	}

	bool FMToperatingareascheduler::getBounds(const std::vector<FMToperatingareascheme>::const_iterator& operatingareaiterator,std::vector<int>& targeteditems,std::vector<double>& bounds, const bool& boundall, const size_t& schemeid, bool keeploose) const
	{
		try{
			if (!boundall)
			{
				if (useprimal)
				{
					operatingareaiterator->boundPrimalScheme(targeteditems, bounds, schemeid);
				}
				else {
					const double* dualsolution = this->getRowActivity();
					bool emptyness = operatingareaiterator->unboundDualScheme(dualsolution,targeteditems, bounds,schemeid, keeploose);
				}
				return true;
			}
			else {
				if (useprimal)
				{
					operatingareaiterator->boundAllPrimalSchemes(targeteditems, bounds);
				}
				else {
					operatingareaiterator->boundAllDualSchemes(targeteditems, bounds);
				}
				return false;
			}
		}catch(...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::getbounds", __LINE__, __FILE__);
			}
		return false;
	}

	size_t FMToperatingareascheduler::setBounds(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& tobound,const std::vector<int>& schemestoskip,bool keeploose)
		{
			size_t gotschedule = 0;
			try{
			const double* primalsolution = this->getColSolution();
			const double* dualsolution = this->getRowActivity();
			const double* lowerprimalbounds = this->getColLower();
			const double* upperprimalbounds = this->getColUpper();
			const double* rowupperbound = this->getRowUpper();
			std::vector<int>ltargeteditems;
			std::vector<double>lbounds;
			size_t opat = 0;
			for (std::vector<FMToperatingareascheme>::const_iterator opit : tobound)
			{
				std::vector<FMToperatingareascheme>allneighbors;
				for (const Core::FMTmask& neighbormask : opit->getNeighbors())
				{
					std::vector<FMToperatingareascheme>::const_iterator opneighbor = std::find_if(operatingareas.begin(), operatingareas.end(), FMToperatingareaschemecomparator(neighbormask));
					if (opneighbor != operatingareas.end())
					{
						allneighbors.push_back(*opneighbor);
					}

				}
				std::vector<size_t> potentialschemes;
				if (useprimal)
				{
					potentialschemes = opit->getPotentialPrimalSchemes(primalsolution, lowerprimalbounds, upperprimalbounds, allneighbors);
				}
				else {
					potentialschemes = opit->getPotentialDualSchemes(dualsolution, rowupperbound, allneighbors);
				}

				bool boundallscheme = true;
				size_t schemeid;
				if(!potentialschemes.empty())
				{
					//if (userandomness)
					//{
					//std::shuffle(potentialschemes.begin(), potentialschemes.end(), generator);
					//}
					boundallscheme = false;
					schemeid = *potentialschemes.begin();
					if (!schemestoskip.empty())
					{
						const int toskip = schemestoskip.at(opat);
						if(toskip>0)
						{
							schemeid = selectScheme(potentialschemes,toskip);
						}
					}
				}
				const bool opgotschedule = getBounds(opit, ltargeteditems, lbounds, boundallscheme, schemeid, keeploose);
				if (!opgotschedule && schemestoskip.empty())
				{
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"No schedule found for Operating area " + std::string(opit->getmask()),
						"FMToperatingareascheduler::setbounds", __LINE__, __FILE__);
				}
				gotschedule += opgotschedule;
				/*if (!potentialschemes.empty())
				{
					//if (userandomness)
					//{
					//	std::shuffle(potentialschemes.begin(), potentialschemes.end(), generator);
					//}
					++gotschedule;
					if (useprimal)
					{
						opit->boundPrimalScheme(targeteditems, bounds, *potentialschemes.begin());
					}
					else {
						bool emptyness = opit->unboundDualScheme(targeteditems, bounds, *potentialschemes.begin());
					}
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"No schedule found for Operating area "+std::s__LINE__opit->getmask()),
						"FMToperatingareascheduler::setBounds",__LINE__, __FILE__);
					if (useprimal)
					{
						opit->boundAllPrimalSchemes(targeteditems, bounds);
					}
					else {
						opit->boundAllDualSchemes(targeteditems, bounds);
					}
				}*/
				++opat;
			}
			if (!ltargeteditems.empty())
			{
				if (useprimal)
				{
					this->setColSetBounds(&ltargeteditems[0], &ltargeteditems.back() + 1, &lbounds[0]);
				}
				else {
					this->setRowSetBounds(&ltargeteditems[0], &ltargeteditems.back() + 1, &lbounds[0]);
					this->clearRowCache();
				}
			}
			
			}catch(...)
	            {
	           _exhandler->raisefromcatch("", "FMToperatingareascheduler::setbounds", __LINE__, __FILE__);
	            }
			return gotschedule;
			}

	std::vector<Core::FMTtimeyieldhandler> FMToperatingareascheduler::getSolution(const std::string& yldname) const
		{
		std::vector<Core::FMTtimeyieldhandler>allhandlers;
		try {
            const double* primalsolution = this->getColSolution();
            const double* rowupperbound = this->getRowUpper();
			const double* rowsolution = this->getRowActivity();
            for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
                operatingareait != operatingareas.end(); ++operatingareait)
                {
                std::vector<double>data;
                if (useprimal)
                    {
                    data=operatingareait->getPrimalSolution(primalsolution);
                }else {
					bool breakneighboringextended = false;
                    data=operatingareait->getDualSolution(rowupperbound, rowsolution, breakneighboringextended);
					if (breakneighboringextended)
						{
						_exhandler->raise(Exception::FMTexc::FMTignore,
							"Extending solution past last period can breaks neighboring constraints on "+std::string(operatingareait->getmask()),
							"FMToperatingareascheduler::getsolution", __LINE__, __FILE__);
						}
                    }
                std::vector<std::string>source;
				Core::FMTtimeyieldhandler handler(operatingareait->getmask());
                handler.pushData(yldname,Core::FMTdata(data, Core::FMTyieldparserop::FMTnone, source));
				handler.pushBase(0);
				allhandlers.push_back(handler);
                }
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::getsolution", __LINE__, __FILE__);
			}
		return allhandlers;
		}

	std::vector<Core::FMToutput>FMToperatingareascheduler::getLevelSolution(const std::string& outputname, const std::string& aggregate,int outputid) const
	{
		std::vector<Core::FMToutput>alloutputs;
		try {
			const double* rowlowerbound = this->getRowLower();
			const double* rowupperbound = this->getRowUpper();
			size_t cid = 0;
			for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
				operatingareait != operatingareas.end(); ++operatingareait)
			{
				std::vector<double>data;
				if (!useprimal)
					{
					data = operatingareait->getDualLowerBounds(rowlowerbound, rowupperbound);
					}
				std::vector<Core::FMToutputsource>sources;
				std::vector<Core::FMToperator>operators;
				sources.push_back(Core::FMToutputsource(Core::FMTspec(), operatingareait->getmask(), Core::FMTotar::actual, "", aggregate, outputid));
				++outputid;
				const Core::FMToutput variableoutput(outputname + std::to_string(cid), "OPAREA " + std::to_string(cid), "BFECOPT",sources,operators);
				sources.clear();
				const std::string levelname = outputname + "bound" + std::to_string(cid);
				sources.push_back(Core::FMToutputsource(Core::FMTotar::level, data, outputid,-1, levelname));
				++outputid;
				const Core::FMToutput leveloutput(levelname, "OPAREABOUND" + std::to_string(cid), "BFECOPT", sources, operators);
				alloutputs.push_back(variableoutput);
				alloutputs.push_back(leveloutput);
				++cid;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::getlevelsolution", __LINE__, __FILE__);
		}
		return alloutputs;

	}


	FMToperatingareascheduler::FMToperatingareascheduler(const std::vector<FMToperatingareascheme>& loperatingareas,
		const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& maingraph,
		const Models::FMTmodel& model,
		const Core::FMToutputnode& target,
		Models::FMTlpsolver& basesolve, size_t lseed,
		double proportionofset, bool userandomness, bool copysolver):
		FMTlpheuristic(basesolve,lseed,copysolver),operatingareas(loperatingareas),adjacencyconstraints(), proportionofset(proportionofset),
		userandomness(userandomness), useprimal(false)
		{
		try {
			/*if (copysolver)
				{
				*_logger<<"copy"<<"\n";
				FMTlpsolver::operator=(basesolve);
			}else {
				FMTlpsolver::passInSolver(basesolve);
				}*/
			updateRowsAndColsNames();
			const double baseobj = this->getObjValue();
			this->setOperatingAreasConstraints(maingraph, model, target);
			size_t complexity=0;
			for (const auto& oparea : operatingareas)
			{
				complexity+=oparea.getNumberofscheme();
			}
			const std::string logof = "Complexity calculated by scheduler : " + std::to_string(complexity) + "\n";
			(*_logger) << logof;
			bool adjacencyconstraintset = this->setAdjacencyConstraints();
			updateRowsAndColsNames();
			//const std::chrono::time_point<std::chrono::high_resolution_clock>teststart = getClock();
			/*if (!useprimal)//set it in the right form remove it from initialSolve....
			{
				this->unboundAll(); //Make sure rhs are right need to be released
				this->closePrimalBounds(); //Need that to get some activities
			}*/


			if (false/*completeInitialSolution()*/) // If you can complete the initial solution then you juste need a warmstart
			{
				this->stockResolve();
				/*}else if(gotRejectedNodes()) {
				FMTlpsolver::initialSolve();*/
			}else {
				this->resolveModel();//else just do an initialSolve...
			}
			if (!adjacencyconstraintset)
			{
				if(std::abs(this->getObjValue() - baseobj)>0.01*baseobj)
				{
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
												"Difference in objvalue after setting operating areaconstraints with new objective "+std::to_string(this->getObjValue()-baseobj),
												"FMToperatingareascheduler::FMToperatingareascheduler", __LINE__, __FILE__);

				}
			}
			if (!this->isProvenOptimal())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
													"Infeasi__LINE__del",
													"FMToperatingareascheduler::FMToperatingareascheduler", __LINE__, __FILE__);

			}
			//_logger->logwithlevel("Solved wamrstart in  " + getDurationInSeconds(teststart) + "\n", 1);
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::FMToperatingareaheuristic", __LINE__, __FILE__);
			}
		}

	void FMToperatingareascheduler::setAsRandom()
		{
		userandomness = true;
		}

	void FMToperatingareascheduler::setAsPrimal()
		{
		useprimal = true;
		}

	void FMToperatingareascheduler::setProportionOfSet(const double& proportion)
		{
		proportionofset=proportion;
		}

	double FMToperatingareascheduler::generateInitialProportionOfSet() const
	{
		double calculatedproportion = 0.25;
		try {
			size_t complexity = 0;
			size_t basecomplexity = 0;
			for (const Heuristics::FMToperatingareascheme& oparea : operatingareas)
			{
				complexity += oparea.getNumberofscheme();
				basecomplexity += oparea.getNumberofsimplescheme();
			}
			const double complexdif = static_cast<double>(complexity - basecomplexity);
			const double complexprop = std::min((complexdif / static_cast<double>(basecomplexity)),1.0);
			if (complexprop > 0.01)
				{
				calculatedproportion -= (complexprop * 0.05);
				calculatedproportion = std::max(0.01, calculatedproportion);
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::generateinitialproportionofset", __LINE__, __FILE__);
		}
		return calculatedproportion;
	}


	std::string FMToperatingareascheduler::getThreadId() const
	{
		return boost::lexical_cast<std::string>(boost::this_thread::get_id());
	}


}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMToperatingareascheduler)
#endif
