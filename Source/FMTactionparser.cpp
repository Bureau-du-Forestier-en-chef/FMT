/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTactionparser.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTyields.h"
//#ifdef FMTWITHTORCH
	#include <boost/property_tree/json_parser.hpp>
//#endif // FMTWITHTORCH

namespace Parser{

FMTactionparser::FMTactionparser() : FMTparser(),
    rxsection("^(\\*ACTION)([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([NY])([\\s\\t]*)(_LOCKEXEMPT)|(\\*ACTION)([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([NY])|(\\*OPERABLE)([\\s\\t]*)([^\\s^\\t]*)|(\\*AGGREGATE)([\\s\\t])(.+)|(\\*PARTIAL)([\\s\\t])(.+)",std::regex_constants::ECMAScript| std::regex_constants::icase),
    rxoperator("((\\w+)[\\s\\t]*([<=>]*)[\\s\\t]*(\\d+))|(and)|(or)|([^\\s^\\t]*)", std::regex_constants::ECMAScript| std::regex_constants::icase)
    {
	setsection(Core::FMTsection::Action);
	}

FMTactionparser::FMTactionparser(const FMTactionparser& rhs):FMTparser(rhs),rxsection(rhs.rxsection),rxoperator(rhs.rxoperator)
    {
	setsection(Core::FMTsection::Action);
    }
FMTactionparser& FMTactionparser::operator = (const FMTactionparser& rhs)
    {
    if(this!=&rhs)
        {
        FMTparser::operator=(rhs);
        rxsection = rhs.rxsection;
        rxoperator = rhs.rxoperator;
		setsection(Core::FMTsection::Action);
        }
    return *this;
    }


