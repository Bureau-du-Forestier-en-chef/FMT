/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTheme.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include <boost/dynamic_bitset.hpp>
#include <unordered_map>
#include "FMTExceptionHandler.h"

#if defined FMTWITHR
	#include <Rcpp.h>
#endif 

namespace Core {

	FMTTheme::FMTTheme():
		m_id(),
		m_start(),
		m_attributes(),
		m_attributenames(),
		m_aggregates(),
		m_aggregatenames(),
		m_indexes(),
		m_attributem_locations(),
		m_name()
		{

		}


	FMTTheme::lookiterator FMTTheme::getAttribute(const std::string& p_value,bool p_raiseifnotfound) const
		{
		FMTTheme::lookiterator lookit = m_attributem_locations.end();
		try {
			lookit = m_attributem_locations.find(p_value);
			if (p_raiseifnotfound &&lookit== m_attributem_locations.end())
				{
				_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
					p_value + " at theme "+std::to_string(getId())+" "+getName(),
					"FMTTheme::getAttribute", __LINE__, __FILE__);
				}
		}catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTTheme::getAttribute", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
		return lookit;
		}


	void FMTTheme::buildAttributeLocations()
	{
		try {
			m_attributem_locations.clear();
			m_attributem_locations.reserve(m_attributes.size());
			size_t location = 0;
			for (const std::string& attribute : m_attributes)
				{
				m_attributem_locations[attribute] = std::vector<size_t>(1, location);
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
				m_attributem_locations[m_aggregates.at(agglocation)] = locationofagg;
				++agglocation;
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTTheme::buildAttributeLocations", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}

	}





FMTTheme::FMTTheme(const std::vector<std::string>& p_attributes,
				const std::vector<std::string>& p_attributenames,
				const std::vector<std::string>& p_aggregates,
				const std::vector<std::vector<std::string>>& p_aggregatenames,
				const std::vector<std::map<std::string, double>>& p_indexes ,
				const size_t& p_id, const size_t& p_start, const std::string& p_name) :
	FMTObject(),
	m_id(p_id),
	m_start(p_start),
	m_attributes(p_attributes),
	m_attributenames(p_attributenames),
	m_aggregates(p_aggregates),
	m_aggregatenames(p_aggregatenames),
	m_indexes(p_indexes),
	m_attributem_locations(),
	m_name(p_name)
	
{
	buildAttributeLocations();
}

FMTTheme::FMTTheme(const std::vector<std::string>& p_attributes,
	const std::vector<std::string>& p_attributenames,
	const std::vector<std::string>& p_aggregates,
	const std::vector<std::vector<std::string>>& p_aggregatenames,
	const size_t& p_id, const size_t& p_start, const std::string& p_name) :
	FMTObject(),
	m_id(p_id),
	m_start(p_start),
	m_attributes(p_attributes),
	m_attributenames(),
	m_aggregates(),
	m_aggregatenames(),
	m_indexes(),
	m_attributem_locations(),
	m_name()
	
	
{
	*this = FMTTheme(p_attributes, p_attributenames, p_aggregates,
		p_aggregatenames, std::vector<std::map<std::string, double>>(), p_id, p_start, p_name);
}


FMTTheme::FMTTheme(const std::vector<std::string>& p_attributes,
	const size_t& p_id, const size_t& p_start, const std::string& p_name):
	FMTObject(),
	m_id(p_id),
	m_start(p_start),
	m_attributes(p_attributes),
	m_attributenames(),
	m_aggregates(),
	m_aggregatenames(),
	m_indexes(),
	m_attributem_locations(),
	m_name(p_name)
	
{
	buildAttributeLocations();
}


FMTTheme::FMTTheme(const FMTTheme& p_rhs) :
	FMTObject(p_rhs),
	m_id(p_rhs.m_id),
	m_start(p_rhs.m_start),
	m_attributes(p_rhs.m_attributes),
	m_attributenames(p_rhs.m_attributenames),
	m_aggregates(p_rhs.m_aggregates),
	m_aggregatenames(p_rhs.m_aggregatenames),
	m_indexes(p_rhs.m_indexes),
	m_attributem_locations(p_rhs.m_attributem_locations),
	m_name(p_rhs.m_name)
	
{
	
}

FMTTheme& FMTTheme::operator = (const FMTTheme& p_rhs)
	{
	if (this!=&p_rhs)
		{
		FMTObject::operator=(p_rhs);
		m_id = p_rhs.m_id;
		m_start = p_rhs.m_start;
		m_attributes = p_rhs.m_attributes;
		m_attributenames = p_rhs.m_attributenames;
		m_aggregates = p_rhs.m_aggregates;
		m_aggregatenames = p_rhs.m_aggregatenames;
		m_indexes = p_rhs.m_indexes;
		m_name = p_rhs.m_name;
		m_attributem_locations = p_rhs.m_attributem_locations;
		}
	return *this;
	}


bool FMTTheme::inAggregate(const std::string& p_value, const std::string& p_aggregate)
{
	try {
		lookiterator lookit = getAttribute(p_aggregate);
		if (lookit != m_attributem_locations.end())
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
		_exhandler->raiseFromCatch("For aggregate: " + p_aggregate + " at value " + p_value, "FMTTheme::inAggregate", __LINE__, __FILE__, Core::FMTsection::Landscape);
	}
	return false;
}

bool FMTTheme::isValid(const std::string& p_value) const
	{
	try {
		return (p_value == "?" || isAttribute(p_value));
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("For value: " + p_value, "FMTTheme::isValid", __LINE__, __FILE__, Core::FMTsection::Landscape);
	}
	return false;
	}


bool FMTTheme::isIndex(const std::string& p_value) const
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
		_exhandler->raiseFromCatch("For value: " + p_value, "FMTTheme::isIndex", __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	return false;

}

bool FMTTheme::isIndex(const std::string& p_attribute, const std::string& p_value) const
{
	try {
		lookiterator lookit = getAttribute(p_attribute);
		if (lookit!=m_attributem_locations.end())
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
		_exhandler->raiseFromCatch("For attribute: " + p_attribute+" at value "+ p_value, "FMTTheme::isIndex", __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	return false;
}

bool FMTTheme::useIndex() const
    {
    if(!m_indexes.empty())
        {
        return true;
        }
    return false;
    }

 double FMTTheme::getIndex(const std::string& p_attribute,const std::string& p_value) const
    {
	 try {
		 lookiterator lookit = getAttribute(p_attribute,true);
		 if (lookit != m_attributem_locations.end())
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
		 _exhandler->raiseFromCatch("For attribute: " + p_attribute + " at value " + p_value, "FMTTheme::getIndex", __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	return 0;
    }

bool FMTTheme::operator == (const FMTTheme& p_rhs) const
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



 boost::dynamic_bitset<uint8_t> FMTTheme::strToBits(const std::string& p_value) const
        {
        boost::dynamic_bitset<uint8_t> bits;
		try{
			if (p_value == "?")
			{
				bits.resize(m_attributes.size(), true);
			}else {
				bool sense = true;
				lookiterator lookit = m_attributem_locations.end();
				if (p_value.at(0)=='!')
					{
					sense = false;
					lookit = getAttribute(p_value.substr(1, p_value.size()), true);
				}else {
					lookit = getAttribute(p_value, true);
					}
				bits.resize(m_attributes.size(),!sense);
				
				if (lookit != m_attributem_locations.end())
					{
					for (const size_t& location : lookit->second)
						{
						bits[location] = sense;
						}
					

				}else {
					_exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
						std::string(p_value) + " for theme " + std::to_string(m_id), "FMTTheme::strToBits", __LINE__, __FILE__,Core::FMTsection::Landscape);
					}

				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("For value: " + p_value, "FMTTheme::strToBits", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
        return bits;
        }

 std::string FMTTheme::bitsToStr(const Core::FMTMask& p_mask) const
	{
	 try {
		 const size_t BITS_COUNT = _getCount(p_mask);
		 const size_t BITS_SIZE = size();
		 if (BITS_SIZE > 1 && BITS_COUNT == BITS_SIZE)
		 {
			 return "?";
		 }
		 else {
			 if (BITS_COUNT == 1)
			 {
				 const size_t FIRST_TRUE = _findFirst(p_mask);
				 if (FIRST_TRUE < m_attributes.size())
				 {
					 return m_attributes.at(FIRST_TRUE);
				 }
			 }
			 else {
				 for (const std::string& AGGREGATE : m_aggregates)
				 {
					 const boost::dynamic_bitset<uint8_t>TO_TEST = strToBits(AGGREGATE);
					 if (_isEqual(p_mask, TO_TEST))
					 {
						 return AGGREGATE;
					 }
					 else if (_isFlipEqual(p_mask, TO_TEST))
					 {
						 return "!" + AGGREGATE;
					 }
				 }
				 if (_getFlipCount(p_mask) == 1)
				 {
					 const size_t FIRST_TRUE = _findFirstFlip(p_mask);
					 if (FIRST_TRUE < m_attributes.size())
					 {
						 return "!" + m_attributes.at(FIRST_TRUE);
					 }
				 }
			 }
			 _exhandler->raise(Exception::FMTexc::FMTundefined_attribute,
				 "for bitset count of " + std::to_string(BITS_COUNT) + " in theme " + std::to_string(m_id),
				 "FMTTheme:::bitsToStr", __LINE__, __FILE__, Core::FMTsection::Landscape);
		 }


	 }catch (...)
		{
		 _exhandler->raiseFromCatch("", "FMTTheme::bitsToStr",
			 __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	}

 size_t FMTTheme::_getCount(const Core::FMTMask& p_mask) const
	{
	size_t countOf = 0;
	for (size_t i = m_start; i < (m_start + size());++i)
		{
		countOf += static_cast<size_t>(p_mask[i]);
		}
	return countOf; 
	}

 size_t FMTTheme::_getFlipCount(const Core::FMTMask& p_mask) const
 {
	 size_t countOf = 0;
	 for (size_t i = m_start; i < (m_start + size()); ++i)
	 {
		 countOf += static_cast<size_t>(!p_mask[i]);
	 }
	 return countOf;
 }

 size_t FMTTheme::_findFirstFlip(const Core::FMTMask& p_mask) const
 {
	 size_t STOP = (m_start + size());
	 size_t firstOf = STOP;
	 size_t i = m_start;
	 while (firstOf == STOP &&
		 i < STOP)
	 {
		 if (!p_mask[i])
		 {
			 firstOf = i - m_start;
		 }
		 ++i;
	 }
	 return firstOf;
 }

 size_t FMTTheme::_findFirst(const Core::FMTMask& p_mask) const
	{
	const size_t STOP = (m_start + size());
	size_t firstOf = STOP;
	size_t i = m_start;
	while (firstOf== STOP &&
				i<STOP)
		{
		if (p_mask[i])
			{
			firstOf = i - m_start;
			}
		++i;
		}
	return firstOf;
	}

 bool FMTTheme::_isEqual(const Core::FMTMask& p_mask,
	 const boost::dynamic_bitset<uint8_t>& p_bits) const
 {
	 size_t j= 0;
	 for (size_t i = m_start; i < (m_start + size()); ++i)
	 {
		 if (p_mask[i] != p_bits[j])
			{
			return false;
			}
		++j;
	 }
	 return true;
 }

 bool FMTTheme::_isFlipEqual(const Core::FMTMask& p_mask,
	 const boost::dynamic_bitset<uint8_t>& p_bits) const
 {
	 size_t j = 0;
	 for (size_t i = m_start; i < (m_start + size()); ++i)
	 {
		 if (p_mask[i] != (!p_bits[j]))
		 {
			 return false;
		 }
		 ++j;
	 }
	 return true;
 }



const std::string& FMTTheme::_getAttribute(size_t p_attributeId) const
	{
	return m_attributes.at(p_attributeId);
	}


std::vector<std::string>FMTTheme::getAttributes(const std::string& p_value, bool p_aggregate_source) const
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
				if (isAggregate(TARGET) && lookit != m_attributem_locations.end())
				{
					BANNED.reserve(lookit->second.size());
					for (const size_t& location : lookit->second)
					{
						BANNED.push_back(m_attributes.at(location));
					}
				}else if (lookit != m_attributem_locations.end())
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
				if (isAggregate(p_value) && lookit != m_attributem_locations.end())
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
				else if (lookit != m_attributem_locations.end())
				{
					result.push_back(m_attributes.at(*lookit->second.begin()));
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTTheme::getAttributes", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
        return result;
        }

FMTTheme::operator std::string() const
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
			_exhandler->raiseFromCatch("", "FMTTheme::operator::std::string()", __LINE__, __FILE__, Core::FMTsection::Landscape);
			}
        return fulltheme;
        }

void FMTTheme::fillupAggregates(std::vector<int>& p_themeids, std::vector<std::string>& p_locattributes, std::vector<std::string>& p_locaggregates) const
{
	try {
		for (const std::string& aggregate : m_aggregates)
			{
			for (const std::string& attribute : getAttributes(aggregate))
				{
				p_themeids.push_back(static_cast<int>(m_id)+1);
				p_locattributes.push_back(attribute);
				p_locaggregates.push_back(aggregate);
				}
			}
	}catch (...)
	{
		_exhandler->raiseFromCatch("for theme " + std::to_string(m_id), "FMTTheme::fillupAggregates", __LINE__, __FILE__, Core::FMTsection::Landscape);
	}

}


void FMTTheme::pushAggregate(const std::string& p_aggregatename)
	{
	try {
		if (isAggregate(p_aggregatename))
			{
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				p_aggregatename+" is already an aggregate", "FMTTheme::presolve", __LINE__, __FILE__);
			}
		m_aggregates.push_back(p_aggregatename);
		m_aggregatenames.push_back(std::vector<std::string>());
		//buildattributelocations();
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTTheme::pushAggregate", __LINE__, __FILE__);
		}
	}


void FMTTheme::pushAggregateValue(const std::string& p_aggregatename, const std::string& p_value)
	{
	try {
		if (isAggregate(p_aggregatename))
			{
			m_aggregatenames[std::distance(m_aggregates.begin(), std::find(m_aggregates.begin(), m_aggregates.end(), p_aggregatename))].push_back(p_value);
			}
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTTheme::pushAggregateValue", __LINE__, __FILE__);
		}
	}

std::string FMTTheme::updateFromMask(const Core::FMTMask& p_globalmask)
	{
	try {
		const boost::dynamic_bitset<uint8_t>global = p_globalmask.subset(*this);
		if ((global.count()>1)&&(global.count()<global.size()))
			{
			std::string lastFMTaggregate("~FMT"+std::to_string(getId())+"A_0");
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
			pushAggregate(newaggregate);
			std::vector<size_t>aggregateindex;
			for (size_t bid = 0; bid < global.size();++bid)
				{
				if (global[bid])
					{
					pushAggregateValue(newaggregate,m_attributes.at(bid));
					aggregateindex.push_back(bid);
					}

				}
			m_attributem_locations[newaggregate] = aggregateindex;
			//buildattributelocations();
			}
		return bitsToStr(p_globalmask);
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTTheme::updateFromMask", __LINE__, __FILE__);
		}
	return "?";
	}

std::vector<std::string>FMTTheme::getAggregates() const
	{
	return m_aggregates;
	}


FMTTheme FMTTheme::presolve(FMTMaskFilter& p_maskfilter, size_t& p_newid, size_t& p_newstart) const
	{
	try {
		if (p_maskfilter.empty())
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange,
				"Empty selection", "FMTTheme::presolve", __LINE__, __FILE__);
		}
		if (p_maskfilter.flippedselection.empty())
		{
			p_maskfilter.flippedselection = boost::dynamic_bitset<uint8_t>(p_maskfilter.selection.size(), false);
		}
		FMTTheme newtheme(*this);
		newtheme.m_attributes.clear();
		newtheme.m_attributenames.clear();
		newtheme.m_aggregates.clear();
		newtheme.m_aggregatenames.clear();
		newtheme.m_indexes.clear();
		newtheme.m_attributem_locations.clear();

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
		_exhandler->raiseFromCatch("for theme "+std::to_string(m_id),"FMTTheme::presolve", __LINE__, __FILE__, Core::FMTsection::Landscape);
		}
	return FMTTheme();
	}


FMTThemeComparator::FMTThemeComparator(const FMTTheme& p_lbase_theme, const bool& p_lcomparedwithpresolved): 
	m_base_theme(p_lbase_theme),m_comparedwithpresolved(p_lcomparedwithpresolved)
	{

	}

bool FMTThemeComparator::operator()(const FMTTheme& p_theme) const
	{
	if (!m_comparedwithpresolved)
	{
		return (p_theme == m_base_theme);
	}else{
		for (const std::string& attribute : p_theme.getBaseAttributes())
		{
			if (!m_base_theme.isAttribute(attribute))
			{
				return false;
			}
		}
		return true;
	}
	}

#if defined FMTWITHR
Rcpp::DataFrame FMTTheme::getAggregatesAsDataFrame() const
{
	Rcpp::DataFrame data = Rcpp::DataFrame();
	try {
		if (!m_aggregates.empty())
			{
			std::vector<int>themeids;
			std::vector<std::string>locattributes;
			std::vector<std::string>locaggregates;
			this->fillupAggregates(themeids, locattributes, locaggregates);
			Rcpp::IntegerVector rids(themeids.size());
			std::copy(themeids.begin(), themeids.end(), rids.begin());
			Rcpp::StringVector Rattributes(locattributes.size());
			std::copy(locattributes.begin(), locattributes.end(), Rattributes.begin());
			Rcpp::StringVector Raggregates(locaggregates.size());
			std::copy(locaggregates.begin(), locaggregates.end(), Raggregates.begin());
			data.push_back(rids, "THEMES");
			data.push_back(Rattributes, "ATTRIBUTES");
			data.push_back(Raggregates, "AGGREGATES");
			if (themeids.size() == 0) {
				data.attr("row.names") = Rcpp::IntegerVector::create(0); // Attribuer simplement 0
			}
			else {
				data.attr("row.names") = Rcpp::seq(1, themeids.size());
			}
			}
		data.attr("class") = "data.frame";
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTTheme::getAggregatesAsDataFrame", __LINE__, __FILE__);
	}
	return data;
}

Rcpp::DataFrame FMTTheme::getAttributesAsDataFrame() const
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
		_exhandler->raiseFromCatch("", "FMTTheme::getAggregatesAsDataFrame", __LINE__, __FILE__);
	}
	return data;
}

#endif

bool FMTTheme::checkMask(const std::vector<Core::FMTTheme>& p_themes,
	const std::vector<std::string>& p_values, std::string& p_mask, const std::string& p_otherinformation)
{
	//otherinformation = " at line " + std::to_string(m_line);
	bool returnvalue = true;
	if (p_themes.size() > p_values.size())
	{
		//_exhandler->raise(Exception::FMTexc::FMTinvalid_maskrange, mask + otherinformation,"FMTObject::checkMask", __LINE__, __FILE__, m_section);
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
			"Extended mask " + original + " to " + p_mask, "FMTObject::checkMask", __LINE__, __FILE__);
		//returnvalue = true; //???
	}
	size_t id = 0;
	const std::string original(p_mask);

	p_mask.clear();
	for (const Core::FMTTheme& theme : p_themes)
	{
		if (id < p_values.size() && !theme.isValid(p_values[id]))
		{
			const std::string message = p_values[id] + " at theme " + std::to_string(theme.getId() + 1) + p_otherinformation;
			_exhandler->raise(Exception::FMTexc::FMTundefined_attribute, message,
				"FMTthem::checkMask", __LINE__, __FILE__);
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
			"Subset mask " + original + " to " + p_mask, "FMTTheme::checkMask", __LINE__, __FILE__);
	}

	return  returnvalue;
}

	bool FMTTheme::validate(const std::vector<Core::FMTTheme>& p_themes, std::string& p_mask, std::string p_otherinformation)
	{
		std::vector<std::string>values;
		boost::split(values, p_mask, boost::is_any_of(" \t"), boost::token_compress_on);
		return checkMask(p_themes, values, p_mask, p_otherinformation);
	}


}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTTheme)
