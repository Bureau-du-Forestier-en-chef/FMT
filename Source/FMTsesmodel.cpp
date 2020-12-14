/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsesmodel.h"

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

	std::vector<Core::FMTschedule> FMTsesmodel::getschedule() const
	{
		try
		{
			return spschedule.getschedules(actions);
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
			forest.passinobject(*this);
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
		int period, Graph::FMToutputlevel level) const
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

	std::string FMTsesmodel::getdisturbancestats() const
	{
		return spschedule.getpatchstats(actions);
	}

	std::unique_ptr<FMTmodel>FMTsesmodel::presolve(int presolvepass,
		std::vector<Core::FMTactualdevelopment> optionaldevelopments ) const
		{
		try {
			if (spschedule.actperiod() == 1)//just presolve if no solution
			{
				const std::vector<Core::FMTactualdevelopment>areas = spschedule.getforestperiod(0).getarea();
				optionaldevelopments.insert(optionaldevelopments.end(), areas.begin(), areas.end());
				std::unique_ptr<FMTmodel>presolvedmod(new FMTsesmodel(*(FMTmodel::presolve(presolvepass, optionaldevelopments))));
				FMTsesmodel*presolvedses = dynamic_cast<FMTsesmodel*>(presolvedmod.get());
				const Core::FMTmask presolvedmask = presolvedses->getselectedmask(themes);
				const Core::FMTmask basemask = this->getbasemask(optionaldevelopments);
				const boost::dynamic_bitset<>&bitsets = basemask.getbitsetreference();
				presolvedses->spschedule = Spatial::FMTspatialschedule(spschedule.getforestperiod(0).presolve(presolvedmask, presolvedses->themes));
				return presolvedmod;
			}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsesmodel::presolve", __LINE__, __FILE__);
			}
		return std::unique_ptr<FMTmodel>(nullptr);
		}

	std::unique_ptr<FMTmodel>FMTsesmodel::postsolve(const FMTmodel& originalbasemodel) const
		{
		try {
			if (spschedule.actperiod()>=1)//just postsolve if you have a solution
			{
				std::unique_ptr<FMTmodel>presolvedmod(new FMTsesmodel(*(FMTmodel::postsolve(originalbasemodel))));
				const Core::FMTmask presolvedmask = this->getselectedmask(themes);
				FMTsesmodel*postsolvedses = dynamic_cast<FMTsesmodel*>(presolvedmod.get());
				//postsolvedses->mapping = this->mapping.postsolve(presolvedmask, postsolvedses->themes);
				//Disturbance stack doesn't need changes
				//take care of the FMTspatialactions
				return presolvedmod;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsesmodel::presolve", __LINE__, __FILE__);
			}
		return std::unique_ptr<FMTmodel>(nullptr);
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

	void FMTsesmodel::passinobject(const Core::FMTobject& rhs)
		{
		FMTmodel::passinobject(rhs);
		spschedule.passinobject(rhs);
		}

    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsesmodel)