	std::string FMTactionparser::getbounds(std::string& line, Core::FMTspec& spec,const Core::FMTconstants& constants, const Core::FMTyields& ylds)
        {
		std::string mask = "";
		try {
			const std::vector<std::string>elements = spliter(line, FMTparser::rxseparator);
			const std::array<std::string, 5> baseoperators = this->getbaseoperators();
			size_t loc = 0;
			int maskloc = 0;
			bool gotsomething = false;
			std::vector<std::string>yields;
			for (const std::string& op : elements)
			{
				if (std::find(baseoperators.begin(), baseoperators.end(), op) != baseoperators.end())
				{
					const std::string  yield = elements[loc - 1];
					if (yield == "_AGE")
					{
						spec.addbounds(Core::FMTagebounds(bounds<int>(constants, elements[loc + 1], op, Core::FMTsection::Action)));
					}
					else if (yield == "_CP")
					{
						spec.setbounds(Core::FMTperbounds(bounds<int>(constants, elements[loc + 1], op, Core::FMTsection::Action)));
					}
					else {
						yields.push_back(yield);
						spec.addbounds(Core::FMTyldbounds(yield, bounds<double>(constants, elements[loc + 1], op, Core::FMTsection::Action)));
					}
					if (maskloc == 0)
					{
						maskloc = static_cast<int>(loc) - 1;
					}
					gotsomething = true;
				}
				++loc;
			}
			if (!gotsomething)
				{
				maskloc = static_cast<int>(elements.size());
				}
			mask = "";
			for (int id = 0; id < maskloc; ++id)
			{
				mask += elements[id] + " ";
			}
			mask = mask.substr(0, mask.size() - 1);
			for (const std::string yldname : yields)
			{
				if (!isyld(ylds, yldname, Core::FMTsection::Action)) continue;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch(
				"for "+line,"FMTactionparser::getbounds", __LINE__, __FILE__, _section);
			}
        return mask;
        }
	std::map<std::string, std::vector<std::string>>FMTactionparser::valagg(std::vector<Core::FMTaction>& actions, std::map<std::string, std::vector<std::string>>& aggregates)
        {
		std::map<std::string, std::vector<std::string>>aggs;
		try {
			for(std::map<std::string, std::vector<std::string>>::iterator it = aggregates.begin(); it!=aggregates.end();it++)
				{
				const std::vector<std::string>* oldagg = &it->second;
				if(!oldagg->empty())
					{
					aggs[it->first] = *oldagg;
					}
				}
		}catch (...)
			{
			_exhandler->raisefromcatch(
				"","FMTactionparser::valagg", __LINE__, __FILE__, _section);
			}
        return aggs;
        }
    std::vector<Core::FMTaction>FMTactionparser::read(const std::vector<Core::FMTtheme>& themes,
							const Core::FMTyields& yields,
							const Core::FMTconstants& constants,
							const std::string& location)
        {
		std::vector<Core::FMTaction>cleanedactions;
		try {
			std::ifstream actionstream(location);
			std::string line;
			std::string operablename;
			std::string aggregatename;
			std::string partialname;
			std::vector<Core::FMTaction>actions;
			std::map<std::string, std::vector<std::string>>aggregates;
			Core::FMTaction* theaction = nullptr;
			if (FMTparser::tryopening(actionstream, location))
			{
				while (actionstream.is_open())
				{
					line = getcleanlinewfor(actionstream, themes, constants);
					if (!line.empty())
					{
						std::smatch kmatch;
						
						if (!std::regex_search(line, kmatch, FMTactionparser::rxsection))
						{
							//crash here
						}
						const std::string action = std::string(kmatch[1]) + std::string(kmatch[8]);
						const std::string operable = kmatch[13];
						const std::string aggregate = kmatch[16];
						const std::string partial = kmatch[19];
						if (!action.empty())
						{
							operablename.clear();
							aggregatename.clear();
							partialname.clear();
							const std::string actionname = std::string(kmatch[3]) + std::string(kmatch[10]);
							const std::string locking = kmatch[7];
							const std::string capage = std::string(kmatch[5]) + std::string(kmatch[12]);
							const bool resetage = (capage == "Y") ? true : false;
							const bool respectlock = (locking.empty()) ? true : false;
							actions.push_back(Core::FMTaction(actionname, respectlock, resetage));
						}
						else if (!operable.empty())
						{
							operablename = kmatch[15];
							if (operablename.empty())
								{
								_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,"Empty operable for "+actions.back().getname() + " at line " + std::to_string(_line),
														"FMTactionparser::read", __LINE__, __FILE__, _section);
								}
							const std::vector<Core::FMTaction*>pactions = sameactionas(operablename, actions);
							theaction = pactions.at(0);
							operablename = theaction->getname();
							if (pactions.size() > 1)
							{
								std::vector<std::pair<Core::FMTmask, Core::FMTspec>>::const_iterator dataof = pactions.at(1)->begin();
								for (size_t id = 0; id < pactions.at(1)->size(); ++id)
								{
									theaction->push_back(dataof->first,dataof->second);
								}
							}
							aggregatename.clear();
							partialname.clear();
						}
						else if (!aggregate.empty())
						{
							aggregatename = kmatch[18];
							boost::trim(aggregatename);
							operablename.clear();
							partialname.clear();
							aggregates[aggregatename] = std::vector<std::string>();
						}
						else if (!partial.empty())
						{
							
							partialname = kmatch[21];
							const std::vector<Core::FMTaction*>pactions = sameactionas(partialname, actions);
							operablename.clear();
							aggregatename.clear();
							theaction = pactions.at(0);
							partialname = theaction->getname();
							if (pactions.size() > 1)
							{
								for (const std::string& samepartial : pactions.at(1)->getpartials())
								{
									theaction->push_partials(samepartial);
								}
								
							}
						}
						else if (!operablename.empty())
						{
							Core::FMTspec spec;
							std::string mask = getbounds(line, spec, constants, yields);
							
							if (!validate(themes, mask, " at line " + std::to_string(_line))) continue;
							const Core::FMTmask newmask(mask, themes);
							const size_t loc = std::distance(actions.begin(), std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(operablename)));
							actions[loc].push_back(newmask, spec);
						}
						else if (!aggregatename.empty())
						{
							const std::vector<std::string>splited = FMTparser::spliter(line, FMTparser::rxseparator);
							for (const std::string& val : splited)
							{
								if (std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(val)) != actions.end())
								{
									aggregates[aggregatename].push_back(val);
								}
								else {
									_exhandler->raise(Exception::FMTexc::FMTundefined_aggregate_value,
										val + " at line " + std::to_string(_line),"FMTactionparser::read", __LINE__, __FILE__, _section);
								}
							}
						}
						else if (!partialname.empty())
						{
							if (theaction && theaction->isresetage())
							{
								_exhandler->raise(Exception::FMTexc::FMTwrong_partial, partialname + " at line " + std::to_string(_line),
									"FMTactionparser::read", __LINE__, __FILE__, _section);
							}
							const std::vector<std::string>splited = FMTparser::spliter(line, FMTparser::rxseparator);
							for (const std::string& val : splited)
							{
								theaction->push_partials(val);
							}
						}

					}

				}
				std::vector<size_t>todelete;
				size_t id = 0;
				for (Core::FMTaction& action : actions)
				{
					if (find(todelete.begin(), todelete.end(), id) == todelete.end())
					{
						action.shrink();
						cleanedactions.push_back(action);
					}
					else {
						_exhandler->raise(Exception::FMTexc::FMTempty_action, 
							action.getname(),"FMTactionparser::read", __LINE__, __FILE__, _section);
					}
					++id;
				}
				std::map<std::string, std::vector<std::string>>cleanedag = valagg(actions, aggregates);
				aggregates = cleanedag;
			}
			for (const auto& aggobj : aggregates)
			{
				for (Core::FMTaction& action : cleanedactions)
				{
					if (std::find(aggobj.second.begin(), aggobj.second.end(), action.getname()) != aggobj.second.end())
					{
						action.push_aggregate(aggobj.first);
					}
				action.passinobject(*this);
				}
			}
			//#ifdef FMTWITHTORCH
			cleanedactions = getGCBMactionsaggregate(cleanedactions);
			//#endif // FMTWITHTORCH
			cleanedactions.shrink_to_fit();
		}catch (...)
			{
			_exhandler->raisefromcatch(
				"In "+_location+" at line "+std::to_string(_line),"FMTactionparser::read", __LINE__, __FILE__, _section);
			}
        return cleanedactions;
        }

		std::vector<Core::FMTaction>FMTactionparser::getGCBMactionsaggregate(const std::vector<Core::FMTaction>& actions) const
		{
			std::vector<Core::FMTaction>actionswithgcbmaggregate(actions);
			try {
			//#ifdef FMTWITHTORCH
				const boost::filesystem::path filelocation = boost::filesystem::path(getruntimelocation()) / boost::filesystem::path("YieldPredModels") / boost::filesystem::path("actionsmapping.json");
				std::ifstream jsonstream(filelocation.string());
				if (FMTparser::tryopening(jsonstream, filelocation.string()))
				{
					boost::property_tree::ptree root;
					boost::property_tree::read_json(jsonstream, root);
					for (Core::FMTaction& action : actionswithgcbmaggregate)
						{
						if (root.find(action.getname())==root.not_found()||
							root.get_child(action.getname()).find("id") == root.get_child(action.getname()).not_found()||
							root.get_child(action.getname()).find("name") == root.get_child(action.getname()).not_found())
							{
							_exhandler->raise(Exception::FMTexc::FMTignore, action.getname() + " at line " + std::to_string(_line),
								"FMTactionparser::getactionsidsofmodelyields", __LINE__, __FILE__, _section);
						}else {
							//test to int!
							const int idofaction = getnum<int>(root.get<std::string>(action.getname() + ".id"));
							if (idofaction== FMTGCBMGROWTHID || idofaction == FMTGCBMUNKNOWNID || idofaction == FMTGCBMDEATHID)
								{
								_exhandler->raise(Exception::FMTinvalid_number,"cannot use GCBM actions id "+std::to_string(FMTGCBMGROWTHID)+" or "+ std::to_string(FMTGCBMUNKNOWNID) + " or " + std::to_string(FMTGCBMDEATHID) + " at line " + std::to_string(_line),
									"FMTactionparser::getactionsidsofmodelyields", __LINE__, __FILE__, _section);
								}
							const std::string GCBMaggregate = "~GCBM:" + root.get<std::string>(action.getname() + ".id") + ":" + root.get<std::string>(action.getname() + ".name");
							action.push_aggregate(GCBMaggregate);
							}
						}
					jsonstream.close();
				}

			//#endif // FMTWITHTORCH
			}
			catch (...)
			{
				_exhandler->raisefromcatch(
					"", "FMTactionparser::getGCBMactionsaggregates", __LINE__, __FILE__, _section);
			}
			return actionswithgcbmaggregate;
		}


    void FMTactionparser::write(const std::vector<Core::FMTaction>& actions,
		const std::string& location) const
        {
		try {
			std::ofstream actionstream;
			actionstream.open(location);
			std::map<std::string, std::vector<std::string>>allaggregates;
			if (tryopening(actionstream, location))
			{
				for (const Core::FMTaction& act : actions)
				{
					actionstream << std::string(act) << "\n";
					for (const std::string& aggregate : act.getaggregates())
					{
						if (allaggregates.find(aggregate) == allaggregates.end())
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
					for (const std::string& act_str : aggit->second)
					{
						actionstream << act_str << "\n";
					}
				}
				actionstream << "\n";
				actionstream.close();
			}
		}catch (...)
			{
			_exhandler->raisefromcatch(
				"","FMTactionparser::write", __LINE__, __FILE__, _section);
			}
        }

    std::vector<Core::FMTaction*> FMTactionparser::sameactionas(const std::string& all_set, std::vector<Core::FMTaction>& actions) const
        {
		std::vector<Core::FMTaction*>all_pointers;
		try {
			const std::vector<std::string>response = sameas(all_set);
			for(const std::string& actname : response)
				{
				all_pointers.push_back(&(*(std::find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(actname)))));
				}
		}catch (...)
			{
			_exhandler->raisefromcatch(
				"","FMTactionparser::sameactionas", __LINE__, __FILE__, _section);
			}
        return all_pointers;
        }

}
