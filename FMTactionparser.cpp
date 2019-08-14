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

#include "FMTactionparser.h"

namespace WSParser{

FMTactionparser::FMTactionparser() : FMTparser(),
    rxsection(regex("^(\\*ACTION)([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([NY])([\\s\\t]*)(_LOCKEXEMPT)|(\\*ACTION)([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([NY])|(\\*OPERABLE)([\\s\\t]*)([^\\s^\\t]*)|(\\*AGGREGATE)([\\s\\t])(.+)|(\\*PARTIAL)([\\s\\t])(.+)",regex_constants::ECMAScript|regex_constants::icase)),
    rxoperator(regex("((\\w+)[\\s\\t]*([<=>]*)[\\s\\t]*(\\d+))|(and)|(or)|([^\\s^\\t]*)",regex_constants::ECMAScript|regex_constants::icase)),
    operators({"=","<=",">=","<",">"})
    {}

FMTactionparser::FMTactionparser(const FMTactionparser& rhs):FMTparser(rhs),rxsection(rhs.rxsection),rxoperator(rhs.rxoperator),operators(rhs.operators)
    {

    }
FMTactionparser& FMTactionparser::operator = (const FMTactionparser& rhs)
    {
    if(this!=&rhs)
        {
        FMTparser::operator=(rhs);
        rxsection = rhs.rxsection;
        rxoperator = rhs.rxoperator;
        operators= rhs.operators;
        }
    return *this;
    }


