/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTConstraint.h"
#include "FMTTransition.h"
#include "FMTAction.h"
#include "FMTTheme.h"
#include "FMTYields.h"
#include "FMTTimeYieldHandler.h"
#include <memory>
#include "FMTExceptionHandler.h"
#include <math.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/container_hash/hash.hpp>

namespace Core

{
	FMTConstraint::FMTConstraint() :
		FMTOutput(),
		FMTSpec(),
		m_type()
		{

		}


	FMTConstraint::FMTConstraint(FMTconstrainttype ltype, const FMTOutput& loutput):
		FMTOutput(loutput),
		FMTSpec(),
		m_type(ltype)
		{

		}
	FMTConstraint::FMTConstraint(const FMTConstraint& rhs) :
		FMTOutput(rhs),
		FMTSpec(rhs),
		m_type(rhs.m_type)
		{

		}
	FMTConstraint& FMTConstraint::operator = (const FMTConstraint& rhs)
		{
		if (this!=&rhs)
			{
			FMTOutput::operator =(rhs);
			FMTSpec::operator =(rhs);
			m_type = rhs.m_type;
			}
		return *this;
		}
	bool FMTConstraint::operator == (const FMTConstraint& rhs) const
		{
		return(m_type == rhs.m_type &&
			FMTOutput::operator == (rhs) &&
			FMTSpec::operator == (rhs));
		}

    bool FMTConstraint::operator != (const FMTConstraint& rhs) const
        {
        return (!(*this==rhs));
        }

	bool FMTConstraint::isObjective() const
		{
		return (m_type == FMTconstrainttype::FMTMAXobjective ||
			m_type == FMTconstrainttype::FMTMINobjective ||
			m_type == FMTconstrainttype::FMTMAXMINobjective ||
			m_type == FMTconstrainttype::FMTMINMAXobjective);
		}

	bool FMTConstraint::extraVariables() const
		{
		if (m_type == FMTconstrainttype::FMTMAXMINobjective ||
			m_type == FMTconstrainttype::FMTMINMAXobjective)
			{
			return true;
			}
		return false;
		}

	std::vector<std::string>FMTConstraint::getVariableLevels() const
        {
		std::vector<std::string>names;
        for (const FMTOutputSource& source : m_sources)
            {
            if (source.isVariableLevel())
                {
                names.push_back(source.getAction());
                }
            }
        return names;
        }

	std::vector<std::string>FMTConstraint::getPenalties(double& sense) const
        {
        sense = 1;
		std::vector<std::string>penalties;
        if (!this->emptyYlds())
			{
			for (size_t id = 0; id < yieldnames.size(); ++id)
				{
				if (yieldnames.at(id).find("Penalty") != std::string::npos)
					{
					std::vector<std::string>names;
					boost::split(names, yieldnames.at(id), boost::is_any_of("_"));
					char str_sense = names.at(0).back();
					if (str_sense == '-')
						{
						sense = -1;
						}
					for (size_t nameid = 0; nameid < names.size(); ++nameid)
						{
						penalties.push_back(names.at(1));
						}
					}
				}
			std::sort(penalties.begin(), penalties.end());
			penalties.erase(std::unique(penalties.begin(), penalties.end()), penalties.end());
            }
        return penalties;
        }

