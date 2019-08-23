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

#include "FMToptimizationparser.h"

namespace WSParser
{

	FMToptimizationparser::FMToptimizationparser() : FMTparser(),
		rxsections(regex("^(\\*)([^\\s^\\t]*)", regex_constants::ECMAScript | regex_constants::icase)),
		rxobjectives(regex("^(_MAXMIN|_MINMAX|_MAX|_MIN|_GOAL)([\\s\\t]*)(.+)([\\s\\t])((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))", regex_constants::ECMAScript | regex_constants::icase)),
		rxexclude(regex("^([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))", regex_constants::ECMAScript | regex_constants::icase)),
		rxconstraints("^(_EVEN|_NDY|_SEQ)([\\s\\t]*)(\\()((([^\\)^,]*)(,)([\\d]*%|[\\d]*)(,)([\\d]*%|[\\d]*))|(([^\\)^,]*)(,)([\\d]*%|[\\d]*))|([^\\)^,]*))(\\)*)([\\s\\t]*)((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))", regex_constants::ECMAScript | regex_constants::icase),
		//rxconstraints("^(_EVEN|_NDY|_SEQ)([\\s\\t]*)(\\()((([^\\s^\\t^,]*)(,)([\\d]*%|[\\d]*)(,)([\\d]*%|[\\d]*))|(([^\\s^\\t^,]*)(,)([\\d]*%|[\\d]*))|([^\\s^\\t^,]*))(\\))([\\s\\t]*)((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))", regex_constants::ECMAScript | regex_constants::icase),
		rxequations(regex("^(((.+)((<=)|(>=))(.+))|((.+)(=)(.+)))([\\s\\t])((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))", regex_constants::ECMAScript | regex_constants::icase)),
		rxgoal(regex("^(.+)(_GOAL)(\\()([^,]*)(,)([^\\)]*)(\\))", regex_constants::ECMAScript | regex_constants::icase)),
		//rxoutput("^([^\\(\\[]*)|(\\()([^)]*)(\\))|(\\[)((#.+|[\\d]*))(\\])", regex_constants::ECMAScript | regex_constants::icase),
		rxoutput("^(.+)(\\()([^)]*)(\\))(\\[)(#.+|[-\\d]*)(\\])|(.+)(\\()([^)]*)(\\))|(.+)(\\[)(#.+|[-\\d]*)(\\])|(.+)", regex_constants::ECMAScript | regex_constants::icase),
		//rxoutput("^(\\b_SUM\\(\\b)|(.+)(\\()([^)]*)(\\))(\\[)(#.+|[-\\d]*)(\\])|(.+)(\\()([^)]*)(\\))|(.+)(\\[)(#.+|[-\\d]*)(\\])|(.+)(\\))|(.+)", regex_constants::ECMAScript | regex_constants::icase),
		rxpenalty("^(_PENALTY)(\\()([^\\)]*)(\\))", regex_constants::ECMAScript | regex_constants::icase),
		ineach()
		{

		}

	FMToptimizationparser::FMToptimizationparser(const FMToptimizationparser& rhs) :
		FMTparser(rhs),
		rxsections(rhs.rxsections),
		rxobjectives(rhs.rxobjectives),
		rxexclude(rhs.rxexclude),
		rxconstraints(rhs.rxconstraints),
		rxequations(rhs.rxequations),
		rxgoal(rhs.rxgoal),
		rxoutput(rhs.rxoutput),
		rxpenalty(rhs.rxpenalty),
		ineach(rhs.ineach)
		{

		}
	FMToptimizationparser& FMToptimizationparser::operator = (const FMToptimizationparser& rhs)
		{
		if (this!=&rhs)
			{
			FMTparser::operator=(rhs);
			rxsections = rhs.rxsections;
			rxobjectives = rhs.rxobjectives;
			rxexclude = rhs.rxexclude;
			rxconstraints = rhs.rxconstraints;
			rxequations = rhs.rxequations;
			rxgoal = rhs.rxgoal;
			rxoutput = rhs.rxoutput;
			rxpenalty = rhs.rxpenalty;
			ineach = rhs.ineach;
			}
		return *this;
		}
	bool FMToptimizationparser::setgoal(FMTconstraint& constraint, const string& line, const FMTconstants& constants)
		{
		smatch kmatch;
		if (regex_search(line, kmatch, rxgoal))
			{
			string target = "GOAL_"+string(kmatch[4]);
			string value = kmatch[6];
			double goal_var = getnum<double>(value, constants);
			constraint.addbounds(FMTyldbounds(FMTwssect::Optimize, target, goal_var, goal_var));
			return true;
			}
		return false;
		}

