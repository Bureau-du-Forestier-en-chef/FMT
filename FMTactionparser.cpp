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

namespace Parser{

FMTactionparser::FMTactionparser() : FMTparser(),
    rxsection("^(\\*ACTION)([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([NY])([\\s\\t]*)(_LOCKEXEMPT)|(\\*ACTION)([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([NY])|(\\*OPERABLE)([\\s\\t]*)([^\\s^\\t]*)|(\\*AGGREGATE)([\\s\\t])(.+)|(\\*PARTIAL)([\\s\\t])(.+)",std::regex_constants::ECMAScript| std::regex_constants::icase),
    rxoperator("((\\w+)[\\s\\t]*([<=>]*)[\\s\\t]*(\\d+))|(and)|(or)|([^\\s^\\t]*)", std::regex_constants::ECMAScript| std::regex_constants::icase)
    {}

FMTactionparser::FMTactionparser(const FMTactionparser& rhs):FMTparser(rhs),rxsection(rhs.rxsection),rxoperator(rhs.rxoperator)
    {

    }
FMTactionparser& FMTactionparser::operator = (const FMTactionparser& rhs)
    {
    if(this!=&rhs)
        {
        FMTparser::operator=(rhs);
        rxsection = rhs.rxsection;
        rxoperator = rhs.rxoperator;
        }
    return *this;
    }


