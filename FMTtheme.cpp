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
FMTtheme::FMTtheme():id(),start(),aggregates(), source_aggregates(),valuenames(),indexes(),name()

	{

	}
std::map<std::string, std::vector<std::string>>FMTtheme::desagregate(std::map<std::string, std::vector<std::string>>laggregates,const std::vector<std::string>&lbases)
    {
    std::vector<std::string>keys;
    for (std::map<std::string, std::vector<std::string>>::const_iterator it=laggregates.begin(); it!=laggregates.end(); ++it)
        {
        keys.push_back(it->first);
        }
    bool processdone = false;
    while(!processdone)
        {
        processdone = true;
        for (const std::string& key : keys)
            {
            const std::vector<std::string>values = laggregates.at(key);
			std::vector<std::string>newvalues;
            for(const std::string& value : values)
                {
                if (std::find(lbases.begin(),lbases.end(),value)==lbases.end())
                    {
                    for(const std::string& newvalue : laggregates.at(value))
                        {
                        if(std::find(newvalues.begin(),newvalues.end(),newvalue)==newvalues.end())
                            {
                            if(std::find(lbases.begin(),lbases.end(),newvalue)==lbases.end())
                                {
                                processdone = false;
                                }
                            newvalues.push_back(newvalue);
                            }
                        }
                    }else if(std::find(newvalues.begin(),newvalues.end(),value)==newvalues.end())
                        {
                        newvalues.push_back(value);
                        }
                }
            laggregates[key] = newvalues;
            }
        }
    return laggregates;
    }
FMTtheme::FMTtheme(const std::map<std::string, std::vector<std::string>>&laggregates,const std::map<std::string, std::string>&lvaluenames,const int& lid,const int& lstart,const std::string& lname) : id(lid),start(lstart),aggregates(), source_aggregates(laggregates),valuenames(lvaluenames),indexes(),name(lname)
        {
        std::vector<std::string>basevalues;
         for (std::map<std::string, std::string>::const_iterator it=lvaluenames.begin(); it!=lvaluenames.end(); ++it)
            {
            basevalues.push_back(it->first);
            }
		 if (!laggregates.empty())
			{
			aggregates = desagregate(laggregates, basevalues);
			}
        }

