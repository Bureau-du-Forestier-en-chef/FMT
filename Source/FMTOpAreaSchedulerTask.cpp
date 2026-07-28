/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifdef FMTWITHOSI
#include "FMTOpAreaSchedulerTask.h"
#include "FMTYieldParser.h"
#include "FMTLpModel.h"
#include "FMTOutputNode.h"
#include "FMTOperatingAreaScheme.h"
#include "FMTOperatingAreaScheduler.h"
#include "FMTTimeYieldHandler.h"
#include <limits>
#include <boost/thread.hpp>
#include "FMTExceptionHandler.h"
#include "FMTScheduleParser.h"
#include "FMTModelParser.h"
#include "FMTAreaParser.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include "FMTOutput.h"
#include "FMTConstraint.h"
#include "FMTOutputParser.h"
#include "FMTOptimizationParser.h"

namespace Parallel
{
	boost::mutex FMTOpAreaSchedulerTask::generalmutex;
	std::string FMTOpAreaSchedulerTask::solutionlocation = std::string();
	unsigned int FMTOpAreaSchedulerTask::iterations = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> FMTOpAreaSchedulerTask::stoptime = std::chrono::time_point<std::chrono::high_resolution_clock>();
	double FMTOpAreaSchedulerTask::relax_objective = 0.0;
	std::string FMTOpAreaSchedulerTask::outyldname = std::string();
	std::unique_ptr<Models::FMTLpModel> FMTOpAreaSchedulerTask::basemodel(nullptr);


