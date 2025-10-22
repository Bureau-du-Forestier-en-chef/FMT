/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTscheduleparser.h"

#include <CoinTime.hpp>

#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTschedule.h"
#include "FMTexceptionhandler.h"

#include <boost/filesystem.hpp>

namespace Parser {

	FMTscheduleparser::FMTscheduleparser() :FMTparser()
	{
		setSection(Core::FMTsection::Schedule);
	}
	FMTscheduleparser::FMTscheduleparser(const FMTscheduleparser& rhs) : FMTparser(rhs)
	{
		setSection(Core::FMTsection::Schedule);
	}

	int FMTscheduleparser::getvariable() const
	{
		int value = 0;
		try {
			if (!m_comment.empty())
			{
				std::string strvalue = m_comment;
				strvalue.erase(strvalue.begin() + 1);
				std::vector<std::string>strsources;
				boost::split(strsources, strvalue, boost::is_any_of(";\t "), boost::token_compress_on);
				value = getNum<int>(strsources.at(1));
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch( "for comment " + m_comment,"FMTscheduleparser::getvariable", __LINE__, __FILE__, m_section);
		}
		return value;
	}

	std::string FMTscheduleparser::getSchedulePath(const std::string& p_primary_path, const std::string& p_output_scenario_name) 
	{
		std::string thePath;
		try {
			// On vérifie si le dossier Scenarios existe et si le fichier de schedule existe, sinon on écrit dans le root
			const boost::filesystem::path primpath(p_primary_path);
			const boost::filesystem::path primary_name = primpath.stem();
			const boost::filesystem::path basefolder = primpath.parent_path();
			// On construit notre path attendu et vérifie s'il existe dans le dossier Scenarios / notre_scenario
			boost::filesystem::path schedule_path = basefolder / "Scenarios" / boost::filesystem::path(p_output_scenario_name) / boost::filesystem::path(primary_name.string() + "._seq");

			if (!boost::filesystem::exists(schedule_path))
			{
				schedule_path = basefolder / boost::filesystem::path(primary_name.string() + ".seq");
			}

			thePath =  schedule_path.string();

		}catch (...)
		{
			_exhandler->printexceptions("for primary " + p_primary_path, "FMTscheduleparser::getSchedulePath", __LINE__, __FILE__, m_section);
		}
		return thePath;
	}

	FMTscheduleparser& FMTscheduleparser::operator = (const FMTscheduleparser& rhs)
	{
		if (this != &rhs)
		{
			FMTparser::operator=(rhs);
			setSection(Core::FMTsection::Schedule);
		}
		return *this;
	}
	std::vector<Core::FMTschedule> FMTscheduleparser::read(const std::vector<Core::FMTtheme>& themes,
		const std::vector<Core::FMTaction>& actions, const std::string& location, double tolerance)
	{
		std::vector<Core::FMTschedule>schedules;
		try {
			std::ifstream schedulestream(location);
			if (FMTparser::tryOpening(schedulestream, location))
			{
				std::vector<std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::map<int, double>>>>data;
				bool uselock = false;
				bool firstline = true;
				while (schedulestream.is_open())
				{
					std::string line = FMTparser::getCleanLine(schedulestream);
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
							if (!Core::FMTtheme::validate(themes, mask, " at line " + std::to_string(m_line))) continue;
							const int age = getNum<int>(values[id]);
							++id;
							const double area = getNum<double>(values[id]);
							if (area > tolerance)
							{
								++id;
								int lock = 0;
								if(uselock)
								{
									lock = getNum<int>(values[id]);
									++id;
								}
								const std::string actionname = values[id];
								if (!isAct(Core::FMTsection::Schedule, actions, actionname)) 
								{
									_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, 
									"The schedule must specify an action existing in the model for each developement. No action named " + actionname + " at line " + std::to_string(m_line),
									"FMTscheduleparser::read", __LINE__, __FILE__);
								}
								++id;
								const int period = getNum<int>(values[id]);
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
			_exhandler->raisefromcatch("In " + m_location + " at line " + std::to_string(m_line),"FMTscheduleparser::read", __LINE__, __FILE__, m_section);
		}
		return schedules;
	}

	void FMTscheduleparser::_writeSchedule(std::ofstream& p_stream, const std::vector<Core::FMTschedule>& p_schedules)
	{
		for (const Core::FMTschedule& sch : p_schedules)
		{
			if (!sch.empty())
			{
				p_stream << std::string(sch);
			}
		}
	}

	std::vector<Core::FMTschedule>::const_iterator FMTscheduleparser::_getFirstEmptySchedule(const std::vector<Core::FMTschedule>& p_schedules)
	{
		std::vector<Core::FMTschedule>::const_iterator firstnonemptyschedule = p_schedules.begin();
		while (firstnonemptyschedule != p_schedules.end() && firstnonemptyschedule->empty())
		{
			++firstnonemptyschedule;
		}

		std::vector<Core::FMTschedule>::const_iterator result = p_schedules.end();

		if (!p_schedules.empty() && firstnonemptyschedule != p_schedules.cend() &&
			(!firstnonemptyschedule	-> empty()) &&
			(!firstnonemptyschedule	-> begin() -> second.empty()) &&
			(!firstnonemptyschedule	-> begin() -> second.begin() -> second.empty()))
		{
			result = firstnonemptyschedule;
		}
		return result;
	}

	void FMTscheduleparser::write(const std::vector<Core::FMTschedule>& schedules,
		const std::string& location, bool append) const
	{
		try {
			std::ofstream schedulestream;
			// test Gabriel 2024-07-05
			if (append)
			{
				schedulestream.open(location, std::ios::app);  // Open for append
			}
			else
			{
				schedulestream.open(location);  // Open normaly (overwrite file)
			}

			if (tryOpening(schedulestream, location))
			{
				const auto firstnonemptyschedule = _getFirstEmptySchedule(schedules);
				if (firstnonemptyschedule != schedules.end())
				{
					if (!append) // Write header only if not exist, modify after new feature append
					{ 
						const std::string maskstr = std::string(
							firstnonemptyschedule -> begin() -> second.begin() -> first.getmask());
						std::vector<std::string> splittedmask;
						const std::string forstrsep = FMT_STR_SEPARATOR;
						boost::split(splittedmask, maskstr, boost::is_any_of(forstrsep),
							boost::token_compress_on);
						schedulestream << ";";
						for (size_t thid = 0; thid < splittedmask.size(); ++thid)
						{
							schedulestream << "TH" + std::to_string(thid + 1) + " ";
						}
						schedulestream << "AGE AREA ";
						if (firstnonemptyschedule -> douselock())
						{
							schedulestream << "LOCK ";
						}
						schedulestream << "ACTION PERIOD\n";
					}
					// Only portion that write
					_writeSchedule(schedulestream, schedules);
				}
				schedulestream.close();
			}
		}
		catch (...)
		{
			_exhandler->printexceptions("at " + location, "FMTscheduleparser::write", __LINE__, __FILE__, m_section);
		}
	}

}