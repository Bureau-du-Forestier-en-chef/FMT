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

#include "FMToutputparser.h"

namespace WSParser
{

       FMToutputparser::FMToutputparser():FMTparser(),
            //rxoperators(regex("([^\\+\\-\\/\\*]*)([\\+\\-\\/\\*]*)",regex_constants::icase)),
            rxoutput(regex("(\\*OUTPUT|\\*LEVEL)(([\\s\\t]*)([^\\s\\t\\(]*)([\\s\\t]*)(\\()([^\\s\\t\\)]*)(\\))([\\s\\t]*)(.+))|((\\*OUTPUT|\\*LEVEL)([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(.+))",regex_constants::ECMAScript|regex_constants::icase)),
            rxsource(regex("(\\*SOURCE)([\\s\\t]*)(.+)",regex_constants::ECMAScript|regex_constants::icase)),
            rxtar(regex("(([\\s\\t]*)(_INVENT)([\\s\\t]*)(\\()([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)(\\))([\\s\\t]*)((_AREA)|([^\\s\\t]*)))|(([\\s\\t]*)((_INVENT)|(_INVLOCK))([\\s\\t]*)((_AREA)|([^\\s\\t]*)))|(([\\s\\t]*)([^\\s\\t]*)([\\s\\t]*)((_AREA)|([^\\s\\t]*)))",regex_constants::ECMAScript|regex_constants::icase)),
            rxgrp(regex("(\\*GROUP)([\\s\\t]*)([^\\s\\t\\(]*)(.+)",regex_constants::ECMAScript|regex_constants::icase))
            {

            }
        FMToutputparser::FMToutputparser(const FMToutputparser& rhs): FMTparser(rhs),
            //rxoperators(rhs.rxoperators),
            rxoutput(rhs.rxoutput),
            rxsource(rhs.rxsource),
            rxtar(rhs.rxtar),
            rxgrp(rhs.rxgrp)
            {

            }
        FMToutputparser& FMToutputparser::operator = (const FMToutputparser& rhs)
            {
            if(this!=&rhs)
                {
                FMTparser::operator =(rhs);
               // rxoperators = rhs.rxoperators;
                rxoutput = rhs.rxoutput;
                rxsource = rhs.rxsource;
                rxtar = rhs.rxtar;
                rxgrp = rhs.rxgrp;
                }
            return *this;
            }


