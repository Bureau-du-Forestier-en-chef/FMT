/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsemodel.h"
#include "FMTforest.h"
#include "FMTexceptionhandler.h"
#include "FMTlayer.hpp"


namespace Models
    {
    FMTsemodel::FMTsemodel(): FMTmodel(),solution(), m_staticMaskMemorize()
        {

        }
    FMTsemodel::FMTsemodel(const FMTsemodel& rhs):
        FMTmodel(rhs),
		solution(rhs.solution),
		m_staticMaskMemorize(rhs.m_staticMaskMemorize)
        {

        }

	FMTsemodel::FMTsemodel(const FMTmodel& rhs, const Spatial::FMTforest& forest) :
		FMTmodel(rhs), solution(forest, static_cast<size_t>(getparameter(FMTintmodelparameters::LENGTH)+2)),
		m_staticMaskMemorize()
	{

	}

    FMTsemodel::FMTsemodel(const FMTmodel& rhs):
        FMTmodel(rhs), solution(), m_staticMaskMemorize()
        {

        }
    FMTsemodel& FMTsemodel::operator = (const FMTsemodel& rhs)
        {
        if (this!=&rhs)
            {
            FMTmodel::operator = (rhs);
			solution = rhs.solution;
			m_staticMaskMemorize = rhs.m_staticMaskMemorize;
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
	bool FMTsemodel::setinitialmapping(const Spatial::FMTforest& forest)
        {
		try {
			const size_t LENGTH = static_cast<size_t>(getparameter(FMTintmodelparameters::LENGTH)+2);
			Spatial::FMTspatialschedule newSolution(forest, LENGTH);
			solution.swap(newSolution);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::setinitialmapping", __LINE__, __FILE__);
		}
		return true;
        }

	void FMTsemodel::LogConstraintsInfeasibilities() const
	{
		try {
			size_t cid = 0;
			double brokenup = 0;
			double total = 0;
			*_logger << "Constraints infeasibilities report: " << "\n";
			const std::vector<Core::FMTconstraint>constraints = getconstraints();
			for (double value : solution.getconstraintsvalues(*this))
			{
				if (cid > 0 && !constraints.at(cid).isspatial())
				{
					if (value > 0)
					{
						if (constraints.at(cid).isgoal())
						{
							double GoalValue;
							std::string GoalName;
							constraints.at(cid).getgoal(GoalName, GoalValue);
							if (GoalName=="_WEIGHT")
								{
								value /= GoalValue;
								}
						}
						std::string constraintname = std::string(constraints.at(cid));
						std::replace(constraintname.begin(), constraintname.end(), '\n', ' ');
						constraintname += ("(" + std::to_string(static_cast<int>(value))+")");
						*_logger << constraintname << "\n";
						++brokenup;
					}
					++total;
				}
				++cid;
			}
			double ratio = 0;
			if (brokenup > 0)
			{
				ratio = (brokenup / total) * 100;
			}
			const std::string brisglobal = "Percentage of infeasible constraints " + std::to_string(static_cast<int>(ratio)) + " %";
			*_logger << brisglobal << "\n";
			*_logger << "Constraints infeasibilities report done" << "\n";
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::LogConstraintsInfeasibilities", __LINE__, __FILE__);
		}
	}

	void FMTsemodel::LogConstraintsFactors() const
	{
		try {
			*_logger << "Constraints factor report" << "\n";
			const std::vector<double>factors = solution.getconstraintsfactor();
			size_t constraintid = 0;
			for (const double& value : factors)
				{
				std::string constraintname = constraints.at(constraintid);
				std::replace(constraintname.begin(), constraintname.end(), '\n', ' ');
				constraintname+= " (" + std::to_string(value)+")";
				*_logger << constraintname << "\n";
				++constraintid;
				}
			*_logger << "Constraints factor report done" << "\n";
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::LogConstraintsFactors", __LINE__, __FILE__);
		}
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

	Spatial::FMTlayer<double> FMTsemodel::getspatialoutput(const Core::FMToutput& output, int period) const
	{
		try {
			return solution.getoutput(*this, output, period);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getoutput", __LINE__, __FILE__);
		}
		return Spatial::FMTlayer<double>();
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
				//const std::vector<Core::FMTactualdevelopment>areas = solution.getforestperiod(0).getarea();
				const std::vector<Core::FMTactualdevelopment>areas = solution.getarea();
				optionaldevelopments.insert(optionaldevelopments.end(), areas.begin(), areas.end());
				std::unique_ptr<FMTmodel>presolvedmod(new FMTsemodel(*(FMTmodel::presolve(optionaldevelopments))));
				FMTsemodel*presolvedses = dynamic_cast<FMTsemodel*>(presolvedmod.get());
				Core::FMTmaskfilter presolveFilter = presolvedses->getpresolvefilter(themes);
				const Core::FMTmask baseMask = this->getbasemask(optionaldevelopments);
				const boost::dynamic_bitset<uint8_t>&bitsets = baseMask.getbitsetreference();
				//presolvedses->solution = Spatial::FMTspatialschedule(solution.getforestperiod(0).presolve(presolvefilter, presolvedses->themes));
				const size_t LENGTH = static_cast<size_t>(getparameter(FMTintmodelparameters::LENGTH) + 2);
				Spatial::FMTspatialschedule presolvedSolution = solution.presolve(presolveFilter, presolvedses->themes, LENGTH);
				presolvedses->solution.swap(presolvedSolution);
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

	void FMTsemodel::swap_ptr(std::unique_ptr<FMTmodel>& rhs)
	{
		*this = std::move(*dynamic_cast<FMTsemodel*>(rhs.get()));
	}


	std::vector<Core::FMTactualdevelopment>FMTsemodel::getarea(int period, bool beforegrowanddeath) const
	{
		try {
			return solution.getarea(period, beforegrowanddeath);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getarea", __LINE__, __FILE__);
		}
	return std::vector<Core::FMTactualdevelopment>();
	}

	std::unique_ptr<FMTmodel> FMTsemodel::getcopy(int period) const
	{
		try {
			return std::unique_ptr<FMTmodel>(new FMTsemodel(*this, solution.getforestperiod(period)));
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getcopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
	}

	double FMTsemodel::getobjectivevalue() const
	{
		double value = std::numeric_limits<double>::quiet_NaN();
		try {
			value = solution.getglobalobjective(*this);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getobjectivevalue", __LINE__, __FILE__);
		}
		return value;
	}

	Core::FMTmask FMTsemodel::getstaticmask(const Core::FMToutputnode& node, bool ignoreoutputvariables) const
		{
			Core::FMTmask staticMask;
			try {
				boost::unordered_map<Core::FMToutputsource, Core::FMTmask>::const_iterator lookUp = m_staticMaskMemorize.find(node.source);
				if (lookUp!= m_staticMaskMemorize.end())
					{
					staticMask = lookUp->second;
				}else {
					staticMask = FMTmodel::getstaticmask(node, ignoreoutputvariables);
					m_staticMaskMemorize[node.source] = staticMask;
					}
			}catch (...)
			{
				_exhandler->printexceptions("", "FMTsemodel::getstaticmask", __LINE__, __FILE__);
			}
			return staticMask;
		}

    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsemodel)