FMTtheme::FMTtheme(const std::map<std::string, std::vector<std::string>>&laggregates,const std::map<std::string,std::string>&lvaluenames,
                 const std::map<std::string, std::map<std::string,double>>& indexing,const int& lid,const int& lstart,const std::string& lname) : id(lid),start(lstart),aggregates(), source_aggregates(laggregates),valuenames(lvaluenames),indexes(indexing),name(lname)
        {
        std::vector<std::string>basevalues;
         for (std::map<std::string, std::string>::const_iterator it=lvaluenames.begin(); it!=lvaluenames.end(); ++it)
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

std::map<std::string, std::string>FMTtheme::getvaluenames() const
    {
    return valuenames;
    }

FMTtheme::FMTtheme(const std::vector<std::string>& lvaluenames,const int& lid,const int& lstart,const std::string& lname): id(lid),start(lstart),aggregates(), source_aggregates(),valuenames(),indexes(),name(lname)
    {
    for(const std::string& val : lvaluenames)
        {
        valuenames[val]= "";
        }
    }


bool FMTtheme::inaggregate(const std::string& value,const std::string& aggregate)
        {
		const std::vector<std::string>* vecp = &aggregates.at(aggregate);
        return (std::find(vecp->begin(),vecp->end(),value)!=vecp->end());
        }

bool FMTtheme::isvalid(const std::string& value) const
	{
	return value == "?" || isattribute(value) || isaggregate(value);
	}

bool FMTtheme::isindex(const std::string& value) const
    {
    if(!indexes.empty())
        {
        for (std::map<std::string, std::map<std::string,double>>::const_iterator index_it = indexes.begin(); index_it != indexes.end(); index_it++)
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

 double FMTtheme::getindex(const std::string& attribute,const std::string& value) const
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



boost::dynamic_bitset<> FMTtheme::strtobits(const std::string& value) const
        {
        boost::dynamic_bitset<> bits(valuenames.size());
        if (isattribute(value))
            {
            bits[distance(valuenames.begin(),valuenames.find(value))]=true;
            }else if(isaggregate(value))
                {
                for(const std::string attvalue : aggregates.at(value))
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
std::string FMTtheme::bitstostr(const boost::dynamic_bitset<>& bits) const
        {
        std::string value;
        if (bits.count()==1)
            {
            const int location =  static_cast<int>(bits.find_first());
			std::map<std::string, std::string>::const_iterator start = valuenames.begin();
			std::map<std::string, std::string>::const_iterator it  = std::next(start, location);
            value = it->first;
            }else if(bits.count()==bits.size())
                {
                value = "?";
                }else{
                for (std::map<std::string, std::vector<std::string>>::const_iterator it=aggregates.begin(); it!=aggregates.end(); ++it)
                    {
					const std::string aggregate = it->first;
                    const boost::dynamic_bitset<>totest = strtobits(aggregate);
                    if (totest == bits)
                        {
                        return aggregate;
                        }
                    }
                //crash here
                }
        return value;
        }
std::vector<std::string>FMTtheme::getattributes(const std::string& value, bool aggregate_source) const
        {
	std::vector<std::string>result;
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
                for(std::map<std::string, std::string>::const_iterator it=valuenames.begin(); it!=valuenames.end(); ++it)
                    {
                    result.push_back(it->first);
                    }
                }else if(isattribute(value))
                    {
                    result.push_back(value);
                    }
        return result;
        }
FMTtheme::operator std::string() const
        {
		std::string fulltheme = "*THEME "+ std::to_string(id+1)+" "+name +"\n";
        for (std::map<std::string, std::string>::const_iterator it=valuenames.begin(); it!=valuenames.end(); ++it)
            {
            fulltheme+=" "+it->first;
            fulltheme+=" "+it->second+"\n";
            }
        for (std::map<std::string, std::vector<std::string>>::const_iterator it=source_aggregates.begin(); it!= source_aggregates.end(); ++it)
            {
			const std::vector<std::string>values = it->second;
            fulltheme+="*AGGREGATE "+it->first+"\n";
            for(const std::string& value : values)
                {
                fulltheme+=" "+value+"\n";
                }
            }
        return fulltheme;
        }
FMTtheme FMTtheme::presolve(const FMTmask& basemask, int& newid, int& newstart, FMTmask& selected) const
	{
	if (selected.empty())
		{
		selected.data = boost::dynamic_bitset<>(basemask.data.size(),false);
		selected.name.clear();
		}
	FMTtheme newtheme(*this);
	newtheme.valuenames.clear();
	newtheme.aggregates.clear();
	newtheme.source_aggregates.clear();
	newtheme.indexes.clear();
	std::map<std::string, std::string>newvaluenames;
	std::map<std::string, std::string>::const_iterator valueit = valuenames.begin();
	for (size_t binlocation = start; binlocation < (start+this->size());++binlocation)
		{
		if (basemask.data[binlocation])
			{
			selected.data[binlocation] = true;
			newvaluenames[valueit->first] = valueit->second;
			}
		++valueit;
		}
	if (newvaluenames.size()>1)
		{
		newtheme.valuenames = newvaluenames;
		newtheme.id = newid;
		++newid;
		newtheme.start = newstart;
		newstart+= newtheme.size();
		for (std::map<std::string,std::vector<std::string>>::const_iterator aggit = aggregates.begin();
			aggit != aggregates.end(); aggit++)
			{
			std::vector<std::string>newattributes;
			for (const std::string& attribute : aggit->second)
				{
				if (newtheme.valuenames.find(attribute)!= newtheme.valuenames.end())
					{
					newattributes.push_back(attribute);
					}
				}
			if (!newattributes.empty())
				{
				newtheme.aggregates[aggit->first] = newattributes;
			}

			}
		for (std::map<std::string, std::vector<std::string>>::const_iterator aggit = source_aggregates.begin();
			aggit != source_aggregates.end(); aggit++)
			{
			std::vector<std::string>newaggregates;
			for (const std::string& aggregate : aggit->second)
				{
					if (newtheme.aggregates.find(aggregate) != newtheme.aggregates.end() ||
						newtheme.valuenames.find(aggregate) != newtheme.valuenames.end())
					{
						newaggregates.push_back(aggregate);
					}
				}
			if (!newaggregates.empty())
				{
				newtheme.source_aggregates[aggit->first] = newaggregates;
				}
			}
	}else {
		for (size_t binlocation = start; binlocation < (start + this->size()); ++binlocation)
			{
			selected.data[binlocation] = false;
			}
		}	
	return newtheme;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTtheme);