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

#include "FMTtransitionparser.h"


namespace WSParser{

FMTtransitionparser::FMTtransitionparser():FMTparser(),
    rxsection(regex("^(\\*CASE)([\\s\\t]*)([^\\s^\\t]*)|(\\*SOURCE)([\\s\\t]*)(.+)|(\\*TARGET)([\\s\\t]*)(.+)",regex_constants::ECMAScript|regex_constants::icase)),
    rxlock(regex("^(.+)(_LOCK)([\\s\\t]*)([0-9]*)(.+)",regex_constants::ECMAScript|regex_constants::icase)),
    rxage(regex("^(.+)(_AGE)([\\s\\t]*)([0-9]*)(.+)",regex_constants::ECMAScript|regex_constants::icase)),
    rxreplace(regex("^(.+)(_REPLACE)(....)([0-9]*)([\\s\\t]*)(\\,)([\\s\\t]*)(_TH)([0-9]*)([\\s\\t]*)(\\+)([\\s\\t]*)([0-9]*)(.+)",regex_constants::ECMAScript|regex_constants::icase)),
    rxtyld(regex("^([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([^\\s^\\t]*)",regex_constants::ECMAScript|regex_constants::icase))
    {

    }

FMTtransitionparser::FMTtransitionparser(const FMTtransitionparser& rhs) : FMTparser(rhs),
    rxsection(rhs.rxsection),
    rxlock(rhs.rxlock),
    rxage(rhs.rxage),
    rxreplace(rhs.rxreplace),
    rxtyld(rhs.rxtyld)
    {

    }
FMTtransitionparser& FMTtransitionparser::operator = (const FMTtransitionparser& rhs)
    {
    if(this!=&rhs)
        {
        FMTparser::operator =(rhs);
        rxsection = rhs.rxsection;
        rxlock = rhs.rxlock;
        rxage = rhs.rxage;
        rxtyld = rhs.rxtyld;
        rxreplace = rhs.rxreplace;
        }
    return *this;
    }

FMTmask FMTtransitionparser::getsource(string& line, FMTspec& spec,const vector<FMTtheme>& themes,FMTwssect section,const FMTconstants& constants,const FMTyields& ylds)
    {
    vector<string>elements = FMTparser::spliter(line,FMTparser::rxseparator);
    if (elements.size() == themes.size())
        {
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "IN "<<line << "\n";
		validate(themes,line);
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "OUT " << line << "\n";
        return FMTmask(line, themes);
        }else{
            string mask;
            string rest=" ";
            for(size_t theme = 0; theme < elements.size(); ++theme)
                {
                if (theme < themes.size())
                    {
                    mask+=elements[theme]+" ";
                    }else{
                    rest+=elements[theme]+" ";
                    }
                }
            mask = mask.substr(0, mask.size()-1);
            rest = rest.substr(0, rest.size()-1);
			validate(themes,mask);
            FMTmask newmask(mask,themes);
            rest += " ";
            rest = setspec(FMTwssect::Transition,FMTwskwor::Source,ylds,constants,spec,rest);
            return newmask;
            }
    }

vector<FMTtransitionmask> FMTtransitionparser::getmasktran(const string& line,const vector<FMTtheme>& themes,
                                          const FMTconstants& constants, const FMTyields& ylds,
                                          const FMTmask& sourcemask, int& replaced)
    {
    vector<string>elements = FMTparser::spliter(line,FMTparser::rxseparator);
    vector<FMTmask>multiples;
    string mask = "";
    double proportion;
    size_t id = 1;
    while(id < (themes.size()+1))
        {
        mask += elements[id] +" ";
        ++id;
        }
    mask = mask.substr(0, mask.size()-1);
    validate(themes,mask);
    proportion = getnum<double>(elements[id],constants);
    ++id;
    string rest=" ";
    while(id < elements.size())
        {
        rest+=elements[id]+" ";
        ++id;
        }
    smatch kmatch;
    int age = -1;
    int lock = 0;
    FMTtransitionmask trans(mask,themes,/*lock,*/proportion);
    if (regex_search(rest,kmatch,FMTtransitionparser::rxlock))
        {
        string strlock = kmatch[4];
        lock = getnum<int>(strlock,constants);
        rest = string(kmatch[1]) + string(kmatch[5]);
        trans.addbounds(FMTlockbounds(FMTwssect::Transition,FMTwskwor::Target,lock,lock));
        }
    if (regex_search(rest,kmatch,FMTtransitionparser::rxage))
        {
        string strage = kmatch[4];
        age = getnum<int>(strage,constants);
        rest = string(kmatch[1]) + string(kmatch[5]);
        trans.addbounds(FMTagebounds(FMTwssect::Transition,FMTwskwor::Target,age,age));
        }
     if (regex_search(rest,kmatch,FMTtransitionparser::rxreplace))
        {
        /*_exhandler->raise(FMTexc::WSunsupported_transition,FMTwssect::Transition," for _replace keyword at line " + to_string(_line));*/
        //no support
        //add up a num
        string strtargettheme = kmatch[4];
        string stroptheme = kmatch[9];
        string stradd = kmatch[13];
        int targettheme  = stoi(strtargettheme)-1;
        //int optheme  = stoi(stroptheme)-1;
        int addupp = stoi(stradd);
        FMTmask targetmask(mask,themes);
        targetmask.set(themes[targettheme],sourcemask.get(themes[targettheme]));
        for (FMTmask& lmask : targetmask.decompose(themes[targettheme]))
            {
            string actual = lmask.get(themes[targettheme]);
            int newint = stoi(actual) + addupp;
            string newval = to_string(newint);
            if (themes[targettheme].isattribute(newval))
                {
                lmask.set(themes[targettheme],newval);//set is probably not working?!?!?!?!
                multiples.push_back(lmask);
                }
            }
        replaced = targettheme;
        rest = string(kmatch[1])+string(kmatch[14]);
        }
    if (isvalid(rest) && regex_search(rest,kmatch,FMTtransitionparser::rxtyld))
        {
        string yld = kmatch[2];
        string strvalue = kmatch[4];
        if (!yld.empty() && !strvalue.empty())
            {
            double upperbound = numeric_limits<double>::max();
            double lowerbound = getnum<double>(strvalue,constants);
            isyld(ylds,yld,FMTwssect::Transition);
            trans.addbounds(FMTyldbounds(FMTwssect::Transition,FMTwskwor::Target,yld,upperbound,lowerbound));
            }
        }
    vector<FMTtransitionmask>alltrans;
    if (!multiples.empty())
        {
        for(const FMTmask& msk : multiples)
            {
            alltrans.push_back(FMTtransitionmask(trans,msk,themes));
            }
        }else{
        alltrans.push_back(trans);
        }
    return alltrans;
    }


vector<FMTtransition> FMTtransitionparser::read(const vector<FMTtheme>& themes,const vector<FMTaction>& actions,const FMTyields& ylds,const FMTconstants& constants,string location)
    {
    ifstream transitionstream(location);
    vector<FMTtransition>transitions;
	vector<FMTtransition>temp_transitions;
    if (FMTparser::tryopening(transitionstream,location))
        {
        string line;
        string CASE;
        string SOURCE;
        string TARGET;
        string actionname;
       // map<FMTmask,vector<FMTfork>>forks;
        //FMTfork* fptr = nullptr;
        vector<FMTfork*>fptrs;
		vector<int>fork_ids;
        vector<int>replacedvec;
        FMTmask srcmsk;
		vector<FMTtransition>::iterator last_transition = temp_transitions.end();
        while(transitionstream.is_open())
            {
            line = getcleanline(transitionstream);
            if (!line.empty())
                {
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << line << "\n";
                smatch kmatch;
                if(!regex_search(line,kmatch,FMTtransitionparser::rxsection))
                    {
                    //crash here
                    }
                CASE = kmatch[3];
                SOURCE = kmatch[4];
                TARGET = kmatch[7];
                if (!CASE.empty())
                    {
                    /*if (!actionname.empty() && forks.empty())
                        {
                        _exhandler->raise(FMTexc::WSempty_transition,_section,actionname+" at line " + to_string(_line), __LINE__, __FILE__);
                        }*/
                    vector<string>ptransitionname = sameas(CASE);
					actionname = ptransitionname.at(0);
					temp_transitions.push_back(FMTtransition(actionname));
					last_transition = --temp_transitions.end();
                    if (ptransitionname.size()>1)
                        {
                        vector<FMTtransition>::const_iterator same_tr = find_if(temp_transitions.begin(), temp_transitions.end(), FMTtransitioncomparator(ptransitionname.at(1)));
                        vector<FMTmask>::const_iterator mask_it = same_tr->maskbegin();
                        vector<FMTfork>::const_iterator data_it = same_tr->databegin();
                        for (size_t id = 0 ; id<same_tr->size(); ++id)
                            {
                            last_transition->push_back(*(mask_it+id),*(data_it+id));
                            }
                        }
                    if (!isact(FMTwssect::Transition,actions,actionname)) continue;
                    }else if(!SOURCE.empty())
                        {
                        string data;
                        data = kmatch[6];
                        FMTfork fork;
                        srcmsk = getsource(data,fork,themes,FMTwssect::Transition,constants,ylds);
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<"SOURCE!!!! "<< srcmsk.getstr() << "\n";
						last_transition->push_back(srcmsk, fork);
                        fptrs.clear();
						fork_ids.clear();
                        replacedvec.clear();
                        fptrs.push_back(&(*--last_transition->dataend()));
						fork_ids.push_back(int(last_transition->size()) - 1);
                        }else if(!TARGET.empty())
                            {
                            int replaced = -1;
                            vector<FMTtransitionmask>mtrs = getmasktran(line,themes,constants,ylds,srcmsk,replaced);
                            if (replaced>-1)
                                {
                                for(const int& rep : replacedvec)
                                    {
                                    if (rep != replaced)
                                        {
                                        _exhandler->raise(FMTexc::WSunsupported_transition,_section,actionname+" at line " + to_string(_line), __LINE__, __FILE__);
                                        }
                                    }
                                replacedvec.push_back(replaced);
                                }

                            if (replaced==-1|| fork_ids.size() > 1)//fptrs.size()>1)
                                {
								//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "rep 1 " << fptrs.size()<<" "<< replaced<<" "<< mtrs.size() <<"\n";
                                int repid = 0;
                                for(const int& id : fork_ids)//FMTfork* fptr : fptrs)
                                    {
									//FMTfork* fr = ->;
										if (replaced == -1)
										{
											(last_transition->databegin() + id)->add(mtrs[0]);
										}else {
											(last_transition->databegin() + id)->add(mtrs[repid]);
										}

                                    ++repid;
                                    }
                                }else{
										//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "rep 2 " << "\n";
                                        vector<FMTfork*>newstack;
										vector<int>new_ids;
                                        FMTfork basefork;
                                        basefork = *fptrs[0];
										//delete the last fork of the transition
										if (!last_transition->empty())
											{
											last_transition->pop_back();
											}
                                        /*if(!forks[srcmsk].empty())
                                            {
                                            forks[srcmsk].pop_back();
                                            }
                                        if(forks[srcmsk].empty())
                                            {
                                            forks.erase(srcmsk);
                                            }*/
										//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "Starting decomposing " << "\n";
                                        vector<FMTmask>multisourcesmask = srcmsk.decompose(themes[replaced]);
                                        for(size_t id=0; id<mtrs.size(); ++id)
                                            {
                                            /*if(forks.find(multisourcesmask[id])==forks.end())
                                                {
                                                forks[multisourcesmask[id]] = vector<FMTfork>();
                                                }
                                            forks[multisourcesmask[id]].push_back(basefork);
                                            FMTfork* newfptr = &forks[multisourcesmask[id]].back();*/
											//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "PUSHING " << "\n";
											last_transition->push_back(multisourcesmask[id], basefork);
											FMTfork* newfptr = &(*--last_transition->dataend());
                                            newfptr->add(mtrs[id]);
                                            newstack.push_back(newfptr);
											new_ids.push_back(int(last_transition->size()) - 1);
                                            }
                                        fptrs = newstack;
										fork_ids = new_ids;
                                    }
                            }
                }
            }

			for (const FMTtransition& transition : temp_transitions)
				{
				if (!transition.empty())
					{
					if (transition.isleaking())
						{
						_exhandler->raise(FMTexc::WSleakingtransition, _section, transition.name, __LINE__, __FILE__);
					}else {
						transitions.push_back(transition);
						}
					}

				}
            std::sort(transitions.begin(),transitions.end());

        }
    return transitions;
    }

bool FMTtransitionparser::write(const vector<FMTtransition>& transitions, string location)
    {
    ofstream transitionstream;
    transitionstream.open(location);
    if (tryopening(transitionstream,location))
        {
        for(const FMTtransition& tra : transitions)
            {
            transitionstream<<string(tra)<<"\n";
            }
        transitionstream.close();
        return true;
        }
    return false;
    }

}