	void FMToptimizationparser::setperiods(FMTconstraint& constraint, const string& lower, const string& upper, const FMTconstants& constants)
		{
		int startperiod = getnum<int>(lower, constants);
		int stopperiod = startperiod;
		if (!upper.empty() && upper != "_LENGTH")
		{
			stopperiod = getnum<int>(upper, constants);
		}else if (upper == "_LENGTH")
			{
			stopperiod = numeric_limits<int>::max();
			}
		constraint.setbounds(FMTperbounds(FMTwssect::Optimize, stopperiod, startperiod));
		}

    map<string,double>FMToptimizationparser::getequation(const string& line, const FMTconstants& constants,
                                                         const vector<FMToutput>& outputs,size_t lhssize)
        {
        vector<string>simplificaiton;
        size_t eq_location = 0;
        string simple_value;
        vector<FMToutput>::const_iterator constant_output;
        //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "getting equation " << line << "\n";
        for(char strvalue : line)
            {
            if ((strvalue == '+' || strvalue == '-' || strvalue == '/' || strvalue == '*') && (simple_value.empty() || (!simple_value.empty() && simple_value.back() != '[')))
                {
                if (eq_location >= lhssize)
                    {
                    if (strvalue == '+')
                        {
                        strvalue = '-';
                        }else if (strvalue == '-')
                            {
                            strvalue = '+';
                            }
                    }
                if (!simple_value.empty())
                        {
                        boost::trim(simple_value);
                        if (isnum(simple_value) || constants.isconstant(simple_value))
                            {
                            simple_value = to_string(getnum<double>(simple_value, constants));
                            }/*else {
                            //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "getting " << simple_value << "\n";
                            constant_output = find_if(outputs.begin(), outputs.end(), FMToutputcomparator(simple_value));
                            if (constant_output != outputs.end() && constant_output->isconstant())
                                {
                                //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "is constant " << "\n";
                                simple_value = to_string(constant_output->getconstantvalue());
                                }
                            }*/
						if (!simple_value.empty())
							{
							simplificaiton.push_back(simple_value);
							}
						 simple_value.clear();
                        }else if(!simplificaiton.empty())
                            {
                            simplificaiton.pop_back();
                            }
						string pushedval = string(1, strvalue);
						boost::trim(pushedval);
						if (!pushedval.empty())
							{
							simplificaiton.push_back(pushedval);
							}
                    
                    }else{
                        simple_value += strvalue;
                        }
                    ++eq_location;
                }
            boost::trim(simple_value);
            if (isnum(simple_value) || constants.isconstant(simple_value))
                {
				
                simple_value = to_string(getnum<double>(simple_value, constants));
                }/*else {
                   // Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "getting " << simple_value << "\n";
                    constant_output = find_if(outputs.begin(), outputs.end(), FMToutputcomparator(simple_value));
                    if (constant_output != outputs.end() && constant_output->isconstant())
                        {
                        // Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "is constant " << "\n";
                        simple_value = to_string(constant_output->getconstantvalue());
                        }
                    }*/
		if (!simple_value.empty())
			{
			simplificaiton.push_back(simple_value);
			}
        
        map<string,double>nodes;
		/*for (const string& val : simplificaiton)
			{
			Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<"|"<< val<<"|"<< "\n";
			}*/
        FMTexpression simplification = FMTexpression(simplificaiton).simplify(nodes);
        return nodes;
        }

