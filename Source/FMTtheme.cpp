/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtheme.h"
#include "FMTmask.h"
#include "FMTmaskfilter.h"
#include <boost/dynamic_bitset.hpp>
#include <unordered_map>
#include "FMTexceptionhandler.h"

#if defined FMTWITHR
	#include <Rcpp.h>
#endif 

namespace Core {


	FMTtheme::lookiterator FMTtheme::getAttribute(const std::string& p_value,bool p_raiseifnotfound) const
		{
		FMTtheme::lookiterator lookit = m_attribute_locations.end();
		try {
			lookit = m_attribute_locations.find(p_value);
			if (p_raiseifnotfound &&lookit== m_attribute_locations.end())
				{
				_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
					p_value + " at theme "+std::to_string(getid())+" "+getname(),
					"FMTtheme::getAttribute", __LINE__, __FILE__);
				}
		}catch (...)
			{
				_exhandler->raisefromcatch("", "FMTtheme::getAttribute", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
		return lookit;
		}


	void FMTtheme::buildAttributeLocations()
	{
		try {
			m_attribute_locations.clear();
			m_attribute_locations.reserve(m_attributes.size());
			size_t location = 0;
			for (const std::string& attribute : m_attributes)
				{
				m_attribute_locations[attribute] = std::vector<size_t>(1, location);
				++location;
				}
			bool processdone = false;
			std::vector<std::vector<std::string>>newaggregates = m_aggregatenames;
			while (!processdone)
			{
				processdone = true;
				size_t aggregatelocation = 0;
				for (const std::string& key : m_aggregates)
				{
					const std::vector<std::string> &aggvalues = newaggregates.at(aggregatelocation);
					std::vector<std::string>newvalues;
					newvalues.reserve(m_attributes.size());
					for (const std::string& value : aggvalues)
					{
						if (std::find(m_attributes.begin(), m_attributes.end(), value) == m_attributes.end())
						{
							const size_t baselocation = std::distance(m_aggregates.begin(), std::find(m_aggregates.begin(), m_aggregates.end(), value));
							for (const std::string& newvalue : newaggregates.at(baselocation))
							{
								if (std::find(newvalues.begin(), newvalues.end(), newvalue) == newvalues.end())
								{
									if (std::find(m_attributes.begin(), m_attributes.end(), newvalue) == m_attributes.end())
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
					locationofagg.push_back(std::distance(m_attributes.begin(), std::find(m_attributes.begin(), m_attributes.end(), val)));
					}
				m_attribute_locations[m_aggregates.at(agglocation)] = locationofagg;
				++agglocation;
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTtheme::buildAttributeLocations", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}

	}





FMTtheme::FMTtheme(const std::vector<std::string>& p_attributes,
				const std::vector<std::string>& p_attributenames,
				const std::vector<std::string>& p_aggregates,
				const std::vector<std::vector<std::string>>& p_aggregatenames,
				const std::vector<std::map<std::string, double>>& p_indexes ,
				const size_t& p_id, const size_t& p_start, const std::string& p_name) :
	FMTobject(),
	m_id(p_id),
	m_start(p_start),
	m_attributes(p_attributes),
	m_attributenames(p_attributenames),
	m_aggregates(p_aggregates),
	m_aggregatenames(p_aggregatenames),
	m_indexes(p_indexes),
	m_attribute_locations(),
	m_name(p_name)
	
{
	buildAttributeLocations();
}

FMTtheme::FMTtheme(const std::vector<std::string>& p_attributes,
	const std::vector<std::string>& p_attributenames,
	const std::vector<std::string>& p_aggregates,
	const std::vector<std::vector<std::string>>& p_aggregatenames,
	const size_t& p_id, const size_t& p_start, const std::string& p_name) :
	FMTobject(),
	m_id(p_id),
	m_start(p_start),
	m_attributes(p_attributes),
	m_attributenames(),
	m_aggregates(),
	m_aggregatenames(),
	m_indexes(),
	m_attribute_locations(),
	m_name()
	
	
{
	*this = FMTtheme(p_attributes, p_attributenames, p_aggregates,
		p_aggregatenames, std::vector<std::map<std::string, double>>(), p_id, p_start, p_name);
}


FMTtheme::FMTtheme(const std::vector<std::string>& p_attributes,
	const size_t& p_id, const size_t& p_start, const std::string& p_name):
	FMTobject(),
	m_id(p_id),
	m_start(p_start),
	m_attributes(p_attributes),
	m_attributenames(),
	m_aggregates(),
	m_aggregatenames(),
	m_indexes(),
	m_attribute_locations(),
	m_name(p_name)
	
{
	buildAttributeLocations();
}


FMTtheme::FMTtheme(const FMTtheme& p_rhs) :
	FMTobject(p_rhs),
	m_id(p_rhs.m_id),
	m_start(p_rhs.m_start),
	m_attributes(p_rhs.m_attributes),
	m_attributenames(p_rhs.m_attributenames),
	m_aggregates(p_rhs.m_aggregates),
	m_aggregatenames(p_rhs.m_aggregatenames),
	m_indexes(p_rhs.m_indexes),
	m_attribute_locations(p_rhs.m_attribute_locations),
	m_name(p_rhs.m_name)
	
{
	
}

FMTtheme& FMTtheme::operator = (const FMTtheme& p_rhs)
	{
	if (this!=&p_rhs)
		{
		FMTobject::operator=(p_rhs);
		m_id = p_rhs.m_id;
		m_start = p_rhs.m_start;
		m_attributes = p_rhs.m_attributes;
		m_attributenames = p_rhs.m_attributenames;
		m_aggregates = p_rhs.m_aggregates;
		m_aggregatenames = p_rhs.m_aggregatenames;
		m_indexes = p_rhs.m_indexes;
		m_name = p_rhs.m_name;
		m_attribute_locations = p_rhs.m_attribute_locations;
		}
	return *this;
	}


bool FMTtheme::inAggregate(const std::string& p_value, const std::string& p_aggregate)
{
	try {
		lookiterator lookit = getAttribute(p_aggregate);
		if (lookit != m_attribute_locations.end())
		{
			for (const size_t& location : lookit->second)
			{
				if (m_attributes.at(location) == p_value)
				{
					return true;
				}
			}

		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("For aggregate: " + p_aggregate + " at value " + p_value, "FMTtheme::inAggregate", __LINE__, __FILE__, Core::FMTsection::Landscape);
	}
	return false;
}

bool FMTtheme::isValid(const std::string& p_value) const
	{
	try {
		return (p_value == "?" || isattribute(p_value) || (p_value.at(0) == '!' && isattribute(p_value.substr(1, p_value.size()))));
	}
	catch (...)
	{
		_exhandler->raisefromcatch("For value: " + p_value, "FMTtheme::isValid", __LINE__, __FILE__, Core::FMTsection::Landscape);
	}
	return false;
	}


bool FMTtheme::isindex(const std::string& p_value) const
{
	try{
		for (const std::map<std::string, double>& indexer : m_indexes)
			{
			if (indexer.find(p_value)!=indexer.end())
				{
				return true;
				}
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("For value: " + p_value, "FMTtheme::isindex", __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	return false;

}

bool FMTtheme::isindex(const std::string& p_attribute, const std::string& p_value) const
{
	try {
		lookiterator lookit = getAttribute(p_attribute);
		if (lookit!=m_attribute_locations.end())
			{
			for (const size_t& location : lookit->second)
				{
				if (m_indexes.at(location).find(p_value)!= m_indexes.at(location).end())
					{
					return true;
					}
				}

			}
	}catch (...)
		{
		_exhandler->raisefromcatch("For attribute: " + p_attribute+" at value "+ p_value, "FMTtheme::isindex", __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	return false;
}

bool FMTtheme::useindex() const
    {
    if(!m_indexes.empty())
        {
        return true;
        }
    return false;
    }

 double FMTtheme::getindex(const std::string& p_attribute,const std::string& p_value) const
    {
	 try {
		 lookiterator lookit = getAttribute(p_attribute,true);
		 if (lookit != m_attribute_locations.end())
			{
			for (const size_t& location : lookit->second)
				{
				 if (m_indexes.at(location).find(p_value) != m_indexes.at(location).end())
					{
					return m_indexes.at(location).at(p_value);
					}
				}
			}
	 }catch (...)
		{
		 _exhandler->raisefromcatch("For attribute: " + p_attribute + " at value " + p_value, "FMTtheme::getindex", __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	return 0;
    }

bool FMTtheme::operator == (const FMTtheme& p_rhs) const
	{
	return (m_id == p_rhs.m_id &&
		m_start == p_rhs.m_start &&
		m_attributes == p_rhs.m_attributes &&
		m_attributenames == p_rhs.m_attributenames &&
		m_aggregates == p_rhs.m_aggregates &&
		m_aggregatenames == p_rhs.m_aggregatenames &&
		m_indexes == p_rhs.m_indexes &&
		m_name == p_rhs.m_name);
	}



 boost::dynamic_bitset<uint8_t> FMTtheme::strToBits(const std::string& p_value) const
        {
        boost::dynamic_bitset<uint8_t> bits;
		try{
			if (p_value == "?")
			{
				bits.resize(m_attributes.size(), true);
			}else {
				bool sense = true;
				lookiterator lookit = m_attribute_locations.end();
				if (p_value.at(0)=='!')
					{
					sense = false;
					lookit = getAttribute(p_value.substr(1, p_value.size()), true);
				}else {
					lookit = getAttribute(p_value, true);
					}
				bits.resize(m_attributes.size(),!sense);
				
				if (lookit != m_attribute_locations.end())
					{
					for (const size_t& location : lookit->second)
						{
						bits[location] = sense;
						}
					

				}else {
					_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
						std::string(p_value) + " for theme " + std::to_string(m_id), "FMTtheme::strToBits", __LINE__, __FILE__,Core::FMTsection::Landscape);
					}

				}
		}catch (...)
			{
			_exhandler->raisefromcatch("For value: " + p_value, "FMTtheme::strToBits", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
        return bits;
        }

std::string FMTtheme::bitsToStr(const boost::dynamic_bitset<uint8_t>& p_bits) const
        {
		try {
			const size_t bitcounts = p_bits.count();
			const size_t themesize = p_bits.size();
			if (themesize>1 && bitcounts == themesize)
			{
				return "?";
			}
			else {
				if (bitcounts == 1)
				{
					const size_t firsttrue = p_bits.find_first();
					if (firsttrue < m_attributes.size())
					{
						return m_attributes.at(firsttrue);
					}
				}
				else {
					for (const std::string& aggregate : m_aggregates)
					{
						 boost::dynamic_bitset<uint8_t>totest = strToBits(aggregate);
						if (totest == p_bits)
						{
							return aggregate;
						}else if (totest.flip() == p_bits)
							{
							return "!" + aggregate;
							}
					}
					 boost::dynamic_bitset<uint8_t> flipped(p_bits);
					flipped.flip();
					if (flipped.count()==1)
					{
						const size_t firstTrue = flipped.find_first();
						if (firstTrue < m_attributes.size())
						{
							return "!" + m_attributes.at(firstTrue);
						}
					}
				}
				_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
					"for bitset count of " + std::to_string(bitcounts) + " in theme " + std::to_string(m_id), "FMTtheme:::bitsToStr", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTtheme:::bitsToStr", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
		return "";
        }

std::vector<std::string>FMTtheme::getattributes(const std::string& p_value, bool p_aggregate_source) const
        {
		std::vector<std::string>result;
		try {
			if (p_value == "?")
			{
				result = m_attributes;
			}else if(p_value.at(0)=='!')
			{ 
				const std::string TARGET = p_value.substr(1, p_value.size());
				lookiterator lookit = getAttribute(TARGET, true);
				std::vector<std::string>BANNED;
				if (isaggregate(TARGET) && lookit != m_attribute_locations.end())
				{
					BANNED.reserve(lookit->second.size());
					for (const size_t& location : lookit->second)
					{
						BANNED.push_back(m_attributes.at(location));
					}
				}else if (lookit != m_attribute_locations.end())
				{
					BANNED.push_back(m_attributes.at(*lookit->second.begin()));
				}
				for (const std::string& ATTRIBUTE : m_attributes)
					{
					if (std::find(BANNED.begin(), BANNED.end(), ATTRIBUTE)== BANNED.end())
						{
						result.push_back(ATTRIBUTE);
						}
					}
			
			}else {
				lookiterator lookit = getAttribute(p_value, true);
				if (isaggregate(p_value) && lookit != m_attribute_locations.end())
				{
					if (p_aggregate_source)
					{
						std::vector<std::string>::const_iterator cntit = std::find(m_aggregates.begin(), m_aggregates.end(), p_value);
						result = m_aggregatenames.at(std::distance(m_aggregates.begin(), cntit));
					}
					else {
						result.reserve(lookit->second.size());
						for (const size_t& location : lookit->second)
						{
							result.push_back(m_attributes.at(location));
						}
					}

				}
				else if (lookit != m_attribute_locations.end())
				{
					result.push_back(m_attributes.at(*lookit->second.begin()));
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
		std::string fulltheme = "*THEME "+ std::to_string(m_id+1)+" "+ m_name +"\n";
		try {
			for (size_t location = 0; location < m_attributes.size(); ++location)
				{
				fulltheme += " " + m_attributes.at(location);
				if (!m_attributenames.empty())
					{
					fulltheme += " " + m_attributenames.at(location);
					}
				if (!m_indexes.empty())
					{
					if (!m_indexes.at(location).empty())
						{
						fulltheme += " _INDEX(";
						for (std::map<std::string, double>::const_iterator mit = m_indexes.at(location).begin(); mit != m_indexes.at(location).end(); mit++)
						{
							fulltheme += (mit->first + "=" + std::to_string(mit->second) + ",");
						}
						fulltheme.pop_back();
						fulltheme += ")";
						}
					}
				fulltheme +="\n";
				}
			for (size_t location = 0; location < m_aggregates.size(); ++location)
			{
				fulltheme += "*AGGREGATE " + m_aggregates.at(location) + "\n";
				for (const std::string& aggregatename : m_aggregatenames.at(location))
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

void FMTtheme::fillupAggregates(std::vector<int>& p_themeids, std::vector<std::string>& p_locattributes, std::vector<std::string>& p_locaggregates) const
{
	try {
		for (const std::string& aggregate : m_aggregates)
			{
			for (const std::string& attribute : getattributes(aggregate))
				{
				p_themeids.push_back(static_cast<int>(m_id)+1);
				p_locattributes.push_back(attribute);
				p_locaggregates.push_back(aggregate);
				}
			}
	}catch (...)
	{
		_exhandler->raisefromcatch("for theme " + std::to_string(m_id), "FMTtheme::fillupAggregates", __LINE__, __FILE__, Core::FMTsection::Landscape);
	}

}


void FMTtheme::push_aggregate(const std::string& p_aggregatename)
	{
	try {
		if (isaggregate(p_aggregatename))
			{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				p_aggregatename+" is already an aggregate", "FMTtheme::presolve", __LINE__, __FILE__);
			}
		m_aggregates.push_back(p_aggregatename);
		m_aggregatenames.push_back(std::vector<std::string>());
		//buildattributelocations();
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTtheme::push_aggregate", __LINE__, __FILE__);
		}
	}


void FMTtheme::push_aggregate_value(const std::string& p_aggregatename, const std::string& p_value)
	{
	try {
		if (isaggregate(p_aggregatename))
			{
			m_aggregatenames[std::distance(m_aggregates.begin(), std::find(m_aggregates.begin(), m_aggregates.end(), p_aggregatename))].push_back(p_value);
			}
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTtheme::push_aggregate_value", __LINE__, __FILE__);
		}
	}

std::string FMTtheme::updateFromMask(const Core::FMTmask& p_globalmask)
	{
	try {
		const boost::dynamic_bitset<uint8_t>global = p_globalmask.subset(*this);
		if ((global.count()>1)&&(global.count()<global.size()))
			{
			std::string lastFMTaggregate("~FMT"+std::to_string(getid())+"A_0");
			for (const std::string& aggregate : m_aggregates)
				{
				if (strToBits(aggregate) == global)
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
					push_aggregate_value(newaggregate,m_attributes.at(bid));
					aggregateindex.push_back(bid);
					}

				}
			m_attribute_locations[newaggregate] = aggregateindex;
			//buildattributelocations();
			}
		return bitsToStr(global);
	}catch (...)
		{
		_exhandler->raisefromcatch("", "FMTtheme::updateFromMask", __LINE__, __FILE__);
		}
	return "?";
	}

std::vector<std::string>FMTtheme::getaggregates() const
	{
	return m_aggregates;
	}


FMTtheme FMTtheme::presolve(FMTmaskfilter& p_maskfilter, size_t& p_newid, size_t& p_newstart) const
	{
	try {
		if (p_maskfilter.empty())
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
				"Empty selection", "FMTtheme::presolve", __LINE__, __FILE__);
		}
		if (p_maskfilter.flippedselection.empty())
		{
			p_maskfilter.flippedselection = boost::dynamic_bitset<uint8_t>(p_maskfilter.selection.size(), false);
		}
		FMTtheme newtheme(*this);
		newtheme.m_attributes.clear();
		newtheme.m_attributenames.clear();
		newtheme.m_aggregates.clear();
		newtheme.m_aggregatenames.clear();
		newtheme.m_indexes.clear();
		newtheme.m_attribute_locations.clear();

		std::vector<std::string>newattributes;
		newattributes.reserve(m_attributes.size());
		std::vector<std::string>newattributenames;
		newattributenames.reserve(m_attributenames.size());
		size_t location = 0;
		for (size_t binlocation = m_start; binlocation < (m_start + this->size()); ++binlocation)
			{
				if (p_maskfilter.selection[binlocation])
				{
					p_maskfilter.flippedselection[binlocation] = true;
					newattributes.push_back(m_attributes.at(location));
					if (!m_attributenames.empty())
						{
						newattributenames.push_back(m_attributenames.at(location));
						}
					
				}
				++location;
			}

		if (newattributes.size() > 1)
		{
			newtheme.m_attributes = newattributes;
			newtheme.m_attributenames = newattributenames;
			newtheme.m_id = p_newid;
			++p_newid;
			newtheme.m_start = p_newstart;
			p_newstart += newtheme.size();
			for (const std::string& aggregate : m_aggregates)
				{
				std::vector<std::string>aggvalues;
				const std::vector<size_t> & aggAttributes = getAttribute(aggregate)->second;
				aggvalues.reserve(aggAttributes.size());
				for (const size_t& baselocation : aggAttributes)
					{
					if (std::find(newattributes.begin(),newattributes.end(), m_attributes.at(baselocation))!= newattributes.end())
						{
						aggvalues.push_back(m_attributes.at(baselocation));
						}
					}
				if (!aggvalues.empty())
					{
					newtheme.m_aggregates.push_back(aggregate);
					newtheme.m_aggregatenames.push_back(aggvalues);
					}
				}
		}else {
			for (size_t binlocation = m_start; binlocation < (m_start + this->size()); ++binlocation)
			{
				p_maskfilter.flippedselection[binlocation] = false;
			}
		}
		newtheme.buildAttributeLocations();
		return newtheme;
	}catch (...)
		{
		_exhandler->raisefromcatch("for theme "+std::to_string(m_id),"FMTtheme::presolve", __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	return FMTtheme();
	}


FMTthemecomparator::FMTthemecomparator(const FMTtheme& p_lbase_theme, const bool& p_lcomparedwithpresolved): 
	m_base_theme(p_lbase_theme),m_comparedwithpresolved(p_lcomparedwithpresolved)
	{

	}

bool FMTthemecomparator::operator()(const FMTtheme& p_theme) const
	{
	if (!m_comparedwithpresolved)
	{
		return (p_theme == m_base_theme);
	}else{
		for (const std::string& attribute : p_theme.getbaseattributes())
		{
			if (!m_base_theme.isattribute(attribute))
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
		if (!m_aggregates.empty())
			{
			std::vector<int>themeids;
			std::vector<std::string>locattributes;
			std::vector<std::string>locaggregates;
			this->fillupAggregates(themeids, locattributes, locaggregates);
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
		if (!m_attributenames.empty())
		{
			bool gotnames = false;
			for (const std::string& name : m_attributenames)
			{
				if (!name.empty())
				{
					gotnames = true;
					break;
				}
			}
			if (gotnames)
			{
				std::vector<int>ids(m_attributes.size(), m_id + 1);
				Rcpp::IntegerVector rids(ids.begin(), ids.end());
				Rcpp::StringVector rattributes(m_attributes.begin(), m_attributes.end());
				Rcpp::StringVector rnames(m_attributenames.begin(), m_attributenames.end());
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

bool FMTtheme::checkMask(const std::vector<Core::FMTtheme>& p_themes,
	const std::vector<std::string>& p_values, std::string& p_mask, const std::string& p_otherinformation)
{
	//otherinformation = " at line " + std::to_string(_line);
	bool returnvalue = true;
	if (p_themes.size() > p_values.size())
	{
		//_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange, mask + otherinformation,"FMTobject::checkmask", __LINE__, __FILE__, _section);
		const std::string original(p_mask);
		p_mask.clear();
		for (const std::string& value : p_values)
		{
			p_mask += value + " ";
		}
		for (size_t id = p_values.size(); id < p_themes.size(); ++id)
		{
			p_mask += "? ";
		}
		p_mask.pop_back();
		_exhandler->raise(Exception::FMTexc::FMTignore,
			"Extended mask " + original + " to " + p_mask, "FMTobject::checkmask", __LINE__, __FILE__);
		returnvalue = true;
	}
	else {
		size_t id = 0;
		const std::string original(p_mask);
		p_mask.clear();
		for (const Core::FMTtheme& theme : p_themes)
		{
			if (id < p_values.size() && !theme.isValid(p_values[id]))
			{
				const std::string message = p_values[id] + " at theme " + std::to_string(theme.getid() + 1) + p_otherinformation;
				_exhandler->raise(Exception::FMTexc::FMTundefined_attribute, message,
					"FMTthem::checkmask", __LINE__, __FILE__);
				returnvalue = false;
			}
			std::string value = "?";
			if (id < p_values.size())
			{
				value = p_values[id];
			}
			p_mask += value + " ";
			++id;
		}
		p_mask.pop_back();
		if (p_values.size() != p_themes.size())
		{
			_exhandler->raise(Exception::FMTexc::FMTignore,
				"Subset mask " + original + " to " + p_mask, "FMTtheme::checkmask", __LINE__, __FILE__);
		}

	}
	return  returnvalue;
}

bool FMTtheme::validate(const std::vector<Core::FMTtheme>& p_themes, std::string& p_mask, std::string p_otherinformation)
{
	std::vector<std::string>values;
	boost::split(values, p_mask, boost::is_any_of(" \t"), boost::token_compress_on);
	return checkMask(p_themes, values, p_mask, p_otherinformation);
}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTtheme)
