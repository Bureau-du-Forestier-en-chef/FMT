/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTplanningtask.hpp"
#include "FMTparallelwriter.hpp"
#include "FMTmodel.hpp"
#include "FMToutput.hpp"
#include "FMTschedule.hpp"


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
		allschedules(rhs.allschedules)
	{

		
	}

	FMTplanningtask& FMTplanningtask::operator = (const FMTplanningtask& rhs)
	{
		if (this!=&rhs)
		{
			resultswriter = rhs.resultswriter;
			models = copymodels(rhs.models);
			allschedules = rhs.allschedules;
		}
		return *this;
	}


	FMTplanningtask::FMTplanningtask(
		const int& minoutputperiod,
		const int& maxoutputperiod,
		const std::string& outputlocation,
		std::string gdaldriver,
		std::vector<std::string>creationoptions,
		Core::FMToutputlevel outputlevel):
		resultswriter(),
		models(),
		allschedules()
	{
		try {
			resultswriter = std::shared_ptr<FMTparallelwriter>(new FMTparallelwriter(outputlocation,gdaldriver,outputlevel, creationoptions,minoutputperiod,maxoutputperiod));
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTplanningtask::FMTplanningtask", __LINE__, __FILE__);
			}
	}

	void FMTplanningtask::push_back(const Models::FMTmodel& model,
		std::vector<Core::FMTschedule>schedules)
	{
		try {
			models.push_back(std::move(model.clone()));
			allschedules.push_back(schedules);
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
			int modelspertask = (static_cast<int>(models.size()) / numberoftasks);
			std::list<std::unique_ptr<Models::FMTmodel>>allmodels=copymodels(models);
			std::list<std::vector<Core::FMTschedule>>modelschedules=allschedules;
			for (size_t taskid = 0; taskid < numberoftasks; ++taskid)
				{
				FMTplanningtask newtask(*this);
				std::list<std::unique_ptr<Models::FMTmodel>>modelsoftask;
				std::list<std::vector<Core::FMTschedule>>schedulesoftask;
				for (int model = 0 ; model < modelspertask;++model)
					{
					modelsoftask.push_back(std::move(allmodels.front()->clone()));
					schedulesoftask.push_back(modelschedules.front());
					allmodels.pop_front();
					modelschedules.pop_front();
					}
				if ((taskid == numberoftasks - 1) && !allmodels.empty())
					{
					modelsoftask.push_back(std::move(allmodels.front()->clone()));
					schedulesoftask.push_back(modelschedules.front());
					allmodels.pop_front();
					modelschedules.pop_front();
					}
				newtask.models = copymodels(modelsoftask);
				newtask.allschedules = schedulesoftask;
				tasks.push_back(std::move(newtask.clone()));
				}
			
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTplanningtask::gettasks", __LINE__, __FILE__);
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
				singlemodel.push_back(std::move(models.front()->clone()));
				singleschedule.push_back(allschedules.front());
				models.pop_front();
				allschedules.pop_front();
				newtask.models.swap(singlemodel);
				newtask.allschedules.swap(singleschedule);
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


	void FMTplanningtask::work()
	{
		try {
			while (!models.empty())
			{
				_logger->logwithlevel("Thread:" + getthreadid() + " Planning of " + models.front()->getname() + " started\n",0);
				if (models.front()->doplanning(true, allschedules.front()))
					{
					resultswriter->getandwrite(models.front());
					}
				_logger->logwithlevel("Thread:" + getthreadid() + " Planning of " + models.front()->getname() + " done\n", 0);
				models.pop_front();
				allschedules.pop_front();
			}
			setstatus(true);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTplanningtask::work", __LINE__, __FILE__);
		}

	}

}