    FMToutput FMToptimizationparser::resume_output(const map<string,double>& nodes,
                                                   const vector<FMToutput>& outputs,
                                                   const vector<FMTtheme>& themes,
                                                   const FMTconstants& constants)
        {
        FMToutput final_output;
        for (map<string,double>::const_iterator output_it = nodes.begin(); output_it!=nodes.end();output_it++)
            {
            smatch out_match;
            if (regex_search(output_it->first, out_match, rxoutput))
                    {
                    string output_name = string(out_match[1])+ string(out_match[8]) + string(out_match[12])+ string(out_match[16]);
					//string output_name = string(out_match[2]) + string(out_match[9]) + string(out_match[13]) + string(out_match[17]) + string(out_match[19]);
					string allperiods;
					boost::erase_all(output_name, " ");
					if (output_name.find("_SUM(")!=string::npos)
						{
						allperiods = "SUM(";
						boost::erase_all(output_name, "_SUM(");
						}
					vector<FMToutput>::const_iterator target_out = find_if(outputs.begin(), outputs.end(), FMToutputcomparator(output_name));
                    if (target_out == outputs.end())
                        {
                        _exhandler->raise(FMTexc::WSundefined_output, _section, output_name + " at line " + to_string(_line), __LINE__, __FILE__);
                        }
                        string target_attribute = string(out_match[3])+string(out_match[10]);
						//string target_attribute = string(out_match[4]) + string(out_match[11]);
						boost::trim(target_attribute);
                        FMTtheme targeted_theme = target_out->targettheme(themes);
                        if (!target_attribute.empty())
                            {
                            if (targeted_theme.empty() ||
										(!targeted_theme.empty() &&
										(!targeted_theme.isattribute(target_attribute) &&
										!targeted_theme.isaggregate(target_attribute))))
                                        {
                                        _exhandler->raise(FMTexc::WSundefined_attribute, _section, target_attribute + " at line " + to_string(_line), __LINE__, __FILE__);
                                        }
                            }
                        string target_period = string(out_match[6]) + string(out_match[14]);
						//string target_period = string(out_match[7]) + string(out_match[15]);
                        int inttarget_period = -1;
                        FMTperbounds bounding;
                        if (!target_period.empty())
                                {
                                inttarget_period = getnum<int>(target_period, constants);
                                bounding = FMTperbounds(FMTwssect::Optimize, inttarget_period, inttarget_period);
								}
						if (!allperiods.empty())
							{
							int minbound = 1;
							int maxbound = numeric_limits<int>::max();
							bounding = FMTperbounds(FMTwssect::Optimize,maxbound, minbound);
							}
                        //copy the output and the specify the attribute and the periods!!!
                        FMToutput newoutput = target_out->boundto(themes, bounding, target_attribute);
                        newoutput *= output_it->second;
                        final_output += newoutput;
						}else{
                            //fuck_up
                            _exhandler->raise(FMTexc::FMTinvalid_constraint, _section, output_it->first + " at line " + to_string(_line), __LINE__, __FILE__);
                            }
                }
       // Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "final "<< string(final_output)<< "\n";
        return final_output;
        }


