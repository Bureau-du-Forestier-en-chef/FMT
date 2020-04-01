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

namespace Parser
{

	FMToptimizationparser::FMToptimizationparser() : FMTparser(),
		rxsections("^(\\*)([^\\s^\\t]*)", std::regex_constants::ECMAScript | std::regex_constants::icase),
		rxobjectives("^(_MAXMIN|_MINMAX|_MAX|_MIN|_GOAL)([\\s\\t]*)(.+)([\\s\\t])((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))", std::regex_constants::ECMAScript | std::regex_constants::icase),
		rxexclude("^(\\*EXCLUDE)([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))", std::regex_constants::ECMAScript | std::regex_constants::icase),
		rxconstraints("^(_EVEN|_NDY|_SEQ)([\\s\\t]*)(\\()((([^,]*)(,)([\\d\\.]*%|[\\d\\.]*)(,)([\\d\\.]*%|[\\d\\.]*))|(([^,]*)(,)([\\d\\.]*%|[\\d\\.]*))|([^\\)^,]*))(\\)*)([\\s\\t]*)((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))", std::regex_constants::ECMAScript | std::regex_constants::icase),
		rxequations("^(((.+)((<=)|(>=))(.+))|((.+)(=)(.+)))([\\s\\t])((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))", std::regex_constants::ECMAScript | std::regex_constants::icase),
		rxgoal("^(.+)(_GOAL)(\\()([^,]*)(,)([^\\)]*)(\\))", std::regex_constants::ECMAScript | std::regex_constants::icase),
		rxoutput("^(.+)(\\()([^)]*)(\\))(\\[)(#.+|[-\\d]*)(\\])|(.+)(\\()([^)]*)(\\))|(.+)(\\[)(#.+|[-\\d]*)(\\])|(.+)", std::regex_constants::ECMAScript | std::regex_constants::icase),
		rxpenalty("^(_PENALTY)(\\()([^\\)]*)(\\))", std::regex_constants::ECMAScript | std::regex_constants::icase),
		rxspecialoutput("^(_AVG|_SUM)(\\()(([^,]*)(,)(([^,]*)(([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))|(.+))(\\))", std::regex_constants::ECMAScript | std::regex_constants::icase),
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
		rxspecialoutput(rhs.rxspecialoutput),
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
			rxspecialoutput = rhs.rxspecialoutput;
			ineach = rhs.ineach;
			}
		return *this;
		}
	bool FMToptimizationparser::setgoal(Core::FMTconstraint& constraint, const std::string& line, const Core::FMTconstants& constants)
		{
		std::smatch kmatch;
		if (std::regex_search(line, kmatch, rxgoal))
			{
			const std::string target = "GOAL_"+ std::string(kmatch[4]);
			const std::string value = kmatch[6];
			const double goal_var = getnum<double>(value, constants);
			constraint.addbounds(Core::FMTyldbounds(Core::FMTwssect::Optimize, target, goal_var, goal_var));
			return true;
			}
		return false;
		}

	void FMToptimizationparser::setperiods(Core::FMTconstraint& constraint, const std::string& lower, const std::string& upper, const Core::FMTconstants& constants)
		{
		const int startperiod = getnum<int>(lower, constants);
		int stopperiod = startperiod;
		if (!upper.empty() && upper != "_LENGTH")
		{
			stopperiod = getnum<int>(upper, constants);
		}else if (upper == "_LENGTH")
			{
			stopperiod = std::numeric_limits<int>::max();
			}
		constraint.setlength(startperiod, stopperiod);
		}

	std::map<std::string,double>FMToptimizationparser::getequation(const std::string& line, const Core::FMTconstants& constants,
                                                         const std::vector<Core::FMToutput>& outputs,size_t lhssize)
        {
		std::vector<std::string>simplificaiton;
        size_t eq_location = 0;
		std::string simple_value;
		std::vector<Core::FMToutput>::const_iterator constant_output;
		bool lastcharspace = false;
        for(char strvalue : line)
            {
            if (lastcharspace && (strvalue == '+' || strvalue == '-' || strvalue == '/' || strvalue == '*') && (simple_value.empty() || (!simple_value.empty() && simple_value.back() != '[')))
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
                            simple_value = std::to_string(getnum<double>(simple_value, constants));
                            }
						if (!simple_value.empty())
							{
							simplificaiton.push_back(simple_value);
							}
						 simple_value.clear();
                        }else if(!simplificaiton.empty())
                            {
                            simplificaiton.pop_back();
                            }
						std::string pushedval = std::string(1, strvalue);
						boost::trim(pushedval);
						if (!pushedval.empty())
							{
							simplificaiton.push_back(pushedval);
							}
                    
                    }else{
                        simple_value += strvalue;
                        }
                    ++eq_location;
					if (isspace(strvalue))
					{
						lastcharspace = true;
					}else {
						lastcharspace = false;
					}

                }
            boost::trim(simple_value);
            if (isnum(simple_value) || constants.isconstant(simple_value))
                {
				
                simple_value = std::to_string(getnum<double>(simple_value, constants));
                }
		if (!simple_value.empty())
			{
			simplificaiton.push_back(simple_value);
			}
        
		std::map<std::string,double>nodes;
        const Core::FMTexpression simplification = Core::FMTexpression(simplificaiton).simplify(nodes);
        return nodes;
        }

    Core::FMToutput FMToptimizationparser::resume_output(const std::map<std::string,double>& nodes,
                                                   const std::vector<Core::FMToutput>& outputs,
                                                   const std::vector<Core::FMTtheme>& themes,
                                                   const Core::FMTconstants& constants)
        {
		Core::FMToutput final_output;
        for (std::map<std::string,double>::const_iterator output_it = nodes.begin(); output_it!=nodes.end();output_it++)
            {
			std::smatch out_match;
            if (std::regex_search(output_it->first, out_match, rxoutput))
                    {
					std::string output_name = std::string(out_match[1])+ std::string(out_match[8]) + std::string(out_match[12])+ std::string(out_match[16]);
					boost::erase_all(output_name, " ");
					std::smatch special_match;
					std::string specialtype;
					//int minbound = 1;
					//int maxbound = std::numeric_limits<int>::max();
					if (std::regex_search(output_it->first, special_match, rxspecialoutput))
						{
						specialtype = special_match[1];
						const std::string lowerperiod = std::string(special_match[7]) + std::string(special_match[12]);
						const std::string upperperiod = std::string(special_match[11]) + std::string(special_match[12]);
						if (!lowerperiod.empty() && !upperperiod.empty())
							{
							//minbound = getnum<int>(lowerperiod, constants);
							//maxbound = getnum<int>(upperperiod, constants);
							}
						output_name = std::string(special_match[4])+ std::string(special_match[13]);
						if (!std::regex_search(output_name, out_match, rxoutput))
							{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_constraint, _section, output_name + " at line " + std::to_string(_line), __LINE__, __FILE__);
						}
						else {
							output_name = std::string(out_match[1]) + std::string(out_match[8]) + std::string(out_match[12]) + std::string(out_match[16]);
						}
						boost::erase_all(output_name, " ");
						}
					std::vector<Core::FMToutput>::const_iterator target_out = find_if(outputs.begin(), outputs.end(), Core::FMToutputcomparator(output_name));
                    if (target_out == outputs.end())
                        {
                        _exhandler->raise(Exception::FMTexc::WSundefined_output, _section, output_name + " at line " + std::to_string(_line), __LINE__, __FILE__);
                        }
						std::string target_attribute = std::string(out_match[3])+ std::string(out_match[10]);
						boost::trim(target_attribute);
                        const Core::FMTtheme targeted_theme = target_out->targettheme(themes);
                        if (!target_attribute.empty())
                            {
                            if (targeted_theme.empty() ||
										(!targeted_theme.empty() &&
										(!targeted_theme.isattribute(target_attribute) &&
										!targeted_theme.isaggregate(target_attribute))))
                                        {
                                        _exhandler->raise(Exception::FMTexc::WSundefined_attribute, _section, target_attribute + " at line " + std::to_string(_line), __LINE__, __FILE__);
                                        }
                            }
						const std::string target_period = std::string(out_match[6]) + std::string(out_match[14]);
                        int inttarget_period = -1;
                        Core::FMTperbounds bounding;
                        if (!target_period.empty())
                                {
                                inttarget_period = getnum<int>(target_period, constants);
                                bounding = Core::FMTperbounds(Core::FMTwssect::Optimize, inttarget_period, inttarget_period);
								}
                        //copy the output and the specify the attribute and the periods!!!
                        Core::FMToutput newoutput = target_out->boundto(themes, bounding, specialtype, target_attribute);
                        newoutput *= output_it->second;
                        final_output += newoutput;
						}else{
                            _exhandler->raise(Exception::FMTexc::FMTinvalid_constraint, _section, output_it->first + " at line " + std::to_string(_line), __LINE__, __FILE__);
                            }
                }
        return final_output;
        }


	Core::FMTconstraint FMToptimizationparser::getconstraint(const std::string& line, const Core::FMTconstants& constants,
												const std::vector<Core::FMToutput>& outputs, const std::vector<Core::FMTtheme>& themes)
		{
		std::smatch kmatch;
		Core::FMTconstraint constraint;
		std::string rest = line;
        if (setgoal(constraint, line, constants))
            {
            rest = line.substr(0, rest.find("_GOAL"));
            boost::trim(rest);
            }

		if (std::regex_search(rest, kmatch, rxconstraints))
			{
			std::string target = std::string(kmatch[6])+ std::string(kmatch[12]) + std::string(kmatch[15]);
			boost::trim(target);
			if (target.find("(")!= std::string::npos && target.find(")") == std::string::npos)
				{
				target += ")";
				}
			std::map<std::string, double> nodes = getequation(target, constants, outputs, target.size());
			nodes.erase("RHS");
			const Core::FMToutput targetout = resume_output(nodes, outputs, themes, constants);
			const std::string keyword = kmatch[1];
			Core::FMTconstrainttype ctype = Core::FMTconstrainttype::FMTstandard;

			if (keyword == "_EVEN")
			{
				ctype = Core::FMTconstrainttype::FMTevenflow;

			}
			else if (keyword == "_NDY")
			{
				ctype = Core::FMTconstrainttype::FMTnondeclining;
			}
			else if (keyword == "_SEQ")
			{
				ctype = Core::FMTconstrainttype::FMTsequence;
			}
			constraint.setoutput(targetout);
			constraint.setconstrainttype(ctype);
			std::string lower_variation = std::string(kmatch[14]) + std::string(kmatch[8]);
			double lower_var = 0;
			double higher_var = 0;
			if (!lower_variation.empty())
				{
				if (lower_variation.find('%') != std::string::npos)
					{
					lower_variation.erase(std::remove(lower_variation.begin(), lower_variation.end(), '%'), lower_variation.end());
					}
				lower_var = getnum<double>(lower_variation, constants);
				}
			std::string high_variation = std::string(kmatch[10]);
			if (!high_variation.empty())
				{
				if (high_variation.find('%') != std::string::npos)
					{
					high_variation.erase(std::remove(high_variation.begin(), high_variation.end(), '%'), high_variation.end());
					}
				higher_var = getnum<double>(high_variation, constants);
				}
			const std::string yld_name = "Variation";
			constraint.addbounds(Core::FMTyldbounds(Core::FMTwssect::Optimize, yld_name, higher_var, lower_var));
			const std::string start_str = std::string(kmatch[20]) + std::string(kmatch[23]);
			const std::string stop_str = std::string(kmatch[22]);
			setperiods(constraint, start_str, stop_str, constants);
		}else if (std::regex_search(rest, kmatch, rxequations))
				{
				Core::FMTconstrainttype cctype = Core::FMTconstrainttype::FMTstandard;
				std::string lower_period = std::string(kmatch[15]) + std::string(kmatch[18]);
				std::string upper_period = std::string(kmatch[17]);
				setperiods(constraint, lower_period, upper_period, constants);
				const std::string str_operator = std::string(kmatch[5]) + std::string(kmatch[6]) + std::string(kmatch[10]);
				const std::string LHS = std::string(kmatch[3]) + std::string(kmatch[9]);
				const std::string RHS = std::string(kmatch[7]) + std::string(kmatch[11]);
				const std::string full_equation = LHS + std::string(1,'+') + RHS;
				std::map<std::string,double> nodes = getequation(full_equation,constants,outputs,LHS.size());
                double bound = nodes["RHS"];
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
					upper = std::numeric_limits<double>::infinity();
                    }
				else if (str_operator == "<=")
                    {
                    lower = std::numeric_limits<double>::lowest();
					upper = bound;
                    }
                Core::FMToutput final_output = resume_output(nodes,outputs,themes,constants);
                constraint.setoutput(final_output);
				constraint.setconstrainttype(cctype);
				constraint.setrhs(lower,upper);
				}
		return constraint;
		}


	Core::FMTconstraint FMToptimizationparser::getobjective(const std::string& line,
		const Core::FMTconstants& constants,
		const std::vector<Core::FMToutput>& outputs,
		const std::vector<Core::FMTtheme>& themes)
		{
		std::smatch kmatch;
		Core::FMTconstraint objective;
		if (std::regex_search(line, kmatch, rxobjectives))
			{
			const std::string objective_type = kmatch[1];
			const std::string lower_period = std::string(kmatch[7])+ std::string(kmatch[10]);
			const std::string upper_period = std::string(kmatch[9]);
			std::string body = std::string(kmatch[3]);
			if (body.at(0)=='(')
				{
				body.erase(0,1);
				body.erase(body.size()-1);
				}
			std::string main_equation = body;
			std::string penalty_equation;
			std::string penalty_operator;
            if (body.find("_PENALTY")!= std::string::npos)
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
			std::smatch kpenalty;
            if (std::regex_search(penalty_equation, kpenalty, rxpenalty))
                {
				const std::string penalty_values = kpenalty[3];
				std::string yldbound = "Penalty";
                double lower_penalty = 0;
                double upper_penalty = 0;
                if (penalty_values == "_ALL")
                    {
                    yldbound += (penalty_operator +penalty_values);
                    objective.addbounds(Core::FMTyldbounds(Core::FMTwssect::Optimize, yldbound, upper_penalty, lower_penalty));
                }else {
					std::vector<std::string>penalty_split;
                    boost::split(penalty_split, penalty_values, boost::is_any_of(","));
                    for (const std::string& penalty_var : penalty_split)
                        {
						const std::string yldname = yldbound + penalty_operator +"_"+penalty_var;
                        objective.addbounds(Core::FMTyldbounds(Core::FMTwssect::Optimize, yldname, upper_penalty, lower_penalty));
                        }
                    }
				}
			Core::FMToutput final_output;
			if (!main_equation.empty())
				{
				std::map<std::string, double>nodes = getequation(main_equation, constants, outputs);
				nodes.erase("RHS");
				final_output = resume_output(nodes, outputs, themes, constants);
				}
			objective.setoutput(final_output);
			Core::FMTconstrainttype cctype = Core::FMTconstrainttype::FMTstandard;
			if (objective_type == "_MAX")
			{
				cctype = Core::FMTconstrainttype::FMTMAXobjective;
			}
			else if (objective_type == "_MIN")
			{
				cctype = Core::FMTconstrainttype::FMTMINobjective;
			}
			else if (objective_type == "_MAXMIN")
			{
				cctype = Core::FMTconstrainttype::FMTMAXMINobjective;
			}
			else if (objective_type == "_MINMAX")
			{
				cctype = Core::FMTconstrainttype::FMTMINMAXobjective;
			}
			objective.setconstrainttype(cctype);
			setperiods(objective, lower_period, upper_period, constants);
			}
		return objective;
		}

	FMToptimizationsection FMToptimizationparser::getsection(const std::string& line) const
		{
		FMToptimizationsection section = FMToptimizationsection::none;
		std::smatch kmatch;
		if (std::regex_search(line, kmatch, rxsections))
			{
			const std::string target = std::string(kmatch[2]);
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

	std::vector<Core::FMTconstraint> FMToptimizationparser::read(const std::vector<Core::FMTtheme>& themes,
									const std::vector<Core::FMTaction>& actions,
									const Core::FMTconstants& constants,
									const std::vector<Core::FMToutput>& outputs,
									std::vector<Core::FMTaction>& excluded,
									const std::string& location)
		{
		std::vector<Core::FMTconstraint>constraints;
		if (!location.empty())
			{
			std::ifstream optimizestream(location);
			if (FMTparser::tryopening(optimizestream, location))
				{
				FMToptimizationsection section = FMToptimizationsection::none;
				while (optimizestream.is_open())
					{
					std::string line = getoptline(optimizestream, themes, constants, outputs);
					if (!line.empty())
						{
					FMToptimizationsection newsection = getsection(line);
					if (newsection == FMToptimizationsection::exclude)
						{
						boost::trim(line);
						section = newsection;
						}
					if (newsection != FMToptimizationsection::none && 
						(newsection != FMToptimizationsection::exclude || 
						(newsection == FMToptimizationsection::exclude && line == "*EXCLUDE")))
						{
						section = newsection;
					}else {
						switch (section)
							{
							case FMToptimizationsection::objective:
								{
								Core::FMTconstraint objective = getobjective(line, constants, outputs,themes);
								if (objective.emptyperiod())
									{
									_exhandler->raise(Exception::FMTexc::FMTmissingobjective, _section, " at line " + std::to_string(_line), __LINE__, __FILE__);
									}
								constraints.push_back(objective);
								break;
								}
							case FMToptimizationsection::constraints:
								{
								Core::FMTconstraint constraint = getconstraint(line, constants, outputs, themes);
								constraints.push_back(constraint);
								break;
								}
							case FMToptimizationsection::exclude:
								{
								if (line.find("*EXCLUDE")== std::string::npos)
									{
									line = "*EXCLUDE " + line;
									}
								std::smatch kmatch;
								if (std::regex_search(line, kmatch, rxexclude))
									{
									std::string action_name = kmatch[3];
									const std::vector<const Core::FMTaction*>action_ptrs = Core::FMTactioncomparator(action_name).getallaggregates(actions);
									const int period_lower = getnum<int>(std::string(kmatch[7])+ std::string(kmatch[10]),constants)-1;
									int period_upper = std::numeric_limits<int>::max();
									const std::string str_upper = std::string(kmatch[9]);
									if (!str_upper.empty() && str_upper != "_LENGTH")
										{
										period_upper  = getnum<int>(str_upper, constants);
									}else if (str_upper.empty())
										{
										period_upper = period_lower;
										}
									for (const Core::FMTaction* target_actionptr : action_ptrs)
										{
										std::vector<Core::FMTspec>newspecs;
										std::vector<Core::FMTmask>newmask;
										std::vector<Core::FMTaction>::iterator actit = std::find_if(excluded.begin(), excluded.end(), Core::FMTactioncomparator(target_actionptr->getname()));
										if (actit != excluded.end())
											{
			
											for (auto& specobject : *actit)
												{
												if (str_upper != "_LENGTH")
													{
													Core::FMTspec upperspec = specobject.second;
														constexpr int max_upper = std::numeric_limits<int>::max() - 2;
														const int upper = period_upper + 1;
														upperspec.setbounds(Core::FMTperbounds(Core::FMTwssect::Action, max_upper, upper));
														newspecs.push_back(upperspec);
														newmask.push_back(specobject.first);
														}
													const int startperiod_upper = period_lower - 1;
													specobject.second.setbounds(Core::FMTperbounds(Core::FMTwssect::Action, period_lower, startperiod_upper));
		
													}
												for (size_t newspec = 0; newspec < newspecs.size();++newspec)
													{
													actit->push_back(newmask.at(newspec),newspecs.at(newspec));
													}
												actit->update();
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
			}
		return constraints;
		}

	void FMToptimizationparser::write(const std::vector<Core::FMTconstraint>& constraints, std::string location)
		{
		std::ofstream optimizestream;
		optimizestream.open(location);
		if (tryopening(optimizestream, location))
			{
			optimizestream << "*OBJECTIVE " << "\n";
			optimizestream << std::string(constraints.at(0));
			if (constraints.size()-1 > 0)
                {
                optimizestream << "*CONSTRAINTS " << "\n";
                for (size_t cid = 1; cid < constraints.size(); ++cid)
                    {
                    optimizestream << std::string(constraints.at(cid));
                    }
                }
			optimizestream.close();
			}
		}

	std::queue<std::string> FMToptimizationparser::geteachlines(const std::string& line, const Core::FMTconstants& constants,const std::vector<Core::FMToutput>& outputs, const std::vector<Core::FMTtheme>& themes) const
		{
		int themeid = -1;
		std::vector<Core::FMToutput>::const_iterator target_out;
		std::string subline = line;
		const std::string keyword = "_EACH";
		std::queue<std::string>valuestoreplace;
		while (subline.find(keyword)!= std::string::npos)
			{
			boost::erase_all(subline, " ");
			int endoutputlocation = static_cast<int>((subline.find(keyword)))-1;
			char outchar = subline.at(endoutputlocation);
			while (outchar == '(')
				{
				--endoutputlocation;
				if (endoutputlocation >= 0)
					{
					outchar = subline.at(endoutputlocation);
					}
				}
			std::string output_name = "";
			while (!Core::FMToperator(std::string(1, outchar)).valid()
				&& outchar!='(' && endoutputlocation >=0)
				{
				output_name.insert(output_name.begin(),outchar);
				--endoutputlocation;
				if (endoutputlocation >= 0)
					{
					outchar = subline.at(endoutputlocation);
					}
				}
			if (output_name.find("#")!= std::string::npos)//constant
				{

			}else{
				target_out = find_if(outputs.begin(), outputs.end(), Core::FMToutputcomparator(output_name));
				if (target_out==outputs.end())
					{
					_exhandler->raise(Exception::FMTexc::WSundefined_output, _section, output_name + " at line " + std::to_string(_line), __LINE__, __FILE__);
					}
				if (themeid >= 0)
					{
					if (themeid != target_out->targetthemeid())
						{
						_exhandler->raise(Exception::FMTexc::FMTinvalid_constraint, _section, "Non matching _EACH outputs attributes at line " + std::to_string(_line), __LINE__, __FILE__);
						}
					}
				
				themeid = target_out->targetthemeid();
				boost::replace_all(subline, keyword, "");
				}
			for (const std::string& value : target_out->getdecomposition(themes))
				{
				valuestoreplace.push(boost::replace_all_copy(line, "_EACH", value));
				}
			}
		return valuestoreplace;
		}

	std::string FMToptimizationparser::getoptline(std::ifstream& stream, const std::vector<Core::FMTtheme>& themes,
		const Core::FMTconstants& cons, const std::vector<Core::FMToutput>& outputs)
		{
		std::string line = "";
		if (!ineach.empty())
			{
			line = ineach.front();
			ineach.pop();
		}else {
			line = getcleanlinewfor(stream, themes, cons);
			if (line.find("_EACH") != std::string::npos)
				{
				ineach = geteachlines(line, cons,outputs, themes);
				line = ineach.front();
				ineach.pop();
				}
			}	
		return line;
		}
}
