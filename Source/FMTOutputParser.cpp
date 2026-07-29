/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOutputParser.h"
#include "FMTTheme.h"
#include "FMTAction.h"
#include "FMTYields.h"
#include "FMTConstants.h"
#include "FMTOutput.h"
#include "FMTExceptionHandler.h"

namespace Parser
{
	const boost::regex FMTOutputParser::m_rxoutput=boost::regex("(\\*OUTPUT|\\*LEVEL)(([\\s\\t]*)([^\\s\\t\\(]*)([\\s\\t]*)(\\()([^\\s\\t\\)]*)(\\))([\\s\\t]*)(.+))|((\\*OUTPUT|\\*LEVEL)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(.+))", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTOutputParser::m_rxsource = boost::regex("(\\*SOURCE)([\\s\\t]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTOutputParser::m_rxtar = boost::regex("(([\\s\\t]*)(_INVENT)([\\s\\t]*)(\\()([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(\\))([\\s\\t]*)((_AREA)|([^\\s\\t]*)))|(([\\s\\t]*)((_INVENT)|(_INVLOCK))([\\s\\t]*)((_AREA)|([^\\s\\t]*)))|(([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)((_AREA)|([^\\s\\t]*)))", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTOutputParser::m_rxgrp = boost::regex("(\\*GROUP)([\\s\\t]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTOutputParser::m_rxoutputconstant = boost::regex("([^\\[]*)(\\[[\\s\\t]*)(\\-?[0-9])([\\s\\t]*\\])", boost::regex_constants::ECMAScript | boost::regex_constants::icase);


       FMTOutputParser::FMTOutputParser():FMTParser()
            {
		    _setSection(Core::FMTsection::Outputs);
            }

	   void FMTOutputParser::_appendToOutput(
		   const std::string& strvalue,
		   const int& outputid,
		   const int& themetarget,
		   const size_t& lastoutput,
		   std::string& lastoperator,
		   std::vector<std::string>& stroperators,
		   std::vector<Core::FMTOutputSource>& sources,
		   std::vector<Core::FMTOperator>& operators) const
	   {
		   try {
			   double value = 0;
			   Core::FMTotar targetof = Core::FMTotar::timeyld;
			   bool isNumber = false;
			   std::string yldtarget = strvalue;
			   if (_isNum(strvalue))
			   {
				   value = std::stod(strvalue);
				   isNumber = true;
				   targetof = Core::FMTotar::val;
				   Core::FMTOperator lastopof(lastoperator);
				   if (lastopof.isFactor())
						{
					    for (size_t factorId = 0; factorId < operators.size() - 1; ++factorId)
							{
							if (operators.at(factorId).isDivide())
								{
								if (lastopof.isDivide())
								{
									value = 1.0 * value;
								}else {
									value = 1 / value;
								}
								break;
								}
	
							}
						}

				   
				   yldtarget.clear();
			   }
			   if (!lastoperator.empty())
			   {
				   std::vector<Core::FMTOutputSource>newsources;
				   std::vector<Core::FMTOperator>newoperators;
				   size_t lastop = 0;
				   size_t id = 0;
				   for (; id < lastoutput; ++id)
				   {
					   newsources.push_back(sources.at(id));
					   if (id > 0)
					   {
						   newoperators.push_back(operators.at(lastop));
						   ++lastop;
					  }
				   }
				   for (; id < sources.size(); ++id)
				   {
					   //if (isNumber)
					   //{
						   double srcvalue = value;
						   if (id > 0 && (sources.at(id - 1).isVariable() || (sources.at(id  - 1).isLevel() && !sources.at(id  - 1).isVariableLevel())) /* && operators.at(lastop).isFactor()*/)
						   {
							   /*if (!operators.at(lastop).isFactor()) // If it comes from same output dont multiply...
							   {
								   std::cout << "Test "<<id << "\n";
							   }*/
							   //if (isNumber)
							   //{
								   if (sources.at(id).isConstant()&&isNumber)
								   {
									   srcvalue = Core::FMTOperator(operators.at(lastop)).call(srcvalue, sources.at(id).getValue());
								   }
								   else{
									   newoperators.push_back(Core::FMTOperator(lastoperator));
								   }
								newsources.push_back(Core::FMTOutputSource(targetof, srcvalue, yldtarget, "", sources.at(id).getOutputOrigin(), sources.at(id).getThemeTarget()));
							   //}
							   
						   }
						   
					   //}
					   if (sources.at(id).isVariable()  || sources.at(id).isLevel() || sources.at(id).isTimeYield() || !isNumber)
					   {
						   newsources.push_back(sources.at(id));
					   }
					   if (id > 0)
					   {
						   newoperators.push_back(operators.at(lastop));
						   ++lastop;
					   }
				   }
				   if (newsources.back().isVariable() || newsources.back().isLevel() /* || !isNumber*/)
				   {
					   newsources.push_back(Core::FMTOutputSource(targetof, value, yldtarget, "", newsources.back().getOutputOrigin(), newsources.back().getThemeTarget()));

					   newoperators.push_back(Core::FMTOperator(lastoperator));
				   }

				   operators = newoperators;
				   sources = newsources;
				   lastoperator.clear();
				   if (!stroperators.empty())
				   {
					   operators.push_back(Core::FMTOperator(stroperators.front()));
					   lastoperator = stroperators.front();
					   stroperators.erase(stroperators.begin());
				   }
			   }
			   else {
				   sources.push_back(Core::FMTOutputSource(targetof, value, yldtarget, "", outputid, themetarget));

			   }
		   }catch (...)
		   {
			   _exhandler->raiseFromCatch("In " + m_location + " at line " + std::to_string(m_line), "FMTOutputParser::_appendToOutput", __LINE__, __FILE__, m_section);
		   }
	
	   }


		void FMTOutputParser::_readNFill(std::vector<Core::FMTOutput>* outputs, 
					const std::vector<Core::FMTTheme>& themes,
					const std::vector<Core::FMTAction>& actions,
					const Core::FMTYields& ylds,const Core::FMTConstants& constants,
					const std::string& location)
		{
			std::string name, description;
			try {
				if (!location.empty())
				{
					std::ifstream outputstream(location);
					std::vector<Core::FMTOutputSource>sources;
					std::vector<Core::FMTOperator>operators;
					bool insource = false;
					bool processing_level = false;
					int themetarget = -1;
					//size_t lastopt = 0;
					size_t lastoutput = 0;
					int lastsourcelineid =0;
					int outputid = 0;
					std::string lastgroup;
					if (!outputs->empty())
					{
						outputid = static_cast<int>(outputs->size());
					}
					if (FMTParser::tryOpening(outputstream, location))
					{
						std::queue<FMTParser::FMTLineInfo>Lines = FMTParser::_getCleanLinewfor(outputstream, themes, constants);
						while (!Lines.empty())
						{
							const std::string line = _getLine(Lines);
							if (!line.empty())
							{
								boost::smatch kmatch;
								const std::string outline = line + " ";
								if (boost::regex_search(outline, kmatch, m_rxoutput))
								{
									if (!sources.empty() || (processing_level && !insource))
									{
										if (processing_level && sources.empty())
										{
											sources.push_back(Core::FMTOutputSource(Core::FMTotar::level, 0, "", name,outputid,themetarget));
										}
										if (!processing_level)
										{
											if (operators.size()==sources.size())
											{
												_exhandler->raise(Exception::FMTexc::FMToutput_too_much_operator,
																	name +" at line "+std::to_string(lastsourcelineid) ,"FMTOutputParser::read", __LINE__, __FILE__, m_section);
												operators.pop_back();
											}else if(operators.size()>sources.size())
											{
												_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
																name +" at line "+std::to_string(lastsourcelineid) ,"FMTOutputParser::read", __LINE__, __FILE__, m_section);

											}else if (operators.size()<sources.size()-1)
											{
												_exhandler->raise(Exception::FMTexc::FMToutput_missing_operator,
																	name +" at line "+std::to_string(lastsourcelineid) ,"FMTOutputParser::read", __LINE__, __FILE__, m_section);
											}
										}
										outputs->push_back(Core::FMTOutput(name, description, lastgroup, sources, operators));
										
										/**_logger<<name<<"\n";
										int id = 0;
										for(const auto& s:sources)
										{
											const std::string operatorstrloop = (id<operators.size()) ? std::string(operators.at(id)) : " ";
											*_logger<<std::string(s)+" "+operatorstrloop<<"\n";
											++id;
										}
										*_logger<<"\n"<<std::string(outputs->back())<<"\n";*/
										//outputs->back().passinobject(*this);
										++outputid;
									}
									sources.clear();
									//lastopt = 0;
									lastoutput = 0;
									operators.clear();
									const std::string outtype = std::string(kmatch[1]) + std::string(kmatch[12]);
									if (outtype == "*LEVEL")
									{
										processing_level = true;
									}
									else {
										processing_level = false;
									}
									std::string thtarget = std::string(kmatch[7]);
									if (_isValid(thtarget))
									{
										thtarget.erase(thtarget.begin(), thtarget.begin() + 3);
										themetarget = _getNum<int>(thtarget) - 1;
									}else{
										themetarget=-1;
									}
									name = std::string(kmatch[4]) + std::string(kmatch[14]);
									lastsourcelineid = m_line;
									description = std::string(kmatch[10]) + std::string(kmatch[16]);
									boost::trim_right(description);
									insource = false;
								}
								if (boost::regex_search(line, kmatch, m_rxgrp))
								{
									std::string groupname(kmatch[3]);
									boost::trim(groupname);
									lastgroup = groupname;
									insource = false;
								}
								else if (boost::regex_search(line, kmatch, m_rxsource) || insource)
								{
									std::string rest;
									if (insource && line.find("*SOURCE") == std::string::npos)
									{
										rest = line;
									}
									else {
										rest = kmatch[3];
									}
									std::vector<std::string>strsources;
									std::vector<std::string>stroperators;
									const std::string stroprators("-*/+");
									std::string stacked_char;
									std::string opstr;
									//size_t letterid = 0;
									bool inparenthesis = false;
									bool inmask = true;
									bool lastonespace = true;
									bool gotgeneralspace = false;
									bool lookslikeoutput = false;
									size_t thcound = 0;
									for (const char& letter : rest)
									{
										if (inmask)
										{
											if ((letter == ' ' || letter == '\t'))
											{
												lastonespace = true;
											}
											else if (lastonespace)
											{
												if (thcound == 1 && stroprators.find(letter) != std::string::npos)
												{
													lookslikeoutput = true;
												}
												++thcound;
												lastonespace = false;
											}
											if (thcound >= themes.size())
											{
												inmask = false;
											}
										}
							
										if (stroprators.find(letter) != std::string::npos && (!inmask || lookslikeoutput) && !inparenthesis && gotgeneralspace) // && !inparenthesis 
										{
											
											stroperators.push_back(std::string(1, letter));
											
											
											if (!stacked_char.empty())
											{
												strsources.push_back(stacked_char);
											}
											stacked_char = "";
											opstr += letter;
										}
										else {
											stacked_char += letter;
										}
										if (letter == '(' || letter=='[')
										{
											inparenthesis = true;
										}
										else if (letter == ')' || letter == ']')
										{
											inparenthesis = false;
										}
										if ((letter == ' ' || letter == '\t'))
										{
											gotgeneralspace = true;
										}
										else {
											gotgeneralspace = false;
										}

									}
									if (!stacked_char.empty())
									{
										if (!stroperators.empty() && _isNum(stacked_char))
										{
											if (stroperators.back()=="-")
											{
												stacked_char = "-"+ stacked_char;
												stroperators.pop_back();
											}

										}
										strsources.push_back(stacked_char);
									}
									replace(opstr.begin(), opstr.end(), '.', 'r');
									replace(opstr.begin(), opstr.end(), ',', 'r');
									std::string lastoperator;
									for (std::string& strsrc : strsources)
									{
										boost::algorithm::trim(strsrc);
										if (strsrc.empty())
										{
											continue;
										}
										if (!processing_level && (_isNum(strsrc) || constants.isConstant(strsrc)))
										{
											const double value = _getNum<double>(strsrc, constants);
											if ((/*(!stroperators.empty() &&
												(stroperators.at(0) == "+" || stroperators.at(0) == "-")) ||*/
												(!lastoperator.empty() &&
												(lastoperator == "+" || lastoperator == "-"))) &&
													(std::find_if(sources.begin(), sources.end(), Core::FMTOutputSourceComparator(true)) == sources.end()))
											{
												
												_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
													name + " at line " + std::to_string(m_line),"FMTOutputParser::read", __LINE__, __FILE__, m_section);
											}
											_appendToOutput(
												std::to_string(value),
												outputid,
												themetarget,
												lastoutput,
												lastoperator,
												stroperators,
												sources,
												operators);
											

										}
										else if (processing_level)
										{
											std::vector<double>values;
											if (constants.isConstant(strsrc))
											{
												for (size_t period = 0; period < constants.length(strsrc); ++period)
												{
													values.push_back(_getNum<double>(strsrc, constants, static_cast<int>(period)));
												}
											}
											else {
												std::vector<std::string>all_numbers;
												boost::split(all_numbers, strsrc, boost::is_any_of(" /t"), boost::token_compress_on);
												for (const std::string& number : all_numbers)
												{
													values.push_back(_getNum<double>(number, constants));
												}
											}
											if (operators.empty()&&!sources.empty()&&!sources.back().isVariableLevel())//Well push it
												{
												sources.back()._pushValues(values);
											}else {
												sources.push_back(Core::FMTOutputSource(Core::FMTotar::level, values, outputid, themetarget,name));//constant level!
												}

											
										}
										else {
											std::vector<std::string>values = spliter(strsrc, FMTParser::m_SEPARATOR);
											boost::smatch constantmatch;
											if (values.size() == 1)
											{
												//need to use get equation to simplify output!!!
												std::vector<Core::FMTOutput>::const_iterator it = std::find_if(outputs->begin(), outputs->end(), Core::FMTOutputComparator(strsrc));
												if (it != outputs->end()||boost::regex_search(strsrc, constantmatch, m_rxoutputconstant))
												{
													lastoutput = sources.size() + 1;
													Core::FMTOutput targetoutput;
													if (it==outputs->end())
														{
														const std::string outputname = constantmatch[1];
														const int inttarget_period = _getNum<int>(std::string(constantmatch[3]), constants);
														targetoutput = *std::find_if(outputs->begin(), outputs->end(), Core::FMTOutputComparator(outputname));
														Core::FMTPerBounds bounding(Core::FMTsection::Optimize, inttarget_period, inttarget_period);
														targetoutput = targetoutput.boundTo(themes, bounding, "");
													}else {
														targetoutput = *it;
														}

													if (!targetoutput.isLevel() || (targetoutput.isLevel() && !targetoutput.getSources().empty()))
													{
														//lastoutput = sources.size();
														bool themediff=false;
														for (const Core::FMTOutputSource& src : targetoutput.getSources())
														{
															if(src.getThemeTarget()!=themetarget)
															{
																//warning
																Core::FMTOutputSource newsource = src;
																newsource.setThemeTarget(themetarget);
																sources.push_back(newsource);
																themediff=true;

															}else{
																sources.push_back(src);
															}
														}
														if (themediff)
														{
															_exhandler->raise(Exception::FMTexc::FMTthematic_output_diff,
																				"The thematic of the output "+name+ " is different from the source "+ targetoutput.getName(),
																				"FMTOutputParser::read",
																				__LINE__,
																				__FILE__,
																				m_section);
														}
														//lastopt = operators.size();
														bool convertoperator = false;
														if (!operators.empty() && operators.back().getKey() == Core::FMTokey::sub)
														{
															
															convertoperator = true;
														}
														for (const Core::FMTOperator& src : targetoutput.getOpes())
														{
															if (convertoperator)
															{
																operators.push_back(src.reverse());
															}
															else {
																operators.push_back(src);
															}

														}
													}
													else {
														sources.push_back(Core::FMTOutputSource(Core::FMTotar::level, 0, strsrc,"",outputid,themetarget));
													}
													if (!stroperators.empty())
													{
														operators.push_back(Core::FMTOperator(stroperators.front()));
														lastoperator = stroperators.front();
														stroperators.erase(stroperators.begin());
													}
												}
												else if (strsrc.find("#") != std::string::npos)
												{
													_exhandler->raise(Exception::FMTexc::FMTundefined_constant, strsrc + " at line " + std::to_string(m_line),"FMTOutputParser::read", __LINE__, __FILE__, m_section);
												}
												else if (ylds.isYld(strsrc))//_isYld(ylds,strsrc,m_section))
												{
													//sources.push_back(Core::FMTOutputSource(Core::FMTotar::timeyld, 0, strsrc,"",outputid,themetarget));
													_appendToOutput(
														strsrc,
														outputid,
														themetarget,
														lastoutput,
														lastoperator,
														stroperators,
														sources,
														operators);
													//lastoutput = 0;
												
												}else{
													_exhandler->raise(Exception::FMTexc::FMTundefined_output,
															strsrc + " at line " + std::to_string(m_line),"FMTOutputParser::read", __LINE__, __FILE__, m_section);
												}
											}
											else {
												std::string mask = "";
												std::string rest = " ";
												if ((values.size() < (themes.size() + 2)))//shrinked mask
													{
													size_t themeid = 0;
													while (themeid < themes.size() && 
														themes.at(themeid).isValid(values.at(themeid)))
														{
														mask += values.at(themeid) + " ";
														++themeid;
														}
													while (themeid < values.size())
														{
														rest += values.at(themeid) + " ";
														++themeid;
														}
													}else {
													size_t id = 0;
													for (const std::string& value : values)
													{
														if (id < themes.size())
														{
															mask += value + " ";
														}
														else {
															rest += value + " ";
														}
														++id;
													}
													
													}
												mask = mask.substr(0, mask.size() - 1);
												if (!Core::FMTTheme::validate(themes, mask, " at line " + std::to_string(m_line))) continue;
												std::vector<Core::FMTSpec> specs; 
												const std::string inds = _setSpecs(Core::FMTsection::Outputs, Core::FMTkwor::Source, ylds, constants, specs, rest);
												if (!specs.empty())
												{
													/*std::cout<<name<<std::endl;	
													for(const auto& spec : specs)
													{
														std::cout<<std::string(spec)<<std::endl;
													}*/
													rest = inds;
												}
												Core::FMTOperator opspecs;
												if(operators.empty())
												{
													opspecs = Core::FMTOperator("+");
												}else
												{	
													opspecs = operators.back();
												}
												/*Core::FMTSpec spec;
												const std::string inds = _setSpec(Core::FMTsection::Outputs, Core::FMTkwor::Source, ylds, constants, spec, rest);
												if (!spec.empty())
												{
													rest = inds;
												}*/
												if (inds.find('@') != std::string::npos)
												{
													const std::string warningstr = inds.substr(inds.find('@'), inds.find_first_of(')'));
													_exhandler->raise(Exception::FMTexc::FMTemptybound,
														warningstr + " at line " + std::to_string(m_line),"FMTOutputParser::read", __LINE__, __FILE__, m_section);
													rest = inds.substr(inds.find_first_of(')') + 1, inds.size() - 1);
												}
												if (_isValid(rest))
												{
													if (boost::regex_search(rest, kmatch, m_rxtar))
													{
														if (!std::string(kmatch[25]).empty())
														{
															const std::string action = std::string(kmatch[25]);
															_isAct(m_section, actions, action);
															std::string yld = std::string(kmatch[29]);
															if (_isValid(yld))
															{
																if (!ylds.isYld(yld))
																{
																	_exhandler->raise(Exception::FMTexc::FMTignore,
																		yld + " at line " + std::to_string(m_line),"FMTOutputParser::read", __LINE__, __FILE__, m_section);
																}
															}
															else {
																yld = "";
															}
															//Create outputsources with specs
															if (specs.empty())
															{
																sources.push_back(Core::FMTOutputSource(Core::FMTSpec(), Core::FMTMask(mask, themes),
																Core::FMTotar::actual, yld, action,outputid,themetarget));	
															}
															else if (specs.size()>1)
															{
																bool addoperator = false;
																//only add operators after setting the first source, the number of operators  must be the size of the sources -1
																for(Core::FMTSpec spec : specs)
																{
																	sources.push_back(Core::FMTOutputSource(spec, Core::FMTMask(mask, themes),
																		Core::FMTotar::actual, yld, action,outputid,themetarget));
																	if(addoperator)
																	{
																		operators.push_back(opspecs);
																	}
																	addoperator = true;
																}
															}else{
																sources.push_back(Core::FMTOutputSource(specs.at(0), Core::FMTMask(mask, themes),
																Core::FMTotar::actual, yld, action,outputid,themetarget));
															}
														}
														else if (!std::string(kmatch[17]).empty() || !std::string(kmatch[18]).empty())
														{
															const std::string invtype = std::string(kmatch[17]) + std::string(kmatch[18]);
															std::string yld = std::string(kmatch[22]);
															if (_isValid(yld))
															{
																if (!ylds.isYld(yld))
																{
																	_exhandler->raise(Exception::FMTexc::FMTignore,
																		yld + " at line " + std::to_string(m_line),"FMTOutputParser::read", __LINE__, __FILE__, m_section);
																}

															}
															else {
																yld = "";
															}

															const std::string lockinv = kmatch[18];
															//Create outputsources with specs
															if (specs.empty())
															{
																Core::FMTSpec spec;
																if (!lockinv.empty())
																{
																	const int lower = 1;
																	constexpr int upper = std::numeric_limits<int>::max();
																	spec.addBounds(Core::FMTLockBounds(Core::FMTsection::Outputs,
																							Core::FMTkwor::Source, upper, lower));
																}
																sources.push_back(Core::FMTOutputSource(spec, Core::FMTMask(mask, themes),
																					Core::FMTotar::inventory, yld,"",outputid,themetarget));	
															}
															else if (specs.size()>1)
															{
																bool addoperator = false;
																//only add operators after setting the first source, the number of operators  must be the size of the sources -1
																for(Core::FMTSpec spec : specs)
																{
																	if (!lockinv.empty())
																	{
																		const int lower = 1;
																		constexpr int upper = std::numeric_limits<int>::max();
																		spec.addBounds(Core::FMTLockBounds(Core::FMTsection::Outputs,
																								Core::FMTkwor::Source, upper, lower));
																	}
																	sources.push_back(Core::FMTOutputSource(spec, Core::FMTMask(mask, themes),
																					Core::FMTotar::inventory, yld,"",outputid,themetarget));
																	if(addoperator)
																	{
																		operators.push_back(opspecs);
																	}
																	addoperator = true;
																}
															}
															else
															{
																if (!lockinv.empty())
																{
																	const int lower = 1;
																	constexpr int upper = std::numeric_limits<int>::max();
																	specs.at(0).addBounds(Core::FMTLockBounds(Core::FMTsection::Outputs,
																							Core::FMTkwor::Source, upper, lower));
																}

																sources.push_back(Core::FMTOutputSource(specs.at(0), Core::FMTMask(mask, themes),
																					Core::FMTotar::inventory, yld,"",outputid,themetarget));
															}
														}
														else if (!std::string(kmatch[3]).empty())
														{
															const std::string action = std::string(kmatch[7]);
															_isAct(m_section, actions, action);
															std::string yld = std::string(kmatch[13]);

															if (_isValid(yld))
															{
																if (!ylds.isYld(yld))
																{

																	_exhandler->raise(Exception::FMTexc::FMTignore,
																		yld + " at line " + std::to_string(m_line),"FMTOutputParser::read", __LINE__, __FILE__, m_section);
																}

															}
															else {
																yld.clear();
															}
															//Create outputsources with specs
															if (specs.empty())
															{
																sources.push_back(Core::FMTOutputSource(Core::FMTSpec(), Core::FMTMask(mask, themes),
																					Core::FMTotar::inventory, yld, action,outputid,themetarget));	
															}
															else if (specs.size()>1)
															{
																bool addoperator = false;
																//only add operators after setting the first source, the number of operators  must be the size of the sources -1
																for(Core::FMTSpec spec : specs)
																{
																	sources.push_back(Core::FMTOutputSource(spec, Core::FMTMask(mask, themes),
																						Core::FMTotar::inventory, yld, action,outputid,themetarget));
																	if(addoperator)
																	{
																		operators.push_back(opspecs);
																	}
																	addoperator = true;
																}

															}else{
																sources.push_back(Core::FMTOutputSource(specs.at(0), Core::FMTMask(mask, themes),
																					Core::FMTotar::inventory, yld, action,outputid,themetarget));
																}

														}
														if (!stroperators.empty())
														{
															lastoutput = sources.size();
															operators.push_back(Core::FMTOperator(stroperators.front()));
															lastoperator = stroperators.front();
															stroperators.erase(stroperators.begin());
														}

													}
												}else {
													
													_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
														"Non valid output keywords "+rest + " at line " + std::to_string(m_line), "FMTOutputParser::read", __LINE__, __FILE__, m_section);
												}

											}
										}

									}

									for (const std::string& strope : stroperators)
									{
										operators.push_back(Core::FMTOperator(strope));
									}
									insource = true;

								}
							}
						}
						if (!sources.empty() || (processing_level && !insource))
						{
							if (processing_level && sources.empty())
							{
								sources.push_back(Core::FMTOutputSource(Core::FMTotar::level, 0, "", name,outputid,themetarget));
							}
							if (!processing_level)
							{
								if (operators.size()==sources.size())
								{
									_exhandler->raise(Exception::FMTexc::FMToutput_too_much_operator,
														name +" at line "+std::to_string(lastsourcelineid) ,"FMTOutputParser::read", __LINE__, __FILE__, m_section);
									operators.pop_back();
								}else if(operators.size()>sources.size())
								{
									_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
													name +" at line "+std::to_string(lastsourcelineid) ,"FMTOutputParser::read", __LINE__, __FILE__, m_section);

								}else if (operators.size()<sources.size()-1)
								{
									_exhandler->raise(Exception::FMTexc::FMToutput_missing_operator,
														name +" at line "+std::to_string(lastsourcelineid) ,"FMTOutputParser::read", __LINE__, __FILE__, m_section);
								}
							}
							
							outputs->push_back(Core::FMTOutput(name, description, lastgroup, sources, operators));
							/**_logger<<name<<"\n";
							int id = 0;
							for(const auto& s:sources)
							{
								const std::string operatorstrloop = (id<operators.size()) ? std::string(operators.at(id)) : " ";
								*_logger<<std::string(s)+" "+operatorstrloop<<"\n";
								++id;
							}
							*_logger<<"\n"<<std::string(outputs->back())<<"\n";*/
							//outputs->back().passinobject(*this);
							++outputid;
						}
					}
				}
			outputs->shrink_to_fit();
			/*-----------------------------------------
			-----------Fix for outputorigin------------
			-------------------------------------------*/
			int index = 0; 
			for (Core::FMTOutput& output : *outputs)
				{
				output.changeSourcesId(index);
				++index;
				}
			/*-----------------------------------------
			-----------Fix for outputorigin------------
			-------------------------------------------*/
			}catch(...)
			{
				_exhandler->raiseFromCatch("Output "+name +" In " + m_location + " at line " + std::to_string(m_line),"FMTOutputParser::_readNFill", __LINE__, __FILE__,m_section);
			}
		}


		std::vector<Core::FMTOutput> FMTOutputParser::read(const std::vector<Core::FMTTheme>& themes,const std::vector<Core::FMTAction>& actions,
			const Core::FMTYields& ylds,const Core::FMTConstants& constants, const std::string& location)
            {
			std::vector<Core::FMTOutput>outputs;
			try {
				_readNFill(&outputs,themes,actions,ylds,constants,location);
			}catch (...)
				{
				_exhandler->raiseFromCatch("In " + m_location + " at line " + std::to_string(m_line),"FMTOutputParser::read", __LINE__, __FILE__,m_section);
				}
            return outputs;
            }
		std::vector<Core::FMTOutput> FMTOutputParser::addOutputs(const std::vector<Core::FMTOutput> oldoutputs, 
																	const std::vector<Core::FMTTheme>& themes,
																	const std::vector<Core::FMTAction>& actions,
																	const Core::FMTYields& ylds,const Core::FMTConstants& constants,
																	const std::string& location,
																	std::vector<std::string> outputsnames)
		{
			std::vector<Core::FMTOutput>outputs = oldoutputs;
			try {
				_readNFill(&outputs,themes,actions,ylds,constants,location);
				//This part remove all outputs parsed from file that are not in outputsname
				if (!outputsnames.empty())
				{
					std::vector<Core::FMTOutput>returnoutputs = oldoutputs;
					std::set<int> themeids;
					for(int themeid=0;themeid<themes.size();++themeid)
					{
						themeids.insert(themeid);
					}
					std::set<int> outputkeptid;
					std::set<int>::const_iterator hint=outputkeptid.end();
					for(int oldoutputid=0;oldoutputid<oldoutputs.size();++oldoutputid)
					{
						outputkeptid.insert(hint,oldoutputid);
					}
					for(const std::string& outputname : outputsnames)
					{
						std::vector<Core::FMTOutput>::const_iterator it = std::find_if(outputs.begin()+oldoutputs.size(), outputs.end(), Core::FMTOutputComparator(outputname));
						if(it==outputs.end())
						{
							_exhandler->raise(Exception::FMTexc::FMTundefined_output,
														outputname+" not found in file "+location,"FMTOutputParser::addOutputs", __LINE__, __FILE__, m_section);
						}
						returnoutputs.push_back(*it);
						outputkeptid.insert(static_cast<int>(std::distance(outputs.cbegin(),it)));
					}
					for(Core::FMTOutput& output : returnoutputs)
					{
						output.changeSourcesId(outputkeptid,themeids);
					}
					outputs=returnoutputs;
				}
			}catch (...)
				{
				_exhandler->printExceptions("In " + m_location + " at line " + std::to_string(m_line),"FMTOutputParser::addOutputs", __LINE__, __FILE__,m_section);
				}
            return outputs;
           }
        void FMTOutputParser::write(const std::vector<Core::FMTOutput>& outputs, const std::string& location) const
            {
			try {
				std::ofstream outputstream;
				outputstream.open(location);
				std::string lastgroup;
				if (tryOpening(outputstream, location))
				{
					for (const Core::FMTOutput& out : outputs)
					{
						if (lastgroup != out.getGroup())
							{
							outputstream << "*GROUP "+out.getGroup() << "\n";
							lastgroup = out.getGroup();
							}
							outputstream << std::string(out) << "\n";
						}
					outputstream.close();
				}
			}catch (...)
				{
				_exhandler->raiseFromCatch("at " + location,"FMTOutputParser::write", __LINE__, __FILE__,m_section);
				}
            }

}
