/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsesmodel.hpp"
#include "FMTforest.hpp"


namespace Models
    {
    FMTsesmodel::FMTsesmodel(): FMTmodel(),spschedule()
        {

        }
    FMTsesmodel::FMTsesmodel(const FMTsesmodel& rhs):
        FMTmodel(rhs),
		spschedule(rhs.spschedule)
        {

        }
    FMTsesmodel::FMTsesmodel(const FMTmodel& rhs):
        FMTmodel(rhs),spschedule()
        {

        }
    FMTsesmodel& FMTsesmodel::operator = (const FMTsesmodel& rhs)
        {
        if (this!=&rhs)
            {
            FMTmodel::operator = (rhs);
			spschedule = rhs.spschedule;
            }
        return *this;
        }

	std::vector<Core::FMTschedule> FMTsesmodel::getschedule(bool withlock) const
	{
		try
		{
			return spschedule.getschedules(actions,withlock);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::getschedulesp", __LINE__, __FILE__);
		}
		return std::vector<Core::FMTschedule>();
	}
	bool FMTsesmodel::setinitialmapping(Spatial::FMTforest forest)
        {
		try {
			//forest.passinobject(*this);
			spschedule = Spatial::FMTspatialschedule(forest);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsesmodel::setinitialmapping", __LINE__, __FILE__);
		}

		return true;
        }

	std::map<std::string, double> FMTsesmodel::greedyreferencebuild(const Core::FMTschedule& schedule,
																	const size_t& randomiterations,
																	unsigned int seed,
																	double tolerance)
	{
	std::map<std::string, double>bestresults;
	try {
		bestresults = spschedule.greedyreferencebuild(schedule, *this, randomiterations, seed, tolerance);
		}catch (...)
		{
		_exhandler->printexceptions("", "FMTsesmodel::greedyreferencebuild", __LINE__, __FILE__);
		}
	return bestresults;
	}

	std::map<std::string, double> FMTsesmodel::getoutput(const Core::FMToutput& output,
		int period, Core::FMToutputlevel level) const
	{
		std::map<std::string, double>values;
		try {
			const std::map<std::string, std::vector<double>>periodvalues = spschedule.getoutput(*this, output, period, period, level);
			for (std::map<std::string, std::vector<double>>::const_iterator it = periodvalues.begin();it!=periodvalues.end();it++)
				{
				values[it->first] = *it->second.begin();
				}

		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsesmodel::getoutput", __LINE__, __FILE__);
			}
		return values;
	}

	Core::FMTschedule FMTsesmodel::getsolution(int period, bool withlock) const
	{
		Core::FMTschedule baseschedule;
		try {
			const std::vector<Core::FMTschedule> allschedules = spschedule.getschedules(actions,withlock);
			size_t scheduleid = 0;
			while (scheduleid < allschedules.size())
			{
				if (allschedules.at(scheduleid).getperiod()==period)
					{
					baseschedule = allschedules.at(scheduleid);
					break;
					}
				++scheduleid;
			}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsesmodel::getsolution", __LINE__, __FILE__);
			}
		return baseschedule;
	}


	std::string FMTsesmodel::getdisturbancestats() const
	{
		return spschedule.getpatchstats(actions);
	}

	std::unique_ptr<FMTmodel>FMTsesmodel::presolve(
		std::vector<Core::FMTactualdevelopment> optionaldevelopments ) const
		{
		try {
			if (spschedule.actperiod() == 1)//just presolve if no solution
			{
				const std::vector<Core::FMTactualdevelopment>areas = spschedule.getforestperiod(0).getarea();
				optionaldevelopments.insert(optionaldevelopments.end(), areas.begin(), areas.end());
				std::unique_ptr<FMTmodel>presolvedmod(new FMTsesmodel(*(FMTmodel::presolve(optionaldevelopments))));
				FMTsesmodel*presolvedses = dynamic_cast<FMTsesmodel*>(presolvedmod.get());
				Core::FMTmaskfilter presolvefilter = presolvedses->getpresolvefilter(themes);
				const Core::FMTmask basemask = this->getbasemask(optionaldevelopments);
				const boost::dynamic_bitset<>&bitsets = basemask.getbitsetreference();
				presolvedses->spschedule = Spatial::FMTspatialschedule(spschedule.getforestperiod(0).presolve(presolvefilter, presolvedses->themes));
				return presolvedmod;
			}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsesmodel::presolve", __LINE__, __FILE__);
			}
		return std::unique_ptr<FMTmodel>(nullptr);
		}

	void FMTsesmodel::postsolve(const FMTmodel& originalbasemodel)
		{
		try {
			if (spschedule.actperiod()>=1)//just postsolve if you have a solution
			{
				const Core::FMTmaskfilter presolvedmask = this->getpostsolvefilter(originalbasemodel.getthemes(), originalbasemodel.getarea().begin()->getmask());
				spschedule.postsolve(presolvedmask,this->getactions(),originalbasemodel);
				FMTmodel::postsolve(originalbasemodel);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsesmodel::presolve", __LINE__, __FILE__);
			}
		}

	Spatial::FMTforest FMTsesmodel::getmapping() const
		{
		try {
			return spschedule.getforestperiod(spschedule.actperiod()-1);
		}
		catch (...)
			{
			_exhandler->printexceptions("", "FMTsesmodel::getmapping", __LINE__, __FILE__);
			}
		return Spatial::FMTforest();
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
			_exhandler->printexceptions("", "FMTsesmodel::doplanning", __LINE__, __FILE__);
		}
	return true;
	}

	void FMTsesmodel::swap_ptr(const std::unique_ptr<FMTmodel>& rhs)
	{
		*this = std::move(*dynamic_cast<FMTsesmodel*>(rhs.get()));
	}

    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsesmodel)
