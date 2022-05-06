/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtheme.hpp"
#include "FMTmask.hpp"
#include "FMTmaskfilter.hpp"
#include <boost/dynamic_bitset.hpp>
#include <unordered_map>
#include "FMTexceptionhandler.hpp"

namespace Core {


	FMTtheme::lookiterator FMTtheme::getattribute(const std::string& value,bool raiseifnotfound) const
		{
		FMTtheme::lookiterator lookit = attribute_locations.end();
		try {
			lookit = attribute_locations.find(value);
			if (raiseifnotfound&&lookit== attribute_locations.end())
				{
				_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
					value + " at theme "+std::to_string(getid())+" "+getname(),
					"FMTtheme::getattribute", __LINE__, __FILE__);
				}
		}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTtheme::getattribute", __LINE__, __FILE__, Core::FMTsection::Landscape);
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
				attribute_locations[attribute] = std::vector<size_t>(1, location);
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
				attribute_locations[aggregates.at(agglocation)] = locationofagg;
				++agglocation;
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTtheme::buildattributelocations", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}

	}



FMTtheme::FMTtheme(const std::vector<std::string>& lattributes,
				const std::vector<std::string>& lattributenames,
				const std::vector<std::string>& laggregates,
				const std::vector<std::vector<std::string>>& laggregatenames,
				const std::vector<std::map<std::string, double>>& lindexes ,
				const size_t& lid, const size_t& lstart, const std::string& lname) :
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
	const size_t& lid, const size_t& lstart, const std::string& lname):
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


FMTtheme::FMTtheme(const FMTtheme& rhs) :
	FMTobject(rhs),
	id(rhs.id),
	start(rhs.start),
	attributes(rhs.attributes),
	attributenames(rhs.attributenames),
	aggregates(rhs.aggregates),
	aggregatenames(rhs.aggregatenames),
	indexes(rhs.indexes),
	attribute_locations(rhs.attribute_locations),
	name(rhs.name)
{
	
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
		attribute_locations = rhs.attribute_locations;
		}
	return *this;
	}


