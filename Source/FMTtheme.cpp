/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtheme.h"
#include "FMTmask.h"

namespace Core {


	boost::unordered_set<FMTlookup<std::vector<size_t>, std::string>>::const_iterator FMTtheme::getattribute(const std::string& value,bool raiseifnotfound) const
		{
		boost::unordered_set<FMTlookup<std::vector<size_t>, std::string>>::const_iterator lookit = attribute_locations.end();
		try {
			FMTlookup<std::vector<size_t>, std::string>lookfor(value);
			lookit = attribute_locations.find(lookfor);
			if (raiseifnotfound&&lookit== attribute_locations.end())
				{
				_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
					value + " at theme "+getname(),
					"FMTtheme::getattribute", __LINE__, __FILE__);
				}
		}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTtheme::getattribute", __LINE__, __FILE__, _section);
			}
		return lookit;
		}


	void FMTtheme::buildattributelocations()
	{
		try {
			attribute_locations.clear();
			size_t location = 0;
			for (const std::string& attribute : attributes)
				{
				FMTlookup<std::vector<size_t>, std::string>addfor(std::vector<size_t>(1, location), attribute);
				attribute_locations.insert(addfor);
				++location;
				}
			bool processdone = false;
			std::vector<std::vector<std::string>>newaggregates = aggregatenames;
			while (!processdone)
			{
				processdone = true;
				size_t aggregatelocation = 0;
				for (const std::string& key : aggregates)
				{
					const std::vector<std::string>aggvalues = newaggregates.at(aggregatelocation);
					std::vector<std::string>newvalues;
					for (const std::string& value : aggvalues)
					{
						if (std::find(attributes.begin(), attributes.end(), value) == attributes.end())
						{
							const size_t baselocation = std::distance(aggregates.begin(), std::find(aggregates.begin(), aggregates.end(), value));
							for (const std::string& newvalue : newaggregates.at(baselocation))
							{
								if (std::find(newvalues.begin(), newvalues.end(), newvalue) == newvalues.end())
								{
									if (std::find(attributes.begin(), attributes.end(), newvalue) == attributes.end())
									{
										processdone = false;
									}
									newvalues.push_back(newvalue);
								}
							}
						}
						else if (std::find(newvalues.begin(), newvalues.end(), value) == newvalues.end())
						{
							newvalues.push_back(value);
						}
					}
					newaggregates[aggregatelocation] = newvalues;
				++aggregatelocation;
				}
			}
			size_t agglocation = 0;
			for (const std::vector<std::string>& aggvalues : newaggregates)
				{
				std::vector<size_t>locationofagg;
				locationofagg.reserve(aggvalues.size());
				for (const std::string& val : aggvalues)
					{
					locationofagg.push_back(std::distance(attributes.begin(), std::find(attributes.begin(), attributes.end(), val)));
					}
				FMTlookup<std::vector<size_t>, std::string>addfor(locationofagg, aggregates.at(agglocation));
				attribute_locations.insert(addfor);
				++agglocation;
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTtheme::buildattributelocations", __LINE__, __FILE__, _section);
			}

	}

/*std::map<std::string, std::vector<std::string>>FMTtheme::desagregate(std::map<std::string, std::vector<std::string>>laggregates,const std::vector<std::string>&lbases)
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


FMTtheme::FMTtheme(const std::map<std::string, std::vector<std::string>>&laggregates,const std::map<std::string, std::string>&lvaluenames,const int& lid,const int& lstart,const std::string& lname) : FMTobject(),id(lid),start(lstart),aggregates(), source_aggregates(laggregates),valuenames(lvaluenames),indexes(),name(lname)
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
                 const std::map<std::string, std::map<std::string,double>>& indexing,const int& lid,const int& lstart,const std::string& lname) : FMTobject(),id(lid),start(lstart),aggregates(), source_aggregates(laggregates),valuenames(lvaluenames),indexes(indexing),name(lname)
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
        }*/


FMTtheme::FMTtheme(const std::vector<std::string>& lattributes,
				const std::vector<std::string>& lattributenames,
				const std::vector<std::string>& laggregates,
				const std::vector<std::vector<std::string>>& laggregatenames,
				const std::vector<std::map<std::string, double>>& lindexes ,
				const int& lid, const int& lstart, const std::string& lname) : 
	FMTobject(),
	id(lid),
	start(lstart),
	attributes(lattributes),
	attributenames(lattributenames),
	aggregates(laggregates),
	aggregatenames(laggregatenames),
	indexes(lindexes),
	attribute_locations(),
	name(lname)
{
	buildattributelocations();
}

FMTtheme::FMTtheme(const std::vector<std::string>& lattributes,
	const int& lid, const int& lstart, const std::string& lname):
	FMTobject(),
	id(lid),
	start(lstart),
	attributes(lattributes),
	attributenames(),
	aggregates(),
	aggregatenames(),
	indexes(),
	attribute_locations(),
	name(lname)
{
	buildattributelocations();
}


/*FMTtheme::FMTtheme(const FMTtheme& rhs) : FMTobject(rhs),id(rhs.id), start(rhs.start), aggregates(rhs.aggregates),
									source_aggregates(rhs.source_aggregates), valuenames(rhs.valuenames),indexes(rhs.indexes), name(rhs.name)
	{

	}*/

FMTtheme::FMTtheme(const FMTtheme& rhs) :
	FMTobject(rhs),
	id(rhs.id),
	start(rhs.start),
	attributes(rhs.attributes),
	attributenames(rhs.attributenames),
	aggregates(rhs.aggregates),
	aggregatenames(rhs.aggregatenames),
	indexes(rhs.indexes),
	attribute_locations(),
	name(rhs.name)
{
	buildattributelocations();
}

FMTtheme& FMTtheme::operator = (const FMTtheme& rhs)
	{
	if (this!=&rhs)
		{
		FMTobject::operator=(rhs);
		id = rhs.id;
		start = rhs.start;
		attributes = rhs.attributes;
		attributenames = rhs.attributenames;
		aggregates = rhs.aggregates;
		aggregatenames = rhs.aggregatenames;
		indexes = rhs.indexes;
		name = rhs.name;
		buildattributelocations();
		}
	return *this;
	}

/*std::map<std::string, std::string>FMTtheme::getvaluenames() const
    {
    return valuenames;
    }

FMTtheme::FMTtheme(const std::vector<std::string>& lvaluenames,const int& lid,const int& lstart,const std::string& lname): id(lid),start(lstart),aggregates(), source_aggregates(),valuenames(),indexes(),name(lname)
    {
    for(const std::string& val : lvaluenames)
        {
        valuenames[val]= "";
        }
    }*/


/*bool FMTtheme::inaggregate(const std::string& value,const std::string& aggregate)
        {
		const std::vector<std::string>* vecp = &aggregates.at(aggregate);
        return (std::find(vecp->begin(),vecp->end(),value)!=vecp->end());
        }*/

bool FMTtheme::inaggregate(const std::string& value, const std::string& aggregate)
{
	try {
		boost::unordered_set<FMTlookup<std::vector<size_t>, std::string>>::const_iterator lookit = getattribute(aggregate);
		if (lookit != attribute_locations.end())
			{
			for (const size_t& location : lookit->memoryobject)
				{
				if (attributes.at(location)==value)
					{
					return true;
					}
				}

			}
	}catch (...)
		{
		_exhandler->raisefromcatch("For aggregate: "+aggregate+" at value "+value, "FMTtheme::inaggregate", __LINE__, __FILE__, _section);
		}
	return false;
}

bool FMTtheme::isvalid(const std::string& value) const
	{
	try {
		return (value == "?" || isattribute(value) || isaggregate(value));
	}
	catch (...)
	{
		_exhandler->raisefromcatch("For value: " + value, "FMTtheme::isvalid", __LINE__, __FILE__, _section);
	}
	return false;
	}

/*bool FMTtheme::isindex(const std::string& value) const
{
	if (!indexes.empty())
	{
		for (std::map<std::string, std::map<std::string, double>>::const_iterator index_it = indexes.begin(); index_it != indexes.end(); index_it++)
		{
			if (index_it->second.find(value) != index_it->second.end())
			{
				return true;
			}
		}
	}
	return false;

}*/

bool FMTtheme::isindex(const std::string& value) const
{
	try{
		for (const std::map<std::string, double>& indexer : indexes)
			{
			if (indexer.find(value)!=indexer.end())
				{
				return true;
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("For value: " + value, "FMTtheme::isindex", __LINE__, __FILE__, _section);
		}
	return false;

}


/*bool FMTtheme::isindex(const std::string& attribute, const std::string& value) const
    {
    if(!indexes.empty() && indexes.find(attribute)!=indexes.end())
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
    }*/

bool FMTtheme::isindex(const std::string& attribute, const std::string& value) const
{
	try {
		boost::unordered_set<FMTlookup<std::vector<size_t>, std::string>>::const_iterator lookit = getattribute(attribute);
		if (lookit!=attribute_locations.end())
			{
			for (const size_t& location : lookit->memoryobject)
				{
				if (indexes.at(location).find(value)!= indexes.at(location).end())
					{
					return true;
					}
				}

			}
	}catch (...)
		{
		_exhandler->raisefromcatch("For attribute: " +attribute+" at value "+value, "FMTtheme::isindex", __LINE__, __FILE__, _section);
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

 const double& FMTtheme::getindex(const std::string& attribute,const std::string& value) const
    {
	 try {
		 boost::unordered_set<FMTlookup<std::vector<size_t>, std::string>>::const_iterator lookit = getattribute(attribute);
		 if (lookit != attribute_locations.end())
			{
			for (const size_t& location : lookit->memoryobject)
				{
				 if (indexes.at(location).find(value) != indexes.at(location).end())
					{
					return indexes.at(location).at(value);
					}
				}
			}
	 }catch (...)
		{
		 _exhandler->raisefromcatch("For attribute: " + attribute + " at value " + value, "FMTtheme::getindex", __LINE__, __FILE__, _section);
		}
	return 0;
    }

bool FMTtheme::operator == (const FMTtheme& rhs) const
	{
	return (id == rhs.id &&
		start == rhs.start &&
		attributes == rhs.attributes &&
		attributenames == rhs.attributenames &&
		aggregates == rhs.aggregates &&
		aggregatenames == rhs.aggregatenames &&
		indexes == rhs.indexes && 
		name == rhs.name);
	}



boost::dynamic_bitset<> FMTtheme::strtobits(const std::string& value) const
        {
        boost::dynamic_bitset<> bits;
		try{
			if (value == "?")
			{
				bits.resize(attributes.size(), true);
			}else {
				bits.resize(attributes.size(),false);
				boost::unordered_set<FMTlookup<std::vector<size_t>, std::string>>::const_iterator lookit = getattribute(value);
				if (lookit != attribute_locations.end())
					{
					for (const size_t& location : lookit->memoryobject)
						{
						bits[location] = true;
						}

				}else {
					_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
						std::string(value) + " for theme " + std::to_string(id), "FMTtheme::strtobits", __LINE__, __FILE__, _section);
					}

				}
		}catch (...)
			{
			_exhandler->raisefromcatch("For value: " + value, "FMTtheme::strtobits", __LINE__, __FILE__, _section);
			}
        return bits;
        }

std::string FMTtheme::bitstostr(const boost::dynamic_bitset<>& bits) const
        {
		try {
			const size_t bitcounts = bits.count();
			if (bitcounts == bits.size())
			{
				return "?";
			}
			else {
				if (bitcounts == 1)
				{
					const size_t firsttrue = bits.find_first();
					if (firsttrue < attributes.size())
					{
						return attributes.at(firsttrue);
					}
				}
				else {
					for (const std::string& aggregate : aggregates)
					{
						const boost::dynamic_bitset<>totest = strtobits(aggregate);
						if (totest == bits)
						{
							return aggregate;
						}
					}

				}
				_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
					"for bitset count of " + std::to_string(bitcounts) + " in theme " + std::to_string(id), "FMTtheme::bitstostr", __LINE__, __FILE__, _section);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTtheme::bitstostr", __LINE__, __FILE__, _section);
			}
		return "";
        }

std::vector<std::string>FMTtheme::getattributes(const std::string& value, bool aggregate_source) const
        {
		std::vector<std::string>result;
		try {
			boost::unordered_set<FMTlookup<std::vector<size_t>, std::string>>::const_iterator lookit = getattribute(value);
			if (isaggregate(value)&& lookit != attribute_locations.end())
			{
				if (aggregate_source)
				{
					std::vector<std::string>::const_iterator cntit = std::find(aggregates.begin(), aggregates.end(), value);
					result = aggregatenames.at(std::distance(aggregates.begin(), cntit));
				}else {
					result.reserve(lookit->memoryobject.size());
					for (const size_t& location : lookit->memoryobject)
						{
						result.push_back(attributes.at(location));
						}
					}

			}
			else if (value == "?")
			{
				result = attributes;
			}
			else if (lookit != attribute_locations.end())
			{
				result.push_back(attributes.at(*lookit->memoryobject.begin()));
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTtheme::getattributes", __LINE__, __FILE__, _section);
			}
        return result;
        }

FMTtheme::operator std::string() const
        {
		std::string fulltheme = "*THEME "+ std::to_string(id+1)+" "+name +"\n";
		try {
			for (size_t location = 0; location < attributes.size(); ++location)
				{
				fulltheme += " " + attributes.at(location);
				if (!attributenames.empty())
					{
					fulltheme += " " + attributenames.at(location);
					}
				if (!indexes.empty())
					{
					if (!indexes.at(location).empty())
						{
						fulltheme += " _INDEX(";
						for (std::map<std::string, double>::const_iterator mit = indexes.at(location).begin(); mit != indexes.at(location).end(); mit++)
						{
							fulltheme += (mit->first + "=" + std::to_string(mit->second) + ",");
						}
						fulltheme.pop_back();
						fulltheme += ")";
						}
					}
				fulltheme +="\n";
				}
			for (size_t location = 0; location < aggregates.size(); ++location)
			{
				fulltheme += "*AGGREGATE " + aggregates.at(location) + "\n";
				for (const std::string& aggregatename : aggregatenames.at(location))
				{
					fulltheme += " " + aggregatename + "\n";
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTtheme::operator::std::string()", __LINE__, __FILE__, _section);
			}
        return fulltheme;
        }
FMTtheme FMTtheme::presolve(const FMTmask& basemask, int& newid, int& newstart, FMTmask& selected) const
	{
	try {
		if (selected.empty())
		{
			selected.data = boost::dynamic_bitset<>(basemask.data.size(), false);
			selected.name.clear();
		}
		FMTtheme newtheme(*this);
		newtheme.attributes.clear();
		newtheme.attributenames.clear();
		newtheme.aggregates.clear();
		newtheme.aggregatenames.clear();
		newtheme.indexes.clear();
		newtheme.attribute_locations.clear();

		std::vector<std::string>newattributes;
		std::vector<std::string>newattributenames;
		size_t location = 0;
		for (size_t binlocation = start; binlocation < (start + this->size()); ++binlocation)
			{
				if (basemask.data[binlocation])
				{
					selected.data[binlocation] = true;
					newattributes.push_back(attributes.at(location));
					if (!attributenames.empty())
						{
						newattributenames.push_back(attributenames.at(location));
						}
					
				}
				++location;
			}

		if (newattributes.size() > 1)
		{
			newtheme.attributes = newattributes;
			newtheme.attributenames = newattributenames;
			newtheme.id = newid;
			++newid;
			newtheme.start = newstart;
			newstart += newtheme.size();
			for (const std::string& aggregate : aggregates)
				{
				std::vector<std::string>aggvalues;
				for (const size_t& baselocation : getattribute(aggregate)->memoryobject)
					{
					if (std::find(newattributes.begin(),newattributes.end(),attributes.at(baselocation))!= newattributes.end())
						{
						aggvalues.push_back(attributes.at(baselocation));
						}
					}
				if (!aggvalues.empty())
					{
					newtheme.aggregates.push_back(aggregate);
					newtheme.aggregatenames.push_back(aggvalues);
					}
				}
		newtheme.buildattributelocations();
		}else {
			for (size_t binlocation = start; binlocation < (start + this->size()); ++binlocation)
			{
				selected.data[binlocation] = false;
			}
		}
		return newtheme;
	}catch (...)
		{
		_exhandler->raisefromcatch("for theme "+std::to_string(id),"FMTtheme::presolve", __LINE__, __FILE__, _section);
		}
	return FMTtheme();
	}

size_t FMTtheme::getstart() const
	{
	return static_cast<size_t>(start);
	}

FMTthemecomparator::FMTthemecomparator(const FMTtheme& lbase_theme): base_theme(lbase_theme)
	{

	}

bool FMTthemecomparator::operator()(const FMTtheme& theme) const
	{
	return (theme == base_theme);
	}



}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTtheme)