/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include "FMTMask.h"
#include "FMTTheme.h"
#include "FMTMaskFilter.h"
#include <stdexcept>

namespace Core{

FMTMask::FMTMask(): m_name(),m_data()
    {

    }

FMTMask::operator bool() const
	{
	return (!m_data.empty());
	}

FMTMask::FMTMask(const std::string& mask,const boost::dynamic_bitset<uint8_t>& bits):m_name(mask),m_data(bits)
	{
	//name.shrink_to_fit();
	}
FMTMask::FMTMask(const boost::dynamic_bitset<uint8_t>& bits): m_name(),m_data(bits)
	{

	}

FMTMask::FMTMask(const std::vector<FMTTheme>& themes)
	{
	size_t basesize = 0;
	for (const FMTTheme& theme : themes)
	{
		basesize += theme.size();
		m_name += "!? ";
	}
	m_name.pop_back();
	m_data.resize(basesize, false);
	}


FMTMask::FMTMask(const std::string& mask,const std::vector<FMTTheme>& themes):m_name(),m_data()
    {
	this->set(themes, mask);
	//name.shrink_to_fit();
    }
FMTMask::FMTMask(const std::vector<std::string>&values,const std::vector<FMTTheme>& themes):m_name(),m_data()
    {
    for(const std::string& el : values)
        {
        m_name+=el+" ";
        }
    //name = name.substr(0, name.size()-1);
	m_name.pop_back();
	this->set(themes, m_name);
	//name.shrink_to_fit();
    }
std::vector<FMTMask> FMTMask::decompose(const FMTTheme &theme) const
    {
    const std::string VALUE = get(theme);
	const std::vector<std::string>ATTRIBUTES = theme.getAttributes(VALUE);
	std::vector<FMTMask>NewMasks(ATTRIBUTES.size(), *this);
	for (size_t Id = 0; Id < NewMasks.size();++Id)
		{
		NewMasks[Id].set(theme, ATTRIBUTES.at(Id));
		}
    return NewMasks;
    }
 boost::dynamic_bitset<uint8_t> FMTMask::_subset(const FMTTheme& theme) const
    {
    boost::dynamic_bitset<uint8_t> sub(theme.size(),false);
    int locit = 0;
    for(size_t id = theme.m_start; id < (theme.m_start + theme.size()); ++id)
        {
		sub.at(locit) = m_data.at(id);
        ++locit;
        }
    return sub;
    }

 bool FMTMask::operator [](int i) const
	{
	return m_data[i];
	}


size_t FMTMask::_countTheme(const FMTTheme& theme) const
 {
	 size_t count = 0;
	 for (size_t id = theme.m_start; id < (theme.m_start + theme.size()); ++id)
	 {
		 count += static_cast<size_t>(m_data.at(id));
	
	 }
	 return count;
 }

