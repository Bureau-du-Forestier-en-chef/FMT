/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
			type == FMTconstrainttype::FMTMINMAXobjective /*||
			isgoal()*/)
			{
			return true;
			}
		return false;
		}

    vector<string>FMTconstraint::getvariablelevels() const
        {
        vector<string>names;
        for (const FMToutputsource& source : sources)
            {
            if (source.isvariablelevel())
                {
                names.push_back(source.getaction());
                }
            }
        return names;
        }

    vector<string>FMTconstraint::getpenalties(double& sense) const
        {
        sense = 1;
        vector<string>penalties;
        if (!this->emptyylds())
			{
			map<string, FMTyldbounds> thebounds = this->getyldsbounds();
			for (map<string,FMTyldbounds>::const_iterator yields = thebounds.begin();
				yields != thebounds.end(); yields++)
				{
				if (yields->first.find("Penalty") != string::npos)
					{
					vector<string>names;
					boost::split(names, yields->first, boost::is_any_of("_"));
					char str_sense = names.at(0).back();
					if (str_sense=='-')
                        {
                        sense = -1;
                        }
					for (const string& name : names)
                        {
                        penalties.push_back(names.at(1));
                        }
					}
				}
            }
        return penalties;
        }

   void FMTconstraint::getgoal(string& name,double& value) const
        {
        if (!this->emptyylds())
			{
				map<string, FMTyldbounds> thebounds = this->getyldsbounds();
				for (map<string, FMTyldbounds>::const_iterator yields = thebounds.begin();
					yields != thebounds.end(); yields++)
					{
					if (yields->first.find("GOAL_") != string::npos)
						{
                        vector<string>names;
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
				map<string, FMTyldbounds> thebounds = this->getyldsbounds();
				for (map<string, FMTyldbounds>::const_iterator yields = thebounds.begin();
					yields != thebounds.end(); yields++)
					{
					if (yields->first.find("GOAL_") != string::npos ||
                        yields->first.find("Penalty") != string::npos)
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

	void FMTconstraint::getbounds(double& lower, double& upper, int period) const
		{
        lower = 0;
        upper = 0;
		map<string, FMTyldbounds> thebounds = this->getyldsbounds();
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
		map<string, FMTyldbounds> thebounds = this->getyldsbounds();
		if (thebounds.find("Variation") != thebounds.end())
			{
			lower = (thebounds.at("Variation").getlower() / 100);
			upper = (thebounds.at("Variation").getupper() / 100);
			}
		}

	bool FMTconstraint::ismultiple() const
		{
		map<string, FMTyldbounds> thebounds = this->getyldsbounds();
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

	FMTconstrainttype FMTconstraint::getconstrainttype() const
		{
		return type;
		}

	FMTconstraint::operator string() const
		{
		string line = "";
		string period_bounds = "";
		period_bounds += to_string(this->getperiodlowerbound());
		period_bounds += "..";
		int maxperiod = this->getperiodupperbound();
		if (maxperiod == numeric_limits<int>::max())
			{
			period_bounds += "_LENGTH";
		}else {
			period_bounds += to_string(maxperiod);
			}
		if (this->getperiodupperbound() == this->getperiodlowerbound())
			{
			period_bounds = to_string(maxperiod);
			}
		string variation = "";
		if (!this->emptyylds())
			{
			map<string, FMTyldbounds> thebounds = this->getyldsbounds();

			if (thebounds.find("Variation") != thebounds.end())
				{
				if (thebounds.at("Variation").getlower()!= thebounds.at("Variation").getupper()
					&& thebounds.at("Variation").getupper() > 0)
					{
					variation += ","+to_string(int(thebounds.at("Variation").getlower()))+ "%,";
					variation += to_string(int(thebounds.at("Variation").getupper())) + "%";
				}
				else if (thebounds.at("Variation").getlower() != 0 )
					{
					variation += "," + to_string(int(thebounds.at("Variation").getlower())) + "%";
					}


				}
			}


		string goal = "";
		string penalty = "";
		if (!this->emptyylds())
			{
			map<string, FMTyldbounds> thebounds = this->getyldsbounds();
			for (map<string,FMTyldbounds>::const_iterator yields = thebounds.begin();
				yields != thebounds.end(); yields++)
				{
				if (yields->first.find("GOAL_") != string::npos)
					{
					vector<string>names;
					boost::split(names, yields->first,boost::is_any_of("_"));
					goal += names[1]+",";
					goal += to_string(yields->second.getlower());
					}
				if (yields->first.find("Penalty") != string::npos)
					{
					vector<string>names;
					boost::split(names, yields->first, boost::is_any_of("_"));

					if (names.at(1)=="ALL")
						{
						penalty += (string(1,names.at(0).back()) + "_PENALTY(_ALL");
					}else {
						if (!penalty.empty())
							{
							penalty += "," + names.at(1);
						}else {
							penalty += (string(1, names.at(0).back()) + "_PENALTY(");
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
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << int(this->type) << "\n";
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

		void FMTconstraint::standardstring(string& line, string& period_bounds, string& goal) const
		{
		    map<string, FMTyldbounds> thebounds = this->getyldsbounds();
            double lower_b = thebounds.at("RHS").getlower();
            double upper_b = thebounds.at("RHS").getupper();
            string opt_str = "";
            if (lower_b == upper_b)
            {
                opt_str = "=";
                opt_str += to_string(lower_b);
            }
            else if (upper_b == numeric_limits<double>::infinity())
                {
                opt_str = ">=";
                opt_str += to_string(lower_b);
                }
            else {
                opt_str = "<=";
                opt_str += to_string(upper_b);
            }
            line += (this->name +" "+ opt_str+" " + goal);
            line += " " + period_bounds + "\n";
		}

}


