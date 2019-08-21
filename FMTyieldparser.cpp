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

#include "FMTyieldparser.h"

namespace WSParser{


FMTyieldparser::FMTyieldparser():FMTparser(),
    rxyieldtype(regex("^(\\*)([^\\s^\\t]*)([\\s\\t]*)(.+)(_OVERRIDE)|^(\\*)([^\\s^\\t]*)([\\s\\t]*)(.+)",regex_constants::ECMAScript|regex_constants::icase)),
    rxcomplex(regex("^([^\\s^\\t]*)([\\s\\t]*)((_RANGE)|(_MULTIPLY)|(_SUM)|(_SUBTRACT)|(_YTP)|(_MAI)|(_CAI)|(_DIVIDE)|(_EQUATION))([\\s\\t]*)(\\()(.+)(\\))",regex_constants::ECMAScript|regex_constants::icase)),
	rxeqs(regex("([\\(\\)\\-\\+\\*\\/]*)([^\\(\\)\\-\\+\\*\\/]*)"))
        {

        }

FMTyieldparser::FMTyieldparser(const FMTyieldparser& rhs) : FMTparser(rhs),rxyieldtype(rhs.rxyieldtype),rxcomplex(rhs.rxcomplex), rxeqs(rhs.rxeqs)
    {

    }
FMTyieldparser& FMTyieldparser::operator = (const FMTyieldparser& rhs)
    {
    if(this!=&rhs)
        {
        FMTparser::operator = (rhs);
        rxyieldtype = rhs.rxyieldtype;
        rxcomplex = rhs.rxcomplex;
		rxeqs = rhs.rxeqs;
        }
    return *this;
    }
FMTyldwstype FMTyieldparser::getyldtype(const string& value) const
    {
    if (value == "Y")
        {
        return FMTyldwstype::FMTageyld;
        }else if(value == "YT")
            {
            return FMTyldwstype::FMTtimeyld;
            }else if(value == "YC")
                {
                return FMTyldwstype::FMTcomplexyld;
                }else{
                _exhandler->raise(FMTexc::WSinvalid_yield,_section," at line " + to_string(_line), __LINE__, __FILE__);
                }
    return FMTyldwstype::FMTageyld;
    }

FMTyieldparserop FMTyieldparser::getyldctype(const string& value) const
    {
    if (value == "_RANGE")
        {
        return FMTyieldparserop::FMTwsrange;
        }else if(value == "_MULTIPLY")
            {
            return FMTyieldparserop::FMTwsmultiply;
            }else if(value == "_SUM")
                {
                return FMTyieldparserop::FMTwssum;
                }else if(value == "_SUBTRACT")
                    {
                    return FMTyieldparserop::FMTwssubstract;
                    }else if(value == "_YTP")
                        {
                        return FMTyieldparserop::FMTwsytp;
                        }else if(value == "_MAI")
                            {
                            return FMTyieldparserop::FMTwsmai;
                            }else if(value == "_CAI")
                                {
                                return FMTyieldparserop::FMTwscai;
                                }else if(value == "_DIVIDE")
                                    {
                                    return FMTyieldparserop::FMTwsdivide;
								}else if (value == "_EQUATION")
									{
									return FMTyieldparserop::FMTwsequation;
								}/*else if (value == "_DISCOUNTFACTOR")
									{
										return FMTyieldparserop::FMTwsdiscountfactor;
									}*/else{
                                        _exhandler->raise(FMTexc::WSinvalid_yield,_section," at line " + to_string(_line), __LINE__, __FILE__);
                                        }
    return FMTyieldparserop::FMTwsnone;
    }
vector<string> FMTyieldparser::getylduse(FMTyields& yielddata,
                                   vector<FMTyieldhandler>::iterator actualyield,
                                   const vector<string>& values) const
    {
    vector<string>dump;
    vector<FMTyieldhandler>::const_iterator it = yielddata.databegin();
	vector<FMTmask>::const_iterator actual_msk = yielddata.maskbegin() + std::distance(yielddata.databegin(),actualyield);
	vector<FMTmask>::const_iterator mskit = yielddata.maskbegin();
    while(it!=actualyield)
        {
		if (mskit->data.is_subset_of(actual_msk->data) ||
			actual_msk->data.is_subset_of(mskit->data) ||
			mskit->data == actual_msk->data)
			{
			vector<string> pyld = it->compare(values);
			for (const string& name : values)
				{
				if (find(pyld.begin(), pyld.end(), name) != pyld.end()
					&& find(dump.begin(), dump.end(), name) == dump.end())
					{
					dump.push_back(name);
					}
				}
			}
        ++it;
		++mskit;
        }
    return dump;
    }
void FMTyieldparser::checkpreexisting(const vector<string>& preexists) const
    {
    if (!preexists.empty())
        {
        for (const string& yl : preexists)
            {
            _exhandler->raise(FMTexc::WSpreexisting_yield,_section,yl+" at line "+ to_string(_line), __LINE__, __FILE__);
            }
        }
    }

bool FMTyieldparser::isfunction(const string& strfunction) const
	{
	vector<string>cplxf = {"EXP","LN"};
	if (std::find(cplxf.begin(), cplxf.end(), strfunction)!= cplxf.end())
		{
		return true;
		}
	return false;
	}

FMTdata FMTyieldparser::geteq(const string& basestr,
	const FMTconstants& constants, const FMTyields& ylds,
	const vector<FMTtheme>& themes)
	{
		vector<string> valuesnoperators;
		vector<double>numbers;
		smatch kmatch;
		//string value,opstr;
		//size_t endsize;
		//string nbase = basestr;
        boost::char_separator<char>separators("","+-*/()^");
        boost::tokenizer<boost::char_separator<char>>tokens(basestr,separators);
        for (string token : tokens)
            {
            boost::trim(token);
            if (isvalid(token))
				{
                //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "token of:  " <<"|"<< token<<"|" << "\n";
                //cin.get();
                double number = 0;
                string source_value;
                if (isnum(token) || constants.isconstant(token))
					{
					number = getnum<double>(token, constants);
					}else if (!FMToperator(token).valid() && !FMTfunctioncall(token).valid() && token != "(" && token != ")")
						{
                        vector<string>yldss = ylds.getyldsnames(); //not so optimzed
						if (find(yldss.begin(), yldss.end(), token) == yldss.end())
							{
                            bool should_throw = true;
                            for (const FMTtheme& theme : themes)
                                {
                                if (theme.isindex(token))
                                    {
                                    should_throw = false;
                                    //decompose the main mask! by theme index
                                    break;
                                    }
                                }
                            if (should_throw)
                                {
                                _exhandler->raise(FMTexc::WSinvalid_yield, _section, token + " at line " + to_string(_line), __LINE__, __FILE__);
                                }
							}
						source_value = token;
                        }else {
						source_value = token;
						}
                valuesnoperators.push_back(source_value);
				numbers.push_back(number);
                }


            }

		/*while (!nbase.empty())
		{
		    Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "testing:  " <<"|"<< nbase<<"|" << "\n";
			bool didmatch = regex_search(nbase, kmatch, rxeqs);
			value = string(kmatch[2]);
			Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "matching:  " << didmatch<<" " <<"first |"<<value<<"| last |"<<string(kmatch[1])<< "|\n";
			endsize = (string(kmatch[0]).size());
			Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<"end size "<< string(kmatch[0])<< "\n";
            cin.get();
			boost::trim(value);
			if (isvalid(value))
				{
				string source_value = "";
				double number = 0;
				if (isnum(value) || constants.isconstant(value))
					{
					number = getnum<double>(value, constants);
					}else if (!isfunction(value))
						{
						vector<string>yldss = ylds.getyldsnames(); //not so optimzed
						if (find(yldss.begin(), yldss.end(), value) == yldss.end())
							{
                            bool should_throw = true;
                            for (const FMTtheme& theme : themes)
                                {
                                if (theme.isindex(value))
                                    {
                                    should_throw = false;
                                    //decompose the main mask! by theme index
                                    break;
                                    }
                                }
                            if (should_throw)
                                {
                                _exhandler->raise(FMTexc::WSinvalid_yield, _section, value + " at line " + to_string(_line), __LINE__, __FILE__);
                                }
							}
						source_value = value;
					}else {
						source_value = value;
						}
				valuesnoperators.push_back(source_value);
				numbers.push_back(number);
				}
			opstr = string(kmatch[1]);
			boost::trim(opstr);
			if (isvalid(opstr))
				{
				for (const char& strchar : opstr)
					{
					string nval="";
					nval += strchar;
					valuesnoperators.push_back(nval);
					numbers.push_back(0);
					}

				}
			endsize = (string(kmatch[0]).size());
			nbase.erase(nbase.begin(), nbase.begin() + endsize);
		}*/
		return FMTdata(numbers,FMTyieldparserop::FMTwsequation,valuesnoperators);
	}


FMTyields FMTyieldparser::read(const vector<FMTtheme>& themes,const FMTconstants& constants,string location)
    {
    ifstream yieldstream(location);
    string line;
    vector<string>yldsnames;
    vector<string>dump;
    vector<FMTyieldhandler>::iterator actualyield;
    //map<FMTmask,vector<FMTyieldhandler>>datas;
    FMTyields yields;
	vector<FMTmask>::iterator maskit = yields.maskend();
	vector<size_t>indexed_yields;
	vector<FMTyieldhandler>::iterator datait = yields.dataend();
    FMTmask tmask;
    bool sided = false;
    if (FMTparser::tryopening(yieldstream,location))
        {
        while(yieldstream.is_open())
            {
            //line = FMTparser::getcleanline(yieldstream);
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << location<<" "<<themes.size() << "\n";
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << _line << " |" << line << "\n";
			line = FMTparser::getcleanlinewfor(yieldstream, themes, constants);
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << "done line" << "\n";

			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << "ILINE  " << line<< "\n";
            if (!line.empty())
                {
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << "testing with  " << line << "\n";
                smatch kmatch;
                if (regex_search(line,kmatch,rxyieldtype))
                    {
                    string yieldtype = string(kmatch[2])+string(kmatch[7]);
                    //string mask = string(kmatch[2])+string(kmatch[9]);
					string mask = string(kmatch[4]) + string(kmatch[9]);
					boost::trim(mask);
                    bool overyld = false;
                    sided = true;
                    if(!string(kmatch[5]).empty())
                        {
                        overyld = true;
                        }

                    if (!validate(themes,mask)) continue;
                    FMTyldwstype yldtype = getyldtype(yieldtype);
                    tmask = FMTmask(mask,themes);
                    FMTyieldhandler newyield(yldtype,tmask);
					if (!overyld)
						{
						yields.push_back(tmask, newyield);
						maskit = --yields.maskend();
						datait = --yields.dataend();
						}else {
						//_exhandler->raise(FMTexc::WSunsupported_yield, _section, line + " at line " + to_string(_line), __LINE__, __FILE__);
						yields.push_front(tmask, newyield);
						maskit = yields.maskbegin();
						datait = yields.databegin();
						}

					actualyield = datait;//(datait->end() - 1);
                    }else{
                            vector<string>values;
                            boost::split(values,line,boost::is_any_of("\t "),boost::token_compress_on);
                            if(actualyield->gettype() == FMTyldwstype::FMTageyld)
                                {
                                if (values[0]=="_AGE")
                                    {
                                    sided = false;
                                    yldsnames.clear();
                                    dump.clear();
                                    values.erase(values.begin());
                                    yldsnames = values;
                                    /*if (datait->begin()!=actualyield)
                                        {*/
                                        dump = getylduse(yields,actualyield,values);
                                        checkpreexisting(dump);
                                       // }
                                    }else if(!sided)
                                        {
										int newbase = getnum<int>(values[0], constants);
										const vector<int>& bases = actualyield->getbases();
										if (std::find(bases.begin(), bases.end(), newbase)==bases.end())
											{
											actualyield->push_base(newbase);
											}
                                        values.erase(values.begin());
                                        int id = 0;
                                        for(const string& value : values)
                                            {
                                            /*if (find(dump.begin(),dump.end(),yldsnames[id])==dump.end())
                                                {*/
                                                actualyield->push_data(yldsnames[id],getnum<double>(value,constants));
                                             //   }
                                            ++id;
                                            }
                                        }else{
                                            vector<string>tyld = {values[0]};
                                            values.erase(values.begin());
                                            /*if (datait->begin() != actualyield)
                                                {*/
                                            dump = getylduse(yields,actualyield,tyld);
                                            checkpreexisting(dump);
                                             //   }
                                            if (actualyield->elements.empty())
                                                {
                                                actualyield->push_base(getnum<int>(values[0],constants));
                                                }
                                            actualyield->push_data(tyld[0],getnum<double>(values[1],constants));
                                        }

                                }else if(actualyield->gettype() == FMTyldwstype::FMTtimeyld)
                                    {

									if (values[0] == "_CP")
										{
										sided = false;
										yldsnames.clear();
										dump.clear();
										values.erase(values.begin());
										yldsnames = values;
										/*if (datait->begin() != actualyield)
											{*/
											dump = getylduse(yields, actualyield, values);
											checkpreexisting(dump);
										//	}
										}
									else if (!sided)
										{
										if (actualyield->elements.empty())
											{
											actualyield->push_base(getnum<int>(values[0], constants));
											values.erase(values.begin());
											int id = 0;
											for (const string& value : values)
												{
												/*if (find(dump.begin(), dump.end(), yldsnames[id]) == dump.end())
													{*/
													actualyield->push_data(yldsnames[id], getnum<double>(value, constants));
												//	}
												++id;
												}
											}else{
												int newbase = getnum<int>(values[0], constants);
												values.erase(values.begin());
												for (int base = actualyield->getlastbase(); base <= newbase;++base)
													{
													actualyield->push_base(base);
													int id = 0;
													for (const string& value : values)
														{
														double thevalue = actualyield->getlastvalue(yldsnames[id]);
														if (base == newbase)
															{
															thevalue = getnum<double>(value, constants);
															}
														actualyield->push_data(yldsnames[id], thevalue);
														++id;
														}
													}
												}


										}else {
											if (isnum(values[0]))
												{
												for (const string& value : values)
													{
													actualyield->push_data(yldsnames.back() , getnum<double>(value, constants));
													}
											}else{


											string yldname = values[0];
											//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << yldname << "\n";
											yldsnames.push_back(values[0]);
											dump.clear();
											/*if (datait->begin() != actualyield)
												{*/
												dump = getylduse(yields, actualyield, values);
												checkpreexisting(dump);
											//	}
											//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << yldname <<" "<< dump.empty() <<"\n";
											/*if (dump.empty())
											{*/
												values.erase(values.begin());
												actualyield->push_base(getnum<int>(values[0], constants));
												values.erase(values.begin());
												for (const string& value : values)
												{
													actualyield->push_data(yldname, getnum<double>(value, constants));
												}
											//}
											}
											}


									}


                                    else if(actualyield->gettype() == FMTyldwstype::FMTcomplexyld)
                                        {
                                        smatch kmatch;
										//Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << "MI COMPLEX?  " << line << "\n";
										//Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << "IN COMPLEX?  " << regex_search(line, kmatch, rxcomplex) << "\n";
										size_t should_be_equation = line.find_first_of("+-*/");
										bool simple_match = regex_search(line, kmatch, rxcomplex);
										if (simple_match || should_be_equation > 0)
                                            {
											string yldname;
											string cyld;
											string data;
											if (!simple_match && should_be_equation > 0)
												{
												vector<string>wrong_equation;
												boost::split(wrong_equation, line, boost::is_any_of("\t "), boost::token_compress_on);
												yldname = wrong_equation[0];
												cyld = "_EQUATION";
												for (size_t cid = 1; cid < wrong_equation.size();++cid)
													{
													data += wrong_equation.at(cid);
													}
												boost::trim(data);
											}else {
												yldname = kmatch[1];
												for (int id = 4; id < 13; ++id) //12 - > 13
													{
													cyld += string(kmatch[id]);
													}
												data = kmatch[15];
												}
                                           // Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) <<"1|"<< data<<"|" << "\n";
                                            dump.clear();
                                            vector<string>theylds = {yldname};
                                            dump = getylduse(yields,actualyield,theylds);
                                            checkpreexisting(dump);
                                            FMTyieldparserop complextype = getyldctype(cyld);

                                            vector<string>values;
											boost::trim_if(data, boost::is_any_of("\t "));
											//Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) <<"2|"<< data<<"|" << "\n";
                                            boost::split(values,data,boost::is_any_of("\t ,"), boost::token_compress_on);
											//Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << "ITTEREE: " << "\n";
                                            vector<double>cvalues;
                                            vector<string>csource;
											if (complextype == FMTyieldparserop::FMTwsequation)
											{
												//csource = valuesandoperators(data,constants,yields);
												actualyield->push_data(yldname, geteq(data,constants,yields,themes));
												//_exhandler->raise(FMTexc::WSignore, _section, " NOT IMPLEMENTED! at line " + to_string(_line), __LINE__, __FILE__);
											}else{
												vector<bool>stacking;
												for(size_t id = 0 ; id < values.size(); ++id)
													{
													//Need to keep ordering clear here!!!!
													if (isnum(values[id]) || constants.isconstant(values[id]))
														{
                                                        //Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) <<"getting|"<< values[id]<<"|" << "\n";
														double value = getnum<double>(values[id],constants);
														cvalues.push_back(value);
														stacking.push_back(false);
													}
													else if (values[id].find("#") != string::npos)
													{
														_exhandler->raise(FMTexc::WSundefined_constant, _section, values[id] + " at line " + to_string(_line), __LINE__, __FILE__);
													}else {
														vector<string>ylds = yields.getyldsnames(); //not so optimzed
														if (find(ylds.begin(),ylds.end(),values[id])==ylds.end())
															{
															_exhandler->raise(FMTexc::WSignore, _section,
																values[id] + " at line " + to_string(_line), __LINE__, __FILE__);
															//continue;//Mettre warning!!!!!!!
															//_exhandler->raise(FMTexc::WSinvalid_yield,_section,values[id]+" at line " + to_string(_line), __LINE__, __FILE__);
															}
														stacking.push_back(true);
														csource.push_back(values[id]);
														}
													}
											actualyield->push_data(yldname, FMTdata(cvalues, complextype, csource,stacking));
											}

											//Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << "OPUTTTT " << "\n";
                                            }else{
                                            _exhandler->raise(FMTexc::WSunsupported_yield,_section,line+" at line "+ to_string(_line), __LINE__, __FILE__);
                                            }
                                        }
                        }
                }
            }

        //iterate on all yieldhandler if equation with index then take the handler
        //delete the handler at it's yields location
        //decompose and insert all new handlers
        vector<FMTyieldhandler>::iterator handler_it = yields.databegin();
        vector<string>yldnames = yields.getyldsnames();
        while (handler_it!=yields.dataend())
            {
            vector<string>indexvalues = handler_it->indexes(yldnames);
            if (!indexvalues.empty())
                {
               // Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "index uses " << string(*handler_it) << "\n";
                size_t location = std::distance(yields.databegin(),handler_it);
                FMTmask oldmask = *(yields.maskbegin() + location); //copy
                FMTyieldhandler oldhandler = *handler_it; //copy
                yields.erase(location);
                vector<FMTmask>todecompose;
                todecompose.push_back(oldmask);
                vector<FMTtheme>themes_windex;
                for (const FMTtheme& theme: themes)
                    {
                    if (theme.useindex())
                        {
                        for (const string& index : indexvalues)
                            {
                            if (theme.isindex(index))
                                {
                                vector<FMTmask>newdecomposer;
                                while(!todecompose.empty())
                                    {
                                    vector<FMTmask>allmasks = (todecompose.front()).decompose(theme);
                                    todecompose.erase(todecompose.begin());
                                    newdecomposer.insert(newdecomposer.end(),allmasks.begin(),allmasks.end());
                                    }
                                todecompose=newdecomposer;
                                themes_windex.push_back(theme);
                                break;
                                }
                            }
                        }
                    }
                for (const FMTmask& newmask : todecompose)
                    {
                    FMTyieldhandler newhandler(FMTyldwstype::FMTcomplexyld,newmask);
                    map<string,double>handler_values;
                    for (const FMTtheme& theme: themes_windex)
                        {
                        string attribute = newmask.get(theme);
                        for (const string& index : indexvalues)
                            {
                            if (theme.isindex(index))
                                {
                                handler_values[index] = theme.getindex(attribute,index);
                                //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "got values " << handler_values[index] << "\n";
                                }
                            }
                        }
                    map<string,FMTdata>alladata = oldhandler.getdataelements();
                    for (map<string,FMTdata>::const_iterator datait = alladata.begin(); datait != alladata.end(); datait++)
                        {
                        FMTexpression expression = datait->second.toexpression();
                        vector<string>allvalues = expression.getinfix();
                        vector<double>numbers(allvalues.size(),0);
                        vector<string>valuesnoperators(allvalues.size());
                        size_t data_loc = 0;
                        for (string& source : allvalues)
                            {
                            if (handler_values.find(source)!= handler_values.end())
                                {
                                numbers[data_loc]= handler_values.at(source);
                                }else if (isnum(source))
                                    {
                                    numbers[data_loc]= stod(source);
                                    }else{
                                    valuesnoperators[data_loc] = source;
                                    }
                            ++data_loc;
                            }
                        newhandler.push_data(datait->first,FMTdata(numbers,FMTyieldparserop::FMTwsequation,valuesnoperators));
                        }
                    //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "new handler " << string(newhandler) << "\n";
                    yields.insert(location,newmask,newhandler);
                    ++location;
                    }
                handler_it = (yields.databegin() + location);
                //cin.get();
                }else{
                ++handler_it;
                }
            }
        }
    yields.update();
    return yields;
    }

bool FMTyieldparser::write(const FMTyields& yields, string location)
    {
    ofstream yieldstream;
    yieldstream.open(location);
    if (tryopening(yieldstream,location))
        {
        const vector<string>stackedyields = yields.getstacked();
        for (const string& val : stackedyields)
            {
            yieldstream<<string(val);//
            }
        yieldstream.close();
        return true;
        }
    return false;
    }
}
