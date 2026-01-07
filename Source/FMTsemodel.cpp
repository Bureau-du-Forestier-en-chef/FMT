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



	void FMTsemodel::_BuildArea(const Spatial::FMTforest& p_Forest)
		{
		try{
			
			setarea(p_Forest.getarea());
		}catch (...)
			{
			_exhandler->printexceptions("", 
				"FMTsemodel::_BuildArea", __LINE__, __FILE__);
			}
		}
	void FMTsemodel::_BuildGraphs(double p_cellSize)
		{
		try {
			m_SpatialGraphs.SetModel(*this);
			m_SpatialGraphs = Spatial::FMTSpatialGraphs(*this, p_cellSize);
		}catch (...)
			{
			_exhandler->printexceptions("", 
				"FMTsemodel::_BuildGraphs", __LINE__, __FILE__);
			}
		}

	void FMTsemodel::_BuildSolution(const Spatial::FMTforest& p_Forest)
	{
		try {
			m_BestSolution = Spatial::FMTSpatialSchedule(p_Forest,
				static_cast<size_t>(getparameter(FMTintmodelparameters::LENGTH) + 2),
				m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printexceptions("",
				"FMTsemodel::_BuildSolution", __LINE__, __FILE__);
		}
	}

	void FMTsemodel::_CopyGraphs(const Spatial::FMTSpatialGraphs& pToCopy)
	{
		try {
			m_SpatialGraphs = pToCopy;
			m_SpatialGraphs.SetModel(*this);
			
		}
		catch (...)
		{
			_exhandler->printexceptions("",
				"FMTsemodel::_CopyGraphs", __LINE__, __FILE__);
		}
	}
	void FMTsemodel::_CopySolution(const Spatial::FMTSpatialSchedule& pToCopy)
	{
		try {
			m_BestSolution.SetSpatialGraphs(pToCopy,m_SpatialGraphs);

		}catch (...)
		{
			_exhandler->printexceptions("",
				"FMTsemodel::_CopySolution", __LINE__, __FILE__);
		}

	}


    FMTsemodel::FMTsemodel(): FMTmodel(), m_BestSolution(), m_SpatialGraphs()
        {

        }
    FMTsemodel::FMTsemodel(const FMTsemodel& rhs):
        FMTmodel(rhs),
		m_BestSolution(),
		m_SpatialGraphs()
        {
		_CopyGraphs(rhs.m_SpatialGraphs);
		_CopySolution(rhs.m_BestSolution);
        }

	FMTsemodel::FMTsemodel(const FMTmodel& rhs, const Spatial::FMTforest& forest) :
		FMTmodel(rhs), m_BestSolution(),
		m_SpatialGraphs()
	{
		
		_BuildArea(forest);
		_BuildGraphs(forest.getcellsize());
		_BuildSolution(forest);
		
	}

	FMTsemodel::FMTsemodel(const FMTmodel& rhs) :
		FMTmodel(rhs), m_BestSolution(), m_SpatialGraphs()
        {
		
        }
    FMTsemodel& FMTsemodel::operator = (const FMTsemodel& rhs)
        {
        if (this!=&rhs)
            {
            FMTmodel::operator = (rhs);
			_CopyGraphs(rhs.m_SpatialGraphs);
			_CopySolution(rhs.m_BestSolution);
            }
        return *this;
        }

	std::vector<Core::FMTschedule> FMTsemodel::getschedule(bool withlock) const
	{
		try
		{
			return m_BestSolution.getschedules(m_SpatialGraphs,withlock);
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
			_BuildArea(forest);
			_BuildGraphs(forest.getcellsize());
			_BuildSolution(forest);
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
			for (double value : m_BestSolution.getconstraintsvalues(m_SpatialGraphs))
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
			const std::vector<double>FACTORS = m_BestSolution.getConstraintsFactor();
			size_t constraintid = 0;
			for (const double& value : FACTORS)
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
			values = m_BestSolution.getoutput(m_SpatialGraphs, output, period, level);
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsemodel::getoutput", __LINE__, __FILE__);
			}
		return values;
	}


	Spatial::FMTlayer<double> FMTsemodel::getspatialoutput(const Core::FMToutput& output, int period) const
	{
		try {
			return m_BestSolution.getSpatialOutput(*this, output, period);
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
			const std::vector<Core::FMTschedule> allschedules = m_BestSolution.getschedules(m_SpatialGraphs,withlock);
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
		return m_BestSolution.getpatchstats(actions);
	}

	std::unique_ptr<FMTmodel>FMTsemodel::presolve(
		std::vector<Core::FMTactualdevelopment> optionaldevelopments ) const
		{
		try {
			if (m_BestSolution.actperiod() == 1)//just presolve if no solution
			{
				//const std::vector<Core::FMTactualdevelopment>areas = solution.getforestperiod(0).getarea();
				const std::vector<Core::FMTactualdevelopment>areas = m_BestSolution.getarea();
				optionaldevelopments.insert(optionaldevelopments.end(), areas.begin(), areas.end());
				std::unique_ptr<FMTmodel>presolvedmod(new FMTsemodel(*(FMTmodel::presolve(optionaldevelopments))));
				FMTsemodel*presolvedses = dynamic_cast<FMTsemodel*>(presolvedmod.get());
				Core::FMTmaskfilter presolveFilter = presolvedses->getpresolvefilter(themes);
				const Core::FMTmask baseMask = this->getbasemask(optionaldevelopments);
				const boost::dynamic_bitset<uint8_t>&bitsets = baseMask.getbitsetreference();
				//presolvedses->solution = Spatial::FMTspatialschedule(solution.getforestperiod(0).presolve(presolvefilter, presolvedses->themes));
				const size_t LENGTH = static_cast<size_t>(getparameter(FMTintmodelparameters::LENGTH) + 2);
				presolvedses->m_SpatialGraphs = Spatial::FMTSpatialGraphs(*presolvedses, m_BestSolution.getcellsize());
				Spatial::FMTSpatialSchedule presolvedSolution = m_BestSolution.presolve(presolveFilter, presolvedses->m_SpatialGraphs, LENGTH);
				presolvedses->m_BestSolution.swap(presolvedSolution);
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
			if (m_BestSolution.actperiod()>=1)//just postsolve if you have a solution
			{
				const Core::FMTmaskfilter presolvedmask = this->getpostsolvefilter(originalbasemodel.getthemes(), originalbasemodel.getarea().begin()->getmask());
				Spatial::FMTSpatialGraphs postSolvedGraphs = Spatial::FMTSpatialGraphs(originalbasemodel, m_BestSolution.getcellsize());
				m_BestSolution.postsolve(presolvedmask,this->getactions(), postSolvedGraphs);
				m_SpatialGraphs.swap(postSolvedGraphs);
				FMTmodel::postsolve(originalbasemodel);
				m_SpatialGraphs.SetModel(*this);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsemodel::postsolve", __LINE__, __FILE__);
			}
		}

	Spatial::FMTforest FMTsemodel::getmapping() const
		{
		try {
			return m_BestSolution.getforestperiod(m_BestSolution.actperiod()-1);
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
			return m_BestSolution.getarea(period, beforegrowanddeath);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getarea", __LINE__, __FILE__);
		}
	return std::vector<Core::FMTactualdevelopment>();
	}

	std::unique_ptr<FMTmodel> FMTsemodel::getcopy(int period) const
	{
		try {
			return std::unique_ptr<FMTmodel>(new FMTsemodel(*this, m_BestSolution.getforestperiod(period)));
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
			value = m_BestSolution.getglobalobjective(m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getobjectivevalue", __LINE__, __FILE__);
		}
		return value;
	}

	double FMTsemodel::GetGlobalObjective(const Spatial::FMTSpatialSchedule& p_Schedule) const
	{
		double value = 0.0;
		try {
			value = p_Schedule.getglobalobjective(m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::GetGlobalObjective", __LINE__, __FILE__);
		}
		return value;
	}


	std::map<std::string, double> FMTsemodel::GreedyReferenceBuild(
		Spatial::FMTSpatialSchedule& p_SpatialSchedule,
		const Core::FMTschedule& schedule,
		const size_t& randomiterations,
		unsigned int seed,
		double tolerance,
		bool log) const
	{
		std::map<std::string, double> value;
		try {
			value = p_SpatialSchedule.greedyreferencebuild(schedule, m_SpatialGraphs, randomiterations, seed, tolerance, log);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::GreedyReferenceBuild", __LINE__, __FILE__);
		}
		return value;
	}

	std::vector<double> FMTsemodel::GetConstraintsValues(const Spatial::FMTSpatialSchedule& p_SpatialSchedule) const
		{
		std::vector<double> values;
		try {
			values = p_SpatialSchedule.getconstraintsvalues(m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::GetConstraintsValues", __LINE__, __FILE__);
		}
		return values;
		}

	void FMTsemodel::GetSolutionStatus(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,
		double& p_Objective, double& p_PrimalInFeasibility,
		bool withsense, bool withfactorization, bool withspatial) const
	{
		try {
			p_SpatialSchedule.getsolutionstatus(p_Objective, p_PrimalInFeasibility, m_SpatialGraphs, withsense, withfactorization, withspatial);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::GetSolutionStatus", __LINE__, __FILE__);
		}

	}

	double FMTsemodel::GetConstraintEvaluation(size_t p_Constraint) const
	{
		double value = 0.0;
		try {
				value = m_BestSolution.getconstraintevaluation(m_SpatialGraphs, p_Constraint);

		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::GetConstraintEvaluation", __LINE__, __FILE__);
		}
		return value;
	}

	void FMTsemodel::DoReFactortorization(Spatial::FMTSpatialSchedule& p_SpatialSchedule) const
	{
		try {
			p_SpatialSchedule.dorefactortorization(m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printexceptions("", " FMTsemodel::DoReFactortorization", __LINE__, __FILE__);
		}
	}

	Spatial::FMTSpatialSchedule  FMTsemodel::GetNewSolution(const Spatial::FMTSpatialSchedule& p_FromSolution) const
		{
		return p_FromSolution.GetBaseSchedule(m_SpatialGraphs);
		}

	std::vector<Core::FMTschedule>  FMTsemodel::GetSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,bool withlock) const
	{
		return  p_SpatialSchedule.getschedules(m_SpatialGraphs, withlock);
	}



    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsemodel)
