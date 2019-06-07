#include "FMTlifespanparser.h"

namespace WSParser{

FMTlifespanparser::FMTlifespanparser():FMTparser()
    {

    }

FMTlifespanparser::FMTlifespanparser(const FMTlifespanparser& rhs):FMTparser(rhs)
    {

    }
FMTlifespanparser& FMTlifespanparser::operator = (const FMTlifespanparser& rhs)
    {
    if (this!=&rhs)
        {
        FMTparser::operator=(rhs);
        }
    return *this;
    }

FMTlifespans FMTlifespanparser::read(const vector<FMTtheme>& themes,const FMTconstants& constants,string location)
    {
    ifstream LIFstream(location);
    string line;
    FMTlifespans lifespan;
    if (FMTparser::tryopening(LIFstream,location))
        {
        while(LIFstream.is_open())
            {
            line = FMTparser::getcleanline(LIFstream);
            if (!line.empty())
                {
                vector<string>splited = FMTparser::spliter(line,FMTparser::rxseparator);
                string page = splited[splited.size()-1];
                int age = getnum<int>(page,constants);
                splited.pop_back();
                string mask = boost::algorithm::join(splited, " ");
                if (!validate(themes, mask)) continue;
				lifespan.push_back(FMTmask(mask,themes), age);
                }
            }
        }
    lifespan.shrink();
    return lifespan;
    }

bool FMTlifespanparser::write(const FMTlifespans& lifespan,string location)
    {
    ofstream lifespanstream;
    lifespanstream.open(location);
    if (tryopening(lifespanstream,location))
        {
        lifespanstream<<string(lifespan);
        lifespanstream.close();
        return true;
        }
    return false;
    }

}
