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
	void FMToperatingareascheduler::clearrowcache()
		{
		if (!useprimal)
			{
			FMTlpsolver::clearrowcache();
			}
		}


	void FMToperatingareascheduler::unboundall(bool atprimal)
		{
		try {
			this->clearrowcache();
			std::vector<int>targeteditems;
			std::vector<double>bounds;
			for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
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
			this->clearrowcache();
			std::vector<int>targeteditems;
			std::vector<double>bounds;
			for (std::vector<FMToperatingareascheme>::const_iterator operatingareait : tounbound)
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
					this->setColSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
				}else {
					this->setRowSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
					}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::unbound", __LINE__, __FILE__);
			}
		}


	void FMToperatingareascheduler::closeprimalbounds()
		{
        try{
            std::vector<int>variables;
            std::vector<double>bounds;
            for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
                operatingareait != operatingareas.end(); ++operatingareait)
                    {
                    operatingareait->boundallprimalschemes(variables,bounds,0);
                    }
            this->setColSetBounds(&variables[0], &variables.back() + 1, &bounds[0]);
        }catch (...)
            {
			_exhandler->printexceptions("", "FMToperatingareascheduler::closeprimalbounds", __LINE__, __FILE__);
            }
		}

	void FMToperatingareascheduler::setallinteger()
		{
        try{
		if (!useprimal)//need to turn back the formulation to primal to make it work ith MIP
			{
				this->clearrowcache();
				const double* rowupper = this->getRowUpper();
				std::vector<int>rowsitems;
				std::vector<double>rowsbounds;
				std::vector<int>colsitems;
				std::vector<double>colsbounds;
				for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
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
				this->setRowSetBounds(&rowsitems[0], &rowsitems.back() + 1, &rowsbounds[0]);
				this->setColSetBounds(&colsitems[0], &colsitems.back() + 1, &colsbounds[0]);
				this->stockresolve();
			}
		std::vector<int>integervariables;
		for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
			operatingareait != operatingareas.end(); ++operatingareait)
				{
				operatingareait->pushbinaries(integervariables);
				}
		this->setInteger(&integervariables[0], static_cast<int>(integervariables.size()));
        }catch(...)
            {
            _exhandler->raisefromcatch("","FMToperatingareascheduler::setallinteger", __LINE__, __FILE__);
            }
		}

	int FMToperatingareascheduler::resolvemodel()
		{
		try {
			if (!useprimal || solvertype != Models::FMTsolverinterface::CLP)
			{
				FMTlpsolver::resolve();
			}
			else {
				this->stockresolve();
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::resolvemodel", __LINE__, __FILE__);
			}
		return FMTlpsolver::getiterationcount();
		}

	bool FMToperatingareascheduler::empty() const
		{
		return operatingareas.empty();
		}

	void FMToperatingareascheduler::setfordualinitialsolve()
	{
		try {
			this->unboundall(); //Make sure rhs are right need to be released
			this->closeprimalbounds(); //Need that to get some activities
			this->resolvemodel();
		}catch (...)
			{
			_exhandler->printexceptions("Thread : " + getthreadid() + " ", "FMToperatingareascheduler::setfordualinitialsolve", __LINE__, __FILE__);
			}

	}


	int FMToperatingareascheduler::initialmovesolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected)
	{
		int iterations = 0;
		try {
			this->setbounds(selected);
			iterations = this->resolvemodel();
		}catch (...)
		{
			_exhandler->printexceptions("Thread : " + getthreadid() + " ", "FMToperatingareascheduler::initialmovesolution", __LINE__, __FILE__);
		}
		return iterations;
	}

	void FMToperatingareascheduler::resetbaseinitialsolution()
	{
		try {
			this->unboundall(); //release everything
			if (!useprimal)
			{
				this->closeprimalbounds();
			}
			this->resolvemodel();
		}
		catch (...)
		{
			_exhandler->printexceptions("Thread : " + getthreadid() + " ", "FMToperatingareascheduler::resetbaseinitialsolution", __LINE__, __FILE__);
		}

	}

	bool FMToperatingareascheduler::initialsolve()
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
					setfordualinitialsolve();
					problemsolved = "dual";
				}
				std::vector<std::vector<FMToperatingareascheme>::const_iterator> selected;
				do {
					this->clearrowcache();
					selected = this->setdraw();
					const int iterations = initialmovesolution(selected);
					opareaprocessed += selected.size();
					if (!selected.empty())
					{
						int setratio = static_cast<int>(((static_cast<double>(opareaprocessed)) / (static_cast<double>(this->operatingareas.size()))) * 100);
						(*_logger) << "Thread : " + getthreadid() + " Solution generation phase (" + std::to_string(setratio) + "%) took " + std::to_string(iterations) + " iterations on " + problemsolved + " formulation" << "\n";
					}
				} while (!selected.empty());
				if (this->isProvenOptimal())
				{
					foundsolution = true;
					const double newobjective = this->getObjValue();
					const std::string relativevalue = std::to_string(static_cast<int>(std::abs(initialobjectivevalue - newobjective) * 100 / initialobjectivevalue));
					(*_logger) << "Thread : " + getthreadid() + " Feasible solution found objective: " + std::to_string(newobjective) + " (" + relativevalue + "%) " << "\n";
					this->clearrowcache();
				}
				else {
					if (!userandomness)
					{
						_exhandler->raise(Exception::FMTexc::FMTschemefailed,
							"Thread : " + getthreadid() + " FMToperatingareascheduler failed initialsolve switching to random for next try.",
							"FMToperatingareascheduler::initialsolve", __LINE__, __FILE__);
						userandomness = true; //Switch to random now

					}
					else {
						_exhandler->raise(Exception::FMTexc::FMTschemefailed,
							"Thread : " + getthreadid() + " FMToperatingareascheduler initialsolve at random failed, trying another scheme.",
							"FMToperatingareascheduler::initialsolve", __LINE__, __FILE__);
					}
					resetbaseinitialsolution();
				}
			}else{
				{
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Thread : " + getthreadid() + " Model is not optimal at the beginning of initialsolve",
														"FMToperatingareascheduler::intialsolve", __LINE__, __FILE__);

				}
			}
		}catch (...)
		{
			_exhandler->printexceptions("Thread : " + getthreadid()+" ", "FMToperatingareascheduler::initialsolve", __LINE__, __FILE__);
		}
		return foundsolution;
		}

	bool FMToperatingareascheduler::branchnboundsolve()
		{
        try{
			if (this->isProvenOptimal())
			{
				//In that order it seems to work...
				this->setallinteger();
				this->branchAndBound();
			}
			this->MIPparameters();
			this->unboundall(true);
			this->branchAndBound();
        	}catch(...)
            	{
        		_exhandler->printexceptions("", "FMToperatingareascheduler::branchnboundsolve", __LINE__, __FILE__);
            	}
        useprimal=true;
		return this->isProvenOptimal();
		}

	void FMToperatingareascheduler::resetbasegreedysolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected,
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
						getbounds(opit, targeteditems, bounds, false, static_cast<size_t>(schemeid));
					}
				}
				else {
					getbounds(opit, targeteditems, bounds, true);
				}
				++opat;
			}
			if (useprimal)
			{
				this->setColSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
			}
			else {
				this->setRowSetBounds(&targeteditems[0], &targeteditems.back() + 1, &bounds[0]);
				this->clearrowcache();
			}
			this->resolvemodel();
		}catch (...)
		{
			_exhandler->printexceptions("Thread : " + getthreadid() +
				" Selected size of " + std::to_string(selected.size()) +
				" tabou size of " + std::to_string(oldschemeid.size()) +
				" Constraints size of " + std::to_string(oldconstraints.size()) +
				" bounds size of " + std::to_string(oldbounds.size()), "FMToperatingareascheduler::resetbasegreedysolution", __LINE__, __FILE__);
		}

	}


	bool FMToperatingareascheduler::releasegreedysolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected,
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
					cit->fillboundsnvariables(rowlowers, rowuppers, oldconstraints, oldbounds);
				}
			}

			this->unbound(selected);
			this->resolvemodel();
			optimalsolution = this->isProvenOptimal();
		}catch (...)
		{
			_exhandler->printexceptions("Thread : " + getthreadid() +
				" Selected size of "+ std::to_string(selected.size())+
				" Constraints size of "+ std::to_string(oldconstraints.size()) +
				" bounds size of " + std::to_string(oldbounds.size()) , "FMToperatingareascheduler::releasegreedysolution", __LINE__, __FILE__);
		}
		return optimalsolution;
	}

	bool FMToperatingareascheduler::greedymovesolution(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& selected,
		const std::vector<int>& oldschemeid)
	{
		bool optimalsolution = false;
		try {
			this->setbounds(selected, oldschemeid, false);
			this->resolvemodel();
			optimalsolution = this->isProvenOptimal();
		}catch (...)
			{
			_exhandler->printexceptions("Thread : " + getthreadid() +
				" Selected size of " + std::to_string(selected.size()) +
				" tabou size of " + std::to_string(oldschemeid.size()), "FMToperatingareascheduler::greedymovesolution", __LINE__, __FILE__);
			}
		return optimalsolution;
	}

	bool FMToperatingareascheduler::greedypass(const double& initsol, const unsigned int& iteration)
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
					this->closeprimalbounds();
				}
				std::vector<std::vector<FMToperatingareascheme>::const_iterator> opareas;
				opareas.reserve(operatingareas.size());
				for (std::vector<FMToperatingareascheme>::const_iterator areait = operatingareas.begin() ; areait != operatingareas.end() ; ++areait)
				{
					opareas.push_back(areait);
				}
				this->clearrowcache();
				if(!userandomness)
				{
					//Switch to random for next draw
					userandomness = true;
				}
				const std::vector<std::vector<FMToperatingareascheme>::const_iterator> selected = draw(opareas);
				const std::vector<int> oldschemeid = getsolutionindexes(selected);
				std::vector<int>oldconstraints;
				std::vector<double>oldbounds;
				bool gotoptimalsolution = false;
				if (releasegreedysolution(selected,oldconstraints,oldbounds))
					{
					gotoptimalsolution = greedymovesolution(selected, oldschemeid);
					}
				const double newobjective = this->getObjValue();
				if (gotoptimalsolution && ((newobjective*sens < initialobjectivevalue*sens)))
				{
					const std::string relativevalue = std::to_string(static_cast<int>(std::abs(initsol - newobjective) * 100 / initsol));
					(*_logger) << "Thread : " + getthreadid() + " Better solution found objective: " + std::to_string(newobjective) + " (" + relativevalue + "%). "+std::to_string(iteration)+" iterations left." << "\n";
					this->clearrowcache();
				}else{
					if (iteration%10 == 0 && iteration>0)
						{
						const std::string bestgap = std::to_string(static_cast<int>(std::abs(initsol - initialobjectivevalue) * 100 / initsol));
						(*_logger) << "Thread : " + getthreadid() +" ("+bestgap+"%) "+std::to_string(iteration) + " iterations left..." << "\n";
						}
					resetbasegreedysolution(selected,oldschemeid,oldconstraints,oldbounds);
				}
			}
		}catch (...)
		{
			_exhandler->printexceptions("Thread : " + getthreadid(), "FMToperatingareascheduler::greedypass", __LINE__, __FILE__);
		}
		return this->isProvenOptimal();
	}

	std::vector<int> FMToperatingareascheduler::getsolutionindexes(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& opareaits) const
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
					if (opareait->getdualsolutionindex(rhsupper,schemesid))
					{
						indexes.push_back(static_cast<int>(schemesid));
					}
					else{
						indexes.push_back(-1);
					}
				}
				else
				{
					if(opareait->isprimalbounded(lowerbounds, upperbounds))
					{
						indexes.push_back(static_cast<int>(opareait->getprimalsolutionindex(primalsolution)));
					}else{
						indexes.push_back(-1);
					}
				}
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::getsolutionindexes", __LINE__, __FILE__);
		}
		return indexes;
	}

	bool FMToperatingareascheduler::gotrejectednodes() const
	{
		try {
			for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
				operatingareait != operatingareas.end(); ++operatingareait)
			{
				if (operatingareait->getrejectednodescid() > 0)
				{
					return true;
				}
			}

		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::gotrejectednodes", __LINE__, __FILE__);
		}
		return false;
	}

	bool FMToperatingareascheduler::completeinitialsolution()
	{
		try{
			if (!useprimal)//set it in the right form remove it from initialsolve....
			{
				//this->unboundall(); //Make sure rhs are right need to be released
				//this->closeprimalbounds(); //Need that to get some activities
				const double* initialcolsolution = getColSolution();
				const double* initialrowsolution = getRowPrice();
				std::vector<double>newcolsolution(initialcolsolution, initialcolsolution + getNumCols() + 1);
				std::vector<double>newrowsolution(initialrowsolution, initialrowsolution + getNumRows() + 1);
				for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
					operatingareait != operatingareas.end(); ++operatingareait)
				{
					if (operatingareait->getrejectednodescid()>0)
					{
						return false;
					}
					for (const std::vector<int>& scheme : operatingareait->getopeningconstraints())
					{
						for (const int& contraintindex : scheme)
						{
							newrowsolution[contraintindex] = 0.0;
						}
					}
					if (operatingareait->getmaximalschemesconstraint()>0)
					{
						newrowsolution[operatingareait->getmaximalschemesconstraint()] = 0.0;
					}
					
					for (const int& binary : operatingareait->getopeningbinaries())
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
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::completeinitialsolution", __LINE__, __FILE__);
		}
		return false;
	}


	void FMToperatingareascheduler::setoperatingareasconstraints(const Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>& maingraph,
																const Models::FMTmodel& model,
																const Core::FMToutputnode& target)
		{
		try {
			Core::FMToutputnode specifictarget(target);
			const std::vector<Core::FMTaction>modelactions = model.getactions();
			//const std::unordered_map<size_t, Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor> basedescriptors = maingraph.getperiodverticies(maingraph.getfirstactiveperiod());
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
				specifictarget.source.setmask(operatingareait->getmask());
				areatarget.source.setmask(operatingareait->getmask());
				std::vector<std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>>descriptors;
				std::vector<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::FMTvertex_descriptor>totalareadescriptors;
				for (int period = static_cast<int>((maingraph.getfirstactiveperiod() + operatingareait->getstartingperiod())); period < static_cast<int>((maingraph.size() - 1)); ++period)
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
							"FMToperatingareascheduler::setoperatingareasconstraints", __LINE__, __FILE__);
						}
					operatingareait->setconstraints(descriptors, totalareadescriptors, maingraph, *this, primalsolution, actionids);
				}else{
					_exhandler->raise(Exception::FMTexc::FMTignore,
											"No nodes found in graph for "+
											std::string(operatingareait->getmask()),
											"FMToperatingareascheduler::setoperatingareasconstraints", __LINE__, __FILE__);
				}
				if (operatingareait->getarea()==0)
					{
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"area of operating area "+
						std::string(operatingareait->getmask())+" is null",
						"FMToperatingareascheduler::setoperatingareasconstraints", __LINE__, __FILE__);
					}else if (operatingareait->getopeningbinaries().empty())
						{
						_exhandler->raise(Exception::FMTexc::FMTignore,
							std::string(operatingareait->getmask()) + " not operable",
							"FMToperatingareascheduler::setoperatingareasconstraints", __LINE__, __FILE__);
						}
			}
			this->synchronize();
			/*if (!this->stockresolve()){
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
													"Infeasible model",
													"FMToperatingareascheduler::setoperatingareasconstraints", __LINE__, __FILE__);

					}*/
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToperatingareascheduler::setoperatingareasconstraints", __LINE__, __FILE__);
			}
		}

	bool FMToperatingareascheduler::setadjacencyconstraints()
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
			const std::vector<Core::FMTmask>neighbors = operatingareait->getneighbors();
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
						neighborsbin  = operatingareait->getcommonbinairies(*opneighbor);
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
							this->setrowname("adj_"+std::string(operatingareait->getmask())+"_"+std::string(neighbor)+"_"+std::to_string(binit->first)+"_"+std::to_string(index),constraintsid);
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
		/*if (!this->stockresolve()){
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
										"Infeasible model",
										"FMToperatingareascheduler::setadjacencyconstraints", __LINE__, __FILE__);

		}*/
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::setadjacencyconstraints", __LINE__, __FILE__);
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
		    if (!usingsolvercopy&&canupdatesource())
                {
                  //Will need a clean matrix to fit with FMTlpmodel!
                std::vector<int>rowstodelete;
                std::vector<int>columnstodelete;
                for (std::vector<FMToperatingareascheme>::const_iterator operatingareait = operatingareas.begin();
                    operatingareait != operatingareas.end(); ++operatingareait)
                {
                    operatingareait->getressourcestodelete(columnstodelete, rowstodelete);
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
                    this->resolvemodel();
                }
                operatingareas.clear();
                adjacencyconstraints.clear();
                }
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::~", __LINE__, __FILE__);
			}
		}

	std::vector<std::vector<FMToperatingareascheme>::const_iterator> FMToperatingareascheduler::setdraw()
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
                if (!areait->empty() && ((useprimal && !areait->isprimalbounded(lowerbounds, upperbounds) && !areait->isallprimalbounded(lowerbounds, upperbounds))||
                    (!useprimal && !areait->isdualbounded(rhsupper)   && areait->isthresholdactivity(rhsupper))))
                    {
                    //Make sure it's sorted!
                    double value = 0;
                    if (useprimal)
                        {
                        value = areait->getbinariessum(primalsolution);
                    }else {
                        value = areait->getactivitysum(dualsolution);
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
                                        potentialvalue = (*vit)->getbinariessum(primalsolution);
                                    }else {
                                        potentialvalue = (*vit)->getactivitysum(dualsolution);
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
                _exhandler->raise(Exception::FMTexc::FMTrangeerror,"Proportion of selected operating area equal 0","FMToperatingareascheduler::setdraw",__LINE__,__FILE__);
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

	size_t FMToperatingareascheduler::selectscheme(std::vector<size_t>& potentialschemes, const int& schemetoskip) const
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

	bool FMToperatingareascheduler::getbounds(const std::vector<FMToperatingareascheme>::const_iterator& operatingareaiterator,std::vector<int>& targeteditems,std::vector<double>& bounds, const bool& boundall, const size_t& schemeid, bool keeploose) const
	{
		try{
			if (!boundall)
			{
				if (useprimal)
				{
					operatingareaiterator->boundprimalscheme(targeteditems, bounds, schemeid);
				}
				else {
					const double* dualsolution = this->getRowActivity();
					bool emptyness = operatingareaiterator->unbounddualscheme(dualsolution,targeteditems, bounds,schemeid, keeploose);
				}
				return true;
			}
			else {
				if (useprimal)
				{
					operatingareaiterator->boundallprimalschemes(targeteditems, bounds);
				}
				else {
					operatingareaiterator->boundalldualschemes(targeteditems, bounds);
				}
				return false;
			}
		}catch(...)
			{
				_exhandler->raisefromcatch("", "FMToperatingareascheduler::getbounds", __LINE__, __FILE__);
			}
		return false;
	}

	size_t FMToperatingareascheduler::setbounds(const std::vector<std::vector<FMToperatingareascheme>::const_iterator>& tobound,const std::vector<int>& schemestoskip,bool keeploose)
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
				for (const Core::FMTmask& neighbormask : opit->getneighbors())
				{
					std::vector<FMToperatingareascheme>::const_iterator opneighbor = std::find_if(operatingareas.begin(), operatingareas.end(), FMToperatingareaschemecomparator(neighbormask));
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
							schemeid = selectscheme(potentialschemes,toskip);
						}
					}
				}
				const bool opgotschedule = getbounds(opit, ltargeteditems, lbounds, boundallscheme, schemeid, keeploose);
				if (!opgotschedule && schemestoskip.empty())
				{
					_exhandler->raise(Exception::FMTexc::FMTignore,
					"No schedule found for Operating area "+std::string(opit->getmask()),
					"FMToperatingareascheduler::setbounds",__LINE__, __FILE__);
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
						opit->boundprimalscheme(targeteditems, bounds, *potentialschemes.begin());
					}
					else {
						bool emptyness = opit->unbounddualscheme(targeteditems, bounds, *potentialschemes.begin());
					}
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"No schedule found for Operating area "+std::string(opit->getmask()),
						"FMToperatingareascheduler::setbounds",__LINE__, __FILE__);
					if (useprimal)
					{
						opit->boundallprimalschemes(targeteditems, bounds);
					}
					else {
						opit->boundalldualschemes(targeteditems, bounds);
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
					this->clearrowcache();
				}
			}
			
			}catch(...)
	            {
	            _exhandler->raisefromcatch("", "FMToperatingareascheduler::setbounds", __LINE__, __FILE__);
	            }
			return gotschedule;
			}

	std::vector<Core::FMTtimeyieldhandler> FMToperatingareascheduler::getsolution(const std::string& yldname) const
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
                    data=operatingareait->getprimalsolution(primalsolution);
                }else {
					bool breakneighboringextended = false;
                    data=operatingareait->getdualsolution(rowupperbound, rowsolution, breakneighboringextended);
					if (breakneighboringextended)
						{
						_exhandler->raise(Exception::FMTexc::FMTignore,
							"Extending solution past last period can breaks neighboring constraints on "+std::string(operatingareait->getmask()),
							"FMToperatingareascheduler::getsolution", __LINE__, __FILE__);
						}
                    }
                std::vector<std::string>source;
				Core::FMTtimeyieldhandler handler(operatingareait->getmask());
                handler.push_data(yldname,Core::FMTdata(data, Core::FMTyieldparserop::FMTnone, source));
				handler.push_base(0);
				allhandlers.push_back(handler);
                }
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::getsolution", __LINE__, __FILE__);
			}
		return allhandlers;
		}

	std::vector<Core::FMToutput>FMToperatingareascheduler::getlevelsolution(const std::string& outputname, const std::string& aggregate,int outputid) const
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
					data = operatingareait->getduallowerbounds(rowlowerbound, rowupperbound);
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
				FMTlpsolver::passinsolver(basesolve);
				}*/
			updaterowsandcolsnames();
			const double baseobj = this->getObjValue();
			this->setoperatingareasconstraints(maingraph, model, target);
			size_t complexity=0;
			for (const auto& oparea : operatingareas)
			{
				complexity+=oparea.getnumberofscheme();
			}
			const std::string logof = "Complexity calculated by scheduler : " + std::to_string(complexity) + "\n";
			(*_logger) << logof;
			bool adjacencyconstraintset = this->setadjacencyconstraints();
			updaterowsandcolsnames();
			//const std::chrono::time_point<std::chrono::high_resolution_clock>teststart = getclock();
			/*if (!useprimal)//set it in the right form remove it from initialsolve....
			{
				this->unboundall(); //Make sure rhs are right need to be released
				this->closeprimalbounds(); //Need that to get some activities
			}*/


			if (false/*completeinitialsolution()*/) // If you can complete the initial solution then you juste need a warmstart
			{
				this->stockresolve();
				/*}else if(gotrejectednodes()) {
				FMTlpsolver::initialsolve();*/
			}else {
				this->resolvemodel();//else just do an initialsolve...
			}
			if (!adjacencyconstraintset)
			{
				if(std::abs(this->getObjValue() - baseobj)>0.01*baseobj)
				{
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
												"Difference in objvalue after setting operating areaconstraints with new objective "+std::to_string(this->getObjValue() - baseobj),
												"FMToperatingareascheduler::FMToperatingareascheduler", __LINE__, __FILE__);

				}
			}
			if (!this->isProvenOptimal())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
													"Infeasible model",
													"FMToperatingareascheduler::FMToperatingareascheduler", __LINE__, __FILE__);

			}
			//_logger->logwithlevel("Solved wamrstart in  " + getdurationinseconds(teststart) + "\n", 1);
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToperatingareascheduler::FMToperatingareaheuristic", __LINE__, __FILE__);
			}
		}

	void FMToperatingareascheduler::setasrandom()
		{
		userandomness = true;
		}

	void FMToperatingareascheduler::setasprimal()
		{
		useprimal = true;
		}

	void FMToperatingareascheduler::setproportionofset(const double& proportion)
		{
		proportionofset=proportion;
		}

	double FMToperatingareascheduler::generateinitialproportionofset() const
	{
		double calculatedproportion = 0.25;
		try {
			size_t complexity = 0;
			size_t basecomplexity = 0;
			for (const Heuristics::FMToperatingareascheme& oparea : operatingareas)
			{
				complexity += oparea.getnumberofscheme();
				basecomplexity += oparea.getnumberofsimplescheme();
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


	std::string FMToperatingareascheduler::getthreadid() const
	{
		return boost::lexical_cast<std::string>(boost::this_thread::get_id());
	}


}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMToperatingareascheduler)
#endif
