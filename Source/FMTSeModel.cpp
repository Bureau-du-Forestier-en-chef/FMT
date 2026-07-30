/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSeModel.h"
#include "FMTForest.h"
#include "FMTExceptionHandler.h"
#include "FMTLayer.hpp"


namespace Models
    {

	const double FMTSeModel::MAX_FACTOR = 1.0;

	void FMTSeModel::_buildArea(const Spatial::FMTForest& p_Forest)
		{
		try{
			
			setArea(p_Forest.getArea());
		}catch (...)
			{
			_exhandler->printExceptions("", 
				"FMTSeModel::_buildArea", __LINE__, __FILE__);
			}
		}
	void FMTSeModel::_buildGraphs(double p_cellSize)
		{
		try {
			m_SpatialGraphs.setModel(*this);
			m_SpatialGraphs = Spatial::FMTSpatialGraphs(*this, p_cellSize);
		}catch (...)
			{
			_exhandler->printExceptions("", 
				"FMTSeModel::_buildGraphs", __LINE__, __FILE__);
			}
		}

	void FMTSeModel::_buildSolution(const Spatial::FMTForest& p_Forest)
	{
		try {
			m_BestSolution = Spatial::FMTSpatialSchedule(p_Forest,
				static_cast<size_t>(getParameter(FMTintmodelparameters::LENGTH) + 2),
				m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printExceptions("",
				"FMTSeModel::_buildSolution", __LINE__, __FILE__);
		}
	}

	void FMTSeModel::_copyGraphs(const Spatial::FMTSpatialGraphs& pToCopy)
	{
		try {
			m_SpatialGraphs = pToCopy;
			m_SpatialGraphs.setModel(*this);
			
		}
		catch (...)
		{
			_exhandler->printExceptions("",
				"FMTSeModel::_copyGraphs", __LINE__, __FILE__);
		}
	}
	void FMTSeModel::_copySolution(const Spatial::FMTSpatialSchedule& pToCopy)
	{
		try {
			m_BestSolution.setSpatialGraphs(pToCopy,m_SpatialGraphs);

		}catch (...)
		{
			_exhandler->printExceptions("",
				"FMTSeModel::_copySolution", __LINE__, __FILE__);
		}

	}

	double FMTSeModel::_getConstraintFactor(size_t p_constraint, double p_GrossValue) const
	{
		double value =  _getConstraintNumerator(p_constraint) / std::abs(p_GrossValue);
		if (value == std::numeric_limits<double>::infinity())
			{
			value = MAX_FACTOR;
			}
		return value;
	}

	bool FMTSeModel::_isValidFactor(double p_GrossValue)const
		{
		return p_GrossValue < MAX_FACTOR;
		}

	double FMTSeModel::_getConstraintNumerator(size_t p_constraint) const
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


    FMTSeModel::FMTSeModel(): FMTModel(), m_BestSolution(),
		m_SpatialGraphs()
        {

        }
    FMTSeModel::FMTSeModel(const FMTSeModel& rhs):
        FMTModel(rhs),
		m_BestSolution(),
		m_SpatialGraphs()
        {
		_copyGraphs(rhs.m_SpatialGraphs);
		_copySolution(rhs.m_BestSolution);
        }

	FMTSeModel::FMTSeModel(const FMTModel& rhs, const Spatial::FMTForest& forest) :
		FMTModel(rhs), m_BestSolution(),
		m_SpatialGraphs()
	{
		
		_buildArea(forest);
		_buildGraphs(forest.getCellSize());
		_buildSolution(forest);
		
	}

	FMTSeModel::FMTSeModel(const FMTModel& rhs) :
		FMTModel(rhs), m_BestSolution(),
		m_SpatialGraphs()
        {
		
        }
    FMTSeModel& FMTSeModel::operator = (const FMTSeModel& rhs)
        {
        if (this!=&rhs)
            {
            FMTModel::operator = (rhs);
			_copyGraphs(rhs.m_SpatialGraphs);
			_copySolution(rhs.m_BestSolution);
            }
        return *this;
        }

	std::vector<Core::FMTSchedule> FMTSeModel::getSchedule(bool withlock) const
	{
		try
		{
			return m_BestSolution.getSchedules(m_SpatialGraphs,withlock);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSeModel::getschedulesp", __LINE__, __FILE__);
		}
		return std::vector<Core::FMTSchedule>();
	}
	bool FMTSeModel::setInitialMapping(const Spatial::FMTForest& forest)
        {
		try {
			_buildArea(forest);
			_buildGraphs(forest.getCellSize());
			_buildSolution(forest);
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTSeModel::setInitialMapping", __LINE__, __FILE__);
		}
		return true;
        }

	void FMTSeModel::logConstraintsInfeasibilities() const
	{
		try {
			size_t cid = 0;
			double brokenup = 0;
			double total = 0;
			*_logger << "Constraints infeasibilities report: " << "\n";
			const std::vector<Core::FMTConstraint>constraints = getConstraints();
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
			_exhandler->printExceptions("", "FMTSeModel::logConstraintsInfeasibilities", __LINE__, __FILE__);
		}
	}

	void FMTSeModel::logConstraintsFactors() const
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
			_exhandler->printExceptions("", "FMTSeModel::logConstraintsFactors", __LINE__, __FILE__);
		}
	}


	std::map<std::string, double> FMTSeModel::getOutput(const Core::FMTOutput& output,
		int period, Core::FMToutputlevel level) const
	{
		std::map<std::string, double>values;
		try {
			values = m_BestSolution.getOutput(m_SpatialGraphs, output, period, level);
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTSeModel::getOutput", __LINE__, __FILE__);
			}
		return values;
	}


	Spatial::FMTLayer<double> FMTSeModel::getSpatialOutput(const Core::FMTOutput& output, int period) const
	{
		try {
			return m_BestSolution.getSpatialOutput(*this, output, period);
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTSeModel::getOutput", __LINE__, __FILE__);
		}
		return Spatial::FMTLayer<double>();
	}


	Core::FMTSchedule FMTSeModel::getSolution(int period, bool withlock) const
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
			_exhandler->printExceptions("", "FMTSeModel::getSolution", __LINE__, __FILE__);
			}
		return baseschedule;
	}


	std::string FMTSeModel::getDisturbanceStats() const
	{
		return m_BestSolution.getPatchStats(actions);
	}

	std::unique_ptr<FMTModel>FMTSeModel::preSolve(
		std::vector<Core::FMTActualDevelopment> optionaldevelopments ) const
		{
		try {
			if (m_BestSolution.actPeriod() == 1)//just preSolve if no solution
			{
				//const std::vector<Core::FMTActualDevelopment>areas = solution.getForestPeriod(0).getArea();
				const std::vector<Core::FMTActualDevelopment>areas = m_BestSolution.getArea();
				optionaldevelopments.insert(optionaldevelopments.end(), areas.begin(), areas.end());
				std::unique_ptr<FMTModel>presolvedmod(new FMTSeModel(*(FMTModel::preSolve(optionaldevelopments))));
				FMTSeModel*presolvedses = dynamic_cast<FMTSeModel*>(presolvedmod.get());
				Core::FMTMaskFilter presolveFilter = presolvedses->getPreSolveFilter(themes);
				const Core::FMTMask baseMask = this->getBaseMask(optionaldevelopments);
				const boost::dynamic_bitset<uint8_t>&bitsets = baseMask.getBitsetReference();
				//presolvedses->solution = Spatial::FMTSpatialSchedule(solution.getForestPeriod(0).preSolve(presolvefilter, presolvedses->themes));
				const size_t LENGTH = static_cast<size_t>(getParameter(FMTintmodelparameters::LENGTH) + 2);
				presolvedses->m_SpatialGraphs = Spatial::FMTSpatialGraphs(*presolvedses, m_BestSolution.getCellSize());
				Spatial::FMTSpatialSchedule presolvedSolution = m_BestSolution.preSolve(presolveFilter, presolvedses->m_SpatialGraphs, LENGTH);
				presolvedses->m_BestSolution.swap(presolvedSolution);
				return presolvedmod;
			}
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTSeModel::preSolve", __LINE__, __FILE__);
			}
		return std::unique_ptr<FMTModel>(nullptr);
		}

	void FMTSeModel::postSolve(const FMTModel& originalbasemodel)
		{
		try {
			if (m_BestSolution.actPeriod()>=1)//just postSolve if you have a solution
			{
				const Core::FMTMaskFilter presolvedmask = this->getPostSolveFilter(originalbasemodel.getThemes(), originalbasemodel.getArea().begin()->getMask());
				Spatial::FMTSpatialGraphs postSolvedGraphs = Spatial::FMTSpatialGraphs(originalbasemodel, m_BestSolution.getCellSize());
				m_BestSolution.postSolve(presolvedmask,this->getActions(), postSolvedGraphs);
				m_SpatialGraphs.swap(postSolvedGraphs);
				FMTModel::postSolve(originalbasemodel);
				m_SpatialGraphs.setModel(*this);
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSeModel::postSolve", __LINE__, __FILE__);
			}
		}

	Spatial::FMTForest FMTSeModel::getMapping() const
		{
		try {
			return m_BestSolution.getForestPeriod(m_BestSolution.actPeriod()-1);
		}
		catch (...)
			{
			_exhandler->printExceptions("", "FMTSeModel::getMapping", __LINE__, __FILE__);
			}
		return Spatial::FMTForest();
		}

	std::unique_ptr<FMTModel>FMTSeModel::clone() const
		{
		return std::unique_ptr<FMTModel>(new FMTSeModel(*this));
		}

	void FMTSeModel::swapPtr(std::unique_ptr<FMTModel>& rhs)
	{
		*this = std::move(*dynamic_cast<FMTSeModel*>(rhs.get()));
	}


	std::vector<Core::FMTActualDevelopment>FMTSeModel::getArea(int period, bool beforegrowanddeath) const
	{
		try {
			return m_BestSolution.getArea(period, beforegrowanddeath);
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTSeModel::getArea", __LINE__, __FILE__);
		}
	return std::vector<Core::FMTActualDevelopment>();
	}

	std::unique_ptr<FMTModel> FMTSeModel::getCopy(int period) const
	{
		try {
			return std::unique_ptr<FMTModel>(new FMTSeModel(*this, m_BestSolution.getForestPeriod(period)));
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSeModel::getCopy", __LINE__, __FILE__);
		}
		return std::unique_ptr<FMTModel>(nullptr);
	}

	double FMTSeModel::getObjectiveValue() const
	{
		double value = std::numeric_limits<double>::quiet_NaN();
		try {
			value = m_BestSolution.getGlobalObjective(m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSeModel::getObjectiveValue", __LINE__, __FILE__);
		}
		return value;
	}

	double FMTSeModel::_getGlobalObjective(const Spatial::FMTSpatialSchedule& p_Schedule) const
	{
		double value = 0.0;
		try {
			value = p_Schedule.getGlobalObjective(m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSeModel::_getGlobalObjective", __LINE__, __FILE__);
		}
		return value;
	}


	std::map<std::string, double> FMTSeModel::greedyReferenceBuild(
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
			_exhandler->printExceptions("", "FMTSeModel::greedyReferenceBuild", __LINE__, __FILE__);
		}
		return value;
	}

	std::vector<double> FMTSeModel::_getConstraintsValues(const Spatial::FMTSpatialSchedule& p_SpatialSchedule) const
		{
		std::vector<double> values;
		try {
			values = p_SpatialSchedule.getConstraintsValues(m_SpatialGraphs);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSeModel::_getConstraintsValues", __LINE__, __FILE__);
		}
		return values;
		}

	void FMTSeModel::getSolutionStatus(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,
		double& p_Objective, double& p_PrimalInFeasibility,
		bool withsense, bool withfactorization, bool withspatial) const
	{
		try {
			p_SpatialSchedule.getSolutionStatus(p_Objective, p_PrimalInFeasibility, m_SpatialGraphs, withsense, withfactorization, withspatial);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTSeModel::getSolutionStatus", __LINE__, __FILE__);
		}

	}

	double FMTSeModel::getConstraintEvaluation(size_t p_Constraint) const
	{
		double value = 0.0;
		try {
				value = m_BestSolution.getConstraintEvaluation(m_SpatialGraphs, p_Constraint);

		}catch (...)
		{
			_exhandler->printExceptions("", "FMTSeModel::getConstraintEvaluation", __LINE__, __FILE__);
		}
		return value;
	}

	void FMTSeModel::_doRefactorization(Spatial::FMTSpatialSchedule& p_SpatialSchedule) const
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
						NewFactors[cntid] = _getConstraintFactor(cntid, VALUE);
					}
					++cntid;
				}
				p_SpatialSchedule.setConstraintsFactor(*this,NewFactors);
				
				}
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTSeModel::DoReFactortorization", __LINE__, __FILE__);
			}
	}


	Spatial::FMTSpatialSchedule  FMTSeModel::_getNewSolution(const Spatial::FMTSpatialSchedule& p_FromSolution) const
		{
		return p_FromSolution.getBaseSchedule(m_SpatialGraphs);
		}


	std::vector<Core::FMTSchedule>  FMTSeModel::getSchedules(const Spatial::FMTSpatialSchedule& p_SpatialSchedule,bool withlock) const
	{
		return  p_SpatialSchedule.getSchedules(m_SpatialGraphs, withlock);
	}



    }

BOOST_CLASS_EXPORT_IMPLEMENT(Models::FMTSeModel)
