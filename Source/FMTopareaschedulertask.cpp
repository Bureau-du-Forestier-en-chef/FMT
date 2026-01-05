/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifdef FMTWITHOSI
#include "FMTopareaschedulertask.h"
#include "FMTyieldparser.h"
#include "FMTlpmodel.h"
#include "FMToutputnode.h"
#include "FMToperatingareascheme.h"
#include "FMToperatingareascheduler.h"
#include "FMTtimeyieldhandler.h"
#include <limits>
#include <boost/thread.hpp>
#include "FMTexceptionhandler.h"
#include "FMTscheduleparser.h"
#include "FMTmodelparser.h"
#include "FMTareaparser.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include "FMToutput.h"
#include "FMTconstraint.h"
#include "FMToutputparser.h"
#include "FMToptimizationparser.h"

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

	std::vector<Heuristics::FMToperatingareascheme> FMTopareaschedulertask::getreturntimefromoutput(
		Models::FMTlpmodel& model,
		const std::vector<Heuristics::FMToperatingareascheme>& opareas,
		const Core::FMToutput& output) const
	{
		std::vector<Heuristics::FMToperatingareascheme>newschemes;
		try {
			const std::vector<Core::FMTtheme>themes = model.getthemes();
			const int model_length = model.getparameter(Models::FMTintmodelparameters::LENGTH);
			for (const Heuristics::FMToperatingareascheme& opscheduler : opareas)
				{
				double total_value = 0;
				const Core::FMToutput local_output = output.intersectwithmask(opscheduler.getmask(), themes);
				double non_zero = 0;
				for (int period = 0; period < model_length; ++period)
					{
					const double output_value = model.getoutput(local_output, period, Core::FMToutputlevel::totalonly).at("Total");
					if (output_value > FMT_DBL_TOLERANCE)
						{
						total_value += output_value;
						non_zero += 1;
						}
					}
				Heuristics::FMToperatingareascheme new_scheme(opscheduler);
				if (non_zero>0)
				{
					const size_t return_time = static_cast<size_t>(std::round(total_value/non_zero));
					const size_t diff = (new_scheme.getminimalreturntime() - new_scheme.getmaximalreturntime()) / 2;
					const size_t LowerReturn = return_time - diff;
					const size_t UpperReturn = return_time + diff;
					if (LowerReturn == 0 ||
						UpperReturn > std::numeric_limits<short>::max())
						{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
							"Wrong returntime value for "+ std::string(opscheduler.getmask())+" values "+
							std::to_string(LowerReturn)+","+ std::to_string(UpperReturn),
							"FMTopareaschedulertask::getreturntimefromoutput", __LINE__, __FILE__);
						}
					new_scheme.setreturntime(return_time - diff, return_time + diff);
					}
					newschemes.push_back(new_scheme);
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::getreturntimefromoutput", __LINE__, __FILE__);
		}
		return newschemes;
	}

	void FMTopareaschedulertask::setinitialscheduler(Models::FMTlpmodel& model,
		const std::vector<Heuristics::FMToperatingareascheme>& opareas, const Core::FMToutputnode& node)
	{
		try {
			const std::vector<Heuristics::FMToperatingareascheduler>heuristics = model.getoperatingareaschedulerheuristics(opareas, node);
			actualscheduler = std::move(std::unique_ptr<Heuristics::FMToperatingareascheduler>(new Heuristics::FMToperatingareascheduler(heuristics.at(0))));
			const double calculatedpropotion = actualscheduler->generateinitialproportionofset();
			* _logger << "Initial proportion of set of : " + std::to_string(calculatedpropotion) << "\n";
			actualscheduler->setproportionofset(calculatedpropotion);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::setinitialscheduler", __LINE__, __FILE__);
		}
	}



	std::chrono::time_point<std::chrono::high_resolution_clock> FMTopareaschedulertask::getstoppoint(const int& timegap) const
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> then;
		try {
			then= getclock();
			then += std::chrono::seconds(timegap);
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
		const int& maxtime,
		Core::FMToutput returntime_output):
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
			solveinitialmodel(modelcopy);
			if (!returntime_output.empty())
				{
				const std::vector<Heuristics::FMToperatingareascheme> newschemes = getreturntimefromoutput(modelcopy, opareas, returntime_output);
				Parser::FMTareaparser area_parser;
				const std::string location = (outputlocation +"/"+returntime_output.getname() + ".csv");
				std::vector<std::string>layersoptions;
				layersoptions.push_back("SEPARATOR=SEMICOLON");
				layersoptions.push_back("STRING_QUOTING=IF_NEEDED");
				#ifdef FMTWITHGDAL
					area_parser.writeOAschedulerparameters(location, newschemes, layersoptions);
				#endif
				setinitialscheduler(modelcopy, newschemes, node);
				}else {
				setinitialscheduler(modelcopy, opareas, node);
				}
			relax_objective = actualscheduler->getObjValue();
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

	void FMTopareaschedulertask::finalize()
		{
		try {
			if (gotinitialsolution())
			{
				if (!bestscheduler->isProvenOptimal())
				{
					_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
						"Non optimal best scheduler",
						"FMTopareaschedulertask::finalize", __LINE__, __FILE__);
				}
				else {
					writesolution();
					writefinalmodel();
				}
				bestscheduler = std::shared_ptr<Heuristics::FMToperatingareascheduler>(nullptr);
			}

		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::finalize", __LINE__, __FILE__);
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
			std::vector<Core::FMTconstraint>constraints=basemodel->getconstraints();
			std::vector<Core::FMToutput>outputs=basemodel->getoutputs();
			getconstraintssolution(outputs, constraints);
			modelcopy.setconstraints(constraints);
			modelcopy.setoutputs(outputs);
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

	void FMTopareaschedulertask::getconstraintssolution(std::vector<Core::FMToutput>& outputs, std::vector<Core::FMTconstraint>& constraints) const
		{
		try {
			//output levels...
			const std::string bfecoptaggregates("~BFECOPTOUTPUTYOUVERT~");
			const int lastid = static_cast<int>(basemodel->getoutputs().size());
			size_t outoriginalsize = outputs.size();
			for (const Core::FMToutput& output : bestscheduler->getlevelsolution("OPunit", bfecoptaggregates, lastid))
			{
				outputs.push_back(output);
			}
			for (size_t oid = outoriginalsize; oid < outputs.size();oid+=2)
				{
				Core::FMToutput constraintoutput(outputs.at(oid));
				constraintoutput -= outputs.at(oid + 1);
				Core::FMTconstraint newconstraint(Core::FMTconstrainttype::FMTstandard, constraintoutput);
				newconstraint.setlength(1, basemodel->getparameter(Models::FMTintmodelparameters::LENGTH));
				newconstraint.setrhs(-std::numeric_limits<double>::max(), 0.0);
				constraints.push_back(newconstraint);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTopareaschedulertask::getconstraintssolution", __LINE__, __FILE__);
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
			std::vector<Core::FMTconstraint>constraints;
			std::vector<Core::FMToutput>outputs;
			getconstraintssolution(outputs, constraints);
			const std::string outputname = solutionlocation + "_" + std::to_string(bestobjvalue) + "_" + relativevalue + ".out";
			const std::string constraintname = solutionlocation + "_" + std::to_string(bestobjvalue) + "_" + relativevalue + ".opt";
			Parser::FMToutputparser outparser;
			outparser.write(outputs, outputname);
			Parser::FMToptimizationparser optparser;
			optparser.write(constraints, constraintname);

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
		std::vector<std::unique_ptr<FMTtask>> tasks;
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
				std::unique_ptr<FMTtask> newtask(new FMTopareaschedulertask(*this));
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

	void FMTopareaschedulertask::passinlogger(const std::unique_ptr<Logging::FMTlogger>& logger)
		{
		try {
			actualscheduler->passinlogger(logger);
			if (gotinitialsolution())
			{
				const boost::lock_guard<boost::mutex> lock(generalmutex);
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
					actualscheduler->greedypass(relax_objective, iterations);
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
			//setstatus(true);
		}catch (...)
		{
			_exhandler->raisefromthreadcatch("","FMTopareaschedulertask::work", __LINE__, __FILE__);
		}

	}

}

#endif