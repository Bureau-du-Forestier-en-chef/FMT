/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTscheduleparser.h"
#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTschedule.h"
#include "FMTexceptionhandler.h"

namespace Parser {

	FMTscheduleparser::FMTscheduleparser() :FMTparser()
	{
		setsection(Core::FMTsection::Schedule);
	}
	FMTscheduleparser::FMTscheduleparser(const FMTscheduleparser& rhs) : FMTparser(rhs)
	{
		setsection(Core::FMTsection::Schedule);
	}

	int FMTscheduleparser::getvariable() const
	{
		int value = 0;
		try {
			if (!_comment.empty())
			{
				std::string strvalue = _comment;
				strvalue.erase(strvalue.begin() + 1);
				std::vector<std::string>strsources;
				boost::split(strsources, strvalue, boost::is_any_of(";\t "), boost::token_compress_on);
				value = getnum<int>(strsources.at(1));
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch( "for comment " + _comment,"FMTscheduleparser::getvariable", __LINE__, __FILE__, _section);
		}
		return value;
	}

	

	



	FMTscheduleparser& FMTscheduleparser::operator = (const FMTscheduleparser& rhs)
	{
		if (this != &rhs)
		{
			FMTparser::operator=(rhs);
			setsection(Core::FMTsection::Schedule);
		}
		return *this;
	}
	std::vector<Core::FMTschedule> FMTscheduleparser::read(const std::vector<Core::FMTtheme>& themes,
		const std::vector<Core::FMTaction>& actions, const std::string& location, double tolerance)
	{
		std::vector<Core::FMTschedule>schedules;
		try {
			std::ifstream schedulestream(location);
			if (FMTparser::tryopening(schedulestream, location))
			{
				std::vector<std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::map<int, double>>>>data;
				bool uselock = false;
				bool firstline = true;
				while (schedulestream.is_open())
				{
					std::string line = FMTparser::getcleanline(schedulestream);
					if (!line.empty())
					{
						std::vector<std::string>values;
						boost::split(values, line, boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
						if (values.size() < themes.size() || line.find("*STRATA") != std::string::npos)
						{
							break;
						}
						else {
							int variable = getvariable();
							if (firstline&&line.find("_FUTURE")==std::string::npos&&
								line.find("_EXISTING")==std::string::npos&&
								(values.size()-themes.size())==5)
								{
								uselock = true;
								}
							firstline = false;
							std::string mask = "";
							int id = 0;
							for (; id < static_cast<int>(themes.size()); ++id)
							{
								mask += values[id] + " ";
							}
							mask.pop_back();
							if (!Core::FMTtheme::validate(themes, mask, " at line " + std::to_string(_line))) continue;
							const int age = getnum<int>(values[id]);
							++id;
							const double area = getnum<double>(values[id]);
							if (area > tolerance)
							{
								++id;
								int lock = 0;
								if(uselock)
								{
									lock = getnum<int>(values[id]);
									++id;
								}
								const std::string actionname = values[id];
								if (!isact(Core::FMTsection::Schedule, actions, actionname)) 
								{
									_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, 
									"The schedule must specify an action existing in the model for each developement. No action named " + actionname + " at line " + std::to_string(_line),
									"FMTscheduleparser::read", __LINE__, __FILE__);
								}
								++id;
								const int period = getnum<int>(values[id]);
								if (static_cast<size_t>(period) - 1 == data.size())
								{
									data.push_back(std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::map<int, double>>>());
								}
								else if (static_cast<size_t>(period) - 1 > data.size())
								{
									int gap = (period - 1) - static_cast<int>(data.size());
									while (gap >= 0)
									{
										data.push_back(std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::map<int, double>>>());
										--gap;
									}
								}
								Core::FMTdevelopment dev(Core::FMTmask(mask, themes), age, lock, period);
								//dev.passinobject(*this);
								std::vector<Core::FMTaction>::const_iterator act = find_if(actions.begin(), actions.end(), Core::FMTactioncomparator(actionname));
								if (act->dorespectlock())
								{
									variable = 0;
								}
								const int periodloc = period - 1;
								if (data[periodloc].find(*act) == data[periodloc].end())
								{
									data[periodloc][*act] = std::map<Core::FMTdevelopment, std::map<int, double>>();
								}
								if (data[periodloc][*act][dev].find(variable) != data[periodloc][*act][dev].end())
								{
									data[periodloc][*act][dev][variable] += area;
								}
								else {
									data[periodloc][*act][dev][variable] = area;
								}
							}
						}
					}
				}

				int period = 1;
				for (const std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::map<int, double>>>& inschedule : data)
				{
					schedules.emplace_back(period, inschedule);
					//schedules.back().passinobject(*this);
					schedules.back().setuselock(uselock);
					++period;
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("In " + _location + " at line " + std::to_string(_line),"FMTscheduleparser::read", __LINE__, __FILE__, _section);
		}
		return schedules;
	}


	void FMTscheduleparser::write(const std::vector<Core::FMTschedule>& schedules, const std::string& location) const
	{
		try {
			std::ofstream schedulestream;
			schedulestream.open(location);
			if (tryopening(schedulestream, location))
			{
				//Write an header
				std::vector<Core::FMTschedule>::const_iterator firstnonemptyschedule = schedules.begin();
				while (firstnonemptyschedule!= schedules.end()&&firstnonemptyschedule->empty())
					{
					++firstnonemptyschedule;
					}
				if (!schedules.empty() && firstnonemptyschedule!= schedules.cend() && (!firstnonemptyschedule->empty()) && (!firstnonemptyschedule->begin()->second.empty()) &&
					(!firstnonemptyschedule->begin()->second.begin()->second.empty()))
				{
					const std::string maskstr = std::string(firstnonemptyschedule->begin()->second.begin()->first.getmask());
					std::vector<std::string>splittedmask;
					const std::string forstrsep = FMT_STR_SEPARATOR;
					boost::split(splittedmask, maskstr, boost::is_any_of(forstrsep), boost::token_compress_on);
					schedulestream << ";";
					for (size_t thid = 0; thid < splittedmask.size(); ++thid)
					{
						schedulestream << "TH" + std::to_string(thid + 1) + " ";
					}
					schedulestream << "AGE AREA ";
					if (firstnonemptyschedule->douselock())
					{
						schedulestream << "LOCK ";
					}
					schedulestream << "ACTION PERIOD\n";
					for (const Core::FMTschedule& sch : schedules)
					{
						if (!sch.empty())
						{
							schedulestream << std::string(sch);
						}
						
					}
				}
				schedulestream.close();
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("at " + location,"FMTscheduleparser::write", __LINE__, __FILE__, _section);
		}
	}

}