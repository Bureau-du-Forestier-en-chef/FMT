#include "FMTconstants.h"

namespace Core{


FMTconstants::FMTconstants():data(){}
FMTconstants::FMTconstants(const FMTconstants& rhs) : data(rhs.data)
    {

    }
void FMTconstants::set(string key,vector<string>values)
    {
    data[key] = values;
    }
bool FMTconstants::isconstant(string value) const
    {
    if (value.find("#")!=string::npos)
        {
        value.erase(0,1);
        if (data.find(value)!=data.end())
            {
            return true;
            }
        }
    return false;
    }
FMTconstants& FMTconstants::operator = (const FMTconstants& rhs)
    {
    if (this!=&rhs)
        {
        data=rhs.data;
        }
    return *this;
    }
FMTconstants::operator string() const
    {
    string line;
    for (auto it : data)
        {
        line+=it.first+" ";
        for (auto val : it.second)
            {
            line+=val;
            }
        line+="\n";
        }
    return line;
    }
string FMTconstants::getstr(string key,int period) const
    {

    if (key.find("#")!=string::npos)
            {
            key.erase(0,1);
            }
        boost::unordered_map<string,vector<string>>::const_iterator it = data.find(key);
        if (it==data.end())
            {

            }
        vector<string> const* location = &it->second;
        if (period >= int(location->size()))
            {
            period = int(location->size()) -1;
            }
    return location->at(period);
    }

size_t FMTconstants::length(string value) const
    {
    if (value.find("#")!=string::npos)
        {
        value.erase(0,1);
        if (data.find(value)!=data.end())
            {
            return (data.at(value).size());
            }
        }
    return 0;
    }



}