	double FMTOpAreaSchedulerTask::solveInitialModel(Models::FMTLpModel& model) const
	{
		try {
			if (model.doPlanning(true))
				{
				return model.getObjValue();
			}else {
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					model.getName()+" is infeasible at initialsolve",
					"FMTOpAreaSchedulerTask::solveInitialModel", __LINE__, __FILE__);
			}

		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::solveInitialModel", __LINE__, __FILE__);
		}
		return 0;
	}

	std::vector<Heuristics::FMTOperatingAreaScheme> FMTOpAreaSchedulerTask::getReturnTimeFromOutput(
		Models::FMTLpModel& model,
		const std::vector<Heuristics::FMTOperatingAreaScheme>& opareas,
		const Core::FMTOutput& output) const
	{
		std::vector<Heuristics::FMTOperatingAreaScheme>newschemes;
		try {
			const std::vector<Core::FMTTheme>themes = model.getThemes();
			const int model_length = model.getParameter(Models::FMTintmodelparameters::LENGTH);
			for (const Heuristics::FMTOperatingAreaScheme& opscheduler : opareas)
				{
				double total_value = 0;
				const Core::FMTOutput local_output = output.intersectWithMask(opscheduler.getMask(), themes);
				double non_zero = 0;
				for (int period = 0; period < model_length; ++period)
					{
					const double output_value = model.getOutput(local_output, period, Core::FMToutputlevel::totalonly).at("Total");
					if (output_value > FMT_DBL_TOLERANCE)
						{
						total_value += output_value;
						non_zero += 1;
						}
					}
				Heuristics::FMTOperatingAreaScheme new_scheme(opscheduler);
				if (non_zero>0)
				{
					const size_t return_time = static_cast<size_t>(std::round(total_value/non_zero));
					const size_t diff = (new_scheme.getMinimalReturnTime() - new_scheme.getMaximalReturnTime()) / 2;
					const size_t LowerReturn = return_time - diff;
					const size_t UpperReturn = return_time + diff;
					if (LowerReturn == 0 ||
						UpperReturn > std::numeric_limits<short>::max())
						{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Wrong returntime value for "+ std::string(opscheduler.getMask())+" values "+
							std::to_string(LowerReturn)+","+ std::to_string(UpperReturn),
							"FMTOpAreaSchedulerTask::getReturnTimeFromOutput", __LINE__, __FILE__);
						}
					new_scheme.setReturnTime(return_time - diff, return_time + diff);
					}
					newschemes.push_back(new_scheme);
				}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::getReturnTimeFromOutput", __LINE__, __FILE__);
		}
		return newschemes;
	}

	void FMTOpAreaSchedulerTask::setInitialScheduler(Models::FMTLpModel& model,
		const std::vector<Heuristics::FMTOperatingAreaScheme>& opareas, const Core::FMTOutputNode& node)
	{
		try {
			const std::vector<Heuristics::FMTOperatingAreaScheduler>heuristics = model.getOperatingAreaSchedulerHeuristics(opareas, node);
			actualscheduler = std::move(std::unique_ptr<Heuristics::FMTOperatingAreaScheduler>(new Heuristics::FMTOperatingAreaScheduler(heuristics.at(0))));
			const double calculatedpropotion = actualscheduler->generateInitialProportionOfSet();
			* _logger << "Initial proportion of set of : " + std::to_string(calculatedpropotion) << "\n";
			actualscheduler->setProportionOfSet(calculatedpropotion);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::setInitialScheduler", __LINE__, __FILE__);
		}
	}



	std::chrono::time_point<std::chrono::high_resolution_clock> FMTOpAreaSchedulerTask::getStopPoint(const int& timegap) const
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> then;
		try {
			then= getClock();
			then += std::chrono::seconds(timegap);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::getStopPoint", __LINE__, __FILE__);
		}
		return then;
	}

	FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask(const Models::FMTLpModel& model,
		const std::vector<Heuristics::FMTOperatingAreaScheme>& opareas,
		const Core::FMTOutputNode& node,
		const std::string& outputlocation,
		const std::string& outputyieldname,
		const unsigned int& maxiterations,
		const int& maxtime,
		Core::FMTOutput returntime_output):
		bestscheduler(new Heuristics::FMTOperatingAreaScheduler()),
		actualscheduler(),
		lastspawned(0)
	{
		try {
			Models::FMTLpModel modelcopy(model);
			//Force postSolve to keep logic with the FMTOperatingAreaScheme
			modelcopy.FMTModel::setParameter(Models::FMTboolmodelparameters::POSTSOLVE,true);
			//Keep the non build modelcopy.
			basemodel = std::move(std::unique_ptr<Models::FMTLpModel>(new Models::FMTLpModel(modelcopy)));
			solveInitialModel(modelcopy);
			if (!returntime_output.empty())
				{
				const std::vector<Heuristics::FMTOperatingAreaScheme> newschemes = getReturnTimeFromOutput(modelcopy, opareas, returntime_output);
				Parser::FMTAreaParser area_parser;
				const std::string location = (outputlocation +"/"+returntime_output.getName() + ".csv");
				std::vector<std::string>layersoptions;
				layersoptions.push_back("SEPARATOR=SEMICOLON");
				layersoptions.push_back("STRING_QUOTING=IF_NEEDED");
				#ifdef FMTWITHGDAL
					area_parser.writeOAschedulerparameters(location, newschemes, layersoptions);
				#endif
				setInitialScheduler(modelcopy, newschemes, node);
				}else {
				setInitialScheduler(modelcopy, opareas, node);
				}
			relax_objective = actualscheduler->getObjValue();
			iterations = maxiterations;
			solutionlocation = outputlocation;
			stoptime = getStopPoint(maxtime);
			outyldname = outputyieldname;
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask()", __LINE__, __FILE__);
		}

	}

	FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask(const FMTOpAreaSchedulerTask& rhs):
		bestscheduler(rhs.bestscheduler),
		actualscheduler(new Heuristics::FMTOperatingAreaScheduler(*rhs.actualscheduler)),
		lastspawned(rhs.lastspawned)
	{


	}


	FMTOpAreaSchedulerTask& FMTOpAreaSchedulerTask::operator = (const FMTOpAreaSchedulerTask& rhs)
	{
		if (this!=&rhs)
		{
			bestscheduler = rhs.bestscheduler;
			actualscheduler = std::move(std::unique_ptr<Heuristics::FMTOperatingAreaScheduler>(new Heuristics::FMTOperatingAreaScheduler(*rhs.actualscheduler)));
			lastspawned = rhs.lastspawned;
		}
		return *this;
	}

	bool FMTOpAreaSchedulerTask::gotInitialSolution() const
	{
		try {
			const boost::lock_guard<boost::mutex>lock(generalmutex);
			return (!bestscheduler->empty());
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::gotInitialSolution", __LINE__, __FILE__);
		}
		return false;
	}

	bool FMTOpAreaSchedulerTask::goodToGo() const
	{
		try {
			const boost::lock_guard<boost::mutex>lock(generalmutex);
			return (iterations > 0 && getClock() < stoptime);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::goodToGo", __LINE__, __FILE__);
		}
		return false;
	}

	void FMTOpAreaSchedulerTask::finalize()
		{
		try {
			if (gotInitialSolution())
			{
				if (!bestscheduler->isProvenOptimal())
				{
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Non optimal best scheduler",
						"FMTOpAreaSchedulerTask::finalize", __LINE__, __FILE__);
				}
				else {
					writeSolution();
					writeFinalModel();
				}
				bestscheduler = std::shared_ptr<Heuristics::FMTOperatingAreaScheduler>(nullptr);
			}

		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::finalize", __LINE__, __FILE__);
		}
		}



	void FMTOpAreaSchedulerTask::writeFinalModel() const
	{
		try {
			
			const double* thesolution = bestscheduler->getColSolution();
			basemodel->getSolverPtr()->passInMessageHandler(*m_tasklogger.get());
			Models::FMTLpModel modelcopy(*basemodel);
			modelcopy.doPlanning(false);
			Core::FMTYields newyields = modelcopy.getYields();
			newyields.unShrink(modelcopy.getThemes());
			for (const Core::FMTTimeYieldHandler& tyld : bestscheduler->getSolution(outyldname))
			{
				std::unique_ptr<Core::FMTYieldHandler>newyield(new Core::FMTTimeYieldHandler(tyld));
				newyields.push_front(newyield->getMask(), newyield);
			}
			newyields.update();
			modelcopy.setYields(newyields);
			std::vector<Core::FMTConstraint>constraints=basemodel->getconstraints();
			std::vector<Core::FMTOutput>outputs=basemodel->getOutputs();
			getConstraintsSolution(outputs, constraints);
			modelcopy.setConstraints(constraints);
			modelcopy.setOutputs(outputs);
			Models::FMTLpSolver* solver = modelcopy.getSolverPtr();
			solver->setColSolution(thesolution);
			Parser::FMTModelParser modelparser;
			const boost::filesystem::path filepath(solutionlocation + ".txt");
			const boost::filesystem::path folderpath = filepath.parent_path();
			modelparser.write(modelcopy, folderpath.string() + "/");
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::writeFinalModel", __LINE__, __FILE__);
		}

	}

	void FMTOpAreaSchedulerTask::getConstraintsSolution(std::vector<Core::FMTOutput>& outputs, std::vector<Core::FMTConstraint>& constraints) const
		{
		try {
			//output levels...
			const std::string bfecoptaggregates("~BFECOPTOUTPUTYOUVERT~");
			const int lastid = static_cast<int>(basemodel->getOutputs().size());
			size_t outoriginalsize = outputs.size();
			for (const Core::FMTOutput& output : bestscheduler->getLevelSolution("OPunit", bfecoptaggregates, lastid))
			{
				outputs.push_back(output);
			}
			for (size_t oid = outoriginalsize; oid < outputs.size();oid+=2)
				{
				Core::FMTOutput constraintoutput(outputs.at(oid));
				constraintoutput -= outputs.at(oid + 1);
				Core::FMTConstraint newconstraint(Core::FMTconstrainttype::FMTstandard, constraintoutput);
				newconstraint.setLength(1, basemodel->getParameter(Models::FMTintmodelparameters::LENGTH));
				newconstraint.setRhs(-std::numeric_limits<double>::max(), 0.0);
				constraints.push_back(newconstraint);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::getConstraintsSolution", __LINE__, __FILE__);
			}
		}


	void FMTOpAreaSchedulerTask::writeSolution() const
	{
		try {
			const double bestobjvalue = bestscheduler->getObjValue();
			const std::string relativevalue = std::to_string(static_cast<int>(std::abs(relax_objective - bestobjvalue) * 100 / relax_objective));
			*_logger << "Best solution found objective: "+std::to_string(bestobjvalue) +" ("+relativevalue+"%)" << "\n";
			Core::FMTYields yields;
			for (const Core::FMTTimeYieldHandler& tyld : bestscheduler->getSolution(outyldname))
				{
				std::unique_ptr<Core::FMTYieldHandler>newyield(new Core::FMTTimeYieldHandler(tyld));
				yields.push_back(newyield->getMask(),newyield);
				}
			yields.update();
			Parser::FMTYieldParser yldparser;
			const std::string solutionname = solutionlocation +"_"+ std::to_string(bestobjvalue) + "_" + relativevalue + ".yld";
			yldparser.write(yields, solutionname);
			std::vector<Core::FMTConstraint>constraints;
			std::vector<Core::FMTOutput>outputs;
			getConstraintsSolution(outputs, constraints);
			const std::string outputname = solutionlocation + "_" + std::to_string(bestobjvalue) + "_" + relativevalue + ".out";
			const std::string constraintName = solutionlocation + "_" + std::to_string(bestobjvalue) + "_" + relativevalue + ".opt";
			Parser::FMTOutputParser outparser;
			outparser.write(outputs, outputname);
			Parser::FMTOptimizationParser optparser;
			optparser.write(constraints, constraintName);

		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::writeSolution", __LINE__, __FILE__);
		}

	}



	void FMTOpAreaSchedulerTask::evaluateAndCopy()
	{
		try {
			if (!actualscheduler->isProvenOptimal())
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Cannot evaluate actual schedule",
					"FMTOpAreaSchedulerTask::evaluateAndCopy", __LINE__, __FILE__);
				}
				const boost::lock_guard<boost::mutex>lock(generalmutex);
				if (bestscheduler->empty())
				{
					*bestscheduler = *actualscheduler;
					if (!bestscheduler->isProvenOptimal())
					{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Non optimal best schedule copy",
							"FMTOpAreaSchedulerTask::evaluateAndCopy", __LINE__, __FILE__);
					}
				}else {
					if (!bestscheduler->isProvenOptimal())
					{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Cannot evaluate best schedule",
							"FMTOpAreaSchedulerTask::evaluateAndCopy", __LINE__, __FILE__);
					}
					const double sense = actualscheduler->getObjSense();
					const double actualobjective = actualscheduler->getObjValue();
					const double bestobjective = bestscheduler->getObjValue();
					if (bestobjective*sense > actualobjective*sense)
					{
						*bestscheduler = *actualscheduler;
					}else {
						*actualscheduler = *bestscheduler;
						actualscheduler->passInMessageHandler(*m_tasklogger.get());
					}
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::evaluateAndCopy", __LINE__, __FILE__);
			}
	}


	std::unique_ptr<FMTTask> FMTOpAreaSchedulerTask::clone() const
		{
		return std::unique_ptr<FMTTask>(new FMTOpAreaSchedulerTask(*this));
		}

	std::vector<std::unique_ptr<FMTTask>>FMTOpAreaSchedulerTask::split(const unsigned int& numberoftasks) const
	{
		std::vector<std::unique_ptr<FMTTask>> tasks;
		try {
			for (unsigned int taskid = 0 ; taskid < numberoftasks;++taskid)
				{
				tasks.push_back(std::move(std::unique_ptr<FMTTask>(new FMTOpAreaSchedulerTask(*this))));
				FMTOpAreaSchedulerTask* newtaskptr = dynamic_cast<FMTOpAreaSchedulerTask*>(tasks.back().get());
				newtaskptr->actualscheduler->setNumberOfThreads(1);
				if (taskid>0)
					{
					newtaskptr->actualscheduler->setAsRandom();
					}
				newtaskptr->actualscheduler->setGeneratorSeed(taskid);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::split", __LINE__, __FILE__);
			}
		return tasks;
	}

	std::unique_ptr<FMTTask>FMTOpAreaSchedulerTask::spawn()
	{
		try {
			if (goodToGo())
			{
				std::unique_ptr<FMTTask> newtask(new FMTOpAreaSchedulerTask(*this));
				FMTOpAreaSchedulerTask* newtaskptr = dynamic_cast<FMTOpAreaSchedulerTask*>(newtask.get());
				newtaskptr->actualscheduler->setNumberOfThreads(1);
				if (lastspawned > 0)
				{
					newtaskptr->actualscheduler->setAsRandom();
				}
				newtaskptr->actualscheduler->setGeneratorSeed(lastspawned);
				++lastspawned;
				return std::move(newtask);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::spawn", __LINE__, __FILE__);
			}
	return std::unique_ptr<FMTTask>(nullptr);
	}

	void FMTOpAreaSchedulerTask::passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger)
		{
		try {
			actualscheduler->passInLogger(logger);
			if (gotInitialSolution())
			{
				const boost::lock_guard<boost::mutex> lock(generalmutex);
				bestscheduler->passInLogger(logger);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTOpAreaSchedulerTask::passInLogger", __LINE__, __FILE__);
			}
		}


	void FMTOpAreaSchedulerTask::work()
	{
		try {
			//If you dont have initialsolution, you need to do an initialSolve
			actualscheduler->passInMessageHandler(*m_tasklogger.get());
			bool needinitialsolve = !gotInitialSolution();
			while (goodToGo())
			{
				if (needinitialsolve)
					{
					//If initialSolve fail, you need to redo an initialSolve
					needinitialsolve = !actualscheduler->initialSolve();
				}else {
					actualscheduler->greedyPass(relax_objective, iterations);
					}
				if (needinitialsolve) 
				{
					//But if in the mean time another thread got a solution, take it and go to greedy
					if (gotInitialSolution()) 
					{
						needinitialsolve = false;
						*actualscheduler = *bestscheduler;
						actualscheduler->passInMessageHandler(*m_tasklogger.get());
					}
				}
				else {
					evaluateAndCopy();
					//To only draw 1 operating area a the time
					actualscheduler->setProportionOfSet(0.00000000001);
				}
				if (iterations > 0)
				{
					--iterations;
				}
			}
			//setstatus(true);
		}catch (...)
		{
			_exhandler->raiseFromThreadCatch("","FMTOpAreaSchedulerTask::work", __LINE__, __FILE__);
		}

	}

}

#endif