	FMTconstraint FMToptimizationparser::getconstraint(const string& line, const FMTconstants& constants,
												const vector<FMToutput>& outputs, const vector<FMTtheme>& themes)
		{
		smatch kmatch;
		FMTconstraint constraint;

        string rest = line;
        if (setgoal(constraint, line, constants))
            {
            rest = line.substr(0, rest.find("_GOAL"));
            boost::trim(rest);
            }

		if (regex_search(rest, kmatch, rxconstraints))
			{
			string target = string(kmatch[6])+string(kmatch[12]) + string(kmatch[15]);
			boost::trim(target);
			if (target.find("(")!=string::npos)
				{
				target += ")";
				}
			map<string, double> nodes = getequation(target, constants, outputs, target.size());
			nodes.erase("RHS");
			FMToutput targetout = resume_output(nodes, outputs, themes, constants);

			/*vector<FMToutput>::const_iterator target_out = find_if(outputs.begin(), outputs.end(), FMToutputcomparator(target));
			if (target_out == outputs.end())
				{
				_exhandler->raise(FMTexc::WSundefined_output, _section, target + " at line " + to_string(_line), __LINE__, __FILE__);
				}*/



			string keyword = kmatch[1];
			FMTconstrainttype ctype;

			if (keyword == "_EVEN")
			{
				ctype = FMTconstrainttype::FMTevenflow;

			}
			else if (keyword == "_NDY")
			{
				ctype = FMTconstrainttype::FMTnondeclining;
			}
			else if (keyword == "_SEQ")
			{
				ctype = FMTconstrainttype::FMTsequence;
			}
			//constraint = FMTconstraint(ctype, *target_out);
			constraint.setoutput(targetout);
			constraint.setconstrainttype(ctype);
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "OPTIMIZE " << int(constraint.getconstrainttype()) << "\n";
			string lower_variation = string(kmatch[14]) + string(kmatch[8]);
			double lower_var = 0;
			double higher_var = 0;
			if (!lower_variation.empty())
				{
				if (lower_variation.find('%') != string::npos)
					{
					lower_variation.erase(std::remove(lower_variation.begin(), lower_variation.end(), '%'), lower_variation.end());
					}
				lower_var = getnum<double>(lower_variation, constants);
				}
			string high_variation = string(kmatch[10]);
			if (!high_variation.empty())
				{
				if (high_variation.find('%') != string::npos)
					{
					high_variation.erase(std::remove(high_variation.begin(), high_variation.end(), '%'), high_variation.end());
					}
				higher_var = getnum<double>(high_variation, constants);
				}
			//if (!(lower_var== 0 && higher_var == 0))
				//{
			/*if (high_variation.empty())
				{
				lower_var = higher_var;
				}*/
			string yld_name = "Variation";
			constraint.addbounds(FMTyldbounds(FMTwssect::Optimize, yld_name, higher_var, lower_var));
				//}
			string start_str = string(kmatch[20]) + string(kmatch[23]);
			string stop_str = string(kmatch[22]);
			setperiods(constraint, start_str, stop_str, constants);




		}else if (regex_search(rest, kmatch, rxequations))
				{
				FMTconstrainttype cctype = FMTconstrainttype::FMTstandard;
				string lower_period = string(kmatch[15]) + string(kmatch[18]);
				string upper_period = string(kmatch[17]);
				setperiods(constraint, lower_period, upper_period, constants);
				string str_operator = string(kmatch[5]) + string(kmatch[6]) + string(kmatch[10]);
				double lower_bounds = numeric_limits<double>::lowest();
				double upper_bounds = numeric_limits<double>::infinity();
				string LHS = string(kmatch[3]) + string(kmatch[9]);
				string RHS = string(kmatch[7]) + string(kmatch[11]);
				string full_equation = LHS + string(1,'+') + RHS;
                map<string,double> nodes = getequation(full_equation,constants,outputs,LHS.size());
                double bound = nodes["RHS"];
                //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "equation of " << string(simplification)  << "\n";
                //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "bound of " << bound  << "\n";
                nodes.erase("RHS");
                double lower = 0;
				double upper = 0;
                if (str_operator == "=")
                    {
					lower = bound;
					upper = bound;
                    }
				else if (str_operator == ">=")
                    {
					lower = bound;
					upper = numeric_limits<double>::infinity();
                    }
				else if (str_operator == "<=")
                    {
                    lower = numeric_limits<double>::lowest();
					upper = bound;
                    }


                FMToutput final_output = resume_output(nodes,outputs,themes,constants);
                constraint.setoutput(final_output);
				constraint.setconstrainttype(cctype);
				string target = "RHS";
				constraint.addbounds(FMTyldbounds(FMTwssect::Optimize, target, upper, lower));
				}
		return constraint;
		}


	FMTconstraint FMToptimizationparser::getobjective(const string& line,
		const FMTconstants& constants,
		const vector<FMToutput>& outputs,
		const vector<FMTtheme>& themes) //need to fill the objective here!!!!!?!?!?!?!?!?!?!
		{
		smatch kmatch;
		FMTconstraint objective;
		if (regex_search(line, kmatch, rxobjectives))
			{
			string objective_type = kmatch[1];
			string lower_period = string(kmatch[7])+ string(kmatch[10]);
			string upper_period = string(kmatch[9]);
			string body = string(kmatch[3]);

			if (body.at(0)=='(')
				{
				body.erase(0,1);
				body.erase(body.size()-1);
				}
            string main_equation = body;
            string penalty_equation;
            string penalty_operator;
            if (body.find("_PENALTY")!=string::npos)
                {
                main_equation = body.substr(0,body.find("_PENALTY"));
                boost::trim(main_equation);
                penalty_operator = main_equation.back();
                penalty_equation = (body.substr(body.find("_PENALTY")));
                boost::trim(penalty_equation);
                if (!main_equation.empty())
                    {
                    main_equation.pop_back();
                    }
                }
            boost::trim(main_equation);
            smatch kpenalty;
            //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<" PENALTY OF |"<<penalty_equation<<"|"<<"\n";
            if (regex_search(penalty_equation, kpenalty, rxpenalty))
                {
                string penalty_values = kpenalty[3];
                string yldbound = "Penalty";
                double lower_penalty = 0;
                double upper_penalty = 0;
                if (penalty_values == "_ALL")
                    {
                    yldbound += (penalty_operator +penalty_values);
                    objective.addbounds(FMTyldbounds(FMTwssect::Optimize, yldbound, upper_penalty, lower_penalty));
                }else {
                    vector<string>penalty_split;
                    boost::split(penalty_split, penalty_values, boost::is_any_of(","));
                    for (const string& penalty_var : penalty_split)
                        {
                        string yldname = yldbound + penalty_operator +"_"+penalty_var;
                        objective.addbounds(FMTyldbounds(FMTwssect::Optimize, yldname, upper_penalty, lower_penalty));
                        }
                    }
				}
			FMToutput final_output;
			if (!main_equation.empty())
				{
				map<string, double>nodes = getequation(main_equation, constants, outputs);
				nodes.erase("RHS");
				final_output = resume_output(nodes, outputs, themes, constants);
				}
			//Need to potentialy build a new output with the body...
			//vector<string>body_split;
			//boost::split(body_split,body,boost::is_any_of("-*/+"));

			objective.setoutput(final_output);
			FMTconstrainttype cctype;
			if (objective_type == "_MAX")
			{
				cctype = FMTconstrainttype::FMTMAXobjective;
			}
			else if (objective_type == "_MIN")
			{
				cctype = FMTconstrainttype::FMTMINobjective;
			}
			else if (objective_type == "_MAXMIN")
			{
				cctype = FMTconstrainttype::FMTMAXMINobjective;
			}
			else if (objective_type == "_MINMAX")
			{
				cctype = FMTconstrainttype::FMTMINMAXobjective;
			}
			objective.setconstrainttype(cctype);
			setperiods(objective, lower_period, upper_period, constants);
			}
		return objective;
		}

