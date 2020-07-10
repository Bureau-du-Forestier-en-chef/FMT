/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtransitionparser.h"


namespace Parser{

FMTtransitionparser::FMTtransitionparser():FMTparser(),
    rxsection("^(\\*CASE)([\\s\\t]*)([^\\s^\\t]*)|(\\*SOURCE)([\\s\\t]*)(.+)|(\\*TARGET)([\\s\\t]*)(.+)", std::regex_constants::ECMAScript| std::regex_constants::icase),
    rxlock("^(.+)(_LOCK)([\\s\\t]*)([0-9]*)(.+)", std::regex_constants::ECMAScript| std::regex_constants::icase),
    rxage("^(.+)(_AGE)([\\s\\t]*)([0-9]*)(.+)", std::regex_constants::ECMAScript| std::regex_constants::icase),
    rxreplace("^(.+)(_REPLACE)(....)([0-9]*)([\\s\\t]*)(\\,)([\\s\\t]*)(_TH)([0-9]*)([\\s\\t]*)(\\+)([\\s\\t]*)([0-9]*)(.+)", std::regex_constants::ECMAScript| std::regex_constants::icase),
    rxtyld("^([\\s\\t]*)([^\\s^\\t]*)([\\s\\t]*)([^\\s^\\t]*)", std::regex_constants::ECMAScript| std::regex_constants::icase)
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

Core::FMTmask FMTtransitionparser::getsource(std::string& line, Core::FMTspec& spec,const std::vector<Core::FMTtheme>& themes, Core::FMTsection section,const Core::FMTconstants& constants,const Core::FMTyields& ylds)
    {
	try {
		const std::vector<std::string>elements = FMTparser::spliter(line, FMTparser::rxseparator);
		if (elements.size() == themes.size())
		{
			validate(themes, line, " at line " + std::to_string(_line));
			return Core::FMTmask(line, themes);
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
			validate(themes, mask, " at line " + std::to_string(_line));
			const Core::FMTmask newmask(mask, themes);
			rest += " ";
			rest = setspec(Core::FMTsection::Transition, Core::FMTkwor::Source, ylds, constants, spec, rest);
			return newmask;
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("at line " + line,"FMTtransitionparser::getsource", __LINE__, __FILE__, _section);
		}
	return Core::FMTmask();
    }

std::vector<Core::FMTtransitionmask> FMTtransitionparser::getmasktran(const std::string& line,const std::vector<Core::FMTtheme>& themes,
                                          const Core::FMTconstants& constants, const Core::FMTyields& ylds,
                                          const Core::FMTmask& sourcemask, int& replaced)
    {
	std::vector<Core::FMTtransitionmask>alltrans;
	try {
		const std::vector<std::string>elements = FMTparser::spliter(line, FMTparser::rxseparator);
		std::vector<Core::FMTmask>multiples;
		std::string mask = "";
		double proportion;
		size_t id = 1;
		while (id < (themes.size() + 1))
		{
			mask += elements[id] + " ";
			++id;
		}
		mask = mask.substr(0, mask.size() - 1);
		validate(themes, mask, " at line " + std::to_string(_line));
		proportion = getnum<double>(elements[id], constants);
		++id;
		std::string rest = " ";
		while (id < elements.size())
		{
			rest += elements[id] + " ";
			++id;
		}
		std::smatch kmatch;
		int age = -1;
		int lock = 0;
		Core::FMTtransitionmask trans(mask, themes, proportion);
		if (std::regex_search(rest, kmatch, FMTtransitionparser::rxlock))
		{
			const std::string strlock = kmatch[4];
			lock = getnum<int>(strlock, constants);
			rest = std::string(kmatch[1]) + std::string(kmatch[5]);
			trans.addbounds(Core::FMTlockbounds(Core::FMTsection::Transition, Core::FMTkwor::Target, lock, lock));
		}
		if (std::regex_search(rest, kmatch, FMTtransitionparser::rxage))
		{
			std::string strage = kmatch[4];
			age = getnum<int>(strage, constants);
			rest = std::string(kmatch[1]) + std::string(kmatch[5]);
			trans.addbounds(Core::FMTagebounds(Core::FMTsection::Transition, Core::FMTkwor::Target, age, age));
		}
		if (std::regex_search(rest, kmatch, FMTtransitionparser::rxreplace))
		{
			const std::string strtargettheme = kmatch[4];
			const std::string stroptheme = kmatch[9];
			const std::string stradd = kmatch[13];
			const int targettheme = getnum<int>(strtargettheme) - 1;
			const int addupp = getnum<int>(stradd);
			Core::FMTmask targetmask(mask, themes);
			targetmask.set(themes[targettheme], sourcemask.get(themes[targettheme]));
			for (Core::FMTmask& lmask : targetmask.decompose(themes[targettheme]))
			{
				const std::string actual = lmask.get(themes[targettheme]);
				int newint = getnum<int>(actual) + addupp;
				const std::string newval = std::to_string(newint);
				if (themes[targettheme].isattribute(newval))
				{
					lmask.set(themes[targettheme], newval);
					multiples.push_back(lmask);
				}
			}
			replaced = targettheme;
			rest = std::string(kmatch[1]) + std::string(kmatch[14]);
		}
		if (isvalid(rest) && std::regex_search(rest, kmatch, FMTtransitionparser::rxtyld))
		{
			const std::string yld = kmatch[2];
			const std::string strvalue = kmatch[4];
			if (!yld.empty() && !strvalue.empty())
			{
				constexpr double upperbound = std::numeric_limits<double>::max();
				const double lowerbound = getnum<double>(strvalue, constants);
				isyld(ylds, yld, Core::FMTsection::Transition);
				trans.addbounds(Core::FMTyldbounds(Core::FMTsection::Transition, Core::FMTkwor::Target, yld, upperbound, lowerbound));
			}
		}

		if (!multiples.empty())
		{
			for (const Core::FMTmask& msk : multiples)
			{
				alltrans.push_back(Core::FMTtransitionmask(trans, msk, themes));
			}
		}
		else {
			alltrans.push_back(trans);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("at line " + line,"FMTtransitionparser::getmasktran", __LINE__, __FILE__, _section);
		}
    return alltrans;
    }


std::vector<Core::FMTtransition> FMTtransitionparser::read(const std::vector<Core::FMTtheme>& themes,const std::vector<Core::FMTaction>& actions,const Core::FMTyields& ylds,const Core::FMTconstants& constants,const std::string& location)
    {
	std::vector<Core::FMTtransition>transitions;
	try {
		std::ifstream transitionstream(location);
		std::vector<Core::FMTtransition>temp_transitions;
		if (FMTparser::tryopening(transitionstream, location))
		{
			std::string CASE;
			std::string SOURCE;
			std::string TARGET;
			std::string actionname;
			std::vector<Core::FMTfork*>fptrs;
			std::vector<int>fork_ids;
			std::vector<int>replacedvec;
			Core::FMTmask srcmsk;
			std::vector<Core::FMTtransition>::iterator last_transition = temp_transitions.end();
			while (transitionstream.is_open())
			{
				std::string line = getcleanlinewfor(transitionstream, themes, constants);
				if (!line.empty())
				{
					std::smatch kmatch;
					if (!std::regex_search(line, kmatch, FMTtransitionparser::rxsection))
					{
						//crash here
					}
					CASE = kmatch[3];
					SOURCE = kmatch[4];
					TARGET = kmatch[7];
					if (!CASE.empty())
					{
						const std::vector<std::string>ptransitionname = sameas(CASE);
						actionname = ptransitionname.at(0);
						temp_transitions.push_back(Core::FMTtransition(actionname));
						last_transition = --temp_transitions.end();
						if (ptransitionname.size() > 1)
						{
							std::vector<Core::FMTtransition>::const_iterator same_tr = find_if(temp_transitions.begin(), temp_transitions.end(), Core::FMTtransitioncomparator(ptransitionname.at(1)));
							for (const auto& forkobj : *same_tr)
							{
								last_transition->push_back(forkobj);
							}
						}
						if (!isact(Core::FMTsection::Transition, actions, actionname)) continue;
					}
					else if (!SOURCE.empty())
					{
						std::string data;
						data = kmatch[6];
						Core::FMTfork fork;
						srcmsk = getsource(data, fork, themes, Core::FMTsection::Transition, constants, ylds);
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
						std::vector<Core::FMTtransitionmask>mtrs = getmasktran(line, themes, constants, ylds, srcmsk, replaced);
						if (replaced > -1)
						{
							for (const int& rep : replacedvec)
							{
								if (rep != replaced)
								{
									_exhandler->raise(Exception::FMTexc::FMTunsupported_transition,
										actionname + " at line " + std::to_string(_line),
										"FMTtransitionparser::read", __LINE__, __FILE__, _section);
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

							std::vector<Core::FMTfork*>newstack;
							std::vector<int>new_ids;
							Core::FMTfork basefork;
							basefork = *fptrs[0];
							if (!last_transition->empty())
							{
								last_transition->pop_back();
							}
							const std::vector<Core::FMTmask>multisourcesmask = srcmsk.decompose(themes[replaced]);
							for (size_t id = 0; id < mtrs.size(); ++id)
							{
								last_transition->push_back(multisourcesmask[id], basefork);
								Core::FMTfork* newfptr = &((--last_transition->end())->second);
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

			for (const Core::FMTtransition& transition : temp_transitions)
			{
				if (!transition.empty())
				{
					if (transition.isleaking())
					{
						_exhandler->raise(Exception::FMTexc::FMTleakingtransition,
							transition.getname(),"FMTtransitionparser::read", __LINE__, __FILE__, _section);
					}
					else {
						transitions.push_back(transition);
						transitions.back().passinobject(*this);
					}
				}

			}
			std::sort(transitions.begin(), transitions.end());
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTtransitionparser::read", __LINE__, __FILE__, _section);
		}
    return transitions;
    }

void FMTtransitionparser::write(const std::vector<Core::FMTtransition>& transitions,const std::string& location) const
    {
	try {
		std::ofstream transitionstream;
		transitionstream.open(location);
		if (tryopening(transitionstream, location))
		{
			for (const Core::FMTtransition& tra : transitions)
			{
				transitionstream << std::string(tra) << "\n";
			}
			transitionstream.close();
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("at "+location,"FMTtransitionparser::write", __LINE__, __FILE__, _section);
		}
    }

}
