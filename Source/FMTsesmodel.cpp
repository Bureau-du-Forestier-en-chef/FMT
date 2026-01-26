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



	std::map<std::string, double> FMTsesmodel::GreedyReferenceBuild(const Core::FMTschedule& p_schedule,
																	size_t p_randomIterations,
																	int p_seed,
																	double p_tolerance)
	{
	std::map<std::string, double>bestresults;
	try {
		
		bestresults = FMTsemodel::GreedyReferenceBuild(m_BestSolution,p_schedule,p_randomIterations, p_seed, p_tolerance);
		m_SpatialGraphs.DeleteNonCompleteGraphs();
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsesmodel::GreedyReferenceBuild", __LINE__, __FILE__);
			}
	return bestresults;
	}


	std::unique_ptr<FMTmodel>FMTsesmodel::clone() const
		{
		return std::unique_ptr<FMTmodel>(new FMTsesmodel(*this));
		}
	
	bool FMTsesmodel::build(std::vector<Core::FMTschedule> schedules)
	{
		try {
			for (const Core::FMTschedule& schedule : schedules)
			{
				this->GreedyReferenceBuild(schedule,getparameter(NUMBER_OF_ITERATIONS));
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::build", __LINE__, __FILE__);
		}
	return true;
	}

	void FMTsesmodel::swap_ptr(std::unique_ptr<FMTmodel>& rhs)
	{
		*this = std::move(*dynamic_cast<FMTsesmodel*>(rhs.get()));
	}

	std::unique_ptr<FMTmodel>FMTsesmodel::presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments) const
		{
		try {
			return std::unique_ptr<FMTmodel>(new FMTsesmodel(*(dynamic_cast<FMTsemodel*>(FMTsemodel::presolve(optionaldevelopments).get()))));
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::presolve", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
		}

	std::unique_ptr<FMTmodel>FMTsesmodel::getcopy(int period) const
	{
		try {
			return std::unique_ptr<FMTmodel>(new FMTsesmodel(*dynamic_cast<FMTsemodel*>(FMTsemodel::getcopy(period).get())));
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::getcopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
	}


    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsesmodel)