        vector<FMToutput> FMToutputparser::read(const vector<FMTtheme>& themes,const vector<FMTaction>& actions,
			const FMTyields& ylds,const FMTconstants& constants, const map<string, vector<string>>& actions_aggregate,string location)
            {
            ifstream outputstream(location);
            vector<FMToutput>outputs;
            vector<FMToutputsource>sources;
            vector<FMToperator>operators;
            string name,description;
            bool insource = false;
			bool processing_level = false;
			int themetarget = -1;
			size_t lastopt = 0;
			size_t lastoutput = 0;
            if (FMTparser::tryopening(outputstream,location))
                {
                string line;
                while(outputstream.is_open())
                    {

                     line = getcleanlinewfor(outputstream,themes,constants);
                     if (!line.empty())
                        {
                        smatch kmatch;
                        string outline = line+" ";
                        if (regex_search(outline,kmatch,rxoutput))
                            {
                            if (!sources.empty() || (processing_level && !insource))
                                {
                                if (processing_level && sources.empty())
                                    {
                                    sources.push_back(FMToutputsource(FMTotar::level,0,"",name));
                                    }
                                outputs.push_back(FMToutput(name,description, themetarget,sources,operators));
								}
                            sources.clear();
							lastopt = 0;
							lastoutput = 0;
                            operators.clear();
							string outtype = string(kmatch[1]) + string(kmatch[12]);

							if (outtype=="*LEVEL")
								{
								processing_level = true;
								//_exhandler->raise(FMTexc::WSunsupported_output, _section, name + " at line " + to_string(_line), __LINE__, __FILE__);
								//continue;
								}else {
								processing_level = false;
								}


							string thtarget = string(kmatch[7]);
							if (isvalid(thtarget))
								{
								thtarget.erase(thtarget.begin(), thtarget.begin() + 3);
								//PySys_WriteStdout(thtarget.c_str());
								themetarget = stoi(thtarget)-1;
								}
                            name = string(kmatch[4]) + string(kmatch[14]);
                            description = string(kmatch[10]) + string(kmatch[16]);
							boost::trim_right(description);
                            insource = false;
						}
                        if (regex_search(line,kmatch,rxgrp))
                            {
                            insource = false;
                            }else if (regex_search(line,kmatch,rxsource) || insource)
                                {

                                string rest;
                                if (insource && line.find("*SOURCE")==string::npos)
                                    {
                                    rest = line;
                                    }else{
                                    rest = kmatch[3];
                                    }

                                vector<string>strsources;// = spliter(rest,rxsources);
                                boost::split(strsources,rest,boost::is_any_of("-*/+"));
                                string opstr = rest;
                                replace(opstr.begin(),opstr.end(),'.','r');
                                replace(opstr.begin(),opstr.end(),',','r');
                                vector<string>stroperators = spliter(opstr,rxoperators);
								string lastoperator;
                                for(string& strsrc : strsources)
                                    {
                                    boost::algorithm::trim(strsrc);
									//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<"Evaluating !" << strsrc << "\n";
                                    if (!processing_level && (isnum(strsrc) || constants.isconstant(strsrc)))
                                        {
                                        double value = getnum<double>(strsrc,constants);
                                        //sources.push_back(FMToutputsource(FMTotar::val,value));
										//Make sure it is not a adition or substraction!!!!
										if (((!stroperators.empty() &&
											(stroperators.at(0)=="+" || stroperators.at(0)=="-")) ||
											(!lastoperator.empty() &&
											(lastoperator == "+" || lastoperator == "-"))) &&
											(find_if(sources.begin(), sources.end(),FMToutputsourcecomparator(true)) == sources.end()))
											{
											_exhandler->raise(FMTexc::WSunsupported_output, _section, name + " at line " + to_string(_line), __LINE__, __FILE__);
											}

										if (!lastoperator.empty())
											{
											//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<"OPPPPS "<< lastoperator << "\n";
											vector<FMToutputsource>newsources;
											vector<FMToperator>newoperators;
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
											for (; id < sources.size(); ++ id)
												{
												//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << string(sources.at(id)) << "\n";
												if (id > 0 && sources.at(id-1).isvariable())
													{
													if (sources.at(id).isconstant())
														{
														value=FMToperator(operators.at(lastop)).call(value, sources.at(id).getvalue());
													}else {
														//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "pushing lastop!" << "\n";
														newoperators.push_back(FMToperator(lastoperator));
														}
													//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "pushing source!" << "\n";
													newsources.push_back(FMToutputsource(FMTotar::val, value));
													}
												if (sources.at(id).isvariable() || sources.at(id).islevel())
													{
													newsources.push_back(sources.at(id));
													}
												if (id > 0)
													{
													newoperators.push_back(operators.at(lastop));
													++lastop;
													}
												}
											if (newsources.back().isvariable() || newsources.back().islevel())
												{
												newsources.push_back(FMToutputsource(FMTotar::val, value));
												//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "pushing last op "<< lastoperator << "\n";
												newoperators.push_back(FMToperator(lastoperator));
												}

											operators = newoperators;
											sources = newsources;
										}
										else {
											sources.push_back(FMToutputsource(FMTotar::val, value));

										}

                                        }else if(processing_level)
                                        {
                                        vector<double>values;
                                        if (constants.isconstant(strsrc))
                                            {
                                            for (size_t period = 0 ; period < constants.length(strsrc); ++period)
                                                {
                                                values.push_back(getnum<double>(strsrc,constants,period));
                                                }
                                            }else{
                                                //sflit and trim
                                                vector<string>all_numbers;// = spliter(rest,rxsources);
                                                boost::split(all_numbers,strsrc, boost::is_any_of(" /t"), boost::token_compress_on);
                                                for (const string& number : all_numbers)
                                                    {
                                                    values.push_back(getnum<double>(number,constants));
                                                    }
                                                }
                                        sources.push_back(FMToutputsource(FMTotar::level,values));//constant level!
                                        //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<"GOT LEVEL!!!" << strsrc<<" size of "<<values.size()  << "\n";
                                        //sources.push_back(FMToutputsource(FMTotar::level,0,"",strsrc));
                                        }else{
                                            vector<string>values = spliter(strsrc,FMTparser::rxseparator);
                                            if(values.size()==1)
                                            {
											//need to use get equation to simplify output!!!
                                            if (find_if(outputs.begin(),outputs.end(),FMToutputcomparator(strsrc))!=outputs.end())
                                                {
                                                vector<FMToutput>::iterator it = find_if(outputs.begin(),outputs.end(),FMToutputcomparator(strsrc));
												if (!it->islevel() || (it->islevel() && !it->getsources().empty()))
													{
														lastoutput = sources.size();
														for (const FMToutputsource& src : it->getsources())
															{
															sources.push_back(src);
															}
														lastopt = operators.size();
														for (const FMToperator& src : it->getopes())
															{
															operators.push_back(src);
															}
												}else {
													sources.push_back(FMToutputsource(FMTotar::level,0, strsrc));
													}
												if (!stroperators.empty())
													{
													operators.push_back(FMToperator(stroperators.front()));
													lastoperator = stroperators.front();
													stroperators.erase(stroperators.begin());
													}
                                                }else if (strsrc.find("#")!=string::npos)
													{
													_exhandler->raise(FMTexc::WSundefined_constant, _section, strsrc + " at line " + to_string(_line), __LINE__, __FILE__);
													}
												else if (ylds.isyld(strsrc))//isyld(ylds,strsrc,_section))
													{
													sources.push_back(FMToutputsource(FMTotar::timeyld,0,strsrc));

													}else{
													_exhandler->raise(FMTexc::WSundefined_output, _section, strsrc + " at line " + to_string(_line), __LINE__, __FILE__);
													}
                                            }else{
                                                string mask = "";
                                                string rest = " ";
                                                size_t id = 0;
                                                for(const string& value : values)
                                                    {
                                                    if (id < themes.size())
                                                        {
                                                        mask+= value+ " ";
                                                        }else{
                                                        rest+= value + " ";
                                                        }
                                                    ++id;
                                                    }
                                                mask = mask.substr(0, mask.size()-1);
                                                FMTspec spec;
                                                string inds = setspec(FMTwssect::Outputs,FMTwskwor::Source,ylds,constants,spec,rest);
                                                if (!spec.empty())
                                                    {
                                                    rest = inds;
                                                    }
                                                if (isvalid(rest))
                                                    {
                                                    if (regex_search(rest,kmatch,rxtar))
                                                        {
														//14 lockinv
                                                        if (!string(kmatch[25]).empty())
                                                            {
                                                            string action = string(kmatch[25]);
															if (actions_aggregate.find(action) == actions_aggregate.end())
																{
																isact(_section, actions, action);
																}
                                                            //string area = string(kmatch[28]);
                                                            string yld = string(kmatch[29]);
                                                            if (isvalid(yld))
                                                                        {
																		if (!ylds.isyld(yld))
																			{
																			_exhandler->raise(FMTexc::WSignore, _section,
																				yld + " at line " + to_string(_line), __LINE__, __FILE__);
																			}
                                                                       // isyld(ylds,yld,_section);
                                                                        }else{
                                                                        yld = "";
                                                                        }
                                                            sources.push_back(FMToutputsource(spec,FMTmask(mask,themes),
                                                                            FMTotar::actual,yld,action));
                                                            }else if(!string(kmatch[17]).empty() || !string(kmatch[18]).empty())
                                                                {
                                                                string invtype = string(kmatch[17]) + string(kmatch[18]);
                                                                string yld = string(kmatch[22]);
                                                                if (isvalid(yld))
                                                                        {
																		if (!ylds.isyld(yld))
																			{
																				_exhandler->raise(FMTexc::WSignore, _section,
																					yld + " at line " + to_string(_line), __LINE__, __FILE__);
																			}
                                                                        //isyld(ylds,yld,_section);
                                                                        }else{
                                                                        yld = "";
                                                                        }
                                                               // string area = string(kmatch[21]);
																string lockinv = kmatch[18];
																if (!lockinv.empty())
																	{
																	//set a lock bound in the spec???
																	int lower = 1;
																	int upper = numeric_limits<int>::max();
																	spec.addbounds(FMTlockbounds(FMTwssect::Outputs,
																		FMTwskwor::Source,upper,lower));
																	}
																//PySys_WriteStdout(lockinv.c_str());
                                                                //deal with the invlock in spec or here?
																//PySys_WriteStdout(area.c_str());
                                                                sources.push_back(FMToutputsource(spec,FMTmask(mask,themes),
                                                                            FMTotar::inventory,yld));
                                                                }else if(!string(kmatch[3]).empty())
                                                                    {
                                                                    string action = string(kmatch[7]);
																	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "action !" << action << "\n";
																	if (actions_aggregate.find(action) == actions_aggregate.end())
																		{
																		isact(_section, actions, action);
																		}
                                                                    string yld = string(kmatch[13]);
																	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "yield !" << yld << "\n";
                                                                    if (isvalid(yld))
                                                                        {
																		if (!ylds.isyld(yld))
																			{
																			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "NOT YIELD"<< "\n";
																			_exhandler->raise(FMTexc::WSignore, _section,
																				yld + " at line " + to_string(_line), __LINE__, __FILE__);
																			}
                                                                        //isyld(ylds,yld,_section);
                                                                        }else{
																		yld.clear();
                                                                        }
																	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "masking "<<mask << "\n";
                                                                    //string area = string(kmatch[12]);
                                                                    sources.push_back(FMToutputsource(spec,FMTmask(mask,themes),
                                                                            FMTotar::inventory,yld,action));

                                                                    }
                                                        }
                                                    }

                                                }
                                            }
                                        }

                                for(const string& strope : stroperators)
                                    {
                                    operators.push_back(FMToperator(strope));
                                    }
                                insource = true;

                                }

                        }
                    }
				if (!sources.empty() || (processing_level && !insource))
					{
                    if (processing_level && sources.empty())
                        {
                        sources.push_back(FMToutputsource(FMTotar::level,0,"",name));
                        }
					outputs.push_back(FMToutput(name, description, themetarget, sources, operators));
					}
                }
            return outputs;
            }
        bool FMToutputparser::write(const vector<FMToutput>& outputs,string location)
            {
            ofstream outputstream;
            outputstream.open(location);
            if (tryopening(outputstream,location))
                {
                for(const FMToutput& out : outputs)
                    {
                    outputstream<<string(out)<<"\n";
                    }
                outputstream.close();
                return true;
                }
            return false;
            }

}
