/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTconstraint.hpp"
#include "FMTtransition.hpp"
#include "FMTaction.hpp"
#include "FMTtheme.hpp"
#include "FMTyields.hpp"
#include "FMTtimeyieldhandler.hpp"
#include <memory>
#include "FMTexceptionhandler.hpp"
#include <math.h>

namespace Core

{
	FMTconstraint::FMTconstraint() :
		FMToutput(),
		FMTspec(),
		type()
		{

		}


	FMTconstraint::FMTconstraint(FMTconstrainttype ltype, const FMToutput& loutput):
		FMToutput(loutput),
		FMTspec(),
		type(ltype)
		{

		}
	FMTconstraint::FMTconstraint(const FMTconstraint& rhs) :
		FMToutput(rhs),
		FMTspec(rhs),
		type(rhs.type)
		{

		}
	FMTconstraint& FMTconstraint::operator = (const FMTconstraint& rhs)
		{
		if (this!=&rhs)
			{
			FMToutput::operator =(rhs);
			FMTspec::operator =(rhs);
			type = rhs.type;
			}
		return *this;
		}
	bool FMTconstraint::operator == (const FMTconstraint& rhs) const
		{
		return(type == rhs.type &&
			FMToutput::operator == (rhs) &&
			FMTspec::operator == (rhs));
		}

    bool FMTconstraint::operator != (const FMTconstraint& rhs) const
        {
        return (!(*this==rhs));
        }

	bool FMTconstraint::isobjective() const
		{
		return (type == FMTconstrainttype::FMTMAXobjective ||
			type == FMTconstrainttype::FMTMINobjective ||
			type == FMTconstrainttype::FMTMAXMINobjective ||
			type == FMTconstrainttype::FMTMINMAXobjective);
		}

	bool FMTconstraint::extravariables() const
		{
		if (type == FMTconstrainttype::FMTMAXMINobjective ||
			type == FMTconstrainttype::FMTMINMAXobjective)
			{
			return true;
			}
		return false;
		}

	std::vector<std::string>FMTconstraint::getvariablelevels() const
        {
		std::vector<std::string>names;
        for (const FMToutputsource& source : sources)
            {
            if (source.isvariablelevel())
                {
                names.push_back(source.getaction());
                }
            }
        return names;
        }

