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
        if (type == FMTconstrainttype::FMTstandard || type == FMTconstrainttype::FMTspatialadjacency ||
            type == FMTconstrainttype::FMTspatialgreenup || type == FMTconstrainttype::FMTspatialsize )
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


	size_t FMTconstraint::hash() const
		{
		size_t seed = 0;
		boost::hash_combine(seed, FMToutput::hash());
		boost::hash_combine(seed, FMTspec::hash());
		return seed;
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
            case FMTconstrainttype::FMTspatialgreenup:
                {
                standardstring(line,period_bounds,goal);
				break;
				}
            case FMTconstrainttype::FMTspatialsize :
                {
                standardstring(line,period_bounds,goal);
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
            double lower_b = thebounds.at("RHS").getlower();
            double upper_b = thebounds.at("RHS").getupper();
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
			newconstraint.setoutput(FMToutput::presolve(basemask, originalthemes, presolvedmask, newthemes, actions, yields));
			return newconstraint;
			}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTconstraint)
