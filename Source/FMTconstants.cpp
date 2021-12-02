/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTconstants.hpp"

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


template<typename T>
T FMTconstants::get(std::string key, int period) const
{
	try {
		if (key.find("#") != std::string::npos)
		{
			key.erase(0, 1);
		}
		boost::unordered_map<std::string, std::vector<double>>::const_iterator it = data.find(key);
		if (it == data.end())
		{
			_exhandler->raise(Exception::FMTexc::FMTundefined_constant, key + " at period " + std::to_string(period), "FMTconstants::get", __LINE__, __FILE__, Core::FMTsection::Constants);
		}
		std::vector<double> const* location = &it->second;
		if (period >= static_cast<int>(location->size()))
		{
			period = static_cast<int>(location->size()) - 1;
		}
		return static_cast<T>(location->at(period));
	}
	catch (...)
	{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for " + key + " at period " + std::to_string(period), "FMTconstants::get", __LINE__, __FILE__, Core::FMTsection::Constants);
	}
	return T();
}

template double FMTconstants::get<double>(std::string key, int period) const;
template int FMTconstants::get<int>(std::string key, int period) const;
template size_t FMTconstants::get<size_t>(std::string key, int period) const;

template<typename T>
std::vector<T>FMTconstants::getall(std::string key) const
{
	std::vector<T>values;
	try {
		for (size_t period = 0; period < length(key); ++period)
		{
			values.push_back(get<T>(key, period));
		}
	}
	catch (...)
	{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for " + key, "FMTconstants::getall", __LINE__, __FILE__, Core::FMTsection::Constants);
	}
	return values;
}

template std::vector<double>FMTconstants::getall<double>(std::string key) const;
template std::vector<int>FMTconstants::getall<int>(std::string key) const;
template std::vector<size_t> FMTconstants::getall<size_t>(std::string key) const;


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
