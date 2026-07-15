/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTactionparser.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include "FMTexceptionhandler.h"
#include <string>
#include <vector>
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>

namespace Parser{

	const boost::regex FMTactionparser::rxsection = boost::regex("^(\\*ACTION)([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([NY])([\\s\\t]*)(_LOCKEXEMPT)|(\\*ACTION)([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([NY])|(\\*OPERABLE)([\\s\\t]*)([^\\s^\\t]*)|(\\*AGGREGATE)([\\s\\t])(.+)|(\\*PARTIAL)([\\s\\t])(.+)|(\\*ACTIONSERIES)", 
							boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTactionparser::rxoperator = boost::regex("((\\w+)[\\s\\t]*([<=>]*)[\\s\\t]*(\\d+))|(and)|(or)|([^\\s^\\t]*)",
								boost::regex_constants::ECMAScript | boost::regex_constants::icase);
	const boost::regex FMTactionparser::m_SERIES_MATCH = boost::regex("(.+)([\\s\\t]*)((_ASAP)|(_ALAP))|(.+)",
															boost::regex_constants::ECMAScript | boost::regex_constants::icase);

FMTactionparser::FMTactionparser() : FMTparser()
    {
	setSection(Core::FMTsection::Action);
	}

	std::string FMTactionparser::getBounds(
		std::string& line, 
		Core::FMTspec& spec,
		const Core::FMTconstants& constants, 
		const Core::FMTyields& ylds)
        {
		std::string mask = "";
		try {
			const std::vector<std::string>elements = spliter(line, FMTparser::m_SEPARATOR);
			const std::array<std::string, 5> baseoperators = this->getBaseOperators();
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
						spec.addBounds(Core::FMTagebounds(bounds<int>(constants, elements[loc + 1], op, Core::FMTsection::Action)));
					}
					else if (yield == "_CP")
					{
						spec.setBounds(Core::FMTperbounds(bounds<int>(constants, elements[loc + 1], op, Core::FMTsection::Action)));
					}
					else {
						yields.push_back(yield);
						spec.addBounds(Core::FMTyldbounds(yield, bounds<double>(constants, elements[loc + 1], op, Core::FMTsection::Action)));
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
				if (!isYld(ylds, yldname, Core::FMTsection::Action)) continue;
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch(
				"for "+line,"FMTactionparser::getBounds", __LINE__, __FILE__, m_section);
			}
        return mask;
        }

	std::map<std::string, std::vector<std::string>>FMTactionparser::valAgg(
		std::vector<Core::FMTaction>& actions, 
		std::map<std::string, std::vector<std::string>>& aggregates)
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
			_exhandler->raiseFromCatch(
				"","FMTactionparser::valAgg", __LINE__, __FILE__, m_section);
			}
        return aggs;
        }

    std::vector<Core::FMTaction>FMTactionparser::read(
		const std::vector<Core::FMTtheme>& themes,
		const Core::FMTyields& yields,
		const Core::FMTconstants& constants,
		const std::string& location)
        {
		std::vector<Core::FMTaction> cleanedactions;
		try {
			std::ifstream actionstream(location);
			std::string line;
			std::string operablename;
			std::string aggregatename;
			std::string partialname;
			bool inseries = false;
			std::vector<Core::FMTaction> actions;
			std::map<std::string, std::vector<std::string>> aggregates;
			Core::FMTaction* theaction = nullptr;
			std::vector<Core::FMTSerie> allseries;
			if (FMTparser::tryOpening(actionstream, location))
			{
				std::queue<FMTparser::FMTLineInfo>Lines = FMTparser::getCleanLinewfor(actionstream, themes, constants);
				while (!Lines.empty())
				{
					std::string line = getLine(Lines);
					if (!line.empty())
					{
						boost::smatch kmatch;
						
						if (!boost::regex_search(line, kmatch, FMTactionparser::rxsection))
						{
							//crash here
						}
						const std::string action = std::string(kmatch[1]) + std::string(kmatch[8]);
						const std::string operable = kmatch[13];
						const std::string aggregate = kmatch[16];
						const std::string partial = kmatch[19];
						const std::string series = kmatch[22];
						if (!action.empty())
						{
							operablename.clear();
							aggregatename.clear();
							partialname.clear();
							inseries = false;
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
								_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,"Empty operable for "+actions.back().getName() + " at line " + std::to_string(m_line),
														"FMTactionparser::read", __LINE__, __FILE__, m_section);
								}
							if (operablename == "_DEATH" &&
								std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator("_DEATH"))== actions.end())
								{
								actions.push_back(Core::FMTaction("_DEATH", false, true));
								}
							const std::vector<Core::FMTaction*>pactions = sameActionAs(operablename, actions);
							theaction = pactions.at(0);
							operablename = theaction->getName();
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
							inseries = false;
						}
						else if (!aggregate.empty())
						{
							aggregatename = kmatch[18];
							boost::trim(aggregatename);
							operablename.clear();
							partialname.clear();
							inseries = false;
							aggregates[aggregatename] = std::vector<std::string>();
						}
						else if (!partial.empty())
						{
							
							partialname = kmatch[21];
							const std::vector<Core::FMTaction*>pactions = sameActionAs(partialname, actions);
							operablename.clear();
							aggregatename.clear();
							inseries = false;
							theaction = pactions.at(0);
							partialname = theaction->getName();
							if (pactions.size() > 1)
							{
								for (const std::string& samepartial : pactions.at(1)->getPartials())
								{
									theaction->pushPartials(samepartial);
								}
								
							}
						}
						else if (!series.empty())
						{
							operablename.clear();
							aggregatename.clear();
							partialname.clear();
							inseries = true;
						}
						else if (!operablename.empty())
						{
							Core::FMTspec spec;
							std::string mask = getBounds(line, spec, constants, yields);
							
							if (!Core::FMTtheme::validate(themes, mask, " at line " + std::to_string(m_line))) continue;
							const Core::FMTmask newmask(mask, themes);
							const size_t loc = std::distance(actions.begin(), std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator(operablename)));
							actions[loc].push_back(newmask, spec);
						}
						else if (!aggregatename.empty())
						{
							const std::vector<std::string>splited = FMTparser::spliter(line, FMTparser::m_SEPARATOR);
							for (const std::string& val : splited)
							{
								if (std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator(val)) != actions.end())
								{
									aggregates[aggregatename].push_back(val);
								}
								else {
									_exhandler->raise(Exception::FMTexc::FMTundefined_aggregate_value,
										val + " at line " + std::to_string(m_line),"FMTactionparser::read", __LINE__, __FILE__, m_section);
								}
							}
						}
						else if (!partialname.empty())
						{
							if (theaction && theaction->isResetAge())
							{
								_exhandler->raise(Exception::FMTexc::FMTwrong_partial, partialname + " at line " + std::to_string(m_line),
									"FMTactionparser::read", __LINE__, __FILE__, m_section);
							}
							const std::vector<std::string>splited = FMTparser::spliter(line, FMTparser::m_SEPARATOR);
							for (const std::string& val : splited)
							{
								theaction->pushPartials(val);
							}
						}
						else if (inseries)
						{
							const Core::FMTSerie NEW_SERIE = _getSerie(line, actions);
							if (!NEW_SERIE.isEmpty())
								{
								allseries.push_back(NEW_SERIE);
								}
						}
					}
				}
				for (Core::FMTaction& action : actions)
				{
					if (!action.empty())
					{
						action.shrink();
						action.setSeries(allseries);
						cleanedactions.push_back(action);
					}
					else {
						_exhandler->raise(Exception::FMTexc::FMTempty_action, 
							action.getName(),"FMTactionparser::read", __LINE__, __FILE__, m_section);
					}
				}
				std::map<std::string, std::vector<std::string>>cleanedag = valAgg(actions, aggregates);
				aggregates = cleanedag;
			}
			for (const auto& aggobj : aggregates)
			{
				for (Core::FMTaction& action : cleanedactions)
				{
					if (std::find(aggobj.second.begin(), aggobj.second.end(), action.getName()) != aggobj.second.end())
					{
						action.pushAggregate(aggobj.first);
					}
				//action.passinobject(*this);
				}
			}
			cleanedactions = getGCBMactionsaggregate(cleanedactions);
			cleanedactions.shrink_to_fit();
		}catch (...)
			{
			_exhandler->raiseFromCatch(
				"In "+m_location+" at line "+std::to_string(m_line),"FMTactionparser::read", __LINE__, __FILE__, m_section);
			}
        return cleanedactions;
        }

		Core::FMTSerie FMTactionparser::_getSerie(
			const std::string& p_line,
			const std::vector<Core::FMTaction>& p_actions) const
		{
			Core::FMTSerie returnedSerie;
			try {
				boost::smatch kmatch;
				if (!boost::regex_search(p_line, kmatch, FMTactionparser::m_SERIES_MATCH))
					{
					_exhandler->raise(Exception::FMTexc::FMTundefined_action,
						"Not a valid Serie on "+ p_line + std::to_string(m_line),
						"FMTactionparser::_getSerie", __LINE__, __FILE__, m_section);
					}
				const std::string BASE_SERIES = std::string(kmatch[1]) + std::string(kmatch[6]);
				const bool isASAP = !std::string(kmatch[4]).empty();
				const bool isALAP = !std::string(kmatch[5]).empty();
				std::vector<std::string>grossserie;
				const boost::regex SEP("(<-|->)");
				boost::algorithm::split_regex(grossserie, BASE_SERIES, SEP);
				std::vector<std::string>newserie;
				for (std::string& action : grossserie)
				{
					boost::trim(action);
					if (!action.empty())
					{
						if (std::find_if(p_actions.begin(), p_actions.end(),
							Core::FMTActionComparator(action)) != p_actions.end())
						{
							newserie.push_back(action);
						}
						else {
							_exhandler->raise(Exception::FMTexc::FMTundefined_action,
								action + " at line " + std::to_string(m_line), 
								"FMTactionparser::_getSerie", __LINE__, __FILE__, m_section);
						}

					}
				}
				if (newserie.size() > 1)
				{
					returnedSerie = Core::FMTSerie(newserie, isASAP, isALAP);
				}
			}catch (...)
			{
				_exhandler->raiseFromCatch(
					"", "FMTactionparser::FMTactionparser::_GetSerie", __LINE__, __FILE__, m_section);
			}
			return returnedSerie;
		}

		std::vector<Core::FMTaction>FMTactionparser::getGCBMactionsaggregate(const std::vector<Core::FMTaction>& actions) const
		{
			std::vector<Core::FMTaction>actionswithgcbmaggregate(actions);
			std::string onaction;
			std::string location;
			try {
				const boost::filesystem::path filelocation = boost::filesystem::path(getRuntimeLocation()) / boost::filesystem::path("YieldPredModels") / boost::filesystem::path("actionsmapping.json");
				std::ifstream jsonstream(filelocation.string());
				location = filelocation.string();
				if (FMTparser::tryOpening(jsonstream, filelocation.string()))
				{
					boost::property_tree::ptree root;
					boost::property_tree::read_json(jsonstream, root);
					for (Core::FMTaction& action : actionswithgcbmaggregate)
						{
						onaction = action.getName();
						if (root.find(action.getName())==root.not_found()||
							root.get_child(action.getName()).find("id") == root.get_child(action.getName()).not_found()||
							root.get_child(action.getName()).find("name") == root.get_child(action.getName()).not_found())
							{
							_exhandler->raise(Exception::FMTexc::FMTignore, "GCBM mapping for "+action.getName() + " at line " + std::to_string(m_line),
								"FMTactionparser::getactionsidsofmodelyields", __LINE__, __FILE__, Core::FMTsection::Action);
						}else {
							//test to int!
							const int idofaction = getNum<int>(root.get<std::string>(action.getName() + ".id"));
							if (idofaction== FMTGCBMGROWTHID || idofaction == FMTGCBMUNKNOWNID) //|| idofaction == FMTGCBMDEATHID)
								{
								_exhandler->raise(Exception::FMTinvalid_number,"cannot use GCBM actions id "+std::to_string(FMTGCBMGROWTHID)+" or "+ std::to_string(FMTGCBMUNKNOWNID) + " or " + std::to_string(FMTGCBMDEATHID) + " at line " + std::to_string(m_line),
									"FMTactionparser::getactionsidsofmodelyields", __LINE__, __FILE__, Core::FMTsection::Action);
								}
							const std::string GCBMaggregate = "~GCBM:" + root.get<std::string>(action.getName() + ".id") + ":" + root.get<std::string>(action.getName() + ".name");
							action.pushAggregate(GCBMaggregate);
							}
						}
					jsonstream.close();
				}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch(
					"In "+ location +" On action "+ onaction, "FMTactionparser::getGCBMactionsaggregates", __LINE__, __FILE__, m_section);
			}
			return actionswithgcbmaggregate;
		}


    void FMTactionparser::write(
		const std::vector<Core::FMTaction>& actions,
		const std::string& location,
		bool withgcbmagg) const
        {
		try {
			std::ofstream actionstream;
			actionstream.open(location);
			std::map<std::string, std::vector<std::string>>allaggregates;
			if (tryOpening(actionstream, location))
			{
				std::vector<Core::FMTSerie>series;
				for (const Core::FMTaction& act : actions)
				{
					actionstream << std::string(act) << "\n";
					for (const std::string& aggregate : act.getAggregates())
					{
						if (allaggregates.find(aggregate) == allaggregates.end())
						{
							allaggregates[aggregate] = std::vector<std::string>();
						}
						allaggregates[aggregate].push_back(act.getName());
					}
					if (act.isPartOfASerie())
						{
						for (const Core::FMTSerie& SERIE : act.getSeries())
							{
							series.push_back(SERIE);
							}
						}
				}
				actionstream << "\n";
				for (std::map<std::string, std::vector<std::string>>::const_iterator aggit = allaggregates.begin(); aggit != allaggregates.end(); aggit++)
				{
					const std::string name = aggit->first;
					if ((name.find("~GCBM") == std::string::npos) || (withgcbmagg))
					{
						actionstream << "*AGGREGATE " + name << "\n";
						for (const std::string& act_str : aggit->second)
						{
							actionstream << act_str << "\n";
						}
					}
				}
				if (!series.empty())
					{
					actionstream << "*ACTIONSERIES\n";
					for (const auto& serie : series)
						{
						actionstream << std::string(serie) << "\n";
						}
					}
				actionstream << "\n";
				actionstream.close();
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch(
				"","FMTactionparser::write", __LINE__, __FILE__, m_section);
			}
        }

    std::vector<Core::FMTaction*> FMTactionparser::sameActionAs(
		const std::string& all_set, 
		std::vector<Core::FMTaction>& actions) const
        {
		std::vector<Core::FMTaction*>all_pointers;
		try {
			const std::vector<std::string>response = sameAs(all_set);
			for(const std::string& actname : response)
				{
				all_pointers.push_back(&(*(std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator(actname)))));
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch(
				"","FMTactionparser::sameActionAs", __LINE__, __FILE__, m_section);
			}
        return all_pointers;
        }
}
