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

#include "FMTtheme.h"
#include "FMTmask.h"

namespace Core {
FMTtheme::FMTtheme():id(),start(),aggregates(), source_aggregates(),valuenames(),indexes(),name(){}
map<string,vector<string>>FMTtheme::desagregate(map<string,vector<string>>laggregates,vector<string>&lbases)
    {
    vector<string>keys;
    for (map<string,vector<string>>::iterator it=laggregates.begin(); it!=laggregates.end(); ++it)
        {
        keys.push_back(it->first);
        }
    bool processdone = false;
    while(!processdone)
        {
        processdone = true;
        for (string& key : keys)
            {
            vector<string>values = laggregates[key];
            vector<string>newvalues;
            for(string& value : values)
                {
                if (find(lbases.begin(),lbases.end(),value)==lbases.end())
                    {
                    for(string& newvalue : laggregates[value])
                        {
                        if(find(newvalues.begin(),newvalues.end(),newvalue)==newvalues.end())
                            {
                            if(find(lbases.begin(),lbases.end(),newvalue)==lbases.end())
                                {
                                processdone = false;
                                }
                            newvalues.push_back(newvalue);
                            }
                        }
                    }else if(find(newvalues.begin(),newvalues.end(),value)==newvalues.end())
                        {
                        newvalues.push_back(value);
                        }
                }
            laggregates[key] = newvalues;
            }
        }
    return laggregates;
    }
FMTtheme::FMTtheme(map<string,vector<string>>&laggregates,map<string,string>&lvaluenames,int& lid,int& lstart,string& lname) : id(lid),start(lstart),aggregates(), source_aggregates(laggregates),valuenames(lvaluenames),indexes(),name(lname)
        {
        vector<string>basevalues;
         for (map<string,string>::iterator it=lvaluenames.begin(); it!=lvaluenames.end(); ++it)
            {
            basevalues.push_back(it->first);
            }

		 if (!laggregates.empty())
			{
			aggregates = desagregate(laggregates, basevalues);
			}
        }

FMTtheme::FMTtheme(map<string,vector<string>>&laggregates,map<string,string>&lvaluenames,
                 const map<string,map<string,double>>& indexing,int& lid,int& lstart,string& lname) : id(lid),start(lstart),aggregates(), source_aggregates(laggregates),valuenames(lvaluenames),indexes(indexing),name(lname)
        {
        vector<string>basevalues;
         for (map<string,string>::iterator it=lvaluenames.begin(); it!=lvaluenames.end(); ++it)
            {
            basevalues.push_back(it->first);
            }
		 if (!laggregates.empty())
			{
			aggregates = desagregate(laggregates, basevalues);
			}
        }

FMTtheme::FMTtheme(const FMTtheme& rhs) : id(rhs.id), start(rhs.start), aggregates(rhs.aggregates),
									source_aggregates(rhs.source_aggregates), valuenames(rhs.valuenames),indexes(rhs.indexes), name(rhs.name)
	{

	}

FMTtheme& FMTtheme::operator = (const FMTtheme& rhs)
	{
	if (this!=&rhs)
		{
		id = rhs.id;
		start = rhs.start;
		aggregates = rhs.aggregates;
		source_aggregates = rhs.source_aggregates;
		valuenames = rhs.valuenames;
		indexes = rhs.indexes;
		name = rhs.name;
		}
	return *this;
	}

map<string,string>FMTtheme::getvaluenames() const
    {
    return valuenames;
    }

FMTtheme::FMTtheme(const vector<string> lvaluenames,const int& lid,const int& lstart,const string& lname): id(lid),start(lstart),aggregates(), source_aggregates(),valuenames(),indexes(),name(lname)
    {
    for(const string& val : lvaluenames)
        {
        valuenames[val]= "";
        }
    }

bool FMTtheme::usefull()
        {
        if (int(valuenames.size())<=1)
            {
            return false;
            }
        return true;
        }
size_t FMTtheme::size() const
    {
    return valuenames.size();
    }

int FMTtheme::getid() const
	{
	return id;
	}
bool FMTtheme::isattribute(const string& value) const
        {
        return (valuenames.find(value)!=valuenames.end());
        }
bool FMTtheme::isaggregate(const string& value) const
        {
        return (aggregates.find(value)!=aggregates.end());
        }
bool FMTtheme::inaggregate(const string& value,const string& aggregate)
        {
        vector<string>* vecp = &aggregates[aggregate];
        return (find(vecp->begin(),vecp->end(),value)!=vecp->end());
        }

bool FMTtheme::isindex(const string& value) const
    {
    if(!indexes.empty())
        {
        for (map<string,map<string,double>>::const_iterator index_it = indexes.begin(); index_it != indexes.end(); index_it++)
            {
            if (index_it->second.find(value)!=index_it->second.end())
                {
                return true;
                }
            }
        }
    return false;
    }

bool FMTtheme::useindex() const
    {
    if(!indexes.empty())
        {
        return true;
        }
    return false;
    }

