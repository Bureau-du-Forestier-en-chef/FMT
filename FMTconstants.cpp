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

#include "FMTconstants.h"

namespace Core{


FMTconstants::FMTconstants():data(){}
FMTconstants::FMTconstants(const FMTconstants& rhs) : data(rhs.data)
    {

    }
void FMTconstants::set(std::string key, std::vector<std::string>values)
    {
    data[key] = values;
    }
bool FMTconstants::isconstant(std::string value) const
    {
    if (value.find("#")!= std::string::npos)
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
FMTconstants::operator std::string() const
    {
	std::string line;
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
std::string FMTconstants::getstr(std::string key,int period) const
    {

    if (key.find("#")!= std::string::npos)
            {
            key.erase(0,1);
            }
        boost::unordered_map<std::string, std::vector<std::string>>::const_iterator it = data.find(key);
        if (it==data.end())
            {

            }
		std::vector<std::string> const* location = &it->second;
        if (period >= static_cast<int>(location->size()))
            {
            period = static_cast<int>(location->size()) -1;
            }
    return location->at(period);
    }

size_t FMTconstants::length(std::string value) const
    {
    if (value.find("#")!= std::string::npos)
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
