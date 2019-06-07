#include "FMTconstantparser.h"

namespace WSParser{

FMTconstantparser::FMTconstantparser(): FMTparser()
    {
    }

FMTconstantparser::FMTconstantparser(const FMTconstantparser& rhs):FMTparser(rhs){}


FMTconstantparser& FMTconstantparser::operator = (const FMTconstantparser& rhs)
    {
        if (this!=&rhs)
        {
            FMTparser::operator=(rhs);
        }
    return *this;
    }

FMTconstants FMTconstantparser::read(string location)
    {
    ifstream CONstream(location);
    string line;
    FMTconstants constants;
	vector<FMTtheme>themes;
    if (FMTparser::tryopening(CONstream,location))
        {
        while(CONstream.is_open())
            {
            //line = FMTparser::getcleanline(CONstream);
			line = FMTparser::getcleanlinewfor(CONstream, themes, constants);
            if (!line.empty())
                {
                vector<string>splited = FMTparser::spliter(line,FMTparser::rxseparator);
                string key = splited[0];
                vector<string>values;
                for(size_t id=1;id<splited.size();++id)
                    {
                    int period = int((id -1));
                    if (splited[id].find("#")!=string::npos)
                        {
                        string strid = splited[id];
                        strid.erase(0,1);
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<key <<" "<< constants.getstr(strid, period) << "\n";
                        values.push_back(constants.getstr(strid,period));
                        }else{
                        values.push_back(splited[id]);
                        }
                    }
                constants.set(key,values);
                }
            }
        }
    return constants;
    }

bool FMTconstantparser::write(const FMTconstants& constants,string location)
    {
    ofstream constantstream;
    constantstream.open(location);
    if (tryopening(constantstream,location))
        {
        constantstream<<string(constants);
        constantstream.close();
        return true;
        }
    return false;
    }
}
