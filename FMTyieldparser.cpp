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

namespace Parser{


FMTyieldparser::FMTyieldparser():FMTparser(),
    rxyieldtype("^(\\*Y)([^\\s^\\t]*)([\\s\\t]*)(.+)(_OVERRIDE)|^(\\*Y)([^\\s^\\t]*)([\\s\\t]*)(.+)", std::regex_constants::ECMAScript| std::regex_constants::icase),
    rxcomplex("^([^\\s^\\t]*)([\\s\\t]*)((_RANGE)|(_MULTIPLY)|(_SUM)|(_SUBTRACT)|(_YTP)|(_MAI)|(_CAI)|(_DIVIDE)|(_EQUATION)|(_ENDPOINT))([\\s\\t]*)(\\()(.+)(\\))", std::regex_constants::ECMAScript| std::regex_constants::icase),
	rxeqs("([\\(\\)\\-\\+\\*\\/]*)([^\\(\\)\\-\\+\\*\\/]*)"),
	rxdiscount("^(_DISCOUNTFACTOR)(\\()([\\s\\t]*[\\d]*)([^,]*)(,)([^,]*)(,)([\\s\\t]*(NONE|HALF|FULL)[\\s\\t]*)(\\))")
        {

        }

FMTyieldparser::FMTyieldparser(const FMTyieldparser& rhs) : FMTparser(rhs),rxyieldtype(rhs.rxyieldtype),rxcomplex(rhs.rxcomplex), rxeqs(rhs.rxeqs), rxdiscount(rhs.rxdiscount)
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
		rxdiscount = rhs.rxdiscount;
        }
    return *this;
    }
Core::FMTyldtype FMTyieldparser::getyldtype(const std::string& value) const
    {
    if (value == "Y")
        {
        return Core::FMTyldtype::FMTageyld;
        }else if(value == "YT")
            {
            return Core::FMTyldtype::FMTtimeyld;
            }else if(value == "YC")
                {
                return Core::FMTyldtype::FMTcomplexyld;
                }else{
                _exhandler->raise(Exception::FMTexc::FMTinvalid_yield,_section," at line " + std::to_string(_line), __LINE__, __FILE__);
                }
    return Core::FMTyldtype::FMTageyld;
    }

Core::FMTyieldparserop FMTyieldparser::getyldctype(const std::string& value) const
    {
    if (value == "_RANGE")
        {
        return Core::FMTyieldparserop::FMTrange;
        }else if(value == "_MULTIPLY")
            {
            return Core::FMTyieldparserop::FMTmultiply;
            }else if(value == "_SUM")
                {
                return Core::FMTyieldparserop::FMTsum;
                }else if(value == "_SUBTRACT")
                    {
                    return Core::FMTyieldparserop::FMTsubstract;
                    }else if(value == "_YTP")
                        {
                        return Core::FMTyieldparserop::FMTytp;
                        }else if(value == "_MAI")
                            {
                            return Core::FMTyieldparserop::FMTmai;
                            }else if(value == "_CAI")
                                {
                                return Core::FMTyieldparserop::FMTcai;
                                }else if(value == "_DIVIDE")
                                    {
                                    return Core::FMTyieldparserop::FMTdivide;
								}else if (value == "_EQUATION")
									{
									return Core::FMTyieldparserop::FMTequation;
								}else if (value == "_ENDPOINT")
									{
										return Core::FMTyieldparserop::FMTendpoint;
									}else{
                                        _exhandler->raise(Exception::FMTexc::FMTinvalid_yield,_section," at line " + std::to_string(_line), __LINE__, __FILE__);
                                        }
    return Core::FMTyieldparserop::FMTnone;
    }
std::vector<std::string> FMTyieldparser::getylduse(Core::FMTyields& yielddata,
	std::vector<std::pair<Core::FMTmask, Core::FMTyieldhandler>>::iterator actualyield,
                                   const std::vector<std::string>& values) const
    {
	std::vector<std::string>dump;
	std::vector<std::pair<Core::FMTmask,Core::FMTyieldhandler>>::const_iterator it = yielddata.begin();
	const Core::FMTmask& actual_msk = (yielddata.begin() + std::distance(yielddata.begin(), actualyield))->first;
    while(it!=actualyield)
        {
		if (it->first.issubsetof(actual_msk) ||
			actual_msk.issubsetof(it->first) ||
			it->first == actual_msk)
			{
			const std::vector<std::string> pyld = it->second.compare(values);
			for (const std::string& name : values)
				{
				if (std::find(pyld.begin(), pyld.end(), name) != pyld.end()
					&& std::find(dump.begin(), dump.end(), name) == dump.end())
					{
					dump.push_back(name);
					}
				}
			}
        ++it;
        }
    return dump;
    }
