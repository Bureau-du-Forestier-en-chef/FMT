/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsemodel.hpp"
#include "FMTforest.hpp"
#include "FMTexceptionhandler.hpp"


namespace Models
    {
    FMTsemodel::FMTsemodel(): FMTmodel(),solution()
        {

        }
    FMTsemodel::FMTsemodel(const FMTsemodel& rhs):
        FMTmodel(rhs),
		solution(rhs.solution)
        {

        }

	FMTsemodel::FMTsemodel(const FMTmodel& rhs, const Spatial::FMTforest& forest) :
		FMTmodel(rhs), solution(forest)
	{

	}

    FMTsemodel::FMTsemodel(const FMTmodel& rhs):
        FMTmodel(rhs), solution()
        {

        }
    FMTsemodel& FMTsemodel::operator = (const FMTsemodel& rhs)
        {
        if (this!=&rhs)
            {
            FMTmodel::operator = (rhs);
			solution = rhs.solution;
            }
        return *this;
        }

	std::vector<Core::FMTschedule> FMTsemodel::getschedule(bool withlock) const
	{
		try
		{
			return solution.getschedules(actions,withlock);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getschedulesp", __LINE__, __FILE__);
		}
		return std::vector<Core::FMTschedule>();
	}
	bool FMTsemodel::setinitialmapping(Spatial::FMTforest forest)
        {
		try {
			solution = Spatial::FMTspatialschedule(forest);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::setinitialmapping", __LINE__, __FILE__);
		}

		return true;
        }

	std::map<std::string, double> FMTsemodel::getoutput(const Core::FMToutput& output,
		int period, Core::FMToutputlevel level) const
	{
		std::map<std::string, double>values;
		try {
			const std::map<std::string, std::vector<double>>periodvalues = solution.getoutput(*this, output, period, period, level);
			for (std::map<std::string, std::vector<double>>::const_iterator it = periodvalues.begin();it!=periodvalues.end();it++)
				{
				values[it->first] = *it->second.begin();
				}

		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsemodel::getoutput", __LINE__, __FILE__);
			}
		return values;
	}

	Core::FMTschedule FMTsemodel::getsolution(int period, bool withlock) const
	{
		Core::FMTschedule baseschedule;
		try {
			const std::vector<Core::FMTschedule> allschedules = solution.getschedules(actions,withlock);
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
			_exhandler->printexceptions("", "FMTsemodel::getsolution", __LINE__, __FILE__);
			}
		return baseschedule;
	}


	std::string FMTsemodel::getdisturbancestats() const
	{
		return solution.getpatchstats(actions);
	}

	std::unique_ptr<FMTmodel>FMTsemodel::presolve(
		std::vector<Core::FMTactualdevelopment> optionaldevelopments ) const
		{
		try {
			if (solution.actperiod() == 1)//just presolve if no solution
			{
				const std::vector<Core::FMTactualdevelopment>areas = solution.getforestperiod(0).getarea();
				optionaldevelopments.insert(optionaldevelopments.end(), areas.begin(), areas.end());
				std::unique_ptr<FMTmodel>presolvedmod(new FMTsemodel(*(FMTmodel::presolve(optionaldevelopments))));
				FMTsemodel*presolvedses = dynamic_cast<FMTsemodel*>(presolvedmod.get());
				Core::FMTmaskfilter presolvefilter = presolvedses->getpresolvefilter(themes);
				const Core::FMTmask basemask = this->getbasemask(optionaldevelopments);
				const boost::dynamic_bitset<>&bitsets = basemask.getbitsetreference();
				presolvedses->solution = Spatial::FMTspatialschedule(solution.getforestperiod(0).presolve(presolvefilter, presolvedses->themes));
				return presolvedmod;
			}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsemodel::presolve", __LINE__, __FILE__);
			}
		return std::unique_ptr<FMTmodel>(nullptr);
		}

	void FMTsemodel::postsolve(const FMTmodel& originalbasemodel)
		{
		try {
			if (solution.actperiod()>=1)//just postsolve if you have a solution
			{
				const Core::FMTmaskfilter presolvedmask = this->getpostsolvefilter(originalbasemodel.getthemes(), originalbasemodel.getarea().begin()->getmask());
				solution.postsolve(presolvedmask,this->getactions(),originalbasemodel);
				FMTmodel::postsolve(originalbasemodel);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsemodel::postsolve", __LINE__, __FILE__);
			}
		}

	Spatial::FMTforest FMTsemodel::getmapping() const
		{
		try {
			return solution.getforestperiod(solution.actperiod()-1);
		}
		catch (...)
			{
			_exhandler->printexceptions("", "FMTsemodel::getmapping", __LINE__, __FILE__);
			}
		return Spatial::FMTforest();
		}

	std::unique_ptr<FMTmodel>FMTsemodel::clone() const
		{
		return std::unique_ptr<FMTmodel>(new FMTsemodel(*this));
		}

	void FMTsemodel::swap_ptr(const std::unique_ptr<FMTmodel>& rhs)
	{
		*this = std::move(*dynamic_cast<FMTsemodel*>(rhs.get()));
	}


	std::vector<Core::FMTactualdevelopment>FMTsemodel::getarea(int period, bool beforegrowanddeath) const
	{
		try {
			return solution.getforestperiod(period, beforegrowanddeath).getarea();
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getarea", __LINE__, __FILE__);
		}
	return std::vector<Core::FMTactualdevelopment>();
	}

	FMTmodel FMTsemodel::getcopy(int period) const
	{
		try {
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Not implemented", "FMTsemodel::getcopy", __LINE__, __FILE__);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getcopy", __LINE__, __FILE__);
		}
		return FMTmodel();
	}

    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsemodel)