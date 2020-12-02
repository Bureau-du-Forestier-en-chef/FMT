/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTconstraint.h"

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
			const std::map<std::string, FMTyldbounds> thebounds = this->getyldsbounds();
			for (std::map<std::string,FMTyldbounds>::const_iterator yields = thebounds.begin();
				yields != thebounds.end(); yields++)
				{
				if (yields->first.find("Penalty") != std::string::npos)
					{
					std::vector<std::string>names;
					boost::split(names, yields->first, boost::is_any_of("_"));
					char str_sense = names.at(0).back();
					if (str_sense=='-')
                        {
                        sense = -1;
                        }
					for (size_t nameid = 0; nameid<names.size();++nameid)
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
				const std::map<std::string, FMTyldbounds> thebounds = this->getyldsbounds();
				for (std::map<std::string, FMTyldbounds>::const_iterator yields = thebounds.begin();
					yields != thebounds.end(); yields++)
					{
					if (yields->first.find("GOAL_") != std::string::npos)
						{
						std::vector<std::string>names;
                        boost::split(names, yields->first,boost::is_any_of("_"));
                        name = names[1];
                        value = yields->second.getlower();
                        break;
						}
					}
			}
        }

	bool FMTconstraint::isgoal() const
		{
		if (!this->emptyylds())
			{
				std::map<std::string, FMTyldbounds> thebounds = this->getyldsbounds();
				for (std::map<std::string, FMTyldbounds>::const_iterator yields = thebounds.begin();
					yields != thebounds.end(); yields++)
					{
					if (yields->first.find("GOAL_") != std::string::npos ||
                        yields->first.find("Penalty") != std::string::npos)
						{
						return true;
						}
					}
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

	void FMTconstraint::setrhs(const double& lower,const double& upper)
		{
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
		const std::map<std::string, FMTyldbounds> thebounds = this->getyldsbounds();
        if (type == FMTconstrainttype::FMTstandard || type == FMTconstrainttype::FMTspatialadjacency ||type == FMTconstrainttype::FMTspatialsize )
            {
            lower = thebounds.at("RHS").getlower();
            upper = thebounds.at("RHS").getupper();
            }
        if (islevel())
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
		std::map<std::string, FMTyldbounds> thebounds = this->getyldsbounds();
		if (thebounds.find("Variation") != thebounds.end())
			{
			lower = (thebounds.at("Variation").getlower() / 100);
			upper = (thebounds.at("Variation").getupper() / 100);
			}
		}

	bool FMTconstraint::ismultiple() const
		{
		std::map<std::string, FMTyldbounds> thebounds = this->getyldsbounds();
		return (thebounds.find("Variation") != thebounds.end());
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


	FMTconstraint::operator std::string() const
		{
		std::string line = "";
		std::string period_bounds = "";
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
			std::map<std::string, FMTyldbounds> thebounds = this->getyldsbounds();

			if (thebounds.find("Variation") != thebounds.end())
				{
				if (thebounds.at("Variation").getlower()!= thebounds.at("Variation").getupper()
					&& thebounds.at("Variation").getupper() > 0)
					{
					variation += ","+ std::to_string(static_cast<int>(thebounds.at("Variation").getlower()))+ "%,";
					variation += std::to_string(static_cast<int>(thebounds.at("Variation").getupper())) + "%";
				}
				else if (thebounds.at("Variation").getlower() != 0 )
					{
					variation += "," + std::to_string(static_cast<int>(thebounds.at("Variation").getlower())) + "%";
					}


				}
			}


		std::string goal = "";
		std::string penalty = "";
		if (!this->emptyylds())
			{
			const std::map<std::string, FMTyldbounds> thebounds = this->getyldsbounds();
			for (std::map<std::string,FMTyldbounds>::const_iterator yields = thebounds.begin();
				yields != thebounds.end(); yields++)
				{
				if (yields->first.find("GOAL_") != std::string::npos)
					{
					std::vector<std::string>names;
					boost::split(names, yields->first,boost::is_any_of("_"));
					goal += names[1]+",";
					goal += std::to_string(yields->second.getlower());
					}
				if (yields->first.find("Penalty") != std::string::npos)
					{
					std::vector<std::string>names;
					boost::split(names, yields->first, boost::is_any_of("_"));

					if (names.at(1)=="ALL")
						{
						penalty += (std::string(1,names.at(0).back()) + "_PENALTY(_ALL");
					}else {
						if (!penalty.empty())
							{
							penalty += "," + names.at(1);
						}else {
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
                standardstring(line,period_bounds,goal);
				break;
				}
            case FMTconstrainttype::FMTspatialadjacency:
                {
                standardstring(line,period_bounds,goal);
				break;
				}
            case FMTconstrainttype::FMTspatialsize :
                {
				standardstring(line, period_bounds, goal);
				break;
				}
			default:
			break;
			}
		return line;
		}

		void FMTconstraint::standardstring(std::string& line, std::string& period_bounds, std::string& goal) const
		{
			const std::map<std::string, FMTyldbounds> thebounds = this->getyldsbounds();
            const double lower_b = thebounds.at("RHS").getlower();
            const double upper_b = thebounds.at("RHS").getupper();
			std::string opt_str = "";
            if (lower_b == upper_b)
            {
                opt_str = "=";
                opt_str += std::to_string(lower_b);
            }
            else if (upper_b == std::numeric_limits<double>::infinity())
                {
                opt_str = ">=";
                opt_str += std::to_string(lower_b);
                }
            else {
                opt_str = "<=";
                opt_str += std::to_string(upper_b);
            }
            line += (this->name +" "+ opt_str+" " + goal);
            line += " " + period_bounds + "\n";
		}

		bool FMTconstraint::isspatial() const
			{
			return (type==Core::FMTconstrainttype::FMTspatialadjacency||type== Core::FMTconstrainttype::FMTspatialsize);
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

		FMTconstraint FMTconstraint::presolve(const FMTmask& basemask,
			const std::vector<FMTtheme>& originalthemes,
			const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& newthemes,
			const std::vector<FMTaction>& actions, const FMTyields& yields) const
			{
			FMTconstraint newconstraint(*this);
			try {
				newconstraint.setoutput(FMToutput::presolve(basemask, originalthemes, presolvedmask, newthemes, actions, yields));
			}catch (...)
				{
				_exhandler->raisefromcatch("for " + std::string(*this),"FMTconstraint::presolve", __LINE__, __FILE__, _section);
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
			getvariations(lowervariation, uppervariation);
			double lastvalue = 0;
			double costsum = 0;

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
					returnedvalue = getvariability(temporalvalues, minimal, lowervariation);
					/*returnedvalue = maximal - minimal;
					if (lowervariation != 0)
						{
						returnedvalue = returnedvalue > (minimal * 1.0 + lowervariation) ? returnedvalue : 0;
						}*/
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
					_exhandler->raisefromcatch("", "FMTconstraint::isactionsused", __LINE__, __FILE__);
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
				_exhandler->raisefromcatch("", "FMTconstraint::getactionids", __LINE__, __FILE__);
				}
			return ids;
			}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTconstraint)
