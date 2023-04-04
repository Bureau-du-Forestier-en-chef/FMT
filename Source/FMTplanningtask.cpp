/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTplanningtask.hpp"
#include "FMTparallelwriter.hpp"
#include "FMTmodel.hpp"
#include "FMToutput.hpp"
#include "FMTschedule.hpp"
#include "FMTexceptionhandler.hpp"


namespace Parallel
{

	std::list<std::unique_ptr<Models::FMTmodel>>FMTplanningtask::copymodels(const std::list<std::unique_ptr<Models::FMTmodel>>& tocopy) const
	{
		std::list<std::unique_ptr<Models::FMTmodel>>newmodels;
		try {
			for (const std::unique_ptr<Models::FMTmodel>& model : tocopy)
				{
				newmodels.push_back(std::move(model->clone()));
				}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTplanningtask::copymodels", __LINE__, __FILE__);
		}
		return newmodels;
	}


	FMTplanningtask::FMTplanningtask(const FMTplanningtask& rhs):
		resultswriter(rhs.resultswriter),
		models(copymodels(rhs.models)),
		allschedules(rhs.allschedules),
		outputs(rhs.outputs),
		keepmodels(rhs.keepmodels)
	{

		
	}

	FMTplanningtask& FMTplanningtask::operator = (const FMTplanningtask& rhs)
	{
		if (this!=&rhs)
		{
			resultswriter = rhs.resultswriter;
			models = copymodels(rhs.models);
			allschedules = rhs.allschedules;
			outputs = rhs.outputs;
			keepmodels = rhs.keepmodels;
		}
		return *this;
	}


	FMTplanningtask::FMTplanningtask(
		const int& minoutputperiod,
		const int& maxoutputperiod,
		const std::string& outputlocation,
		std::string gdaldriver,
		std::vector<std::string>creationoptions,
		Core::FMToutputlevel outputlevel,
		std::string primaryfilelocatiron):
		resultswriter(),
		models(),
		allschedules(),
		outputs(),
		keepmodels(false)
	{
		try {
			resultswriter = std::shared_ptr<FMTparallelwriter>(new FMTparallelwriter(outputlocation,gdaldriver,outputlevel, creationoptions,minoutputperiod,maxoutputperiod, primaryfilelocatiron));
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTplanningtask::FMTplanningtask", __LINE__, __FILE__);
			}
	}

	void FMTplanningtask::push_back(const Models::FMTmodel& model,
		std::vector<Core::FMTschedule>schedules, std::vector<Core::FMToutput>loutputs)
	{
		try {
			models.push_back(std::move(model.clone()));
			models.back()->setparallellogger(*tasklogger.get());
			allschedules.push_back(schedules);
			outputs.push_back(loutputs);
			resultswriter->setlayer(&model);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTplanningtask::push_back", __LINE__, __FILE__);
			}
	}


	std::unique_ptr<FMTtask> FMTplanningtask::clone() const
		{
		return std::unique_ptr<FMTtask>(new FMTplanningtask(*this));
		}

	std::vector<std::unique_ptr<FMTtask>>FMTplanningtask::split(const unsigned int& numberoftasks) const
	{
		std::vector<std::unique_ptr<FMTtask>>tasks;
		try {
			std::list<std::unique_ptr<Models::FMTmodel>>allmodels=copymodels(models);
			std::list<std::vector<Core::FMTschedule>>modelschedules(allschedules);
			std::list<std::vector<Core::FMToutput>>modeloutputs(outputs);
			for (const size_t tasksize : splitwork(numberoftasks, static_cast<int>(models.size())))
				{
				FMTplanningtask newtask(*this);
				std::list<std::unique_ptr<Models::FMTmodel>>modelsoftask;
				std::list<std::vector<Core::FMTschedule>>schedulesoftask;
				std::list<std::vector<Core::FMToutput>>outputsoftask;
				for (int model = 0; model < tasksize; ++model)
					{
					modelsoftask.push_back(std::move(allmodels.front()->clone()));
					schedulesoftask.push_back(modelschedules.front());
					outputsoftask.push_back(modeloutputs.front());
					allmodels.pop_front();
					modelschedules.pop_front();
					modeloutputs.pop_front();
					}
				newtask.models = copymodels(modelsoftask);
				newtask.allschedules = schedulesoftask;
				newtask.outputs = outputsoftask;
				tasks.push_back(std::move(newtask.clone()));
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTplanningtask::split", __LINE__, __FILE__);
			}
		return tasks;
	}

	std::unique_ptr<FMTtask>FMTplanningtask::spawn()
	{
		try {
			if (!models.empty())
				{
				FMTplanningtask newtask(*this);
				std::list<std::unique_ptr<Models::FMTmodel>>singlemodel;
				std::list<std::vector<Core::FMTschedule>>singleschedule;
				std::list<std::vector<Core::FMToutput>>singleoutputs;
				singlemodel.push_back(std::move(models.front()->clone()));
				singleschedule.push_back(allschedules.front());
				singleoutputs.push_back(outputs.front());
				models.pop_front();
				allschedules.pop_front();
				outputs.pop_front();
				newtask.models.swap(singlemodel);
				newtask.allschedules.swap(singleschedule);
				newtask.outputs.swap(singleoutputs);
				return std::move(newtask.clone());
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTplanningtask::spawn", __LINE__, __FILE__);
			}
	return std::unique_ptr<FMTtask>(nullptr);
	}

	void FMTplanningtask::passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger)
		{
		try {
			for (std::unique_ptr<Models::FMTmodel>& model : models)
				{
				model->passinlogger(logger);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTplanningtask::passinlogger", __LINE__, __FILE__);
			}
		}

	void FMTplanningtask::setkeepmodels()
		{
		keepmodels = true;
		}


	void FMTplanningtask::work()
	{
		try {
			std::list<std::unique_ptr<Models::FMTmodel>>modelskept;
			while (!models.empty())
			{
				_logger->logwithlevel("Thread:" + getthreadid() + " Planning of " + models.front()->getname() + " started\n",0);
				if (models.front()->doplanning(true, allschedules.front()))
					{
					resultswriter->getandwrite(models.front(),outputs.front());
					}
				_logger->logwithlevel("Thread:" + getthreadid() + " Planning of " + models.front()->getname() + " done\n", 0);
				if (keepmodels)
					{
					modelskept.push_back(std::move(models.front()));
					}
				models.pop_front();
				allschedules.pop_front();
				outputs.pop_front();
			}
			if (!modelskept.empty())
			{
				for (std::unique_ptr<Models::FMTmodel>& model : modelskept)
				{
					models.push_back(std::move(model));
				}
			}
			setstatus(true);
		}catch (...)
		{
			_exhandler->raisefromthreadcatch("","FMTplanningtask::work", __LINE__, __FILE__);
		}
	}

	void FMTplanningtask::finalize()
	{
		try {
		
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTplanningtask::finalize", __LINE__, __FILE__);
		}
	}

}