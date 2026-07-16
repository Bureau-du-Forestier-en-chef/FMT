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

	void FMTsemodel::_buildArea(const Spatial::FMTforest& p_Forest)
		{
		try{
			
			setArea(p_Forest.getArea());
		}catch (...)
			{
			_exhandler->printExceptions("", 
				"FMTsemodel::_buildArea", __LINE__, __FILE__);
			}
		}
	void FMTsemodel::_buildGraphs(double p_cellSize)
		{
		try {
			m_SpatialGraphs.setModel(*this);
			m_SpatialGraphs = Spatial::FMTSpatialGraphs(*this, p_cellSize);
		}catch (...)
			{
			_exhandler->printExceptions("", 
				"FMTsemodel::_buildGraphs", __LINE__, __FILE__);
			}
		}

	void FMTsemodel::_buildSolution(const Spatial::FMTforest& p_Forest)
	{
		try {
			m_BestSolution = Spatial::FMTSpatialSchedule(p_Forest,
				static_cast<size_t>(getParameter(FMTintmodelparameters::LENGTH) + 2),
				m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printExceptions("",
				"FMTsemodel::_buildSolution", __LINE__, __FILE__);
		}
	}

	void FMTsemodel::_copyGraphs(const Spatial::FMTSpatialGraphs& pToCopy)
	{
		try {
			m_SpatialGraphs = pToCopy;
			m_SpatialGraphs.setModel(*this);
			
		}
		catch (...)
		{
			_exhandler->printExceptions("",
				"FMTsemodel::_copyGraphs", __LINE__, __FILE__);
		}
	}
	void FMTsemodel::_copySolution(const Spatial::FMTSpatialSchedule& pToCopy)
	{
		try {
			m_BestSolution.setSpatialGraphs(pToCopy,m_SpatialGraphs);

		}catch (...)
		{
			_exhandler->printExceptions("",
				"FMTsemodel::_copySolution", __LINE__, __FILE__);
		}

	}

	double FMTsemodel::getConstraintFactor(size_t p_constraint, double p_GrossValue) const
	{
		double value =  _getConstraintNumerator(p_constraint) / std::abs(p_GrossValue);
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

	double FMTsemodel::_getConstraintNumerator(size_t p_constraint) const
	{
		double Numerator = MAX_FACTOR;
		const int UPDATE = getParameter(Models::FMTintmodelparameters::UPDATE);
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
		_copyGraphs(rhs.m_SpatialGraphs);
		_copySolution(rhs.m_BestSolution);
        }

	FMTsemodel::FMTsemodel(const FMTmodel& rhs, const Spatial::FMTforest& forest) :
		FMTmodel(rhs), m_BestSolution(),
		m_SpatialGraphs()
	{
		
		_buildArea(forest);
		_buildGraphs(forest.getCellSize());
		_buildSolution(forest);
		
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
			_copyGraphs(rhs.m_SpatialGraphs);
			_copySolution(rhs.m_BestSolution);
            }
        return *this;
        }

	std::vector<Core::FMTSchedule> FMTsemodel::getSchedule(bool withlock) const
	{
		try
		{
			return m_BestSolution.getSchedules(m_SpatialGraphs,withlock);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTsemodel::getschedulesp", __LINE__, __FILE__);
		}
		return std::vector<Core::FMTSchedule>();
	}
	bool FMTsemodel::setInitialMapping(const Spatial::FMTforest& forest)
        {
		try {
			_buildArea(forest);
			_buildGraphs(forest.getCellSize());
			_buildSolution(forest);
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTsemodel::setInitialMapping", __LINE__, __FILE__);
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
			const std::vector<Core::FMTConstraint>constraints = getconstraints();
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
			_exhandler->printExceptions("", "FMTsemodel::logConstraintsInfeasibilities", __LINE__, __FILE__);
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
			_exhandler->printExceptions("", "FMTsemodel::logConstraintsFactors", __LINE__, __FILE__);
		}
	}


	std::map<std::string, double> FMTsemodel::getOutput(const Core::FMTOutput& output,
		int period, Core::FMToutputlevel level) const
	{
		std::map<std::string, double>values;
		try {
			values = m_BestSolution.getOutput(m_SpatialGraphs, output, period, level);
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTsemodel::getOutput", __LINE__, __FILE__);
			}
		return values;
	}


	Spatial::FMTlayer<double> FMTsemodel::getSpatialOutput(const Core::FMTOutput& output, int period) const
	{
		try {
			return m_BestSolution.getSpatialOutput(*this, output, period);
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTsemodel::getOutput", __LINE__, __FILE__);
		}
		return Spatial::FMTlayer<double>();
	}


	Core::FMTSchedule FMTsemodel::getSolution(int period, bool withlock) const
	{
		Core::FMTSchedule baseschedule;
		try {
			const std::vector<Core::FMTSchedule> allschedules = m_BestSolution.getSchedules(m_SpatialGraphs,withlock);
			size_t scheduleid = 0;
			while (scheduleid < allschedules.size())
			{
				if (allschedules.at(scheduleid).getPeriod()==period)
					{
					baseschedule = allschedules.at(scheduleid);
					break;
					}
				++scheduleid;
			}
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTsemodel::getSolution", __LINE__, __FILE__);
			}
		return baseschedule;
	}


	std::string FMTsemodel::getDisturbanceStats() const
	{
		return m_BestSolution.getPatchStats(actions);
	}

	std::unique_ptr<FMTmodel>FMTsemodel::presolve(
		std::vector<Core::FMTActualDevelopment> optionaldevelopments ) const
		{
		try {
			if (m_BestSolution.actPeriod() == 1)//just presolve if no solution
			{
				//const std::vector<Core::FMTActualDevelopment>areas = solution.getForestPeriod(0).getArea();
				const std::vector<Core::FMTActualDevelopment>areas = m_BestSolution.getArea();
				optionaldevelopments.insert(optionaldevelopments.end(), areas.begin(), areas.end());
				std::unique_ptr<FMTmodel>presolvedmod(new FMTsemodel(*(FMTmodel::presolve(optionaldevelopments))));
				FMTsemodel*presolvedses = dynamic_cast<FMTsemodel*>(presolvedmod.get());
				Core::FMTMaskFilter presolveFilter = presolvedses->getPresolveFilter(themes);
				const Core::FMTMask baseMask = this->getBaseMask(optionaldevelopments);
				const boost::dynamic_bitset<uint8_t>&bitsets = baseMask.getBitsetReference();
				//presolvedses->solution = Spatial::FMTspatialschedule(solution.getForestPeriod(0).presolve(presolvefilter, presolvedses->themes));
				const size_t LENGTH = static_cast<size_t>(getParameter(FMTintmodelparameters::LENGTH) + 2);
				presolvedses->m_SpatialGraphs = Spatial::FMTSpatialGraphs(*presolvedses, m_BestSolution.getCellSize());
				Spatial::FMTSpatialSchedule presolvedSolution = m_BestSolution.presolve(presolveFilter, presolvedses->m_SpatialGraphs, LENGTH);
				presolvedses->m_BestSolution.swap(presolvedSolution);
				return presolvedmod;
			}
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTsemodel::presolve", __LINE__, __FILE__);
			}
		return std::unique_ptr<FMTmodel>(nullptr);
		}

	void FMTsemodel::postSolve(const FMTmodel& originalbasemodel)
		{
		try {
			if (m_BestSolution.actPeriod()>=1)//just postSolve if you have a solution
			{
				const Core::FMTMaskFilter presolvedmask = this->getPostsolveFilter(originalbasemodel.getThemes(), originalbasemodel.getArea().begin()->getMask());
				Spatial::FMTSpatialGraphs postSolvedGraphs = Spatial::FMTSpatialGraphs(originalbasemodel, m_BestSolution.getCellSize());
				m_BestSolution.postSolve(presolvedmask,this->getactions(), postSolvedGraphs);
				m_SpatialGraphs.swap(postSolvedGraphs);
				FMTmodel::postSolve(originalbasemodel);
				m_SpatialGraphs.setModel(*this);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTsemodel::postSolve", __LINE__, __FILE__);
			}
		}

	Spatial::FMTforest FMTsemodel::getMapping() const
		{
		try {
			return m_BestSolution.getForestPeriod(m_BestSolution.actPeriod()-1);
		}
		catch (...)
			{
			_exhandler->printExceptions("", "FMTsemodel::getMapping", __LINE__, __FILE__);
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


	std::vector<Core::FMTActualDevelopment>FMTsemodel::getArea(int period, bool beforegrowanddeath) const
	{
		try {
			return m_BestSolution.getArea(period, beforegrowanddeath);
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTsemodel::getArea", __LINE__, __FILE__);
		}
	return std::vector<Core::FMTActualDevelopment>();
	}

	std::unique_ptr<FMTmodel> FMTsemodel::getCopy(int period) const
	{
		try {
			return std::unique_ptr<FMTmodel>(new FMTsemodel(*this, m_BestSolution.getForestPeriod(period)));
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTsemodel::getCopy", __LINE__, __FILE__);
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
			_exhandler->printExceptions("", "FMTsemodel::getObjectiveValue", __LINE__, __FILE__);
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
			_exhandler->printExceptions("", "FMTsemodel::getGlobalObjective", __LINE__, __FILE__);
		}
		return value;
	}


	std::map<std::string, double> FMTsemodel::greedyReferenceBuild(
		Spatial::FMTSpatialSchedule& p_SpatialSchedule,
		const Core::FMTSchedule& schedule,
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
			_exhandler->printExceptions("", "FMTsemodel::greedyReferenceBuild", __LINE__, __FILE__);
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
			_exhandler->printExceptions("", "FMTsemodel::getConstraintsValues", __LINE__, __FILE__);
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
			_exhandler->printExceptions("", "FMTsemodel::getSolutionStatus", __LINE__, __FILE__);
		}

	}

	double FMTsemodel::getConstraintEvaluation(size_t p_Constraint) const
	{
		double value = 0.0;
		try {
				value = m_BestSolution.getConstraintEvaluation(m_SpatialGraphs, p_Constraint);

		}catch (...)
		{
			_exhandler->printExceptions("", "FMTsemodel::getConstraintEvaluation", __LINE__, __FILE__);
		}
		return value;
	}

	void FMTsemodel::doRefactorization(Spatial::FMTSpatialSchedule& p_SpatialSchedule) const
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
					if ((VALUE_WITH_FACTOR > _getConstraintNumerator(cntid) ||
						VALUE_WITH_FACTOR < -_getConstraintNumerator(cntid)))
					{
						NewFactors[cntid] = getConstraintFactor(cntid, VALUE);
					}
					++cntid;
				}
				p_SpatialSchedule.setConstraintsFactor(*this,NewFactors);
				
				}
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTsemodel::DoReFactortorization", __LINE__, __FILE__);
			}
	}


	Spatial::FMTSpatialSchedule  FMTsemodel::getNewSolution(const Spatial::FMTSpatialSchedule& p_FromSolution) const
		{
		return p_FromSolution.getBaseSchedule(m_SpatialGraphs);
		}


	std::vector<Core::FMTSchedule>  FMTsemodel::getSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,bool withlock) const
	{
		return  p_SpatialSchedule.getSchedules(m_SpatialGraphs, withlock);
	}



    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTsemodel)
