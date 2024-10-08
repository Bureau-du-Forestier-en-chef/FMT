/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMToutputparser.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTconstants.h"
#include "FMToutput.h"
#include "FMTexceptionhandler.h"

namespace Parser
{
	const boost::regex FMToutputparser::rxoutput=boost::regex("(\\*OUTPUT|\\*LEVEL)(([\\s\\t]*)([^\\s\\t\\(]*)([\\s\\t]*)(\\()([^\\s\\t\\)]*)(\\))([\\s\\t]*)(.+))|((\\*OUTPUT|\\*LEVEL)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(.+))", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToutputparser::rxsource = boost::regex("(\\*SOURCE)([\\s\\t]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToutputparser::rxtar = boost::regex("(([\\s\\t]*)(_INVENT)([\\s\\t]*)(\\()([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(\\))([\\s\\t]*)((_AREA)|([^\\s\\t]*)))|(([\\s\\t]*)((_INVENT)|(_INVLOCK))([\\s\\t]*)((_AREA)|([^\\s\\t]*)))|(([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)((_AREA)|([^\\s\\t]*)))", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToutputparser::rxgrp = boost::regex("(\\*GROUP)([\\s\\t]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMToutputparser::rxoutputconstant = boost::regex("([^\\[]*)(\\[[\\s\\t]*)(\\-?[0-9])([\\s\\t]*\\])", boost::regex_constants::ECMAScript | boost::regex_constants::icase);


       FMToutputparser::FMToutputparser():FMTparser()
            {
		    setsection(Core::FMTsection::Outputs);
            }

	   void FMToutputparser::appendtooutput(
		   const std::string& strvalue,
		   const int& outputid,
		   const int& themetarget,
		   const size_t& lastoutput,
		   std::string& lastoperator,
		   std::vector<std::string>& stroperators,
		   std::vector<Core::FMToutputsource>& sources,
		   std::vector<Core::FMToperator>& operators) const
	   {
		   try {
			   double value = 0;
			   Core::FMTotar targetof = Core::FMTotar::timeyld;
			   bool isnumber = false;
			   std::string yldtarget = strvalue;
			   if (isnum(strvalue))
			   {
				   value = std::stod(strvalue);
				   isnumber = true;
				   targetof = Core::FMTotar::val;
				   Core::FMToperator lastopof(lastoperator);
				   if (lastopof.isfactor())
						{
					    for (size_t factorId = 0; factorId < operators.size() - 1; ++factorId)
							{
							if (operators.at(factorId).isdivide())
								{
								if (lastopof.isdivide())
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
				   std::vector<Core::FMToutputsource>newsources;
				   std::vector<Core::FMToperator>newoperators;
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
					   //if (isnumber)
					   //{
						   double srcvalue = value;
						   if (id > 0 && (sources.at(id - 1).isvariable() || (sources.at(id  - 1).islevel() && !sources.at(id  - 1).isvariablelevel())) /* && operators.at(lastop).isfactor()*/)
						   {
							   /*if (!operators.at(lastop).isfactor()) // If it comes from same output dont multiply...
							   {
								   std::cout << "Test "<<id << "\n";
							   }*/
							   //if (isnumber)
							   //{
								   if (sources.at(id).isconstant()&&isnumber)
								   {
									   srcvalue = Core::FMToperator(operators.at(lastop)).call(srcvalue, sources.at(id).getvalue());
								   }
								   else{
									   newoperators.push_back(Core::FMToperator(lastoperator));
								   }
								newsources.push_back(Core::FMToutputsource(targetof, srcvalue, yldtarget, "", sources.at(id).getoutputorigin(), sources.at(id).getthemetarget()));
							   //}
							   
						   }
						   
					   //}
					   if (sources.at(id).isvariable()  || sources.at(id).islevel() || sources.at(id).istimeyield() || !isnumber)
					   {
						   newsources.push_back(sources.at(id));
					   }
					   if (id > 0)
					   {
						   newoperators.push_back(operators.at(lastop));
						   ++lastop;
					   }
				   }
				   if (newsources.back().isvariable() || newsources.back().islevel() /* || !isnumber*/)
				   {
					   newsources.push_back(Core::FMToutputsource(targetof, value, yldtarget, "", newsources.back().getoutputorigin(), newsources.back().getthemetarget()));

					   newoperators.push_back(Core::FMToperator(lastoperator));
				   }

				   operators = newoperators;
				   sources = newsources;
				   lastoperator.clear();
				   if (!stroperators.empty())
				   {
					   operators.push_back(Core::FMToperator(stroperators.front()));
					   lastoperator = stroperators.front();
					   stroperators.erase(stroperators.begin());
				   }
			   }
			   else {
				   sources.push_back(Core::FMToutputsource(targetof, value, yldtarget, "", outputid, themetarget));

			   }
		   }catch (...)
		   {
			   _exhandler->raisefromcatch("In " + _location + " at line " + std::to_string(_line), "FMToutputparser::appendtooutput", __LINE__, __FILE__, _section);
		   }
	
	   }


		void FMToutputparser::readnfill(std::vector<Core::FMToutput>* outputs, 
					const std::vector<Core::FMTtheme>& themes,
					const std::vector<Core::FMTaction>& actions,
					const Core::FMTyields& ylds,const Core::FMTconstants& constants,
					const std::string& location)
		{
			std::string name, description;
			try {
				if (!location.empty())
				{
					std::ifstream outputstream(location);
					std::vector<Core::FMToutputsource>sources;
					std::vector<Core::FMToperator>operators;
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
					if (FMTparser::tryopening(outputstream, location))
					{
						while (outputstream.is_open())
						{
							std::string line = getcleanlinewfor(outputstream, themes, constants);
							if (!line.empty())
							{
								boost::smatch kmatch;
								const std::string outline = line + " ";
								if (boost::regex_search(outline, kmatch, rxoutput))
								{
									if (!sources.empty() || (processing_level && !insource))
									{
										if (processing_level && sources.empty())
										{
											sources.push_back(Core::FMToutputsource(Core::FMTotar::level, 0, "", name,outputid,themetarget));
										}
										if (!processing_level)
										{
											if (operators.size()==sources.size())
											{
												_exhandler->raise(Exception::FMTexc::FMToutput_too_much_operator,
																	name +" at line "+std::to_string(lastsourcelineid) ,"FMToutputparser::read", __LINE__, __FILE__, _section);
												operators.pop_back();
											}else if(operators.size()>sources.size())
											{
												_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
																name +" at line "+std::to_string(lastsourcelineid) ,"FMToutputparser::read", __LINE__, __FILE__, _section);

											}else if (operators.size()<sources.size()-1)
											{
												_exhandler->raise(Exception::FMTexc::FMToutput_missing_operator,
																	name +" at line "+std::to_string(lastsourcelineid) ,"FMToutputparser::read", __LINE__, __FILE__, _section);
											}
										}
										outputs->push_back(Core::FMToutput(name, description, lastgroup, sources, operators));
										
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
									if (isvalid(thtarget))
									{
										thtarget.erase(thtarget.begin(), thtarget.begin() + 3);
										themetarget = getnum<int>(thtarget) - 1;
									}else{
										themetarget=-1;
									}
									name = std::string(kmatch[4]) + std::string(kmatch[14]);
									lastsourcelineid = _line;
									description = std::string(kmatch[10]) + std::string(kmatch[16]);
									boost::trim_right(description);
									insource = false;
								}
								if (boost::regex_search(line, kmatch, rxgrp))
								{
									std::string groupname(kmatch[3]);
									boost::trim(groupname);
									lastgroup = groupname;
									insource = false;
								}
								else if (boost::regex_search(line, kmatch, rxsource) || insource)
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
										if (!stroperators.empty() && isnum(stacked_char))
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
										if (!processing_level && (isnum(strsrc) || constants.isconstant(strsrc)))
										{
											const double value = getnum<double>(strsrc, constants);
											if ((/*(!stroperators.empty() &&
												(stroperators.at(0) == "+" || stroperators.at(0) == "-")) ||*/
												(!lastoperator.empty() &&
												(lastoperator == "+" || lastoperator == "-"))) &&
													(std::find_if(sources.begin(), sources.end(), Core::FMToutputsourcecomparator(true)) == sources.end()))
											{
												
												_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
													name + " at line " + std::to_string(_line),"FMToutputparser::read", __LINE__, __FILE__, _section);
											}
											appendtooutput(
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
											if (constants.isconstant(strsrc))
											{
												for (size_t period = 0; period < constants.length(strsrc); ++period)
												{
													values.push_back(getnum<double>(strsrc, constants, static_cast<int>(period)));
												}
											}
											else {
												std::vector<std::string>all_numbers;
												boost::split(all_numbers, strsrc, boost::is_any_of(" /t"), boost::token_compress_on);
												for (const std::string& number : all_numbers)
												{
													values.push_back(getnum<double>(number, constants));
												}
											}
											if (operators.empty()&&!sources.empty()&&!sources.back().isvariablelevel())//Well push it
												{
												sources.back().pushvalues(values);
											}else {
												sources.push_back(Core::FMToutputsource(Core::FMTotar::level, values, outputid, themetarget,name));//constant level!
												}

											
										}
										else {
											std::vector<std::string>values = spliter(strsrc, FMTparser::rxseparator);
											boost::smatch constantmatch;
											if (values.size() == 1)
											{
												//need to use get equation to simplify output!!!
												std::vector<Core::FMToutput>::const_iterator it = std::find_if(outputs->begin(), outputs->end(), Core::FMToutputcomparator(strsrc));
												if (it != outputs->end()||boost::regex_search(strsrc, constantmatch, rxoutputconstant))
												{
													lastoutput = sources.size() + 1;
													Core::FMToutput targetoutput;
													if (it==outputs->end())
														{
														const std::string outputname = constantmatch[1];
														const int inttarget_period = getnum<int>(std::string(constantmatch[3]), constants);
														targetoutput = *std::find_if(outputs->begin(), outputs->end(), Core::FMToutputcomparator(outputname));
														Core::FMTperbounds bounding(Core::FMTsection::Optimize, inttarget_period, inttarget_period);
														targetoutput = targetoutput.boundto(themes, bounding, "");
													}else {
														targetoutput = *it;
														}

													if (!targetoutput.islevel() || (targetoutput.islevel() && !targetoutput.getsources().empty()))
													{
														//lastoutput = sources.size();
														bool themediff=false;
														for (const Core::FMToutputsource& src : targetoutput.getsources())
														{
															if(src.getthemetarget()!=themetarget)
															{
																//warning
																Core::FMToutputsource newsource = src;
																newsource.setthemetarget(themetarget);
																sources.push_back(newsource);
																themediff=true;

															}else{
																sources.push_back(src);
															}
														}
														if (themediff)
														{
															_exhandler->raise(Exception::FMTexc::FMTthematic_output_diff,
																				"The thematic of the output "+name+ " is different from the source "+ targetoutput.getname(),
																				"FMToutputparser::read",
																				__LINE__,
																				__FILE__,
																				_section);
														}
														//lastopt = operators.size();
														bool convertoperator = false;
														if (!operators.empty() && operators.back().getkey() == Core::FMTokey::sub)
														{
															
															convertoperator = true;
														}
														for (const Core::FMToperator& src : targetoutput.getopes())
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
														sources.push_back(Core::FMToutputsource(Core::FMTotar::level, 0, strsrc,"",outputid,themetarget));
													}
													if (!stroperators.empty())
													{
														operators.push_back(Core::FMToperator(stroperators.front()));
														lastoperator = stroperators.front();
														stroperators.erase(stroperators.begin());
													}
												}
												else if (strsrc.find("#") != std::string::npos)
												{
													_exhandler->raise(Exception::FMTexc::FMTundefined_constant, strsrc + " at line " + std::to_string(_line),"FMToutputparser::read", __LINE__, __FILE__, _section);
												}
												else if (ylds.isyld(strsrc))//isyld(ylds,strsrc,_section))
												{
													//sources.push_back(Core::FMToutputsource(Core::FMTotar::timeyld, 0, strsrc,"",outputid,themetarget));
													appendtooutput(
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
															strsrc + " at line " + std::to_string(_line),"FMToutputparser::read", __LINE__, __FILE__, _section);
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
												if (!Core::FMTtheme::validate(themes, mask, " at line " + std::to_string(_line))) continue;
												std::vector<Core::FMTspec> specs; 
												const std::string inds = setspecs(Core::FMTsection::Outputs, Core::FMTkwor::Source, ylds, constants, specs, rest);
												if (!specs.empty())
												{
													/*std::cout<<name<<std::endl;	
													for(const auto& spec : specs)
													{
														std::cout<<std::string(spec)<<std::endl;
													}*/
													rest = inds;
												}
												Core::FMToperator opspecs;
												if(operators.empty())
												{
													opspecs = Core::FMToperator("+");
												}else
												{	
													opspecs = operators.back();
												}
												/*Core::FMTspec spec;
												const std::string inds = setspec(Core::FMTsection::Outputs, Core::FMTkwor::Source, ylds, constants, spec, rest);
												if (!spec.empty())
												{
													rest = inds;
												}*/
												if (inds.find('@') != std::string::npos)
												{
													const std::string warningstr = inds.substr(inds.find('@'), inds.find_first_of(')'));
													_exhandler->raise(Exception::FMTexc::FMTemptybound,
														warningstr + " at line " + std::to_string(_line),"FMToutputparser::read", __LINE__, __FILE__, _section);
													rest = inds.substr(inds.find_first_of(')') + 1, inds.size() - 1);
												}
												if (isvalid(rest))
												{
													if (boost::regex_search(rest, kmatch, rxtar))
													{
														if (!std::string(kmatch[25]).empty())
														{
															const std::string action = std::string(kmatch[25]);
															isact(_section, actions, action);
															std::string yld = std::string(kmatch[29]);
															if (isvalid(yld))
															{
																if (!ylds.isyld(yld))
																{
																	_exhandler->raise(Exception::FMTexc::FMTignore,
																		yld + " at line " + std::to_string(_line),"FMToutputparser::read", __LINE__, __FILE__, _section);
																}
															}
															else {
																yld = "";
															}
															//Create outputsources with specs
															if (specs.empty())
															{
																sources.push_back(Core::FMToutputsource(Core::FMTspec(), Core::FMTmask(mask, themes),
																Core::FMTotar::actual, yld, action,outputid,themetarget));	
															}
															else if (specs.size()>1)
															{
																bool addoperator = false;
																//only add operators after setting the first source, the number of operators  must be the size of the sources -1
																for(Core::FMTspec spec : specs)
																{
																	sources.push_back(Core::FMToutputsource(spec, Core::FMTmask(mask, themes),
																		Core::FMTotar::actual, yld, action,outputid,themetarget));
																	if(addoperator)
																	{
																		operators.push_back(opspecs);
																	}
																	addoperator = true;
																}
															}else{
																sources.push_back(Core::FMToutputsource(specs.at(0), Core::FMTmask(mask, themes),
																Core::FMTotar::actual, yld, action,outputid,themetarget));
															}
														}
														else if (!std::string(kmatch[17]).empty() || !std::string(kmatch[18]).empty())
														{
															const std::string invtype = std::string(kmatch[17]) + std::string(kmatch[18]);
															std::string yld = std::string(kmatch[22]);
															if (isvalid(yld))
															{
																if (!ylds.isyld(yld))
																{
																	_exhandler->raise(Exception::FMTexc::FMTignore,
																		yld + " at line " + std::to_string(_line),"FMToutputparser::read", __LINE__, __FILE__, _section);
																}

															}
															else {
																yld = "";
															}

															const std::string lockinv = kmatch[18];
															//Create outputsources with specs
															if (specs.empty())
															{
																Core::FMTspec spec;
																if (!lockinv.empty())
																{
																	const int lower = 1;
																	constexpr int upper = std::numeric_limits<int>::max();
																	spec.addbounds(Core::FMTlockbounds(Core::FMTsection::Outputs,
																							Core::FMTkwor::Source, upper, lower));
																}
																sources.push_back(Core::FMToutputsource(spec, Core::FMTmask(mask, themes),
																					Core::FMTotar::inventory, yld,"",outputid,themetarget));	
															}
															else if (specs.size()>1)
															{
																bool addoperator = false;
																//only add operators after setting the first source, the number of operators  must be the size of the sources -1
																for(Core::FMTspec spec : specs)
																{
																	if (!lockinv.empty())
																	{
																		const int lower = 1;
																		constexpr int upper = std::numeric_limits<int>::max();
																		spec.addbounds(Core::FMTlockbounds(Core::FMTsection::Outputs,
																								Core::FMTkwor::Source, upper, lower));
																	}
																	sources.push_back(Core::FMToutputsource(spec, Core::FMTmask(mask, themes),
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
																	specs.at(0).addbounds(Core::FMTlockbounds(Core::FMTsection::Outputs,
																							Core::FMTkwor::Source, upper, lower));
																}

																sources.push_back(Core::FMToutputsource(specs.at(0), Core::FMTmask(mask, themes),
																					Core::FMTotar::inventory, yld,"",outputid,themetarget));
															}
														}
														else if (!std::string(kmatch[3]).empty())
														{
															const std::string action = std::string(kmatch[7]);
															isact(_section, actions, action);
															std::string yld = std::string(kmatch[13]);

															if (isvalid(yld))
															{
																if (!ylds.isyld(yld))
																{

																	_exhandler->raise(Exception::FMTexc::FMTignore,
																		yld + " at line " + std::to_string(_line),"FMToutputparser::read", __LINE__, __FILE__, _section);
																}

															}
															else {
																yld.clear();
															}
															//Create outputsources with specs
															if (specs.empty())
															{
																sources.push_back(Core::FMToutputsource(Core::FMTspec(), Core::FMTmask(mask, themes),
																					Core::FMTotar::inventory, yld, action,outputid,themetarget));	
															}
															else if (specs.size()>1)
															{
																bool addoperator = false;
																//only add operators after setting the first source, the number of operators  must be the size of the sources -1
																for(Core::FMTspec spec : specs)
																{
																	sources.push_back(Core::FMToutputsource(spec, Core::FMTmask(mask, themes),
																						Core::FMTotar::inventory, yld, action,outputid,themetarget));
																	if(addoperator)
																	{
																		operators.push_back(opspecs);
																	}
																	addoperator = true;
																}

															}else{
																sources.push_back(Core::FMToutputsource(specs.at(0), Core::FMTmask(mask, themes),
																					Core::FMTotar::inventory, yld, action,outputid,themetarget));
																}

														}
														if (!stroperators.empty())
														{
															lastoutput = sources.size();
															operators.push_back(Core::FMToperator(stroperators.front()));
															lastoperator = stroperators.front();
															stroperators.erase(stroperators.begin());
														}

													}
												}else {
													
													_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
														"Non valid output keywords "+rest + " at line " + std::to_string(_line), "FMToutputparser::read", __LINE__, __FILE__, _section);
												}

											}
										}

									}

									for (const std::string& strope : stroperators)
									{
										operators.push_back(Core::FMToperator(strope));
									}
									insource = true;

								}
							}
						}
						if (!sources.empty() || (processing_level && !insource))
						{
							if (processing_level && sources.empty())
							{
								sources.push_back(Core::FMToutputsource(Core::FMTotar::level, 0, "", name,outputid,themetarget));
							}
							if (!processing_level)
							{
								if (operators.size()==sources.size())
								{
									_exhandler->raise(Exception::FMTexc::FMToutput_too_much_operator,
														name +" at line "+std::to_string(lastsourcelineid) ,"FMToutputparser::read", __LINE__, __FILE__, _section);
									operators.pop_back();
								}else if(operators.size()>sources.size())
								{
									_exhandler->raise(Exception::FMTexc::FMTunsupported_output,
													name +" at line "+std::to_string(lastsourcelineid) ,"FMToutputparser::read", __LINE__, __FILE__, _section);

								}else if (operators.size()<sources.size()-1)
								{
									_exhandler->raise(Exception::FMTexc::FMToutput_missing_operator,
														name +" at line "+std::to_string(lastsourcelineid) ,"FMToutputparser::read", __LINE__, __FILE__, _section);
								}
							}
							
							outputs->push_back(Core::FMToutput(name, description, lastgroup, sources, operators));
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
			for (Core::FMToutput& output : *outputs)
				{
				output.changesourcesid(index);
				++index;
				}
			/*-----------------------------------------
			-----------Fix for outputorigin------------
			-------------------------------------------*/
			}catch(...)
			{
				_exhandler->raisefromcatch("Output "+name +" In " + _location + " at line " + std::to_string(_line),"FMToutputparser::readnfill", __LINE__, __FILE__,_section);
			}
		}


		std::vector<Core::FMToutput> FMToutputparser::read(const std::vector<Core::FMTtheme>& themes,const std::vector<Core::FMTaction>& actions,
			const Core::FMTyields& ylds,const Core::FMTconstants& constants, const std::string& location)
            {
			std::vector<Core::FMToutput>outputs;
			try {
				readnfill(&outputs,themes,actions,ylds,constants,location);
			}catch (...)
				{
				_exhandler->raisefromcatch("In " + _location + " at line " + std::to_string(_line),"FMToutputparser::read", __LINE__, __FILE__,_section);
				}
            return outputs;
            }
		std::vector<Core::FMToutput> FMToutputparser::addoutputs(const std::vector<Core::FMToutput> oldoutputs, 
																	const std::vector<Core::FMTtheme>& themes,
																	const std::vector<Core::FMTaction>& actions,
																	const Core::FMTyields& ylds,const Core::FMTconstants& constants,
																	const std::string& location,
																	std::vector<std::string> outputsnames)
		{
			std::vector<Core::FMToutput>outputs = oldoutputs;
			try {
				readnfill(&outputs,themes,actions,ylds,constants,location);
				//This part remove all outputs parsed from file that are not in outputsname
				if (!outputsnames.empty())
				{
					std::vector<Core::FMToutput>returnoutputs = oldoutputs;
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
						std::vector<Core::FMToutput>::const_iterator it = std::find_if(outputs.begin()+oldoutputs.size(), outputs.end(), Core::FMToutputcomparator(outputname));
						if(it==outputs.end())
						{
							_exhandler->raise(Exception::FMTexc::FMTundefined_output,
														outputname+" not found in file "+location,"FMToutputparser::addoutputs", __LINE__, __FILE__, _section);
						}
						returnoutputs.push_back(*it);
						outputkeptid.insert(static_cast<int>(std::distance(outputs.cbegin(),it)));
					}
					for(Core::FMToutput& output : returnoutputs)
					{
						output.changesourcesid(outputkeptid,themeids);
					}
					outputs=returnoutputs;
				}
			}catch (...)
				{
				_exhandler->printexceptions("In " + _location + " at line " + std::to_string(_line),"FMToutputparser::addoutputs", __LINE__, __FILE__,_section);
				}
            return outputs;
           }
        void FMToutputparser::write(const std::vector<Core::FMToutput>& outputs, const std::string& location) const
            {
			try {
				std::ofstream outputstream;
				outputstream.open(location);
				std::string lastgroup;
				if (tryopening(outputstream, location))
				{
					for (const Core::FMToutput& out : outputs)
					{
						if (lastgroup != out.getgroup())
							{
							outputstream << "*GROUP "+out.getgroup() << "\n";
							lastgroup = out.getgroup();
							}
							outputstream << std::string(out) << "\n";
						}
					outputstream.close();
				}
			}catch (...)
				{
				_exhandler->raisefromcatch("at " + location,"FMToutputparser::write", __LINE__, __FILE__,_section);
				}
            }

}