void FMTyieldparser::checkpreexisting(const std::vector<std::string>& preexists) const
    {
    if (!preexists.empty())
        {
        for (const std::string& yl : preexists)
            {
            _exhandler->raise(Exception::FMTexc::FMTpreexisting_yield,_section,yl+" at line "+ std::to_string(_line), __LINE__, __FILE__);
            }
        }
    }

double FMTyieldparser::getnumwithproportion(const std::string& value,const Core::FMTconstants& constants,
	const std::vector<double>& proportions, const int& location)
	{
	double factor = 1;
	if (!proportions.empty())
		{
		factor = proportions.at(std::min(static_cast<int>(proportions.size()) - 1, location));
		}
	return getnum<double>(value, constants)*factor;
	}

bool FMTyieldparser::isfunction(const std::string& strfunction) const
	{
	const std::vector<std::string>cplxf = {"EXP","LN"};
	if (std::find(cplxf.begin(), cplxf.end(), strfunction)!= cplxf.end())
		{
		return true;
		}
	return false;
	}

Core::FMTdata FMTyieldparser::geteq(const std::string& basestr,
	const Core::FMTconstants& constants, const Core::FMTyields& ylds,
	const std::vector<Core::FMTtheme>& themes)
	{
		std::vector<std::string> valuesnoperators;
		std::vector<double>numbers;
		std::smatch kmatch;
        const boost::char_separator<char>separators("","+-*/()^");
        const boost::tokenizer<boost::char_separator<char>>tokens(basestr,separators);
        for (std::string token : tokens)
            {
            boost::trim(token);
            if (isvalid(token))
				{
                double number = 0;
				std::string source_value;
                if (isnum(token) || constants.isconstant(token))
					{
					number = getnum<double>(token, constants);
					}else if (!Core::FMToperator(token).valid() && !Core::FMTfunctioncall(token).valid() && token != "(" && token != ")")
						{
						const std::vector<std::string>yldss = ylds.getyldsnames();
						if (find(yldss.begin(), yldss.end(), token) == yldss.end())
							{
                            bool should_throw = true;
                            for (const Core::FMTtheme& theme : themes)
                                {
                                if (theme.isindex(token))
                                    {
                                    should_throw = false;
                                    break;
                                    }
                                }
                            if (should_throw)
                                {
                                _exhandler->raise(Exception::FMTexc::FMTinvalid_yield, _section, token + " at line " + std::to_string(_line), __LINE__, __FILE__);
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
		return Core::FMTdata(numbers, Core::FMTyieldparserop::FMTequation,valuesnoperators);
	}


Core::FMTyields FMTyieldparser::read(const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& constants,const std::string& location)
    {
	Core::FMTyields yields;
	try {
		std::ifstream yieldstream(location);
		std::vector<std::string>yldsnames;
		std::vector<std::string>dump;
		std::vector<std::pair<Core::FMTmask, Core::FMTyieldhandler>>::iterator actualyield;
		std::vector<size_t>indexed_yields;
		std::vector<std::pair<Core::FMTmask, Core::FMTyieldhandler>>::iterator datait = yields.end();
		Core::FMTmask tmask;
		bool sided = false;
		std::vector<double>proportion;
		if (FMTparser::tryopening(yieldstream, location))
		{
			while (yieldstream.is_open())
			{
				std::string line = FMTparser::getcleanlinewfor(yieldstream, themes, constants);
				if (!line.empty())
				{
					std::smatch kmatch;
					if (std::regex_search(line, kmatch, rxyieldtype))
					{
						const std::string yieldtype = "Y" + std::string(kmatch[2]) + std::string(kmatch[7]);
						std::string mask = std::string(kmatch[4]) + std::string(kmatch[9]);
						boost::trim(mask);
						bool overyld = false;
						sided = true;
						if (!std::string(kmatch[5]).empty())
						{
							overyld = true;
						}

						if (!validate(themes, mask, " at line " + std::to_string(_line))) continue;
						Core::FMTyldtype yldtype = getyldtype(yieldtype);
						tmask = Core::FMTmask(mask, themes);
						Core::FMTyieldhandler newyield(yldtype, tmask);
						if (!overyld)
						{
							yields.push_back(tmask, newyield);
							datait = --yields.end();
						}
						else {
							yields.push_front(tmask, newyield);
							datait = yields.begin();
						}
						actualyield = datait;
						proportion.clear();
					}
					else {
						std::vector<std::string>values;
						boost::split(values, line, boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
						if ((actualyield->second.gettype() == Core::FMTyldtype::FMTageyld ||
							actualyield->second.gettype() == Core::FMTyldtype::FMTtimeyld) && values[0] == "*P")
						{
							values.erase(values.begin());
							proportion.clear();
							//size_t proportion_id = 0;
							for (const std::string& value : values)
							{
								proportion.push_back(getnum<double>(value) / 100.0);
							}
							continue;
						}
						if (actualyield->second.gettype() == Core::FMTyldtype::FMTageyld)
						{
							if (values[0] == "_AGE")
							{
								sided = false;
								yldsnames.clear();
								dump.clear();
								values.erase(values.begin());
								yldsnames = values;
								dump = getylduse(yields, actualyield, values);
								checkpreexisting(dump);
							}
							else if (!sided)
							{
								const int newbase = getnum<int>(values[0], constants);
								const std::vector<int>& bases = actualyield->second.getbases();
								if (std::find(bases.begin(), bases.end(), newbase) == bases.end())
								{
									actualyield->second.push_base(newbase);
								}
								values.erase(values.begin());
								int id = 0;
								for (const std::string& value : values)
								{
									actualyield->second.push_data(yldsnames[id], getnumwithproportion(value, constants, proportion, id));
									++id;
								}
							}
							else {
								std::vector<std::string>tyld = { values[0] };
								values.erase(values.begin());
								dump = getylduse(yields, actualyield, tyld);
								checkpreexisting(dump);
								if (actualyield->second.elements.empty())
								{
									actualyield->second.push_base(getnum<int>(values[0], constants));
								}
								const int location = static_cast<int>(actualyield->second.elements.size());
								actualyield->second.push_data(tyld[0], getnumwithproportion(values[1], constants, proportion, location));
							}

						}
						else if (actualyield->second.gettype() == Core::FMTyldtype::FMTtimeyld)
						{

							if (values[0] == "_CP")
							{
								sided = false;
								yldsnames.clear();
								dump.clear();
								values.erase(values.begin());
								yldsnames = values;
								dump = getylduse(yields, actualyield, values);
								checkpreexisting(dump);
							}
							else if (!sided)
							{
								if (actualyield->second.elements.empty())
								{
									actualyield->second.push_base(getnum<int>(values[0], constants));
									values.erase(values.begin());
									int id = 0;
									for (const std::string& value : values)
									{
										actualyield->second.push_data(yldsnames[id], getnumwithproportion(value, constants, proportion, id));
										++id;
									}
								}
								else {
									const int newbase = getnum<int>(values[0], constants);
									values.erase(values.begin());
									for (int base = actualyield->second.getlastbase(); base <= newbase; ++base)
									{
										actualyield->second.push_base(base);
										int id = 0;
										for (const std::string& value : values)
										{
											double thevalue = actualyield->second.getlastvalue(yldsnames[id]);
											if (base == newbase)
											{
												thevalue = getnum<double>(value, constants);
											}
											actualyield->second.push_data(yldsnames[id], thevalue);
											++id;
										}
									}
								}


							}
							else {
								if (isnum(values[0]))
								{
									int id = 0;
									for (const std::string& value : values)
									{
										actualyield->second.push_data(yldsnames.back(), getnumwithproportion(value, constants, proportion, id));
										++id;
									}
								}
								else {


									const std::string yldname = values[0];
									yldsnames.push_back(values[0]);
									dump.clear();
									dump = getylduse(yields, actualyield, values);
									checkpreexisting(dump);
									values.erase(values.begin());
									//Need to check if rest of values is a _discountfactor!!!
									const std::string joinedvalues = boost::algorithm::join(values, "");
									std::smatch discountmatch;
									if (std::regex_search(joinedvalues, discountmatch, rxdiscount))
									{
										std::vector<double>yielddata;
										std::vector<std::string>sources;
										std::vector<bool>stacking;
										const std::string percentage = boost::trim_copy(std::string(discountmatch[3]));
										double valueper = getnum<double>(percentage, constants);
										const std::string dopercentage = std::string(discountmatch[4]);
										if (dopercentage.find("%") != std::string::npos)
										{
											valueper /= 100;
										}
										yielddata.push_back(valueper);
										stacking.push_back(false);
										std::string yperperiod = boost::trim_copy(std::string(discountmatch[6]));
										yielddata.push_back(getnum<double>(yperperiod, constants));
										stacking.push_back(false);
										sources.push_back(boost::trim_copy(std::string(discountmatch[9])));
										stacking.push_back(true);
										actualyield->second.push_data(yldname, Core::FMTdata(yielddata, Core::FMTyieldparserop::FMTdiscountfactor, sources, stacking));
									}
									else {
										actualyield->second.push_base(getnum<int>(values[0], constants));
										values.erase(values.begin());
										int id = 0;
										for (const std::string& value : values)
										{
											actualyield->second.push_data(yldname, getnumwithproportion(value, constants, proportion, id));
											++id;
										}
									}
								}
							}


						}


						else if (actualyield->second.gettype() == Core::FMTyldtype::FMTcomplexyld)
						{
							std::smatch kmatch;
							const size_t should_be_equation = line.find_first_of("+-*/");
							bool simple_match = std::regex_search(line, kmatch, rxcomplex);
							if (simple_match || should_be_equation > 0)
							{
								std::string yldname;
								std::string cyld;
								std::string data;
								if (!simple_match && should_be_equation > 0)
								{
									std::vector<std::string>wrong_equation;
									boost::split(wrong_equation, line, boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
									yldname = wrong_equation[0];
									cyld = "_EQUATION";
									for (size_t cid = 1; cid < wrong_equation.size(); ++cid)
									{
										data += wrong_equation.at(cid);
									}
									boost::trim(data);
								}
								else {
									yldname = kmatch[1];
									for (int id = 4; id < 14; ++id) //12 - > 13
									{
										cyld += std::string(kmatch[id]);
									}
									data = kmatch[16];
								}
								dump.clear();
								const std::vector<std::string>theylds = { yldname };
								dump = getylduse(yields, actualyield, theylds);
								checkpreexisting(dump);
								const Core::FMTyieldparserop complextype = getyldctype(cyld);
								std::vector<std::string>values;
								boost::trim_if(data, boost::is_any_of(FMT_STR_SEPARATOR));
								const std::string yldsplitc = FMT_STR_SEPARATOR + std::string(",");
								boost::split(values, data, boost::is_any_of(yldsplitc), boost::token_compress_on);
								std::vector<double>cvalues;
								std::vector<std::string>csource;
								if (complextype == Core::FMTyieldparserop::FMTequation)
								{
									actualyield->second.push_data(yldname, geteq(data, constants, yields, themes));
								}
								else {
									std::vector<bool>stacking;
									for (size_t id = 0; id < values.size(); ++id)
									{
										if (isnum(values[id]) || constants.isconstant(values[id]))
										{
											const double value = getnum<double>(values[id], constants);
											cvalues.push_back(value);
											stacking.push_back(false);
										}
										else if (values[id].find("#") != std::string::npos)
										{
											_exhandler->raise(Exception::FMTexc::FMTundefined_constant, _section, values[id] + " at line " + std::to_string(_line), __LINE__, __FILE__);
										}
										else {
											const std::vector<std::string>ylds = yields.getyldsnames(); //not so optimzed
											if (find(ylds.begin(), ylds.end(), values[id]) == ylds.end())
											{
												_exhandler->raise(Exception::FMTexc::FMTignore, _section,
													values[id] + " at line " + std::to_string(_line), __LINE__, __FILE__);
											}
											stacking.push_back(true);
											csource.push_back(values[id]);
										}
									}
									actualyield->second.push_data(yldname, Core::FMTdata(cvalues, complextype, csource, stacking));
								}
							}
							else {
								_exhandler->raise(Exception::FMTexc::FMTunsupported_yield, _section, line + " at line " + std::to_string(_line), __LINE__, __FILE__);
							}
						}
					}
				}
			}
			//iterate on all yieldhandler if equation with index then take the handler
			//delete the handler at it's yields location
			//decompose and insert all new handlers
			//std::vector<Core::FMTyieldhandler>::iterator handler_it = yields.databegin();
			std::vector<std::pair<Core::FMTmask, Core::FMTyieldhandler>>::iterator handler_it = yields.begin();
			const std::vector<std::string>yldnames = yields.getyldsnames();
			while (handler_it != yields.end())
			{
				const std::vector<std::string>indexvalues = handler_it->second.indexes(yldnames);
				if (!indexvalues.empty())
				{
					size_t location = std::distance(yields.begin(), handler_it);
					const Core::FMTmask oldmask = (yields.begin() + location)->first;
					const Core::FMTyieldhandler oldhandler = handler_it->second;
					yields.erase(location);
					std::vector<Core::FMTmask>todecompose;
					todecompose.push_back(oldmask);
					std::vector<Core::FMTtheme>themes_windex;
					for (const Core::FMTtheme& theme : themes)
					{
						if (theme.useindex())
						{
							for (const std::string& index : indexvalues)
							{
								if (theme.isindex(index))
								{
									std::vector<Core::FMTmask>newdecomposer;
									while (!todecompose.empty())
									{
										const std::vector<Core::FMTmask>allmasks = (todecompose.front()).decompose(theme);
										todecompose.erase(todecompose.begin());
										newdecomposer.insert(newdecomposer.end(), allmasks.begin(), allmasks.end());
									}
									todecompose = newdecomposer;
									themes_windex.push_back(theme);
									break;
								}
							}
						}
					}
					for (const Core::FMTmask& newmask : todecompose)
					{
						Core::FMTyieldhandler newhandler(Core::FMTyldtype::FMTcomplexyld, newmask);
						std::map<std::string, double>handler_values;
						for (const Core::FMTtheme& theme : themes_windex)
						{
							const std::string attribute = newmask.get(theme);
							for (const std::string& index : indexvalues)
							{
								if (theme.isindex(index))
								{
									handler_values[index] = theme.getindex(attribute, index);
								}
							}
						}
						std::map<std::string, Core::FMTdata>alladata = oldhandler.getdataelements();
						for (std::map<std::string, Core::FMTdata>::const_iterator datait = alladata.begin(); datait != alladata.end(); datait++)
						{
							const Core::FMTexpression expression = datait->second.toexpression();
							const std::vector<std::string>allvalues = expression.getinfix();
							std::vector<double>numbers(allvalues.size(), 0);
							std::vector<std::string>valuesnoperators(allvalues.size());
							size_t data_loc = 0;
							for (const std::string& source : allvalues)
							{
								if (handler_values.find(source) != handler_values.end())
								{
									numbers[data_loc] = handler_values.at(source);
								}
								else if (isnum(source))
								{
									numbers[data_loc] = stod(source);
								}
								else {
									valuesnoperators[data_loc] = source;
								}
								++data_loc;
							}
							newhandler.push_data(datait->first, Core::FMTdata(numbers, Core::FMTyieldparserop::FMTequation, valuesnoperators));
						}
						yields.insert(location, newmask, newhandler);
						++location;
					}
					handler_it = (yields.begin() + location);
				}
				else {
					++handler_it;
				}
			}
		}
		yields.update();
	}catch(...)
		{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, _section, "in FMTyieldparser::read", __LINE__, __FILE__);
		}
    return yields;
    }

void FMTyieldparser::write(const Core::FMTyields& yields,const std::string& location) const
    {
	std::ofstream yieldstream;
    yieldstream.open(location);
    if (tryopening(yieldstream,location))
        {
        const std::vector<std::string>stackedyields = yields.getstacked();
        for (const std::string& val : stackedyields)
            {
            yieldstream<< std::string(val);
            }
        yieldstream.close();
        }
    }
}