	FMToptimizationsection FMToptimizationparser::getsection(const string& line) const
		{
		FMToptimizationsection section = FMToptimizationsection::none;
		smatch kmatch;
		if (regex_search(line, kmatch, rxsections))
			{
			string target = string(kmatch[2]);
			if (target=="OBJECTIVE")
				{
				section = FMToptimizationsection::objective;
				}
				else if (target == "CONSTRAINTS")
				{
				section = FMToptimizationsection::constraints;
				}
				else if (target == "EXCLUDE")
				{
				section = FMToptimizationsection::exclude;
				}
				else if (target == "FORMAT")
				{
				section = FMToptimizationsection::format;
				}
			}
		return section;
		}

	vector<FMTconstraint> FMToptimizationparser::read(const vector<FMTtheme>& themes,
									const vector<FMTaction>& actions,
									const FMTconstants& constants,
									const map<string, vector<string>>& actions_aggregate,
									const vector<FMToutput>& outputs,
									vector<FMTaction>& excluded,
									const string& location)
		{
		ifstream optimizestream(location);
		string line;
		vector<FMTconstraint>constraints;
		if (FMTparser::tryopening(optimizestream, location))
			{
			FMToptimizationsection section = FMToptimizationsection::none;
			while (optimizestream.is_open())
				{
				//line = getcleanlinewfor(optimizestream, themes, constants);
				line = getoptline(optimizestream, themes, constants, outputs);
				if (!line.empty())
					{
				FMToptimizationsection newsection = getsection(line);
				if (newsection != FMToptimizationsection::none)
					{
					section = newsection;
				}else {
					switch (section)
						{
						case FMToptimizationsection::objective:
							{
							FMTconstraint objective = getobjective(line, constants, outputs,themes);

							if (objective.emptyperiod())
								{
								_exhandler->raise(FMTexc::FMTmissingobjective, _section, " at line " + to_string(_line), __LINE__, __FILE__);
								}
							constraints.push_back(objective);
							break;
							}
						case FMToptimizationsection::constraints:
							{
							FMTconstraint constraint = getconstraint(line, constants, outputs, themes);
							/*for (const FMToutputsource& src : constraint.getsources())
                                {
                                Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<string(src)<<"\n";
                                Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "targetss " <<src.emptyperiod()<<" "<<src.getperiodlowerbound()<<" "<<src.getperiodupperbound()<< "\n";
                                }*/
							//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "targetss " << constraint.issingleperiod()<<" str: "<<line<< "\n";
							constraints.push_back(constraint);
							//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << string(constraints.at(1)) << "\n";
							break;
							}

						case FMToptimizationsection::exclude:
							{

							smatch kmatch;
							if (regex_search(line, kmatch, rxexclude))
								{
								string action_name = kmatch[2];
								vector<string>action_names;
								if (actions_aggregate.find(action_name) != actions_aggregate.end())
									{
									for (const string& actname : actions_aggregate.at(action_name))
										{
										action_names.push_back(actname);
										}
								}else {
									action_names.push_back(action_name);
									}
								int period_lower = getnum<int>(string(kmatch[6])+ string(kmatch[9]),constants);
								int period_upper = numeric_limits<int>::max();
								string str_upper = string(kmatch[8]);
								if (!str_upper.empty() && str_upper != "_LENGTH")
									{
									period_upper  = getnum<int>(str_upper, constants);
								}else if (str_upper.empty())
									{
									period_upper = period_lower;
									}
								for (const string& target_action : action_names)
									{
									vector<FMTaction>::iterator actit = std::find_if(excluded.begin(), excluded.end(), FMTactioncomparator(target_action));
									if (actit != excluded.end())
										{
										for (vector<FMTspec>::iterator spec_it = actit->databegin(); spec_it != actit->dataend(); ++spec_it)
											{
											spec_it->setbounds(FMTperbounds(FMTwssect::Action, period_upper, period_lower));
											}
										}
									}
								}
							break;
							}
						default:
							{

							break;
							}
						};
					}
				}
				}
			}
		return constraints;
		}

