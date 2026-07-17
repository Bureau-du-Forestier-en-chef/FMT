/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsesmodel.h"
#include "FMTforest.h"
#include "FMTexceptionhandler.h"


namespace Models
    {

    FMTSesModel::FMTSesModel(const FMTModel& rhs):
		FMTSeModel(rhs)
        {
		
        }

	FMTSesModel::FMTSesModel(const FMTSeModel& rhs):
		FMTSeModel(rhs)
	{

	}


	FMTSesModel::FMTSesModel(const FMTModel& rhs, const Spatial::FMTForest& forest) :
		FMTSeModel(rhs, forest)
	{
		
	}



	std::map<std::string, double> FMTSesModel::greedyReferenceBuild(
		const Core::FMTSchedule& p_schedule,
		size_t p_randomIterations,
		int p_seed,
		double p_tolerance)
	{
	std::map<std::string, double> bestresults;
	try {
		
		bestresults = FMTSeModel::greedyReferenceBuild(
			m_BestSolution, 
			p_schedule, 
			p_randomIterations, 
			p_seed, 
			p_tolerance);
		m_SpatialGraphs.deleteNonCompleteGraphs();
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTSesModel::greedyReferenceBuild", __LINE__, __FILE__);
			}
	return bestresults;
	}


	std::unique_ptr<FMTModel>FMTSesModel::clone() const
		{
		return std::unique_ptr<FMTModel>(new FMTSesModel(*this));
		}
	
	bool FMTSesModel::build(std::vector<Core::FMTSchedule> schedules)
	{
		try {
			for (const Core::FMTSchedule& schedule : schedules)
			{
				this->greedyReferenceBuild(schedule,getParameter(NUMBER_OF_ITERATIONS));
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSesModel::build", __LINE__, __FILE__);
		}
	return true;
	}

	void FMTSesModel::swapPtr(std::unique_ptr<FMTModel>& rhs)
	{
		*this = std::move(*dynamic_cast<FMTSesModel*>(rhs.get()));
	}

	std::unique_ptr<FMTModel>FMTSesModel::presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments) const
		{
		try {
			return std::unique_ptr<FMTModel>(new FMTSesModel(*(dynamic_cast<FMTSeModel*>(FMTSeModel::presolve(optionaldevelopments).get()))));
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTSesModel::presolve", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTModel>(nullptr);
		}

	std::unique_ptr<FMTModel>FMTSesModel::getCopy(int period) const
	{
		try {
			return std::unique_ptr<FMTModel>(new FMTSesModel(*dynamic_cast<FMTSeModel*>(FMTSeModel::getCopy(period).get())));
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSesModel::getCopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTModel>(nullptr);
	}


    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTSesModel)
