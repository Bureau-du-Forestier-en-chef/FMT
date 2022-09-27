/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTopareaschedulertask.hpp"
#include "FMTyieldparser.hpp"
#include "FMTlpmodel.hpp"
#include "FMToutputnode.hpp"
#include "FMToperatingareascheme.hpp"
#include "FMToperatingareascheduler.hpp"
#include "FMTtimeyieldhandler.hpp"
#include <limits>
#include <boost/thread.hpp>
#include "FMTexceptionhandler.hpp"
#include "FMTscheduleparser.hpp"
#include "FMTmodelparser.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>


namespace Parallel
{
	boost::mutex FMTopareaschedulertask::generalmutex;
	std::string FMTopareaschedulertask::solutionlocation = std::string();
	unsigned int FMTopareaschedulertask::iterations = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> FMTopareaschedulertask::stoptime = std::chrono::time_point<std::chrono::high_resolution_clock>();
	double FMTopareaschedulertask::relax_objective = 0.0;
	std::string FMTopareaschedulertask::outyldname = std::string();
	std::unique_ptr<Models::FMTlpmodel> FMTopareaschedulertask::basemodel(nullptr);


	double FMTopareaschedulertask::solveinitialmodel(Models::FMTlpmodel& model) const
	{
		try {
			if (model.doplanning(true))
				{
				return model.getObjValue();
			}else {
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					model.getname()+" is infeasible at initialsolve",
					"FMTopareaschedulertask::solveinitialmodel", __LINE__, __FILE__);
			}

		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::solveinitialmodel", __LINE__, __FILE__);
		}
		return 0;
	}

	void FMTopareaschedulertask::setinitialscheduler(Models::FMTlpmodel& model,
		const std::vector<Heuristics::FMToperatingareascheme>& opareas, const Core::FMToutputnode& node)
	{
		try {
			const std::vector<Heuristics::FMToperatingareascheduler>heuristics = model.getoperatingareaschedulerheuristics(opareas, node);
			actualscheduler = std::move(std::unique_ptr<Heuristics::FMToperatingareascheduler>(new Heuristics::FMToperatingareascheduler(heuristics.at(0))));
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::setinitialscheduler", __LINE__, __FILE__);
		}
	}



	std::chrono::time_point<std::chrono::high_resolution_clock> FMTopareaschedulertask::getstoppoint(const double& timegap) const
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> then;
		try {
			then= getclock();
			then += std::chrono::seconds(static_cast<int>(timegap));
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::getstoppoint", __LINE__, __FILE__);
		}
		return then;
	}

	FMTopareaschedulertask::FMTopareaschedulertask(const Models::FMTlpmodel& model,
		const std::vector<Heuristics::FMToperatingareascheme>& opareas,
		const Core::FMToutputnode& node,
		const std::string& outputlocation,
		const std::string& outputyieldname,
		const unsigned int& maxiterations,
		const double& maxtime):
		bestscheduler(new Heuristics::FMToperatingareascheduler()),
		actualscheduler(),
		lastspawned(0)
	{
		try {
			Models::FMTlpmodel modelcopy(model);
			//Force postsolve to keep logic with the FMToperatingareascheme
			modelcopy.FMTmodel::setparameter(Models::FMTboolmodelparameters::POSTSOLVE,true);
			//Keep the non build modelcopy.
			basemodel = std::move(std::unique_ptr<Models::FMTlpmodel>(new Models::FMTlpmodel(modelcopy)));
			//basemodel->getsolverptr()->passinmessagehandler(*tasklogger.get());
			relax_objective = solveinitialmodel(modelcopy);
			setinitialscheduler(modelcopy,opareas,node);
			iterations = maxiterations;
			solutionlocation = outputlocation;
			stoptime = getstoppoint(maxtime);
			outyldname = outputyieldname;
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTopareaschedulertask::FMTopareaschedulertask()", __LINE__, __FILE__);
		}

	}

	FMTopareaschedulertask::FMTopareaschedulertask(const FMTopareaschedulertask& rhs):
		bestscheduler(rhs.bestscheduler),
		actualscheduler(new Heuristics::FMToperatingareascheduler(*rhs.actualscheduler)),
		lastspawned(rhs.lastspawned)
	{


	}


	FMTopareaschedulertask& FMTopareaschedulertask::operator = (const FMTopareaschedulertask& rhs)
	{
		if (this!=&rhs)
		{
			bestscheduler = rhs.bestscheduler;
			actualscheduler = std::move(std::unique_ptr<Heuristics::FMToperatingareascheduler>(new Heuristics::FMToperatingareascheduler(*rhs.actualscheduler)));
			lastspawned = rhs.lastspawned;
		}
		return *this;
	}

	bool FMTopareaschedulertask::gotinitialsolution() const
	{
		try {
			const boost::lock_guard<boost::mutex>lock(generalmutex);
			return (!bestscheduler->empty());
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::gotinitialsolution", __LINE__, __FILE__);
		}
		return false;
	}

	bool FMTopareaschedulertask::goodtogo() const
	{
		try {
			const boost::lock_guard<boost::mutex>lock(generalmutex);
			return (iterations > 0 && getclock() < stoptime);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::goodtogo", __LINE__, __FILE__);
		}
		return false;
	}

	FMTopareaschedulertask::~FMTopareaschedulertask()
	{
		try {
			if (gotinitialsolution()&&bestscheduler.use_count()==1)
			{
				if (!bestscheduler->isProvenOptimal())
				{
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Non optimal best scheduler",
						"FMTopareaschedulertask::~FMTopareaschedulertask", __LINE__, __FILE__);
				}
				writesolution();
				writefinalmodel();
			}

		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::~FMTopareaschedulertask()", __LINE__, __FILE__);
		}
	}


	void FMTopareaschedulertask::writefinalmodel() const
	{
		try {
			
			const double* thesolution = bestscheduler->getColSolution();
			basemodel->getsolverptr()->passinmessagehandler(*tasklogger.get());
			Models::FMTlpmodel modelcopy(*basemodel);
			modelcopy.doplanning(false);
			Core::FMTyields newyields = modelcopy.getyields();
			newyields.unshrink(modelcopy.getthemes());
			for (const Core::FMTtimeyieldhandler& tyld : bestscheduler->getsolution(outyldname))
			{
				std::unique_ptr<Core::FMTyieldhandler>newyield(new Core::FMTtimeyieldhandler(tyld));
				newyields.push_front(newyield->getmask(), newyield);
			}
			newyields.update();
			modelcopy.setyields(newyields);
			Models::FMTlpsolver* solver = modelcopy.getsolverptr();
			solver->setColSolution(thesolution);
			Parser::FMTmodelparser modelparser;
			const boost::filesystem::path filepath(solutionlocation + ".txt");
			const boost::filesystem::path folderpath = filepath.parent_path();
			modelparser.write(modelcopy, folderpath.string() + "/");
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::writefinalmodel", __LINE__, __FILE__);
		}

	}


	void FMTopareaschedulertask::writesolution() const
	{
		try {
			const double bestobjvalue = bestscheduler->getObjValue();
			const std::string relativevalue = std::to_string(static_cast<int>(std::abs(relax_objective - bestobjvalue) * 100 / relax_objective));
			*_logger << "Best solution found objective: "+std::to_string(bestobjvalue) +" ("+relativevalue+"%)" << "\n";
			Core::FMTyields yields;
			for (const Core::FMTtimeyieldhandler& tyld : bestscheduler->getsolution(outyldname))
				{
				std::unique_ptr<Core::FMTyieldhandler>newyield(new Core::FMTtimeyieldhandler(tyld));
				yields.push_back(newyield->getmask(),newyield);
				}
			yields.update();
			Parser::FMTyieldparser yldparser;
			const std::string solutionname = solutionlocation +"_"+ std::to_string(bestobjvalue) + "_" + relativevalue + ".yld";
			yldparser.write(yields, solutionname);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::writesolution", __LINE__, __FILE__);
		}

	}



	void FMTopareaschedulertask::evaluateandcopy()
	{
		try {
			if (!actualscheduler->isProvenOptimal())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot evaluate actual schedule",
					"FMTopareaschedulertask::evaluateandcopy", __LINE__, __FILE__);
				}
				const boost::lock_guard<boost::mutex>lock(generalmutex);
				if (bestscheduler->empty())
				{
					*bestscheduler = *actualscheduler;
					if (!bestscheduler->isProvenOptimal())
					{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Non optimal best schedule copy",
							"FMTopareaschedulertask::evaluateandcopy", __LINE__, __FILE__);
					}
				}else {
					if (!bestscheduler->isProvenOptimal())
					{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Cannot evaluate best schedule",
							"FMTopareaschedulertask::evaluateandcopy", __LINE__, __FILE__);
					}
					const double sense = actualscheduler->getObjSense();
					const double actualobjective = actualscheduler->getObjValue();
					const double bestobjective = bestscheduler->getObjValue();
					if (bestobjective*sense > actualobjective*sense)
					{
						*bestscheduler = *actualscheduler;
					}else {
						*actualscheduler = *bestscheduler;
						actualscheduler->passinmessagehandler(*tasklogger.get());
					}
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::evaluateandcopy", __LINE__, __FILE__);
			}
	}


	std::unique_ptr<FMTtask> FMTopareaschedulertask::clone() const
		{
		return std::unique_ptr<FMTtask>(new FMTopareaschedulertask(*this));
		}

	std::vector<std::unique_ptr<FMTtask>>FMTopareaschedulertask::split(const unsigned int& numberoftasks) const
	{
		std::vector<std::unique_ptr<FMTtask>>tasks;
		try {
			for (unsigned int taskid = 0 ; taskid < numberoftasks;++taskid)
				{
				tasks.push_back(std::move(std::unique_ptr<FMTtask>(new FMTopareaschedulertask(*this))));
				FMTopareaschedulertask* newtaskptr = dynamic_cast<FMTopareaschedulertask*>(tasks.back().get());
				newtaskptr->actualscheduler->setnumberofthreads(1);
				if (taskid>0)
					{
					newtaskptr->actualscheduler->setasrandom();
					}
				newtaskptr->actualscheduler->setgeneratorseed(taskid);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::split", __LINE__, __FILE__);
			}
		return tasks;
	}

	std::unique_ptr<FMTtask>FMTopareaschedulertask::spawn()
	{
		try {
			if (goodtogo())
			{
				std::unique_ptr<FMTtask>newtask(new FMTopareaschedulertask(*this));
				FMTopareaschedulertask* newtaskptr = dynamic_cast<FMTopareaschedulertask*>(newtask.get());
				newtaskptr->actualscheduler->setnumberofthreads(1);
				if (lastspawned > 0)
				{
					newtaskptr->actualscheduler->setasrandom();
				}
				newtaskptr->actualscheduler->setgeneratorseed(lastspawned);
				++lastspawned;
				return std::move(newtask);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::spawn", __LINE__, __FILE__);
			}
	return std::unique_ptr<FMTtask>(nullptr);
	}

	void FMTopareaschedulertask::passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger)
		{
		try {
			actualscheduler->passinlogger(logger);
			if (gotinitialsolution())
			{
				const boost::lock_guard<boost::mutex>lock(generalmutex);
				bestscheduler->passinlogger(logger);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::passinlogger", __LINE__, __FILE__);
			}
		}


	void FMTopareaschedulertask::work()
	{
		try {
			//If you dont have initialsolution, you need to do an initialsolve
			actualscheduler->passinmessagehandler(*tasklogger.get());
			bool needinitialsolve = !gotinitialsolution();
			while (goodtogo())
			{
				if (needinitialsolve)
					{
					//If initialsolve fail, you need to redo an initialsolve
					needinitialsolve = !actualscheduler->initialsolve();
				}else {
					actualscheduler->greedypass(relax_objective,iterations);
					}
				if (needinitialsolve) 
				{
					//But if in the mean time another thread got a solution, take it and go to greedy
					if (gotinitialsolution()) 
					{
						needinitialsolve = false;
						*actualscheduler = *bestscheduler;
						actualscheduler->passinmessagehandler(*tasklogger.get());
					}
				}
				else {
					evaluateandcopy();
					//To only draw 1 operating area a the time
					actualscheduler->setproportionofset(0.00000000001);
				}
				if (iterations > 0)
				{
					--iterations;
				}
			}
			setstatus(true);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::work", __LINE__, __FILE__);
		}

	}

}