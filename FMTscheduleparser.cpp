/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTscheduleparser.h"

namespace Parser{

FMTscheduleparser::FMTscheduleparser():FMTparser()
    {

    }
FMTscheduleparser::FMTscheduleparser(const FMTscheduleparser& rhs):FMTparser(rhs)
    {
    }

int FMTscheduleparser::getvariable() const
	{
	int value = 0;
	if (!_comment.empty())
		{
		std::string strvalue = _comment;
		strvalue.erase(strvalue.begin() + 1);
		std::vector<std::string>strsources;
		boost::split(strsources, strvalue,boost::is_any_of(";\t "), boost::token_compress_on);
		value = getnum<int>(strsources.at(1));
		}
	return value;
	}
FMTscheduleparser& FMTscheduleparser::operator = (const FMTscheduleparser& rhs)
    {
    if(this!=&rhs)
        {
        FMTparser::operator=(rhs);
        }
    return *this;
    }
std::vector<Core::FMTschedule> FMTscheduleparser::read(const std::vector<Core::FMTtheme>& themes,const std::vector<Core::FMTaction>& actions,const std::string& location,double tolerance)
    {
	std::vector<Core::FMTschedule>schedules;
	try {
		std::ifstream schedulestream(location);
		if (FMTparser::tryopening(schedulestream, location))
		{
			std::vector<std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::map<int, double>>>>data;
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
						std::string mask = "";
						int id = 0;
						for (; id < static_cast<int>(themes.size()); ++id)
						{
							mask += values[id] + " ";
						}
						mask.pop_back();
						if (!validate(themes, mask, " at line " + std::to_string(_line))) continue;
						const int age = getnum<int>(values[id]);
						++id;
						const double area = getnum<double>(values[id]);
						if (area > tolerance)
						{
							++id;
							const std::string actionname = values[id];
							if (!isact(Core::FMTsection::Schedule, actions, actionname)) continue;
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
							Core::FMTdevelopment dev(Core::FMTmask(mask, themes), age, 0, period);
							dev.passinobject(*this);
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
				schedules.push_back(Core::FMTschedule(period, inschedule));
				schedules.back().passinobject(*this);
				++period;
			}
		}
	}catch (...)
		{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, _section, "in FMTscheduleparser::read", __LINE__, __FILE__);
		}
    return schedules;
    }


void FMTscheduleparser::write(const std::vector<Core::FMTschedule>& schedules,const std::string& location) const
    {
	std::ofstream schedulestream;
    schedulestream.open(location);
    if (tryopening(schedulestream,location))
        {
        for(const Core::FMTschedule& sch : schedules)
            {
            schedulestream<< std::string(sch);
            }
        schedulestream.close();
        }
    }

}