 double FMTtheme::getindex(const string& attribute,const string& value) const
    {
    return indexes.at(attribute).at(value);
    }

bool FMTtheme::operator == (const FMTtheme& rhs) const
	{
	return (id == rhs.id &&
		start == rhs.start &&
		aggregates == rhs.aggregates &&
		source_aggregates == rhs.source_aggregates &&
		valuenames == rhs.valuenames &&
		indexes == rhs.indexes);
	}

bool FMTtheme::empty() const
	{
	return valuenames.empty();
	}


boost::dynamic_bitset<> FMTtheme::strtobits(const string& value) const
        {
        boost::dynamic_bitset<> bits(valuenames.size());
        if (isattribute(value))
            {
            bits[distance(valuenames.begin(),valuenames.find(value))]=true;
            }else if(isaggregate(value))
                {
                for(string attvalue : aggregates.at(value))
                    {
                    bits[distance(valuenames.begin(),valuenames.find(attvalue))]=true;
                    }
                }else if(value=="?")
                    {
                    for (boost::dynamic_bitset<>::size_type i = 0; i < bits.size(); ++i)
                        {
                        bits[i]=true;
                        }
                    }else{
                    //crash
                    }
        return bits;
        }
string FMTtheme::bitstostr(boost::dynamic_bitset<>& bits) const
        {
        string value;
        if (bits.count()==1)
            {
            int location =  int(bits.find_first());
            map<string,string>::const_iterator start = valuenames.begin();
            map<string,string>::const_iterator it  = next(start, location);
            value = it->first;
            }else if(bits.count()==bits.size())
                {
                value = "?";
                }else{
                for (map<string,vector<string>>::const_iterator it=aggregates.begin(); it!=aggregates.end(); ++it)
                    {
                    string aggregate = it->first;
                    boost::dynamic_bitset<>totest = strtobits(aggregate);
                    if (totest == bits)
                        {
                        return aggregate;
                        }
                    }
                //crash here
                }
        return value;
        }
vector<string>FMTtheme::getattributes(const string& value, bool aggregate_source) const
        {
        vector<string>result;
        if(isaggregate(value))
            {
			if (aggregate_source)
				{
				result = source_aggregates.at(value);
				}else {
				result = aggregates.at(value);
				}

            }else if(value=="?")
                {
                for(map<string,string>::const_iterator it=valuenames.begin(); it!=valuenames.end(); ++it)
                    {
                    result.push_back(it->first);
                    }
                }else if(isattribute(value))
                    {
                    result.push_back(value);
                    }
        return result;
        }
FMTtheme::operator string() const
        {
        string fulltheme = "*THEME "+to_string(id+1)+" "+name +"\n";
        for (map<string,string>::const_iterator it=valuenames.begin(); it!=valuenames.end(); ++it)
            {
            fulltheme+=" "+it->first;
            /*if (indexes.find(it->first)!=indexes.end())
                {
                fulltheme+=" _INDEX(";
                for(map<string,double>::const_iterator index_it = indexes.at(it->first).begin(); index_it != indexes.at(it->first).end(); index_it++)
                    {
                    fulltheme+= index_it->first + "=" + to_string(index_it->second)+",";
                    }
                fulltheme.pop_back();
                fulltheme+=")";
                }*/
            fulltheme+=" "+it->second+"\n";
            }
        for (map<string,vector<string>>::const_iterator it=source_aggregates.begin(); it!= source_aggregates.end(); ++it)
            {
            vector<string>values = it->second;
            fulltheme+="*AGGREGATE "+it->first+"\n";
            for(string& value : values)
                {
                fulltheme+=" "+value+"\n";
                }
            }
        return fulltheme;
        }
}