	std::vector<std::string>FMTconstraint::getpenalties(double& sense) const
        {
        sense = 1;
		std::vector<std::string>penalties;
        if (!this->emptyylds())
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
            }
        return penalties;
        }

   void FMTconstraint::getgoal(std::string& name,double& value) const
        {
        if (!this->emptyylds())
			{
			for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				if (yieldnames.at(id).find("GOAL_") != std::string::npos)
				{
					std::vector<std::string>names;
					boost::split(names, yieldnames.at(id), boost::is_any_of("_"));
					name = names[1];
					value = yieldbounds.at(id).getlower();
					break;
				}
			}
			}
        }

	bool FMTconstraint::isgoal() const
		{
		if (!this->emptyylds())
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

	bool FMTconstraint::dosupportrandom() const
		{
		try {
			return (sources.size() == 2 && sources.at(0).isaction() && !sources.at(0).isinventory() && sources.at(0).isvariable());
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTconstraint::dosupportrandom", __LINE__, __FILE__,Core::FMTsection::Optimize);
			}
		return false;
		}

	Core::FMTconstraint FMTconstraint::getfromreplicate(const size_t& replicate, const int& period) const
	{
		try {
			if (period >= getperiodlowerbound() &&
				period <= getperiodupperbound())
			{
				if (!this->emptyylds())
				{
					std::vector<double>values;
					for (size_t id = 0; id < yieldnames.size(); ++id)
					{
						if (yieldnames.at(id).find("REPLICATE_") != std::string::npos)
						{
							std::vector<std::string>repvalues;
							boost::split(repvalues, yieldnames.at(id), boost::is_any_of("_"));
							const int repperiod = std::stoi(repvalues.at(2));
							const size_t repid = std::stoi(repvalues.at(1));
							if (repid==( replicate-1)/*&&repperiod==period*/)
								{
								values.push_back(yieldbounds.at(id).getlower());
								}
						}
					}
					if (!values.empty())
						{
						return getiterationchange(values,period);
						}
				}
			}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTconstraint::getfromreplicate", __LINE__, __FILE__, Core::FMTsection::Optimize);
		}
	return *this;
	}


	Core::FMTconstraint FMTconstraint::getiterationchange(const std::vector<double>& periodchanges,const int& targetperiod) const
	{
		Core::FMTconstraint newconstraint(*this);
		try {
			int period = newconstraint.getperiodlowerbound();
			const int upperperiod = newconstraint.getperiodupperbound();
			if (period!= upperperiod)
				{
				period = targetperiod;
				}
			double lower = 0;
			double upper = 0;
			getbounds(lower, upper);
			if (periodchanges.size()<=(period-1))
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Constraint " + std::string(*this) + " cannot get replicate for period "+std::to_string(period),
					"FMTconstraint::getiterationchange", __LINE__, __FILE__);
				}
			if (lower!= std::numeric_limits<double>::lowest())
			{
				lower = periodchanges.at(period - 1);
			}
			if (upper!= std::numeric_limits<double>::infinity())
			{
				upper = periodchanges.at(period - 1);
			}
				newconstraint.setrhs(lower,upper);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTconstraint::getiterationchange", __LINE__, __FILE__, Core::FMTsection::Optimize);
		}
		return newconstraint;
	}

	double FMTconstraint::getscheduleweight() const
	{
		try {
			if (!this->emptyylds()&&isobjective())
			{
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id).find("_SETGLOBALSCHEDULE") != std::string::npos)
					{
						return getyieldbound("_SETGLOBALSCHEDULE").getlower();
					}
				}
			}
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTconstraint::getscheduleweight", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
		return 0;
	}


	Core::FMTconstraint FMTconstraint::setfrom(const std::string& modeltype, const double& value) const
	{
		Core::FMTconstraint newconstraint(*this);
		try {
			double lower = 0;
			double upper = 0;
			double factor = 1.0;
			if (!this->emptyylds())
			{
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id).find("_SETFROM"+modeltype) != std::string::npos)
					{
						factor = yieldbounds.at(id).getlower();
						break;
					}
				}
			}
			getbounds(lower, upper);
			if (lower != std::numeric_limits<double>::lowest())
			{
				lower = value*factor;
			}
			if (upper != std::numeric_limits<double>::infinity())
			{
				upper = value * factor;
			}
			newconstraint.setrhs(lower, upper);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTconstraint::setfrom", __LINE__, __FILE__, Core::FMTsection::Optimize);
		}
		return newconstraint;
	}

	bool FMTconstraint::isreignore(const int& replanningperiod) const
	{
		try {
			if (!this->emptyylds())
			{
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id).find("_REIGNORE") != std::string::npos&&
						getyieldbound("_REIGNORE").getlower()<= replanningperiod)
					{
						if (type != FMTconstrainttype::FMTstandard)
						{
							_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
								"Constraint " + std::string(*this) + " cannot be set",
								"FMTconstraint::issetfrom", __LINE__, __FILE__);
						}
						return true;
					}
				}
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTconstraint::isreignore", __LINE__, __FILE__);
		}
		return false;
	}

	bool FMTconstraint::issetfrom(const std::string& modeltype) const
	{
		try {
			if (!this->emptyylds())
			{
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id).find("_SETFROM"+ modeltype) != std::string::npos)
					{
						if (type != FMTconstrainttype::FMTstandard)
							{
							_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
								"Constraint "+std::string(*this)+" cannot be set",
								"FMTconstraint::issetfrom", __LINE__, __FILE__);
							}
						return true;
					}
				}
			}
		}
		catch (...)
			{
			_exhandler->printexceptions("", "FMTconstraint::issetfrom", __LINE__, __FILE__);
			}
		return false;
	}

	double FMTconstraint::sense() const
		{
		double sense = 1;
		if (type == FMTconstrainttype::FMTMAXobjective ||
			type == FMTconstrainttype::FMTMAXMINobjective)
			{
			sense = -1;
			}
		return sense;
		}

	void FMTconstraint::setrhs(double lower,double upper)
		{
		
		if (isdivision())
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
			replacedivision(multilywith);
			}
			this->addbounds(FMTyldbounds(FMTsection::Optimize, "RHS", upper, lower));

		}

	void FMTconstraint::setlength(int firstperiod, int lastperiod)
		{
		this->setbounds(FMTperbounds(FMTsection::Optimize, lastperiod, firstperiod));
		}

	void FMTconstraint::getbounds(double& lower, double& upper, int period) const
		{
        lower = 0;
        upper = 0;
		try{
		if (type == FMTconstrainttype::FMTstandard || type == FMTconstrainttype::FMTspatialadjacency || type == FMTconstrainttype::FMTspatialsize || type == FMTconstrainttype::FMTrandomaction)
		{
			for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				if (yieldnames.at(id).find("RHS") != std::string::npos)
				{
					lower = yieldbounds.at(id).getlower();
					upper = yieldbounds.at(id).getupper();
					break;
				}
			}
		}
		/*if (!canbenodesonly())
			{
			this->getRHSvalue(period,lower,upper);
			}*/
        /*if (islevel())
            {
            size_t location = 0;
            size_t op_location = 0;
            double factor = 1;
            for (const FMToutputsource& source : sources)
                {
                if (op_location<operators.size())
                    {
                    factor = operators.at(op_location).call(0,factor);
                    }
                if (source.islevel() && !source.isvariable())//constant level!
                    {
                    lower += source.getvalue(period) * factor;
                    upper += source.getvalue(period) * factor;
                    }
                if (location > 0)
                    {
                    ++op_location;
                    }
                factor = 1;
                ++location;
                }

            }*/
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTconstraint::getbounds", __LINE__, __FILE__, Core::FMTsection::Optimize);
		}
		}

    bool FMTconstraint::acrossperiod() const
        {
        if (type == FMTconstrainttype::FMTevenflow ||
			type == FMTconstrainttype::FMTnondeclining ||
			type == FMTconstrainttype::FMTsequence)
			{
			return true;
			}
        return false;
        }


	void FMTconstraint::getvariations(double& lower, double& upper) const
		{
		lower = 0;
		upper = 0;
		for (size_t id = 0; id < yieldnames.size(); ++id)
		{
			if (yieldnames.at(id).find("Variation") != std::string::npos)
			{
				lower = (yieldbounds.at(id).getlower() / 100);
				upper = (yieldbounds.at(id).getupper() / 100);
				break;
			}
		}
		}

	bool FMTconstraint::ismultiple() const
		{
		//std::map<std::string, FMTyldbounds> thebounds = this->getyldsbounds();
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


	size_t FMTconstraint::hash(bool hashrhs,bool hashoutputonly) const
		{
		size_t seed = 0;
		boost::hash_combine(seed, FMToutput::hash());
		if (!hashoutputonly)
		{
			if (hashrhs)
			{
				boost::hash_combine(seed, FMTspec::hash());
			}
			else {//only hash periods from the constraint
				boost::hash_combine(seed, FMTspec::getperiodlowerbound());
				boost::hash_combine(seed, FMTspec::getperiodlowerbound());
			}
		}
		
		return seed;
		}

	size_t FMTconstraint::hashasoutput() const
		{
		return FMToutput::hash();
		}

	void FMTconstraint::setoutput(const FMToutput& out)
		{
		FMToutput::operator =(out);
		}

	void FMTconstraint::setconstrainttype(FMTconstrainttype ltype)
		{
		this->type = ltype;
		}

	FMTconstrainttype  FMTconstraint::getconstrainttype() const
		{
		return type;
		}


	FMTconstraint::operator std::string() const
		{
		std::string line = "";
		std::string period_bounds = "";
		try{
			
		period_bounds += std::to_string(this->getperiodlowerbound());
		period_bounds += "..";
		int maxperiod = this->getperiodupperbound();
		if (maxperiod == std::numeric_limits<int>::max())
			{
			period_bounds += "_LENGTH";
		}else {
			period_bounds += std::to_string(maxperiod);
			}
		if (this->getperiodupperbound() == this->getperiodlowerbound())
			{
			period_bounds = std::to_string(maxperiod);
			}
		std::string variation = "";
		if (!this->emptyylds())
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
				if (/*yieldbounds.at(location).getlower() != yieldbounds.at(location).getupper() &&*/
					yieldbounds.at(location).getupper() > 0)
				{
					variation += "," + std::to_string(static_cast<int>(yieldbounds.at(location).getlower())) + "%,";
					variation += std::to_string(static_cast<int>(yieldbounds.at(location).getupper())) + "%";
				}
				else if (yieldbounds.at(location).getlower() != 0)
				{
					variation += "," + std::to_string(static_cast<int>(yieldbounds.at(location).getlower())) + "%";
				}


			}
			
			}


		std::string goal = "";
		std::string penalty = "";
		std::string global = "";
		if (!this->emptyylds())
			{
			for (size_t id = 0; id < yieldnames.size(); ++id)
			{
				if (yieldnames.at(id).find("GOAL_") != std::string::npos)
				{
					std::vector<std::string>names;
					boost::split(names, yieldnames.at(id), boost::is_any_of("_"));
					goal += names[1] + ",";
					goal += std::to_string(yieldbounds.at(id).getlower());
				}
				if (yieldnames.at(id).find("_SETTOGLOBAL") != std::string::npos)
				{
					global += std::to_string(yieldbounds.at(id).getlower());
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
				global = "_SETTOGLOBAL(" + global + ")";
				}
			}
		switch (this->type)
			{
			case FMTconstrainttype::FMTMAXobjective:
				{
				line += "_MAX(";
				line += this->name;
				if (!penalty.empty())
					{
					line += penalty;
					}
				line += ") ";
				line += period_bounds + "\n";
				break;
				}
			case FMTconstrainttype::FMTMINobjective:
				{
				line += "_MIN(";
				line += this->name;
				if(!penalty.empty())
					{
					line += penalty;
					}
				line += ") ";
				line += period_bounds + "\n";
				break;
				}
			case FMTconstrainttype::FMTMAXMINobjective:
				{
				line += "_MAXMIN(";
				line += this->name;
				if (!penalty.empty())
					{
					line += penalty;
					}
				line += ") ";
				line += period_bounds + "\n";
				break;
				}
			case FMTconstrainttype::FMTMINMAXobjective:
				{
				line += "_MINMAX(";
				line += this->name;
				if (!penalty.empty())
				{
					line += penalty;
				}
				line += ") ";
				line += period_bounds + "\n";
				break;
				}
			case FMTconstrainttype::FMTevenflow:
				{
				line += "_EVEN(";
				line += this->name;
				if (!variation.empty())
					{
					line += variation;
					}
				line += ") ";
				line += period_bounds + " " + goal + "\n";
				break;
				}
			case FMTconstrainttype::FMTnondeclining:
				{
				line += "_NDY(";
				line += this->name;
				if (!variation.empty())
					{
					line += variation;
					}
				line += ") ";
				line += period_bounds +" "+ goal + "\n";
				break;
				}
			case FMTconstrainttype::FMTsequence:
				{
				line += "_SEQ(";
				line += this->name;
				if (!variation.empty())
					{
					line += variation;
					}
					line += ") ";
				line += period_bounds + " "+ goal + "\n";
				break;
				}
			case FMTconstrainttype::FMTstandard:
				{
                standardstring(line,period_bounds,goal,global);
				break;
				}
            case FMTconstrainttype::FMTspatialadjacency:
                {
                standardstring(line,period_bounds,goal, global);
				break;
				}
            case FMTconstrainttype::FMTspatialsize :
                {
				standardstring(line, period_bounds, goal, global);
				break;
				}
			case FMTconstrainttype::FMTrandomaction:
			{
				standardstring(line, period_bounds, goal, global);
				break;
			}
			default:
			break;
			}
			}
			catch (...)
			{
				_exhandler->printexceptions("", "FMTconstraint::operator std::string()", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
		return line;
		}

		void FMTconstraint::standardstring(std::string& line, std::string& period_bounds, std::string& goal, std::string& global) const
		{
			try {
				double lower_b = 0;
				double upper_b = 0;
				for (size_t id = 0; id < yieldnames.size(); ++id)
				{
					if (yieldnames.at(id) == "RHS")
					{
						lower_b = yieldbounds.at(id).getlower();
						upper_b = yieldbounds.at(id).getupper();
						break;
					}
				}
				std::string opt_str = "";
				if (lower_b == upper_b)
				{
					opt_str = "= ";
					opt_str += std::to_string(lower_b);
				}
				else if (upper_b == std::numeric_limits<double>::infinity())
				{
					opt_str = ">= ";
					opt_str += std::to_string(lower_b);
				}
				else {
					opt_str = "<= ";
					opt_str += std::to_string(upper_b);
				}
				line += (this->name + " " + opt_str + " " + goal+" "+global);
				line += " " + period_bounds + "\n";
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTconstraint::standardstring", __LINE__, __FILE__, Core::FMTsection::Optimize);
		}
		}

		bool FMTconstraint::isspatial() const
			{
			return (type==Core::FMTconstrainttype::FMTspatialadjacency||type== Core::FMTconstrainttype::FMTspatialsize);
			}

		bool FMTconstraint::israndomaction() const
			{
			return (type == Core::FMTconstrainttype::FMTrandomaction);
			}

		size_t FMTconstraint::getgroup() const
			{
			size_t groupofconstraint = 2;//get it with member data for user
			if (isobjective())
				{
				groupofconstraint = 0;
			}else if (isspatial())
				{
				groupofconstraint = 1;
				}
			return groupofconstraint;
			}

		size_t FMTconstraint::outputempty() const
			{
			return FMToutput::empty();
			}

		FMTconstraint FMTconstraint::presolve(const FMTmaskfilter& filter,
			const std::vector<FMTtheme>& originalthemes,
			const std::vector<FMTtheme>& selectedthemes,
			const std::vector<FMTtheme>& newthemes,
			const std::vector<FMTaction>& actions, const FMTyields& yields) const
			{
			FMTconstraint newconstraint(*this);
			try {
				newconstraint.setoutput(FMToutput::presolve(filter, originalthemes, selectedthemes, newthemes, actions, yields));
			}catch (...)
				{
				_exhandler->raisefromcatch("for " + std::string(*this),"FMTconstraint::presolve", __LINE__, __FILE__, Core::FMTsection::Optimize);
				}
			return newconstraint;
			}

		void FMTconstraint::getmaxandmin(const std::vector<double>& values, double& min, double& max) const
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

		double FMTconstraint::getvariability(const std::vector<double>& values, const double& var, const double& lowarvar) const
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

		double FMTconstraint::getsum(const std::vector<double>& values) const
			{
			double totalvalue = 0;
			for (const double& value : values)
				{
				totalvalue += value;
				}
			return totalvalue;
			}

		double FMTconstraint::getperiodicvariationcost(const std::vector<double>& values,bool evaluateupper) const
			{
			double lowervariation = 0;
			double uppervariation = 0;
			double lastvalue = 0;
			double costsum = 0;
			try{
			getvariations(lowervariation, uppervariation);
			

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
				_exhandler->printexceptions("", "FMTconstraint::getperiodicvariationcost", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
			return costsum;
			}

		double FMTconstraint::evaluate(const std::vector<double>& temporalvalues) const
			{
			double returnedvalue = 0;
			try {
				switch (this->type)
				{
				case FMTconstrainttype::FMTMAXobjective:
				{
					returnedvalue = -1.0 * getsum(temporalvalues);
					break;
				}
				case FMTconstrainttype::FMTMINobjective:
				{
					returnedvalue = getsum(temporalvalues);
					break;
				}
				case FMTconstrainttype::FMTMAXMINobjective:
				{
					double maximal = 0;
					double minimal = 0;
					getmaxandmin(temporalvalues, minimal, maximal);
					returnedvalue = -1.0 * minimal;
					break;
				}
				case FMTconstrainttype::FMTMINMAXobjective:
				{
					double maximal = 0;
					double minimal = 0;
					getmaxandmin(temporalvalues, minimal, maximal);
					returnedvalue = maximal;
					break;
				}
				case FMTconstrainttype::FMTevenflow:
				{
					double maximal = 0;
					double minimal = 0;
					double lowervariation = 0;
					double uppervariation = 0;
					getvariations(lowervariation, uppervariation);
					getmaxandmin(temporalvalues, minimal, maximal);
					returnedvalue = getvariability(temporalvalues, maximal, lowervariation);
					break;
				}
				case FMTconstrainttype::FMTnondeclining:
				{
					returnedvalue = getperiodicvariationcost(temporalvalues);
					break;
				}
				case FMTconstrainttype::FMTsequence:
				{
					returnedvalue = getperiodicvariationcost(temporalvalues,true);
					break;
				}
				case FMTconstrainttype::FMTstandard:
				{
					double totalcost = 0;
					int period = getperiodlowerbound();
					for (size_t periodid = 0; periodid < temporalvalues.size(); ++periodid)
						{
						const double& value = temporalvalues.at(periodid);
						double lowerbound = 0;
						double upperbound = 0;
						getbounds(lowerbound, upperbound, period);
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
			if ((!isobjective()&&(returnedvalue < 0 || std::isnan(returnedvalue) || isinf(returnedvalue)))||
				(isobjective() && isinf(returnedvalue)))
				{
				_exhandler->raise(Exception::FMTexc::FMTrangeerror,
					"Got a bad global constraint evaluation for "+std::string(*this)+" at "+std::to_string(returnedvalue),
					"FMTconstraint::evaluate", __LINE__, __FILE__);

				}
			}catch (...)
				{
				_exhandler->raisefromcatch("", "FMTconstraint::evaluate", __LINE__, __FILE__);
				}
			return returnedvalue;
			}

		std::vector<bool>FMTconstraint::isactionsused(const std::vector<Core::FMTaction>& actions) const
		{
				std::vector<bool>ids(actions.size(),false);
				try {
					for (const std::string& actionname : this->getylds())
					{
						std::vector<Core::FMTaction>::const_iterator cit = std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(actionname));
						if (cit != actions.end())
						{
							const int distance = static_cast<int>(std::distance(actions.cbegin(), cit));
							ids[distance] = true;
						}
					}
				}
				catch (...)
				{
					_exhandler->raisefromcatch("", "FMTconstraint::isactionsused", __LINE__, __FILE__, Core::FMTsection::Optimize);
				}
				return ids;
		}

		std::vector<int>FMTconstraint::getactionids(const std::vector<Core::FMTaction>& actions) const
			{
			std::vector<int>ids;
			try {
				int actionid = 0;
				for (const bool& use : isactionsused(actions))
					{
					if (use)
						{
						ids.push_back(actionid);
						}
					++actionid;
					}
			}catch(...)
				{
				_exhandler->raisefromcatch("", "FMTconstraint::getactionids", __LINE__, __FILE__, Core::FMTsection::Optimize);
				}
			return ids;
			}

		bool FMTconstraint::canbeturnedtoyieldsbasedontransitions() const
				{
				try {
					double lower;
					double upper;
					getbounds(lower, upper);
					if (type == Core::FMTconstrainttype::FMTstandard && lower<=0 && upper==0 && !islevel() && !isobjective() && !isgoal())
						{
						for (const Core::FMToutputsource& source: sources)
						{
							if ((source.isvariable()&&(!source.getyield().empty()||!source.empty()||!source.getaction().empty()||!source.emptyage()))||
								(source.isconstant()&&source.getvalue()<0))
							{
								return false;
							}

						}
						
						for (const Core::FMToperator& op : operators)
							{
							if (op != Core::FMToperator("+") && 
								op != Core::FMToperator("*"))
								{
								return false;
								}
							}
						return true;
						}
				}
				catch (...)
				{
					_exhandler->raisefromcatch("", "FMTconstraint::canbeturnedtoyieldsbasedontransitions", __LINE__, __FILE__, Core::FMTsection::Optimize);
				}
				return false;
				}

		void FMTconstraint::turntoyieldsbasedontransition(	const std::vector<Core::FMTtheme>& themes,
															const std::vector<Core::FMTtransition>& trans,
															std::vector<Core::FMTaction>&actions,
															Core::FMTyields& yields,
															const int& constraintid) const
		{
			try {
				std::vector<Core::FMToutputsource> sourcestoturnintoyield;
				std::vector<Core::FMToperator> operatorstoturnintoyield;
				Core::FMTotar newtarget=FMTotar::actual;
				size_t transitionid=0;
				for(const FMTtransition& transition : trans)
				{
					const Core::FMTaction trigerringaction=actions.at(transitionid);
					for (const Core::FMToutputsource& source : sources)
					{
						if (source.isvariable())
						{

							Core::FMTmask sourcemask = source.getmask();
							for (const FMTmask mask : transition.canproduce(sourcemask,themes))
							{
								sourcestoturnintoyield.push_back(Core::FMToutputsource(Core::FMTspec(),mask,newtarget,"",trigerringaction.getname(),source.getoutputorigin(),source.getthemetarget()));
							}
						}
					}
					++transitionid;
				}
				FMTconstraint toturnintoyield(*this);
				toturnintoyield.sources = sourcestoturnintoyield;
				toturnintoyield.turntoyieldsandactions(themes, actions, yields,constraintid);
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTconstraint::turntoyieldsbasedontransition", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
		}
		bool FMTconstraint::canbeturnedtoyields() const
		{
			try {
				double lower;
				double upper;
				getbounds(lower, upper);
				if (type == Core::FMTconstrainttype::FMTstandard && lower<=0 && upper==0 && !islevel() && !isinventory() && !isobjective() && !isgoal())
					{
					for (const Core::FMToutputsource& source: sources)
					{
						if ((source.isvariable()&&(!source.getyield().empty()||!source.empty()||source.getaction().empty()||!source.emptyage()))||
							(source.isconstant()&&source.getvalue()<0))
						{
							return false;
						}

					}
					
					for (const Core::FMToperator& op : operators)
						{
						if (op != Core::FMToperator("+") && 
							op != Core::FMToperator("*"))
							{
							return false;
							}
						}
					return true;
					}
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTconstraint::canbeturnedtoyields", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
			return false;
		}

		void FMTconstraint::turntoyieldsandactions(const std::vector<Core::FMTtheme>& themes,
			std::vector<Core::FMTaction>&actions,
			Core::FMTyields& yields,
			const int& constraintid) const
		{
			
			try {
				std::vector<Core::FMTyieldhandler>handlers;
				const std::string baseyieldnames("~constraint" + std::to_string(constraintid));
				const int startingperiod = getperiodlowerbound();
				int stopingperiod = getperiodupperbound();
				bool dorecloseof = true;
				if (stopingperiod == std::numeric_limits<int>::max())
					{
					stopingperiod = startingperiod;
					dorecloseof = false;
					}
				std::vector<double>patternvalues;
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
				for (const Core::FMTtheme& theme : themes)
					{
					defaultstrmask += "? ";
					}
				defaultstrmask.pop_back();
				const Core::FMTmask defaultmask(defaultstrmask, themes);
				std::unique_ptr<Core::FMTyieldhandler> defaulthandler(new Core::FMTtimeyieldhandler(defaultmask));
				defaulthandler->push_base(0);
				//defaulthandler.push_base(1);
				size_t sourceid = 0;
				yields.unshrink(themes);
				for (const Core::FMToutputsource& source : sources)
				{
					if (source.isvariable())
					{
						const std::string yieldname(baseyieldnames + "_" + std::to_string(sourceid));
						for (const Core::FMTaction* actionptr : Core::FMTactioncomparator(source.getaction()).getallaggregates(actions, false))
						{
							for (auto& itvalue : actions[std::distance(&*(actions.cbegin()), actionptr)])
							{
								itvalue.second.addbounds(Core::FMTyldbounds(Core::FMTsection::Action, yieldname, 1.0, 1.0));
							}
						}
						for (const double& pattern : defaultvalues)
						{
							defaulthandler->push_data(yieldname, pattern);
						}
						std::unique_ptr<Core::FMTyieldhandler> yieldhandler(new Core::FMTtimeyieldhandler(source.getmask()));
						yieldhandler->push_base(0);
						//yieldhandler.push_base(1);
						for (const double& pattern : patternvalues)
						{
							yieldhandler->push_data(yieldname, pattern);
						}
						yields.push_back(source.getmask(), yieldhandler);
					}
					++sourceid;
				}
				yields.push_back(defaultmask, defaulthandler);
				yields.shrink();
			}
			catch (...)
			{
				_exhandler->raisefromcatch("", "FMTconstraint::turntoyieldsandactions", __LINE__, __FILE__, Core::FMTsection::Optimize);
			}
		}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTconstraint)