	void FMToptimizationparser::write(const vector<FMTconstraint>& constraints, string location)
		{
		ofstream optimizestream;
		optimizestream.open(location);
		if (tryopening(optimizestream, location))
			{
			optimizestream << "*OBJECTIVE " << "\n";
			optimizestream << string(constraints.at(0));
			if (constraints.size()-1 > 0)
                {
                optimizestream << "*CONSTRAINTS " << "\n";
                for (size_t cid = 1; cid < constraints.size(); ++cid)
                    {
                    optimizestream << string(constraints.at(cid));
                    }
                }
			optimizestream.close();
			}
		}

	std::queue<string> FMToptimizationparser::geteachlines(const string& line, const vector<FMToutput>& outputs, const vector<FMTtheme>& themes) const
		{
		int themeid = -1;
		vector<FMToutput>::const_iterator target_out;
		string subline = line;
		string keyword = "_EACH";
		while (subline.find(keyword)!= string::npos)
			{
			boost::erase_all(subline, " ");
			int endoutputlocation = static_cast<int>((subline.find(keyword)))-1;
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "line " << subline << "\n";
			char outchar = subline.at(endoutputlocation);
			while (outchar == '(') //emptystuff
				{
				--endoutputlocation;
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << outchar <<" " << endoutputlocation << "\n";
				if (endoutputlocation >= 0)
					{
					outchar = line.at(endoutputlocation);
					}
				}
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<< endoutputlocation << "starting at " << outchar << "\n";
			string output_name = "";
			while (!FMToperator(string(1, outchar)).valid()
				&& outchar!='(' && endoutputlocation >=0)
				{
				output_name.insert(output_name.begin(),outchar);
				--endoutputlocation;
				if (endoutputlocation >= 0)
					{
					outchar = line.at(endoutputlocation);
					}
				}
			target_out = find_if(outputs.begin(), outputs.end(), FMToutputcomparator(output_name));
			if (target_out==outputs.end())
				{
				_exhandler->raise(FMTexc::WSundefined_output, _section, output_name + " at line " + to_string(_line), __LINE__, __FILE__);
				}
			if (themeid >= 0)
				{
				if (themeid != target_out->targetthemeid())
					{
					_exhandler->raise(FMTexc::FMTinvalid_constraint, _section, "Non matching _EACH outputs attributes at line " + to_string(_line), __LINE__, __FILE__);
					}
				}
			themeid = target_out->targetthemeid();
			boost::replace_all(subline, keyword, "");
			//remove _EACH in subline
			//subline
			}
		std::queue<string>valuestoreplace;
		for (const string& value : target_out->getdecomposition(themes))
			{
			valuestoreplace.push(boost::replace_all_copy(line, "_EACH", value));
			}
		return valuestoreplace;
		}

	string FMToptimizationparser::getoptline(ifstream& stream, const vector<FMTtheme>& themes,
		const FMTconstants& cons, const vector<FMToutput>& outputs)
		{
		string line = ""; 
		if (!ineach.empty())
			{
			line = ineach.front();
			ineach.pop();
		}else {
			line = getcleanlinewfor(stream, themes, cons);
			if (line.find("_EACH") != string::npos)
				{
				ineach = geteachlines(line, outputs, themes);
				line = ineach.front();
				ineach.pop();
				}
			}	
		return line;
		}
}