	std::string FMTactionparser::getbounds(std::string& line, Core::FMTspec& spec,const Core::FMTconstants& constants, const Core::FMTyields& ylds)
        {
        const std::vector<std::string>elements=spliter(line,FMTparser::rxseparator);
		const std::array<std::string, 5> baseoperators = this->getbaseoperators();
		std::string mask = "";
        size_t loc=0;
        int maskloc=0;
		std::vector<std::string>yields;
        for(const std::string& op : elements)
            {
            if (std::find(baseoperators.begin(), baseoperators.end(),op)!= baseoperators.end())
                {
				const std::string  yield = elements[loc-1];
                if(yield=="_AGE")
                    {
                    spec.addbounds(Core::FMTagebounds(bounds<int>(constants,elements[loc+1],op, Core::FMTwssect::Action)));
				}else if (yield == "_CP")
					{
					spec.setbounds(Core::FMTperbounds(bounds<int>(constants, elements[loc + 1], op, Core::FMTwssect::Action)));
					}else {
                        yields.push_back(yield);
                        spec.addbounds(Core::FMTyldbounds(yield,bounds<double>(constants,elements[loc+1],op, Core::FMTwssect::Action)));
                        }
                    if (maskloc==0)
                        {
                        maskloc = static_cast<int>(loc) - 1;
                        }
                }
            ++loc;
            }
        mask = "";
        for(int id =0;id<maskloc;++id)
            {
            mask+=elements[id]+" ";
            }
        mask = mask.substr(0, mask.size()-1);
        for (const std::string yldname : yields)
            {
            if (!isyld(ylds,yldname, Core::FMTwssect::Action)) continue;
            }
        return mask;
        }
	std::map<std::string, std::vector<std::string>>FMTactionparser::valagg(std::vector<Core::FMTaction>& actions, std::map<std::string, std::vector<std::string>>& aggregates)
        {
		std::map<std::string, std::vector<std::string>>aggs;
        for(std::map<std::string, std::vector<std::string>>::iterator it = aggregates.begin(); it!=aggregates.end();it++)
            {
			const std::vector<std::string>* oldagg = &it->second;
            if(!oldagg->empty())
                {
                aggs[it->first] = *oldagg;
                }
            }
        return aggs;
        }
    std::vector<Core::FMTaction>FMTactionparser::read(const std::vector<Core::FMTtheme>& themes,
							const Core::FMTyields& yields,
							const Core::FMTconstants& constants,
							std::string location)
        {
        std::ifstream actionstream(location);
		std::string line;
		std::string operablename;
		std::string aggregatename;
		std::string partialname;
		std::vector<Core::FMTaction>actions;
		std::vector<Core::FMTaction>cleanedactions;
		std::map<std::string, std::vector<std::string>>aggregates;
		Core::FMTaction* theaction = nullptr;
        if (FMTparser::tryopening(actionstream,location))
            {
            while(actionstream.is_open())
                {
				line = getcleanlinewfor(actionstream, themes, constants);
                if (!line.empty())
                    {
                    std::smatch kmatch;
                    if(!std::regex_search(line,kmatch,FMTactionparser::rxsection))
                        {
                        //crash here
                        }
					const std::string action = std::string(kmatch[1])+ std::string(kmatch[8]);
					const std::string operable = kmatch[13];
					const std::string aggregate = kmatch[16];
					const std::string partial = kmatch[19];
                    if(!action.empty())
                        {
                        operablename.clear();
                        aggregatename.clear();
                        partialname.clear();
						const std::string actionname = std::string(kmatch[3])+ std::string(kmatch[10]);
						const std::string locking = kmatch[7];
						const std::string capage = std::string(kmatch[5]) + std::string(kmatch[12]);
                        const bool resetage = (capage=="Y") ? true : false;
                        const bool respectlock = (locking.empty()) ? true : false;
                        actions.push_back(Core::FMTaction(actionname,respectlock,resetage));
                        }else if(!operable.empty())
                            {
                            operablename = kmatch[15];
							const std::vector<Core::FMTaction*>pactions = sameactionas(operablename,actions);
                            theaction = pactions.at(0);
                            operablename = theaction->getname();
                            if (pactions.size()>1)
                                {
								std::vector<std::pair<Core::FMTmask,Core::FMTspec>>::const_iterator dataof=pactions.at(1)->begin();
                                for (size_t id = 0 ; id<pactions.at(1)->size(); ++id)
                                    {
									theaction->push_back(*dataof);
                                    }
                                }
                            aggregatename.clear();
                            partialname.clear();
                            }else if(!aggregate.empty())
                                {
                                aggregatename = kmatch[18];
                                operablename.clear();
                                partialname.clear();
                                aggregates[aggregatename]=std::vector<std::string>();
                                }else if(!partial.empty())
                                    {
                                    partialname = kmatch[21];
                                    const std::vector<Core::FMTaction*>pactions = sameactionas(partialname,actions);
                                    operablename.clear();
                                    aggregatename.clear();
                                    theaction = pactions.at(0);
                                    partialname = theaction->getname();
                                    if (pactions.size()>1)
                                        {
                                        for (const std::string& samepartial : pactions.at(1)->getpartials())
                                            {
                                            theaction->push_partials(samepartial);
                                            }
                                        }
                                    }else if(!operablename.empty())
                                        {
                                        Core::FMTspec spec;
                                        std::string mask = getbounds(line,spec,constants,yields);
                                        if (!validate(themes, mask, " at line " + std::to_string(_line))) continue;
                                        const Core::FMTmask newmask(mask,themes);
                                        const size_t loc = std::distance(actions.begin(), std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(operablename)));
										actions[loc].push_back(newmask,spec);
                                        }else if(!aggregatename.empty())
                                            {
                                            const std::vector<std::string>splited = FMTparser::spliter(line,FMTparser::rxseparator);
                                            for(const std::string& val : splited)
                                                {
                                                if (std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(val)) !=actions.end())
                                                    {
                                                    aggregates[aggregatename].push_back(val);
                                                    }else{
                                                    _exhandler->raise(Exception::FMTexc::WSundefined_aggregate_value,_section,val+" at line" + std::to_string(_line), __LINE__, __FILE__);
                                                    }
                                                }
                                            }else if(!partialname.empty())
                                                {
                                                if (theaction && theaction->isresetage())
                                                    {
                                                    _exhandler->raise(Exception::FMTexc::WSwrong_partial,_section,partialname+" at line" + std::to_string(_line), __LINE__, __FILE__);
                                                    }
                                                const std::vector<std::string>splited = FMTparser::spliter(line,FMTparser::rxseparator);
                                                for(const std::string& val : splited)
                                                    {
                                                    theaction->push_partials(val);
                                                    }
                                                }

                    }

                }
            std::vector<size_t>todelete;
            size_t id = 0;
            for(Core::FMTaction& action : actions)
                {
                if(find(todelete.begin(),todelete.end(),id)==todelete.end())
                    {
					action.shrink();
                    cleanedactions.push_back(action);
                    }else{
                    _exhandler->raise(Exception::FMTexc::WSempty_action,_section,action.getname(), __LINE__, __FILE__);
                    }
                ++id;
                }
            std::map<std::string,std::vector<std::string>>cleanedag = valagg(actions,aggregates);
			aggregates = cleanedag;
            }
			for (const auto& aggobj : aggregates)
				{
				for (Core::FMTaction& action : cleanedactions)
					{
					if (std::find(aggobj.second.begin(), aggobj.second.end(),action.getname())!= aggobj.second.end())
						{
						action.push_aggregate(aggobj.first);
						}
					}
				}
        std::sort(cleanedactions.begin(),cleanedactions.end());
        return cleanedactions;
        }
    bool FMTactionparser::write(const std::vector<Core::FMTaction>& actions,
		std::string location)
        {
        std::ofstream actionstream;
        actionstream.open(location);
		std::map<std::string, std::vector<std::string>>allaggregates;
        if (tryopening(actionstream,location))
            {
            for(const Core::FMTaction& act : actions)
                {
                actionstream<<std::string(act)<<"\n";
				for (const std::string& aggregate : act.getaggregates())
					{
					if (allaggregates.find(aggregate)== allaggregates.end())
						{
						allaggregates[aggregate] = std::vector<std::string>();
						}
					allaggregates[aggregate].push_back(act.getname());
					}
                }
			actionstream << "\n";
			for (std::map<std::string, std::vector<std::string>>::const_iterator aggit = allaggregates.begin(); aggit != allaggregates.end(); aggit++)
				{
				actionstream << "*AGGREGATE " + aggit->first << "\n";
				for(const std::string& act_str : aggit->second)
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

    std::vector<Core::FMTaction*> FMTactionparser::sameactionas(const std::string& all_set, std::vector<Core::FMTaction>& actions) const
        {
		std::vector<Core::FMTaction*>all_pointers;
        const std::vector<std::string>response = sameas(all_set);
        for(const std::string& actname : response)
            {
            all_pointers.push_back(&(*(std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(actname)))));
            }
        return all_pointers;
        }

}