    string FMTactionparser::getbounds(string& line, FMTspec& spec,const FMTconstants& constants, const FMTyields& ylds)
        {
        vector<string>elements=spliter(line,FMTparser::rxseparator);
        string mask = "";
        size_t loc=0;
        int maskloc=0;
        vector<string>yields;
        for(string& op : elements)
            {
            if (find(operators.begin(),operators.end(),op)!=operators.end())
                {
                string  yield = elements[loc-1];
                if(yield=="_AGE")
                    {
                    spec.addbounds(FMTagebounds(bounds<int>(constants,elements[loc+1],op,FMTwssect::Action)));
                    //spec.addbounds(FMTagebounds(FMTwssect::Action,op,elements[loc+1]));
                    //cout<<spec.empty()<<endl;
				}else if (yield == "_CP")
					{
					spec.addbounds(FMTperbounds(bounds<int>(constants, elements[loc + 1], op, FMTwssect::Action)));
					}else {
                        yields.push_back(yield);
                        spec.addbounds(FMTyldbounds(yield,bounds<double>(constants,elements[loc+1],op,FMTwssect::Action)));
                        //spec.addbounds(FMTyldbounds(FMTwssect::Action,yield,op,elements[loc+1]));
                        }
                    if (maskloc==0)
                        {
                        maskloc = int(loc) - 1;
                        }
                }
            ++loc;
            }
        mask = "";
        for(int id =0;id<maskloc;++id)
            {
            /*if (find(usedthemes.begin(),usedthemes.end(),id)==usedthemes.end())
                {
                usedthemes.push_back(id);
                }*/
            mask+=elements[id]+" ";
            }
        mask = mask.substr(0, mask.size()-1);
        for (const string yldname : yields)
            {
            if (!isyld(ylds,yldname,FMTwssect::Action)) continue;
            }
        return mask;
        }
    map<string,vector<string>>FMTactionparser::valagg(vector<FMTaction>& actions,map<string,vector<string>>& aggregates)
        {
        map<string,vector<string>>aggs;
        for(map<string,vector<string>>::iterator it = aggregates.begin(); it!=aggregates.end();it++)
            {
            vector<string>* oldagg = &it->second;
            if(!oldagg->empty())
                {
                /*vector<string> newagg;
                for(string& agg : *oldagg)
                    {
                    if (find(actions.begin(),actions.end(),FMTaction(agg))!=actions.end())
                        {
                        newagg.push_back(agg);
                        }
                    }*/
                /*if(!newagg.empty())
                    {*/
                    aggs[it->first] = *oldagg;
                   /* }*/
                }
            }
        return aggs;
        }
    vector<FMTaction>FMTactionparser::read(const vector<FMTtheme>& themes,
							const FMTyields& yields,
							const FMTconstants& constants,
							string location,
							map<string,vector<string>>& aggregates)
        {
        ifstream actionstream(location);
        string line;
        string operablename;
        string aggregatename;
        string partialname;
        //map<string,vector<string>>aggregates;
        //map<string,vector<string>>partials;
        vector<FMTaction>actions;
        vector<FMTaction>cleanedactions;
        FMTaction* theaction = nullptr;
        //vector<map<FMTmask,FMTspec>>actops;
        if (FMTparser::tryopening(actionstream,location))
            {
            while(actionstream.is_open())
                {
                line = FMTparser::getcleanline(actionstream);
                if (!line.empty())
                    {
                    smatch kmatch;
                    if(!regex_search(line,kmatch,FMTactionparser::rxsection))
                        {
                        //crash here
                        }
                    string action = string(kmatch[1])+string(kmatch[8]);
                    string operable = kmatch[13];
                    string aggregate = kmatch[16];
                    string partial = kmatch[19];

                    if(!action.empty())
                        {
                        operablename.clear();
                        aggregatename.clear();
                        partialname.clear();
                        string actionname = string(kmatch[3])+string(kmatch[10]);
                        string locking = kmatch[7];
                        string capage = string(kmatch[5]) + string(kmatch[12]);
                        bool resetage = (capage=="Y") ? true : false;
                        bool respectlock = (locking.empty()) ? true : false;

                        actions.push_back(FMTaction(actionname,respectlock,resetage));
                        //actops.push_back(map<FMTmask,FMTspec>());
                        }else if(!operable.empty())
                            {
                            operablename = kmatch[15];
                            vector<FMTaction*>pactions = sameactionas(operablename,actions);
                            theaction = pactions.at(0);
                            operablename = theaction->name;
                            if (pactions.size()>1)
                                {
                                vector<FMTmask>::const_iterator mask_it =  pactions.at(1)->maskbegin();
                                vector<FMTspec>::const_iterator data_it =  pactions.at(1)->databegin();
                                for (size_t id = 0 ; id<pactions.at(1)->size(); ++id)
                                    {
                                    theaction->push_back(*(mask_it+id),*(data_it+id));
                                    }
                                }
                            aggregatename.clear();
                            partialname.clear();
                            //theaction = &(*(find_if(actions.begin(), actions.end(), FMTactioncomparator(operablename))));
                            }else if(!aggregate.empty())
                                {
                                aggregatename = kmatch[18];
                                operablename.clear();
                                partialname.clear();
                                aggregates[aggregatename]=vector<string>();
                                }else if(!partial.empty())
                                    {
                                    //vector<string>splited = FMTparser::spliter(line,FMTparser::rxseparator);
                                    partialname = kmatch[21];
                                    vector<FMTaction*>pactions = sameactionas(partialname,actions);
                                    operablename.clear();
                                    aggregatename.clear();
                                    theaction = pactions.at(0);
                                    partialname = theaction->name;
                                    if (pactions.size()>1)
                                        {
                                        for (const string& samepartial : pactions.at(1)->getpartials())
                                            {
                                            theaction->push_partials(samepartial);
                                            }
                                        }
                                    }else if(!operablename.empty())
                                        {
                                        FMTspec spec;
                                        string mask = getbounds(line,spec,constants,yields);
                                        //if (!spec.empty())
                                            //{
                                            if (!validate(themes, mask)) continue;
                                            FMTmask newmask(mask,themes);
                                            //theaction->push_bounds(newmask,bounds);

                                            size_t loc = std::distance(actions.begin(), find_if(actions.begin(), actions.end(), FMTactioncomparator(operablename)));
											/*vector<FMTmask>::iterator mskit = find(actions[loc].maskbegin(), actions[loc].maskend(), newmask);
											Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << " ADDING " << "\n";
											if(mskit != actions[loc].maskend())
                                                {
												vector<FMTspec>::iterator datait = (actions[loc].databegin() + std::distance(actions[loc].maskbegin(), mskit));
												datait->add(spec);
                                                }else{*/
												actions[loc].push_back(newmask,spec);
                                               // }


                                            //}
                                        }else if(!aggregatename.empty())
                                            {
                                            vector<string>splited = FMTparser::spliter(line,FMTparser::rxseparator);
                                            for(string& val : splited)
                                                {
                                                if (find_if(actions.begin(), actions.end(), FMTactioncomparator(val)) !=actions.end())
                                                    {
                                                    aggregates[aggregatename].push_back(val);
                                                    }else{
                                                    _exhandler->raise(FMTexc::WSundefined_aggregate_value,_section,val+" at line" + to_string(_line), __LINE__, __FILE__);
                                                    }
                                                }
                                            }else if(!partialname.empty())
                                                {
                                                if (theaction->reset)
                                                    {
                                                    _exhandler->raise(FMTexc::WSwrong_partial,_section,partialname+" at line" + to_string(_line), __LINE__, __FILE__);
                                                    }
                                                vector<string>splited = FMTparser::spliter(line,FMTparser::rxseparator);
                                                for(string& val : splited)
                                                    {
                                                    theaction->push_partials(val);
                                                    }
                                                }

                    }

                }
            vector<size_t>todelete;
            /*for(size_t id =0;id<actions.size();++id)
                {
                if (actions[id].empty())
                    {
                    todelete.push_back(id);
                    }
                }*/
            size_t id = 0;
            for(FMTaction& action : actions)
                {
                if(find(todelete.begin(),todelete.end(),id)==todelete.end())
                    {
					action.shrink();
                    cleanedactions.push_back(action);
                    }else{
                    _exhandler->raise(FMTexc::WSempty_action,_section,action.name, __LINE__, __FILE__);
                    }
                ++id;
                }
            map<string,vector<string>>cleanedag = valagg(actions,aggregates);
			aggregates = cleanedag;
            }
        std::sort(cleanedactions.begin(),cleanedactions.end());
        return cleanedactions;
        }
    bool FMTactionparser::write(const vector<FMTaction>& actions,
		string location,const  map<string, vector<string>>& aggregates)
        {
        ofstream actionstream;
        actionstream.open(location);
        if (tryopening(actionstream,location))
            {
            for(const FMTaction& act : actions)
                {
                actionstream<<string(act)<<"\n";
                }
			actionstream << "\n";
			for (map<string, vector<string>>::const_iterator aggit = aggregates.begin(); aggit != aggregates.end(); aggit++)
				{
				actionstream << "*AGGREGATE " + aggit->first << "\n";
				for(const string& act_str : aggit->second)
					{
					actionstream << act_str << "\n";
					}
				}
			actionstream << "\n";
            actionstream.close();
            return true;
            }
        return false;
        }

    vector<FMTaction*> FMTactionparser::sameactionas(const string& all_set,vector<FMTaction>& actions) const
        {
        vector<FMTaction*>all_pointers;
        vector<string>response = sameas(all_set);
        for(const string& actname : response)
            {
            all_pointers.push_back(&(*(find_if(actions.begin(), actions.end(), FMTactioncomparator(actname)))));
            }
        return all_pointers;
        }

}