   void FMTConstraint::getGoal(std::string& name,double& value) const
        {
        if (!this->emptyYlds())
			{
			for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				const size_t GOAL_FINDER = yieldnames.at(id).find("GOAL_");
				if (GOAL_FINDER != std::string::npos)
				{
					//std::vector<std::string>names;
					//boost::split(names, yieldnames.at(id), boost::is_any_of("_"));
					//name = names[1];
					name = yieldnames.at(id).substr(GOAL_FINDER+5, yieldnames.at(id).size());
					value = yieldbounds.at(id).getLower();
					break;
				}
			}
			}
        }

   void FMTConstraint::setGoal(const std::string& goalname, const double& value)
   {
	   try {
		   if (!isObjective())
				{
			    const std::string yieldtarget = "GOAL_" + goalname;
			    addBounds(Core::FMTYldBounds(Core::FMTsection::Optimize, yieldtarget, value, value));
				}
	   }
	   catch (...)
	   {
		   _exhandler->printExceptions("", "FMTConstraint::setGoal", __LINE__, __FILE__, Core::FMTsection::Optimize);
	   }
   }

   void FMTConstraint::setPenalties(const std::string& penaltyoperator, const std::vector<std::string>& variables)
   {
	   try {
		   if (isObjective())
			{
			   const std::string yldbound = "Penalty";
			   if (variables.size()==1 && (*variables.begin()) == "_ALL")
			   {
				   const std::string target = yldbound + (penaltyoperator + (*variables.begin()));
				   addBounds(Core::FMTYldBounds(Core::FMTsection::Optimize, target,0.0,0.0));
			   }else {
				   for (const std::string& penalty_var : variables)
				   {
					   const std::string target = yldbound + penaltyoperator + "_" + penalty_var;
					   addBounds(Core::FMTYldBounds(Core::FMTsection::Optimize, target,0.0,0.0));
				   }
			   }
			}
	   }
	   catch (...)
	   {
		   _exhandler->printExceptions("", "FMTConstraint::setPenalties", __LINE__, __FILE__, Core::FMTsection::Optimize);
	   }
   }

	bool FMTConstraint::isGoal() const
		{
		if (!this->emptyYlds())
			{
			for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				if (yieldnames.at(id).find("GOAL_") != std::string::npos ||
					yieldnames.at(id).find("Penalty") != std::string::npos)
				{
					return true;
				}
			}
			}
		return false;
		}

	bool FMTConstraint::doSupportRandom() const
		{
		try {
			return (m_sources.size() == 2 && m_sources.at(0).isAction() && !m_sources.at(0).isInventory() && m_sources.at(0).isVariable());
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTConstraint::doSupportRandom", __LINE__, __FILE__,Core::FMTsection::Optimize);
			}
		return false;
		}

	bool FMTConstraint::_getReplicateValue(size_t p_replicate, int p_period, double& p_bound) const
	{

		if (p_period >= getPeriodLowerBound() &&
			p_period <= getPeriodUpperBound())
		{
			if (!this->emptyYlds())
			{
				int period = getPeriodLowerBound();
				const int upperperiod = getPeriodUpperBound();
				if (period != upperperiod)
				{
					period = p_period;
				}
				bool gotSomething = false;
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id).find("REPLICATE_") != std::string::npos)
					{
						gotSomething = true;
						const size_t RepId = std::stoi(yieldnames.at(id).substr(yieldnames.at(id).find_first_of("_")+1, yieldnames.at(id).find_last_of("_")-1));
						const int RepPeriod = std::stoi(yieldnames.at(id).substr(yieldnames.at(id).find_last_of("_")+1, yieldnames.at(id).size()));
						if (RepId == (p_replicate - 1) && period == RepPeriod)
						{
							p_bound = yieldbounds.at(id).getLower();
							return true;

						}
					}
				}
				if (gotSomething)
					{
					_exhandler->raise(Exception::FMTexc::FMTrangeerror,
						"Constraint " + std::string(*this) + " cannot get replicate for period " + std::to_string(period),
						"FMTConstraint::_getReplicateValue", __LINE__, __FILE__);
					}
			}
		}
		return false;
	}


	void FMTConstraint::_setIterationChange(double p_bound)
	{
		double lower = 0;
		double upper = 0;
		getBounds(lower, upper);
		if (lower != std::numeric_limits<double>::lowest())
		{
			lower = p_bound;
		}
		if (upper != std::numeric_limits<double>::infinity())
		{
			upper = p_bound;
		}
		setRhs(lower, upper);
	}

	Core::FMTConstraint FMTConstraint::getFromReplicate(const size_t& replicate, const int& period) const
	{
		try {
			double bound = 0.0;
			if (_getReplicateValue(replicate, period, bound))
				{
				return _getIterationChange(bound);
				}
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTConstraint::getFromReplicate", __LINE__, __FILE__, Core::FMTsection::Optimize);
		}
	return *this;
	}

	void FMTConstraint::setFromReplicate(size_t p_replicate, int p_period)
		{
		double bound = 0.0;
		if (_getReplicateValue(p_replicate, p_period, bound))
			{
			_setIterationChange(bound);
			}
		}

	bool FMTConstraint::gotReplicate(const int& p_period) const
	{
		bool gotIt = false;
		if (p_period >= getPeriodLowerBound() &&
			p_period <= getPeriodUpperBound())
		{
			if (!this->emptyYlds())
			{
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id).find("REPLICATE_") != std::string::npos)
					{
						gotIt = true;
						break;
					}
				}
			}
		}
		return gotIt;
	}


	Core::FMTConstraint FMTConstraint::_getIterationChange(double p_PeriodChanges) const
	{
		Core::FMTConstraint newconstraint(*this);
		try {
				newconstraint._setIterationChange(p_PeriodChanges);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTConstraint::_getIterationChange", __LINE__, __FILE__, Core::FMTsection::Optimize);
		}
		return newconstraint;
	}

	double FMTConstraint::getScheduleWeight() const
	{
		try {
			if (!this->emptyYlds()&&isObjective())
			{
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id).find("_SETGLOBALSCHEDULE") != std::string::npos)
					{
						return getYieldBound("_SETGLOBALSCHEDULE").getLower();
					}
				}
			}
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTConstraint::getScheduleWeight", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
		return 0;
	}


	Core::FMTConstraint FMTConstraint::setFrom(const std::string& modeltype, const double& value) const
	{
		Core::FMTConstraint newconstraint(*this);
		try {
			double lower = 0;
			double upper = 0;
			double factor = 1.0;
			if (!this->emptyYlds())
			{
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id).find("_SETFROM"+modeltype) != std::string::npos)
					{
						factor = yieldbounds.at(id).getLower();
						break;
					}
				}
			}
			getBounds(lower, upper);
			if (lower != std::numeric_limits<double>::lowest())
			{
				lower = value*factor;
			}
			if (upper != std::numeric_limits<double>::infinity())
			{
				upper = std::max(value * factor,0.0);
			}
			newconstraint.setRhs(lower, upper);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTConstraint::setFrom", __LINE__, __FILE__, Core::FMTsection::Optimize);
		}
		return newconstraint;
	}

	bool FMTConstraint::isReIgnore(const int& replanningperiod) const
	{
		try {
			if (!this->emptyYlds())
			{
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id).find("_REIGNORE") != std::string::npos&&
						getYieldBound("_REIGNORE").getLower()<= replanningperiod)
					{
						if (m_type != FMTconstrainttype::FMTstandard)
						{
							_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
								"Constraint " + std::string(*this) + " cannot be set",
								"FMTConstraint::isSetFrom", __LINE__, __FILE__);
						}
						return true;
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTConstraint::isReIgnore", __LINE__, __FILE__);
		}
		return false;
	}

	bool FMTConstraint::isSetFrom(const std::string& modeltype) const
	{
		try {
			if (!this->emptyYlds())
			{
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id).find("_SETFROM"+ modeltype) != std::string::npos)
					{
						if (m_type != FMTconstrainttype::FMTstandard)
							{
							_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
								"Constraint "+std::string(*this)+" cannot be set",
								"FMTConstraint::isSetFrom", __LINE__, __FILE__);
							}
						return true;
					}
				}
			}
		}
		catch (...)
			{
			_exhandler->printExceptions("", "FMTConstraint::isSetFrom", __LINE__, __FILE__);
			}
		return false;
	}

	double FMTConstraint::sense() const
		{
		double sense = 1;
		if (m_type == FMTconstrainttype::FMTMAXobjective ||
			m_type == FMTconstrainttype::FMTMAXMINobjective)
			{
			sense = -1;
			}
		return sense;
		}

	void FMTConstraint::setRhs(double lower,double upper)
		{
		
		if (_isDivision())
			{
			double multilywith = 0;
			if (lower != std::numeric_limits<double>::lowest())
			{
				multilywith = lower;
				lower = 0.0;
			}
			if (upper != std::numeric_limits<double>::infinity())
			{
				multilywith = upper;
				upper = 0.0;
			}
			_replaceDivision(multilywith);
			}
			this->addBounds(FMTYldBounds(FMTsection::Optimize, "RHS", upper, lower));

		}

	void FMTConstraint::setLength(int firstPeriod, int lastPeriod)
		{
		this->setBounds(FMTPerBounds(FMTsection::Optimize, lastPeriod, firstPeriod));
		}

	void FMTConstraint::getBounds(double& lower, double& upper, int period) const
		{
        lower = 0;
        upper = 0;
		try{
		if (m_type == FMTconstrainttype::FMTstandard || isSpatial() || m_type == FMTconstrainttype::FMTrandomaction)
		{
			for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				if (yieldnames.at(id).find("RHS") != std::string::npos)
				{
					lower = yieldbounds.at(id).getLower();
					upper = yieldbounds.at(id).getUpper();
					break;
				}
			}
		}
		/*if (!canBeNodesOnly())
			{
			this->getRHSvalue(period,lower,upper);
			}*/
        /*if (isLevel())
            {
            size_t location = 0;
            size_t opm_location = 0;
            double factor = 1;
            for (const FMTOutputSource& source : m_sources)
                {
                if (opm_location<m_operators.size())
                    {
                    factor = m_operators.at(opm_location).call(0,factor);
                    }
                if (source.isLevel() && !source.isVariable())//constant level!
                    {
                    lower += source.getValue(period) * factor;
                    upper += source.getValue(period) * factor;
                    }
                if (location > 0)
                    {
                    ++opm_location;
                    }
                factor = 1;
                ++location;
                }

            }*/
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTConstraint::getBounds", __LINE__, __FILE__, Core::FMTsection::Optimize);
		}
		}

    bool FMTConstraint::acrossPeriod() const
        {
        if (m_type == FMTconstrainttype::FMTevenflow ||
			m_type == FMTconstrainttype::FMTnondeclining ||
			m_type == FMTconstrainttype::FMTsequence)
			{
			return true;
			}
        return false;
        }


	void FMTConstraint::getVariations(double& lower, double& upper) const
		{
		lower = 0;
		upper = 0;
		for (size_t id = 0; id < yieldnames.size(); ++id)
		{
			if (yieldnames.at(id).find("Variation") != std::string::npos)
			{
				lower = (yieldbounds.at(id).getLower() / 100);
				upper = (yieldbounds.at(id).getUpper() / 100);
				break;
			}
		}
		}

	bool FMTConstraint::isMultiple() const
		{
		//std::map<std::string, FMTYldBounds> thebounds = this->getYldsBounds();
		//return (thebounds.find("Variation") != thebounds.end());
		for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				if (yieldnames.at(id).find("Variation") != std::string::npos)
					{
					return true;
					}
			}
		return false;
		}


	size_t FMTConstraint::hash(bool hashrhs,bool hashoutputonly) const
		{
		size_t seed = 0;
		boost::hash_combine(seed, FMTOutput::hash());
		if (!hashoutputonly)
		{
			if (hashrhs)
			{
				boost::hash_combine(seed, FMTSpec::hash());
			}
			else {//only hash periods from the constraint
				boost::hash_combine(seed, FMTSpec::getPeriodLowerBound());
				boost::hash_combine(seed, FMTSpec::getPeriodLowerBound());
			}
		}
		
		return seed;
		}

	size_t FMTConstraint::hasHasOutput() const
		{
		return FMTOutput::hash();
		}

	void FMTConstraint::setOutput(const FMTOutput& out)
		{
		FMTOutput::operator =(out);
		}

	void FMTConstraint::setConstraintType(FMTconstrainttype ltype)
		{
		this->m_type = ltype;
		}

	FMTconstrainttype  FMTConstraint::getConstraintType() const
		{
		return m_type;
		}


	FMTConstraint::operator std::string() const
		{
		std::string line = "";
		std::string period_bounds = "";
		try {
			period_bounds += std::to_string(this->getPeriodLowerBound());
			period_bounds += "..";
			int maxperiod = this->getPeriodUpperBound();
			if (maxperiod == std::numeric_limits<int>::max())
			{
				period_bounds += "_LENGTH";
			}
			else {
				period_bounds += std::to_string(maxperiod);
			}
			if (this->getPeriodUpperBound() == this->getPeriodLowerBound())
			{
				period_bounds = std::to_string(maxperiod);
			}
			std::string variation = "";
			if (!this->emptyYlds())
			{
				size_t location = 0;
				bool gotvariation = false;
				for (const std::string& yldname : yieldnames)
				{
					if (yldname.find("Variation") != std::string::npos)
					{
						gotvariation = true;
						break;
					}
					++location;
				}
				if (gotvariation)
				{
					if (/*yieldbounds.at(location).getLower() != yieldbounds.at(location).getUpper() &&*/
						yieldbounds.at(location).getUpper() > 0)
					{
						variation += "," + std::to_string(static_cast<int>(yieldbounds.at(location).getLower())) + "%,";
						variation += std::to_string(static_cast<int>(yieldbounds.at(location).getUpper())) + "%";
					}
					else if (yieldbounds.at(location).getLower() != 0)
					{
						variation += "," + std::to_string(static_cast<int>(yieldbounds.at(location).getLower())) + "%";
					}


				}

			}

			std::string goal = "";
			std::string penalty = "";
			std::string global = "";
			std::string ScheduleWeight = "";
			if (!this->emptyYlds())
			{
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					const size_t GOAL_FINDER = yieldnames.at(id).find("GOAL_");
					if (GOAL_FINDER != std::string::npos)
					{
						goal += yieldnames.at(id).substr(GOAL_FINDER + 5, yieldnames.at(id).size()) + ",";
						goal += std::to_string(yieldbounds.at(id).getLower());
					}
					if (yieldnames.at(id).find("_SETFROMGLOBAL") != std::string::npos)
					{
						global += std::to_string(yieldbounds.at(id).getLower());
					}
					if (yieldnames.at(id).find("Penalty") != std::string::npos)
					{
						std::vector<std::string>names;
						boost::split(names, yieldnames.at(id), boost::is_any_of("_"));

						if (names.at(1) == "_ALL")
						{
							penalty += (std::string(1, names.at(0).back()) + "_PENALTY(_ALL");
						}
						else {
							if (!penalty.empty())
							{
								penalty += "," + names.at(1);
							}
							else {
								penalty += (std::string(1, names.at(0).back()) + "_PENALTY(");
								penalty += names.at(1);
							}

						}
					}
					if (yieldnames.at(id).find("_SETGLOBALSCHEDULE") != std::string::npos)
					{
						ScheduleWeight += std::to_string(yieldbounds.at(id).getLower());
					}
				}

				if (!penalty.empty())
				{
					penalty += ")";
				}
				if (!goal.empty())
				{
					goal.pop_back();
					goal = "_GOAL(" + goal + ")";
				}
				if (!global.empty())
				{
					global = "_SETFROMGLOBAL(" + global + ")";
				}
				if (!ScheduleWeight.empty())
				{
					ScheduleWeight = "_SETGLOBALSCHEDULE(" + ScheduleWeight + ")";
				}
			}
			switch (this->m_type)
			{
				case FMTconstrainttype::FMTMAXobjective:
				case FMTconstrainttype::FMTMINobjective:
				case FMTconstrainttype::FMTMAXMINobjective:
				case FMTconstrainttype::FMTMINMAXobjective:
				{
					static const std::unordered_map<FMTconstrainttype, std::string> prefixes = {
						{FMTconstrainttype::FMTMAXobjective,    "_MAX "},
						{FMTconstrainttype::FMTMINobjective,    "_MIN "},
						{FMTconstrainttype::FMTMAXMINobjective, "_MAXMIN "},
						{FMTconstrainttype::FMTMINMAXobjective, "_MINMAX "},
					};
					line += prefixes.at(this->m_type);
					line += this->m_name;
					if (!penalty.empty()) line += penalty;
					line += " ";
					line += period_bounds + " " + ScheduleWeight;
					break;
				}

				case FMTconstrainttype::FMTevenflow:
				case FMTconstrainttype::FMTnondeclining:
				case FMTconstrainttype::FMTsequence:
				{
					static const std::unordered_map<FMTconstrainttype, std::string> keywords = {
						{FMTconstrainttype::FMTevenflow,    "_EVEN("},
						{FMTconstrainttype::FMTnondeclining, "_NDY("},
						{FMTconstrainttype::FMTsequence,    "_SEQ("},
					};
					line += keywords.at(this->m_type);
					line += this->m_name;
					if (!variation.empty()) line += variation;
					line += ") ";
					line += period_bounds + " " + goal;
					break;
				}
				case FMTconstrainttype::FMTspatialadjacency:
				case FMTconstrainttype::FMTSpatialGroup:
				case FMTconstrainttype::FMTspatialsize:
					_standardString(line, period_bounds, goal, global, true);
					break;

				case FMTconstrainttype::FMTstandard:
				case FMTconstrainttype::FMTrandomaction:
					_standardString(line, period_bounds, goal, global);
					break;

				default:
					break;
			}
		}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTConstraint::operator std::string()", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
		boost::algorithm::trim_right(line);
		return line;
		}

		void FMTConstraint::_standardString(std::string& line, std::string& period_bounds,
			std::string& goal, std::string& global,bool asInt) const
		{
			try {
				double lower_b = 0;
				double upper_b = 0;
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id) == "RHS")
					{
						lower_b = yieldbounds.at(id).getLower();
						upper_b = yieldbounds.at(id).getUpper();
						break;
					}
				}
				std::string lowerStr,upperStr;
				if (asInt)
					{
					lowerStr = std::to_string(static_cast<int>(lower_b));
					upperStr = std::to_string(static_cast<int>(upper_b));
				}else {
					lowerStr = std::to_string(lower_b);
					upperStr = std::to_string(upper_b);
				}
				std::string opt_str = "";
				if (lower_b == upper_b)
				{
					opt_str = "= ";
					opt_str += lowerStr;
				}
				else if (upper_b == std::numeric_limits<double>::infinity())
				{
					opt_str = ">= ";
					opt_str += lowerStr;
				}
				else {
					opt_str = "<= ";
					opt_str += upperStr;
				}
				line += (this->m_name + " " + opt_str + " "+ period_bounds+" " + goal + " " + global+"\n");
				//line += " " + period_bounds + "\n";
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTConstraint::_standardString", __LINE__, __FILE__, Core::FMTsection::Optimize);
		}
		}

		bool FMTConstraint::isSpatial() const
			{
			return (m_type==Core::FMTconstrainttype::FMTspatialadjacency||
				m_type== Core::FMTconstrainttype::FMTspatialsize ||
				m_type == Core::FMTconstrainttype::FMTSpatialGroup);
			}

		bool FMTConstraint::isRandomAction() const
			{
			return (m_type == Core::FMTconstrainttype::FMTrandomaction);
			}

		size_t FMTConstraint::getGroup() const
			{
			size_t groupofconstraint = 2;//get it with member data for user
			if (isObjective())
				{
				groupofconstraint = 0;
			}else if (isSpatial())
				{
				groupofconstraint = 1;
				}
			return groupofconstraint;
			}

		size_t FMTConstraint::outputEmpty() const
			{
			return FMTOutput::empty();
			}

		FMTConstraint FMTConstraint::preSolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			const std::vector<const FMTTheme*>& selectedthemes,
			const std::vector<FMTTheme>& newthemes,
			const std::vector<FMTAction>& actions,
			const std::vector<bool>& p_valideActions, const FMTYields& yields) const
			{
			FMTConstraint newconstraint(*this);
			try {
				newconstraint.preSolveRef(filter, originalthemes, selectedthemes, newthemes, actions, p_valideActions, yields);
			}catch (...)
				{
				_exhandler->raiseFromCatch("for " + std::string(*this),"FMTConstraint::preSolve", __LINE__, __FILE__, Core::FMTsection::Optimize);
				}
			return newconstraint;
			}

		void FMTConstraint::preSolveRef(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_originalThemes,
			const std::vector<const FMTTheme*>& p_selectedThemes,
			const std::vector<FMTTheme>& p_newThemes,
			const std::vector<FMTAction>& p_actions,
			const std::vector<bool>& p_valideActions,
			const FMTYields& p_yields)
		{
			try {
				setOutput(FMTOutput::preSolve(p_filter, p_originalThemes, p_selectedThemes, p_newThemes, p_actions, p_valideActions, p_yields));
				if (m_type == Core::FMTconstrainttype::FMTSpatialGroup)
					{
					const std::string THEME_TARGET("THEME");
					double ThemeId = getYieldBound(THEME_TARGET).getLower();
					if (ThemeId >= 0)
						{
						const size_t BASE_ID = static_cast<size_t>(ThemeId) - 1;
						std::vector<FMTTheme>::const_iterator SelectedIt =
							std::find_if(p_newThemes.begin(), p_newThemes.end(), 
								Core::FMTThemeComparator(p_originalThemes.at(BASE_ID),true));
						if (SelectedIt!= p_newThemes.end())
							{
							ThemeId = static_cast<double>(std::distance(p_newThemes.begin(), SelectedIt)) + 1.0;
							addBounds(Core::FMTYldBounds(Core::FMTsection::Optimize, THEME_TARGET,
								ThemeId, ThemeId));
						}else {
							_exhandler->raise(Exception::FMTexc::FMTrangeerror,
								"Presolved removed theme used for grouping " + std::string(*this) ,
								"FMTConstraint::preSolveRef", __LINE__, __FILE__);
							}
						}
					}

			}catch (...)
				{
				_exhandler->raiseFromCatch("for " + std::string(*this), "FMTConstraint::preSolveRef", __LINE__, __FILE__, Core::FMTsection::Optimize);
				}
		}

		void FMTConstraint::_getMaxAndMin(const std::vector<double>& values, double& min, double& max) const
			{
			min = std::numeric_limits<double>::max();
			max = std::numeric_limits<double>::lowest();
			for (const double& value : values)
			{
				if (value > max)
				{
					max = value;
				}
				if (value < min)
				{
					min = value;
				}
			}
			}

		double FMTConstraint::_getVariability(const std::vector<double>& values, const double& var, const double& lowarvar) const
		{
			double total = 0;
			for (const double& value : values)
			{
				if (lowarvar != 0)
				{
					total += value > (var * 1.0 + lowarvar) ? std::abs(value - var) : 0;
				}else {
					total += std::abs(value - var);
				}
				
			}
			return total;
		}

		double FMTConstraint::_getSum(const std::vector<double>& values) const
			{
			double totalvalue = 0;
			for (const double& value : values)
				{
				totalvalue += value;
				}
			return totalvalue;
			}

		double FMTConstraint::_getPeriodicVariationCost(const std::vector<double>& values,bool evaluateupper) const
			{
			double lowervariation = 0;
			double uppervariation = 0;
			double lastvalue = 0;
			double costsum = 0;
			try{
			getVariations(lowervariation, uppervariation);
			

			for (size_t periodid = 0; periodid < values.size(); ++periodid)
				{
				const double& value = values.at(periodid);
				if (periodid !=0)
					{
					double variation = value - lastvalue;
					if (variation<0)
						{
						if (lowervariation!=0)
							{
							variation = variation <= (lowervariation*lastvalue) ? 0 : variation;
							}
						costsum += (-1 * variation);
					}else if (evaluateupper && variation>0)
						{
						if (uppervariation != 0)
							{
							variation = variation <= (uppervariation*lastvalue) ? 0 : variation;
							}
						costsum += (variation);
						}
					}
				lastvalue = value;
				}
			}
			catch (...)
			{
				_exhandler->printExceptions("", "FMTConstraint::_getPeriodicVariationCost", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
			return costsum;
			}

		double FMTConstraint::evaluate(const std::vector<double>& temporalvalues) const
			{
			double returnedvalue = 0;
			try {
				switch (this->m_type)
				{
				case FMTconstrainttype::FMTMAXobjective:
				{
					returnedvalue = -1.0 * _getSum(temporalvalues);
					break;
				}
				case FMTconstrainttype::FMTMINobjective:
				{
					returnedvalue = _getSum(temporalvalues);
					break;
				}
				case FMTconstrainttype::FMTMAXMINobjective:
				{
					double maximal = 0;
					double minimal = 0;
					_getMaxAndMin(temporalvalues, minimal, maximal);
					returnedvalue = -1.0 * minimal;
					break;
				}
				case FMTconstrainttype::FMTMINMAXobjective:
				{
					double maximal = 0;
					double minimal = 0;
					_getMaxAndMin(temporalvalues, minimal, maximal);
					returnedvalue = maximal;
					break;
				}
				case FMTconstrainttype::FMTevenflow:
				{
					double maximal = 0;
					double minimal = 0;
					double lowervariation = 0;
					double uppervariation = 0;
					getVariations(lowervariation, uppervariation);
					_getMaxAndMin(temporalvalues, minimal, maximal);
					returnedvalue = _getVariability(temporalvalues, maximal, lowervariation);
					break;
				}
				case FMTconstrainttype::FMTnondeclining:
				{
					returnedvalue = _getPeriodicVariationCost(temporalvalues);
					break;
				}
				case FMTconstrainttype::FMTsequence:
				{
					returnedvalue = _getPeriodicVariationCost(temporalvalues,true);
					break;
				}
				case FMTconstrainttype::FMTstandard:
				{
					double totalcost = 0;
					int period = getPeriodLowerBound();
					for (size_t periodid = 0; periodid < temporalvalues.size(); ++periodid)
						{
						const double& value = temporalvalues.at(periodid);
						double lowerbound = 0;
						double upperbound = 0;
						getBounds(lowerbound, upperbound, period);
						if (lowerbound!=std::numeric_limits<double>::lowest()&&value<lowerbound)
							{
							totalcost += lowerbound - value;
							}
						if (upperbound!= std::numeric_limits<double>::max() && value > upperbound)
							{
							totalcost += value - upperbound;
							}
						++period;
						}
					returnedvalue = totalcost;
					break;
				}
				default:
				break;
				}
			if ((!isObjective()&&(returnedvalue < 0 || std::isnan(returnedvalue) || isinf(returnedvalue)))||
				(isObjective() && isinf(returnedvalue)))
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Got a bad global constraint evaluation for "+std::string(*this)+" at "+std::to_string(returnedvalue),
					"FMTConstraint::evaluate", __LINE__, __FILE__);

				}
			}catch (...)
				{
				_exhandler->raiseFromCatch("", "FMTConstraint::evaluate", __LINE__, __FILE__);
				}
			return returnedvalue;
			}

		std::vector<bool>FMTConstraint::isActionsused(const std::vector<Core::FMTAction>& actions) const
		{
				std::vector<bool>ids(actions.size(),false);
				try {
					for (const std::string& actionname : this->getYlds())
					{
						std::vector<Core::FMTAction>::const_iterator cit = std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator(actionname));
						if (cit != actions.end())
						{
							const int distance = static_cast<int>(std::distance(actions.cbegin(), cit));
							ids[distance] = true;
						}
					}
				}
				catch (...)
				{
					_exhandler->raiseFromCatch("", "FMTConstraint::isActionsused", __LINE__, __FILE__, Core::FMTsection::Optimize);
				}
				return ids;
		}

		std::vector<int>FMTConstraint::getActionIds(const std::vector<Core::FMTAction>& actions) const
			{
			std::vector<int>ids;
			try {
				int actionid = 0;
				for (const bool& use : isActionsused(actions))
					{
					if (use)
						{
						ids.push_back(actionid);
						}
					++actionid;
					}
			}catch(...)
				{
				_exhandler->raiseFromCatch("", "FMTConstraint::getActionIds", __LINE__, __FILE__, Core::FMTsection::Optimize);
				}
			return ids;
			}

		bool FMTConstraint::canBeTurnedToYieldsBasedOnTransitions(const std::vector<Core::FMTTheme>& p_themes,
															const std::vector<Core::FMTTransition>& p_trans,
															const std::vector<Core::FMTAction>& p_actions,
															const std::vector<bool>& p_valideActions) const
				{
			    bool isValid = false;
				try {
					double lower;
					double upper;
					getBounds(lower, upper);
					if (m_type == Core::FMTconstrainttype::FMTstandard && lower<=0 && upper==0 && !isLevel() && !isObjective() && !isGoal())
						{
						isValid = true;
						for (const Core::FMTOutputSource& source: m_sources)
						{
							if ((source.isVariable()&&(!source.getYield().empty()||!source.empty()||!source.getAction().empty()||!source.emptyAge()))||
								(source.isConstant()&&source.getValue()<0))
							{
								isValid = false;
							}

						}
						
						for (const Core::FMTOperator& op : m_operators)
							{
							if (op != Core::FMTOperator("+") && 
								op != Core::FMTOperator("*"))
								{
								isValid = false;
								}
							}
						if (isValid)
						{
							size_t transitionId = 0;
							while (isValid && transitionId < p_trans.size())
							{
								if (p_valideActions[transitionId])
								{
									for (const Core::FMTOutputSource& source : m_sources)
									{
										if (source.isVariable())
										{
											for (const FMTMask& MASK : p_trans.at(transitionId).canProduce(
												source.getMask(), p_themes))
											{
												size_t actionId = 0;//Check for all actions for other periods.
												while (isValid && actionId < p_actions.size())
												{
													if (p_valideActions[actionId])
													{
														if (!p_actions.at(actionId).findSets(MASK).empty())
														{
															isValid = false;
														}
													}
													
													++actionId;
												}
											}
										}
									}
								}
								++transitionId;
							}
						}

						}
				}
				catch (...)
				{
					_exhandler->raiseFromCatch("", "FMTConstraint::canBeTurnedToYieldsBasedOnTransitions", __LINE__, __FILE__, Core::FMTsection::Optimize);
				}
				return isValid;
				}

		void FMTConstraint::turnToYieldsBasedOnTransition(	const std::vector<Core::FMTTheme>& themes,
															const std::vector<Core::FMTTransition>& trans,
															std::vector<Core::FMTAction>&actions,
															const std::vector<bool>& p_valideActions,
															Core::FMTYields& yields,
															const int& constraintid) const
		{
			try {
				std::vector<Core::FMTOutputSource> sourcestoturnintoyield;
				sourcestoturnintoyield.reserve(m_sources.size());
				std::vector<Core::FMTOperator> operatorstoturnintoyield;
				operatorstoturnintoyield.reserve(m_operators.size());
				Core::FMTotar newtarget=FMTotar::actual;
				size_t transitionid=0;
				for(const FMTTransition& transition : trans)
				{
					if (p_valideActions[transitionid])
					{
						const Core::FMTAction& trigerringaction = actions.at(transitionid);
						for (const Core::FMTOutputSource& source : m_sources)
						{
							if (source.isVariable())
							{

								const Core::FMTMask& sourcemask = source.getMask();
								for (const FMTMask mask : transition.canProduce(sourcemask,themes))
								{
									sourcestoturnintoyield.push_back(Core::FMTOutputSource(Core::FMTSpec(),mask,newtarget,"",trigerringaction.getName(),source.getOutputOrigin(),source.getThemeTarget()));
								}
							}
						}
					}
					++transitionid;
				}
				//if (!sourcestoturnintoyield.empty())
				//{
					FMTConstraint toturnintoyield(*this);
					toturnintoyield.m_sources = sourcestoturnintoyield;
					toturnintoyield.turnToYieldsAndActions(themes, actions, p_valideActions, yields, constraintid);
				//}
				
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTConstraint::turnToYieldsBasedOnTransition", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
		}
		bool FMTConstraint::canBeTurnedToYields() const
		{
			try {
				double lower;
				double upper;
				getBounds(lower, upper);
				if (m_type == Core::FMTconstrainttype::FMTstandard && lower<=0 && upper==0 && !isLevel() && !isInventory() && !isObjective() && !isGoal())
					{
					for (const Core::FMTOutputSource& source: m_sources)
					{
						if ((source.isVariable()&&(!source.getYield().empty()||!source.empty()||source.getAction().empty()||!source.emptyAge()))||
							(source.isConstant()&&source.getValue()<0))
						{
							return false;
						}

					}
					
					for (const Core::FMTOperator& op : m_operators)
						{
						if (op != Core::FMTOperator("+") && 
							op != Core::FMTOperator("*"))
							{
							return false;
							}
						}
					return true;
					}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTConstraint::canBeTurnedToYields", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
			return false;
		}

		void FMTConstraint::turnToYieldsAndActions(const std::vector<Core::FMTTheme>& themes,
			std::vector<Core::FMTAction>&actions,
			const std::vector<bool>& p_valideActions,
			Core::FMTYields& yields,
			const int& constraintid) const
		{
			
			try {
				std::vector<Core::FMTYieldHandler>handlers;
				const std::string baseyieldnames("~constraint" + std::to_string(constraintid));
				const int startingperiod = getPeriodLowerBound();
				int stopingperiod = getPeriodUpperBound();
				bool dorecloseof = true;
				if (stopingperiod == std::numeric_limits<int>::max())
					{
					stopingperiod = startingperiod;
					dorecloseof = false;
					}
				std::vector<double>patternvalues;
				patternvalues.reserve(startingperiod+ stopingperiod+1);
				//Base is 0 so the first period to iterate is 0 and at the starting period it stops.
				for (int period = 0;period< startingperiod;++period)//open
					{
					patternvalues.push_back(1.0);
					}
				for (int period = startingperiod; period <= stopingperiod; ++period)//closed
					{
					patternvalues.push_back(0);
					}
				if (dorecloseof)//open
					{
					patternvalues.push_back(1.0);
					}
				const std::vector<double>defaultvalues(patternvalues.size(),1.0);
				std::string defaultstrmask;
				defaultstrmask.reserve(themes.size() * 2);
				for (const Core::FMTTheme& theme : themes)
					{
					defaultstrmask += "? ";
					}
				defaultstrmask.pop_back();
				const Core::FMTMask defaultmask(defaultstrmask, themes);
				std::unique_ptr<Core::FMTYieldHandler> defaulthandler(new Core::FMTTimeYieldHandler(defaultmask));
				defaulthandler->pushBase(0);
				//defaulthandler.pushBase(1);
				size_t sourceid = 0;
				yields.unShrink(themes);
				for (const Core::FMTOutputSource& source : m_sources)
				{
					if (source.isVariable())
					{
					const std::string yieldname(baseyieldnames + "_" + std::to_string(sourceid));
					const bool IS_VALId_ACTION = _isValidAction(source.getAction(), actions, p_valideActions);
					if (IS_VALId_ACTION)
							{
								for (const Core::FMTAction* actionptr : Core::FMTActionComparator(source.getAction()).getAllAggregates(actions, false))
								{
									for (auto& itvalue : actions[std::distance(&*(actions.cbegin()), actionptr)])
									{
										itvalue.second.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, yieldname, 1.0, 1.0));
									}
								}
							}
							for (const double& pattern : defaultvalues)
							{
								defaulthandler->pushData(yieldname, pattern);
							}
							std::unique_ptr<Core::FMTYieldHandler> yieldhandler(new Core::FMTTimeYieldHandler(source.getMask()));
							yieldhandler->pushBase(0);
							//yieldhandler.pushBase(1);
							for (const double& pattern : patternvalues)
							{
								yieldhandler->pushData(yieldname, pattern);
							}
					yields.push_back(source.getMask(), yieldhandler);	
					}
					++sourceid;
				}
				yields.push_back(defaultmask, defaulthandler);
				yields.shrink();
				yields.update();
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTConstraint::turnToYieldsAndActions", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
		}


	int FMTConstraint::getThemeTarget() const
		{
		int target = targetThemeId();
		if (m_type == FMTconstrainttype::FMTSpatialGroup)
			{
			target = static_cast<int>(getYieldBound("THEME").getLower());
			}
		return target;
		}

	double FMTConstraint::getWeight() const
	{
		double returned = 0.0;
		if (isGoal())
			{
			std::string GoalName;
			double GoalValue = 0;
			getGoal(GoalName, GoalValue);
			if (GoalName == "_WEIGHT")
				{
				returned = GoalValue;
				}
			}
		return returned;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTConstraint)
