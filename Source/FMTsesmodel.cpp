/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsesmodel.hpp"
#include "FMTforest.hpp"
#include "FMTexceptionhandler.hpp"


namespace Models
    {
    FMTsesmodel::FMTsesmodel(): FMTsemodel()
        {

        }
    FMTsesmodel::FMTsesmodel(const FMTsesmodel& rhs):
		FMTsemodel(rhs)
        {

        }
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

    FMTsesmodel& FMTsesmodel::operator = (const FMTsesmodel& rhs)
        {
        if (this!=&rhs)
            {
            FMTsemodel::operator = (rhs);
            }
        return *this;
        }


	std::map<std::string, double> FMTsesmodel::greedyreferencebuild(const Core::FMTschedule& schedule,
																	const size_t& randomiterations,
																	unsigned int seed,
																	double tolerance)
	{
	std::map<std::string, double>bestresults;
	try {
		bestresults = solution.greedyreferencebuild(schedule, *this, randomiterations, seed, tolerance);
		}catch (...)
		{
		_exhandler->printexceptions("", "FMTsesmodel::greedyreferencebuild", __LINE__, __FILE__);
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
				this->greedyreferencebuild(schedule,getparameter(NUMBER_OF_ITERATIONS));
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::build", __LINE__, __FILE__);
		}
	return true;
	}

	void FMTsesmodel::swap_ptr(const std::unique_ptr<FMTmodel>& rhs)
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
