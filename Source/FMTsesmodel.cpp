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

    FMTsesmodel::FMTsesmodel(const FMTmodel& rhs):
		FMTsemodel(rhs)
        {
		
        }

	FMTsesmodel::FMTsesmodel(const FMTsemodel& rhs):
		FMTsemodel(rhs)
	{

	}


	FMTsesmodel::FMTsesmodel(const FMTmodel& rhs, const Spatial::FMTforest& forest) :
		FMTsemodel(rhs, forest)
	{
		
	}



	std::map<std::string, double> FMTsesmodel::greedyReferenceBuild(
		const Core::FMTSchedule& p_schedule,
		size_t p_randomIterations,
		int p_seed,
		double p_tolerance)
	{
	std::map<std::string, double> bestresults;
	try {
		
		bestresults = FMTsemodel::greedyReferenceBuild(
			m_BestSolution, 
			p_schedule, 
			p_randomIterations, 
			p_seed, 
			p_tolerance);
		m_SpatialGraphs.deleteNonCompleteGraphs();
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTsesmodel::greedyReferenceBuild", __LINE__, __FILE__);
			}
	return bestresults;
	}


	std::unique_ptr<FMTmodel>FMTsesmodel::clone() const
		{
		return std::unique_ptr<FMTmodel>(new FMTsesmodel(*this));
		}
	
	bool FMTsesmodel::build(std::vector<Core::FMTSchedule> schedules)
	{
		try {
			for (const Core::FMTSchedule& schedule : schedules)
			{
				this->greedyReferenceBuild(schedule,getParameter(NUMBER_OF_ITERATIONS));
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTsesmodel::build", __LINE__, __FILE__);
		}
	return true;
	}

	void FMTsesmodel::swapPtr(std::unique_ptr<FMTmodel>& rhs)
	{
		*this = std::move(*dynamic_cast<FMTsesmodel*>(rhs.get()));
	}

	std::unique_ptr<FMTmodel>FMTsesmodel::presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments) const
		{
		try {
			return std::unique_ptr<FMTmodel>(new FMTsesmodel(*(dynamic_cast<FMTsemodel*>(FMTsemodel::presolve(optionaldevelopments).get()))));
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTsesmodel::presolve", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
		}

	std::unique_ptr<FMTmodel>FMTsesmodel::getCopy(int period) const
	{
		try {
			return std::unique_ptr<FMTmodel>(new FMTsesmodel(*dynamic_cast<FMTsemodel*>(FMTsemodel::getCopy(period).get())));
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTsesmodel::getCopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
	}


    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsesmodel)
