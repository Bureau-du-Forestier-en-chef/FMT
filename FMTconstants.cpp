/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTconstants.h"

namespace Core{


FMTconstants::FMTconstants():FMTobject(),data(){}
FMTconstants::FMTconstants(const FMTconstants& rhs) : FMTobject(rhs),data(rhs.data)
    {

    }
void FMTconstants::set(const std::string& key, std::vector<double>values)
    {
    data[key] = values;
    }
bool FMTconstants::isconstant(std::string value) const
    {
    if (value.find("#")!= std::string::npos)
        {
        value.erase(0,1);
		return (data.find(value) != data.end());
        }
    return false;
    }
FMTconstants& FMTconstants::operator = (const FMTconstants& rhs)
    {
    if (this!=&rhs)
        {
		FMTobject::operator=(rhs);
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
            line+=std::to_string(val);
            }
        line+="\n";
        }
    return line;
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

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTconstants)
