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

	const double FMTsemodel::MAX_FACTOR = 1.0;

	void FMTsemodel::_BuildArea(const Spatial::FMTforest& p_Forest)
		{
		try{
			
			setArea(p_Forest.getarea());
		}catch (...)
			{
			_exhandler->printexceptions("", 
				"FMTsemodel::_BuildArea", __LINE__, __FILE__);
			}
		}
	void FMTsemodel::_BuildGraphs(double p_cellSize)
		{
		try {
			m_SpatialGraphs.setModel(*this);
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
			m_SpatialGraphs.setModel(*this);
			
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
			m_BestSolution.setSpatialGraphs(pToCopy,m_SpatialGraphs);

		}catch (...)
		{
			_exhandler->printexceptions("",
				"FMTsemodel::_CopySolution", __LINE__, __FILE__);
		}

	}

	double FMTsemodel::getConstraintFactor(size_t p_constraint, double p_GrossValue) const
	{
		double value =  _GetConstraintNumerator(p_constraint) / std::abs(p_GrossValue);
		if (value == std::numeric_limits<double>::infinity())
			{
			value = MAX_FACTOR;
			}
		return value;
	}

	bool FMTsemodel::isValidFactor(double p_GrossValue)const
		{
		return p_GrossValue < MAX_FACTOR;
		}

	double FMTsemodel::_GetConstraintNumerator(size_t p_constraint) const
	{
		double Numerator = MAX_FACTOR;
		const int UPDATE = getparameter(Models::FMTintmodelparameters::UPDATE);
		if (constraints[p_constraint].getPeriodUpperBound() < UPDATE)
			{
			Numerator *= 10;
			}
		if (constraints[p_constraint].isSpatial())
			{
			Numerator *= 10;
			}
		return Numerator;
	}


    FMTsemodel::FMTsemodel(): FMTmodel(), m_BestSolution(),
		m_SpatialGraphs()
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
		FMTmodel(rhs), m_BestSolution(),
		m_SpatialGraphs()
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

	std::vector<Core::FMTschedule> FMTsemodel::getSchedule(bool withlock) const
	{
		try
		{
			return m_BestSolution.getSchedules(m_SpatialGraphs,withlock);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getschedulesp", __LINE__, __FILE__);
		}
		return std::vector<Core::FMTschedule>();
	}
	bool FMTsemodel::setInitialMapping(const Spatial::FMTforest& forest)
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

	void FMTsemodel::logConstraintsInfeasibilities() const
	{
		try {
			size_t cid = 0;
			double brokenup = 0;
			double total = 0;
			*_logger << "Constraints infeasibilities report: " << "\n";
			const std::vector<Core::FMTconstraint>constraints = getconstraints();
			for (double value : m_BestSolution.getConstraintsValues(m_SpatialGraphs))
			{
				if (cid > 0 /* && !constraints.at(cid).isSpatial()*/)
				{
					if (value > 0)
					{
						if (constraints.at(cid).isGoal())
						{
							double GoalValue;
							std::string GoalName;
							constraints.at(cid).getGoal(GoalName, GoalValue);
							if (GoalName=="_WEIGHT")
								{
								value /= GoalValue;
								}
						}
						std::string constraintName = std::string(constraints.at(cid));
						std::replace(constraintName.begin(), constraintName.end(), '\n', ' ');
						constraintName += ("(" + std::to_string(static_cast<int>(value))+")");
						*_logger << constraintName << "\n";
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

	void FMTsemodel::logConstraintsFactors() const
	{
		try {
			*_logger << "Constraints factor report" << "\n";
			const std::vector<double>FACTORS = m_BestSolution.getConstraintsFactor();
			size_t constraintid = 0;
			for (const double& value : FACTORS)
				{
				std::string constraintName = constraints.at(constraintid);
				std::replace(constraintName.begin(), constraintName.end(), '\n', ' ');
				constraintName+= " (" + std::to_string(value)+")";
				*_logger << constraintName << "\n";
				++constraintid;
				}
			*_logger << "Constraints factor report done" << "\n";
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::LogConstraintsFactors", __LINE__, __FILE__);
		}
	}


	std::map<std::string, double> FMTsemodel::getOutput(const Core::FMToutput& output,
		int period, Core::FMToutputlevel level) const
	{
		std::map<std::string, double>values;
		try {
			values = m_BestSolution.getOutput(m_SpatialGraphs, output, period, level);
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsemodel::getoutput", __LINE__, __FILE__);
			}
		return values;
	}


	Spatial::FMTlayer<double> FMTsemodel::getSpatialOutput(const Core::FMToutput& output, int period) const
	{
		try {
			return m_BestSolution.getSpatialOutput(*this, output, period);
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getoutput", __LINE__, __FILE__);
		}
		return Spatial::FMTlayer<double>();
	}


	Core::FMTschedule FMTsemodel::getSolution(int period, bool withlock) const
	{
		Core::FMTschedule baseschedule;
		try {
			const std::vector<Core::FMTschedule> allschedules = m_BestSolution.getSchedules(m_SpatialGraphs,withlock);
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


	std::string FMTsemodel::getDisturbanceStats() const
	{
		return m_BestSolution.getPatchStats(actions);
	}

	std::unique_ptr<FMTmodel>FMTsemodel::presolve(
		std::vector<Core::FMTactualdevelopment> optionaldevelopments ) const
		{
		try {
			if (m_BestSolution.actPeriod() == 1)//just presolve if no solution
			{
				//const std::vector<Core::FMTactualdevelopment>areas = solution.getForestPeriod(0).getarea();
				const std::vector<Core::FMTactualdevelopment>areas = m_BestSolution.getarea();
				optionaldevelopments.insert(optionaldevelopments.end(), areas.begin(), areas.end());
				std::unique_ptr<FMTmodel>presolvedmod(new FMTsemodel(*(FMTmodel::presolve(optionaldevelopments))));
				FMTsemodel*presolvedses = dynamic_cast<FMTsemodel*>(presolvedmod.get());
				Core::FMTmaskfilter presolveFilter = presolvedses->getPresolveFilter(themes);
				const Core::FMTmask baseMask = this->getBaseMask(optionaldevelopments);
				const boost::dynamic_bitset<uint8_t>&bitsets = baseMask.getBitsetReference();
				//presolvedses->solution = Spatial::FMTspatialschedule(solution.getForestPeriod(0).presolve(presolvefilter, presolvedses->themes));
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
			if (m_BestSolution.actPeriod()>=1)//just postsolve if you have a solution
			{
				const Core::FMTmaskfilter presolvedmask = this->getPostsolveFilter(originalbasemodel.getthemes(), originalbasemodel.getarea().begin()->getmask());
				Spatial::FMTSpatialGraphs postSolvedGraphs = Spatial::FMTSpatialGraphs(originalbasemodel, m_BestSolution.getcellsize());
				m_BestSolution.postsolve(presolvedmask,this->getactions(), postSolvedGraphs);
				m_SpatialGraphs.swap(postSolvedGraphs);
				FMTmodel::postsolve(originalbasemodel);
				m_SpatialGraphs.setModel(*this);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTsemodel::postsolve", __LINE__, __FILE__);
			}
		}

	Spatial::FMTforest FMTsemodel::getMapping() const
		{
		try {
			return m_BestSolution.getForestPeriod(m_BestSolution.actPeriod()-1);
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

	void FMTsemodel::swapPtr(std::unique_ptr<FMTmodel>& rhs)
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

	std::unique_ptr<FMTmodel> FMTsemodel::getCopy(int period) const
	{
		try {
			return std::unique_ptr<FMTmodel>(new FMTsemodel(*this, m_BestSolution.getForestPeriod(period)));
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getcopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTmodel>(nullptr);
	}

	double FMTsemodel::getObjectiveValue() const
	{
		double value = std::numeric_limits<double>::quiet_NaN();
		try {
			value = m_BestSolution.getGlobalObjective(m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::getobjectivevalue", __LINE__, __FILE__);
		}
		return value;
	}

	double FMTsemodel::getGlobalObjective(const Spatial::FMTSpatialSchedule& p_Schedule) const
	{
		double value = 0.0;
		try {
			value = p_Schedule.getGlobalObjective(m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::GetGlobalObjective", __LINE__, __FILE__);
		}
		return value;
	}


	std::map<std::string, double> FMTsemodel::greedyReferenceBuild(
		Spatial::FMTSpatialSchedule& p_SpatialSchedule,
		const Core::FMTschedule& schedule,
		const size_t& randomiterations,
		unsigned int seed,
		double tolerance,
		bool log) const
	{
		std::map<std::string, double> value;
		try {
			value = p_SpatialSchedule.greedyReferenceBuild(schedule, m_SpatialGraphs, randomiterations, seed, tolerance, log);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::GreedyReferenceBuild", __LINE__, __FILE__);
		}
		return value;
	}

	std::vector<double> FMTsemodel::getConstraintsValues(const Spatial::FMTSpatialSchedule& p_SpatialSchedule) const
		{
		std::vector<double> values;
		try {
			values = p_SpatialSchedule.getConstraintsValues(m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::GetConstraintsValues", __LINE__, __FILE__);
		}
		return values;
		}

	void FMTsemodel::getSolutionStatus(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,
		double& p_Objective, double& p_PrimalInFeasibility,
		bool withsense, bool withfactorization, bool withspatial) const
	{
		try {
			p_SpatialSchedule.getSolutionStatus(p_Objective, p_PrimalInFeasibility, m_SpatialGraphs, withsense, withfactorization, withspatial);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::GetSolutionStatus", __LINE__, __FILE__);
		}

	}

	double FMTsemodel::getConstraintEvaluation(size_t p_Constraint) const
	{
		double value = 0.0;
		try {
				value = m_BestSolution.getConstraintEvaluation(m_SpatialGraphs, p_Constraint);

		}catch (...)
		{
			_exhandler->printexceptions("", "FMTsemodel::GetConstraintEvaluation", __LINE__, __FILE__);
		}
		return value;
	}

	void FMTsemodel::doReFactortorization(Spatial::FMTSpatialSchedule& p_SpatialSchedule) const
	{
		try {
			const std::vector<double>& FACTORS = p_SpatialSchedule.getConstraintsFactor();
			if (!FACTORS.empty())
				{
				std::vector<double>NewFactors(FACTORS);
				size_t cntid = 0;
				for (const double& VALUE : p_SpatialSchedule.getConstraintsValues(m_SpatialGraphs))
				{
					const double VALUE_WITH_FACTOR = FACTORS.at(cntid) * VALUE;
					if ((VALUE_WITH_FACTOR > _GetConstraintNumerator(cntid) ||
						VALUE_WITH_FACTOR < -_GetConstraintNumerator(cntid)))
					{
						NewFactors[cntid] = getConstraintFactor(cntid, VALUE);
					}
					++cntid;
				}
				p_SpatialSchedule.setConstraintsFactor(*this,NewFactors);
				
				}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTsemodel::DoReFactortorization", __LINE__, __FILE__);
			}
	}


	Spatial::FMTSpatialSchedule  FMTsemodel::getNewSolution(const Spatial::FMTSpatialSchedule& p_FromSolution) const
		{
		return p_FromSolution.getBaseSchedule(m_SpatialGraphs);
		}


	std::vector<Core::FMTschedule>  FMTsemodel::getSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,bool withlock) const
	{
		return  p_SpatialSchedule.getSchedules(m_SpatialGraphs, withlock);
	}



    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsemodel)