bool FMTtheme::inaggregate(const std::string& value, const std::string& aggregate)
{
	try {
		lookiterator lookit = getattribute(aggregate);
		if (lookit != attribute_locations.end())
		{
			for (const size_t& location : lookit->second)
			{
				if (attributes.at(location) == value)
				{
					return true;
				}
			}

		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("For aggregate: " + aggregate + " at value " + value, "FMTtheme::inaggregate", __LINE__, __FILE__, Core::FMTsection::Landscape);
	}
	return false;
}

bool FMTtheme::isvalid(const std::string& value) const
	{
	try {
		return (value == "?" || isattribute(value));
	}
	catch (...)
	{
		_exhandler->raisefromcatch("For value: " + value, "FMTtheme::isvalid", __LINE__, __FILE__, Core::FMTsection::Landscape);
	}
	return false;
	}


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
		_exhandler->raisefromcatch("For value: " + value, "FMTtheme::isindex", __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	return false;

}

bool FMTtheme::isindex(const std::string& attribute, const std::string& value) const
{
	try {
		lookiterator lookit = getattribute(attribute);
		if (lookit!=attribute_locations.end())
			{
			for (const size_t& location : lookit->second)
				{
				if (indexes.at(location).find(value)!= indexes.at(location).end())
					{
					return true;
					}
				}

			}
	}catch (...)
		{
		_exhandler->raisefromcatch("For attribute: " +attribute+" at value "+value, "FMTtheme::isindex", __LINE__, __FILE__, Core::FMTsection::Landscape);
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
	 try {
		 lookiterator lookit = getattribute(attribute,true);
		 if (lookit != attribute_locations.end())
			{
			for (const size_t& location : lookit->second)
				{
				 if (indexes.at(location).find(value) != indexes.at(location).end())
					{
					return indexes.at(location).at(value);
					}
				}
			}
	 }catch (...)
		{
		 _exhandler->raisefromcatch("For attribute: " + attribute + " at value " + value, "FMTtheme::getindex", __LINE__, __FILE__, Core::FMTsection::Landscape);
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
				lookiterator lookit = getattribute(value,true);
				if (lookit != attribute_locations.end())
					{
					for (const size_t& location : lookit->second)
						{
						bits[location] = true;
						}

				}else {
					_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
						std::string(value) + " for theme " + std::to_string(id), "FMTtheme::strtobits", __LINE__, __FILE__,Core::FMTsection::Landscape);
					}

				}
		}catch (...)
			{
			_exhandler->raisefromcatch("For value: " + value, "FMTtheme::strtobits", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
        return bits;
        }

std::string FMTtheme::bitstostr(const boost::dynamic_bitset<>& bits) const
        {
		try {
			const size_t bitcounts = bits.count();
			const size_t themesize = bits.size();
			if (themesize>1 && bitcounts == themesize)
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
					"for bitset count of " + std::to_string(bitcounts) + " in theme " + std::to_string(id), "FMTtheme::bitstostr", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTtheme::bitstostr", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
		return "";
        }

std::vector<std::string>FMTtheme::getattributes(const std::string& value, bool aggregate_source) const
        {
		std::vector<std::string>result;
		try {
			if (value == "?")
			{
				result = attributes;
			}
			else {
				lookiterator lookit = getattribute(value, true);
				if (isaggregate(value) && lookit != attribute_locations.end())
				{
					if (aggregate_source)
					{
						std::vector<std::string>::const_iterator cntit = std::find(aggregates.begin(), aggregates.end(), value);
						result = aggregatenames.at(std::distance(aggregates.begin(), cntit));
					}
					else {
						result.reserve(lookit->second.size());
						for (const size_t& location : lookit->second)
						{
							result.push_back(attributes.at(location));
						}
					}

				}
				else if (lookit != attribute_locations.end())
				{
					result.push_back(attributes.at(*lookit->second.begin()));
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTtheme::getattributes", __LINE__, __FILE__, Core::FMTsection::Landscape);
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
			_exhandler->raisefromcatch("", "FMTtheme::operator::std::string()", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
        return fulltheme;
        }

void FMTtheme::fillupaggregates(std::vector<int>& themeids, std::vector<std::string>& locattributes, std::vector<std::string>& locaggregates) const
{
	try {
		for (const std::string& aggregate : aggregates)
			{
			for (const std::string& attribute : getattributes(aggregate))
				{
				themeids.push_back(static_cast<int>(id)+1);
				locattributes.push_back(attribute);
				locaggregates.push_back(aggregate);
				}
			}
	}catch (...)
	{
		_exhandler->raisefromcatch("for theme " + std::to_string(id), "FMTtheme::fillupaggregates", __LINE__, __FILE__, Core::FMTsection::Landscape);
	}

}


void FMTtheme::push_aggregate(const std::string& aggregatename)
	{
	try {
		if (isaggregate(aggregatename))
			{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				aggregatename+" is already an aggregate", "FMTtheme::presolve", __LINE__, __FILE__);
			}
		aggregates.push_back(aggregatename);
		aggregatenames.push_back(std::vector<std::string>());
		//buildattributelocations();
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTtheme::push_aggregate", __LINE__, __FILE__);
		}
	}


void FMTtheme::push_aggregate_value(const std::string& aggregatename, const std::string& value)
	{
	try {
		if (isaggregate(aggregatename))
			{
			aggregatenames[std::distance(aggregates.begin(), std::find(aggregates.begin(), aggregates.end(), aggregatename))].push_back(value);
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTtheme::push_aggregate_value", __LINE__, __FILE__);
		}
	}

std::string FMTtheme::updatefrommask(const Core::FMTmask& globalmask)
	{
	try {
		const boost::dynamic_bitset<>global = globalmask.subset(*this);
		if ((global.count()>1)&&(global.count()<global.size()))
			{
			std::string lastFMTaggregate("~FMT"+std::to_string(getid())+"A_0");
			for (const std::string& aggregate : aggregates)
				{
				if (strtobits(aggregate) == global)
					{
					return aggregate;
					}
				if (aggregate.find('~')!=std::string::npos)
					{
					lastFMTaggregate=aggregate;
					}
				}
			//Ok so we need a new aggregate
			std::vector<std::string>splittedFMTagg;
			boost::split(splittedFMTagg,lastFMTaggregate,boost::is_any_of("_"));
			int id = std::stoi(splittedFMTagg.at(1));
			id += 1;
			const std::string newaggregate = splittedFMTagg.at(0) + "_" + std::to_string(id);
			push_aggregate(newaggregate);
			std::vector<size_t>aggregateindex;
			for (size_t bid = 0; bid < global.size();++bid)
				{
				if (global[bid])
					{
					push_aggregate_value(newaggregate,attributes.at(bid));
					aggregateindex.push_back(bid);
					}

				}
			attribute_locations[newaggregate] = aggregateindex;
			//buildattributelocations();
			}
		return bitstostr(global);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTtheme::updatefrommask", __LINE__, __FILE__);
		}
	return "?";
	}


FMTtheme FMTtheme::presolve(FMTmaskfilter& maskfilter, size_t& newid, size_t& newstart) const
	{
	try {
		if (maskfilter.empty())
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
				"Empty selection", "FMTtheme::presolve", __LINE__, __FILE__);
		}
		if (maskfilter.flippedselection.empty())
		{
			maskfilter.flippedselection = boost::dynamic_bitset<>(maskfilter.selection.size(), false);
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
				if (maskfilter.selection[binlocation])
				{
					maskfilter.flippedselection[binlocation] = true;
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
				for (const size_t& baselocation : getattribute(aggregate)->second)
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
		}else {
			for (size_t binlocation = start; binlocation < (start + this->size()); ++binlocation)
			{
				maskfilter.flippedselection[binlocation] = false;
			}
		}
		newtheme.buildattributelocations();
		return newtheme;
	}catch (...)
		{
		_exhandler->raisefromcatch("for theme "+std::to_string(id),"FMTtheme::presolve", __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	return FMTtheme();
	}


FMTthemecomparator::FMTthemecomparator(const FMTtheme& lbase_theme, const bool& lcomparedwithpresolved): base_theme(lbase_theme),comparedwithpresolved(lcomparedwithpresolved)
	{

	}

bool FMTthemecomparator::operator()(const FMTtheme& theme) const
	{
	if (!comparedwithpresolved)
	{
		return (theme == base_theme);
	}else{
		for (const std::string& attribute : theme.getbaseattributes())
		{
			if (!base_theme.isattribute(attribute))
			{
				return false;
			}
		}
		return true;
	}
	}

#if defined FMTWITHR
Rcpp::DataFrame FMTtheme::getaggregatesasdataframe() const
{
	Rcpp::DataFrame data = Rcpp::DataFrame();
	try {
		if (!aggregates.empty())
			{
			std::vector<int>themeids;
			std::vector<std::string>locattributes;
			std::vector<std::string>locaggregates;
			this->fillupaggregates(themeids, locattributes, locaggregates);
			Rcpp::IntegerVector rids(themeids.begin(), themeids.end());
			Rcpp::StringVector rattributes(locattributes.begin(), locattributes.end());
			Rcpp::StringVector raggregates(locaggregates.begin(), locaggregates.end());
			data.push_back(rids, "THEMES");
			data.push_back(rattributes, "ATTRIBUTES");
			data.push_back(raggregates, "AGGREGATES");
			data.attr("row.names") = Rcpp::seq(1, themeids.size());
			}
		data.attr("class") = "data.frame";
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTtheme::getaggregatesasdataframe", __LINE__, __FILE__);
	}
	return data;
}

Rcpp::DataFrame FMTtheme::getattributesasdataframe() const
{
	Rcpp::DataFrame data = Rcpp::DataFrame();
	try {
		if (!attributenames.empty())
		{
			bool gotnames = false;
			for (const std::string& name : attributenames)
			{
				if (!name.empty())
				{
					gotnames = true;
					break;
				}
			}
			if (gotnames)
			{
				std::vector<int>ids(attributes.size(), id + 1);
				Rcpp::IntegerVector rids(ids.begin(), ids.end());
				Rcpp::StringVector rattributes(attributes.begin(), attributes.end());
				Rcpp::StringVector rnames(attributenames.begin(), attributenames.end());
				data.push_back(rids, "THEMES");
				data.push_back(rattributes, "ATTRIBUTES");
				data.push_back(rnames, "NAMES");
				data.attr("row.names") = Rcpp::seq(1, ids.size());
			}
		}
		data.attr("class") = "data.frame";
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTtheme::getaggregatesasdataframe", __LINE__, __FILE__);
	}
	return data;
}

#endif

bool FMTtheme::checkmask(const std::vector<Core::FMTtheme>& themes,
	const std::vector<std::string>& values, std::string& mask, const std::string& otherinformation)
{
	//otherinformation = " at line " + std::to_string(_line);
	bool returnvalue = true;
	if (themes.size() > values.size())
	{
		//_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange, mask + otherinformation,"FMTobject::checkmask", __LINE__, __FILE__, _section);
		const std::string original(mask);
		mask.clear();
		for (const std::string& value : values)
		{
			mask += value + " ";
		}
		for (size_t id = values.size(); id < themes.size(); ++id)
		{
			mask += "? ";
		}
		mask.pop_back();
		_exhandler->raise(Exception::FMTexc::FMTignore,
			"Extended mask " + original + " to " + mask, "FMTobject::checkmask", __LINE__, __FILE__);
		returnvalue = true;
	}
	else {
		size_t id = 0;
		const std::string original(mask);
		mask.clear();
		for (const Core::FMTtheme& theme : themes)
		{
			if (id < values.size() && !theme.isvalid(values[id]))
			{
				const std::string message = values[id] + " at theme " + std::to_string(theme.getid() + 1) + otherinformation;
				_exhandler->raise(Exception::FMTexc::FMTundefined_attribute, message,
					"FMTobject::checkmask", __LINE__, __FILE__);
				returnvalue = false;
			}
			std::string value = "?";
			if (id < values.size())
			{
				value = values[id];
			}
			mask += value + " ";
			++id;
		}
		mask.pop_back();
		if (values.size() != themes.size())
		{
			_exhandler->raise(Exception::FMTexc::FMTignore,
				"Subset mask " + original + " to " + mask, "FMTobject::checkmask", __LINE__, __FILE__);
		}

	}
	return  returnvalue;
}

bool FMTtheme::validate(const std::vector<Core::FMTtheme>& themes, std::string& mask, std::string otherinformation)
{
	std::vector<std::string>values;
	boost::split(values, mask, boost::is_any_of(" \t"), boost::token_compress_on);
	return checkmask(themes, values, mask, otherinformation);
}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTtheme)
