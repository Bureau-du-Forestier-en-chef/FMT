/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMToptimizationparser.h"
#include <boost/icl/interval.hpp>
#include <boost/icl/interval_set.hpp>
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMToutput.h"
#include "FMTconstraint.h"
#include "FMTexpression.h"
#include "FMTaction.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include "FMTexceptionhandler.h"
#include "FMTyields.h"
#include <boost/numeric/interval.hpp>
//#include <boost/regex.hpp>


namespace Parser
{

	const boost::regex FMToptimizationparser::m_rxsections = boost::regex("^(\\*)([^\\s^\\t]*)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToptimizationparser::m_rxobjectives = boost::regex("^(_MAXMIN|_MINMAX|_MAX|_MIN|_GOAL)([\\s\\t]*)(.+)([\\s\\t])((([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToptimizationparser::m_rxexclude = boost::regex("^(\\*EXCLUDE)([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToptimizationparser::m_rxconstraints = boost::regex("^(_EVEN|_NDY|_SEQ)([\\s\\t]*)(\\()((([^,]*)(,)([\\s\\t]*)([\\d\\.]*%|[\\d\\.]*)([\\s\\t]*)(,)([\\s\\t]*)([\\d\\.]*%|[\\d\\.]*))|(([^,]*)(,)([\\s\\t]*)([\\d\\.]*%|[\\d\\.]*))|([^,]*))([\\s\\t]*)(\\))([\\s\\t]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	//const boost::regex FMToptimizationparser::rxequations = boost::regex("^(.+)((((<=)|(>=))(.+))|((.+)((=))(.+)))(?<=[^,])[\\s\\t](?=\\d)(.+)", boost::regex_constants::ECMAScript);
	const boost::regex FMToptimizationparser::m_rxequations = boost::regex("^(.+)((((<=)|(>=))(.+))|((.+)((=))(.+)))(?=[^,])[\\s\\t](?=\\d)(.+)", boost::regex_constants::ECMAScript);
	const boost::regex FMToptimizationparser::m_m_ENDing = boost::regex("^(.+)(((_GOAL)(\\()([^,]*)(,)([^\\)]*)(\\)))|(_SETFROMGLOBAL|_SETFROMLOCAL|_REIGNORE)([\\s\\t]*)(\\()([\\s\\t]*)(.+)([\\s\\t]*)(\\))|(_REPLICATE)([\\s\\t]*)(\\()([\\s\\t]*)(.+)([\\s\\t]*)(\\)))", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToptimizationparser::m_rxoutput = boost::regex("^(.+)(\\()([^)]*)(\\))(\\[)(#.+|[-\\d]*)(\\])|([^\\[]*)(\\()([^)]*)(\\))|(.+)(\\[)(#.+|[-\\d]*)(\\])|(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToptimizationparser::m_rxpenalty = boost::regex("^(_PENALTY)(\\()([^\\)]*)(\\))", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToptimizationparser::m_rxspecialoutput = boost::regex("^(_AVG|_SUM)(\\()(([^,]*)(,)(([^,]*)(([\\d]*|#.+)(\\.\\.)(#.+|_LENGTH|[\\d]*))|(#.+|[\\d]*))|(.+))(\\))", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToptimizationparser::m_rxspatial = boost::regex("^(_SIZE|_ADJACENCY|_RANDOM|_GROUP)([\\s\\t]*)(\\()(.+)(\\))([\\s\\t]*)(>=|<=|=)([\\s\\t]*)(#[^\\s^\\t]*|[\\d]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToptimizationparser::m_rxspecialobjective = boost::regex("^(.+)(_SETGLOBALSCHEDULE)(\\()([\\d]*)(\\))", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToptimizationparser::m_rxstartwithoperator = boost::regex("([\\s\\t]*)([-+])(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	
	FMToptimizationparser::FMToptimizationparser() :
		FMTparser()/*,
		m_ineach()*/
		{
		setSection(Core::FMTsection::Optimize);
		}

	bool FMToptimizationparser::setending(Core::FMTconstraint& constraint,std::string& line, const Core::FMTconstants& constants)
		{
		boost::smatch kmatch;
		try{
		if (boost::regex_search(line, kmatch, m_m_ENDing))
			{
			const std::string target = std::string(kmatch[4]) + std::string(kmatch[10]) + std::string(kmatch[17]);
			const std::string numvalue = std::string(kmatch[8]) + std::string(kmatch[14]);
			if (target == "_REPLICATE")
			{
				int period = 1;//should selected only the targeted period?!?!?!
				for (const std::vector<double>& periodvalues : getreplicatechanges(std::string(kmatch[21])))
					{
					int replicateid = 0;
					for (const double& value : periodvalues)
						{
						//Replicate_replicatedid_perioid
						const std::string replicatename = "REPLICATE_" + std::to_string(replicateid) + "_" + std::to_string(period);
						constraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, replicatename, value, value));
						++replicateid;
						}
					++period;
					}
				
			}else {
				const double variale_value = getNum<double>(numvalue, constants);
				std::string yieldtarget(target);
				if (target == "_GOAL")
				{
					constraint.setgoal(std::string(kmatch[6]), variale_value);
					//yieldtarget = "GOAL_" + std::string(kmatch[6]);
				}
				else {
					constraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, yieldtarget, variale_value, variale_value));
				}
				
				}
			line = line.substr(0, line.find(target));
			boost::trim(line);
			if (target != "_GOAL" && !line.empty())
				{
				setending(constraint, line, constants);
				}
			return true;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("at line " + line,"FMToptimizationparser::setending", __LINE__, __FILE__);
			}
		return false;
		}


	std::map<std::string,double>FMToptimizationparser::getEquation(const std::string& pm_line, const Core::FMTconstants& p_constants,
												const Core::FMTyields& p_yields, const std::vector<Core::FMToutput>& p_outputs,size_t p_lhssize)
        {
		std::map<std::string, double>nodes;
		try {
			std::vector<std::string>simplificaiton;
			size_t eqm_location = 0;
			std::string simple_value;
			bool lastcharspace = false;
			std::map<std::string, std::string>specialCases;
			for (char strvalue : pm_line)
			{
				if (lastcharspace && (strvalue == '+' || strvalue == '-' || strvalue == '/' || strvalue == '*') && (simple_value.empty() || (!simple_value.empty() && simple_value.back() != '[')))
				{
					if (eqm_location >= p_lhssize)
					{
						if (strvalue == '+')
						{
							strvalue = '-';
						}
						else if (strvalue == '-')
						{
							strvalue = '+';
						}
					}
					if (!simple_value.empty())
					{
						boost::trim(simple_value);
						if (isNum(simple_value, p_constants,false))
						{
							if (p_constants.isconstant(simple_value) && p_constants.length(simple_value) > 1)
								{
								_exhandler->raise(Exception::FMTexc::FMTundefined_constant,
									"Periodic based not supported "+ simple_value + " at line " + std::to_string(m_line),
									"FMToptimizationparser::getEquation", __LINE__, __FILE__, m_section);
								}
							simple_value = std::to_string(getNum<double>(simple_value, p_constants));
						}else if (isYld(p_yields, simple_value, Core::FMTsection::Optimize,false) &&
							std::find_if(p_outputs.begin(), p_outputs.end(), Core::FMToutputcomparator(simple_value))== p_outputs.end())
						{
							specialCases[simplificaiton.at(simplificaiton.size() - 2)] =
								" " + simplificaiton.at(simplificaiton.size() - 1) + " " + simple_value;
							simple_value = "1.0";
						}
						if (!simple_value.empty())
						{
							simplificaiton.push_back(simple_value);
						}
						simple_value.clear();
					}
					else if (!simplificaiton.empty())
					{
						simplificaiton.pop_back();
					}
					std::string pushedval = std::string(1, strvalue);
					boost::trim(pushedval);
					if (!pushedval.empty())
					{
						simplificaiton.push_back(pushedval);
					}

				}
				else {
					simple_value += strvalue;
				}
				++eqm_location;
				if (isspace(strvalue))
				{
					lastcharspace = true;
				}
				else {
					lastcharspace = false;
				}

			}
			boost::trim(simple_value);
			if (isNum(simple_value, p_constants, false))
			{
				if (p_constants.isconstant(simple_value) && p_constants.length(simple_value) > 1)
				{
					_exhandler->raise(Exception::FMTexc::FMTundefined_constant,
						"Periodic based not supported " + simple_value + " at line " + std::to_string(m_line),
						"FMToptimizationparser::getEquation", __LINE__, __FILE__, m_section);
				}
				simple_value = std::to_string(getNum<double>(simple_value, p_constants));
			}else if(isYld(p_yields, simple_value,Core::FMTsection::Optimize,false)&&
				std::find_if(p_outputs.begin(), p_outputs.end(), Core::FMToutputcomparator(simple_value)) == p_outputs.end())
				{
				specialCases[simplificaiton.at(simplificaiton.size() - 2)] =
					" "+simplificaiton.at(simplificaiton.size() - 1) + " " + simple_value;
				simple_value = "1.0";
				}
			if (!simple_value.empty())
			{
				simplificaiton.push_back(simple_value);
			}
			const Core::FMTexpression simplification = Core::FMTexpression(simplificaiton).simplify(nodes);
			for (const auto& specialCase : specialCases)
				{
				nodes[specialCase.first + specialCase.second] = nodes[specialCase.first];
				nodes.erase(specialCase.first);
				}
		
		}catch (...)
			{
			_exhandler->raisefromcatch( "at line " + pm_line,
				"FMToptimizationparser::getEquation", __LINE__, __FILE__,m_section);
			}
        return nodes;
        }

	std::vector<std::vector<double>>FMToptimizationparser::getreplicatechanges(const std::string& replicateargument)
		{
		std::vector<std::vector<double>>periodicvalues;
		//_REPPLICATES(filename.txt) keyword?
		try {
			const boost::filesystem::path FILEm_location = boost::filesystem::path(m_location).parent_path() / replicateargument;
			if (boost::filesystem::is_regular_file(FILEm_location))
				{
				for (const std::vector<std::string>& line : readCsv(FILEm_location.string(), ';'))
					{
					std::vector<double>iterationvalues;
					for (const std::string& itvalue : line)
						{
						iterationvalues.push_back(std::stod(itvalue));
						}
					periodicvalues.push_back(iterationvalues);
					}
			}else {
				_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
					FILEm_location.string() + " at line " + std::to_string(m_line)+" in "+m_location,
					"FMToptimizationparser::getreplicatechanges", __LINE__, __FILE__, m_section);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToptimizationparser::getreplicatechanges", __LINE__, __FILE__, m_section);
			}
		return periodicvalues;
		}


    Core::FMToutput FMToptimizationparser::resume_output(const std::map<std::string,double>& nodes,
                                                   const std::vector<Core::FMToutput>& outputs,
                                                   const std::vector<Core::FMTtheme>& themes,
                                                   const Core::FMTconstants& constants)
        {
		Core::FMToutput final_output;
		try {
			for (std::map<std::string, double>::const_iterator output_it = nodes.begin(); output_it != nodes.end(); output_it++)
			{
				boost::smatch out_match;
				std::string target(output_it->first);
				const size_t OPERATOR = target.find_first_of("*");
				Core::FMToutputsource OtherFactor;
				Core::FMToperator OtherOperator;
				if (OPERATOR!=std::string::npos)//Got the case of yield or temp const multi...
					{
					std::string yield_factor = target.substr(OPERATOR + 1, target.size());
					boost::trim(yield_factor);
					std::string opra;
					opra += target.at(OPERATOR);
					target = target.substr(0, OPERATOR);
					OtherOperator = Core::FMToperator(opra);
					OtherFactor = Core::FMToutputsource(Core::FMTotar::timeyld, 0.0, yield_factor);
					}

				if (boost::regex_search(target, out_match, m_rxoutput))
				{
					std::string output_name = std::string(out_match[1]) + std::string(out_match[8]) + std::string(out_match[12]) + std::string(out_match[16]);
					boost::erase_all(output_name, " ");
					boost::smatch special_match;
					std::string specialtype;
					//int minbound = 1;
					//int maxbound = std::numeric_limits<int>::max();
					std::string target_attribute;
					Core::FMTperbounds bounding;
					if (boost::regex_search(target, special_match, m_rxspecialoutput))
					{

						specialtype = special_match[1];
						const std::string lowerperiod = std::string(special_match[7]) + std::string(special_match[12]);
						const std::string upperperiod = std::string(special_match[11]) + std::string(special_match[12]);
						if (!lowerperiod.empty() || !upperperiod.empty())
						{		
							bounding = getPerBound(lowerperiod, upperperiod, constants);
						}
						output_name = std::string(special_match[4]) + std::string(special_match[13]);

						//boost::smatch outname_match;
						//out_match = boost::smatch();
						if (!boost::regex_search(output_name, out_match, m_rxoutput))
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_constraint,
								output_name + " at line " + std::to_string(m_line),
								"FMToptimizationparser::resume_output", __LINE__, __FILE__, m_section);
						}
						else {
							const std::string BACKUP(output_name);
							output_name = std::string(out_match[1]) +std::string(out_match[8]) + std::string(out_match[12]) + std::string(out_match[16]);
							boost::regex_search(BACKUP, out_match, m_rxoutput);
							target_attribute = std::string(out_match[10]) + std::string(out_match[3]);
							boost::trim(target_attribute);
						}
						boost::erase_all(output_name, " ");
					}
					std::vector<Core::FMToutput>::const_iterator target_out = find_if(outputs.begin(), outputs.end(), Core::FMToutputcomparator(output_name));
					if (target_out == outputs.end())
					{
						_exhandler->raise(Exception::FMTexc::FMTundefined_output,
							 output_name + " at line " + std::to_string(m_line),
							"FMToptimizationparser::resume_output", __LINE__, __FILE__, m_section);
					}
					if (target_attribute.empty())
						{
						target_attribute = std::string(out_match[10]) + std::string(out_match[3]);
						}
					boost::trim(target_attribute);

					const Core::FMTtheme targeted_theme = target_out->targettheme(themes);
					if (!target_attribute.empty())
					{
						if (targeted_theme.empty() ||
							(!targeted_theme.empty() &&
							(!targeted_theme.isattribute(target_attribute) &&
								!targeted_theme.isaggregate(target_attribute))))
						{
							_exhandler->raise(Exception::FMTexc::FMTundefinedoutput_attribute,
								target_attribute + " at line " + std::to_string(m_line),
								"FMToptimizationparser::resume_output", __LINE__, __FILE__, m_section);
							target_attribute.clear();
						}
					}
					const std::string target_period = std::string(out_match[6]) + std::string(out_match[14]);
					int inttarget_period = -1;
					
					if (!target_period.empty())
					{
						inttarget_period = getNum<int>(target_period, constants);
						bounding = Core::FMTperbounds(Core::FMTsection::Optimize, inttarget_period, inttarget_period);
					}
					//copy the output and the specify the attribute and the periods!!!
					
					Core::FMToutput newoutput = target_out->boundto(themes, bounding, specialtype, target_attribute);
					
					if (OPERATOR != std::string::npos)
						{
						if (OtherOperator.isdivide())
							{
							newoutput /= OtherFactor;
						}else {
							newoutput *= OtherFactor;
							}

					}else{
						newoutput *= Core::FMToutputsource(Core::FMTotar::val, output_it->second);
					}
					final_output += newoutput;
					
					
				}
				else {
					_exhandler->raise(Exception::FMTexc::FMTinvalid_constraint,
						output_it->first + " at line " + std::to_string(m_line),
						"FMToptimizationparser::resume_output", __LINE__, __FILE__, m_section);
				}
			}
			
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMToptimizationparser::resume_output", __LINE__, __FILE__,m_section);
			}
        return final_output;
        }

	Core::FMTconstraint FMToptimizationparser::getSpatialConstraint(const Core::FMTconstraint& p_baseconstraint,
		const boost::smatch& p_match,const std::string& pm_line,
		const Core::FMTconstants& p_constants, const Core::FMTyields& p_yields,
		const std::vector<Core::FMTaction>& p_actions,const std::vector<Core::FMToutput>& p_outputs, const std::vector<Core::FMTtheme>& p_themes)
	{
		Core::FMTconstraint constraint;
		try {
			const std::string constrainttypestr(p_match[1]);
			const std::string senseofconstraint(p_match[7]);
			const std::string rhsstring(p_match[9]);
			const std::string periodstring(p_match[10]);
			Core::FMTconstrainttype constrainttype;
			double lowergreenup = 0;
			double uppergreenup = std::numeric_limits<double>::max();
			double lowerneighborsize = 0;
			double upperneighborsize =std::numeric_limits<double>::max();
			std::vector<std::string>splitted;
			const std::string inargument(p_match[4]);
			double ThemeId = -1;
			std::string actionoraggregates;
			if (constrainttypestr == "_RANDOM")
			{
				constraint = p_baseconstraint;
				std::map<std::string, double> nodes = getEquation(inargument, p_constants, p_yields, p_outputs, inargument.size());
				nodes.erase("RHS");
				const Core::FMToutput targetout = resume_output(nodes, p_outputs, p_themes, p_constants);
				constrainttype = Core::FMTconstrainttype::FMTrandomaction;
				constraint.setconstrainttype(constrainttype);
				constraint.setoutput(targetout);
				
			}
			else {
				boost::split(splitted, inargument, boost::is_any_of(","));
				actionoraggregates=boost::trim_copy(splitted.at(0));
				isAct(m_section, p_actions, actionoraggregates);
				const std::string naming = constrainttypestr + "(" + inargument + ")";
				if (constrainttypestr == "_SIZE")
				{
					constrainttype = Core::FMTconstrainttype::FMTspatialsize;
					lowerneighborsize = getNum<double>(boost::trim_copy(splitted.at(1)), p_constants);
				}
				else if (constrainttypestr == "_ADJACENCY")
				{
					lowergreenup = getNum<double>(boost::trim_copy(splitted.at(1)), p_constants);
					constrainttype = Core::FMTconstrainttype::FMTspatialadjacency;
				}
				else if (constrainttypestr == "_GROUP")
				{
					boost::smatch ThMatch;
					if (boost::regex_search(boost::trim_copy(splitted.at(1)), ThMatch,
						boost::regex("(_TH)([\\d]*)")))
					{
						ThemeId = getNum<double>(ThMatch[2], p_constants);
						lowergreenup = getNum<double>(
							boost::trim_copy(splitted.at(2)), p_constants);
					}else {
						lowergreenup = getNum<double>(
							boost::trim_copy(splitted.at(1)), p_constants);
						}
					constrainttype = Core::FMTconstrainttype::FMTSpatialGroup;
				}
				constraint = Core::FMTconstraint(constrainttype, Core::FMToutput(naming));
				const std::string GUP_TARGET("GUP");
				if (Core::FMTconstrainttype::FMTspatialadjacency == constrainttype)
				{
					constraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, GUP_TARGET,
						uppergreenup, lowergreenup));
				}
				else if (Core::FMTconstrainttype::FMTspatialsize == constrainttype)
				{
					const std::string target("NSIZE");
					constraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, target, upperneighborsize, lowerneighborsize));
				}
				else if (Core::FMTconstrainttype::FMTSpatialGroup == constrainttype)
				{
					const std::string THEME_TARGET("THEME");
					constraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, GUP_TARGET,
						uppergreenup, lowergreenup));
					constraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, THEME_TARGET,
						ThemeId, ThemeId));
				}
			}
			double lower = 0;
			double upper = 0;
			const double rhs = getNum<double>(rhsstring, p_constants);
			fillbounds(senseofconstraint, rhs,lower,upper);
			const std::string target("RHS");
			
			constraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, target,upper,lower));
			if (constrainttypestr != "_RANDOM")
			{
				for (const Core::FMTaction* actionptr : Core::FMTactioncomparator(actionoraggregates).getallaggregates(p_actions))
				{
					constraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, actionptr->getname(), rhs, rhs));
				}
			}
			const std::vector<Core::FMTconstraint> returnedconstraints = getperiodsbounds(periodstring, constraint, p_constants);
			constraint = *returnedconstraints.begin();
		}catch (...)
		{
			_exhandler->raisefromcatch("at line " + pm_line, "FMToptimizationparser::getSpatialConstraint", __LINE__, __FILE__, m_section);
		}
		return constraint;
	}


	std::vector<Core::FMTconstraint> FMToptimizationparser::getConstraints(const std::string& pm_line,
												const Core::FMTconstants& p_constants, const Core::FMTyields& p_yields,
												const std::vector<Core::FMToutput>& p_outputs, const std::vector<Core::FMTtheme>& p_themes,
												const std::vector<Core::FMTaction>& p_actions)
		{
		Core::FMTconstraint constraint;
		std::vector<Core::FMTconstraint>returnedconstraints;
		boost::match_results<std::string::const_iterator> Bmatch;
		

		//boost::match_flag_type flags = boost::match_default;
		try {
			boost::smatch kmatch;
			std::string rest = pm_line;
			setending(constraint, rest, p_constants);
			if (boost::regex_search(rest, kmatch, m_rxspatial))
			{
				returnedconstraints.push_back(getSpatialConstraint(constraint,kmatch, pm_line, p_constants,p_yields, p_actions, p_outputs, p_themes));
			}else if (boost::regex_search(rest, kmatch, m_rxconstraints))
				{
				//std::string target = std::string(kmatch[6]) + std::string(kmatch[12]) + std::string(kmatch[15]);
				std::string target = std::string(kmatch[6]) + std::string(kmatch[15]) + std::string(kmatch[19]);
				boost::trim(target);
				if (target.find("(") != std::string::npos && target.find(")") == std::string::npos)
				{
					target += ")";
				}

				std::map<std::string, double> nodes = getEquation(target, p_constants, p_yields, p_outputs, target.size());
				nodes.erase("RHS");
				const Core::FMToutput targetout = resume_output(nodes, p_outputs, p_themes, p_constants);
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
				//std::string lower_variation = std::string(kmatch[14]) + std::string(kmatch[8]);
				std::string lower_variation = std::string(kmatch[18]) + std::string(kmatch[9]);
				double lower_var = 0;
				double higher_var = 0;
				if (!lower_variation.empty())
				{
					if (lower_variation.find('%') != std::string::npos)
					{
						lower_variation.erase(std::remove(lower_variation.begin(), lower_variation.end(), '%'), lower_variation.end());
					}
					lower_var = getNum<double>(lower_variation, p_constants);
				}
				//std::string high_variation = std::string(kmatch[10]);
				std::string high_variation = std::string(kmatch[13]);
				if (!high_variation.empty())
				{
					if (high_variation.find('%') != std::string::npos)
					{
						high_variation.erase(std::remove(high_variation.begin(), high_variation.end(), '%'), high_variation.end());
					}
					higher_var = getNum<double>(high_variation, p_constants);
				}
				const std::string yld_name = "Variation";
				constraint.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize, yld_name, higher_var, lower_var));
				//const std::string periodstring = std::string(kmatch[18]);
				const std::string periodstring = std::string(kmatch[23]);
				
				returnedconstraints = getperiodsbounds(periodstring, constraint, p_constants);

			}
			else if (boost::regex_search(rest.cbegin(), rest.cend(), Bmatch, m_rxequations))
			{
				Core::FMTconstrainttype cctype = Core::FMTconstrainttype::FMTstandard;
				const std::string periodstring = std::string(Bmatch[13]);
				const std::string str_operator = std::string(Bmatch[5]) + std::string(Bmatch[6]) + std::string(Bmatch[11]);
				std::string LHS = std::string(Bmatch[1]);
				std::string beempty(Bmatch[9]);
				boost::trim(beempty);
				if (!beempty.empty())
					{
					LHS += beempty;
					}
				const std::string RHS = std::string(Bmatch[7]) + std::string(Bmatch[12]);
				std::string rhsoperator;
				if (!boost::regex_match(RHS, m_rxstartwithoperator))
					{
					rhsoperator = "+";
					}
				const std::string full_equation = std::string(1, ' ') + LHS + std::string(1, ' ') + rhsoperator + RHS;
				std::map<std::string, double> nodes = getEquation(full_equation, p_constants, p_yields, p_outputs, LHS.size()+2);
				double bound = nodes["RHS"];
				nodes.erase("RHS");
				double lower = 0;
				double upper = 0;
				fillbounds(str_operator, bound, lower, upper);
				const Core::FMToutput final_output = resume_output(nodes, p_outputs, p_themes, p_constants);

				for (Core::FMTconstraint baseconstraint : getperiodsbounds(periodstring, constraint, p_constants))
				{
					baseconstraint.setoutput(final_output);
					baseconstraint.setconstrainttype(cctype);
					baseconstraint.setrhs(lower, upper);
					returnedconstraints.push_back(baseconstraint);
					
				}
			}else{
				_exhandler->raise(Exception::FMTexc::FMTinvalid_constraint,
					pm_line+"  at line " + std::to_string(m_line),
					"FMToptimizationparser::getConstraints", __LINE__, __FILE__, m_section);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("at line "+ pm_line,"FMToptimizationparser::getConstraints", __LINE__, __FILE__, m_section);
			}
		return returnedconstraints;
		}

		void FMToptimizationparser::fillbounds(const std::string& operatorvalue, const double& rhs, double& lower, double& upper) const
			{
			try {
				lower = 0;
				upper = 0;
				if (operatorvalue == "=")
				{
					lower = rhs;
					upper = rhs;
				}
				else if (operatorvalue == ">=")
				{
					lower = rhs;
					upper = std::numeric_limits<double>::infinity();
				}
				else if (operatorvalue == "<=")
				{
					lower = std::numeric_limits<double>::lowest();
					upper = rhs;
				}
			}catch (...)
				{
				_exhandler->raisefromcatch("","FMToptimizationparser::fillbounds", __LINE__, __FILE__, m_section);
				}
			}

		std::vector<Core::FMTconstraint> FMToptimizationparser::getperiodsbounds(std::string periodstr, const Core::FMTconstraint& constraint, const Core::FMTconstants& constants) const
		{
			std::vector<Core::FMTconstraint>constraints;
			try {
				std::vector<std::string>splitted;
				boost::split(splitted,periodstr, boost::is_any_of(","));
				for (const std::string& value : splitted)
					{
					Core::FMTconstraint newconstraint(constraint);
					if (!setPeriods(newconstraint, value, constants))
						{
						_exhandler->raise(Exception::FMTexc::FMTemptybound,
							" for " + value,
							"FMToptimizationparser::getperiodsbounds", __LINE__, __FILE__, m_section);
						}
					constraints.push_back(newconstraint);
					}
	
			}catch (...)
				{
				_exhandler->raisefromcatch("for " + periodstr, "FMToptimizationparser::getperiodsbounds", __LINE__, __FILE__, m_section);
				}
			return constraints;
		}


	Core::FMTconstraint FMToptimizationparser::getObjective(const std::string& pm_line,
		const Core::FMTconstants& p_constants,
		const Core::FMTyields& p_yields,
		const std::vector<Core::FMToutput>& p_outputs,
		const std::vector<Core::FMTtheme>& p_themes)
		{
		Core::FMTconstraint objective;
		try {
			boost::smatch specialmatch;
			std::string subline(pm_line);
			if (boost::regex_search(pm_line, specialmatch, m_rxspecialobjective))
				{
				subline=std::string(specialmatch[1]);
				boost::trim(subline);
				const std::string keyword(specialmatch[2]);
				if (keyword=="_SETGLOBALSCHEDULE")
					{
					const double scheduleweight = getNum<double>(std::string(specialmatch[4]), p_constants);
					objective.addbounds(Core::FMTyldbounds(Core::FMTsection::Optimize,keyword,scheduleweight, scheduleweight));
					}

				}

			boost::smatch kmatch;
			if (boost::regex_search(subline, kmatch, m_rxobjectives))
			{
				const std::string objective_type = kmatch[1];
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
				const std::string lower_period = std::string(kmatch[7]) + std::string(kmatch[10]);
				const std::string upper_period = std::string(kmatch[9]);
				std::string body = std::string(kmatch[3]);
				if (body.at(0) == '(')
				{
					body.erase(0, 1);
					body.erase(body.size() - 1);
				}
				std::string main_equation = body;
				std::string penalty_equation;
				std::string penalty_operator;
				if (body.find("_PENALTY") != std::string::npos)
				{
					main_equation = body.substr(0, body.find("_PENALTY"));
					boost::trim(main_equation);
					if (!main_equation.empty())
						{
						penalty_operator = main_equation.back();
						}
					penalty_equation = (body.substr(body.find("_PENALTY")));
					boost::trim(penalty_equation);
					if (!main_equation.empty())
					{
						main_equation.pop_back();
					}
				}
				boost::trim(main_equation);
				boost::smatch kpenalty;
				if (boost::regex_search(penalty_equation, kpenalty, m_rxpenalty))
				{
					const std::string penalty_values = kpenalty[3];
					std::vector<std::string>allpenalties;
					if (penalty_values == "_ALL")
					{
						allpenalties.push_back(penalty_values);
					}else {
						boost::split(allpenalties, penalty_values, boost::is_any_of(","));
					}
					objective.setpenalties(penalty_operator, allpenalties);
				}
				Core::FMToutput final_output;
				if (!main_equation.empty())
				{
					std::map<std::string, double>nodes = getEquation(main_equation, p_constants,p_yields, p_outputs);
					nodes.erase("RHS");
					final_output = resume_output(nodes, p_outputs, p_themes, p_constants);
				}
				objective.setoutput(final_output);
				
				setPeriodWithBounds(objective, lower_period, upper_period, p_constants);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("at line "+ pm_line,"FMToptimizationparser::getObjective", __LINE__, __FILE__,m_section);
			}
		return objective;
		}

	FMToptimizationsection FMToptimizationparser::getsection(const std::string& line) const
		{
		FMToptimizationsection section = FMToptimizationsection::none;
		try {
			boost::smatch kmatch;
			if (boost::regex_search(line, kmatch, m_rxsections))
			{
				const std::string target = std::string(kmatch[2]);
				if (target == "OBJECTIVE")
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
		}catch (...)
			{
			_exhandler->raisefromcatch("at line " + line,"FMToptimizationparser::getsection", __LINE__, __FILE__, m_section);
			}
		return section;
		}

	std::vector<Core::FMTconstraint> FMToptimizationparser::read(const std::vector<Core::FMTtheme>& p_themes,
									const std::vector<Core::FMTaction>& p_actions,
									const Core::FMTconstants& p_constants,
									const std::vector<Core::FMToutput>& p_outputs,
									const Core::FMTyields& p_yields,
									std::vector<Core::FMTaction>& p_excluded,
									const std::string& pm_location)
		{
		std::vector<Core::FMTconstraint>constraints;
		try {
			if (!pm_location.empty())
			{
				std::vector<boost::icl::interval_set<int>>ActionPeriods;
				const boost::icl::discrete_interval<int> IntervalReference = boost::icl::discrete_interval<int>::closed(-1, std::numeric_limits<int>::max());
				std::ifstream optimizestream(pm_location);
				if (FMTparser::tryOpening(optimizestream, pm_location))
				{
					FMToptimizationsection section = FMToptimizationsection::none;
					std::queue<FMTparser::FMTLineInfo>Lines  = getoptline(optimizestream, p_themes, p_constants, p_outputs);
					while (!Lines.empty())
					{
						std::string line = GetLine(Lines);
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
							}
							else {
								switch (section)
								{
								case FMToptimizationsection::objective:
								{
									Core::FMTconstraint objective = getObjective(line, p_constants, p_yields, p_outputs, p_themes);
									if (objective.emptyperiod())
									{
										_exhandler->raise(Exception::FMTexc::FMTmissingobjective,
											" at line " + std::to_string(m_line),
											"FMToptimizationparser::read", __LINE__, __FILE__, m_section);
									}
									constraints.push_back(objective);
									break;
								}
								case FMToptimizationsection::constraints:
								{
									for (Core::FMTconstraint baseconstraint : getConstraints(line, p_constants,p_yields, p_outputs, p_themes,p_actions))
										{
										//baseconstraint.passinobject(*this);
										constraints.push_back(baseconstraint);
										}
									
									break;
								}
								case FMToptimizationsection::exclude:
								{
									if (line.find("*EXCLUDE") == std::string::npos)
									{
										line = "*EXCLUDE " + line;
									}
									boost::smatch kmatch;
									if (boost::regex_search(line, kmatch, m_rxexclude))
									{

										const std::string action_name = kmatch[3];
										//const int period_lower = getNum<int>(std::string(kmatch[7]) + std::string(kmatch[10]), constants);
										const std::string periodstr = std::string(kmatch[5]);
										std::vector<Core::FMTconstraint> BoundedConstraints = getperiodsbounds(periodstr, Core::FMTconstraint(), p_constants);
										const std::vector<const Core::FMTaction*>action_ptrs = Core::FMTactioncomparator(action_name).getallaggregates(p_excluded);
										if (!action_ptrs.empty())
											{
											if (ActionPeriods.empty())
												{
												boost::icl::interval_set<int>BaseRange;
												BaseRange += IntervalReference;
												ActionPeriods = std::vector<boost::icl::interval_set<int>>(p_excluded.size(), BaseRange);
												}
											for (const Core::FMTaction* actptr : action_ptrs)
												{
												const size_t ActId = actptr - &(*p_excluded.cbegin());
												boost::icl::interval_set<int>& TheSet = ActionPeriods[ActId];
												if (!TheSet.empty())
												{
													for (const Core::FMTspec& Bound : BoundedConstraints)
													{
														const int Upper = Bound.getperiodupperbound();
														const int Lower = Bound.getperiodlowerbound();
														if (boost::icl::discrete_interval<int>::closed(Lower, Upper) == IntervalReference)
														{
															TheSet.clear();
														}else
														{
															TheSet -= boost::icl::discrete_interval<int>::closed(Lower, Upper);
														}
														if (Lower <= -1)
														{
															_exhandler->raise(Exception::FMTexc::FMTrangeerror,
																"excluded with invalid lower bound at line " + std::to_string(m_line),
																"FMToptimizationparser::read", __LINE__, __FILE__, m_section);
														}

													}
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
				if (!ActionPeriods.empty())
				{
					size_t ActionId = 0;
					for (boost::icl::interval_set<int>& ActionIntervals : ActionPeriods)
						{
						if (ActionIntervals.empty()||(!ActionIntervals.empty() && ((*ActionIntervals.begin()) != IntervalReference)))
						{
							Core::FMTaction& NewAction = p_excluded.at(ActionId);
							
							while (!NewAction.empty())
							{
								NewAction.erase(0);
							}
							if (ActionIntervals.empty())
							{
								ActionIntervals += boost::icl::discrete_interval<int>::closed(-1, 1);
							}
							for (const boost::icl::discrete_interval<int>& OperableInterval : ActionIntervals)
							{
								for (const auto& ActionData : p_actions.at(ActionId))
								{
									Core::FMTspec theSpec = ActionData.second;
									bool intersectWithAction = true;
									boost::icl::discrete_interval<int> TheInterval(OperableInterval);
									if (!theSpec.emptyperiod())
										{
										const int Upper = theSpec.getperiodupperbound();
										const int Lower = theSpec.getperiodlowerbound();
										intersectWithAction = boost::icl::intersects(TheInterval, boost::icl::discrete_interval<int>::closed(Lower, Upper));
										if (intersectWithAction)
										{
											TheInterval = TheInterval & boost::icl::discrete_interval<int>::closed(Lower, Upper);
										}
									}
									int Upper = 0;
									int Lower = 0;
									if (intersectWithAction)
									{
										Upper = TheInterval.upper();
										Lower = TheInterval.lower();
										if (TheInterval == boost::icl::discrete_interval<int>::right_open(TheInterval.lower(), TheInterval.upper()))
										{
											--Upper;
										}
										if (TheInterval == boost::icl::discrete_interval<int>::left_open(TheInterval.lower(), TheInterval.upper()))
										{
											++Lower;
										}
									}
									theSpec.setbounds(Core::FMTperbounds(Core::FMTsection::Action, Upper, Lower));
									NewAction.push_back(ActionData.first, theSpec);	
								}
							}
							NewAction.update();
						}
						++ActionId;
						}
				}
			}
		constraints.shrink_to_fit();
		}catch (...)
			{
			_exhandler->raisefromcatch("In " + m_location + " at line " + std::to_string(m_line),"FMToptimizationparser::read", __LINE__, __FILE__, m_section);
			}
		return constraints;
		}

	void FMToptimizationparser::write(const std::vector<Core::FMTconstraint>& constraints,const std::string& location) const
		{
		try {
			std::ofstream optimizestream;
			optimizestream.open(location);
			if (tryOpening(optimizestream, location))
			{
				size_t objid = 0;
				if (constraints.at(0).isobjective())
				{
					optimizestream << "*OBJECTIVE " << "\n";
					optimizestream << std::string(constraints.at(0));
					++objid;
				}
				if (constraints.size() - objid > 0)
				{
					optimizestream << "*CONSTRAINTS " << "\n";
					for (size_t cid = objid; cid < constraints.size(); ++cid)
					{
						optimizestream << std::string(constraints.at(cid));
					}
				}
				optimizestream.close();
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("at " + location,"FMToptimizationparser::write", __LINE__, __FILE__, m_section);
			}
		}

	std::queue<std::string> FMToptimizationparser::geteachlines(const std::string& line, const Core::FMTconstants& constants,const std::vector<Core::FMToutput>& outputs, const std::vector<Core::FMTtheme>& themes) const
		{
		std::queue<std::string>valuestoreplace;
		try {
			int themeid = -1;
			std::vector<Core::FMToutput>::const_iterator target_out;
			std::string subline = line;
			const std::string keyword = "_EACH";
			while (subline.find(keyword) != std::string::npos)
			{
				boost::erase_all(subline, " ");
				int endoutputlocation = static_cast<int>((subline.find(keyword))) - 1;
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
					&& outchar != '(' && endoutputlocation >= 0)
				{
					output_name.insert(output_name.begin(), outchar);
					--endoutputlocation;
					if (endoutputlocation >= 0)
					{
						outchar = subline.at(endoutputlocation);
					}
				}
				if (output_name.find("#") != std::string::npos)//constant
				{

				}
				else {
					target_out = find_if(outputs.begin(), outputs.end(), Core::FMToutputcomparator(output_name));
					if (target_out == outputs.end())
					{
						_exhandler->raise(Exception::FMTexc::FMTundefined_output,
							output_name + " at line " + std::to_string(m_line),
							"FMToptimizationparser::geteachlines",__LINE__, __FILE__, m_section);
					}
					if (themeid >= 0)
					{
						if (themeid != target_out->targetthemeid())
						{
							_exhandler->raise(Exception::FMTexc::FMTinvalid_constraint,
								"Non matching _EACH outputs attributes at line " + std::to_string(m_line),
								"FMToptimizationparser::geteachlines",__LINE__, __FILE__, m_section);
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
		}catch (...)
			{
			_exhandler->raisefromcatch("at line " + line,"FMToptimizationparser::geteachlines", __LINE__, __FILE__, m_section);
			}
		return valuestoreplace;
		}

	std::queue<FMTparser::FMTLineInfo> FMToptimizationparser::getoptline(std::ifstream& stream, const std::vector<Core::FMTtheme>& themes,
		const Core::FMTconstants& cons, const std::vector<Core::FMToutput>& outputs)
		{
		std::queue<FMTLineInfo>FinalLines;
		try {
			std::queue<FMTparser::FMTLineInfo>Lines = FMTparser::GetCleanLinewfor(stream, themes, cons);
			while (!Lines.empty())
				{
				const std::string line = GetLine(Lines);
				if (line.find("_EACH") != std::string::npos)
					{
					std::queue<std::string> EACH_LINES = geteachlines(line, cons, outputs, themes);
					while (!EACH_LINES.empty())
						{
						FinalLines.push(FMTLineInfo(EACH_LINES.front(),m_line,m_location));
						EACH_LINES.pop();
						}
				}else {
					FinalLines.push(FMTLineInfo(line,m_line, m_location));
					}
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMToptimizationparser::getoptline", __LINE__, __FILE__, m_section);
			}
		return FinalLines;
		}
}

