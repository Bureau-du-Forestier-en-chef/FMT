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

#include "FMTscheduleparser.h"

namespace WSParser{

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
		string strvalue = _comment;
		strvalue.erase(strvalue.begin() + 1);
		vector<string>strsources;
		boost::split(strsources, strvalue,boost::is_any_of(";\t "), boost::token_compress_on);
		value = stoi(strsources.at(1));
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
vector<FMTschedule> FMTscheduleparser::read(const vector<FMTtheme>& themes,const vector<FMTaction>& actions,string location,double tolerance)
    {
    ifstream schedulestream(location);
    string line;
	vector<FMTschedule>schedules;
    if (FMTparser::tryopening(schedulestream,location))
        {
		vector<map<FMTaction, map<FMTdevelopment, map<int, double>>>>data;
        while(schedulestream.is_open())
            {
            line = FMTparser::getcleanline(schedulestream);
            if (!line.empty())
                {
                vector<string>values;
                boost::split(values,line,boost::is_any_of("\t "),boost::token_compress_on);
                if(values.size() < themes.size()||line.find("*STRATA")!=string::npos)
                    {
					
                    /*if (!data.empty())
                        {
                        schedules.push_back(FMTschedule(actualperiod,data));
                        data.clear();
                        }*/
                    break;
                    }else{
						int variable = getvariable(); //from comment...
                        string mask = "";
                        int id = 0;
                        for(;id < int(themes.size());++id)
                            {
                            mask+=values[id]+" ";
                            }
                        mask.pop_back();
                        if (!validate(themes, mask)) continue;
						const int age = getnum<int>(values[id]);//stoi(values[id]);
                        ++id;
                        const double area = getnum<double>(values[id]);
						if (area > tolerance) // Weird stuff non basis solution in schedule...
							{
							++id;
							const string actionname = values[id];
						

							if (!isact(FMTwssect::Schedule,actions,actionname)) continue;
							++id;
							const int period = getnum<int>(values[id]);
							/*if (actionname == "ACFP" && period == 2)
							{
								ACFPread += area;
							}*/
							if (period -1 == data.size())
								{
								data.push_back(map<FMTaction, map<FMTdevelopment, map<int,double>>>());
							}else if (period - 1 > data.size())
								{
								int gap = (period - 1) - int(data.size());
								while (gap >= 0)
									{
									data.push_back(map<FMTaction, map<FMTdevelopment, map<int, double>>>());
									--gap;
									}
								}
                        
							 FMTdevelopment dev(FMTmask(mask,themes),age,0,period);
							 vector<FMTaction>::const_iterator act = find_if(actions.begin(),actions.end(),FMTactioncomparator(actionname));
							 //if the action is not _lockexempt then break the variable thing!
							 if (act->lock)
								{
								variable = 0;
								}
							 if (data[period-1].find(*act) == data[period - 1].end())
								 {
								 data[period - 1][*act] = map<FMTdevelopment, map<int, double>>();
								 }
							 if (data[period - 1][*act][dev].find(variable)!= data[period - 1][*act][dev].end())
								{
								data[period - 1][*act][dev][variable] += area;
								}else {
								data[period - 1][*act][dev][variable] = area;
								}
							}
                        }
                }
            }

	/*for (FMTschedule& sch : schedules)
		{
		sch.sort();
		}*/
	
	int period = 1;
	for (const map<FMTaction, map<FMTdevelopment, map<int, double>>>& inschedule : data)
		{
		schedules.push_back(FMTschedule(period, inschedule));
		++period;
		}
		}
    return schedules;
    }


void FMTscheduleparser::write(const vector<FMTschedule>& schedules,string location)
    {
    ofstream schedulestream;
    schedulestream.open(location);
    if (tryopening(schedulestream,location))
        {
        for(const FMTschedule& sch : schedules)
            {
            schedulestream<<string(sch);
            }
        schedulestream.close();
        }
    }

}
