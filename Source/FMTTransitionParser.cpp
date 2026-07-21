/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTransitionParser.h"
#include "FMTOperator.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "FMTMask.h"
#include "FMTGCBMTransition.h"
#include "FMTConstants.h"
#include "FMTTheme.h"
#include "FMTAction.h"
#include "FMTTransition.h"
#include "FMTTransitionMask.h"
#include "FMTYields.h"
#include "FMTExceptionHandler.h"


namespace Parser{

const boost::regex FMTTransitionParser::m_rxsection = boost::regex("^(\\*CASE)([\\s\\t]*)([^\\s^\\t]*)|(\\*SOURCE)([\\s\\t]*)(.+)|(\\*TARGET)([\\s\\t]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
const boost::regex FMTTransitionParser::m_rxlock = boost::regex("^(.+)(_LOCK)([\\s\\t]*)([0-9]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
const boost::regex FMTTransitionParser::m_rxage = boost::regex("^(.+)(_AGE)([\\s\\t]*)([0-9]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
const boost::regex FMTTransitionParser::m_rxreplace = boost::regex("^(.+)(_REPLACE)(....)([0-9]*)([\\s\\t]*)(\\,)([\\s\\t]*)(_TH)([0-9]*)([\\s\\t]*)([\\+\\-\\*\\/])([\\s\\t]*)([0-9]*)(.+)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);
const boost::regex FMTTransitionParser::m_rxtyld = boost::regex("^([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([^\\s^\\t]*)", boost::regex_constants::ECMAScript | boost::regex_constants::icase);

FMTTransitionParser::FMTTransitionParser():FMTParser()
    {
	setSection(Core::FMTsection::Transition);
    }


Core::FMTMask FMTTransitionParser::getSource(std::string& line, Core::FMTSpec& spec,const std::vector<Core::FMTTheme>& themes, Core::FMTsection section,const Core::FMTConstants& constants,const Core::FMTYields& ylds)
    {
	try {
		const std::vector<std::string>elements = FMTParser::spliter(line, FMTParser::m_SEPARATOR);
		if (elements.size() == themes.size())
		{
			Core::FMTTheme::validate(themes, line, " at line " + std::to_string(m_line));
			return Core::FMTMask(line, themes);
		}
		else {
			std::string mask;
			std::string rest = " ";
			for (size_t theme = 0; theme < elements.size(); ++theme)
			{
				if (theme < themes.size())
				{
					mask += elements[theme] + " ";
				}
				else {
					rest += elements[theme] + " ";
				}
			}
			mask = mask.substr(0, mask.size() - 1);
			rest = rest.substr(0, rest.size() - 1);
			Core::FMTTheme::validate(themes, mask, " at line " + std::to_string(m_line));
			const Core::FMTMask newmask(mask, themes);
			rest += " ";
			rest = setSpec(Core::FMTsection::Transition, Core::FMTkwor::Source, ylds, constants, spec, rest);
			return newmask;
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("at line " + line,"FMTTransitionParser::getSource", __LINE__, __FILE__, m_section);
		}
	return Core::FMTMask();
    }

std::vector<Core::FMTTransitionMask> FMTTransitionParser::getMaskTran(const std::string& line,const std::vector<Core::FMTTheme>& themes,
                                          const Core::FMTConstants& constants, const Core::FMTYields& ylds,
                                          const Core::FMTMask& sourcemask, int& replaced)
    {
	std::vector<Core::FMTTransitionMask>alltrans;
	try {
		const std::vector<std::string>elements = FMTParser::spliter(line, FMTParser::m_SEPARATOR);
		std::vector<Core::FMTMask>multiples;
		std::string mask = "";
		double proportion;
		size_t id = 1;
		while (id < (themes.size() + 1))
		{
			mask += elements[id] + " ";
			++id;
		}
		mask = mask.substr(0, mask.size() - 1);
		Core::FMTTheme::validate(themes, mask, " at line " + std::to_string(m_line));
		proportion = getNum<double>(elements[id], constants);
		++id;
		std::string rest = " ";
		while (id < elements.size())
		{
			rest += elements[id] + " ";
			++id;
		}
		boost::smatch kmatch;
		int age = -1;
		int lock = 0;
		Core::FMTTransitionMask trans(mask, themes, proportion);
		if (boost::regex_search(rest, kmatch, FMTTransitionParser::m_rxlock))
		{
			const std::string strlock = kmatch[4];
			lock = getNum<int>(strlock, constants);
			rest = std::string(kmatch[1]) + std::string(kmatch[5]);
			trans.addBounds(Core::FMTLockBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, lock, lock));
		}
		if (boost::regex_search(rest, kmatch, FMTTransitionParser::m_rxage))
		{
			std::string strage = kmatch[4];
			age = getNum<int>(strage, constants);
			rest = std::string(kmatch[1]) + std::string(kmatch[5]);
			trans.addBounds(Core::FMTAgeBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, age, age));
		}
		if (boost::regex_search(rest, kmatch, FMTTransitionParser::m_rxreplace))
		{
			const std::string strtargettheme = kmatch[4];
			const std::string stroptheme = kmatch[9];
			const std::string stroperator = kmatch[11];
			Core::FMTOperator baseoperator;
			if (!stroperator.empty())
				{
				baseoperator = Core::FMTOperator(stroperator);
				}
			const std::string stradd = kmatch[13];
			const int targetTheme = getNum<int>(strtargettheme) - 1;
			const double addupp = getNum<double>(stradd);
			Core::FMTMask targetmask(mask, themes);
			targetmask.set(themes[targetTheme], sourcemask.get(themes[targetTheme]));
			for (Core::FMTMask& lmask : targetmask.decompose(themes[targetTheme]))
			{
				
				const std::string actual = lmask.get(themes[targetTheme]);
				std::string newval;
				if (isNum(actual))//just math
					{
					const int newint =  static_cast<int>(baseoperator.call(getNum<double>(actual),addupp));
					newval = std::to_string(newint);
				}else {
					newval = actual+ stradd;
					}
				if (themes[targetTheme].isAttribute(newval))
				{
					lmask.set(themes[targetTheme], newval);
					multiples.push_back(lmask);
				}else {
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"_REPLACE generated " + newval+" THEME("+std::string(strtargettheme)+") not in landscape", "FMTTransitionParser::getMaskTran", __LINE__, __FILE__,m_section);
				}

			}
			
			replaced = targetTheme;
			rest = std::string(kmatch[1]) + std::string(kmatch[14]);
		}
		if (isValid(rest) && boost::regex_search(rest, kmatch, FMTTransitionParser::m_rxtyld))
		{
			const std::string yld = kmatch[2];
			const std::string strvalue = kmatch[4];
			if (!yld.empty() && !strvalue.empty())
			{
				constexpr double upperbound = std::numeric_limits<double>::max();
				const double lowerbound = getNum<double>(strvalue, constants);
				isYld(ylds, yld, Core::FMTsection::Transition);
				trans.addBounds(Core::FMTYldBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, yld, upperbound, lowerbound));
			}
		}

		if (!multiples.empty())
		{
			for (const Core::FMTMask& msk : multiples)
			{
				alltrans.push_back(Core::FMTTransitionMask(trans, msk, themes));
			}
		}
		else {
			alltrans.push_back(trans);
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("at line " + line,"FMTTransitionParser::getMaskTran", __LINE__, __FILE__, m_section);
		}
    return alltrans;
    }


std::vector<Core::FMTTransition> FMTTransitionParser::read(const std::vector<Core::FMTTheme>& themes,const std::vector<Core::FMTAction>& actions,const Core::FMTYields& ylds,const Core::FMTConstants& constants,const std::string& location)
    {
	std::vector<Core::FMTTransition>transitions;
	try {
		std::ifstream transitionstream(location);
		std::vector<Core::FMTTransition>temp_transitions;
		if (FMTParser::tryOpening(transitionstream, location))
		{
			std::string CASE;
			std::string SOURCE;
			std::string TARGET;
			std::string actionname;
			std::vector<Core::FMTFork*>fptrs;
			std::vector<int>fork_ids;
			std::vector<int>replacedvec;
			Core::FMTMask srcmsk;
			std::vector<Core::FMTTransition>::iterator last_transition = temp_transitions.end();
			std::queue<FMTParser::FMTLineInfo>Lines = FMTParser::getCleanLinewfor(transitionstream, themes, constants);
			while (!Lines.empty())
			{
				const std::string line = getLine(Lines);
				if (!line.empty())
				{
					
					boost::smatch kmatch;
					if (!boost::regex_search(line, kmatch, FMTTransitionParser::m_rxsection))
					{
						//crash here
					}
					CASE = kmatch[3];
					SOURCE = kmatch[4];
					TARGET = kmatch[7];
					if (!CASE.empty())
					{
						const std::vector<std::string>ptransitionname = sameAs(CASE);
						actionname = ptransitionname.at(0);
						temp_transitions.push_back(Core::FMTTransition(actionname));
						last_transition = --temp_transitions.end();
						if (ptransitionname.size() > 1)
						{
							std::vector<Core::FMTTransition>::const_iterator same_tr = find_if(temp_transitions.begin(), temp_transitions.end(), Core::FMTTransitionComparator(ptransitionname.at(1)));
							for (const auto& forkobj : *same_tr)
							{
								last_transition->push_back(forkobj.first,forkobj.second);
							}
						}
						if (!isAct(Core::FMTsection::Transition, actions, actionname)) continue;
					}
					else if (!SOURCE.empty())
					{
						std::string data;
						data = kmatch[6];
						Core::FMTFork fork;
						srcmsk = getSource(data, fork, themes, Core::FMTsection::Transition, constants, ylds);
						
						last_transition->push_back(srcmsk, fork);
						fptrs.clear();
						fork_ids.clear();
						replacedvec.clear();
						fptrs.push_back(&(--last_transition->end())->second);
						fork_ids.push_back(int(last_transition->size()) - 1);
					}
					else if (!TARGET.empty())
					{
						int replaced = -1;
						
						std::vector<Core::FMTTransitionMask>mtrs = getMaskTran(line, themes, constants, ylds, srcmsk, replaced);
						if (replaced > -1)
						{
							for (const int& rep : replacedvec)
							{
								if (rep != replaced)
								{
									_exhandler->raise(Exception::FMTexc::FMTunsupported_transition,
										actionname + " at line " + std::to_string(m_line),
										"FMTTransitionParser::read", __LINE__, __FILE__, m_section);
								}
							}
							replacedvec.push_back(replaced);
						}

						if (replaced == -1 || fork_ids.size() > 1)
						{
							int repid = 0;
							for (const int& id : fork_ids)
							{
								if (replaced == -1)
								{
									(last_transition->begin() + id)->second.add(mtrs[0]);
								}
								else {
									(last_transition->begin() + id)->second.add(mtrs[repid]);
								}

								++repid;
							}
						}
						else {

							std::vector<Core::FMTFork*>newstack;
							std::vector<int>new_ids;
							Core::FMTFork basefork;
							basefork = *fptrs[0];
							if (!last_transition->empty())
							{
								last_transition->pop_back();
							}
							const std::vector<Core::FMTMask>multisourcesmask = srcmsk.decompose(themes[replaced]);
							for (size_t id = 0; id < mtrs.size(); ++id)
							{
								last_transition->push_back(multisourcesmask[id], basefork);
								Core::FMTFork* newfptr = &((--last_transition->end())->second);
								newfptr->add(mtrs[id]);
								newstack.push_back(newfptr);
								new_ids.push_back(static_cast<int>(last_transition->size()) - 1);
							}
							fptrs = newstack;
							fork_ids = new_ids;
						}
					}
				}
			}

			for (const Core::FMTTransition& transition : temp_transitions)
			{
				if (!transition.empty())
				{
					if (transition.isLeaking())
					{
						_exhandler->raise(Exception::FMTexc::FMTleakingtransition,
							transition.getName(),"FMTTransitionParser::read", __LINE__, __FILE__, m_section);
					}
					else {
						transitions.push_back(transition);
						
					}
				}

			}
			//std::sort(transitions.begin(), transitions.end());
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("In " + m_location + " at line " + std::to_string(m_line),"FMTTransitionParser::read", __LINE__, __FILE__, m_section);
		}
    return transitions;
    }

void FMTTransitionParser::write(const std::vector<Core::FMTTransition>& transitions,const std::string& location) const
    {
	try {
		std::ofstream transitionstream;
		transitionstream.open(location);
		if (tryOpening(transitionstream, location))
		{
			for (const Core::FMTTransition& tra : transitions)
			{
				transitionstream << std::string(tra) << "\n";
			}
			transitionstream.close();
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("at "+location,"FMTTransitionParser::write", __LINE__, __FILE__, m_section);
		}
    }

void FMTTransitionParser::writeGCBM(const std::vector<Core::FMTGCBMTransition>& transitions, const std::string& location) const
	{
	try{
		std::ofstream transitionstream;
		transitionstream.open(location);
		if (tryOpening(transitionstream, location)&& !transitions.empty())
		{
			transitionstream << "22 serialization::archive 17 0 0 " + std::to_string(transitions.size())+" 0 0 0 ";
			transitionstream << std::to_string(transitions.at(0).ageafter) << " 0 0 ";
			transitionstream << std::to_string(transitions.at(0).themes.size()) << " 0 0 0 ";
			for (const auto& telement : transitions.at(0).themes)
			{
				transitionstream << std::to_string(telement.first.size()) << " " << telement.first <<" ";
				transitionstream << std::to_string(telement.second.size()) << " " << telement.second << " ";
			}
			transitionstream << transitions.at(0).name.size() << " " << transitions.at(0).name;
			if (transitions.size()>1)
			{
				transitionstream << " ";
			}
			for (size_t trid = 1; trid < transitions.size();++trid)
				{
				transitionstream << std::to_string(transitions.at(trid).ageafter) << " "<< std::to_string(transitions.at(trid).themes.size()) +" 0 ";
				for (const auto& telement : transitions.at(trid).themes)
					{
					transitionstream << std::to_string(telement.first.size()) << " " << telement.first << " ";
					transitionstream << std::to_string(telement.second.size()) << " " << telement.second << " ";
					}
				transitionstream << transitions.at(trid).name.size() << " " << transitions.at(trid).name;
				}
			
			//"22 serialization::archive 17 0 0 "+vectorsize+" 0 0 0 "+ageafter+" 0 0 "+ +dictsize +" 0 0 0 "+ " keysize "+key +" elementsize "+element + " namesize " + name + ageafter + dictsize + " 0 " + ...

			//boost::archive::text_oarchive transitionsarchive(transitionstream);
			//transitionsarchive << BOOST_SERIALIZATION_NVP(transitions);
			transitionstream.close();
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("at " + location, "FMTTransitionParser::writeGCBM", __LINE__, __FILE__, m_section);
	}

	}

std::vector<Core::FMTGCBMTransition>FMTTransitionParser::readGCBM(const std::string& location) const
	{
	std::vector<Core::FMTGCBMTransition>transitions;
	try {
		std::ifstream transitionstream;
		transitionstream.open(location);
		if (tryOpening(transitionstream, location))
		{
			boost::archive::text_iarchive transitionsarchive(transitionstream);
			transitionsarchive >> BOOST_SERIALIZATION_NVP(transitions);
			transitionstream.close();
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("at " + location, "FMTTransitionParser::readGCBM", __LINE__, __FILE__, m_section);
	}
	return transitions;
	}

}