 bool FMTMask::_anyIntersect(const FMTMask& p_MASK, const FMTTheme& p_THEME) const
	 {
	 size_t bIt = p_THEME.m_start;
	 bool gotIntersect = false;
	 while (!gotIntersect &&
		 bIt < (p_THEME.m_start + p_THEME.size()))
		 {
		 gotIntersect = m_data.at(bIt) && p_MASK.m_data.at(bIt);
		 ++bIt;
		 }
	 return gotIntersect;
	 }

FMTMask FMTMask::getPostSolveMask(const FMTMask& mask, const std::vector<FMTTheme>& themes) const
	{
	FMTMask postsolvedmask(*this);
	for (const Core::FMTTheme & basetheme : themes)
		{
		if (mask.getSubsetCount(basetheme)!=0)//scrap this theme in the devmask
			{
			 boost::dynamic_bitset<uint8_t> sub(basetheme.size(),false);
			postsolvedmask._setSubset(basetheme, sub);
			}
		}
	return postsolvedmask;
	}

size_t FMTMask::getSubsetCount(const FMTTheme& theme) const
{
	return _countTheme(theme);
}

void FMTMask::_setSubset(const FMTTheme& theme,const boost::dynamic_bitset<uint8_t>& subset)
    {
    int locit = 0;
    for(size_t id = theme.m_start; id < (theme.m_start+theme.size()); ++id)
        {
		m_data.at(id) = subset.at(locit);
        ++locit;
        }
    }

std::string FMTMask::get(const std::vector<FMTTheme>& themes) const
    {
	std::string value = "";
    for(const FMTTheme& theme : themes)
        {
        value += theme._bitsToStr(*this) + " ";
        }
	value.pop_back();
    return value;
    }



void  FMTMask::setExclusiveBits(const FMTMask& p_mask, const FMTTheme& p_theme)
{
	const boost::dynamic_bitset<uint8_t>BASE = this->_subset(p_theme);
	boost::dynamic_bitset<uint8_t>RHS = p_mask._subset(p_theme);
	RHS.flip(); 
	_setSubset(p_theme, BASE & RHS);
	m_name.clear();
}

std::vector<size_t>FMTMask::getNonFullBlocks() const
{
	std::vector<size_t>Blocks;
	Blocks.reserve(m_data.m_bits.size());
	for (size_t Id = 0; Id < m_data.m_bits.size();++Id)
		{
		if (m_data.m_bits.at(Id) != 0xff)
			{
			Blocks.push_back(Id);
			}
		}
	return Blocks;
}

bool FMTMask::isSubsetOf(const FMTMask& p_mask, const std::vector<size_t>& p_subset) const
{
	bool allFalse = true;
	size_t i = 0;
	while (allFalse && i < p_subset.size())
		{
		if (m_data.m_bits.at(p_subset.at(i)) & ~p_mask.m_data.m_bits.at(p_subset.at(i)))
			{
			allFalse = false;
			}
		++i;
		}
	return allFalse;
}


void FMTMask::set(const std::vector<FMTTheme>& themes,const std::string& value)
    {
	m_name = value;
	std::vector<std::string>bases;
    boost::split(bases,value,boost::is_any_of(FMT_STR_SEPARATOR),boost::token_compress_on);
	if (bases.size()< themes.size())
	{
		size_t toDefault = bases.size();
		while (bases.size() != themes.size())
		{
			bases.push_back(*themes.at(toDefault).getBaseAttributes().begin());
			m_name += " " + bases.back();
			++toDefault;
		}
	}
    size_t fullsize = 0;
    for(const FMTTheme& theme : themes)
        {
        fullsize+=theme.size();
        }
    m_data.resize(fullsize,false);
    for(const FMTTheme& theme : themes)
        {
		const boost::dynamic_bitset<uint8_t>bits = theme._strToBits(bases.at(theme.m_id));
		this->_setSubset(theme, bits);
        }
	//name.shrink_to_fit();
    }
std::string FMTMask::get(const FMTTheme& theme) const
    {
    //const boost::dynamic_bitset<uint8_t>bits =  this->subset(theme);
    //return theme.bitsToStr(bits);
	return  theme._bitsToStr(*this);
    }

const std::string& FMTMask::getAttribute(const FMTTheme& p_theme) const
	{
	size_t bIt = p_theme.m_start;
	const size_t FULL_SIZE = p_theme.m_start + p_theme.size();
	bool gotBit = false;
	while (!gotBit &&
		bIt < FULL_SIZE)
		{
		gotBit = m_data.at(bIt);
		++bIt;
		}
	return p_theme._getAttribute(bIt - p_theme.m_start - 1);
	}

bool FMTMask::empty() const
	{
	return m_data.empty();
	}

std::vector<const Core::FMTTheme*>FMTMask::getStaticThemes(const std::vector<const Core::FMTTheme*>& themes) const
	{
	std::vector<const Core::FMTTheme*>staticths;
	for (const Core::FMTTheme* theme : themes)
		{
		const std::string value = this->get(*theme);
		if (!theme->isAggregate(value) && !theme->isAttribute(value))
			{
			staticths.push_back(theme);
			}
		}
	return staticths;
	}

std::vector<const Core::FMTTheme*> FMTMask::getSelectedThemes(const  std::vector<Core::FMTTheme>& themes) const
{
	std::vector<const Core::FMTTheme*>selected;
	for (const Core::FMTTheme& theme : themes)
	{
		if (_subset(theme).any())
		{
			selected.push_back(&theme);
		}
	}
	return selected;
}


void FMTMask::set(const FMTTheme& theme,const std::string& value)
    {
    const boost::dynamic_bitset<uint8_t>sub=theme._strToBits(value);
    std::vector<std::string>bases;
    boost::split(bases,m_name,boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
	bases.at(theme.m_id) = value;
    m_name = boost::algorithm::join(bases," ");
	
    this->_setSubset(theme,sub);

	//name.shrink_to_fit();
    }

void FMTMask::append(const boost::dynamic_bitset<uint8_t> &bits)
            {
            const size_t thissize = m_data.size();
            m_data.resize(m_data.size() + bits.size());
            size_t bid = 0;
            for (size_t i=thissize; i<m_data.size(); ++i)
                {
				m_data.at(i) = bits.at(bid);
                ++bid;
                }
            }

void FMTMask::update(const std::vector<FMTTheme>& themes)
    {
	m_name.clear();
    for(const FMTTheme& THEME : themes)
        {
        m_name+= THEME._bitsToStr(*this)+" ";
        }
	m_name.pop_back();
    }

void FMTMask::unionWith(const FMTMask& rhs)
	{
	m_name.clear();
	m_data |= rhs.m_data;
	}

FMTMask FMTMask::getUnion(const FMTMask& rhs) const
	{
	FMTMask newmask(m_data);
	newmask.m_data |= rhs.m_data;
	return newmask;
	}

FMTMask FMTMask::getIntersect(const FMTMask& rhs) const
	{
	FMTMask newmask(m_data);
	newmask.m_data &= rhs.m_data;
	return newmask;
	}
void FMTMask::setIntersect(const FMTMask& rhs)
	{
	m_data &= rhs.m_data;
	}


 boost::dynamic_bitset<uint8_t> FMTMask::getBitsetIntersect(const FMTMask& rhs) const
	{
	return m_data & rhs.m_data;
	}


FMTMask::FMTMask(const FMTMask& rhs) : m_name(rhs.m_name),m_data(rhs.m_data)
            {


            }

FMTMask::FMTMask(FMTMask&& rhs) noexcept:
	m_data(std::move(rhs.m_data)), m_name(std::move(rhs.m_name))
	{

	}

FMTMask& FMTMask::operator=(FMTMask&& rhs) noexcept
{
	m_data = std::move(rhs.m_data);
	m_name = std::move(rhs.m_name);
	return *this;
}

void FMTMask::swap(FMTMask& rhs)
	{
	m_data.swap(rhs.m_data);
	m_name.swap(rhs.m_name);
	}

FMTMask& FMTMask::operator = (const FMTMask& rhs)
            {
            if (this!=&rhs)
                {
                m_data = (rhs.m_data);
                m_name =rhs.m_name;
                }
            return *this;
            }

FMTMask FMTMask::resume(const boost::dynamic_bitset<uint8_t>& rhs) const
            {
			FMTMask newmask(m_name, boost::dynamic_bitset<uint8_t>(rhs.count()));
			size_t location = rhs.find_first();
			size_t newlocation = 0;
			while (location!=rhs.npos)
				{
				newmask.m_data.at(newlocation) = m_data.at(location);
				location = rhs.find_next(location);
				++newlocation;
				}
			return newmask;
            }

FMTMask FMTMask::resume(const std::vector<size_t>& indexes) const
		{
		FMTMask newmask(m_name, boost::dynamic_bitset<uint8_t>(indexes.size()));
		size_t baseid = 0;
		for (const size_t& id : indexes)
			{
			newmask.m_data.at(baseid) = m_data.at(id);
			++baseid;
			}
		return newmask;
		}


bool FMTMask::operator == (const FMTMask& rhs) const
    {
    if(m_data.size() == rhs.m_data.size())
        {
        return m_data == rhs.m_data;
        }
    return false;
    }

bool FMTMask::operator != (const FMTMask& rhs) const
    {
    return !(*this==rhs);
    }

bool FMTMask::operator < (const FMTMask& rhs) const
    {
    if(m_data.size() == rhs.m_data.size())
        {
        return (m_data < rhs.m_data);
        }else if(m_data.size() < rhs.m_data.size())
            {
            return true;
            }
    return false;
    }

FMTMask FMTMask::removeAggregates(const std::vector<FMTTheme>& themes, bool questionmarkonly) const
	{
	FMTMask newmask(*this);
	newmask.m_name.clear();
	const boost::dynamic_bitset<uint8_t> nullmask(m_data.size(), false);
	for (const FMTTheme& theme : themes)
		{
		const boost::dynamic_bitset<uint8_t> localtheme = newmask._subset(theme);
		if (!questionmarkonly&&(localtheme.count()>1 || (localtheme.count()==1 && localtheme.size() == 1))||
			questionmarkonly&&localtheme.count()==localtheme.size())
			{
			newmask._setSubset(theme, nullmask);
			}
		}
	return newmask;
	}

void FMTMask::clear()
	{
	m_data.clear();
	m_name.clear();
	}
size_t FMTMask::size() const
	{
	return m_data.size();
	}

size_t FMTMask::count() const
	{
	return m_data.count();
	}


bool FMTMask::isNotThemesSubset(const FMTMask& rhs, const std::vector<const Core::FMTTheme*>& themes) const
	{
	for (const Core::FMTTheme* theme : themes)
		{
		/*const size_t themestart = theme->getStart();
		const size_t themestop = themestart + theme->size()-1;
		if (!(rhs.m_data[themestart]&this->m_data[themestart])&&
			!(rhs.m_data[themestop]&this->m_data[themestop])&&
			!((_subset(*theme) & rhs._subset(*theme)).any())
			)
			{
			return true;
			}*/
		if (!_anyIntersect(rhs, *theme))
			{
			return true;
			}
		}
	return false;
	}

bool FMTMask::isNotThemesSubset(const FMTMask& rhs, const  std::vector<Core::FMTTheme>& themes) const
	{
		for (const Core::FMTTheme& theme : themes)
		{
			/*const size_t themestart = theme.getStart();
			const size_t themestop = themestart + theme.size() - 1;
			if (!(rhs.m_data[themestart] & this->m_data[themestart]) &&
				!(rhs.m_data[themestop] & this->m_data[themestop]) &&
				!((_subset(theme) & rhs._subset(theme)).any()))
			{
				return true;
			}*/
			if (!_anyIntersect(rhs,theme))
			{
				return true;
			}
		}
		return false;
	}

std::string FMTMask::getBitsString() const
            {
			std::string buffer;
			boost::to_string(m_data, buffer);
            return buffer;
            }

bool FMTMask::isSubsetOf(const FMTMask& p_rhs) const
	{
	return m_data.is_subset_of(p_rhs.m_data);
	}

void FMTMask::reserve(size_t p_size)
	{
	m_data.reserve(p_size);
	}


FMTMask FMTMask::refine(const FMTMask& mask,const std::vector<FMTTheme>& themes) const
	{
		if(size()==count())
		{
			return mask;
		}
		std::vector<std::string>bases;
		std::vector<std::string>maskbases;
		boost::split(bases,m_name,boost::is_any_of(FMT_STR_SEPARATOR),boost::token_compress_on);
		const std::string baseMask = std::string(mask);
		boost::split(maskbases, baseMask,boost::is_any_of(FMT_STR_SEPARATOR),boost::token_compress_on);
		for(const FMTTheme& theme : themes)
			{	
			const size_t thiscount = _subset(theme).count();
			const size_t targetcount = mask._subset(theme).count();
			if (thiscount == theme.size() ||  targetcount < thiscount)
			{
				bases.at(theme.m_id) = maskbases.at(theme.m_id);
			}
			/*if (targetcount > 1 && !allow_aggregates)
			{
				//crash
			}else if (thiscount==theme.size()||((targetcount== 1||allow_aggregates) && targetcount < thiscount))
				{
				bases.at(theme.id) = maskbases.at(theme.id);
				}*/
			}
		return FMTMask(boost::algorithm::join(bases," "),themes);
	}	

FMTMask FMTMask::preSolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>&presolvedthemes) const
	{
	FMTMask newMask(*this);
	newMask.preSolveRef(filter, presolvedthemes);
	return newMask;
	}


boost::dynamic_bitset<uint8_t> FMTMask::_getPreSolveMask(const FMTMaskFilter& p_filter,
	const std::vector<FMTTheme>& p_presolvedThemes) const
{
	boost::dynamic_bitset<uint8_t>newData(p_filter.flippedselection.count(), false);
	size_t selectedloc = 0;
	for (size_t bitid = 0; bitid < p_filter.flippedselection.size(); ++bitid)
	{
		if (p_filter.flippedselection.at(bitid))
		{
			newData.at(selectedloc) = m_data.at(bitid);
			++selectedloc;
		}
	}
	return newData;
}

void FMTMask::preSolveRef(const FMTMaskFilter& p_filter, 
	const std::vector<FMTTheme>& p_presolvedThemes, bool p_allowReallocation)
{
	m_data = _getPreSolveMask(p_filter, p_presolvedThemes);
	if (!m_name.empty())
	{
		m_name.clear();
		for (const FMTTheme& theme : p_presolvedThemes)
			{
			m_name += theme._bitsToStr(*this) + " ";
			}
		m_name.pop_back();
		if (p_allowReallocation)
			{
			m_name.shrink_to_fit();
			}
	}
}

bool FMTMask::canPreSolve(const FMTMaskFilter& p_filter,
	const std::vector<FMTTheme>& p_presolvedThemes) const
{
	const Core::FMTMask PRESOLVED = Core::FMTMask(_getPreSolveMask(p_filter, p_presolvedThemes));
	for (const FMTTheme& theme : p_presolvedThemes)
		{
		if (PRESOLVED._subset(theme).count() == 0)
			{
			return false;
			}
		}
	return true;
}

FMTMask FMTMask::postSolve(const FMTMaskFilter& filter,
	const std::vector<FMTTheme>&basethemes) const
	{
	FMTMask newmask(filter.flippedselection);
	size_t presolvedid = 0;
	for (size_t mid = 0; mid < filter.selection.size();++mid)
		{
		if (filter.selection.at(mid))
			{
			newmask.m_data[mid] = m_data.at(presolvedid);
			++presolvedid;
			}
		}
	for (const FMTTheme& theme: basethemes)
		{
		newmask.m_name += theme._bitsToStr(newmask) + " ";
		}
	newmask.m_name.pop_back();
	//newmask.name.shrink_to_fit();
	return newmask;
	}

FMTMaskComparator::FMTMaskComparator(const FMTMask& lbase_mask) : m_baseMask(lbase_mask)
    {

    }

bool FMTMaskComparator::operator()(const FMTMask& mask) const
    {
    return (m_baseMask == mask);
    }



}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTMask)
